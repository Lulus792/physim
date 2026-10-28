#include "project_wizard_tests.inc"
/* Reach actual controls by Tab; never assign the product's focus or step. */
static bool manager_test_focus(app *a,int id) {
    if(a->manager_focus_known && a->manager_focus==id && a->manager_keyboard)return !a->manager_focus_reveal;
    test_window_key_mod(a->window,SDLK_TAB,0);return false;
}
static void manager_test_text(app *a,const char *text) {
    test_window_key_mod(a->window,SDLK_A,PS_UI_COMMAND_MOD);
    SDL_Event e={0};e.type=SDL_EVENT_TEXT_INPUT;e.text.windowID=SDL_GetWindowID(a->window);e.text.text=text;SDL_PushEvent(&e);
    test_window_key_mod(a->window,SDLK_TAB,0);
}
static bool manager_test_open(app *a) {
    toolbar_entry entries[9];unsigned count=toolbar_entries(a,1,entries);int at=a->toolbar_keyboard_item;
    if(at<0 || (unsigned)at>=count)return false;
    test_window_key_mod(a->window,entries[at].action==TOOLBAR_NEW?SDLK_RETURN:SDLK_DOWN,0);
    return entries[at].action==TOOLBAR_NEW;
}
/* All32 template/language pairs plus two analysis-only projects still build,
 * record/import and analyze. New review/back steps preserve every choice. */
static void project_manager_keyboard_test_frame(app *a,const char *root,int *stage,int *code,const char **capture) {
    static unsigned project,wait_project;static int wait_stage=-1;static double wait_started,matrix_started,project_started;
    static char parent[4096],name[256],producer[4096];
    bool only=project>=32;unsigned template=project%8,experiment=project/16,analysis=(project/8)%2;if(only)analysis=project-32;
    bool ok=true;int checked=*stage;
    double now=ps_clock();
    if(wait_stage!=*stage || wait_project!=project){wait_stage=*stage;wait_project=project;wait_started=now;}
    double limit=*stage>=14 && *stage<=19?180:60;
    if(now-wait_started>limit){fprintf(stderr,"Project wizard stalled: project=%u stage=%d elapsed=%.3f limit=%.0f\n",project,*stage,now-wait_started,limit);*code=1;a->quitting=true;return;}
#define MANAGER_KEY(key) test_window_key_mod(a->window,key,0)
#define MANAGER_FOCUS(id) if(!manager_test_focus(a,id)){--*stage;return;}
    switch((*stage)++) {
    case 0:project=0;matrix_started=now;snprintf(parent,sizeof parent,"%s",root);producer[0]=0;SDL_SetWindowSize(a->window,1080,740);break;
    case 1:case 25:project_started=now;MANAGER_KEY(SDLK_F10);MANAGER_KEY(SDLK_DOWN);break;
    case 2:case 26:if(!manager_test_open(a)){--*stage;return;}break;
    case 3:ok=a->project_manager && a->manager_step==0;MANAGER_FOCUS(3);if(a->manager_analysis_only!=only)MANAGER_KEY(SDLK_SPACE);break;
    case 4:if(!only){MANAGER_FOCUS(11+(int)template);MANAGER_KEY(SDLK_RETURN);}break;
    case 5:MANAGER_FOCUS(7);MANAGER_KEY(SDLK_RETURN);break;
    case 6:ok=a->manager_step==1;MANAGER_FOCUS(2);snprintf(name,sizeof name,"keyboard-%02u ä",project);manager_test_text(a,name);break;
    case 7:MANAGER_FOCUS(0);manager_test_text(a,parent);break;
    case 8:if(!only){MANAGER_FOCUS(5);if(a->manager_experiment_language!=(int)experiment)MANAGER_KEY(SDLK_RIGHT);}break;
    case 9:MANAGER_FOCUS(6);if(a->template_analysis_language!=(int)analysis)MANAGER_KEY(SDLK_RIGHT);break;
    case 10:MANAGER_FOCUS(7);MANAGER_KEY(SDLK_RETURN);break;
    case 11: {
        ok=a->manager_step==2 && a->project_manager && !strcmp(a->manager_name,name) && !strcmp(a->manager_parent,parent);
        char path[4096];join(path,sizeof path,parent,name);ok &= !exists(path);
        MANAGER_FOCUS(9);MANAGER_KEY(SDLK_RETURN);break;
    }
    case 12:ok=a->manager_step==1 && a->manager_analysis_only==only && a->template_analysis_language==(int)analysis &&
        (only || (a->manager_template==(int)template && a->manager_experiment_language==(int)experiment)) &&
        !strcmp(a->manager_name,name) && !strcmp(a->manager_parent,parent);MANAGER_FOCUS(7);MANAGER_KEY(SDLK_RETURN);break;
    case 13:MANAGER_FOCUS(7);if(project==31)*capture="manager-keyboard-focus.bmp";MANAGER_KEY(SDLK_RETURN);break;
    case 14:
        if(a->library_thread || !idle(a)){--*stage;return;}
        ok=a->loaded && !a->project_manager && a->analysis_only==only && a->language_analysis==(analysis!=0) && (only || a->language_experiment==(experiment!=0));
        if(ok){char path[4096];join(path,sizeof path,a->project,"physim.project");ok=exists(path);}if(ok)MANAGER_KEY(SDLK_F5);break;
    case 15:if(!idle(a) || a->library_thread){--*stage;return;}ok=a->built;
        if(only){ok &= producer[0]!=0;if(ok)import_run(a,producer);*stage=18;break;}MANAGER_KEY(SDLK_F6);break;
    case 16:if(!a->hello || a->simulation_time<.03){--*stage;return;}command(a,PS_MSG_STOP);a->stop_at=ps_clock();break;
    case 17:if(!idle(a) || a->library_thread){--*stage;return;}ok=a->data.total>=4 && a->last_run[0];if(!project)snprintf(producer,sizeof producer,"%s",a->last_run);break;
    case 18:if(!idle(a) || a->library_thread || a->import_thread){--*stage;return;}ok=a->data.total>=4;analyze(a,false);break;
    case 19:if(!idle(a) || a->library_thread || a->report_thread){--*stage;return;}
        {uint32_t plots=0;ok=a->analysis_report && ps_report_describe(a->analysis_report,NULL,NULL,&plots,NULL)==PS_OK && plots>0;}
        if(ok){printf("KEYBOARD PROJECT %02u: built, recorded and analyzed\n{\"kind\":\"wizard_project_timing\",\"project\":%u,\"project_seconds\":%.3f,\"matrix_seconds\":%.3f}\n",project,project,now-project_started,now-matrix_started);fflush(stdout);char path[4096];join(path,sizeof path,parent,"keyboard-progress.txt");FILE *progress=fopen(path,"a");if(progress){fprintf(progress,"%02u built, recorded and analyzed\n",project);fclose(progress);}}
        if(!ok)break;if(++project<34)*stage=1;else *stage=25;break;
    case 27:MANAGER_FOCUS(8);MANAGER_KEY(SDLK_RETURN);break;
    case 28:ok=!a->project_manager;a->quitting=true;printf("PROJECT MANAGER KEYBOARD SELF-TEST: %s\n",*code?"FAILED":"PASSED");break;
    }
    if(!ok){fprintf(stderr,"Project keyboard failure project=%u stage=%d focus=%d step=%d status=%s\n",project,checked,a->manager_focus,a->manager_step,a->status);*code=1;a->quitting=true;}
#undef MANAGER_KEY
#undef MANAGER_FOCUS
}
static void project_manager_error_test_frame(app *a,const char *root,int *stage,int *code,const char **capture) {
    static char parent[4096],original[4096],not_directory[4096];bool ok=true;int checked=*stage;
#define FORM_KEY(key) test_window_key_mod(a->window,key,0)
#define FORM_FOCUS(id) if(!manager_test_focus(a,id)){--*stage;return;}
    switch((*stage)++) {
    case 0: {snprintf(parent,sizeof parent,"%s",root);char directory[4096];join(directory,sizeof directory,parent,"existing ä");ok=ps_make_directory_exclusive(directory);
        join(original,sizeof original,directory,"main.c");FILE *file=fopen(original,"wbx");ok=ok && file;if(file){ok=ok && fputs("keep this file ä\n",file)>=0;ok=ok && !fclose(file);}
        join(not_directory,sizeof not_directory,root,"not-directory");file=fopen(not_directory,"wx");ok=ok && file;if(file){fputs("not a directory",file);fclose(file);}
        FORM_KEY(SDLK_F10);FORM_KEY(SDLK_DOWN);break;}
    case 1:if(!manager_test_open(a)){--*stage;return;}break;
    case 2:FORM_FOCUS(7);FORM_KEY(SDLK_RETURN);break;
    case 3:FORM_FOCUS(2);manager_test_text(a,"invalid/name");break;
    case 4:FORM_FOCUS(7);FORM_KEY(SDLK_RETURN);break;
    case 5:if(a->manager_error_reveal){--*stage;return;}ok=a->manager_step==1 && strstr(a->status,"gültigen");*capture="manager-keyboard-name-error.bmp";break;
    case 6:FORM_FOCUS(2);manager_test_text(a,"fresh");break;
    case 7:FORM_FOCUS(0);manager_test_text(a,not_directory);break;
    case 8:FORM_FOCUS(7);FORM_KEY(SDLK_RETURN);break;
    case 9:if(a->manager_error_reveal){--*stage;return;}ok=a->manager_step==1 && strstr(a->status,"Zielordner");*capture="manager-keyboard-parent-error.bmp";break;
    case 10:FORM_FOCUS(0);manager_test_text(a,parent);break;
    case 11:FORM_FOCUS(2);manager_test_text(a,"existing ä");break;
    case 12:FORM_FOCUS(7);FORM_KEY(SDLK_RETURN);break;
    case 13:ok=a->manager_step==2;FORM_FOCUS(7);FORM_KEY(SDLK_RETURN);break;
    case 14: {if(a->manager_error_reveal){--*stage;return;}size_t bytes=0;char *text=load_utf8_text(original,&bytes);ok=a->project_manager && strstr(a->status,"existieren bereits") && text && !strcmp(text,"keep this file ä\n");free(text);*capture="manager-keyboard-existing-error.bmp";FORM_KEY(SDLK_ESCAPE);break;}
    case 15:ok=!a->project_manager;FORM_KEY(SDLK_F10);FORM_KEY(SDLK_DOWN);break;
    case 16:if(!manager_test_open(a)){--*stage;return;}break;
    case 17:ok=a->project_manager;if(!ok)break;FORM_FOCUS(8);*capture="manager-keyboard-cancel.bmp";FORM_KEY(SDLK_RETURN);break;
    case 18:ok=!a->project_manager;a->quitting=true;printf("PROJECT MANAGER ERRORS SELF-TEST: %s\n",*code?"FAILED":"PASSED");break;
    }
    if(!ok){fprintf(stderr,"Project manager error fixture stage=%d manager=%d focus=%d step=%d menu=%d keyboard=%d native=%d custom=%d: %s\n",checked,a->project_manager,a->manager_focus,a->manager_step,a->toolbar_menu,a->toolbar_keyboard,nk_sdl_accessibility_has_focus(a->ui),nk_sdl_accessibility_keyboard_owned(a->ui),a->status);*code=1;a->quitting=true;}
#undef FORM_KEY
#undef FORM_FOCUS
}
