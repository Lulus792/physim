/* ADR023: an independent, inert call HUD. It never owns editor input, candidate
 * selection or insertion. Current source identity is checked before publication,
 * including the failure-preserving locals cache. */
static void signature_suppression_clear(app *a){
    free(a->signature_suppressed_callee);a->signature_suppressed_callee=NULL;
    a->signature_suppressed_bytes=0;a->signature_suppressed_owner=NULL;
}
static void signature_clear(app *a){
    ps_signature_help_destroy(&a->signature_help);
    free(a->signature_status);a->signature_status=NULL;
    a->signature_visible=false;signature_suppression_clear(a);
    a->signature_hud_bounds=a->signature_field_bounds=a->signature_status_bounds=nk_rect(0,0,0,0);
}
static bool signature_eligible(app *a,struct nk_text_edit *edit){
    return edit && completion_target(a)==edit && document_language(a,edit)>=0 && edit->active &&
        edit->select_start==edit->select_end && !a->completion_composing &&
        !(a->show_search && (a->search_keyboard || a->search_focus));
}
static bool signature_current(app *a,struct nk_text_edit *edit){
    ps_signature_help *h=&a->signature_help;
    const char *text=edit?nk_str_get_const(&edit->string):NULL;
    size_t bytes=edit?(size_t)nk_str_len_char(&edit->string):0;
    return signature_eligible(a,edit) && h->owner==edit && h->language==(document_language(a,edit)==1) &&
        h->cursor==editor_search_byte(edit,edit->cursor) && h->bytes==bytes && h->source &&
        !memcmp(h->source,text?text:"",bytes);
}
static void signature_update(app *a,struct nk_text_edit *edit,bool manual){
    a->signature_visible=false;
    if(!signature_eligible(a,edit)){
        if(!edit || completion_target(a)!=a->signature_help.owner || a->completion_composing ||
           (edit && edit->select_start!=edit->select_end))signature_suppression_clear(a);
        return;
    }
    const char *text=nk_str_get_const(&edit->string);if(!text)text="";
    size_t bytes=(size_t)nk_str_len_char(&edit->string),cursor=editor_search_byte(edit,edit->cursor);
    bool same=signature_current(a,edit);
    if(!same){
        free(a->signature_status);a->signature_status=NULL;
        if(ps_completion_locals_update(&a->completion_locals,edit,text,bytes,cursor,document_language(a,edit)==1)!=PS_OK)return;
        ps_completion_locals *l=&a->completion_locals;
        if(l->owner!=edit || l->bytes!=bytes || l->cursor!=cursor || l->language!=(document_language(a,edit)==1) ||
           !l->source || memcmp(l->source,text,bytes) || ps_signature_help_update(&a->signature_help,l)!=PS_OK ||
           !signature_current(a,edit))return;
    }
    ps_signature_help *h=&a->signature_help;
    if(h->state!=PS_SIGNATURE_READY || !h->display || !h->call.active){signature_suppression_clear(a);return;}
    size_t name_bytes=h->call.callee_end-h->call.callee_start;
    if(manual || a->signature_suppressed_owner!=edit || a->signature_suppressed_open!=h->call.open ||
       (a->signature_suppressed_callee && (a->signature_suppressed_bytes!=name_bytes ||
        memcmp(a->signature_suppressed_callee,h->source+h->call.callee_start,name_bytes))))signature_suppression_clear(a);
    if(a->signature_suppressed_owner==edit)return;
    if(!same || !a->signature_status){
        ps_signature_display *d=h->display;char prefix[128];size_t begin=0,length=0;
        if(d->unspecified)snprintf(prefix,sizeof prefix,"Argument %zu · Parameter nicht angegeben",h->call.argument+1);
        else if(h->active_parameter!=SIZE_MAX){
            ps_signature_parameter p=d->parameters[h->active_parameter];begin=p.begin;length=p.end-p.begin;
            snprintf(prefix,sizeof prefix,"Parameter %zu von %zu · ",h->active_parameter+1,d->count);
        }else if(d->variadic_end>d->variadic_begin && h->call.argument>=d->count)
            snprintf(prefix,sizeof prefix,"Argument %zu · variabler Parameterbereich",h->call.argument+1);
        else snprintf(prefix,sizeof prefix,"Argument %zu · erwartet %zu Parameter",h->call.argument+1,d->count);
        size_t n=strlen(prefix);char *status=malloc(n+length+1);if(!status)return;
        memcpy(status,prefix,n);memcpy(status+n,d->text+begin,length);status[n+length]=0;
        free(a->signature_status);a->signature_status=status;
    }
    a->signature_visible=true;
}
static bool signature_keyboard_key(app *a,const SDL_KeyboardEvent *key){
    struct nk_text_edit *edit=completion_target(a);
    if(key->key==SDLK_SPACE && (key->mod&(SDL_KMOD_CTRL|SDL_KMOD_SHIFT))==(SDL_KMOD_CTRL|SDL_KMOD_SHIFT) &&
       !(key->mod&(SDL_KMOD_ALT|SDL_KMOD_GUI)) && signature_eligible(a,edit)){
        if(!key->repeat)signature_update(a,edit,true);return true;
    }
    if(key->key!=SDLK_ESCAPE || a->completion_active || a->completion_pending_edit ||
       !a->signature_visible || !signature_current(a,edit))return false;
    ps_signature_help *h=&a->signature_help;
    signature_suppression_clear(a);
    a->signature_suppressed_owner=edit;a->signature_suppressed_open=h->call.open;
    a->signature_suppressed_bytes=h->call.callee_end-h->call.callee_start;
    a->signature_suppressed_callee=malloc(a->signature_suppressed_bytes+1);
    if(a->signature_suppressed_callee){
        memcpy(a->signature_suppressed_callee,h->source+h->call.callee_start,a->signature_suppressed_bytes);
        a->signature_suppressed_callee[a->signature_suppressed_bytes]=0;
    }
    /* If even the tiny suppression key cannot be allocated, keep this call
     * hidden until manual reopening or leaving it; never show stale data. */
    a->signature_visible=false;return true;
}
static ps_syntax_role signature_role(void *user,size_t offset,size_t bytes){
    ps_signature_display *d=user;
    if(offset==d->name_begin && bytes==d->name_end-d->name_begin)return PS_SYNTAX_FUNCTION;
    for(size_t i=0;i<d->count;i++)if(offset==d->parameters[i].name_begin &&
        bytes==d->parameters[i].name_end-d->parameters[i].name_begin)return PS_SYNTAX_VARIABLE;
    return PS_SYNTAX_TEXT;
}
/* Measure and paint with the same real regular/semibold glyph metrics. Byte
 * spans remain exact through UTF-8 wrapping; no five-parameter truncation. */
static float signature_code(app *a,float width,float x,float y,bool draw){
    ps_signature_help *h=&a->signature_help;ps_signature_display *d=h->display;
    const struct nk_user_font *regular=a->font_code,*bold=&a->completion_bold_font;
    struct nk_command_buffer *canvas=draw?nk_window_get_canvas(a->ui):NULL;
    float start=x,pitch=regular->height+6,top=y,pen=0;size_t line=0;
    size_t active_begin=SIZE_MAX,active_end=0;
    if(h->active_parameter!=SIZE_MAX){ps_signature_parameter p=d->parameters[h->active_parameter];active_begin=p.begin;active_end=p.end;}
    ps_syntax_scan scan;ps_syntax_init(&scan,d->text,d->text_bytes,h->language);scan.resolve=signature_role;scan.user=d;
    ps_syntax_token token;
    while(ps_syntax_next(&scan,&token))for(size_t at=token.begin;at<token.end;){
        nk_rune rune;int n=nk_utf_decode(d->text+at,&rune,(int)(token.end-at));if(n<=0)n=1;
        if(rune=='\r' || rune=='\n'){if(rune=='\n'){pen=0;line++;}at+=(size_t)n;continue;}
        bool active=at>=active_begin && at<active_end;const struct nk_user_font *font=active?bold:regular;
        float glyph=font->width(font->userdata,font->height,d->text+at,n);
        /* Accumulate the SAME relative pen for measuring and painting. Adding
         * advances to an absolute screen X can round the last glyph onto an
         * unmeasured second line (observed at22px:28px versus56px). */
        if(pen>0 && pen+glyph>width){pen=0;line++;}
        x=start+pen;y=top+(float)line*pitch;
        if(draw){
            if(active){nk_fill_rect(canvas,nk_rect(x,y,glyph,pitch-1),0,UI_COLOR(text_selection));
                nk_stroke_line(canvas,x,y+pitch-2,x+glyph,y+pitch-2,1,UI_COLOR(selection_accent));}
            nk_draw_text(canvas,nk_rect(x,y,glyph+1,font->height),d->text+at,n,font,nk_rgba(0,0,0,0),syntax_color(token.role));
        }
        pen+=glyph;at+=(size_t)n;
    }
    return (float)(line+1)*pitch;
}
static void signature_view(app *a,int width,int height){
    struct nk_context *ui=a->ui;struct nk_text_edit *edit=completion_target(a);
    a->signature_hud_bounds=a->signature_field_bounds=a->signature_status_bounds=nk_rect(0,0,0,0);
    if(!a->signature_visible || !signature_current(a,edit) || !a->dock_visible[PS_DOCK_WORKSPACE] ||
       SDL_GetKeyboardFocus()!=a->window || a->editor_caret.y<a->source_editor_bounds.y ||
       a->editor_caret.y+a->editor_caret.h>a->source_editor_bounds.y+a->source_editor_bounds.h){
        nk_window_show(ui,"call-parameter-help",NK_HIDDEN);return;
    }
    a->completion_bold_font=*a->workbench_marker_source;a->completion_bold_font.height=a->font_code->height;
    const struct nk_user_font *small=&a->workbench_fonts[9];
    float text_width=0,line_width=0;ps_signature_display *d=a->signature_help.display;
    for(size_t at=0;at<d->text_bytes;){
        nk_rune rune;int n=nk_utf_decode(d->text+at,&rune,(int)(d->text_bytes-at));if(n<=0)n=1;
        const struct nk_user_font *font=a->font_code;
        if(a->signature_help.active_parameter!=SIZE_MAX){ps_signature_parameter p=d->parameters[a->signature_help.active_parameter];if(at>=p.begin && at<p.end)font=&a->completion_bold_font;}
        if(rune=='\n'){text_width=fmaxf(text_width,line_width);line_width=0;}
        else if(rune!='\r')line_width+=font->width(font->userdata,font->height,d->text+at,n);
        at+=(size_t)n;
    }
    text_width=fmaxf(text_width,line_width);
    float status_width=small->width(small->userdata,small->height,a->signature_status,(int)strlen(a->signature_status));
    /* Padding + border + the real reserved six-pixel scrollbar gutter. The
     * canvas clip is narrower than a manually enlarged widget rectangle. */
    float w=fminf(ceilf(fmaxf(text_width,status_width))+34,fminf(646,fminf(width-16.f,a->source_editor_bounds.w-16)));
    if(w<48){nk_window_show(ui,"call-parameter-help",NK_HIDDEN);return;}
    float inner=w-34,code_h=signature_code(a,inner,0,0,false);
    float status_h=documentation_height(small,a->signature_status,strlen(a->signature_status),inner);
    float natural=code_h+status_h+26;
    float h=fminf(natural,fmaxf(48,fminf(height-16.f,a->source_editor_bounds.h*.45f)));
    float top=fmaxf(8,a->source_editor_bounds.y),bottom=fminf(height-8.f,a->source_editor_bounds.y+a->source_editor_bounds.h);
    float x=fmaxf(8,fminf(a->editor_caret.x,width-w-8.f)),y=a->editor_caret.y+a->editor_caret.h+4;
    /* Preserve the normal below-caret stack. With insufficient space move the
     * HUD above; the list separately chooses a free side of that rectangle. */
    float list_min=a->completion_active?fmaxf(28,a->font_code->height+8)+40:0;
    if(y+h+list_min>bottom && a->editor_caret.y-top>bottom-y)y=fmaxf(top,a->editor_caret.y-h-4);
    y=fmaxf(top,fminf(y,bottom-h));a->signature_hud_bounds=nk_rect(x,y,w,h);
    struct nk_window *previous=ui->active;
    nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_color(UI_COLOR(header)));
    nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(12,9));
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(0,6));
    nk_style_push_vec2(ui,&ui->style.window.scrollbar_size,nk_vec2(6,0));
    nk_style_push_float(ui,&ui->style.window.rounding,5);nk_style_push_float(ui,&ui->style.window.border,1);
    nk_window_show(ui,"call-parameter-help",NK_SHOWN);
    if(nk_input_is_mouse_hovering_rect(&ui->input,a->signature_hud_bounds))ui->active=nk_window_find(ui,"call-parameter-help");
    if(nk_begin(ui,"call-parameter-help",a->signature_hud_bounds,NK_WINDOW_BORDER|NK_WINDOW_BACKGROUND)){
        ui->active=previous;
        ps_ui_row_exact_dynamic(ui,code_h,1);struct nk_rect r;nk_widget(&r,ui);r.w=inner;a->signature_field_bounds=r;
        struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
        signature_code(a,inner,r.x,r.y,true);
        ps_ui_accessibility_label(ui,r,a->signature_help.display->text);
        ps_ui_row_exact_dynamic(ui,status_h,1);nk_widget(&r,ui);r.w=inner;a->signature_status_bounds=r;
        size_t at=0,bytes=strlen(a->signature_status);float sy=r.y;
        do{size_t length,used=documentation_line(small,a->signature_status+at,bytes-at,r.w,&length);
            nk_draw_text(canvas,nk_rect(r.x,sy,r.w,small->height),a->signature_status+at,(int)length,small,nk_rgba(0,0,0,0),UI_COLOR(muted));
            at+=used;sy+=small->height+6;
        }while(at<bytes);
        ps_ui_accessibility_label(ui,r,a->signature_status);
    }
    nk_end(ui);ui->active=previous;
    nk_style_pop_float(ui);nk_style_pop_float(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);nk_style_pop_style_item(ui);
    nk_sdl_window_raise(ui,"call-parameter-help");
}
