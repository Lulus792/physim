static bool settings_test_reveal(app *a,struct nk_rect bounds) {
    struct nk_rect panel=a->dock_panels[PS_DOCK_WORKSPACE];
    float top=panel.y+30,bottom=panel.y+panel.h-10;
    if(bounds.y>=top && bounds.y+bounds.h<=bottom)return true;
    test_plot_wheel(a,panel.x+panel.w*.5f,panel.y+panel.h*.5f,bounds.y<top?3:-3);
    return false;
}
static void test_theme_frame(app *a, const char *mode, int *stage, int *code,
                             const char **capture) {
    bool reading = strstr(mode, "-read") != NULL, cancel = !strcmp(mode, "cancel");
    bool defaults = !strcmp(mode, "defaults");
    unsigned target = !strncmp(mode, "light", 5) ? PS_THEME_LIGHT
                      : !strcmp(mode, "dark-read") || defaults ? PS_THEME_DARK
                                                              : PS_THEME_HIGH_CONTRAST;
    const ps_ui_palette *expected = target == PS_THEME_LIGHT ? &PS_UI_LIGHT
                                   : target == PS_THEME_HIGH_CONTRAST ? &PS_UI_HIGH_CONTRAST
                                                                      : &PS_UI_DARK;
    bool ok = true;
    static uint32_t previous;
    static char name[80];
    switch (*stage) {
    case 0:
        SDL_SetWindowSize(a->window, 1080, 740);
        previous = a->preferences.theme;
        open_documentation(a, 8);
        ok = a->doc_visible && a->documentation && !a->doc_error[0];
        ok &= ps_report_create("Darstellung", "Kurven und Achsen", &a->analysis_report) == PS_OK;
        if (ok) {
            ps_plot_info info = {0};
            strcpy(info.title, "Diagramm in der gewählten Darstellung");
            strcpy(info.x_label, "Zeit");
            strcpy(info.y_label, "Position");
            ps_report_unit_from(PS_SECOND, &info.x_unit);
            ps_report_unit_from(PS_METRE, &info.y_unit);
            ps_plot_handle handle;
            ok = ps_report_add_plot(a->analysis_report, &info, &handle) == PS_OK;
            ps_curve_data *curve = calloc(1, sizeof *curve);
            ok &= curve != NULL;
            for (unsigned j = 0; ok && j < 3; j++) {
                snprintf(curve->label, sizeof curve->label, "Messreihe %u", j + 1);
                curve->kind = PS_PLOT_LINE;
                curve->count = curve->source_count = 25;
                for (unsigned i = 0; i < curve->count; i++) {
                    curve->x[i] = i * .2;
                    curve->y[i] = sin(i * .3 + j);
                }
                ok = ps_report_add_curve(a->analysis_report, handle, curve) == PS_OK;
            }
            free(curve);
        }
        if (reading)
            ok &= previous == target;
        a->tab = 0;
        open_settings(a);
        break;
    case 2:
        test_mouse(a, defaults ? a->settings_bounds[5]
                               : a->theme_bounds[cancel ? PS_THEME_LIGHT : target], true);
        break;
    case 3:
        test_mouse(a, defaults ? a->settings_bounds[5]
                               : a->theme_bounds[cancel ? PS_THEME_LIGHT : target], false);
        break;
    case 5:
        ok = a->settings_draft.theme == (cancel ? PS_THEME_LIGHT : target) &&
             a->preferences.theme == previous;
        test_mouse(a, a->settings_bounds[cancel || reading ? 4 : 0], true);
        break;
    case 6:
        test_mouse(a, a->settings_bounds[cancel || reading ? 4 : 0], false);
        break;
    case 8:
        ok = a->tab == 0 && a->preferences.theme == (cancel ? previous : target) &&
             ui_palette == expected && a->ui->style.font == a->font_ui &&
             a->doc_ui->style.font == a->doc_font_ui &&
             !memcmp(&a->ui->style.text.color, &expected->text, sizeof expected->text) &&
             !memcmp(&a->doc_ui->style.text.color, &expected->text, sizeof expected->text);
        snprintf(name, sizeof name, "theme-%s.bmp", mode);
        *capture = name;
        break;
    case 9:
        snprintf(name, sizeof name, "docs-theme-%s.bmp", mode);
        *capture = name;
        break;
    case 10:
        a->tab = 2;
        a->show_report = true;
        break;
    case 11:
        snprintf(name, sizeof name, "theme-plot-%s.bmp", mode);
        *capture = name;
        break;
    case 12:
        a->tab = 0;
        open_settings(a);
        break;
    case 14:
        snprintf(name, sizeof name, "theme-settings-%s.bmp", mode);
        *capture = name;
        break;
    case 15:
        a->quitting = true;
        break;
    }
    if (!ok) {
        fprintf(stderr, "Theme %s stage %d: stored=%u draft=%u target=%u\n", mode, *stage,
                a->preferences.theme, a->settings_draft.theme, target);
        *code = 1;
        a->quitting = true;
    }
    if (a->quitting)
        printf("THEME %s SELF-TEST: %s\n", mode, *code ? "FAILED" : "PASSED");
    (*stage)++;
}
static void test_settings_frame(app *a, const char *mode, int *stage, int *code,
                               const char **capture) {
    if(!strncmp(mode,"ui-size-",8)) {
        test_ui_size_frame(a,mode,stage,code,capture);return;
    }
    if (!strncmp(mode, "theme-", 6)) {
        test_theme_frame(a, mode + 6, stage, code, capture);
        return;
    }
    bool writing = !strcmp(mode, "write"), resetting = !strcmp(mode, "reset");
    bool ok = true;
    static struct nk_rect drag;
    /* Restored windows fit the usable desktop, including menu bar and Dock.
     * The test requests 1200x800 but hosted Macs may expose a smaller desktop. */
    int restored_width = 1200, restored_height = 800;
    SDL_Rect usable;
    if (SDL_GetDisplayUsableBounds(SDL_GetDisplayForWindow(a->window), &usable)) {
        if (usable.w < restored_width)
            restored_width = usable.w < 1080 ? 1080 : usable.w;
        if (usable.h < restored_height)
            restored_height = usable.h < 740 ? 740 : usable.h;
    }
    if (!strcmp(mode, "maxwrite") || !strcmp(mode, "maxread")) {
        if (*stage == 0 && !strcmp(mode, "maxwrite"))
            SDL_MaximizeWindow(a->window);
        if (!(SDL_GetWindowFlags(a->window) & SDL_WINDOW_MAXIMIZED))
            return;
        ok = a->preferences.width == (uint32_t)restored_width &&
             a->preferences.height == (uint32_t)restored_height;
        if (!strcmp(mode, "maxread"))
            ok &= a->preferences.maximized == 1;
        if (*stage == 3)
            a->quitting = true;
    } else if (!strcmp(mode, "corrupt")) {
        if (*stage == 0) {
            ok = !a->preferences_writable && a->preferences_error[0] &&
                 a->preferences.editor_size == 16;
            test_key(a, SDLK_COMMA);
        } else if (*stage == 3) {
            *capture = "settings-corrupt.bmp";
        } else if (*stage == 4)
            a->quitting = true;
    } else if (!strcmp(mode, "defaults")) {
        ok = a->preferences.editor_size == 16 && a->preferences.autosave_seconds == 30 &&
             a->preferences.sidebar_width == 248 && a->preferences.log_height == 178 &&
             a->preferences.view_flags == 31;
        if (*stage == 3)
            a->quitting = true;
    } else if (!writing) {
        switch (*stage) {
        case 0:
            ok = a->preferences.editor_size == 20 && a->font_code->height == 20 &&
                 a->preferences.autosave_seconds == 60 && a->preferences.sidebar_width == 312 &&
                 a->preferences.log_height == 238 && a->preferences.view_flags == 79 &&
                 a->tab == 1 && a->view_disclosure == NK_MAXIMIZED;
            test_key(a, SDLK_COMMA);
            break;
        case 2: {
            int w, h;
            SDL_GetWindowSize(a->window, &w, &h);
            ok = w == restored_width && h == restored_height;
            if (!ok)
                fprintf(stderr, "Restored window: %dx%d; expected %dx%d on this desktop\n",
                        w, h, restored_width, restored_height);
        }
            *capture = "settings-restored.bmp";
            test_mouse(a, a->settings_bounds[5], true);
            break;
        case 3:
            test_mouse(a, a->settings_bounds[5], false);
            break;
        case 5:
            ok = a->settings_draft.editor_size == 16 && a->preferences.editor_size == 20;
            test_mouse(a, a->settings_bounds[resetting ? 0 : 4], true);
            break;
        case 6:
            test_mouse(a, a->settings_bounds[resetting ? 0 : 4], false);
            break;
        case 8:
            ok = a->tab == 1 && a->preferences.editor_size == (resetting ? 16u : 20u);
            break;
        case 9:
            a->quitting = true;
            break;
        }
    } else {
        switch (*stage) {
        case 0:
            SDL_SetWindowSize(a->window, 1080, 740);
            test_key(a, SDLK_COMMA);
            break;
        case 2:
            ok = a->tab == 6;
            test_mouse(a, a->settings_bounds[1], true);
            break;
        case 3:
            test_mouse(a, a->settings_bounds[1], false);
            break;
        case 5:
            if(!settings_test_reveal(a,a->settings_bounds[2]))return;
            ok = a->settings_draft.editor_size == 20 && a->preferences.editor_size == 16;
            a->settings_draft.autosave_seconds = 60;
            test_mouse(a, a->settings_bounds[2], true);
            break;
        case 6:
            test_mouse(a, a->settings_bounds[2], false);
            break;
        case 8:
            if(!settings_test_reveal(a,a->settings_bounds[3]))return;
            test_mouse(a, a->settings_bounds[3], true);
            break;
        case 9:
            test_mouse(a, a->settings_bounds[3], false);
            break;
        case 11:
            if(!settings_test_reveal(a,a->settings_bounds[0]))return;
            *capture = "settings-draft.bmp";
            test_mouse(a, a->settings_bounds[0], true);
            break;
        case 12:
            test_mouse(a, a->settings_bounds[0], false);
            break;
        case 14:
            ok = a->tab == 0 && a->preferences.editor_size == 20 && a->font_code->height == 20 &&
                 a->preferences.autosave_seconds == 60 && !a->show_grid && a->show_log;
            *capture = "settings-editor.bmp";
            SDL_SetWindowSize(a->window, 1200, 800);
            break;
        case 15:
            drag = a->panel_bounds[0];
            test_mouse(a, drag, true);
            break;
        case 16:
            test_plot_motion(a, drag.x + drag.w * .5f + 64, drag.y + drag.h * .5f);
            break;
        case 17:
            test_mouse(a, drag, false);
            break;
        case 19:
            ok = a->preferences.sidebar_width == 312;
            drag = a->panel_bounds[1];
            test_mouse(a, drag, true);
            break;
        case 20:
            test_plot_motion(a, drag.x + drag.w * .5f, drag.y + drag.h * .5f - 60);
            break;
        case 21:
            test_mouse(a, drag, false);
            break;
        case 23:
            ok = a->preferences.log_height == 238;
            a->view_disclosure = NK_MAXIMIZED;
            test_key(a, SDLK_2);
            break;
        case 25:
            ok = a->tab == 1 && a->preferences.width == 1200 && a->preferences.height == 800;
            *capture = "settings-layout.bmp";
            {
                a->saved_source[0] = calloc(1, 1);
                a->saved_source[1] = calloc(1, 1);
                a->dirty = true;
                a->autosave_due = 10000;
                char path[4096];
                autosave_path(a, path, sizeof path);
                autosave_tick(a, 9999);
                ok &= !exists(path);
                autosave_tick(a, 10000);
                ps_autosave *snapshot = NULL;
                ok &= a->autosave_due == 10060 && ps_autosave_read(path, &snapshot) == PS_OK;
                ps_autosave_destroy(snapshot);
                a->dirty = false;
            }
            break;
        case 26:
            a->quitting = true;
            break;
        }
    }
    if (!ok) {
        fprintf(stderr, "Settings %s stage %d: tab=%d font=%u draft=%u flags=%u side=%u log=%u\n",
                mode, *stage, a->tab, a->preferences.editor_size, a->settings_draft.editor_size,
                a->preferences.view_flags, a->preferences.sidebar_width, a->preferences.log_height);
        *code = 1;
        a->quitting = true;
    }
    if (a->quitting)
        printf("SETTINGS %s SELF-TEST: %s\n", mode, *code ? "FAILED" : "PASSED");
    (*stage)++;
}
