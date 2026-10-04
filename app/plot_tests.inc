/* Exercises the real mouse-event path and rendered report/raw-data controls. */
static bool test_plot_png_size(const char *path, unsigned width, unsigned height) {
    size_t bytes = 0;
    unsigned char *data = SDL_LoadFile(path, &bytes);
    bool ok = data && bytes >= 33 && !memcmp(data, "\211PNG\r\n\032\n", 8);
    if (ok) {
        unsigned w = (unsigned)data[16] << 24 | (unsigned)data[17] << 16 |
                     (unsigned)data[18] << 8 | data[19];
        unsigned h = (unsigned)data[20] << 24 | (unsigned)data[21] << 16 |
                     (unsigned)data[22] << 8 | data[23];
        ok = w == width && h == height;
    }
    SDL_free(data);
    return ok;
}
static bool test_plot_region_images(app *a) {
    ps_report *report = NULL;
    ps_curve_data *curve = calloc(1, sizeof *curve);
    if (!curve) return false;
    bool ok = ps_report_create("Ausschnitt", "Referenz", &report) == PS_OK;
    ps_plot_info info = {0};
    strcpy(info.title, "Ausschnittreferenz");
    strcpy(info.x_label, "X");
    strcpy(info.y_label, "Y");
    ps_report_unit_from(PS_METRE, &info.x_unit);
    ps_report_unit_from(PS_METRE, &info.y_unit);
    const double region[] = {0, 1, 0, 1};
    for (unsigned i = 0; ok && i < 3; i++) {
        ps_plot_handle plot;
        ps_report_unit_from(i == 1 ? PS_ONE : PS_METRE, &info.y_unit);
        ok = ps_report_add_plot(report, &info, &plot) == PS_OK;
        strcpy(curve->label, "Kurve");
        curve->kind = i == 0 ? PS_PLOT_LINE : i == 1 ? PS_PLOT_HISTOGRAM : PS_PLOT_SCATTER;
        curve->count = i == 1 ? 1 : 2;
        curve->source_count = curve->count;
        curve->x[0] = i == 1 ? .5 : -1;
        curve->x[1] = 2;
        curve->y[0] = i == 1 ? 2 : .5;
        curve->y[1] = .5;
        curve->bar_width = i == 1 ? 2 : 0;
        ok = ok && ps_report_add_curve(report, plot, curve) == PS_OK;
        char path[4096], name[64];
        snprintf(name, sizeof name, "figure-region-%u.png", i);
        join(path, sizeof path, a->project, name);
        ok = ok && ps_report_image_region(a->ui, a->font_ui, report, i, 1, region, path) == PS_OK;
        snprintf(name, sizeof name, "figure-region-%u.svg", i);
        join(path, sizeof path, a->project, name);
        ok = ok && ps_report_export_svg_region(report, i, region, path) == PS_OK;
    }
    char path[4096];
    join(path, sizeof path, a->project, "invalid-region.png");
    const double invalid[][4] = {{0, 0, 0, 1}, {1, 0, 0, 1}, {0, 1, NAN, 1}};
    for (unsigned i = 0; ok && i < 3; i++)
        ok = ps_report_image_region(a->ui, a->font_ui, report, 0, 1, invalid[i], path) == PS_INVALID;
    ok = ok && !exists(path);
    ps_report_destroy(report);
    free(curve);
    return ok;
}
static bool test_plot_scaled_images(app *a) {
    for (unsigned scale = 1; scale <= 4; scale++) {
        char path[4096], name[64];
        snprintf(name, sizeof name, "figure-scale-%u.png", scale);
        join(path, sizeof path, a->project, name);
        if (ps_report_image_scaled(a->ui, a->font_ui, a->analysis_report, 3, scale, path) != PS_OK ||
            !test_plot_png_size(path, 1200 * scale, 850 * scale)) return false;
    }
    char path[4096];
    join(path, sizeof path, a->project, "invalid-scale.png");
    return ps_report_image_scaled(a->ui, a->font_ui, a->analysis_report, 0, 0, path) == PS_INVALID &&
           ps_report_image_scaled(a->ui, a->font_ui, a->analysis_report, 0, 5, path) == PS_INVALID &&
           nk_sdl_export_png_scaled(a->ui, a->ui, 4096, 1, 4, path) == PS_LIMIT && !exists(path);
}
static bool test_plot_dense_image(app *a) {
    ps_report *report = NULL;
    ps_curve_data *curve = calloc(1, sizeof *curve);
    if (!curve)
        return false;
    bool ok = ps_report_create("Exportreferenz", "Dichter Plot", &report) == PS_OK;
    ps_plot_info info = {0};
    strcpy(info.title,
           "Acht Messreihen · 16384 Punkte · vollständige Legende und reduzierte Berichtsdaten");
    strcpy(info.x_label, "Zeit");
    strcpy(info.y_label, "Position");
    ps_report_unit_from(PS_SECOND, &info.x_unit);
    ps_report_unit_from(PS_METRE, &info.y_unit);
    ps_plot_handle plot = {0};
    if (ok)
        ok = ps_report_add_plot(report, &info, &plot) == PS_OK;
    for (unsigned c = 0; ok && c < 8; c++) {
        snprintf(curve->label, sizeof curve->label,
                 "Messreihe %u: Vergleich der Position bei unterschiedlichen Anfangsbedingungen",
                 c + 1);
        curve->kind = PS_PLOT_SCATTER;
        curve->count = PS_REPORT_MAX_POINTS;
        curve->source_count = 4096;
        for (unsigned i = 0; i < curve->count; i++) {
            curve->x[i] = (double)i / (curve->count - 1);
            curve->y[i] = c + .2 * sin(i * .01);
        }
        ok = ps_report_add_curve(report, plot, curve) == PS_OK;
    }
    char path[4096];
    join(path, sizeof path, a->project, "figure-dense.png");
    if (ok)
        ok = ps_report_image(a->ui, a->font_ui, report, 0, path) == PS_OK;
    ps_report_destroy(report);
    free(curve);
    return ok;
}
static bool test_plot_setup(app *a, const char *directory) {
    snprintf(a->project, sizeof a->project, "%s", directory);
    if (!ps_make_directory_exclusive(directory))
        return false;
    char path[4096];
    join(path, sizeof path, directory, "runs");
    if (!ps_make_directory(path))
        return false;
    if (ps_report_create("Diagrammausschnitte", "Deterministische Bedienreferenz",
                         &a->analysis_report) != PS_OK)
        return false;
    ps_curve_data *curve = calloc(1, sizeof *curve);
    if (!curve)
        return false;
    bool ok = true;
    for (unsigned kind = 1; kind <= 4 && ok; kind++) {
        ps_plot_info plot = {0};
        ps_plot_handle h;
        snprintf(plot.title, sizeof plot.title, "%s",
                 kind == 1   ? "Linie"
                 : kind == 2 ? "Messpunkte"
                 : kind == 3 ? "Histogramm"
                             : "Kleine Abweichungen mit großem Offset");
        strcpy(plot.x_label, "Zeit");
        strcpy(plot.y_label, kind == 3 ? "Anzahl" : "Position");
        ps_report_unit_from(PS_SECOND, &plot.x_unit);
        ps_report_unit_from(kind == 3 ? PS_ONE : PS_METRE, &plot.y_unit);
        ok = ps_report_add_plot(a->analysis_report, &plot, &h) == PS_OK;
        strcpy(curve->label, "Vollständige Messreihe");
        curve->kind = kind == 4 ? PS_PLOT_LINE : (ps_plot_kind)kind;
        curve->count = kind == 3 ? 5 : 21;
        curve->source_count = kind == 3 ? 15 : 21;
        curve->bar_width = kind == 3 ? 1 : 0;
        for (unsigned i = 0; i < curve->count; i++) {
            curve->x[i] = kind == 3 ? i : i * .2;
            curve->y[i] = kind == 3 ? (double)i + 1 : 2 * sin(i * .6);
            if (kind == 4) {
                curve->x[i] = 1e6 + i * .001;
                curve->y[i] = 1e9 + sin(i * .6) * .001;
            }
        }
        if (ok)
            ok = ps_report_add_curve(a->analysis_report, h, curve) == PS_OK;
    }
    free(curve);
    join(path, sizeof path, directory, "full.csv");
    if (ok)
        ok = ps_report_export_plot_csv(a->analysis_report, 2, path) == PS_OK;
    join(path, sizeof path, directory, "fixture.psreport");
    if (ok)
        ok = ps_report_save(a->analysis_report, path) == PS_OK;
    ps_context c = {0};
    c.dt_s = 1;
    ps_channel_add(&c, "position.x", PS_METRE, "Model");
    ps_channel_add(&c, "sensor.x", PS_METRE, "Measurement");
    ps_channel_add(&c, "sensor.x.status", PS_ONE, "Validity");
    join(a->last_run, sizeof a->last_run, directory, "runs/fixture.psrun");
    ps_run_writer writer;
    if (ok)
        ok = ps_run_create(&writer, a->last_run, &c, "Plot test") == PS_OK;
    if (ok) {
        for (unsigned i = 0; i < 5; i++) {
            double values[] = {(double)i - 2, i == 2 ? 0 : (double)i - 2, i == 2 ? 2 : 1};
            if (ps_run_append(&writer, i, values) != PS_OK)
                ok = false;
        }
        if (ps_run_close(&writer) != PS_OK)
            ok = false;
    }
    a->loaded = true;
    a->tab = 2;
    a->show_report = true;
    request_dataset(a);
    return ok;
}
static void test_pointer_noise(app *a) {
    SDL_Event e = {0};
    e.type = SDL_EVENT_MOUSE_MOTION;
    e.motion.windowID = SDL_GetWindowID(a->window);
    e.motion.x = 1;
    e.motion.y = 1;
    SDL_PushEvent(&e);
    e.type = SDL_EVENT_MOUSE_BUTTON_UP;
    e.button.button = SDL_BUTTON_LEFT;
    e.button.down = false;
    SDL_PushEvent(&e);
    e.type = SDL_EVENT_MOUSE_WHEEL;
    e.wheel.which = 0;
    e.wheel.x = 0;
    e.wheel.y = 10;
    SDL_PushEvent(&e);
    e.type = SDL_EVENT_WINDOW_FOCUS_LOST;
    e.window.windowID = SDL_GetWindowID(a->window);
    SDL_PushEvent(&e);
}
static void test_plot_motion(app *a, float x, float y) {
    SDL_Event e = {0};
    e.type = SDL_EVENT_MOUSE_MOTION;
    e.motion.windowID = SDL_GetWindowID(a->window);
    e.motion.which = PS_TEST_MOUSE_ID;
    e.motion.x = x;
    e.motion.y = y;
    SDL_PushEvent(&e);
}
static void test_plot_wheel(app *a, float x, float y, float amount) {
    test_plot_motion(a, x, y);
    SDL_Event e = {0};
    e.type = SDL_EVENT_MOUSE_WHEEL;
    e.wheel.windowID = SDL_GetWindowID(a->window);
    e.wheel.which = PS_TEST_MOUSE_ID;
    e.wheel.y = amount;
    SDL_PushEvent(&e);
}
static void test_scene_key(app *a, SDL_Keycode key, SDL_Keymod mod, bool repeat) {
    SDL_Event e = {0};
    e.type = SDL_EVENT_KEY_DOWN;
    e.key.windowID = SDL_GetWindowID(a->window);
    e.key.key = key;
    e.key.mod = mod;
    e.key.repeat = repeat;
    e.key.down = true;
    SDL_PushEvent(&e);
    e.type = SDL_EVENT_KEY_UP;
    e.key.down = false;
    e.key.repeat = false;
    SDL_PushEvent(&e);
}
static void test_plot_frame(app *a, int *stage, int *code, const char **capture) {
    static ps_plot_view panned, raw;
    static struct nk_rect pick_target;
    bool ok = true;
    struct nk_rect area = a->plot_area;
    switch (*stage) {
    case 0:
        if (!a->active_plot_view || a->data.thread)
            return;
        ok = area.w > 100 && area.h > 50;
        *capture = "plot-fit.bmp";
        test_plot_wheel(a, area.x + .25f * area.w, area.y + .25f * area.h, 2);
        break;
    case 1:
        break;
    case 2:
        ok = fabs(a->report_views[0].low[0] - .09) < 1e-5 &&
             fabs(a->report_views[0].low[1] - .27) < 1e-5 &&
             fabs(a->report_views[0].high[0] - .73) < 1e-5;
        *capture = "plot-zoom.bmp";
        test_mouse(a, area, true);
        break;
    case 3:
        test_plot_motion(a, area.x + .6f * area.w, area.y + .6f * area.h);
        break;
    case 4:
        break;
    case 5:
        panned = a->report_views[0];
        ok = fabs(panned.low[0] - .026) < .002 && fabs(panned.low[1] - .334) < .005;
        *capture = "plot-pan.bmp";
        test_mouse(a, nk_rect(area.x + .6f * area.w, area.y + .6f * area.h, 0, 0), false);
        break;
    case 6:
        a->result_view = 1;
        test_plot_wheel(a, area.x + .25f * area.w, area.y + .25f * area.h, 2);
        break;
    case 7:
        break;
    case 8:
        ok = ps_plot_view_changed(&a->report_views[1]) &&
             !memcmp(&panned, &a->report_views[0], sizeof panned);
        *capture = "plot-scatter.bmp";
        *stage = 79; /* Capture before changing the selected diagram next frame. */
        break;
    case 80:
        a->result_view = 2;
        test_plot_wheel(a, area.x + .25f * area.w, area.y + .25f * area.h, 2);
        *stage = 8;
        break;
    case 9:
        break;
    case 10:
        ok = ps_plot_view_changed(&a->report_views[2]);
        *capture = "plot-histogram.bmp";
        test_mouse(a, a->result_export_bounds[0], true);
        break;
    case 11:
        test_mouse(a, a->result_export_bounds[0], false);
        break;
    case 12:
        break;
    case 13: {
        char path[4096];
        join(path, sizeof path, a->project, "full.csv");
        size_t n = 0, m = 0;
        char *expected = load_utf8_text(path, &n), *actual = load_utf8_text(a->result_export, &m);
        ok = expected && actual && n == m && !memcmp(expected, actual, n);
        free(expected);
        free(actual);
        test_mouse(a, a->plot_buttons[2], true);
        break;
    }
    case 14:
        test_mouse(a, a->plot_buttons[2], false);
        break;
    case 15:
        break;
    case 16:
        ok = !ps_plot_view_changed(&a->report_views[2]) &&
             !memcmp(&panned, &a->report_views[0], sizeof panned);
        a->show_report = false;
        a->plot_channel = 0;
        break;
    case 17:
        ok = a->active_plot_view == &a->data_views[0];
        test_plot_wheel(a, area.x + .25f * area.w, area.y + .25f * area.h, 2);
        break;
    case 18:
        break;
    case 19:
        ok = ps_plot_view_changed(&a->data_views[0]);
        raw = a->data_views[0];
        *capture = "plot-data.bmp";
        *stage = 80;
        break;
    case 81:
        a->plot_channel = 1;
        *stage = 19;
        break;
    case 20:
        ok = a->active_plot_view == &a->data_views[1] && !ps_plot_view_changed(&a->data_views[1]);
        test_mouse(a, a->plot_buttons[0], true);
        break;
    case 21:
        test_mouse(a, a->plot_buttons[0], false);
        break;
    case 22:
        break;
    case 23:
        ok = fabs(a->data_views[1].low[0] - .25) < 1e-9 && a->data.stats[1].count == 4 &&
             isnan(a->data.values[1][2]);
        *capture = "plot-sensor.bmp";
        test_plot_wheel(a, 40, 35, 2);
        break;
    case 24:
        break;
    case 25:
        ok = fabs(a->data_views[1].low[0] - .25) < 1e-9 &&
             !memcmp(&raw, &a->data_views[0], sizeof raw);
        a->show_report = true;
        a->result_view = 0;
        break;
    case 26:
        ok = !memcmp(&panned, &a->report_views[0], sizeof panned);
        {
            char path[4096];
            join(path, sizeof path, a->project, "fixture.psreport");
            request_report_path(a, path);
        }
        break;
    case 27:
        if (a->report_thread)
            return;
        ok = a->report_result == PS_OK && !ps_plot_view_changed(&a->report_views[0]) &&
             !ps_plot_view_changed(&a->report_views[1]);
        *capture = "plot-reopened.bmp";
        break;
    case 28:
        a->result_view = 3;
        break;
    case 29:
        ok = a->active_plot_view == &a->report_views[3];
        *capture = "plot-offset.bmp";
        test_plot_wheel(a, area.x + .5f * area.w, area.y + .5f * area.h, 4);
        break;
    case 30:
        break;
    case 31:
        ok = ps_plot_view_changed(&a->report_views[3]);
        *capture = "plot-offset-zoom.bmp";
        test_mouse(a, a->plot_buttons[1], true);
        break;
    case 32:
        test_mouse(a, a->plot_buttons[1], false);
        break;
    case 33:
        break;
    case 34:
        ok = fabs((a->report_views[3].high[0] - a->report_views[3].low[0]) - .8192) < 1e-9;
        test_mouse(a, a->result_export_bounds[2], true);
        break;
    case 35:
        test_mouse(a, a->result_export_bounds[2], false);
        break;
    case 36:
        break;
    case 37: {
        size_t count = 0;
        unsigned char *exported = SDL_LoadFile(a->result_export, &count);
        ok = exported && count > 33 && !memcmp(exported, "\211PNG\r\n\032\n", 8);
        for (uint32_t i = 0; ok && i < 4; i++) {
            char path[4096], name[64];
            snprintf(name, sizeof name, "figure-%u.png", i);
            join(path, sizeof path, a->project, name);
            ok = ps_report_image(a->ui, a->font_ui, a->analysis_report, i, path) == PS_OK;
            if (ok && i == 3) {
                size_t size = 0;
                void *full = SDL_LoadFile(path, &size);
                ok = full && size == count && !memcmp(full, exported, count) &&
                     ps_report_image(a->ui, a->font_ui, a->analysis_report, i, path) == PS_IO;
                SDL_free(full);
            }
        }
        SDL_free(exported);
        if (ok)
            ok = test_plot_dense_image(a);
        if (ok)
            ok = a->png_scale == 2 && test_plot_scaled_images(a);
        *capture = "plot-after-png.bmp";
        break;
    } break;
    case 38: test_mouse(a, a->png_size_bounds, true); break;
    case 39: test_mouse(a, a->png_size_bounds, false); break;
    case 40: test_mouse(a, a->png_size_choices[0], true); break;
    case 41: test_mouse(a, a->png_size_choices[0], false); break;
    case 42:
        ok = a->png_scale == 1;
        test_mouse(a, a->result_export_bounds[2], true);
        break;
    case 43: test_mouse(a, a->result_export_bounds[2], false); break;
    case 44:
        ok = test_plot_png_size(a->result_export, 1200, 850);
        test_mouse(a, a->png_size_bounds, true);
        break;
    case 45: test_mouse(a, a->png_size_bounds, false); break;
    case 46: test_mouse(a, a->png_size_choices[3], true); break;
    case 47: test_mouse(a, a->png_size_choices[3], false); break;
    case 48:
        ok = a->png_scale == 4;
        *capture = "plot-png-size.bmp";
        test_mouse(a, a->result_export_bounds[2], true);
        break;
    case 49: test_mouse(a, a->result_export_bounds[2], false); break;
    case 50:
        ok = test_plot_png_size(a->result_export, 4800, 3400);
        a->png_scale = 1;
        test_mouse(a, a->png_region_bounds, true);
        break;
    case 51: test_mouse(a, a->png_region_bounds, false); break;
    case 52:
        ok = strstr(a->result_export, "-ausschnitt.png") &&
             test_plot_png_size(a->result_export, 1200, 850) && test_plot_region_images(a);
        *capture = "plot-region-export.bmp";
        test_mouse(a, a->svg_region_bounds, true);
        break;
    case 53: test_mouse(a, a->svg_region_bounds, false); break;
    case 54:
        ok = strstr(a->result_export, "-ausschnitt.svg") && exists(a->result_export);
        break;
    case 55:
        a->tab = 1;
        a->scene = (ps_scene){0};
        ps_scene_add(&a->scene, PS_SPHERE, ps_v3(-.5, 0, 0), ps_v3(0, 0, 0), .3, 0xff4444ff);
        ps_scene_add(&a->scene, PS_SPHERE, ps_v3(.5, 0, 0), ps_v3(0, 0, 0), .3, 0x4488ffff);
        ps_scene_label(&a->scene, ps_v3(0, .7, 0), "Sichtbarkeit", 0xffffffff);
        for (uint32_t i = 0; i < a->scene.count; i++) a->scene.objects[i].id = 101 + i;
        ps_scene_view_sync(&a->scene_view, &a->scene);
        a->scene_disclosure = NK_MAXIMIZED;
        break;
    case 56:
        *capture = "scene-all.bmp";
        test_mouse(a, a->scene_visibility_bounds[0], true);
        break;
    case 57: test_mouse(a, a->scene_visibility_bounds[0], false); break;
    case 58:
        ok = !ps_scene_view_visible(&a->scene_view, 0) && ps_scene_view_visible(&a->scene_view, 1) && a->scene.count == 3;
        *capture = "scene-hidden.bmp";
        test_mouse(a, a->scene_visibility_bounds[0], true);
        break;
    case 59: test_mouse(a, a->scene_visibility_bounds[0], false); break;
    case 60:
        ok = ps_scene_view_visible(&a->scene_view, 0);
        test_mouse(a, a->scene_visibility_bounds[0], true);
        break;
    case 61: test_mouse(a, a->scene_visibility_bounds[0], false); break;
    case 62: {
        ok = !ps_scene_view_visible(&a->scene_view, 0);
        ps_object swap = a->scene.objects[0];
        a->scene.objects[0] = a->scene.objects[1];
        a->scene.objects[1] = swap;
        ps_scene_view_sync(&a->scene_view, &a->scene);
        break;
    }
    case 63:
        ok = ps_scene_view_visible(&a->scene_view, 0) && !ps_scene_view_visible(&a->scene_view, 1);
        *capture = "scene-reordered.bmp";
        break;
    case 64: {
        ps_camera camera = {a->yaw, a->pitch, a->zoom, a->camera_target,
                             a->orthographic, a->show_vectors, a->show_grid};
        struct nk_rect r = a->scene_viewport_bounds;
        float x, y;
        ok = ps_graphics_project(&camera, a->scene.objects[0].a, r.w / r.h, &x, &y);
        if (ok) {
            pick_target = nk_rect(r.x + x*r.w, r.y + y*r.h, 0, 0);
            test_mouse(a, pick_target, true);
        }
        break;
    }
    case 65: test_mouse(a, pick_target, false); break;
    case 66: {
        ok = a->scene_selected && a->scene_selected_id == 102;
        *capture = "scene-picked.bmp";
        ps_object swap = a->scene.objects[0];
        a->scene.objects[0] = a->scene.objects[1];
        a->scene.objects[1] = swap;
        ps_scene_view_sync(&a->scene_view, &a->scene);
        break;
    }
    case 67:
        ok = a->scene_selected && a->scene_selected_slot == 1 && a->scene_selected_id == 102;
        pick_target = nk_rect(a->scene_viewport_bounds.x + 10, a->scene_viewport_bounds.y + 10, 0, 0);
        test_mouse(a, pick_target, true);
        break;
    case 68: test_mouse(a, pick_target, false); break;
    case 69:
        ok = !a->scene_selected;
        test_mouse(a, a->scene_label_bounds[2], true);
        break;
    case 70: test_mouse(a, a->scene_label_bounds[2], false); break;
    case 71:
        ok = a->scene_selected && a->scene_selected_id == 103;
        break;
    case 72: {
        ps_object swap = a->scene.objects[0];
        a->scene.objects[0] = a->scene.objects[2];
        a->scene.objects[2] = swap;
        ps_scene_view_sync(&a->scene_view, &a->scene);
        test_mouse(a, a->scene_hide_selection_bounds, true);
        break;
    }
    case 73: test_mouse(a, a->scene_hide_selection_bounds, false); break;
    case 74:
        ok = !a->scene_selected && !ps_scene_view_visible(&a->scene_view, 0) &&
             ps_scene_view_visible(&a->scene_view, 1);
        *stage = 99;
        break;
    case 100: test_scene_key(a, SDLK_HOME, SDL_KMOD_ALT, false); break;
    case 101:
        ok = a->yaw == PS_CAMERA_DEFAULT.yaw && a->zoom == PS_CAMERA_DEFAULT.distance;
        test_scene_key(a, SDLK_RIGHT, SDL_KMOD_ALT, false);
        break;
    case 102:
        ok = fabsf(a->yaw - PS_CAMERA_DEFAULT.yaw - .08f) < 1e-6f;
        test_scene_key(a, SDLK_UP, SDL_KMOD_ALT | SDL_KMOD_SHIFT, false);
        break;
    case 103:
        ok = a->camera_target.y > PS_CAMERA_DEFAULT.target.y;
        test_scene_key(a, SDLK_PAGEUP, SDL_KMOD_ALT, false);
        break;
    case 104:
        ok = fabsf(a->zoom - PS_CAMERA_DEFAULT.distance * .9f) < 1e-6f;
        a->scene_view.hidden = 0;
        a->show_labels = false;
        a->scene.objects[1].color &= ~255u;
        a->scene_selected = false;
        test_scene_key(a, SDLK_N, SDL_KMOD_ALT, false);
        break;
    case 105:
        ok = a->scene_selected && a->scene_selected_id == 101 && a->scene_selected_slot == 2;
        *capture = "scene-keyboard.bmp";
        test_scene_key(a, SDLK_H, SDL_KMOD_ALT, true);
        break;
    case 106: {
        ok = a->scene_selected && ps_scene_view_visible(&a->scene_view, 2);
        ps_object swap = a->scene.objects[1];
        a->scene.objects[1] = a->scene.objects[2];
        a->scene.objects[2] = swap;
        test_scene_key(a, SDLK_H, SDL_KMOD_ALT, false);
        break;
    }
    case 107:
        ok = !a->scene_selected && !ps_scene_view_visible(&a->scene_view, 1) &&
             ps_scene_view_visible(&a->scene_view, 2);
        test_scene_key(a, SDLK_N, SDL_KMOD_ALT, false);
        break;
    case 108:
        ok = !a->scene_selected;
        a->show_labels = true;
        test_scene_key(a, SDLK_P, SDL_KMOD_ALT, false);
        break;
    case 109:
        ok = a->scene_selected && a->scene_selected_id == 103;
        a->tab = 0;
        test_scene_key(a, SDLK_RIGHT, SDL_KMOD_ALT, false);
        break;
    case 110:
        ok = fabsf(a->yaw - PS_CAMERA_DEFAULT.yaw - .08f) < 1e-6f;
        a->tab = 2;
        test_scene_key(a, SDLK_RIGHT, SDL_KMOD_ALT, false);
        break;
    case 111:
        ok = fabsf(a->yaw - PS_CAMERA_DEFAULT.yaw - .08f) < 1e-6f;
        a->tab = 1;
        test_scene_key(a, SDLK_RIGHT, SDL_KMOD_ALT | SDL_KMOD_CTRL, false);
        break;
    case 112:
        ok = fabsf(a->yaw - PS_CAMERA_DEFAULT.yaw - .08f) < 1e-6f;
        for (unsigned i = 0; i < 80; i++) {
            test_scene_key(a, SDLK_UP, SDL_KMOD_ALT, true);
            test_scene_key(a, SDLK_PAGEDOWN, SDL_KMOD_ALT, true);
        }
        break;
    case 113:
        ok = a->pitch == (float)PS_PI / 2 && a->zoom == 12;
        test_scene_key(a, SDLK_HOME, SDL_KMOD_ALT, false);
        break;
    case 114:
        ok = a->yaw == PS_CAMERA_DEFAULT.yaw && a->pitch == PS_CAMERA_DEFAULT.pitch &&
             a->zoom == PS_CAMERA_DEFAULT.distance &&
             ps_vlength(ps_vsub(a->camera_target, PS_CAMERA_DEFAULT.target)) == 0;
        a->scene.count = 0;
        test_scene_key(a, SDLK_N, SDL_KMOD_ALT, false);
        break;
    case 115: ok = !a->scene_selected; break;
    default:
        printf("PLOT UI SELF-TEST: %s\n", *code ? "FAILED" : "PASSED");
        a->quitting = true;
        return;
    }
    if (!ok) {
        fprintf(stderr, "Plot UI stage %d failed, area=(%.1f,%.1f,%.1f,%.1f), range=(%.9g,%.9g)\n",
                *stage, area.x, area.y, area.w, area.h, a->report_views[0].low[0],
                a->report_views[0].low[1]);
        *code = 1;
        a->quitting = true;
    }
    (*stage)++;
}
