/* Real widgets and runner processes verify live pacing without changing dt. */
static bool speed_test_start(app *a, const char *root, const char *mode) {
    join(a->project_input, sizeof a->project_input, root, "project");
    if (!strcmp(mode, "speed-read")) {
        open_project(a);
        return a->loaded && a->simulation_speed == 0 && a->dt == .005;
    }
    a->example = !strcmp(mode, "speed-phys") ? 10 : 5;
    new_project(a);
    if (!a->loaded || a->simulation_speed != 1) return false;
    select_build_profile(a, 1);
    return true;
}
static void speed_test_frame(app *a, const char *mode, int *stage, int *code, const char **capture) {
    static double held_time, held_wall, initial[PS_MAX_CHANNELS], first[PS_MAX_CHANNELS];
    static char original[4096];
    static uint32_t channels;
    static int reported = -1;
    if (!strcmp(mode, "speed-read")) {
        a->quitting = true;
        printf("SPEED %s SELF-TEST: PASSED\n", mode);
        return;
    }
    if (reported != *stage) {
        fprintf(stderr, "Speed %s stage %d: time=%.3f speed=%.3g %s\n", mode, *stage,
                a->simulation_time, a->simulation_speed, a->status);
        reported = *stage;
    }
    bool ok = true;
    switch (*stage) {
    case 0: build_project(a); break;
    case 1:
        if (!idle(a) || a->library_thread) return;
        ok = a->built;
        start_run(a);
        snprintf(original, sizeof original, "%s", a->last_run);
        break;
    case 2:
        if (!a->hello || a->simulation_time < .1) return;
        test_mouse(a, a->run_control_bounds[1], true); break;
    case 3: test_mouse(a, a->run_control_bounds[1], false); break;
    case 4:
        if (!a->paused) return;
        held_time = a->simulation_time;
        test_mouse(a, a->speed_bounds, true); break;
    case 5: test_mouse(a, a->speed_bounds, false); break;
    case 6: test_mouse(a, a->speed_choices[0], true); break;
    case 7: test_mouse(a, a->speed_choices[0], false); break;
    case 8:
        ok = a->simulation_speed == .25 && a->paused && a->simulation_time == held_time;
        *capture = "speed-paused.bmp";
        test_mouse(a, a->run_control_bounds[2], true); break;
    case 9: test_mouse(a, a->run_control_bounds[2], false); break;
    case 10:
        if (a->simulation_time == held_time) return;
        ok = a->paused && fabs(a->simulation_time - held_time - .005) < 1e-10;
        held_time = a->simulation_time;
        held_wall = ps_clock();
        test_mouse(a, a->run_control_bounds[1], true); break;
    case 11: test_mouse(a, a->run_control_bounds[1], false); break;
    case 12:
        if (ps_clock() - held_wall < .8) return;
        ok = !a->paused && a->simulation_time - held_time > .1 &&
             a->simulation_time - held_time < .32;
        test_mouse(a, a->run_control_bounds[1], true); break;
    case 13: test_mouse(a, a->run_control_bounds[1], false); break;
    case 14:
        if (!a->paused) return;
        test_mouse(a, a->speed_bounds, true); break;
    case 15: test_mouse(a, a->speed_bounds, false); break;
    case 16: test_mouse(a, a->speed_choices[4], true); break;
    case 17: test_mouse(a, a->speed_choices[4], false); break;
    case 18:
        ok = a->simulation_speed == 4 && a->paused;
        held_time = a->simulation_time;
        held_wall = ps_clock();
        test_mouse(a, a->run_control_bounds[1], true); break;
    case 19: test_mouse(a, a->run_control_bounds[1], false); break;
    case 20:
        if (ps_clock() - held_wall < .5) return;
        ok = !a->paused && a->simulation_time - held_time > 1.4 &&
             a->simulation_time - held_time < 2.8;
        /* Change speed through the widget while physics is still running. */
        test_mouse(a, a->speed_bounds, true); break;
    case 21: test_mouse(a, a->speed_bounds, false); break;
    case 22: test_mouse(a, a->speed_choices[2], true); break;
    case 23: test_mouse(a, a->speed_choices[2], false); break;
    case 24:
        ok = a->simulation_speed == 1 && !a->paused;
        held_time = a->simulation_time;
        held_wall = ps_clock();
        *capture = "speed-live.bmp";
        break;
    case 25:
        if (ps_clock() - held_wall < .5) return;
        ok = !a->paused && a->simulation_time - held_time > .25 &&
             a->simulation_time - held_time < .85;
        test_mouse(a, a->run_control_bounds[1], true); break;
    case 26: test_mouse(a, a->run_control_bounds[1], false); break;
    case 27:
        if (!a->paused) return;
        held_time = a->simulation_time;
        test_mouse(a, a->speed_bounds, true); break;
    case 28: test_mouse(a, a->speed_bounds, false); break;
    case 29: test_mouse(a, a->speed_choices[7], true); break;
    case 30: test_mouse(a, a->speed_choices[7], false); break;
    case 31:
        ok = a->simulation_speed == 0 && a->paused && a->simulation_time == held_time;
        held_wall = ps_clock(); break;
    case 32:
        if (ps_clock() - held_wall < .2) return;
        ok = a->paused && a->simulation_time == held_time;
        test_mouse(a, a->run_control_bounds[2], true); break;
    case 33: test_mouse(a, a->run_control_bounds[2], false); break;
    case 34:
        if (a->simulation_time == held_time) return;
        ok = a->paused && fabs(a->simulation_time - held_time - .005) < 1e-10;
        held_time = a->simulation_time;
        test_mouse(a, a->run_control_bounds[1], true); break;
    case 35: test_mouse(a, a->run_control_bounds[1], false); break;
    case 36:
        if (a->simulation_time < held_time + 1) return;
        ok = !a->paused && a->dt == .005;
        test_mouse(a, a->run_control_bounds[3], true); break;
    case 37: test_mouse(a, a->run_control_bounds[3], false); break;
    case 38: {
        if (a->reset_pending || a->reset_starting || !strcmp(a->last_run, original) || !a->hello) return;
        ps_run_reader reader;
        ok = ps_run_open(&reader, original) == PS_OK;
        if (ok) {
            channels = reader.channels;
            double t;
            ok = ps_run_next(&reader, &t, initial) == PS_OK && t == 0 &&
                 ps_run_next(&reader, &t, first) == PS_OK && t == .005;
            ps_run_reader_close(&reader);
        }
        ok &= a->paused && a->simulation_time == 0 && a->simulation_speed == 0 &&
              a->channel_count == channels && reset_test_values(a->values, initial, channels);
        *capture = "speed-reset.bmp";
        test_mouse(a, a->run_control_bounds[2], true); break;
    }
    case 39: test_mouse(a, a->run_control_bounds[2], false); break;
    case 40:
        if (a->simulation_time == 0) return;
        ok = a->paused && a->simulation_time == .005 && reset_test_values(a->values, first, channels);
        test_mouse(a, a->run_control_bounds[4], true); break;
    case 41: test_mouse(a, a->run_control_bounds[4], false); break;
    case 42:
        if (!idle(a) || a->library_thread) return;
        save_project(a);
        ok = !a->project_settings_dirty && a->dt == .005;
        a->quitting = true; break;
    }
    if (!ok) {
        fprintf(stderr, "Speed failure stage %d: time %.17g paused %d speed %.17g\n",
                *stage, a->simulation_time, a->paused, a->simulation_speed);
        *code = 1;
        a->quitting = true;
    }
    if (a->quitting) printf("SPEED %s SELF-TEST: %s\n", mode, *code ? "FAILED" : "PASSED");
    (*stage)++;
}
