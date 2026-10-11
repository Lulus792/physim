static const char *editor_replace_original="// Präfix α 😀\nconst char *label = \"ALT\";\n// ALT\n";
static const char *editor_replace_changed="// Präfix α 😀\nconst char *label = \"ä😀LONG\";\n// ALT\n";
static const char *editor_replace_deleted="// Präfix α 😀\nconst char *label = \"ä😀LONG\";\n// \n";
static bool editor_replace_start(app *a,const char *root) {
    join(a->project_input,sizeof a->project_input,root,"project");a->example=0;new_project(a);
    if(!a->loaded)return false;
    const char *names[]={"main.c","analysis.c","notes α.c"};
    for(unsigned i=0;i<3;i++) {
        char path[4096];join(path,sizeof path,a->project,names[i]);FILE *file=fopen(path,"wb");
        if(!file)return false;bool ok=fputs(editor_replace_original,file)>=0;ok &= fclose(file)==0;if(!ok)return false;
    }
    return true;
}
static bool editor_replace_equal(struct nk_text_edit *edit,const char *text) {
    int length=nk_str_len_char(&edit->string);return length==(int)strlen(text) &&
        !memcmp(nk_str_get_const(&edit->string),text,(size_t)length);
}
static void editor_replace_frame(app *a,int *stage,int *code,const char **capture) {
    static unsigned target;static bool prepared;bool ok=true;
    workspace_document *document=active_document(a);
    struct nk_text_edit *edit=target==0?&a->experiment:target==1?&a->analysis:document?&document->edit:NULL;
    if(!edit){*code=1;a->quitting=true;printf("EDITOR REPLACE SELF-TEST: FAILED\n");return;}
    struct nk_rect focus=nk_rect(a->source_editor_bounds.x+20,a->source_editor_bounds.y+20,2,2);
    switch(*stage) {
    case 0:
        if(!prepared){if(a->library_thread)return;ok=open_project(a);prepared=ok;}
        ok=ok && editor_replace_equal(edit,editor_replace_original);
        test_window_key_mod(a->window,SDLK_F,PS_UI_COMMAND_MOD);break;
    case 1:
        ok=a->show_search;strcpy(a->find,"ALT");strcpy(a->replace,"ä😀LONG");
        break;
    case 2:ok=a->search_bounds.w>0;test_mouse(a,a->search_bounds,true);break;
    case 3:test_mouse(a,a->search_bounds,false);break;
    case 4:
        ok=edit->select_end-edit->select_start==3;
        test_mouse(a,a->replace_bounds,true);break;
    case 5:test_mouse(a,a->replace_bounds,false);break;
    case 6:
        ok=editor_replace_equal(edit,editor_replace_changed);
        test_mouse(a,focus,true);break;
    case 7:test_mouse(a,focus,false);break;
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
        *capture=target==0?"replace-experiment.bmp":target==1?"replace-analysis.bmp":"replace-document.bmp";
        break;
    case 21:
        if(++target==3){a->quitting=true;break;}
        if(target==1){a->tab=2;a->analysis_tab=1;}else {char path[4096];join(path,sizeof path,a->project,"notes α.c");preview_workspace_path(a,path);ok=a->tab==8 && active_document(a) && !active_document(a)->draft_blocked && !active_document(a)->draft_error[0];}
        if(!ok)break;
        a->show_search=false;*stage=0;return;
    }
    if(!ok){fprintf(stderr,"Editor replace target %u stage %d: %s\n",target,*stage,nk_str_get_const(&edit->string));*code=1;a->quitting=true;}
    if(a->quitting)printf("EDITOR REPLACE SELF-TEST: %s\n",*code?"FAILED":"PASSED");
    (*stage)++;
}
