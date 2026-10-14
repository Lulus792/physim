/* SDL input/clipboard routines adapted from Nuklear's SDL3 demo.
 * Copyright (c) 2017 Micha Mettke. See third_party/Nuklear-LICENSE (MIT option).
 * Physim supplies the OpenGL renderer and font lifecycle. */
#include "ui.h"
#include "accessibility_native.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifdef _WIN32
#include <windows.h>
#include <dwmapi.h>
#endif
/* Ask the compositor for rounded outer corners, including the integrated main
 * window header. Windows keeps maximized/snapped windows flush with the screen. */
static void window_chrome(SDL_Window *window) {
#ifdef _WIN32
    HWND hwnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(window),
                                             SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
    if (hwnd) {
        BOOL dark = TRUE;
        DWORD corners = 2; /* DWMWCP_ROUND; ignored on Windows versions before 11. */
        COLORREF background = RGB(20, 21, 24), text = RGB(237, 238, 242);
        DwmSetWindowAttribute(hwnd, 20 /* immersive dark mode */, &dark, sizeof dark);
        DwmSetWindowAttribute(hwnd, 33 /* corner preference */, &corners, sizeof corners);
        DwmSetWindowAttribute(hwnd, 34 /* border color */, &background, sizeof background);
        DwmSetWindowAttribute(hwnd, 35 /* caption color */, &background, sizeof background);
        DwmSetWindowAttribute(hwnd, 36 /* text color */, &text, sizeof text);
    }
#else
    (void)window;
#endif
}
static bool accessibility_window_enabled(SDL_Window *window) {
 if(SDL_GetWindowFlags(window)&(SDL_WINDOW_HIDDEN|SDL_WINDOW_MINIMIZED))return false;
#ifdef _WIN32
 HWND hwnd=(HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(window),SDL_PROP_WINDOW_WIN32_HWND_POINTER,NULL);
 return hwnd && IsWindowEnabled(hwnd);
#else
 return true;
#endif
}
struct nk_sdl {
    ps_ui_font_layout font_layout; /* First: shared layout lookup without SDL linkage. */
    SDL_Window *win;
    ps_graphics *graphics;
    struct nk_context ctx;
    struct nk_font_atlas atlas;
    struct nk_buffer commands;
    struct nk_draw_null_texture null_texture;
    struct nk_allocator allocator;
    unsigned font_texture;
    Uint64 last_render;
    bool insert_toggle;
    ps_a11y_model *accessibility;
    SDL_Mutex *accessibility_mutex;
    ps_a11y_native *accessibility_native;
};
static unsigned accessibility_widget(void *user,const char *window,const char *label,
                                 int role,const float bounds[4],bool enabled,bool checked,const char *group,bool activated) {
    struct nk_sdl *sdl=user;if(!sdl->accessibility)return false;
    enabled=enabled && accessibility_window_enabled(sdl->win);
    SDL_LockMutex(sdl->accessibility_mutex);
    uint64_t pending=sdl->accessibility->pending_focus;
    bool pressed=group?ps_a11y_record_option(sdl->accessibility,window,group,label,bounds,enabled,checked,activated)
                      :ps_a11y_record_state(sdl->accessibility,window,label,(ps_a11y_role)role,bounds,enabled,checked);
    ps_a11y_model *model=sdl->accessibility;
    bool granted=pending && !model->pending_focus && model->focused_id==pending && model->draft_count && model->draft[model->draft_count-1].focusable;
    bool focused=model->draft_count && model->draft[model->draft_count-1].id==model->focused_id && model->draft[model->draft_count-1].focusable && model->keyboard_focus && (!pending || pending==model->focused_id);
    SDL_UnlockMutex(sdl->accessibility_mutex);
    if(granted){SDL_RaiseWindow(sdl->win);nk_edit_unfocus(&sdl->ctx);}
    return (pressed?1u:0u)|(granted?2u:0u)|(focused?4u:0u);
}
static unsigned accessibility_range_widget(void *user,const char *window,const char *label,const float bounds[4],bool enabled,double values[4]) {
    struct nk_sdl *sdl=user;SDL_LockMutex(sdl->accessibility_mutex);
    enabled=enabled && accessibility_window_enabled(sdl->win);
    ps_a11y_model *model=sdl->accessibility;uint64_t pending=model->pending_focus;size_t before=model->draft_count;
    bool changed=ps_a11y_record_range(model,window,label,bounds,enabled,values[1],values[2],values[3],&values[0]);
    bool recorded=model->draft_count>before;
    /* The rendered slider owns a float; publish its actual rounded value. */
    values[0]=(double)(float)values[0];if(recorded)model->draft[before].value=values[0];
    bool granted=recorded && pending && !model->pending_focus && model->focused_id==pending && model->draft[before].id==pending;
    bool focused=recorded && model->draft[before].id==model->focused_id && model->draft[before].focusable && model->keyboard_focus;
    SDL_UnlockMutex(sdl->accessibility_mutex);
    if(granted){SDL_RaiseWindow(sdl->win);nk_edit_unfocus(&sdl->ctx);}
    return (changed?1u:0u)|(granted?2u:0u)|(focused?4u:0u);
}
static void *ui_alloc(nk_handle user, void *old, nk_size size) {
    (void)user;
    (void)old;
    return malloc(size);
}
static void ui_free(nk_handle user, void *memory) {
    (void)user;
    free(memory);
}
static void nk_sdl_clipboard_paste(nk_handle usr, struct nk_text_edit *edit);
static void nk_sdl_clipboard_copy(nk_handle usr, const char *text, int len);
struct nk_context *nk_sdl_init(SDL_Window *window, ps_graphics *graphics) {
    window_chrome(window);
    struct nk_sdl *sdl = calloc(1, sizeof *sdl);
    if (!sdl)
        return NULL;
    sdl->win = window;
    sdl->graphics = graphics;
    sdl->font_layout=(ps_ui_font_layout){.magic=PS_UI_LAYOUT_MAGIC,.ui_size=16};
    sdl->last_render = SDL_GetTicksNS();
    sdl->allocator.alloc = ui_alloc;
    sdl->allocator.free = ui_free;
    if (!nk_init(&sdl->ctx, &sdl->allocator, NULL)) {
        free(sdl);
        return NULL;
    }
    sdl->ctx.userdata = nk_handle_ptr(sdl);
    sdl->ctx.clip.copy = nk_sdl_clipboard_copy;
    sdl->ctx.clip.paste = nk_sdl_clipboard_paste;
    sdl->ctx.clip.userdata = nk_handle_ptr(sdl);
    nk_buffer_init_default(&sdl->commands);
    sdl->accessibility=malloc(sizeof *sdl->accessibility);
    sdl->accessibility_mutex=SDL_CreateMutex();
    if(sdl->accessibility && sdl->accessibility_mutex) {
        ps_a11y_init(sdl->accessibility);ps_a11y_begin(sdl->accessibility);
        sdl->accessibility_native=ps_a11y_native_create(window,sdl->accessibility,sdl->accessibility_mutex);
    }
    if(sdl->accessibility_native) {
        sdl->font_layout.accessibility=accessibility_widget;
        sdl->font_layout.accessibility_range=accessibility_range_widget;
        sdl->font_layout.accessibility_user=sdl;
    } else {
        free(sdl->accessibility);sdl->accessibility=NULL;
        if(sdl->accessibility_mutex)SDL_DestroyMutex(sdl->accessibility_mutex);
        sdl->accessibility_mutex=NULL;
    }
    return &sdl->ctx;
}
bool nk_sdl_accessibility_available(struct nk_context *ctx) {
    return ctx && ctx->userdata.ptr && ((struct nk_sdl *)ctx->userdata.ptr)->accessibility_native;
}
#ifdef _WIN32
static size_t nk_sdl_edit_byte(const char *text,int scalar) {
 size_t at=0;while(scalar>0 && text[at]){nk_rune rune;int n=nk_utf_decode(text+at,&rune,(int)strlen(text+at));if(n<=0)break;at+=(size_t)n;--scalar;}return at;
}
static float nk_sdl_edit_extent(const struct nk_user_font *font,const char *text,size_t end) {
 float x=0;for(size_t at=0;at<end;){nk_rune rune;int n=nk_utf_decode(text+at,&rune,(int)(end-at));if(n<=0)break;
  x+=font->width(font->userdata,font->height,text+at,n);at+=(size_t)n;}return x;
}
#endif
nk_flags nk_sdl_edit_named(struct nk_context *ctx,const char *name,nk_flags flags,char *text,int capacity,nk_plugin_filter filter) {
#ifdef _WIN32
 struct nk_sdl *sdl=ctx && ctx->userdata.ptr?ctx->userdata.ptr:NULL;size_t index=SIZE_MAX;uint64_t id=0;bool selected=false;size_t select_start=0,select_end=0;
 bool scrolled=false,scroll_top=true;size_t scroll_start=0,scroll_end=0;float text_scroll=0;
 struct nk_rect edit_bounds=nk_widget_bounds(ctx);
 if(sdl && sdl->accessibility && ctx->current && name && text && capacity>=2) {
  struct nk_rect bounds=nk_widget_bounds(ctx),clip=ctx->current->layout->clip;
  float left=fmaxf(bounds.x,clip.x),top=fmaxf(bounds.y,clip.y);
  float right=fminf(bounds.x+bounds.w,clip.x+clip.w),bottom=fminf(bounds.y+bounds.h,clip.y+clip.h);
  float box[]={left,top,right-left,bottom-top};
  bool routed=sdl->font_layout.accessibility_route_window==ctx->current;
  bool enabled=!ctx->current->widgets_disabled && (routed?sdl->font_layout.accessibility_route_allowed:!(ctx->current->flags&NK_WINDOW_ROM)) &&
   accessibility_window_enabled(sdl->win);
  for(const struct nk_window *above=ctx->current->next;above && enabled;above=above->next)
   if(!(above->flags&(NK_WINDOW_HIDDEN|NK_WINDOW_MINIMIZED|NK_WINDOW_CLOSED)) &&
      above->bounds.x<right && above->bounds.x+above->bounds.w>left &&
      above->bounds.y<bottom && above->bounds.y+above->bounds.h>top)enabled=false;
  SDL_LockMutex(sdl->accessibility_mutex);ps_a11y_model *m=sdl->accessibility;
  uint64_t pending=m->pending_focus;size_t before=m->draft_count;
  bool changed=ps_a11y_record_edit(m,ctx->current->name_string,name,box,enabled,text,(size_t)capacity);
  bool granted=m->draft_count>before && pending && !m->pending_focus && m->focused_id==pending && m->draft[before].id==pending;
  if(m->draft_count>before){index=before;id=m->draft[before].id;text_scroll=m->draft[before].text_scroll;
   selected=ps_a11y_edit_take_selection(m,index,&select_start,&select_end);
   scrolled=ps_a11y_edit_take_scroll(m,index,&scroll_start,&scroll_end,&scroll_top);}
  SDL_UnlockMutex(sdl->accessibility_mutex);
  if(granted){SDL_RaiseWindow(sdl->win);nk_window_set_focus(ctx,ctx->current->name_string);nk_edit_unfocus(ctx);nk_edit_focus(ctx,NK_EDIT_ALWAYS_INSERT_MODE);}
  if(changed && ctx->current->edit.active){ctx->current->edit.cursor=nk_utf_len(text,(int)strlen(text));ctx->current->edit.sel_start=ctx->current->edit.sel_end=ctx->current->edit.cursor;}
  if(selected){
   ctx->current->edit.sel_start=nk_utf_len(text,(int)select_start);ctx->current->edit.sel_end=nk_utf_len(text,(int)select_end);
   ctx->current->edit.cursor=ctx->current->edit.sel_end;
   /* A native selection follows the same horizontal text origin as Nuklear. */
   const struct nk_user_font *font=ctx->style.font;float x=nk_sdl_edit_extent(font,text,select_end);
   float width=edit_bounds.w-2*ctx->style.edit.padding.x-2*ctx->style.edit.border;
   float scroll=(float)ctx->current->edit.scrollbar.x;
   if(x<scroll || x>=scroll+width)ctx->current->edit.scrollbar.x=(nk_uint)fmaxf(0,x-width*.75f);
   text_scroll=(float)ctx->current->edit.scrollbar.x;
  }
  if(index!=SIZE_MAX) {
   const struct nk_user_font *font=ctx->style.font;
   float width=fmaxf(1,edit_bounds.w-2*ctx->style.edit.padding.x-2*ctx->style.edit.border);
   if(scrolled){
    float start=nk_sdl_edit_extent(font,text,scroll_start),end=nk_sdl_edit_extent(font,text,scroll_end);
    if(start==end)end+=ctx->style.edit.cursor_size;
    if(end-start>width-2)text_scroll=scroll_top?start:fmaxf(0,end-width+2);
    else {if(start<text_scroll)text_scroll=start;if(end>text_scroll+width-2)text_scroll=fmaxf(0,end-width+2);}
   }
   float maximum=fmaxf(0,nk_sdl_edit_extent(font,text,strlen(text))+ctx->style.edit.cursor_size+2-width);
   text_scroll=fminf(fmaxf(0,text_scroll),maximum);
  }
 }
#else
 (void)name;
#endif
 nk_flags state;
#ifdef _WIN32
 if(index!=SIZE_MAX){int length=(int)strlen(text);state=nk_edit_string_scrolled(ctx,flags,text,&length,capacity,filter,&text_scroll);text[length]=0;}
 else state=nk_edit_string_zero_terminated(ctx,flags,text,capacity,filter);
#else
 state=nk_edit_string_zero_terminated(ctx,flags,text,capacity,filter);
#endif
#ifdef _WIN32
 if(index!=SIZE_MAX) {
  SDL_LockMutex(sdl->accessibility_mutex);ps_a11y_model *m=sdl->accessibility;
  if(index<m->draft_count && m->draft[index].id==id) {
   size_t start=m->draft[index].text_start,end=m->draft[index].text_end;
   if(state&NK_EDIT_ACTIVE){start=nk_sdl_edit_byte(text,ctx->text_edit.select_start);end=nk_sdl_edit_byte(text,ctx->text_edit.select_end);}
   if(ps_a11y_edit_update(m,index,text,start,end)) {
    ps_a11y_node *node=&m->draft[index];node->text_active=(state&NK_EDIT_ACTIVE)!=0;node->text_scroll=text_scroll;
    if(node->text_active){node->text_caret=nk_sdl_edit_byte(text,ctx->text_edit.cursor);if(ctx->current==ctx->active)m->focused_id=id;}
    else if(m->focused_id==id)m->focused_id=0;
    const struct nk_style_edit *style=&ctx->style.edit;const struct nk_user_font *font=ctx->style.font;
    struct nk_rect area={edit_bounds.x+style->padding.x+style->border,edit_bounds.y+style->padding.y+style->border,
     edit_bounds.w-2*style->padding.x-2*style->border,edit_bounds.h-2*style->padding.y-2*style->border};
    float x=area.x-ctx->text_edit.scrollbar.x,y=area.y+(area.h-font->height)/2;size_t length=strlen(text);
    for(size_t at=0;at<length;){nk_rune rune;int n=nk_utf_decode(text+at,&rune,(int)(length-at));if(n<=0)break;
     float width=font->width(font->userdata,font->height,text+at,n),left=fmaxf(x,area.x),right=fminf(x+width,area.x+area.w);
     float box[]={left,fmaxf(y,area.y),right-left,fminf(y+font->height,area.y+area.h)-fmaxf(y,area.y)};
     ps_a11y_edit_box(m,index,at,at+(size_t)n,box);x+=width;at+=(size_t)n;
    }
    float caret_x=area.x-ctx->text_edit.scrollbar.x;
    for(size_t at=0;at<node->text_caret && at<length;){nk_rune rune;int n=nk_utf_decode(text+at,&rune,(int)(length-at));if(n<=0)break;caret_x+=font->width(font->userdata,font->height,text+at,n);at+=(size_t)n;}
    float left=fmaxf(caret_x,fmaxf(area.x,node->bounds[0])),right=fminf(caret_x+style->cursor_size,fminf(area.x+area.w,node->bounds[0]+node->bounds[2]));
    float top=fmaxf(y,fmaxf(area.y,node->bounds[1])),bottom=fminf(y+font->height,fminf(area.y+area.h,node->bounds[1]+node->bounds[3]));
    node->caret_bounds[0]=left;node->caret_bounds[1]=top;node->caret_bounds[2]=fmaxf(0,right-left);node->caret_bounds[3]=fmaxf(0,bottom-top);
   }
  }
  if(m->focused_id==id && id)state|=PS_UI_EDIT_NATIVE_FOCUS;
  SDL_UnlockMutex(sdl->accessibility_mutex);
 }
#endif
 return state;
}
bool nk_sdl_accessibility_document(struct nk_context *ctx,uint64_t version,const char *name,const char *text,struct nk_rect viewport) {
 if(!ctx || !ctx->userdata.ptr)return false;struct nk_sdl *sdl=ctx->userdata.ptr;if(!sdl->accessibility)return false;
 float bounds[]={viewport.x,viewport.y,viewport.w,viewport.h};SDL_LockMutex(sdl->accessibility_mutex);
 bool ok=ps_a11y_document_begin(sdl->accessibility,version,name,text,bounds);SDL_UnlockMutex(sdl->accessibility_mutex);return ok;
}
void nk_sdl_accessibility_text_box(struct nk_context *ctx,size_t begin,size_t end,struct nk_rect bounds) {
 if(!ctx || !ctx->userdata.ptr)return;struct nk_sdl *sdl=ctx->userdata.ptr;if(!sdl->accessibility)return;
 float box[]={bounds.x,bounds.y,bounds.w,bounds.h};SDL_LockMutex(sdl->accessibility_mutex);
 ps_a11y_document_box(sdl->accessibility,begin,end,box);SDL_UnlockMutex(sdl->accessibility_mutex);
}
bool nk_sdl_accessibility_document_scroll(struct nk_context *ctx,uint64_t version,size_t *byte,bool *top) {
 if(!ctx || !ctx->userdata.ptr)return false;struct nk_sdl *sdl=ctx->userdata.ptr;if(!sdl->accessibility)return false;
 SDL_LockMutex(sdl->accessibility_mutex);bool ok=ps_a11y_document_take_scroll(sdl->accessibility,version,byte);
 if(ok && top)*top=sdl->accessibility->document_scroll_top;
 SDL_UnlockMutex(sdl->accessibility_mutex);return ok;
}
bool nk_sdl_accessibility_document_visible(struct nk_context *ctx,uint64_t version,size_t byte) {
 if(!ctx || !ctx->userdata.ptr)return false;struct nk_sdl *sdl=ctx->userdata.ptr;if(!sdl->accessibility)return false;
 SDL_LockMutex(sdl->accessibility_mutex);const ps_a11y_document *d=sdl->accessibility->document;
 bool visible=false;if(d && d->active && d->version==version)
  for(size_t i=0;i<d->count;i++)if(d->boxes[i].begin<=byte && d->boxes[i].end>byte){visible=true;break;}
 SDL_UnlockMutex(sdl->accessibility_mutex);return visible;
}
bool nk_sdl_accessibility_document_preserve_position(struct nk_context *ctx,uint64_t version) {
 if(!ctx || !ctx->userdata.ptr)return false;struct nk_sdl *sdl=ctx->userdata.ptr;if(!sdl->accessibility)return false;
 SDL_LockMutex(sdl->accessibility_mutex);
 bool ok=ps_a11y_document_preserve_position(sdl->accessibility,version);
 SDL_UnlockMutex(sdl->accessibility_mutex);return ok;
}
bool nk_sdl_accessibility_document_link(struct nk_context *ctx,const char *name,uint64_t version,
                                        size_t begin,size_t end,struct nk_rect bounds) {
 if(!ctx || !ctx->userdata.ptr || !ctx->current || !ctx->current->layout)return false;
 struct nk_sdl *sdl=ctx->userdata.ptr;if(!sdl->accessibility)return false;
 struct nk_rect clip=ctx->current->layout->clip;
 float left=fmaxf(bounds.x,clip.x),top=fmaxf(bounds.y,clip.y);
 float right=fminf(bounds.x+bounds.w,clip.x+clip.w),bottom=fminf(bounds.y+bounds.h,clip.y+clip.h);
 float visible[4]={0};if(right>left && bottom>top){visible[0]=left;visible[1]=top;visible[2]=right-left;visible[3]=bottom-top;}
 bool routed=sdl->font_layout.accessibility_route_window==ctx->current;
 bool enabled=!ctx->current->widgets_disabled && (routed?sdl->font_layout.accessibility_route_allowed:!(ctx->current->flags&NK_WINDOW_ROM)) &&
     accessibility_window_enabled(sdl->win);
 for(const struct nk_window *above=ctx->current->next;above && enabled;above=above->next) {
  if(above->flags&(NK_WINDOW_HIDDEN|NK_WINDOW_MINIMIZED|NK_WINDOW_CLOSED))continue;
  struct nk_rect cover=above->bounds;
  if(cover.x<clip.x+clip.w && cover.x+cover.w>clip.x && cover.y<clip.y+clip.h && cover.y+cover.h>clip.y)enabled=false;
 }
 SDL_LockMutex(sdl->accessibility_mutex);ps_a11y_model *m=sdl->accessibility;
 uint64_t pending=m->pending_focus;size_t before=m->draft_count;
 bool pressed=ps_a11y_document_link(m,ctx->current->name_string,name,version,begin,end,visible,enabled);
 bool granted=m->draft_count>before && pending && !m->pending_focus && m->focused_id==pending && m->draft[before].id==pending;
 bool focused=m->draft_count>before && m->draft[before].id==m->focused_id && m->keyboard_focus;
 if(granted)ps_a11y_document_scroll_align(m,version,begin,true);
 SDL_UnlockMutex(sdl->accessibility_mutex);
 if(granted){SDL_RaiseWindow(sdl->win);nk_window_set_focus(ctx,ctx->current->name_string);nk_edit_unfocus(ctx);}
 if(focused && visible[2]>0 && visible[3]>0)nk_stroke_rect(nk_window_get_canvas(ctx),
     nk_rect(left-3,top-2,right-left+6,bottom-top+4),3,1,ctx->style.button.text_normal);
 return pressed;
}
void nk_sdl_accessibility_document_link_invoked(struct nk_context *ctx,uint64_t version,size_t begin,size_t end) {
 if(!ctx || !ctx->userdata.ptr)return;struct nk_sdl *sdl=ctx->userdata.ptr;
 ps_a11y_native_document_link_invoked(sdl->accessibility_native,version,begin,end);
}
void nk_sdl_accessibility_suspend(struct nk_context *ctx) {
 if(!ctx || !ctx->userdata.ptr)return;struct nk_sdl *sdl=ctx->userdata.ptr;if(!sdl->accessibility)return;
 SDL_LockMutex(sdl->accessibility_mutex);bool first=!sdl->accessibility->suspended;
 ps_a11y_suspend(sdl->accessibility);SDL_UnlockMutex(sdl->accessibility_mutex);
 if(first) {
  /* No current Nuklear window exists between frames; clear every window's edit
   * cache explicitly, otherwise it reacquires native focus after reopening. */
  for(struct nk_window *window=ctx->begin;window;window=window->next){window->edit.active=nk_false;window->edit.name=0;}
  ctx->text_edit.active=nk_false;SDL_StopTextInput(sdl->win);
 }
 ps_a11y_native_publish(sdl->accessibility_native,false);
}
bool nk_sdl_accessibility_press(struct nk_context *ctx,const char *label) {
    if(!ctx || !ctx->userdata.ptr)return false;
    struct nk_sdl *sdl=ctx->userdata.ptr;
    return ps_a11y_native_press_label(sdl->accessibility_native,label);
}
bool nk_sdl_accessibility_focus(struct nk_context *ctx,const char *group,const char *label) {
 if(!ctx || !ctx->userdata.ptr)return false;
 struct nk_sdl *sdl=ctx->userdata.ptr;
 return ps_a11y_native_focus_label(sdl->accessibility_native,group,label);
}
bool nk_sdl_accessibility_choose(struct nk_context *ctx,const char *group,const char *label) {
    if(!ctx || !ctx->userdata.ptr)return false;
    struct nk_sdl *sdl=ctx->userdata.ptr;
    return ps_a11y_native_press_choice(sdl->accessibility_native,group,label);
}
bool nk_sdl_accessibility_is_focused(struct nk_context *ctx,const char *group,const char *label) {
 if(!ctx || !ctx->userdata.ptr || !label)return false;
 struct nk_sdl *sdl=ctx->userdata.ptr;
 if(!sdl->accessibility)return false;
 SDL_LockMutex(sdl->accessibility_mutex);
 const ps_a11y_node *node=ps_a11y_find(sdl->accessibility,sdl->accessibility->focused_id);
 const ps_a11y_node *parent=node?ps_a11y_find(sdl->accessibility,node->parent):NULL;
 bool focused=node && node->focused && !strcmp(node->label,label) && (!group || (parent && !strcmp(parent->label,group)));
 SDL_UnlockMutex(sdl->accessibility_mutex);return focused;
}
bool nk_sdl_accessibility_has_focus(struct nk_context *ctx) {
 if(!ctx || !ctx->userdata.ptr)return false;
 struct nk_sdl *sdl=ctx->userdata.ptr;
 if(!sdl->accessibility)return false;
 SDL_LockMutex(sdl->accessibility_mutex);
 bool focused=sdl->accessibility->focused_id!=0;SDL_UnlockMutex(sdl->accessibility_mutex);return focused;
}
bool nk_sdl_accessibility_event(struct nk_context *ctx,const SDL_Event *event) {
 if(!ctx || !ctx->userdata.ptr || !event)return false;
 struct nk_sdl *sdl=ctx->userdata.ptr;
 if(!sdl->accessibility || SDL_GetWindowFromEvent(event)!=sdl->win)return false;
 SDL_LockMutex(sdl->accessibility_mutex);ps_a11y_model *m=sdl->accessibility;bool handled=false;
 if(event->type==SDL_EVENT_WINDOW_FOCUS_LOST || event->type==SDL_EVENT_MOUSE_BUTTON_DOWN)ps_a11y_blur(m,0);
 else if(m->focused_id && event->type==SDL_EVENT_TEXT_INPUT){const ps_a11y_node *node=ps_a11y_find(m,m->focused_id);handled=!node || node->role!=PS_A11Y_EDIT;}
 else if(m->focused_id && (event->type==SDL_EVENT_KEY_DOWN || event->type==SDL_EVENT_KEY_UP) &&
         !(event->key.mod&(SDL_KMOD_CTRL|SDL_KMOD_GUI|SDL_KMOD_ALT))) {
  SDL_Keycode key=event->key.key;bool down=event->type==SDL_EVENT_KEY_DOWN;
  const ps_a11y_node *current=ps_a11y_find(m,m->focused_id);bool editing=current && current->role==PS_A11Y_EDIT;
  if((key==SDLK_RETURN || key==SDLK_KP_ENTER || key==SDLK_SPACE) && !editing){handled=true;if(down && !event->key.repeat)ps_a11y_press(m,m->focused_id);}
  else if(key==SDLK_TAB){
   if(editing){if(down)ps_a11y_blur(m,0);}
   else {handled=true;if(down)ps_a11y_focus_move(m,event->key.mod&SDL_KMOD_SHIFT?-1:1,false);}
  }
  else if(key==SDLK_ESCAPE)ps_a11y_blur(m,0);
  else if(key==SDLK_LEFT || key==SDLK_RIGHT || key==SDLK_UP || key==SDLK_DOWN) {
   const ps_a11y_node *node=ps_a11y_find(m,m->focused_id);
   if(node && node->role==PS_A11Y_RADIO){handled=true;if(down)ps_a11y_focus_move(m,key==SDLK_LEFT || key==SDLK_UP?-1:1,true);}
   if(node && node->role==PS_A11Y_SLIDER){handled=true;if(down)ps_a11y_adjust(m,node->id,key==SDLK_LEFT || key==SDLK_DOWN?-1:1);}
  }
 }
 SDL_UnlockMutex(sdl->accessibility_mutex);return handled;
}
void nk_sdl_set_ui_size(struct nk_context *ctx,unsigned size) {
    if(ctx && ctx->userdata.ptr && size>=16 && size<=32 && size%2==0)
        ((struct nk_sdl *)ctx->userdata.ptr)->font_layout.ui_size=size;
}
struct nk_font_atlas *nk_sdl_font_stash_begin(struct nk_context *ctx) {
    struct nk_sdl *sdl = ctx->userdata.ptr;
    nk_font_atlas_init_default(&sdl->atlas);
    nk_font_atlas_begin(&sdl->atlas);
    return &sdl->atlas;
}
bool nk_sdl_font_stash_end(struct nk_context *ctx) {
    struct nk_sdl *sdl = ctx->userdata.ptr;
    int w=0,h=0,limit=ps_graphics_texture_limit(sdl->graphics);
    const void *pixels = nk_font_atlas_bake_limited(&sdl->atlas, &w, &h, NK_FONT_ATLAS_RGBA32,limit);
    if (!pixels) {
        SDL_SetError("Complete font atlas cannot fit or allocate within GPU limit %d",limit);
        return false;
    }
    printf("Font atlas: %dx%d, %d fonts, GPU limit %d\n",w,h,sdl->atlas.font_num,limit);
    /* Nuklear may bake a partial atlas when its packing space is exhausted.
     * Successful texture allocation alone does not prove that text can render. */
    for(struct nk_font *font=sdl->atlas.fonts;font;font=font->next)
        for(nk_rune c=33;c<=126;c++) {
            const struct nk_font_glyph *glyph=nk_font_find_glyph(font,c);
            if(!glyph || glyph->x1<=glyph->x0 || glyph->y1<=glyph->y0 || glyph->u1<=glyph->u0 || glyph->v1<=glyph->v0) {
                SDL_SetError("Font atlas incomplete: %dx%d, size %.0f, character U+%04X",w,h,font->handle.height,(unsigned)c);
                return false;
            }
        }
    sdl->font_texture = ps_graphics_texture(sdl->graphics, pixels, w, h);
    if (!sdl->font_texture)
        return false;
    nk_font_atlas_end(&sdl->atlas, nk_handle_id((int)sdl->font_texture), &sdl->null_texture);
    return true;
}
bool nk_sdl_render(struct nk_context *ctx) {
    struct nk_sdl *sdl = ctx->userdata.ptr;
    Uint64 now = SDL_GetTicksNS();
    ctx->delta_time_seconds = (float)(now - sdl->last_render) / (float)SDL_NS_PER_SECOND;
    sdl->last_render = now;
    bool ok = ps_graphics_ui(sdl->graphics, ctx, &sdl->commands, &sdl->null_texture);
    if(sdl->accessibility) {
        SDL_LockMutex(sdl->accessibility_mutex);
        /* Recheck the completed stack, including windows first drawn later in
         * this frame. Their presence must invalidate a covered native target. */
        for(size_t i=0;i<sdl->accessibility->draft_count;i++) {
            ps_a11y_node *node=&sdl->accessibility->draft[i];if(!node->focusable)continue;
            struct nk_window *window=nk_window_find(ctx,node->window);
            if(!window){node->enabled=false;node->focusable=false;continue;}
            const float *target=node->bounds;
            const ps_a11y_document *document=sdl->accessibility->document_draft;
            if(node->offscreen && document && document->active && document->version==node->document_version)target=document->viewport;
            for(struct nk_window *above=window->next;above;above=above->next) {
                if(above->flags&(NK_WINDOW_HIDDEN|NK_WINDOW_MINIMIZED|NK_WINDOW_CLOSED))continue;
                struct nk_rect cover=above->bounds;
                if(cover.x<target[0]+target[2] && cover.x+cover.w>target[0] &&
                   cover.y<target[1]+target[3] && cover.y+cover.h>target[1]) {
                    node->enabled=false;node->focusable=false;break;
                }
            }
        }
        sdl->accessibility->keyboard_focus=SDL_GetKeyboardFocus()==sdl->win;
        bool changed=ps_a11y_publish(sdl->accessibility);
        SDL_UnlockMutex(sdl->accessibility_mutex);
        ps_a11y_native_publish(sdl->accessibility_native,changed);
        SDL_LockMutex(sdl->accessibility_mutex);ps_a11y_begin(sdl->accessibility);
        SDL_UnlockMutex(sdl->accessibility_mutex);
    }
    nk_clear(ctx);
    nk_buffer_clear(&sdl->commands);
    return ok;
}
void nk_sdl_update_TextInput(struct nk_context *ctx) {
    struct nk_sdl *sdl = ctx->userdata.ptr;
    bool active = ctx->active && (ctx->active->popup.win ? ctx->active->popup.win->edit.active
                                                         : ctx->active->edit.active);
    if (active && !SDL_TextInputActive(sdl->win))
        SDL_StartTextInput(sdl->win);
    if (!active && SDL_TextInputActive(sdl->win))
        SDL_StopTextInput(sdl->win);
}
ps_result nk_sdl_export_png(struct nk_context *ctx, struct nk_context *drawing, int width,
                            int height, const char *path) {
    return nk_sdl_export_png_scaled(ctx, drawing, width, height, 2, path);
}
ps_result nk_sdl_export_png_scaled(struct nk_context *ctx, struct nk_context *drawing, int width,
                                   int height, unsigned scale, const char *path) {
    if (!ctx || !ctx->userdata.ptr) return PS_INVALID;
    struct nk_sdl *sdl = ctx->userdata.ptr;
    return ps_graphics_ui_png_scaled(sdl->graphics, drawing, &sdl->null_texture, width, height, scale, path);
}
void nk_sdl_shutdown(struct nk_context *ctx) {
    if (!ctx)
        return;
    struct nk_sdl *sdl = ctx->userdata.ptr;
    ps_a11y_native_destroy(sdl->accessibility_native);
    ps_a11y_destroy(sdl->accessibility);
    free(sdl->accessibility);
    ps_graphics_delete_texture(sdl->graphics, sdl->font_texture);
    if (sdl->atlas.temporary.alloc)
        nk_font_atlas_clear(&sdl->atlas);
    nk_buffer_free(&sdl->commands);
    nk_free(ctx);
    free(sdl);
}
static bool nk_sdl_paste_text(struct nk_text_edit *edit, const char *text) {
    /* The vendored paste patch takes bytes and preserves scalar-based undo. */
    size_t bytes = strlen(text);
    return bytes && bytes <= 262144 && (!edit->single_line || (!strchr(text,'\r') && !strchr(text,'\n') && !strchr(text,'\t'))) && nk_textedit_paste(edit, text, (int)bytes);
}
static void nk_sdl_clipboard_paste(nk_handle usr, struct nk_text_edit *edit) {
    char *text;
    (void)usr;

    text = SDL_GetClipboardText();
    if (!text)
        return;

    nk_sdl_paste_text(edit, text);
    SDL_free(text);
}

static void nk_sdl_clipboard_copy(nk_handle usr, const char *text, int len) {
    const char *ptext;
    char *str;
    size_t buflen;
    int i;
    struct nk_sdl *sdl = (struct nk_sdl *)usr.ptr;
    SDL_assert(sdl);
    if (len <= 0 || text == NULL)
        return;

    /* This snapshot's copy callback supplies a Unicode scalar count; SDL needs
     * the corresponding UTF-8 bytes. Paste has a separate, patched byte contract. */
    ptext = text;
    for (i = len; i > 0; i--)
        (void)SDL_StepUTF8(&ptext, NULL);
    buflen = (size_t)(ptext - text) + 1;

    str = (char *)sdl->allocator.alloc(sdl->allocator.userdata, 0, buflen);
    if (!str)
        return;
    SDL_strlcpy(str, text, buflen);
    SDL_SetClipboardText(str);
    sdl->allocator.free(sdl->allocator.userdata, str);
}

/* Keep platform conventions here so editor and other text fields agree. The
 * platform argument also lets the input regression exercise both mappings. */
static bool nk_sdl_edit_key(struct nk_context *ctx, SDL_Keycode key, SDL_Keymod mod, bool down,
                            bool mac) {
    bool command = (mod & (mac ? SDL_KMOD_GUI : SDL_KMOD_CTRL)) != 0;
    bool word = (mod & (mac ? SDL_KMOD_ALT : SDL_KMOD_CTRL)) != 0;
    bool shift = (mod & SDL_KMOD_SHIFT) != 0;
    enum nk_keys keys[3], selected = NK_KEY_NONE;
    unsigned count = 0;
    switch (key) {
    case SDLK_Z:
        keys[count++] = NK_KEY_TEXT_UNDO;
        keys[count++] = NK_KEY_TEXT_REDO;
        if (command)
            selected = shift ? NK_KEY_TEXT_REDO : NK_KEY_TEXT_UNDO;
        break;
    case SDLK_Y:
        if (mac)
            return false;
        /* Ctrl+Y and Ctrl+Shift+Z both repeat the undone edit. */
        /* fall through */
    case SDLK_R:
        keys[count++] = NK_KEY_TEXT_REDO;
        if (command)
            selected = NK_KEY_TEXT_REDO;
        break;
    case SDLK_LEFT:
    case SDLK_RIGHT: {
        bool left = key == SDLK_LEFT;
        keys[count++] = left ? NK_KEY_LEFT : NK_KEY_RIGHT;
        keys[count++] = left ? NK_KEY_TEXT_WORD_LEFT : NK_KEY_TEXT_WORD_RIGHT;
        keys[count++] = left ? NK_KEY_TEXT_LINE_START : NK_KEY_TEXT_LINE_END;
        selected = keys[mac && command ? 2 : word ? 1 : 0];
        break;
    }
    case SDLK_UP:
    case SDLK_DOWN:
        keys[count++] = key == SDLK_UP ? NK_KEY_UP : NK_KEY_DOWN;
        keys[count++] = key == SDLK_UP ? NK_KEY_TEXT_START : NK_KEY_TEXT_END;
        selected = keys[mac && command ? 1 : 0];
        break;
    case SDLK_HOME:
    case SDLK_END: {
        bool home = key == SDLK_HOME;
        keys[count++] = home ? NK_KEY_TEXT_LINE_START : NK_KEY_TEXT_LINE_END;
        keys[count++] = home ? NK_KEY_TEXT_START : NK_KEY_TEXT_END;
        keys[count++] = home ? NK_KEY_SCROLL_START : NK_KEY_SCROLL_END;
        selected = keys[command ? 1 : mac ? 2 : 0];
        break;
    }
    default:
        return false;
    }
    if (ctx->input.keyboard.keys[NK_KEY_SHIFT].down != shift)
        nk_input_key(ctx, NK_KEY_SHIFT, shift);
    /* Key-up can arrive after the modifier was released. Clear every variant
     * of that physical key so a word/line action cannot remain held. */
    for (unsigned i = 0; i < count; i++) {
        bool pressed = down && keys[i] == selected;
        /* Nuklear counts even redundant releases as clicks. Only release an
         * action that
         * was held, while still forwarding repeated key-downs. */
        if (pressed || ctx->input.keyboard.keys[keys[i]].down)
            nk_input_key(ctx, keys[i], pressed);
    }
    return true;
}

NK_API int nk_sdl_handle_event(struct nk_context *ctx, SDL_Event *evt) {
    struct nk_sdl *sdl;

    SDL_assert(ctx);
    SDL_assert(evt);

    sdl = (struct nk_sdl *)ctx->userdata.ptr;
    SDL_assert(sdl);

    /* We only care about Window currently used by Nuklear */
    if (sdl->win != SDL_GetWindowFromEvent(evt)) {
        return 0;
    }

    switch (evt->type) {
    case SDL_EVENT_WINDOW_FOCUS_LOST:
        for (int key = 0; key < NK_KEY_MAX; key++)
            nk_input_key(ctx, (enum nk_keys)key, false);
        for (int button = 0; button < NK_BUTTON_MAX; button++)
            nk_input_button(ctx, (enum nk_buttons)button, 0, 0, false);
        return 1;
    case SDL_EVENT_KEY_UP: /* KEYUP & KEYDOWN share same routine */
    case SDL_EVENT_KEY_DOWN: {
        int down = evt->type == SDL_EVENT_KEY_DOWN;
        int ctrl_down = evt->key.mod & PS_UI_COMMAND_MOD;
#ifdef __APPLE__
        const bool mac = true;
#else
        const bool mac = false;
#endif
        if (nk_sdl_edit_key(ctx, evt->key.key, evt->key.mod, down != 0, mac))
            return 1;

        switch (evt->key.key) {
        case SDLK_LALT:
        case SDLK_RALT:
            nk_input_key(ctx, NK_KEY_ALT, down);
            break;
        case SDLK_RSHIFT: /* RSHIFT & LSHIFT share same routine */
        case SDLK_LSHIFT:
            nk_input_key(ctx, NK_KEY_SHIFT, down);
            break;
        case SDLK_DELETE:
            nk_input_key(ctx, NK_KEY_DEL, down);
            break;
        case SDLK_KP_ENTER:
        case SDLK_RETURN:
            nk_input_key(ctx, NK_KEY_ENTER, down);
            break;
        case SDLK_TAB:
            nk_input_key(ctx, NK_KEY_TAB, down);
            break;
        case SDLK_BACKSPACE:
            nk_input_key(ctx, NK_KEY_BACKSPACE, down);
            break;
        case SDLK_PAGEDOWN:
            nk_input_key(ctx, NK_KEY_SCROLL_DOWN, down);
            break;
        case SDLK_PAGEUP:
            nk_input_key(ctx, NK_KEY_SCROLL_UP, down);
            break;
        case SDLK_F1:
            nk_input_key(ctx, NK_KEY_F1, down);
            break;
        case SDLK_F2:
            nk_input_key(ctx, NK_KEY_F2, down);
            break;
        case SDLK_F3:
            nk_input_key(ctx, NK_KEY_F3, down);
            break;
        case SDLK_F4:
            nk_input_key(ctx, NK_KEY_F4, down);
            break;
        case SDLK_F5:
            nk_input_key(ctx, NK_KEY_F5, down);
            break;
        case SDLK_F6:
            nk_input_key(ctx, NK_KEY_F6, down);
            break;
        case SDLK_F7:
            nk_input_key(ctx, NK_KEY_F7, down);
            break;
        case SDLK_F8:
            nk_input_key(ctx, NK_KEY_F8, down);
            break;
        case SDLK_F9:
            nk_input_key(ctx, NK_KEY_F9, down);
            break;
        case SDLK_F10:
            nk_input_key(ctx, NK_KEY_F10, down);
            break;
        case SDLK_F11:
            nk_input_key(ctx, NK_KEY_F11, down);
            break;
        case SDLK_F12:
            nk_input_key(ctx, NK_KEY_F12, down);
            break;
        case SDLK_A:
            nk_input_key(ctx, NK_KEY_TEXT_SELECT_ALL, down && ctrl_down);
            break;
        case SDLK_C:
            nk_input_key(ctx, NK_KEY_COPY, down && ctrl_down);
            break;
        case SDLK_V:
            nk_input_key(ctx, NK_KEY_PASTE, down && ctrl_down);
            break;
        case SDLK_X:
            nk_input_key(ctx, NK_KEY_CUT, down && ctrl_down);
            break;
        case SDLK_B:
            nk_input_key(ctx, NK_KEY_TEXT_LINE_START, down && ctrl_down);
            break;
        case SDLK_E:
            nk_input_key(ctx, NK_KEY_TEXT_LINE_END, down && ctrl_down);
            break;
        case SDLK_ESCAPE:
            nk_input_key(ctx, NK_KEY_TEXT_RESET_MODE, down);
            break;
        case SDLK_INSERT:
            if (down)
                sdl->insert_toggle = !sdl->insert_toggle;
            if (sdl->insert_toggle) {
                nk_input_key(ctx, NK_KEY_TEXT_INSERT_MODE, down);
            } else {
                nk_input_key(ctx, NK_KEY_TEXT_REPLACE_MODE, down);
            }
            break;
        default:
            return 0;
        }
        return 1;
    }

    case SDL_EVENT_MOUSE_BUTTON_UP: /* MOUSEBUTTONUP & MOUSEBUTTONDOWN share same routine */
    case SDL_EVENT_MOUSE_BUTTON_DOWN: {
        const int x = (int)evt->button.x, y = (int)evt->button.y;
        const int down = evt->button.down;
        switch (evt->button.button) {
        case SDL_BUTTON_LEFT:
            if (evt->button.clicks > 1)
                nk_input_button(ctx, NK_BUTTON_DOUBLE, x, y, down);
            nk_input_button(ctx, NK_BUTTON_LEFT, x, y, down);
            break;
        case SDL_BUTTON_MIDDLE:
            nk_input_button(ctx, NK_BUTTON_MIDDLE, x, y, down);
            break;
        case SDL_BUTTON_RIGHT:
            nk_input_button(ctx, NK_BUTTON_RIGHT, x, y, down);
            break;
        case SDL_BUTTON_X1:
            nk_input_button(ctx, NK_BUTTON_X1, x, y, down);
            break;
        case SDL_BUTTON_X2:
            nk_input_button(ctx, NK_BUTTON_X2, x, y, down);
            break;
        default:
            return 0;
        }
    }
        return 1;

    case SDL_EVENT_MOUSE_MOTION:
        ctx->input.mouse.pos.x = evt->motion.x;
        ctx->input.mouse.pos.y = evt->motion.y;
        ctx->input.mouse.delta.x = ctx->input.mouse.pos.x - ctx->input.mouse.prev.x;
        ctx->input.mouse.delta.y = ctx->input.mouse.pos.y - ctx->input.mouse.prev.y;
        return 1;

    case SDL_EVENT_TEXT_INPUT: {
        const char *text = evt->text.text;
        int remaining = (int)SDL_strlen(text);
        while (remaining > 0) {
            nk_rune codepoint;
            int size = nk_utf_decode(text, &codepoint, remaining);
            if (size <= 0)
                break;
            nk_input_unicode(ctx, codepoint);
            text += size;
            remaining -= size;
        }
    }
        return 1;

    case SDL_EVENT_MOUSE_WHEEL: {
        float direction = evt->wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1.f : 1.f;
        nk_input_scroll(ctx, nk_vec2(direction * evt->wheel.x, direction * evt->wheel.y));
    }
        return 1;
    }
    return 0;
}

static float test_text_width(nk_handle user, float height, const char *text, int length) {
    (void)user;
    (void)text;
    return height * 0.5f * (float)length;
}
static void test_edit_frame(struct nk_context *ctx, struct nk_text_edit *edit) {
    if (nk_begin(ctx, "Keyboard regression", nk_rect(0, 0, 600, 400), 0)) {
        nk_layout_row_dynamic(ctx, 300, 1);
        nk_edit_focus(ctx, NK_EDIT_ALWAYS_INSERT_MODE);
        nk_edit_buffer(ctx, NK_EDIT_BOX | NK_EDIT_ALWAYS_INSERT_MODE, edit, nk_filter_default);
    }
    nk_end(ctx);
    nk_clear(ctx);
}
static bool test_editor_navigation(bool mac) {
    struct nk_user_font font = {0};
    font.height = 16;
    font.width = test_text_width;
    struct nk_context ctx;
    if (!nk_init_default(&ctx, &font))
        return false;
    char buffer[128];
    struct nk_text_edit edit;
    nk_textedit_init_fixed(&edit, buffer, sizeof buffer);
    edit.mode = NK_TEXT_EDIT_MODE_INSERT;
    edit.single_line = 0; /* This fixture tests a multiline NK_EDIT_BOX. */
    nk_sdl_paste_text(&edit, "one two\nthree four");
    test_edit_frame(&ctx, &edit);
    SDL_Keymod command = mac ? SDL_KMOD_GUI : SDL_KMOD_CTRL;
    SDL_Keymod word = mac ? SDL_KMOD_ALT : SDL_KMOD_CTRL;
    const struct {
        SDL_Keycode key;
        SDL_Keymod mod;
        int cursor;
    } cases[] = {
        {SDLK_LEFT, word, 8},
        {SDLK_RIGHT, word, 14},
        {mac ? SDLK_LEFT : SDLK_HOME, mac ? command : 0, 8},
        {mac ? SDLK_RIGHT : SDLK_END, mac ? command : 0, 18},
        {mac ? SDLK_UP : SDLK_HOME, command, 0},
        {mac ? SDLK_DOWN : SDLK_END, command, 18},
        {SDLK_LEFT, 0, 10},
        {SDLK_RIGHT, 0, 12},
        {SDLK_LEFT, (SDL_Keymod)(word | SDL_KMOD_SHIFT), 8},
        {mac ? SDLK_RIGHT : SDLK_END, (SDL_Keymod)((mac ? command : 0) | SDL_KMOD_SHIFT), 18},
    };
    bool ok = true;
    for (unsigned i = 0; i < sizeof cases / sizeof cases[0]; i++) {
        edit.cursor = edit.select_start = edit.select_end = 11;
        nk_input_begin(&ctx);
        nk_sdl_edit_key(&ctx, cases[i].key, cases[i].mod, true, mac);
        nk_input_end(&ctx);
        test_edit_frame(&ctx, &edit);
        bool selected = (cases[i].mod & SDL_KMOD_SHIFT) != 0;
        ok = ok && edit.cursor == cases[i].cursor &&
             (selected ? edit.select_start == 11 && edit.select_end == cases[i].cursor
                       : edit.select_start == edit.select_end);
        /* Release modifiers first: no action may remain held or move the cursor again. */
        nk_input_begin(&ctx);
        nk_sdl_edit_key(&ctx, cases[i].key, 0, false, mac);
        nk_input_end(&ctx);
        test_edit_frame(&ctx, &edit);
        ok = ok && edit.cursor == cases[i].cursor;
        for (int key = 0; key < NK_KEY_MAX; key++)
            ok = ok && !ctx.input.keyboard.keys[key].down;
    }
    /* A quick press/release within one frame must undo or redo exactly once. */
    for (unsigned i = 0; i < 4; i++) {
        SDL_Keycode key = i == 3 ? (mac ? SDLK_R : SDLK_Y) : SDLK_Z;
        SDL_Keymod mod = (SDL_Keymod)(command | (i == 1 ? SDL_KMOD_SHIFT : 0));
        nk_input_begin(&ctx);
        nk_sdl_edit_key(&ctx, key, mod, true, mac);
        nk_sdl_edit_key(&ctx, key, 0, false, mac);
        nk_input_end(&ctx);
        test_edit_frame(&ctx, &edit);
        ok = ok && nk_str_len_char(&edit.string) == (i % 2 ? 18 : 0);
    }
    nk_free(&ctx);
    return ok;
}
bool nk_sdl_test_input(SDL_Window *window, ps_graphics *graphics) {
    struct nk_context *ctx = nk_sdl_init(window, graphics);
    if (!ctx)
        return false;
    SDL_WindowID id = SDL_GetWindowID(window);
    const char text[] = "A\xc3\xa4\xe2\x82\xac";
    SDL_Event event = {0};
    nk_input_begin(ctx);
    event.type = SDL_EVENT_TEXT_INPUT;
    event.text.windowID = id;
    event.text.text = text;
    nk_sdl_handle_event(ctx, &event);
    bool ok = ctx->input.keyboard.text_len == sizeof(text) - 1 &&
              !memcmp(ctx->input.keyboard.text, text, sizeof(text) - 1);
    event.type = SDL_EVENT_KEY_DOWN;
    event.key.windowID = id;
    event.key.key = SDLK_LSHIFT;
    event.key.down = true;
    nk_sdl_handle_event(ctx, &event);
    ok = ok && ctx->input.keyboard.keys[NK_KEY_SHIFT].down;
    event.key.key = SDLK_C;
    event.key.mod = PS_UI_COMMAND_MOD;
    nk_sdl_handle_event(ctx, &event);
    ok = ok && ctx->input.keyboard.keys[NK_KEY_COPY].down;
    event.type = SDL_EVENT_KEY_UP;
    event.key.down = false;
    nk_sdl_handle_event(ctx, &event);
    ok = ok && !ctx->input.keyboard.keys[NK_KEY_COPY].down;
    event.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
    event.button.windowID = id;
    event.button.button = SDL_BUTTON_RIGHT;
    event.button.down = true;
    event.button.x = 100;
    event.button.y = 100;
    nk_sdl_handle_event(ctx, &event);
    ok = ok && ctx->input.mouse.buttons[NK_BUTTON_RIGHT].down;
    event.type = SDL_EVENT_WINDOW_FOCUS_LOST;
    event.window.windowID = id;
    nk_sdl_handle_event(ctx, &event);
    ok = ok && !ctx->input.keyboard.keys[NK_KEY_SHIFT].down &&
         !ctx->input.mouse.buttons[NK_BUTTON_RIGHT].down;
    nk_input_end(ctx);
    char buffer[32];
    struct nk_text_edit edit;
    nk_textedit_init_fixed(&edit, buffer, sizeof buffer);
    edit.mode = NK_TEXT_EDIT_MODE_INSERT;
    ok = ok && nk_sdl_paste_text(&edit, text) && edit.cursor == 3 &&
         nk_str_len_char(&edit.string) == sizeof(text) - 1 &&
         !memcmp(nk_str_get_const(&edit.string), text, sizeof(text) - 1);
    nk_textedit_undo(&edit);
    ok = ok && nk_str_len_char(&edit.string) == 0 && edit.cursor == 0;
    nk_textedit_redo(&edit);
    ok = ok && nk_str_len_char(&edit.string) == sizeof(text) - 1 && edit.cursor == 3;
    edit.single_line=1;edit.select_start=0;edit.select_end=3;
    ok = ok && !nk_sdl_paste_text(&edit,"a\nb") && !nk_sdl_paste_text(&edit,"a\r\nb") && !nk_sdl_paste_text(&edit,"a\tb") &&
         edit.select_start==0 && edit.select_end==3 && nk_str_len_char(&edit.string)==sizeof(text)-1;
    edit.single_line=0;
    char *oversized = malloc(262146);
    if (oversized) {
        memset(oversized, 'x', 262145);
        oversized[262145] = 0;
        edit.select_start = 0;
        edit.select_end = 3;
        ok = ok && !nk_sdl_paste_text(&edit, oversized) && edit.select_end == 3 &&
             nk_str_len_char(&edit.string) == sizeof(text) - 1;
        free(oversized);
    } else
        ok = false;
    ok = test_editor_navigation(false) && ok;
    ok = test_editor_navigation(true) && ok;
    nk_sdl_shutdown(ctx);
    if (!ok)
        SDL_SetError("SDL input test: Unicode, clipboard, editor navigation or focus reset failed");
    else
        puts("INPUT TEST: UTF-8, undo/redo, clipboard, Mac/PC navigation/selection, focus PASSED");
    return ok;
}
