static void toolbar_test_frame(app *a, int *stage, int *code, const char **capture) {
    static double focus_ready;
    bool ok = true;
    switch ((*stage)++) {
    case 0:
        focus_ready = ps_clock() + .2;
        break;
    case 1: {
        if (ps_clock() < focus_ready) {
            --*stage;
            break;
        }
        int width, height;
        SDL_GetWindowSize(a->window, &width, &height);
        ok = a->toolbar_bounds[0].h <= 32 && a->toolbar_bounds[0].y <= 1 &&
             a->navigation_bounds[0].h <= 30 &&
             a->navigation_bounds[0].y >= a->toolbar_bounds[0].y + a->toolbar_bounds[0].h &&
             a->navigation_bounds[0].y == a->navigation_bounds[2].y &&
             fabsf(a->navigation_bounds[0].x) <= 1 &&
             fabsf(a->navigation_bounds[0].w - (float)width / 3) <= 1 &&
             fabsf(a->navigation_bounds[2].x + a->navigation_bounds[2].w - width) <= 1 &&
             (SDL_GetWindowFlags(a->window) & SDL_WINDOW_BORDERLESS);
        SDL_Point drag = {(int)(a->window_drag_bounds.x + 20), 15};
        SDL_Point menu = {(int)a->toolbar_bounds[0].x + 10, 15};
        SDL_Point control = {(int)a->window_control_bounds[1].x + 20, 15};
        SDL_Point edge = {1, height / 2};
        ok = ok && window_hit_test(a->window, &drag, a) == SDL_HITTEST_DRAGGABLE &&
             window_hit_test(a->window, &menu, a) == SDL_HITTEST_NORMAL &&
             window_hit_test(a->window, &control, a) == SDL_HITTEST_NORMAL &&
             window_hit_test(a->window, &edge, a) == SDL_HITTEST_RESIZE_LEFT;
        *capture = "toolbar-tabs.bmp";
        test_mouse(a, a->toolbar_bounds[0], true);
        break;
    }
    case 2:
        test_mouse(a, a->toolbar_bounds[0], false);
        break;
    case 4:
        ok = a->toolbar_item_bounds[3].w > 0;
        *capture = "toolbar-file-menu.bmp";
        test_mouse(a, a->toolbar_item_bounds[3], true);
        break;
    case 5:
        test_mouse(a, a->toolbar_item_bounds[3], false);
        break;
    case 7:
        ok = a->project_manager && !a->loaded;
        test_mouse(a, a->navigation_bounds[1], true);
        break;
    case 8:
        test_mouse(a, a->navigation_bounds[1], false);
        break;
    case 10:
        ok = a->tab == 1 && !a->project_manager;
        test_mouse(a, a->navigation_bounds[2], true);
        break;
    case 11:
        test_mouse(a, a->navigation_bounds[2], false);
        break;
    case 13:
        ok = a->tab == 2;
        *stage = 60;
        break;
    case 60:
        test_mouse(a, a->toolbar_bounds[0], true);
        break;
    case 61:
        test_mouse(a, a->toolbar_bounds[0], false);
        break;
    case 63:
        ok = a->toolbar_menu == 1;
        test_mouse(a, a->toolbar_bounds[1], true);
        break;
    case 64:
        test_mouse(a, a->toolbar_bounds[1], false);
        break;
    case 66:
        ok = a->toolbar_menu == 2;
        *capture = "toolbar-view-selected.bmp";
        test_mouse(a, a->toolbar_bounds[0], true);
        break;
    case 67:
        test_mouse(a, a->toolbar_bounds[0], false);
        break;
    case 69:
        ok = a->toolbar_menu == 1;
        *stage = 80;
        break;
    case 80: {
        SDL_Event event = {0};
        event.type = SDL_EVENT_MOUSE_MOTION;
        event.motion.windowID = SDL_GetWindowID(a->window);
        event.motion.which = PS_TEST_MOUSE_ID;
        event.motion.x = a->toolbar_bounds[1].x + 20;
        event.motion.y = a->toolbar_bounds[1].y + 16;
        SDL_PushEvent(&event);
        break;
    }
    case 82:
        ok = a->toolbar_menu == 2;
        *capture = "toolbar-hover-switch.bmp";
        test_mouse(a, a->toolbar_bounds[3], true);
        *stage = 70;
        break;
    case 70:
        test_mouse(a, a->toolbar_bounds[3], false);
        break;
    case 72:
        ok = !a->toolbar_menu && a->tab == 6;
        *capture = "toolbar-settings-selected.bmp";
        test_mouse(a, a->toolbar_bounds[0], true);
        break;
    case 73:
        test_mouse(a, a->toolbar_bounds[0], false);
        break;
    case 75:
        ok = a->toolbar_menu == 1;
        test_mouse(a, a->toolbar_bounds[2], true);
        break;
    case 76:
        test_mouse(a, a->toolbar_bounds[2], false);
        break;
    case 78:
        ok = !a->toolbar_menu && a->doc_visible;
        documentation_window_hide(a);
        SDL_RaiseWindow(a->window);
        focus_ready = ps_clock() + .2;
        break;
    case 79:
        if (ps_clock() < focus_ready) {
            --*stage;
            break;
        }
        test_mouse(a, a->toolbar_bounds[1], true);
        *stage = 14;
        break;
    case 14:
        test_mouse(a, a->toolbar_bounds[1], false);
        break;
    case 16:
        ok = !a->show_log && a->toolbar_item_bounds[5].w > 0;
        test_mouse(a, a->toolbar_item_bounds[5], true);
        break;
    case 17:
        test_mouse(a, a->toolbar_item_bounds[5], false);
        break;
    case 19:
        ok = a->show_log;
        test_mouse(a, a->toolbar_bounds[3], true);
        break;
    case 20:
        test_mouse(a, a->toolbar_bounds[3], false);
        break;
    case 22:
        ok = a->tab == 6;
        test_key(a, SDLK_1);
        break;
    case 24:
        ok = a->tab == 0 && !a->project_manager && !a->dialog_pending;
        test_mouse(a, a->toolbar_bounds[0], true);
        break;
    case 25:
        test_mouse(a, a->toolbar_bounds[0], false);
        break;
    case 27:
        test_mouse(a, a->toolbar_item_bounds[1], true);
        break;
    case 28:
        test_mouse(a, a->toolbar_item_bounds[1], false);
        break;
    case 30:
        ok = !a->dialog_pending && !a->workspace_open;
        a->show_log = false;
        test_mouse(a, nk_rect(950, 710, 1, 1), true);
        break;
    case 31:
        test_mouse(a, nk_rect(950, 710, 1, 1), false);
        break;
    case 33:
        *capture = "toolbar-final.bmp";
        test_mouse(a, a->window_control_bounds[1], true);
        break;
    case 34:
        test_mouse(a, a->window_control_bounds[1], false);
        break;
    case 38:
        if (!(SDL_GetWindowFlags(a->window) & SDL_WINDOW_MAXIMIZED)) {
            --*stage;
            break;
        }
        *capture = "toolbar-maximized.bmp";
        test_mouse(a, a->window_control_bounds[1], true);
        break;
    case 39:
        test_mouse(a, a->window_control_bounds[1], false);
        break;
    case 43:
        if (SDL_GetWindowFlags(a->window) & SDL_WINDOW_MAXIMIZED) {
            --*stage;
            break;
        }
        test_mouse(a, a->window_control_bounds[0], true);
        break;
    case 44:
        test_mouse(a, a->window_control_bounds[0], false);
        break;
    case 48:
        if (!(SDL_GetWindowFlags(a->window) & SDL_WINDOW_MINIMIZED)) {
            --*stage;
            break;
        }
        SDL_RestoreWindow(a->window);
        SDL_RaiseWindow(a->window);
        focus_ready = ps_clock() + .2;
        break;
    case 53:
        if (ps_clock() < focus_ready) {
            --*stage;
            break;
        }
        if (SDL_GetWindowFlags(a->window) & SDL_WINDOW_MINIMIZED) {
            --*stage;
            break;
        }
        test_mouse(a, a->toolbar_bounds[0], true);
        break;
    case 54:
        test_mouse(a, a->toolbar_bounds[0], false);
        break;
    case 56:
        ok = a->toolbar_menu == 1;
        test_mouse(a, a->window_control_bounds[2], true);
        break;
    case 57:
        test_mouse(a, a->window_control_bounds[2], false);
        break;
    case 59:
        ok = false; /* The close button should have sent the normal close event. */
        break;
    }
    if (!ok) {
        *code = 1;
        a->quitting = true;
    }
    if (a->quitting)
        printf("TOOLBAR AND TABS: %s (stage %d, tab x/w %.1f/%.1f, drag %.1f/%.1f)\n",
               *code ? "FAILED" : "PASSED", *stage, a->navigation_bounds[0].x,
               a->navigation_bounds[0].w, a->window_drag_bounds.x, a->window_drag_bounds.w);
}
