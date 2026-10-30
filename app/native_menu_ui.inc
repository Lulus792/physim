/* Same command definitions as the in-window menus; only presentation changes. */
#ifdef __APPLE__
static bool native_menu_editor_window(const app *a,SDL_WindowID owner) {
    SDL_Window *window=owner?SDL_GetWindowFromID(owner):NULL;
    return window && (window==(a->primary_window?a->primary_window:a->window) || window==a->panels[PS_DOCK_WORKSPACE].window);
}
static bool native_menu_command_enabled(app *a,toolbar_action action,SDL_WindowID owner) {
    if(action==TOOLBAR_SEARCH && owner && a->doc_window && owner==SDL_GetWindowID(a->doc_window))return a->doc_visible;
    if(action==TOOLBAR_SEARCH && native_menu_editor_window(a,owner) && a->tab==4)return true;
    if(!native_menu_editor_window(a,owner) && (action==TOOLBAR_SAVE || action==TOOLBAR_RELOAD || action==TOOLBAR_SEARCH ||
       action==TOOLBAR_REFERENCE || action==TOOLBAR_COMPLETION || action==TOOLBAR_REFERENCE_CLOSE))return false;
    toolbar_entry entries[9];
    for(int menu=1;menu<=6;menu++)for(unsigned i=0,n=toolbar_entries(a,menu,entries);i<n;i++)
        if(entries[i].action==action && entries[i].enabled)return true;
    return false;
}
static void native_menu_shortcut(toolbar_action action,SDL_Keycode *key,SDL_Keymod *modifiers) {
    *key=0;*modifiers=0;
    switch(action) {
    case TOOLBAR_SAVE:*key=SDLK_S;*modifiers=PS_UI_COMMAND_MOD;break;
    case TOOLBAR_SEARCH:*key=SDLK_F;*modifiers=PS_UI_COMMAND_MOD;break;
    case TOOLBAR_LOG:*key=SDLK_L;*modifiers=PS_UI_COMMAND_MOD;break;
    case TOOLBAR_LIBRARY:*key=SDLK_4;*modifiers=PS_UI_COMMAND_MOD;break;
    case TOOLBAR_SETTINGS:*key=SDLK_COMMA;*modifiers=PS_UI_COMMAND_MOD;break;
    case TOOLBAR_HELP:*key=SDLK_F1;break;
    case TOOLBAR_REFERENCE:*key=SDLK_F1;*modifiers=SDL_KMOD_SHIFT;break;
    case TOOLBAR_COMPLETION:*key=SDLK_SPACE;*modifiers=SDL_KMOD_CTRL;break;
    case TOOLBAR_BUILD:*key=SDLK_F5;break;
    default:break;
    }
}
static void native_menu_sync(app *a) {
    if(a->panel_render)return;
    if(!a->native_menu_attempted){a->native_menu_attempted=true;a->native_menu=ps_native_menu_create();}
    if(!a->native_menu)return;
    SDL_Window *focused=SDL_GetKeyboardFocus();SDL_WindowID owner=focused?SDL_GetWindowID(focused):0;
    ps_native_menu_item items[6][9]={0};char titles[6][9][256];ps_native_menu_group groups[6]={0};
    static const char *names[]={"Physim","Datei","Bearbeiten","Ansicht","Build","Hilfe"};
    static const int menus[]={0,1,5,2,6,3};
    for(unsigned group=0;group<6;group++) {
        groups[group].title=names[group];groups[group].items=items[group];groups[group].application=group==0;groups[group].help=group==5;
        toolbar_entry entries[9];unsigned count=group?toolbar_entries(a,menus[group],entries):1;
        if(!group)entries[0]=(toolbar_entry){"Einstellungen …",TOOLBAR_SETTINGS,true};
        for(unsigned i=0;i<count;i++) {
            if(group && entries[i].action==TOOLBAR_SETTINGS)continue;
            size_t slot=groups[group].count++;ps_native_menu_item *item=&items[group][slot];
            SDL_strlcpy(titles[group][slot],entries[i].text,sizeof titles[group][slot]);char *hint=strchr(titles[group][slot],'\t');if(hint)*hint=0;
            item->title=titles[group][slot];item->command=(unsigned)entries[i].action;
            item->enabled=native_menu_command_enabled(a,entries[i].action,owner);
            item->separator_before=(group==1 && (i==3 || i==5)) || (group==3 && i==4);
            native_menu_shortcut(entries[i].action,&item->key,&item->modifiers);
        }
    }
    a->native_menu_active=ps_native_menu_publish(a->native_menu,groups,6,owner);
    if(!a->native_menu_active){ps_native_menu_destroy(a->native_menu);a->native_menu=NULL;}
}
static bool native_menu_input(app *a,const SDL_Event *event) {
    if(!a->native_menu)return false;
    if(ps_native_menu_consumes_key(a->native_menu,event))return true;
    if(event->type==SDL_EVENT_KEY_DOWN) {
        for(unsigned action=0;action<=TOOLBAR_BROWSE;action++) {
            SDL_Keycode key;SDL_Keymod mod;native_menu_shortcut((toolbar_action)action,&key,&mod);
            bool same_mod=true;const SDL_Keymod masks[]={SDL_KMOD_GUI,SDL_KMOD_CTRL,SDL_KMOD_SHIFT,SDL_KMOD_ALT};
            for(unsigned i=0;i<SDL_arraysize(masks);i++)if((bool)(mod&masks[i])!=(bool)(event->key.mod&masks[i]))same_mod=false;
            if(key && event->key.key==key && same_mod && !native_menu_command_enabled(a,(toolbar_action)action,event->key.windowID))return true;
        }
    }
    unsigned command;SDL_WindowID owner;bool shortcut;
    if(!ps_native_menu_event(a->native_menu,event,&command,&owner,&shortcut))return false;
    SDL_Window *focused=SDL_GetKeyboardFocus();SDL_WindowID current=focused?SDL_GetWindowID(focused):0;
    if(owner==current && command<=TOOLBAR_BROWSE && native_menu_command_enabled(a,(toolbar_action)command,owner)) {
        if(shortcut && command==TOOLBAR_HELP) {
            /* Existing F1 preserves the current topic; the menu's Handbuch
             * action explicitly opens the handbook start. */
            open_documentation(a,a->documentation?a->doc_topic:1);
        } else if(command==TOOLBAR_SEARCH && a->doc_window && owner==SDL_GetWindowID(a->doc_window)) {
            a->doc_search_focus=true;a->doc_filter_active=false;
        } else if(command==TOOLBAR_SEARCH && a->tab==4)a->library_search_focus=true;
        else toolbar_dispatch(a,(toolbar_action)command);
    }
    return true;
}
#else
static void native_menu_sync(app *a){(void)a;}
static bool native_menu_input(app *a,const SDL_Event *event){(void)a;(void)event;return false;}
#endif
