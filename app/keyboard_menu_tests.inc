/* Exercise real SDL keyboard input and the same menu actions as the pointer. */
static void keyboard_menu_test_frame(app *a, const char *root, int *stage, int *code,
                                     const char **capture) {
    static double ready;
    static int original_length;
    bool ok=true;
#define MENU_KEY(key) test_window_key_mod(a->window,(key),0)
    switch((*stage)++) {
    case 0: ready=ps_clock()+.2;break;
    case 1:
        if(ps_clock()<ready){--*stage;return;}
        MENU_KEY(SDLK_F10);break;
    case 2: ok=a->toolbar_keyboard && !a->toolbar_menu && a->toolbar_keyboard_top==0;MENU_KEY(SDLK_DOWN);break;
    case 3: ok=a->toolbar_menu==1 && a->toolbar_keyboard_item==0;MENU_KEY(SDLK_DOWN);break;
    case 4: ok=a->toolbar_keyboard_item==3;MENU_KEY(SDLK_DOWN);break;
    case 5: ok=a->toolbar_keyboard_item==5;MENU_KEY(SDLK_DOWN);break;
    case 6: ok=a->toolbar_keyboard_item==0;MENU_KEY(SDLK_UP);break;
    case 7: ok=a->toolbar_keyboard_item==5;MENU_KEY(SDLK_HOME);break;
    case 8: ok=a->toolbar_keyboard_item==0;MENU_KEY(SDLK_RIGHT);break;
    case 9: ok=a->toolbar_menu==2 && a->toolbar_keyboard_top==1 && a->toolbar_keyboard_item==0;MENU_KEY(SDLK_DOWN);break;
    case 10: ok=a->toolbar_keyboard_item==4;MENU_KEY(SDLK_END);break;
    case 11:
        ok=a->toolbar_keyboard_item==8;*capture="keyboard-menu-focus.bmp";MENU_KEY(SDLK_HOME);break;
    case 12: ok=a->toolbar_keyboard_item==0 && !a->show_log;MENU_KEY(SDLK_RETURN);MENU_KEY(SDLK_DOWN);break;
    case 13:
        ok=a->show_log && !a->toolbar_menu && !a->toolbar_keyboard;
        MENU_KEY(SDLK_F10);break;
    case 14: MENU_KEY(SDLK_RIGHT);MENU_KEY(SDLK_RIGHT);break;
    case 15: ok=a->toolbar_keyboard_top==2;MENU_KEY(SDLK_RETURN);break;
    case 16:
        ok=a->doc_visible && !a->toolbar_keyboard;
        documentation_window_hide(a);SDL_RaiseWindow(a->window);ready=ps_clock()+.2;break;
    case 17:
        if(ps_clock()<ready){--*stage;return;}
        MENU_KEY(SDLK_F10);test_window_key_mod(a->window,SDLK_TAB,SDL_KMOD_SHIFT);break;
    case 18: ok=a->toolbar_keyboard_top==3;MENU_KEY(SDLK_RETURN);break;
    case 19: ok=a->tab==6 && !a->toolbar_keyboard;MENU_KEY(SDLK_F10);MENU_KEY(SDLK_DOWN);MENU_KEY(SDLK_ESCAPE);break;
    case 20: ok=!a->toolbar_keyboard && !a->toolbar_menu;MENU_KEY(SDLK_F10);MENU_KEY(SDLK_DOWN);MENU_KEY(SDLK_DOWN);MENU_KEY(SDLK_SPACE);break;
    case 21:
        ok=a->project_manager && !a->toolbar_keyboard;
        a->project_manager=false;
        ok=ok && document_test_start(a,root);
        if(ok)preview_workspace_path(a,a->workspace_tree_test_path);
        break;
    case 22:
        if(a->library_thread || !idle(a)){--*stage;return;}
        ok=active_document(a)!=NULL && a->tab==8;
        test_mouse(a,a->document_bounds[6],true);break;
    case 23: test_mouse(a,a->document_bounds[6],false);break;
    case 24:
        ok=active_document(a)!=NULL && active_document(a)->edit.active;
        original_length=nk_str_len_char(&active_document(a)->edit.string);
        MENU_KEY(SDLK_F10);MENU_KEY(SDLK_DOWN);break;
    case 25: {
        ok=a->toolbar_keyboard && a->toolbar_menu==1;
        SDL_Event text={0};text.type=SDL_EVENT_TEXT_INPUT;text.text.windowID=SDL_GetWindowID(a->window);
        text.text.text="menu text must not edit";SDL_PushEvent(&text);
        MENU_KEY(SDLK_DOWN);break;
    }
    case 26:
        ok=nk_str_len_char(&active_document(a)->edit.string)==original_length &&
           nk_str_len_char(&active_document(a)->edit.string)==(int)strlen("Workspace notes: α = 1\n") &&
           !memcmp(nk_str_get_const(&active_document(a)->edit.string),"Workspace notes: α = 1\n",strlen("Workspace notes: α = 1\n"));
        MENU_KEY(SDLK_ESCAPE);break;
    case 27:
        ok=!a->toolbar_menu && !a->toolbar_keyboard;
        break; /* Return to the previously focused editor without a pointer. */
    case 28: break;
    case 29: {
        SDL_Event text={0};text.type=SDL_EVENT_TEXT_INPUT;text.text.windowID=SDL_GetWindowID(a->window);
        text.text.text="Z";SDL_PushEvent(&text);break;
    }
    case 30:
        if(a->ui->input.keyboard.text_len){--*stage;return;}
        ok=nk_str_len_char(&active_document(a)->edit.string)==original_length+1;
        if(!ok)fprintf(stderr,"Editor return: length=%d expected=%d active=%d ui-popup=%u\n",
                       nk_str_len_char(&active_document(a)->edit.string),original_length+1,
                       active_document(a)->edit.active,a->ui->active?a->ui->active->popup.active:0);
        if(!ok && a->ui->active)fprintf(stderr,"Active window: %s flags=%u edit=%d textbytes=%d\n",
                                        a->ui->active->name_string,(unsigned)a->ui->active->flags,
                                        a->ui->active->edit.active,a->ui->input.keyboard.text_len);
        MENU_KEY(SDLK_F10);MENU_KEY(SDLK_DOWN);MENU_KEY(SDLK_END);MENU_KEY(SDLK_UP);break;
    case 31: ok=a->toolbar_keyboard_item==4;MENU_KEY(SDLK_RETURN);break;
    case 32:
        ok=!active_document(a)->dirty && !a->toolbar_menu && !a->toolbar_keyboard;
        MENU_KEY(SDLK_F10);MENU_KEY(SDLK_DOWN);break;
    case 33: test_mouse(a,a->document_bounds[6],true);break;
    case 34: test_mouse(a,a->document_bounds[6],false);break;
    case 35:
        ok=!a->toolbar_keyboard && !a->toolbar_menu;
        *capture="keyboard-menu-editor.bmp";
        MENU_KEY(SDLK_F10);MENU_KEY(SDLK_DOWN);break;
    case 36: {
        ok=a->toolbar_keyboard && a->toolbar_menu==1;
        SDL_Event event={0};event.type=SDL_EVENT_WINDOW_FOCUS_LOST;
        event.window.windowID=SDL_GetWindowID(a->window);
        event.window.data1=(int)PS_TEST_MOUSE_ID;SDL_PushEvent(&event);break;
    }
    case 37:
        ok=!a->toolbar_keyboard && !a->toolbar_menu;
        a->quitting=true;
        printf("KEYBOARD MENU SELF-TEST: %s\n",*code?"FAILED":"PASSED");break;
    }
    if(!ok) {
        fprintf(stderr,"Keyboard menu failure stage %d: menu=%d top=%d item=%d keyboard=%d\n",
                *stage-1,a->toolbar_menu,a->toolbar_keyboard_top,a->toolbar_keyboard_item,a->toolbar_keyboard);
        *code=1;a->quitting=true;
    }
#undef MENU_KEY
}
