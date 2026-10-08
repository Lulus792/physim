static bool pendulum_tutorial_start(app *a,const char *root,const char *mode) {
    bool language=!strcmp(mode,"pendulum-tutorial-phys");char path[4096];
    join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    a->template_analysis_language=language?1:0;a->example=language?8:0;
    join(a->project_input,sizeof a->project_input,root,"project");new_project(a);if(!a->loaded)return false;
    join(path,sizeof path,a->root,language?"examples/documentation/pendulum_main.phys":"examples/documentation/pendulum_main.c");
    if(!load_editor(&a->experiment,path))return false;
    a->dirty=true;
    join(path,sizeof path,a->root,language?"examples/documentation/pendulum_analysis.phys":"examples/documentation/pendulum_analysis.c");
    if(!load_editor(&a->analysis,path))return false;
    a->analysis_dirty=true;
    a->dt=.005;a->project_settings_dirty=true;select_build_profile(a,1);SDL_SetWindowSize(a->window,1080,740);return true;
}
static void pendulum_tutorial_frame(app *a,int *stage,int *code,const char **capture) {
    static unsigned steps;static double held,initial;bool ok=true;
    switch(*stage) {
    case 0:steps=0;held=initial=0;build_project(a);break;
    case 1: {
        if(!idle(a) || a->library_thread)return;
        ok=a->built && a->parameters.count==3;bool found=false;
        for(unsigned i=0;i<a->parameters.count;i++)if(!strcmp(a->parameters.entries[i].name,"integrator")){strcpy(a->parameters.selected[i],"3");found=true;}
        ok &= found && start_run_mode(a,true);a->camera_target=ps_v3(0,-.75,0);a->zoom=3;break;
    }
    case 2:
        if(!a->hello || !a->history_count)return;
        if(!steps)initial=a->values[4];
        held=a->simulation_time;steps++;ok=command(a,PS_MSG_STEP);break;
    case 3:
        if(a->simulation_time<=held)return;
        if(steps<40){*stage=2;return;}
        ok=a->paused && a->channel_count==6 && a->values[0]<.45 && a->values[1]<0 &&
            fabs(a->values[4]-initial)<1e-4 && a->scene.count==12;
        *capture="pendulum-verlet-scene.bmp";ok &= command(a,PS_MSG_STOP);break;
    case 4:
        if(a->runner.running || !idle(a))return;
        ok=a->data.result==PS_EOF && a->data.total==41 && a->timeline_scenes;if(ok)analyze(a,false);break;
    case 5: {
        if(!idle(a))return;
        uint32_t plots=0,tables=0;ps_table_row row;ps_table_info periods;
        ok=a->analysis_report && ps_report_describe(a->analysis_report,NULL,NULL,&plots,&tables)==PS_OK && plots==2 && tables==3 &&
            ps_report_row_read(a->analysis_report,0,0,&row)==PS_OK && row.values[0]==41 && row.values[1]<1e-4 && row.values[3]==0 &&
            ps_report_table_read(a->analysis_report,1,&periods)==PS_OK && periods.rows==0;
        a->tab=2;a->result_view=1;break;
    }
    case 6:*capture="pendulum-energy-analysis.bmp";a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Pendulum tutorial stage %d: %.17g %s\n",*stage,a->simulation_time,a->status);*code=1;a->quitting=true;}
    if(a->quitting)printf("PENDULUM TUTORIAL SELF-TEST: %s\n",*code?"FAILED":"PASSED");
    (*stage)++;
}
