/* Actual native editor/event/paint path. Model-only vectors live separately. */
static unsigned signature_generation,signature_wait_generation;
static size_t signature_painted_bytes,signature_bold_bytes;
static bool signature_function_ink;
static char signature_code_text[8192];
static bool signature_arrow_broken,signature_arrow_seen,signature_footer_error;
static size_t signature_footer_bytes;
static size_t signature_text_bytes(const char *text,size_t bytes){
    size_t count=0;for(size_t i=0;i<bytes;i++)if(text[i]!=' ' && text[i]!='\t' && text[i]!='\r' && text[i]!='\n')count++;return count;
}
static void *signature_deny_allocate(void *user,size_t bytes){(void)user;(void)bytes;return NULL;}
static void signature_deny_free(void *user,void *pointer,size_t bytes){(void)user;(void)pointer;(void)bytes;abort();}
static void signature_test_draw(app *a){
    signature_generation++;signature_painted_bytes=signature_bold_bytes=0;signature_function_ink=false;
    signature_code_text[0]=0;signature_arrow_broken=signature_arrow_seen=signature_footer_error=false;signature_footer_bytes=0;
    struct nk_window *w=nk_window_find(a->ui,"call-parameter-help");
    if(!w || w->seq!=a->ui->seq || (w->flags&NK_WINDOW_HIDDEN))return;
    const struct nk_command *cmd;const ps_ui_palette *p=a->preferences.theme==PS_THEME_LIGHT?&PS_UI_LIGHT:
        a->preferences.theme==PS_THEME_HIGH_CONTRAST?&PS_UI_HIGH_CONTRAST:&PS_UI_DARK;
    struct nk_rect clip=nk_rect(-1000000,-1000000,2000000,2000000);
    bool dash=false;float dash_y=0;size_t code_bytes=0;
    struct nk_color error=ps_workbench_palette(p)->error;
    nk_foreach(cmd,a->ui){
        if(cmd->type==NK_COMMAND_SCISSOR){const struct nk_command_scissor *s=(const struct nk_command_scissor*)cmd;clip=nk_rect(s->x,s->y,s->w,s->h);continue;}
        nk_size at=(nk_size)((const char*)cmd-(const char*)a->ui->memory.memory.ptr);
        if(cmd->type!=NK_COMMAND_TEXT || at<w->buffer.begin || at>=w->buffer.end)continue;
        const struct nk_command_text *t=(const struct nk_command_text*)cmd;
        float glyph_width=t->font->width(t->font->userdata,t->font->height,t->string,t->length);
        if(t->x<clip.x || t->y<clip.y || t->x+glyph_width>clip.x+clip.w+1 || t->y+t->font->height>clip.y+clip.h+1)continue;
        if(completion_ink_region(t,a->signature_status_bounds)){
            signature_footer_bytes+=signature_text_bytes(t->string,(size_t)t->length);
            if(completion_ink_color(t->foreground,error))signature_footer_error=true;
        }
        bool code_font=t->font->height==a->font_code->height && (t->font->userdata.ptr==a->font_code->userdata.ptr || t->font->userdata.ptr==a->workbench_marker_source->userdata.ptr);
        if(!code_font || !completion_ink_region(t,a->signature_field_bounds))continue;
        if(code_bytes+(size_t)t->length<sizeof signature_code_text){memcpy(signature_code_text+code_bytes,t->string,(size_t)t->length);code_bytes+=(size_t)t->length;signature_code_text[code_bytes]=0;}
        if(t->length==1 && t->string[0]=='>' && dash){signature_arrow_seen=true;if(t->y!=dash_y)signature_arrow_broken=true;}
        dash=t->length==1 && t->string[0]=='-';dash_y=t->y;
        signature_painted_bytes+=signature_text_bytes(t->string,(size_t)t->length);
        if(t->font->userdata.ptr==a->workbench_marker_source->userdata.ptr)signature_bold_bytes+=signature_text_bytes(t->string,(size_t)t->length);
        if(completion_ink_color(t->foreground,p->code_function))signature_function_ink=true;
    }
}
static bool signature_seed(app *a,struct nk_text_edit *edit,const char *marked){
    const char *mark=strchr(marked,'|');size_t bytes=strlen(marked),cursor=mark?(size_t)(mark-marked):bytes;
    char *text=malloc(bytes+1);if(!text || !edit){free(text);return false;}
    memcpy(text,marked,cursor);strcpy(text+cursor,marked+cursor+(mark?1:0));
    completion_close(a);signature_clear(a);
    bool ok=set_editor_text(edit,text,strlen(text));
    edit->cursor=edit->select_start=edit->select_end=nk_utf_len(text,(int)cursor);free(text);
    completion_return_focus(a,edit);signature_wait_generation=signature_generation+2;return ok;
}
static bool signature_test_full(app *a,size_t active){
    ps_signature_display *d=a->signature_help.display;
    if(!d || !a->signature_visible || a->signature_help.active_parameter!=active || !a->signature_hud_bounds.w)return false;
    float measured=signature_code(a,a->signature_field_bounds.w,0,0,false);
    float positioned=signature_code(a,a->signature_field_bounds.w,a->signature_field_bounds.x,a->signature_field_bounds.y,false);
    if(measured!=positioned)return false;
    size_t bytes=signature_text_bytes(d->text,d->text_bytes);
    size_t bold=active==SIZE_MAX?0:signature_text_bytes(d->text+d->parameters[active].begin,d->parameters[active].end-d->parameters[active].begin);
    return signature_painted_bytes==bytes && signature_bold_bytes==bold && signature_function_ink && !signature_arrow_broken && (!strstr(d->text,"->") || signature_arrow_seen);
}
static bool signature_footer_full(app *a){return a->signature_status && signature_footer_bytes==signature_text_bytes(a->signature_status,strlen(a->signature_status));}
static void signature_wheel(app *a,float delta){
    struct nk_rect b=a->signature_body_bounds;float x=b.x+b.w/2,y=b.y+b.h/2;
    SDL_Event e={0};e.type=SDL_EVENT_MOUSE_MOTION;e.motion.windowID=SDL_GetWindowID(a->window);e.motion.which=PS_TEST_MOUSE_ID;e.motion.x=x;e.motion.y=y;SDL_PushEvent(&e);
    e=(SDL_Event){0};e.type=SDL_EVENT_MOUSE_WHEEL;e.wheel.windowID=SDL_GetWindowID(a->window);e.wheel.which=PS_TEST_MOUSE_ID;e.wheel.y=delta;e.wheel.mouse_x=x;e.wheel.mouse_y=y;SDL_PushEvent(&e);
}
static void signature_test_frame(app *a,bool language,int *stage,int *code,const char **capture){
    static unsigned target;static bool prepared;static double timeout;bool ok=true;
    static int long_cursor;static struct nk_vec2 long_scroll;static const void *long_rows;
    workspace_document *doc=active_document(a);
    struct nk_text_edit *edit=target==0?&a->experiment:target==1?&a->analysis:doc?&doc->edit:NULL;
    if(a->library_thread || signature_generation<signature_wait_generation)return;
#define SIG_NEXT(n) do{*stage=(n);timeout=ps_clock()+5;}while(0)
#define SIG_WAIT(x) do{if(!(x)){if(ps_clock()<timeout)return;ok=false;goto finished;}}while(0)
#define SIG_KEY(k,m) do{test_window_key_mod(a->window,k,m);signature_wait_generation=signature_generation+2;}while(0)
#define SIG_TEXT(t) do{completion_auto_text(a,t);signature_wait_generation=signature_generation+2;}while(0)
    switch(*stage){
    case 0:
        if(!prepared){ok=open_project(a);prepared=ok;}
        {ps_preferences p=a->preferences;p.theme=target;p.completion_auto=true;p.ui_size=target==1?22:16;p.editor_size=target==1?22:12;preferences_apply(a,&p);}
        if(target==2){char path[4096];join(path,sizeof path,a->project,language?"completion-note.phys":"completion-note.c");preview_workspace_path(a,path);}
        else{a->tab=target?2:0;if(target)a->analysis_tab=1;}
        SIG_NEXT(1);break;
    case 1:ok=signature_seed(a,edit,language?"func step():\n    Vec3":"void f(){ps_v3");SIG_NEXT(2);break;
    case 2:SIG_WAIT(edit && edit->active);SIG_TEXT("(");SIG_NEXT(3);break;
    case 3:
        SIG_WAIT(signature_test_full(a,0));ok=a->signature_help.language==language && a->signature_help.display->count==3;
        if((!language && target==0) || (language && target==1))*capture="signature-first.bmp";
        SIG_KEY(SDLK_SPACE,SDL_KMOD_CTRL);SIG_NEXT(4);break;
    case 4:
        SIG_WAIT(a->completion_active && signature_test_full(a,0));
        {struct nk_rect s=a->signature_hud_bounds,l=a->completion_list_bounds;
        ok=l.y>=s.y+s.h+5 || l.y+l.h<=s.y-5;}
        SIG_KEY(SDLK_ESCAPE,0);SIG_NEXT(5);break;
    case 5:SIG_WAIT(!a->completion_active && signature_test_full(a,0));SIG_KEY(SDLK_ESCAPE,0);SIG_NEXT(6);break;
    case 6:
        SIG_WAIT(!a->signature_visible && !a->signature_hud_bounds.w && !signature_painted_bytes);
        SIG_TEXT("1, ");SIG_NEXT(7);break;
    case 7:
        SIG_WAIT(a->signature_help.call.argument==1);ok=!a->signature_visible && a->signature_suppressed_owner==edit;
        SIG_KEY(SDLK_SPACE,SDL_KMOD_CTRL|SDL_KMOD_SHIFT);SIG_NEXT(8);break;
    case 8:
        SIG_WAIT(signature_test_full(a,1));ok=strstr(a->signature_status,"Parameter 2 von 3")!=NULL;
        if(a->completion_active)completion_dismiss(a,edit);
        SIG_TEXT("2, 3, ");SIG_NEXT(9);break;
    case 9:
        SIG_WAIT(signature_test_full(a,SIZE_MAX));ok=strstr(a->signature_status,"Argument 4 · erwartet 3 Parameter")!=NULL;
        ok=ok && signature_footer_full(a) && signature_footer_error;
        ok=ok && signature_seed(a,edit,language?"func step():\n    Vec3(1, sin(|), 3)\n":"void f(){ps_v3(1,ps_vlength(|),3);}");SIG_NEXT(10);break;
    case 10:
        SIG_WAIT(signature_test_full(a,0));ok=a->signature_help.display->count==1;
        /* Real caret navigation exits the inner call and resumes the outer. */
        SIG_KEY(SDLK_RIGHT,0);SIG_NEXT(11);break;
    case 11:
        SIG_WAIT(signature_test_full(a,1));ok=a->signature_help.display->count==3;
        ok=ok && signature_seed(a,edit,language?"func step():\n    let amount: Float64 = 1\n    Vec3(am|)\n":"void f(){double amount=1;ps_v3(am|);}");SIG_NEXT(12);break;
    case 12:SIG_WAIT(signature_test_full(a,0));SIG_KEY(SDLK_SPACE,SDL_KMOD_CTRL);SIG_NEXT(13);break;
    case 13:
        SIG_WAIT(a->completion_active);ok=false;
        for(size_t i=0;i<a->completion_count;i++)if(!strcmp(completion_symbol(a,a->completion_indices[i])->name,"amount")){
            a->completion_selected=i;a->completion_explicit=true;ok=true;break;
        }
        SIG_KEY(SDLK_RETURN,0);SIG_NEXT(14);break;
    case 14:
        SIG_WAIT(!a->completion_active && signature_test_full(a,0));
        ok=strstr(a->signature_help.source,language?"Vec3(amount)":"ps_v3(amount)")!=NULL;
        /* IME and a real selection pause publication; no stale source is shown. */
        {SDL_Event e={0};e.type=SDL_EVENT_TEXT_EDITING;e.edit.windowID=SDL_GetWindowID(a->window);e.edit.text="x";SDL_PushEvent(&e);}
        signature_wait_generation=signature_generation+2;SIG_NEXT(15);break;
    case 15:
        SIG_WAIT(a->completion_composing && !a->signature_visible && !signature_painted_bytes);
        {SDL_Event e={0};e.type=SDL_EVENT_TEXT_EDITING;e.edit.windowID=SDL_GetWindowID(a->window);e.edit.text="";SDL_PushEvent(&e);}
        signature_wait_generation=signature_generation+2;SIG_NEXT(16);break;
    case 16:
        SIG_WAIT(signature_test_full(a,0));edit->select_start=edit->cursor-1;signature_wait_generation=signature_generation+2;SIG_NEXT(17);break;
    case 17:
        SIG_WAIT(!a->signature_visible && !signature_painted_bytes);edit->select_start=edit->select_end=edit->cursor;
        signature_wait_generation=signature_generation+2;SIG_NEXT(18);break;
    case 18:
        SIG_WAIT(signature_test_full(a,0));
        {
            /* A locals allocation failure deliberately retains the OLD valid
             * locals/model pair. Publication must compare actual editor bytes. */
            const char *source=a->signature_help.source;size_t n=a->signature_help.bytes;char *changed=malloc(n+1);
            if(!changed){ok=false;break;}memcpy(changed,source,n+1);
            size_t value=a->signature_help.call.open+1;if(value>=n){free(changed);ok=false;break;}
            changed[value]=changed[value]=='b'?'c':'b';int cursor=edit->cursor;bool active=edit->active;
            ok=set_editor_text(edit,changed,n);free(changed);edit->cursor=edit->select_start=edit->select_end=cursor;edit->active=active;
            ps_allocator allocator=a->completion_locals.allocator;const char *old=a->completion_locals.source;
            a->completion_locals.allocator=(ps_allocator){NULL,signature_deny_allocate,signature_deny_free};
            signature_update(a,edit,false);
            bool hidden=!a->signature_visible,held=old==a->completion_locals.source;
            bool matched=ps_signature_help_matches(&a->signature_help,&a->completion_locals),current=signature_current(a,edit);
            if(!hidden || !held || !matched || current)fprintf(stderr,"Signature OOM publication: hidden=%d held=%d old-pair=%d current=%d active=%d\n",hidden,held,matched,current,active);
            ok=ok && hidden && held && matched && !current;
            a->completion_locals.allocator=allocator;
        }
        signature_wait_generation=signature_generation+2;SIG_NEXT(19);break;
    case 19:
        SIG_WAIT(signature_test_full(a,0));
        printf("SIGNATURE HUD language=%s target=%u theme=%u auto-open=1 full-syntax=1 active-semibold=1 list-separated=1 escape-order=1 suppressed-across-comma=1 manual-reopen=1 excess-honest=1 nested-return=1 acceptance-independent=1 ime-selection=1\n",language?"phys":"c",target,target);
        printf("SIGNATURE HUD language=%s target=%u retained-old-locals-model-hidden-on-oom=1 recovered=1\n",language?"phys":"c",target);
        completion_close(a);signature_clear(a);
        if(target<2){target++;SIG_NEXT(0);}else SIG_NEXT(20);
        break;
    case 20:
        {ps_preferences p=a->preferences;p.theme=language?PS_THEME_LIGHT:PS_THEME_DARK;p.ui_size=22;p.editor_size=22;preferences_apply(a,&p);}
        /* Deliberate renderer stress below the shipping1080x740 minimum. SDL
         * otherwise silently clamps this request, which may need no scrolling. */
        ok=SDL_SetWindowMinimumSize(a->window,760,500) && SDL_SetWindowSize(a->window,800,540) && SDL_SyncWindow(a->window);
        {int w=0,h=0;ok=ok && SDL_GetWindowSize(a->window,&w,&h) && w==800 && h==540;
        printf("SIGNATURE WINDOW language=%s requested=800x540 actual=%dx%d test-only-minimum=1\n",language?"phys":"c",w,h);}
        signature_wait_generation=signature_generation+2;SIG_NEXT(21);break;
    case 21:
        {char text[4096];snprintf(text,sizeof text,language?"func many(":"double many(");
        for(unsigned i=0;i<16;i++){char part[64];snprintf(part,sizeof part,language?"%sp%02u: Float64":"%sdouble p%02u",i?", ":"",i);strcat(text,part);}
        strcat(text,language?") -> Float64:\n    return p00\nfunc step():\n    many(":");\nvoid f(){many(");
        for(unsigned i=0;i<15;i++){strcat(text,"0, ");}strcat(text,language?"|)\n":"|);}");
        ok=signature_seed(a,edit,text);}SIG_NEXT(22);break;
    case 22:
        SIG_WAIT(a->signature_visible && a->signature_help.display && a->signature_help.display->count==16 && a->signature_help.active_parameter==15 &&
            a->signature_scroll_y>0 && strstr(signature_code_text,"p15") && signature_footer_full(a));
        long_cursor=edit->cursor;long_scroll=edit->scrollbar;long_rows=a->signature_layout;
        ok=strstr(a->signature_status,"Parameter 16 von 16")!=NULL && !signature_footer_error;
        *capture="signature-long.bmp";SIG_KEY(SDLK_HOME,SDL_KMOD_ALT);SIG_NEXT(23);break;
    case 23:
        /* On a narrow code editor the C return type occupies its own row.
         * Home reveals the actual signature start, not necessarily its name. */
        SIG_WAIT(!a->signature_scroll_y && !strncmp(signature_code_text,language?"many(":"double ",language?5:7) && signature_footer_full(a));
        ok=edit->cursor==long_cursor && edit->scrollbar.x==long_scroll.x && edit->scrollbar.y==long_scroll.y && a->signature_layout==long_rows;
        signature_wheel(a,-1);signature_wait_generation=signature_generation+2;SIG_NEXT(24);break;
    case 24:
        SIG_WAIT(a->signature_scroll_y>0 && signature_footer_full(a));
        ok=edit->cursor==long_cursor && edit->scrollbar.x==long_scroll.x && edit->scrollbar.y==long_scroll.y && a->signature_layout==long_rows;
        SIG_KEY(SDLK_END,SDL_KMOD_ALT);SIG_NEXT(25);break;
    case 25:
        SIG_WAIT(strstr(signature_code_text,language?"Float64":"p15") && signature_footer_full(a));
        SIG_KEY(SDLK_HOME,SDL_KMOD_ALT);SIG_NEXT(26);break;
    case 26:SIG_WAIT(!a->signature_scroll_y);SIG_KEY(SDLK_SPACE,SDL_KMOD_CTRL|SDL_KMOD_SHIFT);SIG_NEXT(27);break;
    case 27:
        SIG_WAIT(a->signature_scroll_y>0 && strstr(signature_code_text,"p15") && signature_footer_full(a));
        ok=a->signature_layout==long_rows && edit->cursor==long_cursor;
        SIG_KEY(SDLK_SPACE,SDL_KMOD_CTRL);SIG_NEXT(28);break;
    case 28:
        SIG_WAIT(a->completion_active && a->signature_hud_bounds.w>0 && signature_footer_full(a));
        {struct nk_rect s=a->signature_hud_bounds,l=a->completion_list_bounds;ok=l.y>=s.y+s.h+5 || l.y+l.h<=s.y-5;}
        SIG_KEY(SDLK_ESCAPE,0);SIG_NEXT(29);break;
    case 29:
        SIG_WAIT(!a->completion_active && a->signature_visible);
        ok=signature_seed(a,edit,language?"func empty() -> Void:\n    return\nfunc step():\n    empty(|)\n":"void empty(void);void f(){empty(|);}");SIG_NEXT(30);break;
    case 30:
        SIG_WAIT(a->signature_visible && a->signature_help.display && !a->signature_help.display->count && signature_footer_full(a));
        ok=!strcmp(a->signature_status,"Keine Parameter erwartet") && !signature_footer_error;
        SIG_TEXT("1");SIG_NEXT(31);break;
    case 31:
        SIG_WAIT(signature_footer_full(a) && signature_footer_error);
        ok=!strcmp(a->signature_status,"Argument 1 · erwartet 0 Parameter") && a->signature_help.active_parameter==SIZE_MAX;
        printf("SIGNATURE LONG language=%s all16=1 active-last=1 footer-fixed=1 wheel-editor-preserved=1 keyboard-home-end=1 manual-reveal=1 list-disjoint=1 zero-excess=1 row-cache-reused=1\n",language?"phys":"c");
        a->quitting=true;break;
    }
finished:
    if(!ok){
        *capture="signature-failure.bmp";
        fprintf(stderr,"Signature geometry: scroll=%u max=%.3f rows=%zu body=%.3f/%.3f/%.3f/%.3f code=%.3f/%.3f/%.3f/%.3f footer=%.3f/%.3f/%.3f/%.3f footer-bytes=%zu reveal=%d current=%d focus=%d text=[%s]\n",
            a->signature_scroll_y,a->signature_scroll_max,a->signature_layout?a->signature_layout->count:0,
            a->signature_body_bounds.x,a->signature_body_bounds.y,a->signature_body_bounds.w,a->signature_body_bounds.h,
            a->signature_field_bounds.x,a->signature_field_bounds.y,a->signature_field_bounds.w,a->signature_field_bounds.h,
            a->signature_status_bounds.x,a->signature_status_bounds.y,a->signature_status_bounds.w,a->signature_status_bounds.h,
            signature_footer_bytes,a->signature_reveal,signature_current(a,edit),SDL_GetKeyboardFocus()==a->window,signature_code_text);
    }
    if(!ok){fprintf(stderr,"Signature HUD failure: language=%d target=%u stage=%d visible=%d bounds=%.0fx%.0f state=%d arg=%zu active=%zu painted=%zu bold=%zu function=%d status=%s\n",language,target,*stage,a->signature_visible,a->signature_hud_bounds.w,a->signature_hud_bounds.h,a->signature_help.state,a->signature_help.call.argument,a->signature_help.active_parameter,signature_painted_bytes,signature_bold_bytes,signature_function_ink,a->signature_status?a->signature_status:"<none>");*code=1;a->quitting=true;}
    if(a->quitting)printf("SIGNATURE HUD SELF-TEST: %s\n",*code?"FAILED":"PASSED");
#undef SIG_NEXT
#undef SIG_WAIT
#undef SIG_KEY
#undef SIG_TEXT
}
