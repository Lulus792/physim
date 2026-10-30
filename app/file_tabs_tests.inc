static bool file_tabs_test_start(app *a,const char *root) {
    join(a->project_input,sizeof a->project_input,root,"project");a->example=0;new_project(a);
    if(!a->loaded)return false;
    char folder[4096];join(folder,sizeof folder,a->project,"models");
    if(!ps_make_directory(folder))return false;
    for(unsigned i=0;i<WORKSPACE_DOCUMENTS;i++) {
        char path[4096],name[64];snprintf(name,sizeof name,"notes-%02u.c",i);
        join(path,sizeof path,a->project,name);
        if(!document_test_write(path,"// α\nps_vl\n"))return false;
        if(i<3)preview_workspace_path(a,path);
    }
    SDL_SetWindowSize(a->window,1080,740);
    ps_dock_hide(&a->preferences.dock,PS_DOCK_INSPECTOR);
    refresh_workspace_entries(a);return a->document_count==3;
}
static void file_tabs_test_frame(app *a,int *stage,int *code,const char **capture) {
    static double ready;static unsigned repeats;
#ifdef __APPLE__
    static uint32_t experiment_crc,analysis_crc;
#endif
    bool ok=true;
    switch((*stage)++) {
    case 0:ready=ps_clock()+.2;break;
    case 1:
        if(ps_clock()<ready){--*stage;return;}
        ok=a->file_tab_bounds[0].w>0 && a->file_tab_bounds[4].w>0;
        file_tab_select(a,2);ok &= document_test_text(a,"edited α\n");
        a->documents[0].edit.cursor=4;a->documents[0].edit.select_start=a->documents[0].edit.select_end=4;
        *capture="file-tabs.bmp";break;
    case 2:test_mouse(a,a->file_tab_bounds[3],true);break;
    case 3:test_mouse(a,a->file_tab_bounds[3],false);break;
    case 4:
        if(a->completion_focus_edit){--*stage;return;}
        ok=a->tab==8 && a->document_active==1 && a->documents[1].edit.active && a->documents[0].dirty && a->documents[0].edit.cursor==4 && a->editor_caret.w==1;
        test_mouse(a,a->file_tab_bounds[2],true);break;
    case 5:test_mouse(a,a->file_tab_bounds[2],false);break;
    case 6:ok=a->document_active==0;test_mouse(a,a->file_tab_bounds[2],true);break;
    case 7: {
        struct nk_rect b=a->file_tab_bounds[3];test_plot_motion(a,b.x+b.w*.8f,b.y+b.h/2);break;
    }
    case 8: {
        struct nk_rect b=a->file_tab_bounds[2];test_mouse(a,b,false);break;
    }
    case 9:
        if(a->file_tab_drag && !nk_input_is_mouse_down(&a->ui->input,NK_BUTTON_LEFT)){--*stage;return;}
        ok=a->file_tab_order[2]==3 && a->file_tab_order[3]==2 && !a->dock_drag && !a->file_tab_drag;
        test_mouse(a,a->file_tab_closes[2],true);break;
    case 10:test_mouse(a,a->file_tab_closes[2],false);break;
    case 11:
        if(a->document_bounds[5].w<=0){--*stage;return;}
        ok=a->document_pending==1 && a->document_count==3;test_mouse(a,a->document_bounds[5],true);break;
    case 12:test_mouse(a,a->document_bounds[5],false);break;
    case 13:ok=!a->document_pending && a->documents[0].dirty;test_key(a,SDLK_S);break;
    case 14:break;
    case 15:ok=!a->documents[0].dirty;test_mouse(a,a->file_tab_closes[2],true);break;
    case 16:test_mouse(a,a->file_tab_closes[2],false);break;
    case 17:ok=a->document_count==2 && !a->document_pending;test_mouse(a,a->file_tab_closes[0],true);break;
    case 18:test_mouse(a,a->file_tab_closes[0],false);break;
    case 19:
        if(a->source_tab_closed[0] && a->file_tab_bounds[0].w>0){--*stage;return;}
        ok=a->source_tab_closed[0] && a->file_tab_bounds[0].w==0;
        file_tab_select(a,0);break;
    case 20:
        ok=!a->source_tab_closed[0] && a->file_tab_bounds[0].w>0;
        for(unsigned i=0;i<WORKSPACE_DOCUMENTS;i++) {
            char path[4096],name[64];snprintf(name,sizeof name,"notes-%02u.c",i);join(path,sizeof path,a->project,name);preview_workspace_path(a,path);
        }
        break;
    case 21:ok=a->document_count==WORKSPACE_DOCUMENTS && a->file_tab_bounds[FILE_TABS-1].w>0;test_window_key_mod(a->window,SDLK_TAB,SDL_KMOD_CTRL);break;
    case 22:
        if(file_tab_active(a)==0 && a->file_tab_bounds[0].w<=0){--*stage;return;}
        ok=file_tab_active(a)==0 && a->file_tab_bounds[0].w>0;test_window_key_mod(a->window,SDLK_TAB,SDL_KMOD_CTRL|SDL_KMOD_SHIFT);break;
    case 23:ok=file_tab_active(a)==FILE_TABS-1;test_mouse(a,a->source_editor_bounds,true);break;
    case 24:test_mouse(a,a->source_editor_bounds,false);break;
    case 25: {
        workspace_document *doc=active_document(a);ok=doc && doc->edit.active;
        if(ok){ok=set_editor_text(&doc->edit,"// α\nps_v",strlen("// α\nps_v"));doc->edit.cursor=doc->edit.select_start=doc->edit.select_end=nk_str_len(&doc->edit.string);}
        SDL_Event event={0};event.type=SDL_EVENT_TEXT_INPUT;event.text.windowID=SDL_GetWindowID(a->window);event.text.text="l";SDL_PushEvent(&event);break;
    }
    case 26:break;
    case 27:
        ok=a->completion_active && a->completion_list_bounds.y>=a->editor_caret.y+a->editor_caret.h && a->completion_list_bounds.y<a->editor_caret.y+40;
        *capture="inline-completion.bmp";test_window_key_mod(a->window,SDLK_ESCAPE,0);break;
    case 28:ok=!a->completion_active;test_key(a,SDLK_S);break;
    case 29:break;
    case 30:
        a->tab=0;a->workspace_disclosure=NK_MAXIMIZED;
        ok=workspace_tree_test_target(a,"models",0);test_plot_motion(a,a->workspace_tree_test_bounds.x+15,a->workspace_tree_test_bounds.y+12);ready=ps_clock()+.1;break;
    case 31:if(ps_clock()<ready){--*stage;return;}test_mouse(a,a->workspace_tree_test_bounds,true);break;
    case 32:test_mouse(a,a->workspace_tree_test_bounds,false);break;
    case 33:break;
    case 34: {
        size_t index=workspace_tree_test_find(a,"models",0);ok=index!=SIZE_MAX && a->workspace_tree.entries[index].expanded==(repeats%2==0);
        if(++repeats<6)*stage=31;else *stage=40;break;
    }
#ifdef __APPLE__
    case 40:
        a->show_search=false;
        if(!native_test_command(a,TOOLBAR_SEARCH,stage,&ok))return;
        experiment_crc=ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
        analysis_crc=ps_crc32((const unsigned char*)nk_str_get_const(&a->analysis.string),(size_t)nk_str_len_char(&a->analysis.string));
        ready=ps_clock()+2;break;
    case 41:
        if(!native_test_model_wait(a->show_search,stage,ready))return;
        ok=a->show_search && native_test_shell(a) && native_test_stroke(a->window,"f",3,((uintptr_t)1<<20));
        ready=ps_clock()+2;break;
    case 42:
        if(!native_test_model_wait(a->show_search,stage,ready))return;
        ok=a->show_search && !a->job.running && native_test_track_menu(a,TOOLBAR_BUILD,NULL,0,true);
        *stage=49;break;
    case 49:
        ok=!a->job.running && native_test_shell(a);
        if(!native_test_command(a,TOOLBAR_SAVE,stage,&ok))return;
        *stage=52;break;
    case 52: {
        ok=native_test_shell(a) && !a->job.running;
        ps_preferences p=a->preferences;p.theme=PS_THEME_LIGHT;preferences_apply(a,&p);ready=ps_clock()+2;break;
    }
    case 53:
        if(!native_test_focus_wait(a,a->window,stage,ready))return;
        ok=a->preferences.theme==PS_THEME_LIGHT && native_test_track_menu(a,TOOLBAR_SAVE,NULL,0,true);
        *stage=55;break;
    case 55: {
        ps_preferences p=a->preferences;p.theme=PS_THEME_HIGH_CONTRAST;preferences_apply(a,&p);ready=ps_clock()+2;break;
    }
    case 56:
        if(!native_test_focus_wait(a,a->window,stage,ready))return;
        ok=a->preferences.theme==PS_THEME_HIGH_CONTRAST && native_test_track_menu(a,TOOLBAR_SAVE,NULL,0,true);break;
    case 57:
        ok=native_test_shell(a) && !a->job.running &&
            experiment_crc==ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string)) &&
            analysis_crc==ps_crc32((const unsigned char*)nk_str_get_const(&a->analysis.string),(size_t)nk_str_len_char(&a->analysis.string));
        if(ok)puts("FILE TABS NATIVE MENU: search=1 command-f=1 build-cancel=1 save=1 light-contrast-cancel=1 sources-preserved=1");
        break;
#else
    case 40:a->show_search=false;test_mouse(a,a->toolbar_bounds[4],true);break;
    case 41:test_mouse(a,a->toolbar_bounds[4],false);break;
    case 42:
        ok=a->toolbar_menu==5;
        *capture="editor-edit-menu.bmp";
        test_mouse(a,nk_rect(a->toolbar_popup_bounds.x+60,a->toolbar_popup_bounds.y+20,4,4),true);break;
    case 43:test_mouse(a,nk_rect(a->toolbar_popup_bounds.x+60,a->toolbar_popup_bounds.y+20,4,4),false);break;
    case 44:ok=a->show_search && !a->toolbar_menu;test_key(a,SDLK_F);break;
    case 45:test_mouse(a,a->toolbar_bounds[5],true);break;
    case 46:test_mouse(a,a->toolbar_bounds[5],false);break;
    case 47:
        ok=a->toolbar_menu==6 && a->editor_build_bounds.w>0;
        *capture="editor-build-menu.bmp";test_window_key_mod(a->window,SDLK_UP,0);break;
    case 48:
        ok=a->toolbar_keyboard && a->toolbar_keyboard_item==1;
        test_window_key_mod(a->window,SDLK_UP,0);*stage=148;break;
    case 148:
        ok=a->toolbar_keyboard && a->toolbar_keyboard_item==0;
        test_window_key_mod(a->window,SDLK_ESCAPE,0);*stage=49;break;
    case 49:test_mouse(a,a->toolbar_bounds[0],true);break;
    case 50:test_mouse(a,a->toolbar_bounds[0],false);break;
    case 51:ok=a->toolbar_menu==1;*capture="editor-file-menu.bmp";test_key(a,SDLK_S);break;
    case 52: {
        ok=!a->toolbar_menu;ps_preferences p=a->preferences;p.theme=PS_THEME_LIGHT;preferences_apply(a,&p);
        test_mouse(a,a->toolbar_bounds[0],true);break;
    }
    case 53:test_mouse(a,a->toolbar_bounds[0],false);break;
    case 54:ok=a->toolbar_menu==1;*capture="editor-file-menu-light.bmp";test_window_key_mod(a->window,SDLK_ESCAPE,0);break;
    case 55: {
        ps_preferences p=a->preferences;p.theme=PS_THEME_HIGH_CONTRAST;preferences_apply(a,&p);
        test_mouse(a,a->toolbar_bounds[0],true);break;
    }
    case 56:test_mouse(a,a->toolbar_bounds[0],false);break;
    case 57:ok=a->toolbar_menu==1;*capture="editor-file-menu-contrast.bmp";test_window_key_mod(a->window,SDLK_ESCAPE,0);break;
#endif
    case 58: {
        ps_preferences p=a->preferences;p.theme=PS_THEME_DARK;preferences_apply(a,&p);
        ok=!a->toolbar_menu;a->quitting=true;puts("FILE TABS SELF-TEST: PASSED");break;
    }
    }
    if(!ok){fprintf(stderr,"File tabs failure stage %d active=%u count=%u drag=%d pending=%d completion=%d\n",*stage-1,file_tab_active(a),a->document_count,a->file_tab_drag,a->document_pending,a->completion_active);*code=1;a->quitting=true;}
}
