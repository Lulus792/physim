static bool isolation_test_angle(app *a,const char *text) {
    for(uint32_t i=0;i<a->parameters.count;i++)if(!strcmp(a->parameters.entries[i].name,"initialAngle")) {
        double value;if(ps_parameter_catalog_input(&a->parameters,i,text,&value)!=PS_OK)return false;
        snprintf(a->parameters.selected[i],sizeof a->parameters.selected[i],"%s",text);return true;
    }
    return false;
}
/* User-code failures are executed only in real separately started runners. */
static bool isolation_test_start(app *a,const char *root,const char *mode) {
    bool language=strstr(mode,"-phys-")!=NULL,create=strstr(mode,"-create-")!=NULL;
    bool hang=strstr(mode,"-hang")!=NULL;
    join(a->project_input,sizeof a->project_input,root,"project");a->example=language?8:0;new_project(a);
    if(!a->loaded)return false;
    const char *source=nk_str_get_const(&a->experiment.string),*signature,*probe;
    char injection[512];
    if(language) {
        signature=create?"func create():":"func step(dt: Float64):";
        probe=hang?"        while true:\n            var waiting: Int64 = 1\n":"        assert(false, \"Injected isolated runtime failure\")\n";
        snprintf(injection,sizeof injection,"\n    if initialAngle > 1%s:\n%s",create?"":" && simulationTime() >= 0.025",probe);
    } else {
        signature=create?"    if(parameter!=PS_OK) return parameter;":"static ps_result step(ps_context *c, double dt) {";
        snprintf(injection,sizeof injection,"\n    if (((pendulum *)c->user)->initial_angle > 1%s) { %s }\n",
                 create?"":" && c->time_s >= .025",hang?"for (;;) {}":"abort();");
    }
    const char *at=strstr(source,signature);
    if(!at || !nk_str_insert_text_char(&a->experiment.string,(int)(at-source+strlen(signature)),injection,(int)strlen(injection)))return false;
    a->dirty=true;invalidate_build(a);a->dt=.005;a->simulation_speed=1;a->runner_wall_seconds=0;
    a->project_settings_dirty=true;a->tab=1;select_build_profile(a,1);return true;
}
static void isolation_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    static char failed[4096];static double started;static uint32_t digest;static bool saved;
    bool create=strstr(mode,"-create-")!=NULL,hang=strstr(mode,"-hang")!=NULL,ok=true;
    switch(*stage) {
    case 0:build_project(a);break;
    case 1:if(!idle(a) || a->library_thread)return;
        ok=a->built && isolation_test_angle(a,"1.5");
        a->project_settings_dirty=true;test_mouse(a,a->run_control_bounds[0],true);break;
    case 2:test_mouse(a,a->run_control_bounds[0],false);break;
    case 3:
        if(!a->runner.running && !a->last_run[0])return;
        snprintf(failed,sizeof failed,"%s",a->last_run);started=ps_clock();break;
    case 4:
        if(hang) {
            if(ps_clock()-started<.5)return;
            ok=a->runner.running && (create?!a->hello:a->hello);
            *capture="isolation-hung.bmp";test_mouse(a,a->run_control_bounds[4],true);
        }
        break;
    case 5:if(hang)test_mouse(a,a->run_control_bounds[4],false);break;
    case 6: {
        if(!idle(a) || a->library_thread)return;
        ok=!a->hello && a->paused && (hang || a->runner.exit_code!=0);
        if(create)ok &= !exists(failed);
        else {
            ps_run_reader reader;ok &= ps_run_open(&reader,failed)==PS_OK;
            if(ok) {
                double t=0,v[PS_MAX_CHANNELS],previous=-1;unsigned count=0;ps_result result;
                while((result=ps_run_next(&reader,&t,v))==PS_OK) {ok &= t>previous;previous=t;count++;}
                ok &= count>=2 && count<=6 && (result==PS_EOF || result==PS_RECOVERED);
                ps_run_reader_close(&reader);
            }
        }
        saved=exists(failed);if(saved)ok &= reset_test_crc(failed,&digest);
        *capture="isolation-recovered.bmp";
        test_window_key_mod(a->window,SDLK_F1,0);break;
    }
    case 7:ok=a->doc_visible;test_window_key_mod(a->doc_window,SDLK_ESCAPE,0);break;
    case 8:ok=!a->doc_visible && isolation_test_angle(a,"0.45");
        a->project_settings_dirty=true;
        snprintf(a->step_breakpoints,sizeof a->step_breakpoints,"7");
        test_mouse(a,a->run_control_bounds[0],true);break;
    case 9:test_mouse(a,a->run_control_bounds[0],false);break;
    case 10:if(!a->hello || !a->paused || a->simulation_time==0)return;
        ok=strcmp(failed,a->last_run) && a->simulation_time==7*.005;
        *capture="isolation-restarted.bmp";test_mouse(a,a->run_control_bounds[4],true);break;
    case 11:test_mouse(a,a->run_control_bounds[4],false);break;
    case 12: {
        if(!idle(a) || a->library_thread)return;
        ps_run_reader reader;ok=ps_run_open(&reader,a->last_run)==PS_OK;
        if(ok) {
            double t,v[PS_MAX_CHANNELS];unsigned count=0;ps_result result;
            while((result=ps_run_next(&reader,&t,v))==PS_OK)count++;
            ok &= result==PS_EOF && count==8;ps_run_reader_close(&reader);
        }
        if(saved){uint32_t current;ok &= reset_test_crc(failed,&current) && current==digest;}
        else ok &= !exists(failed);
        if(ok)puts("RUNNER ISOLATION UI SELF-TEST: PASSED");a->quitting=true;break;
    }
    default:ok=false;break;
    }
    if(!ok){fprintf(stderr,"Isolation %s failed at %d: %s\n",mode,*stage,a->status);*code=1;a->quitting=true;}
    (*stage)++;
}
