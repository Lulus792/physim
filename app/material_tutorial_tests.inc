static bool material_test_start(app *a,const char *root,const char *mode) {
    char path[4096];join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    bool language=!strcmp(mode,"material-phys");a->template_analysis_language=language?1:0;
    join(a->project_input,sizeof a->project_input,root,"project");a->example=language?10:0;new_project(a);if(!a->loaded)return false;
    join(path,sizeof path,a->root,language?"examples/documentation/material_main.phys":"examples/documentation/material_main.c");if(!load_editor(&a->experiment,path))return false;
    join(path,sizeof path,a->root,language?"examples/documentation/material_analysis.phys":"examples/documentation/material_analysis.c");if(!load_editor(&a->analysis,path))return false;
    a->dirty=true;a->analysis_dirty=true;a->dt=.001;a->project_settings_dirty=true;select_build_profile(a,1);
    SDL_SetWindowSize(a->window,1080,740);return true;
}
static void material_test_frame(app *a,int *stage,int *code,const char **capture) {
    static unsigned steps;static double held;bool ok=true;
    switch(*stage) {
    case 0:steps=0;held=0;build_project(a);break;
    case 1:
        if(!idle(a) || a->library_thread)return;
        ok=a->built && a->parameters.count==4 && start_run_mode(a,true);
        a->camera_target=ps_v3(0,.35,0);a->yaw=.3f;a->pitch=.1f;a->zoom=1.6;break;
    case 2:
        if(!a->hello || !a->history_count)return;
        held=a->simulation_time;steps++;ok=command(a,PS_MSG_STEP);break;
    case 3:
        if(a->simulation_time<=held)return;
        if(steps<40){*stage=2;return;}
        ok=a->paused && a->channel_count==13 && a->values[0]>.49 && a->values[0]<.5 && a->values[1]<0 &&
            a->values[6]>0 && a->values[6]<.1 && fabs(a->values[1]-a->values[12])<1e-8 && a->scene.count==5;
        *capture="material-scene.bmp";ok &= command(a,PS_MSG_STOP);break;
    case 4:
        if(a->runner.running || !idle(a))return;
        ok=a->data.result==PS_EOF && a->data.total==41 && a->timeline_scenes;
        if(ok)analyze(a,false);break;
    case 5:
        if(!idle(a))return;
        uint32_t plots=0,tables=0;ok=a->analysis_report && ps_report_describe(a->analysis_report,NULL,NULL,&plots,&tables)==PS_OK && plots==3 && tables==0;
        a->tab=2;break;
    case 6:*capture="material-analysis.bmp";a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Material tutorial stage %d: %.17g %s\n",*stage,a->simulation_time,a->status);*code=1;a->quitting=true;}
    if(a->quitting)printf("MATERIAL TUTORIAL SELF-TEST: %s\n",*code?"FAILED":"PASSED");
    (*stage)++;
}
