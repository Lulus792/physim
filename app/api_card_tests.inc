/* Real documented snippets, an actual editor selection, one undo transaction,
 * and the normal native build/runner path. Fixtures are not production data. */
static const char *api_card_c_source=
    "#include <physim/experiment.h>\n// α😀 selection before the example\n"
    "static ps_result create(ps_context *context) {\n    /* INSERT_API */\n"
    "    return context->channel_count==1 ? PS_OK : PS_INVALID;\n}\n"
    "static ps_result reset(ps_context *c) {c->values[0]=0;return PS_OK;}\n"
    "static ps_result step(ps_context *c,double dt) {c->values[0]=c->time_s+dt;return PS_OK;}\n"
    "static void scene(ps_context *c,ps_scene *s) {(void)c;(void)s;}\n"
    "static void destroy(ps_context *c) {(void)c;}\n"
    "PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {\n"
    "    static const ps_experiment_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,\n"
    "        .name=\"API card example\",.create=create,.reset=reset,.step=step,.build_scene=scene,.destroy=destroy};\n"
    "    return &api;\n}\n";
static const char *api_card_ph_source=
    "// α😀 selection before the example\n/* INSERT_API */\n"
    "func create():\n    metadata(\"model=API card example\")\n\n"
    "func reset():\n    angle.sample(0)\n\n"
    "func step(dt: Float64):\n    angle.sample(simulationTime() + dt)\n\n"
    "func scene():\n    sphere(Vec3(0, 0, 0), 0.1, 0x80B0FFFF, 1)\n";
static bool api_card_test_start(app *a,const char *root,const char *mode) {
    char path[4096];join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    bool language=strstr(mode,"phys")!=NULL;
    join(a->project_input,sizeof a->project_input,root,"project");a->example=language?8:0;new_project(a);
    if(!a->loaded)return false;
    join(path,sizeof path,a->project,experiment_source(a));FILE *file=fopen(path,"wb");if(!file)return false;
    bool ok=fputs(language?api_card_ph_source:api_card_c_source,file)>=0;ok &= fclose(file)==0;
    if(!ok)return false;
    ps_preferences p=a->preferences;p.theme=strstr(mode,"light")?PS_THEME_LIGHT:PS_THEME_DARK;
    if(strstr(mode,"large"))p.ui_size=22;
    preferences_apply(a,&p);select_build_profile(a,1);
    SDL_SetWindowSize(a->window,strstr(mode,"narrow")?1080:1440,strstr(mode,"narrow")?740:1000);
    return true;
}
static void api_card_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    static char *changed;static unsigned settle,scrolls;static bool prepared;static uint32_t analysis_crc;static struct nk_rect original_slot;
    static ps_dock_layout layout;static double focus_retry;
    bool language=strstr(mode,"phys")!=NULL,detached=strstr(mode,"detached")!=NULL,ok=true;
    const char *original=language?api_card_ph_source:api_card_c_source;
    struct nk_text_edit *edit=&a->experiment;ps_panel_window *panel=&a->panels[PS_DOCK_INSPECTOR];
    struct nk_context *ui=detached && panel->detached?panel->ui:a->ui;
    SDL_Window *window=detached && panel->detached?panel->window:a->window;
    if(settle++<2)return;settle=0;
    if(a->library_thread)return;
    switch(*stage) {
    case 0:
        if(SDL_GetKeyboardFocus()!=a->window){if(ps_clock()>=focus_retry){SDL_RaiseWindow(a->window);focus_retry=ps_clock()+.25;}return;}
        if(!prepared){ok=open_project(a);prepared=ok;if(ok)return;break;}
        ok=completion_test_equal(edit,original) && !a->dirty && !a->built;
        analysis_crc=ps_crc32((const unsigned char*)nk_str_get_const(&a->analysis.string),(size_t)nk_str_len_char(&a->analysis.string));
        for(size_t i=0;i<ps_completion_count();i++) {
            const ps_completion_symbol *symbol=ps_completion_at(i);
            if(symbol->language==language && !strcmp(symbol->name,language?"Channel":"ps_channel_add")){ok=ok && api_reference_open(a,symbol);break;}
        }
        ok=ok && a->api_reference && a->api_reference_preview.example!=SIZE_MAX;
        layout=a->preferences.dock;original_slot=a->dock_panels[PS_DOCK_INSPECTOR];break;
    case 1: {
        const ps_doc_block *b=&a->api_reference->blocks[a->api_reference_preview.example];
        const char *marker=strstr(original,"/* INSERT_API */\n");ok=marker!=NULL;if(!ok)break;
        size_t prefix=(size_t)(marker-original),suffix=strlen(marker+17);
        changed=malloc(prefix+b->length+suffix+1);ok=changed!=NULL;if(!ok)break;
        memcpy(changed,original,prefix);memcpy(changed+prefix,a->api_reference->text+b->text,b->length);
        memcpy(changed+prefix+b->length,marker+17,suffix+1);
        edit->select_start=nk_utf_len(original,(int)prefix);edit->select_end=nk_utf_len(original,(int)(prefix+17));edit->cursor=edit->select_end;
        if(detached){int w,h;SDL_GetWindowSize(a->window,&w,&h);test_mouse(a,a->panels[PS_DOCK_INSPECTOR].action,true);}
        break;
    }
    case 2:if(detached){int w,h;SDL_GetWindowSize(a->window,&w,&h);test_plot_motion(a,(float)w+100,240);}break;
    case 3:if(detached){int w,h;SDL_GetWindowSize(a->window,&w,&h);test_mouse(a,nk_rect((float)w+100,240,1,1),false);}break;
    case 4:
        if(detached && !panel->detached){if(scrolls++<8)return;ok=false;break;}
        if(SDL_GetKeyboardFocus()!=window){if(ps_clock()>=focus_retry){SDL_RaiseWindow(window);focus_retry=ps_clock()+.25;}return;}
        ok=a->api_card_bounds.w>0 && a->api_card_action_bounds.w>0 && a->dock_closes[PS_DOCK_LOG].w==0 && completion_test_equal(edit,original) && !a->dirty;
        if(!nk_sdl_accessibility_request_focus(ui,NULL,"Beispiel einfügen")) {
            if(scrolls++>20){ok=false;break;}
            if(detached){SDL_Event wheel={0};wheel.type=SDL_EVENT_MOUSE_WHEEL;wheel.wheel.windowID=SDL_GetWindowID(window);wheel.wheel.y=-2;wheel.wheel.mouse_x=50;wheel.wheel.mouse_y=200;SDL_PushEvent(&wheel);}
            else {struct nk_rect r=a->dock_panels[PS_DOCK_INSPECTOR];test_plot_wheel(a,r.x+40,r.y+100,-2);}
            return;
        }
        break;
    case 5:
        if(!nk_sdl_accessibility_is_focused(ui,NULL,"Beispiel einfügen"))return;
        printf("API CARD BODY: natural=%.3f measured=%.3f available=%.3f last-argument=%.3f/%.3f action=%.3f\n",a->api_card_natural_height,a->api_card_measured_body_height,a->api_reference_bounds.h,a->api_card_arguments_bounds.y,a->api_card_arguments_bounds.h,a->api_card_action_bounds.y);
        ok=a->api_reference_handbook_bounds.x>=a->api_card_bounds.x+12 && a->api_reference_handbook_bounds.y>=a->api_card_bounds.y+12 &&
            a->api_reference_handbook_bounds.y+a->api_reference_handbook_bounds.h<=a->api_card_action_bounds.y-7 &&
            a->api_reference_bounds.y>=a->api_card_bounds.y+12 && a->api_reference_bounds.y+a->api_reference_bounds.h<=a->api_card_action_bounds.y-7 &&
            a->api_card_action_bounds.y+a->api_card_action_bounds.h<=a->api_card_bounds.y+a->api_card_bounds.h-12;
        ok=ok && a->api_card_arguments_bounds.y+a->api_card_arguments_bounds.h<=a->api_reference_bounds.y+a->api_reference_bounds.h+.5f;
        printf("API CARD CAPTION: %.1f,%.1f,%.1f,%.1f inside-card=%d\n",a->api_reference_handbook_bounds.x,a->api_reference_handbook_bounds.y,a->api_reference_handbook_bounds.w,a->api_reference_handbook_bounds.h,ok);
        printf("API CARD GEOMETRY: card=%.1f,%.1f,%.1f,%.1f action=%.1f,%.1f,%.1f,%.1f ui=%u detached=%d\n",a->api_card_bounds.x,a->api_card_bounds.y,a->api_card_bounds.w,a->api_card_bounds.h,a->api_card_action_bounds.x,a->api_card_action_bounds.y,a->api_card_action_bounds.w,a->api_card_action_bounds.h,a->preferences.ui_size,detached);
        if(detached)join(panel->capture,sizeof panel->capture,a->project,"api-card-detached.bmp");else *capture="api-card-ready.bmp";
        break;
    case 6:test_window_key_mod(window,SDLK_RETURN,0);break;
    case 7: {
        ok=completion_test_equal(edit,changed) && a->dirty && !a->built && !a->job.running && !a->runner.running;
        char path[4096];join(path,sizeof path,a->project,experiment_source(a));size_t bytes;char *disk=load_utf8_text(path,&bytes);
        ok=ok && disk && !strcmp(disk,original);free(disk);
        if(detached){SDL_Event close={0};close.type=SDL_EVENT_WINDOW_CLOSE_REQUESTED;close.window.windowID=SDL_GetWindowID(window);SDL_PushEvent(&close);}
        break;
    }
    case 8:
        ok=!panel->detached && !memcmp(&layout,&a->preferences.dock,sizeof layout) && workbench_test_rect(original_slot,a->dock_panels[PS_DOCK_INSPECTOR]);
        if(SDL_GetKeyboardFocus()!=a->window){SDL_RaiseWindow(a->window);return;}
        test_plot_motion(a,a->source_editor_bounds.x+30,a->source_editor_bounds.y+30);
        test_window_key_mod(a->window,SDLK_Z,PS_UI_COMMAND_MOD);break;
    case 9:ok=completion_test_equal(edit,original);test_window_key_mod(a->window,SDLK_Z,PS_UI_COMMAND_MOD|SDL_KMOD_SHIFT);break;
    case 10:
        ok=completion_test_equal(edit,changed) && analysis_crc==ps_crc32((const unsigned char*)nk_str_get_const(&a->analysis.string),(size_t)nk_str_len_char(&a->analysis.string));
        *capture="api-card-inserted.bmp";test_window_key_mod(a->window,SDLK_F5,0);break;
    case 11:if(a->job.running)return;ok=a->built && !a->dirty;test_window_key_mod(a->window,SDLK_F6,0);break;
    case 12:if(!a->hello || a->simulation_time<.06)return;test_window_key_mod(a->window,SDLK_F9,0);break;
    case 13: {
        if(!idle(a))return;
        ps_run_reader run={0};ok=a->last_run[0] && ps_run_open(&run,a->last_run)==PS_OK;
        unsigned rows=0;
        if(ok) {
            ok=run.channels==1 && !strcmp(run.schema[0].name,"angle") && !strcmp(run.schema[0].unit,"rad");
            double time,values[PS_MAX_CHANNELS];ps_result result;
            while((result=ps_run_next(&run,&time,values))==PS_OK){rows++;ok=ok && isfinite(values[0]) && fabs(values[0]-time)<1e-9;}
            ok=ok && result==PS_EOF && rows>=4;ps_run_reader_close(&run);
        }
        printf("API CARD BUILD/RUN: language=%s rows=%u exact-time-channel=%d\n",language?"Physim":"C",rows,ok);
        *capture="api-card-built.bmp";a->quitting=true;break;
    }
    }
    if(!ok){fprintf(stderr,"API card stage %d failed: %s\n",*stage,a->status);*code=1;a->quitting=true;*capture="api-card-failed.bmp";}
    if(a->quitting){free(changed);changed=NULL;printf("API CARD SELF-TEST: %s\n",*code?"FAILED":"PASSED");}
    (*stage)++;
}
