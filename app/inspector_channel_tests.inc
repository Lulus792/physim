/* Actual pendulum, viewed-time selection and persisted display units. */
static struct {unsigned settle,reveal_mask,unit_frames;double unit_deadline;bool wait;int channel;char paint[3][512];} inspector_channel_test;
static bool inspector_channel_start(app *a,const char *root){memset(&inspector_channel_test,0,sizeof inspector_channel_test);if(!inspector_design_start(a,root))return false;channel_units_start(a);return a->channel_units_writable;}
static void inspector_channel_draw(app *a){
    memset(inspector_channel_test.paint,0,sizeof inspector_channel_test.paint);const struct nk_command *command;
    nk_foreach(command,a->ui)if(command->type==NK_COMMAND_TEXT){const struct nk_command_text *t=(const struct nk_command_text*)command;
        for(unsigned i=0;i<2;i++)if(t->font==&a->laboratory_control_font && camera_design_contains(a->inspector_channel_fields[i],t))camera_design_append(inspector_channel_test.paint[i],sizeof inspector_channel_test.paint[i],t);
        if(t->font==&a->laboratory_inspector_value_font && camera_design_contains(a->inspector_channel_value,t))camera_design_append(inspector_channel_test.paint[2],sizeof inspector_channel_test.paint[2],t);
    }
}
static bool inspector_channel_preserved(app *a){uint32_t crc;return !a->dirty && a->paused && a->simulation_time==inspector_design.held && inspector_design.source==ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string)) && timeline_design_run_crc(a->last_run,&crc) && crc==inspector_design.run_crc;}
static void inspector_channel_record(app *a,const char *kind,const char **capture){
    static char name[128];snprintf(name,sizeof name,"channel-%s-%s.png",inspector_design_variant(),kind);*capture=name;
    printf("INSPECTOR CHANNEL kind=%s variant=%s size=%u preserved=%d browsing=%d time=%.17g",kind,inspector_design_variant(),a->preferences.ui_size,inspector_channel_preserved(a),a->timeline_browsing,display_time(a));camera_design_rect("body",a->laboratory_inspector_body);camera_design_rect("value",a->inspector_channel_value);camera_design_rect("hint",a->inspector_channel_hint);
    printf(" value-font=%.4f em=%.4f",a->laboratory_inspector_value_font.height,a->workbench_regular_em);printf(" paint=");camera_design_hex(inspector_channel_test.paint[2]);puts("");
    for(unsigned i=0;i<2;i++){printf("CHANNEL FIELD index=%u",i);camera_design_rect("card",a->inspector_channel_cards[i]);camera_design_rect("label",a->inspector_channel_labels[i]);camera_design_rect("field",a->inspector_channel_fields[i]);printf(" paint=");camera_design_hex(inspector_channel_test.paint[i]);puts("");}
    if(a->channel_count){int selected=a->laboratory_channels[0];ps_display_unit unit=channel_display_unit(a,&a->live_channels[selected]);printf("CHANNEL DATA selected=%d raw=%.17g scale=%.17g symbol=",selected,display_values(a)[selected],unit.scale);camera_design_hex(unit.symbol);printf(" name=");camera_design_hex(a->live_channels[selected].name);puts("");}
}
static void inspector_channel_setup(app *a){int stage=4,code=0;const char *capture=NULL;inspector_design_frame(a,&stage,&code,&capture);a->laboratory_groups[1]=NK_MAXIMIZED;inspector_channel_test.settle=0;}
static bool inspector_channel_reveal_fields(app *a){
    struct nk_rect target=a->inspector_channel_cards[0];float available=a->laboratory_inspector_body.h-20;
    if(target.h>available){
        if(!(inspector_channel_test.reveal_mask&(1u<<inspector_design.variant))){printf("CHANNEL REVEAL: variant=%s card-height=%.4f available=%.4f use-label-and-control=1\n",inspector_design_variant(),target.h,available);inspector_channel_test.reveal_mask|=1u<<inspector_design.variant;}
        target=a->inspector_channel_labels[0];target.h=a->inspector_channel_fields[0].y+a->inspector_channel_fields[0].h-target.y;
    }
    return camera_design_reveal(a,target);
}
static void inspector_channel_step(app *a,int *stage,int *code,const char **capture){
    bool ok=true;if(inspector_channel_test.wait){inspector_channel_test.wait=false;return;}inspector_channel_test.wait=true;
    switch(*stage){
    case 0:if(SDL_GetKeyboardFocus()!=a->window){SDL_RaiseWindow(a->window);return;}inspector_channel_setup(a);break;
    case 1:if(inspector_channel_test.settle++<3 || !camera_design_reveal(a,a->inspector_channel_value))return;ok=!a->channel_count && !strcmp(inspector_channel_test.paint[2],"Noch keine Messwerte");*capture="channel-empty.png";puts("CHANNEL EMPTY actual=1");break;
    case 2:build_project(a);break;
    case 3:if(!idle(a) || a->library_thread)return;ok=a->built;start_run(a);break;
    case 4:if(!a->hello || a->simulation_time<.2)return;command(a,PS_MSG_PAUSE);inspector_channel_test.settle=0;break;
    case 5:if(!a->paused || inspector_channel_test.settle++<2)return;inspector_design.source=ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));inspector_design.held=a->simulation_time;inspector_design.scene=inspector_design_scene(a);ok=timeline_design_run_crc(a->last_run,&inspector_design.run_crc);break;
    case 6:if(!camera_design_reveal(a,a->inspector_channel_fields[0]) || !inspector_design_focus(a,laboratory_choice_names[0]))return;test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 7:inspector_channel_test.channel=-1;for(unsigned i=0;i<a->channel_count;i++)if(!strcmp(a->live_channels[i].name,"position.x"))inspector_channel_test.channel=(int)i;
        ok=inspector_channel_test.channel>=0;if(!ok)break;if(!inspector_design_focus(a,"position.x"))return;test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 8:ok=a->laboratory_channels[0]==inspector_channel_test.channel;if(!ok)break;if(!camera_design_reveal(a,a->inspector_channel_fields[1]) || !inspector_design_focus(a,laboratory_choice_names[1]))return;test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 9:if(!inspector_design_focus(a,"cm"))return;test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 10:{ps_display_unit u=channel_display_unit(a,&a->live_channels[inspector_channel_test.channel]);ok=!a->channel_unit_manager && u.scale==.01 && !strcmp(u.symbol,"cm") && inspector_channel_preserved(a);if(!ok)break;if(!camera_design_reveal(a,a->inspector_channel_value))return;inspector_channel_record(a,"centimetres",capture);break;}
    case 11:timeline_design_mouse(a,0,true);break;
    case 12:timeline_design_mouse(a,0,false);break;
    case 13:ok=a->timeline_browsing && display_time(a)<inspector_design.held && inspector_channel_preserved(a);if(!ok)break;if(!camera_design_reveal(a,a->inspector_channel_value))return;inspector_channel_record(a,"browse",capture);test_mouse(a,a->timeline_bounds[2],true);break;
    case 14:test_mouse(a,a->timeline_bounds[2],false);break;
    case 15:ok=!a->timeline_browsing && inspector_channel_preserved(a);if(!ok)break;if(!camera_design_reveal(a,a->inspector_channel_fields[1]) || !inspector_design_focus(a,laboratory_choice_names[1]))return;test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 16:if(!inspector_design_focus(a,"Benutzerdefiniert …"))return;test_window_key_mod(a->window,SDLK_RETURN,0);inspector_channel_test.unit_deadline=ps_clock()+3;inspector_channel_test.unit_frames=0;break;
    case 17:if(!unit_test_window_focus(a,inspector_channel_test.unit_deadline,&inspector_channel_test.unit_frames,&ok))return;ok=ok && a->channel_unit_manager;if(!ok)break;test_window_key_mod(a->unit_task->window,SDLK_ESCAPE,0);break;
    case 18:ok=!a->channel_unit_manager;if(!ok)break;if(!inspector_design_focus(a,laboratory_choice_names[1]))return;test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 19:if(!inspector_design_focus(a,"m"))return;test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 20:ok=!a->channel_units.count && inspector_channel_preserved(a);inspector_channel_setup(a);break;
    case 21:if(inspector_channel_test.settle++<3 || !inspector_channel_reveal_fields(a))return;ok=inspector_channel_preserved(a);inspector_channel_record(a,"fields",capture);break;
    case 22:if(!camera_design_reveal(a,a->inspector_channel_value))return;ok=inspector_channel_preserved(a);inspector_channel_record(a,"value",capture);if(++inspector_design.variant<6){*stage=20;return;}break;
    case 23:ok=inspector_channel_preserved(a);command(a,PS_MSG_STOP);a->stop_at=ps_clock();break;
    case 24:if(!idle(a) || a->library_thread || a->data.thread)return;puts("INSPECTOR CHANNEL ACTIONS keyboard-channel=1 keyboard-unit=1 inline=1 viewed-time=1 custom-escape=1 restore-SI=1 source-and-run=1");puts("INSPECTOR CHANNEL SELF-TEST: PASSED");a->quitting=true;break;
    default:ok=false;break;
    }
    if(!ok){fprintf(stderr,"INSPECTOR CHANNEL FAILED stage=%d variant=%u paint=%s status=%s\n",*stage,inspector_design.variant,inspector_channel_test.paint[2],a->status);*code=1;a->quitting=true;}else (*stage)++;
}
