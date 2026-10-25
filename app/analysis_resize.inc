/* One quiet border owns the existing chart/statistics split. Desired height is
 * session-owned; clamping for a smaller viewport never overwrites that wish. */
static const char *analysis_plot_separator_name(void) {return "Messkurve und Statistik";}
static float analysis_plot_maximum(const app *a) {
    return fmaxf(180,fminf(700,a->analysis_plot_content-260));
}
static float analysis_plot_dimension(app *a,float content) {
    a->analysis_plot_content=content;
    float wish=a->analysis_plot_manual?a->analysis_plot_height:a->analysis_plot_auto_height;
    return fmaxf(180,fminf(analysis_plot_maximum(a),wish));
}
static bool analysis_plot_fits(const app *a,struct nk_rect panel) {
    float automatic=roundf(fmaxf(0,panel.w-48)*326.f/800.f);
    float maximum=fmaxf(180,fminf(700,panel.h-260));
    float height=fmaxf(180,fminf(maximum,a->analysis_plot_manual?a->analysis_plot_height:automatic));
    return height<=panel.h-400;
}
static void analysis_plot_resize_abort(app *a) {
    if(a->analysis_plot_resizing) {
        if(SDL_getenv("PHYSIM_TEST_TRACE"))fprintf(stderr,"ANALYSIS RESIZE ABORT: current=%.2f original=%.2f manual=%d restore-manual=%d\n",a->analysis_plot_height,a->analysis_plot_original,a->analysis_plot_manual,a->analysis_plot_was_manual);
        a->analysis_plot_height=a->analysis_plot_original;
        a->analysis_plot_manual=a->analysis_plot_was_manual;
    }
    a->analysis_plot_resizing=false;a->analysis_plot_cancelled=true;SDL_CaptureMouse(false);
}
static void analysis_plot_reveal_key(app *a,float before,float after) {
    struct nk_window *owner=nk_window_find(a->ui,"workspace");
    struct nk_rect target=a->analysis_plot_separator_bounds,clip=a->analysis_plot_viewport;
    if(!owner || target.h<=0 || clip.h<=0)return;
    target.y+=after-before;
    float delta=target.y<clip.y?target.y-clip.y:target.y+target.h>clip.y+clip.h?target.y+target.h-clip.y-clip.h:0;
    owner->scrollbar.y=(nk_uint)fmaxf(0,(float)owner->scrollbar.y+(delta>0?ceilf(delta):floorf(delta)));
}
static bool analysis_plot_resize_key(app *a,const SDL_KeyboardEvent *key) {
    if(!analysis_visible(a) || a->show_report || a->toolbar_menu ||
       key->mod&(SDL_KMOD_CTRL|SDL_KMOD_ALT|SDL_KMOD_GUI))return false;
    if(key->key==SDLK_ESCAPE && a->analysis_plot_resizing){analysis_plot_resize_abort(a);return true;}
    if(!nk_sdl_accessibility_is_focused(a->ui,NULL,analysis_plot_separator_name()))return false;
    float before=analysis_plot_dimension(a,a->analysis_plot_content),value=before,step=key->mod&SDL_KMOD_SHIFT?40:10;
    if(key->key==SDLK_UP)value-=step;
    else if(key->key==SDLK_DOWN)value+=step;
    else if(key->key==SDLK_HOME)value=180;
    else if(key->key==SDLK_END)value=analysis_plot_maximum(a);
    else if(key->key==SDLK_RETURN || key->key==SDLK_KP_ENTER){a->analysis_plot_manual=false;analysis_plot_reveal_key(a,before,analysis_plot_dimension(a,a->analysis_plot_content));return true;}
    else return false;
    a->analysis_plot_height=fmaxf(180,fminf(analysis_plot_maximum(a),value));a->analysis_plot_manual=true;
    analysis_plot_reveal_key(a,before,a->analysis_plot_height);
    return true;
}
static void analysis_plot_separator(app *a) {
    struct nk_context *ui=a->ui;struct nk_input *in=&ui->input;
    nk_layout_row_dynamic(ui,1,1);struct nk_rect line;nk_widget(&line,ui);
    a->analysis_plot_separator_bounds=nk_rect(line.x,line.y-4,line.w,9);
    a->analysis_plot_viewport=nk_window_get_canvas(ui)->clip;
    struct nk_rect handle=plot_intersection(a->analysis_plot_separator_bounds,a->analysis_plot_viewport);
    a->analysis_plot_splitter=handle;
    bool blocked=a->toolbar_menu || a->toolbar_pointer_consumed || (ui->current->flags&NK_WINDOW_ROM) ||
        (ui->current->popup.active && ui->current->popup.type!=NK_PANEL_TOOLTIP);
    bool down=nk_input_is_mouse_down(in,NK_BUTTON_LEFT),hover=!blocked && handle.w>0 && handle.h>0 && nk_input_is_mouse_hovering_rect(in,handle);
    if(!down){if(a->analysis_plot_resizing)SDL_CaptureMouse(false);a->analysis_plot_resizing=false;a->analysis_plot_cancelled=false;}
    if(blocked && a->analysis_plot_resizing)analysis_plot_resize_abort(a);
    float value=analysis_plot_dimension(a,a->analysis_plot_content);
    if(hover && !a->analysis_plot_cancelled && nk_input_is_mouse_pressed(in,NK_BUTTON_LEFT)) {
        nk_sdl_accessibility_request_focus(ui,NULL,analysis_plot_separator_name());
        if(nk_input_is_mouse_pressed(in,NK_BUTTON_DOUBLE))a->analysis_plot_manual=false;
        else {
            a->analysis_plot_resizing=true;a->analysis_plot_moved=false;a->analysis_plot_original=a->analysis_plot_height;
            a->analysis_plot_was_manual=a->analysis_plot_manual;
            a->analysis_plot_grab=nk_vec2(value,in->mouse.pos.y);SDL_CaptureMouse(true);
        }
    }
    if(a->analysis_plot_resizing && down && in->mouse.pos.y!=a->analysis_plot_grab.y)a->analysis_plot_moved=true;
    if(a->analysis_plot_resizing && down && a->analysis_plot_moved) {
        a->analysis_plot_height=fmaxf(180,fminf(analysis_plot_maximum(a),a->analysis_plot_grab.x+in->mouse.pos.y-a->analysis_plot_grab.y));
        a->analysis_plot_manual=true;
    }
    if(!blocked && ps_ui_separator_at(ui,handle,analysis_plot_separator_name(),180,&value,analysis_plot_maximum(a))) {
        a->analysis_plot_height=value;a->analysis_plot_manual=true;
    }
    bool focused=nk_sdl_accessibility_is_focused(ui,NULL,analysis_plot_separator_name());
    nk_fill_rect(nk_window_get_canvas(ui),line,0,hover || focused?UI_COLOR(accent):UI_COLOR(border));
    if(SDL_GetMouseFocus()==a->window) {
        if(hover || a->analysis_plot_resizing) {
            if(!a->laboratory_cursors[1])a->laboratory_cursors[1]=SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NS_RESIZE);
            if(a->laboratory_cursors[1])SDL_SetCursor(a->laboratory_cursors[1]);
        } else if(a->analysis_plot_hover)SDL_SetCursor(SDL_GetDefaultCursor());
    }
    a->analysis_plot_hover=hover;
}
