static bool library_selection_test_start(app *a, const char *root, const char *mode) {
    bool language = strstr(mode, "-phys") != NULL;
    char path[4096];
    join(path, sizeof path, root, "preferences.bin");
    preferences_start(a, path);
    join(a->project_input, sizeof a->project_input, root, "project");
    a->example = 0;
    a->template_analysis_language = language ? 1 : 0;
    new_project(a);
    if (!a->loaded)
        return false;
    if (language) {
        join(path, sizeof path, a->root, "examples/language/analysis.phys");
        if (!load_editor(&a->analysis, path))
            return false;
        a->analysis_dirty = true;
    }
    select_build_profile(a, 1);
    for (unsigned run = 0; run < 9; run++) {
        char name[80];
        snprintf(name, sizeof name, "runs/selection-%u.psrun", run + 1);
        join(path, sizeof path, a->project, name);
        ps_context context = {.struct_size = sizeof context, .api_version = PS_API_VERSION,
                              .dt_s = run % 2 ? .02 : .01};
        if (ps_channel_add(&context, "position.x", PS_METRE, "Selection fixture") != 0)
            return false;
        ps_run_writer writer;
        if (ps_run_create(&writer, path, &context, "Selection fixture") != PS_OK)
            return false;
        bool ok = true;
        for (unsigned sample = 0; sample < 5; sample++) {
            double time = sample * context.dt_s;
            context.values[0] = run * 10 + (run + 1) * time;
            ok &= ps_run_append(&writer, time, context.values) == PS_OK;
        }
        ok &= ps_run_close(&writer) == PS_OK;
        if (!ok)
            return false;
    }
    SDL_SetWindowSize(a->window, 1080, 740);
    a->show_log = false;
    ps_dock_hide(&a->preferences.dock, PS_DOCK_LOG);
    if (strstr(mode, "-22")) {
        ps_preferences preferences = a->preferences;
        preferences.ui_size = 22;
        preferences_apply(a, &preferences);
    }
    refresh_library(a);
    return true;
}
static bool library_selection_test_order(const app *a, const unsigned *order, unsigned count) {
    if (a->selected_count != count)
        return false;
    for (unsigned i = 0; i < count; i++) {
        char name[80], path[4096];
        snprintf(name, sizeof name, "runs/selection-%u.psrun", order[i] + 1);
        join(path, sizeof path, a->project, name);
        if (strcmp(path, a->selected_runs[i]))
            return false;
    }
    return true;
}
static void library_selection_test_frame(app *a, const char *mode, int *stage, int *code,
                                         const char **capture) {
    static bool settle;
    static unsigned run;
    bool language = strstr(mode, "-phys") != NULL, ok = true;
    if (settle) { settle = false; return; }
    settle = true;
    if (a->library_thread)
        return;
    switch (*stage) {
    case 0: test_window_key_mod(a->window, SDLK_F5, 0); break;
    case 1:
        if (!idle(a)) return;
        ok = a->built;
        test_key(a, SDLK_4);
        break;
    case 2:
        snprintf(a->library_query, sizeof a->library_query, "selection-%u.psrun", run + 1);
        break;
    case 3:
        ok = a->library_visible_count == 1;
        test_mouse(a, a->library_select_bounds[0], true);
        break;
    case 4: test_mouse(a, a->library_select_bounds[0], false); break;
    case 5: {
        unsigned order[] = {0,1,2,3,4,5,6,7};
        ok = library_selection_test_order(a, order, run < 8 ? run + 1 : 8);
        if (run == 8) {
            ok &= strstr(a->status, "acht") != NULL;
            *capture = "selection-limit.bmp";
            strcpy(a->library_query, "selection-6.psrun");
        } else { run++; *stage = 1; }
        break;
    }
    case 6:
        ok = a->font_ui->width(a->font_ui->userdata, a->font_ui->height,
                              "Als Referenz", 12) + 2 * a->ui->style.button.padding.x <=
             a->library_reference_bounds[0].w;
        test_mouse(a, a->library_reference_bounds[0], true);
        break;
    case 7: test_mouse(a, a->library_reference_bounds[0], false); break;
    case 8: {
        unsigned order[] = {5,0,1,2,3,4,6,7};
        ok = library_selection_test_order(a, order, 8);
        *capture = "selection-reference.bmp";
        test_mouse(a, a->library_select_bounds[0], true);
        break;
    }
    case 9: test_mouse(a, a->library_select_bounds[0], false); break;
    case 10: {
        unsigned order[] = {0,1,2,3,4,6,7};
        ok = library_selection_test_order(a, order, 7);
        strcpy(a->library_query, "selection-9.psrun");
        break;
    }
    case 11: test_mouse(a, a->library_select_bounds[0], true); break;
    case 12: test_mouse(a, a->library_select_bounds[0], false); break;
    case 13: test_mouse(a, a->library_reference_bounds[0], true); break;
    case 14: test_mouse(a, a->library_reference_bounds[0], false); break;
    case 15: {
        unsigned order[] = {8,0,1,2,3,4,6,7};
        ok = library_selection_test_order(a, order, 8);
        ok &= a->font_ui->width(a->font_ui->userdata, a->font_ui->height,
                               "Vergleichen", 11) + 2 * a->ui->style.button.padding.x <=
              a->library_analyze_bounds.w;
        a->library_query[0] = 0;
        *capture = "selection-ready.bmp";
        test_mouse(a, a->library_analyze_bounds, true);
        break;
    }
    case 16: test_mouse(a, a->library_analyze_bounds, false); break;
    case 17: {
        if (!idle(a)) return;
        uint32_t plots = 0, tables = 0;
        ok = a->analysis_report && a->show_report &&
            ps_report_describe(a->analysis_report, NULL, NULL, &plots, &tables) == PS_OK &&
            plots == (language ? 16u : 3u);
        unsigned order[] = {8,0,1,2,3,4,6,7};
        ps_curve_data curve;
        for (unsigned run_index = 0; ok && run_index < 8; run_index++) {
            ok = ps_report_curve_read(a->analysis_report, language ? run_index * 2 + 1 : 0,
                                       language ? 0 : run_index, &curve) == PS_OK && curve.count == 5;
            for (size_t i = 0; ok && i < curve.count; i++)
                ok &= fabs(curve.y[i] - (order[run_index] + 1)) < 1e-9;
        }
        *capture = "selection-multiple-result.bmp";
        test_key(a, SDLK_4);
        break;
    }
    case 18: test_mouse(a, a->library_clear_bounds, true); break;
    case 19: test_mouse(a, a->library_clear_bounds, false); break;
    case 20:
        ok = !a->selected_count;
        strcpy(a->library_query, "selection-3.psrun");
        break;
    case 21: test_mouse(a, a->library_select_bounds[0], true); break;
    case 22: test_mouse(a, a->library_select_bounds[0], false); break;
    case 23: {
        unsigned order[] = {2};
        ok = library_selection_test_order(a, order, 1);
        *capture = "selection-single.bmp";
        test_mouse(a, a->library_analyze_bounds, true);
        break;
    }
    case 24: test_mouse(a, a->library_analyze_bounds, false); break;
    case 25: {
        if (!idle(a)) return;
        ps_curve_data curve;
        ok = a->analysis_report && a->show_report &&
            ps_report_curve_read(a->analysis_report, language ? 1 : 0, 0, &curve) == PS_OK && curve.count == 5;
        for (size_t i = 0; ok && i < curve.count; i++)
            ok &= fabs(curve.y[i] - 3) < 1e-9;
        *capture = "selection-single-result.bmp";
        a->quitting = true;
        break;
    }
    }
    if (!ok) {
        fprintf(stderr, "Library selection stage %d failed: count=%u query=%s status=%s\n",
                *stage, a->selected_count, a->library_query, a->status);
        *code = 1;
        a->quitting = true;
    }
    if (a->quitting)
        printf("LIBRARY SELECTION SELF-TEST: %s\n", *code ? "FAILED" : "PASSED");
    (*stage)++;
}
