static void completion_close(app *a) {
    free(a->completion_indices);a->completion_indices=NULL;a->completion_edit=a->completion_focus_edit=NULL;
    a->completion_active=a->completion_accept_pending=false;a->completion_count=a->completion_selected=0;
}
static struct nk_text_edit *completion_target(app *a) {
    if(a->project_manager || a->document_pending || a->recovery || a->toolbar_keyboard || a->toolbar_menu || a->dialog_pending)return NULL;
    if(a->loaded && a->tab==0)return a->analysis_only?&a->analysis:&a->experiment;
    if(a->loaded && a->tab==2 && a->analysis_tab==1)return &a->analysis;
    workspace_document *doc=active_document(a);
    return a->tab==8 && doc && !doc->recovery?&doc->edit:NULL;
}
static bool completion_refresh(app *a) {
    struct nk_text_edit *edit=a->completion_edit;
    if(!edit || edit->select_start!=edit->select_end){completion_close(a);return false;}
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
    a->completion_count=ps_completion_matches(&a->completion_query,a->completion_indices,ps_completion_count());
    if(!a->completion_count){completion_close(a);return false;}
    a->completion_selected=0;
    for(size_t i=0;i<a->completion_count;i++)if(a->completion_indices[i]==previous){a->completion_selected=i;break;}
    return true;
}
static void completion_open(app *a,struct nk_text_edit *edit) {
    completion_close(a);
    if(document_language(a,edit)<0)return;
    size_t count=ps_completion_count();
    if(count>SIZE_MAX/sizeof *a->completion_indices)return;
    a->completion_indices=malloc(count*sizeof *a->completion_indices);
    if(!a->completion_indices){status(a,"Nicht genügend Speicher für API-Vorschläge.");return;}
    a->completion_edit=edit;a->completion_active=true;
    if(!completion_refresh(a))status(a,"Keine API-Vorschläge an dieser Textposition.");
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
    if(a->completion_active && ((key->mod&(SDL_KMOD_CTRL|SDL_KMOD_GUI|SDL_KMOD_ALT)) ||
       (key->key>=SDLK_F1 && key->key<=SDLK_F12))){completion_close(a);return false;}
    if(!a->completion_active || key->mod&(SDL_KMOD_CTRL|SDL_KMOD_GUI|SDL_KMOD_ALT))return false;
    if(key->key==SDLK_ESCAPE){completion_close(a);a->completion_focus_edit=edit;return true;}
    if(key->key==SDLK_UP || key->key==SDLK_DOWN) {
        a->completion_selected=(a->completion_selected+(key->key==SDLK_UP?a->completion_count-1:1))%a->completion_count;
        return true;
    }
    if(key->key==SDLK_RETURN || key->key==SDLK_KP_ENTER || key->key==SDLK_TAB) {
        if(!key->repeat)a->completion_accept_pending=true;return true;
    }
    return false;
}
static void completion_pointer_event(app *a,const SDL_MouseButtonEvent *event) {
    if(!a->completion_active){if(a->completion_focus_edit)completion_close(a);return;}
    const struct nk_rect regions[]={a->source_editor_bounds,a->completion_list_bounds,a->completion_button_bounds,a->api_reference_button_bounds};
    for(unsigned i=0;i<4;i++)if(event->x>=regions[i].x && event->x<regions[i].x+regions[i].w &&
        event->y>=regions[i].y && event->y<regions[i].y+regions[i].h)return;
    completion_close(a);
}
static void completion_view(app *a) {
    struct nk_context *ui=a->ui;
    nk_layout_row_dynamic(ui,280,1);
    a->completion_list_bounds=nk_widget_bounds(ui);
    if(nk_group_begin(ui,"api-completion",NK_WINDOW_BORDER)) {
        char title[128];snprintf(title,sizeof title,"%zu/%zu · Pfeile wählen · Enter/Tab übernehmen",a->completion_selected+1,a->completion_count);
        nk_layout_row_dynamic(ui,24,1);nk_label(ui,title,NK_TEXT_LEFT);
        size_t first=(a->completion_selected/4)*4;
        for(size_t row=0;row<4 && first+row<a->completion_count;row++) {
            size_t at=first+row;const ps_completion_symbol *symbol=ps_completion_at(a->completion_indices[at]);
            nk_layout_row_dynamic(ui,28,1);a->completion_choice_bounds[row]=nk_widget_bounds(ui);
            char label[180];snprintf(label,sizeof label,"%s · %s",symbol->name,symbol->module);
            if(action_button(ui,label,at==a->completion_selected)){a->completion_selected=at;a->completion_accept_pending=true;}
        }
        const ps_completion_symbol *symbol=ps_completion_at(a->completion_indices[a->completion_selected]);
        nk_layout_row_dynamic(ui,48,1);nk_label_wrap(ui,symbol->signature);
        nk_layout_row_dynamic(ui,28,2);
        a->completion_reference_bounds=nk_widget_bounds(ui);
        if(nk_button_label(ui,"Referenz")) {
            struct nk_text_edit *edit=a->completion_edit;
            if(api_reference_open(a,symbol)){completion_close(a);a->completion_focus_edit=edit;}
        }
        if(nk_button_label(ui,"Schließen · Esc")){struct nk_text_edit *edit=a->completion_edit;completion_close(a);a->completion_focus_edit=edit;}
        nk_group_end(ui);
    }
}
