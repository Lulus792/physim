static void document_list_view(app *a) {
    if (!a->document_count)
        return;
    struct nk_context *ui = a->ui;
    nk_layout_row_dynamic(ui, 24, 1);
    nk_label_colored(ui, "Geöffnete Dokumente", NK_TEXT_LEFT, muted());
    for (unsigned i = 0; i < a->document_count; i++) {
        workspace_document *doc = &a->documents[i];
        char label[4200];
        snprintf(label, sizeof label, "%s%s", workspace_name(doc->path), doc->dirty ? " *" : "");
        nk_layout_row_dynamic(ui, 26, 1);
        struct nk_rect bounds = nk_widget_bounds(ui);
        a->document_list_bounds[i] = bounds;
        if (action_button(ui, label, a->tab == 8 && a->document_active == i))
            document_select(a, i);
        if (nk_input_is_mouse_hovering_rect(&ui->input, bounds))
            nk_tooltip(ui, doc->path);
    }
}
static void document_view(app *a, float content) {
    struct nk_context *ui = a->ui;
    workspace_document *doc = active_document(a);
    nk_layout_row_begin(ui, NK_DYNAMIC, 28, 4);
    nk_layout_row_push(ui, .4f);
    char label[4200];
    snprintf(label, sizeof label, "%s%s", workspace_name(doc->path), doc->dirty ? " *" : "");
    if (nk_combo_begin_label(ui, label, nk_vec2(420, 300))) {
        nk_layout_row_dynamic(ui, 26, 1);
        for (unsigned i = 0; i < a->document_count; i++) {
            snprintf(label, sizeof label, "%s%s", a->documents[i].path,
                     a->documents[i].dirty ? " *" : "");
            if (nk_combo_item_label(ui, label, NK_TEXT_LEFT))
                document_select(a, i);
        }
        nk_combo_end(ui);
    }
    doc = active_document(a);
    nk_begin_disabled(ui, doc->recovery != NULL);
    nk_layout_row_push(ui, .2f);
    a->document_bounds[0] = nk_widget_bounds(ui);
    if (nk_button_label(ui, "Speichern"))
        document_save(a, a->document_active);
    nk_layout_row_push(ui, .2f);
    a->document_bounds[1] = nk_widget_bounds(ui);
    if (nk_button_label(ui, "Neu laden"))
        document_request(a, 2);
    nk_end_disabled(ui);
    nk_layout_row_push(ui, .2f);
    a->document_bounds[2] = nk_widget_bounds(ui);
    bool close = nk_button_label(ui, "Schließen");
    nk_layout_row_end(ui);
    if (close) {
        document_request(a, 1);
        return;
    }
    nk_layout_row_dynamic(ui, 22, 1);
    nk_label_colored(ui, doc->path, NK_TEXT_LEFT, muted());
    float used = 158;
    if (doc->recovery) {
        nk_layout_row_dynamic(ui, 72, 1);
        nk_label_wrap(
            ui,
            doc->recovery_conflict
                ? "Ungespeicherter Entwurf gefunden. Die Datei wurde extern geändert. "
                  "Entwurf laden übernimmt die Sicherung in den Editor; beim nächsten "
                  "Speichern wird der aktuelle Dateistand ersetzt. Schließen behält die Sicherung."
                : "Ungespeicherter Entwurf gefunden. Entwurf laden übernimmt die "
                  "Sicherung in den Editor. Die Datei bleibt bis zum Speichern "
                  "unverändert. Schließen behält die Sicherung für später.");
        nk_layout_row_dynamic(ui, 28, 2);
        a->document_bounds[7] = nk_widget_bounds(ui);
        if (nk_button_label(ui, "Entwurf laden"))
            document_resolve_recovery(a, true);
        a->document_bounds[8] = nk_widget_bounds(ui);
        if (nk_button_label(ui, "Sicherung verwerfen"))
            document_resolve_recovery(a, false);
        used += 112;
    }
    if (a->document_pending) {
        nk_layout_row_dynamic(ui, 44, 1);
        nk_label_wrap(
            ui, a->document_pending == 1
                    ? "Ungespeicherte Änderungen: vor dem Schließen speichern?"
                    : "Neu laden ersetzt deine ungespeicherten Änderungen durch den Dateiinhalt.");
        nk_layout_row_dynamic(ui, 28, 3);
        a->document_bounds[3] = nk_widget_bounds(ui);
        bool save = nk_button_label(ui, "Speichern");
        a->document_bounds[4] = nk_widget_bounds(ui);
        bool discard = nk_button_label(ui, "Änderungen verwerfen");
        a->document_bounds[5] = nk_widget_bounds(ui);
        if (nk_button_label(ui, "Abbrechen"))
            a->document_pending = 0;
        if (save || discard) {
            int action = a->document_pending;
            if (discard || document_save(a, a->document_active)) {
                if (action == 1) {
                    if (discard && !doc->draft_blocked && !document_draft_discard(a, doc, false))
                        return;
                    document_remove(a);
                    return;
                }
                document_reload(a);
            }
        }
        used += 84;
    }
    if (doc->error[0]) {
        nk_layout_row_dynamic(ui, 50, 1);
        nk_label_wrap(ui, doc->error);
        used += 56;
    }
    if (doc->draft_error[0]) {
        nk_layout_row_dynamic(ui, 50, 1);
        nk_label_wrap(ui, doc->draft_error);
        used += 56;
        if (doc->draft_blocked) {
            nk_layout_row_dynamic(ui, 28, 1);
            a->document_bounds[9] = nk_widget_bounds(ui);
            if (nk_button_label(ui, "Unlesbare Sicherung verwerfen"))
                document_draft_discard(a, doc, true);
            used += 34;
        }
    }
    if (a->show_search && !doc->recovery) {
        nk_layout_row_dynamic(ui, 28, 4);
        nk_edit_string_zero_terminated(ui, NK_EDIT_FIELD, a->find, sizeof a->find,
                                       nk_filter_default);
        if (nk_button_label(ui, "Suchen"))
            find_text(a, &doc->edit, false);
        nk_edit_string_zero_terminated(ui, NK_EDIT_FIELD, a->replace, sizeof a->replace,
                                       nk_filter_default);
        if (nk_button_label(ui, "Ersetzen"))
            find_text(a, &doc->edit, true);
        used += 34;
    }
    nk_begin_disabled(ui, a->document_pending != 0 || doc->recovery != NULL);
    editor(a, &doc->edit, fmaxf(100, content - used));
    nk_end_disabled(ui);
    nk_layout_row_dynamic(ui, 22, 1);
    char autosave[96];
    snprintf(autosave, sizeof autosave, "Autosave alle %u Sekunden",
             a->preferences.autosave_seconds);
    nk_label_colored(ui,
                     doc->recovery        ? "Wiederherstellung ausstehend"
                     : doc->draft_blocked ? "Autosave pausiert"
                     : doc->draft_present ? "Automatische Sicherung vorhanden"
                                          : autosave,
                     NK_TEXT_LEFT, muted());
    nk_layout_row_dynamic(ui, 22, 1);
    nk_label_colored(ui, "UTF-8  ·  Ctrl+S speichern  ·  Ctrl+F suchen  ·  Ctrl+W schließen",
                     NK_TEXT_LEFT, muted());
}
