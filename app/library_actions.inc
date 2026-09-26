static int selected_run(const app *a, const char *path) {
    for (unsigned i = 0; i < a->selected_count; i++)
        if (!strcmp(path, a->selected_runs[i]))
            return (int)i;
    return -1;
}
static void select_run(app *a, const char *path, bool selected) {
    int index = selected_run(a, path);
    if (index >= 0 && !selected) {
        if ((unsigned)index + 1 < a->selected_count)
            memmove(a->selected_runs[index], a->selected_runs[index + 1],
                    (a->selected_count - (unsigned)index - 1) * sizeof a->selected_runs[0]);
        a->selected_count--;
    } else if (index < 0 && selected) {
        if (a->selected_count == PS_ANALYSIS_MAX_INPUTS) {
            status(a, "Höchstens acht Läufe gleichzeitig auswählen.");
            return;
        }
        snprintf(a->selected_runs[a->selected_count++], sizeof a->selected_runs[0], "%s", path);
    }
}
static int scan_library(void *user) {
    app *a = user;
    a->library_result = ps_library_scan(a->library_directory, &a->pending_library);
    SDL_SetAtomicInt(&a->library_done, 1);
    return 0;
}
static void refresh_library(app *a) {
    if (!a->loaded)
        return;
    if (a->library_thread) {
        a->library_again = true;
        return;
    }
    join(a->library_directory, sizeof a->library_directory, a->project, "runs");
    if (!ps_make_directory(a->library_directory)) {
        snprintf(a->library_error, sizeof a->library_error, "Der Laufordner ist nicht zugänglich.");
        return;
    }
    SDL_SetAtomicInt(&a->library_done, 0);
    a->library_error[0] = 0;
    a->library_thread = SDL_CreateThread(scan_library, "physim-library", a);
    if (!a->library_thread)
        snprintf(a->library_error, sizeof a->library_error,
                 "Dateiliste konnte nicht geladen werden.");
}
static void pump_library(app *a) {
    if (!a->library_thread || !SDL_GetAtomicInt(&a->library_done))
        return;
    SDL_WaitThread(a->library_thread, NULL);
    a->library_thread = NULL;
    if (a->library_result == PS_OK) {
        ps_library_destroy(a->library);
        a->library = a->pending_library;
        a->pending_library = NULL;
        if (!a->library->limited && !a->library->skipped && !a->job.running && !a->runner.running &&
            !a->library_again) {
            for (unsigned i = 0; i < a->selected_count;) {
                bool found = false;
                for (size_t j = 0; j < a->library->count; j++) {
                    char path[4096];
                    if (a->library->items[j].kind == PS_LIBRARY_RUN &&
                        ps_library_path(a->library, j, path, sizeof path) == PS_OK &&
                        !strcmp(path, a->selected_runs[i])) {
                        found = true;
                        break;
                    }
                }
                if (found)
                    i++;
                else
                    select_run(a, a->selected_runs[i], false);
            }
        }
    } else
        snprintf(a->library_error, sizeof a->library_error,
                 "Dateiliste konnte nicht aktualisiert werden: %s",
                 ps_result_string(a->library_result));
    if (a->library_again) {
        a->library_again = false;
        refresh_library(a);
    }
}
static void open_library(app *a) {
    if (!a->loaded) {
        status(a, "Zuerst ein Projekt öffnen oder anlegen.");
        return;
    }
    a->tab = 4;
    refresh_library(a);
}
static void open_library_item(app *a, size_t index) {
    if (!idle(a) || !a->library || index >= a->library->count)
        return;
    char path[4096];
    if (ps_library_path(a->library, index, path, sizeof path) != PS_OK) {
        status(a, "Dateipfad ist zu lang.");
        return;
    }
    a->tab = 2;
    a->analysis_tab = 0;
    if (a->library->items[index].kind == PS_LIBRARY_REPORT)
        request_report_path(a, path);
    else {
        snprintf(a->last_run, sizeof a->last_run, "%s", path);
        a->show_report = false;
        request_dataset(a);
    }
}
