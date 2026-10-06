#include "text_validation.h"
static bool logging_test_start(app *a,const char *root,const char *mode) {
    char path[4096];join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    join(a->project_input,sizeof a->project_input,root,"project");
    bool language=!strcmp(mode,"logging-phys");a->example=language?10:0;new_project(a);
    if(!a->loaded)return false;
    join(path,sizeof path,a->root,language?"examples/language/logging.phys":"examples/logging/main.c");
    if(!load_editor(&a->experiment,path))return false;
    a->dirty=true;a->dt=.25;snprintf(a->seed,sizeof a->seed,"42");a->project_settings_dirty=true;
    select_build_profile(a,1);SDL_SetWindowSize(a->window,1080,740);return true;
}
static void logging_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    bool ok=true;
    switch(*stage) {
    case 0:build_project(a);break;
    case 1:
        if(!idle(a) || a->library_thread)return;
        ok=a->built;
        if(!strcmp(mode,"logging-flood")){
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
            (!strcmp(mode,"logging-flood") || strstr(a->log,"Experiment [warning, t=0 s]: Step\nnext line"));
        a->show_log=true;dock_show(a,PS_DOCK_LOG);break;
    case 5: {
        struct nk_window *window=nk_window_find(a->ui,dock_windows[PS_DOCK_LOG]);
        ok=window!=NULL;
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
            (!strcmp(mode,"logging-phys") || (!strcmp(mode,"logging-flood")?
            strstr(a->log,"Logger suppressed 908 messages"):
            strstr(a->log,"Experiment [error, t=0.25 s]: Destroyed")));
        char path[4096];snprintf(path,sizeof path,"%s.pslog",a->last_run);
        size_t size=0;void *bytes=SDL_LoadFile(path,&size);ok &= bytes && size>0;SDL_free(bytes);
        a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Logging GUI stage %d: %s\n",*stage,a->status);*code=1;a->quitting=true;}
    if(a->quitting)printf("LOGGING %s SELF-TEST: %s\n",mode,*code?"FAILED":"PASSED");
    (*stage)++;
}
