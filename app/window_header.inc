/* SDL forwards these regions to the OS for moving and resizing the window. */
static SDL_HitTestResult SDLCALL window_hit_test(SDL_Window *window, const SDL_Point *point,
                                                 void *userdata) {
    app *a = userdata;
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    int x = point->x, y = point->y;
    if (!(SDL_GetWindowFlags(window) & SDL_WINDOW_MAXIMIZED)) {
        const int edge = 5;
        if (y < edge) {
            if (x < edge)
                return SDL_HITTEST_RESIZE_TOPLEFT;
            if (x >= width - edge)
                return SDL_HITTEST_RESIZE_TOPRIGHT;
            return SDL_HITTEST_RESIZE_TOP;
        }
        if (y >= height - edge) {
            if (x < edge)
                return SDL_HITTEST_RESIZE_BOTTOMLEFT;
            if (x >= width - edge)
                return SDL_HITTEST_RESIZE_BOTTOMRIGHT;
            return SDL_HITTEST_RESIZE_BOTTOM;
        }
        if (x < edge)
            return SDL_HITTEST_RESIZE_LEFT;
        if (x >= width - edge)
            return SDL_HITTEST_RESIZE_RIGHT;
    }
    struct nk_rect drag = a->window_drag_bounds;
    if (x >= drag.x && x < drag.x + drag.w && y >= drag.y && y < drag.y + drag.h)
        return SDL_HITTEST_DRAGGABLE;
    return SDL_HITTEST_NORMAL;
}
static void window_control(app *a, unsigned control) {
    struct nk_context *ui = a->ui;
    struct nk_rect bounds = nk_widget_bounds(ui);
    a->window_control_bounds[control] = bounds;
    nk_style_push_float(ui, &ui->style.button.rounding, 0);
    nk_style_push_float(ui, &ui->style.button.border, 0);
    if (control == 2) {
        nk_style_push_style_item(ui, &ui->style.button.hover,
                                 nk_style_item_color(nk_rgb(180, 50, 54)));
        nk_style_push_style_item(ui, &ui->style.button.active,
                                 nk_style_item_color(nk_rgb(150, 40, 44)));
    }
    bool clicked = nk_button_label(ui, "");
    /* Caption actions must also work while a menu owns Nuklear's widget focus. */
    clicked = clicked || (nk_input_is_mouse_pressed(&ui->input, NK_BUTTON_LEFT) &&
                          nk_input_is_mouse_hovering_rect(&ui->input, bounds));
    if (control == 2) {
        nk_style_pop_style_item(ui);
        nk_style_pop_style_item(ui);
    }
    nk_style_pop_float(ui);
    nk_style_pop_float(ui);
    struct nk_command_buffer *canvas = nk_window_get_canvas(ui);
    float x = floorf(bounds.x + bounds.w / 2), y = floorf(bounds.y + bounds.h / 2);
    struct nk_color color = UI_COLOR(text);
    if (control == 2 && (ui->last_widget_state & NK_WIDGET_STATE_HOVER))
        color = nk_rgb(255, 255, 255);
    if (control == 0)
        nk_stroke_line(canvas, x - 5, y + 3, x + 5, y + 3, 1, color);
    else if (control == 1) {
        bool maximized = (SDL_GetWindowFlags(a->window) & SDL_WINDOW_MAXIMIZED) != 0;
        if (maximized)
            nk_stroke_rect(canvas, nk_rect(x - 2, y - 5, 8, 8), 0, 1, color);
        nk_fill_rect(canvas, nk_rect(x - 5, y - 3, 9, 9), 0, UI_COLOR(header));
        nk_stroke_rect(canvas, nk_rect(x - 5, y - 3, 9, 9), 0, 1, color);
    } else {
        nk_stroke_line(canvas, x - 4, y - 4, x + 4, y + 4, 1, color);
        nk_stroke_line(canvas, x - 4, y + 4, x + 4, y - 4, 1, color);
    }
    if (!ui->current->popup.active && !a->toolbar_menu &&
        nk_input_is_mouse_hovering_rect(&ui->input, bounds))
        nk_tooltip(ui, control == 0   ? "Minimieren"
                       : control == 1 ? "Maximieren / Wiederherstellen"
                                      : "Schließen");
    if (!clicked)
        return;
    if (control == 0)
        SDL_MinimizeWindow(a->window);
    else if (control == 1) {
        if (SDL_GetWindowFlags(a->window) & SDL_WINDOW_MAXIMIZED)
            SDL_RestoreWindow(a->window);
        else
            SDL_MaximizeWindow(a->window);
    } else {
        /* Use the normal close path so unsaved sources are preserved on failure. */
        SDL_Event event = {0};
        event.type = SDL_EVENT_WINDOW_CLOSE_REQUESTED;
        event.window.windowID = SDL_GetWindowID(a->window);
        SDL_PushEvent(&event);
    }
}
