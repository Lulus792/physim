static void documentation_code_test_motion(app *a,float x,float y,bool down) {
    SDL_Event event;SDL_zero(event);event.type=SDL_EVENT_MOUSE_MOTION;event.motion.windowID=SDL_GetWindowID(a->doc_window);
    event.motion.which=PS_TEST_MOUSE_ID;
    event.motion.x=x;event.motion.y=y;event.motion.state=down?SDL_BUTTON_LMASK:0;SDL_PushEvent(&event);
}
static void documentation_code_test_wheel(app *a,float x,float y) {
    struct nk_rect r=a->doc_context_code_bounds,clip=a->doc_context_content_bounds;
    float px=r.x+30,py=fmaxf(r.y+20,clip.y+20);py=fminf(py,clip.y+clip.h-20);
    documentation_code_test_motion(a,px,py,false);
    SDL_Event event;SDL_zero(event);event.type=SDL_EVENT_MOUSE_WHEEL;event.wheel.windowID=SDL_GetWindowID(a->doc_window);
    event.wheel.which=PS_TEST_MOUSE_ID;
    event.wheel.x=x;event.wheel.y=y;event.wheel.mouse_x=px;event.wheel.mouse_y=py;SDL_PushEvent(&event);
}
static void documentation_code_preview_test_frame(app *a,int *stage,int *code,const char **capture) {
    static unsigned sample,frames;static int experiment,analysis;static uint32_t experiment_hash,analysis_hash;
    static double simulation;static nk_uint scroll;static float before;static char image[80];bool ok=true;
    switch((*stage)++) {
    case 0:
        experiment=a->experiment.string.len;analysis=a->analysis.string.len;
        experiment_hash=ps_crc32((const unsigned char *)nk_str_get_const(&a->experiment.string),(size_t)experiment);
        analysis_hash=ps_crc32((const unsigned char *)nk_str_get_const(&a->analysis.string),(size_t)analysis);
        simulation=a->simulation_time;sample=0;test_window_key_mod(a->window,SDLK_F1,0);break;
    case 1:
        for(int i=0;i<DOCUMENTATION_TOPIC_COUNT;i++)if(!strcmp(documentation_topics[i].path,"docs/pendulum-tutorial.md")){open_documentation(a,i);break;}
        break;
    case 2: {
        bool large=sample>=3;ps_preferences p=a->preferences;p.ui_size=large?22:16;p.editor_size=large?22:16;
        p.theme=(int)(sample%3);preferences_apply(a,&p);documentation_language(a,sample%2?2:1);
        a->doc_context_example=true;a->doc_context_hidden=false;a->doc_navigation_hidden=true;
        a->doc_context_scroll_y[1]=0;a->doc_context_code_x[0]=a->doc_context_code_x[1]=0;
        ok=SDL_RestoreWindow(a->doc_window) && SDL_SetWindowSize(a->doc_window,large?760:1080,large?540:780) && SDL_SyncWindow(a->doc_window);
        const char *source=documentation_context_code(a);ok=ok && source && strlen(source)>1000;
        if(sample==6) {
            ps_code_preview_destroy(a->doc_context_code_layout);a->doc_context_code_layout=NULL;
            free(a->doc_context_source);a->doc_context_source=malloc(6200);ok=ok && a->doc_context_source;
            if(a->doc_context_source) {
                char *out=a->doc_context_source;const char *prefix="// Preserve whitespace α😀\n\t";
                size_t n=strlen(prefix);memcpy(out,prefix,n);memset(out+n,'X',6000);
                snprintf(out+n+6000,6200-n-6000,"TAIL_θ\n\nlast_line\n");
            }
        }
        frames=0;break;
    }
    case 3:
        if(frames++<2){--*stage;return;}
        ok=a->doc_context_code_layout && a->doc_context_code_max>0 && a->doc_context_code_bounds.w>150;
        a->doc_keyboard=true;a->doc_focus=DOC_BODY;a->doc_focus_reveal=false;
        test_window_key_mod(a->doc_window,SDLK_F6,0);break;
    case 4:
        ok=a->doc_focus==DOC_EXAMPLE_TAB;test_window_key_mod(a->doc_window,SDLK_TAB,0);break;
    case 5:
        ok=a->doc_focus==DOC_CONTEXT_COPY;
        snprintf(image,sizeof image,"docs-code-%u-head.bmp",sample);*capture=image;
        test_window_key_mod(a->doc_window,SDLK_TAB,0);frames=0;break;
    case 6:
        if(frames++<2 || a->doc_focus_reveal){--*stage;return;}
        ok=a->doc_focus==DOC_CONTEXT_CODE && nk_sdl_accessibility_is_focused(a->doc_ui,NULL,"Beispielcode horizontal scrollen");
        {struct nk_rect h=a->doc_context_head_bounds,p=a->doc_context_code_bounds;struct nk_color edit=UI_COLOR(edit),divider=UI_COLOR(divider);
         printf("DOCUMENTATION CODE FRAME: case=%u head=%.0f,%.0f,%.0f,%.0f pre=%.0f,%.0f,%.0f,%.0f edit=%u,%u,%u divider=%u,%u,%u\n",
            sample,h.x,h.y,h.w,h.h,p.x,p.y,p.w,p.h,edit.r,edit.g,edit.b,divider.r,divider.g,divider.b);}
        snprintf(image,sizeof image,"docs-code-%u-start.bmp",sample);*capture=image;
        test_window_key_mod(a->doc_window,SDLK_END,0);break;
    case 7:
        ok=fabsf(a->doc_context_code_x[a->doc_track==2]-a->doc_context_code_max)<1;
        snprintf(image,sizeof image,"docs-code-%u-end.bmp",sample);*capture=image;
        test_window_key_mod(a->doc_window,SDLK_HOME,0);break;
    case 8:
        ok=a->doc_context_code_x[a->doc_track==2]==0;scroll=a->doc_context_scroll_y[1];
        documentation_code_test_wheel(a,-2,0);frames=0;break;
    case 9:
        if(frames++<1){--*stage;return;}
        ok=a->doc_context_code_x[a->doc_track==2]>0 && a->doc_context_scroll_y[1]==scroll;
        test_window_key_mod(a->doc_window,SDLK_HOME,0);
        /* SDL_Event is a union: initialize all bytes before selecting a member,
         * including the keyboard down/repeat fields consumed by the UI. */
        {SDL_Event e;SDL_zero(e);e.type=SDL_EVENT_KEY_DOWN;e.key.windowID=SDL_GetWindowID(a->doc_window);e.key.key=SDLK_LSHIFT;e.key.mod=SDL_KMOD_SHIFT;e.key.down=true;SDL_PushEvent(&e);}
        documentation_code_test_wheel(a,0,-2);frames=0;break;
    case 10:
        if(frames++<1){--*stage;return;}
        ok=a->doc_context_code_x[a->doc_track==2]>0 && a->doc_context_scroll_y[1]==scroll;
        {SDL_Event e;SDL_zero(e);e.type=SDL_EVENT_KEY_UP;e.key.windowID=SDL_GetWindowID(a->doc_window);e.key.key=SDLK_LSHIFT;SDL_PushEvent(&e);}
        a->doc_context_scroll_y[1]=1000000;frames=0;break;
    case 11:
        if(frames++<2){--*stage;return;}
        {struct nk_rect bar=a->doc_context_code_bar,clip=a->doc_context_content_bounds;
         ok=bar.y>=clip.y && bar.y+bar.h<=clip.y+clip.h;
         float width=a->doc_context_code_layout->width,max=a->doc_context_code_max;
         before=a->doc_context_code_x[a->doc_track==2];float thumb=fminf(bar.w,fmaxf(18,bar.w*width/(max+width)));
         test_window_mouse(a->doc_window,nk_rect(bar.x+(bar.w-thumb)*before/max+thumb/2,bar.y+bar.h/2,1,1),true);}
        break;
    case 12:documentation_code_test_motion(a,a->doc_context_code_bar.x+a->doc_context_code_bar.w-1,a->doc_context_code_bar.y+3,true);break;
    case 13:test_window_mouse(a->doc_window,nk_rect(a->doc_context_code_bar.x+a->doc_context_code_bar.w-1,a->doc_context_code_bar.y+3,1,1),false);frames=0;break;
    case 14:
        if(frames++<1){--*stage;return;}
        ok=a->doc_context_code_x[a->doc_track==2]>before && !a->doc_context_code_drag.active;
        snprintf(image,sizeof image,"docs-code-%u-drag.bmp",sample);*capture=image;
        test_window_key_mod(a->doc_window,SDLK_TAB,SDL_KMOD_SHIFT);frames=0;break;
    case 15:
        if(frames++<2 || a->doc_focus_reveal){--*stage;return;}
        ok=a->doc_focus==DOC_CONTEXT_COPY && a->doc_context_copy_bounds.y>=a->doc_context_content_bounds.y;
        SDL_SetClipboardText("sentinel");test_window_key_mod(a->doc_window,SDLK_RETURN,0);break;
    case 16: {
        const char *source=documentation_context_code(a);
        ok=documentation_test_clipboard(source) && a->experiment.string.len==experiment && a->analysis.string.len==analysis && a->simulation_time==simulation &&
            ps_crc32((const unsigned char *)nk_str_get_const(&a->experiment.string),(size_t)experiment)==experiment_hash &&
            ps_crc32((const unsigned char *)nk_str_get_const(&a->analysis.string),(size_t)analysis)==analysis_hash;
        printf("DOCUMENTATION CODE PREVIEW: case=%u track=%d ui=%u editor=%u theme=%d bytes=%zu rows=%zu max=%.3f keys=1 wheel=1 shift-wheel=1 drag=1 clipboard-and-editors=%d synthetic=%d\n",
            sample,a->doc_track,a->preferences.ui_size,a->preferences.editor_size,a->preferences.theme,strlen(source),a->doc_context_code_layout->count,a->doc_context_code_max,ok,sample==6);
        fflush(stdout);
        if(++sample<7)*stage=2;else a->quitting=true;break;
    }
    }
    if(!ok){fprintf(stderr,"Code preview stage=%d case=%u focus=%d x=%.2f max=%.2f scroll=%u\n",*stage-1,sample,a->doc_focus,a->doc_context_code_x[a->doc_track==2],a->doc_context_code_max,a->doc_context_scroll_y[1]);*code=1;a->quitting=true;}
    if(a->quitting)printf("DOCUMENTATION CODE PREVIEW SELF-TEST: %s\n",*code?"FAILED":"PASSED");
}
