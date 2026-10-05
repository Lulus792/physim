/* Real process, control-event and data-file checks for a fresh paused run. */
static bool reset_test_start(app *a, const char *root, const char *mode) {
    join(a->project_input, sizeof a->project_input, root, "project");
    a->example = !strcmp(mode, "reset-c") || !strcmp(mode, "reset-hang") ? 5
                 : !strcmp(mode, "reset-parameters") ? 18 : 10;
    new_project(a);
    if (!a->loaded)
        return false;
    if (!strcmp(mode, "reset-hang")) {
        const char *text = nk_str_get_const(&a->experiment.string);
        const char *signature = "static ps_result step(ps_context *c, double dt) {";
        const char *at = strstr(text, signature);
        const char *probe = "\n    if (c->time_s >= .25) { for (;;) {} }\n";
        if (!at || !nk_str_insert_text_char(&a->experiment.string,
                    (int)(at - text + strlen(signature)), probe, (int)strlen(probe)))
            return false;
        a->dirty = true;
        invalidate_build(a);
        text = nk_str_get_const(&a->experiment.string);
        signature = "static ps_result create(ps_context *c) {";
        at = strstr(text, signature);
        probe = "\n    FILE *probe = fopen(\"reset-initialize-hang\", \"rb\");\n"
                "    if (probe) { fclose(probe); for (;;) {} }\n";
        if (!at || !nk_str_insert_text_char(&a->experiment.string,
                    (int)(at - text + strlen(signature)), probe, (int)strlen(probe)))
            return false;
    }
    a->dt = .125;
    snprintf(a->seed, sizeof a->seed, "%llu", (unsigned long long)UINT64_MAX);
    a->project_settings_dirty = true;
    if (!strcmp(mode, "reset-parameters") &&
        !ps_parameter_catalog_restore(&a->parameters, "friction", "0.25"))
        return false;
    select_build_profile(a, 1);
    return true;
}
static bool reset_test_values(const double *actual, const double *expected, uint32_t count) {
    for (uint32_t i = 0; i < count; i++)
        if (actual[i] != expected[i] && !(isnan(actual[i]) && isnan(expected[i])))
            return false;
    return true;
}
static bool reset_test_crc(const char *path, uint32_t *crc) {
    size_t size;
    void *bytes = SDL_LoadFile(path, &size);
    if (!bytes)
        return false;
    *crc = ps_crc32(bytes, size);
    SDL_free(bytes);
    return true;
}
static bool reset_test_read(const char *path, const char *mode, uint32_t *channels,
                            double *initial, double *first_step, bool initial_only) {
    ps_run_reader reader;
    if (ps_run_open(&reader, path) != PS_OK)
        return false;
    double time;
    bool ok = strstr(reader.metadata, "\ndt_s=0.125\n") &&
              strstr(reader.metadata, "\nseed=18446744073709551615\n");
    if (!strcmp(mode, "reset-parameters"))
        ok &= strstr(reader.metadata, "\nparameter.friction=0.25\n") != NULL;
    *channels = reader.channels;
    ok &= ps_run_next(&reader, &time, initial) == PS_OK && time == 0;
    ps_result next = ps_run_next(&reader, &time, first_step);
    ok &= initial_only ? next == PS_EOF : next == PS_OK && time == .125;
    ps_run_reader_close(&reader);
    return ok;
}
static void reset_test_frame(app *a, const char *mode, int *stage, int *code,
                              const char **capture) {
    static char runs[4][4096], source[4096], hidden[4096], module[4096], missing[4096], trigger[4096];
    static double previous_time, previous_values[PS_MAX_CHANNELS];
    static int previous_history;
    static ps_report *previous_report;
    static uint32_t checksums[3], channels;
    static double initial[PS_MAX_CHANNELS], first_step[PS_MAX_CHANNELS], hold;
    static float yaw, pitch, zoom;
    static int reported = -1;
    bool ok = true;
    bool hanging = !strcmp(mode, "reset-hang");
    if (reported != *stage) {
        fprintf(stderr, "Reset %s stage %d: %s\n", mode, *stage, a->status);
        reported = *stage;
    }
    switch (*stage) {
    case 0:
        build_project(a);
        break;
    case 1:
        if (!idle(a) || a->library_thread)
            return;
        ok = a->built;
        start_run(a);
        snprintf(runs[0], sizeof runs[0], "%s", a->last_run);
        a->yaw = .4f;
        a->pitch = .3f;
        a->zoom = 6;
        yaw = a->yaw; pitch = a->pitch; zoom = a->zoom;
        hold = 0;
        break;
    case 2:
        if (!a->hello || a->simulation_time < (hanging ? .125 : .25))
            return;
        if (hanging) {
            if (!hold) hold = ps_clock();
            if (ps_clock() - hold < .35) return;
        }
        ok = !a->paused && a->scene.count > 0;
        a->scene_view.hidden = 1;
        a->scene_selected = true;
        test_mouse(a, a->run_control_bounds[3], true);
        break;
    case 3:
        test_mouse(a, a->run_control_bounds[3], false);
        break;
    case 4:
        if (a->reset_pending || a->reset_starting || !strcmp(a->last_run, runs[0]) || !a->hello || !a->scene.count)
            return;
        ok = reset_test_read(runs[0], mode, &channels, initial, first_step, false) &&
             reset_test_crc(runs[0], &checksums[0]) &&
             a->paused && a->simulation_time == 0 && a->channel_count == channels &&
             reset_test_values(a->values, initial, channels) && a->history_count == 1 &&
             a->history_t[0] == 0 && !a->scene_selected && !a->scene_view.hidden &&
             a->yaw == yaw && a->pitch == pitch && a->zoom == zoom;
        if (hanging)
            ok &= strstr(a->log, "Runner nach Stop-Timeout") != NULL &&
                  a->data.result == PS_RECOVERED;
        snprintf(runs[1], sizeof runs[1], "%s", a->last_run);
        hold = ps_clock();
        *capture = "reset-initial.bmp";
        break;
    case 5:
        ok = a->paused && a->simulation_time == 0;
        if (ps_clock() - hold < .15)
            return;
        test_mouse(a, a->run_control_bounds[2], true);
        break;
    case 6:
        test_mouse(a, a->run_control_bounds[2], false);
        break;
    case 7:
        if (a->simulation_time == 0)
            return;
        ok = a->paused && a->simulation_time == .125 && a->history_count == 2 &&
             reset_test_values(a->values, first_step, channels);
        *capture = "reset-step.bmp";
        hold = ps_clock();
        break;
    case 8:
        ok = a->paused && a->simulation_time == .125;
        if (ps_clock() - hold < .15)
            return;
        test_mouse(a, a->run_control_bounds[3], true);
        break;
    case 9:
        test_mouse(a, a->run_control_bounds[3], false);
        break;
    case 10:
        if (a->reset_pending || a->reset_starting || !strcmp(a->last_run, runs[1]) || !a->hello || !a->scene.count)
            return;
        ok = reset_test_crc(runs[1], &checksums[1]) && a->paused && a->simulation_time == 0 &&
             reset_test_values(a->values, initial, channels);
        snprintf(runs[2], sizeof runs[2], "%s", a->last_run);
        hold = ps_clock();
        break;
    case 11:
        ok = a->paused && a->simulation_time == 0;
        if (ps_clock() - hold < .15)
            return;
        test_mouse(a, a->run_control_bounds[1], true);
        break;
    case 12:
        test_mouse(a, a->run_control_bounds[1], false);
        hold = 0;
        break;
    case 13:
        if (a->simulation_time < (hanging ? .125 : .25))
            return;
        if (hanging) {
            if (!hold) hold = ps_clock();
            if (ps_clock() - hold < .35) return;
        }
        ok = !a->paused;
        test_mouse(a, a->run_control_bounds[4], true);
        break;
    case 14:
        test_mouse(a, a->run_control_bounds[4], false);
        break;
    case 15:
        if (!idle(a) || a->library_thread)
            return;
        ok = reset_test_crc(runs[2], &checksums[2]);
        if (hanging)
            ok &= a->data.result == PS_RECOVERED;
        previous_time = a->simulation_time;
        previous_history = a->history_count;
        memcpy(previous_values, a->values, sizeof previous_values);
        ok &= ps_report_create("Vorherige Analyse", "Fehlererhaltung", &a->analysis_report) == PS_OK;
        previous_report = a->analysis_report;
        join(source, sizeof source, a->project, experiment_source(a));
        join(hidden, sizeof hidden, a->project, "reset-source-hidden");
        ok &= SDL_RenamePath(source, hidden);
        test_key(a, SDLK_F7);
        break;
    case 16:
        if (a->reset_pending)
            return;
        ok = !a->runner.running && !strcmp(a->last_run, runs[2]) &&
             !strcmp(a->selected_runs[0], runs[2]) &&
             strstr(a->status, "Quellcode-Snapshot") != NULL;
        ok &= SDL_RenamePath(hidden, source);
        join(module, sizeof module, a->build_directory, "experiment" MODULE_EXT);
        join(missing, sizeof missing, a->build_directory, "reset-module-hidden" MODULE_EXT);
        ok &= SDL_RenamePath(module, missing);
        test_key(a, SDLK_F7);
        break;
    case 17:
        if (a->reset_pending || a->reset_starting || a->runner.running)
            return;
        ok = !strcmp(a->last_run, runs[2]) && !strcmp(a->selected_runs[0], runs[2]) &&
             a->simulation_time == previous_time && a->history_count == previous_history &&
             reset_test_values(a->values, previous_values, channels) &&
             a->analysis_report == previous_report && strstr(a->status, "Zurücksetzen fehlgeschlagen");
        ok &= SDL_RenamePath(missing, module);
        if (hanging) {
            join(trigger, sizeof trigger, a->project, "reset-initialize-hang");
            FILE *file = fopen(trigger, "wbx");
            ok &= file != NULL;
            if (file) ok &= fclose(file) == 0;
        } else
            *stage = 20;
        hold = ps_clock();
        test_key(a, SDLK_F7);
        break;
    case 18:
        if (!a->reset_starting || !a->runner.running || ps_clock() - hold < .25)
            return;
        ok = !a->hello && a->simulation_time == previous_time &&
             a->analysis_report == previous_report;
        test_mouse(a, a->run_control_bounds[4], true);
        hold = ps_clock();
        break;
    case 19:
        test_mouse(a, a->run_control_bounds[4], false);
        break;
    case 20:
        if (a->runner.running || a->reset_starting || a->reset_pending)
            return;
        ok = ps_clock() - hold >= 1 && !strcmp(a->last_run, runs[2]) &&
             a->simulation_time == previous_time && a->analysis_report == previous_report;
        ok &= remove(trigger) == 0;
        test_key(a, SDLK_F7);
        break;
    case 21:
        if (a->reset_pending || a->reset_starting || !strcmp(a->last_run, runs[2]) || !a->hello || !a->scene.count)
            return;
        ok = a->paused && a->simulation_time == 0 &&
             reset_test_values(a->values, initial, channels);
        ok &= a->analysis_report == NULL;
        snprintf(runs[3], sizeof runs[3], "%s", a->last_run);
        *capture = "reset-retry.bmp";
        test_mouse(a, a->run_control_bounds[4], true);
        break;
    case 22:
        test_mouse(a, a->run_control_bounds[4], false);
        break;
    case 23: {
        if (!idle(a) || a->library_thread)
            return;
        double value[PS_MAX_CHANNELS], step[PS_MAX_CHANNELS];
        uint32_t count;
        ok = reset_test_read(runs[3], mode, &count, value, step, true) &&
             count == channels && reset_test_values(value, initial, count);
        for (unsigned i = 0; i < 3; i++) {
            uint32_t crc;
            ok &= reset_test_crc(runs[i], &crc) && crc == checksums[i];
        }
        a->dirty = true;
        test_mouse(a, a->run_control_bounds[3], true);
        break;
    }
    case 24:
        test_mouse(a, a->run_control_bounds[3], false);
        break;
    case 25:
        ok = !a->runner.running && !a->reset_pending && !strcmp(a->last_run, runs[3]);
        a->dirty = false;
        a->quitting = true;
        break;
    }
    if (!ok) {
        fprintf(stderr, "Reset failure: stage=%d time=%.17g paused=%d pending=%d history=%d\n",
                *stage, a->simulation_time, a->paused, a->reset_pending, a->history_count);
        *code = 1;
        a->quitting = true;
    }
    if (a->quitting)
        printf("RESET %s SELF-TEST: %s\n", mode, *code ? "FAILED" : "PASSED");
    (*stage)++;
}
