static const char *api_reference_c_original[]={"// α😀\nps_vlength\n","// α😀\nPS_METRE\n","// α😀\nps_memory_alignment\n"};
static const char *api_reference_ph_original[]={"// α😀\nVec3\n","// α😀\nMat4.translation\n","// α😀\ndirection.normalized\n"};
static bool api_reference_test_start(app *a,const char *root,const char *mode) {
    bool language=!strcmp(mode,"api-reference-phys");
    if(!completion_test_start(a,root,language?"completion-phys":"completion-c"))return false;
    const char *names[]={experiment_source(a),analysis_source(a),language?"completion-note.phys":"completion-note.c"};
    for(unsigned i=0;i<3;i++){char path[4096];join(path,sizeof path,a->project,names[i]);FILE *file=fopen(path,"wb");
        if(!file)return false;bool ok=fputs(language?api_reference_ph_original[i]:api_reference_c_original[i],file)>=0;ok &= fclose(file)==0;if(!ok)return false;}
    return true;
}
static void api_reference_test_frame(app *a,const char *mode,int *stage,int *code,const char **capture) {
    static unsigned target,seeks;static bool prepared;
    bool language=!strcmp(mode,"api-reference-phys"),ok=true;
    const char *original=language?api_reference_ph_original[target]:api_reference_c_original[target];
    const char *desired=language?(target==0?"Vec3":target==1?"Mat4.translation":"Vec3.normalized"):
        target==0?"ps_vlength":target==1?"PS_METRE":"ps_memory_alignment";
    workspace_document *doc=active_document(a);struct nk_text_edit *edit=target==0?&a->experiment:target==1?&a->analysis:doc?&doc->edit:NULL;
    if(a->library_thread)return;
    switch((*stage)++) {
    case 0:
        if(!prepared){ok=open_project(a);prepared=ok;}
        seeks=0;
        if(target==2){char path[4096];join(path,sizeof path,a->project,language?"completion-note.phys":"completion-note.c");preview_workspace_path(a,path);}
        else {a->tab=target?2:0;if(target)a->analysis_tab=1;}
        break;
    case 1:test_mouse(a,nk_rect(a->source_editor_bounds.x+20,a->source_editor_bounds.y+20,2,2),true);break;
    case 2:test_mouse(a,nk_rect(a->source_editor_bounds.x+20,a->source_editor_bounds.y+20,2,2),false);break;
    case 3:
        ok=edit && completion_test_equal(edit,original);edit->cursor=nk_utf_len(original,(int)strlen(original)-4);edit->select_start=edit->select_end=edit->cursor;
        if(!target){test_window_key_mod(a->window,SDLK_F1,SDL_KMOD_SHIFT);*stage=18;}
        else if(target==1){test_mouse(a,a->api_reference_button_bounds,true);*stage=7;}
        else test_window_key_mod(a->window,SDLK_SPACE,SDL_KMOD_CTRL);
        break;
    case 4:
        ok=a->completion_active;
        if(ok && strcmp(ps_completion_at(a->completion_indices[a->completion_selected])->name,desired)) {
            if(++seeks>ps_completion_count()){ok=false;break;}test_window_key_mod(a->window,SDLK_DOWN,0);--*stage;return;
        }
        *stage=19;break;
    case 19:ok=a->completion_reference_bounds.w>0;test_mouse(a,a->completion_reference_bounds,true);break;
    case 20:test_mouse(a,a->completion_reference_bounds,false);break;
    case 21:*stage=9;break;
    case 7:test_mouse(a,a->api_reference_button_bounds,false);break;
    case 8:break;
    case 18:*stage=9;break;
    case 9:
        ok=a->api_reference && !strcmp(a->api_reference_name,desired) && completion_test_equal(edit,original) &&
            !a->dirty && !a->analysis_dirty && (!doc || !doc->dirty) && a->api_reference_bounds.w>0 &&
            a->api_reference_first<a->api_reference_end && a->api_reference_end<=a->api_reference->count;
        *capture=target==0?"api-reference-experiment.bmp":target==1?"api-reference-analysis.bmp":"api-reference-document.bmp";
        if(target==2)*stage=22;else test_mouse(a,a->api_reference_handbook_bounds,true);
        break;
    case 22:test_mouse(a,nk_rect(a->source_editor_bounds.x+20,a->source_editor_bounds.y+20,2,2),true);break;
    case 23:test_mouse(a,nk_rect(a->source_editor_bounds.x+20,a->source_editor_bounds.y+20,2,2),false);break;
    case 24:
        edit->cursor=nk_utf_len(original,(int)strlen(original));edit->select_start=edit->select_end=edit->cursor;
        {SDL_Event event={0};event.type=SDL_EVENT_TEXT_INPUT;event.text.windowID=SDL_GetWindowID(a->window);event.text.text="x";SDL_PushEvent(&event);}break;
    case 25:break;
    case 26:
        {char changed[256];snprintf(changed,sizeof changed,"%sx",original);ok=completion_test_equal(edit,changed) && doc->dirty && a->api_reference && !strcmp(a->api_reference_name,desired);}
        test_window_key_mod(a->window,SDLK_Z,PS_UI_COMMAND_MOD);break;
    case 27:break;
    case 28:
        ok=completion_test_equal(edit,original) && !doc->dirty && a->api_reference && !strcmp(a->api_reference_name,desired);
        test_mouse(a,a->api_reference_handbook_bounds,true);*stage=10;break;
    case 10:test_mouse(a,a->api_reference_handbook_bounds,false);break;
    case 11:break;
    case 12:
        ok=a->doc_visible && a->documentation && a->documentation!=a->api_reference &&
            !strcmp(documentation_topics[a->doc_topic].path,a->api_reference_path) && !strcmp(a->api_reference_name,desired) &&
            completion_test_equal(edit,original);
        if(a->doc_jump_block>=0){--*stage;return;}
        ok &= a->doc_scroll_y>0;
        test_window_key_mod(a->doc_window,SDLK_ESCAPE,0);break;
    case 13:ok=!a->doc_visible && a->api_reference && !strcmp(a->api_reference_name,desired);test_mouse(a,a->api_reference_close_bounds,true);break;
    case 14:test_mouse(a,a->api_reference_close_bounds,false);break;
    case 15:break;
    case 16:
        ok=!a->api_reference && completion_test_equal(edit,original);
        if(target<2){target++;*stage=0;}else a->quitting=true;
        break;
    }
    if(!ok){fprintf(stderr,"API reference UI target=%u stage=%d name=%s tab=%d status=%s\n",target,*stage-1,a->api_reference_name,a->tab,a->status);*code=1;a->quitting=true;}
    if(a->quitting)printf("API REFERENCE SELF-TEST: %s\n",*code?"FAILED":"PASSED");
}
