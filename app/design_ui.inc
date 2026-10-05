/* Private workspace presentation, included after main.c's application actions. */
#include "design_tokens.h"
static struct nk_color muted(void) { return UI_COLOR(muted); }
static struct nk_color accent(void) { return UI_COLOR(accent); }
static void style(struct nk_context *ui) {
    struct nk_color c[NK_COLOR_COUNT];
    nk_style_default(ui);
    for (int i = 0; i < NK_COLOR_COUNT; i++)
        c[i] = UI_COLOR(surface);
    c[NK_COLOR_TEXT] = UI_COLOR(text);
    c[NK_COLOR_WINDOW] = UI_COLOR(window);
    c[NK_COLOR_HEADER] = UI_COLOR(header);
    c[NK_COLOR_BORDER] = UI_COLOR(border);
    c[NK_COLOR_BUTTON] = UI_COLOR(button);
    c[NK_COLOR_BUTTON_HOVER] = UI_COLOR(button_hover);
    c[NK_COLOR_BUTTON_ACTIVE] = UI_COLOR(button_active);
    c[NK_COLOR_EDIT] = UI_COLOR(edit);
    c[NK_COLOR_EDIT_CURSOR] = accent();
    c[NK_COLOR_SELECT] = UI_COLOR(button);
    c[NK_COLOR_SELECT_ACTIVE] = UI_COLOR(selection_active);
    c[NK_COLOR_TOGGLE] = UI_COLOR(toggle);
    c[NK_COLOR_TOGGLE_HOVER] = UI_COLOR(toggle_hover);
    c[NK_COLOR_TOGGLE_CURSOR] = accent();
    c[NK_COLOR_SLIDER] = UI_COLOR(slider);
    c[NK_COLOR_SLIDER_CURSOR] = accent();
    c[NK_COLOR_SLIDER_CURSOR_HOVER] = UI_COLOR(accent_hover);
    c[NK_COLOR_SLIDER_CURSOR_ACTIVE] = accent();
    c[NK_COLOR_SCROLLBAR] = c[NK_COLOR_WINDOW];
    c[NK_COLOR_SCROLLBAR_CURSOR] = UI_COLOR(scrollbar);
    c[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = UI_COLOR(scrollbar_hover);
    c[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = UI_COLOR(scrollbar_active);
    nk_style_from_table(ui, c);
    ui->style.progress.padding = nk_vec2(0, 0);
    ui->style.progress.rounding = PS_UI_RADIUS_PROGRESS;
    ui->style.progress.cursor_rounding = PS_UI_RADIUS_PROGRESS;
    ui->style.window.rounding = PS_UI_RADIUS_PANEL;
    ui->style.window.padding = nk_vec2(PS_UI_SPACE_LARGE, PS_UI_SPACE_MEDIUM);
    ui->style.window.spacing = nk_vec2(PS_UI_SPACE_SMALL, PS_UI_SPACE_SMALL);
    ui->style.window.scrollbar_size = nk_vec2(7, 7);
    ui->style.button.rounding = PS_UI_RADIUS_CONTROL;
    ui->style.button.border = 0;
    ui->style.button.padding = nk_vec2(10, 6);
    ui->style.edit.rounding = PS_UI_RADIUS_FIELD;
    ui->style.edit.border = 0;
    ui->style.edit.padding = nk_vec2(10, 8);
    ui->style.edit.row_padding = 4;
    ui->style.combo.rounding = PS_UI_RADIUS_FIELD;
    ui->style.combo.border = 0;
    ui->style.property.rounding = PS_UI_RADIUS_FIELD;
    ui->style.tab.background = nk_style_item_color(c[NK_COLOR_WINDOW]);
    ui->style.tab.border = 0;
    ui->style.tab.padding = nk_vec2(0, 8);
    if (ui_palette == &PS_UI_HIGH_CONTRAST) {
        ui->style.button.border = 1;
        ui->style.edit.border = 1;
        ui->style.combo.border = 1;
        ui->style.property.border = 1;
    }
}
static bool action_button(struct nk_context *ui, const char *label, bool primary) {
    if (primary) {
        nk_style_push_style_item(ui, &ui->style.button.normal,
                                 nk_style_item_color(UI_COLOR(primary)));
        nk_style_push_style_item(ui, &ui->style.button.hover,
                                 nk_style_item_color(UI_COLOR(primary_hover)));
        nk_style_push_style_item(ui, &ui->style.button.active,
                                 nk_style_item_color(UI_COLOR(primary_active)));
        nk_style_push_color(ui, &ui->style.button.text_normal, UI_COLOR(primary_text));
        nk_style_push_color(ui, &ui->style.button.text_hover, UI_COLOR(primary_text));
        nk_style_push_color(ui, &ui->style.button.text_active, UI_COLOR(primary_text));
    }
    bool pressed = nk_button_label(ui, label);
    if (primary)
        for (int i = 0; i < 3; i++) {
            nk_style_pop_style_item(ui);
            nk_style_pop_color(ui);
        }
    return pressed;
}
static void section(struct nk_context *ui, const char *label) {
    nk_layout_row_dynamic(ui, 12, 1);
    nk_spacer(ui);
    nk_layout_row_dynamic(ui, 22, 1);
    nk_label_colored(ui, label, NK_TEXT_LEFT, muted());
}
static const char *project_name(const app *a) {
    const char *name = a->project;
    for (const char *p = name; *p; p++)
        if ((*p == '/' || *p == '\\') && p[1])
            name = p + 1;
    return *name ? name : "Neues Experiment";
}
static const char *workspace_name(const char *path) {
    const char *name = path;
    for (const char *p = path; *p; p++)
        if ((*p == '/' || *p == '\\') && p[1])
            name = p + 1;
    return name;
}
// clang-format off
#include "batch_ui.inc"
#include "settings_ui.inc"
#include "timeline_ui.inc"
// clang-format on
static void camera_inspector(app *a) {
    struct nk_context *ui = a->ui;
    scene_selection_sync(a);
    if (a->scene_selected && a->scene_selected_slot < display_scene(a)->count) {
        char selected[96];
        if (a->scene_selected_id) snprintf(selected, sizeof selected, "Auswahl: ID %u", a->scene_selected_id);
        else snprintf(selected, sizeof selected, "Auswahl: Eintrag %u", a->scene_selected_slot + 1);
        nk_layout_row_dynamic(ui, 26, 1);
        nk_label(ui, selected, NK_TEXT_LEFT);
        a->scene_hide_selection_bounds = nk_widget_bounds(ui);
        if (nk_button_label(ui, "Auswahl ausblenden")) {
            a->scene_view.hidden |= UINT32_C(1) << a->scene_selected_slot;
            a->scene_selected = false;
        }
    }
    if (nk_tree_state_push(ui, NK_TREE_TAB, "Szeneneinträge", &a->scene_disclosure)) {
        static const char *names[] = {"Kugel", "Linie", "Box", "Vektor", "Punkt", "Ebene", "Pfad", "Beschriftung"};
        nk_layout_row_dynamic(ui, 26, 1);
        if (nk_button_label(ui, "Alle einblenden")) a->scene_view.hidden = 0;
        nk_layout_row_dynamic(ui, 48, 1);
        nk_label_wrap(ui, "Alt + N/P: Auswahl. Alt + H: ausblenden.");
        for (uint32_t i = 0; i < display_scene(a)->count; i++) {
            const ps_object *o = &display_scene(a)->objects[i];
            char label[112];
            snprintf(label, sizeof label, "%u · %s%s%s", i + 1, names[o->shape],
                     o->text[0] ? " · " : "", o->text);
            if (o->id)
                snprintf(label, sizeof label, "ID %u · %s%s%s", o->id, names[o->shape],
                         o->text[0] ? " · " : "", o->text);
            bool visible = ps_scene_view_visible(&a->scene_view, i);
            nk_layout_row_dynamic(ui, 26, 1);
            a->scene_visibility_bounds[i] = nk_widget_bounds(ui);
            if (nk_checkbox_label(ui, label, &visible)) {
                uint32_t bit = UINT32_C(1) << i;
                if (visible) a->scene_view.hidden &= ~bit;
                else a->scene_view.hidden |= bit;
            }
        }
        nk_tree_state_pop(ui);
    }
    if (nk_tree_state_push(ui, NK_TREE_TAB, "Darstellung", &a->view_disclosure)) {
        nk_layout_row_dynamic(ui, 28, 1);
        nk_checkbox_label(ui, "Orthografische Ansicht", &a->orthographic);
        nk_checkbox_label(ui, "Vektoren", &a->show_vectors);
        nk_checkbox_label(ui, "Flugbahnen", &a->show_paths);
        nk_checkbox_label(ui, "Punkte", &a->show_points);
        nk_checkbox_label(ui, "Beschriftungen", &a->show_labels);
        nk_checkbox_label(ui, "Raster und Achsen", &a->show_grid);
        nk_tree_state_pop(ui);
    }
    if (nk_tree_push(ui, NK_TREE_TAB, "Kamera", NK_MINIMIZED)) {
        nk_layout_row_dynamic(ui, 22, 1);
        nk_label_colored(ui, "Blickrichtung", NK_TEXT_LEFT, muted());
        nk_layout_row_dynamic(ui, 32, 2);
        if (nk_button_label(ui, "Standard")) {
            a->yaw = PS_CAMERA_DEFAULT.yaw;
            a->pitch = PS_CAMERA_DEFAULT.pitch;
            a->zoom = PS_CAMERA_DEFAULT.distance;
            a->camera_target = PS_CAMERA_DEFAULT.target;
            a->orthographic = PS_CAMERA_DEFAULT.orthographic;
        }
        if (nk_button_label(ui, "Vorne")) {
            a->yaw = 0;
            a->pitch = 0;
            a->orthographic = true;
        }
        if (nk_button_label(ui, "Seite")) {
            a->yaw = (float)PS_PI / 2;
            a->pitch = 0;
            a->orthographic = true;
        }
        if (nk_button_label(ui, "Oben")) {
            a->yaw = 0;
            a->pitch = (float)PS_PI / 2;
            a->orthographic = true;
        }
        nk_layout_row_dynamic(ui, 26, 1);
        nk_label(ui, "Orbit", NK_TEXT_LEFT);
        nk_slider_float(ui, -3.14f, &a->yaw, 3.14f, .02f);
        nk_label(ui, "Neigung", NK_TEXT_LEFT);
        nk_slider_float(ui, -(float)PS_PI / 2, &a->pitch, (float)PS_PI / 2, .02f);
        nk_label(ui, "Zoom", NK_TEXT_LEFT);
        nk_slider_float(ui, 1.5f, &a->zoom, 12, .1f);
        nk_layout_row_dynamic(ui, 60, 1);
        nk_label_wrap(ui, "Rechts ziehen: drehen\nMitte ziehen: verschieben\nMausrad: zoomen");
        nk_layout_row_dynamic(ui, 112, 1);
        nk_label_wrap(ui, "Alt + Pfeile: drehen. Alt + Shift + Pfeile: verschieben. "
                          "Alt + Bild auf/ab: zoomen. Alt + Pos1: Standard.");
        nk_tree_pop(ui);
    }
    if (nk_tree_push(ui, NK_TREE_TAB, "Laufeinstellungen", NK_MINIMIZED)) {
        nk_layout_row_dynamic(ui, 24, 1);
        nk_label(ui, "Zeitschritt in Sekunden", NK_TEXT_LEFT);
        nk_layout_row_dynamic(ui, 32, 1);
        nk_begin_disabled(ui, a->runner.running || a->reset_pending);
        double previous_dt = a->dt;
        char previous_seed[sizeof a->seed];
        snprintf(previous_seed, sizeof previous_seed, "%s", a->seed);
        nk_property_double(ui, "dt", DBL_MIN, &a->dt, 1, .001, .0001f);
        nk_layout_row_dynamic(ui, 24, 1);
        nk_label(ui, "Zufallsseed", NK_TEXT_LEFT);
        nk_layout_row_dynamic(ui, 32, 1);
        nk_edit_string_zero_terminated(ui, NK_EDIT_FIELD, a->seed, sizeof a->seed,
                                       nk_filter_decimal);
        nk_end_disabled(ui);
        if (a->loaded && (previous_dt != a->dt || strcmp(previous_seed, a->seed)))
            a->project_settings_dirty = true;
        uint64_t parsed_seed;
        if (!ps_project_seed_parse(a->seed, &parsed_seed)) {
            nk_layout_row_dynamic(ui, 52, 1);
            nk_label_wrap(ui, "Seed: ganze Zahl von 0 bis 18446744073709551615 eingeben.");
        }
        if (a->built && a->parameters.count &&
            nk_tree_push(ui, NK_TREE_TAB, "Experimentparameter", NK_MINIMIZED)) {
            nk_begin_disabled(ui, a->runner.running || a->reset_pending);
            for (uint32_t i = 0; i < a->parameters.count; i++) {
                const ps_parameter *parameter = &a->parameters.entries[i];
                nk_layout_row_dynamic(ui, 24, 1);
                nk_label(ui, parameter->name, NK_TEXT_LEFT);
                char detail[256];
                snprintf(detail, sizeof detail, "%s · Bereich %.6g bis %.6g · Standard %.6g",
                         parameter->description, parameter->minimum, parameter->maximum,
                         parameter->default_value);
                nk_layout_row_dynamic(ui, 52, 1);
                nk_label_wrap(ui, detail);
                nk_layout_row_dynamic(ui, 32, 2);
                char previous[sizeof a->parameters.selected[i]];
                snprintf(previous, sizeof previous, "%s", a->parameters.selected[i]);
                nk_edit_string_zero_terminated(ui, NK_EDIT_FIELD, a->parameters.selected[i],
                                               sizeof a->parameters.selected[i], nk_filter_default);
                if (strcmp(previous, a->parameters.selected[i]))
                    a->project_settings_dirty = true;
                if (nk_button_label(ui, "Standard")) {
                    snprintf(a->parameters.selected[i], sizeof a->parameters.selected[i], "%.17g",
                             parameter->default_value);
                    a->project_settings_dirty = true;
                }
                double selected;
                if (ps_parameter_catalog_value(&a->parameters, i, &selected) != PS_OK) {
                    nk_layout_row_dynamic(ui, 24, 1);
                    nk_label_colored(ui, "Wert außerhalb des gültigen Bereichs", NK_TEXT_LEFT,
                                     UI_COLOR(error));
                }
            }
            nk_end_disabled(ui);
            nk_tree_pop(ui);
        }
        nk_tree_pop(ui);
    }
}
#include "document_ui.inc"
#include "workspace_tree_ui.inc"
#include "window_header.inc"
#include "toolbar_ui.inc"
static void draw_ui(app *a, int width, int height) {
    struct nk_context *ui = a->ui;
    if (a->recovery) {
        nk_style_push_vec2(ui, &ui->style.window.padding, nk_vec2(0, 5));
        nk_style_push_vec2(ui, &ui->style.window.spacing, nk_vec2(0, 0));
        nk_style_push_float(ui, &ui->style.window.rounding, 0);
        if (nk_begin(ui, "recovery-header", nk_rect(0, 0, (float)width, 38), NK_WINDOW_NO_SCROLLBAR)) {
            nk_layout_row_begin(ui, NK_STATIC, 28, 4);
            nk_layout_row_push(ui, (float)width - 138);
            a->window_drag_bounds = nk_widget_bounds(ui);
            nk_label(ui, "   Physim  ·  Wiederherstellung", NK_TEXT_LEFT);
            for (unsigned i = 0; i < 3; i++) {
                nk_layout_row_push(ui, 46);
                window_control(a, i);
            }
            nk_layout_row_end(ui);
        }
        nk_end(ui);
        nk_style_pop_float(ui);
        nk_style_pop_vec2(ui);
        nk_style_pop_vec2(ui);
        float panel_width = 640, panel_height = 420;
        if (nk_begin(ui, "recovery",
                     nk_rect(((float)width - panel_width) / 2, ((float)height - panel_height) / 2,
                             panel_width, panel_height),
                     0)) {
            nk_style_push_font(ui, a->font_title);
            nk_layout_row_dynamic(ui, 40, 1);
            nk_label(ui, "Deine Arbeit ist noch da.", NK_TEXT_LEFT);
            nk_style_pop_font(ui);
            nk_layout_row_dynamic(ui, 28, 1);
            nk_label_colored(ui, project_name(a), NK_TEXT_LEFT, muted());
            nk_layout_row_dynamic(ui, 64, 1);
            nk_label_wrap(ui, "Für Experiment- und Analysecode gibt es eine automatische Sicherung. "
                              "Stelle die Änderungen im Editor wieder her oder verwende die "
                              "gespeicherten Quelldateien und verwirf die Sicherung.");
            nk_layout_row_dynamic(ui, 60, 1);
            nk_label_wrap(
                ui,
                a->recovery_conflict
                    ? "Die Quelldateien wurden seit dieser Sicherung geändert. Wiederherstellen "
                      "lädt die gesicherten Texte; es führt die Änderungen nicht zusammen."
                    : "Beim Wiederherstellen bleiben die Quelldateien zunächst unverändert. "
                      "Du kannst beide Texte prüfen und anschließend speichern.");
            nk_layout_row_dynamic(ui, 40, 1);
            a->recovery_bounds[0] = nk_widget_bounds(ui);
            if (action_button(ui, "Änderungen wiederherstellen", true))
                resolve_recovery(a, true);
            nk_layout_row_dynamic(ui, 40, 1);
            a->recovery_bounds[1] = nk_widget_bounds(ui);
            if (nk_button_label(ui, "Gespeicherte Dateien verwenden"))
                resolve_recovery(a, false);
            nk_layout_row_dynamic(ui, 44, 1);
            nk_label_wrap(ui, a->status);
        }
        nk_end(ui);
        return;
    }
    float w = (float)width, h = (float)height, top = 72;
    bool expanded_log = a->show_log && a->tab != 6;
    float footer = expanded_log ? (float)a->preferences.log_height : 52.f;
    panel_resize(a, w, h, top, footer);
    float side = (float)a->preferences.sidebar_width;
    footer = expanded_log ? (float)a->preferences.log_height : 52.f;
    float content = h - top - footer - 8;
    nk_style_push_vec2(ui, &ui->style.window.padding, nk_vec2(0, 0));
    nk_style_push_vec2(ui, &ui->style.window.spacing, nk_vec2(0, 0));
    nk_style_push_float(ui, &ui->style.window.rounding, 0);
    if (nk_begin(ui, "header", nk_rect(0, 0, w, 64), NK_WINDOW_NO_SCROLLBAR)) {
        toolbar_view(a);
        struct nk_command_buffer *canvas = nk_window_get_canvas(ui);
        struct nk_rect previous_clip = canvas->clip;
        nk_push_scissor(canvas, nk_rect(0, 0, w, h));
        bool side_hover = nk_input_is_mouse_hovering_rect(&ui->input, a->panel_bounds[0]);
        bool log_hover = nk_input_is_mouse_hovering_rect(&ui->input, a->panel_bounds[1]);
        nk_stroke_line(canvas, side + 12, top + 12, side + 12, top + content - 12, 2,
                       side_hover || a->panel_drag == 1 ? accent() : UI_COLOR(divider));
        if (expanded_log)
            nk_stroke_line(canvas, 24, h - footer - 4, w - 24, h - footer - 4, 2,
                           log_hover || a->panel_drag == 2 ? accent() : UI_COLOR(divider));
        nk_push_scissor(canvas, previous_clip);
    }
    nk_end(ui);
    nk_style_pop_float(ui);
    nk_style_pop_vec2(ui);
    nk_style_pop_vec2(ui);
    if (nk_begin(ui, "sidebar", nk_rect(8, top, side, content), 0)) {
        {
            if (nk_tree_state_push(ui, NK_TREE_TAB, "Workspace", &a->workspace_disclosure)) {
                if (a->workspace_open) {
                    workspace_tree_view(a);
                } else {
                    nk_layout_row_dynamic(ui, 45, 1);
                    nk_label_wrap(ui, "Öffne einen Ordner, um seine Dateien hier zu sehen.");
                }
                nk_tree_state_pop(ui);
            }
            document_list_view(a);
            if (a->loaded) {
                nk_layout_row_dynamic(ui, 32, 1);
                if (action_button(ui, "Läufe & Berichte", a->tab == 4))
                    open_library(a);
                a->batch_navigation_bounds = nk_widget_bounds(ui);
                if (action_button(ui, "Monte Carlo", a->tab == 5))
                    a->tab = 5;
                nk_layout_row_dynamic(ui, 32, 1);
                if (nk_button_label(ui, "Messdaten im Ordner")) {
                    char uri[4200];
                    snprintf(uri, sizeof uri, "file:///%s/runs", a->project);
                    SDL_OpenURL(uri);
                }
                if (a->tab == 1) {
                    section(ui, "Inspector");
                    camera_inspector(a);
                } else {
                    section(ui, "Dateien");
                    nk_layout_row_dynamic(ui, 32, 1);
                    char source_label[32];
                    snprintf(source_label, sizeof source_label, "%s%s", experiment_source(a),
                             a->dirty ? "  •" : "");
                    if (nk_button_label(ui, source_label))
                        a->tab = 0;
                    snprintf(source_label, sizeof source_label, "%s%s", analysis_source(a),
                             a->analysis_dirty ? "  •" : "");
                    if (nk_button_label(ui, source_label)) {
                        a->tab = 2;
                        a->analysis_tab = 1;
                    }
                    if (nk_tree_push(ui, NK_TREE_TAB, "Build-Einstellungen", NK_MINIMIZED)) {
                        nk_layout_row_dynamic(ui, 32, 1);
                        const char *profiles[] = {"Debug", "Release"};
                        select_build_profile(a, nk_combo(ui, profiles, 2, a->profile, 28,
                                                       nk_vec2(180, 110)));
                        nk_tree_pop(ui);
                    }
                    nk_layout_row_dynamic(ui, 48, 1);
                    nk_label_wrap(ui, a->built ? "Build erfolgreich"
                                      : (a->dirty || a->analysis_dirty)
                                          ? "Ungespeicherte Änderungen"
                                          : "Build erforderlich");
                    if (a->autosave_error[0]) {
                        nk_layout_row_dynamic(ui, 76, 1);
                        nk_label_wrap(ui, a->autosave_error);
                    } else if (a->dirty || a->analysis_dirty) {
                        nk_layout_row_dynamic(ui, 42, 1);
                        char note[96];
                        snprintf(note, sizeof note, "Automatische Sicherung alle %u Sekunden",
                                 a->preferences.autosave_seconds);
                        nk_label_wrap(ui, a->autosave_current ? "Automatische Sicherung vorhanden"
                                                              : note);
                    }
                }
            }
            if (nk_tree_state_push(ui, NK_TREE_TAB, "Laufgrenzen", &a->limits_disclosure)) {
                nk_begin_disabled(ui, !idle(a));
                nk_layout_row_dynamic(ui, 26, 1);
                nk_label(ui, "Speicherlimit in MiB", NK_TEXT_LEFT);
                nk_layout_row_dynamic(ui, 32, 1);
                a->limit_bounds[0] = nk_widget_bounds(ui);
                nk_property_int(ui, "#Speicher", 0, &a->runner_memory_mib, 16384, 128, 16);
                nk_layout_row_dynamic(ui, 26, 1);
                nk_label(ui, "Zeitlimit in Sekunden", NK_TEXT_LEFT);
                nk_layout_row_dynamic(ui, 32, 1);
                a->limit_bounds[1] = nk_widget_bounds(ui);
                nk_property_double(ui, "#Zeit", 0, &a->runner_wall_seconds, 86400, 30, 1);
                nk_end_disabled(ui);
                nk_layout_row_dynamic(ui, 98, 1);
                nk_label_wrap(ui, "0 = ohne Grenze. Für neue Simulationen und Auswertungen; "
                                  "Zeit zählt inklusive Pausen. Serien haben ein eigenes Zeitlimit.");
                nk_tree_state_pop(ui);
            }
            section(ui, "Hilfe");
            nk_layout_row_dynamic(ui, 32, 1);
            a->documentation_bounds = nk_widget_bounds(ui);
            if (nk_button_label(ui, "Handbuch · F1")) {
                open_documentation(a, 1);
            }
            if (nk_tree_push(ui, NK_TREE_TAB, "Tastaturbefehle", NK_MINIMIZED)) {
                nk_layout_row_dynamic(ui, 154, 1);
                nk_label_wrap(ui, "" PS_UI_SHORTCUT "1/2/3  Arbeitsbereich\n" PS_UI_SHORTCUT "S  Speichern\n" PS_UI_SHORTCUT "F  "
                                  "Suchen\n" PS_UI_SHORTCUT "4  Läufe & Berichte\n" PS_UI_SHORTCUT "L  Protokoll\nF5  "
                                  "Build\nF6  Start / Pause\nF7  Zurücksetzen");
                nk_tree_pop(ui);
            }
        }
    }
    nk_end(ui);
    /* Nuklear otherwise transfers keyboard focus on mere sidebar hover. Keep an
     * activated search editable until a click deactivates it. */
    if (a->tab == 4 && (a->library_search_focus || a->library_search_active))
        nk_window_set_focus(ui, "workspace");
    if (nk_begin(ui, "workspace", nk_rect(side + 16, top, w - side - 24, content), 0)) {
        if (a->project_manager) {
            nk_layout_row_dynamic(ui, 42, 1);
            nk_style_push_font(ui, a->font_title);
            nk_label(ui, "Neues Physim-Projekt", NK_TEXT_LEFT);
            nk_style_pop_font(ui);
            nk_layout_row_dynamic(ui, 42, 1);
            nk_label_wrap(ui, "Wähle Zielordner, Namen, Vorlage und Sprachen. Vorhandene Dateien werden nicht überschrieben.");
            nk_layout_row_dynamic(ui, 22, 1);
            nk_label(ui, "Zielordner", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ui, 34, 2);
            nk_edit_string_zero_terminated(ui, NK_EDIT_FIELD, a->manager_parent,
                                           sizeof a->manager_parent, nk_filter_default);
            if (nk_button_label(ui, "Ordner wählen"))
                choose_workspace_path(a, PS_DIALOG_MANAGER_PARENT);
            nk_layout_row_dynamic(ui, 22, 1);
            nk_label(ui, "Projektname", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ui, 34, 1);
            nk_edit_string_zero_terminated(ui, NK_EDIT_FIELD, a->manager_name,
                                           sizeof a->manager_name, nk_filter_default);
            static const char *c_templates[] = {
                "Pendel", "Wurf mit Luftwiderstand", "Kugelstoß & Medien", "Box auf Ebene",
                "Feder–Masse–Dämpfer", "Wurf mit Unsicherheit", "Boxstoß", "Auftrieb"};
            static const char *language_templates[] = {
                "Pendel", "Wurf mit Luftwiderstand", "Kugelstoß & Medien", "Box auf Ebene",
                "Feder–Masse–Dämpfer", "Wurf mit Unsicherheit", "Boxstoß", "Auftrieb"};
            static const char *languages[] = {"C", "Physim"};
            nk_layout_row_dynamic(ui, 22, 1);
            nk_label(ui, "Vorlage", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ui, 34, 1);
            a->manager_template = nk_combo(ui, a->manager_experiment_language
                                            ? language_templates : c_templates,
                                            8, a->manager_template, 30,
                                            nk_vec2(320, 300));
            nk_layout_row_dynamic(ui, 22, 1);
            nk_label(ui, "Sprache des Experiments", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ui, 34, 1);
            a->manager_experiment_language = nk_combo(ui, languages, 2,
                a->manager_experiment_language, 30, nk_vec2(240, 100));
            nk_layout_row_dynamic(ui, 22, 1);
            nk_label(ui, "Sprache der Auswertung", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ui, 34, 1);
            a->template_analysis_language = nk_combo(ui, languages, 2,
                a->template_analysis_language, 30, nk_vec2(240, 100));
            nk_layout_row_dynamic(ui, 36, 2);
            if (action_button(ui, "Projekt anlegen", true))
                create_managed_project(a);
            if (nk_button_label(ui, "Zurück zum Workspace"))
                a->project_manager = false;
        } else if (a->tab == 8 && active_document(a)) {
            document_view(a, content);
        } else if (a->tab == 6)
            settings_view(a);
        else if (a->tab == 4 && a->loaded)
            library_view(a, content);
        else if (a->tab == 5 && a->loaded)
            batch_view(a);
        else if (!a->loaded) {
            nk_layout_row_dynamic(ui, 34, 1);
            nk_spacer(ui);
            nk_style_push_font(ui, a->font_title);
            nk_layout_row_dynamic(ui, 42, 1);
            nk_label(ui, a->workspace_open ? "Ordner geöffnet" : "Leerer Workspace", NK_TEXT_LEFT);
            nk_style_pop_font(ui);
            if (!a->workspace_open) {
                nk_layout_row_dynamic(ui, 64, 1);
                nk_label_wrap(ui, "Öffne einen Ordner oder lege ein neues Physim-Projekt an. Dateien und weitere Ordner kannst du anschließend hinzufügen.");
                nk_layout_row_static(ui, 36, 220, 2);
                if (action_button(ui, "Ordner öffnen", true))
                    choose_workspace_path(a, PS_DIALOG_OPEN_FOLDER);
                if (nk_button_label(ui, "Neues Projekt"))
                    a->project_manager = true;
                if (a->last_workspace.root[0]) {
                    nk_layout_row_dynamic(ui, 32, 1);
                    nk_label_colored(ui, a->last_workspace.root, NK_TEXT_LEFT, muted());
                    nk_layout_row_static(ui, 36, 260, 2);
                    a->workspace_restore_bounds = nk_widget_bounds(ui);
                    if (nk_button_label(ui, "Letzten Workspace öffnen"))
                        restore_workspace(a);
                    if (nk_button_label(ui, "Eintrag vergessen"))
                        forget_workspace(a);
                }
                if (a->workspace_state_error[0]) {
                    nk_layout_row_dynamic(ui, 64, 1);
                    nk_label_wrap(ui, a->workspace_state_error);
                    nk_layout_row_static(ui, 36, 260, 1);
                    if (nk_button_label(ui, "Workspace zurücksetzen"))
                        forget_workspace(a);
                }
            } else {
                nk_layout_row_dynamic(ui, 64, 1);
                nk_label_wrap(ui, "Kein Physim-Projekt erkannt. Wähle links eine Textdatei zum Bearbeiten oder lege über Datei ein Projekt an.");
            }
        } else if (a->tab == 0 || (a->tab == 2 && a->analysis_tab == 1)) {
            bool analysis = a->tab == 2;
            struct nk_text_edit *edit = analysis ? &a->analysis : &a->experiment;
            nk_layout_row_begin(ui, NK_DYNAMIC, 34, 4);
            nk_layout_row_push(ui, .46f);
            nk_style_push_font(ui, a->font_title);
            nk_label(ui, analysis ? analysis_source(a) : experiment_source(a), NK_TEXT_LEFT);
            nk_style_pop_font(ui);
            nk_layout_row_push(ui, .18f);
            if (nk_button_label(ui, analysis ? "Zum Bericht" : "Suchen")) {
                if (analysis)
                    a->analysis_tab = 0;
                else
                    a->show_search = !a->show_search;
            }
            nk_begin_disabled(ui, !idle(a));
            nk_layout_row_push(ui, .18f);
            if (nk_button_label(ui, "Speichern"))
                save_project(a);
            nk_layout_row_push(ui, .18f);
            if (action_button(ui, "Build", true))
                build_project(a);
            nk_end_disabled(ui);
            nk_layout_row_end(ui);
            if (a->show_search) {
                nk_layout_row_dynamic(ui, 30, 4);
                nk_edit_string_zero_terminated(ui, NK_EDIT_FIELD, a->find, sizeof a->find,
                                               nk_filter_default);
                if (nk_button_label(ui, "Suchen"))
                    find_text(a, edit, false);
                nk_edit_string_zero_terminated(ui, NK_EDIT_FIELD, a->replace, sizeof a->replace,
                                               nk_filter_default);
                if (nk_button_label(ui, "Ersetzen"))
                    find_text(a, edit, true);
            }
            editor(a, edit, content - (a->show_search ? 150 : 112));
            nk_layout_row_dynamic(ui, 22, 1);
            nk_label_colored(ui,
                             analysis ? (a->language_analysis ? "Auswertung  ·  Physim  ·  " PS_UI_SHORTCUT "S speichern" : "Auswertung  ·  C17  ·  " PS_UI_SHORTCUT "S speichern")
                                      : a->language_experiment
                                          ? "Experiment  ·  Physim  ·  " PS_UI_SHORTCUT "F suchen  ·  F5 bauen"
                                          : "Experiment  ·  C17  ·  " PS_UI_SHORTCUT "F suchen  ·  F5 bauen",
                             NK_TEXT_LEFT, muted());
        } else if (a->tab == 1) {
            nk_layout_row_begin(ui, NK_DYNAMIC, 34, 2);
            nk_layout_row_push(ui, .6f);
            nk_style_push_font(ui, a->font_title);
            nk_label(ui, "Simulation", NK_TEXT_LEFT);
            nk_style_pop_font(ui);
            nk_layout_row_push(ui, .4f);
            char time[96];
            if (a->timeline_browsing)
                snprintf(time, sizeof time, "Rückblick %.6g s · %s %.6g s", display_time(a),
                         a->runner.running ? "Live" : "Ende", a->simulation_time);
            else snprintf(time, sizeof time, "%.6g s", a->simulation_time);
            nk_label_colored(ui, time, NK_TEXT_RIGHT, muted());
            nk_layout_row_end(ui);
            nk_layout_row_dynamic(ui, 34, 3);
            a->run_control_bounds[0] = nk_widget_bounds(ui);
            nk_begin_disabled(ui, a->runner.running || !idle(a));
            if (action_button(ui, a->built ? "Neuer Lauf" : "Build",
                              !a->runner.running && idle(a))) {
                if (a->built)
                    start_run(a);
                else
                    build_project(a);
            }
            nk_end_disabled(ui);
            bool run_control_ready = a->runner.running && a->hello && !a->reset_starting &&
                                     a->stop_at == 0;
            nk_begin_disabled(ui, !run_control_ready);
            a->run_control_bounds[1] = nk_widget_bounds(ui);
            if (action_button(ui, a->paused ? "Fortsetzen" : "Pause", run_control_ready))
                command(a, a->paused ? PS_MSG_RUN : PS_MSG_PAUSE);
            nk_begin_disabled(ui, !a->paused);
            a->run_control_bounds[2] = nk_widget_bounds(ui);
            if (nk_button_label(ui, "Einzelschritt"))
                command(a, PS_MSG_STEP);
            nk_end_disabled(ui);
            nk_end_disabled(ui);
            nk_layout_row_dynamic(ui, 34, 3);
            a->run_control_bounds[3] = nk_widget_bounds(ui);
            nk_begin_disabled(ui, !reset_available(a));
            if (nk_button_label(ui, a->reset_pending ? "Zurücksetzen …" : "Zurücksetzen"))
                reset_run(a);
            nk_end_disabled(ui);
            /* A hung create callback may never complete HELLO; stopping still works. */
            nk_begin_disabled(ui, !a->runner.running || a->stop_at != 0);
            a->run_control_bounds[4] = nk_widget_bounds(ui);
            if (nk_button_label(ui, "Stoppen")) {
                command(a, PS_MSG_STOP);
                a->stop_at = ps_clock();
            }
            nk_end_disabled(ui);
            a->run_control_bounds[5] = nk_widget_bounds(ui);
            if (nk_button_label(ui, "Auswerten")) {
                a->tab = 2;
                a->analysis_tab = 0;
            }
            nk_layout_row_dynamic(ui, 30, 2);
            nk_label(ui, "Geschwindigkeit", NK_TEXT_LEFT);
            char speed_label[64];
            if (a->simulation_speed == 0)
                snprintf(speed_label, sizeof speed_label, "Offline · maximale Geschwindigkeit");
            else
                snprintf(speed_label, sizeof speed_label, "%.3g× Echtzeit", a->simulation_speed);
            bool speed_ready = !a->reset_pending && !a->reset_starting &&
                               (!a->runner.running || (a->hello && !a->stop_at));
            nk_begin_disabled(ui, !speed_ready);
            a->speed_bounds = nk_widget_bounds(ui);
            if (nk_combo_begin_label(ui, speed_label, nk_vec2(300, 290))) {
                const double speeds[] = {.25, .5, 1, 2, 4, 8, 16, 0};
                nk_layout_row_dynamic(ui, 28, 1);
                for (unsigned i = 0; i < 8; i++) {
                    char label[64];
                    if (speeds[i] == 0) snprintf(label, sizeof label, "Offline · maximal");
                    else snprintf(label, sizeof label, "%.3g× Echtzeit", speeds[i]);
                    a->speed_choices[i] = nk_widget_bounds(ui);
                    if (nk_button_label(ui, label)) {
                        select_simulation_speed(a, speeds[i]);
                        nk_combo_close(ui);
                    }
                }
                nk_combo_end(ui);
            }
            nk_end_disabled(ui);
            float plot_height = fmaxf(96, content * .21f);
            viewport(a, content - plot_height - 308);
            timeline_controls(a);
            nk_layout_row_dynamic(ui, 28, 2);
            if (a->channel_count) {
                const char *names[PS_MAX_CHANNELS];
                for (uint32_t i = 0; i < a->channel_count; i++)
                    names[i] = a->channel_names[i];
                if (a->plot_channel >= (int)a->channel_count)
                    a->plot_channel = 0;
                a->plot_channel = nk_combo(ui, names, (int)a->channel_count, a->plot_channel, 26,
                                           nk_vec2(280, 240));
                char value[96];
                const double *shown = display_values(a);
                snprintf(value, sizeof value, "%.8g", shown[a->plot_channel]);
                int mask = a->channel_status[a->plot_channel];
                if (mask >= 0 && shown[mask] != 1)
                    snprintf(value, sizeof value, "%s",
                             shown[mask] == 2 ? "Messung ausgefallen" : "Keine neue Messung");
                nk_label(ui, value, NK_TEXT_RIGHT);
                plot(a, a->history_t, a->history_v[a->plot_channel], a->history_count, plot_height,
                     "Live-Messwerte", mask >= 0, NULL);
            } else {
                nk_label_colored(ui, "Messwerte erscheinen während des Laufs.", NK_TEXT_LEFT,
                                 muted());
                nk_spacer(ui);
            }
            if (a->runner.running && ps_clock() - a->heartbeat > 3) {
                nk_layout_row_dynamic(ui, 28, 1);
                nk_label_colored(ui, "Keine Antwort. Mit Stoppen den Lauf beenden.", NK_TEXT_LEFT,
                                 UI_COLOR(error));
            }
        } else {
            nk_layout_row_dynamic(ui, 34, 2);
            nk_style_push_font(ui, a->font_title);
            nk_label(ui, "Auswertung", NK_TEXT_LEFT);
            nk_style_pop_font(ui);
            if (nk_button_label(ui, "Analysecode bearbeiten"))
                a->analysis_tab = 1;
            nk_layout_row_dynamic(ui, 34, 3);
            nk_begin_disabled(ui, !idle(a) || (!a->last_run[0] && !a->selected_count));
            char analyze_label[64];
            if (a->selected_count > 1)
                snprintf(analyze_label, sizeof analyze_label, "%u Läufe vergleichen",
                         a->selected_count);
            else
                snprintf(analyze_label, sizeof analyze_label, "Analyse starten");
            if (action_button(ui, analyze_label, true))
                analyze(a, false);
            nk_end_disabled(ui);
            nk_begin_disabled(ui, !idle(a) || !a->last_run[0]);
            if (nk_button_label(ui, "CSV exportieren"))
                analyze(a, true);
            nk_end_disabled(ui);
            if (nk_button_label(ui, "Läufe & Berichte"))
                open_library(a);
            if (a->report_thread || a->result_error[0]) {
                nk_layout_row_dynamic(ui, 42, 1);
                nk_label_wrap(ui, a->report_thread ? "Analyseergebnis wird geprüft und geladen …"
                                                   : a->result_error);
            }
            if (a->analysis_report) {
                nk_layout_row_dynamic(ui, 32, 2);
                if (action_button(ui, "Analyseergebnis", a->show_report))
                    a->show_report = true;
                if (action_button(ui, "Messdaten", !a->show_report))
                    a->show_report = false;
            }
            if (a->analysis_report && a->show_report) {
                report_view(a, content);
            } else if (!a->data.thread && a->data.count) {
                const char *names[PS_MAX_CHANNELS];
                for (uint32_t i = 0; i < a->data.channel_count; i++)
                    names[i] = a->data.channels[i].name;
                nk_layout_row_dynamic(ui, 30, 2);
                if (a->plot_channel >= (int)a->data.channel_count)
                    a->plot_channel = 0;
                a->plot_channel = nk_combo(ui, names, (int)a->data.channel_count, a->plot_channel,
                                           28, nk_vec2(280, 240));
                char total[120];
                snprintf(total, sizeof total, "%llu Messpunkte · %s",
                         (unsigned long long)a->data.total,
                         a->data.result == PS_RECOVERED ? "rekonstruiert" : "vollständig");
                nk_label_colored(ui, total, NK_TEXT_RIGHT, muted());
                char title[128];
                snprintf(title, sizeof title, "%s [%s]", names[a->plot_channel],
                         a->data.channels[a->plot_channel].unit);
                int mask = -1;
                (void)ps_channel_status_index(a->data.channels, a->data.channel_count,
                                              (uint32_t)a->plot_channel, &mask);
                if (mask >= 0) {
                    nk_layout_row_dynamic(ui, 24, 1);
                    char note[160];
                    snprintf(note, sizeof note,
                             "%llu gültige Messungen · Ausfälle und nicht fällige Werte sind "
                             "ausgeschlossen.",
                             (unsigned long long)a->data.stats[a->plot_channel].count);
                    nk_label_colored(ui, note, NK_TEXT_LEFT, muted());
                }
                plot(a, a->data.time, a->data.values[a->plot_channel], a->data.count,
                     content * .39f, title, mask >= 0, &a->data_views[a->plot_channel]);
                section(ui, "Statistik des gesamten Laufs");
                nk_layout_row_dynamic(ui, 28, 5);
                const char *head[] = {"Kanal", "Mittelwert", "Std.-Abw.", "Minimum", "Maximum"};
                for (int i = 0; i < 5; i++)
                    nk_label_colored(ui, head[i], NK_TEXT_LEFT, muted());
                for (uint32_t i = 0; i < a->data.channel_count; i++) {
                    nk_label(ui, names[i], NK_TEXT_LEFT);
                    double values[] = {a->data.stats[i].mean,
                                       ps_statistics_stddev(&a->data.stats[i]),
                                       a->data.stats[i].min, a->data.stats[i].max};
                    for (int k = 0; k < 4; k++) {
                        char text[80];
                        snprintf(text, sizeof text, "%.8g", values[k]);
                        if (!a->data.stats[i].count || (k == 1 && a->data.stats[i].count < 2))
                            snprintf(text, sizeof text, "—");
                        nk_label(ui, text, NK_TEXT_LEFT);
                    }
                }
            } else {
                nk_layout_row_dynamic(ui, 110, 1);
                nk_label_wrap(ui, a->data.thread ? "Messdaten werden geladen …"
                                  : a->last_run[0]
                                      ? "Werte den gespeicherten Lauf aus. Die Analyse erstellt "
                                        "Messkurven, Statistik und einen Bericht."
                                      : "Hier werden Ergebnisse sichtbar. Starte zuerst eine "
                                        "Simulation und zeichne einen Lauf auf.");
            }
            const char *source_path =
                a->analysis_report && a->show_report ? a->loaded_report_path : a->last_run;
            if (source_path[0] && nk_tree_push(ui, NK_TREE_TAB, "Quelldatei", NK_MINIMIZED)) {
                nk_layout_row_dynamic(ui, 56, 1);
                nk_label_wrap(ui, source_path);
                nk_tree_pop(ui);
            }
        }
    }
    nk_end(ui);
    nk_style_push_vec2(ui, &ui->style.window.padding, nk_vec2(16, 8));
    if (nk_begin(ui, "log", nk_rect(8, h - footer, w - 16, footer - 8), NK_WINDOW_NO_SCROLLBAR)) {
        nk_layout_row_begin(ui, NK_DYNAMIC, 28, 3);
        nk_layout_row_push(ui, .72f);
        nk_label_colored(ui, a->status, NK_TEXT_LEFT, muted());
        nk_layout_row_push(ui, .14f);
        if (a->job.running) {
            if (nk_button_label(ui, "Abbrechen")) {
                ps_process_kill(&a->job);
                ps_process_close(&a->job);
                status(a, "Hintergrundjob abgebrochen.");
            }
        } else
            nk_spacer(ui);
        nk_layout_row_push(ui, .14f);
        if (nk_button_label(ui, expanded_log ? "Protokoll schließen" : "Protokoll")) {
            a->show_log = !expanded_log;
        }
        nk_layout_row_end(ui);
        if (expanded_log) {
            nk_layout_row_dynamic(ui, footer - 66, 1);
            if (a->diagnostic_count) {
                if (nk_group_begin(ui, "Compilerdiagnosen", 0)) {
                    nk_layout_row_dynamic(ui, 30, 1);
                    for (int i = 0; i < a->diagnostic_count; i++)
                        if (nk_button_label(ui, a->diagnostics[i].text))
                            jump_to_diagnostic(a, &a->diagnostics[i]);
                    nk_group_end(ui);
                }
            } else {
                nk_style_push_font(ui, a->font_code);
                nk_edit_string_zero_terminated(
                    ui, NK_EDIT_BOX | NK_EDIT_READ_ONLY | NK_EDIT_GOTO_END_ON_ACTIVATE, a->log,
                    sizeof a->log, nk_filter_default);
                nk_style_pop_font(ui);
            }
        }
    }
    nk_end(ui);
    nk_style_pop_vec2(ui);
    if (a->tab == 4 && a->library_search_active)
        nk_window_set_focus(ui, "workspace");
}
