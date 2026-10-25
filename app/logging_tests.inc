#include "text_validation.h"
/* Explicit test-only child, never a product/demo message source. */
static int protocol_output_fixture(bool cancel) {
    ps_binary_stdio();
    const char *begin="Protocol stream α\n",*end="\nProtocol stream complete\n";
    unsigned char block[256];for(unsigned i=0;i<256;i++)block[i]=(unsigned char)i;
    if(fwrite(begin,1,strlen(begin),stdout)!=strlen(begin))return 2;
    for(unsigned i=0;i<600;i++)if(fwrite(block,1,sizeof block,stdout)!=sizeof block)return 2;
    if(fwrite(end,1,strlen(end),stdout)!=strlen(end) || fflush(stdout))return 2;
    if(cancel)ps_sleep(30000);
    return 7;
}
static void protocol_stream_test_frame(app *a,const char *mode,int *stage,int *code) {
    bool cancel=strstr(mode,"cancel")!=NULL,ok=true;
    const char *begin="Protocol stream α\n",*end="\nProtocol stream complete\n";
    size_t prefix=strlen(begin),payload=600u*256u,total=prefix+payload+strlen(end);
    if(*stage==0) {
        char executable[4096];ok=ps_executable_path(executable,sizeof executable);
        const char *args[]={executable,"--protocol-output-fixture",cancel?"cancel":"complete",NULL};
        ps_process_limits limits={0,20};
        if(ok)ok=ps_process_start_limited(&a->job,args,a->project,&limits);
        a->job_kind=1;a->protocol_job_id=0;a->protocol_job_pid=0;
    } else {
        const ps_protocol_record *record=NULL;
        ps_protocol_summary summary=ps_protocol_describe(&a->protocol);
        for(size_t i=0;i<summary.count;i++) {
            const ps_protocol_record *item=ps_protocol_at(&a->protocol,i);
            if(item->message.origin==PS_PROTOCOL_BUILD && item->message.level==PS_LOG_DEBUG)record=item;
        }
        if(cancel && *stage==1) {
            if(!record || record->message.raw_size<total)return;
            ok=a->job.running;
            if(ok){ps_process_kill(&a->job);protocol_job_cancel(a);ps_process_close(&a->job);}
        } else {
            if(a->job.running)return;
            ok=record && record->message.raw_size==total && !record->omitted_bytes &&
                record->message.outcome==(cancel?PS_PROTOCOL_CANCELLED:PS_PROTOCOL_FAILED) &&
                !record->message.has_time && !strcmp(record->message.context,a->project) &&
                !summary.omitted_bytes && !summary.omitted_messages && ps_text_valid(a->log,sizeof a->log,true) &&
                strstr(a->log,"Protocol stream complete") && !a->runner.running;
            if(ok) {
                const unsigned char *raw=record->message.raw;
                ok=!memcmp(raw,begin,prefix) && !memcmp(raw+prefix+payload,end,strlen(end));
                for(size_t i=0;ok && i<payload;i++)ok=raw[prefix+i]==(unsigned char)(i%256);
                if(ok)printf("PROTOCOL STREAM ORIGINAL: bytes=%zu crc32=%08x outcome=%d\n",total,ps_crc32(raw,total),record->message.outcome);
            }
            a->quitting=true;
        }
    }
    if(!ok){*code=1;a->quitting=true;}
    if(a->quitting)printf("PROTOCOL STREAM %s SELF-TEST: %s\n",cancel?"cancel":"complete",*code?"FAILED":"PASSED");
    (*stage)++;
}
static bool logging_test_start(app *a,const char *root,const char *mode) {
    char path[4096];join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    join(a->project_input,sizeof a->project_input,root,"project");
    bool language=!strncmp(mode,"logging-phys",12);a->example=language?10:0;new_project(a);
    if(!a->loaded)return false;
    if(strstr(mode,"-stream")){SDL_SetWindowSize(a->window,1080,740);return true;}
    if(strstr(mode,"-protocol"))join(path,sizeof path,PS_SOURCE_DIR,language?"tests/fixtures/protocol_nonfatal.phys":"tests/fixtures/protocol_nonfatal.c");
    else join(path,sizeof path,a->root,language?"examples/language/logging.phys":"examples/logging/main.c");
    if(!load_editor(&a->experiment,path))return false;
    a->dirty=true;a->dt=.25;snprintf(a->seed,sizeof a->seed,"42");a->project_settings_dirty=true;
    select_build_profile(a,1);SDL_SetWindowSize(a->window,1080,740);
    if(strstr(mode,"-22")){ps_preferences p=a->preferences;p.ui_size=22;preferences_apply(a,&p);}return true;
}
static void logging_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    if(strstr(mode,"-stream")){protocol_stream_test_frame(a,mode,stage,code);return;}
    static uint32_t source,previous_crc;static char previous_run[4096];
    bool ok=true,flood=strstr(mode,"flood")!=NULL,language=!strncmp(mode,"logging-phys",12),protocol=strstr(mode,"-protocol")!=NULL;
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
        if(protocol)printf("PROTOCOL LEGACY OBSERVE: paused=%d time=%.17g value=%.17g utf8=%d warning=%d level=%d warning-time=%.17g count=%u\n",a->paused,a->simulation_time,a->values[0],ps_text_valid(a->log,sizeof a->log,true),a->live_warning_valid,a->live_warning.level,a->live_warning.time_s,a->live_event_count);
        ok=a->paused && a->simulation_time==.25 && a->values[0]==142.25 &&
            ps_text_valid(a->log,sizeof a->log,true) &&
            (flood || strstr(a->log,"Experiment [warning, t=0 s]: Step\nnext line"));
        ok &= a->live_warning_valid && a->live_warning.level==(protocol?PS_LOG_ERROR:PS_LOG_WARNING) && a->live_warning.time_s==0 &&
            !strcmp(a->live_warning.message,protocol?"Nonfatal error: α\n\toriginal text":"Step\nnext line") && a->live_event_count>0 && a->live_event_count<=8;
        if(flood)ok &= a->live_event_count==8 && a->live_event_omitted>4000;
        if(protocol) {
            const char *text="Nonfatal error: α\n\toriginal text";bool found=false;
            ps_protocol_summary summary=ps_protocol_describe(&a->protocol);
            for(size_t i=0;i<summary.count;i++) {
                const ps_protocol_record *event=ps_protocol_at(&a->protocol,i);
                if(event->message.origin==PS_PROTOCOL_SIMULATION && event->message.level==PS_LOG_ERROR && event->message.raw_size==strlen(text) && !memcmp(event->message.raw,text,strlen(text)))
                    found=event->message.has_time && event->message.time_s==0 && event->message.outcome==PS_PROTOCOL_NO_OUTCOME && !strcmp(event->message.context,a->last_run);
            }
            ok &= found && a->runner.running && a->values[0]==142.25;
            printf("PROTOCOL NONFATAL: original=%d running=%d value=%.2f retained=%zu\n",found,a->runner.running,a->values[0],summary.count);
        }
        if(laboratory_visible(a)){a->laboratory_groups[0]=NK_MINIMIZED;a->model_disclosure=NK_MINIMIZED;a->laboratory_groups[2]=NK_MAXIMIZED;}
        *capture="live-inspector.bmp";
        break;
    case 5: {
        if(!a->show_log){a->show_log=true;dock_show(a,PS_DOCK_LOG);return;}
        struct nk_window *window=nk_window_find(a->ui,dock_windows[PS_DOCK_LOG]);
        ok=window!=NULL;
        ok &= a->live_warning_bounds.w>0 && a->live_event_bounds.w>0;
        if(laboratory_visible(a))ok &= a->live_measurement_bounds.y>=a->scene_viewport_bounds.y+a->scene_viewport_bounds.h &&
            a->run_control_bounds[0].y+a->run_control_bounds[0].h<=a->scene_viewport_bounds.y;
        else ok &= a->live_measurement_bounds.x>=a->scene_viewport_bounds.x+a->scene_viewport_bounds.w &&
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
