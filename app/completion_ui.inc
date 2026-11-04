static void completion_pending_clear(app *a) {
    free(a->completion_pending_text);a->completion_pending_text=NULL;
    a->completion_pending_edit=NULL;a->completion_pending_bytes=0;a->completion_due=0;
}
static void completion_hide(app *a) {
    free(a->completion_indices);a->completion_indices=NULL;a->completion_edit=a->completion_focus_edit=NULL;
    a->completion_active=a->completion_accept_pending=a->completion_explicit=a->completion_manual=false;a->completion_count=a->completion_selected=0;
    a->completion_first=0;a->completion_visible_rows=0;a->completion_scroll_drag=false;
    a->completion_info_bounds=a->completion_scroll_bounds=nk_rect(0,0,0,0);
}
static void completion_close(app *a) {
    completion_pending_clear(a);completion_hide(a);a->completion_suppressed_edit=NULL;
}
static struct nk_text_edit *completion_target(app *a) {
    if(a->document_pending || a->recovery || a->toolbar_keyboard || a->toolbar_menu)return NULL;
    if(a->loaded && a->tab==0)return a->analysis_only?&a->analysis:&a->experiment;
    if(a->loaded && a->tab==2 && a->analysis_tab==1)return &a->analysis;
    workspace_document *doc=active_document(a);
    return a->tab==8 && doc && !doc->recovery?&doc->edit:NULL;
}
static bool completion_refresh(app *a) {
    struct nk_text_edit *edit=a->completion_edit;
    if(!edit || a->completion_composing || edit->select_start!=edit->select_end){completion_close(a);return false;}
    const char *text=nk_str_get_const(&edit->string);size_t bytes=(size_t)nk_str_len_char(&edit->string),cursor=bytes;
    if(!text)text="";
    if(edit->cursor<nk_str_len(&edit->string)) {
        nk_rune rune;int length;const char *at=nk_str_at_const(&edit->string,edit->cursor,&rune,&length);
        if(!at){completion_close(a);return false;}cursor=(size_t)(at-text);
    }
    size_t previous=a->completion_count?a->completion_indices[a->completion_selected]:SIZE_MAX;
    if(ps_completion_prepare(text,bytes,cursor,document_language(a,edit)==1,&a->completion_query)!=PS_OK) {
        completion_close(a);return false;
    }
    size_t count=a->completion_count;
    a->completion_count=ps_completion_matches(&a->completion_query,a->completion_indices,ps_completion_count());
    if(!a->completion_count){completion_close(a);return false;}
    a->completion_selected=0;
    for(size_t i=0;i<a->completion_count;i++)if(a->completion_indices[i]==previous){a->completion_selected=i;break;}
    if(count!=a->completion_count)a->completion_reveal=true;
    return true;
}
static void completion_open_kind(app *a,struct nk_text_edit *edit,bool manual) {
    completion_close(a);
    if(a->completion_composing || document_language(a,edit)<0)return;
    size_t count=ps_completion_count();
    if(count>SIZE_MAX/sizeof *a->completion_indices)return;
    a->completion_indices=malloc(count*sizeof *a->completion_indices);
    if(!a->completion_indices){status(a,"Nicht genügend Speicher für API-Vorschläge.");return;}
    a->completion_edit=edit;a->completion_active=true;a->completion_explicit=a->completion_manual=manual;a->completion_cursor=edit->cursor;
    a->completion_reveal=a->completion_info_visible=true;
    if(!completion_refresh(a) && manual)status(a,"Keine API-Vorschläge an dieser Textposition.");
}
static void completion_open(app *a,struct nk_text_edit *edit) {completion_open_kind(a,edit,true);}
static void completion_edit_input(app *a) {
    /* A queued acceptance belongs to the source that was shown. Later input
     * invalidates it. Automatic choices likewise belong to their prefix;
     * manual Ctrl+Space retains its established first-result behavior. */
    if(a->completion_accept_pending)completion_close(a);
    else if(a->completion_active && !a->completion_manual)a->completion_explicit=false;
}
static bool completion_query_for(app *a,struct nk_text_edit *edit,ps_completion_query *query) {
    if(!edit || a->completion_composing || !edit->active || edit->select_start!=edit->select_end ||
       completion_target(a)!=edit || document_language(a,edit)<0)return false;
    const char *text=nk_str_get_const(&edit->string);if(!text)text="";
    nk_rune rune;int n;const char *at=nk_str_at_const(&edit->string,edit->cursor,&rune,&n);
    size_t bytes=(size_t)nk_str_len_char(&edit->string),cursor=at?(size_t)(at-text):bytes;
    return ps_completion_prepare(text,bytes,cursor,document_language(a,edit)==1,query)==PS_OK;
}
static void completion_dismiss(app *a,struct nk_text_edit *edit) {
    ps_completion_query query;
    if(completion_query_for(a,edit,&query)){a->completion_suppressed_edit=edit;a->completion_suppressed_start=query.start;a->completion_suppressed_cursor=edit->cursor;}
    completion_pending_clear(a);completion_hide(a);a->completion_focus_edit=edit;
}
static void completion_auto_update(app *a,struct nk_text_edit *edit,bool changed) {
    if(!a->preferences.completion_auto){
        completion_pending_clear(a);
        if(a->completion_active && !a->completion_manual)completion_hide(a);
        return;
    }
    if(a->completion_active){a->completion_cursor=edit->cursor;return;}
    /* Idle frames do not rescan the source. Suppression only needs a new
     * query after text, caret or owner changes; a pending request still checks
     * its complete source snapshot even without a normal edit notification. */
    if(!changed && !a->completion_pending_edit && (!a->completion_suppressed_edit ||
       (a->completion_suppressed_edit==edit && a->completion_suppressed_cursor==edit->cursor)))return;
    ps_completion_query query;
    if(!completion_query_for(a,edit,&query)){
        completion_pending_clear(a);a->completion_suppressed_edit=NULL;return;
    }
    if(a->completion_suppressed_edit && (a->completion_suppressed_edit!=edit ||
       a->completion_suppressed_start!=query.start || (!query.member && query.end==query.start)))a->completion_suppressed_edit=NULL;
    if(a->completion_suppressed_edit==edit){a->completion_suppressed_cursor=edit->cursor;return;}
    size_t bytes=(size_t)nk_str_len_char(&edit->string);const char *text=nk_str_get_const(&edit->string);if(!text)text="";
    if(changed && a->completion_typed){
        completion_pending_clear(a);
        if(!query.member && strlen(query.prefix)<2)return;
        char *copy=malloc(bytes+1);if(!copy)return;
        memcpy(copy,text,bytes);copy[bytes]=0;
        a->completion_pending_text=copy;a->completion_pending_edit=edit;
        a->completion_pending_cursor=edit->cursor;a->completion_pending_bytes=bytes;a->completion_due=ps_clock()+.140;
        return;
    }
    if(!a->completion_pending_edit)return;
    /* The delayed request owns exact source bytes, cursor and editor. An
     * equal-length replacement outside the prefix invalidates it as well. */
    if(a->completion_pending_edit!=edit || a->completion_pending_cursor!=edit->cursor ||
       a->completion_pending_bytes!=bytes || memcmp(a->completion_pending_text,text,bytes)){
        completion_pending_clear(a);return;
    }
    if(ps_clock()>=a->completion_due)completion_open_kind(a,edit,false);
}
static void completion_commit(app *a) {
    struct nk_text_edit *edit=a->completion_edit;
    const ps_completion_symbol *symbol=ps_completion_at(a->completion_indices[a->completion_selected]);
    const char *insert=ps_completion_insertion(&a->completion_query,symbol),*text=nk_str_get_const(&edit->string);
    if(!text)text="";
    if(strlen(insert)==a->completion_query.end-a->completion_query.start &&
       !memcmp(text+a->completion_query.start,insert,strlen(insert))) {
        completion_close(a);a->completion_focus_edit=edit;return;
    }
    int start=edit->select_start,end=edit->select_end,cursor=edit->cursor;
    edit->select_start=nk_utf_len(text,(int)a->completion_query.start);
    edit->select_end=nk_utf_len(text,(int)a->completion_query.end);edit->cursor=edit->select_end;
    if(!nk_textedit_paste(edit,insert,(int)strlen(insert))) {
        edit->select_start=start;edit->select_end=end;edit->cursor=cursor;
        status(a,"Vorschlag konnte nicht eingefügt werden. Text bleibt erhalten.");
    }
    completion_close(a);
    a->completion_focus_edit=edit;
}
static bool completion_keyboard_key(app *a,const SDL_KeyboardEvent *key) {
    struct nk_text_edit *edit=completion_target(a);
    if(a->completion_active && edit!=a->completion_edit)completion_close(a);
    if(edit && document_language(a,edit)>=0 && edit->active && key->key==SDLK_SPACE && (key->mod&SDL_KMOD_CTRL) &&
       !(key->mod&(SDL_KMOD_ALT|SDL_KMOD_GUI))) {
        if(!key->repeat)completion_open(a,edit);return true;
    }
    if(key->key==SDLK_ESCAPE && (a->completion_active || a->completion_pending_edit)){completion_dismiss(a,edit);return true;}
    bool control_move=a->completion_active && (key->mod&SDL_KMOD_CTRL) && !(key->mod&(SDL_KMOD_ALT|SDL_KMOD_GUI|SDL_KMOD_SHIFT)) && (key->key==SDLK_N || key->key==SDLK_P);
    if(a->completion_active && !control_move && ((key->mod&(SDL_KMOD_CTRL|SDL_KMOD_GUI|SDL_KMOD_ALT)) ||
       (key->key>=SDLK_F1 && key->key<=SDLK_F12))){completion_close(a);return false;}
    if(!a->completion_active || (!control_move && key->mod&(SDL_KMOD_CTRL|SDL_KMOD_GUI|SDL_KMOD_ALT)))return false;
    if(control_move || key->key==SDLK_UP || key->key==SDLK_DOWN) {
        bool up=key->key==SDLK_UP || key->key==SDLK_P;
        a->completion_selected=a->completion_explicit?(a->completion_selected+(up?a->completion_count-1:1))%a->completion_count:up?a->completion_count-1:0;
        a->completion_explicit=true;
        a->completion_reveal=true;
        return true;
    }
    if(key->key==SDLK_RETURN || key->key==SDLK_KP_ENTER || key->key==SDLK_TAB) {
        if(!a->completion_explicit){completion_close(a);return false;}
        if(!key->repeat)a->completion_accept_pending=true;return true;
    }
    if(key->key==SDLK_LEFT || key->key==SDLK_RIGHT || key->key==SDLK_HOME || key->key==SDLK_END)completion_close(a);
    return false;
}
static void completion_pointer_event(app *a,const SDL_MouseButtonEvent *event) {
    if(!a->completion_active){if(a->completion_focus_edit)completion_close(a);return;}
    const struct nk_rect regions[]={a->source_editor_bounds,a->completion_list_bounds,a->completion_info_bounds,a->completion_button_bounds,a->api_reference_button_bounds};
    for(unsigned i=0;i<5;i++)if(event->x>=regions[i].x && event->x<regions[i].x+regions[i].w &&
        event->y>=regions[i].y && event->y<regions[i].y+regions[i].h)return;
    completion_close(a);
}
/* A whole-row viewport over the complete catalog result. Wheel movement is
 * independent of acceptance selection; only explicit arrows reveal a choice. */
static void completion_label(struct nk_context *ui,struct nk_rect r,const char *text,const struct nk_user_font *font,struct nk_color ink) {
    int n=(int)strlen(text),shown=n;float dots=font->width(font->userdata,font->height,"…",3);
    while(shown>0 && font->width(font->userdata,font->height,text,shown)>r.w-(shown<n?dots:0)){
        shown--;while(shown>0 && ((unsigned char)text[shown]&0xc0)==0x80)shown--;
    }
    r.y+=(r.h-font->height)/2;r.h=font->height;
    nk_draw_text(nk_window_get_canvas(ui),r,text,shown,font,nk_rgba(0,0,0,0),ink);
    if(shown<n){r.x+=font->width(font->userdata,font->height,text,shown);r.w=dots;
        nk_draw_text(nk_window_get_canvas(ui),r,"…",3,font,nk_rgba(0,0,0,0),ink);}
}
static void completion_type(const ps_completion_symbol *symbol,char text[128]) {
    const char *name=strstr(symbol->signature,symbol->name),*arrow=strstr(symbol->signature," -> ");
    if(symbol->kind==PS_COMPLETION_TYPE)snprintf(text,128,"Typ");
    else if(symbol->kind==PS_COMPLETION_FUNCTION && symbol->language && arrow)snprintf(text,128,"%s",arrow+4);
    else if(symbol->kind==PS_COMPLETION_FUNCTION && name && name>symbol->signature){
        size_t n=(size_t)(name-symbol->signature);while(n && symbol->signature[n-1]==' ')n--;
        snprintf(text,128,"%.*s",(int)fmin(n,127),symbol->signature);
    }else snprintf(text,128,"%s",symbol->module);
}
static void completion_view(app *a,int width,int height) {
    struct nk_context *ui=a->ui;
    struct nk_text_edit *target=completion_target(a);
    if(!a->dock_visible[PS_DOCK_WORKSPACE] || (a->completion_pending_edit && a->completion_pending_edit!=target) ||
       (a->completion_active && a->completion_edit!=target) ||
       (a->completion_suppressed_edit && a->completion_suppressed_edit!=target))completion_close(a);
    a->completion_info_bounds=nk_rect(0,0,0,0);
    if(!a->completion_active || !a->dock_visible[PS_DOCK_WORKSPACE]) {
        nk_window_show(ui,"api-completion",NK_HIDDEN);nk_window_show(ui,"api-completion-info",NK_HIDDEN);return;
    }
    float w=fminf(400,(float)width-24),row_height=fmaxf(28,(float)a->preferences.ui_size+8),footer=26;
    unsigned rows=(unsigned)fmin(a->completion_count,a->preferences.completion_rows);
    float editor_top=fmaxf(8,a->source_editor_bounds.y),editor_bottom=fminf(height-8.f,a->source_editor_bounds.y+a->source_editor_bounds.h);
    float below=fmaxf(0,editor_bottom-(a->editor_caret.y+a->editor_caret.h+4));
    float above=fmaxf(0,a->editor_caret.y-editor_top-4);
    unsigned room=(unsigned)fmaxf(1,floorf((fmaxf(below,above)-footer-8)/row_height));
    if(rows>room)rows=room;
    if(rows!=a->completion_visible_rows)a->completion_reveal=true;
    a->completion_visible_rows=rows;
    float h=rows*row_height+footer+8;
    float x=fmaxf(8,fminf(a->editor_caret.x,(float)width-w-8)),y=a->editor_caret.y+a->editor_caret.h+4;
    if(h>below)y=fmaxf(editor_top,a->editor_caret.y-h-4);
    a->completion_list_bounds=nk_rect(x,y,w,h);
    size_t max_first=a->completion_count-rows;
    if(!max_first)a->completion_scroll_drag=false;
    if(a->completion_first>max_first)a->completion_first=max_first;
    if(a->completion_reveal){
        if(a->completion_selected<a->completion_first)a->completion_first=a->completion_selected;
        if(a->completion_selected>=a->completion_first+rows)a->completion_first=a->completion_selected-rows+1;
        a->completion_reveal=false;
    }
    struct nk_window *previous=ui->active;
    nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(1,3));
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(0,0));
    nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_color(UI_COLOR(header)));
    nk_style_push_float(ui,&ui->style.window.rounding,5);
    nk_style_push_float(ui,&ui->style.window.border,1);
    memset(a->completion_choice_bounds,0,sizeof a->completion_choice_bounds);
    a->completion_scroll_bounds=nk_rect(0,0,0,0);
    nk_window_show(ui,"api-completion",NK_SHOWN);
    if(nk_begin(ui,"api-completion",a->completion_list_bounds,NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER|NK_WINDOW_BACKGROUND)) {
        ui->active=previous;
        ui->current->flags &= ~(nk_flags)NK_WINDOW_ROM;ui->current->layout->flags &= ~(nk_flags)NK_WINDOW_ROM;
        struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
        if(nk_input_is_mouse_hovering_rect(&ui->input,a->completion_list_bounds) && ui->input.mouse.scroll_delta.y!=0){
            double next=(double)a->completion_first-ui->input.mouse.scroll_delta.y;
            a->completion_first=(size_t)fmax(0,fmin((double)max_first,next));
            ui->input.mouse.scroll_delta=nk_vec2(0,0);
        }
        if(max_first){
            struct nk_rect track=nk_rect(x+w-8,y+4,6,rows*row_height);
            float thumb_h=fmaxf(18,track.h*rows/(float)a->completion_count),travel=track.h-thumb_h;
            float top=track.y+travel*(float)a->completion_first/(float)max_first;
            bool down=nk_input_is_mouse_down(&ui->input,NK_BUTTON_LEFT);
            if(nk_input_is_mouse_pressed(&ui->input,NK_BUTTON_LEFT) && nk_input_is_mouse_hovering_rect(&ui->input,track)){
                a->completion_scroll_drag=true;
                a->completion_scroll_grab=ui->input.mouse.pos.y>=top && ui->input.mouse.pos.y<top+thumb_h?ui->input.mouse.pos.y-top:thumb_h/2;
            }
            if(a->completion_scroll_drag && down && travel>0){
                float position=(ui->input.mouse.pos.y-track.y-a->completion_scroll_grab)/travel;
                a->completion_first=(size_t)floor(fmax(0,fmin(1,position))*max_first+.5);
                top=track.y+travel*(float)a->completion_first/(float)max_first;
            }
            if(!down)a->completion_scroll_drag=false;
            a->completion_scroll_bounds=nk_rect(track.x,top,track.w,thumb_h);
        }
        for(unsigned row=0;row<rows;row++){
            size_t at=a->completion_first+row;const ps_completion_symbol *symbol=ps_completion_at(a->completion_indices[at]);
            nk_layout_row_dynamic(ui,row_height,1);struct nk_rect b=nk_widget_bounds(ui);nk_spacer(ui);
            b.w-=max_first?10:0;a->completion_choice_bounds[row]=b;
            bool chosen=a->completion_explicit && at==a->completion_selected;
            if(chosen || nk_input_is_mouse_hovering_rect(&ui->input,b))nk_fill_rect(canvas,b,0,UI_COLOR(text_selection));
            if(chosen)nk_fill_rect(canvas,nk_rect(b.x,b.y,2,b.h),0,UI_COLOR(selection_accent));
            ps_ui_draw_icon(canvas,symbol->kind==PS_COMPLETION_FUNCTION?PS_ICON_FUNCTION:symbol->kind==PS_COMPLETION_TYPE?PS_ICON_TYPE:PS_ICON_VALUE,nk_rect(b.x+7,b.y+4,18,b.h-8),UI_COLOR(accent));
            char type[128];completion_type(symbol,type);
            const struct nk_user_font *font=a->font_code;
            float type_w=fminf(105,font->width(font->userdata,font->height,type,(int)strlen(type)));
            completion_label(ui,nk_rect(b.x+33,b.y,fmaxf(1,b.w-49-type_w),b.h),ps_completion_insertion(&a->completion_query,symbol),font,UI_COLOR(code_text));
            completion_label(ui,nk_rect(b.x+b.w-type_w-8,b.y,type_w,b.h),type,font,UI_COLOR(muted));
            if(ps_ui_button_at(ui,b,symbol->name,chosen)){a->completion_selected=at;a->completion_explicit=true;a->completion_accept_pending=true;}
        }
        if(max_first)nk_fill_rect(canvas,a->completion_scroll_bounds,3,UI_COLOR(selection_accent));
        nk_layout_row_dynamic(ui,footer,1);struct nk_rect b=nk_widget_bounds(ui);nk_spacer(ui);
        nk_stroke_line(canvas,b.x,b.y,b.x+b.w,b.y,1,UI_COLOR(border));
        char count[64];snprintf(count,sizeof count,a->completion_count==1?"1 Vorschlag":"%zu Vorschläge",a->completion_count);
        completion_label(ui,nk_rect(b.x+8,b.y,b.w*.42f,b.h),count,ui->style.font,UI_COLOR(muted));
        completion_label(ui,nk_rect(b.x+b.w*.44f,b.y,b.w*.47f-8,b.h),a->completion_explicit?"Enter / Tab":"↑ ↓ auswählen",ui->style.font,UI_COLOR(muted));
        struct nk_rect info=nk_rect(b.x+b.w-25,b.y+3,19,19);
        completion_label(ui,info,"i",ui->style.font,UI_COLOR(muted));
        if(ps_ui_button_at(ui,info,a->completion_info_visible?"Kurzinfo ausblenden":"Kurzinfo anzeigen",a->completion_info_visible))a->completion_info_visible=!a->completion_info_visible;
    }
    nk_end(ui);ui->active=previous;
    nk_style_pop_float(ui);nk_style_pop_float(ui);nk_style_pop_style_item(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);
    nk_sdl_window_raise(ui,"api-completion");
    /* The candidate card describes a real catalog item. Call-parameter help
     * has its own controller; do not pass this off as that missing feature. */
    if(a->completion_info_visible){
        const ps_completion_symbol *symbol=ps_completion_at(a->completion_indices[a->completion_selected]);
        float iw=fminf(330,width-16.f);
        float right=width-8.f-(x+w+6),left=x-14;
        bool on_right=right>=220 || right>=iw;
        if(on_right)iw=fminf(iw,right);
        else if(left>=220)iw=fminf(iw,left);
        float sh=documentation_height(ui->style.font,symbol->signature,strlen(symbol->signature),iw-42);
        float ih=fminf(260,sh+16+3*row_height+24+3*ui->style.window.spacing.y+10);
        float ix=x+w+6,iy=y;
        if(!on_right){if(x-iw-6>=8)ix=x-iw-6;else{ix=fmaxf(8,fminf(x,width-iw-8));iy=y+h+6;}}
        iy=fmaxf(8,fminf(iy,height-ih-8));
        a->completion_info_bounds=nk_rect(ix,iy,iw,ih);
        nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_color(UI_COLOR(completion_info)));
        nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(13,12));
        nk_style_push_vec2(ui,&ui->style.window.scrollbar_size,nk_vec2(6,6));
        nk_style_push_float(ui,&ui->style.window.border,1);
        nk_style_push_float(ui,&ui->style.window.rounding,5);
        nk_window_show(ui,"api-completion-info",NK_SHOWN);
        if(nk_begin(ui,"api-completion-info",a->completion_info_bounds,NK_WINDOW_BORDER|NK_WINDOW_BACKGROUND)){
            ui->active=previous;ui->current->flags&=~(nk_flags)NK_WINDOW_ROM;ui->current->layout->flags&=~(nk_flags)NK_WINDOW_ROM;
            nk_layout_row_dynamic(ui,row_height,1);nk_label(ui,symbol->name,NK_TEXT_LEFT);
            nk_layout_row_dynamic(ui,sh+16,1);struct nk_rect signature=nk_widget_bounds(ui);nk_spacer(ui);
            nk_fill_rect(nk_window_get_canvas(ui),signature,3,UI_COLOR(edit));
            ps_ui_accessibility_label(ui,signature,symbol->signature);
            int at=0,n=(int)strlen(symbol->signature);float sy=signature.y+8;
            while(at<n){int used=ps_ui_text_line(ui->style.font,symbol->signature+at,n-at,signature.w-16,false);if(used<=0)used=1;
                nk_draw_text(nk_window_get_canvas(ui),nk_rect(signature.x+8,sy,signature.w-16,ui->style.font->height),symbol->signature+at,used,ui->style.font,nk_rgba(0,0,0,0),UI_COLOR(code_text));
                at+=used;sy+=ui->style.font->height;}
            nk_layout_row_dynamic(ui,row_height,1);nk_label_colored(ui,symbol->module,NK_TEXT_LEFT,UI_COLOR(muted));
            nk_layout_row_dynamic(ui,row_height,1);a->completion_reference_bounds=nk_widget_bounds(ui);nk_spacer(ui);
            completion_label(ui,a->completion_reference_bounds,"Referenz öffnen",ui->style.font,UI_COLOR(accent));
            if(ps_ui_button_at(ui,a->completion_reference_bounds,"Referenz öffnen",false)){
                struct nk_text_edit *edit=a->completion_edit;
                if(api_reference_open(a,symbol)){completion_close(a);a->completion_focus_edit=edit;}
            }
        }
        nk_end(ui);ui->active=previous;
        nk_style_pop_float(ui);nk_style_pop_float(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);nk_style_pop_style_item(ui);
        nk_sdl_window_raise(ui,"api-completion-info");
    }else nk_window_show(ui,"api-completion-info",NK_HIDDEN);
}
