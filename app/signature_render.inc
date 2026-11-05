/* One immutable row map supplies drawing and height/reveal calculations. The
 * source stays owned by the signature model until the controller discards it. */
static float signature_measure(void *user,const char *text,size_t bytes){
    app *a=user;ps_signature_help *h=&a->signature_help;ps_signature_display *d=h->display;
    uintptr_t at=(uintptr_t)text-(uintptr_t)d->text;bool active=false;
    if(h->active_parameter!=SIZE_MAX && at<d->text_bytes){ps_signature_parameter p=d->parameters[h->active_parameter];active=at>=p.begin && at<p.end;}
    const struct nk_user_font *font=active?&a->completion_bold_font:a->font_code;
    return font->width(font->userdata,font->height,text,(int)bytes);
}
static bool signature_font_equal(const struct nk_user_font *a,const struct nk_user_font *b){
    return a->height==b->height && a->width==b->width && a->userdata.ptr==b->userdata.ptr;
}
static ps_code_preview *signature_rows(app *a,float width){
    ps_code_preview *p=a->signature_layout;ps_signature_help *h=&a->signature_help;
    if(p && p->source==h->display->text && a->signature_layout_width==width &&
       a->signature_layout_parameter==h->active_parameter && signature_font_equal(&a->signature_layout_font,a->font_code) &&
       signature_font_equal(&a->signature_layout_bold,&a->completion_bold_font))return p;
    ps_code_preview *next=NULL;
    if(!ps_code_preview_build_signature(h->display->text,h->display->text_bytes,width,signature_measure,a,&next))return NULL;
    ps_code_preview_destroy(p);a->signature_layout=next;a->signature_layout_width=width;
    a->signature_layout_parameter=h->active_parameter;a->signature_layout_font=*a->font_code;a->signature_layout_bold=a->completion_bold_font;
    return next;
}
static float signature_code(app *a,float width,float x,float y,bool draw){
    ps_code_preview *p=signature_rows(a,width);if(!p)return 0;
    ps_signature_help *h=&a->signature_help;ps_signature_display *d=h->display;
    float pitch=a->font_code->height+6;
    if(draw){
        struct nk_command_buffer *canvas=nk_window_get_canvas(a->ui);
        ps_syntax_scan scan;ps_syntax_init(&scan,d->text,d->text_bytes,h->language);scan.resolve=signature_role;scan.user=d;
        ps_syntax_token token;bool have=ps_syntax_next(&scan,&token);
        size_t active_begin=SIZE_MAX,active_end=0;
        if(h->active_parameter!=SIZE_MAX){ps_signature_parameter active=d->parameters[h->active_parameter];active_begin=active.begin;active_end=active.end;}
        for(size_t row=0;row<p->count;row++){
            float pen=0,sy=y+(float)row*pitch;ps_code_preview_line line=p->lines[row];
            for(size_t at=line.begin;at<line.end;){
                uint32_t rune;size_t n=ps_code_preview_rune(d->text+at,line.end-at,&rune);if(!n)break;
                while(have && token.end<=at)have=ps_syntax_next(&scan,&token);
                bool active=at>=active_begin && at<active_end,invisible=ps_code_preview_invisible(rune) || rune=='\t';
                const struct nk_user_font *font=active?&a->completion_bold_font:a->font_code;
                float glyph=invisible?0:signature_measure(a,d->text+at,n),next=ps_code_preview_advance(rune,pen,p->tab,glyph),sx=x+pen;
                if(active){nk_fill_rect(canvas,nk_rect(sx,sy,next-pen,pitch-1),0,UI_COLOR(text_selection));nk_stroke_line(canvas,sx,sy+pitch-2,x+next,sy+pitch-2,1,UI_COLOR(selection_accent));}
                if(!invisible)nk_draw_text(canvas,nk_rect(sx,sy,glyph+1,font->height),d->text+at,(int)n,font,nk_rgba(0,0,0,0),syntax_color(have?token.role:PS_SYNTAX_TEXT));
                pen=next;at+=n;
            }
        }
    }
    return (float)p->count*pitch;
}
static bool signature_excess(const app *a){
    const ps_signature_help *h=&a->signature_help;const ps_signature_display *d=h->display;
    return d && !d->unspecified && d->variadic_end==d->variadic_begin && h->call.argument>=d->count && (d->count || !h->call.at_start);
}
static void signature_status_draw(app *a,struct nk_rect r){
    const struct nk_user_font *font=&a->workbench_fonts[9];size_t at=0,bytes=strlen(a->signature_status),line=0;
    do{size_t length,used=documentation_line(font,a->signature_status+at,bytes-at,r.w,&length);
        nk_draw_text(nk_window_get_canvas(a->ui),nk_rect(r.x,r.y+(float)line*(font->height+6),r.w,font->height),
            a->signature_status+at,(int)length,font,nk_rgba(0,0,0,0),signature_excess(a)?UI_COLOR(error):UI_COLOR(muted));
        at+=used;line++;
    }while(at<bytes);
    ps_ui_accessibility_label(a->ui,r,a->signature_status);
}
static void signature_reveal_active(app *a,float body_height,float pitch){
    if(!a->signature_reveal)return;
    float top=0,bottom=pitch;ps_signature_help *h=&a->signature_help;ps_code_preview *p=a->signature_layout;
    if(h->active_parameter!=SIZE_MAX){
        ps_signature_parameter active=h->display->parameters[h->active_parameter];bool found=false;
        for(size_t i=0;i<p->count;i++)if(p->lines[i].begin<active.end && p->lines[i].end>active.begin){
            if(!found){top=(float)i*pitch;}bottom=(float)(i+1)*pitch;found=true;
        }
    }
    float next=(float)a->signature_scroll_y;
    if(bottom-top>body_height || top<next)next=top;else if(bottom>next+body_height)next=bottom-body_height;
    a->signature_scroll_y=(nk_uint)fmaxf(0,fminf(a->signature_scroll_max,ceilf(next)));a->signature_reveal=false;
}
static void signature_view(app *a,int width,int height){
    struct nk_context *ui=a->ui;struct nk_text_edit *edit=completion_target(a);
    a->signature_hud_bounds=a->signature_field_bounds=a->signature_status_bounds=a->signature_body_bounds=nk_rect(0,0,0,0);
    if(!a->signature_visible || !signature_current(a,edit) || !a->dock_visible[PS_DOCK_WORKSPACE] ||
       SDL_GetKeyboardFocus()!=a->window || a->editor_caret.y<a->source_editor_bounds.y ||
       a->editor_caret.y+a->editor_caret.h>a->source_editor_bounds.y+a->source_editor_bounds.h){nk_window_show(ui,"call-parameter-help",NK_HIDDEN);return;}
    a->completion_bold_font=*a->workbench_marker_source;a->completion_bold_font.height=a->font_code->height;
    const struct nk_user_font *small=&a->workbench_fonts[9];ps_signature_display *d=a->signature_help.display;
    float text_width=0,line_width=0,pitch=a->font_code->height+6;
    for(size_t at=0;at<d->text_bytes;){
        uint32_t rune;size_t n=ps_code_preview_rune(d->text+at,d->text_bytes-at,&rune);if(!n)return;
        if(rune=='\n'){text_width=fmaxf(text_width,line_width);line_width=0;}
        else if(rune!='\r')line_width=ps_code_preview_advance(rune,line_width,signature_measure(a," ",1)*8,ps_code_preview_invisible(rune)?0:signature_measure(a,d->text+at,n));
        at+=n;
    }
    text_width=fmaxf(text_width,line_width);
    float status_width=small->width(small->userdata,small->height,a->signature_status,(int)strlen(a->signature_status));
    float limit=fminf(646,width-16.f),w=fminf(ceilf(fmaxf(text_width,status_width))+34,fminf(limit,a->source_editor_bounds.w-16));
    if(w<48){nk_window_show(ui,"call-parameter-help",NK_HIDDEN);return;}
    ps_code_preview *rows=signature_rows(a,w-34);
    if(rows && rows->maximum>w-34 && w<limit){w=fminf(limit,ceilf(rows->maximum)+34);rows=signature_rows(a,w-34);}
    if(!rows){nk_window_show(ui,"call-parameter-help",NK_HIDDEN);return;}
    float inner=w-34,code_h=(float)rows->count*pitch,status_h=documentation_height(small,a->signature_status,strlen(a->signature_status),inner),natural=code_h+status_h+26;
    float list_min=a->completion_active?fmaxf(fmaxf(28,a->preferences.ui_size+8.f),a->font_code->height+8)+40:0;
    float side=fmaxf(a->editor_caret.y-8,height-8.f-a->editor_caret.y-a->editor_caret.h);
    float minimum=pitch+fminf(status_h,3*(small->height+6))+26;
    float budget=fmaxf(48,fminf(height-16.f,side-list_min));
    float h=fminf(natural,fminf(budget,fmaxf(minimum,a->source_editor_bounds.h*.45f)));
    bool fixed_footer=pitch+status_h+26<=h;
    float body_h=fmaxf(1,fixed_footer?h-status_h-26:h-20),extent=fixed_footer?code_h:code_h+6+status_h;
    a->signature_scroll_max=ceilf(fmaxf(0,extent-body_h));
    a->signature_scroll_y=(nk_uint)fminf((float)a->signature_scroll_y,a->signature_scroll_max);a->signature_scroll_x=0;
    signature_reveal_active(a,body_h,pitch);
    float top=fmaxf(8,fminf(a->source_editor_bounds.y,height-h-8.f)),bottom=fminf(height-8.f,fmaxf(top+h,a->source_editor_bounds.y+a->source_editor_bounds.h));
    float x=fmaxf(8,fminf(a->editor_caret.x,width-w-8.f)),y=a->editor_caret.y+a->editor_caret.h+4;
    if(y+h+list_min>bottom && a->editor_caret.y-top>bottom-y)y=fmaxf(top,a->editor_caret.y-h-4);
    y=fmaxf(top,fminf(y,bottom-h));a->signature_hud_bounds=nk_rect(x,y,w,h);
    struct nk_window *previous=ui->active;
    nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_color(UI_COLOR(header)));
    nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(12,9));nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(0,6));
    nk_style_push_vec2(ui,&ui->style.window.scrollbar_size,nk_vec2(6,0));nk_style_push_vec2(ui,&ui->style.window.group_padding,nk_vec2(0,0));
    nk_style_push_float(ui,&ui->style.window.rounding,5);nk_style_push_float(ui,&ui->style.window.border,1);
    nk_window_show(ui,"call-parameter-help",NK_SHOWN);
    if(nk_begin(ui,"call-parameter-help",a->signature_hud_bounds,NK_WINDOW_BORDER|NK_WINDOW_BACKGROUND|NK_WINDOW_NO_SCROLLBAR)){
        ui->active=previous;
        if(nk_input_is_mouse_hovering_rect(&ui->input,a->signature_hud_bounds) && ui->input.mouse.scroll_delta.y!=0){
            float next=(float)a->signature_scroll_y-ui->input.mouse.scroll_delta.y*3*pitch;
            a->signature_scroll_y=(nk_uint)fmaxf(0,fminf(a->signature_scroll_max,next));a->signature_reveal=false;ui->input.mouse.scroll_delta.y=0;
        }
        struct nk_rect r;
        if(a->signature_scroll_max>0){
            ps_ui_row_exact_dynamic(ui,body_h,1);a->signature_body_bounds=nk_widget_bounds(ui);
            ui->active=ui->current;
            /* This inert background card retains editor focus. Groups inherit
             * its ROM flag, though: allow pointer input ONLY while constructing
             * the signature scrollbar, then restore the card's prior state. */
            nk_flags readonly=ui->current->flags&NK_WINDOW_ROM;
            ui->current->flags&=~(nk_flags)NK_WINDOW_ROM;
            if(nk_group_scrolled_offset_begin(ui,&a->signature_scroll_x,&a->signature_scroll_y,"call-signature-body",0)){
                ps_ui_row_exact_dynamic(ui,code_h,1);nk_widget(&r,ui);r.w=inner;a->signature_field_bounds=r;
                signature_code(a,inner,r.x,r.y,true);
                if(!fixed_footer){ps_ui_row_exact_dynamic(ui,status_h,1);nk_widget(&r,ui);r.w=inner;a->signature_status_bounds=r;signature_status_draw(a,r);}
                nk_group_scrolled_end(ui);
            }
            ui->current->flags|=readonly;
            ui->active=previous;
        }else{
            ps_ui_row_exact_dynamic(ui,code_h,1);nk_widget(&r,ui);r.w=inner;a->signature_body_bounds=a->signature_field_bounds=r;
            signature_code(a,inner,r.x,r.y,true);
        }
        ps_ui_accessibility_label(ui,a->signature_body_bounds,d->text);
        if(fixed_footer){ps_ui_row_exact_dynamic(ui,status_h,1);nk_widget(&r,ui);r.w=inner;a->signature_status_bounds=r;signature_status_draw(a,r);}
    }
    nk_end(ui);ui->active=previous;
    nk_style_pop_float(ui);nk_style_pop_float(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);nk_style_pop_style_item(ui);
    nk_sdl_window_raise(ui,"call-parameter-help");
}
