static bool batch_missing_test_start(app *a,const char *root,const char *mode) {
    char path[4096];join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    join(a->project_input,sizeof a->project_input,root,"project");a->example=!strcmp(mode,"missing-phys")?10:5;
    new_project(a);select_build_profile(a,1);SDL_SetWindowSize(a->window,1080,740);return a->loaded;
}
static void batch_missing_test_frame(app *a,int *stage,int *code,const char **capture) {
    static bool settle;bool ok=true;
    if(settle){settle=false;return;}settle=true;
    switch(*stage) {
    case 0:build_project(a);break;
    case 1:
        if(!idle(a) || a->library_thread)return;
        ok=a->built;a->tab=5;a->batch_runs=64;a->batch_steps=1;a->batch_workers=4;a->batch_dt=.01;
        strcpy(a->batch_channel,"sensor.x");break;
    case 2:test_mouse(a,a->batch_start_bounds,true);break;
    case 3:test_mouse(a,a->batch_start_bounds,false);break;
    case 4: {
        if(!idle(a) || a->library_thread)return;
        uint32_t plots,tables;ok=a->batch_code==PS_OK && a->batch_result.completed==64 &&
            a->batch_result.valid>0 && a->batch_result.valid<64 && a->analysis_report &&
            ps_report_describe(a->analysis_report,NULL,NULL,&plots,&tables)==PS_OK && plots==1 && tables==2;
        *capture="missing-histogram.bmp";break;
    }
    case 5:a->result_view=2;*capture="missing-coverage.bmp";break;
    case 6:a->tab=5;a->batch_dt=.005;break;
    case 7:test_mouse(a,a->batch_start_bounds,true);break;
    case 8:test_mouse(a,a->batch_start_bounds,false);break;
    case 9: {
        if(!idle(a) || a->library_thread)return;
        uint32_t plots,tables;ok=a->batch_code==PS_OK && a->batch_result.completed==64 && !a->batch_result.valid &&
            a->analysis_report && ps_report_describe(a->analysis_report,NULL,NULL,&plots,&tables)==PS_OK && !plots && tables==1;
        a->result_view=0;break;
    }
    case 10:*capture="missing-empty.bmp";a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Missing endpoint UI stage %d failed: %s (%s)\n",*stage,a->status,a->batch_result.error);*code=1;a->quitting=true;}
    if(a->quitting)printf("BATCH MISSING SELF-TEST: %s\n",*code?"FAILED":"PASSED");
    (*stage)++;
}
