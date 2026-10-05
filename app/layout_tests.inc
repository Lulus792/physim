static bool layout_test_start(app *a,const char *root,const char *mode) {
    char path[4096];join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    join(a->project_input,sizeof a->project_input,root,"project");
    SDL_SetWindowSize(a->window,1080,740);
    if(!strcmp(mode,"layouts-read") || !strcmp(mode,"layouts-corrupt") || !strcmp(mode,"layouts-reset")) {
        open_project(a);return a->loaded && !a->built;
    }
    a->example=!strcmp(mode,"layouts-c")?0:8;new_project(a);
    a->dt=.01;a->project_settings_dirty=true;select_build_profile(a,1);return a->loaded;
}
static void layout_test_text(app *a,const char *text) {
    SDL_Event e={0};e.type=SDL_EVENT_TEXT_INPUT;e.text.windowID=SDL_GetWindowID(a->window);e.text.text=text;SDL_PushEvent(&e);
}
static void layout_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    static ps_layout_state first,second;static uint32_t source;static double started;
    /* Events queued by a stage are consumed and drawn on the following frame. */
    static bool settle;
    if(settle){settle=false;return;}settle=true;
    bool ok=true;ps_dock_layout *d=&a->preferences.dock;
    if(!strcmp(mode,"layouts-read")) {
        if(*stage==0){a->layout_manager=true;(*stage)++;return;}
        ok=a->layouts_writable && a->layouts.count==1 && !strcmp(a->layouts.entries[0].name,"Simulation") &&
            (d->floating&8) && d->floats[3].w==320 && d->floats[3].h==480 && !a->runner.running;
        *capture="layouts-reopened.bmp";a->quitting=true;
    } else if(!strcmp(mode,"layouts-corrupt") || !strcmp(mode,"layouts-reset")) {
        switch(*stage) {
        case 0:ok=!a->layouts_writable && *a->layouts_error && !a->layouts.count;a->layout_manager=true;break;
        case 1:*capture="layouts-corrupt.bmp";test_mouse(a,a->layout_bounds[1],true);break;
        case 2:test_mouse(a,a->layout_bounds[1],false);break;
        case 3:
            ok=!a->layouts_writable && !a->layouts.count;
            if(!strcmp(mode,"layouts-corrupt"))a->quitting=true;
            else test_mouse(a,a->layout_bounds[4],true);
            break;
        case 4:test_mouse(a,a->layout_bounds[4],false);break;
        case 5:ok=a->layouts_writable && !a->layouts.count && !*a->layouts_error;a->quitting=true;break;
        }
    } else switch(*stage) {
    case 0:build_project(a);break;
    case 1:if(!idle(a) || a->library_thread)return;ok=a->built;start_run(a);break;
    case 2:
        if(!a->hello || a->simulation_time<.2)return;
        started=a->simulation_time;
        nk_str_append_text_char(&a->experiment.string,"\n/* layouts keep source */\n",(int)sizeof "\n/* layouts keep source */\n"-1);a->dirty=true;
        source=ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
        a->show_log=true;first=layout_capture(a);test_mouse(a,a->toolbar_bounds[1],true);break;
    case 3:test_mouse(a,a->toolbar_bounds[1],false);break;
    case 4:test_mouse(a,a->dock_menu_bounds[4],true);break;
    case 5:test_mouse(a,a->dock_menu_bounds[4],false);break;
    case 6:
        if(!a->layout_bounds[0].w)return;
        ok=a->layout_manager;test_mouse(a,a->layout_bounds[0],true);break;
    case 7:test_mouse(a,a->layout_bounds[0],false);break;
    case 8:layout_test_text(a,"Programmieren ä");break;
    case 9:ok=!strcmp(a->layout_name,"Programmieren ä");test_mouse(a,a->layout_bounds[1],true);break;
    case 10:test_mouse(a,a->layout_bounds[1],false);break;
    case 11:
        ok=a->layouts.count==1 && !memcmp(&first,&a->layouts.entries[0].state,sizeof first);
        *capture="layouts-saved.bmp";test_mouse(a,a->layout_bounds[5],true);break;
    case 12:test_mouse(a,a->layout_bounds[5],false);break;
    case 13: {
        ok=!a->layout_manager;
        ps_preferences p=a->preferences;p.theme=PS_THEME_LIGHT;p.editor_size=18;p.inspector_width=480;p.sidebar_width=360;p.log_height=300;
        preferences_apply(a,&p);
        ok &= ps_dock_move(d,0,1,PS_DOCK_TAB) && ps_dock_select(d,1) && ps_dock_float_panel(d,3,(ps_dock_float){500,120,320,480}) && ps_dock_hide(d,2);
        a->show_log=false;second=layout_capture(a);a->layout_manager=true;break;
    }
    case 14:test_mouse(a,a->layout_bounds[0],true);break;
    case 15:test_mouse(a,a->layout_bounds[0],false);break;
    case 16:test_key(a,SDLK_A);break;
    case 17:layout_test_text(a,"Simulation");break;
    case 18:ok=!strcmp(a->layout_name,"Simulation");test_mouse(a,a->layout_bounds[1],true);break;
    case 19:test_mouse(a,a->layout_bounds[1],false);break;
    case 20:
        ok=a->layouts.count==2 && !memcmp(&second,&a->layouts.entries[1].state,sizeof second);
        test_mouse(a,a->layout_entries[0],true);break;
    case 21:test_mouse(a,a->layout_entries[0],false);break;
    case 22:test_mouse(a,a->layout_bounds[2],true);break;
    case 23:test_mouse(a,a->layout_bounds[2],false);break;
    case 24: {
        ps_layout_state current=layout_capture(a);ok=!memcmp(&current,&first,sizeof first) && a->preferences.theme==PS_THEME_LIGHT && a->preferences.editor_size==18;
        *capture="layouts-applied.bmp";
        /* These keys normally switch tabs, stop/reset or move the camera. */
        test_key(a,SDLK_3);test_window_key_mod(a->window,SDLK_F7,0);test_window_key_mod(a->window,SDLK_RIGHT,SDL_KMOD_ALT);
        break;
    }
    case 25:
        ok=a->tab==1 && a->runner.running && !a->reset_pending && !a->paused;
        a->preferences.inspector_width=336;first=layout_capture(a);
        test_mouse(a,a->layout_bounds[1],true);break;
    case 26:test_mouse(a,a->layout_bounds[1],false);break;
    case 27:
        ok=a->layouts.count==2 && a->layouts.entries[0].state.inspector_width==336;
        test_mouse(a,a->layout_entries[1],true);break;
    case 28:test_mouse(a,a->layout_entries[1],false);break;
    case 29:test_mouse(a,a->layout_bounds[2],true);break;
    case 30:test_mouse(a,a->layout_bounds[2],false);break;
    case 31: {
        ps_layout_state current=layout_capture(a);ok=!memcmp(&current,&second,sizeof second);
        test_mouse(a,a->layout_entries[0],true);break;
    }
    case 32:test_mouse(a,a->layout_entries[0],false);break;
    case 33:test_mouse(a,a->layout_bounds[3],true);break;
    case 34:test_mouse(a,a->layout_bounds[3],false);break;
    case 35:
        ok=a->layouts.count==1 && !strcmp(a->layouts.entries[0].name,"Simulation");
        /* The header must remain usable when every panel is hidden. */
        for(unsigned p=0;p<PS_DOCK_PANELS;p++)ok &= ps_dock_hide(d,p);
        a->show_log=false;test_window_key_mod(a->window,SDLK_ESCAPE,0);break;
    case 36:break;
    case 37:ok=!a->layout_manager && d->hidden==PS_DOCK_ALL;test_mouse(a,a->toolbar_bounds[1],true);break;
    case 38:test_mouse(a,a->toolbar_bounds[1],false);break;
    case 39:test_mouse(a,a->dock_menu_bounds[4],true);break;
    case 40:test_mouse(a,a->dock_menu_bounds[4],false);break;
    case 41:ok=a->layout_manager;*capture="layouts-empty.bmp";test_mouse(a,a->layout_entries[0],true);break;
    case 42:test_mouse(a,a->layout_entries[0],false);break;
    case 43:test_mouse(a,a->layout_bounds[2],true);break;
    case 44:test_mouse(a,a->layout_bounds[2],false);break;
    case 45:test_mouse(a,a->layout_bounds[5],true);break;
    case 46:test_mouse(a,a->layout_bounds[5],false);break;
    case 47: {
        ps_layout_state current=layout_capture(a);ok=!a->layout_manager && !memcmp(&current,&second,sizeof second) && a->preferences.theme==PS_THEME_LIGHT && a->preferences.editor_size==18 &&
            a->dirty && a->runner.running && !a->paused && a->simulation_time>started &&
            source==ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
        *capture="layouts-restored.bmp";command(a,PS_MSG_STOP);a->stop_at=ps_clock();break;
    }
    case 48:
        if(!idle(a) || a->library_thread)return;
        ok=a->data.result==PS_EOF && a->data.total>2;
        open_settings(a);a->settings_draft.theme=PS_THEME_HIGH_CONTRAST;a->settings_draft.editor_size=22;
        a->preferences.sidebar_width=208;a->preferences.inspector_width=208;a->preferences.log_height=150;
        a->layout_manager=true;break;
    case 49:test_mouse(a,a->layout_bounds[2],true);break;
    case 50:test_mouse(a,a->layout_bounds[2],false);break;
    case 51: {
        ps_layout_state current=layout_capture(a);
        ok=a->tab==6 && !memcmp(&current,&second,sizeof second) &&
            a->settings_draft.sidebar_width==second.sidebar_width && a->settings_draft.inspector_width==second.inspector_width &&
            a->settings_draft.log_height==second.log_height && !memcmp(&a->settings_draft.dock,&second.dock,sizeof second.dock) &&
            a->settings_draft.theme==PS_THEME_HIGH_CONTRAST && a->settings_draft.editor_size==22 &&
            a->preferences.theme==PS_THEME_LIGHT && a->preferences.editor_size==18;
        a->quitting=true;break;
    }
    }
    if(!ok){fprintf(stderr,"Layouts failed at stage %d: manager=%d count=%u selected=%d name=%s error=%s\n",*stage,a->layout_manager,a->layouts.count,a->layout_selected,a->layout_name,a->layouts_error);*capture="layouts-failed.bmp";*code=1;a->quitting=true;}
    if(a->quitting)printf("LAYOUTS %s SELF-TEST: %s\n",mode,*code?"FAILED":"PASSED");
    (*stage)++;
}
