/* Independent native window: its input, fonts and GL resources belong to this viewer. */
static void documentation_window_destroy(app *a) {
    for (int i = 0; i < DOCUMENTATION_TOPIC_COUNT; i++) {
        free(documentation_index[i]);
        documentation_index[i] = NULL;
    }
    if (a->doc_graphics) {
        ps_graphics_make_current(a->doc_graphics);
        nk_sdl_shutdown(a->doc_ui);
        ps_graphics_destroy(a->doc_graphics);
    }
    if (a->doc_window)
        SDL_DestroyWindow(a->doc_window);
    a->doc_window = NULL;
    a->doc_graphics = NULL;
    a->doc_ui = NULL;
    a->doc_visible = false;
    ps_graphics_make_current(a->graphics);
}

static bool documentation_window_open(app *a) {
    if (!a->doc_window) {
        a->doc_window = SDL_CreateWindow("Physim | Dokumentation", 1080, 780,
                                         SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY |
                                             SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
        if (!a->doc_window)
            goto fail;
        SDL_SetWindowMinimumSize(a->doc_window, 760, 540);
        a->doc_graphics = ps_graphics_create(a->doc_window);
        if (!a->doc_graphics)
            goto fail;
        ps_graphics_background(a->doc_graphics, UI_COLOR(canvas).r, UI_COLOR(canvas).g,
                               UI_COLOR(canvas).b);
        /* The main window already paces rendering through vsync. */
        SDL_GL_SetSwapInterval(0);
        a->doc_ui = nk_sdl_init(a->doc_window, a->doc_graphics);
        if (!a->doc_ui)
            goto fail;
        struct nk_font_atlas *atlas = nk_sdl_font_stash_begin(a->doc_ui);
        for(int i=0;i<4;i++) {
            struct nk_font *font=system_font(atlas,(float)(16+i*2),false,false);
            struct nk_font *title=system_font(atlas,(float)(23+i*2),false,true);
            if(!font || !title)goto fail;
            a->doc_ui_fonts[i]=&font->handle;a->doc_title_fonts[i]=&title->handle;
        }
        a->doc_font_ui=a->doc_ui_fonts[(a->preferences.ui_size-16)/2];
        a->doc_font_title=a->doc_title_fonts[(a->preferences.ui_size-16)/2];
        for (int i = 0; i < 4; i++) {
            struct nk_font *code = system_font(atlas, (float)(16 + i * 2), true, false);
            if (!code)
                goto fail;
            a->doc_code_fonts[i] = &code->handle;
        }
        if (!nk_sdl_font_stash_end(a->doc_ui))
            goto fail;
        style(a->doc_ui);
        nk_style_set_font(a->doc_ui, a->doc_font_ui);
        nk_sdl_set_ui_size(a->doc_ui,a->preferences.ui_size);
        a->doc_reset_sidebar = true;
        int x, y, w, h;
        SDL_Rect usable;
        if (SDL_GetWindowPosition(a->window, &x, &y) && SDL_GetWindowSize(a->window, &w, &h) &&
            SDL_GetDisplayUsableBounds(SDL_GetDisplayForWindow(a->window), &usable)) {
            int dw = usable.w < 1080 ? usable.w : 1080;
            int dh = usable.h < 780 ? usable.h : 780;
            int dx = x + w + 12;
            if (dx + dw > usable.x + usable.w)
                dx = usable.x + usable.w - dw;
            if (dx < usable.x)
                dx = usable.x;
            if (y + dh > usable.y + usable.h)
                y = usable.y + usable.h - dh;
            if (y < usable.y)
                y = usable.y;
            SDL_SetWindowSize(a->doc_window, dw, dh);
            SDL_SetWindowPosition(a->doc_window, dx, y);
        }
        ps_graphics_make_current(a->graphics);
    }
    SDL_ShowWindow(a->doc_window);
    if (SDL_GetWindowFlags(a->doc_window) & SDL_WINDOW_MINIMIZED)
        SDL_RestoreWindow(a->doc_window);
    SDL_RaiseWindow(a->doc_window);
    a->doc_visible = true;
    return true;
fail:
    snprintf(a->doc_error, sizeof a->doc_error, "Dokumentationsfenster: %.130s", SDL_GetError());
    status(a, a->doc_error);
    documentation_window_destroy(a);
    return false;
}

static void documentation_window_hide(app *a) {
    SDL_Event focus_lost = {0};
    focus_lost.type = SDL_EVENT_WINDOW_FOCUS_LOST;
    focus_lost.window.windowID = SDL_GetWindowID(a->doc_window);
    nk_sdl_handle_event(a->doc_ui, &focus_lost);
    SDL_StopTextInput(a->doc_window);
    SDL_HideWindow(a->doc_window);
    a->doc_visible = false;
}

static bool documentation_window_render(app *a, const char *capture_path) {
    if (!a->doc_visible)
        return true;
    if (!ps_graphics_make_current(a->doc_graphics))
        return false;
    struct nk_context *ui = a->doc_ui;
    int w, h;
    SDL_GetWindowSize(a->doc_window, &w, &h);
    /* Font changes also invalidate cached line heights in the open document. */
    const struct nk_user_font *code = a->doc_code_fonts[(a->preferences.editor_size - 16) / 2];
    if (a->documentation && a->doc_font_code != code)
        a->documentation->layout_width = 0;
    a->doc_font_code = code;
    float side = (w < 1000 ? 220.0f : 270.0f)*(a->preferences.ui_size/16.f);
    side=fminf(side,(float)w*.4f);
    nk_style_push_vec2(ui, &ui->style.window.padding, nk_vec2(14, 14));
    if (nk_begin(ui, "topics", nk_rect(8, 8, side, (float)h - 16), 0)) {
        if (a->doc_reset_sidebar) {
            nk_window_set_scroll(ui, 0, 0);
            a->doc_reset_sidebar = false;
        }
        documentation_sidebar(a);
    }
    nk_end(ui);
    if (a->doc_search_focus || a->doc_search_active)
        nk_window_set_focus(ui, "document");
    else if (a->doc_filter_active)
        nk_window_set_focus(ui, "topics");
    if (nk_begin(ui, "document", nk_rect(side + 16, 8, (float)w - side - 24, (float)h - 16),
                 NK_WINDOW_NO_SCROLLBAR))
        documentation_view(a, (float)h - 16);
    nk_end(ui);
    if (a->doc_search_active)
        nk_window_set_focus(ui, "document");
    else if (a->doc_filter_active)
        nk_window_set_focus(ui, "topics");
    nk_style_pop_vec2(ui);
    nk_sdl_update_TextInput(ui);
    bool ok = nk_sdl_render(ui);
    if (ok && capture_path)
        ok = ps_graphics_capture(a->doc_graphics, capture_path);
    if (ok)
        ok = ps_graphics_present(a->doc_graphics);
    return ps_graphics_make_current(a->graphics) && ok;
}
