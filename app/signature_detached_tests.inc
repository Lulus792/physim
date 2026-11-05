/* Observe the actual auxiliary context before its commands are submitted and
 * cleared. All input travels through the real SDL window/event router. */
static struct {
    bool prepared;
    unsigned target,wait_generation;
    double deadline;
    char root[4096],source[4096],main_capture[4096];
    size_t bytes;
    int cursor,select_start,select_end,resume_cursor;
    struct nk_vec2 scroll;
    struct nk_rect thumb,outside;
    ps_dock_layout layout;
    SDL_Process *peer;
    bool peer_ready;
    char release[4096];
    unsigned child_blurs;
} signature_detached;
/* A separate process is essential: another SDL window in this process would
 * not exercise loss of application focus. The peer acknowledges real OS focus
 * on its pipe and stays alive until the controller releases it. */
static int signature_focus_peer(const char *release){
    if(!SDL_Init(SDL_INIT_VIDEO))return 2;
    SDL_Window *window=SDL_CreateWindow("Physim focus test peer",320,120,0);
    if(!window){SDL_Quit();return 2;}
    SDL_RaiseWindow(window);Uint64 deadline=SDL_GetTicks()+15000;bool ready=false;
    int result=1;
    while(SDL_GetTicks()<deadline){
        SDL_Event event;while(SDL_PollEvent(&event)){}
        if(!ready && SDL_GetKeyboardFocus()==window){putchar('R');fflush(stdout);ready=true;}
        if(ready && exists(release)){result=0;break;}
        SDL_Delay(10);
    }
    SDL_DestroyWindow(window);SDL_Quit();return result;
}
static void signature_detached_focus_event(app *a,const SDL_Event *event){
    if(a->signature_detached_test_active && event->type==SDL_EVENT_WINDOW_FOCUS_LOST &&
       a->panels[PS_DOCK_WORKSPACE].window && event->window.windowID==SDL_GetWindowID(a->panels[PS_DOCK_WORKSPACE].window))
        signature_detached.child_blurs++;
}
static void signature_detached_draw(app *a){signature_test_draw(a);}
static bool signature_detached_hidden(app *a){
    return a->signature_hud_bounds.w==0 && !signature_painted_bytes && !signature_footer_bytes;
}
static struct nk_text_edit *signature_detached_edit(app *a){
    workspace_document *doc=active_document(a);
    return signature_detached.target==0?&a->experiment:signature_detached.target==1?&a->analysis:doc?&doc->edit:NULL;
}
static bool signature_detached_start(app *a,const char *root,const char *mode){
    memset(&signature_detached,0,sizeof signature_detached);
    int n=snprintf(signature_detached.root,sizeof signature_detached.root,"%s",root);
    if(n<0 || (size_t)n>=sizeof signature_detached.root)return false;
    a->signature_detached_test_active=true;
    return completion_test_start(a,root,strstr(mode,"phys")?"completion-phys":"completion-c");
}
static bool signature_detached_remember(struct nk_text_edit *edit){
    if(!edit)return false;int bytes=nk_str_len_char(&edit->string);
    if(bytes<0 || (size_t)bytes>=sizeof signature_detached.source)return false;
    signature_detached.bytes=(size_t)bytes;
    if(bytes)memcpy(signature_detached.source,nk_str_get_const(&edit->string),(size_t)bytes);
    signature_detached.source[bytes]=0;signature_detached.cursor=edit->cursor;
    signature_detached.select_start=edit->select_start;signature_detached.select_end=edit->select_end;
    signature_detached.scroll=edit->scrollbar;return true;
}
static bool signature_detached_preserved(struct nk_text_edit *edit,bool scroll){
    return edit && (size_t)nk_str_len_char(&edit->string)==signature_detached.bytes &&
        (!signature_detached.bytes || !memcmp(nk_str_get_const(&edit->string),signature_detached.source,signature_detached.bytes)) &&
        edit->cursor==signature_detached.cursor && edit->select_start==signature_detached.select_start && edit->select_end==signature_detached.select_end &&
        (!scroll || (edit->scrollbar.x==signature_detached.scroll.x && edit->scrollbar.y==signature_detached.scroll.y));
}
static bool signature_detached_outside(app *a){
    struct nk_rect e=a->source_editor_bounds,h=a->signature_hud_bounds,t=signature_thumb_bounds;
    /* The starting thumb MUST overlap the source field. Otherwise a leaked
     * gesture could miss the underlying editor and falsely pass preservation. */
    if(t.x<e.x || t.x+t.w>e.x+e.w || t.y<e.y || t.y+t.h>e.y+e.h || e.w<8 || e.h<8)return false;
    struct nk_vec2 points[]={nk_vec2(e.x+3,e.y+e.h-3),nk_vec2(e.x+e.w-3,e.y+e.h-3),nk_vec2(e.x+3,e.y+3),nk_vec2(e.x+e.w-3,e.y+3)};
    for(unsigned i=0;i<SDL_arraysize(points);i++)if(points[i].x<h.x || points[i].x>=h.x+h.w || points[i].y<h.y || points[i].y>=h.y+h.h){
        signature_detached.outside=nk_rect(points[i].x,points[i].y,1,1);return true;
    }
    return false;
}
static bool signature_detached_long(app *a,struct nk_text_edit *edit,bool language){
    char text[4096];snprintf(text,sizeof text,language?"func many(":"double many(");
    for(unsigned i=0;i<16;i++){char part[64];snprintf(part,sizeof part,language?"%sp%02u: Float64":"%sdouble p%02u",i?", ":"",i);strcat(text,part);}
    strcat(text,language?") -> Float64:\n    return p00\nfunc step():\n    many(":");\nvoid f(){many(");
    /* Keep the active argument at the left edge on a continuation line. The
     * real HUD thumb then lies over editable source, not its right gutter. */
    for(unsigned i=0;i<15;i++)strcat(text,"0, ");strcat(text,language?"\n|)\n":"\n|);}");
    return signature_seed(a,edit,text);
}
static void signature_detached_inner(app *a,bool language,int *stage,int *code){
    ps_panel_window *p=&a->panels[PS_DOCK_WORKSPACE];struct nk_text_edit *edit=signature_detached_edit(a);bool ok=true;
    if(a->library_thread)return;
    if(p->detached && signature_generation<signature_detached.wait_generation)return;
#define SD_NEXT(n,seconds) do{*stage=(n);signature_detached.deadline=ps_clock()+(seconds);signature_detached.wait_generation=signature_generation+2;}while(0)
#define SD_WAIT(condition) do{if(!(condition)){if(ps_clock()<signature_detached.deadline)return;ok=false;goto done;}}while(0)
#define SD_KEY(key,mod) test_window_key_mod(a->window,key,mod)
    switch(*stage){
    case 0:
        if(!signature_detached.prepared){ok=open_project(a);signature_detached.prepared=ok;}
        {ps_preferences pref=a->preferences;pref.theme=signature_detached.target;pref.ui_size=signature_detached.target?22:16;pref.editor_size=signature_detached.target?22:12;pref.completion_auto=true;preferences_apply(a,&pref);}
        if(signature_detached.target==2){char path[4096];join(path,sizeof path,a->project,language?"completion-note.phys":"completion-note.c");preview_workspace_path(a,path);}
        else{a->tab=signature_detached.target?2:0;if(signature_detached.target)a->analysis_tab=1;}
        SD_NEXT(1,5);break;
    case 1:
        SD_WAIT(edit && !a->document_pending);
        ok=signature_seed(a,edit,language?"func step():\n    Vec3":"void f(){ps_v3");SD_NEXT(2,5);break;
    case 2:
        SD_WAIT(edit->active && a->completion_focus_edit!=edit && a->dock_panels[PS_DOCK_WORKSPACE].w>0);
        signature_detached.layout=a->preferences.dock;ok=panel_window_open(a,PS_DOCK_WORKSPACE);SD_NEXT(3,30);break;
    case 3:
        SD_WAIT(p->detached && p->window!=a->primary_window && SDL_GetKeyboardFocus()==p->window &&
            edit->active && a->completion_focus_edit!=edit && !p->restore_editor_focus && a->signature_test_paint_window==SDL_GetWindowID(p->window));
        /* Finish the extraction's show/home-size request before issuing a
         * second resize. X11 may still be completing the reused window's map. */
        SD_WAIT(SDL_SyncWindow(p->window));
        ok=SDL_SetWindowSize(p->window,520,400) && SDL_SyncWindow(p->window);SD_NEXT(4,5);break;
    case 4:
        {int w=0,h=0;SD_WAIT(SDL_GetWindowSize(p->window,&w,&h) && w==520 && h==400);}
        SD_WAIT(SDL_TextInputActive(p->window) && edit->active);
        completion_auto_text(a,"(");SD_NEXT(5,5);break;
    case 5:
        SD_WAIT(a->signature_test_paint_window==SDL_GetWindowID(p->window) && signature_test_full(a,0) && signature_footer_full(a));
        {struct nk_rect r=a->signature_hud_bounds;ok=r.x>=0 && r.y>=0 && r.x+r.w<=521 && r.y+r.h<=401;}
        if(signature_detached.target==1)join(p->capture,sizeof p->capture,signature_detached.root,"signature-detached-normal.bmp");
        ok=ok && signature_detached_remember(edit);SD_KEY(SDLK_SPACE,SDL_KMOD_CTRL);SD_NEXT(6,5);break;
    case 6:
        SD_WAIT(a->completion_active && signature_test_full(a,0));
        {struct nk_rect h=a->signature_hud_bounds,l=a->completion_list_bounds;ok=l.y>=h.y+h.h+5 || l.y+l.h<=h.y-5;}
        ok=ok && signature_detached_remember(edit);signature_detached.child_blurs=0;
        ok=ok && SDL_RaiseWindow(a->primary_window);SD_NEXT(30,15);break;
    case 7:
        SD_WAIT(!a->completion_active && signature_test_full(a,0) && a->completion_focus_edit!=edit);
        completion_auto_text(a,"1");SD_NEXT(8,5);break;
    case 8:
        SD_WAIT(edit->cursor==signature_detached.cursor+1 && signature_current(a,edit));
        ok=signature_detached_remember(edit);
        if(signature_detached.target<2){SD_KEY(SDLK_RETURN,PS_UI_COMMAND_MOD|SDL_KMOD_SHIFT);SD_NEXT(20,30);}
        else SD_NEXT(9,5);
        break;
    case 9:
        ok=SDL_SetWindowSize(p->window,520,320) && SDL_SyncWindow(p->window) && signature_detached_long(a,edit,language);SD_NEXT(10,5);break;
    case 10:
        SD_WAIT(a->signature_test_paint_window==SDL_GetWindowID(p->window) && a->signature_visible && a->signature_help.display &&
            a->signature_help.display->count==16 && a->signature_help.active_parameter==15 && a->signature_scroll_y>0 &&
            strstr(signature_code_text,"p15") && signature_footer_full(a));
        {int w=0,h=0;SD_WAIT(SDL_GetWindowSize(p->window,&w,&h) && w==520 && h==320);}
        join(p->capture,sizeof p->capture,signature_detached.root,"signature-detached-long.bmp");
        ok=ok && signature_detached_remember(edit);SD_KEY(SDLK_HOME,SDL_KMOD_ALT);SD_NEXT(11,5);break;
    case 11:
        SD_WAIT(!a->signature_scroll_y && signature_thumb_bounds.w>0 && signature_footer_full(a));
        ok=signature_detached_preserved(edit,true) && signature_detached_outside(a);signature_detached.thumb=signature_thumb_bounds;
        if(!ok)fprintf(stderr,"Signature overlap: preserved=%d editor=%.3f/%.3f/%.3f/%.3f thumb=%.3f/%.3f/%.3f/%.3f selection=%d/%d expected=%d/%d scroll=%.3f/%.3f expected=%.3f/%.3f\n",signature_detached_preserved(edit,true),a->source_editor_bounds.x,a->source_editor_bounds.y,a->source_editor_bounds.w,a->source_editor_bounds.h,signature_thumb_bounds.x,signature_thumb_bounds.y,signature_thumb_bounds.w,signature_thumb_bounds.h,edit->select_start,edit->select_end,signature_detached.select_start,signature_detached.select_end,edit->scrollbar.x,edit->scrollbar.y,signature_detached.scroll.x,signature_detached.scroll.y);
        test_window_mouse(p->window,signature_detached.thumb,true);SD_NEXT(12,5);break;
    case 12:
        ok=signature_detached_preserved(edit,true);signature_move(a,signature_detached.outside);SD_NEXT(13,5);break;
    case 13:
        ok=signature_detached_preserved(edit,true) && signature_current(a,edit);
        test_window_mouse(p->window,signature_detached.outside,false);SD_NEXT(14,5);break;
    case 14:
        SD_WAIT(!a->ui->input.mouse.buttons[NK_BUTTON_LEFT].down);
        ok=signature_detached_preserved(edit,true);completion_auto_text(a,"1");SD_NEXT(15,5);break;
    case 15:
        SD_WAIT(edit->cursor==signature_detached.cursor+1 && signature_current(a,edit));
        SD_KEY(SDLK_BACKSPACE,0);SD_NEXT(16,5);break;
    case 16:
        SD_WAIT(signature_detached_preserved(edit,false) && signature_current(a,edit));
        printf("SIGNATURE DETACHED language=%s target=2 actual-child-context=1 long16=1 thumb-over-editor=1 outside-drag-preserved=1 typing-after-drag=1\n",language?"phys":"c");
        SD_KEY(SDLK_RETURN,PS_UI_COMMAND_MOD|SDL_KMOD_SHIFT);SD_NEXT(20,30);break;
    case 20:
        SD_WAIT(!p->detached && SDL_GetKeyboardFocus()==a->primary_window && a->signature_test_paint_window==SDL_GetWindowID(a->primary_window) &&
            edit->active && a->completion_focus_edit!=edit && SDL_TextInputActive(a->primary_window) &&
            signature_current(a,edit) && a->signature_hud_bounds.w>0 && signature_footer_full(a));
        ok=signature_detached_preserved(edit,false) && !memcmp(&a->preferences.dock,&signature_detached.layout,sizeof signature_detached.layout) &&
            signature_current(a,edit) && a->signature_hud_bounds.w>0 && signature_footer_full(a);
        if(!ok)fprintf(stderr,"Signature return invariants: preserved=%d layout=%d current=%d hud=%d footer=%d selection=%d/%d\n",signature_detached_preserved(edit,false),!memcmp(&a->preferences.dock,&signature_detached.layout,sizeof signature_detached.layout),signature_current(a,edit),a->signature_hud_bounds.w>0,signature_footer_full(a),edit->select_start,edit->select_end);
        completion_auto_text(a,"2");SD_NEXT(21,5);break;
    case 21:
        SD_WAIT(edit->cursor==signature_detached.cursor+1 && signature_current(a,edit));
        printf("SIGNATURE DETACHED language=%s target=%u actual-child-context=1 auto-open=1 list-disjoint=1 child-typing=1 native-return=1 source-caret-layout-preserved=1 primary-typing=1\n",language?"phys":"c",signature_detached.target);
        if(signature_detached.target<2){signature_detached.target++;SD_NEXT(0,5);}else a->quitting=true;
        break;
    case 30:
        SD_WAIT(SDL_GetKeyboardFocus()==a->primary_window && signature_detached.child_blurs>0);
        ok=signature_detached_preserved(edit,true) && !a->completion_active && signature_detached_hidden(a);
        if(ok)ok=SDL_RaiseWindow(p->window);SD_NEXT(31,15);break;
    case 31:
        SD_WAIT(SDL_GetKeyboardFocus()==p->window && SDL_TextInputActive(p->window) && signature_test_full(a,0));
        ok=signature_detached_preserved(edit,true);
        {char executable[4096];join(signature_detached.release,sizeof signature_detached.release,signature_detached.root,"signature-focus-release");
         if(exists(signature_detached.release))ok=ok && remove(signature_detached.release)==0;
         ok=ok && ps_executable_path(executable,sizeof executable);
         if(ok){const char *args[]={executable,"--signature-focus-peer",signature_detached.release,NULL};
             signature_detached.child_blurs=0;signature_detached.peer_ready=false;
             signature_detached.peer=SDL_CreateProcess(args,true);ok=signature_detached.peer!=NULL;}}
        SD_NEXT(32,15);break;
    case 32:
        if(!signature_detached.peer_ready){char ready=0;size_t count=SDL_ReadIO(SDL_GetProcessOutput(signature_detached.peer),&ready,1);
            signature_detached.peer_ready=count==1 && ready=='R';}
        SD_WAIT(signature_detached.peer_ready);
        SD_WAIT(!SDL_GetKeyboardFocus() && signature_detached.child_blurs>0);
        /* The OS acknowledgement can arrive after the last child paint.
         * Observe fresh child commands after the actual blur, not the old HUD
         * geometry from just before SDL delivered that event. */
        SD_NEXT(38,5);break;
    case 38:
        SD_WAIT(!SDL_GetKeyboardFocus() && signature_detached.child_blurs>0);
        ok=signature_detached_preserved(edit,true) && !a->completion_active && signature_detached_hidden(a);
        {FILE *release=fopen(signature_detached.release,"wb");if(!release)ok=false;else if(fclose(release))ok=false;}
        SD_NEXT(33,5);break;
    case 33:
        {int exit_code=-1;SD_WAIT(SDL_WaitProcess(signature_detached.peer,false,&exit_code));ok=exit_code==0;
         SDL_DestroyProcess(signature_detached.peer);signature_detached.peer=NULL;}
        ok=ok && remove(signature_detached.release)==0 && SDL_RaiseWindow(p->window);SD_NEXT(34,15);break;
    case 34:
        SD_WAIT(SDL_GetKeyboardFocus()==p->window && SDL_TextInputActive(p->window) && signature_test_full(a,0));
        ok=signature_detached_preserved(edit,true);completion_auto_text(a,"3");SD_NEXT(35,5);break;
    case 35:
        SD_WAIT(edit->cursor==signature_detached.cursor+1 && signature_current(a,edit));
        SD_KEY(SDLK_BACKSPACE,0);SD_NEXT(36,5);break;
    case 36:
        SD_WAIT(signature_detached_preserved(edit,true) && signature_current(a,edit));
        printf("SIGNATURE FOCUS language=%s target=%u main-child=1 separate-process=1 actual-blur=1 hud-hidden=1 source-caret-selection-scroll=1 resumed-typing=1\n",language?"phys":"c",signature_detached.target);
        signature_detached.resume_cursor=edit->cursor;SD_KEY(SDLK_LEFT,SDL_KMOD_SHIFT);SD_NEXT(40,5);
        break;
    case 40:
        SD_WAIT(edit->select_start!=edit->select_end && edit->cursor==signature_detached.resume_cursor-1);
        ok=signature_detached_remember(edit);signature_detached.child_blurs=0;
        ok=ok && SDL_RaiseWindow(a->primary_window);SD_NEXT(41,15);break;
    case 41:
        SD_WAIT(SDL_GetKeyboardFocus()==a->primary_window && signature_detached.child_blurs>0);
        ok=signature_detached_preserved(edit,true);if(ok)ok=SDL_RaiseWindow(p->window);SD_NEXT(42,15);break;
    case 42:
        SD_WAIT(SDL_GetKeyboardFocus()==p->window && SDL_TextInputActive(p->window) && a->completion_focus_edit!=edit);
        ok=signature_detached_preserved(edit,true) && signature_detached_hidden(a);
        if(ok)printf("SIGNATURE FOCUS SELECTION language=%s target=%u nonempty-range=1 actual-return=1 source-caret-anchors-scroll=1\n",language?"phys":"c",signature_detached.target);
        SD_KEY(SDLK_RIGHT,0);SD_NEXT(43,5);break;
    case 43:
        SD_WAIT(edit->cursor==signature_detached.resume_cursor && edit->select_start==edit->select_end && signature_test_full(a,0));
        ok=(size_t)nk_str_len_char(&edit->string)==signature_detached.bytes &&
            !memcmp(nk_str_get_const(&edit->string),signature_detached.source,signature_detached.bytes) && signature_detached_remember(edit);
        SD_NEXT(7,5);break;
    }
done:
    if(!ok){
        int w=0,h=0;SDL_GetWindowSize(a->window,&w,&h);
        fprintf(stderr,"Signature detached input: size=%dx%d SDL-text=%d UI-owner=%s owner-edit=%d completion-owner=%d\n",w,h,SDL_TextInputActive(a->window),a->ui->active?a->ui->active->name_string:"(none)",a->ui->active?a->ui->active->edit.active:0,a->completion_keyboard_edit==edit);
        if(p->detached)join(p->capture,sizeof p->capture,signature_detached.root,"signature-detached-failure.bmp");
        fprintf(stderr,"Signature detached failure: language=%d target=%u stage=%d detached=%d child=%u paint=%u focus=%u current=%u active=%d cursor=%d expected=%d bytes=%d expected-bytes=%zu pending=%p visible=%d bounds=%.3f/%.3f/%.3f/%.3f scroll=%u max=%.3f footer=%zu text=[%s]\n",language,signature_detached.target,*stage,p->detached,p->window?SDL_GetWindowID(p->window):0,a->signature_test_paint_window,SDL_GetKeyboardFocus()?SDL_GetWindowID(SDL_GetKeyboardFocus()):0,SDL_GetWindowID(a->window),edit?edit->active:0,edit?edit->cursor:-1,signature_detached.cursor,edit?nk_str_len_char(&edit->string):-1,signature_detached.bytes,(void*)a->completion_focus_edit,a->signature_visible,a->signature_hud_bounds.x,a->signature_hud_bounds.y,a->signature_hud_bounds.w,a->signature_hud_bounds.h,a->signature_scroll_y,a->signature_scroll_max,signature_footer_bytes,signature_code_text);
        *code=1;a->quitting=true;
    }
    if(a->quitting){
        if(signature_detached.peer){SDL_KillProcess(signature_detached.peer,true);SDL_WaitProcess(signature_detached.peer,true,NULL);SDL_DestroyProcess(signature_detached.peer);signature_detached.peer=NULL;}
        printf("SIGNATURE DETACHED SELF-TEST: %s\n",*code?"FAILED":"PASSED");
    }
#undef SD_NEXT
#undef SD_WAIT
#undef SD_KEY
}
static void signature_detached_frame(app *a,bool language,int *stage,int *code,const char **capture){
    ps_panel_window *p=&a->panels[PS_DOCK_WORKSPACE];
    if(!p->detached){
        signature_detached_inner(a,language,stage,code);
        if(a->quitting && *code){join(signature_detached.main_capture,sizeof signature_detached.main_capture,signature_detached.root,"signature-detached-failure.bmp");*capture=signature_detached.main_capture;}
        return;
    }
    struct nk_context *ui=a->ui;SDL_Window *window=a->window;int tab=a->tab;
    a->ui=p->ui;a->window=p->window;a->tab=p->tab;
    signature_detached_inner(a,language,stage,code);
    a->ui=ui;a->window=window;a->tab=tab;
}
