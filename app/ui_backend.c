/* Third-party implementations are isolated from the application's warning policy. */
#ifdef _MSC_VER
#pragma warning(push, 0)
#pragma warning(disable : 4116 4701 4706)
#endif
#define NK_IMPLEMENTATION
#include "ui.h"
#include <limits.h>
#include <math.h>
void ps_ui_draw_list_clip(struct nk_draw_list *list,struct nk_rect bounds) {
 if(list)nk_draw_list_add_clip(list,bounds);
}
static unsigned font_be32(const unsigned char *p) {
 return ((unsigned)p[0]<<24)|((unsigned)p[1]<<16)|((unsigned)p[2]<<8)|p[3];
}
struct nk_font *ps_ui_font_add_file_face(struct nk_font_atlas *atlas,const char *path,
                                        float height,const struct nk_font_config *config,bool bold) {
 if(!atlas || !path || !atlas->permanent.alloc || !atlas->permanent.free)return NULL;
 nk_size length=0;unsigned char *bytes=nk_file_load(path,&length,&atlas->permanent);
 if(!bytes)return NULL;
 int selected=-1;
 int count=length>=12?stbtt_GetNumberOfFonts(bytes):0;
 if(length>=12 && !memcmp(bytes,"ttcf",4) && (count<0 || (nk_size)count>(length-12)/4))count=0;
 for(int i=0;i<count;i++) {
  int offset=stbtt_GetFontOffsetForIndex(bytes,i);stbtt_fontinfo info={0};
  if(offset<0 || (nk_size)offset>length || length-(nk_size)offset<12)continue;
  unsigned tables=((unsigned)bytes[offset+4]<<8)|bytes[offset+5];
  if(tables>(length-(nk_size)offset-12)/16)continue;
  bool bounded=true;
  for(unsigned t=0;t<tables;t++) {
   const unsigned char *record=bytes+offset+12+t*16;
   unsigned begin=font_be32(record+8),size=font_be32(record+12);
   if((nk_size)begin>length || size>length-(nk_size)begin){bounded=false;break;}
  }
  if(!bounded || !stbtt_InitFont(&info,bytes,offset) ||
     info.head<0 || (nk_size)info.head>length || length-(nk_size)info.head<46)continue;
  unsigned style=((unsigned)bytes[info.head+44]<<8)|bytes[info.head+45];
  if((style&3u)==(bold?1u:0u)){selected=i;break;}
 }
 if(selected<0){atlas->permanent.free(atlas->permanent.userdata,bytes);return NULL;}
 struct nk_font_config cfg=config?*config:nk_font_config(height);
 cfg.ttf_blob=bytes;cfg.ttf_size=length;cfg.ttf_index=selected;cfg.size=height;cfg.ttf_data_owned_by_atlas=1;
 struct nk_font *font=nk_font_atlas_add(atlas,&cfg);
 if(!font)atlas->permanent.free(atlas->permanent.userdata,bytes);
 return font;
}
bool ps_ui_font_has_glyph(const struct nk_user_font *handle,nk_rune codepoint) {
 if(!handle || !handle->userdata.ptr || codepoint>0x10ffffu)return false;
 const struct nk_font *font=handle->userdata.ptr;
 const struct nk_font_glyph *glyph=nk_font_find_glyph(font,codepoint);
 return glyph && glyph->codepoint==codepoint && glyph->source_present;
}
float ps_ui_font_em_ratio(const struct nk_font *font) {
 if(!font || !font->config || !font->config->ttf_blob || font->config->ttf_size<12)return 1;
 const unsigned char *bytes=font->config->ttf_blob;stbtt_fontinfo info={0};
 int offset=stbtt_GetFontOffsetForIndex(bytes,font->config->ttf_index);
 if(offset<0 || !stbtt_InitFont(&info,bytes,offset))return 1;
 float pixel=stbtt_ScaleForPixelHeight(&info,1),em=stbtt_ScaleForMappingEmToPixels(&info,1);
 float ratio=em/pixel;
 return isfinite(ratio) && ratio>=.5f && ratio<=3?ratio:1;
}
enum { PS_UI_PAIR_CACHE=512 };
typedef struct {nk_rune first,second;int units;} ps_ui_pair_entry;
struct ps_ui_font_pairs {
 struct nk_allocator allocator;
 stbtt_fontinfo font;
 ps_ui_pair_entry *cache;
 float height_span;
};
ps_ui_font_pairs *ps_ui_font_pairs_create(const struct nk_font *font,const struct nk_allocator *allocator) {
 if(!font || !font->config || !allocator || !allocator->alloc || !allocator->free)return NULL;
 const struct nk_font_config *cfg=font->config;
 size_t overhead=sizeof(ps_ui_font_pairs)+PS_UI_PAIR_CACHE*sizeof(ps_ui_pair_entry);
 if(!cfg->ttf_blob || cfg->ttf_size<12 || cfg->ttf_size>INT_MAX || cfg->ttf_index<0 ||
    cfg->ttf_size>SIZE_MAX-overhead)return NULL;
 ps_ui_font_pairs *pairs=allocator->alloc(allocator->userdata,NULL,overhead+cfg->ttf_size);
 if(!pairs)return NULL;
 memset(pairs,0,overhead);pairs->allocator=*allocator;pairs->cache=(ps_ui_pair_entry *)(pairs+1);
 unsigned char *bytes=(unsigned char *)(pairs->cache+PS_UI_PAIR_CACHE);memcpy(bytes,cfg->ttf_blob,cfg->ttf_size);
 int offset=stbtt_GetFontOffsetForIndex(bytes,cfg->ttf_index);
 if(offset<0 || (nk_size)offset>cfg->ttf_size-12 || !stbtt_InitFont(&pairs->font,bytes,offset)) {
  ps_ui_font_pairs_destroy(pairs);return NULL;
 }
 int ascent,descent;stbtt_GetFontVMetrics(&pairs->font,&ascent,&descent,NULL);
 pairs->height_span=(float)(ascent-descent);
 if(pairs->height_span<=0){ps_ui_font_pairs_destroy(pairs);return NULL;}
 return pairs;
}
void ps_ui_font_pairs_destroy(ps_ui_font_pairs *pairs) {
 if(pairs)pairs->allocator.free(pairs->allocator.userdata,pairs);
}
float ps_ui_font_pair_advance(const ps_ui_font_pairs *pairs,float height,nk_rune first,nk_rune second) {
 if(!pairs || !first || !second || first>0x10ffffu || second>0x10ffffu || !isfinite(height) || height<=0)return 0;
 unsigned index=(first*0x9e3779b1u^second)&(PS_UI_PAIR_CACHE-1);
 ps_ui_pair_entry *entry=&pairs->cache[index];
 if(entry->first!=first || entry->second!=second) {
  int a=stbtt_FindGlyphIndex(&pairs->font,(int)first),b=stbtt_FindGlyphIndex(&pairs->font,(int)second);
  entry->units=a && b?stbtt_GetGlyphKernAdvance(&pairs->font,a,b):0;
  entry->first=first;entry->second=second;
 }
 /* Preserve stb's arithmetic order; cache only font units, never a size. */
 return entry->units*(height/pairs->height_span);
}
bool ps_ui_font_logical_scale(struct nk_font_atlas *atlas,float raster_scale) {
 if(!atlas || !atlas->glyphs || !isfinite(raster_scale) || raster_scale<1 || raster_scale>4)return false;
 for(struct nk_font *f=atlas->fonts;f;f=f->next)
  if(!isfinite(f->info.height) || f->info.height<=0)return false;
 for(struct nk_font *f=atlas->fonts;f;f=f->next) {
  f->handle.height=f->info.height/raster_scale;f->scale=1/raster_scale;
 }
 return true;
}
bool ps_ui_font_add_fallback(struct nk_font_atlas *atlas,struct nk_font *target,
                             const char *path,const nk_rune *ranges) {
 if(!atlas || !target || !path || !ranges || !target->config || target->glyphs)return false;
 struct nk_font *last=atlas->fonts;while(last && last->next)last=last->next;
 if(last!=target)return false;
 size_t count=0;
 for(const nk_rune *r=ranges;*r;r+=2) {
  if(r[0]>r[1] || r[1]>0x10ffffu)return false;
  size_t n=(size_t)r[1]-r[0]+1;
  if(n>SIZE_MAX-count)return false;count+=n;
 }
 if(count>(SIZE_MAX/sizeof(nk_rune)-1)/2)return false;
 nk_size length=0;void *bytes=nk_file_load(path,&length,&atlas->permanent);
 if(!bytes)return false;
 stbtt_fontinfo info={0};int offset=stbtt_GetFontOffsetForIndex(bytes,0);
 if(offset<0 || !stbtt_InitFont(&info,bytes,offset)) {
  atlas->permanent.free(atlas->permanent.userdata,bytes);return false;
 }
 nk_rune *missing=atlas->permanent.alloc(atlas->permanent.userdata,NULL,(count*2+1)*sizeof *missing);
 if(!missing){atlas->permanent.free(atlas->permanent.userdata,bytes);return false;}
 /* First mark candidate glyphs. Even slots leave room to compress surviving
  * codepoints into range pairs in place, without another allocation. */
 size_t candidates=0;
 for(const nk_rune *r=ranges;*r;r+=2)for(nk_rune c=r[0];c<=r[1];c++) {
  if(stbtt_FindGlyphIndex(&info,(int)c))missing[2*candidates++]=c;
 }
 /* Parse each existing source once per addition, rather than once for every
  * candidate codepoint. Font selection, ranges and glyph priority stay intact. */
 const struct nk_font_config *source=target->config;
 do {
  stbtt_fontinfo prior={0};
  int source_offset=source->ttf_blob && source->ttf_index>=0?stbtt_GetFontOffsetForIndex(source->ttf_blob,source->ttf_index):-1;
  if(source_offset<0 || !stbtt_InitFont(&prior,source->ttf_blob,source_offset))continue;
  size_t remaining=0;
  for(size_t i=0;i<candidates;i++) {
   nk_rune c=missing[2*i];
   bool declared=false;
   for(const nk_rune *r=source->range;r && *r;r+=2)
    if(c>=r[0] && c<=r[1]){declared=true;break;}
   if(!declared || !stbtt_FindGlyphIndex(&prior,(int)c))missing[2*remaining++]=c;
  }
  candidates=remaining;
 }while((source=source->n)!=target->config);
 size_t used=0;
 for(size_t i=0;i<candidates;i++) {
  nk_rune c=missing[2*i];if(!c)continue;
  if(used && missing[used-1]+1==c)missing[used-1]=c;
  else {missing[used++]=c;missing[used++]=c;}
 }
 missing[used]=0;
 if(!used) {
  atlas->permanent.free(atlas->permanent.userdata,missing);
  atlas->permanent.free(atlas->permanent.userdata,bytes);return true;
 }
 struct nk_font_config config=nk_font_config(target->config->size);
 config.ttf_blob=bytes;config.ttf_size=length;config.ttf_data_owned_by_atlas=1;
 config.range=missing;config.range_owned_by_atlas=1;config.merge_mode=1;
 config.oversample_h=target->config->oversample_h;config.oversample_v=target->config->oversample_v;
 if(nk_font_atlas_add(atlas,&config)!=target) {
  atlas->permanent.free(atlas->permanent.userdata,missing);
  atlas->permanent.free(atlas->permanent.userdata,bytes);return false;
 }
 return true;
}
#include <math.h>
#undef nk_checkbox_label
#undef nk_label_wrap
#undef nk_button_label
#undef nk_label
#undef nk_label_colored
/* Resolution independent outline icons; semantic names remain available to
 * assistive technology even when the visual control contains no text. */
void ps_ui_draw_icon(struct nk_command_buffer *c,ps_ui_icon icon,struct nk_rect b,struct nk_color color) {
    float x=floorf(b.x+b.w/2-7),y=floorf(b.y+b.h/2-7),t=1.25f;
    switch(icon) {
    case PS_ICON_CLOSE:
        nk_stroke_line(c,x+3,y+3,x+11,y+11,t,color);nk_stroke_line(c,x+3,y+11,x+11,y+3,t,color);break;
    case PS_ICON_DRAG:
        for(int row=0;row<3;row++)for(int col=0;col<2;col++)nk_fill_circle(c,nk_rect(x+4+col*4,y+2+row*4,2,2),color);break;
    case PS_ICON_FOLDER:
        nk_stroke_rect(c,nk_rect(x+1,y+4,12,9),1,t,color);nk_stroke_line(c,x+1,y+4,x+1,y+1,t,color);
        nk_stroke_line(c,x+1,y+1,x+6,y+1,t,color);nk_stroke_line(c,x+6,y+1,x+8,y+4,t,color);break;
    case PS_ICON_FILE:
        nk_stroke_rect(c,nk_rect(x+3,y+1,9,12),1,t,color);nk_stroke_line(c,x+5,y+6,x+10,y+6,t,color);
        nk_stroke_line(c,x+5,y+9,x+10,y+9,t,color);break;
    case PS_ICON_SEARCH:
        nk_stroke_circle(c,nk_rect(x+1,y+1,9,9),t,color);nk_stroke_line(c,x+9,y+9,x+13,y+13,t,color);break;
    case PS_ICON_SAVE:
        nk_stroke_rect(c,nk_rect(x+1,y+1,12,12),1,t,color);nk_stroke_rect(c,nk_rect(x+4,y+1,6,4),0,t,color);
        nk_stroke_rect(c,nk_rect(x+4,y+8,6,5),0,t,color);break;
    case PS_ICON_BUILD:
        nk_stroke_line(c,x+2,y+12,x+11,y+3,2,color);nk_stroke_line(c,x+7,y+2,x+12,y+7,3,color);break;
    case PS_ICON_LEFT: case PS_ICON_RIGHT: {
        float sign=icon==PS_ICON_LEFT?-1.f:1.f;
        nk_stroke_line(c,x+7-sign*2,y+3,x+7+sign*2,y+7,t,color);
        nk_stroke_line(c,x+7+sign*2,y+7,x+7-sign*2,y+11,t,color);break;
    }
    case PS_ICON_ERROR:
        nk_stroke_circle(c,nk_rect(x+1,y+1,12,12),t,color);nk_stroke_line(c,x+7,y+4,x+7,y+8,t,color);
        nk_fill_circle(c,nk_rect(x+6,y+10,2,2),color);break;
    case PS_ICON_FUNCTION:
        nk_stroke_line(c,x+5,y+2,x+2,y+7,t,color);nk_stroke_line(c,x+2,y+7,x+5,y+12,t,color);
        nk_stroke_line(c,x+9,y+2,x+12,y+7,t,color);nk_stroke_line(c,x+12,y+7,x+9,y+12,t,color);break;
    case PS_ICON_TYPE:
        nk_stroke_rect(c,nk_rect(x+2,y+2,10,10),1,t,color);break;
    case PS_ICON_VALUE:
        nk_stroke_circle(c,nk_rect(x+3,y+3,8,8),t,color);break;
    case PS_ICON_FILE_ADD: case PS_ICON_FOLDER_ADD:
        ps_ui_draw_icon(c,icon==PS_ICON_FILE_ADD?PS_ICON_FILE:PS_ICON_FOLDER,b,color);
        nk_stroke_line(c,x+10,y+8,x+10,y+14,2,color);nk_stroke_line(c,x+7,y+11,x+13,y+11,2,color);break;
    case PS_ICON_WINDOW: case PS_ICON_SIDEBAR: case PS_ICON_INSPECTOR:
        nk_stroke_rect(c,nk_rect(x+1,y+2,12,10),1,t,color);
        nk_stroke_line(c,x+1,y+5,x+13,y+5,t,color);
        if(icon!=PS_ICON_WINDOW)nk_stroke_line(c,x+(icon==PS_ICON_SIDEBAR?5:9),y+5,x+(icon==PS_ICON_SIDEBAR?5:9),y+12,t,color);break;
    case PS_ICON_LAYOUT: case PS_ICON_LAYOUT_RESET:
        nk_stroke_rect(c,nk_rect(x+1,y+2,12,10),1,t,color);
        nk_stroke_line(c,x+5,y+2,x+5,y+12,t,color);nk_stroke_line(c,x+5,y+8,x+13,y+8,t,color);
        if(icon==PS_ICON_LAYOUT_RESET){nk_stroke_line(c,x+9,y+1,x+13,y+1,t,color);nk_stroke_line(c,x+13,y+1,x+13,y+5,t,color);}break;
    case PS_ICON_CONSOLE:
        nk_stroke_rect(c,nk_rect(x+1,y+2,12,10),1,t,color);
        nk_stroke_line(c,x+3,y+5,x+5,y+7,t,color);nk_stroke_line(c,x+5,y+7,x+3,y+9,t,color);
        nk_stroke_line(c,x+7,y+9,x+11,y+9,t,color);break;
    case PS_ICON_REFRESH:
        nk_stroke_arc(c,x+7,y+7,5,0.4f,5.5f,t,color);
        nk_stroke_line(c,x+12,y+3,x+12,y+7,t,color);nk_stroke_line(c,x+12,y+7,x+8,y+7,t,color);break;
    case PS_ICON_CHART:
        nk_stroke_line(c,x+1,y+1,x+1,y+13,t,color);nk_stroke_line(c,x+1,y+13,x+13,y+13,t,color);
        nk_stroke_line(c,x+3,y+10,x+6,y+6,t,color);nk_stroke_line(c,x+6,y+6,x+9,y+8,t,color);nk_stroke_line(c,x+9,y+8,x+12,y+3,t,color);break;
    case PS_ICON_DICE:
        nk_stroke_rect(c,nk_rect(x+1,y+1,12,12),2,t,color);
        for(int i=0;i<3;i++)nk_fill_circle(c,nk_rect(x+3+i*3,y+3+i*3,2,2),color);break;
    case PS_ICON_BOOK: case PS_ICON_REFERENCE:
        nk_stroke_rect(c,nk_rect(x+1,y+2,12,10),1,t,color);nk_stroke_line(c,x+7,y+2,x+7,y+12,t,color);
        nk_stroke_line(c,x+3,y+5,x+5,y+5,t,color);nk_stroke_line(c,x+9,y+5,x+11,y+5,t,color);
        if(icon==PS_ICON_REFERENCE){nk_stroke_line(c,x+3,y+8,x+5,y+8,t,color);nk_stroke_line(c,x+9,y+8,x+11,y+8,t,color);}break;
    case PS_ICON_COMPLETION:
        nk_stroke_line(c,x+6,y+1,x+6,y+11,t,color);nk_stroke_line(c,x+1,y+6,x+11,y+6,t,color);
        nk_stroke_line(c,x+2,y+2,x+10,y+10,t,color);nk_stroke_line(c,x+2,y+10,x+10,y+2,t,color);
        nk_fill_circle(c,nk_rect(x+11,y+11,2,2),color);break;
    }
}
void ps_ui_flush_edit(struct nk_context *ctx,const char *name,char *text,size_t capacity,struct nk_rect bounds) {
    if(!ctx || !text || capacity<2 || !ctx->style.font)return;
    struct nk_window *window=nk_find_window(ctx,nk_murmur_hash(name,nk_strlen(name),NK_WINDOW_TITLE),name);
    if(!window)return;
    size_t length=strlen(text);if(length>=capacity)return;
    struct nk_text_edit edit;nk_textedit_init_fixed(&edit,text,capacity-1);
    edit.string.buffer.allocated=length;edit.string.len=nk_utf_len(text,(int)length);
    edit.cursor=window->edit.cursor;edit.select_start=window->edit.sel_start;edit.select_end=window->edit.sel_end;
    edit.active=nk_true;edit.mode=NK_TEXT_EDIT_MODE_INSERT;edit.clip=ctx->clip;nk_textedit_clamp(&edit);
    struct nk_input input=ctx->input;
    input.mouse.buttons[NK_BUTTON_LEFT].clicked=0;input.mouse.buttons[NK_BUTTON_RIGHT].clicked=0;
    char storage[4096];struct nk_buffer commands;struct nk_command_buffer canvas;
    nk_buffer_init_fixed(&commands,storage,sizeof storage);nk_command_buffer_init(&canvas,&commands,NK_CLIPPING_OFF);
    canvas.clip=nk_rect(0,0,0,0);nk_flags state=0;
    nk_do_edit(&state,&canvas,bounds,NK_EDIT_FIELD,nk_filter_default,&edit,&ctx->style.edit,&input,ctx->style.font);
    text[edit.string.buffer.allocated]=0;
    window->edit.cursor=edit.cursor;window->edit.sel_start=edit.select_start;window->edit.sel_end=edit.select_end;window->edit.mode=edit.mode;
    ctx->input.keyboard.text_len=0;
    for(int i=0;i<NK_KEY_MAX;i++)ctx->input.keyboard.keys[i].clicked=0;
}
float nk_sdl_row_height(const struct nk_context *ctx,float requested) {
    if(!ctx || !ctx->userdata.ptr || requested<16)return requested;
    const ps_ui_font_layout *layout=ctx->userdata.ptr;
    if(layout->magic!=PS_UI_LAYOUT_MAGIC)return requested;
    unsigned size=layout->ui_size;
    return size>16 && requested<(float)size+4?(float)size+4:requested;
}
void ps_ui_accessibility_route(struct nk_context *ctx,bool allowed) {
 if(!ctx || !ctx->userdata.ptr)return;
 ps_ui_font_layout *layout=ctx->userdata.ptr;if(layout->magic!=PS_UI_LAYOUT_MAGIC)return;
 layout->accessibility_route_window=ctx->current;layout->accessibility_route_allowed=allowed;
}
void ps_ui_draw_backdrop(struct nk_context *ctx,const ps_ui_backdrop *material) {
    if(!ctx || !ctx->current || !material)return;
    struct nk_command_buffer *canvas=nk_window_get_canvas(ctx);struct nk_rect clip=canvas->clip;
    nk_handle userdata=ctx->userdata;
    /* Solid shapes do not split conversion batches on userdata alone. The
     * scissor commands isolate this material from the ordinary UI shader. */
    nk_set_user_data(ctx,nk_handle_ptr((void*)material));nk_push_scissor(canvas,clip);
    nk_fill_rect(canvas,nk_rect(material->x,material->y,material->w,material->h),material->blur_only?0:16,nk_rgb(255,255,255));
    nk_set_user_data(ctx,userdata);nk_push_scissor(canvas,clip);
}
void ps_ui_draw_shadow(struct nk_context *ctx,const ps_ui_shadow *shadow) {
    if(!ctx || !ctx->current || !shadow || !isfinite(shadow->x) || !isfinite(shadow->y) ||
       !isfinite(shadow->w) || !isfinite(shadow->h) || !isfinite(shadow->radius) ||
       !isfinite(shadow->offset_x) || !isfinite(shadow->offset_y) || !isfinite(shadow->blur) ||
       shadow->w<=0 || shadow->h<=0 || shadow->blur<=0 || shadow->radius<0 ||
       shadow->radius>shadow->blur*.15f || shadow->radius>fminf(shadow->w,shadow->h)*.5f)return;
    struct nk_command_buffer *canvas=nk_window_get_canvas(ctx);struct nk_rect clip=canvas->clip;
    nk_handle userdata=ctx->userdata;float halo=2*shadow->blur;
    nk_set_user_data(ctx,nk_handle_ptr((void*)shadow));nk_push_scissor(canvas,clip);
    nk_fill_rect(canvas,nk_rect(floorf(shadow->x+shadow->offset_x-halo),floorf(shadow->y+shadow->offset_y-halo),
        ceilf(shadow->w+2*halo+1),ceilf(shadow->h+2*halo+1)),0,nk_rgba(shadow->red,shadow->green,shadow->blue,shadow->alpha));
    nk_set_user_data(ctx,userdata);nk_push_scissor(canvas,clip);
}
void ps_ui_set_focus_color(struct nk_context *ctx,struct nk_color color) {
    if(!ctx || !ctx->userdata.ptr)return;
    ps_ui_font_layout *layout=ctx->userdata.ptr;if(layout->magic==PS_UI_LAYOUT_MAGIC)layout->focus_color=color;
}
struct nk_color ps_ui_focus_color(const struct nk_context *ctx) {
    if(ctx && ctx->userdata.ptr) {
        const ps_ui_font_layout *layout=ctx->userdata.ptr;
        if(layout->magic==PS_UI_LAYOUT_MAGIC && layout->focus_color.a)return layout->focus_color;
    }
    return ctx?ctx->style.button.text_normal:nk_rgb(255,255,255);
}
static bool accessibility_emit(struct nk_context *ctx,const char *text,int role,struct nk_rect bounds,bool checked,const char *group,bool activated,double *range,bool *focused) {
    if(focused)*focused=false;
    if(!ctx || !ctx->userdata.ptr || !ctx->current || !ctx->current->layout || !text)return false;
    const ps_ui_font_layout *layout=ctx->userdata.ptr;
    if(layout->magic!=PS_UI_LAYOUT_MAGIC || !layout->accessibility)return false;
    struct nk_rect clip=ctx->current->layout->clip;
    float left=fmaxf(bounds.x,clip.x),top=fmaxf(bounds.y,clip.y);
    float right=fminf(bounds.x+bounds.w,clip.x+clip.w),bottom=fminf(bounds.y+bounds.h,clip.y+clip.h);
    if(right<=left || bottom<=top)return false;
    float visible[]={left,top,right-left,bottom-top};
    bool routed=layout->accessibility_route_window==ctx->current;
    bool enabled=!ctx->current->widgets_disabled && (routed?layout->accessibility_route_allowed:!(ctx->current->flags&NK_WINDOW_ROM));
    /* A native target must not activate a control underneath a higher window. */
    for(const struct nk_window *above=ctx->current->next;above && enabled;above=above->next) {
        if(above->flags&(NK_WINDOW_HIDDEN|NK_WINDOW_MINIMIZED|NK_WINDOW_CLOSED))continue;
        /* Crossing nine-pixel separator targets remain independent controls.
         * Ordinary windows and popups still block the separator beneath them. */
        if(role==8 && (!strncmp(above->name_string,"dock-splitter-",14) || !strncmp(above->name_string,"laboratory-separator-",21)))continue;
        struct nk_rect cover=above->bounds;
        if(cover.x<right && cover.x+cover.w>left && cover.y<bottom && cover.y+cover.h>top)enabled=false;
    }
    unsigned result=range?(layout->accessibility_range?layout->accessibility_range(layout->accessibility_user,ctx->current->name_string,text,role,visible,enabled,range):0)
        :layout->accessibility(layout->accessibility_user,ctx->current->name_string,text,role,visible,enabled,checked,group,activated);
    if(result&2u) {
        struct nk_window *owner=ctx->current;
        while(owner->parent)owner=owner->parent;
        /* Internal popup windows are not in Nuklear's top-level list. Asking
         * set_focus for their name would clear ctx->active instead. */
        nk_window_set_focus(ctx,owner->name_string);
    }
    if(focused)*focused=(result&4u)!=0;
    else if((result&4u) && right-left>3 && bottom-top>3)nk_stroke_rect(nk_window_get_canvas(ctx),nk_rect(left+1,top+1,right-left-2,bottom-top-2),3,2,ps_ui_focus_color(ctx));
    return (result&1u)!=0;
}
static bool accessibility_widget(struct nk_context *ctx,const char *text,int role,struct nk_rect bounds,bool checked,const char *group,bool activated) {
    return accessibility_emit(ctx,text,role,bounds,checked,group,activated,NULL,NULL);
}
nk_bool ps_ui_slider_named(struct nk_context *ctx,const char *name,float minimum,float *value,float maximum,float increment) {
    return ps_ui_slider_named_custom_focus(ctx,name,minimum,value,maximum,increment,NULL);
}
nk_bool ps_ui_slider_named_custom_focus(struct nk_context *ctx,const char *name,float minimum,float *value,float maximum,float increment,bool *focused) {
    if(focused)*focused=false;
    if(!ctx || !value)return nk_false;
    struct nk_rect bounds=nk_widget_bounds(ctx);
    nk_bool changed=nk_slider_float(ctx,minimum,value,maximum,increment);
    double range[]={*value,minimum,maximum,increment};
    if(accessibility_emit(ctx,name,5,bounds,false,NULL,false,range,focused)){*value=(float)range[0];changed=nk_true;}
    return changed;
}
nk_bool ps_ui_range_at(struct nk_context *ctx,struct nk_rect bounds,const char *name,float *value,float maximum,float increment) {
    return ps_ui_range_at_custom_focus(ctx,bounds,name,value,maximum,increment,NULL);
}
nk_bool ps_ui_range_at_custom_focus(struct nk_context *ctx,struct nk_rect bounds,const char *name,float *value,float maximum,float increment,bool *focused) {
    if(focused)*focused=false;
    if(!ctx || !value || !isfinite(*value) || !isfinite(maximum) || maximum<=0 || !isfinite(increment) || increment<=0)return nk_false;
    double range[]={*value,0,maximum,increment};
    if(accessibility_emit(ctx,name,5,bounds,false,NULL,false,range,focused)){*value=(float)range[0];return nk_true;}
    return nk_false;
}
nk_bool ps_ui_horizontal_scrollbar_at(struct nk_context *ctx,struct nk_rect r,float *value,float maximum,float page,ps_ui_scrollbar_drag *drag,bool input_enabled) {
    if(!ctx || !ctx->current || !value || !drag || !isfinite(r.x) || !isfinite(r.y) || !isfinite(r.w) || !isfinite(r.h) ||
       r.w<=0 || r.h<=0 || !isfinite(*value) || !isfinite(maximum) || maximum<=0 || !isfinite(page) || page<=0 || !isfinite(maximum+page))return nk_false;
    float before=*value;*value=fmaxf(0,fminf(maximum,*value));
    struct nk_rect clip=ctx->current->layout->clip;
    bool visible=r.x<clip.x+clip.w && r.x+r.w>clip.x && r.y<clip.y+clip.h && r.y+r.h>clip.y;
    bool enabled=input_enabled && visible && !(ctx->current->layout->flags&NK_WINDOW_ROM) && !ctx->current->widgets_disabled;
    struct nk_input *input=&ctx->input;
    float thumb=fminf(r.w,fmaxf(18,r.w*page/(maximum+page))),travel=r.w-thumb;
    struct nk_rect cursor=nk_rect(r.x+(travel>0?travel*(*value/maximum):0),r.y,thumb,r.h);
    if(!enabled || !nk_input_is_mouse_down(input,NK_BUTTON_LEFT))drag->active=false;
    if(enabled && nk_input_is_mouse_pressed(input,NK_BUTTON_LEFT) && nk_input_is_mouse_hovering_rect(input,r)) {
        if(nk_input_is_mouse_hovering_rect(input,cursor)) {
            drag->active=true;drag->pointer=input->mouse.pos.x;drag->value=*value;
        } else *value=fmaxf(0,fminf(maximum,*value+(input->mouse.pos.x<cursor.x?-page:page)));
    }
    if(drag->active && travel>0)*value=fmaxf(0,fminf(maximum,drag->value+(input->mouse.pos.x-drag->pointer)*maximum/travel));
    cursor.x=r.x+(travel>0?travel*(*value/maximum):0);
    nk_flags state=drag->active?NK_WIDGET_STATE_ACTIVE:nk_input_is_mouse_hovering_rect(input,r)?NK_WIDGET_STATE_HOVERED:0;
    if(visible) {
        const struct nk_style_scrollbar *style=&ctx->style.scrollh;struct nk_command_buffer *canvas=nk_window_get_canvas(ctx);
        if(style->draw_begin)style->draw_begin(canvas,style->userdata);
        nk_draw_scrollbar(canvas,state,style,&r,&cursor);
        if(style->draw_end)style->draw_end(canvas,style->userdata);
    }
    return before!=*value;
}
nk_bool ps_ui_option_label(struct nk_context *ctx,const char *group,const char *text,nk_bool active) {
    if(!ctx)return active;
    struct nk_rect bounds=nk_widget_bounds(ctx);
    nk_bool selected=nk_option_label(ctx,text,active);
    return accessibility_widget(ctx,text,3,bounds,selected,group,selected&&!active) || selected;
}
nk_bool ps_ui_separator_at(struct nk_context *ctx,struct nk_rect bounds,const char *name,float minimum,float *value,float maximum) {
    return ps_ui_separator_step_at(ctx,bounds,name,minimum,value,maximum,10);
}
nk_bool ps_ui_separator_step_at(struct nk_context *ctx,struct nk_rect bounds,const char *name,float minimum,float *value,float maximum,float step) {
    double range[]={*value,minimum,maximum,step};bool focused=false;
    /* Every separator paints its own one-pixel focused line. Suppress the
     * generic inset rectangle around its nine-pixel input target. */
    if(accessibility_emit(ctx,name,8,bounds,false,NULL,false,range,&focused)){*value=(float)range[0];return nk_true;}
    return nk_false;
}
nk_bool ps_ui_checkbox_named(struct nk_context *ctx,const char *text,const char *name,nk_bool *active) {
    if(!ctx || !active)return nk_false;
    struct nk_rect bounds=nk_widget_bounds(ctx);
    nk_bool changed=nk_checkbox_label(ctx,text,active);
    if(accessibility_widget(ctx,name,2,bounds,*active,NULL,false)) {*active=!*active;changed=nk_true;}
    return changed;
}
nk_bool ps_ui_checkbox_label(struct nk_context *ctx,const char *text,nk_bool *active) {
    return ps_ui_checkbox_named(ctx,text,text,active);
}
void ps_ui_accessibility_label(struct nk_context *ctx,struct nk_rect bounds,const char *text) {
    (void)accessibility_widget(ctx,text,0,bounds,false,NULL,false);
}
void ps_ui_label(struct nk_context *ctx,const char *text,nk_flags alignment) {
    accessibility_widget(ctx,text,0,nk_widget_bounds(ctx),false,NULL,false);nk_label(ctx,text,alignment);
}
void ps_ui_label_colored(struct nk_context *ctx,const char *text,nk_flags alignment,struct nk_color color) {
    accessibility_widget(ctx,text,0,nk_widget_bounds(ctx),false,NULL,false);nk_label_colored(ctx,text,alignment,color);
}
nk_bool ps_ui_icon_button(struct nk_context *ctx,ps_ui_icon icon,const char *name) {
    struct nk_rect bounds=nk_widget_bounds(ctx);
    nk_bool pressed=accessibility_widget(ctx,name,1,bounds,false,NULL,false);
    nk_bool clicked=nk_button_label(ctx,"");
    ps_ui_draw_icon(nk_window_get_canvas(ctx),icon,bounds,ctx->style.button.text_normal);
    return pressed || clicked;
}
nk_bool ps_ui_button_at(struct nk_context *ctx,struct nk_rect bounds,const char *name,bool selected) {
    if(bounds.w<=0 || bounds.h<=0)return nk_false;
    nk_bool pressed=accessibility_widget(ctx,name,1,bounds,selected,NULL,false);
    bool enabled=!(ctx->current->layout->flags&NK_WINDOW_ROM) && !ctx->current->widgets_disabled;
    return pressed || (enabled && nk_input_is_mouse_pressed(&ctx->input,NK_BUTTON_LEFT) && nk_input_is_mouse_hovering_rect(&ctx->input,ctx->current->layout->clip) && nk_input_is_mouse_hovering_rect(&ctx->input,bounds));
}
nk_bool ps_ui_button_at_custom_focus(struct nk_context *ctx,struct nk_rect bounds,const char *name,bool selected,bool *focused) {
    if(focused)*focused=false;
    if(!ctx || !ctx->current || bounds.w<=0 || bounds.h<=0)return nk_false;
    nk_bool pressed=accessibility_emit(ctx,name,1,bounds,selected,NULL,false,NULL,focused);
    bool enabled=!(ctx->current->layout->flags&NK_WINDOW_ROM) && !ctx->current->widgets_disabled;
    return pressed || (enabled && nk_input_is_mouse_pressed(&ctx->input,NK_BUTTON_LEFT) && nk_input_is_mouse_hovering_rect(&ctx->input,ctx->current->layout->clip) && nk_input_is_mouse_hovering_rect(&ctx->input,bounds));
}
int ps_ui_text_line(const struct nk_user_font *font,const char *text,int length,float space,bool path) {
    if(!font || !font->width || !text || length<=0 || !isfinite(space) || space<=0)return 0;
    int bytes=0,separator=0;
    while(bytes<length) {
        nk_rune rune;int count=nk_utf_decode(text+bytes,&rune,length-bytes);
        if(count<=0)return 0;
        float width=font->width(font->userdata,font->height,text,bytes+count);
        if(!isfinite(width))return 0;
        if(width>space)return separator?separator:bytes;
        bytes+=count;
        if(rune==' ' || (path && (rune=='/' || rune=='\\')))separator=bytes;
    }
    return bytes;
}
nk_bool ps_ui_combo_begin_named(struct nk_context *ctx,const char *name,const char *label,struct nk_vec2 size) {
    return ps_ui_combo_begin_named_scaled(ctx,name,label,size,1);
}
nk_bool ps_ui_combo_begin_named_scaled(struct nk_context *ctx,const char *name,const char *label,struct nk_vec2 size,float scale) {
    if(!ctx || !ctx->current || !name || !label)return nk_false;
    if(!isfinite(scale) || scale<=0)return nk_false;
    struct nk_window *window=ctx->current;struct nk_rect bounds;
    if(nk_widget(&bounds,ctx)==NK_WIDGET_INVALID)return nk_false;
    bool expanded=window->popup.win && window->popup.type==NK_PANEL_COMBO && window->popup.name==window->popup.combo_count;
    nk_bool activated=accessibility_widget(ctx,name,1,bounds,expanded,NULL,false);
    bool enabled=!window->widgets_disabled && !(window->layout->flags&NK_WINDOW_ROM);
    bool clicked=enabled && nk_input_is_mouse_pressed(&ctx->input,NK_BUTTON_LEFT) && nk_input_is_mouse_hovering_rect(&ctx->input,ctx->current->layout->clip) && nk_input_is_mouse_hovering_rect(&ctx->input,bounds);
    const struct nk_user_font *font=ctx->style.font;struct nk_color color=ctx->style.combo.label_normal;
    nk_draw_text(&window->buffer,nk_rect(bounds.x+9*scale,bounds.y+(bounds.h-font->height)/2,fmaxf(1,bounds.w-42*scale),font->height),label,(int)strlen(label),font,nk_rgba(0,0,0,0),color);
    nk_draw_symbol(&window->buffer,NK_SYMBOL_CHEVRON_DOWN,nk_rect(bounds.x+bounds.w-25*scale,bounds.y+(bounds.h-8*scale)/2,8*scale,8*scale),nk_rgba(0,0,0,0),color,scale,font);
    ps_ui_accessibility_label(ctx,bounds,label);
    /* accessibility_widget already paints the hook's native focus result. */
    nk_bool open=nk_combo_begin(ctx,window,size,activated || clicked,bounds);
    if(open) {
        /* Nuklear's internal popup has no public name. Stable owner/name
         * identity lets the native model retain and activate its entries. */
        snprintf(ctx->current->name_string,sizeof ctx->current->name_string,"%.32s/menu/%08x",window->name_string,(unsigned)nk_murmur_hash(name,nk_strlen(name),0));
    }
    return open;
}
nk_bool ps_ui_combo_begin_named_painted(struct nk_context *ctx,const char *name,const char *label,struct nk_vec2 size,ps_ui_combo_painter paint,void *user) {
    if(!ctx || !ctx->current || !name || !label || !paint)return nk_false;
    struct nk_rect bounds;
    if(nk_widget(&bounds,ctx)==NK_WIDGET_INVALID)return nk_false;
    return ps_ui_combo_at_named_painted(ctx,name,label,bounds,size,paint,user);
}
nk_bool ps_ui_combo_at_named_painted(struct nk_context *ctx,const char *name,const char *label,struct nk_rect bounds,struct nk_vec2 size,ps_ui_combo_painter paint,void *user) {
    if(!ctx || !ctx->current || !name || !label || !paint || bounds.w<=0 || bounds.h<=0)return nk_false;
    struct nk_window *window=ctx->current;
    bool expanded=window->popup.win && window->popup.type==NK_PANEL_COMBO && window->popup.name==window->popup.combo_count,focused=false;
    bool activated=accessibility_emit(ctx,name,1,bounds,expanded,NULL,false,NULL,&focused);
    bool enabled=!window->widgets_disabled && !(window->layout->flags&NK_WINDOW_ROM);
    bool clicked=enabled && nk_input_is_mouse_pressed(&ctx->input,NK_BUTTON_LEFT) &&
        nk_input_is_mouse_hovering_rect(&ctx->input,window->layout->clip) && nk_input_is_mouse_hovering_rect(&ctx->input,bounds);
    paint(ctx,bounds,focused,user);ps_ui_accessibility_label(ctx,bounds,label);
    nk_bool open=nk_combo_begin(ctx,window,size,activated || clicked,bounds);
    if(open)snprintf(ctx->current->name_string,sizeof ctx->current->name_string,"%.32s/menu/%08x",window->name_string,(unsigned)nk_murmur_hash(name,nk_strlen(name),0));
    return open;
}
nk_bool ps_ui_switch_at(struct nk_context *ctx,struct nk_rect bounds,const char *name,nk_bool *checked) {
    return ps_ui_switch_at_custom_focus(ctx,bounds,name,checked,NULL);
}
nk_bool ps_ui_switch_at_custom_focus(struct nk_context *ctx,struct nk_rect bounds,const char *name,nk_bool *checked,bool *focused) {
    if(focused)*focused=false;
    if(!ctx || !checked || bounds.w<=0 || bounds.h<=0)return nk_false;
    bool enabled=!(ctx->current->layout->flags&NK_WINDOW_ROM) && !ctx->current->widgets_disabled;
    bool clicked=enabled && nk_input_is_mouse_pressed(&ctx->input,NK_BUTTON_LEFT) && nk_input_is_mouse_hovering_rect(&ctx->input,ctx->current->layout->clip) && nk_input_is_mouse_hovering_rect(&ctx->input,bounds);
    if(accessibility_emit(ctx,name,2,bounds,*checked,NULL,false,NULL,focused) || clicked){*checked=!*checked;return nk_true;}
    return nk_false;
}
nk_bool ps_ui_option_at(struct nk_context *ctx,struct nk_rect bounds,const char *group,const char *name,bool selected) {
    if(!ctx || !ctx->current || bounds.w<=0 || bounds.h<=0)return nk_false;
    bool enabled=!(ctx->current->layout->flags&NK_WINDOW_ROM) && !ctx->current->widgets_disabled;
    bool clicked=enabled && nk_input_is_mouse_pressed(&ctx->input,NK_BUTTON_LEFT) && nk_input_is_mouse_hovering_rect(&ctx->input,ctx->current->layout->clip) && nk_input_is_mouse_hovering_rect(&ctx->input,bounds);
    return accessibility_widget(ctx,name,3,bounds,selected || clicked,group,clicked) || clicked;
}
nk_bool ps_ui_accessibility_button(struct nk_context *ctx,struct nk_rect bounds,const char *name) {
    return accessibility_widget(ctx,name,1,bounds,false,NULL,false);
}
nk_bool ps_ui_fixed_button_label(struct nk_context *ctx,const char *text) {
    if(!ctx || !ctx->current || !text)return nk_false;
    struct nk_rect bounds=nk_widget_bounds(ctx);
    nk_bool clicked=nk_button_label(ctx,text);
    return accessibility_widget(ctx,text,1,bounds,false,NULL,false) || clicked;
}
nk_bool ps_ui_button_label(struct nk_context *ctx,const char *text) {
    if(!ctx || !ctx->current || !text || nk_sdl_row_height(ctx,16)<=16) {
        struct nk_rect bounds=nk_widget_bounds(ctx);
        nk_bool clicked=nk_button_label(ctx,text);
        bool pressed=accessibility_widget(ctx,text,1,bounds,false,NULL,false);
        return clicked || pressed;
    }
    const struct nk_user_font *font=ctx->style.font;
    const struct nk_style_button *style=&ctx->style.button;
    int length=nk_strlen(text);
    /* A single glyph (such as a panel close control) uses the full button
     * interior; ordinary label padding would hide it at large font sizes. */
    bool symbol=nk_utf_len(text,length)==1;
    float inset=symbol?style->border:style->padding.x+style->border+style->rounding;
    float space=nk_widget_width(ctx)-2*inset;
    if(space<=0 || (!symbol && font->width(font->userdata,font->height,text,length)<=space)) {
        struct nk_rect bounds=nk_widget_bounds(ctx);
        nk_bool clicked=nk_button_label(ctx,text);
        bool pressed=accessibility_widget(ctx,text,1,bounds,false,NULL,false);
        return clicked || pressed;
    }
    int done=0,lines=0,glyphs;float width;nk_rune separator=' ';
    while(done<length) {
        int fitting=nk_text_clamp(font,text+done,length-done,space,&glyphs,&width,&separator,1);
        if(fitting<=0)fitting=nk_text_clamp(font,text+done,length-done,space,&glyphs,&width,0,0);
        if(fitting<=0)break;
        done+=fitting;lines++;
    }
    float height=lines*font->height+2*(symbol?style->border:style->padding.y+style->border+style->rounding);
    if(height>nk_widget_height(ctx))ctx->current->layout->row.height=height+ctx->style.window.spacing.y;
    struct nk_rect bounds=nk_widget_bounds(ctx);
    nk_bool pressed=accessibility_widget(ctx,text,1,bounds,false,NULL,false);
    nk_bool clicked=nk_button_label(ctx,"");
    struct nk_color color=style->text_normal;
    if(nk_input_is_mouse_hovering_rect(&ctx->input,bounds))
        color=ctx->input.mouse.buttons[NK_BUTTON_LEFT].down?style->text_active:style->text_hover;
    color=nk_rgb_factor(color,style->color_factor_text);
    float y=bounds.y+(bounds.h-lines*font->height)/2;
    done=0;
    while(done<length) {
        int fitting=nk_text_clamp(font,text+done,length-done,space,&glyphs,&width,&separator,1);
        if(fitting<=0)fitting=nk_text_clamp(font,text+done,length-done,space,&glyphs,&width,0,0);
        if(fitting<=0)break;
        /* nk_text_clamp reports the width before its final decoded glyph. */
        width=font->width(font->userdata,font->height,text+done,fitting);
        nk_draw_text(nk_window_get_canvas(ctx),nk_rect(bounds.x+(bounds.w-width)/2,y,width+1,font->height),
                     text+done,fitting,font,nk_rgba(0,0,0,0),color);
        done+=fitting;y+=font->height;
    }
    return clicked || pressed;
}
void ps_ui_label_wrap(struct nk_context *ctx,const char *text) {
    if(ctx && ctx->current && text && nk_sdl_row_height(ctx,16)>16) {
        const struct nk_user_font *font=ctx->style.font;
        float space=nk_widget_width(ctx)-2*ctx->style.text.padding.x;
        int length=nk_strlen(text),done=0,lines=0,glyphs;
        float width;
        nk_rune separator=' ';
        while(done<length) {
            int fitting=nk_text_clamp(font,text+done,length-done,space,&glyphs,&width,&separator,1);
            if(fitting<=0)break;
            done+=fitting;lines++;
        }
        float height=lines*(font->height+2*ctx->style.text.padding.y)+4*ctx->style.text.padding.y+1;
        if(height>nk_widget_height(ctx))
            ctx->current->layout->row.height=height+ctx->style.window.spacing.y;
    }
    accessibility_widget(ctx,text,0,nk_widget_bounds(ctx),false,NULL,false);
    nk_label_wrap(ctx,text);
}

void nk_sdl_window_raise(struct nk_context *ctx, const char *name) {
    struct nk_window *window=nk_find_window(ctx,nk_murmur_hash(name,nk_strlen(name),NK_WINDOW_TITLE),name);
    if(!window || window==ctx->end) return;
    struct nk_window *active=ctx->active;
    struct nk_window *previous_top=ctx->end;
    nk_flags window_flags=window->flags,top_flags=previous_top->flags;
    nk_remove_window(ctx,window);
    nk_insert_window(ctx,window,NK_INSERT_BACK);
    /* Nuklear's insert also changes input flags. Stacking must preserve them,
     * including a held mouse drag in an editor, plot or slider. */
    window->flags=window_flags;
    previous_top->flags=top_flags;
    ctx->active=active;
}
