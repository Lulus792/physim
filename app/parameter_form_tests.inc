static bool parameter_form_test_start(app *a,const char *root,const char *mode) {
    bool language=strstr(mode,"-phys")!=NULL,read=strstr(mode,"-read")!=NULL;char path[4096];
    join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    join(a->project_input,sizeof a->project_input,root,"project");
    if(read) {
        open_project(a);
        if(a->parameters.count!=2 || !a->parameters.entries[0].defined ||
           !a->parameters.units[0].declared || strcmp(a->parameters.units[0].symbol,"cm") ||
           a->parameters.entries[0].default_value!=.5 || a->parameters.entries[0].minimum!=.1 ||
           a->parameters.entries[0].maximum!=2 || strcmp(a->parameters.selected[0],"75"))return false;
    }
    else {
        a->example=language?8:0;a->template_analysis_language=language?1:0;new_project(a);
        join(path,sizeof path,root,language?"parameter_form.phys":"parameter_form.c");
        if(!load_editor(&a->experiment,path))return false;a->dirty=true;
        select_build_profile(a,1);a->dt=.01;a->project_settings_dirty=true;
    }
    SDL_SetWindowSize(a->window,1080,740);a->show_log=false;ps_dock_hide(&a->preferences.dock,PS_DOCK_LOG);
    return a->loaded;
}
static bool parameter_form_run_valid(app *a) {
    ps_run_reader reader;if(ps_run_open(&reader,a->last_run)!=PS_OK)return false;
    bool ok=reader.channels==2 && !strcmp(reader.schema[0].name,"distance") && !strcmp(reader.schema[0].unit,"m") &&
        strstr(reader.metadata,"parameter.distance=0.75\n") && strstr(reader.metadata,"parameter.gain=3.5\n") &&
        strstr(reader.metadata,"parameter_default.distance=0.5\n") && strstr(reader.metadata,"parameter_unit.distance=cm\n") &&
        strstr(reader.metadata,"parameter_scale.distance=0.01\n");
    double time,values[PS_MAX_CHANNELS];size_t count=0;ps_result result;
    while((result=ps_run_next(&reader,&time,values))==PS_OK) {
        ok &= fabs(time-(double)count*.01)<1e-10 && values[0]==.75 && values[1]==3.5;count++;
    }
    ok &= result==PS_EOF && count>1;ps_run_reader_close(&reader);return ok;
}
static void parameter_form_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    static bool settle;static uint32_t source;bool ok=true,read=strstr(mode,"-read")!=NULL;
    if(settle){settle=false;return;}settle=true;
    if(a->library_thread)return;
    double distance=0,gain=0;
    switch(*stage) {
    case 0:
        source=ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
        test_window_key_mod(a->window,SDLK_F5,0);break;
    case 1:
        if(!idle(a))return;
        ok=a->built && a->parameters.count==2 && !strcmp(a->parameters.entries[0].name,"distance") &&
            !strcmp(a->parameters.entries[1].name,"gain") && a->parameters.units[0].declared &&
            !strcmp(a->parameters.units[0].symbol,"cm") && a->parameters.units[0].scale==.01 &&
            a->parameters.entries[0].default_value==.5 && a->parameters.entries[0].minimum==.1 &&
            a->parameters.entries[0].maximum==2 && !a->parameters.units[1].declared;
        ok &= ps_parameter_catalog_value(&a->parameters,0,&distance)==PS_OK && distance==(read?.75:.5) &&
            ps_parameter_catalog_value(&a->parameters,1,&gain)==PS_OK && gain==(read?3.5:2);
        a->tab=1;
        a->laboratory_groups[0]=NK_MINIMIZED;a->model_disclosure=NK_MINIMIZED;
        break;
    case 2:
        if(laboratory_visible(a)) {a->laboratory_groups[0]=NK_MINIMIZED;a->model_disclosure=NK_MINIMIZED;}
        if(!hierarchy_test_click(a,a->parameter_header_bounds))return;break;
    case 3:test_mouse(a,a->parameter_header_bounds,false);break;
    case 4:
        ok=a->parameter_disclosure==NK_MAXIMIZED && a->parameter_input_bounds[0].w>0 && a->parameter_input_bounds[1].w>0;
        *capture=read?"parameters-restored.bmp":"parameters-default.bmp";
        if(read){ok &= !strcmp(a->parameters.selected[0],"75") && !strcmp(a->parameters.selected[1],"3,5");a->quitting=true;}
        break;
    case 5:if(!hierarchy_test_click(a,a->parameter_input_bounds[0]))return;break;
    case 6:test_mouse(a,a->parameter_input_bounds[0],false);break;
    case 7:test_key(a,SDLK_A);break;
    case 8:layout_test_text(a,"9");break;
    case 9:
        ok=!strcmp(a->parameters.selected[0],"9") && ps_parameter_catalog_value(&a->parameters,0,&distance)==PS_INVALID && a->project_settings_dirty;
        *capture="parameters-invalid.bmp";test_window_key_mod(a->window,SDLK_F6,0);break;
    case 10:ok=!a->runner.running && !a->last_run[0] && a->built;break;
    case 11:scene_camera_2d(a,true);if(!hierarchy_test_click(a,a->parameter_default_bounds[0]))return;break;
    case 12:test_mouse(a,a->parameter_default_bounds[0],false);break;
    case 13:ok=a->camera_2d && !strcmp(a->parameters.selected[0],"50") && ps_parameter_catalog_value(&a->parameters,0,&distance)==PS_OK && distance==.5;break;
    case 14:if(!hierarchy_test_click(a,a->parameter_input_bounds[0]))return;break;
    case 15:test_mouse(a,a->parameter_input_bounds[0],false);break;
    case 16:test_key(a,SDLK_A);break;
    case 17:layout_test_text(a,"75");break;
    case 18:ok=!strcmp(a->parameters.selected[0],"75") && ps_parameter_catalog_value(&a->parameters,0,&distance)==PS_OK && distance==.75;break;
    case 19:if(!hierarchy_test_click(a,a->parameter_input_bounds[1]))return;break;
    case 20:test_mouse(a,a->parameter_input_bounds[1],false);break;
    case 21:test_key(a,SDLK_A);break;
    case 22:layout_test_text(a,"3.5");break;
    case 23:
        ok=!strcmp(a->parameters.selected[1],"3.5") && ps_parameter_catalog_value(&a->parameters,1,&gain)==PS_OK && gain==3.5;
        *capture="parameters-selected.bmp";test_window_key_mod(a->window,SDLK_F6,0);break;
    case 24:
        if(!a->hello || a->simulation_time<.02)return;
        ok=a->runner.running && a->values[0]==.75 && a->values[1]==3.5 && !a->project_settings_dirty;
        if(!hierarchy_test_click(a,a->parameter_default_bounds[0]))return;break;
    case 25:test_mouse(a,a->parameter_default_bounds[0],false);break;
    case 26:
        ok=!strcmp(a->parameters.selected[0],"75") && a->values[0]==.75 && !a->project_settings_dirty;
        test_window_key_mod(a->window,SDLK_F9,0);break;
    case 27:
        if(!idle(a))return;ok=!a->runner.running && parameter_form_run_valid(a);break;
    case 28: {
        ok=source==ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string)) && !a->dirty;
        SDL_Event event={0};event.type=SDL_EVENT_QUIT;ok &= SDL_PushEvent(&event);
        printf("PARAMETER FORM SELF-TEST: %s\n",ok?"PASSED":"FAILED");break;
    }
    }
    if(!ok){fprintf(stderr,"Parameter form stage %d selected=%s/%s built=%d running=%d status=%s\n",*stage,a->parameters.selected[0],a->parameters.selected[1],a->built,a->runner.running,a->status);*code=1;a->quitting=true;}
    if(a->quitting)printf("PARAMETER FORM SELF-TEST: %s\n",*code?"FAILED":"PASSED");
    (*stage)++;
}
