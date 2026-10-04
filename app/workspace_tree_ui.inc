static void workspace_tree_view(app *a) {
    struct nk_context *ui = a->ui;
    nk_layout_row_dynamic(ui, 28, 1);
    if (nk_button_label(ui, "Dateibaum aktualisieren"))
        refresh_workspace_entries(a);
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
        float indent = fminf((float)entry->depth * .05f, .3f);
        nk_layout_row_begin(ui, NK_DYNAMIC, 27, entry->depth ? 2 : 1);
        if (entry->depth) {
            nk_layout_row_push(ui, indent);
            nk_spacer(ui);
        }
        nk_layout_row_push(ui, 1.f - indent);
        struct nk_rect bounds = nk_widget_bounds(ui);
        if (!strcmp(entry->path, a->workspace_tree_test_path))
            a->workspace_tree_test_bounds = bounds;
        char label[4200];
        snprintf(label, sizeof label, "%s%s", entry->missing ? "? " : "   ",
                 workspace_name(entry->path));
        nk_style_push_flags(ui, &ui->style.button.text_alignment, NK_TEXT_LEFT);
        bool selected = !strcmp(entry->path, a->workspace_preview_path);
        bool clicked = action_button(ui, label, selected);
        nk_style_pop_flags(ui);
        nk_layout_row_end(ui);
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
