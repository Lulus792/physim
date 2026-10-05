static bool series_test_start(app *a,const char *root,const char *mode) {
    join(a->project_input,sizeof a->project_input,root,"project");
    if(!strcmp(mode,"series-read")) {open_project(a);return a->loaded && !a->built;}
    bool language=strstr(mode,"phys")!=NULL;
    a->example=language?8:0;new_project(a);if(!a->loaded)return false;
    if(strstr(mode,"-cm-")) {
        char *source=copy_editor_text(&a->experiment);if(!source)return false;
        const char *needle=language?"parameterWithUnit(\"length\", metres, 1.5":"PS_METRE,length_m";
        const char *replacement=language?"parameterWithUnit(\"length\", Unit(1,0,0,0,0,0,0,0.01,\"cm\"), 0.29":"((ps_unit){{1,0,0,0,0,0,0},.01,\"cm\"}),.29";
        char *at=strstr(source,needle);if(!at){free(source);return false;}
        size_t capacity=strlen(source)+strlen(replacement)+1;char *edited=malloc(capacity);
        if(!edited){free(source);return false;}
        snprintf(edited,capacity,"%.*s%s%s",(int)(at-source),source,replacement,at+strlen(needle));
        bool changed=set_editor_text(&a->experiment,edited,strlen(edited));free(edited);free(source);
        if(!changed)return false;
        a->dirty=true;
    }
    SDL_SetWindowSize(a->window,1080,740);a->preferences.sidebar_width=300;
    ps_dock_hide(&a->preferences.dock,PS_DOCK_LOG);select_build_profile(a,1);
    a->batch_runs=3;a->batch_workers=3;a->batch_steps=1000;a->batch_dt=.1;
    a->batch_minimum_dt=1e-6;a->batch_maximum_dt=.2;
    snprintf(a->batch_channel,sizeof a->batch_channel,"angle");return true;
}
static bool series_test_click(app *a,struct nk_rect r) {
    struct nk_rect panel=a->dock_panels[PS_DOCK_WORKSPACE];
    if(!r.w)return false;
    if(r.y<panel.y+32 || r.y+r.h>panel.y+panel.h-24) {
        test_plot_wheel(a,panel.x+panel.w/2,panel.y+panel.h/2,r.y<panel.y+32?1:-1);return false;
    }
    test_mouse(a,r,true);return true;
}
static void series_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    static int row,reported=-1;bool ok=true;bool scaled=strstr(mode,"-cm-")!=NULL;
    if(reported!=*stage) {fprintf(stderr,"Series %s stage %d: %s\n",mode,*stage,a->status);reported=*stage;}
    if(!strcmp(mode,"series-read")) {
        switch(*stage) {
        case 0:a->library_kind=1;open_library(a);break;
        case 1: {
            if(a->library_thread)return;
            row=-1;char target[4096];join(target,sizeof target,a->project,"runs/timed-summary.psreport");
            for(unsigned i=0;i<a->library_visible_count;i++) {
                char path[4096];if(ps_library_path(a->library,a->library_visible_indices[i],path,sizeof path)==PS_OK && !strcmp(path,target))row=(int)i;
            }
            if(row<0)return;
            test_mouse(a,a->library_open_bounds[row],true);break;
        }
        case 2:test_mouse(a,a->library_open_bounds[row],false);break;
        case 3:
            if(a->report_thread || !a->analysis_report)return;
            ok=!a->built && !a->runner.running && a->show_report;
            *capture="series-reopened.bmp";a->quitting=true;break;
        }
    } else switch(*stage) {
    case 0:build_project(a);break;
    case 1:
        if(!idle(a) || a->library_thread)return;
        ok=a->built && a->parameters.count==2;
        if(ok && scaled) {
            double selected;
            ok=ps_parameter_catalog_value(&a->parameters,0,&selected)==PS_OK && selected==.29 &&
                a->parameters.entries[0].default_value==.29;
            /* Explicitly apply the selection, as the parameter form does. */
            a->project_settings_dirty=true;
            save_project(a);
            ok &= test_source_has(a,"physim.project","parameter.length=0.28999999999999998");
        }
        if(!hierarchy_test_click(a,a->batch_navigation_bounds))return;
        break;
    case 2:test_mouse(a,a->batch_navigation_bounds,false);break;
    case 3:
        ok=a->tab==5 && !a->batch_adaptive;
        if(!series_test_click(a,a->batch_adaptive_bounds))return;
        break;
    case 4:test_mouse(a,a->batch_adaptive_bounds,false);break;
    case 5:ok=a->batch_adaptive && a->batch_target;break;
    case 6:if(!series_test_click(a,a->batch_timing_bounds[1]))return;break;
    case 7:test_mouse(a,a->batch_timing_bounds[1],false);break;
    case 8:test_key(a,SDLK_A);break;
    case 9: {
        SDL_Event e={0};e.type=SDL_EVENT_TEXT_INPUT;e.text.windowID=SDL_GetWindowID(a->window);e.text.text="0";ok=SDL_PushEvent(&e);break;
    }
    case 10:break;
    case 11:
        ok=a->batch_end_time==0;start_batch(a);
        ok &= !a->batch_thread && !a->batch_options.runs && strstr(a->status,"Endzeit");
        test_key(a,SDLK_A);break;
    case 12: {
        SDL_Event e={0};e.type=SDL_EVENT_TEXT_INPUT;e.text.windowID=SDL_GetWindowID(a->window);e.text.text="7e-1";ok=SDL_PushEvent(&e);break;
    }
    case 13:break;
    case 14:
        ok=a->batch_end_time==.7;*capture="series-timing.bmp";
        if(!series_test_click(a,a->batch_sweep_bounds))return;
        break;
    case 15:test_mouse(a,a->batch_sweep_bounds,false);break;
    case 16:
        ok=a->batch_sweep && a->batch_sweep_ready && !strcmp(a->parameters.entries[a->batch_sweep_parameter].name,"length");
        snprintf(a->batch_sweep_start,sizeof a->batch_sweep_start,"0.5");snprintf(a->batch_sweep_end,sizeof a->batch_sweep_end,"2.5");break;
    case 17:
        if(!scaled){*stage=27;break;}
        ok=a->parameters.units[0].declared && a->parameters.units[0].scale==.01;
        if(!series_test_click(a,a->batch_sweep_value_bounds[0]))return;
        break;
    case 18:test_mouse(a,a->batch_sweep_value_bounds[0],false);break;
    case 19:test_key(a,SDLK_A);break;
    case 20: {
        SDL_Event e={0};e.type=SDL_EVENT_TEXT_INPUT;e.text.windowID=SDL_GetWindowID(a->window);e.text.text="50";ok=SDL_PushEvent(&e);break;
    }
    case 21:break;
    case 22:if(!series_test_click(a,a->batch_sweep_value_bounds[1]))return;break;
    case 23:test_mouse(a,a->batch_sweep_value_bounds[1],false);break;
    case 24:test_key(a,SDLK_A);break;
    case 25: {
        SDL_Event e={0};e.type=SDL_EVENT_TEXT_INPUT;e.text.windowID=SDL_GetWindowID(a->window);e.text.text="250";ok=SDL_PushEvent(&e);break;
    }
    case 26:break;
    case 27:ok=!strcmp(a->batch_sweep_start,"50") && !strcmp(a->batch_sweep_end,"250");break;
    case 28:if(!series_test_click(a,a->batch_start_bounds))return;break;
    case 29:test_mouse(a,a->batch_start_bounds,false);break;
    case 30:
        if(a->batch_thread)return;
        ok=a->batch_code==PS_OK && a->batch_result.completed==3 && a->batch_options.adaptive &&
           a->batch_options.end_time==.7 && a->batch_options.steps==1000 && a->batch_options.sweep;
        for(unsigned i=0;ok && i<3;i++) {
            char path[4096];snprintf(path,sizeof path,"%s/run-%04u.psrun",a->batch_options.directory,i+1);
            ps_run_reader r;ok=ps_run_open(&r,path)==PS_OK;if(!ok)break;
            double t,v[PS_MAX_CHANNELS],previous=-1;unsigned n=0;ps_result status;
            while((status=ps_run_next(&r,&t,v))==PS_OK) {ok &= t>previous && t<=.7;previous=t;n++;}
            ok &= status==PS_EOF && previous==.7 && n>5 && n<1001 && v[0]==a->batch_result.values[i];ps_run_reader_close(&r);
        }
        break;
    case 31:
        if(a->report_thread || a->library_thread || !a->analysis_report)return;
        ok=a->show_report && a->tab==2;
        {
            ps_plot_info plot;ok &= ps_report_plot_read(a->analysis_report,0,&plot)==PS_OK && !strcmp(plot.x_unit.symbol,scaled?"cm":"m") && plot.x_unit.dimension[0]==1 && plot.x_unit.scale==(scaled?.01:1);
            char path[4096];snprintf(path,sizeof path,"%s/study.png",a->batch_options.directory);
            ok &= ps_report_image_scaled(a->ui,a->font_ui,a->analysis_report,0,1,path)==PS_OK;
            snprintf(path,sizeof path,"%s/study.svg",a->batch_options.directory);
            ok &= ps_report_export_svg(a->analysis_report,0,path)==PS_OK;
        }
        *capture="series-summary.bmp";a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Series failure at stage %d: target %.17g input [%s] code %d completed %u\n",*stage,a->batch_end_time,a->batch_timing_text[1],a->batch_code,a->batch_result.completed);*code=1;a->quitting=true;}
    if(a->quitting)printf("TIMED SERIES %s SELF-TEST: %s\n",mode,*code?"FAILED":"PASSED");
    (*stage)++;
}
