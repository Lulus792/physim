/* The project form uses the existing project builder and native folder picker. */
static void open_project_manager(app *a) {
    a->project_manager=true;a->workspace_preview_path[0]=0;
    a->manager_error[0]=0;a->manager_error_reveal=false;
    a->manager_keyboard=a->manager_focus_known=a->manager_focus_reveal=false;a->manager_focus=0;
}
static bool project_manager_keyboard_key(app *a,const SDL_KeyboardEvent *key) {
    if(!a->project_manager || a->toolbar_keyboard || a->toolbar_menu || a->dialog_pending)return false;
    if(key->mod&(SDL_KMOD_CTRL|SDL_KMOD_GUI|SDL_KMOD_ALT))return false;
    if(key->key==SDLK_ESCAPE){a->project_manager=false;a->manager_keyboard=false;return true;}
    if(key->key==SDLK_TAB) {
        if(a->manager_focus_known && (a->manager_focus==0 || a->manager_focus==2)) {
            char *field=a->manager_focus==0?a->manager_parent:a->manager_name;char before[4096];snprintf(before,sizeof before,"%s",field);
            ps_ui_flush_edit(a->ui,"workspace",field,a->manager_focus==0?sizeof a->manager_parent:sizeof a->manager_name,a->manager_focus_bounds[a->manager_focus]);
            if(strcmp(before,field)){a->manager_error[0]=0;a->manager_error_reveal=false;}
        }
        int direction=key->mod&SDL_KMOD_SHIFT?-1:1,focus=a->manager_focus;
        if(!a->manager_focus_known)focus=direction>0?-1:9;
        do {focus=(focus+direction+9)%9;}while(a->manager_analysis_only && (focus==4 || focus==5));
        a->manager_focus=focus;a->manager_keyboard=a->manager_focus_known=a->manager_focus_reveal=true;
        return true;
    }
    if(!a->manager_keyboard || a->manager_focus==0 || a->manager_focus==2)return false;
    bool activate=key->key==SDLK_RETURN || key->key==SDLK_KP_ENTER || key->key==SDLK_SPACE;
    int direction=key->key==SDLK_LEFT || key->key==SDLK_UP?-1:key->key==SDLK_RIGHT || key->key==SDLK_DOWN?1:0;
    if(activate && key->repeat)return true;
    int focus=a->manager_focus;
    if(focus==1 && activate)choose_workspace_path(a,PS_DIALOG_MANAGER_PARENT);
    else if(focus==3 && (activate || direction))a->manager_analysis_only=!a->manager_analysis_only;
    else if(focus==4 && (activate || direction))a->manager_template=(a->manager_template+(direction<0?7:1))%8;
    else if(focus==5 && (activate || direction))a->manager_experiment_language=1-a->manager_experiment_language;
    else if(focus==6 && (activate || direction))a->template_analysis_language=1-a->template_analysis_language;
    else if(focus==7 && activate)create_managed_project(a);
    else if(focus==8 && activate){a->project_manager=false;a->manager_keyboard=false;}
    return true;
}
static void project_manager_focus_draw(app *a,int id,struct nk_rect bounds) {
    struct nk_context *ui=a->ui;a->manager_focus_bounds[id]=bounds;
    if(nk_input_is_mouse_pressed(&ui->input,NK_BUTTON_LEFT) && nk_input_is_mouse_hovering_rect(&ui->input,bounds)) {
        a->manager_keyboard=false;a->manager_focus_known=true;a->manager_focus=id;
    }
    if(!a->manager_keyboard || a->manager_focus!=id || a->toolbar_keyboard)return;
    nk_stroke_rect(nk_window_get_canvas(ui),bounds,3,2,UI_COLOR(accent));
    if(a->manager_focus_reveal) {
        struct nk_rect clip=nk_window_get_content_region(ui);float delta=0;
        if(bounds.y<clip.y)delta=bounds.y-clip.y;
        else if(bounds.y+bounds.h>clip.y+clip.h)delta=bounds.y+bounds.h-clip.y-clip.h;
        nk_uint x,y;nk_window_get_scroll(ui,&x,&y);
        if(delta)nk_window_set_scroll(ui,x,(nk_uint)fmaxf(0,(float)y+delta));
        else a->manager_focus_reveal=false;
    }
}
static void project_manager_field(app *a,int id,char *text,int capacity) {
    struct nk_context *ui=a->ui;
    if(a->manager_keyboard) {
        if(a->manager_focus==id && !ui->current->edit.active)nk_edit_focus(ui,NK_EDIT_ALWAYS_INSERT_MODE);
        else if(a->manager_focus!=id)nk_edit_unfocus(ui);
    }
    char before[4096];snprintf(before,sizeof before,"%s",text);
    struct nk_rect bounds=nk_widget_bounds(ui);
    nk_edit_string_zero_terminated(ui,NK_EDIT_FIELD,text,capacity,nk_filter_default);
    if(strcmp(before,text))a->manager_error[0]=0;
    project_manager_focus_draw(a,id,bounds);
}
