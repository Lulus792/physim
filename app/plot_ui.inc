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
static void plot_axis_label(char *text,size_t capacity,double value,double low,double high) {
    /* Round only the label at the precision of this axis, not the sample or
     * tick coordinate. Preserve small values on an equally small axis. */
    double half_span=fabs(high*.5-low*.5);
    if(fabs(value)<half_span*1e-4)value=0;
    snprintf(text,capacity,"%.4g",value);
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
/* Continue a logical-pixel dash phase across adjacent samples; gaps reset it.
 * The same solid/long/short roles are used by the canonical export figure. */
static void plot_pattern(struct nk_command_buffer *canvas,struct nk_rect area,double x,double y,double xx,double yy,
    double dash,double *phase,struct nk_color color) {
    if(!dash){plot_segment(canvas,area,x,y,xx,yy,color);return;}
    double length=hypot((xx-x)*area.w,(yy-y)*area.h),period=dash+6,old=*phase;
    if(!isfinite(length))return;
    *phase=fmod(old+fmod(length,period),period);
    double x0=x,y0=y,x1=xx,y1=yy;if(!ps_plot_clip_line(&x0,&y0,&x1,&y1))return;
    double clipped=hypot((x1-x0)*area.w,(y1-y0)*area.h);if(!clipped)return;
    double skipped=hypot((x0-x)*area.w,(y0-y)*area.h),at=0,pos=fmod(old+fmod(skipped,period),period);
    while(at<clipped) {
        bool ink=pos<dash;double amount=fmin(clipped-at,(ink?dash:period)-pos);if(amount<=0)return;
        if(ink){double a=at/clipped,b=(at+amount)/clipped;plot_segment(canvas,area,x0+(x1-x0)*a,y0+(y1-y0)*a,x0+(x1-x0)*b,y0+(y1-y0)*b,color);}
        at+=amount;pos=ink?dash:0;
    }
}
static void analysis_plot_legend(app *a,const ps_graphic_snapshot *snapshot) {
    const ps_report *report=ps_graphic_snapshot_report(snapshot);ps_graphic_description d;ps_graphic_snapshot_describe(snapshot,&d);
    const struct nk_user_font *font=&a->workbench_fonts[2];float available=nk_window_get_content_region(a->ui).w;
    float height=fmaxf(30,font->height+12),x=0;unsigned rows=1;char labels[8][160];float widths[8];
    for(uint32_t i=0;i<d.plot.curves;i++) {
        const ps_curve_data *c;ps_report_curve_view(report,0,i,&c);
        snprintf(labels[i],sizeof labels[i],"%s%s",c->label,i==d.reference?" · Referenz":"");
        widths[i]=fminf(available,32+font->width(font->userdata,font->height,labels[i],(int)strlen(labels[i])));
        if(x && x+widths[i]>available){rows++;x=0;}x+=widths[i]+14;
    }
    struct nk_rect r;nk_layout_row_dynamic(a->ui,height*rows,1);nk_widget(&r,a->ui);x=r.x;float y=r.y;
    for(uint32_t i=0;i<d.plot.curves;i++) {
        if(x>r.x && x+widths[i]>r.x+r.w){x=r.x;y+=height;}
        double dash=(i==d.reference || (d.reference==PS_GRAPHIC_NO_REFERENCE && !i))?0:(i%2?10:3),phase=0;
        plot_pattern(nk_window_get_canvas(a->ui),nk_rect(x,y+height/2,20,1),0,1,1,1,dash,&phase,ui_palette->curves[i]);
        analysis_text(a,nk_rect(x+26,y+(height-font->height)/2,widths[i]-26,font->height),labels[i],font,UI_COLOR(muted));
        x+=widths[i]+14;
    }
}
static void analysis_plot_heading(app *a,ps_plot_view *view,const double full[4],const ps_graphic_snapshot *snapshot) {
    struct nk_context *ui=a->ui;struct nk_rect r;
    const struct nk_user_font *small=&a->workspace_status_font,*title=&a->analysis_chart_font,*font=&a->workbench_fonts[0];
    float scale=a->preferences.ui_size/16.f;
    const char *labels[]={"+","−","Alles zeigen","Grafik exportieren…"},*names[]={"Zoom +","Zoom -","Alles zeigen","Grafik exportieren…"};
    float widths[]={28*scale,28*scale,0,0};
    for(unsigned i=2;i<4;i++)widths[i]=fmaxf((i==2?82:157)*scale,ceilf(font->width(font->userdata,font->height,labels[i],(int)strlen(labels[i]))+18*scale));
    unsigned actions=snapshot?4:3;float controls=4*scale*(actions-1);
    for(unsigned i=0;i<actions;i++)controls+=widths[i];
    const char *name=a->data.channels[a->plot_channel].name;
    float title_width=title->width(title->userdata,title->height,name,(int)strlen(name));
    float button_height=fmaxf(28*scale,font->height+12*scale),available=nk_window_get_content_region(ui).w;
    bool wrap=title_width+controls+14>available;unsigned tool_rows=1;float used=0;
    for(unsigned i=0;i<actions;i++){if(used && used+widths[i]>available){tool_rows++;used=0;}used+=widths[i]+4*scale;}
    float tool_top=wrap?44+title->height+12:34;
    float height=wrap?tool_top+button_height+12:76;
    height+=(tool_rows-1)*(button_height+8*scale);
    nk_layout_row_dynamic(ui,height,1);nk_widget(&r,ui);struct nk_command_buffer *canvas=nk_window_get_canvas(ui);
    nk_draw_text(canvas,nk_rect(r.x,r.y+22,r.w,small->height),"MESSKURVE",9,small,nk_rgba(0,0,0,0),UI_COLOR(muted));
    analysis_text(a,nk_rect(r.x,r.y+44,wrap?r.w:fmaxf(1,r.w-controls-14),title->height),name,title,UI_COLOR(text));
    float x=wrap?r.x:r.x+r.w-controls,y=r.y+tool_top;
    ps_plot_view before=*view;if(!view->initialized)ps_plot_view_reset(view);
    for(unsigned i=0;i<actions;i++) {
        if(x>r.x && x+widths[i]>r.x+r.w){x=r.x;y+=button_height+8*scale;}
        struct nk_rect button=nk_rect(x,y,fminf(widths[i],r.w),button_height);x+=button.w+4*scale;
        if(i<3)a->plot_buttons[i]=button;else a->result_export_bounds[2]=button;
        bool focused=nk_sdl_accessibility_is_focused(ui,NULL,names[i]);
        nk_stroke_rect(canvas,button,5*scale,focused?2.f:1.f,focused?UI_COLOR(accent):UI_COLOR(border));
        float text_width=font->width(font->userdata,font->height,labels[i],(int)strlen(labels[i]));
        nk_draw_text(canvas,nk_rect(button.x+(button.w-text_width)/2,button.y+(button.h-font->height)/2,text_width+1,font->height),labels[i],(int)strlen(labels[i]),font,nk_rgba(0,0,0,0),i==3?UI_COLOR(accent):UI_COLOR(text));
        nk_begin_disabled(ui,!full);
        if(ps_ui_button_at(ui,button,names[i],false)) {
            if(i==3)graphic_export_comparison(a);
            else {if(i==2)ps_plot_view_reset(view);else ps_plot_view_zoom(view,i==0?2:.5,.5,.5);a->plot_drag=NULL;}
        }
        nk_end_disabled(ui);
    }
    if(full){double shown[4];plot_display_bounds(view,full,shown);if(!(shown[0]<shown[1] && shown[2]<shown[3]))*view=before;}
    if(snapshot)analysis_plot_legend(a,snapshot);
}
