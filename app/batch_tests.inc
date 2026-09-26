/* Internal SDL-event workflow. Uses real compiler, runner processes and reports. */
static void test_parallel_arrow(app *a, bool down) {
    struct nk_rect arrow = a->batch_workers_bounds;
    arrow.x += arrow.w - 22;
    arrow.w = 20;
    test_mouse(a, arrow, down);
}
static void test_batch_frame(app *a, int *stage, int *code, const char **capture, int workers) {
    switch (*stage) {
    case 90:
        if (a->job.running || a->library_thread)
            break;
        if (!a->built) {
            *code = 1;
            a->quitting = true;
            break;
        }
        test_mouse(a, a->batch_navigation_bounds, true);
        *stage = 91;
        break;
    case 91:
        test_mouse(a, a->batch_navigation_bounds, false);
        *stage = 92;
        break;
    case 92:
        if (a->tab != 5)
            *code = 1;
        test_parallel_arrow(a, true);
        *stage = 110;
        break;
    case 110:
        test_parallel_arrow(a, false);
        *stage = 111;
        break;
    case 111:
        if (a->batch_workers != workers)
            *code = 1;
        *capture = "batch-setup.bmp";
        test_mouse(a, a->batch_start_bounds, true);
        *stage = 93;
        break;
    case 93:
        test_mouse(a, a->batch_start_bounds, false);
        *stage = 94;
        break;
    case 94:
        if (!a->batch_thread) {
            *code = 1;
            a->quitting = true;
            break;
        }
        if (SDL_GetAtomicInt(&a->batch_completed) < 64)
            break;
        *capture = "batch-progress.bmp";
        *stage = 95;
        break;
    case 95:
        if (!idle(a) || a->library_thread)
            break;
        if (a->batch_code != PS_OK || a->batch_result.cancelled ||
            a->batch_result.completed != 256 || a->tab != 2 ||
            a->batch_options.workers != (uint32_t)workers ||
            a->batch_result.peak_active != (uint32_t)workers ||
            strcmp(a->loaded_report_path, a->batch_last_report))
            *code = 1;
        uint32_t plots, tables;
        if (ps_report_describe(a->analysis_report, NULL, NULL, &plots, &tables) != PS_OK ||
            plots != 1 || tables != 2)
            *code = 1;
        *capture = "batch-histogram.bmp";
        a->result_view = 0;
        *stage = 96;
        break;
    case 96:
        a->result_view = 1;
        *capture = "batch-statistics.bmp";
        *stage = 97;
        break;
    case 97:
        a->result_view = 2;
        *capture = "batch-confidence.bmp";
        *stage = 98;
        break;
    case 98:
        open_documentation(a, DOCUMENTATION_MONTE_CARLO_TOPIC);
        if (!a->documentation || a->doc_error[0])
            *code = 1;
        *capture = "batch-documentation.bmp";
        *stage = 99;
        break;
    case 99:
        a->tab = 5;
        a->batch_runs = 1000;
        *stage = 100;
        break;
    case 100:
        test_mouse(a, a->batch_start_bounds, true);
        *stage = 101;
        break;
    case 101:
        test_mouse(a, a->batch_start_bounds, false);
        *stage = 102;
        break;
    case 102:
        if (!a->batch_thread) {
            *code = 1;
            a->quitting = true;
            break;
        }
        if (SDL_GetAtomicInt(&a->batch_completed) < 1)
            break;
        test_mouse(a, a->batch_cancel_bounds, true);
        *stage = 103;
        break;
    case 103:
        test_mouse(a, a->batch_cancel_bounds, false);
        *stage = 104;
        break;
    case 104:
        if (!idle(a))
            break;
        if (a->batch_code != PS_OK || !a->batch_result.cancelled || !a->batch_result.completed ||
            a->batch_result.completed >= 1000 || a->batch_last_report[0])
            *code = 1;
        char path[4096];
        join(path, sizeof path, a->batch_options.directory, "summary.psreport");
        if (exists(path))
            *code = 1;
        *capture = "batch-cancelled.bmp";
        *stage = 105;
        break;
    case 105:
        test_mouse(a, a->batch_start_bounds, true);
        *stage = 106;
        break;
    case 106:
        test_mouse(a, a->batch_start_bounds, false);
        *stage = 107;
        break;
    case 107:
        if (!a->batch_thread) {
            *code = 1;
            a->quitting = true;
            break;
        }
        if (!SDL_GetAtomicInt(&a->batch_completed))
            break;
        SDL_Event quit = {0};
        quit.type = SDL_EVENT_QUIT;
        if (!SDL_PushEvent(&quit)) {
            *code = 1;
            a->quitting = true;
        }
        *stage = 108;
        break;
    }
}
