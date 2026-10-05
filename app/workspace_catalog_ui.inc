static void workspace_catalog_view(app *a,int width,int height) {
    struct nk_context *ui=a->ui;
    nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(0,5));
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(0,0));
    if(nk_begin(ui,"workspaces-header",nk_rect(0,0,(float)width,38),NK_WINDOW_NO_SCROLLBAR)) {
        nk_layout_row_begin(ui,NK_STATIC,28,4);nk_layout_row_push(ui,(float)width-138);
        a->window_drag_bounds=nk_widget_bounds(ui);nk_label(ui,"   Physim  ·  Workspaces",NK_TEXT_LEFT);
        for(unsigned i=0;i<3;i++){nk_layout_row_push(ui,46);window_control(a,i);}nk_layout_row_end(ui);
    }
    nk_end(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(8,4));
    if(nk_begin(ui,"workspaces-manager",nk_rect(((float)width-700)/2,((float)height-660)/2,700,660),NK_WINDOW_NO_SCROLLBAR)) {
        nk_style_push_font(ui,a->font_title);nk_layout_row_dynamic(ui,32,1);nk_label(ui,"Workspaces",NK_TEXT_LEFT);nk_style_pop_font(ui);
        nk_layout_row_dynamic(ui,32,1);nk_label_wrap(ui,"Bis zu acht Workspaces. Ein vorhandener Name wird beim Speichern ersetzt.");
        nk_layout_row_dynamic(ui,22,1);nk_label_colored(ui,"Gespeicherte Workspaces",NK_TEXT_LEFT,muted());
        nk_layout_row_dynamic(ui,22,1);
        for(unsigned i=0;i<PS_WORKSPACE_MAX;i++) {
            a->workspace_catalog_entries[i]=nk_widget_bounds(ui);
            if(a->workspaces && i<a->workspaces->count) {
                nk_bool chosen=a->workspace_selected==(int)i;
                if(nk_selectable_label(ui,a->workspaces->entries[i].name,NK_TEXT_LEFT,&chosen)) {
                    a->workspace_selected=(int)i;snprintf(a->workspace_name,sizeof a->workspace_name,"%s",a->workspaces->entries[i].name);
                }
            } else nk_label_colored(ui,i==0?"Noch kein Workspace gespeichert.":"",NK_TEXT_LEFT,muted());
        }
        bool selected=a->workspaces_writable && a->workspaces && a->workspace_selected>=0 && (unsigned)a->workspace_selected<a->workspaces->count;
        nk_layout_row_dynamic(ui,32,1);
        const char *root=selected?a->workspaces->entries[a->workspace_selected].state.root:a->workspace_open?a->workspace:NULL;
        struct nk_rect root_bounds=nk_widget_bounds(ui);
        if(root) {
            const char *tail=root;size_t remaining=strlen(root);
            const struct nk_user_font *font=ui->style.font;
            float available=root_bounds.w-font->width(font->userdata,font->height,"…",3);
            while(remaining && font->width(font->userdata,font->height,tail,(int)remaining)>available) {
                nk_rune rune;int step=nk_utf_decode(tail,&rune,(int)remaining);if(step<=0)step=1;
                tail+=step;remaining-=(size_t)step;
            }
            char display[PS_WORKSPACE_PATH+4];snprintf(display,sizeof display,"%s%s",tail==root?"":"…",tail);
            nk_label(ui,display,NK_TEXT_LEFT);
            if(nk_input_is_mouse_hovering_rect(&ui->input,root_bounds))nk_tooltip(ui,root);
        } else nk_label(ui,"Öffne einen Ordner, um einen Workspace zu speichern.",NK_TEXT_LEFT);
        nk_layout_row_dynamic(ui,20,1);nk_label(ui,"Name",NK_TEXT_LEFT);
        nk_layout_row_dynamic(ui,30,1);a->workspace_catalog_bounds[0]=nk_widget_bounds(ui);
        nk_edit_string_zero_terminated(ui,NK_EDIT_FIELD,a->workspace_name,sizeof a->workspace_name,nk_filter_default);
        nk_layout_row_dynamic(ui,34,3);a->workspace_catalog_bounds[1]=nk_widget_bounds(ui);
        if(action_button(ui,"Workspace speichern",a->workspaces_writable && a->workspace_open))workspace_catalog_store(a);
        a->workspace_catalog_bounds[2]=nk_widget_bounds(ui);
        bool can_open=selected && idle(a) && !a->library_thread && !a->recovery && !a->dialog_pending && !a->document_pending;
        for(unsigned i=0;i<a->document_count;i++)if(a->documents[i].recovery)can_open=false;
        if(action_button(ui,"Öffnen",can_open))workspace_catalog_open(a);
        a->workspace_catalog_bounds[3]=nk_widget_bounds(ui);if(action_button(ui,"Löschen",selected))workspace_catalog_delete(a);
        nk_layout_row_dynamic(ui,40,1);
        nk_label_wrap(ui,*a->workspaces_error?a->workspaces_error:selected && !can_open?"Zum Wechseln zuerst den laufenden Job oder offenen Dialog beenden und Wiederherstellungen klären.":"Öffnen speichert aktuelle Änderungen. Speicherfehler verhindern den Wechsel. Build und Simulation starten danach auf deinen Wunsch.");
        nk_layout_row_dynamic(ui,34,2);a->workspace_catalog_bounds[4]=nk_widget_bounds(ui);
        if(!a->workspaces_writable && *a->workspaces_path) {
            if(nk_button_label(ui,"Datei bewusst zurücksetzen"))workspace_catalog_reset(a);
        } else nk_label(ui,"",NK_TEXT_LEFT);
        a->workspace_catalog_bounds[5]=nk_widget_bounds(ui);if(nk_button_label(ui,"Schließen · Esc"))a->workspace_manager=false;
    }
    nk_end(ui);nk_style_pop_vec2(ui);
}
