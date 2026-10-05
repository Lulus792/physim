static uint32_t timeline_test_crc(const ps_snapshot *s) {
    ps_context context = {.time_s = s->time, .channel_count = s->count};
    memcpy(context.values, s->values, sizeof context.values);
    unsigned char bytes[PS_SNAPSHOT_MAX];
    size_t size = ps_snapshot_encode(bytes, &context, &s->scene, s->paused);
    return size ? ps_crc32(bytes, size) : 0;
}
static bool timeline_test_start(app *a, const char *root, const char *mode) {
    join(a->project_input, sizeof a->project_input, root, "project");
    bool create = !strcmp(mode, "timeline-c") || !strcmp(mode, "timeline-phys");
    if (create) {
        a->example = !strcmp(mode, "timeline-c") ? 5 : 10;
        new_project(a);
        if (!a->loaded) return false;
        a->dt = .01;
        a->project_settings_dirty = true;
        select_build_profile(a, 1);
    } else open_project(a);
    return a->loaded;
}
static void timeline_test_frame(app *a, const char *mode, int *stage, int *code, const char **capture) {
    bool create = !strcmp(mode, "timeline-c") || !strcmp(mode, "timeline-phys");
    static double held_time, held_wall;
    static ps_snapshot frozen;
    static uint32_t crc, file_crc;
    static char original[4096], target[4096];
    static int reported = -1, row;
    bool ok = true;
    if (reported != *stage) {
        fprintf(stderr, "Timeline %s stage %d: live=%.4f viewed=%.4f browse=%d play=%d %s\n",
                mode, *stage, a->simulation_time, display_time(a), a->timeline_browsing,
                a->timeline_playing, a->status);
        reported = *stage;
    }
    switch (*stage) {
    case 0:
        if (create) build_project(a);
        else {
            char name[96]; snprintf(name, sizeof name, "runs/%s.psrun", mode);
            join(target, sizeof target, a->project, name);
            ok = reset_test_crc(target, &file_crc);
            open_library(a);
        }
        break;
    case 1:
        if (!idle(a) || a->library_thread) return;
        if (create) {
            ok = a->built;
            start_run(a);
            snprintf(original, sizeof original, "%s", a->last_run);
        } else {
            row = -1;
            for (unsigned i = 0; i < a->library_visible_count; i++) {
                char path[4096];
                if (ps_library_path(a->library, a->library_visible_indices[i], path, sizeof path) == PS_OK &&
                    !strcmp(path, target)) row = (int)i;
            }
            if (row < 0) return;
            test_mouse(a, a->library_open_bounds[row], true);
            *stage = 49;
        }
        break;
    case 2:
        if (!a->hello || a->simulation_time < .2 || ps_timeline_count(&a->timeline) < 4) return;
        held_time = a->simulation_time;
        test_mouse(a, a->timeline_bounds[0], true); break;
    case 3: test_mouse(a, a->timeline_bounds[0], false); break;
    case 4:
        ok = a->timeline_browsing && !a->paused && display_time(a) < a->simulation_time &&
             display_scene(a) == &a->timeline_view.scene;
        frozen = a->timeline_view;
        crc = timeline_test_crc(&frozen);
        held_wall = ps_clock();
        *capture = "timeline-live-review.bmp"; break;
    case 5:
        ok = timeline_test_crc(&a->timeline_view) == crc && a->timeline_browsing;
        if (ps_clock() - held_wall < .15) return;
        ok &= a->simulation_time > held_time;
        test_mouse(a, a->timeline_bounds[2], true); break;
    case 6: test_mouse(a, a->timeline_bounds[2], false); break;
    case 7:
        ok = !a->timeline_browsing && display_scene(a) == &a->scene && !a->paused;
        test_mouse(a, a->run_control_bounds[1], true); break;
    case 8: test_mouse(a, a->run_control_bounds[1], false); break;
    case 9:
        if (!a->paused) return;
        held_time = a->simulation_time;
        break;
    case 10: {
        struct nk_rect seek = a->timeline_bounds[4];
        if (!strcmp(mode, "timeline-phys")) seek.x += seek.w - a->ui->style.slider.padding.x - 1;
        seek.w = 1;
        test_mouse(a, seek, true); break;
    }
    case 11: {
        struct nk_rect left = a->timeline_bounds[4]; left.w = 1;
        if (!strcmp(mode, "timeline-phys")) test_plot_motion(a, left.x + .5f, left.y + left.h * .5f);
        break;
    }
    case 12: {
        struct nk_rect left = a->timeline_bounds[4]; left.w = 1;
        test_mouse(a, left, false); break;
    }
    case 13:
        ok = a->timeline_browsing && display_time(a) == ps_timeline_get(&a->timeline, 0)->time &&
             a->simulation_time == held_time;
        *capture = "timeline-initial.bmp";
        test_mouse(a, a->timeline_bounds[1], true); break;
    case 14: test_mouse(a, a->timeline_bounds[1], false); break;
    case 15: {
        const ps_snapshot *expected = ps_timeline_get(&a->timeline, 1);
        ok = expected && timeline_test_crc(&a->timeline_view) == timeline_test_crc(expected) &&
             a->simulation_time == held_time && !a->timeline_playing;
        test_mouse(a, a->timeline_bounds[0], true); break;
    }
    case 16: test_mouse(a, a->timeline_bounds[0], false); break;
    case 17:
        ok = display_time(a) == ps_timeline_get(&a->timeline, 0)->time && a->simulation_time == held_time;
        if (!strcmp(mode, "timeline-phys")) test_window_key_mod(a->window, SDLK_SPACE, SDL_KMOD_NONE);
        else test_mouse(a, a->timeline_bounds[3], true);
        break;
    case 18:
        if (strcmp(mode, "timeline-phys")) test_mouse(a, a->timeline_bounds[3], false);
        break;
    case 19:
        ok = a->timeline_browsing && a->simulation_time == held_time &&
             (create ? a->paused : !a->runner.running);
        if (a->timeline_playing) return;
        ok &= display_time(a) == a->timeline.latest.time;
        *capture = "timeline-playback-end.bmp";
        test_mouse(a, a->timeline_bounds[2], true); break;
    case 20: test_mouse(a, a->timeline_bounds[2], false); break;
    case 21:
        ok = !a->timeline_browsing && !a->timeline_playing;
        if (!create) {
            uint32_t after;
            ok &= reset_test_crc(target, &after) && after == file_crc && !a->built;
            a->quitting = true;
        } else test_mouse(a, a->timeline_bounds[0], true);
        break;
    case 22: test_mouse(a, a->timeline_bounds[0], false); break;
    case 23:
        ok = a->timeline_browsing;
        test_mouse(a, a->run_control_bounds[3], true); break;
    case 24: test_mouse(a, a->run_control_bounds[3], false); break;
    case 25:
        if (a->reset_pending || a->reset_starting || !strcmp(a->last_run, original) || !a->hello) return;
        ok = !a->timeline_browsing && !a->timeline_playing && a->paused && a->simulation_time == 0 &&
             ps_timeline_count(&a->timeline) == 1;
        test_mouse(a, a->run_control_bounds[4], true); break;
    case 26: test_mouse(a, a->run_control_bounds[4], false); break;
    case 27:
        if (!idle(a) || a->library_thread) return;
        ok = ps_timeline_count(&a->timeline) == 1 && !a->timeline_browsing;
        a->quitting = true; break;
    case 50: test_mouse(a, a->library_open_bounds[row], false); break;
    case 51:
        if (!idle(a) || a->library_thread) return;
        ok = !strcmp(a->last_run, target) && ps_timeline_count(&a->timeline) > 3 && !a->runner.running &&
             !a->timeline_browsing && a->timeline_scenes == (strncmp(mode, "timeline-legacy", 15) != 0);
        ok &= a->data.result == (!strcmp(mode, "timeline-recovered") ? PS_RECOVERED : PS_EOF);
        test_mouse(a, a->navigation_bounds[1], true); break;
    case 52: test_mouse(a, a->navigation_bounds[1], false); break;
    case 53:
        ok = a->tab == 1;
        held_time = a->simulation_time;
        *stage = 9; break;
    }
    if (!ok) {
        fprintf(stderr, "Timeline failed stage %d: count=%u live=%.17g view=%.17g\n",
                *stage, ps_timeline_count(&a->timeline), a->simulation_time, display_time(a));
        *code = 1; a->quitting = true;
    }
    if (a->quitting) printf("TIMELINE %s SELF-TEST: %s\n", mode, *code ? "FAILED" : "PASSED");
    (*stage)++;
}
