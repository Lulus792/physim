/* Selected inspector B: flat camera fields, actual values and shared view state. */
static const char *laboratory_camera_names[2][2]={{"Blickrichtung im Szenenbaum","2D-Ebene im Szenenbaum"},{"Blickrichtung im Inspektor","2D-Ebene im Inspektor"}};
static void laboratory_camera_space(app *a,float pixels) {ps_ui_row_exact_dynamic(a->ui,pixels*a->preferences.ui_size/16.f,1);nk_spacer(a->ui);}
static struct nk_color laboratory_camera_color(app *a,struct nk_color color) {if(a->ui->current->widgets_disabled)color.a=(nk_byte)lroundf(color.a*.45f);return color;}
static void laboratory_camera_focus(app *a,struct nk_rect r,float radius) {
    laboratory_focus_outline(a,r,radius);
}
static void laboratory_camera_label(app *a,const char *label,float margin) {
    if(margin)laboratory_camera_space(a,margin);
    float f=a->preferences.ui_size/16.f;struct nk_context *ui=a->ui;const struct nk_user_font *font=&a->laboratory_caption_font;
    float h=15.95f*f*laboratory_wrapped_lines(font,label,nk_window_get_content_region(ui).w);
    ps_ui_row_exact_dynamic(ui,h,1);struct nk_rect r;if(nk_widget(&r,ui)!=NK_WIDGET_INVALID){laboratory_wrapped_text(nk_window_get_canvas(ui),font,label,r,15.95f*f,UI_COLOR(text));ps_ui_accessibility_label(ui,r,label);}
    laboratory_camera_space(a,5);
}
static int laboratory_camera_direction(const app *a) {
    if(!a->camera_2d && fabsf(a->yaw-PS_CAMERA_DEFAULT.yaw)<.0001f && fabsf(a->pitch-PS_CAMERA_DEFAULT.pitch)<.0001f)return 0;
    if(fabsf(a->yaw)<.0001f && fabsf(a->pitch)<.0001f)return 1;
    if(fabsf(a->yaw-(float)PS_PI/2)<.0001f && fabsf(a->pitch)<.0001f)return 2;
    if(fabsf(a->yaw)<.0001f && fabsf(a->pitch-(float)PS_PI/2)<.0001f)return 3;
    return -1;
}
static void laboratory_camera_preset(app *a,unsigned choice) {
    if(!choice){a->camera_2d=false;a->yaw=PS_CAMERA_DEFAULT.yaw;a->pitch=PS_CAMERA_DEFAULT.pitch;a->zoom=PS_CAMERA_DEFAULT.distance;a->camera_target=PS_CAMERA_DEFAULT.target;a->orthographic=PS_CAMERA_DEFAULT.orthographic;}
    else {scene_camera_plane(a,choice-1);a->orthographic=true;}
}
typedef struct {app *a;const char *label;} laboratory_camera_paint;
static void laboratory_camera_field(struct nk_context *ui,struct nk_rect r,bool focused,void *user) {
    laboratory_camera_paint *paint=user;app *a=paint->a;float f=a->preferences.ui_size/16.f;
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);const struct nk_user_font *font=&a->laboratory_control_font;
    struct nk_color color=laboratory_camera_color(a,UI_COLOR(text));
    nk_fill_rect(canvas,r,5*f,laboratory_camera_color(a,UI_COLOR(window)));
    nk_stroke_rect(canvas,nk_rect(r.x+f/2,r.y+f/2,r.w-f,r.h-f),4.5f*f,f,laboratory_camera_color(a,UI_COLOR(border)));
    float line=18.85f*f,width=fmaxf(1,r.w-38*f),h=line*laboratory_wrapped_lines(font,paint->label,width);
    laboratory_wrapped_text(canvas,font,paint->label,nk_rect(r.x+8*f,r.y+(r.h-h)/2,width,h),line,color);
    float x=r.x+r.w-16*f,y=r.y+r.h/2;nk_stroke_line(canvas,x-4*f,y-2*f,x,y+2*f,1.25f*f,color);nk_stroke_line(canvas,x,y+2*f,x+4*f,y-2*f,1.25f*f,color);
    if(focused)laboratory_camera_focus(a,r,9);
}
static void laboratory_camera_select(app *a,unsigned side,unsigned kind) {
    struct nk_context *ui=a->ui;float f=a->preferences.ui_size/16.f;
    const char *views[]={"Standard","Vorne","Seite","Oben"},*planes[]={"XY","YZ","XZ"};int selected=kind?(int)a->camera_plane:laboratory_camera_direction(a);
    const char *text=selected<0?"Eigene Ansicht":kind?planes[selected]:views[selected];unsigned count=kind?3:4;
    float content_width=fmaxf(1,nk_window_get_content_region(ui).w-38*f);
    float lines=(float)laboratory_wrapped_lines(&a->laboratory_control_font,text,content_width);
    /* A live preset-to-free transition must not move the slider being dragged.
     * Reserve enough text rows for every caption at this actual pane width. */
    for(unsigned i=0;i<count;i++)lines=fmaxf(lines,(float)laboratory_wrapped_lines(&a->laboratory_control_font,kind?planes[i]:views[i],content_width));
    if(!kind)lines=fmaxf(lines,(float)laboratory_wrapped_lines(&a->laboratory_control_font,"Eigene Ansicht",content_width));
    ps_ui_row_exact_dynamic(ui,lines<=1?32*f:16*f+lines*18.85f*f,1);
    struct nk_rect r=nk_widget_bounds(ui);a->laboratory_camera_fields[side][kind]=r;
    if(kind)a->camera_plane_bounds=r;else a->camera_view_bounds=r;
    memset(a->laboratory_camera_choices[side][kind],0,sizeof a->laboratory_camera_choices[side][kind]);
    a->laboratory_camera_open[side][kind]=false;
    nk_begin_disabled(ui,kind && !a->camera_2d);
    nk_style_push_font(ui,&a->laboratory_control_font);nk_style_push_vec2(ui,&ui->style.window.padding,nk_vec2(6*f,6*f));nk_style_push_vec2(ui,&ui->style.window.spacing,nk_vec2(0,0));
    nk_style_push_flags(ui,&ui->style.button.text_alignment,NK_TEXT_LEFT);nk_style_push_float(ui,&ui->style.button.rounding,0);
    nk_style_push_style_item(ui,&ui->style.button.normal,nk_style_item_color(UI_COLOR(window)));
    laboratory_camera_paint paint={a,text};
    if(ps_ui_combo_begin_named_painted(ui,laboratory_camera_names[side][kind],text,nk_vec2(fmaxf(r.w,120*f),(count*30+12)*f),laboratory_camera_field,&paint)) {
        /* A global 2D shortcut can disable an already open plane selector. */
        bool enabled=!kind || a->camera_2d;
        a->laboratory_camera_open[side][kind]=enabled;ps_ui_row_exact_dynamic(ui,30*f,1);
        if(!enabled)nk_combo_close(ui);
        for(unsigned i=0;enabled && i<count;i++) {
            struct nk_rect option=nk_widget_bounds(ui);a->laboratory_camera_choices[side][kind][i]=option;if(!kind)a->camera_preset_bounds[i]=option;
            if(ps_ui_fixed_button_label(ui,kind?planes[i]:views[i])) {
                if(kind)scene_camera_plane(a,i);else laboratory_camera_preset(a,i);
                a->laboratory_camera_return[side][kind]=true;nk_combo_close(ui);
            }
        }
        if(enabled && nk_input_is_key_pressed(&ui->input,NK_KEY_TEXT_RESET_MODE)){a->laboratory_camera_return[side][kind]=true;nk_combo_close(ui);}
        nk_combo_end(ui);
    }
    nk_style_pop_style_item(ui);nk_style_pop_float(ui);nk_style_pop_flags(ui);nk_style_pop_vec2(ui);nk_style_pop_vec2(ui);nk_style_pop_font(ui);nk_end_disabled(ui);
    if(kind && !a->camera_2d)a->laboratory_camera_return[side][kind]=false;
    if(a->laboratory_camera_return[side][kind] && nk_sdl_accessibility_request_focus(ui,NULL,laboratory_camera_names[side][kind]))a->laboratory_camera_return[side][kind]=false;
}
static void laboratory_camera_check(app *a,const char *name,bool *value,struct nk_rect *bounds) {laboratory_checkbox(a,name,value,0,bounds);laboratory_camera_space(a,3);}
static void laboratory_camera_slider(app *a,unsigned side,unsigned index,const char *label,const char *name,float minimum,float *value,float maximum,float step,bool disabled) {
    struct nk_context *ui=a->ui;float f=a->preferences.ui_size/16.f;const struct nk_user_font *font=&a->laboratory_caption_font,*small=&a->workspace_status_font;
    char text[48];snprintf(text,sizeof text,index==2?"%.1f m":"%.2f rad",*value);for(char *p=text;*p;p++)if(*p=='.')*p=',';
    laboratory_camera_space(a,10);float width=nk_window_get_content_region(ui).w,number=small->width(small->userdata,small->height,text,(int)strlen(text));
    bool stacked=font->width(font->userdata,font->height,label,(int)strlen(label))+8*f+number>width;
    ps_ui_row_exact_dynamic(ui,(stacked?30.45f:15.95f)*f,1);struct nk_rect row;if(nk_widget(&row,ui)!=NK_WIDGET_INVALID) {
        struct nk_command_buffer *canvas=nk_window_get_canvas(ui);laboratory_wrapped_text(canvas,font,label,nk_rect(row.x,row.y,row.w,15.95f*f),15.95f*f,UI_COLOR(text));
        struct nk_rect output=nk_rect(stacked?row.x:row.x+row.w-number,row.y+(stacked?15.95f*f:0),number,14.5f*f);a->laboratory_camera_outputs[side][index]=output;
        struct nk_rect ink=output;ink.w+=1;nk_draw_text(canvas,ink,text,(int)strlen(text),small,nk_rgba(0,0,0,0),muted());ps_ui_accessibility_label(ui,row,text);
    }
    laboratory_camera_space(a,3);ps_ui_row_exact_dynamic(ui,16*f,1);a->laboratory_camera_sliders[side][index]=nk_widget_bounds(ui);
    /* Nuklear centers its endpoint cursor on the track edge. Reserve half a
     * 16-pixel thumb inside the input, as in the approved native timeline. */
    struct nk_style_slider saved=ui->style.slider;ui->style.slider.padding=nk_vec2(8*f,0);ui->style.slider.spacing=nk_vec2(0,0);ui->style.slider.border=0;ui->style.slider.rounding=2*f;ui->style.slider.bar_height=4*f;ui->style.slider.cursor_size=nk_vec2(16*f,16*f);ui->style.slider.show_buttons=false;
    ui->style.slider.normal=ui->style.slider.hover=ui->style.slider.active=nk_style_item_color(nk_rgba(0,0,0,0));
    ui->style.slider.bar_normal=ui->style.slider.bar_hover=ui->style.slider.bar_active=UI_COLOR(border);ui->style.slider.bar_filled=accent();
    ui->style.slider.cursor_normal=ui->style.slider.cursor_hover=ui->style.slider.cursor_active=nk_style_item_color(accent());
    bool focused=false;nk_begin_disabled(ui,disabled);if(disabled)ui->style.slider.color_factor=.45f;
    /* HTML range controls also jump on a track press. Nuklear's built-in
     * behavior only starts a drag from the existing thumb. Keep that drag
     * after positioning the thumb at the actual quantized click value. */
    struct nk_rect hit=a->laboratory_camera_sliders[side][index];
    if(!ui->current->widgets_disabled && !(ui->current->layout->flags&NK_WINDOW_ROM) &&
       nk_input_is_mouse_pressed(&ui->input,NK_BUTTON_LEFT) &&
       nk_input_is_mouse_hovering_rect(&ui->input,plot_intersection(hit,ui->current->layout->clip))) {
        float fraction=(ui->input.mouse.pos.x-hit.x-8*f)/fmaxf(1,hit.w-16*f);
        *value=fmaxf(minimum,fminf(maximum,minimum+roundf(fraction*(maximum-minimum)/step)*step));
    }
    ps_ui_slider_named_custom_focus(ui,name,minimum,value,maximum,step,&focused);nk_end_disabled(ui);ui->style.slider=saved;
    if(focused)laboratory_camera_focus(a,a->laboratory_camera_sliders[side][index],0);
    laboratory_camera_space(a,6);
}
static void laboratory_camera_save(app *a) {
    struct nk_context *ui=a->ui;float f=a->preferences.ui_size/16.f;const struct nk_user_font *font=&a->workbench_fonts[2];const char *text="Als Projektvorgabe speichern";
    laboratory_camera_space(a,10);float width=nk_window_get_content_region(ui).w,line=17.4f*f;unsigned lines=laboratory_wrapped_lines(font,text,fmaxf(1,width-18*f));
    ps_ui_row_exact_dynamic(ui,18*f+line*lines,1);struct nk_rect r;enum nk_widget_layout_states state=nk_widget(&r,ui);a->display_save_bounds=r;
    if(state==NK_WIDGET_INVALID)return;
    nk_begin_disabled(ui,!a->loaded || a->display_save_thread || a->recovery);bool focused=false;
    if(ps_ui_button_at_custom_focus(ui,plot_intersection(r,ui->current->layout->clip),text,false,&focused))project_display_save(a);
    bool hover=!ui->current->widgets_disabled && !(ui->current->layout->flags&NK_WINDOW_ROM) && nk_input_is_mouse_hovering_rect(&ui->input,plot_intersection(r,ui->current->layout->clip));
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);nk_fill_rect(canvas,r,6*f,laboratory_camera_color(a,hover?UI_COLOR(selection_active):UI_COLOR(surface)));
    if(a->preferences.theme==PS_THEME_HIGH_CONTRAST)nk_stroke_rect(canvas,nk_rect(r.x+f/2,r.y+f/2,r.w-f,r.h-f),5.5f*f,f,laboratory_camera_color(a,UI_COLOR(border)));
    /* Preserve full wrapped words using the same line breaker as the fields. */
    float y=r.y+9*f;
    for(int at=0,length=(int)strlen(text);at<length;) {
        int count=ps_ui_text_line(font,text+at,length-at,fmaxf(1,width-18*f),false);
        if(count<=0){nk_rune rune;count=nk_utf_decode(text+at,&rune,length-at);}if(count<=0)break;
        float w=font->width(font->userdata,font->height,text+at,count);
        nk_draw_text(canvas,nk_rect(r.x+(r.w-w)/2,y+(line-font->height)/2,w+1,font->height),text+at,count,font,nk_rgba(0,0,0,0),laboratory_camera_color(a,UI_COLOR(text)));
        at+=count;while(at<length && text[at]==' ')at++;y+=line;
    }
    if(focused)laboratory_camera_focus(a,r,10);
    nk_end_disabled(ui);
}
static void laboratory_camera(app *a,bool compact) {
    struct nk_context *ui=a->ui;unsigned side=compact?0:1;a->laboratory_camera_side=side;
    laboratory_camera_label(a,"Blickrichtung",11);laboratory_camera_select(a,side,0);laboratory_camera_space(a,12);
    bool planar=a->camera_2d;if(laboratory_checkbox(a,"2D-Modus",&planar,0,&a->camera_mode_bounds))scene_camera_2d(a,planar);laboratory_camera_space(a,3);
    laboratory_camera_check(a,"Orthografische Ansicht",&a->orthographic,&a->camera_projection_bounds);
    laboratory_camera_label(a,"2D-Ebene",0);laboratory_camera_select(a,side,1);
    laboratory_camera_slider(a,side,0,"Orbit","Kamera seitlich (rad)",-3.14f,&a->yaw,3.14f,.02f,a->camera_2d);
    laboratory_camera_slider(a,side,1,"Neigung","Kamera hoch (rad)",-(float)PS_PI/2,&a->pitch,(float)PS_PI/2,.02f,a->camera_2d);
    laboratory_camera_slider(a,side,2,"Zoom","Kameraabstand (m)",1.5f,&a->zoom,12,.1f,false);
    laboratory_camera_space(a,14);ps_ui_row_exact_dynamic(ui,1,1);struct nk_rect line;if(nk_widget(&line,ui)!=NK_WIDGET_INVALID)nk_fill_rect(nk_window_get_canvas(ui),line,0,UI_COLOR(border));laboratory_camera_space(a,14);
    if(!compact){laboratory_camera_check(a,"Vektoren",&a->show_vectors,&a->display_vector_bounds);laboratory_camera_check(a,"Flugbahnen",&a->show_paths,&a->display_path_bounds);}
    laboratory_camera_check(a,"Punkte",&a->show_points,&a->display_points_bounds);laboratory_camera_check(a,"Beschriftungen",&a->show_labels,&a->display_labels_bounds);
    if(!compact)laboratory_camera_check(a,"Raster und Achsen",&a->show_grid,NULL);
    laboratory_camera_save(a);
    laboratory_note(a,a->camera_2d?"Rechts/Mitte ziehen: verschieben · Mausrad: zoomen":"Rechts ziehen: drehen · Mitte ziehen: verschieben · Mausrad: zoomen");
    laboratory_note(a,a->camera_2d?"Alt + Pfeile: verschieben. Alt + 2: 2D/3D. Alt + Bild auf/ab: zoomen. Alt + Pos1: Standard.":"Alt + Pfeile: drehen. Alt + Shift + Pfeile: verschieben. Alt + 2: 2D/3D. Alt + Bild auf/ab: zoomen. Alt + Pos1: Standard.");
}
