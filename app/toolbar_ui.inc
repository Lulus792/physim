static void select_workspace_tab(app *a, int tab) {
    a->project_manager = false;
    a->tab = tab == 0 && !a->loaded && a->document_count ? 8 : tab;
}
static bool toolbar_menu_item(struct nk_context *ui, const char *text, bool enabled) {
    struct nk_rect bounds;
    enum nk_widget_layout_states state = nk_widget(&bounds, ui);
    bool hover =
        enabled && state == NK_WIDGET_VALID && nk_input_is_mouse_hovering_rect(&ui->input, bounds);
    struct nk_command_buffer *canvas = nk_window_get_canvas(ui);
    if (hover)
        nk_fill_rect(canvas, bounds, 5, PS_UI_DARK.selection_active);
    const struct nk_user_font *font = ui->style.font;
    const char *shortcut = strchr(text, '\t');
    int length = shortcut ? (int)(shortcut - text) : (int)strlen(text);
    float y = bounds.y + (bounds.h - font->height) / 2;
    nk_draw_text(canvas, nk_rect(bounds.x + 10, y, bounds.w - 20, font->height), text, length, font,
                 nk_rgba(0, 0, 0, 0), enabled ? PS_UI_DARK.text : muted());
    if (shortcut) {
        ++shortcut;
        int count = (int)strlen(shortcut);
        float width = font->width(font->userdata, font->height, shortcut, count);
        nk_draw_text(canvas, nk_rect(bounds.x + bounds.w - 10 - width, y, width + 1, font->height),
                     shortcut, count, font, nk_rgba(0, 0, 0, 0), hover ? PS_UI_DARK.text : muted());
    }
    return hover && nk_input_is_mouse_pressed(&ui->input, NK_BUTTON_LEFT);
}
static bool toolbar_menu_action(app *a, const char *text, bool enabled) {
    if (!toolbar_menu_item(a->ui, text, enabled))
        return false;
    a->toolbar_menu = 0;
    nk_popup_close(a->ui);
    return true;
}
static void toolbar_popup(app *a, int previous) {
    struct nk_context *ui = a->ui;
    if (!a->toolbar_menu && !previous)
        return;
    const bool can_choose = !a->dialog_pending && idle(a) && !a->library_thread && !a->recovery;
    struct nk_rect anchor = a->toolbar_bounds[(a->toolbar_menu ? a->toolbar_menu : previous) - 1];
    a->toolbar_popup_bounds =
        nk_rect(anchor.x, anchor.y + anchor.h + 2, 286, a->toolbar_menu == 2 ? 132.f : 170.f);
    nk_style_push_vec2(ui, &ui->style.window.popup_padding, nk_vec2(5, 5));
    nk_style_push_vec2(ui, &ui->style.window.spacing, nk_vec2(4, 4));
    nk_style_push_float(ui, &ui->style.window.rounding, 9);
    if (nk_popup_begin(ui, NK_POPUP_STATIC, "application-menu", NK_WINDOW_NO_SCROLLBAR,
                       a->toolbar_popup_bounds)) {
        if (!a->toolbar_menu)
            nk_popup_close(ui);
        else if (a->toolbar_menu == 1) {
            nk_layout_row_dynamic(ui, 26, 1);
            a->toolbar_item_bounds[0] = nk_widget_bounds(ui);
            if (toolbar_menu_action(a, "Ordner öffnen …", can_choose))
                choose_workspace_path(a, PS_DIALOG_OPEN_FOLDER);
            a->toolbar_item_bounds[1] = nk_widget_bounds(ui);
            if (toolbar_menu_action(a, "Datei hinzufügen …", can_choose && a->workspace_open))
                choose_workspace_path(a, PS_DIALOG_ADD_FILE);
            a->toolbar_item_bounds[2] = nk_widget_bounds(ui);
            if (toolbar_menu_action(a, "Ordner hinzufügen …", can_choose && a->workspace_open))
                choose_workspace_path(a, PS_DIALOG_ADD_FOLDER);
            nk_layout_row_dynamic(ui, 8, 1);
            struct nk_rect separator;
            nk_widget(&separator, ui);
            nk_stroke_line(nk_window_get_canvas(ui), separator.x + 10,
                           separator.y + separator.h / 2, separator.x + separator.w - 10,
                           separator.y + separator.h / 2, 1, PS_UI_DARK.button);
            nk_layout_row_dynamic(ui, 26, 1);
            a->toolbar_item_bounds[3] = nk_widget_bounds(ui);
            if (toolbar_menu_action(a, "Neues Projekt …", can_choose)) {
                a->project_manager = true;
                a->workspace_preview_path[0] = 0;
            }
            a->toolbar_item_bounds[4] = nk_widget_bounds(ui);
            if (toolbar_menu_action(a, "Speichern\t" PS_UI_SHORTCUT "S",
                                    (a->loaded || a->document_count) && idle(a)))
                save_active_document(a);

        } else {
            nk_layout_row_dynamic(ui, 26, 1);
            a->toolbar_item_bounds[5] = nk_widget_bounds(ui);
            if (toolbar_menu_action(a,
                                    a->show_log ? "Protokoll ausblenden\t" PS_UI_SHORTCUT "L"
                                                : "Protokoll einblenden\t" PS_UI_SHORTCUT "L",
                                    true))
                a->show_log = !a->show_log;
            if (toolbar_menu_action(a, "Dateibaum aktualisieren", a->workspace_open))
                refresh_workspace_entries(a);
            if (toolbar_menu_action(a, "Läufe und Berichte\t" PS_UI_SHORTCUT "4", a->loaded)) {
                a->project_manager = false;
                open_library(a);
            }
            a->batch_navigation_bounds = nk_widget_bounds(ui);
            if (toolbar_menu_action(a, "Monte Carlo", a->loaded)) {
                a->project_manager = false;
                a->tab = 5;
            }
        }
        nk_popup_end(ui);
    }
    nk_style_pop_float(ui);
    nk_style_pop_vec2(ui);
    nk_style_pop_vec2(ui);
}
static void toolbar_view(app *a) {
    struct nk_context *ui = a->ui;
    nk_fill_rect(nk_window_get_canvas(ui), nk_rect(0, 0, nk_window_get_bounds(ui).w, 32), 0,
                 PS_UI_DARK.header);
    nk_style_push_vec2(ui, &ui->style.button.padding, nk_vec2(7, 2));
    nk_style_push_style_item(ui, &ui->style.button.normal, nk_style_item_color(PS_UI_DARK.header));
    nk_menubar_begin(ui);
    float available = nk_window_get_content_region(ui).w;
    nk_layout_row_begin(ui, NK_STATIC, 32, 10);
    nk_layout_row_push(ui, 72);
    nk_label(ui, "Physim", NK_TEXT_CENTERED);
    const char *menus[] = {"Datei", "Ansicht", "Hilfe", "Einstellungen"};
    const float menu_width[] = {58, 72, 54, 116};
    int previous = a->toolbar_menu;
    /* Header actions remain responsive while the popup makes its parent read-only. */
    bool press = nk_input_is_mouse_pressed(&ui->input, NK_BUTTON_LEFT);
    int clicked = -1;
    for (int i = 0; i < 4; i++) {
        if (press && nk_input_is_mouse_hovering_rect(&ui->input, a->toolbar_bounds[i]))
            clicked = i;
    }
    if (clicked >= 0) {
        if (clicked < 2)
            a->toolbar_menu = a->toolbar_menu == clicked + 1 ? 0 : clicked + 1;
        else {
            a->toolbar_menu = 0;
            if (clicked == 2)
                open_documentation(a, 1);
            else {
                a->project_manager = false;
                open_settings(a);
            }
        }
    } else if (a->toolbar_menu &&
               (nk_input_is_key_pressed(&ui->input, NK_KEY_TEXT_RESET_MODE) ||
                (press && !nk_input_is_mouse_hovering_rect(&ui->input, a->toolbar_popup_bounds))))
        a->toolbar_menu = 0;
    else if (a->toolbar_menu && !press &&
             (ui->input.mouse.delta.x != 0 || ui->input.mouse.delta.y != 0)) {
        for (int i = 0; i < 2; i++) {
            if (nk_input_is_mouse_hovering_rect(&ui->input, a->toolbar_bounds[i]))
                a->toolbar_menu = i + 1;
        }
    }
    for (int i = 0; i < 4; i++) {
        nk_layout_row_push(ui, menu_width[i]);
        struct nk_rect bounds;
        nk_widget(&bounds, ui);
        a->toolbar_bounds[i] = bounds;
        bool selected = a->toolbar_menu == i + 1 || (!a->toolbar_menu && i == 3 && a->tab == 6);
        bool hover = nk_input_is_mouse_hovering_rect(&ui->input, bounds);
        struct nk_command_buffer *canvas = nk_window_get_canvas(ui);
        if (selected || hover)
            nk_fill_rect(canvas, nk_rect(bounds.x + 2, bounds.y + 3, bounds.w - 4, bounds.h - 6), 6,
                         selected ? PS_UI_DARK.button_active : PS_UI_DARK.button_hover);
        const struct nk_user_font *font = ui->style.font;
        float text_width =
            font->width(font->userdata, font->height, menus[i], (int)strlen(menus[i]));
        nk_draw_text(canvas,
                     nk_rect(bounds.x + (bounds.w - text_width) / 2,
                             bounds.y + (bounds.h - font->height) / 2, text_width + 1,
                             font->height),
                     menus[i], (int)strlen(menus[i]), font, nk_rgba(0, 0, 0, 0), PS_UI_DARK.text);
    }
    nk_layout_row_push(ui, fmaxf(100, available - 72 - 58 - 72 - 54 - 116 - 175 - 138));
    struct nk_rect path_bounds = nk_widget_bounds(ui);
    nk_label_colored(ui, a->workspace_open ? a->workspace : "Kein Ordner geöffnet", NK_TEXT_LEFT,
                     muted());
    if (a->workspace_open && !ui->current->popup.active && !a->toolbar_menu &&
        nk_input_is_mouse_hovering_rect(&ui->input, path_bounds))
        nk_tooltip(ui, a->workspace);
    nk_layout_row_push(ui, 175);
    nk_label_colored(ui,
                     a->runner.running ? (a->paused ? "Pausiert" : "Simulation läuft")
                     : a->batch_thread ? "Laufserie läuft"
                     : a->job.running  ? "Wird verarbeitet …"
                     : (a->dirty || a->analysis_dirty || a->project_settings_dirty || documents_dirty(a))
                         ? "Ungespeicherte Änderungen"
                         : "Bereit",
                     NK_TEXT_RIGHT, muted());
    struct nk_rect status_bounds = nk_widget_bounds(ui);
    a->window_drag_bounds =
        nk_rect(path_bounds.x, path_bounds.y, status_bounds.x - path_bounds.x, path_bounds.h);
    for (unsigned i = 0; i < 3; i++) {
        nk_layout_row_push(ui, 46);
        window_control(a, i);
    }
    nk_layout_row_end(ui);
    toolbar_popup(a, previous);
    nk_menubar_end(ui);
    nk_style_pop_style_item(ui);
    nk_layout_row_dynamic(ui, 30, 3);
    const char *tabs[] = {"Entwickeln", "Simulieren", "Auswerten"};
    for (int i = 0; i < 3; i++) {
        bool selected = !a->project_manager && (a->tab == i || (a->tab == 8 && i == 0));
        struct nk_rect bounds;
        enum nk_widget_layout_states state = nk_widget(&bounds, ui);
        a->navigation_bounds[i] = bounds;
        bool hovered = nk_input_is_mouse_hovering_rect(&ui->input, bounds);
        if (state == NK_WIDGET_VALID && !a->toolbar_menu && hovered &&
            nk_input_is_mouse_pressed(&ui->input, NK_BUTTON_LEFT))
            select_workspace_tab(a, i);
        struct nk_command_buffer *canvas = nk_window_get_canvas(ui);
        struct nk_rect surface = nk_rect(bounds.x + 5, bounds.y + 3, bounds.w - 10, bounds.h - 6);
        if (selected || hovered)
            nk_fill_rect(canvas, surface, 8,
                         selected ? PS_UI_DARK.button_hover : PS_UI_DARK.button);
        const struct nk_user_font *font = ui->style.font;
        float text_width = font->width(font->userdata, font->height, tabs[i], (int)strlen(tabs[i]));
        nk_draw_text(canvas,
                     nk_rect(bounds.x + (bounds.w - text_width) / 2,
                             bounds.y + (bounds.h - font->height) / 2, text_width + 1,
                             font->height),
                     tabs[i], (int)strlen(tabs[i]), font, nk_rgba(0, 0, 0, 0),
                     selected ? PS_UI_DARK.text : PS_UI_DARK.muted);
    }
    nk_style_pop_vec2(ui);
}
