static bool analysis_project_test_start(app *a,const char *root,const char *mode) {
    snprintf(a->manager_parent,sizeof a->manager_parent,"%s",root);
    char path[4096];join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    SDL_SetWindowSize(a->window,1080,740);
    if(!strcmp(mode,"analysis-project-read")) {
        join(a->project_input,sizeof a->project_input,root,"Independent analysis");return open_project(a) && a->analysis_only;
    }
    join(a->project_input,sizeof a->project_input,root,"Producer");
    a->example=!strcmp(mode,"analysis-project-c")?8:0;new_project(a);select_build_profile(a,1);
    a->dt=.01;a->project_settings_dirty=true;return a->loaded;
}
static void analysis_project_test_frame(app *a,const char *root,const char *mode,int *stage,int *code,const char **capture) {
    static bool settle;static char original[4096];static uint64_t total;
    if(settle){settle=false;return;}settle=true;bool ok=true;
    bool read=!strcmp(mode,"analysis-project-read");
    switch(*stage) {
    case 0:
        if(read){ok=!a->built && !nk_str_len_char(&a->experiment.string);a->tab=0;*capture="analysis-only-reopened.bmp";a->quitting=true;break;}
        build_project(a);break;
    case 1:if(!idle(a) || a->library_thread)return;ok=a->built;start_run(a);break;
    case 2:if(!a->hello || a->simulation_time<.3)return;command(a,PS_MSG_STOP);a->stop_at=ps_clock();break;
    case 3:
        if(!idle(a) || a->library_thread)return;
        ok=a->data.total>2;total=a->data.total;snprintf(original,sizeof original,"%s",a->last_run);
        a->project_manager=true;(void)root;
        strcpy(a->manager_name,"Independent analysis");a->template_analysis_language=!strcmp(mode,"analysis-project-phys");break;
    case 4:test_mouse(a,a->project_kind_bounds,true);break;
    case 5:test_mouse(a,a->project_kind_bounds,false);break;
    case 6:test_mouse(a,a->project_kind_choices[1],true);break;
    case 7:test_mouse(a,a->project_kind_choices[1],false);break;
    case 8:ok=a->manager_analysis_only;*capture="analysis-only-manager.bmp";test_mouse(a,a->project_create_bounds,true);break;
    case 9:test_mouse(a,a->project_create_bounds,false);break;
    case 10: {
        if(a->library_thread)return;
        ok=a->loaded && a->analysis_only && !a->project_manager && !nk_str_len_char(&a->experiment.string);
        char path[4096];join(path,sizeof path,a->project,"main.c");ok &= !exists(path);join(path,sizeof path,a->project,"main.phys");ok &= !exists(path);
        *capture="analysis-only-editor.bmp";build_project(a);break;
    }
    case 11:if(!idle(a) || a->library_thread)return;
        {char module[4096];join(module,sizeof module,a->build_directory,"experiment" MODULE_EXT);ok=!exists(module);}
        ok &= a->built;ok &= !start_run_mode(a,false) && !reset_available(a);a->tab=2;a->analysis_tab=0;snprintf(a->import_test_path,sizeof a->import_test_path,"%s",original);break;
    case 12:test_mouse(a,a->import_bounds,true);break;
    case 13:test_mouse(a,a->import_bounds,false);break;
    case 14:
        if(!idle(a) || a->library_thread)return;
        ok=a->data.total==total && a->selected_count==1 && strcmp(a->last_run,original);
        *capture="analysis-only-imported.bmp";analyze(a,false);break;
    case 15:
        if(!idle(a) || a->library_thread)return;
        ok=a->analysis_report && a->show_report;
        if(ok){uint32_t plots=0,tables=0;ok=ps_report_describe(a->analysis_report,NULL,NULL,&plots,&tables)==PS_OK && plots==2;}
        *capture="analysis-only-report.bmp";a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Analysis project stage %d failed: %s\n",*stage,a->status);*code=1;a->quitting=true;}
    if(a->quitting)printf("ANALYSIS PROJECT %s SELF-TEST: %s\n",mode,*code?"FAILED":"PASSED");(*stage)++;
}
