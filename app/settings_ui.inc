/* App-owned presentation state, separate from project sources and run metadata. */
static void preferences_apply(app *a, const ps_preferences *p) {
    if(!documentation_fonts_prepare_sizes(a,a->doc_zoom_size?a->doc_zoom_size:p->ui_size,
        a->doc_zoom_size?a->doc_zoom_size:p->editor_size,p->ui_size,p->editor_size))return;
    a->preferences = *p;
    a->font_ui=a->ui_fonts[(p->ui_size-16)/2];
    a->font_title=a->title_fonts[(p->ui_size-16)/2];
    ui_palette = p->theme == PS_THEME_LIGHT ? &PS_UI_LIGHT :
                 p->theme == PS_THEME_HIGH_CONTRAST ? &PS_UI_HIGH_CONTRAST : &PS_UI_DARK;
    style(a->ui);
    nk_style_set_font(a->ui, a->font_ui);
    nk_sdl_set_ui_size(a->ui,p->ui_size);
    ps_graphics_background(a->graphics, UI_COLOR(canvas).r, UI_COLOR(canvas).g, UI_COLOR(canvas).b);
    struct nk_color scene=UI_COLOR(canvas),grid=UI_COLOR(plot_grid);
    ps_graphics_scene_colors(a->graphics,((uint32_t)scene.r<<24)|((uint32_t)scene.g<<16)|((uint32_t)scene.b<<8)|255,
        ((uint32_t)grid.r<<24)|((uint32_t)grid.g<<16)|((uint32_t)grid.b<<8)|255);
    if (a->doc_ui) {
        a->doc_font_ui=a->doc_ui_fonts[(documentation_font_size(a)-16)/2];
        a->doc_font_title=a->doc_title_fonts[(documentation_font_size(a)-16)/2];
        a->doc_font_code=a->doc_code_fonts[(documentation_code_size(a)-16)/2];
        if(a->documentation)a->documentation->layout_width=0;
        style(a->doc_ui);
        nk_style_set_font(a->doc_ui, a->doc_font_ui);
        nk_sdl_set_ui_size(a->doc_ui,p->ui_size);
        /* Theme padding and text reflow can move the focused action below the
         * viewport. Re-evaluate its reveal after the new layout is drawn. */
        if(a->doc_visible && a->doc_keyboard)a->doc_focus_reveal=true;
        ps_graphics_background(a->doc_graphics, UI_COLOR(canvas).r, UI_COLOR(canvas).g,
                               UI_COLOR(canvas).b);
    }
    a->font_code = a->code_fonts[(p->editor_size - 12) / 2];
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
    SDL_Window *owner=a->primary_window?a->primary_window:a->window;
    SDL_WindowFlags flags = SDL_GetWindowFlags(owner);
    if (!(flags & (SDL_WINDOW_MAXIMIZED | SDL_WINDOW_MINIMIZED))) {
        int w, h;
        if (SDL_GetWindowSize(owner, &w, &h) && w >= 1080 && w <= 8192 && h >= 740 &&
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
static void preferences_theme_tick(app *a) {
    if(!a->theme_request)return;
    unsigned theme=a->theme_request-1;a->theme_request=0;if(theme>=PS_THEME_COUNT)return;
    /* A presentation-only change at a frame boundary. Do not apply a stale
     * full preferences draft over the current workspace or view state. */
    a->preferences.theme=theme;
    ui_palette=theme==PS_THEME_LIGHT?&PS_UI_LIGHT:theme==PS_THEME_HIGH_CONTRAST?&PS_UI_HIGH_CONTRAST:&PS_UI_DARK;
    style(a->ui);nk_style_set_font(a->ui,a->font_ui);
    ps_graphics_background(a->graphics,UI_COLOR(canvas).r,UI_COLOR(canvas).g,UI_COLOR(canvas).b);
    if(a->doc_ui) {
        style(a->doc_ui);nk_style_set_font(a->doc_ui,a->doc_font_ui);
        ps_graphics_background(a->doc_graphics,UI_COLOR(canvas).r,UI_COLOR(canvas).g,UI_COLOR(canvas).b);
        if(a->documentation)a->documentation->layout_width=0;
        if(a->doc_visible && a->doc_keyboard)a->doc_focus_reveal=true;
    }
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
    if(r==PS_OK) {
        ps_dock_layout fixed=ps_dock_fixed_layout(p.dock.hidden);
        if(memcmp(&fixed,&p.dock,sizeof fixed)) {
            char backup[4096];int n=snprintf(backup,sizeof backup,"%s.before-fixed-panels",a->preferences_path);
            if(n>0 && (size_t)n<sizeof backup) {
                FILE *existing=fopen(backup,"rb");
                if(existing)fclose(existing);
                else if(!copy_file_exclusive(a->preferences_path,backup)) {
                    a->preferences_writable=false;
                    snprintf(a->preferences_error,sizeof a->preferences_error,"Die bisherige Panelanordnung konnte nicht gesichert werden. Automatisches Speichern ist ausgesetzt.");
                    preferences_apply(a,&p);return;
                }
            }
        }
    }
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
    layouts_start(a);
    channel_units_start(a);
}
static void open_settings(app *a) {
    if (a->tab == 6)
        return;
    preferences_capture(a);
    a->settings_draft = a->preferences;
    a->settings_previous_tab = a->tab;
    a->settings_from_analysis=a->analysis_layout_active;a->settings_defaults=false;
    a->tab = 6;
    workspace_context(a);
    a->settings_focus=0;a->settings_keyboard=false;a->settings_focus_scroll=false;
}
/* Semantic focus groups preserve the same draft and save transaction as pointer input. */
static void settings_commit(app *a) {
    ps_preferences *p=&a->settings_draft;
    ps_preferences next = *p;
    preferences_capture(a);
    next.width = a->preferences.width;
    next.height = a->preferences.height;
    next.maximized = a->preferences.maximized;
    bool laboratory=a->settings_previous_tab==1 && a->laboratory_initialized;
    bool analysis=a->settings_from_analysis;
    if(!laboratory && !analysis && !a->settings_defaults)next.dock = a->preferences.dock;
    if((next.view_flags&PS_VIEW_LOG) && (next.dock.hidden&(1u<<PS_DOCK_LOG)))
        ps_dock_reveal(&next.dock,PS_DOCK_LOG,PS_DOCK_BOTTOM);
    if(!documentation_fonts_prepare_sizes(a,a->doc_zoom_size?a->doc_zoom_size:next.ui_size,
        a->doc_zoom_size?a->doc_zoom_size:next.editor_size,next.ui_size,next.editor_size)) {
        snprintf(a->preferences_error,sizeof a->preferences_error,"Schriftgröße konnte nicht geladen werden. Einstellungen bleiben erhalten.");
        return;
    }
    /* Workspace widths are session-owned. A global theme/font save must not
     * replace the workbench layout with the caller's temporary geometry. */
    ps_preferences persisted=next;
    if((laboratory || analysis) && !a->settings_defaults) {
        persisted.dock=a->preferences.dock;
        persisted.sidebar_width=a->preferences.sidebar_width;
        persisted.inspector_width=a->preferences.inspector_width;
        persisted.log_height=a->preferences.log_height;
        persisted.view_flags=(persisted.view_flags&~PS_VIEW_LOG)|(a->show_log?PS_VIEW_LOG:0);
    }
    ps_result r =
        a->preferences_path[0] ? ps_preferences_write(a->preferences_path, &persisted) : PS_IO;
    if (r == PS_OK) {
        preferences_apply(a, &persisted);
        /* Apply accepted geometry to its originating workspace on return. */
        if(a->settings_defaults){a->laboratory_initialized=false;a->analysis_layout_initialized=false;}
        if(laboratory && !a->settings_defaults)a->laboratory_layout=(ps_layout_state){next.dock,next.sidebar_width,
            next.inspector_width,next.log_height,(next.view_flags&PS_VIEW_LOG)!=0};
        if(analysis && !a->settings_defaults)a->analysis_layout=(ps_layout_state){next.dock,next.sidebar_width,
            next.inspector_width,next.log_height,(next.view_flags&PS_VIEW_LOG)!=0};
        a->preferences_writable = true;
        a->preferences_error[0] = 0;
        a->tab = a->settings_previous_tab;
        a->settings_keyboard=false;
        status(a, "Einstellungen gespeichert.");
    } else
        snprintf(
            a->preferences_error, sizeof a->preferences_error,
            "Speichern fehlgeschlagen: %s. Deine bisherigen Einstellungen bleiben erhalten.",
            ps_result_string(r));
}
static void settings_defaults_draft(app *a) {
    a->settings_defaults=true;a->settings_draft=PS_PREFERENCES_DEFAULT;
    a->settings_draft.width=a->preferences.width;a->settings_draft.height=a->preferences.height;
}
static bool settings_keyboard_key(app *a,const SDL_KeyboardEvent *key) {
    if(a->tab!=6 || a->project_manager || a->toolbar_menu || a->toolbar_keyboard)return false;
    if(key->mod&(SDL_KMOD_CTRL|SDL_KMOD_GUI|SDL_KMOD_ALT))return false;
    if(key->key==SDLK_ESCAPE) {a->tab=a->settings_previous_tab;a->settings_keyboard=false;return true;}
    if(key->key==SDLK_TAB) {
        int direction=key->mod&SDL_KMOD_SHIFT?-1:1;
        a->settings_focus=a->settings_keyboard?(a->settings_focus+direction+14)%14:(direction>0?0:13);
        a->settings_keyboard=true;a->settings_focus_scroll=true;return true;
    }
    if(!a->settings_keyboard)return false;
    bool activate=key->key==SDLK_RETURN || key->key==SDLK_KP_ENTER || key->key==SDLK_SPACE;
    int direction=key->key==SDLK_LEFT || key->key==SDLK_UP?-1:
                  key->key==SDLK_RIGHT || key->key==SDLK_DOWN?1:0;
    if(!activate && !direction)return true;
    if(activate && key->repeat)return true;
    ps_preferences *p=&a->settings_draft;int focus=a->settings_focus;
    if(focus==1 && activate && !key->repeat)settings_commit(a);
    else if(focus==2 && activate && !key->repeat){a->tab=a->settings_previous_tab;a->settings_keyboard=false;}
    else if(focus==3 && activate && !key->repeat)settings_defaults_draft(a);
    else if(focus==0 || focus==5) {
        uint32_t *size=focus==0?&p->ui_size:&p->editor_size;
        unsigned minimum=focus==0?16:12,count=focus==0?4:6;
        *size=minimum+2*(((*size-minimum)/2+(direction<0?count-1:1))%count);
    } else if(focus==4)p->theme=(p->theme+(direction<0?PS_THEME_COUNT-1:1))%PS_THEME_COUNT;
    else if(focus==6) {
        const uint32_t seconds[]={10,30,60,120};unsigned selected=0;
        while(selected<3 && seconds[selected]!=p->autosave_seconds)selected++;
        p->autosave_seconds=seconds[(selected+(direction<0?3:1))%4];
    } else if(focus>=7 && activate && !key->repeat) {
        const uint32_t flags[]={PS_VIEW_VECTORS,PS_VIEW_PATHS,PS_VIEW_POINTS,PS_VIEW_LABELS,PS_VIEW_GRID,PS_VIEW_ORTHOGRAPHIC,PS_VIEW_LOG};
        p->view_flags^=flags[focus-7];
    }
    return true;
}
static void settings_focus_draw(app *a,int group,struct nk_rect bounds) {
    struct nk_context *ui=a->ui;
    a->settings_focus_bounds[group]=bounds;
    if(nk_input_is_mouse_pressed(&ui->input,NK_BUTTON_LEFT) && nk_input_is_mouse_hovering_rect(&ui->input,bounds)) {
        a->settings_keyboard=false;a->settings_focus=group;
    }
    if(!a->settings_keyboard || a->settings_focus!=group)return;
    nk_stroke_rect(nk_window_get_canvas(ui),bounds,3,2,UI_COLOR(accent));
    if(a->settings_focus_scroll) {
        struct nk_rect panel=a->dock_panels[PS_DOCK_WORKSPACE];
        float top=panel.y+30,bottom=panel.y+panel.h-10,delta=0;
        if(bounds.y<top)delta=bounds.y-top;
        else if(bounds.y+bounds.h>bottom)delta=bounds.y+bounds.h-bottom;
        nk_uint x,y;nk_window_get_scroll(ui,&x,&y);
        if(delta)nk_window_set_scroll(ui,x,(nk_uint)fmaxf(0,(float)y+delta));
        else a->settings_focus_scroll=false;
    }
}
static void settings_flag(app *a,int group,const char *label,uint32_t flag,uint32_t *mask) {
    struct nk_context *ui=a->ui;struct nk_rect bounds=nk_widget_bounds(ui);
    bool value = (*mask & flag) != 0;
    nk_checkbox_label(ui, label, &value);
    *mask = value ? *mask | flag : *mask & ~flag;
    settings_focus_draw(a,group,bounds);
}
static void settings_view(app *a) {
    struct nk_context *ui = a->ui;
    ps_preferences *p = &a->settings_draft;
    nk_style_push_font(ui, a->font_title);
    nk_layout_row_dynamic(ui, 34, 1);
    nk_label(ui, "Einstellungen", NK_TEXT_LEFT);
    nk_style_pop_font(ui);
    nk_layout_row_dynamic(ui,22,1);
    nk_label_colored(ui,"Schriftgröße der Oberfläche",NK_TEXT_LEFT,muted());
    nk_layout_row_dynamic(ui,28,4);
    struct nk_rect ui_group=nk_widget_bounds(ui);
    const char *ui_sizes[]={"16 px","18 px","20 px","22 px"};
    for(unsigned i=0;i<4;i++) {
        a->ui_size_bounds[i]=nk_widget_bounds(ui);
        if(ps_ui_option_label(ui,"Schriftgröße der Oberfläche",ui_sizes[i],p->ui_size==16+i*2))p->ui_size=16+i*2;
    }
    ui_group.w=4*ui_group.w+3*ui->style.window.spacing.x;settings_focus_draw(a,0,ui_group);
    nk_layout_row_dynamic(ui, 36, a->preferences.ui_size>=20?2:3);
    a->settings_bounds[0] = nk_widget_bounds(ui);
    if (action_button(ui, "Übernehmen", true)) {
        settings_commit(a);
    }
    settings_focus_draw(a,1,a->settings_bounds[0]);
    a->settings_bounds[4] = nk_widget_bounds(ui);
    if (nk_button_label(ui, "Abbrechen"))
        a->tab = a->settings_previous_tab;
    settings_focus_draw(a,2,a->settings_bounds[4]);
    a->settings_bounds[5] = nk_widget_bounds(ui);
    if (nk_button_label(ui, "Standardwerte"))settings_defaults_draft(a);
    settings_focus_draw(a,3,a->settings_bounds[5]);
    nk_layout_row_dynamic(ui, 28, 1);
    nk_label_wrap(ui,
                  "Deine Darstellung und Fensteraufteilung bleiben beim nächsten Start erhalten.");
    nk_layout_row_dynamic(ui, 22, 1);
    nk_label_colored(ui, "Darstellung", NK_TEXT_LEFT, muted());
    nk_layout_row_dynamic(ui, 28, a->preferences.ui_size>=20?2:3);
    const char *themes[] = {"Dunkel", "Hell", "Hoher Kontrast"};
    for (unsigned i = 0; i < PS_THEME_COUNT; i++) {
        a->theme_bounds[i] = nk_widget_bounds(ui);
        if (ps_ui_option_label(ui,"Darstellung", themes[i], p->theme == i))
            p->theme = i;
        if(p->theme==i)settings_focus_draw(a,4,a->theme_bounds[i]);
    }
    nk_layout_row_dynamic(ui, 22, 1);
    nk_label_colored(ui, "Schriftgröße im Code-Editor", NK_TEXT_LEFT, muted());
    nk_layout_row_dynamic(ui, 28, 6);
    const char *sizes[] = {"12 px", "14 px", "16 px", "18 px", "20 px", "22 px"};
    for (unsigned i = 0; i < 6; i++) {
        if (i == 4)
            a->settings_bounds[1] = nk_widget_bounds(ui);
        struct nk_rect bounds=nk_widget_bounds(ui);
        if (ps_ui_option_label(ui,"Schriftgröße im Code-Editor", sizes[i], p->editor_size == 12 + i * 2))
            p->editor_size = 12 + i * 2;
        if(p->editor_size==12+i*2)settings_focus_draw(a,5,bounds);
    }
    nk_style_push_font(ui, a->code_fonts[(p->editor_size - 12) / 2]);
    nk_layout_row_dynamic(ui, 30, 1);
    nk_label(ui, "double energy = 0.5 * mass * speed * speed;", NK_TEXT_LEFT);
    nk_style_pop_font(ui);
    nk_layout_row_dynamic(ui, 22, 1);
    nk_label_colored(ui, "Automatische Sicherung", NK_TEXT_LEFT, muted());
    nk_layout_row_dynamic(ui, 30, a->preferences.ui_size>=20?1:2);
    nk_label(ui, "Ungespeicherte Quellen sichern", NK_TEXT_LEFT);
    const char *intervals[] = {"Alle 10 Sekunden", "Alle 30 Sekunden", "Jede Minute",
                               "Alle 2 Minuten"};
    const uint32_t seconds[] = {10, 30, 60, 120};
    int selected = 0;
    for (int i = 0; i < 4; i++)
        if (p->autosave_seconds == seconds[i])
            selected = i;
    struct nk_rect interval_bounds=nk_widget_bounds(ui);
    p->autosave_seconds = seconds[nk_combo(ui, intervals, 4, selected, 30, nk_vec2(250, 180))];
    settings_focus_draw(a,6,interval_bounds);
    nk_layout_row_dynamic(ui, 22, 1);
    nk_label_colored(ui, "Simulation und Arbeitsfläche", NK_TEXT_LEFT, muted());
    nk_layout_row_dynamic(ui, 24, a->preferences.ui_size>=20?1:2);
    settings_flag(a, 7, "Vektoren", PS_VIEW_VECTORS, &p->view_flags);
    settings_flag(a, 8, "Flugbahnen", PS_VIEW_PATHS, &p->view_flags);
    settings_flag(a, 9, "Punkte", PS_VIEW_POINTS, &p->view_flags);
    settings_flag(a, 10, "Beschriftungen", PS_VIEW_LABELS, &p->view_flags);
    a->settings_bounds[2] = nk_widget_bounds(ui);
    settings_flag(a, 11, "Raster und Achsen", PS_VIEW_GRID, &p->view_flags);
    settings_flag(a, 12, "Orthografische Ansicht", PS_VIEW_ORTHOGRAPHIC, &p->view_flags);
    a->settings_bounds[3] = nk_widget_bounds(ui);
    settings_flag(a, 13, "Protokoll geöffnet", PS_VIEW_LOG, &p->view_flags);
    nk_spacer(ui);
    nk_layout_row_dynamic(ui, 28, 1);
    nk_label_wrap(ui,
                  "Panels an Titelzeilen ziehen; Trennlinien ändern die Größe.");
    if (a->preferences_error[0]) {
        nk_layout_row_dynamic(ui, 64, 1);
        nk_label_wrap(ui, a->preferences_error);
    }
}
