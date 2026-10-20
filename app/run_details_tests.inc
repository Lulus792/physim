static char run_details_test_root[4096];
static uint32_t run_details_test_archives[2],run_details_test_editors[2],run_details_test_display;
static uint32_t run_details_test_digest(const char *path) {
    size_t size=0;void *data=SDL_LoadFile(path,&size);
    uint32_t crc=data?ps_crc32(data,size):0;SDL_free(data);return crc;
}
static bool run_details_test_start(app *a,const char *root,const char *mode) {
    snprintf(run_details_test_root,sizeof run_details_test_root,"%s",root);
    char path[4096];join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    join(a->project_input,sizeof a->project_input,root,"project");a->example=0;new_project(a);
    if(!a->loaded)return false;
    for(unsigned legacy=0;legacy<2;legacy++) {
        ps_context c={.struct_size=sizeof c,.api_version=PS_API_VERSION,.dt_s=.125,.seed=42};
        if(ps_channel_add(&c,"angle",PS_RADIAN,"Fixture angle α")<0)return false;
        if(!legacy)snprintf(c.model_metadata,sizeof c.model_metadata,
            "model=Fixture pendulum α\nexcluded=rod inertia\n"
            "layer.geometry=ideal|Point mass and rigid rod\nlayer.laws=active|Uniform gravity\n"
            "layer.medium=omitted|Vacuum\nlayer.losses=omitted|No drag\nlayer.numerics=active|RK4\n"
            "layer.measurement=ideal|Exact angle\nlayer.sampling=omitted|One deterministic run\n"
            "parameter.mass=2\nparameter_unit.mass=kg\nseries.kind=linear\nseries.index=2\nseries.count=3\nseries.base_seed=41\nseries.seed_policy=base+index-1\nseries.parameter=mass\nseries.start=1\nseries.end=3\nseries.value=2\nfuture_key=DO_NOT_OPEN ; source.c\n");
        if(!legacy) {
            for(unsigned i=0;i<35;i++) {
                size_t used=strlen(c.model_metadata);
                int n=snprintf(c.model_metadata+used,sizeof c.model_metadata-used,"extra.%02u=Archived annotation α\n",i);
                if(n<0 || (size_t)n>=sizeof c.model_metadata-used)return false;
            }
            size_t used=strlen(c.model_metadata);
            snprintf(c.model_metadata+used,sizeof c.model_metadata-used,"last_annotation=LAST_METADATA_α\n");
        }
        join(path,sizeof path,a->project,legacy?"runs/legacy.psrun":"runs/layers.psrun");
        ps_run_writer writer;double value=.1;
        if(ps_run_create(&writer,path,&c,legacy?"Legacy model":"Declared model α")!=PS_OK ||
           ps_run_append(&writer,0,&value)!=PS_OK || ps_run_append(&writer,.125,&value)!=PS_OK || ps_run_close(&writer)!=PS_OK)return false;
        run_details_test_archives[legacy]=run_details_test_digest(path);
    }
    join(a->last_run,sizeof a->last_run,a->project,"runs/layers.psrun");request_dataset(a);
    a->tab=2;a->analysis_tab=0;a->show_report=false;
    SDL_SetWindowSize(a->window,strstr(mode,"-22")?920:1120,strstr(mode,"-22")?620:760);
    if(strstr(mode,"-22")){ps_preferences p=a->preferences;p.ui_size=22;preferences_apply(a,&p);}
    a->show_log=false;ps_dock_hide(&a->preferences.dock,PS_DOCK_LOG);
    a->dirty=a->analysis_dirty=true;
    run_details_test_editors[0]=ps_crc32((const unsigned char *)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
    run_details_test_editors[1]=ps_crc32((const unsigned char *)nk_str_get_const(&a->analysis.string),(size_t)nk_str_len_char(&a->analysis.string));
    return true;
}
static bool run_details_test_reveal_action(app *a) {
    struct nk_rect button=a->run_details_button;
    if(button.w<=0)return false;
    if(analysis_visible(a)) {
        struct nk_rect pane=a->dock_panels[PS_DOCK_INSPECTOR];
        return test_reveal_inspector_action(a,button,pane);
    }
    return true;
}
static void run_details_test_frame(app *a,int *stage,int *code,const char **capture) {
    static bool settle;if(settle){settle=false;return;}settle=true;
    if(a->data.thread)return;
    bool ok=true;char path[4096];
    switch(*stage) {
    case 0:if(!run_details_test_reveal_action(a))return;test_mouse(a,a->run_details_button,true);break;
    case 1:test_mouse(a,a->run_details_button,false);break;
    case 2:
        ok=a->run_details_manager && strstr(a->run_details_overview,"Idealisiert · Point mass") && strstr(a->run_details_overview,"Nicht modelliert · Vacuum") && strstr(a->run_details_overview,"Lineare Parameterstudie") && strstr(a->run_details_overview,"Wirksamer Studienwert [SI]: 2");
        join(path,sizeof path,run_details_test_root,"overview.txt");ok &= report_details_test_write(path,a->run_details_overview);
        *capture="run-details-overview.bmp";test_mouse(a,a->run_details_copy_bounds,true);break;
    case 3:test_mouse(a,a->run_details_copy_bounds,false);break;
    case 4:ok=report_details_test_clipboard(a->run_details_overview);test_mouse(a,a->run_details_tabs[1],true);break;
    case 5:test_mouse(a,a->run_details_tabs[1],false);break;
    case 6:
        ok=a->run_details_tab==1 && strstr(a->run_details_display,"future_key=DO_NOT_OPEN ; source.c");
        run_details_test_display=ps_crc32((const unsigned char *)a->run_details_display,strlen(a->run_details_display));
        join(path,sizeof path,run_details_test_root,"metadata.txt");ok &= report_details_test_write(path,a->run_details_metadata);
        *capture="run-details-metadata.bmp";test_mouse(a,a->run_details_text_bounds,true);break;
    case 7:test_mouse(a,a->run_details_text_bounds,false);break;
    case 8:ok=a->run_details_active;test_window_key_mod(a->window,SDLK_A,PS_UI_COMMAND_MOD);break;
    case 9:layout_test_text(a,"ATTEMPTED CHANGE");break;
    case 10:test_window_key_mod(a->window,SDLK_DELETE,0);break;
    case 11:test_key(a,SDLK_F5);test_key(a,SDLK_F8);break;
    case 12:
        ok=run_details_test_display==ps_crc32((const unsigned char *)a->run_details_display,strlen(a->run_details_display)) && !a->job.running && a->dirty && a->analysis_dirty;
        ok &= run_details_test_editors[0]==ps_crc32((const unsigned char *)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
        ok &= run_details_test_editors[1]==ps_crc32((const unsigned char *)nk_str_get_const(&a->analysis.string),(size_t)nk_str_len_char(&a->analysis.string));
        test_window_key_mod(a->window,SDLK_END,SDL_KMOD_CTRL);*stage=29;break;
    case 30:
        {struct nk_window *window=nk_window_find(a->ui,"run-details");
        ok=window && window->edit.scrollbar.y>0;
        if(!ok)fprintf(stderr,"Details end: cursor=%d expected=%d scroll=%u\n",window?window->edit.cursor:-1,nk_utf_len(a->run_details_display,(int)strlen(a->run_details_display)),window?window->edit.scrollbar.y:0);}
        *capture="run-details-metadata-end.bmp";test_mouse(a,a->run_details_copy_bounds,true);*stage=12;break;
    case 13:test_mouse(a,a->run_details_copy_bounds,false);break;
    case 14:ok=report_details_test_clipboard(a->run_details_metadata);test_key(a,SDLK_ESCAPE);break;
    case 15:
        ok=!a->run_details_manager;join(a->last_run,sizeof a->last_run,a->project,"runs/legacy.psrun");request_dataset(a);break;
    case 16:if(!run_details_test_reveal_action(a))return;test_mouse(a,a->run_details_button,true);break;
    case 17:test_mouse(a,a->run_details_button,false);break;
    case 18:
        ok=a->run_details_manager && strstr(a->run_details_overview,"Geometrie und Anfangszustand\nNicht dokumentiert") && !strstr(a->run_details_overview,"Fixture pendulum");
        join(path,sizeof path,run_details_test_root,"legacy.txt");ok &= report_details_test_write(path,a->run_details_overview);
        *capture="run-details-legacy.bmp";test_mouse(a,a->run_details_close_bounds,true);break;
    case 19:test_mouse(a,a->run_details_close_bounds,false);break;
    case 20:
        ok=!a->run_details_manager;
        join(path,sizeof path,a->project,"runs/layers.psrun");ok &= run_details_test_archives[0]==run_details_test_digest(path);
        join(path,sizeof path,a->project,"runs/legacy.psrun");ok &= run_details_test_archives[1]==run_details_test_digest(path);
        printf("RUN DETAILS: %s\n",ok && !*code?"PASSED":"FAILED");a->dirty=a->analysis_dirty=false;a->quitting=true;break;
    default:ok=false;a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Run details failed at stage %d\n",*stage);*code=1;}
    (*stage)++;
}
