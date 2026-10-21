#include "documentation_layout.h"
#include "documentation_figure_font.inc"
/* Independent native window: its input, fonts and GL resources belong to this viewer. */
static void documentation_window_destroy(app *a) {
    for(unsigned i=0;i<2;i++){ps_ui_font_pairs_destroy(a->doc_font_pairs[i]);a->doc_font_pairs[i]=NULL;}
    ps_code_preview_destroy(a->doc_context_code_layout);a->doc_context_code_layout=NULL;
    free(a->doc_context_source);a->doc_context_source=NULL;
    a->doc_context_code_bounds=a->doc_context_code_bar=nk_rect(0,0,0,0);
    a->doc_context_code_x[0]=a->doc_context_code_x[1]=a->doc_context_code_max=0;
    memset(&a->doc_context_code_drag,0,sizeof a->doc_context_code_drag);
    for (int i = 0; i < DOCUMENTATION_TOPIC_COUNT; i++) {
        free(documentation_index[i]);
        documentation_index[i] = NULL;
    }
    if (a->doc_graphics) {
        ps_graphics_make_current(a->doc_graphics);
        documentation_math_clear(a);
        nk_sdl_shutdown(a->doc_ui);
        ps_graphics_destroy(a->doc_graphics);
    }
    if (a->doc_window)
        SDL_DestroyWindow(a->doc_window);
    a->doc_window = NULL;
    a->doc_graphics = NULL;
    a->doc_ui = NULL;
    a->doc_visible = false;
    a->doc_find_open=a->doc_actions_open=false;
    a->doc_action_pending=0;
    a->doc_raster_density=a->doc_failed_density=0;
    a->doc_font_ui=a->doc_font_title=a->doc_font_code=NULL;
    memset(a->doc_ui_fonts,0,sizeof a->doc_ui_fonts);memset(a->doc_title_fonts,0,sizeof a->doc_title_fonts);
    memset(a->doc_bold_fonts,0,sizeof a->doc_bold_fonts);memset(a->doc_code_fonts,0,sizeof a->doc_code_fonts);
    memset(a->doc_chrome_fonts,0,sizeof a->doc_chrome_fonts);
    memset(a->doc_figure_fonts,0,sizeof a->doc_figure_fonts);
    memset(a->doc_type_fonts,0,sizeof a->doc_type_fonts);memset(a->doc_type_em,0,sizeof a->doc_type_em);
    memset(&a->doc_figure_draw_font,0,sizeof a->doc_figure_draw_font);
    memset(a->doc_font_em,0,sizeof a->doc_font_em);
    memset(a->doc_spaced_fonts,0,sizeof a->doc_spaced_fonts);
    a->doc_type_document=NULL;a->doc_type_version=0;a->doc_lead_text=SIZE_MAX;
    ps_graphics_make_current(a->graphics);
}

/* Missing sizes are appended; an actual density change replaces the bank. */
static bool documentation_fonts_load(app *a,float density,unsigned size,unsigned code_size,unsigned ui_size,unsigned editor_size) {
    /* The handbook retains its readable 16px minimum independently of the
     * compact source editor's additional 12/14px choices. */
    if(code_size==12 || code_size==14)code_size=16;
    if(editor_size==12 || editor_size==14)editor_size=16;
    if(!isfinite(density) || density<1 || density>4 || size<16 || size>32 || (size&1) ||
       code_size<16 || code_size>32 || (code_size&1) || ui_size<16 || ui_size>22 || (ui_size&1) ||
       editor_size<16 || editor_size>32 || (editor_size&1))return false;
    unsigned index=(size-16)/2,code_index=(code_size-16)/2;
    bool replace=!a->doc_font_ui || fabsf(density-a->doc_raster_density)>.01f;
    const struct nk_user_font *regular[9]={0},*titles[9]={0},*bold[11]={0},*code[9]={0};
    const struct nk_user_font *chrome[4]={0},*figures[4]={0};
    float em[6][11]={{0}};
    ps_ui_font_pairs *pairs[2]={replace?NULL:a->doc_font_pairs[0],replace?NULL:a->doc_font_pairs[1]};
    if(!replace) {
        memcpy(regular,a->doc_ui_fonts,sizeof regular);memcpy(titles,a->doc_title_fonts,sizeof titles);
        memcpy(bold,a->doc_bold_fonts,sizeof bold);memcpy(code,a->doc_code_fonts,sizeof code);
        memcpy(chrome,a->doc_chrome_fonts,sizeof chrome);
        memcpy(figures,a->doc_figure_fonts,sizeof figures);
        memcpy(em,a->doc_font_em,sizeof em);
    }
    struct nk_font_atlas candidate;
    if(a->doc_test_font_allocator)nk_font_atlas_init(&candidate,a->doc_test_font_allocator);
    else nk_font_atlas_init_default(&candidate);
    nk_font_atlas_begin(&candidate);
    struct nk_font *default_font=NULL;
    unsigned added=0;
    if(SDL_getenv("PHYSIM_TEST_TRACE"))fprintf(stderr,"APP TEST TRACE: F1 fonts begin %.6f\n",ps_clock());
    for(unsigned role=0;role<6;role++)for(unsigned i=0;i<(role>=4?4u:role==2?11u:9u);i++) {
        bool needed=a->doc_full_font_inventory || (role>=4?i==(ui_size-16)/2:role==3?(i==code_index || i==(editor_size-16)/2):role==2?(i==index || i==index+2 || i==(ui_size-16)/2):
            (i==index || (role==0 && i==(ui_size-16)/2)));
        const struct nk_user_font **slot=role==0?&regular[i]:role==1?&titles[i]:role==2?&bold[i]:role==3?&code[i]:role==4?&chrome[i]:&figures[i];
        if(!needed || *slot)continue;
        float pixels=(role>=4?(16.f+i*2)*(role==4?.875f:1.25f):(float)((role==1?26:16)+i*2))*density;
        struct nk_font *font=role==5?documentation_figure_font(&candidate,a->root,pixels):
            system_font_sampling(&candidate,a->root,pixels,role==3,role==1 || role==2,2,1);
        if(!font)goto fail;
        em[role][i]=ps_ui_font_em_ratio(font);
        if((role==1 || role==2) && !pairs[role-1]) {
            pairs[role-1]=ps_ui_font_pairs_create(font,&candidate.permanent);
            if(!pairs[role-1])goto fail;
        }
        if(role==3 || role==5) {
            /* Code is .8em in H; SVG labels have their own 20px CSS em.
             * Bake their actual em sizes instead of resampling a larger cell. */
            struct nk_font_config *config=font->config;
            do {config->size*=(role==3?.8f:1.f)*em[role][i];config=config->n;}while(config!=font->config);
        }
        if(role==0 && i==index)default_font=font;
        *slot=&font->handle;added++;
    }
    if(!added){nk_font_atlas_clear(&candidate);return true;}
    if(SDL_getenv("PHYSIM_TEST_TRACE"))fprintf(stderr,"APP TEST TRACE: F1 font sources ready %.6f\n",ps_clock());
    bool ok=replace?nk_sdl_font_stash_replace(a->doc_ui,&candidate,density,default_font):
                    nk_sdl_font_stash_append(a->doc_ui,&candidate,density);
    if(!ok)goto fail;
    memcpy(a->doc_ui_fonts,regular,sizeof regular);memcpy(a->doc_title_fonts,titles,sizeof titles);
    memcpy(a->doc_bold_fonts,bold,sizeof bold);memcpy(a->doc_code_fonts,code,sizeof code);
    memcpy(a->doc_chrome_fonts,chrome,sizeof chrome);
    memcpy(a->doc_figure_fonts,figures,sizeof figures);
    memcpy(a->doc_font_em,em,sizeof em);
    for(unsigned i=0;i<2;i++) {
        if(pairs[i]!=a->doc_font_pairs[i])ps_ui_font_pairs_destroy(a->doc_font_pairs[i]);
        a->doc_font_pairs[i]=pairs[i];
    }
    a->doc_raster_density=density;a->doc_failed_density=0;
    if(replace){memset(a->doc_type_fonts,0,sizeof a->doc_type_fonts);memset(a->doc_type_em,0,sizeof a->doc_type_em);
        memset(&a->doc_figure_draw_font,0,sizeof a->doc_figure_draw_font);}
    printf("F1 font demand: %s %u variants, density %.3f\n",replace?"replace":"append",added,density);
    if(SDL_getenv("PHYSIM_TEST_TRACE"))fprintf(stderr,"APP TEST TRACE: F1 fonts ready %.6f\n",ps_clock());
    return true;
fail:
    for(unsigned i=0;i<2;i++)if(pairs[i]!=a->doc_font_pairs[i])ps_ui_font_pairs_destroy(pairs[i]);
    nk_font_atlas_clear(&candidate);return false;
}
static bool documentation_window_fonts(app *a,float density) {
    if(!documentation_fonts_load(a,density,documentation_font_size(a),documentation_code_size(a),a->preferences.ui_size,a->preferences.editor_size))return false;
    unsigned index=(documentation_font_size(a)-16)/2;
    a->doc_font_ui=a->doc_ui_fonts[index];a->doc_font_title=a->doc_title_fonts[index];
    a->doc_font_code=a->doc_code_fonts[(documentation_code_size(a)-16)/2];
    documentation_typography_update(a);
    if(a->documentation) {
        documentation_preserve_reading_position(a);
        a->documentation->layout_width=0;
    }
    const struct nk_font *font=a->doc_font_ui->userdata.ptr;
    printf("F1 font density: %.3f, logical %.1f, raster %.1f\n",density,a->doc_font_ui->height,font->info.height);
    return true;
}
/* Keyboard and settings calls can arrive with the main context current. */
static bool documentation_fonts_prepare_sizes(app *a,unsigned size,unsigned code_size,unsigned ui_size,unsigned editor_size) {
    if(!a->doc_ui)return true;
    SDL_Window *previous_window=SDL_GL_GetCurrentWindow();SDL_GLContext previous=SDL_GL_GetCurrentContext();
    if(!ps_graphics_make_current(a->doc_graphics))return false;
    bool ok=documentation_fonts_load(a,a->doc_raster_density,size,code_size,ui_size,editor_size);
    if(!SDL_GL_MakeCurrent(previous_window,previous)){a->quitting=true;return false;}
    if(!ok)snprintf(a->doc_error,sizeof a->doc_error,"Schriftgröße konnte nicht geladen werden.");
    else if(!strcmp(a->doc_error,"Schriftgröße konnte nicht geladen werden."))a->doc_error[0]=0;
    return ok;
}
static bool documentation_fonts_prepare(app *a,unsigned size,unsigned code_size) {
    return documentation_fonts_prepare_sizes(a,size,code_size,a->preferences.ui_size,a->preferences.editor_size);
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
        float density=SDL_GetWindowPixelDensity(a->doc_window);
        if(!documentation_window_fonts(a,density>0?density:1))goto fail;
        const ps_ui_palette *previous_palette=ui_palette;
        ui_palette=a->preferences.theme==PS_THEME_LIGHT?&PS_UI_LIGHT:a->preferences.theme==PS_THEME_HIGH_CONTRAST?&PS_UI_HIGH_CONTRAST:&PS_UI_DARK;
        style(a->doc_ui);ui_palette=previous_palette;
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
    nk_sdl_accessibility_suspend(a->doc_ui);
    a->doc_visible = false;
    a->doc_keyboard=a->doc_search_active=a->doc_filter_active=false;
    a->doc_find_open=a->doc_actions_open=false;
    a->doc_action_pending=0;
}

static bool documentation_window_render(app *a, const char *capture_path) {
    if (!a->doc_visible || (a->doc_window && SDL_GetWindowFlags(a->doc_window)&(SDL_WINDOW_HIDDEN|SDL_WINDOW_MINIMIZED))) {
        nk_sdl_accessibility_suspend(a->doc_ui);return true;
    }
    if (!ps_graphics_make_current(a->doc_graphics))
        return false;
    documentation_math_begin_frame(a);
    float density=a->doc_test_density>0?a->doc_test_density:SDL_GetWindowPixelDensity(a->doc_window);
    if(density>0 && fabsf(density-a->doc_raster_density)>.01f && fabsf(density-a->doc_failed_density)>.01f) {
        if(!documentation_window_fonts(a,density)) {
            a->doc_failed_density=density;
            snprintf(a->doc_error,sizeof a->doc_error,"Schriftauflösung konnte nicht aktualisiert werden.");
        }
    }
    struct nk_context *ui = a->doc_ui;
    int w, h;
    SDL_GetWindowSize(a->doc_window, &w, &h);
    /* Font changes also invalidate cached line heights in the open document. */
    const struct nk_user_font *code = a->doc_code_fonts[(documentation_code_size(a) - 16) / 2];
    if (a->documentation && a->doc_font_code != code)
        a->documentation->layout_width = 0;
    a->doc_font_code = code;
    documentation_typography_update(a);
    ps_doc_layout layout=ps_documentation_layout((float)w,(float)h,(float)a->preferences.ui_size,
        a->doc_navigation_hidden,a->doc_context_hidden);
    a->doc_left_slot=nk_rect(layout.left.x,layout.left.y,layout.left.w,layout.left.h);
    a->doc_article_slot=nk_rect(layout.article.x,layout.article.y,layout.article.w,layout.article.h);
    a->doc_right_slot=nk_rect(layout.right.x,layout.right.y,layout.right.w,layout.right.h);
    const struct nk_user_font *chrome=documentation_chrome_font(a);
    nk_style_push_float(ui,&ui->style.window.rounding,0);
    nk_style_push_font(ui,chrome);
    nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(13,13));
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(12,ui->style.window.spacing.y));
    nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_color(UI_COLOR(header)));
    if(a->doc_filter_focus || a->doc_filter_active)nk_window_set_focus(ui,"documentation-header");
    if(nk_begin(ui,"documentation-header",nk_rect(0,0,(float)w,layout.header.h),NK_WINDOW_NO_SCROLLBAR)) {
        ps_ui_accessibility_route(ui,true);documentation_header(a);
    }nk_end(ui);
    nk_style_pop_vec2(ui);
    nk_style_pop_style_item(ui);
    nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_color(UI_COLOR(window)));
    nk_window_show(ui,"topics",a->doc_navigation_hidden?NK_HIDDEN:NK_SHOWN);
    if(!a->doc_navigation_hidden) {
        nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(0,0));
        nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(8,0));
        if(nk_begin(ui,"topics",a->doc_left_slot,NK_WINDOW_NO_SCROLLBAR)) {
            a->doc_left_slot=nk_window_get_bounds(ui);
            documentation_side_border(a,false);
            if(a->doc_reset_sidebar){a->doc_navigation_scroll_x=a->doc_navigation_scroll_y=0;a->doc_reset_sidebar=false;}
            ps_ui_accessibility_route(ui,true);documentation_sidebar(a);
        }nk_end(ui);
        nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);
    }
    nk_window_show(ui,"documentation-context",a->doc_context_hidden?NK_HIDDEN:NK_SHOWN);
    if(!a->doc_context_hidden) {
        nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(0,0));
        nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(8,0));
        if(nk_begin(ui,"documentation-context",a->doc_right_slot,NK_WINDOW_NO_SCROLLBAR)) {
            a->doc_right_slot=nk_window_get_bounds(ui);
            documentation_side_border(a,true);
            ps_ui_accessibility_route(ui,true);documentation_context_view(a);
        }nk_end(ui);
        nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);
    }
    nk_style_pop_style_item(ui);
    nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_color(UI_COLOR(header)));
    nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(12,4));
    if(nk_begin(ui,"documentation-footer",nk_rect(0,layout.footer.y,(float)w,layout.footer.h),NK_WINDOW_NO_SCROLLBAR)) {
        ps_ui_accessibility_route(ui,true);documentation_footer(a);
    }nk_end(ui);nk_style_pop_vec2(ui);nk_style_pop_style_item(ui);
    nk_style_pop_font(ui);
    /* Commit the overlay edit before computing matches/reading boxes. Its
     * window is raised after layout, so the article never paints over it. */
    a->doc_article_slot=nk_rect(layout.article.x,layout.article.y,layout.article.w,layout.article.h);
    documentation_find_view(a);
    nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(0,0));
    nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(ui->style.window.spacing.x,0));
    nk_style_push_style_item(ui,&ui->style.window.fixed_background,nk_style_item_color(UI_COLOR(doc_body)));
    if(nk_begin(ui,"document",a->doc_article_slot,NK_WINDOW_NO_SCROLLBAR)) {
        a->doc_article_slot=nk_window_get_bounds(ui);
        ps_ui_accessibility_route(ui,true);documentation_view(a,layout.article.h);
    }nk_end(ui);
    nk_style_pop_style_item(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);
    nk_style_pop_float(ui);
    documentation_actions_view(a);
    if(a->doc_search_active && !a->doc_actions_open)documentation_overlay_focus(ui,"documentation-find");
    else if(a->doc_filter_active)nk_window_set_focus(ui,"documentation-header");
    /* The article records its own metadata bounds while it is built. Select
     * the independent caption target only after all windows have been built. */
    if(a->doc_context_caption_test && !a->doc_context_hidden) {
        a->doc_detail_test_bounds=a->doc_context_caption_bounds;
        snprintf(a->doc_detail_test_text,sizeof a->doc_detail_test_text,"Ideales Pendel · Vakuum");
        a->doc_detail_test_requested=true;
    }
    documentation_detail_draw_probe(a);
    documentation_sidebar_draw_probe(a);
    documentation_math_draw_probe(a);
    documentation_inline_math_draw_probe(a);
    bool type_ok=documentation_typography_probe(a,capture_path) && documentation_article_probe(a,capture_path);
    nk_sdl_update_TextInput(ui);
    bool scaled_capture=capture_path && strlen(capture_path)>=4 && !strcmp(capture_path+strlen(capture_path)-4,".png");
    bool ok=!scaled_capture || nk_sdl_export_png_scaled(ui,ui,w,h,2,capture_path)==PS_OK;
    ok=nk_sdl_render(ui) && ok && type_ok;
    if (ok && capture_path && !scaled_capture) {
        int pixels_w=0,pixels_h=0;ok=SDL_GetWindowSizeInPixels(a->doc_window,&pixels_w,&pixels_h);
        const char *name=capture_path,*slash=strrchr(capture_path,'/'),*backslash=strrchr(capture_path,'\\');
        if(slash)name=slash+1;if(backslash && backslash+1>name)name=backslash+1;
        if(ok)printf("DOCUMENTATION CAPTURE: %s logical=%dx%d pixels=%dx%d font=%u body=%.1f\n",name,w,h,pixels_w,pixels_h,documentation_font_size(a),a->doc_body_bounds.h);
        if(ok)ok = ps_graphics_capture(a->doc_graphics, capture_path);
    }
    if(ok && scaled_capture)printf("DOCUMENTATION RASTER TARGET: logical=%dx%d pixels=%dx%d density=%.3f\n",w,h,w*2,h*2,a->doc_raster_density);
    if (ok)
        ok = ps_graphics_present(a->doc_graphics);
    return ps_graphics_make_current(a->graphics) && ok;
}
