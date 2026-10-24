/* Real pendulum, native field raster matrix, and actual numeric input. */
static struct {unsigned settle,kind;bool bottom,input_settle,locked_prepared;char paint[PS_MAX_PARAMETERS][128];double dt,minimum,maximum;uint32_t completed_crc;char seed[32];} run_fields;
static bool run_fields_start(app *a,const char *root) {memset(&run_fields,0,sizeof run_fields);return inspector_design_start(a,root);}
static void run_fields_draw(app *a) {
    memset(run_fields.paint,0,sizeof run_fields.paint);const struct nk_command *command;
    nk_foreach(command,a->ui)if(command->type==NK_COMMAND_TEXT){const struct nk_command_text *text=(const struct nk_command_text*)command;
        if(text->font!=&a->laboratory_model_font)continue;
        unsigned n=run_fields.kind==0?a->parameters.count:5;
        for(unsigned i=0;i<n;i++){struct nk_rect r=run_fields.kind==0?a->parameter_input_bounds[i]:a->run_field_inputs[i];if(camera_design_contains(r,text))camera_design_append(run_fields.paint[i],sizeof run_fields.paint[i],text);}
    }
}
static bool run_fields_preserved(app *a) {
    uint32_t crc=0;return !a->dirty && inspector_design.source==ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string)) &&
        timeline_design_run_crc(a->last_run,&crc) && crc==run_fields.completed_crc;
}
static void run_fields_record(app *a,const char *kind,const char **capture) {
    printf("RUN FIELDS kind=%s variant=%s size=%u disabled=%d preserved=%d",kind,inspector_design_variant(),a->preferences.ui_size,a->runner.running,run_fields_preserved(a));camera_design_rect("body",a->laboratory_inspector_body);puts("");
    unsigned n=run_fields.kind==0?a->parameters.count:5;
    for(unsigned i=0;i<n;i++){
        struct nk_rect card=run_fields.kind==0?a->parameter_card_bounds[i]:a->run_field_cards[i],entry=run_fields.kind==0?a->parameter_input_bounds[i]:a->run_field_inputs[i],label=run_fields.kind==0?a->parameter_label_bounds[i]:a->run_field_labels[i];if(card.w<=0)continue;
        printf("RUN FIELD index=%u",i);camera_design_rect("card",card);camera_design_rect("input",entry);camera_design_rect("label",label);printf(" paint-width=%.4f paint=",a->laboratory_model_font.width(a->laboratory_model_font.userdata,a->laboratory_model_font.height,run_fields.paint[i],(int)strlen(run_fields.paint[i])));camera_design_hex(run_fields.paint[i]);puts("");
    }
    static char file[112];snprintf(file,sizeof file,"run-fields-%s-%s.png",inspector_design_variant(),kind);*capture=file;
}
static void run_fields_step(app *a,int *stage,int *code,const char **capture) {
    if(*stage<4){inspector_design_frame(a,stage,code,capture);return;}bool ok=true;
    /* Events queued here are handled and drawn on subsequent frames, exactly
     * as in parameter_form_test_frame. Observe after the draw, not before it. */
    if(*stage>=11){if(run_fields.input_settle){run_fields.input_settle=false;return;}run_fields.input_settle=true;}
    switch(*stage) {
    case 4:{int setup=4;inspector_design_frame(a,&setup,code,capture);a->parameter_disclosure=NK_MAXIMIZED;run_fields.settle=0;run_fields.dt=a->dt;run_fields.minimum=a->minimum_dt;run_fields.maximum=a->maximum_dt;snprintf(run_fields.seed,sizeof run_fields.seed,"%s",a->seed);run_fields.completed_crc=inspector_design.run_crc;break;}
    case 5:if(run_fields.settle++<3)return;ok=inspector_design_same(a);run_fields_record(a,"locked-parameters",capture);break;
    case 6:if(!run_fields.locked_prepared){a->parameter_disclosure=NK_MINIMIZED;a->steps_disclosure=NK_MAXIMIZED;run_fields.kind=1;run_fields.settle=0;run_fields.locked_prepared=true;return;}
        if(run_fields.settle++<3)return;ok=inspector_design_same(a);run_fields_record(a,"locked-settings",capture);command(a,PS_MSG_STOP);a->stop_at=ps_clock();break;
    case 7:if(!idle(a) || a->library_thread || a->data.thread)return;ok=timeline_design_run_crc(a->last_run,&run_fields.completed_crc);run_fields.kind=0;break;
    case 8:{int setup=4;inspector_design_frame(a,&setup,code,capture);a->dt=run_fields.dt;a->minimum_dt=run_fields.minimum;a->maximum_dt=run_fields.maximum;snprintf(a->seed,sizeof a->seed,"%s",run_fields.seed);a->step_breakpoints[0]=0;a->adaptive_steps=run_fields.kind==2;
        if(run_fields.kind==0)a->parameter_disclosure=NK_MAXIMIZED;else a->steps_disclosure=NK_MAXIMIZED;
        if(run_fields.kind==3){a->dt=NAN;snprintf(a->seed,sizeof a->seed,"18446744073709551616");snprintf(a->step_breakpoints,sizeof a->step_breakpoints,"bad");}
        run_fields.bottom=false;run_fields.settle=0;break;}
    case 9:{if(run_fields.settle++<3)return;ok=run_fields_preserved(a);char kind[48];const char *names[]={"parameters","fixed","adaptive","invalid"};snprintf(kind,sizeof kind,"%s-%s",names[run_fields.kind],run_fields.bottom?"end":"start");
        struct nk_rect target=run_fields.kind==0?a->parameter_input_bounds[a->parameters.count-1]:a->run_field_inputs[4];
        struct nk_rect first=run_fields.kind==0?a->parameter_input_bounds[0]:a->run_field_inputs[0];
        if(!run_fields.bottom && !camera_design_reveal(a,first))return;
        if(run_fields.bottom && !camera_design_reveal(a,target))return;
        run_fields_record(a,kind,capture);if(!run_fields.bottom){run_fields.bottom=true;run_fields.settle=0;return;}
        if(++run_fields.kind<4){*stage=8;return;}run_fields.kind=0;if(++inspector_design.variant<6){*stage=8;return;}break;}
    case 10:{inspector_design.variant=0;int setup=4;inspector_design_frame(a,&setup,code,capture);a->steps_disclosure=NK_MAXIMIZED;a->adaptive_steps=false;a->dt=run_fields.dt;snprintf(a->seed,sizeof a->seed,"42");a->step_breakpoints[0]=0;run_fields.kind=1;run_fields.settle=0;break;}
    case 11:if(run_fields.settle++<3 || !hierarchy_test_click(a,a->run_field_inputs[0]))return;break;
    case 12:test_mouse(a,a->run_field_inputs[0],false);break;
    case 13:test_key(a,SDLK_A);break;
    case 14:layout_test_text(a,"0,002");break;
    case 15:ok=a->dt==.002 && a->project_settings_dirty && run_fields_preserved(a);test_key(a,SDLK_A);break;
    case 16:layout_test_text(a,"0");break;
    case 17:ok=a->dt==0;test_window_key_mod(a->window,SDLK_F6,0);break;
    case 18:ok=!a->runner.running && a->dt==0 && run_fields_preserved(a);test_key(a,SDLK_A);break;
    case 19:layout_test_text(a,"0.002");break;
    case 20:ok=a->dt==.002;if(!hierarchy_test_click(a,a->run_field_inputs[3]))return;break;
    case 21:test_mouse(a,a->run_field_inputs[3],false);break;
    case 22:test_key(a,SDLK_A);break;
    case 23:layout_test_text(a,"18446744073709551616");break;
    case 24:{uint64_t seed;ok=!ps_project_seed_parse(a->seed,&seed);test_window_key_mod(a->window,SDLK_F6,0);break;}
    case 25:ok=!a->runner.running && run_fields_preserved(a);test_key(a,SDLK_A);break;
    case 26:layout_test_text(a,"42");break;
    case 27:ok=!strcmp(a->seed,"42") && run_fields_preserved(a);if(ok){puts("RUN FIELDS ACTIONS comma=1 point=1 zero-blocked=1 seed-overflow-blocked=1 source-and-run=1");puts("RUN FIELDS DESIGN SELF-TEST: PASSED");a->quitting=true;}break;
    default:ok=false;break;
    }
    if(!ok){fprintf(stderr,"RUN FIELDS FAILED stage=%d variant=%u kind=%u dt=%.17g seed=%s status=%s\n",*stage,inspector_design.variant,run_fields.kind,a->dt,a->seed,a->status);*code=1;a->quitting=true;}else (*stage)++;
}
