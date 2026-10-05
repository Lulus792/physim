static char named_test_root[4096];
static bool workspace_catalog_test_start(app *a,const char *root,const char *mode) {
    snprintf(named_test_root,sizeof named_test_root,"%s",root);
    char path[4096];join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    SDL_SetWindowSize(a->window,1080,740);
    if(strcmp(mode,"named-write")) {
        if(!strcmp(mode,"named-corrupt") || !strcmp(mode,"named-reset")) {
            join(path,sizeof path,root,"Pendel ä");return open_workspace_path(a,path);
        }
        return !a->workspace_open;
    }
    join(a->project_input,sizeof a->project_input,root,"Pendel ä");a->example=0;new_project(a);
    if(!a->loaded)return false;
    a->dt=.01;a->project_settings_dirty=true;select_build_profile(a,1);
    join(path,sizeof path,a->project,"Notizen ä.txt");
    if(!document_test_write(path,"Notizen α = 1\nZweite Zeile\nDritte Zeile\nVierte Zeile\n"))return false;
    join(path,sizeof path,a->project,"Zweite.txt");return document_test_write(path,"Zweite Datei\n");
}
static void workspace_catalog_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    static bool settle;static uint32_t source;static double started;
    if(settle){settle=false;return;}settle=true;
    char path[4096],other[4096];bool ok=true;
    if(!strcmp(mode,"named-corrupt") || !strcmp(mode,"named-reset")) {
        switch(*stage) {
        case 0:ok=!a->workspaces_writable && *a->workspaces_error;a->workspace_manager=true;break;
        case 1:*capture="workspaces-corrupt.bmp";test_mouse(a,a->workspace_catalog_bounds[1],true);break;
        case 2:test_mouse(a,a->workspace_catalog_bounds[1],false);break;
        case 3:ok=!a->workspaces_writable && !a->workspaces->count;
            if(!strcmp(mode,"named-corrupt"))a->quitting=true;else test_mouse(a,a->workspace_catalog_bounds[4],true);break;
        case 4:test_mouse(a,a->workspace_catalog_bounds[4],false);break;
        case 5:ok=a->workspaces_writable && !a->workspaces->count && !*a->workspaces_error;a->quitting=true;break;
        }
    } else if(strcmp(mode,"named-write")) {
        switch(*stage) {
        case 0:
            if(!a->toolbar_bounds[0].w)return;
            ok=a->workspaces_writable && a->workspaces->count==1 && !a->workspace_open;test_mouse(a,a->toolbar_bounds[0],true);break;
        case 1:test_mouse(a,a->toolbar_bounds[0],false);break;
        case 2:test_mouse(a,a->workspace_menu_bounds,true);break;
        case 3:test_mouse(a,a->workspace_menu_bounds,false);break;
        case 4:ok=a->workspace_manager;test_mouse(a,a->workspace_catalog_entries[0],true);break;
        case 5:test_mouse(a,a->workspace_catalog_entries[0],false);break;
        case 6:*capture="workspaces-reopened.bmp";test_mouse(a,a->workspace_catalog_bounds[2],true);break;
        case 7:test_mouse(a,a->workspace_catalog_bounds[2],false);break;
        case 8:
            if(a->library_thread)return;
            ok=!a->workspace_manager && a->loaded && !a->built && !a->runner.running && !a->language_experiment && a->document_count==2 && a->document_active==0 && a->tab==8;
            if(!strcmp(mode,"named-short")) {
                ok &= a->documents[0].edit.cursor==2 && a->documents[0].edit.select_start==2 && a->documents[0].edit.select_end==2;
            } else ok &= a->documents[0].edit.cursor==25;
            *capture="workspaces-restored.bmp";a->quitting=true;break;
        }
    } else switch(*stage) {
    case 0:build_project(a);break;
    case 1:if(!idle(a) || a->library_thread)return;ok=a->built;start_run(a);break;
    case 2:
        if(!a->hello || a->simulation_time<.2)return;started=a->simulation_time;
        nk_str_append_text_char(&a->experiment.string,"\n/* workspace keeps source */\n",(int)sizeof "\n/* workspace keeps source */\n"-1);a->dirty=true;
        source=ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
        join(path,sizeof path,a->project,"Notizen ä.txt");preview_workspace_path(a,path);
        join(path,sizeof path,a->project,"Zweite.txt");preview_workspace_path(a,path);document_select(a,0);
        join(path,sizeof path,named_test_root,"Zusatz ä");ok=ps_make_directory(path);add_workspace_path(a,path);
        test_mouse(a,a->toolbar_bounds[0],true);break;
    case 3:test_mouse(a,a->toolbar_bounds[0],false);break;
    case 4:test_mouse(a,a->workspace_menu_bounds,true);break;
    case 5:test_mouse(a,a->workspace_menu_bounds,false);break;
    case 6:
        ok=a->workspace_manager;a->documents[0].edit.cursor=18;a->documents[0].edit.select_start=3;a->documents[0].edit.select_end=18;
        test_mouse(a,a->workspace_catalog_bounds[0],true);break;
    case 7:test_mouse(a,a->workspace_catalog_bounds[0],false);break;
    case 8:layout_test_text(a,"Pendel ä");break;
    case 9:ok=!strcmp(a->workspace_name,"Pendel ä");test_mouse(a,a->workspace_catalog_bounds[1],true);break;
    case 10:test_mouse(a,a->workspace_catalog_bounds[1],false);break;
    case 11:
        ok=a->workspaces->count==1 && a->workspaces->entries[0].state.document_count==2 && a->workspaces->entries[0].state.documents[0].editor.cursor==18;
        *capture="workspaces-running.bmp";test_mouse(a,a->workspace_catalog_bounds[2],true);break;
    case 12:test_mouse(a,a->workspace_catalog_bounds[2],false);test_window_key_mod(a->window,SDLK_F7,0);break;
    case 13:
        ok=a->workspace_manager && a->runner.running && !a->reset_pending && a->dirty && a->simulation_time>started &&
            source==ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
        command(a,PS_MSG_STOP);a->stop_at=ps_clock();break;
    case 14:
        if(!idle(a) || a->library_thread)return;
        join(a->project_input,sizeof a->project_input,named_test_root,"Sprache ä");a->example=8;new_project(a);ok=a->loaded && a->language_experiment;
        join(path,sizeof path,a->project,"B.txt");ok &= document_test_write(path,"B original\n");preview_workspace_path(a,path);a->tab=2;a->analysis_tab=1;
        test_mouse(a,a->workspace_catalog_bounds[0],true);break;
    case 15:test_mouse(a,a->workspace_catalog_bounds[0],false);break;
    case 16:test_key(a,SDLK_A);break;
    case 17:layout_test_text(a,"Sprache ä");break;
    case 18:ok=!strcmp(a->workspace_name,"Sprache ä");test_mouse(a,a->workspace_catalog_bounds[1],true);break;
    case 19:test_mouse(a,a->workspace_catalog_bounds[1],false);break;
    case 20:
        ok=a->workspaces->count==2 && a->workspaces->entries[1].state.view==2 && a->workspaces->entries[1].state.analysis_editor==1;
        ok &= document_test_text(a,"B local draft\n");join(path,sizeof path,a->project,"B.txt");ok &= document_test_write(path,"B external\n");
        test_mouse(a,a->workspace_catalog_entries[0],true);break;
    case 21:test_mouse(a,a->workspace_catalog_entries[0],false);break;
    case 22:test_mouse(a,a->workspace_catalog_bounds[2],true);break;
    case 23:test_mouse(a,a->workspace_catalog_bounds[2],false);break;
    case 24:
        ok=a->workspace_manager && a->language_experiment && a->documents[0].dirty && *a->workspaces_error && strstr(a->documents[0].error,"extern");
        *capture="workspaces-conflict.bmp";ok &= document_reload(a);
        /* An invalid destination must retain the currently loaded source and docs. */
        join(path,sizeof path,named_test_root,"Pendel ä/analysis.c");snprintf(other,sizeof other,"%s.hidden",path);
        ok &= SDL_RenamePath(path,other);join(path,sizeof path,named_test_root,"Pendel ä");
        ok &= !open_workspace_path(a,path) && a->language_experiment && a->document_count==1;
        join(path,sizeof path,named_test_root,"Pendel ä/analysis.c");ok &= SDL_RenamePath(other,path);
        test_mouse(a,a->workspace_catalog_bounds[2],true);break;
    case 25:test_mouse(a,a->workspace_catalog_bounds[2],false);break;
    case 26:
        if(a->library_thread)return;
        ok=!a->workspace_manager && a->loaded && !a->language_experiment && !a->built && !a->runner.running && a->document_count==2 && a->document_active==0 && a->tab==8 && a->documents[0].edit.cursor==18;
        a->documents[0].edit.cursor=25;a->documents[0].edit.select_start=3;a->documents[0].edit.select_end=25;
        a->workspace_manager=true;break;
    case 27:test_mouse(a,a->workspace_catalog_bounds[1],true);break;
    case 28:test_mouse(a,a->workspace_catalog_bounds[1],false);break;
    case 29:
        ok=a->workspaces->count==2 && a->workspaces->entries[0].state.documents[0].editor.cursor==25;
        test_mouse(a,a->workspace_catalog_entries[1],true);break;
    case 30:test_mouse(a,a->workspace_catalog_entries[1],false);break;
    case 31:test_mouse(a,a->workspace_catalog_bounds[3],true);break;
    case 32:test_mouse(a,a->workspace_catalog_bounds[3],false);break;
    case 33:ok=a->workspaces->count==1 && !strcmp(a->workspaces->entries[0].name,"Pendel ä");*capture="workspaces-saved.bmp";a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Named workspace failed at stage %d: manager=%d count=%u selected=%d name=%s error=%s status=%s\n",*stage,a->workspace_manager,a->workspaces?a->workspaces->count:0,a->workspace_selected,a->workspace_name,a->workspaces_error,a->status);*capture="workspaces-failed.bmp";*code=1;a->quitting=true;}
    if(a->quitting)printf("NAMED WORKSPACES %s SELF-TEST: %s\n",mode,*code?"FAILED":"PASSED");(*stage)++;
}
