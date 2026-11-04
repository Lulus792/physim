/* Real SDL input through the existing editors; no extra raster captures. */
static void completion_auto_text(app *a,const char *text) {
    SDL_Event event={0};event.type=SDL_EVENT_TEXT_INPUT;
    event.text.windowID=SDL_GetWindowID(a->window);event.text.text=text;SDL_PushEvent(&event);
}
static bool completion_auto_seed(app *a,const char *text) {
    completion_close(a);a->tab=0;
    if(!set_editor_text(&a->experiment,text,strlen(text)))return false;
    a->experiment.cursor=nk_str_len(&a->experiment.string);
    a->experiment.select_start=a->experiment.select_end=a->experiment.cursor;
    a->completion_focus_edit=&a->experiment;return true;
}
static void completion_auto_frame(app *a,const char *mode,int *stage,int *code) {
    static double deadline,hold;static char expected[256];
    bool language=strstr(mode,"phys")!=NULL,ok=true;
    const char *prefix=language?"Ve":"ps",*tail=language?"c":"_";
    struct nk_text_edit *edit=&a->experiment;
#define AUTO_WAIT(condition) do{if(!(condition)){if(ps_clock()<deadline)return;ok=false;goto finished;}}while(0)
#define AUTO_NEXT(next) do{*stage=(next);deadline=ps_clock()+3;}while(0)
#define AUTO_KEY(key,mod) test_window_key_mod(a->window,key,mod)
    switch(*stage){
    case 0:ok=completion_auto_seed(a,"");AUTO_NEXT(1);break;
    case 1:
        test_mouse(a,nk_rect(a->source_editor_bounds.x+20,a->source_editor_bounds.y+20,2,2),true);AUTO_NEXT(2);break;
    case 2:
        test_mouse(a,nk_rect(a->source_editor_bounds.x+20,a->source_editor_bounds.y+20,2,2),false);AUTO_NEXT(3);break;
    case 3:AUTO_WAIT(edit->active);completion_auto_text(a,language?"V":"p");AUTO_NEXT(4);break;
    case 4:
        AUTO_WAIT(completion_test_equal(edit,language?"V":"p"));
        ok=!a->completion_active && !a->completion_pending_edit;
        completion_auto_text(a,prefix+1);AUTO_NEXT(5);break;
    case 5:
        AUTO_WAIT(completion_test_equal(edit,prefix));
        ok=!a->completion_active && a->completion_pending_edit==edit;
        AUTO_NEXT(6);break;
    case 6:
        AUTO_WAIT(a->completion_active);
        ok=!a->completion_explicit && !a->completion_pending_edit && completion_test_equal(edit,prefix);
        AUTO_KEY(SDLK_RETURN,0);AUTO_NEXT(7);break;
    case 7:
        snprintf(expected,sizeof expected,"%s\n",prefix);AUTO_WAIT(completion_test_equal(edit,expected));
        ok=!a->completion_active;puts("AUTO COMPLETION enter-without-selection=1");
        ok=ok && completion_auto_seed(a,"");AUTO_NEXT(8);break;
    case 8:AUTO_WAIT(edit->active);completion_auto_text(a,prefix);AUTO_NEXT(9);break;
    case 9:AUTO_WAIT(a->completion_active);ok=!a->completion_explicit;AUTO_KEY(SDLK_TAB,0);AUTO_NEXT(10);break;
    case 10:
        snprintf(expected,sizeof expected,"%s    ",prefix);AUTO_WAIT(completion_test_equal(edit,expected));
        ok=!a->completion_active;puts("AUTO COMPLETION tab-without-selection=1");
        ok=ok && completion_auto_seed(a,"");AUTO_NEXT(11);break;
    case 11:AUTO_WAIT(edit->active);completion_auto_text(a,prefix);AUTO_NEXT(12);break;
    case 12:AUTO_WAIT(a->completion_active);AUTO_KEY(SDLK_ESCAPE,0);AUTO_NEXT(13);break;
    case 13:
        AUTO_WAIT(!a->completion_active && a->completion_suppressed_edit==edit);
        completion_auto_text(a,tail);hold=ps_clock()+.250;AUTO_NEXT(14);break;
    case 14:
        snprintf(expected,sizeof expected,"%s%s",prefix,tail);AUTO_WAIT(completion_test_equal(edit,expected));
        if(ps_clock()<hold)return;
        ok=!a->completion_active && !a->completion_pending_edit;
        AUTO_KEY(SDLK_SPACE,SDL_KMOD_CTRL);AUTO_NEXT(15);break;
    case 15:
        AUTO_WAIT(a->completion_active);ok=a->completion_explicit;
        snprintf(expected,sizeof expected,"%s",ps_completion_insertion(&a->completion_query,ps_completion_at(a->completion_indices[a->completion_selected])));
        AUTO_KEY(SDLK_RETURN,0);AUTO_NEXT(16);break;
    case 16:
        AUTO_WAIT(completion_test_equal(edit,expected));AUTO_KEY(SDLK_Z,PS_UI_COMMAND_MOD);AUTO_NEXT(17);break;
    case 17:
        snprintf(expected,sizeof expected,"%s%s",prefix,tail);AUTO_WAIT(completion_test_equal(edit,expected));
        puts("AUTO COMPLETION escape-suppression=1 manual-override=1 undo=1");
        ok=completion_auto_seed(a,"");AUTO_NEXT(18);break;
    case 18:AUTO_WAIT(edit->active);completion_auto_text(a,prefix);AUTO_NEXT(19);break;
    case 19:AUTO_WAIT(a->completion_active);ok=!a->completion_explicit;AUTO_KEY(SDLK_N,SDL_KMOD_CTRL);AUTO_NEXT(20);break;
    case 20:AUTO_WAIT(a->completion_explicit);ok=a->completion_selected==0;AUTO_KEY(SDLK_P,SDL_KMOD_CTRL);AUTO_NEXT(21);break;
    case 21:
        AUTO_WAIT(a->completion_selected==a->completion_count-1);
        snprintf(expected,sizeof expected,"%s",ps_completion_insertion(&a->completion_query,ps_completion_at(a->completion_indices[a->completion_selected])));
        AUTO_KEY(SDLK_RETURN,0);AUTO_NEXT(22);break;
    case 22:AUTO_WAIT(completion_test_equal(edit,expected));AUTO_KEY(SDLK_Z,PS_UI_COMMAND_MOD);AUTO_NEXT(23);break;
    case 23:
        AUTO_WAIT(completion_test_equal(edit,prefix));puts("AUTO COMPLETION explicit-control-n-p=1 undo=1");
        ok=completion_auto_seed(a,"");AUTO_NEXT(48);break;
    case 48:AUTO_WAIT(edit->active);completion_auto_text(a,prefix);AUTO_NEXT(49);break;
    case 49:AUTO_WAIT(a->completion_active);AUTO_KEY(SDLK_N,SDL_KMOD_CTRL);AUTO_NEXT(50);break;
    case 50:
        AUTO_WAIT(a->completion_explicit && a->completion_selected==0);
        completion_auto_text(a,language?"c3":"_v");AUTO_KEY(SDLK_RETURN,0);AUTO_NEXT(51);break;
    case 51:
        snprintf(expected,sizeof expected,"%s\n",language?"Vec3":"ps_v");AUTO_WAIT(completion_test_equal(edit,expected));
        ok=!a->completion_active;puts("AUTO COMPLETION changed-prefix-needs-selection=1");
        ok=ok && completion_auto_seed(a,"");AUTO_NEXT(52);break;
    case 52:AUTO_WAIT(edit->active);completion_auto_text(a,prefix);AUTO_NEXT(53);break;
    case 53:AUTO_WAIT(a->completion_active);AUTO_KEY(SDLK_BACKSPACE,0);hold=ps_clock()+.250;AUTO_NEXT(54);break;
    case 54:
        AUTO_WAIT(completion_test_equal(edit,language?"V":"p"));if(ps_clock()<hold)return;
        ok=!a->completion_active && !a->completion_pending_edit;
        puts("AUTO COMPLETION short-prefix-closes=1");ok=ok && completion_auto_seed(a,"");AUTO_NEXT(24);break;
    case 24:AUTO_WAIT(edit->active);completion_auto_text(a,prefix);AUTO_NEXT(25);break;
    case 25:AUTO_WAIT(a->completion_pending_edit==edit);AUTO_KEY(SDLK_LEFT,0);hold=ps_clock()+.250;AUTO_NEXT(26);break;
    case 26:
        AUTO_WAIT(edit->cursor==1);if(ps_clock()<hold)return;
        ok=!a->completion_active && !a->completion_pending_edit && completion_test_equal(edit,prefix);
        puts("AUTO COMPLETION stale-cursor=1");ok=ok && completion_auto_seed(a,"// header\n");AUTO_NEXT(27);break;
    case 27:AUTO_WAIT(edit->active);completion_auto_text(a,prefix);AUTO_NEXT(28);break;
    case 28:
        AUTO_WAIT(a->completion_pending_edit==edit);
        nk_str_get(&edit->string)[3]='H';hold=ps_clock()+.250;AUTO_NEXT(29);break;
    case 29:
        if(ps_clock()<hold)return;
        snprintf(expected,sizeof expected,"// Header\n%s",prefix);
        ok=!a->completion_active && !a->completion_pending_edit && completion_test_equal(edit,expected);
        puts("AUTO COMPLETION stale-equal-length-source=1");ok=ok && completion_auto_seed(a,"");AUTO_NEXT(30);break;
    case 30:AUTO_WAIT(edit->active);completion_auto_text(a,prefix);AUTO_NEXT(31);break;
    case 31: {
        AUTO_WAIT(a->completion_pending_edit==edit);
        SDL_Event event={0};event.type=SDL_EVENT_TEXT_EDITING;event.edit.windowID=SDL_GetWindowID(a->window);
        event.edit.text="候";event.edit.length=1;SDL_PushEvent(&event);AUTO_NEXT(32);break;
    }
    case 32:
        AUTO_WAIT(a->completion_composing);ok=!a->completion_active && !a->completion_pending_edit;
        AUTO_KEY(SDLK_SPACE,SDL_KMOD_CTRL);hold=ps_clock()+.250;AUTO_NEXT(33);break;
    case 33:
        if(ps_clock()<hold)return;
        ok=a->completion_composing && !a->completion_active && !a->completion_pending_edit && completion_test_equal(edit,prefix);
        completion_auto_text(a,language?"c":"_v");AUTO_NEXT(34);break;
    case 34:
        AUTO_WAIT(a->completion_active);ok=!a->completion_composing && !a->completion_explicit;
        puts("AUTO COMPLETION ime-pause=1 ime-commit=1");ok=ok && completion_auto_seed(a,language?"// ":"// continued" "\\" "\n");AUTO_NEXT(35);break;
    case 35:AUTO_WAIT(edit->active);completion_auto_text(a,prefix);hold=ps_clock()+.250;AUTO_NEXT(36);break;
    case 36:
        snprintf(expected,sizeof expected,"%s%s",language?"// ":"// continued" "\\" "\n",prefix);AUTO_WAIT(completion_test_equal(edit,expected));if(ps_clock()<hold)return;
        ok=!a->completion_active && !a->completion_pending_edit;ok=ok && completion_auto_seed(a,"\"");AUTO_NEXT(37);break;
    case 37:AUTO_WAIT(edit->active);completion_auto_text(a,prefix);hold=ps_clock()+.250;AUTO_NEXT(38);break;
    case 38:
        snprintf(expected,sizeof expected,"\"%s",prefix);AUTO_WAIT(completion_test_equal(edit,expected));if(ps_clock()<hold)return;
        ok=!a->completion_active && !a->completion_pending_edit;
        puts("AUTO COMPLETION comments-and-strings=1");ok=ok && completion_auto_seed(a,"");AUTO_NEXT(39);break;
    case 39:AUTO_WAIT(edit->active);completion_auto_text(a,prefix);AUTO_NEXT(40);break;
    case 40:
        AUTO_WAIT(a->completion_pending_edit==edit);edit->select_start=0;edit->select_end=2;hold=ps_clock()+.250;AUTO_NEXT(41);break;
    case 41:
        if(ps_clock()<hold)return;
        ok=!a->completion_active && !a->completion_pending_edit;
        puts("AUTO COMPLETION selection-invalidates=1");ok=ok && completion_auto_seed(a,"");AUTO_NEXT(42);break;
    case 42:AUTO_WAIT(edit->active);completion_auto_text(a,prefix);AUTO_NEXT(43);break;
    case 43:AUTO_WAIT(a->completion_pending_edit==edit);a->tab=2;a->analysis_tab=0;hold=ps_clock()+.250;AUTO_NEXT(44);break;
    case 44:
        if(ps_clock()<hold)return;
        ok=!a->completion_active && !a->completion_pending_edit && completion_test_equal(edit,prefix);
        puts("AUTO COMPLETION target-change=1");ok=ok && completion_auto_seed(a,"");a->preferences.completion_auto=0;AUTO_NEXT(56);break;
    case 56:AUTO_WAIT(edit->active);completion_auto_text(a,prefix);hold=ps_clock()+.250;AUTO_NEXT(57);break;
    case 57:
        AUTO_WAIT(completion_test_equal(edit,prefix));if(ps_clock()<hold)return;
        ok=!a->completion_active && !a->completion_pending_edit;
        AUTO_KEY(SDLK_SPACE,SDL_KMOD_CTRL);AUTO_NEXT(58);break;
    case 58:
        AUTO_WAIT(a->completion_active);ok=a->completion_manual;
        puts("AUTO COMPLETION preference-off-manual-remains=1");
        ok=ok && completion_auto_seed(a,"");AUTO_NEXT(59);break;
    case 59:AUTO_WAIT(edit->active);AUTO_KEY(SDLK_SPACE,SDL_KMOD_CTRL);AUTO_NEXT(60);break;
    case 60:
        AUTO_WAIT(a->completion_active && a->completion_visible_rows==5);
        ok=a->completion_count>5 && a->completion_choice_bounds[4].h>0 && !a->completion_choice_bounds[5].w &&
           a->completion_choice_bounds[4].y+a->completion_choice_bounds[4].h<=a->completion_list_bounds.y+a->completion_list_bounds.h-29 &&
           fabsf(a->completion_list_bounds.w-400)<1 && a->completion_scroll_bounds.w==6 && !a->completion_info_visible && !a->completion_info_bounds.w;
        test_plot_wheel(a,a->completion_choice_bounds[0].x+60,a->completion_choice_bounds[0].y+10,-3);AUTO_NEXT(61);break;
    case 61:
        AUTO_WAIT(a->completion_first>0);ok=a->completion_selected==0 && completion_test_equal(edit,"");
        AUTO_KEY(SDLK_UP,0);AUTO_NEXT(62);break;
    case 62:
        AUTO_WAIT(a->completion_selected==a->completion_count-1 && a->completion_first+5==a->completion_count);
        a->preferences.completion_rows=9;AUTO_NEXT(63);break;
    case 63:
        AUTO_WAIT(a->completion_visible_rows==9);ok=a->completion_choice_bounds[8].h>0 && !a->completion_choice_bounds[9].w;
        a->preferences.completion_rows=1;AUTO_NEXT(64);break;
    case 64:
        AUTO_WAIT(a->completion_visible_rows==1);ok=a->completion_choice_bounds[0].h>0 && !a->completion_choice_bounds[1].w &&
            a->completion_first==a->completion_selected;
        if(ok)puts("AUTO COMPLETION viewport-shrink-selection=1");
        {ps_preferences p=a->preferences;p.completion_rows=20;p.ui_size=p.editor_size=22;p.theme=PS_THEME_LIGHT;preferences_apply(a,&p);}AUTO_NEXT(69);break;
    case 69:
        AUTO_WAIT(a->preferences.ui_size==22 && a->completion_visible_rows>0 && a->completion_visible_rows<20);
        {struct nk_rect last=a->completion_choice_bounds[a->completion_visible_rows-1];
        ok=a->font_ui==a->ui_fonts[3] && a->font_code==a->code_fonts[5] && last.h>=30 &&
           last.y+last.h<=a->completion_list_bounds.y+a->completion_list_bounds.h-29 &&
           a->completion_list_bounds.y>=a->source_editor_bounds.y &&
           a->completion_list_bounds.y+a->completion_list_bounds.h<=a->source_editor_bounds.y+a->source_editor_bounds.h;
        ps_preferences p=a->preferences;p.completion_rows=5;p.theme=PS_THEME_HIGH_CONTRAST;preferences_apply(a,&p);}AUTO_NEXT(70);break;
    case 70:
        AUTO_WAIT(a->completion_visible_rows==5 && a->preferences.theme==PS_THEME_HIGH_CONTRAST);
        if(!a->completion_info_visible)AUTO_KEY(SDLK_SPACE,SDL_KMOD_CTRL);
        AUTO_NEXT(71);break;
    case 71:
        AUTO_WAIT(a->completion_info_visible && a->completion_info_bounds.w>0);
        ok=a->completion_choice_bounds[4].h>=30 && a->completion_info_bounds.w>0;
        puts("AUTO COMPLETION viewport-large-light-contrast=1 space-clamp=1");
        {ps_preferences p=a->preferences;p.ui_size=16;p.editor_size=12;p.theme=PS_THEME_DARK;preferences_apply(a,&p);}AUTO_NEXT(65);break;
    case 65:
        AUTO_WAIT(a->completion_visible_rows==5);
        test_mouse(a,a->completion_scroll_bounds,true);AUTO_NEXT(66);break;
    case 66:
        AUTO_WAIT(a->completion_scroll_drag);
        test_plot_motion(a,a->completion_scroll_bounds.x+3,a->completion_list_bounds.y+4);AUTO_NEXT(67);break;
    case 67:
        AUTO_WAIT(a->completion_first==0);
        test_mouse(a,nk_rect(a->completion_scroll_bounds.x,a->completion_list_bounds.y+4,6,2),false);AUTO_NEXT(68);break;
    case 68:
        AUTO_WAIT(!a->completion_scroll_drag);ok=completion_test_equal(edit,"") && a->completion_selected==a->completion_count-1;
        puts("AUTO COMPLETION viewport-five=1 rows-one-nine=1 all-results=1 wheel-no-accept=1 thumb-drag=1");
        a->preferences.completion_auto=1;ok=ok && completion_auto_seed(a,"");AUTO_NEXT(45);break;
    case 45:AUTO_WAIT(edit->active);completion_auto_text(a,prefix);AUTO_NEXT(46);break;
    case 46: {
        AUTO_WAIT(a->completion_pending_edit==edit);
        SDL_Event event={0};event.type=SDL_EVENT_WINDOW_FOCUS_LOST;event.window.windowID=SDL_GetWindowID(a->window);
        event.window.data1=(int)PS_TEST_MOUSE_ID;SDL_PushEvent(&event);hold=ps_clock()+.250;AUTO_NEXT(47);break;
    }
    case 47:
        if(ps_clock()<hold)return;
        ok=!a->completion_active && !a->completion_pending_edit && completion_test_equal(edit,prefix);
        puts("AUTO COMPLETION focus-loss=1");a->quitting=true;break;
    }
finished:
    if(!ok){const char *text=nk_str_get_const(&edit->string);fprintf(stderr,"Auto completion stage=%d active=%d explicit=%d pending=%d composing=%d cursor=%d first=%zu selected=%zu rows=%u count=%zu text=%.*s\n",*stage,a->completion_active,a->completion_explicit,a->completion_pending_edit!=NULL,a->completion_composing,edit->cursor,a->completion_first,a->completion_selected,a->completion_visible_rows,a->completion_count,nk_str_len_char(&edit->string),text?text:"");*code=1;a->quitting=true;}
    if(a->quitting)printf("AUTOMATIC COMPLETION SELF-TEST: %s\n",*code?"FAILED":"PASSED");
#undef AUTO_WAIT
#undef AUTO_NEXT
#undef AUTO_KEY
}
