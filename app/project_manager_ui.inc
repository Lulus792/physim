/* Selected B wizard; the existing builder owns actual project creation. */
static const char *manager_templates[]={"Pendel","Wurf mit Luftwiderstand","Kugelstoß & Medien","Box auf Ebene","Feder–Masse–Dämpfer","Wurf mit Unsicherheit","Boxstoß","Auftrieb"};
static const char *manager_descriptions[]={"Kräfte, Bewegung und Messkanäle","Flugbahn und Widerstand","Kontakt und Material","Bewegung und Reibung","Schwingung und Dämpfung","Seeds und Streuung","Starre Körper und Kontakt","Körper und Medium"};
static const char *project_manager_name(app *a,int id) {
    static const char *names[]={"Speicherort","Speicherort auswählen","Projektname","Nur Auswertung gespeicherter Läufe","Vorlage","Sprache des Experiments","Sprache der Auswertung","Weiter","Abbrechen","Zurück","Projektdialog schließen"};
    return id>=11?manager_templates[id-11]:id==7 && a->manager_step==2?"Projekt anlegen":names[id];
}
static void project_manager_close(app *a) {
    a->project_manager=false;a->manager_keyboard=a->manager_initial_focus=a->manager_popup_close=false;
    a->manager_return_frames=3;
    /* A native/pointer activation runs inside nk_begin/nk_end. Retire these
     * windows in the next frame, when no window is being drawn. */
}
static void open_project_manager(app *a,const char *opener) {
    snprintf(a->manager_opener,sizeof a->manager_opener,"%s",a->ui->active?a->ui->active->name_string:"workspace");
    snprintf(a->manager_return_label,sizeof a->manager_return_label,"%s",opener);a->manager_return_frames=0;
    a->project_manager=true;a->workspace_preview_path[0]=0;a->manager_step=0;
    a->manager_error[0]=0;a->manager_error_reveal=false;
    a->manager_keyboard=a->manager_focus_reveal=a->toolbar_keyboard;a->manager_focus_known=true;a->manager_focus=10;
    a->manager_initial_focus=true;a->manager_popup_close=false;nk_sdl_accessibility_keyboard_begin(a->ui);
    struct nk_window *w=nk_window_find(a->ui,"Neues Projekt");if(w)w->scrollbar=(struct nk_scroll){0,0};
}
static void project_manager_step(app *a,int step) {
    a->manager_step=step;a->manager_popup_close=false;a->manager_focus_known=a->manager_focus_reveal=a->manager_initial_focus=true;
    a->manager_focus=step==1?2:10;nk_sdl_accessibility_keyboard_begin(a->ui);
    struct nk_window *w=nk_window_find(a->ui,"Neues Projekt");if(w){w->edit.active=false;w->scrollbar=(struct nk_scroll){0,0};}
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
    if(!a->project_manager || a->dialog_pending)return false;
    if(key->mod&(SDL_KMOD_CTRL|SDL_KMOD_GUI|SDL_KMOD_ALT))return false;
    if(key->key==SDLK_ESCAPE) {
        struct nk_window *window=nk_window_find(a->ui,"Neues Projekt");
        if(window && window->popup.active)a->manager_popup_close=true;
        else project_manager_close(a);
        return true;
    }
    if(key->key==SDLK_TAB) {
        if(a->manager_focus_known && (a->manager_focus==0 || a->manager_focus==2)) {
            char *field=a->manager_focus==0?a->manager_parent:a->manager_name;
            ps_ui_flush_edit(a->ui,"Neues Projekt",field,a->manager_focus==0?sizeof a->manager_parent:sizeof a->manager_name,a->manager_focus_bounds[a->manager_focus]);
        }
        int ids[19],n=project_manager_order(a,ids),at=-1,direction=key->mod&SDL_KMOD_SHIFT?-1:1;
        if(a->manager_focus_known)for(int i=0;i<n;i++)if(ids[i]==a->manager_focus)at=i;
        if(at<0)at=direction>0?-1:0;
        a->manager_focus=ids[(at+direction+n)%n];a->manager_keyboard=a->manager_focus_known=a->manager_focus_reveal=true;
        nk_sdl_accessibility_keyboard_begin(a->ui);return true;
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
    struct nk_context *ui=a->ui;a->manager_focus_bounds[id]=bounds;
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
    struct nk_context *ui=a->ui;
    if(a->manager_keyboard){if(a->manager_focus==id && !ui->current->edit.active)nk_edit_focus(ui,NK_EDIT_ALWAYS_INSERT_MODE);else if(a->manager_focus!=id)nk_edit_unfocus(ui);}
    else if(nk_sdl_accessibility_is_focused(ui,NULL,project_manager_name(a,id)) && !ui->current->edit.active)nk_edit_focus(ui,NK_EDIT_ALWAYS_INSERT_MODE);
    char before[4096];snprintf(before,sizeof before,"%s",text);struct nk_rect bounds=nk_widget_bounds(ui);
    nk_sdl_edit_named(ui,project_manager_name(a,id),NK_EDIT_FIELD,text,capacity,nk_filter_default);
    if(strcmp(before,text)){a->manager_error[0]=0;a->manager_error_reveal=false;}
    project_manager_focus_draw(a,id,bounds);
}
#include "project_wizard_ui.inc"
