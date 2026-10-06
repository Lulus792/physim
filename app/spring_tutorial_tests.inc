static bool spring_tutorial_start(app *a,const char *root,const char *mode) {
    bool language=!strcmp(mode,"spring-tutorial-phys");char path[4096];
    join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    a->template_analysis_language=language?1:0;a->example=language?16:4;
    join(a->project_input,sizeof a->project_input,root,"project");new_project(a);if(!a->loaded)return false;
    join(path,sizeof path,a->root,language?"examples/documentation/spring_analysis.phys":"examples/documentation/spring_analysis.c");
    if(!load_editor(&a->analysis,path))return false;
    a->analysis_dirty=true;a->dt=.005;a->project_settings_dirty=true;select_build_profile(a,1);
    SDL_SetWindowSize(a->window,1080,740);return true;
}
static void spring_tutorial_frame(app *a,int *stage,int *code,const char **capture) {
    static unsigned steps;static double held;bool ok=true;
    switch(*stage) {
    case 0:steps=0;held=0;build_project(a);break;
    case 1:
        if(!idle(a) || a->library_thread)return;
        ok=a->built && a->parameters.count==1 && !strcmp(a->parameters.entries[0].name,"damping");
        if(ok)strcpy(a->parameters.selected[0],"8");
        ok &= start_run_mode(a,true);a->camera_target=ps_v3(-.25,.15,0);a->yaw=.4f;a->pitch=.15f;a->zoom=3;break;
    case 2:
        if(!a->hello || !a->history_count)return;
        held=a->simulation_time;steps++;ok=command(a,PS_MSG_STEP);break;
    case 3:
        if(a->simulation_time<=held)return;
        if(steps<40){*stage=2;return;}
        ok=a->paused && a->channel_count==11 && fabs(a->values[0]-.35*(1+4*a->simulation_time)*exp(-4*a->simulation_time))<3e-8 &&
            fabs(a->values[6]-.98)<1e-7 && a->values[5]>0 && a->scene.count==13 && a->scene.point_count==65;
        *capture="spring-critical-scene.bmp";ok &= command(a,PS_MSG_STOP);break;
    case 4:
        if(a->runner.running || !idle(a))return;
        ok=a->data.result==PS_EOF && a->data.total==41 && a->timeline_scenes;if(ok)analyze(a,false);break;
    case 5: {
        if(!idle(a))return;
        uint32_t plots=0,tables=0;ps_table_row row;
        ok=a->analysis_report && ps_report_describe(a->analysis_report,NULL,NULL,&plots,&tables)==PS_OK && plots==3 && tables==1 &&
            ps_report_row_read(a->analysis_report,0,0,&row)==PS_OK && row.values[0]==41 && row.values[1]<1e-7;
        a->tab=2;a->result_view=2;break;
    }
    case 6:*capture="spring-energy-analysis.bmp";a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Spring tutorial stage %d: %.17g %s\n",*stage,a->simulation_time,a->status);*code=1;a->quitting=true;}
    if(a->quitting)printf("SPRING TUTORIAL SELF-TEST: %s\n",*code?"FAILED":"PASSED");
    (*stage)++;
}
