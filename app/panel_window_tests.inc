static bool panel_test_start(app *a,const char *root,const char *mode) {
    char variant[96];snprintf(variant,sizeof variant,"laboratory-%s",mode+13);
    if(!laboratory_test_start(a,root,variant))return false;
    join(a->preferences_path,sizeof a->preferences_path,root,"preferences.bin");
    if(strstr(mode,"legacy")) {
        char path[4096],backup[4096];join(path,sizeof path,root,"preferences.bin");
        ps_preferences old=a->preferences;old.workspace=1;
        if(!ps_dock_float_panel(&old.dock,PS_DOCK_INSPECTOR,(ps_dock_float){500,120,320,480}) ||
            ps_preferences_write(path,&old)!=PS_OK)return false;
        preferences_start(a,path);
        snprintf(backup,sizeof backup,"%s.before-fixed-panels",path);ps_preferences preserved;
        if(ps_preferences_read(backup,&preserved)!=PS_OK || memcmp(&preserved,&old,sizeof old))return false;
    }
    a->preferences.dock=PS_DOCK_DEFAULT;a->show_log=true;
    /* The first entry's intended default is covered by laboratory_workflow. */
    laboratory_context(a);a->preferences.dock=PS_DOCK_DEFAULT;
    return true;
}
static void panel_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    static unsigned panel,phase,settle,minimum_phase;static struct nk_rect drop;static ps_dock_layout layout;static struct nk_rect slot;
    static float timeline_wish;static uint32_t log_wish;static nk_uint compact_scroll;
    static double time;static float yaw,curve_before,curve_width,curve_pixels_before;static double curve_focus_retry;static uint32_t selection,source;static int source_bytes,main_width,main_height;static SDL_WindowID ids[PS_PANEL_COUNT];
    bool ok=true;
    switch(*stage) {
    case 0:if(settle++<3)return;build_project(a);break;
    case 1:if(!idle(a) || a->library_thread)return;ok=a->built;start_run(a);break;
    case 2:if(!a->hello || a->simulation_time<.2)return;command(a,PS_MSG_PAUSE);a->show_log=true;
        if(!a->diagnostic_count) {
            parse_diagnostic(a,"native-compact-log-sentinel.c:17:1: warning: Native compact log display probe");
            puts("PANEL SYNTHETIC DIAGNOSTIC: warning-free compiler; explicit display fixture only");
        }
        timeline_wish=a->laboratory_timeline_height;log_wish=a->preferences.log_height;break;
    case 3:
        if(!a->paused)return;
        ok=a->dock_panels[PS_DOCK_WORKSPACE].h>=LABORATORY_SCENE_MIN+1+LABORATORY_MEASURE_MIN &&
            a->scene_viewport_bounds.h>=LABORATORY_SCENE_MIN &&
            a->dock_panels[PS_DOCK_WORKSPACE].h-a->scene_viewport_bounds.h-1>=LABORATORY_MEASURE_MIN &&
            a->dock_panels[PS_DOCK_LOG].h>=LABORATORY_LOG_MIN &&
            a->laboratory_timeline_height==timeline_wish && a->preferences.log_height==log_wish;
        if(!ok)break;
        if(a->diagnostic_count && a->dock_panels[PS_DOCK_LOG].h<=LABORATORY_LOG_MIN+1) {
            struct nk_rect r=a->dock_panels[PS_DOCK_LOG];float top=r.y+dock_panel_header_height(a,PS_DOCK_LOG);
            if(!minimum_phase) {
                *capture="panel-compact-log-start.png";settle=0;minimum_phase=1;
                test_plot_wheel(a,r.x+50,r.y+r.h-10,-1000);return;
            }
            if(settle++<2)return;
            if(minimum_phase==1) {
                struct nk_window *log=nk_window_find(a->ui,"log");
                nk_uint y=log?log->scrollbar.y:0;compact_scroll=y;
                struct nk_rect last=a->diagnostic_bounds[a->diagnostic_count-1];
                ok=y>0 && last.y+last.h/2>=top && last.y+last.h/2<r.y+r.h;
                if(!ok){fprintf(stderr,"PANEL COMPACT LOG END: scroll=%u last=%.3f,%.3f clip=%.3f,%.3f\n",y,last.y,last.h,top,r.y+r.h);break;}
                printf("PANEL COMPACT TEXT: clip=%.3f,%.3f,%.3f,%.3f row=%.3f,%.3f,%.3f,%.3f bytes=%zu\n",r.x,top,r.w,r.y+r.h-top,last.x,last.y,last.w,last.h,strlen(a->diagnostics[a->diagnostic_count-1].text));
                *capture="panel-compact-log-end.png";settle=0;minimum_phase=2;
                test_plot_wheel(a,r.x+50,r.y+r.h-10,1000);return;
            }
            if(minimum_phase==2) {
                struct nk_window *log=nk_window_find(a->ui,"log");
                ok=log && !log->scrollbar.y && a->diagnostic_mode_bounds.y>=top &&
                    a->diagnostic_mode_bounds.y+a->diagnostic_mode_bounds.h<=r.y+r.h;
                if(!ok)break;
                printf("PANEL COMPACT LOG: last-diagnostic-visible=1 end-scroll=%u returned=0 diagnostics=%d\n",compact_scroll,a->diagnostic_count);
                minimum_phase=3;
            }
        }
        printf("PANEL MINIMUM: scene=%.3f measure=%.3f log=%.3f timeline=%.3f wishes=%.3f,%u\n",a->scene_viewport_bounds.h,a->laboratory_measure_bounds.h+48,a->dock_panels[PS_DOCK_LOG].h,laboratory_dimension_value(a,1),a->laboratory_timeline_height,a->preferences.log_height);
        time=display_time(a);yaw=a->yaw;selection=a->scene_selected_id;
        layout=a->preferences.dock;slot=a->dock_panels[PS_DOCK_INSPECTOR];
        test_mouse(a,a->dock_headers[PS_DOCK_INSPECTOR],true);break;
    case 4:test_plot_motion(a,300,280);break;
    case 5:test_mouse(a,nk_rect(300,280,1,1),false);break;
    case 6:
        ok=!a->dock_dragging && !a->preferences.dock.floating && !memcmp(&layout,&a->preferences.dock,sizeof layout);
        panel=phase=0;break;
    case 7:
        if(panel==PS_PANEL_COUNT){*capture="panels-all-detached.bmp";break;}
        if(phase==0){
            ok=a->panels[panel].action.w>0;if(!ok)break;
            if(panel==PS_PANEL_MEASURE) {
                struct nk_rect r=a->panels[panel].action,s=a->dock_panels[PS_DOCK_WORKSPACE];
                struct nk_vec2 center=nk_vec2(r.x+r.w/2,r.y+r.h/2);
                ok=r.y>=s.y && r.y+r.h<=s.y+s.h;
                for(unsigned i=0;i<PS_DOCK_NODES;i++)if(a->dock_splitters[i].w>0)
                    ok=ok && !nk_input_is_mouse_hovering_rect(&(struct nk_input){.mouse.pos=center},a->dock_splitters[i]);
                if(!ok)break;
            }
            if(panel==PS_PANEL_MEASURE && SDL_getenv("PHYSIM_TEST_TRACE")) {
                struct nk_rect r=a->panels[panel].action,s=a->dock_panels[PS_DOCK_WORKSPACE];
                printf("PANEL MEASURE INPUT: log=%d diagnostics=%d action=%.3f,%.3f,%.3f,%.3f workspace=%.3f,%.3f,%.3f,%.3f\n",a->show_log,a->diagnostic_count,r.x,r.y,r.w,r.h,s.x,s.y,s.w,s.h);
                for(unsigned i=0;i<PS_DOCK_NODES;i++)if(a->dock_splitters[i].w>0){r=a->dock_splitters[i];printf("PANEL MEASURE DOCK %u: %.3f,%.3f,%.3f,%.3f\n",i,r.x,r.y,r.w,r.h);}
                *capture="panel-before-measure-pull.png";
            }
            int w,h;SDL_GetWindowSize(a->window,&w,&h);drop=nk_rect((float)w+100,220,1,1);
            test_mouse(a,a->panels[panel].action,true);phase++;return;
        }
        if(phase==1){test_plot_motion(a,drop.x,drop.y);phase++;return;}
        if(phase==2){test_mouse(a,drop,false);phase++;return;}
        if(phase==3){phase++;return;}
        if(phase==4) {
            ps_panel_window *p=&a->panels[panel];ok=p->detached && p->window && p->ui && p->graphics && p->window!=a->window;
            if(!ok){fprintf(stderr,"PANEL PULL STATE: detached=%d pull=%u dragging=%d dock-resize=%u lab-resize=%d request=%u log=%d diagnostics=%d\n",p->detached,a->panel_pull,a->panel_pull_dragging,a->dock_resize,a->laboratory_resize,a->panel_open_request,a->show_log,a->diagnostic_count);break;}
            ids[panel]=SDL_GetWindowID(p->window);
            for(unsigned j=0;j<panel;j++)ok=ok && ids[j]!=ids[panel];
            int x=0,y=0,w=0,h=0;SDL_GetWindowPosition(a->window,&x,&y);SDL_GetWindowSize(a->window,&w,&h);
            SDL_SetWindowPosition(p->window,x+w-80,y+40);
            int display_count=0;SDL_DisplayID *displays=SDL_GetDisplays(&display_count);
            if(displays)for(int d=0;d<display_count;d++)if(displays[d]!=SDL_GetDisplayForWindow(a->window)) {
                SDL_Rect monitor;if(SDL_GetDisplayUsableBounds(displays[d],&monitor))SDL_SetWindowPosition(p->window,monitor.x+30,monitor.y+30);
                break;
            }
            SDL_free(displays);
            SDL_SetWindowSize(p->window,panel==PS_DOCK_INSPECTOR?460:900,panel==PS_PANEL_TIMELINE?250:650);
            phase++;return;
        }
        if(phase++<10)return;
        {ps_panel_window *p=&a->panels[panel];int x,y,w,h,mx,my,mw,mh;
            SDL_GetWindowPosition(p->window,&x,&y);SDL_GetWindowSize(p->window,&w,&h);
            SDL_GetWindowPosition(a->window,&mx,&my);SDL_GetWindowSize(a->window,&mw,&mh);
            ok=x<mx || y<my || x+w>mx+mw || y+h>my+mh;
            printf("PANEL %u: window=%u bounds=%d,%d,%d,%d outside=%d display=%u main-display=%u density=%.3f\n",panel,ids[panel],x,y,w,h,ok,SDL_GetDisplayForWindow(p->window),SDL_GetDisplayForWindow(a->window),SDL_GetWindowPixelDensity(p->window));
            char file[80];snprintf(file,sizeof file,"panel-%u-detached.bmp",panel);join(p->capture,sizeof p->capture,a->project,file);}
        if(!ok)break;panel++;phase=0;return;
    case 8:
        ok=display_time(a)==time && a->yaw==yaw && a->scene_selected_id==selection && a->runner.running && a->paused;
        test_window_mouse(a->panels[PS_PANEL_MEASURE].window,a->laboratory_switch,true);break;
    case 9:test_window_mouse(a->panels[PS_PANEL_MEASURE].window,a->laboratory_switch,false);break;
    case 10: {
        if(settle++<7)return;
        bool enabled=false,checked=false,found=nk_sdl_accessibility_toggle_state(a->panels[PS_PANEL_MEASURE].ui,"Kurven anzeigen",&enabled,&checked);
        printf("PANEL SWITCH: curves=%d found=%d enabled=%d checked=%d\n",a->laboratory_curves,found,enabled,checked);
        ok=a->laboratory_curves && found && enabled && checked;
        if(!ok)break;
        join(a->panels[PS_PANEL_MEASURE].capture,sizeof a->panels[PS_PANEL_MEASURE].capture,a->project,"panel-measure-curves.bmp");
        panel=phase=0;*stage=300;return;
    }
    case 300:
        SDL_SetWindowSize(a->panels[PS_PANEL_MEASURE].window,720,440);settle=0;break;
    case 301:
        if(settle++<3)return;
        ok=a->panels[PS_PANEL_MEASURE].measure_width>0 && fabsf(a->panels[PS_PANEL_MEASURE].measure_width-a->dock_panels[PS_DOCK_WORKSPACE].w)>100;
        ok=ok && nk_sdl_accessibility_request_focus(a->panels[PS_PANEL_MEASURE].ui,NULL,laboratory_separator_name(2));break;
    case 302:
        if(!nk_sdl_accessibility_is_focused(a->panels[PS_PANEL_MEASURE].ui,NULL,laboratory_separator_name(2))) {
            if(ps_clock()>=curve_focus_retry){SDL_RaiseWindow(a->panels[PS_PANEL_MEASURE].window);nk_sdl_accessibility_request_focus(a->panels[PS_PANEL_MEASURE].ui,NULL,laboratory_separator_name(2));curve_focus_retry=ps_clock()+.25;}
            return;
        }
        curve_before=a->laboratory_curve_ratio;curve_width=a->panels[PS_PANEL_MEASURE].measure_width;curve_pixels_before=a->laboratory_plots[0].w;settle=0;
        test_window_key_mod(a->panels[PS_PANEL_MEASURE].window,SDLK_RIGHT,0);break;
    case 303:
        if(settle++<2)return;
        ok=fabsf((a->laboratory_curve_ratio-curve_before)*(curve_width-9)-10)<.1f;
        ok=ok && fabsf(a->laboratory_plots[0].w-curve_pixels_before-10)<=1;
        printf("PANEL CURVE KEYBOARD: child-width=%.3f main-width=%.3f right-pixels=%.6f actual-width-change=%.6f\n",curve_width,a->dock_panels[PS_DOCK_WORKSPACE].w,(a->laboratory_curve_ratio-curve_before)*(curve_width-9),a->laboratory_plots[0].w-curve_pixels_before);
        join(a->panels[PS_PANEL_MEASURE].capture,sizeof a->panels[PS_PANEL_MEASURE].capture,a->project,"panel-measure-curve-keyboard.bmp");settle=0;
        test_window_key_mod(a->panels[PS_PANEL_MEASURE].window,SDLK_LEFT,SDL_KMOD_SHIFT);break;
    case 304:
        if(settle++<2)return;
        ok=fabsf((a->laboratory_curve_ratio-curve_before)*(curve_width-9)+30)<.1f;
        ok=ok && fabsf(a->laboratory_plots[0].w-curve_pixels_before+30)<=1;
        printf("PANEL CURVE SHIFT: net-pixels=%.6f actual-width-change=%.6f\n",(a->laboratory_curve_ratio-curve_before)*(curve_width-9),a->laboratory_plots[0].w-curve_pixels_before);
        test_window_key_mod(a->panels[PS_PANEL_MEASURE].window,SDLK_RETURN,0);break;
    case 305:
        ok=a->laboratory_curve_ratio==.5f && display_time(a)==time && a->yaw==yaw;
        a->tab=0;*stage=90;if(ok)return;break;
    case 90:
        ok=a->panels[PS_DOCK_WORKSPACE].tab==0 && panel_is_detached(a,PS_DOCK_WORKSPACE) && a->runner.running && a->paused;
        a->tab=1;*stage=91;if(ok)return;break;
    case 91:
        ok=a->panels[PS_DOCK_WORKSPACE].tab==1 && panel_is_detached(a,PS_DOCK_WORKSPACE) && display_time(a)==time;
        test_window_key_mod(a->panels[PS_DOCK_WORKSPACE].window,SDLK_F1,0);*stage=92;if(ok)return;break;
    case 92:
        if(!a->doc_visible || !a->documentation || !a->doc_window)return;
        ok=a->tab==1 && a->runner.running && a->paused;
        {SDL_Event close={0};close.type=SDL_EVENT_WINDOW_CLOSE_REQUESTED;close.window.windowID=SDL_GetWindowID(a->doc_window);SDL_PushEvent(&close);}
        break;
    case 93:ok=!a->doc_visible && display_time(a)==time;*stage=100;if(ok)return;break;
    case 100:if(!timeline_test_key(a,a->panels[PS_PANEL_TIMELINE].window,a->panels[PS_PANEL_TIMELINE].ui,SDLK_LEFT))return;break;
    case 101:break;
    case 102:break;
    case 103:
        ok=a->timeline_browsing && display_time(a)<=time && a->runner.running && a->paused;
        test_window_mouse(a->panels[PS_PANEL_TIMELINE].window,a->timeline_bounds[2],true);break;
    case 104:test_window_mouse(a->panels[PS_PANEL_TIMELINE].window,a->timeline_bounds[2],false);break;
    case 105:break;
    case 106:ok=!a->timeline_browsing && display_time(a)==time;test_window_key_mod(a->panels[PS_DOCK_WORKSPACE].window,SDLK_F6,0);break;
    case 107:if(a->paused || a->simulation_time<time+.1)return;test_window_key_mod(a->panels[PS_DOCK_WORKSPACE].window,SDLK_F6,0);break;
    case 108:
        if(!a->paused)return;time=display_time(a);
        SDL_GetWindowSize(a->window,&main_width,&main_height);
        SDL_SetWindowSize(a->panels[PS_DOCK_WORKSPACE].window,main_width+40,main_height+20);
        open_settings(a);*stage=200;return;
    case 200:
        if(a->panels[PS_DOCK_WORKSPACE].tab!=6 || !a->settings_bounds[0].w)return;
        test_window_mouse(a->panels[PS_DOCK_WORKSPACE].window,a->settings_bounds[0],true);break;
    case 201:test_window_mouse(a->panels[PS_DOCK_WORKSPACE].window,a->settings_bounds[0],false);break;
    case 202:break;
    case 203: {
        if(a->tab!=1 || a->panels[PS_DOCK_WORKSPACE].tab!=1)return;
        ps_preferences saved;ok=ps_preferences_read(a->preferences_path,&saved)==PS_OK && saved.width==(unsigned)main_width && saved.height==(unsigned)main_height;
        printf("PANEL SETTINGS OWNER: saved=%u,%u main=%d,%d valid=%d\n",saved.width,saved.height,main_width,main_height,ok);
        *stage=11;if(ok)return;break;
    }
    case 11:
        if(panel==PS_PANEL_COUNT)break;
        if(!phase){
            SDL_Event close={0};close.type=SDL_EVENT_WINDOW_CLOSE_REQUESTED;close.window.windowID=ids[panel];SDL_PushEvent(&close);
            phase=1;return;
        }
        if(phase++<4)return;
        ok=!a->panels[panel].detached && SDL_GetWindowFlags(a->panels[panel].window)&SDL_WINDOW_HIDDEN;
        if(!ok)break;panel++;phase=0;return;
    case 12:
        ok=display_time(a)==time && a->yaw==yaw && a->scene_selected_id==selection && !memcmp(&layout,&a->preferences.dock,sizeof layout) &&
            a->dock_panels[PS_DOCK_INSPECTOR].x==slot.x && a->dock_panels[PS_DOCK_INSPECTOR].w==slot.w &&
            a->laboratory_timeline_height==timeline_wish && a->preferences.log_height==log_wish;
        *capture="panels-reattached.bmp";test_mouse(a,a->panels[PS_DOCK_INSPECTOR].action,true);break;
    case 13:test_plot_motion(a,drop.x,drop.y);*stage=130;return;
    case 130:test_mouse(a,drop,false);*stage=14;return;
    case 14:if(!a->panels[PS_DOCK_INSPECTOR].detached){test_mouse(a,drop,false);return;}
        {SDL_Event e={0};e.type=SDL_EVENT_WINDOW_CLOSE_REQUESTED;e.window.windowID=ids[PS_DOCK_INSPECTOR];SDL_PushEvent(&e);break;}
    case 15:if(a->panels[PS_DOCK_INSPECTOR].detached)return;ok=!a->panels[PS_DOCK_INSPECTOR].detached && a->runner.running && a->paused;command(a,PS_MSG_STOP);a->stop_at=ps_clock();break;
    case 16:if(!idle(a) || a->library_thread)return;ok=a->data.total>2;a->tab=0;break;
    case 17:
        source_bytes=nk_str_len_char(&a->experiment.string);
        source=ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)source_bytes);
        test_mouse(a,a->panels[PS_DOCK_WORKSPACE].action,true);break;
    case 18:test_plot_motion(a,drop.x,drop.y);break;
    case 19:test_mouse(a,drop,false);phase=0;break;
    case 20:
        if(phase++<2)return;
        ok=a->panels[PS_DOCK_WORKSPACE].detached && a->panels[PS_DOCK_WORKSPACE].tab==0;
        test_window_mouse(a->panels[PS_DOCK_WORKSPACE].window,a->source_editor_bounds,true);break;
    case 21:test_window_mouse(a->panels[PS_DOCK_WORKSPACE].window,a->source_editor_bounds,false);break;
    case 22:test_window_key_mod(a->panels[PS_DOCK_WORKSPACE].window,SDLK_END,PS_UI_COMMAND_MOD);break;
    case 23: {
        SDL_Event text={0};text.type=SDL_EVENT_TEXT_INPUT;text.text.windowID=SDL_GetWindowID(a->panels[PS_DOCK_WORKSPACE].window);
        text.text.text="Q";SDL_PushEvent(&text);
        SDL_Event close={0};close.type=SDL_EVENT_WINDOW_CLOSE_REQUESTED;close.window.windowID=text.text.windowID;SDL_PushEvent(&close);break;
    }
    case 24:
        if(a->panels[PS_DOCK_WORKSPACE].detached)return;
        ok=a->dirty && nk_str_len_char(&a->experiment.string)==source_bytes+1 && nk_str_get_const(&a->experiment.string)[source_bytes]=='Q';break;
    case 25:test_window_key_mod(a->window,SDLK_Z,PS_UI_COMMAND_MOD);break;
    case 26:break;
    case 27:
        ok=!a->panels[PS_DOCK_WORKSPACE].detached && source==ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
        *capture="panels-editor-return.bmp";a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"PANEL failed stage %d panel %u phase %u: %s\n",*stage,panel,phase,a->status);*code=1;a->quitting=true;}
    if(a->quitting)printf("FIXED PANELS %s SELF-TEST: %s\n",mode,*code?"FAILED":"PASSED");
    (*stage)++;
}
