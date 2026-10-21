/* Native window acceptance: real queued widgets, rectangles, input and captures. */
static bool documentation_h_same_rect(struct nk_rect a,struct nk_rect b) {
    return fabsf(a.x-b.x)<.1f && fabsf(a.y-b.y)<.1f && fabsf(a.w-b.w)<.1f && fabsf(a.h-b.h)<.1f;
}
static bool documentation_h_sources_unchanged(app *a,const char *experiment,int experiment_bytes,const char *analysis,int analysis_bytes) {
    return experiment && analysis && nk_str_len_char(&a->experiment.string)==experiment_bytes &&
        nk_str_len_char(&a->analysis.string)==analysis_bytes &&
        !memcmp(nk_str_get_const(&a->experiment.string),experiment,(size_t)experiment_bytes) &&
        !memcmp(nk_str_get_const(&a->analysis.string),analysis,(size_t)analysis_bytes);
}
static void documentation_h_test_frame(app *a,int *stage,int *code,const char **capture) {
    static unsigned frame,index;static int experiment,analysis;static double simulation;
    static struct nk_rect left,right;static char name[80];
    static struct nk_rect reading_bounds,menu_item_bounds;static nk_uint reading_scroll;static unsigned reading_size;
    static char reading_query[128];
    static int search_cursor,search_start,search_end;
    static char *experiment_copy,*analysis_copy;static int experiment_bytes,analysis_bytes;
    static int wide_width=1080,wide_height=780;
    static int requested_width,requested_height;
    bool ok=true;
    switch((*stage)++) {
    case 0:
        if(!nk_str_len_char(&a->experiment.string)) {
            const char *fixture="/* Preserve experiment α😀 */\nint experiment_marker = 17;\n";
            nk_str_append_text_char(&a->experiment.string,fixture,(int)strlen(fixture));
        }
        if(!nk_str_len_char(&a->analysis.string)) {
            const char *fixture="/* Preserve analysis β😀 */\nint analysis_marker = 29;\n";
            nk_str_append_text_char(&a->analysis.string,fixture,(int)strlen(fixture));
        }
        experiment_bytes=nk_str_len_char(&a->experiment.string);analysis_bytes=nk_str_len_char(&a->analysis.string);
        experiment_copy=malloc((size_t)experiment_bytes);analysis_copy=malloc((size_t)analysis_bytes);
        ok=experiment_bytes>0 && analysis_bytes>0 && experiment_copy && analysis_copy;
        if(!ok)break;
        memcpy(experiment_copy,nk_str_get_const(&a->experiment.string),(size_t)experiment_bytes);
        memcpy(analysis_copy,nk_str_get_const(&a->analysis.string),(size_t)analysis_bytes);
        a->doc_context_caption_test=true;
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
        a->doc_detail_test_ok=false;a->doc_detail_test_case=index;
        unsigned size=index>=48?1:(index/4)%2,window=index>=48?1:(index/8)%2,theme=index>=48?(index-48)/4:index/16;
        ps_preferences p=a->preferences;p.theme=theme==0?PS_THEME_DARK:theme==1?PS_THEME_LIGHT:PS_THEME_HIGH_CONTRAST;
        /* Workspace preferences support 16..22 px; the reader supports 32 px. */
        p.ui_size=index>=48?22:16;p.editor_size=16;
        preferences_apply(a,&p);
        ok=a->preferences.ui_size==p.ui_size && fabsf(documentation_chrome_font(a)->height/
            a->doc_type_em[DOC_TYPE_CHROME]-11.f*p.ui_size/16)<.01f &&
            nk_sdl_row_height(a->doc_ui,16)==(p.ui_size>16?(float)p.ui_size+4:16.f) &&
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
        /* A frame filling the small Cocoa desktop can be considered zoomed.
         * SDL 3.2.30 ignores SetWindowSize in that state: restore first. */
        printf("DOCUMENTATION H RESTORE: case=%u flags=%llu\n",index,(unsigned long long)SDL_GetWindowFlags(a->doc_window));
        ok=ok && SDL_RestoreWindow(a->doc_window);
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
        ok=ok && !a->doc_find_open && !a->doc_actions_open && a->doc_article_slot.w>=320 && a->doc_body_bounds.w>220 && a->doc_body_bounds.h>=120;
        ok=ok && (a->doc_context_hidden || a->doc_detail_test_ok);
        snprintf(name,sizeof name,"docs-h-%02u.bmp",index);*capture=name;
        printf("DOCUMENTATION H GEOMETRY: case=%u left=%.0f,%.0f article=%.0f,%.0f right=%.0f,%.0f body=%.0f,%.0f ok=%d\n",
            index,a->doc_left_slot.x,a->doc_left_slot.w,a->doc_article_slot.x,a->doc_article_slot.w,
            a->doc_right_slot.x,a->doc_right_slot.w,a->doc_body_bounds.w,a->doc_body_bounds.h,ok);
        const struct nk_user_font *chrome=documentation_chrome_font(a);
        printf("DOCUMENTATION H CHROME: case=%u c=%.2f,%.2f,%.2f,%.2f physim=%.2f,%.2f,%.2f,%.2f model=%.2f,%.2f,%.2f,%.2f\n",
            index,a->doc_track_bounds[0].x,a->doc_track_bounds[0].y,a->doc_track_bounds[0].w,a->doc_track_bounds[0].h,
            a->doc_track_bounds[1].x,a->doc_track_bounds[1].y,a->doc_track_bounds[1].w,a->doc_track_bounds[1].h,
            a->doc_right_slot.x+12,a->doc_right_slot.y,documentation_tab_width(a,"Modell"),fmaxf(46,chrome->height+16));
        if(!a->doc_navigation_hidden)printf("DOCUMENTATION H NAV: case=%u topic=%.2f,%.2f,%.2f,%.2f section=%.2f,%.2f,%.2f,%.2f\n",
            index,a->doc_active_topic_bounds.x,a->doc_active_topic_bounds.y,a->doc_active_topic_bounds.w,a->doc_active_topic_bounds.h,
            a->doc_first_section_bounds.x,a->doc_first_section_bounds.y,a->doc_first_section_bounds.w,a->doc_first_section_bounds.h);
        break;
    case 4:
        if(++index<56)*stage=2;
        else {
            a->doc_context_caption_test=false;
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
        *capture="docs-h-pointer-search.bmp";break;
    case 19:
        if(!documentation_test_seek(a,DOC_FIND_CLOSE,stage,code))return;
        test_window_key_mod(a->doc_window,SDLK_RETURN,0);frame=0;break;
    case 20:
        if(frame++<2 || a->doc_scroll_pending){--*stage;return;}
        ok=!a->doc_find_open && !a->doc_actions_open && a->doc_focus==DOC_BODY &&
            !nk_sdl_accessibility_request_focus(a->doc_ui,NULL,"Zurück") &&
            !nk_sdl_accessibility_request_focus(a->doc_ui,NULL,"Leseschrift vergrößern") &&
            !nk_sdl_accessibility_request_focus(a->doc_ui,NULL,"Aktuelles Dokument durchsuchen");
        reading_bounds=a->doc_body_bounds;reading_scroll=a->doc_scroll_y;reading_size=documentation_font_size(a);
        snprintf(reading_query,sizeof reading_query,"%s",a->doc_query);
        printf("DOCUMENTATION ACTION BASE: body=%.3f,%.3f,%.3f,%.3f scroll=%u focus=%d query=%s\n",reading_bounds.x,reading_bounds.y,reading_bounds.w,reading_bounds.h,reading_scroll,a->doc_focus,a->doc_query);
        test_window_key_mod(a->doc_window,SDLK_F10,SDL_KMOD_SHIFT);break;
    case 21:
        if(a->doc_actions_open && (a->doc_actions_bounds.w<=0 || SDL_GetKeyboardFocus()!=a->doc_window ||
            !nk_sdl_accessibility_is_focused(a->doc_ui,NULL,"Zurück"))){--*stage;return;}
        printf("DOCUMENTATION ACTION OPEN: open=%d menu=%.3f,%.3f,%.3f,%.3f body=%.3f,%.3f,%.3f,%.3f scroll=%u expected=%u focus=%d query=%s\n",a->doc_actions_open,a->doc_actions_bounds.x,a->doc_actions_bounds.y,a->doc_actions_bounds.w,a->doc_actions_bounds.h,a->doc_body_bounds.x,a->doc_body_bounds.y,a->doc_body_bounds.w,a->doc_body_bounds.h,a->doc_scroll_y,reading_scroll,a->doc_focus,a->doc_query);
        ok=a->doc_actions_open && a->doc_actions_bounds.w>0 && a->doc_actions_bounds.h>0 &&
            documentation_h_same_rect(reading_bounds,a->doc_body_bounds) && reading_scroll==a->doc_scroll_y;
        *capture="docs-h-actions-keyboard.bmp";test_window_key_mod(a->doc_window,SDLK_ESCAPE,0);break;
    case 22:
        ok=!a->doc_actions_open && a->doc_keyboard && a->doc_focus==DOC_BODY &&
            documentation_h_same_rect(reading_bounds,a->doc_body_bounds) && reading_scroll==a->doc_scroll_y;
        test_window_key_mod(a->doc_window,SDLK_F,PS_UI_COMMAND_MOD);frame=0;break;
    case 23:
        if(frame++<2 || a->doc_focus_reveal || !a->doc_search_active){--*stage;return;}
        ok=a->doc_find_open && a->doc_focus==DOC_SEARCH && documentation_h_same_rect(reading_bounds,a->doc_body_bounds) &&
            reading_scroll==a->doc_scroll_y && !strcmp(a->doc_query,reading_query);
        *capture="docs-h-find-overlay.bmp";test_window_key_mod(a->doc_window,SDLK_END,0);frame=0;*stage=231;break;
    case 231:
        if(frame++<2){*stage=231;return;}
        {struct nk_window *find=nk_window_find(a->doc_ui,"documentation-find");
         ok=find && find->edit.cursor==nk_utf_len(reading_query,(int)strlen(reading_query));
         if(find){search_cursor=find->edit.cursor;search_start=find->edit.sel_start;search_end=find->edit.sel_end;}}
        test_window_key_mod(a->doc_window,SDLK_F10,SDL_KMOD_SHIFT);*stage=24;break;
    case 24:
        ok=a->doc_actions_open && !a->doc_search_active && !strcmp(a->doc_query,reading_query);
        test_window_key_mod(a->doc_window,SDLK_ESCAPE,0);frame=0;break;
    case 25:
        if(frame++<2 || a->doc_focus_reveal || !a->doc_search_active){--*stage;return;}
        ok=!a->doc_actions_open && a->doc_focus==DOC_SEARCH && SDL_TextInputActive(a->doc_window) && !strcmp(a->doc_query,reading_query);
        {struct nk_window *find=nk_window_find(a->doc_ui,"documentation-find");
         ok=ok && find && find->edit.cursor==search_cursor && find->edit.sel_start==search_start && find->edit.sel_end==search_end;
         printf("DOCUMENTATION FIND RETURN: cursor=%d/%d selection=%d,%d/%d,%d preserved=%d\n",find?find->edit.cursor:-1,search_cursor,find?find->edit.sel_start:-1,find?find->edit.sel_end:-1,search_start,search_end,ok);}
        {SDL_Event event={0};event.type=SDL_EVENT_TEXT_INPUT;event.text.windowID=SDL_GetWindowID(a->doc_window);event.text.text="Z";SDL_PushEvent(&event);}
        *stage=251;break;
    case 251:
        if(a->doc_ui->input.keyboard.text_len){*stage=251;return;}
        {char expected[130];snprintf(expected,sizeof expected,"%sZ",reading_query);ok=!strcmp(a->doc_query,expected);
         printf("DOCUMENTATION FIND INPUT: expected=%s actual=%s inserted=%d\n",expected,a->doc_query,ok);}
        test_window_key_mod(a->doc_window,SDLK_BACKSPACE,0);frame=0;*stage=252;break;
    case 252:
        if(frame++<2){*stage=252;return;}
        ok=!strcmp(a->doc_query,reading_query);*stage=26;break;
    case 26:
        if(!documentation_test_seek(a,DOC_FIND_CLOSE,stage,code))return;
        test_window_key_mod(a->doc_window,SDLK_RETURN,0);break;
    case 27:test_window_mouse(a->doc_window,a->doc_actions_trigger_bounds,true);break;
    case 28:test_window_mouse(a->doc_window,a->doc_actions_trigger_bounds,false);break;
    case 29:
        ok=a->doc_actions_open && !a->doc_action_pending && documentation_font_size(a)==reading_size &&
            documentation_h_same_rect(reading_bounds,a->doc_body_bounds) && reading_scroll==a->doc_scroll_y;
        *capture="docs-h-actions-pointer.bmp";
        test_window_mouse(a->doc_window,nk_rect(reading_bounds.x+2,reading_bounds.y+reading_bounds.h-2,1,1),true);break;
    case 30:test_window_mouse(a->doc_window,nk_rect(reading_bounds.x+2,reading_bounds.y+reading_bounds.h-2,1,1),false);break;
    case 31:
        ok=!a->doc_actions_open && !a->doc_find_open && documentation_h_same_rect(reading_bounds,a->doc_body_bounds) && reading_scroll==a->doc_scroll_y;
        test_window_key_mod(a->doc_window,SDLK_PLUS,PS_UI_COMMAND_MOD);break;
    case 32:
        ok=documentation_font_size(a)==reading_size+2;test_window_key_mod(a->doc_window,SDLK_0,PS_UI_COMMAND_MOD);break;
    case 33:
        if(!documentation_test_seek(a,DOC_ZOOM_IN,stage,code))return;
        test_window_key_mod(a->doc_window,SDLK_RETURN,0);break;
    case 34:
        ok=!a->doc_actions_open && documentation_font_size(a)==reading_size+2 && a->experiment.string.len==experiment &&
            a->analysis.string.len==analysis && a->simulation_time==simulation;
        *capture="docs-h-actions-zoom.bmp";test_window_key_mod(a->doc_window,SDLK_F10,SDL_KMOD_SHIFT);break;
    case 35:
        if(!documentation_test_seek(a,DOC_ZOOM_IN,stage,code))return;
        menu_item_bounds=a->doc_focus_bounds;test_window_mouse(a->doc_window,menu_item_bounds,true);break;
    case 36:test_window_mouse(a->doc_window,menu_item_bounds,false);break;
    case 37:
        ok=!a->doc_actions_open && !a->doc_action_pending && documentation_font_size(a)==reading_size+4 &&
            documentation_h_sources_unchanged(a,experiment_copy,experiment_bytes,analysis_copy,analysis_bytes) && a->simulation_time==simulation;
        *capture="docs-h-actions-pointer-zoom.bmp";break;
    case 38:
        if(!documentation_test_seek(a,DOC_ACTIONS,stage,code))return;
        {char label[96];snprintf(label,sizeof label,"Handbuchaktionen · Leseschrift %u px",documentation_font_size(a));
         ok=nk_sdl_accessibility_is_focused(a->doc_ui,NULL,label);}
        test_window_key_mod(a->doc_window,SDLK_RETURN,0);break;
    case 39:
        if(a->doc_actions_open && !nk_sdl_accessibility_is_focused(a->doc_ui,NULL,"Zurück")){--*stage;return;}
        ok=a->doc_actions_open;test_window_key_mod(a->doc_window,SDLK_ESCAPE,0);break;
    case 40:
        {char label[96];snprintf(label,sizeof label,"Handbuchaktionen · Leseschrift %u px",documentation_font_size(a));
         if(a->doc_focus==DOC_ACTIONS && !nk_sdl_accessibility_is_focused(a->doc_ui,NULL,label)){--*stage;return;}
         ok=!a->doc_actions_open && a->doc_focus==DOC_ACTIONS && SDL_GetKeyboardFocus()==a->doc_window &&
            nk_sdl_accessibility_is_focused(a->doc_ui,NULL,label) &&
            documentation_h_sources_unchanged(a,experiment_copy,experiment_bytes,analysis_copy,analysis_bytes) && a->simulation_time==simulation;}
        break;
    case 41:case 42: {
        SDL_Event event={0};event.type=*stage==42?SDL_EVENT_MOUSE_BUTTON_DOWN:SDL_EVENT_MOUSE_BUTTON_UP;
        event.button.windowID=SDL_GetWindowID(a->doc_window);event.button.which=PS_TEST_MOUSE_ID;
        event.button.button=SDL_BUTTON_RIGHT;event.button.down=event.type==SDL_EVENT_MOUSE_BUTTON_DOWN;
        event.button.x=a->doc_body_bounds.x+20;event.button.y=a->doc_body_bounds.y+a->doc_body_bounds.h/2;
        SDL_PushEvent(&event);break;
    }
    case 43:
        if(a->doc_actions_open && !nk_sdl_accessibility_is_focused(a->doc_ui,NULL,"Zurück")){--*stage;return;}
        ok=a->doc_actions_open && SDL_GetKeyboardFocus()==a->doc_window;
        *capture="docs-h-actions-context.bmp";test_window_key_mod(a->doc_window,SDLK_ESCAPE,0);break;
    case 44:
        ok=!a->doc_actions_open && documentation_h_sources_unchanged(a,experiment_copy,experiment_bytes,analysis_copy,analysis_bytes) && a->simulation_time==simulation;
        printf("DOCUMENTATION H SOURCE: experiment-bytes=%d analysis-bytes=%d byte-exact=%d footer-name-and-focus=%d\n",experiment_bytes,analysis_bytes,ok,ok);
        printf("DOCUMENTATION H SECONDARY: menu=keyboard,pointer,context escape-focus=restored find=overlay text-input=restored outside-click=consumed zoom=shortcut,menu,mouse source-preserved=%d\n",ok);
        reading_scroll=a->doc_scroll_y;frame=0;
        ok=ok && SDL_SetWindowSize(a->doc_window,900,wide_height) && SDL_SyncWindow(a->doc_window);break;
    case 45:
        if(frame++<2 || a->doc_scroll_pending){--*stage;return;}
        ok=!a->doc_navigation_hidden && !a->doc_context_hidden && a->doc_left_slot.w==220 &&
            a->doc_right_slot.w==220 && a->doc_article_slot.x==220 && a->doc_article_slot.w==460 &&
            a->doc_scroll_y==reading_scroll && a->doc_track==2 && a->doc_context_example &&
            !strcmp(a->doc_filter,"Pendel") && !strcmp(a->doc_query,reading_query);
        *capture="docs-h-breakpoint-900.bmp";break;
    case 46:
        frame=0;ok=SDL_SetWindowSize(a->doc_window,901,wide_height) && SDL_SyncWindow(a->doc_window);break;
    case 47:
        if(frame++<2 || a->doc_scroll_pending){--*stage;return;}
        ok=a->doc_left_slot.w==250 && a->doc_right_slot.w==250 && a->doc_article_slot.x==250 &&
            a->doc_article_slot.w==401 && a->doc_scroll_y==reading_scroll && a->doc_track==2 &&
            a->doc_context_example && !strcmp(a->doc_filter,"Pendel") && !strcmp(a->doc_query,reading_query);
        *capture="docs-h-breakpoint-901.bmp";break;
    case 48:
        ok=documentation_h_sources_unchanged(a,experiment_copy,experiment_bytes,analysis_copy,analysis_bytes) && a->simulation_time==simulation;
        printf("DOCUMENTATION H BREAKPOINT: widths=900,901 sides=220,250 source-and-view-preserved=%d\n",ok);
        a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Design H stage %d failed\n",*stage-1);*code=1;a->quitting=true;}
    if(a->quitting){free(experiment_copy);free(analysis_copy);experiment_copy=analysis_copy=NULL;printf("DOCUMENTATION H SELF-TEST: %s\n",*code?"FAILED":"PASSED");}
}
