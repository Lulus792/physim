static bool project_settings_test_start(app *a, const char *root, const char *mode) {
    join(a->project_input, sizeof a->project_input, root, "project");
    if (!strcmp(mode, "project-settings-write")) {
        a->example = 18; /* Collision language template exposes friction. */
        new_project(a);
        if (!a->loaded || a->profile != 0)
            return false;
        select_build_profile(a, 1);
        a->dt = .125;
        snprintf(a->seed, sizeof a->seed, "%llu", (unsigned long long)UINT64_MAX);
        return a->project_settings_dirty && !a->built &&
               ps_parameter_catalog_restore(&a->parameters, "friction", "0.25");
    }
    open_project(a);
    if (!a->loaded || a->project_settings_dirty)
        return false;
    int expected = strcmp(mode, "project-settings-debug") != 0;
    if (a->profile != expected)
        return false;
    if (a->dt != .125 || strcmp(a->seed, "18446744073709551615"))
        return false;
    bool restored = false;
    for (uint32_t i = 0; i < a->parameters.count; i++)
        if (!strcmp(a->parameters.entries[i].name, "friction") &&
            !strcmp(a->parameters.selected[i], "0.25"))
            restored = true;
    return restored;
}
static void project_settings_test_frame(app *a, const char *mode, int *stage, int *code) {
    static int reported_stage = -1;
    if (*stage != reported_stage) {
        fprintf(stderr, "Project settings %s stage %d: %s\n", mode, *stage, a->status);
        reported_stage = *stage;
    }
    if (a->library_thread)
        return;
    bool read = !strcmp(mode, "project-settings-read");
    if (read && *stage >= 2 && *stage <= 3 && !a->runner.running) {
        fprintf(stderr, "Runner stopped before timestep verification: %s\n", a->status);
        *code = 1;
        a->quitting = true;
        return;
    }
    if (read && *stage == 0) {
        build_project(a);
        if (!a->job.running) {
            *code = 1;
            a->quitting = true;
        }
        ++*stage;
        return;
    }
    if (read && *stage == 1) {
        if (a->job.running)
            return;
        char path[4096];
        join(path, sizeof path, a->project, "build/Release/experiment" MODULE_EXT);
        if (!a->built || a->profile != 1 || !exists(path)) {
            *code = 1;
            a->quitting = true;
            return;
        }
        snprintf(a->seed, sizeof a->seed, "18446744073709551616");
        a->project_settings_dirty = true;
        start_run(a);
        if (a->runner.running || a->last_run[0] || !strstr(a->status, "Zufallsseed") ||
            !a->project_settings_dirty) {
            *code = 1;
            a->quitting = true;
            return;
        }
        save_project(a);
        if (!a->project_settings_dirty) {
            *code = 1;
            a->quitting = true;
            return;
        }
        snprintf(a->seed, sizeof a->seed, "%llu", (unsigned long long)UINT64_MAX);
        start_run(a);
        if (!a->runner.running || a->project_settings_dirty || !a->built) {
            *code = 1;
            a->quitting = true;
            return;
        }
        *stage = 2;
        return;
    }
    if (read && *stage == 2) {
        if (!a->hello)
            return;
        /* The app sends RUN after HELLO. Pause before requesting a single step. */
        if (!command(a, PS_MSG_PAUSE) || !command(a, PS_MSG_STEP)) {
            *code = 1;
            a->quitting = true;
            return;
        }
        *stage = 3;
        return;
    }
    if (read && *stage == 3) {
        if (a->simulation_time == 0)
            return;
        if (a->simulation_time < .125 || fmod(a->simulation_time, .125) != 0) {
            *code = 1;
            a->quitting = true;
            return;
        }
        command(a, PS_MSG_STOP);
        a->stop_at = ps_clock();
        *stage = 4;
        return;
    }
    if (read && *stage == 4) {
        if (!idle(a))
            return;
        ps_run_reader run;
        if (ps_run_open(&run, a->last_run) != PS_OK) {
            *code = 1;
            a->quitting = true;
            return;
        }
        bool valid = strstr(run.metadata, "\ndt_s=0.125\n") &&
                     strstr(run.metadata, "\nseed=18446744073709551615\n");
        double time, values[PS_MAX_CHANNELS];
        valid = valid && ps_run_next(&run, &time, values) == PS_OK && time == 0 &&
                ps_run_next(&run, &time, values) == PS_OK && time == .125;
        double previous_time = .125;
        ps_result next = PS_INVALID;
        while (valid && (next = ps_run_next(&run, &time, values)) == PS_OK) {
            valid = time - previous_time == .125;
            previous_time = time;
        }
        valid = valid && next == PS_EOF;
        ps_run_reader_close(&run);
        if (!valid) {
            *code = 1;
            a->quitting = true;
            return;
        }
        select_build_profile(a, 0);
        if (a->built || !a->project_settings_dirty) {
            *code = 1;
            a->quitting = true;
            return;
        }
    }
    SDL_Event event = {0};
    event.type = SDL_EVENT_QUIT; /* Exercise normal automatic project saving. */
    if (!SDL_PushEvent(&event)) {
        *code = 1;
        a->quitting = true;
    }
    printf("PROJECT SETTINGS %s: %s\n", mode, *code ? "FAILED" : "PASSED");
    ++*stage;
}
