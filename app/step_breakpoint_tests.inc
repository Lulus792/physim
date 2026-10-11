static bool breakpoint_test_start(app *a,const char *root,const char *mode) {
    join(a->project_input,sizeof a->project_input,root,"project");
    if (!strcmp(mode,"breakpoints-read")) {
        open_project(a);return a->loaded && !strcmp(a->step_breakpoints,"3,7") && !a->project_settings_dirty;
    }
    a->example=!strcmp(mode,"breakpoints-c")?0:8;new_project(a);
    if (!a->loaded || a->step_breakpoints[0])return false;
    a->preferences.sidebar_width=360;ps_dock_hide(&a->preferences.dock,PS_DOCK_LOG);
    a->view_disclosure=NK_MINIMIZED;a->steps_disclosure=NK_MAXIMIZED;
    a->breakpoint_disclosure=NK_MAXIMIZED;a->tab=1;select_build_profile(a,1);
    return true;
}
static bool breakpoint_test_rows(app *a,unsigned expected) {
    ps_run_reader reader;ps_result opened=ps_run_open(&reader,a->last_run);
    if(opened!=PS_OK){fprintf(stderr,"Breakpoint rows: open %d %s\n",opened,a->last_run);return false;}
    double time=0,values[PS_MAX_CHANNELS];unsigned count=0;ps_result result;
    while((result=ps_run_next(&reader,&time,values))==PS_OK)count++;
    bool ok=(result==PS_EOF || (result==PS_RECOVERED && feof(reader.file) && !ferror(reader.file))) && count==expected && time==a->simulation_time &&
            strstr(reader.metadata,"\nbreak_at_steps=3,7\n");
    if(!ok)fprintf(stderr,"Breakpoint rows: result=%d count=%u expected=%u time=%.17g view=%.17g metadata=%d\n",result,count,expected,time,a->simulation_time,strstr(reader.metadata,"\nbreak_at_steps=3,7\n")!=NULL);
    ps_run_reader_close(&reader);return ok;
}
static void breakpoint_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    static double held,wall;static char original[4096];static unsigned focus_attempts;bool ok=true;
    if(!strcmp(mode,"breakpoints-read")){puts("STEP BREAKPOINT UI SELF-TEST: PASSED");a->quitting=true;return;}
    switch(*stage) {
    case 0:if(a->breakpoint_bounds.w<=0 || !hierarchy_test_click(a,a->breakpoint_bounds))return;break;
    case 1:test_mouse(a,a->breakpoint_bounds,false);break;
    case 2: {
        if(!a->breakpoint_input_active) {
            if(++focus_attempts>16) {
                fprintf(stderr,"Breakpoint edit never focused: rect %.3f/%.3f/%.3f/%.3f\n",
                        a->breakpoint_bounds.x,a->breakpoint_bounds.y,a->breakpoint_bounds.w,a->breakpoint_bounds.h);
                *code=1;a->quitting=true;
            } else *stage=0;
            return;
        }
        SDL_Event e={0};e.type=SDL_EVENT_TEXT_INPUT;e.text.windowID=SDL_GetWindowID(a->window);e.text.text="3,7";
        ok=SDL_PushEvent(&e);break;
    }
    case 3:break;
    case 4:ok=!strcmp(a->step_breakpoints,"3,7") && a->project_settings_dirty;
        if(!ok)fprintf(stderr,"Breakpoint edit text=%s dirty=%d active=%d\n",a->step_breakpoints,a->project_settings_dirty,a->breakpoint_input_active);
        *capture="breakpoints-configured.bmp";save_project(a);ok &= !a->project_settings_dirty;build_project(a);break;
    case 5:if(!idle(a) || a->library_thread)return;ok=a->built;
        test_mouse(a,a->run_control_bounds[0],true);break;
    case 6:test_mouse(a,a->run_control_bounds[0],false);break;
    case 7:if(!a->hello || !a->paused || a->simulation_time==0)return;
        ok=breakpoint_test_rows(a,4);held=a->simulation_time;wall=ps_clock();
        snprintf(original,sizeof original,"%s",a->last_run);break;
    case 8:if(ps_clock()-wall<.1)return;
        ok=a->paused && a->simulation_time==held && breakpoint_test_rows(a,4);
        *capture="breakpoint-first.bmp";test_mouse(a,a->run_control_bounds[2],true);break;
    case 9:test_mouse(a,a->run_control_bounds[2],false);break;
    case 10:if(a->simulation_time==held)return;ok=a->paused && breakpoint_test_rows(a,5);
        test_mouse(a,a->run_control_bounds[1],true);break;
    case 11:test_mouse(a,a->run_control_bounds[1],false);break;
    case 12:if(!a->paused || a->simulation_time<=held+.005)return;
        ok=breakpoint_test_rows(a,8);*capture="breakpoint-second.bmp";
        test_mouse(a,a->run_control_bounds[3],true);break;
    case 13:test_mouse(a,a->run_control_bounds[3],false);break;
    case 14:if(a->reset_pending || a->reset_starting || !a->hello || !strcmp(original,a->last_run))return;
        ok=a->paused && a->simulation_time==0 && !strcmp(a->step_breakpoints,"3,7");
        test_mouse(a,a->run_control_bounds[1],true);break;
    case 15:test_mouse(a,a->run_control_bounds[1],false);break;
    case 16:if(!a->paused || a->simulation_time==0)return;
        ok=breakpoint_test_rows(a,4);*capture="breakpoint-reset.bmp";
        test_mouse(a,a->run_control_bounds[4],true);break;
    case 17:test_mouse(a,a->run_control_bounds[4],false);break;
    case 18:if(a->runner.running || a->library_thread || a->data.thread)return;
        ok=!a->project_settings_dirty;puts("STEP BREAKPOINT UI SELF-TEST: PASSED");a->quitting=true;break;
    default:ok=false;break;
    }
    if(!ok){fprintf(stderr,"Step breakpoint UI failed at %d: %s\n",*stage,a->status);*code=1;a->quitting=true;}
    (*stage)++;
}
