/* Native window acceptance: real queued widgets, rectangles, input and captures. */
static void documentation_h_test_frame(app *a,int *stage,int *code,const char **capture) {
    static unsigned frame,index;static int experiment,analysis;static double simulation;
    static struct nk_rect left,right;static char name[80];
    static int wide_width=1080,wide_height=780;
    static int requested_width,requested_height;
    bool ok=true;
    switch((*stage)++) {
    case 0:
        experiment=a->experiment.string.len;analysis=a->analysis.string.len;simulation=a->simulation_time;
        frame=0;test_window_key_mod(a->window,SDLK_F1,0);break;
    case 1:
        if(frame++<2){--*stage;return;}
        {
            const char *source="# Gemeinsam\nErklärung\n## Vollständiger C-Quellcode\n```c\n#include <stdio.h>\nC_ONLY\n```\n## Vollständiger Physim-Quellcode\n```physim\nPHYSIM_ONLY\n```\n## Werkzeug\n```sh\nSHARED_TOOL\n```\n";
            for(int track=1;track<=2;track++) {
                char text[512];snprintf(text,sizeof text,"%s",source);documentation_language_source(text,track);
                ok=ok && strstr(text,"Erklärung") && strstr(text,"SHARED_TOOL") &&
                    (strstr(text,"C_ONLY")!=NULL)==(track==1) && (strstr(text,"PHYSIM_ONLY")!=NULL)==(track==2);
            }
        }
        for(int i=0;i<DOCUMENTATION_TOPIC_COUNT;i++)if(!strcmp(documentation_topics[i].path,"docs/pendulum-tutorial.md")){open_documentation(a,i);break;}
        {bool selected=false;
         ok=ok && nk_sdl_accessibility_choice_state(a->doc_ui,"Dokumentationssprache","C",&selected) && selected &&
            nk_sdl_accessibility_choice_state(a->doc_ui,"Dokumentationssprache","Physim",&selected) && !selected &&
            nk_sdl_accessibility_choice_state(a->doc_ui,"Kontextansicht","Modell",&selected) && selected &&
            nk_sdl_accessibility_choice_state(a->doc_ui,"Kontextansicht","Beispiel",&selected) && !selected;}
        index=0;break;
    case 2: {
        unsigned size=index>=48?1:(index/4)%2,window=index>=48?1:(index/8)%2,theme=index>=48?(index-48)/4:index/16;
        ps_preferences p=a->preferences;p.theme=theme==0?PS_THEME_DARK:theme==1?PS_THEME_LIGHT:PS_THEME_HIGH_CONTRAST;
        /* Workspace preferences support 16..22 px; the reader supports 32 px. */
        p.ui_size=index>=48?22:16;p.editor_size=16;
        preferences_apply(a,&p);
        ok=a->preferences.ui_size==p.ui_size && documentation_chrome_font(a)->height==(float)p.ui_size &&
            a->doc_code_fonts[(p.editor_size-16)/2]!=NULL;
        a->doc_zoom_size=size?32:16;
        ok=ok && documentation_fonts_prepare(a,a->doc_zoom_size,documentation_code_size(a));
        a->doc_font_ui=a->doc_ui_fonts[(a->doc_zoom_size-16)/2];
        a->doc_font_title=a->doc_title_fonts[(a->doc_zoom_size-16)/2];
        if(a->documentation)a->documentation->layout_width=0;
        SDL_Rect usable={0};int top=0,edge_left=0,bottom=0,edge_right=0;
        ok=ok && SDL_GetDisplayUsableBounds(SDL_GetDisplayForWindow(a->doc_window),&usable) &&
            documentation_inline_math_borders(a->doc_window,&top,&edge_left,&bottom,&edge_right);
        int available_width=usable.w-edge_left-edge_right,available_height=usable.h-top-bottom;
        int wanted_width=window?760:1080,wanted_height=window?540:780;
        int width=wanted_width<available_width?wanted_width:available_width;
        int height=wanted_height<available_height?wanted_height:available_height;
        ok=ok && available_width>=760 && available_height>=540 && SDL_SetWindowSize(a->doc_window,width,height);
        /* Cocoa can time out its synchronous resize wait. Acceptance is the
         * actual size and pane geometry after the rendered frames below. */
        if(ok && !SDL_SyncWindow(a->doc_window))
            fprintf(stderr,"Design H resize wait: case=%u SDL=%s; verifying rendered size\n",index,SDL_GetError());
        if(!ok)fprintf(stderr,"Design H preparation: case=%u ui=%u font=%.3f available=%dx%d target=%dx%d SDL=%s\n",
            index,a->preferences.ui_size,documentation_chrome_font(a)->height,available_width,available_height,width,height,SDL_GetError());
        requested_width=width;requested_height=height;
        if(!window){wide_width=width;wide_height=height;}
        if(index%4==0)printf("DOCUMENTATION H WINDOW: group=%u wanted=%dx%d available=%dx%d target=%dx%d\n",
            index/4,wanted_width,wanted_height,available_width,available_height,width,height);
        /* Gray order changes only one side at each transition. */
        unsigned state=index%4;bool hidden_left=state==1 || state==2,hidden_right=state==2 || state==3;
        if(a->doc_navigation_hidden!=hidden_left)documentation_navigation_toggle(a);
        if(a->doc_context_hidden!=hidden_right)documentation_context_toggle(a);
        frame=0;break;
    }
    case 3:
        if(frame++<2 || a->doc_scroll_pending){--*stage;return;}
        {int width=0,height=0;
         ok=SDL_GetWindowSize(a->doc_window,&width,&height) && width==requested_width && height==requested_height;
         if(!ok)fprintf(stderr,"Design H settled window: case=%u wanted=%dx%d actual=%dx%d SDL=%s\n",
             index,requested_width,requested_height,width,height,SDL_GetError());}
        if(index%4==0){left=a->doc_left_slot;right=a->doc_right_slot;}
        if(!a->doc_navigation_hidden)ok=ok && a->doc_left_slot.x==left.x && a->doc_left_slot.w==left.w;
        if(!a->doc_context_hidden)ok=ok && a->doc_right_slot.x==right.x && a->doc_right_slot.w==right.w;
        ok=ok && a->doc_article_slot.w>=320 && a->doc_body_bounds.w>220 && a->doc_body_bounds.h>=120;
        snprintf(name,sizeof name,"docs-h-%02u.bmp",index);*capture=name;
        printf("DOCUMENTATION H GEOMETRY: case=%u left=%.0f,%.0f article=%.0f,%.0f right=%.0f,%.0f body=%.0f,%.0f ok=%d\n",
            index,a->doc_left_slot.x,a->doc_left_slot.w,a->doc_article_slot.x,a->doc_article_slot.w,
            a->doc_right_slot.x,a->doc_right_slot.w,a->doc_body_bounds.w,a->doc_body_bounds.h,ok);
        break;
    case 4:
        if(++index<56)*stage=2;
        else {
            a->doc_navigation_hidden=false;a->doc_context_hidden=false;
            SDL_SetWindowSize(a->doc_window,wide_width,wide_height);SDL_SyncWindow(a->doc_window);
            ps_preferences p=a->preferences;p.theme=PS_THEME_DARK;p.ui_size=16;preferences_apply(a,&p);documentation_zoom(a,0);
            a->doc_context_example=true;frame=0;
        }break;
    case 5:
        if(frame++<2){--*stage;return;}
        ok=documentation_context_code(a)!=NULL;
        *capture="docs-h-example-c.bmp";
        a->doc_keyboard=true;a->doc_focus=DOC_CONTEXT_COPY;
        test_window_key_mod(a->doc_window,SDLK_RETURN,0);break;
    case 6:
        ok=documentation_test_clipboard(documentation_context_code(a));
        documentation_language(a,2);frame=0;break;
    case 7:
        if(frame++<2){--*stage;return;}
        ok=a->doc_track==2 && !a->doc_navigation_hidden && !a->doc_context_hidden && a->doc_context_example &&
            documentation_context_code(a) && strstr(documentation_context_code(a),"func ") &&
            !strstr(a->documentation->reading,"#include") && strstr(a->documentation->reading,"func ");
        *capture="docs-h-example-physim.bmp";
        test_window_mouse(a->doc_window,a->doc_context_toggle_bounds,true);break;
    case 8:test_window_mouse(a->doc_window,a->doc_context_toggle_bounds,false);frame=0;break;
    case 9:
        if(frame++<2){--*stage;return;}
        ok=a->doc_context_hidden && !a->doc_navigation_hidden && a->doc_focus==DOC_CONTEXT && a->doc_context_example;
        ok=ok && !nk_sdl_accessibility_press(a->doc_ui,"Modell") && !nk_sdl_accessibility_press(a->doc_ui,"Beispiel");
        {bool selected=false;ok=ok && !nk_sdl_accessibility_choice_state(a->doc_ui,"Kontextansicht","Modell",&selected) &&
            !nk_sdl_accessibility_choice_state(a->doc_ui,"Kontextansicht","Beispiel",&selected);}
        {int entries[12*PS_DOC_MAX_BLOCKS+DOCUMENTATION_TOPIC_COUNT+32];unsigned n=documentation_focus_list(a,entries);
         for(unsigned i=0;i<n;i++)ok=ok && entries[i]!=DOC_MODEL_TAB && entries[i]!=DOC_EXAMPLE_TAB && entries[i]!=DOC_CONTEXT_COPY;}
        snprintf(a->doc_filter,sizeof a->doc_filter,"UnfindbarerBegriff73519");a->doc_last_filter[0]=0;frame=0;break;
    case 10:
        if(frame++<2){--*stage;return;}
        ok=a->doc_context_hidden && !a->doc_navigation_hidden;
        {int order[DOCUMENTATION_TOPIC_COUNT];ok=ok && !documentation_topic_order(a,order) &&
            documentation_index[a->doc_topic] && !strstr(documentation_index[a->doc_topic],"#include");}
        *capture="docs-h-empty-search.bmp";
        a->doc_filter[0]=0;documentation_filter(a);documentation_context_toggle(a);
        open_documentation(a,1);frame=0;break;
    case 11:
        if(frame++<2){--*stage;return;}
        ok=!documentation_context_for(a) && a->doc_track==2 && a->doc_context_example;
        *capture="docs-h-no-context.bmp";
        documentation_window_hide(a);open_documentation(a,1);break;
    case 12:
        ok=a->doc_track==2 && !a->doc_context_hidden && !a->doc_navigation_hidden && a->doc_context_example &&
            a->experiment.string.len==experiment && a->analysis.string.len==analysis && a->simulation_time==simulation;
        for(int i=0;i<DOCUMENTATION_TOPIC_COUNT;i++)if(!strcmp(documentation_topics[i].path,"docs/experiment-tutorial.md")) {
            open_documentation(a,i);ok=ok && a->doc_track==1 && strstr(a->documentation->reading,"#include");break;
        }
        for(int i=0;i<DOCUMENTATION_TOPIC_COUNT;i++)if(!strcmp(documentation_topics[i].path,"docs/language-tutorial.md")) {
            open_documentation(a,i);ok=ok && a->doc_track==2 && strstr(a->documentation->reading,"func ");break;
        }
        test_window_key_mod(a->doc_window,SDLK_F,PS_UI_COMMAND_MOD);frame=0;break;
    case 13:
        if(frame++<2 || !a->doc_search_active || a->doc_focus_reveal){--*stage;return;}
        {SDL_Event e={0};e.type=SDL_EVENT_TEXT_INPUT;e.text.windowID=SDL_GetWindowID(a->doc_window);e.text.text="θ";SDL_PushEvent(&e);}
        test_window_key_mod(a->doc_window,SDLK_R,PS_UI_COMMAND_MOD|SDL_KMOD_SHIFT);frame=0;break;
    case 14:
        if(frame++<2){--*stage;return;}
        ok=a->doc_context_hidden && !a->doc_navigation_hidden && a->doc_focus==DOC_CONTEXT && !strcmp(a->doc_query,"θ") &&
            a->experiment.string.len==experiment && a->analysis.string.len==analysis && a->simulation_time==simulation;
        test_window_key_mod(a->doc_window,SDLK_R,PS_UI_COMMAND_MOD|SDL_KMOD_SHIFT);
        test_window_key_mod(a->doc_window,SDLK_TAB,SDL_KMOD_SHIFT);frame=0;break;
    case 15:
        if(frame++<2 || a->doc_focus_reveal){--*stage;return;}
        ok=a->doc_focus==DOC_FILTER && a->doc_filter_active;
        {SDL_Event e={0};e.type=SDL_EVENT_TEXT_INPUT;e.text.windowID=SDL_GetWindowID(a->doc_window);e.text.text="Pendel";SDL_PushEvent(&e);}
        frame=0;break;
    case 16:
        if(frame++<2 || a->doc_ui->input.keyboard.text_len){--*stage;return;}
        ok=!strcmp(a->doc_filter,"Pendel") && a->doc_filter_active && a->doc_first_result_bounds.w>0;
        test_window_mouse(a->doc_window,a->doc_first_result_bounds,true);break;
    case 17:test_window_mouse(a->doc_window,a->doc_first_result_bounds,false);frame=0;break;
    case 18:
        if(frame++<2 || a->doc_scroll_pending){--*stage;return;}
        ok=!strcmp(documentation_topics[a->doc_topic].path,"docs/pendulum-tutorial.md") && !a->doc_filter_active &&
            !a->doc_search_active && !a->doc_navigation_hidden && !a->doc_context_hidden && a->doc_context_example &&
            a->doc_track==2 && !strcmp(a->doc_filter,"Pendel") && a->experiment.string.len==experiment &&
            a->analysis.string.len==analysis && a->simulation_time==simulation;
        *capture="docs-h-pointer-search.bmp";a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Design H stage %d failed\n",*stage-1);*code=1;a->quitting=true;}
    if(a->quitting)printf("DOCUMENTATION H SELF-TEST: %s\n",*code?"FAILED":"PASSED");
}
