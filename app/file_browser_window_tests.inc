/* Real independent native A windows and routed gestures. The late completion
 * below is deliberately injected; physical keys/readers are separate checks. */
static bool browser_test_native_distinct(SDL_Window *one,SDL_Window *two) {
    if(!one || !two || one==two)return false;
#ifdef _WIN32
    void *a=SDL_GetPointerProperty(SDL_GetWindowProperties(one),SDL_PROP_WINDOW_WIN32_HWND_POINTER,NULL);
    void *b=SDL_GetPointerProperty(SDL_GetWindowProperties(two),SDL_PROP_WINDOW_WIN32_HWND_POINTER,NULL);
#else
    Sint64 a=SDL_GetNumberProperty(SDL_GetWindowProperties(one),SDL_PROP_WINDOW_X11_WINDOW_NUMBER,0);
    Sint64 b=SDL_GetNumberProperty(SDL_GetWindowProperties(two),SDL_PROP_WINDOW_X11_WINDOW_NUMBER,0);
#endif
    return a && b && a!=b;
}
static void browser_window_test_frame(app *a,const char *mode,int *stage,int *code) {
    static unsigned settle,font_stage,wide_stage;static bool header_captured,body_captured,main_edited,detached_edited,prior_log;static browser_dialog *parent,*main_picker;static ps_dialog_ticket retired,current;
    static char target[4096];static SDL_WindowID main_id,parent_id;bool okay=true;
    if(settle++<2)return;settle=0;
    for(unsigned i=1;i<PS_DIALOG_REQUEST_ROUTES;i++){browser_dialog *d=a->browser_popups[i];
        if(d && d->visible && (!d->window || browser_busy(d) || d->cache_dirty))return;}
#define PICKER_FOCUS(d,id) do{if((d)->focus!=(id) || !(d)->keyboard || (d)->reveal){test_window_key_mod((d)->window,SDLK_TAB,0);return;}}while(0)
#define PARENT_FOCUS(id) do{if(!manager_test_focus(a,id))return;}while(0)
    switch(*stage){
    case 0:
        if(a->tab!=0){test_window_key(a->window,SDLK_1);return;}
        open_project_manager(a,"Datei");break;
    case 1:PARENT_FOCUS(7);manager_test_key(a,SDLK_RETURN,0);break;
    case 2:PARENT_FOCUS(1);manager_test_key(a,SDLK_RETURN,0);break;
    case 3:
        parent=a->browser_popups[PS_DIALOG_MANAGER_PARENT];okay=parent && parent->visible && parent->source_id==SDL_GetWindowID(a->manager_window);
        if(okay){parent_id=SDL_GetWindowID(parent->window);retired=parent->ticket;test_window_key_mod(parent->window,SDLK_F,PS_UI_COMMAND_MOD);}break;
    case 4:browser_test_type(a,"runs");break;
    case 5:okay=parent->count==1 && !strcmp(parent->query,"runs");toolbar_dispatch(a,TOOLBAR_OPEN);break;
    case 6:
        main_picker=a->browser_popups[PS_DIALOG_OPEN_FOLDER];okay=main_picker && main_picker->visible && parent->visible &&
            main_picker->source_id==SDL_GetWindowID(a->window) && main_picker->ui!=parent->ui && main_picker->worker!=parent->worker &&
            browser_test_native_distinct(main_picker->window,parent->window) && browser_test_native_distinct(main_picker->window,a->window) &&
            browser_test_native_distinct(parent->window,a->manager_window);
        if(okay){main_id=SDL_GetWindowID(main_picker->window);current=main_picker->ticket;test_window_key_mod(main_picker->window,SDLK_F,PS_UI_COMMAND_MOD);}break;
    case 7:browser_test_type(a,"assets");break;
    case 8:okay=a->tab==0 && main_picker->count==1 && !strcmp(main_picker->query,"assets") && !strcmp(parent->query,"runs");test_mouse(a,a->navigation_bounds[1],true);break;
    case 9:test_mouse(a,a->navigation_bounds[1],false);break;
    case 10:
        okay=a->tab==(main_edited?8:1) && main_picker->visible && parent->visible && a->project_manager;
        if(!okay)break;
        if(!main_edited){char path[4096];ps_browser_child(browser_test_root,"main.c",path);preview_workspace_path(a,path);*stage=29;break;}
        toolbar_dispatch(a,TOOLBAR_OPEN);choose_workspace_path(a,PS_DIALOG_MANAGER_PARENT);
        okay &= main_picker->ticket.version==current.version && parent->ticket.version==retired.version &&
            SDL_GetWindowID(main_picker->window)==main_id && SDL_GetWindowID(parent->window)==parent_id &&
            !strcmp(main_picker->query,"assets") && !strcmp(parent->query,"runs");
        PICKER_FOCUS(parent,11);break;
    case 11:
        ps_browser_child(browser_test_root,"runs",target);okay=parent->selected>=0 && !strcmp(parent->listing.entries[parent->selected].path,target);
        PICKER_FOCUS(parent,20);test_window_key_mod(parent->window,SDLK_RETURN,0);break;
    case 12:
        okay=!parent->visible && !parent->window && main_picker->visible && !strcmp(main_picker->query,"assets") &&
            !strcmp(a->manager_parent,target) && !ps_dialog_request_find(&a->dialog_requests,retired);
        browser_complete(a,retired,browser_test_root);test_window_key_mod(main_picker->window,SDLK_ESCAPE,0);break;
    case 13:
        okay=!main_picker->visible && !main_picker->window && !strcmp(a->manager_parent,target) && a->project_manager;
        toolbar_dispatch(a,TOOLBAR_OPEN);break;
    case 14:
        okay=main_picker->visible && main_picker->ticket.version>current.version;
        if(okay){current=main_picker->ticket;SDL_Event e={0};e.type=SDL_EVENT_WINDOW_CLOSE_REQUESTED;e.window.windowID=SDL_GetWindowID(main_picker->window);SDL_PushEvent(&e);}break;
    case 15:
        okay=!main_picker->visible && !main_picker->window && a->project_manager;
        toolbar_dispatch(a,TOOLBAR_OPEN);choose_workspace_path(a,PS_DIALOG_MANAGER_PARENT);break;
    case 16:{
        okay=parent->visible && main_picker->visible && parent->ticket.version>retired.version;retired=parent->ticket;
        SDL_Event e={0};e.type=SDL_EVENT_WINDOW_CLOSE_REQUESTED;e.window.windowID=SDL_GetWindowID(a->manager_window);SDL_PushEvent(&e);break;
    }
    case 17:
        okay=!a->project_manager && !a->manager_window && !parent->visible && !parent->window && main_picker->visible &&
            ps_dialog_request_find(&a->dialog_requests,main_picker->ticket) && !ps_dialog_request_find(&a->dialog_requests,retired);
        browser_complete(a,retired,browser_test_root);SDL_SetWindowSize(main_picker->window,strstr(mode,"small")?320:740,740);
        test_window_key_mod(main_picker->window,SDLK_UP,SDL_KMOD_ALT);break;
    case 18:{
        if(!header_captured){if(strstr(mode,"small")){char path[4096];join(path,sizeof path,browser_test_root,"../picker-window-start.png");SDL_strlcpy(main_picker->capture,path,sizeof main_picker->capture);}header_captured=true;}
        if(!body_captured && strstr(mode,"small")){
            PICKER_FOCUS(main_picker,8);
            printf("{\"kind\":\"independent_picker_places\",\"scale\":%.4f,\"font_height\":%.4f,\"items\":[",a->preferences.ui_size/16.f,main_picker->fonts[4].height);
            for(unsigned k=0;k<3;k++){struct nk_rect b=main_picker->controls[8+k],ink=main_picker->place_ink[k];
                printf("%s{\"button\":[%.4f,%.4f,%.4f,%.4f],\"ink\":[%.4f,%.4f,%.4f,%.4f]}",k?",":"",b.x,b.y,b.w,b.h,ink.x,ink.y,ink.w,ink.h);}
            puts("]}");
            if(strstr(mode,"large")){char path[4096];join(path,sizeof path,browser_test_root,"../picker-window-places.png");SDL_strlcpy(main_picker->capture,path,sizeof main_picker->capture);}
            body_captured=true;return;
        }
        if(main_picker->selected<0){PICKER_FOCUS(main_picker,11);return;}
        PICKER_FOCUS(main_picker,20);int w,h;SDL_GetWindowSize(main_picker->window,&w,&h);
        for(int id=19;id<=20;id++){struct nk_rect r=main_picker->controls[id];okay &= r.w>0 && r.h>0 && r.x>=0 && r.x+r.w<=w+.05f && r.y>=0 && r.y+r.h<=h+.05f;}
        okay &= main_picker->list_bounds.w>0 && main_picker->preview_bounds.w>0;
        if(!okay)break;
        if(font_stage<2){ps_preferences p=a->preferences;p.ui_size=font_stage==0?(p.ui_size==16?22:16):(strstr(mode,"large")?22:16);preferences_apply(a,&p);font_stage++;return;}
        if(!wide_stage){if(strstr(mode,"small")){char path[4096];join(path,sizeof path,browser_test_root,"../picker-window.png");SDL_strlcpy(main_picker->capture,path,sizeof main_picker->capture);}wide_stage=1;return;}
        if(wide_stage==1){SDL_SetWindowSize(main_picker->window,1700,1000);wide_stage=2;return;}
        float limit=fminf(1180*a->preferences.ui_size/16.f,1700);
        okay &= fabsf(main_picker->bounds.w-limit)<.05f && fabsf(main_picker->bounds.x-(1700-limit)/2)<.05f;
        struct nk_window *view=nk_window_find(main_picker->ui,"Dateiauswahl");
        if(main_picker->content_height+2<=1000)okay &= view && !view->scrollbar.y;
        test_window_key_mod(main_picker->window,SDLK_ESCAPE,0);break;
    }
    case 19:
        okay=!main_picker->visible && !main_picker->window && !a->dialog_pending;
        open_project_manager(a,"Datei");choose_workspace_path(a,PS_DIALOG_MANAGER_PARENT);toolbar_dispatch(a,TOOLBAR_OPEN);break;
    case 20:
        okay=parent->visible && main_picker->visible;
        /* Complete the real main-window folder action while B's parent
         * selection stays independently open. */
        test_window_key_mod(main_picker->window,SDLK_UP,SDL_KMOD_ALT);*stage=40;break;
    case 21:
        okay=!main_picker->visible && !main_picker->window && parent->visible && parent->window && a->project_manager &&
            ps_dialog_request_find(&a->dialog_requests,parent->ticket) && !strcmp(a->workspace,target) &&
            !strcmp(a->manager_parent,parent->location);
        printf("{\"kind\":\"independent_picker_windows\",\"native_handles_distinct\":true,\"main_navigation\":true,\"main_edit_and_save\":%s,\"detached_input_and_return\":%s,\"independent_results\":%s,\"reinvoke_preserves_form\":true,\"parent_close_scoped\":true,\"main_change_scoped\":%s,\"live_font_cycles\":%u,\"wide_resize_stage\":%u}\n",main_edited?"true":"false",detached_edited?"true":"false",okay?"true":"false",okay?"true":"false",font_stage,wide_stage);
        puts(okay?"INDEPENDENT PICKER WINDOWS SELF-TEST: PASSED":"INDEPENDENT PICKER WINDOWS SELF-TEST: FAILED");a->quitting=true;break;
    case 30:
        okay=a->tab==8 && active_document(a) && main_picker->visible && parent->visible && a->source_editor_bounds.w>0;
        if(okay)test_mouse(a,a->source_editor_bounds,true);break;
    case 31:test_mouse(a,a->source_editor_bounds,false);break;
    case 32:{
        test_window_key_mod(a->window,SDLK_END,PS_UI_COMMAND_MOD);SDL_Event e={0};e.type=SDL_EVENT_TEXT_INPUT;
        e.text.windowID=SDL_GetWindowID(a->window);e.text.text="\n// Main window edit λ\n";SDL_PushEvent(&e);break;
    }
    case 33:{
        workspace_document *doc=active_document(a);okay=doc && doc->dirty && strstr(nk_str_get_const(&doc->edit.string),"// Main window edit λ") &&
            main_picker->visible && parent->visible && !strcmp(main_picker->query,"assets") && !strcmp(parent->query,"runs");
        if(okay)test_window_key(a->window,SDLK_S);break;
    }
    case 34:{
        workspace_document *doc=active_document(a);size_t bytes=0;char *stored=doc?SDL_LoadFile(doc->file.path,&bytes):NULL;
        okay=doc && !doc->dirty && stored && bytes>53 && strstr(stored,"// Main window edit λ") && main_picker->visible && parent->visible;
        SDL_free(stored);main_edited=okay;prior_log=a->show_log;if(!prior_log)toolbar_dispatch(a,TOOLBAR_LOG);break;
    }
    case 35:okay=panel_window_open(a,PS_DOCK_LOG);break;
    case 36:{
        ps_panel_window *p=&a->panels[PS_DOCK_LOG];okay=p->detached && browser_test_native_distinct(p->window,main_picker->window) &&
            browser_test_native_distinct(p->window,parent->window) && p->ui!=main_picker->ui && a->protocol_view.search.w>0;
        if(okay)test_window_mouse(p->window,a->protocol_view.search,true);break;
    }
    case 37:test_window_mouse(a->panels[PS_DOCK_LOG].window,a->protocol_view.search,false);break;
    case 38:{
        SDL_Event e={0};e.type=SDL_EVENT_TEXT_INPUT;e.text.windowID=SDL_GetWindowID(a->panels[PS_DOCK_LOG].window);e.text.text="Parallelfenster λ";SDL_PushEvent(&e);break;
    }
    case 39:{
        okay=!strcmp(a->protocol_view.query,"Parallelfenster λ") && main_picker->visible && parent->visible &&
            !strcmp(main_picker->query,"assets") && !strcmp(parent->query,"runs");
        SDL_Event e={0};e.type=SDL_EVENT_WINDOW_CLOSE_REQUESTED;e.window.windowID=SDL_GetWindowID(a->panels[PS_DOCK_LOG].window);SDL_PushEvent(&e);break;
    }
    case 40:
        okay=!panel_is_detached(a,PS_DOCK_LOG) && main_picker->visible && parent->visible && !strcmp(a->protocol_view.query,"Parallelfenster λ");
        detached_edited=okay;if(!prior_log)toolbar_dispatch(a,TOOLBAR_LOG);*stage=9;break;
    case 41:test_window_key_mod(main_picker->window,SDLK_F,PS_UI_COMMAND_MOD);break;
    case 42:browser_test_type(a,"assets");break;
    case 43:
        okay=!strcmp(main_picker->query,"assets") && main_picker->count==1;
        PICKER_FOCUS(main_picker,11);break;
    case 44:
        ps_browser_child(browser_test_root,"assets",target);
        okay=main_picker->selected>=0 && !strcmp(main_picker->listing.entries[main_picker->selected].path,target);
        PICKER_FOCUS(main_picker,20);test_window_key_mod(main_picker->window,SDLK_RETURN,0);*stage=20;break;
    }
    if(!okay){fprintf(stderr,"Independent picker stage%d failed: tab=%d font_stage=%u parent=%d main=%d\n",*stage,a->tab,font_stage,parent?parent->visible:0,main_picker?main_picker->visible:0);*code=1;a->quitting=true;}
    (*stage)++;
#undef PICKER_FOCUS
#undef PARENT_FOCUS
}
