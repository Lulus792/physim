/* App-owned presentation state, separate from project sources and run metadata. */
static void preferences_apply(app *a, const ps_preferences *p) {
    if(!ps_preferences_valid(p))return;
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
        documentation_typography_update(a);
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
        p->workspace = a->tab==2 && a->analysis_tab==1?0:(uint32_t)a->tab;
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
    /* Capture the restored size after the asynchronous window request settles,
     * before an immediate maximize can retain the old startup dimensions. */
    if(SDL_SetWindowSize(a->window,width,height))SDL_SyncWindow(a->window);
    if (p.maximized)
        SDL_MaximizeWindow(a->window);
    layouts_start(a);
    channel_units_start(a);
}
#include "settings_window.inc"
