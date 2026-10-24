/* Actual logging experiment; no synthetic event or recording state. */
static struct {unsigned settle,kind,move_wait;bool bottom,input_wait,status_seen;uint32_t source,run_crc;double held;char paint[12][1024];} inspector_tail;
static uint32_t inspector_tail_source(app *a){return ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));}
static bool inspector_tail_start(app *a,const char *root){
    memset(&inspector_tail,0,sizeof inspector_tail);memset(&inspector_design,0,sizeof inspector_design);
    if(!logging_test_start(a,root,"logging-c"))return false;
    char path[4096];join(path,sizeof path,a->root,"tests/fixtures/inspector_tail.c");if(!load_editor(&a->experiment,path))return false;
    a->dirty=true;a->tab=1;a->show_log=false;ps_dock_hide(&a->preferences.dock,PS_DOCK_LOG);return true;
}
static void inspector_tail_draw(app *a){
    memset(inspector_tail.paint,0,sizeof inspector_tail.paint);inspector_tail.status_seen=false;int window_width,window_height;SDL_GetWindowSize(a->window,&window_width,&window_height);const struct nk_command *command;
    nk_foreach(command,a->ui)if(command->type==NK_COMMAND_TEXT){const struct nk_command_text *text=(const struct nk_command_text*)command;
        if(text->font==&a->workspace_status_font && text->x<window_width && text->y>=window_height-WORKSPACE_STATUS_HEIGHT && text->length>=11 && !memcmp(text->string,"Warnung · ",11))inspector_tail.status_seen=true;
        for(unsigned i=0;i<2;i++)if(text->font==&a->laboratory_model_font && camera_design_contains(a->limit_bounds[i],text))camera_design_append(inspector_tail.paint[i],sizeof inspector_tail.paint[i],text);
        for(unsigned i=0;i<8;i++)if(camera_design_contains(a->laboratory_event_messages[i],text))camera_design_append(inspector_tail.paint[2+i],sizeof inspector_tail.paint[2+i],text);
        if(text->font==&a->laboratory_caption_font && camera_design_contains(a->laboratory_recording_name,text))camera_design_append(inspector_tail.paint[10],sizeof inspector_tail.paint[10],text);
        if(text->font==&a->workspace_status_font && camera_design_contains(a->laboratory_recording_meta,text))camera_design_append(inspector_tail.paint[11],sizeof inspector_tail.paint[11],text);
    }
}
static bool inspector_tail_same(app *a){uint32_t crc;return !a->dirty && a->paused && a->simulation_time==inspector_tail.held && a->values[0]==142.25 && a->live_event_count==3 && a->live_warning_valid && inspector_tail.source==inspector_tail_source(a) && timeline_design_run_crc(a->last_run,&crc) && crc==inspector_tail.run_crc;}
static void inspector_tail_record(app *a,const char *kind,const char **capture){
    printf("INSPECTOR TAIL kind=%s variant=%s size=%u preserved=%d count=%u status-warning=%d",kind,inspector_design_variant(),a->preferences.ui_size,inspector_tail_same(a),a->live_event_count,inspector_tail.status_seen);camera_design_rect("body",a->laboratory_inspector_body);camera_design_rect("warning",a->live_warning_bounds);camera_design_rect("log-button",a->laboratory_log_button);puts("");
    for(unsigned i=0;i<2;i++){printf("TAIL LIMIT index=%u",i);camera_design_rect("card",a->laboratory_limit_cards[i]);camera_design_rect("label",a->laboratory_limit_labels[i]);camera_design_rect("input",a->limit_bounds[i]);printf(" paint=");camera_design_hex(inspector_tail.paint[i]);puts("");}
    for(unsigned i=0;i<a->live_event_count;i++){printf("TAIL EVENT index=%u",i);camera_design_rect("card",a->laboratory_event_cards[i]);camera_design_rect("heading",a->laboratory_event_headings[i]);camera_design_rect("message",a->laboratory_event_messages[i]);printf(" paint=");camera_design_hex(inspector_tail.paint[2+i]);puts("");}
    printf("TAIL RECORDING scenes=%d",a->timeline_scenes);camera_design_rect("row",a->laboratory_recording_bounds);camera_design_rect("name",a->laboratory_recording_name);camera_design_rect("meta",a->laboratory_recording_meta);camera_design_rect("dot",a->laboratory_recording_dot);printf(" name-text=");camera_design_hex(inspector_tail.paint[10]);printf(" meta-text=");camera_design_hex(inspector_tail.paint[11]);puts("");
    static char file[128];snprintf(file,sizeof file,"tail-%s-%s.png",inspector_design_variant(),kind);*capture=file;
}
static void inspector_tail_step(app *a,const char *mode,int *stage,int *code,const char **capture){
    bool ok=true,baseline=strstr(mode,"baseline")!=NULL;
    if(*stage>=8){if(inspector_tail.input_wait){inspector_tail.input_wait=false;return;}inspector_tail.input_wait=true;}
    switch(*stage){
    case 0:if(SDL_GetKeyboardFocus()!=a->window){SDL_RaiseWindow(a->window);return;}
        if(!baseline){int setup=4;inspector_design_frame(a,&setup,code,capture);a->laboratory_groups[2]=NK_MAXIMIZED;inspector_tail.settle=0;*stage=140;return;}build_project(a);break;
    case 140:if(inspector_tail.settle++<3 || !camera_design_reveal(a,a->live_event_bounds))return;
        ok=!a->live_event_count && !*a->last_run;printf("TAIL EMPTY kind=events events=%u run-empty=%d",a->live_event_count,!*a->last_run);camera_design_rect("body",a->laboratory_inspector_body);camera_design_rect("target",a->live_event_bounds);puts("");*capture="tail-empty-events.png";if(!ok)break;*stage=141;return;
    case 141:a->laboratory_groups[2]=NK_MINIMIZED;a->laboratory_groups[4]=NK_MAXIMIZED;a->laboratory_scroll_y=0;inspector_tail.settle=0;*stage=142;return;
    case 142:if(inspector_tail.settle++<3 || !camera_design_reveal(a,a->laboratory_recording_bounds))return;
        ok=!a->live_event_count && !*a->last_run && a->laboratory_recording_dot.w==0;printf("TAIL EMPTY kind=recording events=%u run-empty=%d",a->live_event_count,!*a->last_run);camera_design_rect("body",a->laboratory_inspector_body);camera_design_rect("target",a->laboratory_recording_bounds);printf(" name-text=");camera_design_hex(inspector_tail.paint[10]);printf(" meta-text=");camera_design_hex(inspector_tail.paint[11]);puts("");*capture="tail-empty-recording.png";if(!ok)break;*stage=143;return;
    case 143:a->laboratory_groups[4]=NK_MINIMIZED;a->laboratory_scroll_y=0;build_project(a);*stage=1;return;
    case 1:if(!idle(a) || a->library_thread)return;ok=a->built && start_run_mode(a,true);break;
    case 2:if(!a->hello || !a->paused || !a->history_count)return;ok=command(a,PS_MSG_STEP);break;
    case 3:if(!a->paused || a->simulation_time!=.25 || a->live_event_count<3)return;inspector_tail.source=inspector_tail_source(a);inspector_tail.held=a->simulation_time;ok=timeline_design_run_crc(a->last_run,&inspector_tail.run_crc);break;
    case 4:{int setup=4;inspector_design_frame(a,&setup,code,capture);if(inspector_tail.kind==0)a->limits_disclosure=NK_MAXIMIZED;else a->laboratory_groups[inspector_tail.kind==1?2:4]=NK_MAXIMIZED;inspector_tail.settle=0;inspector_tail.bottom=false;inspector_tail.move_wait=0;break;}
    case 5:{if(inspector_tail.settle++<4)return;ok=inspector_tail_same(a);const char *names[]={"limits","events","recording"};char kind[48];snprintf(kind,sizeof kind,"%s-%s",names[inspector_tail.kind],inspector_tail.bottom?"end":"start");
        if(!baseline){struct nk_rect target=inspector_tail.kind==0?a->limit_bounds[inspector_tail.bottom?1:0]:inspector_tail.kind==1?a->laboratory_event_cards[inspector_tail.bottom?a->live_event_count-1:0]:a->laboratory_recording_bounds;
            if(!camera_design_reveal(a,target))return;
            if(!inspector_tail.move_wait){SDL_Event motion={0};motion.type=SDL_EVENT_MOUSE_MOTION;motion.motion.windowID=SDL_GetWindowID(a->window);motion.motion.which=PS_TEST_MOUSE_ID;motion.motion.x=500;motion.motion.y=250;SDL_PushEvent(&motion);inspector_tail.move_wait=2;return;}
            if(inspector_tail.move_wait>1){inspector_tail.move_wait--;return;}}
        inspector_tail_record(a,kind,capture);
        if(!baseline && inspector_tail.kind<2 && !inspector_tail.bottom){inspector_tail.bottom=true;inspector_tail.settle=0;inspector_tail.move_wait=0;return;}
        if(++inspector_tail.kind<3){*stage=4;return;}inspector_tail.kind=0;
        if(!baseline && ++inspector_design.variant<6){*stage=4;return;}break;}
    case 6:ok=inspector_tail_same(a);command(a,PS_MSG_STOP);a->stop_at=ps_clock();break;
    case 7:if(!idle(a) || a->library_thread || a->data.thread)return;
        if(baseline){puts("INSPECTOR TAIL SELF-TEST: PASSED");a->quitting=true;break;}
        ok=timeline_design_run_crc(a->last_run,&inspector_tail.run_crc);inspector_design.variant=0;{int setup=4;inspector_design_frame(a,&setup,code,capture);}a->limits_disclosure=NK_MAXIMIZED;inspector_tail.settle=0;break;
    case 8:if(inspector_tail.settle++<3 || !hierarchy_test_click(a,a->limit_bounds[0]))return;break;
    case 9:test_mouse(a,a->limit_bounds[0],false);break;
    case 10:test_key(a,SDLK_A);break;
    case 11:layout_test_text(a,"0,5");break;
    case 12:ok=a->runner_limit_invalid[0] && a->runner_memory_mib==0;test_window_key_mod(a->window,SDLK_F6,0);break;
    case 13:ok=!a->runner.running && strstr(a->status,"Speicherlimit");if(!ok)break;a->tab=5;inspector_tail.settle=0;*stage=160;return;
    case 160:if(inspector_tail.settle++<3)return;{double number;ok=a->runner_limit_invalid[0] && ps_parse_input_number(a->runner_limit_text[0],&number) && number==.5 && a->laboratory_limit_cards[0].w>0 && fabsf(a->limit_bounds[0].h-35)<.03f;}if(!ok)break;if(!hierarchy_test_click(a,a->limit_bounds[0]))return;*stage=161;return;
    case 161:test_mouse(a,a->limit_bounds[0],false);*stage=162;return;
    case 162:test_key(a,SDLK_A);*stage=14;return;
    case 14:layout_test_text(a,"0");break;
    case 15:ok=!a->runner_limit_invalid[0] && a->runner_memory_mib==0;if(!ok)break;a->tab=1;inspector_tail.settle=0;*stage=163;return;
    case 163:if(inspector_tail.settle++<3)return;if(!hierarchy_test_click(a,a->limit_bounds[1]))return;*stage=16;return;
    case 16:test_mouse(a,a->limit_bounds[1],false);break;
    case 17:test_key(a,SDLK_A);break;
    case 18:layout_test_text(a,"-1");break;
    case 19:ok=a->runner_limit_invalid[1] && a->runner_wall_seconds==0;analyze(a,false);break;
    case 20:ok=idle(a) && strstr(a->status,"Zeitlimit") && !*a->report;test_window_key_mod(a->window,SDLK_F5,0);break;
    case 21:ok=idle(a) && strstr(a->status,"Zeitlimit");test_key(a,SDLK_A);break;
    case 22:layout_test_text(a,"0,25");break;
    case 23:ok=!a->runner_limit_invalid[1] && a->runner_wall_seconds==.25;test_key(a,SDLK_A);break;
    case 24:layout_test_text(a,"0");break;
    case 25:ok=!a->runner_limit_invalid[1] && a->runner_wall_seconds==0;a->limits_disclosure=NK_MINIMIZED;a->laboratory_groups[2]=NK_MAXIMIZED;break;
    case 26:if(!camera_design_reveal(a,a->laboratory_log_button) || !inspector_design_focus(a,"Protokoll öffnen"))return;test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 27:{uint32_t crc;ok=a->show_log && !(a->preferences.dock.hidden&(1u<<PS_DOCK_LOG)) && inspector_tail.source==inspector_tail_source(a) && timeline_design_run_crc(a->last_run,&crc) && crc==inspector_tail.run_crc;
        if(ok){puts("INSPECTOR TAIL ACTIONS fraction-blocked=1 negative-blocked=1 comma=1 build-and-analysis-guard=1 keyboard-log=1 source-and-run=1 cross-view=1");puts("INSPECTOR TAIL SELF-TEST: PASSED");a->quitting=true;}break;}
    default:ok=false;break;
    }
    if(!ok){fprintf(stderr,"INSPECTOR TAIL FAILED stage=%d variant=%u kind=%u status=%s\n",*stage,inspector_design.variant,inspector_tail.kind,a->status);*code=1;a->quitting=true;}else (*stage)++;
}
