static bool saved_run_tutorial_start(app *a,const char *root,const char *mode) {
    snprintf(a->manager_parent,sizeof a->manager_parent,"%s",root);
    bool language=!strcmp(mode,"saved-run-tutorial-c");char path[4096];
    join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    a->example=language?8:0;join(a->project_input,sizeof a->project_input,root,"Producer");
    new_project(a);if(!a->loaded)return false;
    join(path,sizeof path,a->root,language?"examples/documentation/language_main.phys":"examples/documentation/main.c");
    if(!load_editor(&a->experiment,path))return false;
    a->dirty=true;a->dt=.0625;a->project_settings_dirty=true;select_build_profile(a,1);
    SDL_SetWindowSize(a->window,1280,900);a->preferences.sidebar_width=260;
    ps_dock_hide(&a->preferences.dock,PS_DOCK_LOG);ps_dock_hide(&a->preferences.dock,PS_DOCK_INSPECTOR);
    return true;
}
static void saved_run_tutorial_frame(app *a,const char *root,const char *mode,int *stage,int *code,const char **capture) {
    static unsigned steps;static double held;static char original[4096],report[4096];bool ok=true;
    switch(*stage) {
    case 0:steps=0;build_project(a);break;
    case 1:
        if(!idle(a) || a->library_thread)return;
        ok=a->built && start_run_mode(a,true);break;
    case 2:
        if(!a->hello || !a->history_count)return;
        held=a->simulation_time;steps++;ok=command(a,PS_MSG_STEP);break;
    case 3:
        if(a->simulation_time<=held)return;
        if(steps<32){*stage=2;return;}
        ok=a->simulation_time==2 && fabs(a->values[0]-3)<1e-12 && command(a,PS_MSG_STOP);break;
    case 4: {
        if(!idle(a) || a->library_thread)return;
        ok=a->data.total==33;snprintf(original,sizeof original,"%s",a->last_run);
        a->manager_analysis_only=true;a->template_analysis_language=!strcmp(mode,"saved-run-tutorial-phys");
        join(a->project_input,sizeof a->project_input,root,"Consumer");new_project(a);
        ok &= a->loaded && a->analysis_only;
        char path[4096];join(path,sizeof path,a->root,a->language_analysis?"examples/documentation/saved_run_analysis.phys":"examples/documentation/saved_run_analysis.c");
        ok &= load_editor(&a->analysis,path);a->analysis_dirty=true;select_build_profile(a,1);build_project(a);break;
    }
    case 5: {
        if(!idle(a) || a->library_thread)return;
        char path[4096];join(path,sizeof path,a->build_directory,"experiment" MODULE_EXT);
        ok=a->built && !exists(path) && !nk_str_len_char(&a->experiment.string) && !start_run_mode(a,false);
        a->tab=2;a->analysis_tab=0;snprintf(a->import_test_path,sizeof a->import_test_path,"%s",original);break;
    }
    case 6:test_mouse(a,a->import_bounds,true);break;
    case 7:test_mouse(a,a->import_bounds,false);break;
    case 8:
        if(!idle(a) || a->library_thread)return;
        ok=a->data.total==33 && a->selected_count==1 && strcmp(a->last_run,original);
        if(ok)analyze(a,false);
        break;
    case 9: {
        if(!idle(a) || a->library_thread)return;
        uint32_t plots=0,tables=0;ps_table_row row;
        ok=a->analysis_report && ps_report_describe(a->analysis_report,NULL,NULL,&plots,&tables)==PS_OK && plots==3 && tables==2 &&
            ps_report_row_read(a->analysis_report,0,0,&row)==PS_OK && row.values[0]==33 && row.values[1]==0 && row.values[2]==2 && row.values[3]==0 &&
            ps_report_row_read(a->analysis_report,1,0,&row)==PS_OK && row.values[0]==0 && fabs(row.values[1]-3)<1e-12 && fabs(row.values[3]-1.5)<1e-12 && row.values[4]<1e-11;
        snprintf(report,sizeof report,"%s.psreport",a->report);a->result_view=0;break;
    }
    case 10:*capture="saved-run-position.bmp";break;
    case 11:a->result_view=4;break;
    case 12:*capture="saved-run-motion-table.bmp";break;
    case 13:
        snprintf(a->project_input,sizeof a->project_input,"%s",a->project);ok=open_project(a) && a->analysis_only && !a->built;
        if(ok)request_report_path(a,report);
        break;
    case 14:
        if(!idle(a) || a->library_thread)return;
        ok=!a->built && a->analysis_report && a->show_report;a->tab=2;a->analysis_tab=0;a->result_view=1;break;
    case 15:*capture="saved-run-reopened-velocity.bmp";a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Saved run tutorial stage %d: %s\n",*stage,a->status);*code=1;a->quitting=true;}
    if(a->quitting)printf("SAVED RUN TUTORIAL SELF-TEST: %s\n",*code?"FAILED":"PASSED");
    (*stage)++;
}
