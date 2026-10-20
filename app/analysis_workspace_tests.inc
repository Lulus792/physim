/* Real run files and native pointer routing; this is a layout regression, not
 * evidence of multi-run rendering or full reference fidelity. */
static bool analysis_workspace_test_start(app *a,const char *root,const char *mode) {
    char path[4096];join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    join(a->project_input,sizeof a->project_input,root,"project");a->example=0;new_project(a);
    if(!a->loaded)return false;
    ps_context c={.struct_size=sizeof c,.api_version=PS_API_VERSION,.dt_s=.02,.seed=42};
    if(ps_channel_add(&c,"Auslenkung",PS_RADIAN,"Testwinkel")<0)return false;
    snprintf(c.model_metadata,sizeof c.model_metadata,"model=Analysis layout test\nparameter.length=2\nparameter_unit.length=m\n");
    for(unsigned run=0;run<2;run++) {
        join(path,sizeof path,a->project,run?"runs/Lauf 002.psrun":"runs/Lauf 001.psrun");
        ps_run_writer writer;if(ps_run_create(&writer,path,&c,"Analysis layout test")!=PS_OK)return false;
        for(unsigned i=0;i<301;i++){double t=i*.02,value=.3*cos(t*(run?2:1));if(ps_run_append(&writer,t,&value)!=PS_OK)return false;}
        if(ps_run_close(&writer)!=PS_OK)return false;
    }
    join(a->last_run,sizeof a->last_run,a->project,"runs/Lauf 001.psrun");
    refresh_library(a); /* The first scan may predate these new fixture files. */
    a->selected_count=0;select_run(a,a->last_run,true);request_dataset(a);
    a->tab=0;a->analysis_tab=0;a->show_report=false;
    ps_preferences preferences=a->preferences;
    preferences.theme=strstr(mode,"light")?PS_THEME_LIGHT:strstr(mode,"contrast")?PS_THEME_HIGH_CONTRAST:PS_THEME_DARK;
    if(strstr(mode,"large"))preferences.ui_size=22;
    preferences_apply(a,&preferences);
    SDL_SetWindowSize(a->window,strstr(mode,"narrow")?1080:1440,strstr(mode,"narrow")?740:1000);
    return true;
}
static void analysis_workspace_test_frame(app *a,int *stage,int *code,const char **capture) {
    static unsigned settle;static ps_layout_state original;static struct nk_rect center;static double focus_retry;
    static ps_statistics statistics;static ps_plot_view scroll_view;static int viewport_width,viewport_height;static float curve_height,automatic_height,initial_curve_height;static double probe_at;static unsigned external;static struct nk_rect slot;bool ok=true;
    const unsigned external_panels[]={PS_DOCK_SIDEBAR,PS_DOCK_WORKSPACE,PS_DOCK_INSPECTOR};
    if(*stage==0 && SDL_getenv("PHYSIM_TEST_TRACE") && ps_clock()>=probe_at) {
        fprintf(stderr,"ANALYSIS START OBSERVATION: dataset-worker=%d library-worker=%d library-done=%d host=%u keyboard=%u flags=%llu\n",a->data.thread!=NULL,a->library_thread!=NULL,SDL_GetAtomicInt(&a->library_done),SDL_GetWindowID(a->window),SDL_GetWindowID(SDL_GetKeyboardFocus()),(unsigned long long)SDL_GetWindowFlags(a->window));probe_at=ps_clock()+1;
    }
    if(settle++<2)return;settle=0;
    if(a->data.thread || a->library_thread)return;
    switch(*stage) {
    case 0:
        if(SDL_GetKeyboardFocus()!=a->window){if(ps_clock()>=focus_retry){SDL_RaiseWindow(a->window);focus_retry=ps_clock()+.25;}return;}
        original=laboratory_capture(a);statistics=a->data.stats[0];a->tab=2;a->analysis_tab=0;break;
    case 1:
        center=a->dock_panels[PS_DOCK_WORKSPACE];initial_curve_height=a->plot_area.h;
        ok=a->analysis_layout_active && center.y==198 && a->dock_panels[PS_DOCK_SIDEBAR].w==232 && a->dock_panels[PS_DOCK_INSPECTOR].w==242 && a->data.total==301 && a->selected_count==1;
        {int w,h;SDL_GetWindowSize(a->window,&w,&h);ok=ok && a->import_bounds.h>0 && a->import_bounds.y+a->import_bounds.h<=h-28;
        printf("ANALYSIS LAYOUT: client=%dx%d middle=%.1f,%.1f,%.1f,%.1f import-bottom=%.1f\n",w,h,center.x,center.y,center.w,center.h,a->import_bounds.y+a->import_bounds.h);}
        printf("ANALYSIS TAB FIT: width=%.2f text=%.2f\n",a->analysis_tabs[0].w,a->workbench_fonts[2].width(a->workbench_fonts[2].userdata,a->workbench_fonts[2].height,"Messdaten",9));
        ok=ok && a->analysis_tabs[0].w>=a->workbench_fonts[2].width(a->workbench_fonts[2].userdata,a->workbench_fonts[2].height,"Messdaten",9);
        ok=ok && a->plot_buttons[2].w>=a->workbench_fonts[0].width(a->workbench_fonts[0].userdata,a->workbench_fonts[0].height,"Alles zeigen",12);
        *capture="analysis-both.bmp";test_mouse(a,a->analysis_side_buttons[0],true);break;
    case 2:test_mouse(a,a->analysis_side_buttons[0],false);break;
    case 3:
        ok=!a->dock_visible[PS_DOCK_SIDEBAR] && a->dock_visible[PS_DOCK_INSPECTOR] && a->dock_panels[PS_DOCK_WORKSPACE].w>center.w && a->plot_area.h>initial_curve_height;
        *capture="analysis-details.bmp";test_mouse(a,a->analysis_side_buttons[1],true);break;
    case 4:test_mouse(a,a->analysis_side_buttons[1],false);break;
    case 5:
        ok=!a->dock_visible[PS_DOCK_SIDEBAR] && !a->dock_visible[PS_DOCK_INSPECTOR] && a->dock_panels[PS_DOCK_WORKSPACE].w>center.w+400;
        *capture="analysis-center.bmp";test_mouse(a,a->analysis_side_buttons[0],true);break;
    case 6:test_mouse(a,a->analysis_side_buttons[0],false);break;
    case 7:
        ok=a->dock_visible[PS_DOCK_SIDEBAR] && !a->dock_visible[PS_DOCK_INSPECTOR];
        *capture="analysis-runs.bmp";a->tab=0;break;
    case 8:
        ok=!a->analysis_layout_active && a->preferences.sidebar_width==original.sidebar_width && a->preferences.inspector_width==original.inspector_width && a->preferences.dock.hidden==original.dock.hidden;
        a->tab=1;break;
    case 9:ok=a->laboratory_active && !a->analysis_layout_active;a->tab=2;a->analysis_tab=0;break;
    case 10:
        ok=a->analysis_layout_active && !a->laboratory_active && a->dock_visible[PS_DOCK_SIDEBAR] && !a->dock_visible[PS_DOCK_INSPECTOR];
        ok=ok && !memcmp(&statistics,&a->data.stats[0],sizeof statistics);
        if(ok)ok=nk_sdl_accessibility_request_focus(a->ui,NULL,"Laufdetails ein-/ausblenden");break;
    case 11:
        if(!nk_sdl_accessibility_is_focused(a->ui,NULL,"Laufdetails ein-/ausblenden"))return;
        test_window_key_mod(a->window,SDLK_SPACE,0);break;
    case 12:
        ok=a->dock_visible[PS_DOCK_INSPECTOR] && a->dock_visible[PS_DOCK_SIDEBAR] && a->dock_panels[PS_DOCK_WORKSPACE].w>0 && a->plot_area.w>0;
        *capture="analysis-restored.bmp";external=0;break;
    case 13:
        slot=a->dock_panels[external_panels[external]];
        ok=a->panels[external_panels[external]].action.w>0;
        test_mouse(a,a->panels[external_panels[external]].action,true);break;
    case 14:test_mouse(a,nk_rect(-32,250,0,0),true);break;
    case 15:test_mouse(a,nk_rect(-32,250,0,0),false);break;
    case 16: {
        ps_panel_window *p=&a->panels[external_panels[external]];
        struct nk_rect current=a->dock_panels[external_panels[external]];
        ok=p->detached && p->window && !(SDL_GetWindowFlags(p->window)&SDL_WINDOW_HIDDEN);
        ok=ok && current.x==slot.x && current.y==slot.y && current.w==slot.w && current.h==slot.h && a->data.total==301 && a->selected_count==1;
        char name[64];snprintf(name,sizeof name,"analysis-external-%u.bmp",external);
        join(p->capture,sizeof p->capture,a->project,name);break;
    }
    case 17: {
        SDL_Event close={0};close.type=SDL_EVENT_WINDOW_CLOSE_REQUESTED;
        close.window.windowID=SDL_GetWindowID(a->panels[external_panels[external]].window);SDL_PushEvent(&close);break;
    }
    case 18:
        ok=!a->panels[external_panels[external]].detached && a->data.total==301 && a->selected_count==1;
        if(ok && ++external<3){*stage=13;return;}
        *capture="analysis-reattached.bmp";break;
    case 19:
        ok=nk_sdl_accessibility_request_focus(a->ui,NULL,"Messlauflisten-Breite");break;
    case 20:
        if(!nk_sdl_accessibility_is_focused(a->ui,NULL,"Messlauflisten-Breite"))return;
        test_window_key_mod(a->window,SDLK_END,0);break;
    case 21: {
        ok=a->preferences.sidebar_width==480 && a->dock_panels[PS_DOCK_SIDEBAR].w==480;
        char path[4096];ps_layout_catalog catalog={0},loaded={0};ps_layout_state current=layout_capture(a);
        join(path,sizeof path,a->project,"analysis-wide-layout.bin");
        ok=ok && ps_layout_catalog_put(&catalog,"Auswerten",&current)==PS_OK && ps_layout_catalog_write(path,&catalog)==PS_OK && ps_layout_catalog_read(path,&loaded)==PS_OK && loaded.count==1 && loaded.entries[0].state.sidebar_width==480;
        preferences_capture(a);ps_preferences readback;join(path,sizeof path,a->project,"analysis-wide-preferences.bin");
        ok=ok && ps_preferences_write(path,&a->preferences)==PS_OK && ps_preferences_read(path,&readback)==PS_OK && readback.sidebar_width==480;
        *capture="analysis-wide.bmp";break;
    }
    case 22:open_settings(a);break;
    case 23:
        ok=a->tab==6 && !a->analysis_layout_active && a->settings_from_analysis && a->settings_draft.sidebar_width==480 && a->preferences.sidebar_width==original.sidebar_width;
        test_window_key_mod(a->window,SDLK_ESCAPE,0);break;
    case 24:
        ok=a->analysis_layout_active && a->preferences.sidebar_width==480;open_settings(a);break;
    case 25:test_mouse(a,a->settings_bounds[0],true);break;
    case 26:test_mouse(a,a->settings_bounds[0],false);break;
    case 27: {
        ps_preferences saved;
        ok=a->tab==2 && a->analysis_layout_active && a->preferences.sidebar_width==480 && a->dock_panels[PS_DOCK_SIDEBAR].w==480 && ps_preferences_read(a->preferences_path,&saved)==PS_OK && saved.sidebar_width==original.sidebar_width && saved.inspector_width==original.inspector_width && saved.log_height==original.log_height && saved.ui_size==a->preferences.ui_size;
        if(ok)ok=nk_sdl_accessibility_request_focus(a->ui,NULL,"Messlauflisten-Breite");break;
    }
    case 28:
        if(!nk_sdl_accessibility_is_focused(a->ui,NULL,"Messlauflisten-Breite"))return;
        test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 29:
        ok=a->preferences.sidebar_width==232 && a->dock_panels[PS_DOCK_SIDEBAR].w==232;
        a->tab=1;break;
    case 30:ok=a->laboratory_active && !a->analysis_layout_active;open_settings(a);break;
    case 31:test_mouse(a,a->settings_bounds[0],true);break;
    case 32:test_mouse(a,a->settings_bounds[0],false);break;
    case 33:
        ok=a->tab==1 && a->laboratory_active && a->preferences.inspector_width==320;
        a->tab=0;break;
    case 34: {
        ps_preferences saved;
        ok=!a->laboratory_active && !a->analysis_layout_active && a->preferences.sidebar_width==original.sidebar_width && a->preferences.inspector_width==original.inspector_width && a->preferences.log_height==original.log_height && a->selected_count==1 && a->data.total==301 && !memcmp(&statistics,&a->data.stats[0],sizeof statistics) && ps_preferences_read(a->preferences_path,&saved)==PS_OK && saved.sidebar_width==original.sidebar_width && saved.inspector_width==original.inspector_width;
        printf("ANALYSIS SETTINGS CONTEXT: workbench=%u,%u session-analysis=%u session-laboratory=%u valid=%d\n",a->preferences.sidebar_width,a->preferences.inspector_width,a->analysis_layout.sidebar_width,a->laboratory_layout.inspector_width,ok);
        a->tab=2;a->analysis_tab=0;break;
    }
    case 35:
        automatic_height=a->plot_area.h;ok=nk_sdl_accessibility_request_focus(a->ui,NULL,analysis_plot_separator_name());break;
    case 36:
        if(!nk_sdl_accessibility_is_focused(a->ui,NULL,analysis_plot_separator_name()))return;
        test_window_key_mod(a->window,SDLK_HOME,0);break;
    case 37:
        curve_height=a->plot_area.h;ok=a->analysis_plot_manual && a->analysis_plot_height==180;
        test_window_key_mod(a->window,SDLK_DOWN,0);break;
    case 38:
        ok=fabsf(a->plot_area.h-curve_height-10)<.01f;test_window_key_mod(a->window,SDLK_DOWN,SDL_KMOD_SHIFT);break;
    case 39:
        ok=fabsf(a->plot_area.h-curve_height-50)<.01f;test_mouse(a,a->analysis_plot_splitter,true);break;
    case 40:
        ok=a->analysis_plot_resizing;test_mouse(a,nk_rect(a->analysis_plot_splitter.x+a->analysis_plot_splitter.w/2,a->analysis_plot_splitter.y+24,0,0),true);break;
    case 41:
        ok=a->plot_area.h>curve_height+60;test_window_key_mod(a->window,SDLK_ESCAPE,0);break;
    case 42:test_mouse(a,a->analysis_plot_splitter,false);break;
    case 43:
        ok=!a->analysis_plot_resizing && fabsf(a->plot_area.h-curve_height-50)<.01f;
        test_mouse(a,a->analysis_plot_splitter,true);break;
    case 44:test_mouse(a,nk_rect(a->analysis_plot_splitter.x+a->analysis_plot_splitter.w/2,a->analysis_plot_splitter.y+24,0,0),true);break;
    case 45: {
        ok=a->analysis_plot_resizing && a->plot_area.h>curve_height+60;
        SDL_Event lost={0};lost.type=SDL_EVENT_WINDOW_FOCUS_LOST;lost.window.windowID=SDL_GetWindowID(a->window);lost.window.data1=(int)PS_TEST_MOUSE_ID;SDL_PushEvent(&lost);break;
    }
    case 46:test_mouse(a,a->analysis_plot_splitter,false);break;
    case 47: {
        bool requested=nk_sdl_accessibility_request_focus(a->ui,NULL,analysis_plot_separator_name());
        printf("ANALYSIS FOCUS CANCEL: resizing=%d actual-height=%.2f expected-height=%.2f wish=%.2f manual=%d focus-request=%d\n",a->analysis_plot_resizing,a->plot_area.h,curve_height+50,a->analysis_plot_height,a->analysis_plot_manual,requested);
        ok=!a->analysis_plot_resizing && fabsf(a->plot_area.h-curve_height-50)<.01f && requested;break;
    }
    case 48:
        if(!nk_sdl_accessibility_is_focused(a->ui,NULL,analysis_plot_separator_name()))return;
        test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 49:
        ok=!a->analysis_plot_manual && fabsf(a->plot_area.h-automatic_height)<.01f;
        *capture="analysis-curve-restored.bmp";ok=ok && panel_window_open(a,PS_DOCK_WORKSPACE);break;
    case 50:
        ok=a->panels[PS_DOCK_WORKSPACE].detached && nk_sdl_accessibility_request_focus(a->panels[PS_DOCK_WORKSPACE].ui,NULL,analysis_plot_separator_name());break;
    case 51:
        if(!nk_sdl_accessibility_is_focused(a->panels[PS_DOCK_WORKSPACE].ui,NULL,analysis_plot_separator_name()))return;
        test_window_key_mod(a->panels[PS_DOCK_WORKSPACE].window,SDLK_HOME,0);break;
    case 52:
        curve_height=a->plot_area.h;test_window_key_mod(a->panels[PS_DOCK_WORKSPACE].window,SDLK_DOWN,0);break;
    case 53:
        ok=fabsf(a->plot_area.h-curve_height-10)<.01f;test_window_key_mod(a->panels[PS_DOCK_WORKSPACE].window,SDLK_DOWN,SDL_KMOD_SHIFT);break;
    case 54:
        ok=fabsf(a->plot_area.h-curve_height-50)<.01f;test_window_key_mod(a->panels[PS_DOCK_WORKSPACE].window,SDLK_END,0);break;
    case 55:
        ok=a->analysis_plot_manual && a->plot_area.h>curve_height+50 && a->analysis_plot_height<=700;
        test_window_mouse_count(a->panels[PS_DOCK_WORKSPACE].window,a->analysis_plot_splitter,true,2);break;
    case 56:test_window_mouse(a->panels[PS_DOCK_WORKSPACE].window,a->analysis_plot_splitter,false);break;
    case 57: {
        ok=!a->analysis_plot_manual;SDL_Event close={0};close.type=SDL_EVENT_WINDOW_CLOSE_REQUESTED;close.window.windowID=SDL_GetWindowID(a->panels[PS_DOCK_WORKSPACE].window);SDL_PushEvent(&close);break;
    }
    case 58:
        ok=!a->panels[PS_DOCK_WORKSPACE].detached && !a->analysis_plot_manual && a->data.total==301 && a->selected_count==1 && !memcmp(&statistics,&a->data.stats[0],sizeof statistics);
        printf("ANALYSIS CURVE SEPARATOR: actual 10/40 px, pointer Escape/focus cancel, automatic reset and native window passed=%d\n",ok);
        SDL_GetWindowSize(a->window,&viewport_width,&viewport_height);
        a->preferences.sidebar_width=480;a->preferences.inspector_width=480;SDL_SetWindowSize(a->window,1080,740);break;
    case 59:
        ok=a->preferences.inspector_width==480 && a->dock_panels[PS_DOCK_INSPECTOR].w<480;
        {bool found=false;for(unsigned id=0;id<PS_DOCK_NODES;id++)if(dock_context_split(&a->preferences.dock,&a->preferences.dock.nodes[id])){test_mouse(a,a->dock_splitters[id],true);found=true;}ok=ok && found;}break;
    case 60:
        ok=a->preferences.inspector_width==480 && a->dock_resize!=0;
        for(unsigned id=0;id<PS_DOCK_NODES;id++)if(dock_context_split(&a->preferences.dock,&a->preferences.dock.nodes[id]))test_mouse(a,a->dock_splitters[id],false);break;
    case 61:
        ok=a->preferences.inspector_width==480 && !a->dock_resize && a->data.total==301 && !memcmp(&statistics,&a->data.stats[0],sizeof statistics);
        printf("ANALYSIS CLAMPED WISH: inspector-desired=%u actual=%.1f press/release-preserved=%d\n",a->preferences.inspector_width,a->dock_panels[PS_DOCK_INSPECTOR].w,ok);
        a->preferences.sidebar_width=232;a->preferences.inspector_width=242;SDL_SetWindowSize(a->window,viewport_width,viewport_height);break;
    case 62:
        if(SDL_GetKeyboardFocus()!=a->window){SDL_RaiseWindow(a->window);return;}
        for(unsigned id=0;id<PS_DOCK_NODES;id++)if(dock_sidebar_split(&a->preferences.dock,&a->preferences.dock.nodes[id]))test_mouse(a,a->dock_splitters[id],true);break;
    case 63: {
        /* Posted fixture events do not move the physical pointer. SDL 3.2.30
         * captures its mouse-focus window, which can be absent on Windows. */
        bool mouse_focus=SDL_GetMouseFocus()==a->window;
        bool captured=(SDL_GetWindowFlags(a->window)&SDL_WINDOW_MOUSE_CAPTURE)!=0;
        printf("ANALYSIS POINTER HOST: logical-active=%d transaction=%d mouse-focus=%d captured=%d\n",a->dock_resize!=0,a->dock_resize_transaction,mouse_focus,captured);
        ok=a->dock_resize!=0 && a->dock_resize_transaction && captured==mouse_focus;a->tab=1;break;
    }
    case 64:
        ok=!a->dock_resize && !a->dock_resize_transaction && !(SDL_GetWindowFlags(a->window)&SDL_WINDOW_MOUSE_CAPTURE) && a->analysis_layout.sidebar_width==232;
        test_mouse(a,nk_rect(0,0,0,0),false);a->tab=2;break;
    case 65:
        ok=a->analysis_layout_active && a->preferences.sidebar_width==232 && a->data.total==301 && !memcmp(&statistics,&a->data.stats[0],sizeof statistics);
        printf("ANALYSIS CONTEXT POINTER: capture released and original workspace wish restored=%d\n",ok);
        test_mouse(a,a->analysis_plot_splitter,true);break;
    case 66:
        ok=a->analysis_plot_resizing && !a->analysis_plot_manual;test_mouse(a,a->analysis_plot_splitter,false);break;
    case 67:
        ok=!a->analysis_plot_manual && nk_sdl_accessibility_request_focus(a->ui,NULL,analysis_plot_separator_name());break;
    case 68:
        if(!nk_sdl_accessibility_is_focused(a->ui,NULL,analysis_plot_separator_name()))return;
        test_window_key_mod(a->window,SDLK_END,0);break;
    case 69: {
        scroll_view=a->data_views[a->plot_channel];struct nk_rect area=a->dock_panels[PS_DOCK_WORKSPACE];
        test_mouse(a,nk_rect(area.x+30,area.y+area.h-14,0,0),true);break;
    }
    case 70: {
        struct nk_rect area=a->dock_panels[PS_DOCK_WORKSPACE];test_mouse(a,nk_rect(area.x+30,area.y+area.h-14,0,0),false);
        SDL_Event wheel={0};wheel.type=SDL_EVENT_MOUSE_WHEEL;wheel.wheel.windowID=SDL_GetWindowID(a->window);wheel.wheel.which=PS_TEST_MOUSE_ID;wheel.wheel.y=-12;SDL_PushEvent(&wheel);break;
    }
    case 71: {
        struct nk_rect area=a->dock_panels[PS_DOCK_WORKSPACE];
        ok=a->raw_export_bounds.h>0 && a->raw_export_bounds.y>=area.y && a->raw_export_bounds.y+a->raw_export_bounds.h<=area.y+area.h && !memcmp(&scroll_view,&a->data_views[a->plot_channel],sizeof scroll_view) && !memcmp(&statistics,&a->data.stats[0],sizeof statistics);
        bool reachable=nk_sdl_accessibility_request_focus(a->ui,NULL,"CSV · Alle Rohdaten ↗");
        struct nk_window *workspace=nk_window_find(a->ui,"workspace");
        printf("ANALYSIS STATISTICS SCROLL: offset=%u button-bottom=%.1f viewport-bottom=%.1f reachable=%d unchanged-view-and-statistics=%d\n",workspace?workspace->scrollbar.y:0,a->raw_export_bounds.y+a->raw_export_bounds.h,area.y+area.h,reachable,ok);
        ok=ok && reachable;*capture="analysis-statistics-scrolled.bmp";a->quitting=true;break;
    }
    }
    if(!ok){*capture="analysis-failed.bmp";fprintf(stderr,"Analysis workspace stage %d failed: %s\n",*stage,a->status);*code=1;a->quitting=true;}
    if(a->quitting)printf("ANALYSIS WORKSPACE LAYOUT SELF-TEST: %s\n",*code?"FAILED":"PASSED");
    (*stage)++;
}
