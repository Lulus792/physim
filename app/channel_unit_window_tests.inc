static void unit_test_text(app *a,const char *text) {
    SDL_Event event={0};event.type=SDL_EVENT_TEXT_INPUT;event.text.windowID=SDL_GetWindowID(a->unit_task->window);event.text.text=text;SDL_PushEvent(&event);
}
static bool unit_test_focus(app *a,unsigned id,int stage,bool *ok) {
    static int previous=-1,attempts;if(previous!=stage){previous=stage;attempts=0;}
    if(a->unit_task->keyboard && a->unit_task->focus==id)return true;
    if(++attempts>12){*ok=false;return true;}test_window_key_mod(a->unit_task->window,SDLK_TAB,0);return false;
}
static bool unit_test_window_focus(app *a,double deadline,unsigned *frames,bool *ok) {
    if(a->unit_task && SDL_GetKeyboardFocus()==a->unit_task->window){if(++*frames<3)return false;return true;}
    *frames=0;if(ps_clock()<deadline)return false;*ok=false;return true;
}
static bool unit_task_test_start(app *a,const char *root,const char *mode) {
    char path[4096];join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    join(a->project_input,sizeof a->project_input,root,"project");a->example=0;new_project(a);if(!a->loaded)return false;
    ps_context c={.struct_size=sizeof c,.api_version=PS_API_VERSION,.dt_s=.1,.seed=42};
    ps_unit velocity;if(ps_unit_divide(PS_METRE,PS_SECOND,"m/s",&velocity)!=PS_OK)return false;
    if(ps_channel_add(&c,"position.x",PS_METRE,"Position")<0 || ps_channel_add(&c,"velocity.x",velocity,"Geschwindigkeit")<0)return false;
    join(path,sizeof path,a->project,"runs/Einheiten.psrun");ps_run_writer writer;if(ps_run_create(&writer,path,&c,"Einheitenfenster")!=PS_OK)return false;
    for(unsigned i=0;i<3;i++){double values[]={i+1.,2*(i+1.)};if(ps_run_append(&writer,i*.1,values)!=PS_OK)return false;}if(ps_run_close(&writer)!=PS_OK)return false;
    SDL_strlcpy(a->last_run,path,sizeof a->last_run);refresh_library(a);a->selected_count=0;select_run(a,path,true);request_dataset(a);
    ps_preferences p=a->preferences;p.ui_size=strstr(mode,"22")?22:16;p.theme=p.ui_size==22?PS_THEME_LIGHT:PS_THEME_DARK;preferences_apply(a,&p);a->tab=2;a->analysis_tab=0;return true;
}
static void unit_task_test_frame(app *a,int *stage,int *code,const char **capture) {
    static double deadline;static SDL_WindowID window;static char path[4096];static unsigned small_settle,small_scroll;bool ok=true;
    if(a->data.thread || a->library_thread)return;
#define UNIT_FOCUS(id) do{if(!unit_test_focus(a,id,*stage,&ok))return;}while(0)
#define UNIT_KEY(key) test_window_key_mod(a->unit_task->window,key,0)
#define UNIT_TEXT(text) do{test_window_key_mod(a->unit_task->window,SDLK_A,PS_UI_COMMAND_MOD);unit_test_text(a,text);}while(0)
    switch(*stage){
    case 0:
        ok=a->data.channel_count==2 && a->data.count==3;open_settings(a);*stage=39;break;
    case 40:if(!settings_test_tab_to(a,24,*stage,&ok))return;test_window_key_mod(a->settings_window,SDLK_SPACE,0);break;
    case 41:ok=a->settings_group==4;if(!settings_test_tab_to(a,27,*stage,&ok))return;
        if(a->preferences.ui_size==16){struct nk_rect r=a->settings_focus_bounds[27],body=a->settings_body;if(r.w<=0 || r.y<body.y || r.y+r.h>body.y+body.h)return;test_window_mouse(a->settings_window,r,true);*stage=43;}
        else test_window_key_mod(a->settings_window,SDLK_SPACE,0);deadline=ps_clock()+3;break;
    case 44:test_window_mouse(a->settings_window,a->settings_focus_bounds[27],false);deadline=ps_clock()+3;*stage=41;break;
    case 42:
        if((!a->unit_task || !a->unit_task->controls[0].w) && ps_clock()<deadline)return;if(!a->unit_task){ok=false;break;}
        if(a->preferences.ui_size==16){int w,h;ok=SDL_GetWindowSize(a->unit_task->window,&w,&h) && w==850 && h==473;*capture="unit-task-wide-16.png";}break;
    case 43:
        window=SDL_GetWindowID(a->unit_task->window);SDL_SetWindowSize(a->unit_task->window,a->preferences.ui_size==22?620:850,a->preferences.ui_size==22?720:570);SDL_SyncWindow(a->unit_task->window);deadline=ps_clock()+3;*stage=0;break;
    case 1:
        if(!a->unit_task->controls[0].w && ps_clock()<deadline)return;
        ok=a->unit_task->count==2 && a->unit_task->ui!=a->ui && a->unit_task->ui!=a->settings_ui && window!=SDL_GetWindowID(a->window) && window!=SDL_GetWindowID(a->settings_window) && a->unit_task->source==SDL_GetWindowID(a->settings_window);
        if(a->preferences.ui_size==22)*capture="unit-task-small-22.png";test_window_key_mod(a->window,SDLK_1,PS_UI_COMMAND_MOD);deadline=ps_clock()+3;break;
    case 2:
        if(a->tab!=0 && ps_clock()<deadline)return;ok=a->tab==0 && a->settings_open && a->channel_unit_manager;
        UNIT_FOCUS(UNIT_SYMBOL);UNIT_TEXT("cm");break;
    case 3:ok=!strcmp(a->unit_draft.symbol,"cm");UNIT_FOCUS(UNIT_FACTOR);UNIT_TEXT("0.01");break;
    case 4:ok=!strcmp(a->channel_unit_scale,"0.01");UNIT_FOCUS(UNIT_CHANNEL);UNIT_KEY(SDLK_RIGHT);break;
    case 5:ok=!strcmp(a->unit_channel.name,"velocity.x");UNIT_FOCUS(UNIT_PRESET);UNIT_KEY(SDLK_RIGHT);UNIT_KEY(SDLK_RIGHT);break;
    case 6:ok=!strcmp(a->unit_draft.symbol,"km/h");UNIT_FOCUS(UNIT_CHANNEL);UNIT_KEY(SDLK_LEFT);break;
    case 7:
        ok=!strcmp(a->unit_channel.name,"position.x") && !strcmp(a->unit_draft.symbol,"cm") && !strcmp(a->channel_unit_scale,"0.01");
        test_window_key_mod(a->settings_window,SDLK_ESCAPE,0);break;
    case 8:ok=!a->settings_open && a->channel_unit_manager;UNIT_FOCUS(UNIT_FACTOR);UNIT_TEXT("0");break;
    case 9:ok=!strcmp(a->channel_unit_scale,"0");UNIT_FOCUS(UNIT_APPLY);UNIT_KEY(SDLK_SPACE);deadline=ps_clock()+3;break;
    case 10:
        if(a->unit_task->apply)return;
        {struct nk_rect body=a->unit_task->body_rect,error=a->unit_task->message_rect;bool visible=error.h>0 && error.y>=body.y-.1f && error.y+error.h<=body.y+body.h+.1f;if(!visible && ps_clock()<deadline)return;ok=visible;}
        ok=ok && a->channel_unit_manager && *a->channel_units_error && !strcmp(a->channel_unit_scale,"0") && !a->channel_units.count;
        if(a->preferences.ui_size==16){*capture="unit-task-error-16.png";*stage=30;}
        else {SDL_SetWindowSize(a->unit_task->window,360,360);SDL_SyncWindow(a->unit_task->window);small_settle=0;deadline=ps_clock()+3;*stage=31;}break;
    case 31:UNIT_FOCUS(UNIT_FACTOR);UNIT_TEXT("0.01");*stage=10;break;
    case 32:{
        channel_unit_task *d=a->unit_task;struct nk_rect body=d->body_rect,error=d->message_rect;struct nk_window *pane=nk_window_find(d->ui,"unit-body");
        bool visible=body.w<=360.5f && error.h>0 && error.y>=body.y-1.1f && (error.h>body.h?error.y<=body.y+1.1f:error.y+error.h<=body.y+body.h+1.1f);
        if((!visible || d->message_scroll) && ps_clock()<deadline)return;ok=visible && !d->message_scroll && pane;
        if(ok && !small_settle++){small_scroll=pane->scrollbar.y;return;}ok=ok && pane->scrollbar.y==small_scroll;
        printf("UNIT SMALL ERROR: body=%.3f message=%.3f top=%.3f scroll=%u stable=%d\n",body.h,error.h,error.y-body.y,small_scroll,ok);
        SDL_SetWindowSize(d->window,620,720);SDL_SyncWindow(d->window);deadline=ps_clock()+3;*stage=32;break;}
    case 33:if((a->unit_task->body_rect.w<619.5f || a->unit_task->message_scroll) && ps_clock()<deadline)return;ok=a->unit_task->body_rect.w>=619.5f && !a->unit_task->message_scroll;*stage=30;break;
    case 11:ok=!strcmp(a->channel_unit_scale,"0.01");UNIT_FOCUS(UNIT_APPLY);UNIT_KEY(SDLK_SPACE);break;
    case 12:{
        if(a->unit_task->apply)return;ps_channel_units saved;ok=a->channel_unit_manager && a->channel_units.count==1 && channel_display_unit(a,a->data.channels).scale==.01 && ps_channel_units_read(a->channel_units_path,&saved)==PS_OK && saved.count==1;
        UNIT_FOCUS(UNIT_CHANNEL);UNIT_KEY(SDLK_RIGHT);break;}
    case 13:
        ok=!strcmp(a->unit_channel.name,"velocity.x") && !strcmp(a->unit_draft.symbol,"km/h") && channel_display_unit(a,a->data.channels+1).scale==1;
        UNIT_FOCUS(UNIT_CHANNEL);UNIT_KEY(SDLK_LEFT);break;
    case 14:ok=!strcmp(a->unit_draft.symbol,"cm");UNIT_FOCUS(UNIT_SI);UNIT_KEY(SDLK_SPACE);break;
    case 15:
        ok=!strcmp(a->unit_draft.symbol,"m") && !strcmp(a->channel_unit_scale,"1") && channel_display_unit(a,a->data.channels).scale==.01;
        UNIT_FOCUS(UNIT_CLOSE);UNIT_KEY(SDLK_SPACE);break;
    case 16:
        ok=!a->channel_unit_manager && channel_display_unit(a,a->data.channels).scale==.01;channel_unit_open(a,a->data.channels);break;
    case 17:{
        ok=!strcmp(a->unit_draft.symbol,"cm");double original=a->data.values[0][1];a->data.values[0][1]=DBL_MAX;channel_unit_apply(a);
        ok=ok && *a->channel_units_error && a->channel_unit_manager && channel_display_unit(a,a->data.channels).scale==.01;a->data.values[0][1]=original;
        SDL_strlcpy(path,a->channel_units_path,sizeof path);SDL_strlcpy(a->channel_units_path,a->project,sizeof a->channel_units_path);
        SDL_strlcpy(a->unit_draft.symbol,"cm2",sizeof a->unit_draft.symbol);SDL_strlcpy(a->channel_unit_scale,"0.02",sizeof a->channel_unit_scale);channel_unit_apply(a);
        ok=ok && *a->channel_units_error && !strcmp(a->unit_draft.symbol,"cm2") && channel_display_unit(a,a->data.channels).scale==.01;
        SDL_strlcpy(a->channel_units_path,path,sizeof a->channel_units_path);channel_unit_apply(a);ok=ok && channel_display_unit(a,a->data.channels).scale==.02;break;}
    case 18:{SDL_Event close={0};close.type=SDL_EVENT_WINDOW_CLOSE_REQUESTED;close.window.windowID=SDL_GetWindowID(a->unit_task->window);SDL_PushEvent(&close);break;}
    case 19:ok=!a->channel_unit_manager;channel_unit_open(a,a->data.channels);ok=ok && !strcmp(a->unit_draft.symbol,"cm2");channel_unit_si(a);ok=ok && channel_display_unit(a,a->data.channels).scale==.02;channel_unit_apply(a);break;
    case 20:ok=a->channel_unit_manager && !a->channel_units.count;channel_unit_close(a);break;
    case 21:
        ok=!a->channel_unit_manager;channel_unit_open(a,a->data.channels);UNIT_KEY(SDLK_SPACE);deadline=ps_clock()+3;break;
    case 22:{struct nk_window *body=nk_window_find(a->unit_task->ui,"unit-body");
        if((!body || !body->popup.active) && ps_clock()<deadline)return;ok=body && body->popup.active;UNIT_KEY(SDLK_TAB);deadline=ps_clock()+3;break;}
    case 23:{struct nk_window *body=nk_window_find(a->unit_task->ui,"unit-body");
        if(body && body->popup.active && ps_clock()<deadline)return;ok=body && !body->popup.active && a->unit_task->focus==UNIT_PRESET;UNIT_KEY(SDLK_SPACE);deadline=ps_clock()+3;break;}
    case 24:{struct nk_window *body=nk_window_find(a->unit_task->ui,"unit-body");
        if((!body || !body->popup.active) && ps_clock()<deadline)return;ok=body && body->popup.active;UNIT_KEY(SDLK_DOWN);deadline=ps_clock()+3;break;}
    case 25:
        if(!nk_sdl_accessibility_is_focused(a->unit_task->ui,NULL,"mm") && ps_clock()<deadline)return;
        ok=nk_sdl_accessibility_is_focused(a->unit_task->ui,NULL,"mm");UNIT_KEY(SDLK_RETURN);deadline=ps_clock()+3;break;
    case 26:
        if(!nk_sdl_accessibility_is_focus_target(a->unit_task->ui,NULL,"Vorgabe") && ps_clock()<deadline)return;
        ok=!strcmp(a->unit_draft.symbol,"mm") && !a->channel_units.count && nk_sdl_accessibility_is_focus_target(a->unit_task->ui,NULL,"Vorgabe");UNIT_KEY(SDLK_SPACE);deadline=ps_clock()+3;break;
    case 27:{struct nk_window *body=nk_window_find(a->unit_task->ui,"unit-body");
        if((!body || !body->popup.active || !nk_sdl_accessibility_is_focused(a->unit_task->ui,NULL,"mm")) && ps_clock()<deadline)return;ok=body && body->popup.active && nk_sdl_accessibility_is_focused(a->unit_task->ui,NULL,"mm");UNIT_KEY(SDLK_ESCAPE);deadline=ps_clock()+3;break;}
    case 28:{struct nk_window *body=nk_window_find(a->unit_task->ui,"unit-body");
        if(body && body->popup.active && ps_clock()<deadline)return;ok=a->channel_unit_manager && body && !body->popup.active;UNIT_KEY(SDLK_ESCAPE);break;}
    case 29:
        ok=!a->channel_unit_manager && a->data.count==3 && a->data.values[0][1]==2 && a->data.values[1][2]==6;
        printf("UNIT TASK: two-real-channels=1 independent-windows=1 typed-symbol-factor=1 retained-drafts=1 settings-cancel-independent=1 si-draft=1 full-data-overflow=1 write-retry=1 os-close=1 popup-tab-escape=1 complete=%d\n",ok);a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Unit task failed at stage %d: channel=%s symbol=%s factor=%s error=%s\n",*stage,a->unit_channel.name,a->unit_draft.symbol,a->channel_unit_scale,a->channel_units_error);*code=1;a->quitting=true;}
    if(a->quitting)printf("UNIT TASK SELF-TEST: %s\n",*code?"FAILED":"PASSED");(*stage)++;
#undef UNIT_FOCUS
#undef UNIT_KEY
#undef UNIT_TEXT
}
