static bool laboratory_test_start(app *a,const char *root,const char *mode) {
    bool custom=strstr(mode,"custom")!=NULL,read=strstr(mode,"-read")!=NULL;
    const char *reuse=SDL_getenv("PHYSIM_TEST_LAB_PROJECT");
    if(read){join(a->project_input,sizeof a->project_input,root,"project");open_project(a);}
    else if(reuse){snprintf(a->project_input,sizeof a->project_input,"%s",reuse);open_project(a);}
    else {join(a->project_input,sizeof a->project_input,root,"project");a->example=strstr(mode,"phys")?8:strstr(mode,"projectile")?1:0;new_project(a);}
    if(!a->loaded)return false;
    ps_preferences p=PS_PREFERENCES_DEFAULT;p.theme=strstr(mode,"light")?PS_THEME_LIGHT:strstr(mode,"contrast")?PS_THEME_HIGH_CONTRAST:PS_THEME_DARK;
    if(strstr(mode,"large"))p.ui_size=22;
    if(custom) {
        char path[4096];join(path,sizeof path,root,"preferences.bin");
        if(!read){ps_dock_hide(&p.dock,PS_DOCK_LOG);p.view_flags&=~PS_VIEW_LOG;p.sidebar_width=288;p.inspector_width=358;
            if(ps_preferences_write(path,&p)!=PS_OK)return false;}
        preferences_start(a,path);
    } else if(!strstr(mode,"startup"))preferences_apply(a,&p);
    a->tab=1;a->show_log=false;
    SDL_SetWindowSize(a->window,strstr(mode,"narrow")?1080:1440,strstr(mode,"narrow")?740:1000);
    return true;
}
static void laboratory_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    static unsigned combo,phase,warmup;static float dimension;static struct nk_rect grip;static double time;static float yaw;static uint32_t selected;static char image[80];
    static double focus_retry;
    bool ok=true;
    if(strstr(mode,"startup")) {
        if(warmup++<3)return;
        struct nk_rect r=a->scene_viewport_bounds;
        printf("LABORATORY STARTUP: theme=%u scene=%.1f,%.1f,%.1f,%.1f\n",a->preferences.theme,r.x,r.y,r.w,r.h);
        *capture="laboratory-startup.bmp";a->quitting=true;printf("LABORATORY STARTUP SELF-TEST: PASSED\n");return;
    }
    if(strstr(mode,"custom")) {
        switch(*stage) {
        case 0:
            if(warmup++<3)return;
            ok=!(a->preferences.dock.hidden&9) && a->preferences.sidebar_width==288 && a->preferences.inspector_width==358 && !a->laboratory_curves;
            if(strstr(mode,"-read")){*capture="laboratory-custom-read.bmp";a->quitting=true;break;}
            test_mouse(a,a->dock_closes[PS_DOCK_INSPECTOR],true);break;
        case 1:test_mouse(a,nk_rect(1,1,1,1),false);break;
        case 2:ok=(a->preferences.dock.hidden&8)!=0;a->tab=0;break;
        case 3:ok=!(a->preferences.dock.hidden&9) && a->preferences.inspector_width==358;a->tab=1;break;
        case 4:ok=(a->preferences.dock.hidden&8)!=0 && !(a->preferences.dock.hidden&1) && !a->laboratory_curves;
            *capture="laboratory-custom-session.bmp";a->quitting=true;break;
        }
        if(!ok){*code=1;a->quitting=true;}
        if(a->quitting)printf("LABORATORY CUSTOM SELF-TEST: %s\n",*code?"FAILED":"PASSED");
        (*stage)++;return;
    }
    switch(*stage) {
    case 0:
        if(warmup++<3)return;
        ok=(a->preferences.dock.hidden&1) && !(a->preferences.dock.hidden&8) && !a->laboratory_curves;
        build_project(a);break;
    case 1:if(!idle(a) || a->library_thread)return;ok=a->built;start_run(a);break;
    case 2:if(!a->hello || a->simulation_time<.2)return;command(a,PS_MSG_PAUSE);break;
    case 3:
        if(!a->paused)return;
        time=display_time(a);yaw=a->yaw;
        for(unsigned i=0;i<a->scene.count;i++)if(a->scene.objects[i].id==3){a->scene_selected=true;a->scene_selected_slot=i;a->scene_selected_id=3;}
        selected=a->scene_selected_id;*capture="laboratory-initial.bmp";break;
    case 4: {
        bool tree=(a->preferences.dock.hidden&1)==0,inspector=(a->preferences.dock.hidden&8)==0;
        bool target_tree=(combo&1)!=0,target_inspector=(combo&2)!=0,target_curves=(combo&4)!=0;
        if(phase==1){test_mouse(a,nk_rect(1,1,1,1),false);phase=2;return;}
        if(tree!=target_tree){test_mouse(a,a->laboratory_sides[0],true);phase=1;return;}
        if(inspector!=target_inspector){test_mouse(a,a->laboratory_sides[1],true);phase=1;return;}
        if(a->laboratory_curves!=target_curves){test_mouse(a,a->laboratory_switch,true);phase=1;return;}
        if(phase==2){phase=0;return;}
        ok=a->scene_viewport_bounds.w>150 && a->scene_viewport_bounds.h>60 && display_time(a)==time && a->yaw==yaw && a->scene_selected_id==selected;
        snprintf(image,sizeof image,"laboratory-state-%u.bmp",combo);*capture=image;
        printf("LABORATORY STATE %u: scene %.1f %.1f %.1f %.1f time %.9g\n",combo,a->scene_viewport_bounds.x,a->scene_viewport_bounds.y,a->scene_viewport_bounds.w,a->scene_viewport_bounds.h,time);
        if(ok && ++combo<8)return;
        break;
    }
    case 5:
        /* Session switches preserve each workspace's layout independently. */
        a->tab=0;break;
    case 6:a->tab=1;break;
    case 7:
        ok=!(a->preferences.dock.hidden&9) && a->laboratory_curves && display_time(a)==time && a->yaw==yaw;
        timeline_select(a,0);break;
    case 8:
        ok=a->timeline_browsing && display_time(a)==0 && a->runner.running && a->paused;
        *capture="laboratory-history.bmp";timeline_live(a);command(a,PS_MSG_STOP);a->stop_at=ps_clock();break;
    case 9:
        if(!idle(a) || a->library_thread)return;
        ok=a->data.total>1;
        {bool enabled=false,checked=false;bool found=nk_sdl_accessibility_toggle_state(a->ui,"Kurven anzeigen",&enabled,&checked);printf("LABORATORY SWITCH: found=%d enabled=%d checked=%d workspace=%.1f,%.1f,%.1f,%.1f target=%.1f,%.1f,%.1f,%.1f scene=%.1f,%.1f,%.1f,%.1f\n",found,enabled,checked,a->dock_panels[1].x,a->dock_panels[1].y,a->dock_panels[1].w,a->dock_panels[1].h,a->laboratory_switch.x,a->laboratory_switch.y,a->laboratory_switch.w,a->laboratory_switch.h,a->scene_viewport_bounds.x,a->scene_viewport_bounds.y,a->scene_viewport_bounds.w,a->scene_viewport_bounds.h);}
        ok=ok && nk_sdl_accessibility_request_focus(a->ui,NULL,"Kurven anzeigen");break;
    case 10:
        if(!nk_sdl_accessibility_is_focused(a->ui,NULL,"Kurven anzeigen")) {
            if(ps_clock()>=focus_retry){SDL_RaiseWindow(a->window);nk_sdl_accessibility_request_focus(a->ui,NULL,"Kurven anzeigen");focus_retry=ps_clock()+.25;}return;
        }
        test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 11:
        if(a->laboratory_curves)return;
        {bool enabled=false,checked=true;ok=!a->laboratory_curves && nk_sdl_accessibility_toggle_state(a->ui,"Kurven anzeigen",&enabled,&checked) && enabled && !checked;}
        test_window_key_mod(a->window,SDLK_SPACE,0);break;
    case 12:
        if(!a->laboratory_curves)return;
        ok=a->laboratory_curves;dimension=laboratory_dimension_value(a,0);
        ok=ok && nk_sdl_accessibility_request_focus(a->ui,NULL,laboratory_separator_name(0));break;
    case 13:
        if(!nk_sdl_accessibility_is_focused(a->ui,NULL,laboratory_separator_name(0))){if(ps_clock()>=focus_retry){SDL_RaiseWindow(a->window);nk_sdl_accessibility_request_focus(a->ui,NULL,laboratory_separator_name(0));focus_retry=ps_clock()+.25;}return;}
        test_window_key_mod(a->window,SDLK_DOWN,0);break;
    case 14:ok=laboratory_dimension_value(a,0)==dimension-10;test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 15:grip=a->laboratory_splitters[2];test_mouse(a,grip,true);break;
    case 16:test_plot_motion(a,grip.x+40,grip.y+grip.h/2);break;
    case 17:test_mouse(a,nk_rect(grip.x+40,grip.y,1,1),false);break;
    case 18:
        if(a->laboratory_resize)return;
        ok=a->laboratory_curve_ratio>.5f && !a->laboratory_resize;
        dimension=laboratory_dimension_value(a,1);ok=ok && nk_sdl_accessibility_request_focus(a->ui,NULL,laboratory_separator_name(1));break;
    case 19:
        if(!nk_sdl_accessibility_is_focused(a->ui,NULL,laboratory_separator_name(1))){if(ps_clock()>=focus_retry){SDL_RaiseWindow(a->window);nk_sdl_accessibility_request_focus(a->ui,NULL,laboratory_separator_name(1));focus_retry=ps_clock()+.25;}return;}
        test_window_key_mod(a->window,SDLK_UP,0);break;
    case 20:ok=laboratory_dimension_value(a,1)==dimension+10;test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 21:
        ok=display_time(a)==time && a->yaw==yaw && a->scene_selected_id==selected;
        *capture="laboratory-resized.bmp";grip=a->laboratory_splitters[0];dimension=*laboratory_dimension(a,0);test_mouse(a,grip,true);break;
    case 22:test_plot_motion(a,grip.x+grip.w/2,grip.y-20);break;
    case 23:test_window_key_mod(a->window,SDLK_ESCAPE,0);break;
    case 24:ok=*laboratory_dimension(a,0)==dimension && !a->laboratory_resize;test_mouse(a,grip,false);break;
    case 25:
        ok=display_time(a)==time && a->yaw==yaw && a->scene_selected_id==selected;
        ok=ok && nk_sdl_accessibility_request_focus(a->ui,NULL,laboratory_separator_name(2));break;
    case 26:
        if(!nk_sdl_accessibility_is_focused(a->ui,NULL,laboratory_separator_name(2))){if(ps_clock()>=focus_retry){SDL_RaiseWindow(a->window);nk_sdl_accessibility_request_focus(a->ui,NULL,laboratory_separator_name(2));focus_retry=ps_clock()+.25;}return;}
        test_window_key_mod(a->window,SDLK_END,0);break;
    case 27:break;
    case 28:
        ok=a->laboratory_plots[0].w>=179 && a->laboratory_plots[1].w>=179 && display_time(a)==time && a->yaw==yaw;
        *capture="laboratory-curve-limit.bmp";a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"LABORATORY failed stage %d combo %u: %s\n",*stage,combo,a->status);*code=1;a->quitting=true;}
    if(a->quitting)printf("LABORATORY %s SELF-TEST: %s\n",mode,*code?"FAILED":"PASSED");
    (*stage)++;
}
