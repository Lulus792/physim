static bool adaptive_test_start(app *a,const char *root,const char *mode) {
    if(!strcmp(mode,"adaptive-replay")) {
        if(!timeline_test_start(a,root,mode)) return false;
        return a->adaptive_steps && a->dt==.1 && a->minimum_dt==1e-6 && a->maximum_dt==.2;
    }
    join(a->project_input,sizeof a->project_input,root,"project");
    a->example=!strcmp(mode,"adaptive-c")?0:8;new_project(a);
    if(!a->loaded || a->adaptive_steps)return false;
    SDL_SetWindowSize(a->window,1080,740);a->preferences.sidebar_width=360;
    ps_dock_hide(&a->preferences.dock,PS_DOCK_LOG);a->steps_disclosure=NK_MAXIMIZED;
    a->dt=.1;a->minimum_dt=1e-8;a->maximum_dt=.2;a->project_settings_dirty=true;
    select_build_profile(a,1);a->tab=1;return true;
}
static void adaptive_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    if(!strcmp(mode,"adaptive-replay")) {
        timeline_test_frame(a,mode,stage,code,capture);return;
    }
    static char original[4096];static double held,first,first_values[PS_MAX_CHANNELS];
    static int reported=-1;
    if(reported!=*stage) {
        fprintf(stderr,"Adaptive %s stage %d: t=%.6g paused=%d %s\n",mode,*stage,a->simulation_time,a->paused,a->status);reported=*stage;
    }
    bool ok=true;
    switch(*stage) {
    case 0: if(!a->adaptive_bounds.w || !hierarchy_test_click(a,a->adaptive_bounds))return;break;
    case 1: test_mouse(a,a->adaptive_bounds,false);break;
    case 2:
        ok=a->adaptive_steps && a->project_settings_dirty;
        if(!hierarchy_test_click(a,a->adaptive_limit_bounds[0]))return;break;
    case 3: test_mouse(a,a->adaptive_limit_bounds[0],false);break;
    case 4: test_key(a,SDLK_A);break;
    case 5: {
        SDL_Event e={0};e.type=SDL_EVENT_TEXT_INPUT;e.text.windowID=SDL_GetWindowID(a->window);e.text.text="1e-";
        ok=SDL_PushEvent(&e);break;
    }
    case 6: break;
    case 7:
        ok=isnan(a->minimum_dt) && !strcmp(a->adaptive_limit_text[0],"1e-");
        start_run(a);ok &= !a->runner.running && !a->last_run[0] && strstr(a->status,"Adaptive Schritte");
        test_key(a,SDLK_A);break;
    case 8: {
        SDL_Event e={0};e.type=SDL_EVENT_TEXT_INPUT;e.text.windowID=SDL_GetWindowID(a->window);e.text.text="1e-6";
        ok=SDL_PushEvent(&e);break;
    }
    case 9: break;
    case 10:
        ok=a->minimum_dt==1e-6 && !strcmp(a->adaptive_limit_text[0],"1e-6");
        *capture="adaptive-configured.bmp";break;
    case 11: ok=a->adaptive_steps && a->project_settings_dirty;build_project(a);break;
    case 12:
        if(!idle(a) || a->library_thread)return;
        ok=a->built;a->minimum_dt=.2;start_run(a);
        ok &= !a->runner.running && !a->last_run[0] && strstr(a->status,"Adaptive Schritte");
        a->minimum_dt=1e-6;test_mouse(a,a->run_control_bounds[0],true);break;
    case 13: test_mouse(a,a->run_control_bounds[0],false);break;
    case 14:
        if(!a->hello || a->simulation_time<.2)return;
        snprintf(original,sizeof original,"%s",a->last_run);
        test_mouse(a,a->run_control_bounds[1],true);break;
    case 15: test_mouse(a,a->run_control_bounds[1],false);break;
    case 16: {
        if(!a->paused)return;held=a->simulation_time;
        ps_run_reader r;ok=ps_run_open(&r,original)==PS_OK;
        if(ok) {
            double t,v[PS_MAX_CHANNELS];
            ok=ps_run_next(&r,&t,v)==PS_OK && t==0 && ps_run_next(&r,&first,first_values)==PS_OK;
            ps_run_reader_close(&r);
        }
        ok &= first>0 && first<.1;*capture="adaptive-paused.bmp";
        test_mouse(a,a->run_control_bounds[2],true);break;
    }
    case 17: test_mouse(a,a->run_control_bounds[2],false);break;
    case 18:
        if(a->simulation_time==held)return;
        ok=a->paused && a->simulation_time>held && a->simulation_time<=held+.2;
        test_mouse(a,a->run_control_bounds[3],true);break;
    case 19: test_mouse(a,a->run_control_bounds[3],false);break;
    case 20:
        if(a->reset_pending || a->reset_starting || !a->hello || !strcmp(a->last_run,original))return;
        ok=a->paused && a->simulation_time==0 && a->adaptive_steps && a->dt==.1 &&
           a->minimum_dt==1e-6 && a->maximum_dt==.2;
        *capture="adaptive-reset.bmp";test_mouse(a,a->run_control_bounds[2],true);break;
    case 21: test_mouse(a,a->run_control_bounds[2],false);break;
    case 22:
        if(a->simulation_time==0)return;
        ok=a->paused && a->simulation_time==first && reset_test_values(a->values,first_values,a->channel_count);
        test_mouse(a,a->run_control_bounds[1],true);break;
    case 23: test_mouse(a,a->run_control_bounds[1],false);break;
    case 24:
        if(a->simulation_time<.35)return;
        test_mouse(a,a->run_control_bounds[4],true);break;
    case 25: test_mouse(a,a->run_control_bounds[4],false);break;
    case 26: {
        if(!idle(a) || a->library_thread)return;
        ps_run_reader r;ok=ps_run_open(&r,a->last_run)==PS_OK;
        if(ok) {
            ok=strstr(r.metadata,"step_mode=adaptive\n") && strstr(r.metadata,"maximum_dt_s=0.20000000000000001\n");
            double t,v[PS_MAX_CHANNELS],previous=0,interval=0;unsigned count=0;bool varied=false;ps_result result;
            while((result=ps_run_next(&r,&t,v))==PS_OK) {
                if(count) {
                    ok &= t>previous && t-previous>=1e-6 && t-previous<=.2;
                    if(count==1)interval=t;else varied |= fabs(t-previous-interval)>1e-6;
                }
                previous=t;count++;
            }
            ok &= result==PS_EOF && count>5 && varied;
            ps_run_reader_close(&r);
        }
        *capture="adaptive-recorded.bmp";a->quitting=true;break;
    }
    }
    if(!ok) {*code=1;a->quitting=true;fprintf(stderr,"Adaptive failure in stage %d: minimum %.17g input [%s]\n",*stage,a->minimum_dt,a->adaptive_limit_text[0]);}
    if(a->quitting)printf("ADAPTIVE %s SELF-TEST: %s\n",mode,*code?"FAILED":"PASSED");
    (*stage)++;
}
