/* Real key events exercise both directions in each visible pane combination. */
static void documentation_pane_test_frame(app *a,int *stage,int *code,const char **capture) {
    static unsigned sample,frames,step,count,position;static int topic,targets[4];
    static int experiment,analysis;static uint32_t experiment_hash,analysis_hash;
    static double simulation;static bool running,paused,pending,starting,example;
    static nk_uint scroll;static uint64_t version;static char image[80];bool ok=true;
    static double host_focus_deadline;
    switch((*stage)++) {
    case 0:
        experiment=a->experiment.string.len;analysis=a->analysis.string.len;
        experiment_hash=ps_crc32((const unsigned char *)nk_str_get_const(&a->experiment.string),(size_t)experiment);
        analysis_hash=ps_crc32((const unsigned char *)nk_str_get_const(&a->analysis.string),(size_t)analysis);
        simulation=a->simulation_time;running=a->runner.running;paused=a->paused;
        pending=a->reset_pending;starting=a->reset_starting;
        test_window_key_mod(a->window,SDLK_F1,0);break;
    case 1:
        topic=-1;for(int i=0;i<DOCUMENTATION_TOPIC_COUNT;i++)if(!strcmp(documentation_topics[i].path,"docs/pendulum-tutorial.md"))topic=i;
        ok=topic>=0;if(ok)open_documentation(a,topic);
        ok=ok && SDL_RestoreWindow(a->doc_window) && SDL_SetWindowSize(a->doc_window,760,540);
        if(ok)SDL_SyncWindow(a->doc_window);
        sample=0;break;
    case 2: {
        bool large=(sample/4)%2;unsigned theme=sample/8;
        ps_preferences p=a->preferences;p.theme=theme==0?PS_THEME_DARK:theme==1?PS_THEME_LIGHT:PS_THEME_HIGH_CONTRAST;
        p.ui_size=large?22:16;p.editor_size=16;preferences_apply(a,&p);
        a->doc_zoom_size=large?32:16;ok=documentation_fonts_prepare(a,a->doc_zoom_size,documentation_code_size(a));
        a->doc_font_ui=a->doc_ui_fonts[(a->doc_zoom_size-16)/2];a->doc_font_title=a->doc_title_fonts[(a->doc_zoom_size-16)/2];
        if(a->documentation)a->documentation->layout_width=0;
        a->doc_filter[0]=a->doc_last_filter[0]=0;
        documentation_language(a,(sample%2)+1);
        bool left=sample%4==1 || sample%4==2,right=sample%4==2 || sample%4==3;
        if(a->doc_navigation_hidden!=left)documentation_navigation_toggle(a);
        if(a->doc_context_hidden!=right)documentation_context_toggle(a);
        example=sample%2;a->doc_context_example=example;
        snprintf(a->doc_query,sizeof a->doc_query,"theta");frames=0;
        /* A scripted SDL key can be queued before the OS has completed window
         * activation. Establish real host focus before checking native focus. */
        SDL_RaiseWindow(a->doc_window);host_focus_deadline=ps_clock()+1;break;
    }
    case 3:
        if(SDL_GetKeyboardFocus()!=a->doc_window) {
            if(ps_clock()<host_focus_deadline){--*stage;return;}
            fprintf(stderr,"Pane fixture has no host keyboard focus: document=%u keyboard=%u\n",
                SDL_GetWindowID(a->doc_window),SDL_GetKeyboardFocus()?SDL_GetWindowID(SDL_GetKeyboardFocus()):0);
            ok=false;break;
        }
        if(frames++<2 || a->doc_scroll_pending || a->doc_history_restore || a->doc_jump_block>=0){--*stage;return;}
        a->doc_scroll_y=80;frames=0;break;
    case 4:
        if(frames++<2 || a->doc_scroll_pending){--*stage;return;}
        /* Hover creates the real header tooltip, whose cached popup must not
         * disable text input when F6 later enters global search. */
        {SDL_Event e={0};e.type=SDL_EVENT_MOUSE_MOTION;e.motion.windowID=SDL_GetWindowID(a->doc_window);
         e.motion.which=PS_TEST_MOUSE_ID;e.motion.x=a->doc_context_toggle_bounds.x+a->doc_context_toggle_bounds.w*.5f;
         e.motion.y=a->doc_context_toggle_bounds.y+a->doc_context_toggle_bounds.h*.5f;SDL_PushEvent(&e);}
        test_window_key_mod(a->doc_window,SDLK_F,PS_UI_COMMAND_MOD);break;
    case 5:
        if(!a->doc_search_active || a->doc_focus_reveal){--*stage;return;}
        ok=a->doc_focus==DOC_SEARCH && !strcmp(a->doc_query,"theta");
        count=0;targets[count++]=DOC_FILTER;
        if(!a->doc_navigation_hidden)targets[count++]=DOC_TOPIC+topic;
        position=count;targets[count++]=DOC_BODY;
        if(!a->doc_context_hidden)targets[count++]=example?DOC_EXAMPLE_TAB:DOC_MODEL_TAB;
        scroll=a->doc_scroll_y;version=a->doc_reading_version;step=0;
        test_window_key_mod(a->doc_window,SDLK_F6,0);frames=0;break;
    case 6: {
        if(frames++<1 || a->doc_focus_reveal){--*stage;return;}
        bool reverse=step>=count;
        position=(position+count+(reverse?-1:1))%count;
        int expected=targets[position];
        ok=a->doc_keyboard && a->doc_focus==expected && a->doc_focus_bounds.w>0 && a->doc_focus_bounds.h>0 &&
            !strcmp(a->doc_query,"theta") && !a->doc_filter[0] && a->doc_context_example==example &&
            a->doc_scroll_y==scroll && a->doc_reading_version==version;
        if(expected==DOC_FILTER)ok=ok && a->doc_filter_active && SDL_TextInputActive(a->doc_window) &&
            nk_sdl_accessibility_is_focused(a->doc_ui,NULL,"Alle Inhalte durchsuchen");
        if(expected==DOC_TOPIC+topic)ok=ok && nk_sdl_accessibility_is_focused(a->doc_ui,NULL,documentation_topics[topic].title);
        if(expected==DOC_MODEL_TAB || expected==DOC_EXAMPLE_TAB)ok=ok &&
            nk_sdl_accessibility_is_focused(a->doc_ui,"Kontextansicht",expected==DOC_MODEL_TAB?"Modell":"Beispiel");
        if(++step<count*2) {
            test_window_key_mod(a->doc_window,SDLK_F6,step>=count?SDL_KMOD_SHIFT:0);frames=0;--*stage;
        } else {
            if(a->doc_context_example && !a->doc_context_hidden) {
                struct nk_rect head=a->doc_context_head_bounds,copy=a->doc_context_copy_bounds,label=a->doc_context_head_label_bounds;
                bool wrapped=label.y+label.h<=copy.y-7;
                bool layout=head.w>100 && copy.y>=head.y+10 && copy.y+copy.h<=head.y+head.h-9 &&
                    copy.x>=head.x+10 && copy.x+copy.w<=head.x+head.w-9 &&
                    label.y+label.h<=head.y+head.h-9 && (wrapped || label.x+label.w<=copy.x-7);
                ok=ok && layout;
                printf("DOCUMENTATION PANES CODE HEAD: case=%u wrapped=%d height=%.2f complete=%d\n",sample,wrapped,head.h,layout);
            }
            printf("DOCUMENTATION PANES: case=%u groups=%u track=%d size=%u ui=%u theme=%d scroll=%u complete=%d\n",
                sample,count,a->doc_track,documentation_font_size(a),a->preferences.ui_size,a->preferences.theme,scroll,ok);
            snprintf(image,sizeof image,"docs-panes-%02u.bmp",sample);*capture=image;
        }
        break;
    }
    case 7:
        ok=a->experiment.string.len==experiment && a->analysis.string.len==analysis &&
           ps_crc32((const unsigned char *)nk_str_get_const(&a->experiment.string),(size_t)experiment)==experiment_hash &&
           ps_crc32((const unsigned char *)nk_str_get_const(&a->analysis.string),(size_t)analysis)==analysis_hash &&
           a->simulation_time==simulation && a->runner.running==running && a->paused==paused &&
           a->reset_pending==pending && a->reset_starting==starting;
        if(++sample<24)*stage=2;else a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Documentation panes stage=%d sample=%u step=%u focus=%d expected=%d scroll=%u/%u keyboard=%d bounds=%.0f,%.0f filter_active=%d query=%s filter=%s version=%llu/%llu native_header=%d\n",
        *stage-1,sample,step,a->doc_focus,targets[position],a->doc_scroll_y,scroll,a->doc_keyboard,
        a->doc_focus_bounds.w,a->doc_focus_bounds.h,a->doc_filter_active,a->doc_query,a->doc_filter,
        (unsigned long long)a->doc_reading_version,(unsigned long long)version,
        nk_sdl_accessibility_is_focused(a->doc_ui,NULL,"Alle Inhalte durchsuchen"));
        for(struct nk_window *w=a->doc_ui->begin;w;w=w->next)fprintf(stderr,"Pane NK window=%s active=%d flags=%u edit=%d name=%u seq=%u\n",w->name_string,w==a->doc_ui->active,w->flags,w->edit.active,w->edit.name,w->edit.seq);
        *code=1;a->quitting=true;}
    if(a->quitting)printf("DOCUMENTATION PANES SELF-TEST: %s\n",*code?"FAILED":"PASSED");
}
