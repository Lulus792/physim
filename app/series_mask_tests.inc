static bool series_mask_test_start(app *a,const char *root,const char *mode) {
    char path[4096];join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    join(a->project_input,sizeof a->project_input,root,"project");
    if(!strcmp(mode,"mask-read")){open_project(a);return a->loaded && a->analysis_only && !a->built;}
    bool language=!strcmp(mode,"mask-phys");a->manager_analysis_only=true;a->template_analysis_language=language;
    a->example=0;new_project(a);if(!a->loaded || !a->analysis_only)return false;
    join(path,sizeof path,a->root,language?"examples/language/analysis_masks.phys":"examples/analysis_masks/analysis.c");
    if(!load_editor(&a->analysis,path))return false;
    a->analysis_dirty=true;select_build_profile(a,1);
    SDL_SetWindowSize(a->window,1080,740);return true;
}
static void series_mask_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    static bool settle;if(settle){settle=false;return;}settle=true;bool ok=true;
    bool read=!strcmp(mode,"mask-read");
    switch(*stage) {
    case 0:
        if(read){char path[4096];join(path,sizeof path,a->project,"saved-mask.psreport");request_report_path(a,path);a->tab=2;a->analysis_tab=0;*stage=4;return;}
        build_project(a);break;
    case 1:if(!idle(a) || a->library_thread)return;ok=a->built && !a->runner.running;a->tab=2;a->analysis_tab=0;break;
    case 2:test_mouse(a,a->analysis_start_bounds,true);break;
    case 3:test_mouse(a,a->analysis_start_bounds,false);break;
    case 4: {
        if(!idle(a) || a->library_thread)return;
        uint32_t plots=0;const uint8_t *mask=NULL;const ps_curve_data *curve=NULL;
        ok=a->analysis_report && ps_report_describe(a->analysis_report,NULL,NULL,&plots,NULL)==PS_OK && plots==3 &&
            ps_report_curve_mask(a->analysis_report,0,0,&mask)==PS_OK && mask && mask[2]==0 && mask[3]==3 &&
            ps_report_curve_view(a->analysis_report,1,0,&curve)==PS_OK && curve->y[0]==1 && curve->y[3]==7 && curve->y[6]==13;
        *capture=read?"mask-reopened.bmp":"mask-signal.bmp";break;
    }
    case 5:a->result_view=1;*capture="mask-derivative.bmp";break;
    case 6:a->result_view=2;*capture="mask-pchip.bmp";break;
    case 7: {
        char path[4096];join(path,sizeof path,a->project,read?"reopened-mask.svg":"saved-mask.svg");
        ok=ps_report_export_svg(a->analysis_report,0,path)==PS_OK;
        if(!read) {
            join(path,sizeof path,a->project,"saved-mask.psreport");ok &= ps_report_save(a->analysis_report,path)==PS_OK;
            join(path,sizeof path,a->project,"saved-mask.png");ok &= ps_report_image_scaled(a->ui,a->font_ui,a->analysis_report,0,1,path)==PS_OK;
            join(path,sizeof path,a->project,"saved-mask.csv");ok &= ps_report_export_plot_csv(a->analysis_report,0,path)==PS_OK;
        }
        a->quitting=true;break;
    }
    }
    if(!ok){fprintf(stderr,"Series mask GUI failed at stage %d: %s\n",*stage,a->status);*code=1;a->quitting=true;}
    if(a->quitting)printf("SERIES MASK %s SELF-TEST: %s\n",mode,*code?"FAILED":"PASSED");
    (*stage)++;
}
