static const char *editor_replace_original="// Präfix α 😀\nconst char *label = \"ALT\";\n// ALT\n";
static const char *editor_replace_changed="// Präfix α 😀\nconst char *label = \"ä😀LONG\";\n// ALT\n";
static const char *editor_replace_deleted="// Präfix α 😀\nconst char *label = \"ä😀LONG\";\n// \n";
static bool editor_replace_start(app *a,const char *root,const char *mode) {
    bool language=strstr(mode,"phys")!=NULL;
    join(a->project_input,sizeof a->project_input,root,"project");a->example=language?8:0;a->template_analysis_language=language?1:0;new_project(a);
    if(!a->loaded)return false;
    const char *names[]={experiment_source(a),analysis_source(a),"notes α.txt"};
    for(unsigned i=0;i<3;i++) {
        char path[4096];join(path,sizeof path,a->project,names[i]);FILE *file=fopen(path,"wb");
        if(!file)return false;bool ok=fputs(editor_replace_original,file)>=0;ok &= fclose(file)==0;if(!ok)return false;
    }
    return true;
}
static bool editor_replace_equal(struct nk_text_edit *edit,const char *text) {
    int length=nk_str_len_char(&edit->string);return length==(int)strlen(text) &&
        (!length || !memcmp(nk_str_get_const(&edit->string),text,(size_t)length));
}
static void editor_replace_frame(app *a,int *stage,int *code,const char **capture) {
    static unsigned target;static bool prepared;bool ok=true;
    workspace_document *document=active_document(a);
    struct nk_text_edit *edit=target==0?&a->experiment:target==1?&a->analysis:document?&document->edit:NULL;
    if(!edit){*code=1;a->quitting=true;printf("EDITOR REPLACE SELF-TEST: FAILED\n");return;}
    struct nk_rect focus=nk_rect(a->source_editor_bounds.x+20,a->source_editor_bounds.y+150,2,2);
    switch(*stage) {
    case 0:
        if(!prepared){if(a->library_thread)return;ok=open_project(a);prepared=ok;}
        ok=ok && editor_replace_equal(edit,editor_replace_original);
        test_window_key_mod(a->window,SDLK_F,PS_UI_COMMAND_MOD);break;
    case 1:
        if(a->show_search && a->search_expand_bounds.w<=0)return;
        ok=a->show_search;strcpy(a->find,"ALT");strcpy(a->replace,"ä😀LONG");
        if(!a->search_replace_open){test_mouse(a,a->search_expand_bounds,true);*stage=30;return;}
        break;
    case 30:test_mouse(a,a->search_expand_bounds,false);*stage=2;return;
    case 2:ok=a->search_bounds.w>0;test_mouse(a,a->search_bounds,true);break;
    case 3:test_mouse(a,a->search_bounds,false);break;
    case 4:
        ok=edit->select_end-edit->select_start==3;
        test_mouse(a,a->replace_bounds,true);break;
    case 5:test_mouse(a,a->replace_bounds,false);break;
    case 6:
        ok=editor_replace_equal(edit,editor_replace_changed);
        test_mouse(a,focus,true);break;
    case 7:
        test_mouse(a,focus,false);
        /* Hovering the island must not take the keyboard back from the code. */
        test_plot_motion(a,a->search_query_bounds.x+8,a->search_query_bounds.y+8);break;
    case 8:test_window_key_mod(a->window,SDLK_Z,PS_UI_COMMAND_MOD);break;
    case 9:break;
    case 10:
        ok=editor_replace_equal(edit,editor_replace_original);
        test_window_key_mod(a->window,SDLK_Z,PS_UI_COMMAND_MOD|SDL_KMOD_SHIFT);break;
    case 11:break;
    case 12:
        ok=editor_replace_equal(edit,editor_replace_changed);
        edit->cursor=edit->string.len;edit->select_start=edit->select_end=edit->cursor;
        strcpy(a->replace,"");test_mouse(a,a->replace_bounds,true);break;
    case 13:test_mouse(a,a->replace_bounds,false);break;
    case 14:
        /* No selection: search wraps to the first ALT still present. */
        ok=editor_replace_equal(edit,editor_replace_deleted);test_mouse(a,focus,true);break;
    case 15:test_mouse(a,focus,false);break;
    case 16:test_window_key_mod(a->window,SDLK_Z,PS_UI_COMMAND_MOD);break;
    case 17:break;
    case 18:
        ok=editor_replace_equal(edit,editor_replace_changed);
        test_window_key_mod(a->window,SDLK_Z,PS_UI_COMMAND_MOD|SDL_KMOD_SHIFT);break;
    case 19:break;
    case 20:
        ok=editor_replace_equal(edit,editor_replace_deleted);
        ok=ok && (target==0?a->dirty:target==1?a->analysis_dirty:document->dirty);
        *capture=target==0?"replace-experiment.bmp":target==1?"replace-analysis.bmp":"replace-document.bmp";
        test_key(a,SDLK_S);
        break;
    case 21:break;
    case 22:
        ok=!(target==0?a->dirty:target==1?a->analysis_dirty:document->dirty);break;
    case 23:
        if(++target==3){a->quitting=true;break;}
        if(target==1){a->tab=2;a->analysis_tab=1;}else {char path[4096];join(path,sizeof path,a->project,"notes α.txt");preview_workspace_path(a,path);ok=a->tab==8 && active_document(a) && !active_document(a)->draft_blocked && !active_document(a)->draft_error[0];}
        if(!ok)break;
        a->show_search=false;*stage=0;return;
    }
    if(!ok){fprintf(stderr,"Editor replace target %u stage %d: %.*s\nselection %d..%d cursor %d replace button %.1f/%.1f/%.1f/%.1f island %.1f/%.1f expanded %d field '%s'\n",target,*stage,nk_str_len_char(&edit->string),nk_str_get_const(&edit->string),edit->select_start,edit->select_end,edit->cursor,a->replace_bounds.x,a->replace_bounds.y,a->replace_bounds.w,a->replace_bounds.h,a->search_island_bounds.y,a->search_island_bounds.h,a->search_replace_open,a->replace);*capture="replace-failure.bmp";*code=1;a->quitting=true;}
    if(a->quitting)printf("EDITOR REPLACE SELF-TEST: %s\n",*code?"FAILED":"PASSED");
    (*stage)++;
}
