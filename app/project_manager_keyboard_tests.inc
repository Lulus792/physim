/* Create and execute every template/language pair with real SDL keyboard input. */
static void project_manager_keyboard_test_frame(app *a,const char *root,int *stage,int *code,const char **capture) {
    static unsigned project;
    static char parent[4096],name[256],producer[4096];
    bool only=project>=32;unsigned template=project%8,experiment=project/16,analysis=(project/8)%2;
    if(only)analysis=project-32;
    bool ok=true;int checked_stage=*stage;unsigned checked_project=project;
#define MANAGER_KEY(key) test_window_key_mod(a->window,key,0)
#define MANAGER_TEXT(value) do {SDL_Event event={0};event.type=SDL_EVENT_TEXT_INPUT;event.text.windowID=SDL_GetWindowID(a->window);event.text.text=(value);SDL_PushEvent(&event);}while(0)
    switch((*stage)++) {
    case 0: project=0;snprintf(parent,sizeof parent,"%s",root);producer[0]=0;SDL_SetWindowSize(a->window,1080,740);*stage=1;break;
    case 1: MANAGER_KEY(SDLK_F10);MANAGER_KEY(SDLK_DOWN);break;
    case 2: {
        toolbar_entry entries[9];unsigned count=toolbar_entries(a,1,entries);
        if(a->toolbar_keyboard_item<0 || (unsigned)a->toolbar_keyboard_item>=count){ok=false;break;}
        if(entries[a->toolbar_keyboard_item].action!=TOOLBAR_NEW){MANAGER_KEY(SDLK_DOWN);--*stage;return;}
        MANAGER_KEY(SDLK_RETURN);break;
    }
    case 3: ok=a->project_manager;MANAGER_KEY(SDLK_TAB);break;
    case 4: if(a->manager_focus_reveal){--*stage;return;}ok=a->manager_focus==0;
        test_window_key_mod(a->window,SDLK_A,PS_UI_COMMAND_MOD);MANAGER_TEXT(parent);MANAGER_KEY(SDLK_TAB);break;
    case 5: ok=!strcmp(a->manager_parent,parent) && a->manager_focus==1;MANAGER_KEY(SDLK_TAB);break;
    case 6: if(a->manager_focus_reveal){--*stage;return;}ok=a->manager_focus==2;
        snprintf(name,sizeof name,"keyboard-%02u ä",project);
        test_window_key_mod(a->window,SDLK_A,PS_UI_COMMAND_MOD);MANAGER_TEXT(name);MANAGER_KEY(SDLK_TAB);break;
    case 7: ok=!strcmp(a->manager_name,name) && a->manager_focus==3;
        if(a->manager_analysis_only!=only)MANAGER_KEY(SDLK_RIGHT);MANAGER_KEY(SDLK_TAB);break;
    case 8:
        ok=a->manager_analysis_only==only;
        if(only){ok=ok && a->manager_focus==6;*stage=10;break;}
        ok=ok && a->manager_focus==4;
        for(unsigned i=0;i<(template+8-(unsigned)a->manager_template)%8;i++)MANAGER_KEY(SDLK_RIGHT);
        MANAGER_KEY(SDLK_TAB);break;
    case 9: ok=a->manager_template==(int)template && a->manager_focus==5;
        if(a->manager_experiment_language!=(int)experiment)MANAGER_KEY(SDLK_RIGHT);MANAGER_KEY(SDLK_TAB);break;
    case 10: ok=a->manager_focus==6;
        if(a->template_analysis_language!=(int)analysis)MANAGER_KEY(SDLK_RIGHT);MANAGER_KEY(SDLK_TAB);break;
    case 11: if(a->manager_focus_reveal){--*stage;return;}
        ok=a->manager_focus==7 && a->template_analysis_language==(int)analysis;
        if(project==31)*capture="manager-keyboard-focus.bmp";
        MANAGER_KEY(SDLK_RETURN);break;
    case 12:
        if(a->library_thread || !idle(a)){--*stage;return;}
        ok=a->loaded && !a->project_manager && a->analysis_only==only && a->language_analysis==(analysis!=0) && (only || a->language_experiment==(experiment!=0));
        if(ok){char path[4096];join(path,sizeof path,a->project,"physim.project");ok=exists(path);}
        if(ok)MANAGER_KEY(SDLK_F5);break;
    case 13:
        if(!idle(a) || a->library_thread){--*stage;return;}
        ok=a->built;
        if(only){ok=ok && producer[0];if(ok)import_run(a,producer);*stage=16;break;}
        MANAGER_KEY(SDLK_F6);break;
    case 14: if(!a->hello || a->simulation_time<.03){--*stage;return;}
        command(a,PS_MSG_STOP);a->stop_at=ps_clock();break;
    case 15: if(!idle(a) || a->library_thread){--*stage;return;}
        ok=a->data.total>=4 && a->last_run[0];if(!project)snprintf(producer,sizeof producer,"%s",a->last_run);break;
    case 16: if(!idle(a) || a->library_thread || a->import_thread){--*stage;return;}
        ok=a->data.total>=4;analyze(a,false);break;
    case 17: if(!idle(a) || a->library_thread || a->report_thread){--*stage;return;}
        {uint32_t plots=0;ok=a->analysis_report && ps_report_describe(a->analysis_report,NULL,NULL,&plots,NULL)==PS_OK && plots>0;}
        if(ok){printf("KEYBOARD PROJECT %02u: built, recorded and analyzed\n",project);fflush(stdout);
            char path[4096];join(path,sizeof path,parent,"keyboard-progress.txt");FILE *progress=fopen(path,"a");
            if(progress){fprintf(progress,"%02u built, recorded and analyzed\n",project);fclose(progress);}}
        if(!ok)break;
        if(++project<34){*stage=1;break;}
        *stage=18;break;
    case 18: MANAGER_KEY(SDLK_F10);MANAGER_KEY(SDLK_DOWN);break;
    case 19: {
        toolbar_entry entries[9];toolbar_entries(a,1,entries);
        if(entries[a->toolbar_keyboard_item].action!=TOOLBAR_NEW){MANAGER_KEY(SDLK_DOWN);--*stage;return;}
        MANAGER_KEY(SDLK_RETURN);break;
    }
    case 20: MANAGER_KEY(SDLK_TAB);test_window_key_mod(a->window,SDLK_TAB,SDL_KMOD_SHIFT);break;
    case 21: if(a->manager_focus_reveal){--*stage;return;}ok=a->manager_focus==8;MANAGER_KEY(SDLK_RETURN);break;
    case 22: ok=!a->project_manager;a->quitting=true;printf("PROJECT MANAGER KEYBOARD SELF-TEST: %s\n",*code?"FAILED":"PASSED");break;
    }
    if(!ok){fprintf(stderr,"Project keyboard failure project=%u stage=%d focus=%d status=%s\n",checked_project,checked_stage,a->manager_focus,a->status);*code=1;a->quitting=true;}
#undef MANAGER_KEY
#undef MANAGER_TEXT
}

static void project_manager_error_test_frame(app *a,const char *root,int *stage,int *code,const char **capture) {
    static char parent[4096],original[4096],not_directory[4096];bool ok=true;
#define FORM_KEY(key) test_window_key_mod(a->window,key,0)
#define FORM_TEXT(value) do {SDL_Event e={0};e.type=SDL_EVENT_TEXT_INPUT;e.text.windowID=SDL_GetWindowID(a->window);e.text.text=(value);SDL_PushEvent(&e);}while(0)
#define FORM_REPLACE(value) do {test_window_key_mod(a->window,SDLK_A,PS_UI_COMMAND_MOD);FORM_TEXT(value);FORM_KEY(SDLK_TAB);}while(0)
#define FORM_BACK(count) for(int i=0;i<(count);i++)test_window_key_mod(a->window,SDLK_TAB,SDL_KMOD_SHIFT)
    switch((*stage)++) {
    case 0: {snprintf(parent,sizeof parent,"%s",root);char directory[4096];join(directory,sizeof directory,parent,"existing ä");ok=ps_make_directory_exclusive(directory);
        join(original,sizeof original,directory,"main.c");FILE *file=fopen(original,"wx");ok=ok && file;
        if(file){ok=ok && fputs("keep this file ä\n",file)>=0;ok=ok && !fclose(file);}
        join(not_directory,sizeof not_directory,root,"not-directory");file=fopen(not_directory,"wx");ok=ok && file;if(file){fputs("not a directory",file);fclose(file);}
        FORM_KEY(SDLK_F10);FORM_KEY(SDLK_DOWN);break;}
    case 1: case 19: {
        toolbar_entry entries[9];unsigned count=toolbar_entries(a,1,entries);int selected=a->toolbar_keyboard_item;
        if(selected<0 || (unsigned)selected>=count){ok=false;break;}
        if(entries[selected].action!=TOOLBAR_NEW){FORM_KEY(SDLK_DOWN);--*stage;return;}FORM_KEY(SDLK_RETURN);break;}
    case 2: FORM_KEY(SDLK_TAB);break;
    case 3: if(a->manager_focus_reveal){--*stage;return;}FORM_REPLACE(parent);break;
    case 4: ok=a->manager_focus==1;FORM_KEY(SDLK_TAB);break;
    case 5: if(a->manager_focus_reveal){--*stage;return;}FORM_REPLACE("invalid/name");break;
    case 6: for(int i=0;i<4;i++)FORM_KEY(SDLK_TAB);FORM_KEY(SDLK_RETURN);break;
    case 7: if(a->manager_error_reveal){--*stage;return;}*capture="manager-keyboard-name-error.bmp";ok=a->project_manager && strstr(a->status,"gültigen")!=NULL;FORM_BACK(5);break;
    case 8: if(a->manager_focus_reveal){--*stage;return;}FORM_REPLACE("fresh");FORM_BACK(3);break;
    case 9: if(a->manager_focus_reveal){--*stage;return;}FORM_REPLACE(not_directory);break;
    case 10: for(int i=0;i<6;i++)FORM_KEY(SDLK_TAB);FORM_KEY(SDLK_RETURN);break;
    case 11: if(a->manager_error_reveal){--*stage;return;}*capture="manager-keyboard-parent-error.bmp";ok=a->project_manager && strstr(a->status,"Zielordner")!=NULL;FORM_BACK(7);break;
    case 12: if(a->manager_focus_reveal){--*stage;return;}FORM_REPLACE(parent);FORM_KEY(SDLK_TAB);break;
    case 13: if(a->manager_focus_reveal){--*stage;return;}FORM_REPLACE("existing ä");break;
    case 14: for(int i=0;i<4;i++)FORM_KEY(SDLK_TAB);FORM_KEY(SDLK_RETURN);break;
    case 15: {if(a->manager_error_reveal){--*stage;return;}*capture="manager-keyboard-existing-error.bmp";size_t bytes=0;char *text=load_utf8_text(original,&bytes);ok=a->project_manager && strstr(a->status,"existieren bereits") && text && !strcmp(text,"keep this file ä\n");free(text);FORM_KEY(SDLK_ESCAPE);break;}
    case 16: ok=!a->project_manager;FORM_KEY(SDLK_F10);FORM_KEY(SDLK_DOWN);*stage=19;break;
    case 20: FORM_KEY(SDLK_TAB);test_window_key_mod(a->window,SDLK_TAB,SDL_KMOD_SHIFT);break;
    case 21: if(a->manager_focus_reveal){--*stage;return;}ok=a->manager_focus==8;FORM_KEY(SDLK_RETURN);break;
    case 22: ok=!a->project_manager;a->quitting=true;printf("PROJECT MANAGER ERRORS SELF-TEST: %s\n",*code?"FAILED":"PASSED");break;
    }
    if(!ok){fprintf(stderr,"Project manager error fixture stage %d focus=%d: %s\n",*stage-1,a->manager_focus,a->status);*code=1;a->quitting=true;}
#undef FORM_KEY
#undef FORM_TEXT
#undef FORM_REPLACE
#undef FORM_BACK
}
