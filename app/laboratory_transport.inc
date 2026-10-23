/* The approved simulation.css transport: 11px text, natural nowrap buttons,
 * 6px gaps, 32px controls, a separate stop/reset divider and a compact select.
 * Every font handle used by queued commands belongs to app, not this stack. */
static float laboratory_transport_text(const struct nk_user_font *font,const char *text) {
    return font->width(font->userdata,font->height,text,(int)strlen(text));
}
static struct nk_color laboratory_transport_color(struct nk_context *ui,struct nk_color color) {
    if(ui->current->widgets_disabled)color.a=(nk_byte)lroundf(color.a*.45f);
    return color;
}
static void laboratory_transport_focus(struct nk_context *ui,struct nk_command_buffer *canvas,struct nk_rect r,float f) {
    struct nk_rect clip=canvas->clip;
    nk_push_scissor(canvas,nk_window_get_bounds(ui));
    nk_stroke_rect(canvas,nk_rect(r.x-4*f,r.y-4*f,r.w+8*f,r.h+8*f),10*f,2*f,UI_COLOR(accent));
    nk_push_scissor(canvas,clip);
}
static bool laboratory_transport_button(app *a,struct nk_rect r,const char *name,const char *caption,bool primary) {
    struct nk_context *ui=a->ui;float f=a->preferences.ui_size/16.f;
    bool enabled=!ui->current->widgets_disabled;
    bool hover=enabled && !(ui->current->layout->flags&NK_WINDOW_ROM) && nk_input_is_mouse_hovering_rect(&ui->input,r);
    bool focused=false;
    struct nk_color fill=primary?UI_COLOR(primary):hover?UI_COLOR(selection_active):UI_COLOR(surface);
    if(primary && hover){if(a->preferences.theme==PS_THEME_HIGH_CONTRAST)fill=UI_COLOR(primary_hover);else{fill.r=(nk_byte)fminf(255,roundf(fill.r*1.12f));fill.g=(nk_byte)fminf(255,roundf(fill.g*1.12f));fill.b=(nk_byte)fminf(255,roundf(fill.b*1.12f));}}
    struct nk_color text=primary?UI_COLOR(primary_text):UI_COLOR(text);
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);struct nk_rect clip=canvas->clip;
    nk_push_scissor(canvas,nk_window_get_bounds(ui));
    nk_fill_rect(canvas,r,6*f,laboratory_transport_color(ui,fill));
    if(a->preferences.theme==PS_THEME_HIGH_CONTRAST)nk_stroke_rect(canvas,r,6*f,f,laboratory_transport_color(ui,UI_COLOR(border)));
    const struct nk_user_font *font=&a->workbench_fonts[0];float width=laboratory_transport_text(font,caption);
    nk_draw_text(canvas,nk_rect(r.x+(r.w-width)/2,r.y+(r.h-font->height)/2,width+1,font->height),caption,(int)strlen(caption),font,nk_rgba(0,0,0,0),laboratory_transport_color(ui,text));
    nk_push_scissor(canvas,clip);
    bool pressed=ps_ui_button_at_custom_focus(ui,r,name,false,&focused);
    if(focused)laboratory_transport_focus(ui,canvas,r,f);
    return pressed;
}
static void laboratory_speed_text(double value,char text[80],bool detail) {
    if(!value)snprintf(text,80,"%s",detail?"Offline · maximal":"Offline");
    else snprintf(text,80,detail?"%.3g× Echtzeit":"%.3g×",value);
    for(char *p=text;*p;p++)if(*p=='.')*p=',';
}
typedef struct {app *a;const char *label;} laboratory_speed_paint;
static void laboratory_speed_field(struct nk_context *ui,struct nk_rect r,bool focused,void *user) {
    laboratory_speed_paint *paint=user;app *a=paint->a;float f=a->preferences.ui_size/16.f;
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);struct nk_rect clip=canvas->clip;
    nk_push_scissor(canvas,nk_window_get_bounds(ui));
    nk_fill_rect(canvas,r,6*f,laboratory_transport_color(ui,UI_COLOR(surface)));
    if(a->preferences.theme==PS_THEME_HIGH_CONTRAST)nk_stroke_rect(canvas,r,6*f,f,laboratory_transport_color(ui,UI_COLOR(border)));
    const struct nk_user_font *font=&a->workbench_fonts[0];struct nk_color color=laboratory_transport_color(ui,UI_COLOR(text));
    nk_draw_text(canvas,nk_rect(r.x+9*f,r.y+(r.h-font->height)/2,fmaxf(1,r.w-33*f),font->height),paint->label,(int)strlen(paint->label),font,nk_rgba(0,0,0,0),color);
    struct nk_rect arrow=nk_rect(r.x+r.w-16*f,r.y+(r.h-8*f)/2,8*f,8*f);a->laboratory_speed_chevron=arrow;
    nk_stroke_line(canvas,arrow.x,arrow.y+2*f,arrow.x+4*f,arrow.y+6*f,1.25f*f,color);
    nk_stroke_line(canvas,arrow.x+4*f,arrow.y+6*f,arrow.x+8*f,arrow.y+2*f,1.25f*f,color);
    nk_push_scissor(canvas,clip);if(focused)laboratory_transport_focus(ui,canvas,r,f);
}
static struct nk_rect laboratory_transport_slot(struct nk_context *ui,float x,float width,float height) {
    struct nk_rect r;nk_layout_space_push(ui,nk_rect(x,0,width,height));nk_widget(&r,ui);return r;
}
static void laboratory_transport(app *a,float w) {
    struct nk_context *ui=a->ui;float f=a->preferences.ui_size/16.f;
    bool narrow=w<=1150;float height=(narrow?31:32)*f,gap=(narrow?4:6)*f,padding=(narrow?8:11)*f;
    const struct nk_user_font *font=&a->workbench_fonts[0];
    nk_style_push_font(ui,font);nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(18,(77-height)/2));
    if(laboratory_begin(a,"laboratory-transport",nk_rect(0,WORKSPACE_CONTENT_TOP,w,77),NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BACKGROUND)) {
        const char *name=a->loaded?project_name(a):"Simulation";
        const char *main_action=a->runner.running?(a->paused?"Fortsetzen":"Pause"):a->built?"Neuer Lauf":"Build";
        char main_caption[80];snprintf(main_caption,sizeof main_caption,"%s %s",a->runner.running && !a->paused?"Ⅱ":"▶",main_action);
        const char *reset=a->reset_pending?"Zurücksetzen …":"Zurücksetzen";
        char reset_caption[80];snprintf(reset_caption,sizeof reset_caption,"↺ %s",reset);
        const char *captions[]={main_caption,"▸│ Schritt","■ Stop",reset_caption};float widths[4];
        for(unsigned i=0;i<4;i++)widths[i]=laboratory_transport_text(font,captions[i])+2*padding+2*f;
        char speed[80];laboratory_speed_text(a->simulation_speed,speed,false);
        const char *state=a->analysis_only?"▷ Wiedergabe":a->runner.running?(!a->hello?"○ Startet …":a->paused?"Ⅱ Pausiert":"● Läuft"):a->job.running?"○ Build …":a->built?"○ Bereit":"○ Ungebaut";
        float status_width=laboratory_transport_text(font,state)+22*f;
        float project_width=190*f,project_gap=(narrow?14:24)*f;
        float speed_width=fmaxf(70*f,laboratory_transport_text(font,speed)+33*f),tempo_width=laboratory_transport_text(font,"Tempo");
        float controls=project_gap+widths[0]+widths[1]+widths[2]+widths[3]+6*gap+26*f+tempo_width+speed_width+status_width;
        /* Platform fallback metrics can be wider than the Segoe reference.
         * Give the title the remaining space before any action is clipped. */
        project_width=fminf(project_width,fmaxf(96*f,nk_window_get_content_region(ui).w-controls));
        struct nk_command_buffer *canvas=nk_window_get_canvas(ui);struct nk_rect clip=canvas->clip;
        nk_layout_space_begin(ui,NK_STATIC,height,9);
        struct nk_rect title=laboratory_transport_slot(ui,0,project_width,height);float cy=title.y+height/2;
        nk_push_scissor(canvas,nk_window_get_bounds(ui));
        struct nk_rect icon=nk_rect(title.x,cy-18*f,34*f,36*f);nk_fill_rect(canvas,icon,8*f,UI_COLOR(surface));
        float cx=icon.x+17*f,icy=icon.y+18*f;
        nk_stroke_line(canvas,cx,icy-7*f,cx+7*f,icy,1.5f*f,UI_COLOR(accent));nk_stroke_line(canvas,cx+7*f,icy,cx,icy+7*f,1.5f*f,UI_COLOR(accent));
        nk_stroke_line(canvas,cx,icy+7*f,cx-7*f,icy,1.5f*f,UI_COLOR(accent));nk_stroke_line(canvas,cx-7*f,icy,cx,icy-7*f,1.5f*f,UI_COLOR(accent));
        nk_fill_circle(canvas,nk_rect(cx-2*f,icy-2*f,4*f,4*f),UI_COLOR(accent));
        const char *context=a->analysis_only?"Aufgezeichnete Szene":a->runner.running?"Aktueller Messlauf":"Experiment";
        float title_height=a->laboratory_title_font.height+3*f+font->height,text_x=title.x+45*f,text_width=fmaxf(1,title.w-45*f);
        nk_draw_text(canvas,nk_rect(text_x,cy-title_height/2,text_width,a->laboratory_title_font.height),name,(int)strlen(name),&a->laboratory_title_font,nk_rgba(0,0,0,0),UI_COLOR(text));
        nk_draw_text(canvas,nk_rect(text_x,cy+title_height/2-font->height,text_width,font->height),context,(int)strlen(context),font,nk_rgba(0,0,0,0),UI_COLOR(muted));
        nk_push_scissor(canvas,clip);ps_ui_accessibility_label(ui,title,name);
        float x=project_width+project_gap;bool ready=runner_control_ready(a);
        struct nk_rect button=laboratory_transport_slot(ui,x,widths[0],height);a->run_control_bounds[0]=a->run_control_bounds[1]=button;
        nk_begin_disabled(ui,!a->loaded || a->analysis_only || (a->runner.running?!ready:!idle(a)));
        if(laboratory_transport_button(a,button,main_action,main_caption,true)){if(a->runner.running)command(a,a->paused?PS_MSG_RUN:PS_MSG_PAUSE);else if(a->built)start_run(a);else build_project(a);}nk_end_disabled(ui);x+=widths[0]+gap;
        button=laboratory_transport_slot(ui,x,widths[1],height);a->run_control_bounds[2]=button;nk_begin_disabled(ui,!ready || !a->paused);
        if(laboratory_transport_button(a,button,"Schritt",captions[1],false))command(a,PS_MSG_STEP);nk_end_disabled(ui);x+=widths[1]+gap;
        button=laboratory_transport_slot(ui,x,widths[2],height);a->run_control_bounds[4]=button;nk_begin_disabled(ui,!a->runner.running || a->stop_at!=0);
        if(laboratory_transport_button(a,button,"Stop",captions[2],false)){command(a,PS_MSG_STOP);a->stop_at=ps_clock();}nk_end_disabled(ui);x+=widths[2]+gap;
        nk_push_scissor(canvas,nk_window_get_bounds(ui));float divider_x=title.x+x+5*f;
        nk_stroke_line(canvas,divider_x,cy-10*f,divider_x,cy+10*f,f,UI_COLOR(border));nk_push_scissor(canvas,clip);x+=11*f+gap;
        button=laboratory_transport_slot(ui,x,widths[3],height);a->run_control_bounds[3]=button;nk_begin_disabled(ui,!reset_available(a));
        if(laboratory_transport_button(a,button,reset,reset_caption,false))reset_run(a);nk_end_disabled(ui);x+=widths[3]+gap+8*f;
        struct nk_rect label=laboratory_transport_slot(ui,x,tempo_width,height);
        nk_draw_text(canvas,nk_rect(label.x,label.y+(label.h-font->height)/2,label.w+1,font->height),"Tempo",5,font,nk_rgba(0,0,0,0),UI_COLOR(muted));x+=tempo_width+7*f;
        nk_layout_space_push(ui,nk_rect(x,0,speed_width,height));a->speed_bounds=nk_widget_bounds(ui);
        nk_begin_disabled(ui,a->reset_pending || a->reset_starting || (a->runner.running && (!a->hello || a->stop_at)));
        laboratory_speed_paint paint={a,speed};
        nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(6*f,6*f));
        nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(0,0));
        nk_style_push_flags(ui,&ui->style.button.text_alignment,NK_TEXT_LEFT);
        nk_style_push_float(ui,&ui->style.button.rounding,0);
        nk_style_push_style_item(ui,&ui->style.button.normal,nk_style_item_color(UI_COLOR(window)));
        if(ps_ui_combo_begin_named_painted(ui,"Simulationstempo auswählen",speed,nk_vec2(fmaxf(220*f,speed_width),8*30*f+14*f),laboratory_speed_field,&paint)) {
            const double speeds[]={.25,.5,1,2,4,8,16,0};
            ps_ui_row_exact_dynamic(ui,30*f,1);
            for(unsigned i=0;i<8;i++){char text[80];laboratory_speed_text(speeds[i],text,true);a->speed_choices[i]=nk_widget_bounds(ui);if(ps_ui_fixed_button_label(ui,text)){select_simulation_speed(a,speeds[i]);a->laboratory_speed_focus_return=true;nk_combo_close(ui);}}
            if(nk_input_is_key_pressed(&ui->input,NK_KEY_TEXT_RESET_MODE)){a->laboratory_speed_focus_return=true;nk_combo_close(ui);}
            nk_combo_end(ui);
        }
        nk_style_pop_style_item(ui);nk_style_pop_float(ui);nk_style_pop_flags(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);
        nk_end_disabled(ui);x+=speed_width;
        if(a->laboratory_speed_focus_return && nk_sdl_accessibility_request_focus(ui,NULL,"Simulationstempo auswählen"))a->laboratory_speed_focus_return=false;
        float status_x=fmaxf(x+gap,nk_window_get_content_region(ui).w-status_width);
        struct nk_rect pill=laboratory_transport_slot(ui,status_x,status_width,height);a->laboratory_transport_status=pill;
        nk_push_scissor(canvas,nk_window_get_bounds(ui));struct nk_rect outline=nk_rect(pill.x,cy-13*f,pill.w,26*f);
        nk_stroke_rect(canvas,outline,13*f,f,UI_COLOR(border));float state_width=laboratory_transport_text(font,state);
        nk_draw_text(canvas,nk_rect(pill.x+(pill.w-state_width)/2,cy-font->height/2,state_width+1,font->height),state,(int)strlen(state),font,nk_rgba(0,0,0,0),UI_COLOR(muted));nk_push_scissor(canvas,clip);
        ps_ui_accessibility_label(ui,pill,state);nk_layout_space_end(ui);
        workspace_rule(canvas,nk_rect(0,194,w,1));
    }nk_end(ui);nk_style_pop_vec2(ui);nk_style_pop_font(ui);
}
