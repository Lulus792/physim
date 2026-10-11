static void report_details_clear(app *a) {
    free(a->report_details_text);
    free(a->report_details_display);
    a->report_details_text = NULL;
    a->report_details_display = NULL;
    a->report_details_length = 0;
    a->report_details_display_length = 0;
    a->report_details_path[0] = a->report_details_error[0] = 0;
    a->report_details_tab = 0;
    a->report_details_loaded = -1;
    a->report_details_ready = false;
    a->report_details_active = false;
}
static bool report_details_sidecar(const app *a, const char *suffix, char path[4096]) {
    size_t length = strlen(a->loaded_report_path), extension = strlen(".psreport");
    if (length < extension || SDL_strcasecmp(a->loaded_report_path + length - extension, ".psreport"))
        return false;
    int n = snprintf(path, 4096, "%.*s%s", (int)(length - extension),
                     a->loaded_report_path, suffix);
    return n >= 0 && n < 4096;
}
static void report_details_load(app *a, const char *provenance) {
    if (a->report_details_ready && a->report_details_loaded == a->report_details_tab)
        return;
    free(a->report_details_text);
    free(a->report_details_display);
    a->report_details_text = NULL;
    a->report_details_display = NULL;
    a->report_details_length = 0;
    a->report_details_display_length = 0;
    a->report_details_path[0] = a->report_details_error[0] = 0;
    a->report_details_loaded = a->report_details_tab;
    a->report_details_ready = true;
    if (!a->report_details_tab) {
        size_t length = strlen(provenance);
        a->report_details_text = malloc(length + 1);
        if (a->report_details_text) {
            memcpy(a->report_details_text, provenance, length + 1);
            a->report_details_length = length;
        } else strcpy(a->report_details_error, "Herkunftsangaben konnten nicht geladen werden.");
        return;
    }
    /* Resolve only companions of the opened report. Provenance text is never
     * interpreted as a path, command or link. Archived sources remain read-only. */
    const char *suffix = a->report_details_tab == 1 ? ".source.c"
        : a->report_details_tab == 2 ? ".inputs.csv" : ".build.txt";
    if (!report_details_sidecar(a, suffix, a->report_details_path)) {
        strcpy(a->report_details_error, "Zu diesem Bericht ist kein Begleitdateipfad verfügbar.");
        return;
    }
    if (a->report_details_tab == 1 && !exists(a->report_details_path))
        (void)report_details_sidecar(a, ".source.phys", a->report_details_path);
    a->report_details_text = load_utf8_text(a->report_details_path, &a->report_details_length);
    if (!a->report_details_text)
        strcpy(a->report_details_error,
               "Begleitdatei fehlt, ist unlesbar, kein gültiger UTF-8-Text oder größer als 256 KiB.");
}
static void report_details_view(app *a, const char *provenance, float height) {
    struct nk_context *ui = a->ui;
    a->report_details_active = false;
    const char *tabs[] = {"Herkunft", "Quellkopie", "Eingänge", "Build"};
    nk_layout_row_dynamic(ui, 34, a->preferences.ui_size >= 20 ? 2 : 4);
    for (int i = 0; i < 4; i++) {
        a->report_details_tabs[i] = nk_widget_bounds(ui);
        if (action_button(ui, tabs[i], a->report_details_tab == i))
            a->report_details_tab = i;
    }
    report_details_load(a, provenance);
    if (a->report_details_text && !a->report_details_display) {
        a->report_details_display = malloc(a->report_details_length + 1);
        if (a->report_details_display) {
            size_t written = 0;
            for (size_t i = 0; i < a->report_details_length; i++) {
                char ch = a->report_details_text[i];
                if (ch == '\r' && i + 1 < a->report_details_length && a->report_details_text[i + 1] == '\n') continue;
                a->report_details_display[written++] = ch == '\r' ? '\n' : ch;
            }
            a->report_details_display[written] = 0;
            a->report_details_display_length = written;
        } else strcpy(a->report_details_error, "Textansicht konnte nicht vorbereitet werden.");
    }
    nk_layout_row_dynamic(ui, 30, 2);
    nk_label_colored(ui, "Nur lesen", NK_TEXT_LEFT, UI_COLOR(muted));
    a->report_details_copy_bounds = nk_widget_bounds(ui);
    nk_begin_disabled(ui, !a->report_details_text || !a->report_details_length);
    if (nk_button_label(ui, "Kopieren")) {
        if (SDL_SetClipboardText(a->report_details_text))
            status(a, "Vollständigen Text kopiert.");
        else status(a, "Text konnte nicht kopiert werden.");
    }
    nk_end_disabled(ui);
    if (a->report_details_path[0]) {
        nk_layout_row_dynamic(ui, 26, 1);
        struct nk_rect path_bounds = nk_widget_bounds(ui);
        nk_label(ui, diagnostic_basename(a->report_details_path), NK_TEXT_LEFT);
        if (nk_input_is_mouse_hovering_rect(&ui->input, path_bounds))
            nk_tooltip(ui, a->report_details_path);
    }
    if (a->report_details_error[0]) {
        nk_layout_row_dynamic(ui, 72, 1);
        nk_label_wrap(ui, a->report_details_error);
        return;
    }
    if (!a->report_details_length) {
        nk_layout_row_dynamic(ui, 40, 1);
        nk_label(ui, "Keine Herkunftsangaben gespeichert.", NK_TEXT_LEFT);
        return;
    }
    (void)height;
    nk_layout_row_dynamic(ui, 1, 1);
    struct nk_rect next = nk_widget_bounds(ui), region = nk_window_get_content_region(ui);
    nk_uint outer_x = 0, outer_y = 0;
    nk_window_get_scroll(ui, &outer_x, &outer_y);
    nk_layout_row_dynamic(ui, fmaxf(100, region.y + region.h - next.y - (float)outer_y - 14), 1);
    a->report_details_text_bounds = nk_widget_bounds(ui);
    nk_style_push_font(ui, a->font_code);
    nk_flags flags = nk_edit_string_zero_terminated(ui, NK_EDIT_BOX | NK_EDIT_READ_ONLY,
                                   a->report_details_display, (int)a->report_details_display_length + 1,
                                   nk_filter_default);
    a->report_details_active = (flags & NK_EDIT_ACTIVE) != 0;
    nk_style_pop_font(ui);
}
