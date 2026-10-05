static void timeline_controls(app *a) {
    struct nk_context *ui = a->ui;
    uint32_t count = ps_timeline_count(&a->timeline);
    uint32_t selected = a->timeline_browsing
                            ? ps_timeline_nearest(&a->timeline, a->timeline_view.time)
                            : count ? count - 1 : 0;
    bool ready = count && !a->reset_pending && !a->reset_starting;
    nk_layout_row_begin(ui, NK_DYNAMIC, 30, 5);
    nk_layout_row_push(ui, .06f);
    a->timeline_bounds[0] = nk_widget_bounds(ui);
    nk_begin_disabled(ui, !ready || !selected);
    if (nk_button_symbol(ui, NK_SYMBOL_TRIANGLE_LEFT)) {
        a->timeline_playing = false;
        timeline_select(a, selected - 1);
    }
    nk_end_disabled(ui);
    if (nk_input_is_mouse_hovering_rect(&ui->input, a->timeline_bounds[0]))
        nk_tooltip(ui, "Vorheriger aufgezeichneter Zustand");
    nk_layout_row_push(ui, .06f);
    a->timeline_bounds[1] = nk_widget_bounds(ui);
    nk_begin_disabled(ui, !ready || selected + 1 >= count);
    if (nk_button_symbol(ui, NK_SYMBOL_TRIANGLE_RIGHT)) {
        a->timeline_playing = false;
        timeline_select(a, selected + 1);
    }
    nk_end_disabled(ui);
    nk_layout_row_push(ui, .12f);
    a->timeline_bounds[2] = nk_widget_bounds(ui);
    nk_begin_disabled(ui, !ready);
    if (action_button(ui, a->runner.running ? "Live" : "Letzter", !a->timeline_browsing)) timeline_live(a);
    nk_layout_row_push(ui, .20f);
    a->timeline_bounds[3] = nk_widget_bounds(ui);
    nk_begin_disabled(ui, count < 2);
    if (nk_button_label(ui, a->timeline_playing ? "Anhalten" : "Abspielen"))
        timeline_toggle_play(a);
    nk_end_disabled(ui);
    nk_layout_row_push(ui, .56f);
    a->timeline_bounds[4] = nk_widget_bounds(ui);
    if (ready && count > 1 &&
        nk_input_is_mouse_hovering_rect(&ui->input, a->timeline_bounds[4]) &&
        nk_input_is_mouse_pressed(&ui->input, NK_BUTTON_LEFT)) {
        /* Clicking the track seeks directly; dragging the handle remains available. */
        float left = a->timeline_bounds[4].x + ui->style.slider.padding.x;
        float width = a->timeline_bounds[4].w - 2 * ui->style.slider.padding.x;
        float fraction = fmaxf(0, fminf(1, (ui->input.mouse.pos.x - left) / fmaxf(1, width)));
        selected = (uint32_t)lroundf(fraction * (count - 1));
        a->timeline_playing = false;
        timeline_select(a, selected);
    }
    float position = count > 1 ? (float)selected / (count - 1) : 0;
    if (nk_slider_float(ui, 0, &position, 1, count > 1 ? 1.f / (count - 1) : 1)) {
        a->timeline_playing = false;
        timeline_select(a, (uint32_t)lroundf(position * (count ? count - 1 : 0)));
    }
    nk_end_disabled(ui);
    if (nk_input_is_mouse_hovering_rect(&ui->input, a->timeline_bounds[4])) {
        char detail[240];
        if (a->timeline_error[0]) snprintf(detail, sizeof detail, "%s", a->timeline_error);
        else if (count) snprintf(detail, sizeof detail,
            "Zeitleiste: Zustand %u/%u · %.6g s · %llu aufgezeichnete Zeitpunkte%s",
            selected + 1, count, display_time(a), (unsigned long long)a->timeline.seen,
            a->timeline_scenes ? "" : " · Messwerte ohne Szene");
        else snprintf(detail, sizeof detail, "Zeitleiste erscheint nach dem ersten aufgezeichneten Zustand.");
        nk_tooltip(ui, detail);
    }
    nk_layout_row_end(ui);
}
