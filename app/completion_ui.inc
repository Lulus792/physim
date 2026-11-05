static const ps_completion_symbol *completion_symbol(app *a,size_t id){
    const ps_completion_symbol *catalog=ps_completion_at(id);if(catalog)return catalog;
    const ps_completion_local *local=ps_completion_locals_at(&a->completion_locals,id);return local?&local->symbol:NULL;
}
static const ps_completion_local *completion_local(app *a,const ps_completion_symbol *symbol){
    if(!symbol || symbol->kind!=PS_COMPLETION_VARIABLE)return NULL;
    return ps_completion_locals_find(&a->completion_locals,symbol->name,strlen(symbol->name));
}
static void completion_source_label(app *a,struct nk_text_edit *edit,const ps_completion_local *local,char label[4160]){
    const char *name="Aktuelle Datei";
    if(edit==&a->experiment)name=experiment_source(a);
    else if(edit==&a->analysis)name=analysis_source(a);
    else{workspace_document *doc=active_document(a);if(doc && &doc->edit==edit){
        name=doc->path;const char *slash=strrchr(name,'/'),*back=strrchr(name,'\\');
        if(back && (!slash || back>slash))slash=back;
        if(slash)name=slash+1;
    }}
    snprintf(label,4160,"%s · Zeile %zu",name,local->declaration.name.line);
}
static void completion_info_geometry_clear(app *a){
    a->completion_info_bounds=a->completion_signature_bounds=a->completion_help_body_bounds=
        a->completion_summary_bounds=a->completion_arguments_bounds=a->completion_reference_bounds=nk_rect(0,0,0,0);
    a->completion_help_natural_height=0;
}
static void completion_info_stop(app *a){
    ps_completion_info_cancel(&a->completion_info);a->completion_info_visible=false;completion_info_geometry_clear(a);
}
static ps_completion_info_key completion_info_current(app *a){
    ps_completion_info_key key={0};struct nk_text_edit *edit=a->completion_edit;
    if(!edit || !a->completion_active || !a->completion_count || a->completion_selected>=a->completion_count)return key;
    key.editor=edit;key.candidate=a->completion_indices[a->completion_selected];key.cursor=(size_t)edit->cursor;
    key.text=nk_str_get_const(&edit->string);key.bytes=(size_t)nk_str_len_char(&edit->string);
    key.scroll_x=edit->scrollbar.x;key.scroll_y=edit->scrollbar.y;return key;
}
static void completion_info_request(app *a,bool immediate){
    ps_completion_info_key key=completion_info_current(a);
    ps_result result=ps_completion_info_request(&a->completion_info,&key,ps_clock(),immediate);
    if(result!=PS_OK){completion_info_stop(a);if(result==PS_MEMORY)status(a,"Nicht genügend Speicher für die Kurzinfo. Vorschläge bleiben verfügbar.");return;}
    a->completion_info_visible=immediate;a->completion_help_scroll_x=a->completion_help_scroll_y=0;
    if(!immediate)completion_info_geometry_clear(a);
}
static void completion_info_toggle(app *a){if(a->completion_info_visible)completion_info_stop(a);else completion_info_request(a,true);}
static void completion_pending_clear(app *a) {
    free(a->completion_pending_text);a->completion_pending_text=NULL;
    a->completion_pending_edit=NULL;a->completion_pending_bytes=0;a->completion_due=0;
}
static void completion_hide(app *a) {
    completion_info_stop(a);a->completion_info_button_bounds=nk_rect(0,0,0,0);
    ps_completion_help_destroy(&a->completion_help);
    ps_completion_locals_destroy(&a->completion_locals);
    a->completion_help_scroll_x=a->completion_help_scroll_y=0;
    free(a->completion_indices);a->completion_indices=NULL;a->completion_edit=a->completion_focus_edit=NULL;
    a->completion_active=a->completion_accept_pending=a->completion_explicit=a->completion_manual=false;a->completion_count=a->completion_selected=a->completion_capacity=0;
    a->completion_first=0;a->completion_visible_rows=0;a->completion_scroll_drag=false;
    a->completion_pointer_capture=false;
    a->completion_info_bounds=a->completion_signature_bounds=a->completion_scroll_bounds=nk_rect(0,0,0,0);
}
static void completion_close(app *a) {
    completion_pending_clear(a);completion_hide(a);a->completion_suppressed_edit=NULL;a->completion_keyboard_edit=NULL;
}
static void completion_return_focus(app *a,struct nk_text_edit *edit){
    a->completion_focus_edit=a->completion_keyboard_edit=edit;
    if(edit){nk_sdl_accessibility_keyboard_begin(a->ui);nk_window_set_focus(a->ui,"workspace");}
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
    if(ps_completion_locals_update(&a->completion_locals,edit,text,bytes,cursor,a->completion_query.language)!=PS_OK){completion_close(a);return false;}
    size_t prefix=(size_t)nk_utf_len(a->completion_query.prefix,(int)strlen(a->completion_query.prefix));
    bool argument=a->completion_locals.call.active && (prefix || a->completion_locals.call.at_start);
    if(!a->completion_manual && !a->completion_query.member && prefix<2 && !argument){completion_close(a);return false;}
    size_t capacity=ps_completion_count()+a->completion_locals.count;
    if(capacity>SIZE_MAX/sizeof *a->completion_indices){completion_close(a);return false;}
    if(capacity>a->completion_capacity){size_t *indices=realloc(a->completion_indices,capacity*sizeof *indices);
        if(!indices){completion_close(a);return false;}a->completion_indices=indices;a->completion_capacity=capacity;}
    size_t locals=ps_completion_locals_matches(&a->completion_locals,&a->completion_query,a->completion_indices,capacity);
    size_t api=ps_completion_matches(&a->completion_query,a->completion_indices+locals,capacity-locals);
    a->completion_count=locals;
    for(size_t i=0;i<api;i++){
        size_t id=a->completion_indices[locals+i];const ps_completion_symbol *symbol=ps_completion_at(id);
        if(!a->completion_query.member && ps_completion_locals_find(&a->completion_locals,symbol->name,strlen(symbol->name)))continue;
        a->completion_indices[a->completion_count++]=id;
    }
    if(!a->completion_count){completion_close(a);return false;}
    a->completion_selected=0;
    for(size_t i=0;i<a->completion_count;i++)if(a->completion_indices[i]==previous){a->completion_selected=i;break;}
    if(count!=a->completion_count)a->completion_reveal=true;
    return true;
}
static void completion_open_kind(app *a,struct nk_text_edit *edit,bool manual) {
    completion_close(a);
    if(a->completion_composing || document_language(a,edit)<0)return;
    a->completion_edit=edit;a->completion_active=true;a->completion_explicit=a->completion_manual=manual;a->completion_cursor=edit->cursor;
    a->completion_reveal=true;
    if(!completion_refresh(a) && manual)status(a,"Keine Vorschläge an dieser Textposition.");
}
static void completion_open(app *a,struct nk_text_edit *edit) {completion_open_kind(a,edit,true);}
static void completion_edit_input(app *a) {
    completion_info_stop(a);
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
    completion_pending_clear(a);completion_hide(a);completion_return_focus(a,edit);
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
        nk_rune rune;int length;const char *at=nk_str_at_const(&edit->string,edit->cursor,&rune,&length);
        size_t cursor=at?(size_t)(at-text):bytes;
        if(!query.member && nk_utf_len(query.prefix,(int)strlen(query.prefix))<2 &&
           !ps_completion_locals_argument_hint(text,bytes,cursor,query.language))return;
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
    const ps_completion_symbol *symbol=completion_symbol(a,a->completion_indices[a->completion_selected]);
    const char *insert=ps_completion_insertion(&a->completion_query,symbol),*text=nk_str_get_const(&edit->string);
    if(!text)text="";
    if(strlen(insert)==a->completion_query.end-a->completion_query.start &&
       !memcmp(text+a->completion_query.start,insert,strlen(insert))) {
        completion_close(a);completion_return_focus(a,edit);return;
    }
    int start=edit->select_start,end=edit->select_end,cursor=edit->cursor;
    edit->select_start=nk_utf_len(text,(int)a->completion_query.start);
    edit->select_end=nk_utf_len(text,(int)a->completion_query.end);edit->cursor=edit->select_end;
    if(!nk_textedit_paste(edit,insert,(int)strlen(insert))) {
        edit->select_start=start;edit->select_end=end;edit->cursor=cursor;
        status(a,"Vorschlag konnte nicht eingefügt werden. Text bleibt erhalten.");
    }
    completion_close(a);
    completion_return_focus(a,edit);
}
static bool completion_keyboard_key(app *a,const SDL_KeyboardEvent *key) {
    struct nk_text_edit *edit=completion_target(a);
    if(a->completion_active && edit!=a->completion_edit)completion_close(a);
    if(edit && document_language(a,edit)>=0 && (edit->active || a->completion_active) && key->key==SDLK_SPACE && (key->mod&SDL_KMOD_CTRL) &&
       !(key->mod&(SDL_KMOD_ALT|SDL_KMOD_GUI|SDL_KMOD_SHIFT))) {
        if(!key->repeat){if(a->completion_active)completion_info_toggle(a);else completion_open(a,edit);}return true;
    }
    if(key->key==SDLK_ESCAPE && (a->completion_active || a->completion_pending_edit)){completion_dismiss(a,edit);return true;}
    if(a->completion_active && key->key==SDLK_TAB && (key->mod&(SDL_KMOD_SHIFT|SDL_KMOD_CTRL|SDL_KMOD_ALT|SDL_KMOD_GUI))){
        completion_close(a);
        if((key->mod&SDL_KMOD_SHIFT) && !(key->mod&(SDL_KMOD_CTRL|SDL_KMOD_ALT|SDL_KMOD_GUI))){
            /* Nuklear's ALLOW_TAB path inserts indentation even with Shift.
             * The completion contract instead leaves the source editor. */
            edit->active=false;
            struct nk_window *workspace=nk_window_find(a->ui,"workspace");
            if(workspace)workspace->edit.active=false;
            nk_sdl_accessibility_keyboard_release(a->ui);
            return true;
        }
        return false;
    }
    if(a->completion_active && a->completion_info_visible && (key->mod&SDL_KMOD_ALT) &&
       !(key->mod&(SDL_KMOD_CTRL|SDL_KMOD_GUI|SDL_KMOD_SHIFT)) &&
       (key->key==SDLK_PAGEUP || key->key==SDLK_PAGEDOWN || key->key==SDLK_HOME || key->key==SDLK_END)){
        double maximum=fmax(0,floor(a->completion_help_natural_height-a->completion_help_body_bounds.h));
        double next=a->completion_help_scroll_y,page=fmax(1,floor(a->completion_help_body_bounds.h*.85));
        if(key->key==SDLK_HOME)next=0;else if(key->key==SDLK_END)next=maximum;
        else next+=key->key==SDLK_PAGEUP?-page:page;
        a->completion_help_scroll_y=(nk_uint)fmax(0,fmin(maximum,next));return true;
    }
    bool control_move=a->completion_active && (key->mod&SDL_KMOD_CTRL) && !(key->mod&(SDL_KMOD_ALT|SDL_KMOD_GUI|SDL_KMOD_SHIFT)) && (key->key==SDLK_N || key->key==SDLK_P);
    if(a->completion_active && !control_move && ((key->mod&(SDL_KMOD_CTRL|SDL_KMOD_GUI|SDL_KMOD_ALT)) ||
       (key->key>=SDLK_F1 && key->key<=SDLK_F12))){completion_close(a);return false;}
    if(!a->completion_active || (!control_move && key->mod&(SDL_KMOD_CTRL|SDL_KMOD_GUI|SDL_KMOD_ALT)))return false;
    if(control_move || key->key==SDLK_UP || key->key==SDLK_DOWN) {
        bool up=key->key==SDLK_UP || key->key==SDLK_P;
        a->completion_selected=a->completion_explicit?(a->completion_selected+(up?a->completion_count-1:1))%a->completion_count:up?a->completion_count-1:0;
        a->completion_explicit=true;
        a->completion_reveal=true;
        completion_info_request(a,false);
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
static void completion_name(app *a,struct nk_rect r,const char *name,struct nk_color ink){
    const char *prefix=a->completion_query.prefix;size_t n=strlen(prefix);
    if(n && n<=strlen(name) && !memcmp(name,prefix,n)){
        float width=a->completion_bold_font.width(a->completion_bold_font.userdata,a->completion_bold_font.height,prefix,(int)n);
        completion_label(a->ui,r,prefix,&a->completion_bold_font,ink);
        if(width>=r.w)return;
        r.x+=width;r.w-=width;name+=n;
    }
    completion_label(a->ui,r,name,a->font_code,ink);
}
static ps_result completion_help_load(void *user,const char *relative,ps_document **out){
    app *a=user;char path[4096];join(path,sizeof path,a->root,relative);
    size_t bytes=0;char *text=load_utf8_text(path,&bytes);if(!text)return PS_IO;
    ps_result result=ps_document_parse(text,bytes,false,out);free(text);return result;
}
/* Measure and paint the same wrapped bytes, including the full argument table.
 * The bounded card scrolls vertically; content is never shortened to fit it. */
static float completion_help_text(app *a,const struct nk_user_font *font,const char *text,size_t bytes,
                                  float width,struct nk_color color,bool draw,struct nk_rect *bounds){
    float h=documentation_height(font,text,bytes,fmaxf(1,width));
    if(draw){
        ps_ui_row_exact_dynamic(a->ui,h,1);struct nk_rect r;nk_widget(&r,a->ui);r.w=fminf(r.w,width);
        if(bounds)*bounds=r;
        size_t at=0;float y=r.y;
        do{size_t length=0,used=documentation_line(font,text+at,bytes-at,fmaxf(1,width),&length);
            nk_draw_text(nk_window_get_canvas(a->ui),nk_rect(r.x,y,width,font->height),text+at,(int)length,font,nk_rgba(0,0,0,0),color);
            at+=used;y+=font->height+6;
        }while(at<bytes);
    }
    return h;
}
static void completion_help_gap(app *a,float height,bool draw){if(draw){ps_ui_row_exact_dynamic(a->ui,height,1);nk_spacer(a->ui);}}
static ps_syntax_role completion_signature_role(void *user,size_t offset,size_t bytes){
    const ps_completion_symbol *symbol=user;
    size_t begin=!strncmp(symbol->signature,"let ",4) || !strncmp(symbol->signature,"var ",4)?4:0;
    return offset==begin && bytes==strlen(symbol->name)?PS_SYNTAX_VARIABLE:PS_SYNTAX_TEXT;
}
static float completion_help_content(app *a,const ps_completion_symbol *symbol,float width,bool draw){
    ps_completion_help *help=&a->completion_help;ps_document *d=help->document;
    const struct nk_user_font *code=a->font_code,*body=&a->workbench_fonts[2],*small=&a->workbench_fonts[9],*link=&a->workbench_fonts[0];
    const ps_completion_local *local=completion_local(a,symbol);
    float total=0;const char *kind=local?"Variable":symbol->kind==PS_COMPLETION_FUNCTION?"Funktion":symbol->kind==PS_COMPLETION_TYPE?"Typ":"Wert";
    float name_width=code->width(code->userdata,code->height,symbol->name,(int)strlen(symbol->name));
    float kind_width=small->width(small->userdata,small->height,kind,(int)strlen(kind));
    bool inline_kind=name_width+kind_width+9<=width;
    struct nk_rect title={0};
    total+=completion_help_text(a,code,symbol->name,strlen(symbol->name),width,syntax_color(syntax_symbol_kind(symbol)),draw,&title);
    if(draw){
        ps_ui_accessibility_label(a->ui,title,symbol->name);
        if(inline_kind)completion_label(a->ui,nk_rect(title.x+name_width+9,title.y,kind_width,title.h),kind,small,UI_COLOR(muted));
    }
    if(!inline_kind)total+=completion_help_text(a,small,kind,strlen(kind),width,UI_COLOR(muted),draw,NULL);
    total+=7;completion_help_gap(a,7,draw);
    float sh=documentation_height(code,symbol->signature,strlen(symbol->signature),fmaxf(1,width-16))+16;
    if(draw){
        ps_ui_row_exact_dynamic(a->ui,sh,1);struct nk_rect r;nk_widget(&r,a->ui);r.w=width;a->completion_signature_bounds=r;
        nk_fill_rect(nk_window_get_canvas(a->ui),r,3,UI_COLOR(edit));
        ps_ui_accessibility_label(a->ui,r,symbol->signature);
        syntax_cursor cursor={0};ps_syntax_init(&cursor.scan,symbol->signature,strlen(symbol->signature),symbol->language);
        if(local){cursor.scan.resolve=completion_signature_role;cursor.scan.user=(void*)symbol;}
        size_t at=0,n=strlen(symbol->signature);float y=r.y+8;
        do{size_t length=0,used=documentation_line(code,symbol->signature+at,n-at,fmaxf(1,width-16),&length);
            syntax_draw_range(nk_window_get_canvas(a->ui),code,&cursor,at,at+length,r.x+8,y);at+=used;y+=code->height+6;
        }while(at<n);
    }
    total+=sh+10;completion_help_gap(a,10,draw);
    if(local){
        const char *summary=local->declaration.kind==PS_LANG_EDITOR_PARAMETER?"Parameter aus dem aktuellen Gültigkeitsbereich.":
            local->declaration.kind==PS_LANG_EDITOR_SELF?"Instanz des aktuellen Typs.":"Variable aus dem aktuellen Gültigkeitsbereich.";
        total+=completion_help_text(a,body,summary,strlen(summary),width,UI_COLOR(text),draw,&a->completion_summary_bounds);
        if(draw){
            ps_ui_accessibility_label(a->ui,a->completion_summary_bounds,summary);
            a->completion_arguments_bounds=a->completion_reference_bounds=nk_rect(0,0,0,0);
        }
        total+=9;completion_help_gap(a,9,draw);
        char source[4160];completion_source_label(a,a->completion_edit,local,source);
        total+=completion_help_text(a,small,source,strlen(source),width,UI_COLOR(muted),draw,NULL);
        return total;
    }
    const char *summary=help->status!=PS_OK?"Dokumentation nicht verfügbar. Name und Signatur bleiben verfügbar.":
        help->guide?"Keine Kurzbeschreibung zu diesem Typ vorhanden.":
        help->preview.summary==SIZE_MAX?"Keine Kurzbeschreibung in der Referenz vorhanden.":d->text+d->blocks[help->preview.summary].text;
    total+=completion_help_text(a,body,summary,strlen(summary),width,UI_COLOR(text),draw,&a->completion_summary_bounds);
    if(draw)ps_ui_accessibility_label(a->ui,a->completion_summary_bounds,summary);
    if(help->status==PS_OK && !help->guide && help->preview.arguments!=SIZE_MAX){
        size_t first=help->preview.arguments;
        const ps_doc_block *heading=&d->blocks[first];
        const ps_doc_cell *meaning=&d->cells[heading->first_cell+1];
        total+=8;completion_help_gap(a,8,draw);
        total+=completion_help_text(a,small,d->text+meaning->text,meaning->length,width,UI_COLOR(muted),draw,NULL);
        for(size_t i=first+1;i<help->end;i++){
            const ps_doc_block *b=&d->blocks[i];if(b->kind!=PS_DOC_TABLE_ROW || b->table_first!=first)break;
            if(b->cell_count!=2)continue;
            const ps_doc_cell *name=&d->cells[b->first_cell],*description=name+1;
            total+=8;completion_help_gap(a,8,draw);struct nk_rect r={0},last={0};
            float h=completion_help_text(a,code,d->text+name->text,name->length,width,UI_COLOR(code_variable),draw,&r);
            h+=completion_help_text(a,body,d->text+description->text,description->length,width,UI_COLOR(text),draw,&last);total+=h;
            if(draw){a->completion_arguments_bounds=nk_rect(r.x,r.y,width,last.y+last.h-r.y);ps_ui_accessibility_label(a->ui,a->completion_arguments_bounds,d->text+b->text);}
        }
    }
    total+=9;completion_help_gap(a,9,draw);
    char source[384];snprintf(source,sizeof source,"%s · %s",symbol->module,*help->path?help->path:"Referenz");
    total+=completion_help_text(a,small,source,strlen(source),width,UI_COLOR(muted),draw,NULL);
    total+=11;completion_help_gap(a,11,draw);
    const char *reference="Referenz öffnen · Umschalt+F1";float rh=fmaxf(24,link->height+8);total+=rh;
    if(draw){
        ps_ui_row_exact_dynamic(a->ui,rh,1);a->completion_reference_bounds=nk_widget_bounds(a->ui);nk_spacer(a->ui);
        completion_label(a->ui,a->completion_reference_bounds,reference,link,help->status==PS_OK?UI_COLOR(accent):UI_COLOR(muted));
        if(help->status!=PS_OK)nk_widget_disable_begin(a->ui);
        bool open=ps_ui_button_at(a->ui,a->completion_reference_bounds,reference,false);
        if(help->status!=PS_OK)nk_widget_disable_end(a->ui);
        if(open && help->status==PS_OK){struct nk_text_edit *edit=a->completion_edit;
            if(api_reference_open(a,symbol)){completion_close(a);completion_return_focus(a,edit);}}
    }
    /* Integer Nuklear scroll offsets need a small trailing inset when fractional
     * font metrics put the final button just beyond the viewport boundary. */
    total+=2;completion_help_gap(a,2,draw);
    return total;
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
    ps_completion_info_key info_key=completion_info_current(a);
    a->completion_info_visible=ps_completion_info_update(&a->completion_info,&info_key,ps_clock());
    if(!a->completion_info_visible)completion_info_geometry_clear(a);
    float w=fminf(400,(float)width-24),row_height=fmaxf(fmaxf(28,(float)a->preferences.ui_size+8),a->font_code->height+8),footer=26;
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
    /* Reuse the real semibold monospace face in the atlas; no synthetic stroke. */
    a->completion_bold_font=*a->workbench_marker_source;
    a->completion_bold_font.height=a->font_code->height;
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
        bool wheel_frame=ui->input.mouse.scroll_delta.y!=0;
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
            size_t at=a->completion_first+row;const ps_completion_symbol *symbol=completion_symbol(a,a->completion_indices[at]);
            nk_layout_row_dynamic(ui,row_height,1);struct nk_rect b=nk_widget_bounds(ui);nk_spacer(ui);
            b.w-=max_first?10:0;a->completion_choice_bounds[row]=b;
            if(at!=a->completion_selected && !wheel_frame && !a->completion_scroll_drag &&
               nk_input_is_mouse_moved(&ui->input) && nk_input_is_mouse_hovering_rect(&ui->input,b)){
                a->completion_selected=at;a->completion_explicit=false;completion_info_request(a,false);
            }
            bool chosen=a->completion_explicit && at==a->completion_selected;
            if(chosen || nk_input_is_mouse_hovering_rect(&ui->input,b))nk_fill_rect(canvas,b,0,UI_COLOR(text_selection));
            if(chosen)nk_fill_rect(canvas,nk_rect(b.x,b.y,2,b.h),0,UI_COLOR(selection_accent));
            struct nk_color ink=syntax_color(syntax_symbol_kind(symbol));
            completion_label(ui,nk_rect(b.x+7,b.y,18,b.h),symbol->kind==PS_COMPLETION_VARIABLE?"x":symbol->kind==PS_COMPLETION_FUNCTION?"ƒ":symbol->kind==PS_COMPLETION_TYPE?"T":"#",a->font_code,ink);
            char catalog_type[128];completion_type(symbol,catalog_type);
            const ps_completion_local *local=completion_local(a,symbol);
            const char *type=local?(*local->type?local->type:"—"):catalog_type;
            const struct nk_user_font *font=a->font_code;
            float type_w=fminf(105,font->width(font->userdata,font->height,type,(int)strlen(type)));
            completion_name(a,nk_rect(b.x+33,b.y,fmaxf(1,b.w-49-type_w),b.h),ps_completion_insertion(&a->completion_query,symbol),ink);
            completion_label(ui,nk_rect(b.x+b.w-type_w-8,b.y,type_w,b.h),type,font,UI_COLOR(muted));
            if(ps_ui_button_at(ui,b,symbol->name,chosen)){a->completion_selected=at;a->completion_explicit=true;a->completion_accept_pending=true;completion_info_stop(a);}
        }
        if(max_first)nk_fill_rect(canvas,a->completion_scroll_bounds,3,UI_COLOR(selection_accent));
        nk_layout_row_dynamic(ui,footer,1);struct nk_rect b=nk_widget_bounds(ui);nk_spacer(ui);
        nk_stroke_line(canvas,b.x,b.y,b.x+b.w,b.y,1,UI_COLOR(border));
        char count[64];snprintf(count,sizeof count,a->completion_count==1?"1 Vorschlag":"%zu Vorschläge",a->completion_count);
        completion_label(ui,nk_rect(b.x+8,b.y,b.w*.42f,b.h),count,ui->style.font,UI_COLOR(muted));
        completion_label(ui,nk_rect(b.x+b.w*.44f,b.y,b.w*.47f-8,b.h),a->completion_explicit?"Enter / Tab":"↑ ↓ auswählen",ui->style.font,UI_COLOR(muted));
        struct nk_rect info=nk_rect(b.x+b.w-25,b.y+3,19,19);
        a->completion_info_button_bounds=info;
        completion_label(ui,info,"i",ui->style.font,UI_COLOR(muted));
        if(ps_ui_button_at(ui,info,a->completion_info_visible?"Kurzinfo ausblenden (Strg+Leertaste)":"Kurzinfo anzeigen (Strg+Leertaste)",a->completion_info_visible))completion_info_toggle(a);
        if(nk_input_is_mouse_hovering_rect(&ui->input,info))nk_tooltip(ui,"Kurzinfo: Strg+Leertaste. Scrollen: Alt+BildAuf/BildAb, Alt+Pos1/Ende.");
    }
    nk_end(ui);ui->active=previous;
    nk_style_pop_float(ui);nk_style_pop_float(ui);nk_style_pop_style_item(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);
    nk_sdl_window_raise(ui,"api-completion");
    /* The candidate card describes a catalog item or a real declaration. Call-parameter help
     * has its own controller; do not pass this off as that missing feature. */
    if(a->completion_info_visible){
        const ps_completion_symbol *symbol=completion_symbol(a,a->completion_indices[a->completion_selected]);
        if(symbol->kind==PS_COMPLETION_VARIABLE){
            ps_completion_help_destroy(&a->completion_help);
        }else if(a->completion_help.symbol!=symbol){
            ps_completion_help_select(&a->completion_help,symbol,completion_help_load,a);
            a->completion_help_scroll_x=a->completion_help_scroll_y=0;
        }
        float iw=fminf(330,width-16.f);
        float right=width-8.f-(x+w+6),left=x-14;
        bool on_right=right>=220 || right>=iw;
        if(on_right)iw=fminf(iw,right);
        else if(left>=220)iw=fminf(iw,left);
        float inner=fmaxf(1,floorf(iw-36));
        a->completion_help_natural_height=completion_help_content(a,symbol,inner,false);
        float ih=fminf(fminf(260,height-16.f),a->completion_help_natural_height+28);
        float ix=x+w+6,iy=y;
        if(!on_right){if(x-iw-6>=8)ix=x-iw-6;else{ix=fmaxf(8,fminf(x,width-iw-8));iy=y+h+6;}}
        iy=fmaxf(8,fminf(iy,height-ih-8));
        a->completion_info_bounds=nk_rect(ix,iy,iw,ih);
        nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_color(UI_COLOR(completion_info)));
        nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(13,12));
        nk_style_push_vec2(ui,&ui->style.window.scrollbar_size,nk_vec2(6,0));
        nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(0,0));
        nk_style_push_vec2(ui,&ui->style.window.group_padding,nk_vec2(0,0));
        nk_style_push_float(ui,&ui->style.window.border,1);
        nk_style_push_float(ui,&ui->style.window.rounding,5);
        nk_window_show(ui,"api-completion-info",NK_SHOWN);
        if(nk_begin(ui,"api-completion-info",a->completion_info_bounds,NK_WINDOW_BORDER|NK_WINDOW_BACKGROUND|NK_WINDOW_NO_SCROLLBAR)){
            ui->active=previous;ui->current->flags&=~(nk_flags)NK_WINDOW_ROM;ui->current->layout->flags&=~(nk_flags)NK_WINDOW_ROM;
            ps_ui_row_exact_dynamic(ui,fmaxf(1,ih-26),1);a->completion_help_body_bounds=nk_widget_bounds(ui);
            /* Nuklear only routes a group's wheel/scrollbar to its active root
             * window. Temporarily own that route, then restore the editor's
             * active window; browsing the card must not steal text focus. */
            ui->active=ui->current;
            if(nk_group_scrolled_offset_begin(ui,&a->completion_help_scroll_x,&a->completion_help_scroll_y,"completion-help-body",0)){
                a->completion_arguments_bounds=nk_rect(0,0,0,0);
                completion_help_content(a,symbol,inner,true);nk_group_scrolled_end(ui);
            }
            ui->active=previous;
        }
        nk_end(ui);ui->active=previous;
        nk_style_pop_float(ui);nk_style_pop_float(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);nk_style_pop_style_item(ui);
        nk_sdl_window_raise(ui,"api-completion-info");
    }else nk_window_show(ui,"api-completion-info",NK_HIDDEN);
}
