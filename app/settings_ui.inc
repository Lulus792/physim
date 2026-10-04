/* App-owned presentation state, separate from project sources and run metadata. */
static void preferences_apply(app *a, const ps_preferences *p) {
    a->preferences = *p;
    ui_palette = p->theme == PS_THEME_LIGHT ? &PS_UI_LIGHT :
                 p->theme == PS_THEME_HIGH_CONTRAST ? &PS_UI_HIGH_CONTRAST : &PS_UI_DARK;
    style(a->ui);
    nk_style_set_font(a->ui, a->font_ui);
    ps_graphics_background(a->graphics, UI_COLOR(canvas).r, UI_COLOR(canvas).g, UI_COLOR(canvas).b);
    if (a->doc_ui) {
        style(a->doc_ui);
        nk_style_set_font(a->doc_ui, a->doc_font_ui);
        ps_graphics_background(a->doc_graphics, UI_COLOR(canvas).r, UI_COLOR(canvas).g,
                               UI_COLOR(canvas).b);
    }
    a->font_code = a->code_fonts[(p->editor_size - 16) / 2];
    a->show_vectors = (p->view_flags & PS_VIEW_VECTORS) != 0;
    a->show_paths = (p->view_flags & PS_VIEW_PATHS) != 0;
    a->show_points = (p->view_flags & PS_VIEW_POINTS) != 0;
    a->show_labels = (p->view_flags & PS_VIEW_LABELS) != 0;
    a->show_grid = (p->view_flags & PS_VIEW_GRID) != 0;
    a->orthographic = (p->view_flags & PS_VIEW_ORTHOGRAPHIC) != 0;
    a->show_log = (p->view_flags & PS_VIEW_LOG) != 0;
    a->view_disclosure = p->inspector_open ? NK_MAXIMIZED : NK_MINIMIZED;
    a->autosave_due = ps_clock() + p->autosave_seconds;
}
static void preferences_capture(app *a) {
    ps_preferences *p = &a->preferences;
    SDL_WindowFlags flags = SDL_GetWindowFlags(a->window);
    if (!(flags & (SDL_WINDOW_MAXIMIZED | SDL_WINDOW_MINIMIZED))) {
        int w, h;
        if (SDL_GetWindowSize(a->window, &w, &h) && w >= 1080 && w <= 8192 && h >= 740 &&
            h <= 8192) {
            p->width = (uint32_t)w;
            p->height = (uint32_t)h;
        }
    }
    if (!(flags & SDL_WINDOW_MINIMIZED))
        p->maximized = (flags & SDL_WINDOW_MAXIMIZED) != 0;
    p->view_flags = (a->show_vectors ? PS_VIEW_VECTORS : 0) | (a->show_paths ? PS_VIEW_PATHS : 0) |
                    (a->show_points ? PS_VIEW_POINTS : 0) | (a->show_labels ? PS_VIEW_LABELS : 0) |
                    (a->show_grid ? PS_VIEW_GRID : 0) |
                    (a->orthographic ? PS_VIEW_ORTHOGRAPHIC : 0) | (a->show_log ? PS_VIEW_LOG : 0);
    p->inspector_open = a->view_disclosure == NK_MAXIMIZED;
    if (a->tab >= 0 && a->tab <= 2)
        p->workspace = (uint32_t)a->tab;
}
static void preferences_start(app *a, const char *override_path) {
    if (override_path)
        snprintf(a->preferences_path, sizeof a->preferences_path, "%s", override_path);
    else {
        char *directory = SDL_GetPrefPath("Physim", "Physim");
        if (directory) {
            int n = snprintf(a->preferences_path, sizeof a->preferences_path, "%spreferences.bin",
                             directory);
            if (n < 0 || (size_t)n >= sizeof a->preferences_path)
                a->preferences_path[0] = 0;
            SDL_free(directory);
        }
    }
    ps_preferences p = PS_PREFERENCES_DEFAULT;
    ps_result r = ps_preferences_read(a->preferences_path, &p);
    a->preferences_writable = r == PS_OK || r == PS_EOF;
    if (!a->preferences_writable)
        snprintf(a->preferences_error, sizeof a->preferences_error,
                 "Einstellungen konnten nicht geladen werden (%s). Standardwerte werden verwendet. "
                 "Die Datei bleibt bis zum bewussten Speichern erhalten.",
                 ps_result_string(r));
    preferences_apply(a, &p);
    a->tab = (int)p.workspace;
    int width = (int)p.width, height = (int)p.height;
    SDL_Rect bounds;
    if (SDL_GetDisplayUsableBounds(SDL_GetDisplayForWindow(a->window), &bounds)) {
        width = (int)fmax(1080, fmin(width, bounds.w));
        height = (int)fmax(740, fmin(height, bounds.h));
    }
    SDL_SetWindowSize(a->window, width, height);
    if (p.maximized)
        SDL_MaximizeWindow(a->window);
}
static void open_settings(app *a) {
    if (a->tab == 6)
        return;
    preferences_capture(a);
    a->settings_draft = a->preferences;
    a->settings_previous_tab = a->tab;
    a->tab = 6;
}
static void settings_flag(struct nk_context *ui, const char *label, uint32_t flag, uint32_t *mask) {
    bool value = (*mask & flag) != 0;
    nk_checkbox_label(ui, label, &value);
    *mask = value ? *mask | flag : *mask & ~flag;
}
static void settings_view(app *a) {
    struct nk_context *ui = a->ui;
    ps_preferences *p = &a->settings_draft;
    nk_style_push_font(ui, a->font_title);
    nk_layout_row_dynamic(ui, 34, 1);
    nk_label(ui, "Einstellungen", NK_TEXT_LEFT);
    nk_style_pop_font(ui);
    nk_layout_row_dynamic(ui, 36, 3);
    a->settings_bounds[0] = nk_widget_bounds(ui);
    if (action_button(ui, "Übernehmen", true)) {
        ps_preferences next = *p;
        preferences_capture(a);
        next.width = a->preferences.width;
        next.height = a->preferences.height;
        next.maximized = a->preferences.maximized;
        ps_result r =
            a->preferences_path[0] ? ps_preferences_write(a->preferences_path, &next) : PS_IO;
        if (r == PS_OK) {
            preferences_apply(a, &next);
            a->preferences_writable = true;
            a->preferences_error[0] = 0;
            a->tab = a->settings_previous_tab;
            status(a, "Einstellungen gespeichert.");
        } else
            snprintf(
                a->preferences_error, sizeof a->preferences_error,
                "Speichern fehlgeschlagen: %s. Deine bisherigen Einstellungen bleiben erhalten.",
                ps_result_string(r));
    }
    a->settings_bounds[4] = nk_widget_bounds(ui);
    if (nk_button_label(ui, "Abbrechen"))
        a->tab = a->settings_previous_tab;
    a->settings_bounds[5] = nk_widget_bounds(ui);
    if (nk_button_label(ui, "Standardwerte")) {
        *p = PS_PREFERENCES_DEFAULT;
        p->width = a->preferences.width;
        p->height = a->preferences.height;
    }
    nk_layout_row_dynamic(ui, 40, 1);
    nk_label_wrap(ui,
                  "Deine Darstellung und Fensteraufteilung bleiben beim nächsten Start erhalten.");
    nk_layout_row_dynamic(ui, 26, 1);
    nk_label_colored(ui, "Darstellung", NK_TEXT_LEFT, muted());
    nk_layout_row_dynamic(ui, 32, 3);
    const char *themes[] = {"Dunkel", "Hell", "Hoher Kontrast"};
    for (unsigned i = 0; i < PS_THEME_COUNT; i++) {
        a->theme_bounds[i] = nk_widget_bounds(ui);
        if (nk_option_label(ui, themes[i], p->theme == i))
            p->theme = i;
    }
    nk_layout_row_dynamic(ui, 26, 1);
    nk_label_colored(ui, "Schriftgröße im Code-Editor", NK_TEXT_LEFT, muted());
    nk_layout_row_dynamic(ui, 32, 4);
    const char *sizes[] = {"16 px", "18 px", "20 px", "22 px"};
    for (unsigned i = 0; i < 4; i++) {
        if (i == 2)
            a->settings_bounds[1] = nk_widget_bounds(ui);
        if (nk_option_label(ui, sizes[i], p->editor_size == 16 + i * 2))
            p->editor_size = 16 + i * 2;
    }
    nk_style_push_font(ui, a->code_fonts[(p->editor_size - 16) / 2]);
    nk_layout_row_dynamic(ui, 38, 1);
    nk_label(ui, "double energy = 0.5 * mass * speed * speed;", NK_TEXT_LEFT);
    nk_style_pop_font(ui);
    nk_layout_row_dynamic(ui, 26, 1);
    nk_label_colored(ui, "Automatische Sicherung", NK_TEXT_LEFT, muted());
    nk_layout_row_dynamic(ui, 30, 2);
    nk_label(ui, "Ungespeicherte Quellen sichern", NK_TEXT_LEFT);
    const char *intervals[] = {"Alle 10 Sekunden", "Alle 30 Sekunden", "Jede Minute",
                               "Alle 2 Minuten"};
    const uint32_t seconds[] = {10, 30, 60, 120};
    int selected = 0;
    for (int i = 0; i < 4; i++)
        if (p->autosave_seconds == seconds[i])
            selected = i;
    p->autosave_seconds = seconds[nk_combo(ui, intervals, 4, selected, 30, nk_vec2(250, 180))];
    nk_layout_row_dynamic(ui, 26, 1);
    nk_label_colored(ui, "Simulation und Arbeitsfläche", NK_TEXT_LEFT, muted());
    nk_layout_row_dynamic(ui, 28, 2);
    settings_flag(ui, "Vektoren", PS_VIEW_VECTORS, &p->view_flags);
    settings_flag(ui, "Flugbahnen", PS_VIEW_PATHS, &p->view_flags);
    settings_flag(ui, "Punkte", PS_VIEW_POINTS, &p->view_flags);
    settings_flag(ui, "Beschriftungen", PS_VIEW_LABELS, &p->view_flags);
    a->settings_bounds[2] = nk_widget_bounds(ui);
    settings_flag(ui, "Raster und Achsen", PS_VIEW_GRID, &p->view_flags);
    settings_flag(ui, "Orthografische Ansicht", PS_VIEW_ORTHOGRAPHIC, &p->view_flags);
    a->settings_bounds[3] = nk_widget_bounds(ui);
    settings_flag(ui, "Protokoll geöffnet", PS_VIEW_LOG, &p->view_flags);
    nk_spacer(ui);
    nk_layout_row_dynamic(ui, 40, 1);
    nk_label_wrap(ui,
                  "Seitenleiste und geöffnetes Protokoll lassen sich an ihren Trennlinien ziehen.");
    if (a->preferences_error[0]) {
        nk_layout_row_dynamic(ui, 64, 1);
        nk_label_wrap(ui, a->preferences_error);
    }
}
static void panel_resize(app *a, float width, float height, float top, float footer) {
    struct nk_input *input = &a->ui->input;
    float side = (float)a->preferences.sidebar_width;
    a->panel_bounds[0] = nk_rect(side + 8, top, 8, height - top - footer - 8);
    a->panel_bounds[1] = nk_rect(8, height - footer - 8, width - 16, 8);
    if (!nk_input_is_mouse_down(input, NK_BUTTON_LEFT))
        a->panel_drag = 0;
    if (!a->panel_drag) {
        if (a->ui->active && a->ui->active->popup.active)
            return;
        if (nk_input_has_mouse_click_down_in_rect(input, NK_BUTTON_LEFT, a->panel_bounds[0], true))
            a->panel_drag = 1;
        else if (a->show_log && a->tab != 6 &&
                 nk_input_has_mouse_click_down_in_rect(input, NK_BUTTON_LEFT, a->panel_bounds[1],
                                                       true))
            a->panel_drag = 2;
    }
    if (a->panel_drag == 1)
        a->preferences.sidebar_width = (uint32_t)fmaxf(208, fminf(360, input->mouse.pos.x - 12));
    if (a->panel_drag == 2)
        a->preferences.log_height =
            (uint32_t)fmaxf(150, fminf(340, height - input->mouse.pos.y - 4));
    if (a->panel_drag && a->tab == 6) {
        a->settings_draft.sidebar_width = a->preferences.sidebar_width;
        a->settings_draft.log_height = a->preferences.log_height;
    }
}
