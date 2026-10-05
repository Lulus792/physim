static bool pchip_test_start(app *a,const char *root,const char *mode) {
    char path[4096];join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    join(a->project_input,sizeof a->project_input,root,"project");
    if(!strcmp(mode,"pchip-read")){open_project(a);return a->loaded && !a->built;}
    bool language=!strcmp(mode,"pchip-phys");a->example=language?8:0;a->template_analysis_language=language;
    new_project(a);if(!a->loaded)return false;
    join(path,sizeof path,a->root,language?"tests/fixtures/language/analysis_pchip_dense.phys":"tests/fixtures/pchip_analysis.c");
    if(!load_editor(&a->analysis,path))return false;
    a->analysis_dirty=true;a->dt=.01;a->project_settings_dirty=true;select_build_profile(a,1);
    SDL_SetWindowSize(a->window,1080,740);return true;
}
static void pchip_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    static bool settle;if(settle){settle=false;return;}settle=true;
    bool ok=true;bool read=!strcmp(mode,"pchip-read");
    switch(*stage) {
    case 0:
        if(read) {
            char path[4096];join(path,sizeof path,a->project,"saved-pchip.psreport");request_report_path(a,path);a->tab=2;*stage=5;return;
        }
        build_project(a);break;
    case 1:if(!idle(a) || a->library_thread)return;ok=a->built;start_run(a);break;
    case 2:if(!a->hello || a->simulation_time<.15)return;command(a,PS_MSG_STOP);a->stop_at=ps_clock();break;
    case 3:if(!idle(a) || a->library_thread)return;ok=a->data.result==PS_EOF && a->data.total>2;analyze(a,false);break;
    case 4:if(!idle(a) || a->library_thread)return;ok=a->analysis_report && a->show_report;a->tab=2;break;
    case 5: {
        if(!idle(a) || a->library_thread)return;
        const ps_curve_data *cubic=NULL,*linear=NULL;ps_plot_info info;
        ok=a->analysis_report && ps_report_plot_read(a->analysis_report,0,&info)==PS_OK &&
            ps_report_curve_view(a->analysis_report,0,0,&cubic)==PS_OK &&
            ps_report_curve_view(a->analysis_report,0,1,&linear)==PS_OK &&
            cubic->count==129 && linear->count==129 && info.x_unit.dimension[2]==1 && info.y_unit.dimension[0]==1;
        if(ok) {
            ok=fabs(cubic->y[32]-.3125)<2e-14 && fabs(cubic->y[96]-2.1875)<2e-14 &&
                linear->y[32]==.5 && linear->y[96]==2.5 && cubic->y[0]==0 && cubic->y[128]==4;
            for(unsigned i=1;i<cubic->count;i++)ok &= cubic->y[i]>=cubic->y[i-1] && cubic->x[i]==i/64.0;
        }
        *capture=read?"pchip-reopened.bmp":"pchip-curves.bmp";break;
    }
    case 6: {
        char path[4096];join(path,sizeof path,a->project,read?"reopened-pchip.svg":"saved-pchip.svg");
        ok=ps_report_export_svg(a->analysis_report,0,path)==PS_OK;
        if(!read) {
            join(path,sizeof path,a->project,"saved-pchip.psreport");ok &= ps_report_save(a->analysis_report,path)==PS_OK;
            join(path,sizeof path,a->project,"saved-pchip.png");ok &= ps_report_image_scaled(a->ui,a->font_ui,a->analysis_report,0,1,path)==PS_OK;
        }
        a->quitting=true;break;
    }
    }
    if(!ok){fprintf(stderr,"PCHIP GUI failed at stage %d: %s\n",*stage,a->status);*code=1;a->quitting=true;}
    if(a->quitting)printf("PCHIP %s SELF-TEST: %s\n",mode,*code?"FAILED":"PASSED");(*stage)++;
}
