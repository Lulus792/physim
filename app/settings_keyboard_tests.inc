/* Actual visible order of Settings A, including its new OS window. */
static void test_settings_keyboard_frame(app *a,bool reading,int *stage,int *code,const char **capture) {
    bool ok=true;static unsigned flag;static double deadline;
#define SET_KEY(key) test_window_key_mod(a->settings_window,key,0)
#define SET_FOCUS(id) do{if(!settings_test_tab_to(a,id,*stage-1,&ok)){--*stage;return;}}while(0)
    switch((*stage)++){
    case 0:
#ifdef __APPLE__
        if(!native_test_keyboard_command(a,TOOLBAR_SETTINGS,stage,&ok))return;
#else
        test_window_key_mod(a->window,SDLK_COMMA,PS_UI_COMMAND_MOD);
#endif
        deadline=ps_clock()+3;break;
    case 1:
        if(!a->settings_open && ps_clock()<deadline){--*stage;return;}
        ok=a->settings_open && a->tab!=6;
        if(reading){ok=ok && a->preferences.ui_size==22 && a->preferences.editor_size==20 && a->preferences.autosave_seconds==120 && a->preferences.theme==PS_THEME_LIGHT;*stage=30;break;}
        SET_FOCUS(0);SET_KEY(SDLK_LEFT);break;
    case 2:ok=a->settings_draft.ui_size==22;SET_FOCUS(4);SET_KEY(SDLK_RIGHT);break;
    case 3:ok=a->settings_draft.theme==PS_THEME_LIGHT;SET_FOCUS(21);SET_KEY(SDLK_SPACE);break;
    case 4:ok=a->settings_group==1;SET_FOCUS(5);for(int i=0;i<4;i++)SET_KEY(SDLK_RIGHT);break;
    case 5:ok=a->settings_draft.editor_size==20;SET_FOCUS(6);SET_KEY(SDLK_LEFT);SET_KEY(SDLK_LEFT);break;
    case 6:ok=a->settings_draft.autosave_seconds==120;SET_FOCUS(22);SET_KEY(SDLK_SPACE);flag=0;break;
    case 7:
        ok=a->settings_group==2;SET_FOCUS(7+(int)flag);SET_KEY(SDLK_SPACE);flag++;
        if(flag<6)--*stage;break;
    case 8:SET_FOCUS(23);SET_KEY(SDLK_SPACE);break;
    case 9:ok=a->settings_group==3;SET_FOCUS(13);SET_KEY(SDLK_SPACE);break;
    case 10:ok=a->settings_draft.view_flags==(PS_VIEW_LOG|PS_VIEW_ORTHOGRAPHIC);SET_FOCUS(18);SET_KEY(SDLK_SPACE);break;
    case 11:ok=a->settings_review && a->preferences.ui_size==16;SET_FOCUS(1);*capture="keyboard-settings-focus.png";SET_KEY(SDLK_RETURN);break;
    case 12:if(a->settings_apply_requested){--*stage;return;}ok=!a->settings_open && a->preferences.ui_size==22 && a->preferences.editor_size==20 && a->preferences.autosave_seconds==120 && a->preferences.theme==PS_THEME_LIGHT && a->preferences.view_flags==(PS_VIEW_LOG|PS_VIEW_ORTHOGRAPHIC);open_settings(a);*stage=30;break;
    case 30:SET_FOCUS(0);SET_KEY(SDLK_RIGHT);break;
    case 31:ok=a->settings_draft.ui_size==16 && a->preferences.ui_size==22;SET_KEY(SDLK_ESCAPE);break;
    case 32:ok=!a->settings_open && a->preferences.ui_size==22;open_settings(a);break;
    case 33:SET_FOCUS(3);SET_KEY(SDLK_SPACE);break;
    case 34:ok=a->settings_draft.ui_size==16 && a->settings_draft.editor_size==12 && a->settings_draft.view_flags==31 && a->settings_draft.theme==PS_THEME_DARK && a->preferences.ui_size==22;SET_KEY(SDLK_ESCAPE);break;
    case 35:ok=!a->settings_open && a->preferences.ui_size==22;a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Settings A keyboard failed at stage %d: focus=%d group=%u open=%d tab=%d draft=%u/%u/%u flags=%u error=%s\n",*stage-1,a->settings_focus,a->settings_group,a->settings_open,a->tab,a->settings_draft.ui_size,a->settings_draft.editor_size,a->settings_draft.autosave_seconds,a->settings_draft.view_flags,a->preferences_error);*code=1;a->quitting=true;}
    if(a->quitting)printf("SETTINGS KEYBOARD SELF-TEST: %s\n",*code?"FAILED":"PASSED");
#undef SET_FOCUS
#undef SET_KEY
}
