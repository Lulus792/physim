static bool pendulum_medium_start(app *a,const char *root,const char *mode) {
    bool language=!strcmp(mode,"pendulum-medium-phys");
    a->example=language?8:0;a->template_analysis_language=language?1:0;
    join(a->project_input,sizeof a->project_input,root,"project");new_project(a);
    if(!a->loaded)return false;
    a->dt=.002;a->simulation_speed=10;a->project_settings_dirty=true;
    select_build_profile(a,1);SDL_SetWindowSize(a->window,1080,740);return true;
}
static void pendulum_medium_frame(app *a,int *stage,int *code,const char **capture) {
    static double initial;static unsigned raw_reveals;bool ok=true;
    switch(*stage) {
    case 0:build_project(a);break;
    case 1: {
        if(!idle(a) || a->library_thread)return;
        ok=a->built && a->parameters.count==8;
        const char *names[]={"mass","airDensity","dragCoefficient","area","sensorNoise","integrator"};
        const char *values[]={"2","1.225","0.8","0.08","0.02","2"};
        for(unsigned j=0;j<6;j++) {
            bool found=false;
            for(unsigned i=0;i<a->parameters.count;i++)if(!strcmp(a->parameters.entries[i].name,names[j])) {
                snprintf(a->parameters.selected[i],sizeof a->parameters.selected[i],"%s",values[j]);found=true;
            }
            ok &= found;
        }
        a->project_settings_dirty=true;save_project(a);ok &= start_run_mode(a,true);break;
    }
    case 2:
        if(!a->hello || !a->history_count)return;
        initial=a->values[4];ok=a->paused && a->values[0]==.45 && a->values[5]!=a->values[0];
        ok &= fabs(initial-2*9.80665*1.5*(1-cos(.45)))<1e-12 && command(a,PS_MSG_RUN);break;
    case 3:
        if(a->simulation_time<6)return;
        ok=a->values[4]<initial*.99 && a->channel_count==11 &&
           fabs(a->values[6]-1.5*cos(a->values[0])*a->values[1])<1e-12 &&
           fabs(a->values[7]-1.5*sin(a->values[0])*a->values[1])<1e-12 &&
           fabs(a->values[8]-hypot(a->values[6],a->values[7]))<1e-12;
        {unsigned drag=0;for(unsigned i=0;i<a->scene.count;i++)drag+=a->scene.objects[i].id==9 && a->scene.objects[i].shape==PS_ARROW;
         ok &= drag==1;}
        *capture="pendulum-medium-scene.bmp";ok &= command(a,PS_MSG_STOP);break;
    case 4: {
        if(a->runner.running || !idle(a) || a->library_thread)return;
        ps_run_reader reader;ok=a->data.result==PS_EOF && a->data.total>=3001 && ps_run_open(&reader,a->last_run)==PS_OK;
        if(ok) {
            ok=strstr(reader.metadata,"\nparameter.mass=2\n") &&
               strstr(reader.metadata,"\nparameter.airDensity=1.225") &&
               strstr(reader.metadata,"\nparameter.sensorNoise=0.02") &&
               strstr(reader.metadata,"parameter_unit.airDensity=kg/m3\n") &&
               strstr(reader.metadata,"parameter_unit.mass=kg\n") &&
               strstr(reader.metadata,"\nintegrator=RK4\n") &&
               strstr(reader.metadata,"\nparameter.integrator=2\n");
            ps_run_reader_close(&reader);
        }
        if(ok)analyze(a,false);break;
    }
    case 5:
        if(!idle(a) || a->report_thread)return;
        {uint32_t plots=0,tables=0;ok=a->analysis_report &&
            ps_report_describe(a->analysis_report,NULL,NULL,&plots,&tables)==PS_OK &&
            plots==(a->language_analysis?3u:5u) && tables==(a->language_analysis?4u:3u);}
        a->tab=2;a->result_view=a->language_analysis?1:3;break;
    case 6:*capture="pendulum-medium-energy.bmp";break;
    case 7:a->result_view=a->language_analysis?5:7;break;
    case 8: {
        ps_table_info info;ps_table_row row;
        ok=ps_report_table_read(a->analysis_report,2,&info)==PS_OK &&
           !strcmp(info.title,"Observed amplitude decay") && info.rows==1 &&
           ps_report_row_read(a->analysis_report,2,0,&row)==PS_OK && row.values[0]>=1 && row.values[2]>0;
        *capture="pendulum-medium-decay.bmp";break;
    }
    case 9:test_mouse(a,a->result_export_bounds[0],true);break;
    case 10:test_mouse(a,a->result_export_bounds[0],false);break;
    case 11:
        ok=exists(a->result_export) && strstr(a->result_export,".csv");
        a->result_view=a->language_analysis?1:3;break;
    case 12: {
        int exported=test_report_graphic_export(a,true);if(!exported)return;
        ok=exported>0;*stage=13;break;
    }
    case 14: {
        int exported=test_report_graphic_export(a,false);if(!exported)return;
        ok=exported>0;*stage=15;break;
    }
    case 16:
        ok=exists(a->result_export) && strstr(a->result_export,".png");
        raw_reveals=0;
        test_mouse(a,a->analysis_tabs[0],true);break;
    case 17:test_mouse(a,a->analysis_tabs[0],false);*stage=19;break;
    case 20:
        if(a->show_report || ps_comparison_read(a->comparison).loading)return;
        /* Raw export belongs to the measured-data body, below its statistics.
         * Reveal that actual control rather than clicking stale report bounds. */
        {struct nk_rect area=a->dock_panels[PS_DOCK_WORKSPACE],button=a->raw_export_bounds;
         if(button.w<=0 || button.h<=0 || button.y<area.y+54 || button.y+button.h>area.y+area.h) {
             if(raw_reveals++>=24){
                 fprintf(stderr,"Raw export reveal failed: button=%.1f/%.1f/%.1f/%.1f viewport=%.1f/%.1f/%.1f/%.1f plot=%.1f/%.1f/%.1f/%.1f\n",button.x,button.y,button.w,button.h,area.x,area.y,area.w,area.h,a->plot_area.x,a->plot_area.y,a->plot_area.w,a->plot_area.h);
                 ok=false;break;
             }
             /* The plot owns wheel input over its data rectangle. Scroll the
              * body through its left gutter, as in the statistics regression. */
             test_plot_wheel(a,area.x+30,area.y+area.h*.5f,-6);*stage=21;break;
         }}
        test_mouse(a,a->raw_export_bounds,true);break;
    case 21:test_mouse(a,a->raw_export_bounds,false);*stage=17;break;
    case 22:*stage=19;break; /* Observe fresh layout after the wheel frame. */
    case 18:
        if(!idle(a) || a->library_thread)return;
        ok=a->job_kind==4 && exists(a->report) && strstr(a->report,".csv");
        *capture="pendulum-medium-exports.bmp";a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Pendulum medium stage %d: %s\n",*stage,a->status);*code=1;a->quitting=true;}
    if(a->quitting)printf("PENDULUM MEDIUM SELF-TEST: %s\n",*code?"FAILED":"PASSED");
    (*stage)++;
}
