/* Real native B host, actual routed keys/pointers, and domain changes while it
 * stays open. Physical focus and external accessibility clients are separate. */
static int project_window_test_width;
static bool project_window_test_start(app *a,const char *root,const char *mode) {
    char path[4096];join(path,sizeof path,root,"preferences.bin");preferences_start(a,path);
    SDL_strlcpy(a->manager_parent,root,sizeof a->manager_parent);join(a->project_input,sizeof a->project_input,root,"Original");a->example=0;new_project(a);
    if(!a->loaded)return false;
    ps_preferences p=a->preferences;p.theme=strstr(mode,"light")?PS_THEME_LIGHT:strstr(mode,"contrast")?PS_THEME_HIGH_CONTRAST:PS_THEME_DARK;
    p.ui_size=strstr(mode,"large")?22:16;project_window_test_width=strstr(mode,"small")?320:680;
    preferences_apply(a,&p);SDL_SetWindowSize(a->window,1440,1000);return true;
}
static void project_window_test_frame(app *a,const char *root,int *stage,int *code) {
    static unsigned settle,setup,font_stage,saved_ui_size;static SDL_Window *held;static char target[4096],parent[4096],alternate[4096],error_file[4096],previous_input[4096];static int previous_example;static ps_dialog_ticket child;static uint32_t source_crc;static bool changed_project;
    if(settle++<2)return;settle=0;bool okay=true;
#define PROJECT_FOCUS(id) do{if(!manager_test_focus(a,id))return;}while(0)
    switch(*stage){
    case 0:if(a->library_thread)return;
        if(!setup){join(alternate,sizeof alternate,root,"Anderes Projekt");SDL_strlcpy(a->project_input,alternate,sizeof a->project_input);a->example=1;new_project(a);
            if(!a->loaded || strcmp(a->project,alternate)){okay=false;break;}setup=1;return;}
        if(setup==1){join(a->project_input,sizeof a->project_input,root,"Original");if(!open_project(a)){okay=false;break;}setup=2;return;}
        SDL_strlcpy(parent,root,sizeof parent);
        source_crc=ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));open_project_manager(a,"Datei");
        if(project_window_test_width==320 && a->manager_window)SDL_SetWindowSize(a->manager_window,320,740);break;
    case 1:{
        okay=a->manager_window && a->manager_ui && a->manager_graphics && a->manager_ui!=a->ui && a->manager_graphics!=a->graphics;
        if(!okay)break;held=a->manager_window;
#ifdef _WIN32
        void *native=SDL_GetPointerProperty(SDL_GetWindowProperties(held),SDL_PROP_WINDOW_WIN32_HWND_POINTER,NULL);
        void *main=SDL_GetPointerProperty(SDL_GetWindowProperties(a->window),SDL_PROP_WINDOW_WIN32_HWND_POINTER,NULL);
        okay=native && main && native!=main;
#else
        Sint64 native=SDL_GetNumberProperty(SDL_GetWindowProperties(held),SDL_PROP_WINDOW_X11_WINDOW_NUMBER,0);
        Sint64 main=SDL_GetNumberProperty(SDL_GetWindowProperties(a->window),SDL_PROP_WINDOW_X11_WINDOW_NUMBER,0);
        okay=!strcmp(SDL_GetCurrentVideoDriver(),"x11") && native && main && native!=main;
#endif
        printf("{\"kind\":\"independent_project_window\",\"main_id\":%u,\"project_id\":%u,\"native_handles_distinct\":%s}\n",SDL_GetWindowID(a->window),SDL_GetWindowID(held),okay?"true":"false");
        if(project_window_test_width==320)join(a->manager_capture,sizeof a->manager_capture,parent,"project-window-start.png");break;
    }
    case 2:
        if(a->manager_template!=2){PROJECT_FOCUS(13);manager_test_key(a,SDLK_RETURN,0);return;}
        PROJECT_FOCUS(7);manager_test_key(a,SDLK_RETURN,0);break;
    case 3:okay=a->manager_step==1;PROJECT_FOCUS(2);manager_test_text(a,"Fenster λ");break;
    case 4:okay=!strcmp(a->manager_name,"Fenster λ");PROJECT_FOCUS(5);manager_test_key(a,SDLK_SPACE,0);break;
    case 5:PROJECT_FOCUS(6);manager_test_key(a,SDLK_SPACE,0);break;
    case 6:okay=a->manager_experiment_language==1 && a->template_analysis_language==1;test_mouse(a,a->navigation_bounds[1],true);break;
    case 7:test_mouse(a,a->navigation_bounds[1],false);break;
    case 8:okay=a->tab==1 && a->project_manager && a->manager_window==held && !strcmp(a->manager_name,"Fenster λ") && !strcmp(a->manager_parent,parent) &&
        source_crc==ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));open_project_manager(a,"Datei");break;
    case 9:{
        okay=a->manager_window==held && a->manager_step==1 && a->manager_template==2 && !strcmp(a->manager_name,"Fenster λ") && a->manager_experiment_language==1 && a->template_analysis_language==1;
        int w,h;SDL_GetWindowSize(a->window,&w,&h);okay &= SDL_SetWindowSize(held,project_window_test_width,740);int nw,nh;SDL_GetWindowSize(a->window,&nw,&nh);okay &= w==nw && h==nh;saved_ui_size=a->preferences.ui_size;break;
    }
    case 10:
        if(changed_project && (!a->project_manager || a->manager_window!=held)){okay=false;break;}
        PROJECT_FOCUS(7);
        okay=a->manager_bounds.x>=0 && a->manager_bounds.y>=0 && a->manager_bounds.w<=project_window_test_width && a->manager_bounds.h<=740;
        for(unsigned id=7;id<=9;id++){
            struct nk_rect r=a->manager_focus_bounds[id];
            okay &= r.w>0 && r.h>0 && r.x>=0 && r.x+r.w<=project_window_test_width && r.y>=0 && r.y+r.h<=740;
        }
        if(!okay)break;
        if(a->manager_bounds.h<740){struct nk_window *view=nk_window_find(a->manager_ui,"Neues Projekt");okay=view && !view->scrollbar.y;if(!okay)break;}
        if(font_stage<2){ps_preferences draft=a->preferences;draft.ui_size=font_stage==0?(saved_ui_size==16?22:16):saved_ui_size;
            preferences_apply(a,&draft);okay=a->preferences.ui_size==draft.ui_size;if(!okay)break;font_stage++;return;}
        if(!changed_project){if(a->library_thread)return;SDL_strlcpy(a->project_input,alternate,sizeof a->project_input);
            okay=open_project(a) && a->project_manager && a->manager_window==held;if(!okay)break;changed_project=true;return;}
        if(a->library_thread)return;
        okay=a->loaded && !strcmp(a->project,alternate) && a->project_manager && a->manager_window==held && a->manager_step==1 && a->manager_template==2 &&
            !strcmp(a->manager_name,"Fenster λ") && !strcmp(a->manager_parent,parent) && a->manager_experiment_language==1 && a->template_analysis_language==1;
        okay &= ps_browser_create_directory(parent,"Anderer Workspace",target)==PS_BROWSER_CREATED && open_workspace_path(a,target);
        okay &= a->project_manager && a->manager_window==held && a->manager_step==1 && a->manager_template==2 && !strcmp(a->manager_name,"Fenster λ") && !strcmp(a->manager_parent,parent) && a->manager_experiment_language==1 && a->template_analysis_language==1;
        printf("{\"kind\":\"project_window_domain_changes\",\"loaded_project_and_bare_workspace_preserve_form\":%s,\"live_font_reflow_preserves_focus\":%s,\"template\":%d,\"step\":%d,\"experiment_language\":%d,\"analysis_language\":%d}\n",okay?"true":"false",font_stage==2 && a->preferences.ui_size==saved_ui_size?"true":"false",a->manager_template,a->manager_step,a->manager_experiment_language,a->template_analysis_language);
        join(a->manager_capture,sizeof a->manager_capture,parent,"project-window.png");test_window_key_mod(a->window,SDLK_ESCAPE,0);break;
    case 11:okay=a->project_manager && a->workspace_open && !strcmp(a->workspace,target);manager_test_key(a,SDLK_ESCAPE,0);break;
    case 12:okay=!a->project_manager && !a->manager_window && a->workspace_open && !strcmp(a->workspace,target);open_project_manager(a,"Datei");break;
    case 13:PROJECT_FOCUS(7);manager_test_key(a,SDLK_RETURN,0);break;
    case 14:PROJECT_FOCUS(1);manager_test_key(a,SDLK_RETURN,0);break;
    case 15:{
        if(a->browser && browser_busy(a->browser))return;
        okay=browser_active(a) && a->project_manager && a->browser->mode==PS_DIALOG_MANAGER_PARENT;
        if(okay){child=a->browser->ticket;SDL_Event event={0};event.type=SDL_EVENT_WINDOW_CLOSE_REQUESTED;event.window.windowID=SDL_GetWindowID(a->manager_window);SDL_PushEvent(&event);}break;
    }
    case 16:
        okay=!a->project_manager && !a->manager_window && !browser_active(a) && !a->dialog_pending && !ps_dialog_request_find(&a->dialog_requests,child) && !strcmp(a->workspace,target);
        open_project_manager(a,"Datei");if(a->manager_window)SDL_SetWindowSize(a->manager_window,project_window_test_width,740);break;
    case 17:PROJECT_FOCUS(7);manager_test_key(a,SDLK_RETURN,0);break;
    case 18:{
        PROJECT_FOCUS(7);
        okay=a->manager_step==1;char folder[4096];
        okay &= ps_browser_create_directory(parent,a->manager_name,folder)==PS_BROWSER_CREATED;
        if(!okay)break;join(error_file,sizeof error_file,folder,"main.c");FILE *file=fopen(error_file,"wbx");okay=file!=NULL;
        if(file){okay &= fputs("keep native B test source\n",file)>=0;okay &= !fclose(file);}
        if(!okay)break;SDL_strlcpy(previous_input,a->project_input,sizeof previous_input);previous_example=a->example;
        manager_test_key(a,SDLK_RETURN,0);break;
    }
    case 19:okay=a->manager_step==2;PROJECT_FOCUS(7);manager_test_key(a,SDLK_RETURN,0);break;
    case 20:{
        if(a->manager_error_reveal)return;
        size_t length=0;char *text=load_utf8_text(error_file,&length);
        okay=a->project_manager && a->manager_step==2 && strstr(a->manager_error,"existieren bereits") && !a->loaded &&
            !strcmp(a->workspace,target) && !strcmp(a->project_input,previous_input) && a->example==previous_example &&
            text && !strcmp(text,"keep native B test source\n");free(text);
        struct nk_rect box=a->manager_error_bounds,ink=a->manager_error_text_bounds;
        okay &= box.h>0 && ink.h>0 && ink.x>=box.x && ink.y>=box.y && ink.x+ink.w<=box.x+box.w+.1f && ink.y+ink.h<=box.y+box.h+.1f && box.y>=0 && box.y+box.h<=740;
        printf("{\"kind\":\"project_window_error\",\"passed\":%s,\"ui_size\":%u,\"panel\":[%.4f,%.4f,%.4f,%.4f],\"text\":[%.4f,%.4f,%.4f,%.4f]}\n",okay?"true":"false",a->preferences.ui_size,box.x,box.y,box.w,box.h,ink.x,ink.y,ink.w,ink.h);
        if(project_window_test_width==320 && saved_ui_size==22)join(a->manager_capture,sizeof a->manager_capture,parent,"project-window-error.png");break;
    }
    case 21:PROJECT_FOCUS(9);manager_test_key(a,SDLK_RETURN,0);break;
    case 22:okay=a->manager_step==1;manager_test_key(a,SDLK_ESCAPE,0);break;
    case 23:okay=!a->project_manager && !a->manager_window && !strcmp(a->workspace,target);open_project_manager(a,"Datei");break;
    default:
        okay=a->project_manager && a->manager_window && a->manager_ui && !strcmp(a->workspace,target);
        puts(okay?"INDEPENDENT PROJECT WINDOW SELF-TEST: PASSED":"INDEPENDENT PROJECT WINDOW SELF-TEST: FAILED");a->quitting=true;break;
    }
    if(!okay){struct nk_window *view=a->manager_ui?nk_window_find(a->manager_ui,"Neues Projekt"):NULL;
        fprintf(stderr,"Independent project window stage%d failed: main-tab=%d manager=%d step=%d size=%u font_stage=%u scroll=%u next=%.1f/%.1f\n",*stage,a->tab,a->project_manager,a->manager_step,a->preferences.ui_size,font_stage,view?view->scrollbar.y:0,a->manager_focus_bounds[7].y,a->manager_focus_bounds[7].h);*code=1;a->quitting=true;}
    (*stage)++;
#undef PROJECT_FOCUS
}
