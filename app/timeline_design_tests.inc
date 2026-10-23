/* Actual adaptive runner, native input and the approved timeline presentation. */
static struct {
    unsigned settle,variant,expected;bool seen_play,complete;double held,fraction;uint32_t source,run_crc,view_crc;
    char run[4096],root[4096],tick_text[7][80];struct nk_rect tick_ink[7];SDL_Window *sink;
} timeline_design;
static const char *timeline_design_variant(void) {
    static const char *names[]={"dark","light","medium","larger","large","contrast","minimum","detached-min","detached-browse","detached-playing","detached-wide","returned"};
    return names[timeline_design.variant<SDL_arraysize(names)?timeline_design.variant:0];
}
static SDL_Window *timeline_design_window(app *a) {return panel_is_detached(a,PS_PANEL_TIMELINE)?a->panels[PS_PANEL_TIMELINE].window:a->window;}
static struct nk_context *timeline_design_ui(app *a) {return panel_is_detached(a,PS_PANEL_TIMELINE)?a->panels[PS_PANEL_TIMELINE].ui:a->ui;}
static bool timeline_design_source(app *a) {
    return !a->dirty && timeline_design.source==ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string)) &&
        (!timeline_design.run[0] || !strcmp(timeline_design.run,a->last_run)) && a->simulation_time==timeline_design.held;
}
static bool timeline_design_start(app *a,const char *root) {
    memset(&timeline_design,0,sizeof timeline_design);snprintf(timeline_design.root,sizeof timeline_design.root,"%s",root);
    if(!laboratory_test_start(a,root,"laboratory-dark"))return false;
    join(a->preferences_path,sizeof a->preferences_path,root,"preferences.bin");
    a->adaptive_steps=true;a->dt=.001;a->minimum_dt=1e-6;a->maximum_dt=.2;a->project_settings_dirty=true;
    timeline_design.source=ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));a->timeline_design_active=true;return true;
}
static bool timeline_design_run_crc(const char *path,uint32_t *crc) {
    if(reset_test_crc(path,crc))return true;
    fprintf(stderr,"TIMELINE LIVE FILE SDL READ: %s\n",SDL_GetError());
    /* The run uses C stdio. Keep that sharing contract when SDL's Windows
     * file open rejects an already writable handle. No writer flag changes. */
    errno=0;FILE *file=fopen(path,"rb");if(!file){fprintf(stderr,"TIMELINE STDIO READ FAILED: %s\n",strerror(errno));return false;}
    bool ok=fseek(file,0,SEEK_END)==0;long length=ok?ftell(file):-1;
    ok=ok && length>=0 && length<=32*1024*1024 && fseek(file,0,SEEK_SET)==0;
    unsigned char *bytes=ok?malloc(length?(size_t)length:1):NULL;ok=ok && bytes;
    if(ok)ok=fread(bytes,1,(size_t)length,file)==(size_t)length && !ferror(file);
    if(ok)*crc=ps_crc32(bytes,(size_t)length);
    free(bytes);if(fclose(file))ok=false;
    if(ok)fprintf(stderr,"TIMELINE LIVE FILE STDIO READ: bytes=%ld crc=%08x\n",length,*crc);
    return ok;
}
static void timeline_design_mouse(app *a,double fraction,bool down) {
    struct nk_rect r=a->timeline_track_bounds;float f=a->preferences.ui_size/16.f;
    test_window_mouse(timeline_design_window(a),nk_rect(r.x+8*f+(r.w-16*f)*(float)fraction-.5f,r.y,1,r.h),down);
}
static void timeline_design_motion(app *a,double fraction) {
    struct nk_rect r=a->timeline_track_bounds;float f=a->preferences.ui_size/16.f;SDL_Event e={0};e.type=SDL_EVENT_MOUSE_MOTION;e.motion.windowID=SDL_GetWindowID(timeline_design_window(a));e.motion.which=PS_TEST_MOUSE_ID;e.motion.x=r.x+8*f+(r.w-16*f)*(float)fraction;e.motion.y=r.y+r.h/2;SDL_PushEvent(&e);
}
static bool timeline_design_focus(app *a,const char *name,bool request) {
    SDL_Window *window=timeline_design_window(a);struct nk_context *ui=timeline_design_ui(a);
    if(SDL_GetKeyboardFocus()==window && nk_sdl_accessibility_is_focused(ui,NULL,name))return true;
    SDL_RaiseWindow(window);if(request)nk_sdl_accessibility_request_focus(ui,NULL,name);return false;
}
static void timeline_design_capture(app *a,const char *state,const char **capture) {
    static char file[96];snprintf(file,sizeof file,"timeline-%s-%s.png",timeline_design_variant(),state);
    if(panel_is_detached(a,PS_PANEL_TIMELINE))join(a->panels[PS_PANEL_TIMELINE].capture,sizeof a->panels[PS_PANEL_TIMELINE].capture,timeline_design.root,file);else *capture=file;
}
static void timeline_design_draw(app *a) {
    bool heading=false,play=false,live=false;unsigned numbers=0,ticks=0;const struct nk_command *cmd;float factor=a->preferences.ui_size/16.f;
    memset(timeline_design.tick_text,0,sizeof timeline_design.tick_text);memset(timeline_design.tick_ink,0,sizeof timeline_design.tick_ink);
    nk_foreach(cmd,a->ui)if(cmd->type==NK_COMMAND_TEXT) {
        const struct nk_command_text *t=(const struct nk_command_text*)cmd;
        if(t->y<a->timeline_heading_bounds.y-1 || t->y>a->timeline_track_bounds.y+34*factor)continue;
        if(t->font==&a->workbench_fonts[3] && t->length==10 && !memcmp(t->string,"Zeitleiste",10))heading=true;
        const char *caption=a->timeline_playing?"Ⅱ Anhalten":"▷ Abspielen";
        if(t->font==&a->workspace_status_font && t->length==(int)strlen(caption) && !memcmp(t->string,caption,(size_t)t->length))play=true;
        if(t->font==&a->workspace_status_font && t->length==strlen("● Live") && !memcmp(t->string,"● Live",strlen("● Live")))live=true;
        if(t->font==&a->laboratory_timeline_font && t->length==1 && t->string[0]>='0' && t->string[0]<='9')numbers++;
        if(t->font==&a->workbench_fonts[1] && t->y>=a->timeline_track_bounds.y+19*factor && ticks<7 && t->length>0 && t->length<80) {
            memcpy(timeline_design.tick_text[ticks],t->string,(size_t)t->length);timeline_design.tick_ink[ticks]=nk_rect(t->x,t->y,t->w,t->height);ticks++;
        }
    }
    timeline_design.complete=heading && play && live && numbers && ticks==a->timeline_tick_count;
}
static bool timeline_design_geometry(app *a) {
    int w,h;SDL_GetWindowSize(timeline_design_window(a),&w,&h);float f=a->preferences.ui_size/16.f;struct nk_rect range=a->timeline_track_bounds,play=a->timeline_bounds[3],live=a->timeline_bounds[2],readout=a->timeline_readout_bounds,heading=a->timeline_heading_bounds;
    bool ok=timeline_design.complete && fabsf(live.x+live.w-(w-18))<.01f && fabsf(play.x+play.w+6*f-live.x)<.01f && fabsf(play.h-24.5f*f)<.01f && fabsf(live.h-play.h)<.01f &&
        fabsf(range.h-17*f)<.01f && fabsf(range.x-(readout.x+readout.w+25*f))<.01f && fabsf(range.x+range.w-(w-18))<.01f && readout.x==18 && range.y>heading.y+heading.h &&
        fabsf(a->laboratory_timeline_font.height/a->workbench_regular_em/f-18)<.001f && a->timeline_tick_count>=2 && a->timeline_tick_count<=7 && a->timeline_control_window==SDL_GetWindowID(timeline_design_window(a));
    for(unsigned i=0;i<a->timeline_tick_count;i++) {
        struct nk_rect r=a->timeline_tick_bounds[i];ok=ok && r.x>=range.x-.01f && r.x+r.w<=range.x+range.w+.01f && r.y>=range.y+range.h && r.y+r.h<=h-WORKSPACE_STATUS_HEIGHT*(panel_is_detached(a,PS_PANEL_TIMELINE)?0:1)+.01f;
        if(i)ok=ok && a->timeline_tick_values[i]>a->timeline_tick_values[i-1] && r.x>=a->timeline_tick_bounds[i-1].x+a->timeline_tick_bounds[i-1].w+2;
    }
    return ok && timeline_design_source(a);
}
static void timeline_design_log(app *a,const char *state) {
    printf("TIMELINE DESIGN %s variant=%s size=%u complete=%d source=%d browsing=%d playing=%d time=%.17g end=%.17g owner=%u",state,timeline_design_variant(),a->preferences.ui_size,timeline_design.complete,timeline_design_source(a),a->timeline_browsing,a->timeline_playing,display_time(a),a->timeline.latest.time,a->timeline_control_window);
    const struct nk_rect r[]={a->timeline_heading_bounds,a->timeline_bounds[3],a->timeline_bounds[2],a->timeline_readout_bounds,a->timeline_track_bounds};const char *names[]={"heading","play","live","readout","range"};
    for(unsigned i=0;i<5;i++)printf(" %s=%.4f,%.4f,%.4f,%.4f",names[i],r[i].x,r[i].y,r[i].w,r[i].h);
    puts("");
    for(unsigned i=0;i<a->timeline_tick_count;i++){struct nk_rect b=timeline_design.tick_ink[i];printf("TIMELINE TICK variant=%s value=%.17g rect=%.4f,%.4f,%.4f,%.4f text=%s\n",timeline_design_variant(),a->timeline_tick_values[i],b.x,b.y,b.w,b.h,timeline_design.tick_text[i]);}
}
static void timeline_design_frame(app *a,int *stage,int *code,const char **capture) {
    bool ok=true;uint32_t count=ps_timeline_count(&a->timeline);
    switch(*stage) {
    case 0:if(timeline_design.settle++<3)return;build_project(a);break;
    case 1:if(!idle(a) || a->library_thread)return;ok=a->built;if(ok){start_run(a);snprintf(timeline_design.run,sizeof timeline_design.run,"%s",a->last_run);}break;
    case 2:if(!a->hello || a->simulation_time<6 || count<10)return;command(a,PS_MSG_PAUSE);timeline_design.settle=0;break;
    case 3:if(!a->paused || timeline_design.settle++<3)return;timeline_design.held=a->simulation_time;ok=timeline_design_run_crc(a->last_run,&timeline_design.run_crc);break;
    case 4: {
        if(SDL_GetKeyboardFocus()!=a->window){SDL_RaiseWindow(a->window);return;}
        bool found=false;double first=ps_timeline_get(&a->timeline,0)->time,end=a->timeline.latest.time;
        for(unsigned j=1;j<20 && !found;j++) {
            double f=j/20.,target=first+(end-first)*f,best=DBL_MAX;unsigned selected=0;
            for(unsigned i=0;i<count;i++){double distance=fabs(ps_timeline_get(&a->timeline,i)->time-target);if(distance<best){best=distance;selected=i;}}
            if(selected+1<count && selected!=(unsigned)lround(f*(count-1))){timeline_design.expected=selected;timeline_design.fraction=f;found=true;}
        }
        ok=found && timeline_design_source(a);printf("TIMELINE NONUNIFORM: count=%u fraction=%.17g expected=%u old-index=%u start=%.17g end=%.17g\n",count,timeline_design.fraction,timeline_design.expected,(unsigned)lround(timeline_design.fraction*(count-1)),first,end);
        for(unsigned i=0;i<count;i++)printf("TIMELINE SAMPLE index=%u time=%.17g\n",i,ps_timeline_get(&a->timeline,i)->time);
        if(ok){timeline_design_mouse(a,.9,true);timeline_design_motion(a,timeline_design.fraction);timeline_design_mouse(a,timeline_design.fraction,false);}break;
    }
    case 5:break;
    case 6:ok=a->timeline_browsing && !a->timeline_dragging && a->paused && display_time(a)==ps_timeline_get(&a->timeline,timeline_design.expected)->time && timeline_design_source(a);
        if(!timeline_test_key(a,a->window,a->ui,SDLK_RIGHT))return;
        printf("TIMELINE POINTER BATCH: wanted=%u actual=%u time=%.17g drag=%d\n",timeline_design.expected,ps_timeline_nearest(&a->timeline,display_time(a)),display_time(a),a->timeline_dragging);break;
    case 7:if(display_time(a)!=ps_timeline_get(&a->timeline,timeline_design.expected+1)->time)return;ok=timeline_design_source(a);if(!timeline_test_key(a,a->window,a->ui,SDLK_LEFT))return;break;
    case 8:if(display_time(a)!=ps_timeline_get(&a->timeline,timeline_design.expected)->time)return;ok=timeline_design_source(a);if(!timeline_test_key(a,a->window,a->ui,SDLK_HOME))return;break;
    case 9:ok=display_time(a)==ps_timeline_get(&a->timeline,0)->time;if(!timeline_test_key(a,a->window,a->ui,SDLK_END))return;break;
    case 10:ok=display_time(a)==a->timeline.latest.time;if(!timeline_test_key(a,a->window,a->ui,SDLK_SPACE))return;timeline_design.seen_play=false;break;
    case 11:if(a->timeline_playing){timeline_design.seen_play=true;return;}if(!timeline_design.seen_play)return;ok=a->timeline_browsing && display_time(a)==a->timeline.latest.time && timeline_design_source(a);test_mouse(a,a->timeline_bounds[2],true);break;
    case 12:test_mouse(a,a->timeline_bounds[2],false);break;
    case 13:ok=!a->timeline_browsing && !a->timeline_playing;timeline_design_mouse(a,.25,true);break;
    case 14:timeline_design_motion(a,.6);break;
    case 15:ok=a->timeline_dragging && a->timeline_browsing && timeline_design_source(a);test_window_key_mod(a->window,SDLK_ESCAPE,0);break;
    case 16:ok=!a->timeline_dragging && !a->timeline_browsing && timeline_design_source(a);timeline_design_mouse(a,.6,false);break;
    case 17:timeline_design_mouse(a,.3,true);break;
    case 18:timeline_design_mouse(a,.3,false);break;
    case 19:ok=a->timeline_browsing;timeline_design.view_crc=timeline_test_crc(&a->timeline_view);timeline_design_mouse(a,.8,true);break;
    case 20:ok=a->timeline_dragging && timeline_test_crc(&a->timeline_view)!=timeline_design.view_crc;SDL_SetWindowSize(a->window,1080,740);timeline_design.settle=0;break;
    case 21:if(timeline_design.settle++<3)return;ok=!a->timeline_dragging && timeline_test_crc(&a->timeline_view)==timeline_design.view_crc && timeline_design_source(a);timeline_design_mouse(a,.8,false);break;
    case 22:timeline_design_mouse(a,.8,true);break;
    case 23:ok=a->timeline_dragging;timeline_design.sink=SDL_CreateWindow("Physim timeline focus probe",160,100,SDL_WINDOW_UTILITY);ok=ok && timeline_design.sink;if(ok)SDL_RaiseWindow(timeline_design.sink);break;
    case 24:
        if(SDL_GetKeyboardFocus()!=timeline_design.sink || a->timeline_dragging){SDL_RaiseWindow(timeline_design.sink);return;}
        ok=!a->timeline_dragging && timeline_test_crc(&a->timeline_view)==timeline_design.view_crc && timeline_design_source(a);SDL_DestroyWindow(timeline_design.sink);timeline_design.sink=NULL;SDL_RaiseWindow(a->window);break;
    case 25:if(SDL_GetKeyboardFocus()!=a->window){SDL_RaiseWindow(a->window);return;}timeline_design_mouse(a,.8,false);*stage=40;return;
    case 40: {
        const unsigned sizes[]={16,16,18,20,22,22};ps_preferences p=a->preferences;p.ui_size=sizes[timeline_design.variant];p.theme=timeline_design.variant==0 || timeline_design.variant==2?PS_THEME_DARK:timeline_design.variant==5?PS_THEME_HIGH_CONTRAST:PS_THEME_LIGHT;preferences_apply(a,&p);
        SDL_SetWindowSize(a->window,timeline_design.variant<2?1440:1080,timeline_design.variant<2?1000:740);timeline_design.settle=0;break;
    }
    case 41:if(timeline_design.settle++<3)return;test_mouse(a,a->timeline_bounds[2],true);break;
    case 42:test_mouse(a,a->timeline_bounds[2],false);timeline_design.settle=0;break;
    case 43:if(timeline_design.settle++<2)return;ok=timeline_design_geometry(a) && !a->timeline_browsing;timeline_design_log(a,"normal");timeline_design_capture(a,"normal",capture);break;
    case 44:if(!timeline_design_focus(a,"Abspielen",true))return;timeline_design_capture(a,"focus",capture);break;
    case 45:test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 46:if(!a->timeline_playing || !timeline_design_focus(a,"Anhalten",false))return;ok=timeline_design_geometry(a);timeline_design_log(a,"playing");timeline_design_capture(a,"playing",capture);break;
    case 47:test_window_key_mod(a->window,SDLK_RETURN,0);break;
    case 48:if(a->timeline_playing || !timeline_design_focus(a,"Abspielen",false))return;timeline_design_mouse(a,.4,true);break;
    case 49:timeline_design_mouse(a,.4,false);timeline_design.settle=0;break;
    case 50:if(timeline_design.settle++<2)return;ok=timeline_design_geometry(a) && a->timeline_browsing && !a->timeline_playing;timeline_design_log(a,"browse");timeline_design_capture(a,"browse",capture);timeline_design.settle=0;break;
    case 51:
        if(!timeline_design.settle){if(!timeline_design_focus(a,"Position im gespeicherten Lauf",false))return;test_window_key_mod(a->window,SDLK_RIGHT,0);timeline_design.settle=1;return;}
        if(timeline_design.settle++<3)return;
        ok=timeline_design_geometry(a) && !a->timeline_pointer_focus;timeline_design_capture(a,"range-focus",capture);
        printf("TIMELINE ACTIONS variant=%s native-enter=1 automatic-return=1 time-seek=1 preserved=1\n",timeline_design_variant());if(++timeline_design.variant<6){*stage=40;return;}break;
    case 52:{ps_preferences p=a->preferences;p.theme=PS_THEME_LIGHT;preferences_apply(a,&p);nk_sdl_accessibility_request_focus(a->ui,NULL,"Zeitleistenhöhe");break;}
    case 53:if(!timeline_design_focus(a,"Zeitleistenhöhe",true))return;test_window_key_mod(a->window,SDLK_HOME,0);timeline_design.variant=6;timeline_design.settle=0;break;
    case 54:if(timeline_design.settle++<3)return;ok=laboratory_dimension_value(a,1)==95 && timeline_design_geometry(a);timeline_design_log(a,"minimum");timeline_design_capture(a,"normal",capture);test_mouse(a,a->panels[PS_PANEL_TIMELINE].action,true);break;
    case 55:{int w,h;SDL_GetWindowSize(a->window,&w,&h);test_plot_motion(a,(float)w+40,a->timeline_heading_bounds.y+12);break;}
    case 56:{int w,h;SDL_GetWindowSize(a->window,&w,&h);test_mouse(a,nk_rect((float)w+40,a->timeline_heading_bounds.y+12,1,1),false);break;}
    case 57:if(!panel_is_detached(a,PS_PANEL_TIMELINE))return;SDL_SetWindowSize(a->panels[PS_PANEL_TIMELINE].window,480,160);SDL_RaiseWindow(a->panels[PS_PANEL_TIMELINE].window);timeline_design.variant=7;timeline_design.settle=0;break;
    case 58:if(timeline_design.settle++<4)return;ok=timeline_design_geometry(a);timeline_design_log(a,"detached");timeline_design_capture(a,"normal",capture);timeline_design_mouse(a,.3,true);break;
    case 59:timeline_design_mouse(a,.3,false);timeline_design.variant=8;timeline_design.settle=0;break;
    case 60:if(timeline_design.settle++<2)return;ok=timeline_design_geometry(a);timeline_design_log(a,"detached");timeline_design_capture(a,"normal",capture);break;
    case 61:if(!timeline_design_focus(a,"Abspielen",true))return;test_window_key_mod(timeline_design_window(a),SDLK_RETURN,0);timeline_design.variant=9;break;
    case 62:if(!a->timeline_playing || !timeline_design_focus(a,"Anhalten",false))return;ok=timeline_design_geometry(a);timeline_design_log(a,"detached");timeline_design_capture(a,"normal",capture);test_window_key_mod(timeline_design_window(a),SDLK_RETURN,0);break;
    case 63:if(a->timeline_playing || !timeline_design_focus(a,"Abspielen",false))return;SDL_SetWindowSize(timeline_design_window(a),900,250);timeline_design.variant=10;timeline_design.settle=0;break;
    case 64:if(timeline_design.settle++<3)return;ok=timeline_design_geometry(a);timeline_design_log(a,"detached");timeline_design_capture(a,"normal",capture);break;
    case 65:{SDL_Event e={0};e.type=SDL_EVENT_WINDOW_CLOSE_REQUESTED;e.window.windowID=SDL_GetWindowID(timeline_design_window(a));SDL_PushEvent(&e);timeline_design.variant=11;timeline_design.settle=0;break;}
    case 66:if(panel_is_detached(a,PS_PANEL_TIMELINE) || timeline_design.settle++<3)return;ok=timeline_design_geometry(a);timeline_design_log(a,"returned");timeline_design_capture(a,"normal",capture);break;
    case 67:{uint32_t crc=0;ok=timeline_design_run_crc(a->last_run,&crc) && crc==timeline_design.run_crc && timeline_design_source(a);if(ok)puts("TIMELINE INVARIANTS: live-run-unchanged=1 source=1 snapshot=1 escape=1 resize=1 actual-focus-loss=1 popout=1 return=1");command(a,PS_MSG_STOP);a->stop_at=ps_clock();break;}
    case 68:if(!idle(a) || a->library_thread)return;ok=timeline_design_source(a);if(ok)puts("TIMELINE DESIGN SELF-TEST: PASSED");a->quitting=true;break;
    }
    if(!ok){timeline_design_log(a,"failure");fprintf(stderr,"TIMELINE DESIGN FAILED stage=%d variant=%u count=%u status=%s\n",*stage,timeline_design.variant,count,a->status);*code=1;a->quitting=true;}
    else (*stage)++;
    if(a->quitting && timeline_design.sink){SDL_DestroyWindow(timeline_design.sink);timeline_design.sink=NULL;}
}

/* Explicit synthetic states isolate context disposal, not physical integration. */
static uint32_t timeline_context_source;
static unsigned timeline_context_settle;
static bool timeline_context_start(app *a,const char *root) {
    if(!laboratory_test_start(a,root,"laboratory-dark-narrow"))return false;
    timeline_context_settle=0;timeline_context_source=ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
    timeline_clear(a);ps_snapshot frame={0};frame.count=1;a->channel_count=1;
    for(unsigned i=0;i<4;i++){frame.time=5+i;frame.values[0]=100+i;if(ps_timeline_push(&a->timeline,&frame)!=PS_OK)return false;}
    a->simulation_time=8;timeline_select(a,0);return true;
}
static void timeline_context_frame(app *a,int *stage,int *code,const char **capture) {
    bool ok=true;
    switch(*stage) {
    case 0:if(timeline_context_settle++<3)return;if(SDL_GetKeyboardFocus()!=a->window){SDL_RaiseWindow(a->window);return;}timeline_design_mouse(a,.5,true);break;
    case 1:ok=a->timeline_dragging && a->timeline_browsing;*capture="timeline-context-before.png";snprintf(a->project_input,sizeof a->project_input,"%s",a->project);open_project(a);break;
    case 2:printf("TIMELINE CONTEXT AFTER OPEN: dragging=%d browsing=%d count=%u\n",a->timeline_dragging,a->timeline_browsing,ps_timeline_count(&a->timeline));test_window_key_mod(a->window,SDLK_ESCAPE,0);timeline_design_mouse(a,.5,false);break;
    case 3:
        ok=!a->timeline_dragging && !a->timeline_browsing && !a->timeline_playing && !ps_timeline_count(&a->timeline) && display_time(a)==0 &&
            timeline_context_source==ps_crc32((const unsigned char*)nk_str_get_const(&a->experiment.string),(size_t)nk_str_len_char(&a->experiment.string));
        *capture="timeline-context-after.png";printf("TIMELINE CONTEXT FINAL: dragging=%d browsing=%d count=%u time=%.17g\n",a->timeline_dragging,a->timeline_browsing,ps_timeline_count(&a->timeline),display_time(a));
        if(ok)puts("TIMELINE CONTEXT SELF-TEST: PASSED");
        a->quitting=true;break;
    }
    if(!ok){fprintf(stderr,"TIMELINE CONTEXT FAILED stage=%d\n",*stage);*code=1;a->quitting=true;}else (*stage)++;
}
