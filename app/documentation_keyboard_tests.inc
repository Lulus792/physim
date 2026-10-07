/* Traverse actual SDL focus events; no pointer input or direct focus assignment. */
static bool documentation_test_seek(app *a,int id,int *stage) {
    if(a->doc_keyboard && a->doc_focus==id) {
        if(a->doc_focus_reveal){--*stage;return false;}
        return true;
    }
    test_window_key_mod(a->doc_window,SDLK_TAB,0);--*stage;return false;
}
static void documentation_keyboard_test_frame(app *a,int *stage,int *code,const char **capture) {
    static int copy_block,link_block,link_topic,heading,tutorial_topic;
    static nk_uint before_scroll;
    bool ok=true;
#define DOC_KEY(key) test_window_key_mod(a->doc_window,key,0)
    switch((*stage)++) {
    case 0: test_window_key_mod(a->window,SDLK_F1,0);break;
    case 1: tutorial_topic=-1;for(int i=0;i<DOCUMENTATION_TOPIC_COUNT;i++)if(!strcmp(documentation_topics[i].path,"docs/experiment-tutorial.md"))tutorial_topic=i;
        ok=a->doc_visible && tutorial_topic>=0;SDL_SetWindowSize(a->doc_window,760,540);DOC_KEY(SDLK_TAB);break;
    case 2: ok=a->doc_keyboard && a->doc_focus==DOC_C;DOC_KEY(SDLK_RETURN);break;
    case 3: ok=a->doc_topic==DOCUMENTATION_C_GUIDE_TOPIC;DOC_KEY(SDLK_TAB);DOC_KEY(SDLK_RETURN);break;
    case 4: ok=a->doc_topic==DOCUMENTATION_PHYSIM_GUIDE_TOPIC;test_window_key_mod(a->doc_window,SDLK_TAB,SDL_KMOD_SHIFT);DOC_KEY(SDLK_RETURN);break;
    case 5:
        if(!documentation_test_seek(a,DOC_TOPIC+tutorial_topic,stage))return;DOC_KEY(SDLK_RETURN);*stage=25;break;
    case 25:
        ok=a->doc_topic==tutorial_topic;copy_block=link_block=link_topic=heading=-1;
        for(size_t i=0;i<a->documentation->count;i++) {
            ps_doc_block *b=&a->documentation->blocks[i];
            if(copy_block<0 && b->kind==PS_DOC_CODE)copy_block=(int)i;
            if(heading<0 && b->kind==PS_DOC_HEADING && b->level<=2 && i>0)heading=(int)i;
            if(link_block<0 && b->kind==PS_DOC_LINK) {
                const char *target=a->documentation->text+b->target;
                for(int j=0;j<DOCUMENTATION_TOPIC_COUNT;j++)if(!strcmp(target,documentation_topics[j].path) || (!strncmp(documentation_topics[j].path,"docs/",5) && !strcmp(target,documentation_topics[j].path+5))){link_block=(int)i;link_topic=j;break;}
            }
        }
        ok=ok && copy_block>=0 && link_block>=0 && heading>=0;*stage=6;break;
    case 6: if(!documentation_test_seek(a,DOC_CONTENTS,stage))return;DOC_KEY(SDLK_RETURN);break;
    case 7: if(!documentation_test_seek(a,DOC_HEADING+heading,stage))return;DOC_KEY(SDLK_RETURN);break;
    case 8: ok=a->doc_scroll_y>0;*capture="docs-keyboard-heading.bmp";break;
    case 9: if(!documentation_test_seek(a,DOC_COPY+copy_block,stage))return;SDL_SetClipboardText("sentinel");DOC_KEY(SDLK_SPACE);break;
    case 10: {char *text=SDL_GetClipboardText();ok=text && !strcmp(text,a->documentation->text+a->documentation->blocks[copy_block].text);SDL_free(text);break;}
    case 11: if(!documentation_test_seek(a,DOC_LINK+link_block,stage))return;DOC_KEY(SDLK_RETURN);break;
    case 12: ok=a->doc_topic==link_topic;break;
    case 13: if(!documentation_test_seek(a,DOC_BACK,stage))return;DOC_KEY(SDLK_RETURN);break;
    case 14: ok=a->doc_topic==tutorial_topic;break;
    case 15: if(!documentation_test_seek(a,DOC_BODY,stage))return;DOC_KEY(SDLK_HOME);break;
    case 16: ok=a->doc_scroll_y==0;before_scroll=a->doc_scroll_y;DOC_KEY(SDLK_PAGEDOWN);break;
    case 17: ok=a->doc_scroll_y>before_scroll;DOC_KEY(SDLK_END);break;
    case 18: ok=a->doc_scroll_y>0;DOC_KEY(SDLK_HOME);break;
    case 19: ok=a->doc_scroll_y==0;test_window_key_mod(a->doc_window,SDLK_F,PS_UI_COMMAND_MOD);break;
    case 20: {if(a->doc_focus_reveal){--*stage;return;}ok=a->doc_focus==DOC_SEARCH && a->doc_search_active;SDL_Event e={0};e.type=SDL_EVENT_TEXT_INPUT;e.text.windowID=SDL_GetWindowID(a->doc_window);e.text.text="Physim";SDL_PushEvent(&e);break;}
    case 21: if(a->doc_ui->input.keyboard.text_len){--*stage;return;}ok=!strcmp(a->doc_query,"Physim");DOC_KEY(SDLK_TAB);break;
    case 22: ok=a->doc_focus==DOC_PREVIOUS && !a->doc_search_active;DOC_KEY(SDLK_TAB);DOC_KEY(SDLK_RETURN);break;
    case 23: ok=a->doc_focus==DOC_NEXT;*capture="docs-keyboard-search.bmp";*stage=26;break;
    case 26: if(!documentation_test_seek(a,DOC_TOPICS,stage))return;DOC_KEY(SDLK_RETURN);break;
    case 27: if(!documentation_test_seek(a,DOC_FILTER,stage))return;break;
    case 28: {ok=a->doc_filter_active;SDL_Event e={0};e.type=SDL_EVENT_TEXT_INPUT;e.text.windowID=SDL_GetWindowID(a->doc_window);e.text.text="Pendel";SDL_PushEvent(&e);break;}
    case 29: if(a->doc_ui->input.keyboard.text_len){--*stage;return;}ok=!strcmp(a->doc_filter,"Pendel");DOC_KEY(SDLK_TAB);break;
    case 30: ok=a->doc_focus==DOC_GROUP && !a->doc_filter_active;DOC_KEY(SDLK_SPACE);break;
    case 31: ok=!a->doc_filter[0];test_window_key_mod(a->doc_window,SDLK_F,PS_UI_COMMAND_MOD);break;
    case 32: if(a->doc_focus_reveal){--*stage;return;}
        {SDL_Event e={0};e.type=SDL_EVENT_TEXT_INPUT;e.text.windowID=SDL_GetWindowID(a->doc_window);e.text.text="Z";SDL_PushEvent(&e);DOC_KEY(SDLK_TAB);}break;
    case 33: ok=a->doc_focus==DOC_PREVIOUS && !strcmp(a->doc_query,"PhysimZ");DOC_KEY(SDLK_ESCAPE);*stage=24;break;
    case 24: ok=!a->doc_visible && !a->doc_keyboard;a->quitting=true;printf("DOCUMENTATION KEYBOARD SELF-TEST: %s\n",*code?"FAILED":"PASSED");break;
    }
    if(!ok){fprintf(stderr,"Query debug: query=%s active=%d events bytes=%d window=%s edit=%d flags=%u\n",a->doc_query,a->doc_search_active,a->doc_ui->input.keyboard.text_len,a->doc_ui->active?a->doc_ui->active->name_string:"none",a->doc_ui->active?a->doc_ui->active->edit.active:0,a->doc_ui->active?(unsigned)a->doc_ui->active->flags:0);fprintf(stderr,"Documentation keyboard failure stage %d focus=%d topic=%d scroll=%u reveal=%d\n",*stage-1,a->doc_focus,a->doc_topic,a->doc_scroll_y,a->doc_focus_reveal);*code=1;a->quitting=true;}
#undef DOC_KEY
}
