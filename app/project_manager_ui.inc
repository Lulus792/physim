/* Selected B wizard; the existing builder owns actual project creation. */
static const char *manager_templates[]={"Pendel","Wurf mit Luftwiderstand","Kugelstoß & Medien","Box auf Ebene","Feder–Masse–Dämpfer","Wurf mit Unsicherheit","Boxstoß","Auftrieb"};
static const char *manager_descriptions[]={"Kräfte, Bewegung und Messkanäle","Flugbahn und Widerstand","Kontakt und Material","Bewegung und Reibung","Schwingung und Dämpfung","Seeds und Streuung","Starre Körper und Kontakt","Körper und Medium"};
static const char *project_manager_name(app *a,int id) {
    static const char *names[]={"Speicherort","Speicherort auswählen","Projektname","Nur Auswertung gespeicherter Läufe","Vorlage","Sprache des Experiments","Sprache der Auswertung","Weiter","Abbrechen","Zurück","Projektdialog schließen"};
    return id>=11?manager_templates[id-11]:id==7 && a->manager_step==2?"Projekt anlegen":names[id];
}
static void project_manager_window_destroy(app *a) {
    if(!a->manager_window){a->manager_close_pending=false;return;}
    ps_graphics_make_current(a->manager_graphics);
    nk_sdl_shutdown(a->manager_ui);ps_graphics_destroy(a->manager_graphics);SDL_DestroyWindow(a->manager_window);
    a->manager_window=NULL;a->manager_graphics=NULL;a->manager_ui=NULL;a->manager_close_pending=false;
    ps_graphics_make_current(a->graphics);
}
static bool project_manager_window_open(app *a) {
    if(a->manager_close_pending)project_manager_window_destroy(a);
    if(a->manager_window){SDL_ShowWindow(a->manager_window);SDL_RaiseWindow(a->manager_window);return true;}
    a->manager_source_id=SDL_GetWindowID(a->window);a->manager_source_ui=a->ui;
    int w,h,x,y;SDL_GetWindowSize(a->window,&w,&h);SDL_GetWindowPosition(a->window,&x,&y);float f=a->preferences.ui_size/16.f;
    if(!ps_graphics_make_current(a->graphics) || !SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT,1))goto fail;
    a->manager_window=SDL_CreateWindow("Physim | Neues Projekt",(int)fminf(1080*f,fmaxf(320,w-80)),
        (int)fminf(920*f,fmaxf(320,h-80)),SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE|SDL_WINDOW_HIGH_PIXEL_DENSITY|SDL_WINDOW_HIDDEN);
    if(a->manager_window)a->manager_graphics=ps_graphics_create(a->manager_window);
    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT,0);
    if(!a->manager_graphics)goto fail;
    SDL_GL_SetSwapInterval(0);a->manager_ui=nk_sdl_init(a->manager_window,a->manager_graphics);if(!a->manager_ui)goto fail;
    nk_sdl_borrow_fonts(a->manager_ui,a->ui);nk_input_begin(a->manager_ui);nk_input_end(a->manager_ui);
    SDL_SetWindowMinimumSize(a->manager_window,320,320);SDL_SetWindowPosition(a->manager_window,x+40,y+40);
    SDL_ShowWindow(a->manager_window);SDL_RaiseWindow(a->manager_window);ps_graphics_make_current(a->graphics);return true;
fail:
    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT,0);
    if(a->manager_graphics){ps_graphics_make_current(a->manager_graphics);if(a->manager_ui)nk_sdl_shutdown(a->manager_ui);ps_graphics_destroy(a->manager_graphics);}
    if(a->manager_window)SDL_DestroyWindow(a->manager_window);
    a->manager_window=NULL;a->manager_graphics=NULL;a->manager_ui=NULL;ps_graphics_make_current(a->graphics);
    status(a,"Das Projektfenster konnte nicht geöffnet werden.");return false;
}
static void project_manager_close(app *a) {
    if(!a->project_manager)return;
    browser_cancel_project_parent(a);
    ps_dialog_request_cancel_route(&a->dialog_requests,PS_DIALOG_MANAGER_PARENT);
    a->dialog_pending=ps_dialog_requests_pending(&a->dialog_requests);
    a->project_manager=false;a->manager_keyboard=a->manager_initial_focus=a->manager_popup_close=false;
    SDL_Window *source=SDL_GetWindowFromID(a->manager_source_id);
    bool return_focus=a->manager_window && SDL_GetKeyboardFocus()==a->manager_window && source &&
        !(SDL_GetWindowFlags(source)&(SDL_WINDOW_HIDDEN|SDL_WINDOW_MINIMIZED));
    a->manager_return_frames=return_focus?3:0;a->manager_close_pending=true;
    if(return_focus)SDL_RaiseWindow(source);
    /* A native/pointer activation runs inside nk_begin/nk_end. Retire these
     * windows in the next frame, when no window is being drawn. */
}
static void open_project_manager(app *a,const char *opener) {
    if(a->project_manager){if(a->manager_window){SDL_ShowWindow(a->manager_window);SDL_RaiseWindow(a->manager_window);}return;}
    snprintf(a->manager_opener,sizeof a->manager_opener,"%s",a->ui->active?a->ui->active->name_string:"workspace");
    snprintf(a->manager_return_label,sizeof a->manager_return_label,"%s",opener);a->manager_return_frames=0;
    if(!project_manager_window_open(a))return;
    a->project_manager=true;a->manager_step=0;
    a->manager_error[0]=0;a->manager_error_reveal=false;
    a->manager_keyboard=a->manager_focus_reveal=a->toolbar_keyboard;a->manager_focus_known=true;a->manager_focus=10;
    a->manager_initial_focus=true;a->manager_popup_close=false;nk_sdl_accessibility_keyboard_begin(a->manager_ui);
    struct nk_window *w=nk_window_find(a->manager_ui,"Neues Projekt");if(w)w->scrollbar=(struct nk_scroll){0,0};
}
static void project_manager_step(app *a,int step) {
    a->manager_step=step;a->manager_popup_close=false;a->manager_focus_known=a->manager_focus_reveal=a->manager_initial_focus=true;
    a->manager_focus=step==1?2:10;nk_sdl_accessibility_keyboard_begin(a->manager_ui);
    struct nk_window *w=nk_window_find(a->manager_ui,"Neues Projekt");if(w){w->edit.active=false;w->scrollbar=(struct nk_scroll){0,0};}
}
static bool project_manager_check(app *a) {
    const char *error=NULL;SDL_PathInfo info;char target[4096];
    if(!*a->manager_name || !strcmp(a->manager_name,".") || !strcmp(a->manager_name,"..") || strpbrk(a->manager_name,"/\\"))error="Einen gültigen Projektordnernamen eingeben.";
    else if(!SDL_GetPathInfo(a->manager_parent,&info) || info.type!=SDL_PATHTYPE_DIRECTORY)error="Der Zielordner für das Projekt existiert nicht.";
    else if(snprintf(target,sizeof target,"%s/%s",a->manager_parent,a->manager_name)>=(int)sizeof target)error="Projektpfad ist zu lang.";
    if(error){status(a,error);snprintf(a->manager_error,sizeof a->manager_error,"%s",error);a->manager_error_reveal=true;return false;}
    a->manager_error[0]=0;return true;
}
static void project_manager_next(app *a) {
    if(a->manager_step==0)project_manager_step(a,1);
    else if(a->manager_step==1){if(project_manager_check(a))project_manager_step(a,2);}
    else create_managed_project(a);
}
static int project_manager_order(app *a,int ids[19]) {
    int n=0;ids[n++]=10;
    if(a->manager_step==0){ids[n++]=3;if(!a->manager_analysis_only)for(int i=11;i<19;i++)ids[n++]=i;}
    else if(a->manager_step==1){ids[n++]=2;ids[n++]=0;ids[n++]=1;if(!a->manager_analysis_only)ids[n++]=5;ids[n++]=6;}
    ids[n++]=8;if(a->manager_step)ids[n++]=9;ids[n++]=7;return n;
}
static bool project_manager_keyboard_key(app *a,const SDL_KeyboardEvent *key) {
    if(!a->project_manager || a->dialog_requests.pending[PS_DIALOG_MANAGER_PARENT])return false;
    if(key->mod&(SDL_KMOD_CTRL|SDL_KMOD_GUI|SDL_KMOD_ALT))return false;
    if(key->key==SDLK_ESCAPE) {
        struct nk_window *window=nk_window_find(a->manager_ui,"Neues Projekt");
        if(window && window->popup.active)a->manager_popup_close=true;
        else project_manager_close(a);
        return true;
    }
    if(key->key==SDLK_TAB) {
        if(a->manager_focus_known && (a->manager_focus==0 || a->manager_focus==2)) {
            char *field=a->manager_focus==0?a->manager_parent:a->manager_name;
            ps_ui_flush_edit(a->manager_ui,"Neues Projekt",field,a->manager_focus==0?sizeof a->manager_parent:sizeof a->manager_name,a->manager_focus_bounds[a->manager_focus]);
        }
        int ids[19],n=project_manager_order(a,ids),at=-1,direction=key->mod&SDL_KMOD_SHIFT?-1:1;
        if(a->manager_focus_known)for(int i=0;i<n;i++)if(ids[i]==a->manager_focus)at=i;
        if(at<0)at=direction>0?-1:0;
        a->manager_focus=ids[(at+direction+n)%n];a->manager_keyboard=a->manager_focus_known=a->manager_focus_reveal=true;
        nk_sdl_accessibility_keyboard_begin(a->manager_ui);return true;
    }
    if(!a->manager_keyboard || a->manager_focus==0 || a->manager_focus==2)return false;
    bool activate=key->key==SDLK_RETURN || key->key==SDLK_KP_ENTER || key->key==SDLK_SPACE;
    int direction=key->key==SDLK_LEFT || key->key==SDLK_UP?-1:key->key==SDLK_RIGHT || key->key==SDLK_DOWN?1:0;
    if(activate && key->repeat)return true;int focus=a->manager_focus;
    if(focus==1 && activate)choose_workspace_path(a,PS_DIALOG_MANAGER_PARENT);
    else if(focus==3 && activate)a->manager_analysis_only=!a->manager_analysis_only;
    else if(focus>=11 && (activate || direction)){int i=activate?focus-11:(focus-11+direction+8)%8;a->manager_template=i;a->manager_focus=11+i;a->manager_focus_reveal=true;}
    else if(focus==5 && (activate || direction))a->manager_experiment_language=1-a->manager_experiment_language;
    else if(focus==6 && (activate || direction))a->template_analysis_language=1-a->template_analysis_language;
    else if(focus==7 && activate)project_manager_next(a);
    else if(focus==9 && activate)project_manager_step(a,a->manager_step-1);
    else if((focus==8 || focus==10) && activate)project_manager_close(a);
    return true;
}
static void project_manager_focus_draw(app *a,int id,struct nk_rect bounds) {
    struct nk_context *ui=a->manager_ui;a->manager_focus_bounds[id]=bounds;
    if(a->manager_initial_focus && a->manager_focus==id && nk_sdl_accessibility_keyboard_target(ui,bounds))a->manager_initial_focus=false;
    bool native=nk_sdl_accessibility_is_focus_target(ui,NULL,project_manager_name(a,id));
    if(!a->manager_keyboard && native){if(!a->manager_focus_known || a->manager_focus!=id)a->manager_focus_reveal=true;a->manager_focus=id;a->manager_focus_known=true;}
    if(nk_input_is_mouse_pressed(&ui->input,NK_BUTTON_LEFT) && nk_input_is_mouse_hovering_rect(&ui->input,bounds)){a->manager_keyboard=false;a->manager_focus_known=true;a->manager_focus=id;}
    bool custom=a->manager_keyboard && a->manager_focus==id;
    if(!custom && !native)return;
    if(custom){nk_sdl_accessibility_keyboard_target(ui,bounds);nk_stroke_rect(nk_window_get_canvas(ui),bounds,4,2,UI_COLOR(accent));}
    if(a->manager_focus_reveal) {
        struct nk_rect clip=nk_window_get_content_region(ui);float delta=0;
        if(bounds.y<clip.y)delta=bounds.y-clip.y;else if(bounds.y+bounds.h>clip.y+clip.h)delta=bounds.y+bounds.h-clip.y-clip.h;
        nk_uint x,y;nk_window_get_scroll(ui,&x,&y);
        if(SDL_getenv("PHYSIM_TEST_TRACE"))fprintf(stderr,"PROJECT FOCUS: id=%d scroll=%u delta=%.4f bounds=%.4f/%.4f clip=%.4f/%.4f\n",id,(unsigned)y,delta,bounds.y,bounds.h,clip.y,clip.h);
        if(delta)nk_window_set_scroll(ui,x,(nk_uint)fmaxf(0,ceilf((float)y+delta)));else a->manager_focus_reveal=false;
    }
}
static void project_manager_field(app *a,int id,char *text,int capacity) {
    struct nk_context *ui=a->manager_ui;
    if(a->manager_keyboard){if(a->manager_focus==id && !ui->current->edit.active)nk_edit_focus(ui,NK_EDIT_ALWAYS_INSERT_MODE);else if(a->manager_focus!=id)nk_edit_unfocus(ui);}
    else if(nk_sdl_accessibility_is_focused(ui,NULL,project_manager_name(a,id)) && !ui->current->edit.active)nk_edit_focus(ui,NK_EDIT_ALWAYS_INSERT_MODE);
    char before[4096];snprintf(before,sizeof before,"%s",text);struct nk_rect bounds=nk_widget_bounds(ui);
    nk_sdl_edit_named(ui,project_manager_name(a,id),NK_EDIT_FIELD,text,capacity,nk_filter_default);
    if(id==0 && strcmp(before,text) && SDL_getenv("PHYSIM_TEST_TRACE"))fprintf(stderr,"PROJECT LOCATION EDIT: before=%s after=%s\n",before,text);
    if(strcmp(before,text)){a->manager_error[0]=0;a->manager_error_reveal=false;}
    project_manager_focus_draw(a,id,bounds);
}
#include "project_wizard_ui.inc"
static void project_manager_tick(app *a) {
    if(a->manager_close_pending)project_manager_window_destroy(a);
    if(!a->manager_return_frames)return;
    SDL_Window *source=SDL_GetWindowFromID(a->manager_source_id);
    SDL_Window *focus=SDL_GetKeyboardFocus();
    if(!source || (SDL_GetWindowFlags(source)&(SDL_WINDOW_HIDDEN|SDL_WINDOW_MINIMIZED)) || (focus && focus!=source)){a->manager_return_frames=0;return;}
    /* Destroying the child can precede its parent's queued FOCUS_GAINED. Never
     * steal another window's focus; give the expected return a short grace. */
    if(!focus){--a->manager_return_frames;return;}
    if(*a->manager_opener)nk_window_set_focus(a->manager_source_ui,a->manager_opener);
    if(nk_sdl_accessibility_request_focus(a->manager_source_ui,NULL,a->manager_return_label) || !--a->manager_return_frames)a->manager_return_frames=0;
}
static bool project_manager_event(app *a,const SDL_Event *event,SDL_Window *window) {
    if(!a->manager_window || window!=a->manager_window)return false;
    if(event->type==SDL_EVENT_WINDOW_CLOSE_REQUESTED){project_manager_close(a);return true;}
    if(!a->project_manager)return true;
    if(event->type==SDL_EVENT_WINDOW_RESIZED && a->manager_keyboard && a->manager_focus_known)a->manager_focus_reveal=true;
    bool blocked=a->dialog_requests.pending[PS_DIALOG_MANAGER_PARENT]!=NULL;
    if(blocked && event->type==SDL_EVENT_KEY_DOWN && event->key.key==SDLK_ESCAPE){project_manager_close(a);return true;}
    if(blocked && (event->type==SDL_EVENT_KEY_DOWN || event->type==SDL_EVENT_TEXT_INPUT || event->type==SDL_EVENT_TEXT_EDITING ||
        event->type==SDL_EVENT_MOUSE_BUTTON_DOWN || event->type==SDL_EVENT_MOUSE_WHEEL))return true;
    if(event->type==SDL_EVENT_KEY_DOWN){
        if(event->key.key==SDLK_F1 && !event->key.repeat){open_documentation(a,a->documentation?a->doc_topic:1);return true;}
        if(project_manager_keyboard_key(a,&event->key))return true;
    }
    if(nk_sdl_accessibility_event(a->manager_ui,event))return true;
    SDL_Event copy=*event;nk_sdl_handle_event(a->manager_ui,&copy);return true;
}
static bool project_manager_window_render(app *a) {
    if(!a->project_manager || !a->manager_window || SDL_GetWindowFlags(a->manager_window)&(SDL_WINDOW_HIDDEN|SDL_WINDOW_MINIMIZED))return true;
    if(!ps_graphics_make_current(a->manager_graphics))return false;
    nk_style_set_font(a->manager_ui,a->font_ui);nk_sdl_set_ui_size(a->manager_ui,a->preferences.ui_size);
    const ps_ui_palette *palette=a->preferences.theme==PS_THEME_LIGHT?&PS_UI_LIGHT:a->preferences.theme==PS_THEME_HIGH_CONTRAST?&PS_UI_HIGH_CONTRAST:&PS_UI_DARK;
    struct nk_color background=a->preferences.theme==PS_THEME_HIGH_CONTRAST?palette->window:a->preferences.theme==PS_THEME_LIGHT?nk_rgb(250,251,253):nk_rgb(32,35,42);
    ps_graphics_background(a->manager_graphics,background.r,background.g,background.b);
    graphic_input_guard guard={0};
    if(a->dialog_requests.pending[PS_DIALOG_MANAGER_PARENT]){
        guard.held=true;guard.input=a->manager_ui->input;ps_ui_font_layout *layout=a->manager_ui->userdata.ptr;
        guard.hook=layout->accessibility;layout->accessibility=NULL;memset(&a->manager_ui->input,0,sizeof a->manager_ui->input);
        a->manager_ui->input.mouse.pos=nk_vec2(-10000,-10000);
    }
    int w,h;SDL_GetWindowSize(a->manager_window,&w,&h);project_manager_view(a,w,h);
    if(guard.held){a->manager_ui->input=guard.input;((ps_ui_font_layout*)a->manager_ui->userdata.ptr)->accessibility=guard.hook;}
    nk_sdl_update_TextInput(a->manager_ui);bool okay=nk_sdl_render(a->manager_ui);
    if(*a->manager_capture){okay=ps_graphics_capture(a->manager_graphics,a->manager_capture) && okay;a->manager_capture[0]=0;}
    if(!ps_graphics_present(a->manager_graphics))okay=false;
    if(!ps_graphics_make_current(a->graphics))okay=false;return okay;
}
