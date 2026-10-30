static void project_wizard_help_frame(app *a,int *stage,int *code) {
    static unsigned settle;static char name[256];if(settle++<2)return;settle=0;bool ok=true;
    switch((*stage)++) {
    case 0:snprintf(name,sizeof name,"%s",a->manager_name);open_project_manager(a,"Neues Projekt");break;
    case 1:manager_test_key(a,SDLK_TAB,SDL_KMOD_SHIFT);break;
    case 2:if(a->manager_focus_reveal){--*stage;return;}manager_test_key(a,SDLK_RETURN,0);break;
    case 3:ok=a->manager_step==1;test_window_mouse(a->manager_window,a->manager_focus_bounds[5],true);break;
    case 4:test_window_mouse(a->manager_window,a->manager_focus_bounds[5],false);break;
    case 5: {struct nk_window *window=nk_window_find(a->manager_ui,"Neues Projekt");ok=window && window->popup.active;manager_test_key(a,SDLK_ESCAPE,0);break;}
    case 6: {struct nk_window *window=nk_window_find(a->manager_ui,"Neues Projekt");ok=a->project_manager && a->manager_step==1 && window && !window->popup.active;manager_test_key(a,SDLK_F1,0);break;}
    case 7:
        if(!a->doc_window || !a->documentation){--*stage;return;}
        ok=a->project_manager && a->manager_step==1 && !strcmp(name,a->manager_name);
        test_window_key_mod(a->doc_window,SDLK_ESCAPE,0);break;
    case 8:ok=!a->doc_visible && a->project_manager && a->manager_step==1;manager_test_key(a,SDLK_ESCAPE,0);break;
    case 9:ok=!a->project_manager;a->quitting=true;printf("PROJECT WIZARD HELP SELF-TEST: %s\n",ok?"PASSED":"FAILED");break;
    }
    if(!ok){struct nk_window *w=nk_window_find(a->manager_ui,"Neues Projekt");fprintf(stderr,"Project wizard help/popup failure at stage%d manager=%d step=%d popup=%d\n",*stage-1,a->project_manager,a->manager_step,w && w->popup.active);*code=1;a->quitting=true;}
}
