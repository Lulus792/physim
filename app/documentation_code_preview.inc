static float documentation_code_measure(void *user,const char *text,size_t bytes) {
    return documentation_text_width(user,text,bytes);
}
static struct nk_rect documentation_code_intersection(struct nk_rect a,struct nk_rect b) {
    float x=fmaxf(a.x,b.x),y=fmaxf(a.y,b.y);
    return nk_rect(x,y,fmaxf(0,fminf(a.x+a.w,b.x+b.w)-x),fmaxf(0,fminf(a.y+a.h,b.y+b.h)-y));
}
static void documentation_code_label(app *a,const char *source,size_t bytes,struct nk_rect r) {
    while(bytes) {
        char text[1024];size_t n=bytes<sizeof text-1?bytes:sizeof text-1;
        while(n<bytes && n && ((unsigned char)source[n]&0xc0)==0x80)--n;
        if(!n)break;memcpy(text,source,n);text[n]=0;
        ps_ui_accessibility_label(a->doc_ui,r,text);source+=n;bytes-=n;
    }
}
static void documentation_context_code_view(app *a,const char *source) {
    struct nk_context *ui=a->doc_ui;struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    const struct nk_user_font *font=documentation_type_font(a,DOC_TYPE_CONTEXT_CODE);
    float width=fmaxf(1,ui->current->layout->bounds.w-22);
    if(!a->doc_context_code_layout || a->doc_context_code_font!=font->userdata.ptr ||
       a->doc_context_code_width!=width || a->doc_context_code_font_height!=font->height || a->doc_context_code_density!=a->doc_raster_density) {
        ps_code_preview *candidate=NULL;
        if(!ps_code_preview_build(source,strlen(source),width,documentation_code_measure,(void *)font,&candidate)) {
            documentation_h_text_row(a,DOC_TYPE_CONTEXT,"Codevorschau konnte nicht aufgebaut werden. Der vollständige Quelltext kann weiterhin kopiert werden.",1.7f,0,14,14,true);return;
        }
        ps_code_preview_destroy(a->doc_context_code_layout);a->doc_context_code_layout=candidate;
        a->doc_context_code_width=width;a->doc_context_code_font_height=font->height;
        a->doc_context_code_font=font->userdata.ptr;a->doc_context_code_density=a->doc_raster_density;
        a->doc_context_code_drag.active=false;
    }
    const ps_code_preview *layout=a->doc_context_code_layout;
    float line=font->height/a->doc_type_em[DOC_TYPE_CONTEXT_CODE]*1.75f;
    a->doc_context_code_max=fmaxf(0,layout->maximum-width);
    a->doc_context_code_step=fmaxf(1,documentation_text_width(font," ",1)*4);
    unsigned track=a->doc_track==2;float *offset=&a->doc_context_code_x[track];
    *offset=fmaxf(0,fminf(a->doc_context_code_max,*offset));
    float rail=a->doc_context_code_max>0?7:0;
    ps_ui_row_exact_dynamic(ui,(float)layout->count*line+30+rail,1);
    struct nk_rect measured=nk_widget_bounds(ui),widget;nk_widget(&widget,ui);
    struct nk_rect r=documentation_h_snap(measured),clip=ui->current->layout->clip;
    a->doc_context_code_bounds=r;
    struct nk_rect visible=documentation_code_intersection(r,clip);
    bool input=!a->doc_actions_open && !(a->doc_find_open && nk_input_is_mouse_hovering_rect(&ui->input,a->doc_find_bounds));
    if(visible.w>0 && visible.h>0)nk_fill_rect(canvas,visible,0,UI_COLOR(edit));
    struct nk_rect borders[]={nk_rect(r.x,r.y,r.w,1),nk_rect(r.x,r.y+r.h-1,r.w,1),nk_rect(r.x,r.y,1,r.h),nk_rect(r.x+r.w-1,r.y,1,r.h)};
    for(unsigned i=0;i<4;i++){struct nk_rect edge=documentation_code_intersection(borders[i],clip);if(edge.w>0 && edge.h>0)nk_fill_rect(canvas,edge,0,UI_COLOR(divider));}
    a->doc_context_code_bar=rail?nk_rect(r.x+1,r.y+r.h-rail-1,r.w-2,rail):nk_rect(0,0,0,0);
    if(a->doc_context_code_max>0) {
        if(input && nk_input_is_mouse_hovering_rect(&ui->input,visible)) {
            if(ui->input.mouse.scroll_delta.x) {
                *offset=fmaxf(0,fminf(a->doc_context_code_max,*offset-ui->input.mouse.scroll_delta.x*a->doc_context_code_step));
                ui->input.mouse.scroll_delta.x=0;
            }
            if(nk_input_is_mouse_pressed(&ui->input,NK_BUTTON_LEFT)) {
                a->doc_keyboard=true;a->doc_focus=DOC_CONTEXT_CODE;a->doc_focus_reveal=false;
                nk_sdl_accessibility_keyboard_begin(ui);
            }
        }
        ps_ui_horizontal_scrollbar_at(ui,a->doc_context_code_bar,offset,a->doc_context_code_max,width,&a->doc_context_code_drag,input);
        if(a->doc_actions_open)ui->current->widgets_disabled++;
        ps_ui_range_at(ui,visible,"Beispielcode horizontal scrollen",offset,a->doc_context_code_max,a->doc_context_code_step);
        if(a->doc_actions_open)ui->current->widgets_disabled--;
        if(!a->doc_keyboard && nk_sdl_accessibility_is_focused(ui,NULL,"Beispielcode horizontal scrollen")) {
            a->doc_keyboard=true;a->doc_focus=DOC_CONTEXT_CODE;a->doc_focus_reveal=false;
        }
        if(a->doc_keyboard && a->doc_focus==DOC_CONTEXT_CODE) {
            if(a->doc_focus_reveal) {
                float delta=r.y<clip.y || r.y>=clip.y+clip.h?r.y-clip.y:0;
                if(delta)a->doc_context_scroll_y[1]=(nk_uint)fmaxf(0,(float)a->doc_context_scroll_y[1]+delta);
                bool pending=delta!=0;a->doc_focus_reveal=false;
                if(visible.w>0 && visible.h>0)documentation_focus_draw(a,DOC_CONTEXT_CODE,visible,false);
                a->doc_focus_reveal=pending;
            } else if(visible.w>0 && visible.h>0)documentation_focus_draw(a,DOC_CONTEXT_CODE,visible,false);
        }
    } else if(a->doc_focus==DOC_CONTEXT_CODE) {a->doc_focus=DOC_CONTEXT_COPY;a->doc_focus_reveal=true;}
    struct nk_rect code_clip=documentation_code_intersection(nk_rect(r.x+11,r.y+15,width,(float)layout->count*line),clip);
    if(code_clip.w<=0 || code_clip.h<=0)return;
    nk_push_scissor(canvas,code_clip);
    size_t first=(size_t)fmaxf(0,floorf((code_clip.y-r.y-15)/line));
    size_t last=(size_t)fminf((float)layout->count,ceilf((code_clip.y+code_clip.h-r.y-15)/line));
    for(size_t row=first;row<last;row++) {
        const ps_code_preview_line *part=&layout->lines[row];float x=0,y=r.y+15+(float)row*line;
        size_t visible_begin=SIZE_MAX,visible_end=0;float left=0,right=0;
        for(size_t at=part->begin;at<part->end;) {
            uint32_t c;size_t bytes=ps_code_preview_rune(source+at,part->end-at,&c);
            if(!bytes)break;
            float glyph=c=='\t' || ps_code_preview_invisible(c)?0:documentation_text_width(font,source+at,bytes);
            float next=ps_code_preview_advance(c,x,layout->tab,glyph),gx=r.x+11+x-*offset;
            if(gx>code_clip.x+code_clip.w)break;
            if(gx+next-x>=code_clip.x) {
                if(visible_begin==SIZE_MAX){visible_begin=at;left=fmaxf(code_clip.x,gx);}
                visible_end=at+bytes;right=fminf(code_clip.x+code_clip.w,gx+next-x);
                if(c!='\t' && !ps_code_preview_invisible(c))nk_draw_text(canvas,nk_rect(gx,y+(line-font->height)/2,glyph+1,font->height),source+at,(int)bytes,font,nk_rgba(0,0,0,0),UI_COLOR(code_text));
            }
            x=next;at+=bytes;
        }
        if(part->hyphen)nk_draw_text(canvas,nk_rect(r.x+11+x-*offset,y+(line-font->height)/2,documentation_text_width(font,"-",1)+1,font->height),"-",1,font,nk_rgba(0,0,0,0),UI_COLOR(code_text));
        if(visible_begin!=SIZE_MAX)documentation_code_label(a,source+visible_begin,visible_end-visible_begin,nk_rect(left,y,fmaxf(1,right-left),line));
    }
    nk_push_scissor(canvas,clip);
}
