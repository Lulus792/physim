#include "text_validation.h"
static bool logging_test_start(app *a,const char *root,const char *mode) {
    char path[4096];join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    join(a->project_input,sizeof a->project_input,root,"project");
    bool language=!strncmp(mode,"logging-phys",12);a->example=language?10:0;new_project(a);
    if(!a->loaded)return false;
    join(path,sizeof path,a->root,language?"examples/language/logging.phys":"examples/logging/main.c");
    if(!load_editor(&a->experiment,path))return false;
    a->dirty=true;a->dt=.25;snprintf(a->seed,sizeof a->seed,"42");a->project_settings_dirty=true;
    select_build_profile(a,1);SDL_SetWindowSize(a->window,1080,740);
    if(strstr(mode,"-22")){ps_preferences p=a->preferences;p.ui_size=22;preferences_apply(a,&p);}return true;
}
static void logging_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    static uint32_t source,previous_crc;static char previous_run[4096];
    bool ok=true,flood=strstr(mode,"flood")!=NULL,language=!strncmp(mode,"logging-phys",12);
    switch(*stage) {
    case 0:source=ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));build_project(a);break;
    case 1:
        if(!idle(a) || a->library_thread)return;
        ok=a->built;
        if(flood){
            ok &= a->parameters.count==1 && !strcmp(a->parameters.entries[0].name,"burst");
            strcpy(a->parameters.selected[0],"5000");
        }
        ok &= start_run_mode(a,true);break;
    case 2:
        if(!a->hello || !a->history_count)return;
        ok=a->paused && a->simulation_time==0 && a->values[0]==142 &&
            strstr(a->log,"Experiment [info, t=0 s]: Created: α") &&
            strstr(a->log,"Experiment [debug, t=0 s]: Reset \"quoted\"\t☃");
        test_mouse(a,a->run_control_bounds[2],true);break;
    case 3:test_mouse(a,a->run_control_bounds[2],false);break;
    case 4:
        if(a->simulation_time==0)return;
        ok=a->paused && a->simulation_time==.25 && a->values[0]==142.25 &&
            ps_text_valid(a->log,sizeof a->log,true) &&
            (flood || strstr(a->log,"Experiment [warning, t=0 s]: Step\nnext line"));
        ok &= a->live_warning_valid && a->live_warning.level==PS_LOG_WARNING && a->live_warning.time_s==0 &&
            !strcmp(a->live_warning.message,"Step\nnext line") && a->live_event_count>0 && a->live_event_count<=8;
        if(flood)ok &= a->live_event_count==8 && a->live_event_omitted>4000;
        *capture="live-inspector.bmp";
        break;
    case 5: {
        if(!a->show_log){a->show_log=true;dock_show(a,PS_DOCK_LOG);return;}
        struct nk_window *window=nk_window_find(a->ui,dock_windows[PS_DOCK_LOG]);
        ok=window!=NULL;
        ok &= a->live_measurement_bounds.x>=a->scene_viewport_bounds.x+a->scene_viewport_bounds.w &&
            a->live_warning_bounds.w>0 && a->live_event_bounds.w>0 &&
            a->run_control_bounds[0].y>=a->scene_viewport_bounds.y+a->scene_viewport_bounds.h;
        if(window){window->edit.active=true;window->edit.name=0;
            window->edit.cursor=nk_utf_len(a->log,(int)strlen(a->log));
            window->edit.sel_start=window->edit.sel_end=window->edit.cursor;
            window->edit.scrollbar.y=10000;}
        break;
    }
    case 6:*capture="logging-messages.bmp";ok=command(a,PS_MSG_STOP);break;
    case 7:
        if(a->runner.running || !idle(a))return;
        ok=ps_text_valid(a->log,sizeof a->log,true) &&
            (language || (flood?
            strstr(a->log,"Logger suppressed 908 messages"):
            strstr(a->log,"Experiment [error, t=0.25 s]: Destroyed")));
        char path[4096];snprintf(path,sizeof path,"%s.pslog",a->last_run);
        size_t size=0;void *bytes=SDL_LoadFile(path,&size);ok &= bytes && size>0;SDL_free(bytes);
        ps_run_reader reader={0};ok &= ps_run_open(&reader,a->last_run)==PS_OK;
        if(reader.file){double t,values[PS_MAX_CHANNELS];ok &= reader.channels==1 &&
            ps_run_next(&reader,&t,values)==PS_OK && t==0 && values[0]==142 &&
            ps_run_next(&reader,&t,values)==PS_OK && t==.25 && values[0]==142.25 &&
            ps_run_next(&reader,&t,values)==PS_EOF;ps_run_reader_close(&reader);}
        ok &= source==ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
        if(strstr(mode,"-22")) {
            snprintf(previous_run,sizeof previous_run,"%s",a->last_run);ok &= reset_test_crc(previous_run,&previous_crc);
            test_window_key_mod(a->window,SDLK_F7,0);
        } else a->quitting=true;
        break;
    case 8:
        if(a->reset_pending || a->reset_starting || !a->hello || !strcmp(a->last_run,previous_run) || !a->history_count)return;
        ok=a->paused && a->simulation_time==0 && a->values[0]==142 && !a->live_warning_valid &&
            a->live_event_count==2 && a->live_event_omitted==0;
        *capture="live-inspector-reset.bmp";test_window_key_mod(a->window,SDLK_F9,0);break;
    case 9: {
        if(!idle(a))return;uint32_t current=0;
        ok=reset_test_crc(previous_run,&current) && current==previous_crc && !a->live_event_omitted;
        a->quitting=true;break;
    }
    }
    if(!ok){fprintf(stderr,"Logging GUI stage %d: %s\n",*stage,a->status);*code=1;a->quitting=true;}
    if(a->quitting)printf("LOGGING %s SELF-TEST: %s\n",mode,*code?"FAILED":"PASSED");
    (*stage)++;
}
