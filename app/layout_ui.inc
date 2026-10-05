/* Personal panel presets; applying a preset touches presentation geometry only. */
static void layouts_start(app *a) {
    a->layouts=(ps_layout_catalog){0};a->layout_selected=-1;
    a->layouts_path[0]=a->layouts_error[0]=a->layout_name[0]=0;
    const char *path=a->preferences_path;
    const char *slash=strrchr(path,'/'),*backslash=strrchr(path,'\\');
    if(backslash && (!slash || backslash>slash))slash=backslash;
    size_t parent=slash?(size_t)(slash-path)+1:0;
    if(*path && parent+sizeof "layouts.bin"<=sizeof a->layouts_path) {
        memcpy(a->layouts_path,path,parent);
        memcpy(a->layouts_path+parent,"layouts.bin",sizeof "layouts.bin");
    }
    ps_result r=ps_layout_catalog_read(a->layouts_path,&a->layouts);
    a->layouts_writable=r==PS_OK || r==PS_EOF;
    if(!a->layouts_writable)
        snprintf(a->layouts_error,sizeof a->layouts_error,"Panelanordnungen konnten nicht geladen werden (%s). Die Datei bleibt erhalten.",ps_result_string(r));
}
static ps_layout_state layout_capture(const app *a) {
    return (ps_layout_state){a->preferences.dock,a->preferences.sidebar_width,
        a->preferences.inspector_width,a->preferences.log_height,a->show_log};
}
static bool layouts_save(app *a,const ps_layout_catalog *next) {
    ps_result r=ps_layout_catalog_write(a->layouts_path,next);
    if(r!=PS_OK) {
        snprintf(a->layouts_error,sizeof a->layouts_error,"Panelanordnungen konnten nicht gespeichert werden (%s).",ps_result_string(r));
        return false;
    }
    a->layouts=*next;a->layouts_error[0]=0;a->layouts_writable=true;return true;
}
static void layout_store(app *a) {
    ps_layout_catalog next=a->layouts;ps_layout_state state=layout_capture(a);
    ps_result r=ps_layout_catalog_put(&next,a->layout_name,&state);
    if(r!=PS_OK) {
        snprintf(a->layouts_error,sizeof a->layouts_error,"%s",r==PS_LIMIT?
            "Alle acht Plätze sind belegt. Wähle einen vorhandenen Namen zum Ersetzen.":
            "Bitte einen Namen mit bis zu 63 UTF-8-Bytes ohne Rand-Leerzeichen oder Steuerzeichen eingeben.");
        return;
    }
    if(layouts_save(a,&next))
        for(unsigned i=0;i<next.count;i++)if(!strcmp(next.entries[i].name,a->layout_name))a->layout_selected=(int)i;
}
static void layout_apply(app *a) {
    const ps_layout_state *s=&a->layouts.entries[a->layout_selected].state;
    a->preferences.dock=s->dock;a->preferences.sidebar_width=s->sidebar_width;
    a->preferences.inspector_width=s->inspector_width;a->preferences.log_height=s->log_height;
    a->show_log=s->show_log;
    a->preferences.view_flags=s->show_log?a->preferences.view_flags|PS_VIEW_LOG:a->preferences.view_flags&~PS_VIEW_LOG;
    a->dock_resize=a->dock_float_resize=a->dock_drag=a->panel_drag=0;
    a->dock_dragging=false;a->dock_last_tab=a->tab;
    if(a->tab==6) {
        a->settings_draft.dock=s->dock;a->settings_draft.sidebar_width=s->sidebar_width;
        a->settings_draft.inspector_width=s->inspector_width;a->settings_draft.log_height=s->log_height;
        a->settings_draft.view_flags=s->show_log?a->settings_draft.view_flags|PS_VIEW_LOG:a->settings_draft.view_flags&~PS_VIEW_LOG;
    }
    a->layouts_error[0]=0;
}
static void layout_manager_view(app *a,int width,int height) {
    struct nk_context *ui=a->ui;
    nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(0,5));
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(0,0));
    if(nk_begin(ui,"layouts-header",nk_rect(0,0,(float)width,38),NK_WINDOW_NO_SCROLLBAR)) {
        nk_layout_row_begin(ui,NK_STATIC,28,4);nk_layout_row_push(ui,(float)width-138);
        a->window_drag_bounds=nk_widget_bounds(ui);nk_label(ui,"   Physim  ·  Panelanordnungen",NK_TEXT_LEFT);
        for(unsigned i=0;i<3;i++){nk_layout_row_push(ui,46);window_control(a,i);}
        nk_layout_row_end(ui);
    }
    nk_end(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(8,4));
    if(nk_begin(ui,"layouts-manager",nk_rect(((float)width-700)/2,((float)height-660)/2,700,660),NK_WINDOW_NO_SCROLLBAR)) {
        nk_style_push_font(ui,a->font_title);nk_layout_row_dynamic(ui,38,1);
        nk_label(ui,"Panelanordnungen",NK_TEXT_LEFT);nk_style_pop_font(ui);
        nk_layout_row_dynamic(ui,36,1);
        nk_label_wrap(ui,"Speichere bis zu acht Anordnungen. Ein vorhandener Name wird beim Speichern ersetzt.");
        nk_layout_row_dynamic(ui,24,1);
        nk_label_colored(ui,"Gespeicherte Anordnungen",NK_TEXT_LEFT,muted());
        nk_layout_row_dynamic(ui,24,1);
        for(unsigned i=0;i<PS_LAYOUT_MAX;i++) {
            a->layout_entries[i]=nk_widget_bounds(ui);
            if(i<a->layouts.count) {
                nk_bool chosen=a->layout_selected==(int)i;
                if(nk_selectable_label(ui,a->layouts.entries[i].name,NK_TEXT_LEFT,&chosen)) {
                    a->layout_selected=(int)i;
                    snprintf(a->layout_name,sizeof a->layout_name,"%s",a->layouts.entries[i].name);
                }
            } else nk_label_colored(ui,i==0?"Noch keine Anordnung gespeichert.":"",NK_TEXT_LEFT,muted());
        }
        nk_layout_row_dynamic(ui,24,1);nk_label(ui,"Name",NK_TEXT_LEFT);
        nk_layout_row_dynamic(ui,32,1);a->layout_bounds[0]=nk_widget_bounds(ui);
        nk_edit_string_zero_terminated(ui,NK_EDIT_FIELD,a->layout_name,sizeof a->layout_name,nk_filter_default);
        bool selected=a->layouts_writable && a->layout_selected>=0 && (unsigned)a->layout_selected<a->layouts.count;
        nk_layout_row_dynamic(ui,36,3);
        a->layout_bounds[1]=nk_widget_bounds(ui);
        if(action_button(ui,"Anordnung speichern",a->layouts_writable))layout_store(a);
        a->layout_bounds[2]=nk_widget_bounds(ui);
        if(action_button(ui,"Anwenden",selected))layout_apply(a);
        a->layout_bounds[3]=nk_widget_bounds(ui);
        if(action_button(ui,"Löschen",selected)) {
            ps_layout_catalog next=a->layouts;ps_layout_catalog_remove(&next,(unsigned)a->layout_selected);
            if(layouts_save(a,&next)){a->layout_selected=-1;a->layout_name[0]=0;}
        }
        nk_layout_row_dynamic(ui,40,1);nk_label_wrap(ui,a->layouts_error);
        nk_layout_row_dynamic(ui,36,2);a->layout_bounds[4]=nk_widget_bounds(ui);
        if(!a->layouts_writable && *a->layouts_path) {
            if(nk_button_label(ui,"Datei bewusst zurücksetzen")) {
                ps_layout_catalog empty={0};
                if(layouts_save(a,&empty)){a->layout_selected=-1;a->layout_name[0]=0;}
            }
        } else nk_label(ui,"",NK_TEXT_LEFT);
        a->layout_bounds[5]=nk_widget_bounds(ui);
        if(nk_button_label(ui,"Schließen · Esc"))a->layout_manager=false;
    }
    nk_end(ui);nk_style_pop_vec2(ui);
}
