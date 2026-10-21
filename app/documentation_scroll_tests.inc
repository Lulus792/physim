/* Exercise the real SDL queue and submitted native geometry, not a parallel
 * layout model. Each size/theme checks the article, including its breadcrumb. */
static void documentation_scroll_wheel(app *a,float x,float y,float delta) {
    SDL_Event e={0};e.type=SDL_EVENT_MOUSE_MOTION;e.motion.windowID=SDL_GetWindowID(a->doc_window);
    e.motion.which=PS_TEST_MOUSE_ID;e.motion.x=x;e.motion.y=y;SDL_PushEvent(&e);
    e=(SDL_Event){0};e.type=SDL_EVENT_MOUSE_WHEEL;e.wheel.windowID=SDL_GetWindowID(a->doc_window);
    e.wheel.which=PS_TEST_MOUSE_ID;e.wheel.mouse_x=x;e.wheel.mouse_y=y;e.wheel.y=delta;SDL_PushEvent(&e);
}
static void documentation_scroll_test_frame(app *a,int *stage,int *code,const char **capture) {
    static unsigned sample,frames;static int topic,heading,section;
    static struct nk_rect crumb,title,viewport,left,right;
    static nk_uint scroll;static uint64_t version;static uint32_t reading,experiment,analysis;
    static int cursor,selection_begin,selection_end;static char image[80];
    bool ok=true;
    switch((*stage)++) {
    case 0:
        experiment=ps_crc32((const unsigned char *)nk_str_get_const(&a->experiment.string),(size_t)a->experiment.string.len);
        analysis=ps_crc32((const unsigned char *)nk_str_get_const(&a->analysis.string),(size_t)a->analysis.string.len);
        test_window_key_mod(a->window,SDLK_F1,0);break;
    case 1:
        topic=-1;for(int i=0;i<DOCUMENTATION_TOPIC_COUNT;i++)if(!strcmp(documentation_topics[i].path,"docs/pendulum-tutorial.md"))topic=i;
        ok=topic>=0;sample=0;break;
    case 2: {
        static const int widths[]={1080,760,1440},heights[]={780,540,900};
        ps_preferences p=a->preferences;p.theme=sample%2?PS_THEME_LIGHT:PS_THEME_DARK;
        p.ui_size=sample%2?22:16;p.editor_size=16;preferences_apply(a,&p);
        a->doc_zoom_size=sample%2?32:16;
        ok=documentation_fonts_prepare(a,a->doc_zoom_size,documentation_code_size(a));
        a->doc_font_ui=a->doc_ui_fonts[(a->doc_zoom_size-16)/2];a->doc_font_title=a->doc_title_fonts[(a->doc_zoom_size-16)/2];
        a->doc_navigation_hidden=a->doc_context_hidden=false;
        a->doc_find_open=false;documentation_finish_edits(a);open_documentation(a,topic);
        ok=ok && SDL_RestoreWindow(a->doc_window) && SDL_SetWindowSize(a->doc_window,widths[sample/2],heights[sample/2]) && SDL_SyncWindow(a->doc_window);
        heading=section=-1;
        for(size_t i=0;i<a->documentation->count;i++)if(a->documentation->blocks[i].kind==PS_DOC_HEADING) {
            if(heading<0)heading=(int)i;else if(section<0)section=(int)i;
        }
        ok=ok && heading>=0 && section>=0;frames=0;SDL_RaiseWindow(a->doc_window);break;
    }
    case 3:
        if(frames++<3 || a->doc_scroll_pending || a->doc_jump_block>=0 || a->doc_history_restore){--*stage;return;}
        if(!documentation_test_seek(a,DOC_BODY,stage,code))return;
        test_window_key_mod(a->doc_window,SDLK_HOME,0);frames=0;break;
    case 4: {
        if(frames++<2){--*stage;return;}
        int w,h;SDL_GetWindowSize(a->doc_window,&w,&h);(void)h;
        float padding=w<=900?20:28,box=fminf(810,a->doc_article_slot.w);
        viewport=a->doc_body_bounds;crumb=a->doc_crumb_bounds;
        ps_doc_block *b=&a->documentation->blocks[heading];title=nk_rect(b->hit[0],b->hit[1],b->hit[2],b->hit[3]);
        left=a->doc_left_slot;right=a->doc_right_slot;version=a->doc_reading_version;
        reading=ps_crc32((const unsigned char *)a->documentation->reading,strlen(a->documentation->reading));
        ok=!a->doc_scroll_y && documentation_h_same_rect(viewport,a->doc_article_slot) &&
            fabsf(a->doc_article_column_bounds.w-(box-2*padding))<1 &&
            fabsf(crumb.x-a->doc_article_column_bounds.x)<1 && fabsf(crumb.w-a->doc_article_column_bounds.w)<1 &&
            fabsf(crumb.y-viewport.y-(w<=900?35:37))<1 &&
            fabsf(a->doc_active_topic_bounds.w-(left.w-31))<1;
        printf("DOCUMENTATION SCROLL GEOMETRY: case=%u viewport=%.0f,%.0f,%.0f,%.0f column=%.0f,%.0f nav=%.0f ok=%d\n",
            sample,viewport.x,viewport.y,viewport.w,viewport.h,a->doc_article_column_bounds.x,a->doc_article_column_bounds.w,a->doc_active_topic_bounds.w,ok);
        snprintf(image,sizeof image,"docs-scroll-%u-top.bmp",sample);*capture=image;
        test_window_key_mod(a->doc_window,SDLK_PAGEDOWN,0);frames=0;break;
    }
    case 5:
        if(frames++<2){--*stage;return;}
        scroll=a->doc_scroll_y;
        ok=scroll>0 && fabsf(a->doc_crumb_bounds.y-crumb.y+(float)scroll)<1 &&
            fabsf(a->documentation->blocks[heading].hit[1]-title.y+(float)scroll)<1 &&
            a->doc_crumb_bounds.y+a->doc_crumb_bounds.h<viewport.y &&
            documentation_h_same_rect(left,a->doc_left_slot) && documentation_h_same_rect(right,a->doc_right_slot);
        snprintf(image,sizeof image,"docs-scroll-%u-page.bmp",sample);*capture=image;
        *stage=51;break;
    case 51:
        /* Both events precede the next layout: a cached old height cannot
         * satisfy End after this reflow. Keep the page capture before either. */
        test_window_key_mod(a->doc_window,sample%2?SDLK_MINUS:SDLK_PLUS,PS_UI_COMMAND_MOD);
        test_window_key_mod(a->doc_window,SDLK_END,0);frames=0;*stage=6;break;
    case 6:
        if(frames++<2){--*stage;return;}
        {const ps_doc_block *last=NULL;
         for(size_t i=0;i<a->documentation->count;i++)if(a->documentation->blocks[i].inline_parent==SIZE_MAX &&
             a->documentation->blocks[i].hit[2]>0)last=&a->documentation->blocks[i];
         float bottom=last?last->hit[1]+last->hit[3]:-1,gap=viewport.y+viewport.h-bottom;
         ok=a->doc_scroll_y==a->doc_scroll_limit && a->doc_scroll_y>scroll && last &&
             bottom>viewport.y && gap>=35 && gap<=40+2*documentation_font_size(a);
         printf("DOCUMENTATION SCROLL END: case=%u scroll=%u limit=%u last=%.1f gap=%.1f ok=%d\n",sample,a->doc_scroll_y,a->doc_scroll_limit,bottom,gap,ok);}
        snprintf(image,sizeof image,"docs-scroll-%u-end.bmp",sample);*capture=image;
        *stage=61;break;
    case 61:
        documentation_zoom(a,sample%2?1:-1);
        test_window_key_mod(a->doc_window,SDLK_HOME,0);frames=0;*stage=7;break;
    case 7:
        if(frames++<2){--*stage;return;}
        ok=!a->doc_scroll_y && documentation_h_same_rect(crumb,a->doc_crumb_bounds);
        documentation_zoom(a,sample%2?-1:1);frames=0;break;
    case 8:
        if(frames++<2 || a->doc_scroll_pending){--*stage;return;}
        ok=!a->doc_scroll_y && a->doc_crumb_bounds.y>=viewport.y;
        documentation_zoom(a,sample%2?1:-1);documentation_navigation_toggle(a);frames=0;break;
    case 9:
        if(frames++<2 || a->doc_scroll_pending){--*stage;return;}
        ok=!a->doc_scroll_y && a->doc_navigation_hidden;
        documentation_navigation_toggle(a);frames=0;break;
    case 10:
        if(frames++<2 || a->doc_scroll_pending){--*stage;return;}
        ok=!a->doc_scroll_y && documentation_h_same_rect(crumb,a->doc_crumb_bounds);
        test_window_key_mod(a->doc_window,SDLK_F,PS_UI_COMMAND_MOD);frames=0;break;
    case 11:
        if(frames++<2 || !a->doc_search_active || a->doc_focus_reveal){--*stage;return;}
        ok=a->doc_find_open && !a->doc_scroll_y && documentation_h_same_rect(viewport,a->doc_body_bounds);
        {SDL_Event e={0};e.type=SDL_EVENT_TEXT_INPUT;e.text.windowID=SDL_GetWindowID(a->doc_window);e.text.text="Pendel";SDL_PushEvent(&e);}
        frames=0;*stage=111;break;
    case 111:
        if(frames++<2 || a->doc_scroll_pending){--*stage;return;}
        ok=!strcmp(a->doc_query,"Pendel") && a->doc_search_active && !a->doc_scroll_y;
        test_window_key_mod(a->doc_window,SDLK_HOME,0);
        frames=0;*stage=113;break;
    case 113:
        if(frames++<2){--*stage;return;}
        test_window_key_mod(a->doc_window,SDLK_RIGHT,SDL_KMOD_SHIFT);frames=0;*stage=112;break;
    case 112:
        if(frames++<2){--*stage;return;}
        cursor=a->doc_ui->text_edit.cursor;selection_begin=a->doc_ui->text_edit.select_start;selection_end=a->doc_ui->text_edit.select_end;
        ok=cursor==1 && selection_begin==0 && selection_end==1;
        documentation_scroll_wheel(a,viewport.x+1,viewport.y+viewport.h*.75f,-2);frames=0;*stage=12;break;
    case 12:
        if(frames++<2){--*stage;return;}
        scroll=a->doc_scroll_y;
        ok=scroll>0 && a->doc_search_active && a->doc_focus==DOC_SEARCH && !strcmp(a->doc_query,"Pendel") &&
            a->doc_ui->text_edit.cursor==cursor && a->doc_ui->text_edit.select_start==selection_begin &&
            a->doc_ui->text_edit.select_end==selection_end &&
            fabsf(a->doc_crumb_bounds.y-crumb.y+(float)scroll)<1;
        documentation_scroll_wheel(a,a->doc_find_bounds.x+a->doc_find_bounds.w/2,a->doc_find_bounds.y+a->doc_find_bounds.h/2,-2);frames=0;break;
    case 13:
        if(frames++<2){--*stage;return;}
        ok=a->doc_scroll_y==scroll && a->doc_search_active && a->doc_ui->text_edit.cursor==cursor &&
            a->doc_ui->text_edit.select_start==selection_begin && a->doc_ui->text_edit.select_end==selection_end;
        a->doc_jump_block=section;frames=0;break;
    case 14:
        if(frames++<2 || a->doc_scroll_pending || a->doc_jump_block>=0){--*stage;return;}
        ok=a->documentation->blocks[section].hit[1]>=a->doc_find_bounds.y+a->doc_find_bounds.h &&
            a->documentation->blocks[section].hit[1]<viewport.y+viewport.h;
        snprintf(image,sizeof image,"docs-scroll-%u-find.bmp",sample);*capture=image;
        test_window_key_mod(a->doc_window,SDLK_ESCAPE,0);frames=0;break;
    case 15:
        if(frames++<2 || a->doc_scroll_pending){--*stage;return;}
        ok=a->doc_reading_version==version && reading==ps_crc32((const unsigned char *)a->documentation->reading,strlen(a->documentation->reading)) &&
            experiment==ps_crc32((const unsigned char *)nk_str_get_const(&a->experiment.string),(size_t)a->experiment.string.len) &&
            analysis==ps_crc32((const unsigned char *)nk_str_get_const(&a->analysis.string),(size_t)a->analysis.string.len);
        printf("DOCUMENTATION SCROLL CASE: case=%u size=%u theme=%u complete=%d\n",sample,documentation_font_size(a),a->preferences.theme,ok);
        a->doc_query[0]=a->doc_last_query[0]=0;
        if(++sample<6)*stage=2;else a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"Documentation scroll failed stage=%d case=%u scroll=%u limit=%u crumb=%.1f body=%.1f focus=%d search=%d\n",
        *stage-1,sample,a->doc_scroll_y,a->doc_scroll_limit,a->doc_crumb_bounds.y,a->doc_body_bounds.y,a->doc_focus,a->doc_search_active);*code=1;a->quitting=true;}
    if(a->quitting)printf("DOCUMENTATION SCROLL SELF-TEST: %s\n",*code?"FAILED":"PASSED");
}
