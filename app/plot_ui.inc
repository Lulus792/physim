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
    nk_layout_row_begin(ui, NK_DYNAMIC, 26, 4);
    nk_layout_row_push(ui, .14f);
    a->plot_buttons[0] = nk_widget_bounds(ui);
    if (nk_button_label(ui, "Zoom +")) {
        ps_plot_view_zoom(v, 2, .5, .5);
        a->plot_drag = NULL;
    }
    nk_layout_row_push(ui, .14f);
    a->plot_buttons[1] = nk_widget_bounds(ui);
    if (nk_button_label(ui, "Zoom -")) {
        ps_plot_view_zoom(v, .5, .5, .5);
        a->plot_drag = NULL;
    }
    nk_layout_row_push(ui, .20f);
    a->plot_buttons[2] = nk_widget_bounds(ui);
    if (nk_button_label(ui, "Alles zeigen")) {
        ps_plot_view_reset(v);
        a->plot_drag = NULL;
    }
    nk_layout_row_push(ui, .52f);
    nk_label_colored(ui, "Mausrad: Zoom · Ziehen: Verschieben", NK_TEXT_RIGHT,
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
