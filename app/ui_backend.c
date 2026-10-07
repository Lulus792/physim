/* Third-party implementations are isolated from the application's warning policy. */
#ifdef _MSC_VER
#pragma warning(push, 0)
#pragma warning(disable : 4116 4701 4706)
#endif
#define NK_IMPLEMENTATION
#include "ui.h"
#include <math.h>
#undef nk_label_wrap
#undef nk_button_label
#undef nk_label
#undef nk_label_colored
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
static bool accessibility_widget(struct nk_context *ctx,const char *text,int role,struct nk_rect bounds) {
    if(!ctx || !ctx->userdata.ptr || !ctx->current || !ctx->current->layout || !text)return false;
    const ps_ui_font_layout *layout=ctx->userdata.ptr;
    if(layout->magic!=PS_UI_LAYOUT_MAGIC || !layout->accessibility)return false;
    struct nk_rect clip=ctx->current->layout->clip;
    float left=fmaxf(bounds.x,clip.x),top=fmaxf(bounds.y,clip.y);
    float right=fminf(bounds.x+bounds.w,clip.x+clip.w),bottom=fminf(bounds.y+bounds.h,clip.y+clip.h);
    if(right<=left || bottom<=top)return false;
    float visible[]={left,top,right-left,bottom-top};
    bool enabled=!ctx->current->widgets_disabled && !(ctx->current->flags&NK_WINDOW_ROM);
    return layout->accessibility(layout->accessibility_user,ctx->current->name_string,text,role,visible,enabled);
}
void ps_ui_label(struct nk_context *ctx,const char *text,nk_flags alignment) {
    accessibility_widget(ctx,text,0,nk_widget_bounds(ctx));nk_label(ctx,text,alignment);
}
void ps_ui_label_colored(struct nk_context *ctx,const char *text,nk_flags alignment,struct nk_color color) {
    accessibility_widget(ctx,text,0,nk_widget_bounds(ctx));nk_label_colored(ctx,text,alignment,color);
}
nk_bool ps_ui_button_label(struct nk_context *ctx,const char *text) {
    if(!ctx || !ctx->current || !text || nk_sdl_row_height(ctx,16)<=16) {
        struct nk_rect bounds=nk_widget_bounds(ctx);
        nk_bool clicked=nk_button_label(ctx,text);
        bool pressed=accessibility_widget(ctx,text,1,bounds);
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
        bool pressed=accessibility_widget(ctx,text,1,bounds);
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
    nk_bool pressed=accessibility_widget(ctx,text,1,bounds);
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
    accessibility_widget(ctx,text,0,nk_widget_bounds(ctx));
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
