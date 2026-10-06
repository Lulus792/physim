static bool monte_carlo_tutorial_start(app *a,const char *root,const char *mode) {
    bool language=!strcmp(mode,"monte-carlo-tutorial-phys");char path[4096];
    join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    a->template_analysis_language=language?1:0;a->example=language?10:5;
    join(a->project_input,sizeof a->project_input,root,"project");new_project(a);if(!a->loaded)return false;
    join(path,sizeof path,a->root,language?"examples/documentation/monte_carlo_main.phys":"examples/documentation/monte_carlo_main.c");
    if(!load_editor(&a->experiment,path))return false;
    a->dirty=true;
    join(path,sizeof path,a->root,language?"examples/documentation/monte_carlo_analysis.phys":"examples/documentation/monte_carlo_analysis.c");
    if(!load_editor(&a->analysis,path))return false;
    a->analysis_dirty=true;a->dt=.03125;a->project_settings_dirty=true;select_build_profile(a,1);
    a->batch_runs=256;a->batch_workers=4;a->batch_steps=32;a->batch_dt=.03125;
    a->batch_target=true;a->batch_end_time=1;a->batch_adaptive=false;a->batch_sweep=false;
    strcpy(a->batch_seed,"42");strcpy(a->batch_channel,"position.x");
    SDL_SetWindowSize(a->window,1440,960);a->preferences.sidebar_width=260;
    ps_dock_hide(&a->preferences.dock,PS_DOCK_LOG);ps_dock_hide(&a->preferences.dock,PS_DOCK_INSPECTOR);
    return true;
}
static void monte_carlo_tutorial_frame(app *a,int *stage,int *code,const char **capture) {
    static unsigned steps;static double held;bool ok=true;
    switch(*stage) {
    case 0:steps=0;held=0;ok=!batch_analysis_ready(a);analyze_first_batch_run(a);ok &= !a->job_kind;build_project(a);break;
    case 1:
        if(!idle(a) || a->library_thread)return;
        ok=a->built && a->parameters.count==4 && start_run_mode(a,true);
        a->camera_target=ps_v3(-.5,.8,0);a->yaw=0;a->pitch=0;a->zoom=5;break;
    case 2:
        if(!a->hello || !a->history_count)return;
        held=a->simulation_time;steps++;ok=command(a,PS_MSG_STEP);break;
    case 3:
        if(a->simulation_time<=held)return;
        if(steps<32){*stage=2;return;}
        ok=a->paused && a->channel_count==9 && a->simulation_time==1 &&
            fabs(a->values[6]-.15)<1e-12 && fabs(a->values[7]-.25)<1e-12 &&
            a->scene.count==11 && a->scene.point_count==64;
        *capture="monte-carlo-single-scene.bmp";ok &= command(a,PS_MSG_STOP);break;
    case 4:
        if(a->runner.running || !idle(a) || a->library_thread)return;
        ok=a->data.result==PS_EOF && a->data.total==33 && a->timeline_scenes;
        if(ok)start_batch(a);break;
    case 5:
        if(!idle(a) || a->library_thread)return;
        ok=a->batch_code==PS_OK && !a->batch_result.cancelled && a->batch_result.completed==256 &&
            a->batch_result.valid==256 && a->batch_result.peak_active==4 && batch_analysis_ready(a);
        a->tab=5;break;
    case 6:if(!series_test_click(a,a->batch_analyze_bounds))return;break;
    case 7:test_mouse(a,a->batch_analyze_bounds,false);break;
    case 8: {
        if(!idle(a) || a->library_thread)return;
        char path[4096];join(path,sizeof path,a->batch_options.directory,"run-0001.psrun");
        uint32_t plots=0,tables=0;ps_table_row x,y,ci;
        ok=!strcmp(a->last_run,path) && a->data.total==33 && a->analysis_report &&
            ps_report_describe(a->analysis_report,NULL,NULL,&plots,&tables)==PS_OK && plots==4 && tables==2 &&
            ps_report_row_read(a->analysis_report,0,0,&x)==PS_OK && ps_report_row_read(a->analysis_report,0,1,&y)==PS_OK &&
            ps_report_row_read(a->analysis_report,1,0,&ci)==PS_OK &&
            fabs(x.values[5]-1)<1e-12 && fabs(x.values[6]-.15)<1e-12 && fabs(x.values[1]-.15)<.025 &&
            fabs(y.values[5]-(5-4.903325))<1e-12 && fabs(y.values[6]-.25)<1e-12 && fabs(y.values[1]-.25)<.04 &&
            fabs((ci.values[0]+ci.values[1])/2-x.values[0])<1e-12;
        a->tab=2;a->analysis_tab=0;a->result_view=1;break;
    }
    case 9:*capture="monte-carlo-histogram.bmp";break;
    case 10:a->result_view=4;break;
    case 11:*capture="monte-carlo-statistics.bmp";break;
    case 12:a->result_view=5;break;
    case 13:*capture="monte-carlo-mean-interval.bmp";a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Monte Carlo tutorial stage %d: %.17g %s\n",*stage,a->simulation_time,a->status);*code=1;a->quitting=true;}
    if(a->quitting)printf("MONTE CARLO TUTORIAL SELF-TEST: %s\n",*code?"FAILED":"PASSED");
    (*stage)++;
}
