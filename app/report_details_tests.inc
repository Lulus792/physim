static char report_details_expected[8192], report_details_source[12000];
static const char *report_details_inputs = "role,index,path,bytes,fnv1a64\r\ninput,1,\"Lauf α.psrun\",123,0123456789abcdef\r\n";
static const char *report_details_build = "physim_native_build=1\ncompiler=Fixture compiler\nprofile=Release\napi_version=1\nabi_version=3\n";
static bool report_details_test_write(const char *path, const char *text) {
    FILE *file = fopen(path, "wb");
    if (!file) return false;
    bool ok = fputs(text, file) >= 0;
    return fclose(file) == 0 && ok;
}
static bool report_details_test_start(app *a, const char *root, const char *mode) {
    bool language = strstr(mode, "-phys") != NULL;
    char path[4096];
    join(path, sizeof path, root, "preferences.bin"); preferences_start(a, path);
    join(a->project_input, sizeof a->project_input, root, "project");
    a->example = 0; a->template_analysis_language = language ? 1 : 0; new_project(a);
    if (!a->loaded) return false;
    strcpy(report_details_expected, "analysis_source=forged-source.c\ncompiler=Fixture compiler\nbuild=Fixture build\n");
    for (unsigned i = 0; i < 180; i++) {
        char line[48]; snprintf(line, sizeof line, "Eingabe %03u · Parameter α = %u\n", i, i);
        strcat(report_details_expected, line);
    }
    strcat(report_details_expected, "LAST_PROVENANCE_α\n");
    strcpy(report_details_source, language ? "// Archived Physim α\r\nlet value = 42\r\n" : "// Archived C α\r\nint value = 42;\r\n");
    for (unsigned i = 0; i < 200; i++) {
        char line[48]; snprintf(line, sizeof line, "// Archived line %03u α\r\n", i);
        strcat(report_details_source, line);
    }
    strcat(report_details_source, "// LAST_SOURCE_α\r\n");
    ps_report *report = NULL;
    if (ps_report_create("Stored analysis", report_details_expected, &report) != PS_OK) return false;
    ps_plot_info plot = {0}; ps_plot_handle handle;
    strcpy(plot.title, "Position"); strcpy(plot.x_label, "Time"); strcpy(plot.y_label, "Position");
    ps_report_unit_from(PS_SECOND, &plot.x_unit); ps_report_unit_from(PS_METRE, &plot.y_unit);
    ps_curve_data curve = {.kind = PS_PLOT_LINE, .count = 3, .source_count = 3, .x = {0,1,2}, .y = {4,5,6}};
    strcpy(curve.label, "Stored positions");
    bool ok = ps_report_add_plot(report, &plot, &handle) == PS_OK && ps_report_add_curve(report, handle, &curve) == PS_OK;
    join(path, sizeof path, a->project, "runs/details.psreport");
    ok &= ps_report_save(report, path) == PS_OK; ps_report_destroy(report);
    if (!ok) return false;
    request_report_path(a, path);
    join(path, sizeof path, a->project, language ? "runs/details.source.phys" : "runs/details.source.c");
    ok &= report_details_test_write(path, report_details_source);
    join(path, sizeof path, a->project, "runs/details.inputs.csv"); ok &= report_details_test_write(path, report_details_inputs);
    join(path, sizeof path, a->project, "runs/details.build.txt"); ok &= report_details_test_write(path, report_details_build);
    join(path, sizeof path, a->project, "forged-source.c"); ok &= report_details_test_write(path, "DO NOT DISPLAY THIS FILE\n");
    if (ps_report_create("Empty analysis", "EMPTY_PROVENANCE_α\n", &report) != PS_OK) return false;
    join(path, sizeof path, a->project, "runs/empty.PSREPORT"); ok &= ps_report_save(report, path) == PS_OK; ps_report_destroy(report);
    join(path, sizeof path, a->project, language ? "runs/empty.source.phys" : "runs/empty.source.c");
    ok &= report_details_test_write(path, "// EMPTY_ARCHIVE_α\n");
    join(path, sizeof path, a->project, "runs/invalid.psreport"); ok &= report_details_test_write(path, "invalid report\n");
    nk_str_append_text_char(&a->experiment.string, "// Unsaved experiment α\n", (int)strlen("// Unsaved experiment α\n"));
    nk_str_append_text_char(&a->analysis.string, "// Unsaved analysis β\n", (int)strlen("// Unsaved analysis β\n"));
    a->dirty = a->analysis_dirty = true;
    SDL_SetWindowSize(a->window, 1080, 740); a->show_log = false; ps_dock_hide(&a->preferences.dock, PS_DOCK_LOG);
    if (strstr(mode, "-22")) { ps_preferences preferences = a->preferences; preferences.ui_size = 22; preferences_apply(a, &preferences); }
    a->tab = 2; a->analysis_tab = 0;
    return ok;
}
static bool report_details_test_clipboard(const char *expected) {
    char *text = SDL_GetClipboardText(); bool ok = text && !strcmp(text, expected); SDL_free(text); return ok;
}
static void report_details_test_frame(app *a, const char *mode, int *stage, int *code, const char **capture) {
    static bool settle; static uint32_t original[2], display_crc;
    bool ok = true; (void)mode;
    if (settle) { settle = false; return; } settle = true;
    if (a->report_thread || a->library_thread) return;
    switch (*stage) {
    case 0:
        if (!a->report_details_bounds.w) return;
        ok = a->analysis_report && a->show_report && a->result_view == 0;
        original[0] = ps_crc32((const unsigned char *)nk_str_get_const(&a->experiment.string), (size_t)nk_str_len_char(&a->experiment.string));
        original[1] = ps_crc32((const unsigned char *)nk_str_get_const(&a->analysis.string), (size_t)nk_str_len_char(&a->analysis.string));
        test_mouse(a, a->report_details_bounds, true); break;
    case 1: test_mouse(a, a->report_details_bounds, false); break;
    case 2:
        ok = a->result_view == 1 && a->report_details_text && !strcmp(a->report_details_text, report_details_expected);
        *capture = "report-provenance.bmp"; test_mouse(a, a->report_details_copy_bounds, true); break;
    case 3: test_mouse(a, a->report_details_copy_bounds, false); break;
    case 4: ok = report_details_test_clipboard(report_details_expected); test_mouse(a, a->report_details_tabs[1], true); break;
    case 5: test_mouse(a, a->report_details_tabs[1], false); break;
    case 6:
        ok = a->report_details_text && !strcmp(a->report_details_text, report_details_source) &&
            strstr(a->report_details_path, "runs/details.source.") && !strstr(a->report_details_text, "DO NOT DISPLAY");
        ok &= a->report_details_display && !strchr(a->report_details_display, '\r');
        if (a->report_details_display) display_crc = ps_crc32((const unsigned char *)a->report_details_display, a->report_details_display_length);
        *capture = "report-source.bmp"; test_mouse(a, a->report_details_text_bounds, true); break;
    case 7: test_mouse(a, a->report_details_text_bounds, false); break;
    case 8: ok = a->report_details_active; test_key(a, SDLK_A); break;
    case 9: layout_test_text(a, "ATTEMPTED CHANGE"); break;
    case 10: test_window_key_mod(a->window, SDLK_DELETE, 0); break;
    case 11:
        ok = !strcmp(a->report_details_text, report_details_source) &&
            display_crc == ps_crc32((const unsigned char *)a->report_details_display, a->report_details_display_length);
        test_window_key_mod(a->window, SDLK_END, SDL_KMOD_CTRL); *stage = 29; break;
    case 30: {
        struct nk_window *window = nk_window_find(a->ui, "workspace");
        ok = window && window->edit.scrollbar.y > 0;
        *capture = "report-source-end.bmp";
        test_mouse(a, a->report_details_copy_bounds, true); *stage = 11; break;
    }
    case 12: test_mouse(a, a->report_details_copy_bounds, false); break;
    case 13: ok = report_details_test_clipboard(report_details_source); test_mouse(a, a->report_details_tabs[2], true); break;
    case 14: test_mouse(a, a->report_details_tabs[2], false); break;
    case 15:
        ok = a->report_details_text && !strcmp(a->report_details_text, report_details_inputs);
        *capture = "report-inputs.bmp"; test_mouse(a, a->report_details_copy_bounds, true); break;
    case 16: test_mouse(a, a->report_details_copy_bounds, false); break;
    case 17: {
        ok = report_details_test_clipboard(report_details_inputs);
        test_mouse(a, a->report_details_tabs[3], true); *stage = 34; break;
    }
    case 35: test_mouse(a, a->report_details_tabs[3], false); break;
    case 36:
        ok = a->report_details_text && !strcmp(a->report_details_text, report_details_build);
        *capture = "report-build.bmp"; test_mouse(a, a->report_details_copy_bounds, true); break;
    case 37: test_mouse(a, a->report_details_copy_bounds, false); break;
    case 38: {
        ok = report_details_test_clipboard(report_details_build);
        char path[4096]; join(path, sizeof path, a->project, "runs/invalid.psreport"); request_report_path(a, path); *stage = 17; break;
    }
    case 18:
        ok = a->report_result != PS_OK && a->analysis_report && a->report_details_text &&
            !strcmp(a->report_details_text, report_details_build) && strstr(a->loaded_report_path, "details.psreport");
        {char path[4096]; join(path, sizeof path, a->project, "runs/empty.PSREPORT"); request_report_path(a, path);} break;
    case 19:
        if (!a->report_details_ready) return;
        ok = a->report_result == PS_OK && a->result_view == 0 && a->report_details_text &&
            !strcmp(a->report_details_text, "EMPTY_PROVENANCE_α\n");
        *capture = "report-empty.bmp"; test_mouse(a, a->report_details_tabs[1], true); break;
    case 20: test_mouse(a, a->report_details_tabs[1], false); break;
    case 21:
        ok = a->report_details_text && !strcmp(a->report_details_text, "// EMPTY_ARCHIVE_α\n");
        *capture = "report-empty-source.bmp"; test_mouse(a, a->report_details_tabs[2], true); break;
    case 22: test_mouse(a, a->report_details_tabs[2], false); break;
    case 23:
        ok = !a->report_details_text && a->report_details_error[0] && a->analysis_report;
        *capture = "report-missing-companion.bmp";
        SDL_SetClipboardText("MISSING_SENTINEL"); test_mouse(a, a->report_details_copy_bounds, true); break;
    case 24: test_mouse(a, a->report_details_copy_bounds, false); break;
    case 25:
        ok = report_details_test_clipboard("MISSING_SENTINEL");
        ok &= a->dirty && a->analysis_dirty && !a->job.running && !a->runner.running &&
            original[0] == ps_crc32((const unsigned char *)nk_str_get_const(&a->experiment.string), (size_t)nk_str_len_char(&a->experiment.string)) &&
            original[1] == ps_crc32((const unsigned char *)nk_str_get_const(&a->analysis.string), (size_t)nk_str_len_char(&a->analysis.string));
        a->quitting = true; break;
    }
    if (!ok) { fprintf(stderr, "Report details stage %d failed: view=%d tab=%d path=%s error=%s status=%s\n", *stage, a->result_view, a->report_details_tab, a->report_details_path, a->report_details_error, a->status); *code = 1; a->quitting = true; }
    if (a->quitting) printf("REPORT DETAILS SELF-TEST: %s\n", *code ? "FAILED" : "PASSED");
    (*stage)++;
}
