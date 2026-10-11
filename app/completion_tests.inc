static const char *completion_c_original[]={"// α😀\nps_vlOLD\n","// α😀\nPS_METOLD\n","// α😀\nps_memory_alOLD\n"};
static const char *completion_c_changed[]={"// α😀\nps_vlength\n","// α😀\nPS_METRE\n","// α😀\nps_memory_alignment\n"};
static const char *completion_ph_original[]={"// α😀\nVecOLD\n","// α😀\nMat4.transOLD\n","// α😀\ndirection.normOLD\n"};
static const char *completion_ph_changed[]={"// α😀\nVec3\n","// α😀\nMat4.translation\n","// α😀\ndirection.normalized\n"};
static bool completion_test_start(app *a,const char *root,const char *mode) {
    bool language=!strcmp(mode,"completion-phys");
    join(a->project_input,sizeof a->project_input,root,"project");a->example=language?8:0;a->template_analysis_language=language?1:0;new_project(a);
    if(!a->loaded)return false;
    const char *names[]={experiment_source(a),analysis_source(a),language?"completion-note.phys":"completion-note.c"};
    for(unsigned i=0;i<3;i++){char path[4096];join(path,sizeof path,a->project,names[i]);FILE *file=fopen(path,"wb");
        if(!file)return false;bool ok=fputs(language?completion_ph_original[i]:completion_c_original[i],file)>=0;ok &= fclose(file)==0;if(!ok)return false;}
    SDL_SetWindowSize(a->window,1080,740);return true;
}
static bool completion_test_equal(struct nk_text_edit *edit,const char *text) {
    size_t length=strlen(text);const char *actual=nk_str_get_const(&edit->string);
    return nk_str_len_char(&edit->string)==(int)length && (!length || (actual && !memcmp(actual,text,length)));
}
static void completion_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    static unsigned target,seeks;static bool prepared;bool language=!strcmp(mode,"completion-phys"),ok=true;
    const char *original=language?completion_ph_original[target]:completion_c_original[target];
    const char *changed=language?completion_ph_changed[target]:completion_c_changed[target];
    workspace_document *doc=active_document(a);
    struct nk_text_edit *edit=target==0?&a->experiment:target==1?&a->analysis:doc?&doc->edit:NULL;
    if(a->library_thread)return;
    switch((*stage)++) {
    case 0:
        if(!prepared){ok=open_project(a);prepared=ok;}
        seeks=0;
        if(target==2){char path[4096];join(path,sizeof path,a->project,language?"completion-note.phys":"completion-note.c");preview_workspace_path(a,path);}
        else {a->tab=target?2:0;if(target)a->analysis_tab=1;}
        break;
    case 1:ok=edit!=NULL;test_mouse(a,nk_rect(a->source_editor_bounds.x+20,a->source_editor_bounds.y+20,2,2),true);break;
    case 2:test_mouse(a,nk_rect(a->source_editor_bounds.x+20,a->source_editor_bounds.y+20,2,2),false);break;
    case 3:
        ok=completion_test_equal(edit,original);edit->cursor=nk_utf_len(original,(int)strlen(original)-4);edit->select_start=edit->select_end=edit->cursor;
        if(target==2){test_mouse(a,a->completion_button_bounds,true);*stage=35;}
        else test_window_key_mod(a->window,SDLK_SPACE,SDL_KMOD_CTRL);
        break;
    case 35:test_mouse(a,a->completion_button_bounds,false);break;
    case 36:break;
    case 37:*stage=4;break;
    case 4: {
        const char *desired=language?(target==0?"Vec3":target==1?"Mat4.translation":"Vec3.normalized"):(target==0?"ps_vlength":target==1?"PS_METRE":"ps_memory_alignment");
        ok=a->completion_active && a->completion_count>0;
        if(ok && strcmp(ps_completion_at(a->completion_indices[a->completion_selected])->name,desired)) {
            if(++seeks>ps_completion_count()){ok=false;break;}
            test_window_key_mod(a->window,SDLK_DOWN,0);--*stage;return;
        }
        break;
    }
    case 5:
        *capture=target==0?"completion-experiment.bmp":target==1?"completion-analysis.bmp":"completion-document.bmp";
        if(target==2)test_mouse(a,a->completion_choice_bounds[a->completion_selected%4],true);
        else test_window_key_mod(a->window,target?SDLK_TAB:SDLK_RETURN,0);
        break;
    case 6:if(target==2){test_mouse(a,a->completion_choice_bounds[a->completion_selected%4],false);*stage=16;}break;
    case 16:*stage=17;break;
    case 17:*stage=7;break;
    case 7:
        ok=completion_test_equal(edit,changed) && !a->completion_active && (target==0?a->dirty:target==1?a->analysis_dirty:doc->dirty);
        test_window_key_mod(a->window,SDLK_Z,PS_UI_COMMAND_MOD);break;
    case 8:break;
    case 9:ok=completion_test_equal(edit,original);test_window_key_mod(a->window,SDLK_Z,PS_UI_COMMAND_MOD|SDL_KMOD_SHIFT);break;
    case 10:break;
    case 11:ok=completion_test_equal(edit,changed);test_window_key_mod(a->window,SDLK_Z,PS_UI_COMMAND_MOD);break;
    case 12:break;
    case 13:ok=completion_test_equal(edit,original);if(target<2){target++;*stage=0;}break;
    case 14:
        {const char *text=language?"// α\nMat":"// α\nps_v";ok=set_editor_text(edit,text,strlen(text));
        edit->cursor=nk_utf_len(text,(int)strlen(text));edit->select_start=edit->select_end=edit->cursor;}
        test_window_key_mod(a->window,SDLK_SPACE,SDL_KMOD_CTRL);break;
    case 15: {
        ok=a->completion_active;SDL_Event event={0};event.type=SDL_EVENT_TEXT_INPUT;
        event.text.windowID=SDL_GetWindowID(a->window);event.text.text=language?"4.trans":"leng";SDL_PushEvent(&event);
        test_window_key_mod(a->window,SDLK_RETURN,0);*stage=22;break;
    }
    case 22:*stage=23;break;
    case 23:
        ok=!a->completion_active && completion_test_equal(edit,language?"// α\nMat4.translation":"// α\nps_vlength");
        test_window_key_mod(a->window,SDLK_Z,PS_UI_COMMAND_MOD);break;
    case 24:*stage=25;break;
    case 25:
        ok=completion_test_equal(edit,language?"// α\nMat4.trans":"// α\nps_vleng");
        test_window_key_mod(a->window,SDLK_Z,PS_UI_COMMAND_MOD|SDL_KMOD_SHIFT);break;
    case 26:break;
    case 27:
        ok=completion_test_equal(edit,language?"// α\nMat4.translation":"// α\nps_vlength");
        test_window_key_mod(a->window,SDLK_SPACE,SDL_KMOD_CTRL);break;
    case 28:ok=a->completion_active;test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 29:break;
    case 30:test_window_key_mod(a->window,SDLK_Z,PS_UI_COMMAND_MOD);break;
    case 31:break;
    case 32:ok=completion_test_equal(edit,language?"// α\nMat4.trans":"// α\nps_vleng");break;
    case 33:
        ok=set_editor_text(edit,"\"ps_vl\"",7);edit->cursor=6;edit->select_start=edit->select_end=6;
        test_window_key_mod(a->window,SDLK_SPACE,SDL_KMOD_CTRL);break;
    case 34:
        ok=!a->completion_active && completion_test_equal(edit,"\"ps_vl\"");
        ok &= set_editor_text(edit,"",0);edit->cursor=edit->select_start=edit->select_end=0;
        test_window_key_mod(a->window,SDLK_SPACE,SDL_KMOD_CTRL);*stage=38;break;
    case 38:ok=a->completion_active && a->completion_count>0;test_window_key_mod(a->window,SDLK_ESCAPE,0);break;
    case 39:ok=!a->completion_active && completion_test_equal(edit,"");a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Completion UI target=%u stage=%d active=%u cursor=%d text=%s\n",target,*stage-1,a->completion_active,edit?edit->cursor:-1,edit?nk_str_get_const(&edit->string):"missing");*code=1;a->quitting=true;}
    if(a->quitting)printf("COMPLETION SELF-TEST: %s\n",*code?"FAILED":"PASSED");
}
