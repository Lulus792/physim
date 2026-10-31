static bool settings_test_tab_to(app *a,int id,int stage,bool *ok) {
    static int previous=-1,attempts;if(previous!=stage){previous=stage;attempts=0;}
    if(a->settings_focus==id && a->settings_keyboard)return true;
    if(++attempts>28){*ok=false;return true;}test_window_key_mod(a->settings_window,SDLK_TAB,0);return false;
}
static void settings_test_text(app *a,const char *text) {
    SDL_Event e={0};e.type=SDL_EVENT_TEXT_INPUT;e.text.windowID=SDL_GetWindowID(a->settings_window);e.text.text=text;SDL_PushEvent(&e);
}
static bool settings_test_action_focused(app *a,const char *pane,const char *label) {
    return nk_window_is_active(a->settings_ui,pane) && nk_sdl_accessibility_is_focus_target(a->settings_ui,NULL,label);
}
static void test_settings_search_frame(app *a,bool large,int *stage,int *code,const char **capture) {
    static double deadline;static struct nk_rect reset;bool ok=true;
    const char *query="kein-treffer-zyx987";
#define SEARCH_KEY(key) test_window_key_mod(a->settings_window,key,0)
#define SEARCH_WAIT(condition) do{if(!(condition) && ps_clock()<deadline)return;ok=(condition);}while(0)
    switch(*stage){
    case 0:{ps_preferences p=a->preferences;p.ui_size=large?22:16;p.theme=large?PS_THEME_LIGHT:PS_THEME_DARK;preferences_apply(a,&p);
        open_settings(a);SDL_SetWindowSize(a->settings_window,large?620:1326,large?720:756);SDL_SyncWindow(a->settings_window);deadline=ps_clock()+3;break;}
    case 1:SEARCH_WAIT(SDL_TextInputActive(a->settings_window));settings_test_text(a,query);deadline=ps_clock()+3;break;
    case 2:SEARCH_WAIT(!strcmp(a->settings_search,query));for(unsigned i=0;i<14;i++)ok=ok && !a->settings_fields[i].w;SEARCH_KEY(SDLK_TAB);deadline=ps_clock()+3;break;
    case 3:{
        bool target=settings_test_action_focused(a,"settings-search","Suche löschen");
        SEARCH_WAIT(target);struct nk_rect r=a->settings_focus_bounds[a->settings_focus];
        printf("SETTINGS SEARCH HEADER: focus=%d name=%s y=%.2f body=%.2f target=%d\n",a->settings_focus,settings_focus_name(a->settings_focus),r.y,a->settings_body.y,target);
        ok=ok && r.y+r.h<=a->settings_body.y;SEARCH_KEY(SDLK_SPACE);deadline=ps_clock()+3;break;}
    case 4:SEARCH_WAIT(!*a->settings_search && a->settings_focus==17 && SDL_TextInputActive(a->settings_window));settings_test_text(a,query);deadline=ps_clock()+3;break;
    case 5:SEARCH_WAIT(!strcmp(a->settings_search,query));if(!settings_test_tab_to(a,24,*stage,&ok))return;SEARCH_KEY(SDLK_TAB);deadline=ps_clock()+3;break;
    case 6:{
        bool target=settings_test_action_focused(a,"settings-body","Suche zurücksetzen");
        struct nk_rect r=a->settings_focus_bounds[a->settings_focus],b=a->settings_body;
        SEARCH_WAIT(target && r.y>=b.y && r.y+r.h<=b.y+b.h);reset=r;
        printf("SETTINGS SEARCH RESET: focus=%d name=%s y=%.2f body=%.2f target=%d\n",a->settings_focus,settings_focus_name(a->settings_focus),r.y,b.y,target);
        *capture=large?"settings-search-reset-22.png":"settings-search-reset-16.png";
        test_window_key_mod(a->settings_window,SDLK_TAB,SDL_KMOD_SHIFT);deadline=ps_clock()+3;break;}
    case 7:SEARCH_WAIT(a->settings_focus==24);SEARCH_KEY(SDLK_TAB);deadline=ps_clock()+3;break;
    case 8:SEARCH_WAIT(settings_test_action_focused(a,"settings-body","Suche zurücksetzen"));SEARCH_KEY(SDLK_RETURN);deadline=ps_clock()+3;break;
    case 9:SEARCH_WAIT(!*a->settings_search && a->settings_focus==17 && SDL_TextInputActive(a->settings_window));settings_test_text(a,"SCHRIFT");deadline=ps_clock()+3;break;
    case 10:SEARCH_WAIT(!strcmp(a->settings_search,"SCHRIFT"));ok=ok && a->settings_fields[1].w>0 && a->settings_fields[2].w>0;
        test_window_key_mod(a->settings_window,SDLK_A,PS_UI_COMMAND_MOD);break;
    case 11:settings_test_text(a,query);deadline=ps_clock()+3;break;
    case 12:SEARCH_WAIT(!strcmp(a->settings_search,query));if(!settings_test_tab_to(a,24,*stage,&ok))return;SEARCH_KEY(SDLK_TAB);deadline=ps_clock()+3;break;
    case 13:{struct nk_rect r=a->settings_focus_bounds[a->settings_focus],b=a->settings_body;
        SEARCH_WAIT(settings_test_action_focused(a,"settings-body","Suche zurücksetzen") && !a->settings_focus_scroll && r.y>=b.y && r.y+r.h<=b.y+b.h);
        reset=r;printf("SETTINGS SEARCH POINTER: y=%.2f bottom=%.2f body=%.2f/%.2f settled=%d\n",r.y,r.y+r.h,b.y,b.y+b.h,!a->settings_focus_scroll);test_window_mouse(a->settings_window,reset,true);break;}
    case 14:test_window_mouse(a->settings_window,reset,false);deadline=ps_clock()+3;break;
    case 15:SEARCH_WAIT(!*a->settings_search && a->settings_focus==17 && SDL_TextInputActive(a->settings_window));
        /* In the two-column large-font layout, scroll category21 behind the
         * fixed search bar. The real pointer must only reach the clear action. */
        if(large){struct nk_window *body=nk_window_find(a->settings_ui,"settings-body");ok=body!=NULL;if(body)body->scrollbar.y=100;}
        settings_test_text(a,query);deadline=ps_clock()+3;break;
    case 16:{struct nk_rect r=a->settings_focus_bounds[26];
        SEARCH_WAIT(!strcmp(a->settings_search,query) && r.w>0 && r.h>0 && r.y+r.h<=a->settings_body.y && r.x+r.w<=a->settings_frame.w);
        reset=r;printf("SETTINGS SEARCH CLEAR POINTER: x=%.2f y=%.2f width=%.2f height=%.2f\n",r.x,r.y,r.w,r.h);test_window_mouse(a->settings_window,reset,true);break;}
    case 17:test_window_mouse(a->settings_window,reset,false);deadline=ps_clock()+3;break;
    case 18:SEARCH_WAIT(!*a->settings_search && a->settings_focus==17 && SDL_TextInputActive(a->settings_window));settings_test_text(a,"Sicherung");deadline=ps_clock()+3;break;
    case 19:SEARCH_WAIT(!strcmp(a->settings_search,"Sicherung"));ok=ok && a->settings_fields[3].w>0 && !settings_changes(a);
        printf("SETTINGS SEARCH: distinct-actions=1 header-keyboard=1 reset-tab-shift-tab-enter=1 pointer-both=1 text-input-return=1 unchanged-draft=1 size=%u complete=%d\n",a->preferences.ui_size,ok);a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Settings search failed stage %d: focus=%d name=%s keyboard=%d search=%s\n",*stage,a->settings_focus,settings_focus_name(a->settings_focus),a->settings_keyboard,a->settings_search);*code=1;a->quitting=true;}
    if(a->quitting)printf("SETTINGS SEARCH SELF-TEST: %s\n",*code?"FAILED":"PASSED");(*stage)++;
#undef SEARCH_KEY
#undef SEARCH_WAIT
}
static void test_settings_window_frame(app *a,bool large,int *stage,int *code,const char **capture) {
    bool ok=true;static SDL_WindowID id;static unsigned theme,main_side;static double deadline;
    switch(*stage){
    case 0:{ps_preferences p=a->preferences;p.ui_size=large?22:16;p.theme=large?PS_THEME_LIGHT:PS_THEME_DARK;preferences_apply(a,&p);theme=p.theme;
        a->tab=0;open_settings(a);id=SDL_GetWindowID(a->settings_window);SDL_SetWindowSize(a->settings_window,large?620:1326,large?720:756);SDL_SyncWindow(a->settings_window);deadline=ps_clock()+2;break;}
    case 1:
        if(!SDL_TextInputActive(a->settings_window) && ps_clock()<deadline)return;
        ok=a->settings_open && a->tab==0 && id!=SDL_GetWindowID(a->window) && a->settings_ui!=a->ui && a->settings_graphics!=a->graphics &&
            a->settings_focus==17 && SDL_TextInputActive(a->settings_window) && !settings_changes(a);
        *capture=large?"settings-a-small-22.png":"settings-a-wide-16.png";settings_test_text(a,"SCHRIFT");break;
    case 2:
        if(!*a->settings_search && ps_clock()<deadline)return;
        ok=!strcmp(a->settings_search,"SCHRIFT") && a->settings_fields[1].w>0 && a->settings_fields[2].w>0 && !a->settings_fields[3].w;
        open_settings(a);ok=ok && id==SDL_GetWindowID(a->settings_window) && !strcmp(a->settings_search,"SCHRIFT");
        test_window_key_mod(a->window,SDLK_2,PS_UI_COMMAND_MOD);deadline=ps_clock()+2;break;
    case 3:
        if(a->tab==1 && !a->laboratory_active && ps_clock()<deadline)return;
        ok=a->tab==1 && a->laboratory_active && a->settings_open && id==SDL_GetWindowID(a->settings_window);
        /* Ordered user gestures: let Select All reach the edit before the
         * following Backspace, rather than coalescing both keys in one frame. */
        test_window_key_mod(a->settings_window,SDLK_A,PS_UI_COMMAND_MOD);*stage=29;break;
    case 30:
        test_window_key_mod(a->settings_window,SDLK_BACKSPACE,0);deadline=ps_clock()+2;*stage=3;break;
    case 4:
        if(*a->settings_search && ps_clock()<deadline)return;
        ok=!*a->settings_search;if(ok && !settings_test_tab_to(a,4,*stage,&ok))return;
        if(ok)test_window_key_mod(a->settings_window,SDLK_RIGHT,0);break;
    case 5:
        ok=a->settings_draft.theme!=(uint32_t)theme && a->preferences.theme==theme && settings_changes(a)==(1u<<PS_SETTING_THEME);
        if(ok && !settings_test_tab_to(a,18,*stage,&ok))return;
        if(ok)test_window_key_mod(a->settings_window,SDLK_SPACE,0);deadline=ps_clock()+2;break;
    case 6:
        if(a->settings_review && a->settings_categories[0].w>0 && ps_clock()<deadline)return;
        ok=a->settings_review && a->preferences.theme==theme && a->tab==1 && !a->settings_categories[0].w;
        if(!large)*capture="settings-a-review-16.png";
        test_window_key_mod(a->settings_window,SDLK_ESCAPE,0);break;
    case 7:
        ok=a->settings_open && !a->settings_review && a->settings_draft.theme!=(uint32_t)theme;
        test_window_key_mod(a->settings_window,SDLK_ESCAPE,0);break;
    case 8:
        ok=!a->settings_open && a->tab==1 && a->preferences.theme==theme;open_settings(a);break;
    case 9:{ok=a->settings_open && a->settings_draft.theme==theme && !settings_changes(a);
        SDL_Event close={0};close.type=SDL_EVENT_WINDOW_CLOSE_REQUESTED;close.window.windowID=SDL_GetWindowID(a->settings_window);SDL_PushEvent(&close);break;}
    case 10:
        ok=!a->settings_open && a->tab==1 && a->preferences.theme==theme;
        main_side=a->preferences.sidebar_width;open_settings(a);break;
    case 11:
        if(!settings_test_tab_to(a,23,*stage,&ok))return;
        test_window_key_mod(a->settings_window,SDLK_SPACE,0);break;
    case 12:
        ok=a->settings_group==3;
        if(ok && !settings_test_tab_to(a,14,*stage,&ok))return;
        test_window_key_mod(a->settings_window,SDLK_A,PS_UI_COMMAND_MOD);settings_test_text(a,"139");deadline=ps_clock()+2;break;
    case 13:
        if(strcmp(a->settings_numbers[0],"139") && ps_clock()<deadline)return;
        ok=!strcmp(a->settings_numbers[0],"139") && (a->settings_invalid_numbers&1) && a->settings_draft.sidebar_width==139 && a->preferences.sidebar_width==main_side;
        if(ok && !settings_test_tab_to(a,1,*stage,&ok))return;
        test_window_key_mod(a->settings_window,SDLK_RETURN,0);break;
    case 14:
        if(a->settings_apply_requested)return;
        ok=a->settings_open && *a->preferences_error && !strcmp(a->settings_numbers[0],"139") && a->preferences.sidebar_width==main_side;
        if(ok && !settings_test_tab_to(a,14,*stage,&ok))return;
        test_window_key_mod(a->settings_window,SDLK_A,PS_UI_COMMAND_MOD);settings_test_text(a,"300");deadline=ps_clock()+2;break;
    case 15:
        if(strcmp(a->settings_numbers[0],"300") && ps_clock()<deadline)return;
        ok=!strcmp(a->settings_numbers[0],"300") && !a->settings_invalid_numbers && a->settings_draft.sidebar_width==300 && a->preferences.sidebar_width==main_side;
        if(ok && !settings_test_tab_to(a,1,*stage,&ok))return;
        test_window_key_mod(a->settings_window,SDLK_RETURN,0);break;
    case 16:{
        if(a->settings_apply_requested)return;
        ps_preferences saved;ok=!a->settings_open && a->preferences.sidebar_width==main_side && a->preferences.theme==theme && ps_preferences_read(a->preferences_path,&saved)==PS_OK && saved.sidebar_width==300;
        open_settings(a);break;}
    case 17:{
        ok=a->settings_open && a->settings_draft.sidebar_width==300 && !strcmp(a->settings_numbers[0],"300") && !settings_changes(a);
        SDL_Event close={0};close.type=SDL_EVENT_WINDOW_CLOSE_REQUESTED;close.window.windowID=SDL_GetWindowID(a->settings_window);SDL_PushEvent(&close);break;}
    case 18:
        ok=!a->settings_open && a->tab==1 && a->preferences.theme==theme && a->preferences.sidebar_width==main_side;
        printf("SETTINGS A WINDOW: separate-window=%u main-tab=%d search-preserved=1 draft-only-theme=1 review-escape=1 os-close=1 typed-size=300 range-retry=1 parallel-sidebar=%u size=%u complete=%d\n",id,a->tab,main_side,a->preferences.ui_size,ok);a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Settings A window failed at stage %d: focus=%d keyboard=%d search=%s group=%u open=%d tab=%d error=%s\n",*stage,a->settings_focus,a->settings_keyboard,a->settings_search,a->settings_group,a->settings_open,a->tab,a->preferences_error);*code=1;a->quitting=true;}
    if(a->quitting)printf("SETTINGS A WINDOW SELF-TEST: %s\n",*code?"FAILED":"PASSED");(*stage)++;
}
