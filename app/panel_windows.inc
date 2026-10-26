/* Fixed content slots with independent SDL/GL/Nuklear windows. Domain state
 * belongs to app, never to a copied app or a second simulation. Fonts are
 * immutable, shared GL resources owned by the main UI until final shutdown. */
static const char *panel_name(unsigned panel) {
    static const char *names[]={"Workspace / Szenenbaum","Editor / Szene","Protokoll","Inspektor","Messbereich","Zeitleiste"};
    return names[panel];
}
static bool panel_is_detached(const app *a,unsigned panel) {
    return panel<PS_PANEL_COUNT && a->panels[panel].detached && (panel<PS_DOCK_PANELS || a->tab==1);
}
/* Existing headings double as drag handles. No added widgets, icons, paint,
 * margins or rows. Accessible activation uses the same heading bounds. */
static void panel_pull_handle(app *a,unsigned panel,struct nk_rect bounds) {
    if(a->panel_render)return;
    a->panels[panel].action=panel_is_detached(a,panel)?nk_rect(0,0,0,0):bounds;
    if((analysis_visible(a) || panel>=PS_PANEL_MEASURE || (panel==PS_DOCK_WORKSPACE && laboratory_visible(a))) && !panel_is_detached(a,panel)) {
        const char *title=analysis_visible(a)?panel==PS_DOCK_SIDEBAR?"Messläufe":panel==PS_DOCK_WORKSPACE?"Messdaten und Analyseergebnis":"Laufdetails":panel_name(panel);
        char name[160];snprintf(name,sizeof name,"%s in eigenem Fenster \u00f6ffnen",title);
        if(ps_ui_accessibility_button(a->ui,bounds,name))a->panel_open_request|=1u<<panel;
    }
}
static void panel_pull_cancel(app *a) {
    a->panel_pull=0;a->panel_pull_dragging=false;SDL_CaptureMouse(false);
}
static void panel_pull_input(app *a,float width,float height) {
    struct nk_input *in=&a->ui->input;
    bool down=nk_input_is_mouse_down(in,NK_BUTTON_LEFT);
    bool pressed=nk_input_is_mouse_pressed(in,NK_BUTTON_LEFT);
    if(a->toolbar_menu || a->dialog_pending || a->recovery || (a->ui->active && a->ui->active->popup.active && a->ui->active->popup.type!=NK_PANEL_TOOLTIP))return;
    if(!a->panel_pull && pressed && !a->dock_resize && !a->laboratory_resize) {
        for(unsigned i=0;i<PS_DOCK_NODES;i++)if(a->dock_splitters[i].w>0 && nk_input_is_mouse_hovering_rect(in,a->dock_splitters[i]))return;
        for(unsigned i=0;i<3;i++)if(a->laboratory_splitters[i].w>0 && nk_input_is_mouse_hovering_rect(in,a->laboratory_splitters[i]))return;
        for(unsigned i=0;i<PS_PANEL_COUNT;i++) {
            if(panel_is_detached(a,i) || (i<PS_DOCK_PANELS && (a->preferences.dock.hidden&(1u<<i)) && !(i==PS_DOCK_LOG && protocol_visible(a))) || (i>=PS_PANEL_MEASURE && !laboratory_visible(a)))continue;
            struct nk_rect r=a->panels[i].action;
            if(r.w>0 && r.h>0 && nk_input_is_mouse_hovering_rect(in,r)) {
                a->panel_pull=i+1;a->panel_pull_origin=in->mouse.pos;
                memcpy(a->panel_pull_file_order,a->file_tab_order,sizeof a->file_tab_order);break;
            }
        }
    }
    if(!a->panel_pull)return;
    if(down && (fabsf(in->mouse.pos.x-a->panel_pull_origin.x)>6 || fabsf(in->mouse.pos.y-a->panel_pull_origin.y)>6)) {
        a->panel_pull_dragging=true;SDL_CaptureMouse(true);
    }
    if(!down) {
        bool outside=in->mouse.pos.x<0 || in->mouse.pos.y<0 || in->mouse.pos.x>=width || in->mouse.pos.y>=height;
        if(a->panel_pull_dragging && outside) {
            unsigned panel=a->panel_pull-1;a->panel_open_request|=1u<<panel;a->panel_drop_mask|=1u<<panel;
            int x,y;SDL_GetWindowPosition(a->window,&x,&y);a->panel_drop_x=x+(int)in->mouse.pos.x;a->panel_drop_y=y+(int)in->mouse.pos.y;
            if(panel==PS_DOCK_WORKSPACE)memcpy(a->file_tab_order,a->panel_pull_file_order,sizeof a->file_tab_order);
            a->file_tab_drag=0;a->file_tab_dragging=false;
        }
        panel_pull_cancel(a);
    }
}
static void panel_window_return(app *a,unsigned panel) {
    ps_panel_window *p=&a->panels[panel];if(!p->detached)return;
    if(a->workspace_tip.owner==p->ui){a->workspace_tip.visible=false;a->workspace_tip.input_held=false;}
    if(panel==PS_DOCK_WORKSPACE && editor_search_target(a)) {
        a->completion_focus_edit=editor_search_target(a);nk_window_set_focus(a->ui,"workspace");
    }
    if(p->resize){a->laboratory_curve_ratio=p->resize_original;p->resize=0;p->resize_cancelled=true;SDL_CaptureMouse(false);}
    if(panel<PS_DOCK_PANELS && p->ui) {
        struct nk_window *body=nk_window_find(p->ui,dock_windows[panel]);
        if(body){p->scroll_x=body->scrollbar.x;p->scroll_y=body->scrollbar.y;}
    }
    p->restore_scroll=true;p->detached=false;
    if(panel<PS_DOCK_PANELS && !(panel==PS_DOCK_LOG && a->show_log)) {
        a->preferences.dock.hidden&=~(1u<<panel);
    }
    SDL_StopTextInput(p->window);SDL_HideWindow(p->window);nk_sdl_accessibility_suspend(p->ui);
    /* Flush held keys/buttons on both sides of a transfer. */
    SDL_Event lost={0};lost.type=SDL_EVENT_WINDOW_FOCUS_LOST;lost.window.windowID=SDL_GetWindowID(p->window);
    nk_sdl_handle_event(p->ui,&lost);
    SDL_RaiseWindow(a->window);
}
static bool panel_window_open(app *a,unsigned panel) {
    ps_panel_window *p=&a->panels[panel];
    if(p->detached){SDL_ShowWindow(p->window);SDL_RaiseWindow(p->window);return true;}
    p->tab=a->tab;
    p->restore_editor_focus=panel==PS_DOCK_WORKSPACE && editor_search_target(a) && editor_search_target(a)->active;
    if(panel<PS_DOCK_PANELS)p->home=laboratory_visible(a) && panel==PS_DOCK_WORKSPACE?a->laboratory_scene_frame:a->dock_panels[panel];
    int width=(int)lroundf(p->home.w),height=(int)lroundf(p->home.h);
    if(width<1 || height<1){SDL_SetError("Der Bereich hat noch keine sichtbaren Inhaltsmaße.");return false;}
    if(!p->window) {
        if(!ps_graphics_make_current(a->graphics))return false;
        if(!SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT,1))return false;
        char title[128];snprintf(title,sizeof title,"Physim | %s",panel_name(panel));
        p->window=SDL_CreateWindow(title,width,height,
            SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE|SDL_WINDOW_HIGH_PIXEL_DENSITY|SDL_WINDOW_HIDDEN);
        if(p->window)p->graphics=ps_graphics_create(p->window);
        SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT,0);
        if(!p->graphics)goto fail;
        SDL_GL_SetSwapInterval(0);
        p->ui=nk_sdl_init(p->window,p->graphics);if(!p->ui)goto fail;
        nk_sdl_borrow_fonts(p->ui,a->ui);
        int x=0,y=0;SDL_GetWindowPosition(a->window,&x,&y);SDL_SetWindowPosition(p->window,x+64,y+64);
    }
    /* Each extraction starts with the source's client size, including reuse of
     * an existing SDL window. Old minimums must not force a different layout. */
    SDL_SetWindowMinimumSize(p->window,(int)fminf((float)width,panel==PS_DOCK_INSPECTOR || panel==PS_DOCK_SIDEBAR?280.f:480.f),
        (int)fminf((float)height,panel==PS_PANEL_TIMELINE?160.f:300.f));
    if(SDL_GetWindowFlags(p->window)&SDL_WINDOW_MAXIMIZED)SDL_RestoreWindow(p->window);
    SDL_SetWindowSize(p->window,width,height);
    if(panel<PS_DOCK_PANELS) {
        struct nk_window *body=nk_window_find(a->ui,dock_windows[panel]);
        if(body){p->scroll_x=body->scrollbar.x;p->scroll_y=body->scrollbar.y;}
    }
    p->restore_scroll=true;p->detached=true;
    SDL_ShowWindow(p->window);SDL_RaiseWindow(p->window);
    ps_graphics_make_current(a->graphics);return true;
fail:
    if(p->graphics){ps_graphics_make_current(p->graphics);if(p->ui)nk_sdl_shutdown(p->ui);ps_graphics_destroy(p->graphics);}
    if(p->window)SDL_DestroyWindow(p->window);
    *p=(ps_panel_window){0};ps_graphics_make_current(a->graphics);return false;
}
static void panel_requests(app *a) {
    if(a->graphic_export)return;
    unsigned returns=a->panel_return_request,opens=a->panel_open_request;
    a->panel_return_request=a->panel_open_request=0;
    for(unsigned panel=0;panel<PS_PANEL_COUNT;panel++) {
        if(returns&(1u<<panel))panel_window_return(a,panel);
        if(opens&(1u<<panel))if(!panel_window_open(a,panel)) {
            char message[220];snprintf(message,sizeof message,"Fenster konnte nicht geöffnet werden: %.150s",SDL_GetError());status(a,message);
        }
        if((a->panel_drop_mask&(1u<<panel)) && a->panels[panel].detached)
            SDL_SetWindowPosition(a->panels[panel].window,a->panel_drop_x-80,a->panel_drop_y-20);
    }
    a->panel_drop_mask=0;
}
static bool panel_window_event(app *a,SDL_Event *event,SDL_Window *window) {
    for(unsigned i=0;i<PS_PANEL_COUNT;i++) {
        ps_panel_window *p=&a->panels[i];if(!p->window || p->window!=window)continue;
        if(event->type==SDL_EVENT_WINDOW_CLOSE_REQUESTED){a->panel_return_request|=1u<<i;return true;}
        if(!p->detached)return true;
        if(i==PS_PANEL_MEASURE && p->resize && (event->type==SDL_EVENT_WINDOW_FOCUS_LOST || event->type==SDL_EVENT_WINDOW_RESIZED)) {
            a->laboratory_curve_ratio=p->resize_original;p->resize=0;p->resize_cancelled=true;SDL_CaptureMouse(false);
        }
        if(i==PS_PANEL_MEASURE && event->type==SDL_EVENT_KEY_DOWN) {
            struct nk_context *ui=a->ui;SDL_Window *win=a->window;int tab=a->tab,resize=a->laboratory_resize;
            struct nk_rect bounds=a->dock_panels[PS_DOCK_WORKSPACE];
            struct nk_rect measure_bounds=a->laboratory_measure_bounds;float measure_width=a->laboratory_measure_width;
            float original=a->laboratory_resize_original;bool cancelled=a->laboratory_resize_cancelled;
            int w,h;SDL_GetWindowSize(p->window,&w,&h);
            a->ui=p->ui;a->window=p->window;a->tab=p->tab;a->laboratory_resize=p->resize;
            a->laboratory_resize_original=p->resize_original;a->laboratory_resize_cancelled=p->resize_cancelled;
            a->dock_panels[PS_DOCK_WORKSPACE]=nk_rect(0,0,(float)w,(float)h);
            a->laboratory_measure_bounds=p->measure_bounds;a->laboratory_measure_width=p->measure_width;
            bool handled=laboratory_resize_key(a,&event->key);
            p->resize=a->laboratory_resize;p->resize_original=a->laboratory_resize_original;p->resize_cancelled=a->laboratory_resize_cancelled;
            a->ui=ui;a->window=win;a->tab=tab;a->laboratory_resize=resize;a->laboratory_resize_original=original;a->laboratory_resize_cancelled=cancelled;
            a->dock_panels[PS_DOCK_WORKSPACE]=bounds;a->laboratory_measure_bounds=measure_bounds;a->laboratory_measure_width=measure_width;if(handled)return true;
        }
        if(i==PS_DOCK_WORKSPACE && p->tab==2) {
            if(a->analysis_plot_resizing && (event->type==SDL_EVENT_WINDOW_FOCUS_LOST || event->type==SDL_EVENT_WINDOW_RESIZED))analysis_plot_resize_abort(a);
            if(event->type==SDL_EVENT_KEY_DOWN) {
                struct nk_context *ui=a->ui;int tab=a->tab;a->ui=p->ui;a->tab=p->tab;
                bool handled=analysis_plot_resize_key(a,&event->key);a->ui=ui;a->tab=tab;if(handled)return true;
            }
        }
        /* Component keys include Shift=40 px and Enter=reset. The generic
         * range handler would otherwise consume them as a one-step adjustment. */
        if(i==PS_DOCK_INSPECTOR && event->type==SDL_EVENT_KEY_DOWN && workbench_context_key(a,p->ui,&event->key,true))return true;
        if(i==PS_DOCK_LOG && event->type==SDL_EVENT_KEY_DOWN) {
            struct nk_context *ui=a->ui;a->ui=p->ui;bool handled=protocol_navigation_key(a,&event->key);a->ui=ui;if(handled)return true;
        }
        if(nk_sdl_accessibility_event(p->ui,event))return true;
        if(i==PS_DOCK_WORKSPACE) {
            int tab=a->tab;struct nk_context *ui=a->ui;a->tab=p->tab;a->ui=p->ui;
            bool handled=event->type==SDL_EVENT_KEY_DOWN && (editor_search_key(a,&event->key) || file_tabs_keyboard(a,&event->key));
            if(event->type==SDL_EVENT_TEXT_INPUT)a->completion_typed=true;
            if(!handled)a->tab=tab;a->ui=ui;if(handled)return true;
        }
        if(event->type==SDL_EVENT_KEY_DOWN && !event->key.repeat) {
            if(event->key.key==SDLK_RETURN && (event->key.mod&PS_UI_COMMAND_MOD) && (event->key.mod&SDL_KMOD_SHIFT)){a->panel_return_request|=1u<<i;return true;}
            if(event->key.key==SDLK_F1){open_documentation(a,a->documentation?a->doc_topic:1);return true;}
            if(event->key.key==SDLK_F5){if(idle(a))build_project(a);return true;}
            if(event->key.key==SDLK_F6 && !a->reset_pending && !a->reset_starting) {
                if(!a->runner.running)start_run(a);else if(a->hello && !a->stop_at)command(a,a->paused?PS_MSG_RUN:PS_MSG_PAUSE);return true;
            }
            if(event->key.key==SDLK_F7){reset_run(a);return true;}
            if(i==PS_DOCK_WORKSPACE && p->tab==1 && scene_shortcut(a,&event->key))return true;
            if(event->key.key==SDLK_W && event->key.mod&PS_UI_COMMAND_MOD && i==PS_DOCK_WORKSPACE){unsigned key=file_tab_active(a);if(key<FILE_TABS)file_tab_close(a,key);return true;}
            if(event->key.key==SDLK_S && event->key.mod&PS_UI_COMMAND_MOD){save_active_document(a);return true;}
            if(event->key.key==SDLK_F && event->key.mod&PS_UI_COMMAND_MOD && i==PS_DOCK_WORKSPACE) {
                int tab=a->tab;struct nk_context *ui=a->ui;a->tab=p->tab;a->ui=p->ui;
                editor_search_open(a);a->tab=tab;a->ui=ui;return true;
            }
        }
        nk_sdl_handle_event(p->ui,event);return true;
    }
    return false;
}
static bool panel_windows_render(app *a) {
    SDL_Window *main_window=a->window;ps_graphics *main_graphics=a->graphics;struct nk_context *main_ui=a->ui;
    int main_tab=a->tab;bool ok=true;
    struct nk_rect saved[PS_DOCK_PANELS];bool visible[PS_DOCK_PANELS];
    float header_heights[PS_DOCK_PANELS];for(unsigned i=0;i<PS_DOCK_PANELS;i++)header_heights[i]=dock_panel_header_height(a,i);
    struct nk_rect splitters[3];memcpy(splitters,a->laboratory_splitters,sizeof splitters);
    struct nk_rect measure_bounds=a->laboratory_measure_bounds;float measure_width=a->laboratory_measure_width;
    struct nk_rect dock_splitters[PS_DOCK_NODES];memcpy(dock_splitters,a->dock_splitters,sizeof dock_splitters);
    int resize=a->laboratory_resize;float original=a->laboratory_resize_original;bool cancelled=a->laboratory_resize_cancelled;
    memcpy(saved,a->dock_panels,sizeof saved);memcpy(visible,a->dock_visible,sizeof visible);
    const ps_ui_palette *palette=ui_palette;
    for(unsigned i=0;i<PS_PANEL_COUNT;i++) {
        ps_panel_window *p=&a->panels[i];if(!p->detached || SDL_GetWindowFlags(p->window)&(SDL_WINDOW_HIDDEN|SDL_WINDOW_MINIMIZED))continue;
        if(!ps_graphics_make_current(p->graphics)){ok=false;break;}
        if(i<PS_DOCK_PANELS)p->tab=main_tab;
        a->window=p->window;a->graphics=p->graphics;a->ui=p->ui;a->tab=p->tab;a->panel_render=(int)i+1;
        int w,h;SDL_GetWindowSize(p->window,&w,&h);
        graphic_input_guard export_guard=graphic_export_guard(a);
        ui_palette=workbench_visible(a) || analysis_visible(a)?ps_workbench_palette(palette):laboratory_visible(a)?ps_laboratory_palette(palette):palette;
        style(p->ui);nk_style_set_font(p->ui,workbench_visible(a) || analysis_visible(a)?&a->workbench_fonts[0]:laboratory_visible(a)?&a->workbench_fonts[2]:a->font_ui);
        nk_sdl_set_ui_size(p->ui,a->preferences.ui_size);
        ps_graphics_background(p->graphics,UI_COLOR(canvas).r,UI_COLOR(canvas).g,UI_COLOR(canvas).b);
        ps_graphics_backdrop(p->graphics,NULL);
        ps_graphics_shadows(p->graphics,NULL,0);
        bool auxiliary=i>=PS_DOCK_PANELS;
        if(auxiliary) {
            nk_style_push_vec2(p->ui,&p->ui->style.window.padding,nk_vec2(0,0));
            nk_style_push_vec2(p->ui,&p->ui->style.window.spacing,nk_vec2(6,4));
        }
        workspace_tip_frame_begin(a);
        if(i<PS_DOCK_PANELS) {
            a->dock_panels[i]=nk_rect(0,0,(float)w,(float)h);a->dock_visible[i]=true;
            bool laboratory_heading=laboratory_visible(a) && (i==PS_DOCK_SIDEBAR || i==PS_DOCK_INSPECTOR);
            nk_window_show(p->ui,"panel-existing-heading",laboratory_heading?NK_SHOWN:NK_HIDDEN);
            if(laboratory_heading) {
                float heading=header_heights[i];
                nk_style_push_vec2(p->ui,&p->ui->style.window.padding,nk_vec2(0,0));
                nk_style_push_float(p->ui,&p->ui->style.window.rounding,0);
                nk_style_push_style_item(p->ui,&p->ui->style.window.fixed_background,nk_style_item_color(UI_COLOR(header)));
                if(nk_begin(p->ui,"panel-existing-heading",nk_rect(0,0,(float)w,heading),NK_WINDOW_BACKGROUND|NK_WINDOW_NO_SCROLLBAR))
                    laboratory_panel_heading(a,i,nk_rect(0,0,(float)w,heading));
                nk_end(p->ui);nk_style_pop_style_item(p->ui);nk_style_pop_float(p->ui);nk_style_pop_vec2(p->ui);
                a->dock_panels[i].y+=heading;a->dock_panels[i].h=fmaxf(1,a->dock_panels[i].h-heading);
            }
            if(i==PS_DOCK_WORKSPACE && workbench_visible(a)) {
                file_tabs_prepare(a);
                float heading=file_tabs_height(a);
                nk_style_push_vec2(p->ui,&p->ui->style.window.padding,nk_vec2(0,0));
                nk_style_push_float(p->ui,&p->ui->style.window.rounding,0);
                if(nk_begin(p->ui,"panel-file-tabs",nk_rect(0,0,(float)w,heading),NK_WINDOW_NO_SCROLLBAR)) {
                    struct nk_rect strip=workbench_editor_tools(a,nk_rect(0,0,(float)w,heading),false,false);
                    file_tabs_view(a,strip);workspace_rule(nk_window_get_canvas(p->ui),nk_rect(0,heading-1,(float)w,1));
                }
                nk_end(p->ui);nk_style_pop_float(p->ui);nk_style_pop_vec2(p->ui);
                a->dock_panels[i].y+=heading;a->dock_panels[i].h-=heading;
            }
            if(i==PS_DOCK_WORKSPACE && p->restore_editor_focus && nk_window_find(p->ui,"workspace")) {
                nk_window_set_focus(p->ui,"workspace");a->completion_focus_edit=editor_search_target(a);p->restore_editor_focus=false;
            }
            if(i==PS_DOCK_SIDEBAR)sidebar_panel(a);
            else if(i==PS_DOCK_WORKSPACE){workspace_panel(a);editor_search_view(a);completion_view(a,w,h);}
            else if(i==PS_DOCK_LOG)log_panel(a);else inspector_panel(a);
        } else if(laboratory_begin(a,"panel-content",nk_rect(0,0,(float)w,(float)h),NK_WINDOW_NO_SCROLLBAR)) {
            a->dock_panels[PS_DOCK_WORKSPACE]=nk_rect(0,0,(float)w,(float)h);
            if(i==PS_PANEL_MEASURE) {
                memcpy(a->laboratory_splitters,p->splitters,sizeof p->splitters);
                a->laboratory_measure_bounds=p->measure_bounds;a->laboratory_measure_width=p->measure_width;
                a->laboratory_resize=p->resize;a->laboratory_resize_original=p->resize_original;a->laboratory_resize_cancelled=p->resize_cancelled;
                laboratory_resize_input(a);
                laboratory_measurements(a,fmaxf(100,nk_window_get_content_region(p->ui).h));
            }
            else timeline_controls(a);
            nk_end(p->ui);
        } else nk_end(p->ui);
        if(auxiliary){nk_style_pop_vec2(p->ui);nk_style_pop_vec2(p->ui);}
        if(i==PS_PANEL_MEASURE) {
            a->laboratory_splitters[0]=a->laboratory_splitters[1]=nk_rect(0,0,0,0);
            memset(a->dock_splitters,0,sizeof a->dock_splitters);
            laboratory_separators(a);
            p->resize=a->laboratory_resize;p->resize_original=a->laboratory_resize_original;p->resize_cancelled=a->laboratory_resize_cancelled;
        }
        workspace_tip_frame_end(a);
        graphic_export_unguard(a,export_guard);
        graphic_export_view(a,w,h);
        if(a->timeline_design_active && i==PS_PANEL_TIMELINE)timeline_design_draw(a);
        nk_sdl_update_TextInput(p->ui);
        memcpy(p->splitters,a->laboratory_splitters,sizeof p->splitters);
        if(i==PS_PANEL_MEASURE){p->measure_bounds=a->laboratory_measure_bounds;p->measure_width=a->laboratory_measure_width;}
        if(!nk_sdl_render(p->ui))ok=false;
        if(p->capture[0]){if(!ps_graphics_capture(p->graphics,p->capture))ok=false;p->capture[0]=0;}
        if(!ps_graphics_present(p->graphics))ok=false;
        if(i<PS_DOCK_PANELS && a->tab!=p->tab)main_tab=a->tab;
        memcpy(a->dock_panels,saved,sizeof saved);memcpy(a->dock_visible,visible,sizeof visible);
        memcpy(a->laboratory_splitters,splitters,sizeof splitters);memcpy(a->dock_splitters,dock_splitters,sizeof dock_splitters);
        a->laboratory_measure_bounds=measure_bounds;a->laboratory_measure_width=measure_width;
        a->laboratory_resize=resize;a->laboratory_resize_original=original;a->laboratory_resize_cancelled=cancelled;
    }
    a->panel_render=0;a->window=main_window;a->graphics=main_graphics;a->ui=main_ui;a->tab=main_tab;
    ui_palette=a->preferences.theme==PS_THEME_LIGHT?&PS_UI_LIGHT:a->preferences.theme==PS_THEME_HIGH_CONTRAST?&PS_UI_HIGH_CONTRAST:&PS_UI_DARK;
    style(main_ui);nk_style_set_font(main_ui,a->font_ui);nk_sdl_set_ui_size(main_ui,a->preferences.ui_size);
    memcpy(a->dock_panels,saved,sizeof saved);memcpy(a->dock_visible,visible,sizeof visible);
    memcpy(a->laboratory_splitters,splitters,sizeof splitters);
    if(!ps_graphics_make_current(main_graphics))ok=false;
    return ok;
}
static void panel_windows_destroy(app *a) {
    for(unsigned i=0;i<PS_PANEL_COUNT;i++) {
        ps_panel_window *p=&a->panels[i];if(p->graphics){ps_graphics_make_current(p->graphics);nk_sdl_shutdown(p->ui);ps_graphics_destroy(p->graphics);}
        if(p->window)SDL_DestroyWindow(p->window);*p=(ps_panel_window){0};
    }
    ps_graphics_make_current(a->graphics);
}
