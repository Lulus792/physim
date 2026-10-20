static const char *outline_test_source(bool language) {
    return language?"// α😀\nlet länge = 1.5\nfunc derivative() -> Float64:\n    return länge\n":
        "// α😀\nstatic const double length_m = 1.5;\nstatic void derivative(void) { int local = 0; }\n";
}
static bool outline_test_start(app *a,const char *root,const char *mode) {
    bool language=strstr(mode,"phys")!=NULL;
    if(!completion_test_start(a,root,language?"completion-phys":"completion-c"))return false;
    const char *names[]={experiment_source(a),language?"outline.phys":"outline.c"};
    for(unsigned i=0;i<2;i++) {
        char path[4096];join(path,sizeof path,a->project,names[i]);
        FILE *file=fopen(path,"wb");if(!file)return false;
        bool ok=fputs(outline_test_source(language),file)>=0;ok&=fclose(file)==0;if(!ok)return false;
    }
    ps_preferences p=a->preferences;p.theme=strstr(mode,"light")?PS_THEME_LIGHT:PS_THEME_DARK;
    p.sidebar_width=218;p.inspector_width=255;p.log_height=154;
    if(strstr(mode,"large")){p.ui_size=22;p.editor_size=22;}
    preferences_apply(a,&p);SDL_SetWindowSize(a->window,strstr(mode,"large")?1600:1204,strstr(mode,"large")?1000:866);
    return true;
}
static void outline_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    static unsigned target,wait;bool ok=true,language=strstr(mode,"phys")!=NULL;
    struct nk_text_edit *edit=target?(active_document(a)?&active_document(a)->edit:NULL):&a->experiment;
    if(a->library_thread)return;
    switch(*stage) {
    case 0:
        ok=open_project(a);a->tab=0;
        if(target){char path[4096];join(path,sizeof path,a->project,language?"outline.phys":"outline.c");preview_workspace_path(a,path);}
        wait=0;break;
    case 1:
        if(wait++<3)return;
        ok=edit && completion_test_equal(edit,outline_test_source(language)) && a->outline.status==PS_OUTLINE_OK && a->outline.count==2 && a->outline_bounds[1].w>0;
        if(ok) {
            if(target)ok=nk_sdl_accessibility_request_focus(a->ui,NULL,"derivative");
            else test_mouse(a,a->outline_bounds[1],true);
        }break;
    case 2:
        if(target)test_window_key_mod(a->window,SDLK_RETURN,0);
        else test_mouse(a,a->outline_bounds[1],false);
        wait=0;break;
    case 3:
        if(wait++<3)return;
        {const char *source=nk_str_get_const(&edit->string),*original=outline_test_source(language);size_t offset=(size_t)(strstr(original,"derivative")-original);
        int expected=nk_utf_len(source,(int)offset);
        ok=edit->select_start==expected && edit->select_end==expected+10 && edit->active && a->ui->active==nk_window_find(a->ui,"workspace");
        ok=ok && !nk_sdl_accessibility_is_focused(a->ui,NULL,"derivative");
        if(!ok)fprintf(stderr,"OUTLINE JUMP expected=%d..%d actual=%d..%d cursor=%d window=%s\n",expected,expected+10,edit->select_start,edit->select_end,edit->cursor,a->ui->active?a->ui->active->name_string:"none");}
        *capture="outline-selected.bmp";
        test_window_key_mod(a->window,SDLK_RIGHT,0);break;
    case 4:{SDL_Event event={0};event.type=SDL_EVENT_TEXT_INPUT;event.text.windowID=SDL_GetWindowID(a->window);event.text.text="X";SDL_PushEvent(&event);wait=0;break;}
    case 5:
        if(wait++<3)return;
        ok=a->outline.status==PS_OUTLINE_OK && a->outline.count==2 && a->outline.symbols[1].length==11 && strstr(a->outline_source,"derivativeX");
        ok=ok && (target?active_document(a)->dirty:a->dirty);
        *capture=target?"outline-document.bmp":"outline-experiment.bmp";
        test_window_key_mod(a->window,SDLK_Z,PS_UI_COMMAND_MOD);wait=0;break;
    case 6:
        if(wait++<3)return;
        ok=completion_test_equal(edit,outline_test_source(language)) && a->outline.status==PS_OUTLINE_OK && a->outline.symbols[1].length==10;
        if(!ok)fprintf(stderr,"OUTLINE UNDO bytes=%d expected=%zu symbol=%zu point=%d keyboard=%d text=[%.*s]\n",nk_str_len_char(&edit->string),strlen(outline_test_source(language)),a->outline.symbols[1].length,edit->undo.undo_point,a->outline_keyboard,nk_str_len_char(&edit->string),nk_str_get_const(&edit->string));
        if(ok && !target){target=1;*stage=0;return;}
        a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"OUTLINE TEST FAILED stage=%d target=%u count=%zu status=%d active=%d\n",*stage,target,a->outline.count,a->outline.status,edit?edit->active:0);*capture="outline-failure.bmp";*code=1;a->quitting=true;}
    if(a->quitting)printf("OUTLINE SELF-TEST: %s\n",*code?"FAILED":"PASSED");
    ++*stage;
}
