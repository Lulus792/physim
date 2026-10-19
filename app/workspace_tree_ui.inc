static void workspace_tree_view(app *a) {
    struct nk_context *ui = a->ui;
    if(!workbench_visible(a)) {
    nk_layout_row_dynamic(ui, 28, 1);
    if (nk_button_label(ui, "Dateibaum aktualisieren"))
        refresh_workspace_entries(a);
    }
    if (a->workspace_tree_error[0]) {
        nk_layout_row_dynamic(ui, 64, 1);
        nk_label_wrap(ui, a->workspace_tree_error);
    }
    a->workspace_tree_test_bounds = nk_rect(0, 0, 0, 0);
    size_t clicked_index = SIZE_MAX;
    for (size_t i = 0; i < a->workspace_tree.count; i++) {
        const ps_workspace_entry *entry = &a->workspace_tree.entries[i];
        if (!entry->depth && entry->root) {
            nk_layout_row_dynamic(ui, 22, 1);
            nk_label_colored(ui, "Hinzugefügt", NK_TEXT_LEFT, muted());
        }
        float width=nk_window_get_content_region(ui).w;
        float indent = fminf((float)entry->depth * 16, fmaxf(0,width-64));
        nk_layout_row_begin(ui, NK_STATIC, 27, entry->depth ? 2 : 1);
        if (entry->depth) {
            nk_layout_row_push(ui, indent);
            nk_spacer(ui);
        }
        nk_layout_row_push(ui, fmaxf(24,width-indent-(entry->depth?ui->style.window.spacing.x:0)));
        struct nk_rect bounds = nk_widget_bounds(ui);
        if (!strcmp(entry->path, a->workspace_tree_test_path))
            a->workspace_tree_test_bounds = bounds;
        char label[4200];
        snprintf(label, sizeof label, entry->directory?"        %s":"      %s",
                 workspace_name(entry->path));
        nk_style_push_flags(ui, &ui->style.button.text_alignment, NK_TEXT_LEFT);
        bool selected = !strcmp(entry->path, a->workspace_preview_path);
        bool clicked;
        if(workbench_visible(a)) {
            char active[4096]={0};workspace_document *doc=active_document(a);
            if(a->tab==8 && doc)snprintf(active,sizeof active,"%s",doc->path);
            else if(a->loaded && a->tab==0)join(active,sizeof active,a->project,a->analysis_only?analysis_source(a):experiment_source(a));
            selected=!strcmp(entry->path,active);
            nk_widget(&bounds,ui);struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
            if(selected || nk_input_is_mouse_hovering_rect(&ui->input,bounds))nk_fill_rect(canvas,bounds,selected?0:4,selected?UI_COLOR(selection_active):UI_COLOR(button_hover));
            float inset=entry->directory?38:28;
            const char *name=workspace_name(entry->path);
            nk_draw_text(canvas,nk_rect(bounds.x+inset,bounds.y+(bounds.h-ui->style.font->height)/2,fmaxf(1,bounds.w-inset-3),ui->style.font->height),name,(int)strlen(name),ui->style.font,nk_rgba(0,0,0,0),selected?UI_COLOR(text):UI_COLOR(muted));
            clicked=ps_ui_button_at(ui,bounds,name,selected);
        } else clicked=action_button(ui,label,selected);
        nk_style_pop_flags(ui);
        nk_layout_row_end(ui);
        struct nk_command_buffer *guides=nk_window_get_canvas(ui);
        for(unsigned depth=0;depth<entry->depth;depth++) {
            float gx=bounds.x-indent+(float)depth*16+8;
            nk_stroke_line(guides,gx,bounds.y,gx,bounds.y+bounds.h,1,UI_COLOR(divider));
        }
        ps_ui_draw_icon(nk_window_get_canvas(ui),entry->missing?PS_ICON_ERROR:entry->directory?PS_ICON_FOLDER:PS_ICON_FILE,
                        nk_rect(bounds.x+(entry->directory?16:4),bounds.y,20,bounds.h),entry->missing?UI_COLOR(error):UI_COLOR(muted));
        if (entry->directory) {
            struct nk_command_buffer *canvas = nk_window_get_canvas(ui);
            float x = bounds.x + 8, y = bounds.y + bounds.h * .5f;
            if (entry->expanded)
                nk_fill_triangle(canvas, x, y - 3, x + 8, y - 3, x + 4, y + 3, UI_COLOR(text));
            else
                nk_fill_triangle(canvas, x, y - 4, x, y + 4, x + 6, y, UI_COLOR(text));
        }
        if (nk_input_is_mouse_hovering_rect(&ui->input, bounds))
            nk_tooltip(ui, entry->path);
        if (clicked)
            clicked_index = i;
        if (entry->error != PS_OK) {
            nk_layout_row_dynamic(ui, 44, 1);
            nk_label_wrap(
                ui,
                entry->error == PS_LIMIT
                    ? "Anzeigegrenze erreicht. Andere Ordner zuklappen und erneut öffnen."
                : entry->missing
                    ? "Pfad nicht erreichbar."
                    : "Ordnerinhalt nicht vollständig lesbar. Erneut öffnen oder aktualisieren.");
        } else if (entry->expanded && (i + 1 == a->workspace_tree.count ||
                                       a->workspace_tree.entries[i + 1].depth <= entry->depth)) {
            nk_layout_row_dynamic(ui, 22, 1);
            nk_label_colored(ui, "Ordner ist leer", NK_TEXT_LEFT, muted());
        }
    }
    /* Finish this frame's layout before a mutation replaces paths and indices. */
    if (clicked_index != SIZE_MAX) {
        const ps_workspace_entry *entry = &a->workspace_tree.entries[clicked_index];
        if (entry->directory) {
            ps_result r = ps_workspace_tree_toggle(&a->workspace_tree, clicked_index);
            if (r != PS_OK)
                status(a, "Ordner nicht vollständig geladen. Hinweis im Dateibaum beachten.");
        } else
            preview_workspace_path(a, entry->path);
    }
}
