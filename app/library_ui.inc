static void library_view(app *a, float height) {
    struct nk_context *ui = a->ui;
    nk_layout_row_dynamic(ui, 34, 2);
    nk_style_push_font(ui, a->font_title);
    nk_label(ui, "Läufe & Berichte", NK_TEXT_LEFT);
    nk_style_pop_font(ui);
    if (nk_button_label(ui, "Zur Auswertung")) {
        a->tab = 2;
        a->analysis_tab = 0;
    }
    nk_layout_row_dynamic(ui, 32, 3);
    if (action_button(ui, "Messläufe", a->library_kind == 0))
        a->library_kind = 0;
    if (action_button(ui, "Analyseberichte", a->library_kind == 1))
        a->library_kind = 1;
    nk_begin_disabled(ui, a->library_thread != NULL);
    if (nk_button_label(ui, "Aktualisieren"))
        refresh_library(a);
    nk_end_disabled(ui);
    nk_layout_row_dynamic(ui, 30, 1);
    if (a->library_search_focus) {
        nk_edit_focus(ui, NK_EDIT_ALWAYS_INSERT_MODE);
        a->library_search_focus = false;
    }
    struct nk_rect search = nk_widget_bounds(ui);
    nk_flags flags = nk_edit_string_zero_terminated(ui, NK_EDIT_FIELD, a->library_query,
                                                    sizeof a->library_query, nk_filter_default);
    a->library_search_active = (flags & NK_EDIT_ACTIVE) != 0;
    if (!a->library_query[0] && !a->library_search_active)
        report_text(a, nk_rect(search.x + 10, search.y + 7, search.w - 20, 22),
                    "Dateiname filtern …", nk_rgb(166, 169, 177));
    nk_layout_row_dynamic(ui, 28, 1);
    char detail[224];
    if (a->library_error[0])
        snprintf(detail, sizeof detail, "%s", a->library_error);
    else if (a->library_thread)
        snprintf(detail, sizeof detail, "Dateiliste wird im Hintergrund aktualisiert …");
    else if (a->library && a->library->limited)
        snprintf(detail, sizeof detail,
                 "Liste auf %u Dateien begrenzt; weitere Dateien sind nicht enthalten.",
                 PS_LIBRARY_MAX_ITEMS);
    else if (a->library && a->library->skipped)
        snprintf(detail, sizeof detail, "%zu Dateien · %zu Einträge konnten nicht gelesen werden.",
                 a->library->count, a->library->skipped);
    else
        snprintf(detail, sizeof detail, "%zu Dateien im Projekt · neueste zuerst · Ctrl+F filtert",
                 a->library ? a->library->count : 0);
    nk_label_colored(ui, detail, NK_TEXT_LEFT, nk_rgb(166, 169, 177));
    nk_layout_row_dynamic(ui, 34, 3);
    snprintf(detail, sizeof detail, "%u von 8 Läufen ausgewählt", a->selected_count);
    nk_label(ui, detail, NK_TEXT_LEFT);
    nk_begin_disabled(ui, !idle(a) || !a->selected_count);
    if (nk_button_label(ui, "Auswahl leeren"))
        a->selected_count = 0;
    a->library_analyze_bounds = nk_widget_bounds(ui);
    if (action_button(ui, a->selected_count > 1 ? "Auswahl vergleichen" : "Auswahl analysieren",
                      true)) {
        a->tab = 2;
        a->analysis_tab = 0;
        analyze(a, false);
    }
    nk_end_disabled(ui);
    a->library_visible_count = 0;
    nk_layout_row_dynamic(ui, fmaxf(120, height - 230), 1);
    if (nk_group_begin(ui, "library-files", 0)) {
        size_t shown = 0;
        if (a->library)
            for (size_t i = 0; i < a->library->count; i++) {
                const ps_library_item *item = &a->library->items[i];
                if ((int)item->kind != a->library_kind ||
                    (a->library_query[0] && !ps_document_find(item->name, a->library_query)))
                    continue;
                char path[4096];
                if (ps_library_path(a->library, i, path, sizeof path) != PS_OK)
                    continue;
                shown++;
                nk_layout_row_begin(ui, NK_DYNAMIC, 34, 3);
                nk_layout_row_push(ui, .06f);
                struct nk_rect select_bounds = nk_widget_bounds(ui);
                nk_begin_disabled(ui, !idle(a));
                if (item->kind == PS_LIBRARY_RUN) {
                    bool selected = selected_run(a, path) >= 0;
                    if (nk_checkbox_label(ui, "", &selected))
                        select_run(a, path, selected);
                } else
                    nk_label_colored(ui, "•", NK_TEXT_CENTERED, nk_rgb(166, 169, 177));
                nk_end_disabled(ui);
                nk_layout_row_push(ui, .75f);
                struct nk_rect name_bounds = nk_widget_bounds(ui);
                int selection = selected_run(a, path);
                char numbered_name[1100];
                if (selection >= 0)
                    snprintf(numbered_name, sizeof numbered_name, "%d · %s%s", selection + 1,
                             selection == 0 ? "Referenz · " : "", item->name);
                else
                    snprintf(numbered_name, sizeof numbered_name, "%s", item->name);
                nk_label(ui, numbered_name, NK_TEXT_LEFT);
                if (nk_input_is_mouse_hovering_rect(&ui->input, name_bounds))
                    nk_tooltip(ui, numbered_name);
                nk_layout_row_push(ui, .19f);
                struct nk_rect open_bounds = nk_widget_bounds(ui);
                nk_begin_disabled(ui, !idle(a));
                if (nk_button_label(ui, item->kind == PS_LIBRARY_RUN ? "Ansehen" : "Öffnen"))
                    open_library_item(a, i);
                nk_end_disabled(ui);
                nk_layout_row_end(ui);
                if (a->library_visible_count < 8 &&
                    open_bounds.y >= nk_window_get_content_region(ui).y &&
                    open_bounds.y + open_bounds.h <=
                        nk_window_get_content_region(ui).y + nk_window_get_content_region(ui).h) {
                    unsigned at = a->library_visible_count++;
                    a->library_visible_indices[at] = i;
                    a->library_open_bounds[at] = open_bounds;
                    a->library_select_bounds[at] = select_bounds;
                }
                time_t seconds = (time_t)(item->modified_ns / INT64_C(1000000000));
                struct tm *local = localtime(&seconds);
                char date[64] = "Datum unbekannt";
                if (local)
                    strftime(date, sizeof date, "%d.%m.%Y · %H:%M:%S", local);
                snprintf(detail, sizeof detail, "%s · %.1f KiB", date, (double)item->bytes / 1024);
                nk_layout_row_dynamic(ui, 22, 1);
                nk_label_colored(ui, detail, NK_TEXT_LEFT, nk_rgb(166, 169, 177));
            }
        if (!shown) {
            nk_layout_row_dynamic(ui, 70, 1);
            nk_label_wrap(
                ui, a->library_query[0] ? "Keine passende Datei gefunden."
                    : a->library_kind
                        ? "Noch keine Berichte. Starte eine Analyse, um Ergebnisse zu speichern."
                        : "Noch keine Läufe. Zeichne zuerst eine Simulation auf.");
        }
        nk_group_end(ui);
    }
}
