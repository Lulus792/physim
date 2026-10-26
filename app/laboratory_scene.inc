/* simulation.css scene-top overlay and scene-bottom flow, using live camera data. */
static int laboratory_scene_style_width(const app *a) {
    /* Detaching changes the surface, not the shared application design variant.
     * Actual frame width still controls wrapping and whether content fits. */
    if(a->laboratory_scene_host_width>0)return a->laboratory_scene_host_width;
    int w,h;SDL_GetWindowSize(a->window,&w,&h);(void)h;return w;
}
static float laboratory_scene_text_width(const struct nk_user_font *font,const char *text) {
    return font->width(font->userdata,font->height,text,(int)strlen(text));
}
static const char *laboratory_scene_caption(const app *a) {
    int direction=laboratory_camera_direction(a);
    if(direction<0)return "Eigene Ansicht";
    if(!direction)return a->orthographic?"Orthografisch":"Perspektive";
    const char *axes[]={"Vorne","Seite","Oben"};return axes[direction-1];
}
static const char *laboratory_scene_state(const app *a) {
    if(!a->loaded)return "Kein Projekt";
    if(a->timeline_error[0])return "Fehler";
    if(a->timeline_browsing || a->analysis_only)return "Aufzeichnung";
    if(a->runner.running)return !a->hello?"Startet …":a->paused?"Pausiert":"Läuft";
    if(a->job.running)return "Build …";
    return a->built?"Bereit":"Ungebaut";
}
static const char *laboratory_scene_hint_text(const app *a) {
    return a->camera_2d?"Verschieben: rechte/Mitteltaste · Zoom: Mausrad":"Orbit: rechte Maustaste · Zoom: Mausrad";
}
typedef struct {float f,padding,button_width,button_height,view_width,view_height,height;bool stacked;} laboratory_scene_footer_metrics;
static laboratory_scene_footer_metrics laboratory_scene_metrics(const app *a,float width) {
    int window_width=laboratory_scene_style_width(a);
    float f=a->preferences.ui_size/16.f;const struct nk_user_font *font=&a->laboratory_scene_font;
    const char *views[]={"Perspektive · Meter","Orthografisch · Meter","Vorne · Meter","Seite · Meter","Oben · Meter","2D · XY · Meter","2D · YZ · Meter","2D · XZ · Meter"};
    laboratory_scene_footer_metrics m={.f=f,.padding=(window_width<=1150?10:14)*f,.button_height=22.5f*f};
    float inner=fmaxf(1,width-2*m.padding);
    m.button_width=laboratory_scene_text_width(&a->workspace_status_font,"Zentrieren")+27*f;
    for(unsigned i=0;i<8;i++)m.view_width=fmaxf(m.view_width,laboratory_scene_text_width(font,views[i]));
    m.stacked=m.view_width+14*f+m.button_width>inner;
    unsigned lines=1;if(m.stacked)for(unsigned i=0;i<8;i++)lines=NK_MAX(lines,laboratory_wrapped_lines(font,views[i],inner));
    m.view_height=13.05f*f*lines;
    m.height=fmaxf(32*f,8*f+(m.stacked?m.view_height+6*f+m.button_height:fmaxf(m.view_height,m.button_height)));
    return m;
}
static struct nk_vec2 laboratory_scene_select_size(const app *a,float width) {
    float f=a->preferences.ui_size/16.f;const struct nk_user_font *font=&a->workspace_status_font;
    const char *names[]={"Perspektive","Vorne","Seite","Oben","Eigene Ansicht","Orthografisch"};float text=0;unsigned lines=1;
    for(unsigned i=0;i<6;i++)text=fmaxf(text,laboratory_scene_text_width(font,names[i]));
    float w=fminf(text+32*f,fmaxf(1,width));
    for(unsigned i=0;i<6;i++)lines=NK_MAX(lines,laboratory_wrapped_lines(font,names[i],fmaxf(1,w-32*f)));
    return nk_vec2(w,(12+14.5f*lines)*f);
}
static float laboratory_scene_minimum(const app *a,float width) {
    if(width<80 || !a->laboratory_scene_font.width || !a->workspace_status_font.width)return LABORATORY_SCENE_MIN;
    float f=a->preferences.ui_size/16.f;int w=laboratory_scene_style_width(a);
    float inset=(w<=1150?12:17)*f;struct nk_vec2 select=laboratory_scene_select_size(a,width-2*inset);
    return fmaxf(LABORATORY_SCENE_MIN,laboratory_scene_metrics(a,width).height+fmaxf(64,(w<=1150?12:14)*f+select.y+8*f));
}
static struct nk_rect laboratory_scene_prepare(app *a,struct nk_rect frame) {
    float f=a->preferences.ui_size/16.f;int w=laboratory_scene_style_width(a);
    laboratory_scene_footer_metrics m=laboratory_scene_metrics(a,frame.w);
    a->laboratory_scene_frame=frame;
    a->laboratory_scene_owner=SDL_GetWindowID(a->window);
    a->laboratory_scene_footer=nk_rect(frame.x,frame.y+frame.h-m.height,frame.w,m.height);
    struct nk_rect view=nk_rect(frame.x,frame.y,frame.w,fmaxf(1,frame.h-m.height));
    float inset=(w<=1150?12:17)*f,top=(w<=1150?12:14)*f;
    struct nk_vec2 select=laboratory_scene_select_size(a,frame.w-2*inset);
    a->laboratory_scene_select=nk_rect(frame.x+frame.w-inset-select.x,frame.y+top,select.x,select.y);
    float label=laboratory_scene_text_width(&a->laboratory_caption_font,"3D-Ansicht"),gap=12*f;
    bool label_visible=label+gap+select.x<=frame.w-2*inset;
    a->laboratory_scene_label=label_visible?nk_rect(frame.x+inset,frame.y+top+(select.y-15.95f*f)/2,label,15.95f*f):nk_rect(0,0,0,0);
    float state=laboratory_scene_text_width(&a->workspace_status_font,laboratory_scene_state(a))+16*f;
    bool status_visible=label_visible && w>850 && label+2*gap+state+select.x<=frame.w-2*inset;
    a->laboratory_scene_status=status_visible?nk_rect(frame.x+inset+label+gap,frame.y+top+(select.y-20.5f*f)/2,state,20.5f*f):nk_rect(0,0,0,0);
    if(a->camera_2d){const char *planes[]={"XY","YZ","XZ"};snprintf(a->laboratory_scene_view_text,sizeof a->laboratory_scene_view_text,"2D · %s · Meter",planes[a->camera_plane]);}
    else {int direction=laboratory_camera_direction(a);const char *axis[]={"Vorne","Seite","Oben"};snprintf(a->laboratory_scene_view_text,sizeof a->laboratory_scene_view_text,"%s · Meter",direction>0?axis[direction-1]:a->orthographic?"Orthografisch":"Perspektive");}
    struct nk_rect bottom=a->laboratory_scene_footer;float right=bottom.x+bottom.w-m.padding;
    a->laboratory_scene_view=nk_rect(bottom.x+m.padding,bottom.y+(m.stacked?4*f:(bottom.h-m.view_height)/2),m.stacked?fmaxf(1,bottom.w-2*m.padding):m.view_width,m.view_height);
    a->laboratory_scene_reset=nk_rect(right-m.button_width,m.stacked?bottom.y+4*f+m.view_height+6*f:bottom.y+(bottom.h-m.button_height)/2,m.button_width,m.button_height);
    float hint=laboratory_scene_text_width(&a->laboratory_scene_font,laboratory_scene_hint_text(a));
    bool hint_visible=!m.stacked && w>1150 && m.view_width+hint+m.button_width+28*f<=bottom.w-2*m.padding;
    a->laboratory_scene_hint=hint_visible?nk_rect(bottom.x+(bottom.w-hint)/2,bottom.y+(bottom.h-13.05f*f)/2,hint,13.05f*f):nk_rect(0,0,0,0);
    return view;
}
static void laboratory_scene_field(struct nk_context *ui,struct nk_rect r,bool focused,void *user) {
    app *a=user;float f=a->preferences.ui_size/16.f;struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    const struct nk_user_font *font=&a->workspace_status_font;const char *text=laboratory_scene_caption(a);
    nk_fill_rect(canvas,r,6*f,UI_COLOR(window));nk_stroke_rect(canvas,nk_rect(r.x+f/2,r.y+f/2,r.w-f,r.h-f),5.5f*f,f,UI_COLOR(border));
    float width=fmaxf(1,r.w-32*f),line=14.5f*f,height=line*laboratory_wrapped_lines(font,text,width);
    laboratory_wrapped_text(canvas,font,text,nk_rect(r.x+8*f,r.y+(r.h-height)/2,width,height),line,UI_COLOR(text));
    float x=r.x+r.w-13*f,y=r.y+r.h/2;nk_stroke_line(canvas,x-4*f,y-2*f,x,y+2*f,1.25f*f,UI_COLOR(text));nk_stroke_line(canvas,x,y+2*f,x+4*f,y-2*f,1.25f*f,UI_COLOR(text));
    if(focused)laboratory_focus_outline(a,r,10);
}
static void laboratory_scene_chrome(app *a) {
    struct nk_context *ui=a->ui;float f=a->preferences.ui_size/16.f;struct nk_command_buffer *canvas=nk_window_get_canvas(ui);struct nk_rect clip=canvas->clip;
    nk_push_scissor(canvas,plot_intersection(clip,a->laboratory_scene_frame));
    struct nk_rect r=a->laboratory_scene_label;
    if(r.w){laboratory_wrapped_text(canvas,&a->laboratory_caption_font,a->camera_2d?"2D-Ansicht":"3D-Ansicht",r,15.95f*f,UI_COLOR(text));ps_ui_accessibility_label(ui,r,a->camera_2d?"2D-Ansicht":"3D-Ansicht");}
    r=a->laboratory_scene_status;
    if(r.w){nk_fill_rect(canvas,r,4*f,UI_COLOR(surface));const char *state=laboratory_scene_state(a);nk_draw_text(canvas,nk_rect(r.x+8*f,r.y+(r.h-a->workspace_status_font.height)/2,r.w-16*f+1,a->workspace_status_font.height),state,(int)strlen(state),&a->workspace_status_font,nk_rgba(0,0,0,0),muted());ps_ui_accessibility_label(ui,r,state);}
    memset(a->laboratory_scene_choices,0,sizeof a->laboratory_scene_choices);a->laboratory_scene_menu=false;
    nk_style_push_font(ui,&a->workspace_status_font);nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(6*f,6*f));nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(0,0));
    if(ps_ui_combo_at_named_painted(ui,"Kameraansicht",laboratory_scene_caption(a),a->laboratory_scene_select,nk_vec2(a->laboratory_scene_select.w,124*f),laboratory_scene_field,a)) {
        const char *choices[]={"Perspektive","Vorne","Seite","Oben"};a->laboratory_scene_menu=true;ps_ui_row_exact_dynamic(ui,28*f,1);
        for(unsigned i=0;i<4;i++){a->laboratory_scene_choices[i]=nk_widget_bounds(ui);if(ps_ui_fixed_button_label(ui,choices[i])){laboratory_camera_preset(a,i);a->laboratory_scene_return=true;nk_combo_close(ui);}}
        if(nk_input_is_key_pressed(&ui->input,NK_KEY_TEXT_RESET_MODE)){a->laboratory_scene_return=true;nk_combo_close(ui);}nk_combo_end(ui);
    }
    nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);nk_style_pop_font(ui);
    nk_push_scissor(canvas,plot_intersection(clip,a->laboratory_scene_frame));
    nk_fill_rect(canvas,a->laboratory_scene_footer,0,UI_COLOR(canvas));
    laboratory_wrapped_text(canvas,&a->laboratory_scene_font,a->laboratory_scene_view_text,a->laboratory_scene_view,13.05f*f,muted());ps_ui_accessibility_label(ui,a->laboratory_scene_view,a->laboratory_scene_view_text);
    if(a->laboratory_scene_hint.w){const char *hint=laboratory_scene_hint_text(a);laboratory_wrapped_text(canvas,&a->laboratory_scene_font,hint,a->laboratory_scene_hint,13.05f*f,muted());ps_ui_accessibility_label(ui,a->laboratory_scene_hint,hint);}
    r=a->laboratory_scene_reset;bool focused=false;bool activated=ps_ui_button_at_custom_focus(ui,r,"Kamera auf Standardansicht zurücksetzen",false,&focused);
    bool hover=!(ui->current->layout->flags&NK_WINDOW_ROM) && !ui->current->widgets_disabled && nk_input_is_mouse_hovering_rect(&ui->input,r);
    if(hover)nk_fill_rect(canvas,r,6*f,UI_COLOR(selection_active));
    if(a->preferences.theme==PS_THEME_HIGH_CONTRAST)nk_stroke_rect(canvas,nk_rect(r.x+f/2,r.y+f/2,r.w-f,r.h-f),5.5f*f,f,UI_COLOR(border));
    float x=r.x+7*f,y=r.y+r.h/2;nk_stroke_circle(canvas,nk_rect(x,y-4*f,8*f,8*f),f,UI_COLOR(text));nk_fill_circle(canvas,nk_rect(x+3*f,y-f,2*f,2*f),UI_COLOR(text));
    nk_draw_text(canvas,nk_rect(r.x+20*f,r.y+(r.h-a->workspace_status_font.height)/2,r.w-27*f+1,a->workspace_status_font.height),"Zentrieren",10,&a->workspace_status_font,nk_rgba(0,0,0,0),UI_COLOR(text));
    if(focused)laboratory_focus_outline(a,r,10);
    if(activated)laboratory_camera_preset(a,0);
    if(a->laboratory_scene_return && nk_sdl_accessibility_request_focus(ui,NULL,"Kameraansicht"))a->laboratory_scene_return=false;
    nk_push_scissor(canvas,clip);
}
