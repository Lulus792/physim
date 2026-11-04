static bool completion_info_painted;
static unsigned completion_info_draw_generation,completion_info_wait_generation;
static void completion_info_test_draw(app *a){
    completion_info_draw_generation++;
    completion_info_painted=false;struct nk_window *w=nk_window_find(a->ui,"api-completion-info");
    if(!w || w->seq!=a->ui->seq || (w->flags&NK_WINDOW_HIDDEN))return;
    const struct nk_command *cmd;
    nk_foreach(cmd,a->ui){nk_size at=(nk_size)((const char*)cmd-(const char*)a->ui->memory.memory.ptr);
        if(cmd->type==NK_COMMAND_TEXT && at>=w->buffer.begin && at<w->buffer.end)completion_info_painted=true;}
}
static bool completion_info_seed(app *a,struct nk_text_edit *edit,const char *text){
    completion_close(a);if(!edit || !set_editor_text(edit,text,strlen(text)))return false;
    edit->cursor=edit->select_start=edit->select_end=nk_str_len(&edit->string);completion_return_focus(a,edit);
    completion_info_wait_generation=completion_info_draw_generation+1;return true;
}
static void completion_info_test_frame(app *a,const char *mode,int *stage,int *code){
    static unsigned target,early;static bool prepared,old_deadline_seen;static double timeout,issued,due,old_due,hold;
    static char expected[256];static size_t chosen;static int cursor;static struct nk_vec2 editor_scroll;static struct nk_rect old_card;
    bool language=strstr(mode,"phys")!=NULL,ok=true;workspace_document *doc=active_document(a);
    struct nk_text_edit *edit=target==0?&a->experiment:target==1?&a->analysis:doc?&doc->edit:NULL;
    if(a->library_thread)return;
    if(completion_info_draw_generation<completion_info_wait_generation)return;
#define INFO_WAIT(x) do{if(!(x)){if(ps_clock()<timeout)return;ok=false;goto finished;}}while(0)
#define INFO_NEXT(n) do{*stage=(n);timeout=ps_clock()+4;}while(0)
#define INFO_KEY(key,mods) do{test_window_key_mod(a->window,key,mods);completion_info_wait_generation=completion_info_draw_generation+2;}while(0)
#define INFO_MOUSE(bounds,down) do{test_mouse(a,bounds,down);completion_info_wait_generation=completion_info_draw_generation+2;}while(0)
    switch(*stage){
    case 0:
        if(!prepared){ok=open_project(a);prepared=ok;}
        {ps_preferences p=a->preferences;p.theme=target;p.completion_rows=5;p.ui_size=p.editor_size=target==1?22:16;if(target!=1)p.editor_size=12;preferences_apply(a,&p);}
        if(target==2){char path[4096];join(path,sizeof path,a->project,language?"completion-note.phys":"completion-note.c");preview_workspace_path(a,path);}
        else{a->tab=target?2:0;if(target)a->analysis_tab=1;}
        snprintf(expected,sizeof expected,"// alpha\n%s",language?"direction.norm":"ps_v");completion_info_wait_generation=completion_info_draw_generation+1;INFO_NEXT(1);break;
    case 1:ok=completion_info_seed(a,edit,expected);INFO_NEXT(2);break;
    case 2:INFO_WAIT(edit && edit->active);INFO_KEY(SDLK_SPACE,SDL_KMOD_CTRL);INFO_NEXT(3);break;
    case 3:
        INFO_WAIT(a->completion_active && a->completion_list_bounds.w>0);
        ok=a->completion_explicit && !a->completion_info_visible && !completion_info_painted && !a->completion_info_bounds.w && !a->completion_help.document;
        chosen=a->completion_selected;INFO_KEY(SDLK_SPACE,SDL_KMOD_CTRL);INFO_NEXT(4);break;
    case 4:
        INFO_WAIT(a->completion_info_visible && completion_info_painted);
        ok=a->completion_selected==chosen && a->completion_explicit && completion_test_equal(edit,expected);
        old_card=a->completion_info_bounds;INFO_KEY(SDLK_SPACE,SDL_KMOD_CTRL);INFO_NEXT(5);break;
    case 5:
        INFO_WAIT(!a->completion_info_visible && !completion_info_painted);
        ok=!a->completion_info_bounds.w;
        INFO_MOUSE(nk_rect(old_card.x+old_card.w-4,old_card.y+3,1,1),true);INFO_NEXT(6);break;
    case 6:INFO_MOUSE(nk_rect(old_card.x+old_card.w-4,old_card.y+3,1,1),false);INFO_NEXT(7);break;
    case 7:
        INFO_WAIT(!a->completion_active);ok=completion_test_equal(edit,expected);
        ok=ok && completion_info_seed(a,edit,expected);INFO_NEXT(8);break;
    case 8:INFO_WAIT(edit->active);INFO_KEY(SDLK_SPACE,SDL_KMOD_CTRL);INFO_NEXT(9);break;
    case 9:INFO_WAIT(a->completion_active);issued=ps_clock();early=0;INFO_KEY(SDLK_DOWN,0);INFO_NEXT(10);break;
    case 10:
        INFO_WAIT(a->completion_info.mode==PS_COMPLETION_INFO_PENDING);
        due=a->completion_info.due;ok=due-issued>=.280 && !a->completion_info_visible && !completion_info_painted;
        early++;INFO_NEXT(11);break;
    case 11:
        if(ps_clock()<due){ok=!a->completion_info_visible && !completion_info_painted;early++;break;}
        INFO_WAIT(a->completion_info_visible && completion_info_painted);
        ok=early>0 && a->completion_help.symbol==ps_completion_at(a->completion_indices[a->completion_selected]);
        INFO_KEY(SDLK_DOWN,0);INFO_NEXT(12);break;
    case 12:INFO_WAIT(a->completion_info.mode==PS_COMPLETION_INFO_PENDING);old_due=a->completion_info.due;hold=ps_clock()+.100;INFO_NEXT(13);break;
    case 13:
        if(ps_clock()<hold){ok=!a->completion_info_visible;break;}
        INFO_KEY(SDLK_DOWN,0);INFO_NEXT(14);break;
    case 14:
        INFO_WAIT(a->completion_info.mode==PS_COMPLETION_INFO_PENDING);
        due=a->completion_info.due;ok=due>old_due+.070;old_deadline_seen=false;INFO_NEXT(15);break;
    case 15:
        if(ps_clock()<due){ok=!a->completion_info_visible && !completion_info_painted;if(ps_clock()>=old_due)old_deadline_seen=true;break;}
        INFO_WAIT(a->completion_info_visible && completion_info_painted);
        ok=old_deadline_seen && completion_test_equal(edit,expected);
        printf("COMPLETION INFO target=%u manual-toggle=1 hidden-pointer=1 delay-ms=280 early-samples=%u rapid-reselection=1\n",target,early);
        chosen=a->completion_selected;
        {size_t row=a->completion_first==chosen?1:0;ok=ok && row<a->completion_visible_rows;
        struct nk_rect r=a->completion_choice_bounds[row];test_plot_motion(a,r.x+60,r.y+r.h/2);completion_info_wait_generation=completion_info_draw_generation+2;}
        INFO_NEXT(40);break;
    case 40:
        INFO_WAIT(a->completion_selected!=chosen && !a->completion_explicit && a->completion_info.mode==PS_COMPLETION_INFO_PENDING);
        due=a->completion_info.due;INFO_NEXT(41);break;
    case 41:
        if(ps_clock()<due){ok=!a->completion_info_visible && !completion_info_painted;break;}
        INFO_WAIT(a->completion_info_visible && completion_info_painted);
        ok=!a->completion_explicit && a->completion_help.symbol==ps_completion_at(a->completion_indices[a->completion_selected]);
        INFO_KEY(SDLK_RETURN,0);INFO_NEXT(42);break;
    case 42:{
        char appended[sizeof expected+2];snprintf(appended,sizeof appended,"%s\n",expected);
        INFO_WAIT(!a->completion_active && completion_test_equal(edit,appended));
        printf("COMPLETION INFO target=%u hover-preview=1 no-keyboard-accept=1\n",target);
        ok=completion_info_seed(a,edit,expected);INFO_NEXT(43);break;}
    case 43:INFO_WAIT(edit->active);INFO_KEY(SDLK_SPACE,SDL_KMOD_CTRL);INFO_NEXT(44);break;
    case 44:INFO_WAIT(a->completion_active);INFO_KEY(SDLK_DOWN,0);INFO_NEXT(16);break;
    case 16:
        INFO_WAIT(a->completion_info.mode==PS_COMPLETION_INFO_PENDING);
        expected[3]='b';ok=set_editor_text(edit,expected,strlen(expected));edit->cursor=edit->select_start=edit->select_end=nk_str_len(&edit->string);
        completion_info_wait_generation=completion_info_draw_generation+1;
        hold=ps_clock()+.350;INFO_NEXT(17);break;
    case 17:
        if(ps_clock()<hold){ok=!a->completion_info_visible && !completion_info_painted;break;}
        ok=a->completion_active && a->completion_info.mode==PS_COMPLETION_INFO_HIDDEN && !a->completion_info.text && !a->completion_info_bounds.w;
        INFO_KEY(SDLK_UP,0);INFO_NEXT(18);break;
    case 18:
        INFO_WAIT(a->completion_info.mode==PS_COMPLETION_INFO_PENDING);
        edit->cursor--;edit->select_start=edit->select_end=edit->cursor;completion_info_wait_generation=completion_info_draw_generation+1;hold=ps_clock()+.350;INFO_NEXT(19);break;
    case 19:
        if(ps_clock()<hold){ok=!a->completion_info_visible && !completion_info_painted;break;}
        ok=!a->completion_active && !a->completion_info.text;
        snprintf(expected,sizeof expected,"%s",language?"Mat4.trans":"ps_channel_add");ok=ok && completion_info_seed(a,edit,expected);INFO_NEXT(20);break;
    case 20:INFO_WAIT(edit->active);INFO_KEY(SDLK_SPACE,SDL_KMOD_CTRL);INFO_NEXT(21);break;
    case 21:INFO_WAIT(a->completion_active);INFO_KEY(SDLK_SPACE,SDL_KMOD_CTRL);INFO_NEXT(22);break;
    case 22:
        INFO_WAIT(a->completion_info_visible && completion_info_painted);
        chosen=a->completion_selected;cursor=edit->cursor;editor_scroll=edit->scrollbar;
        INFO_KEY(SDLK_PAGEDOWN,SDL_KMOD_ALT);INFO_NEXT(23);break;
    case 23:
        ok=a->completion_active && a->completion_info_visible && a->completion_selected==chosen && edit->cursor==cursor && completion_test_equal(edit,expected);
        ok=ok && edit->scrollbar.x==editor_scroll.x && edit->scrollbar.y==editor_scroll.y;
        if(a->completion_help_natural_height>a->completion_help_body_bounds.h+2)ok=ok && a->completion_help_scroll_y>0;
        INFO_KEY(SDLK_END,SDL_KMOD_ALT);INFO_NEXT(24);break;
    case 24:
        INFO_WAIT(completion_reference_visible(a));ok=a->completion_active && a->completion_info_visible && edit->cursor==cursor;
        INFO_KEY(SDLK_PAGEUP,SDL_KMOD_ALT);INFO_NEXT(25);break;
    case 25:
        ok=a->completion_active && edit->cursor==cursor;INFO_KEY(SDLK_HOME,SDL_KMOD_ALT);INFO_NEXT(26);break;
    case 26:
        INFO_WAIT(a->completion_help_scroll_y==0);ok=a->completion_selected==chosen && completion_test_equal(edit,expected);
        INFO_MOUSE(a->completion_info_button_bounds,true);INFO_NEXT(27);break;
    case 27:INFO_MOUSE(a->completion_info_button_bounds,false);INFO_NEXT(28);break;
    case 28:
        INFO_WAIT(!a->completion_info_visible && !completion_info_painted);hold=ps_clock()+.350;INFO_NEXT(29);break;
    case 29:
        if(ps_clock()<hold){ok=!a->completion_info_visible;break;}
        ok=a->completion_active && !a->completion_info_bounds.w;
        INFO_MOUSE(a->completion_info_button_bounds,true);INFO_NEXT(30);break;
    case 30:INFO_MOUSE(a->completion_info_button_bounds,false);INFO_NEXT(31);break;
    case 31:
        INFO_WAIT(a->completion_info_visible && completion_info_painted);
        ok=completion_test_equal(edit,expected) && a->completion_selected==chosen;
        printf("COMPLETION INFO target=%u full-source-cancel=1 cursor-cancel=1 keyboard-scroll=1 i-toggle=1 source-preserved=1\n",target);
        INFO_KEY(SDLK_TAB,SDL_KMOD_SHIFT);INFO_NEXT(32);break;
    case 32:
        INFO_WAIT(!a->completion_active);ok=completion_test_equal(edit,expected) && !edit->active;
        printf("COMPLETION INFO target=%u modified-tab-no-accept=1\n",target);
        if(target<2){target++;completion_close(a);INFO_NEXT(0);}else{a->quitting=true;}
        break;
    }
finished:
    if(!ok){fprintf(stderr,"Completion info target=%u stage=%d active=%d visible=%d painted=%d mode=%d due=%.6f now=%.6f info=%.1f/%.1f scroll=%u cursor=%d\n",target,*stage,a->completion_active,a->completion_info_visible,completion_info_painted,a->completion_info.mode,a->completion_info.due,ps_clock(),a->completion_info_bounds.w,a->completion_info_bounds.h,a->completion_help_scroll_y,edit?edit->cursor:-1);*code=1;a->quitting=true;}
    if(a->quitting)printf("COMPLETION INFO SELF-TEST: %s\n",*code?"FAILED":"PASSED");
#undef INFO_WAIT
#undef INFO_NEXT
#undef INFO_KEY
#undef INFO_MOUSE
}
