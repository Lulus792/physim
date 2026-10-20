static struct nk_rect plot_intersection(struct nk_rect a, struct nk_rect b) {
    float x = fmaxf(a.x, b.x), y = fmaxf(a.y, b.y);
    return nk_rect(x, y, fmaxf(0, fminf(a.x + a.w, b.x + b.w) - x),
                   fmaxf(0, fminf(a.y + a.h, b.y + b.h) - y));
}
static void plot_expand_range(double *low, double *high, bool pad) {
    double delta = *low == *high ? fmax(1, fabs(*low) * .05) : (pad ? *high * .08 - *low * .08 : 0);
    if (isfinite(*low - delta))
        *low -= delta;
    if (isfinite(*high + delta))
        *high += delta;
}
static void plot_display_bounds(const ps_plot_view *v, const double full[4], double b[4]) {
    for (unsigned axis = 0; axis < 2; axis++) {
        b[axis * 2] = ps_plot_axis_value(full[axis * 2], full[axis * 2 + 1], v->low[axis]);
        b[axis * 2 + 1] = ps_plot_axis_value(full[axis * 2], full[axis * 2 + 1], v->high[axis]);
    }
}
/* Suppress interpolation roundoff at a zero tick, never alter sample values. */
static double plot_tick(double low, double high, double fraction) {
    double value = ps_plot_axis_value(low, high, fraction);
    if (low <= 0 && high >= 0 && fabs(value) <= fmax(fabs(low), fabs(high)) * 1e-12)
        return 0;
    return value;
}
static void plot_controls(app *a, ps_plot_view *v, const double full[4]) {
    if (!v->initialized)
        ps_plot_view_reset(v);
    ps_plot_view before = *v;
    struct nk_context *ui = a->ui;
    bool large=a->preferences.ui_size>=20;
    nk_layout_row_begin(ui, NK_DYNAMIC, 26, 4);
    nk_layout_row_push(ui, large?.22f:.14f);
    a->plot_buttons[0] = nk_widget_bounds(ui);
    if (nk_button_label(ui, "Zoom +")) {
        ps_plot_view_zoom(v, 2, .5, .5);
        a->plot_drag = NULL;
    }
    nk_layout_row_push(ui, large?.22f:.14f);
    a->plot_buttons[1] = nk_widget_bounds(ui);
    if (nk_button_label(ui, "Zoom -")) {
        ps_plot_view_zoom(v, .5, .5, .5);
        a->plot_drag = NULL;
    }
    nk_layout_row_push(ui, large?.28f:.20f);
    a->plot_buttons[2] = nk_widget_bounds(ui);
    if (nk_button_label(ui, "Alles zeigen")) {
        ps_plot_view_reset(v);
        a->plot_drag = NULL;
    }
    nk_layout_row_push(ui, large?.28f:.52f);
    if(large) {
        nk_style_push_color(ui,&ui->style.text.color,UI_COLOR(muted));
        nk_label_wrap(ui,"Mausrad: Zoom · Ziehen: Verschieben");
        nk_style_pop_color(ui);
    } else nk_label_colored(ui, "Mausrad: Zoom · Ziehen: Verschieben", NK_TEXT_RIGHT,
                            UI_COLOR(muted));
    nk_layout_row_end(ui);
    if (full) {
        double b[4];
        plot_display_bounds(v, full, b);
        if (!(b[0] < b[1] && b[2] < b[3]))
            *v = before;
    }
}
static void plot_interact(app *a, ps_plot_view *v, struct nk_rect area, const double full[4]) {
    struct nk_context *ui = a->ui;
    a->plot_area = area;
    a->active_plot_view = v;
    if (!v->initialized)
        ps_plot_view_reset(v);
    if (!(area.w > 0 && area.h > 0))
        return;
    bool down = nk_input_is_mouse_down(&ui->input, NK_BUTTON_LEFT) != 0;
    if (!down)
        a->plot_drag = NULL;
    if (ui->current->flags & NK_WINDOW_ROM) {
        a->plot_drag = NULL;
        return;
    }
    struct nk_rect visible = plot_intersection(area, nk_window_get_canvas(ui)->clip);
    bool hover = nk_input_is_mouse_hovering_rect(&ui->input, visible) != 0;
    if (hover) {
        double wheel = ui->input.mouse.scroll_delta.y;
        if (isfinite(wheel) && wheel) {
            double x = fmin(1, fmax(0, (ui->input.mouse.pos.x - area.x) / area.w));
            double y = fmin(1, fmax(0, 1 - (ui->input.mouse.pos.y - area.y) / area.h));
            ps_plot_view before = *v;
            ps_plot_view_zoom(v, pow(1.25, fmin(8, fmax(-8, wheel))), x, y);
            double b[4];
            plot_display_bounds(v, full, b);
            if (!(b[0] < b[1] && b[2] < b[3]))
                *v = before;
            ui->input.mouse.scroll_delta = nk_vec2(0, 0);
        }
        if (nk_input_is_mouse_pressed(&ui->input, NK_BUTTON_LEFT) &&
            nk_input_has_mouse_click_in_rect(&ui->input, NK_BUTTON_LEFT, visible)) {
            a->plot_drag = v;
            a->plot_drag_position = ui->input.mouse.pos;
        }
    }
    if (a->plot_drag == v && down) {
        ps_plot_view_pan(v, -(ui->input.mouse.pos.x - a->plot_drag_position.x) / area.w,
                         (ui->input.mouse.pos.y - a->plot_drag_position.y) / area.h);
        a->plot_drag_position = ui->input.mouse.pos;
    }
}
static double plot_fraction(const ps_plot_view *v, const double full[4], unsigned axis,
                            double value) {
    return (ps_report_axis_fraction(value, full[axis * 2], full[axis * 2 + 1]) - v->low[axis]) /
           (v->high[axis] - v->low[axis]);
}
static void plot_segment(struct nk_command_buffer *canvas, struct nk_rect area, double x0,
                         double y0, double x1, double y1, struct nk_color color) {
    if (ps_plot_clip_line(&x0, &y0, &x1, &y1))
        nk_stroke_line(canvas, area.x + (float)x0 * area.w, area.y + (1 - (float)y0) * area.h,
                       area.x + (float)x1 * area.w, area.y + (1 - (float)y1) * area.h, 1.8f, color);
}
static void plot_dot(struct nk_command_buffer *canvas, struct nk_rect area, double x, double y,
                     float radius, struct nk_color color) {
    if (x >= 0 && x <= 1 && y >= 0 && y <= 1)
        nk_fill_circle(canvas,
                       nk_rect(area.x + (float)x * area.w - radius,
                               area.y + (1 - (float)y) * area.h - radius, 2 * radius, 2 * radius),
                       color);
}
static void analysis_plot_heading(app *a,ps_plot_view *view,const double full[4]) {
    struct nk_context *ui=a->ui;struct nk_rect r;
    nk_layout_row_dynamic(ui,76,1);nk_widget(&r,ui);
    struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    const struct nk_user_font *small=&a->workspace_status_font,*title=&a->analysis_chart_font;
    nk_draw_text(canvas,nk_rect(r.x,r.y+22,r.w,small->height),"MESSKURVE",9,small,nk_rgba(0,0,0,0),UI_COLOR(muted));
    const char *name=a->data.channels[a->plot_channel].name;
    const struct nk_user_font *control_font=&a->workbench_fonts[0];float scale=a->preferences.ui_size/16.f;
    const char *labels[]={"+","−","Alles zeigen"},*names[]={"Zoom +","Zoom -","Alles zeigen"};
    float widths[]={28*scale,28*scale,fmaxf(82*scale,ceilf(control_font->width(control_font->userdata,control_font->height,labels[2],(int)strlen(labels[2]))+18*scale))};
    float controls=widths[0]+widths[1]+widths[2]+8;
    nk_draw_text(canvas,nk_rect(r.x,r.y+44,fmaxf(1,r.w-controls-14),title->height),name,(int)strlen(name),title,nk_rgba(0,0,0,0),UI_COLOR(text));
    char description[192];snprintf(description,sizeof description,"Messkurve: %s",name);ps_ui_accessibility_label(ui,r,description);
    float x=r.x+r.w-controls;
    ps_plot_view before=*view;if(!view->initialized)ps_plot_view_reset(view);
    for(unsigned i=0;i<3;i++) {
        struct nk_rect button=nk_rect(x,r.y+34,widths[i],fmaxf(28,control_font->height+12));a->plot_buttons[i]=button;x+=widths[i]+4;
        bool focused=nk_sdl_accessibility_is_focused(ui,NULL,names[i]);
        nk_stroke_rect(canvas,button,5,focused?2:1,focused?UI_COLOR(accent):UI_COLOR(border));
        const struct nk_user_font *font=&a->workbench_fonts[0];
        float width=font->width(font->userdata,font->height,labels[i],(int)strlen(labels[i]));
        nk_draw_text(canvas,nk_rect(button.x+(button.w-width)/2,button.y+(button.h-font->height)/2,width+1,font->height),labels[i],(int)strlen(labels[i]),font,nk_rgba(0,0,0,0),UI_COLOR(text));
        if(ps_ui_button_at(ui,button,names[i],false)) {
            if(i==2)ps_plot_view_reset(view);else ps_plot_view_zoom(view,i==0?2:.5,.5,.5);
            a->plot_drag=NULL;
        }
    }
    if(full){double shown[4];plot_display_bounds(view,full,shown);if(!(shown[0]<shown[1] && shown[2]<shown[3]))*view=before;}
    nk_layout_row_dynamic(ui,30,1);nk_widget(&r,ui);
    nk_stroke_line(canvas,r.x,r.y+14,r.x+20,r.y+14,2,ui_palette->curves[0]);
    char legend[1200];snprintf(legend,sizeof legend,"%s%s",diagnostic_basename(a->data.path),selected_run(a,a->data.path)==0?" · Referenz":" · Aktiver Lauf");
    const struct nk_user_font *legend_font=&a->workbench_fonts[2];
    nk_draw_text(canvas,nk_rect(r.x+26,r.y+7,r.w-26,legend_font->height),legend,(int)strlen(legend),legend_font,nk_rgba(0,0,0,0),UI_COLOR(muted));
    ps_ui_accessibility_label(ui,r,legend);
}
