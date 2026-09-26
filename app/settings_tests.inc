static void test_settings_frame(app *a, const char *mode, int *stage, int *code,
                                const char **capture) {
    bool writing = !strcmp(mode, "write"), resetting = !strcmp(mode, "reset");
    bool ok = true;
    static struct nk_rect drag;
    if (!strcmp(mode, "maxwrite") || !strcmp(mode, "maxread")) {
        if (*stage == 0 && !strcmp(mode, "maxwrite"))
            SDL_MaximizeWindow(a->window);
        if (!(SDL_GetWindowFlags(a->window) & SDL_WINDOW_MAXIMIZED))
            return;
        ok = a->preferences.width == 1200 && a->preferences.height == 800;
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
            ok = w == 1200 && h == 800;
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
            ok = a->settings_draft.editor_size == 20 && a->preferences.editor_size == 16;
            a->settings_draft.autosave_seconds = 60;
            test_mouse(a, a->settings_bounds[2], true);
            break;
        case 6:
            test_mouse(a, a->settings_bounds[2], false);
            break;
        case 8:
            test_mouse(a, a->settings_bounds[3], true);
            break;
        case 9:
            test_mouse(a, a->settings_bounds[3], false);
            break;
        case 11:
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
