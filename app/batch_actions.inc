static bool batch_continue(uint32_t completed, uint32_t active, void *user) {
    app *a = user;
    SDL_SetAtomicInt(&a->batch_completed, (int)completed);
    SDL_SetAtomicInt(&a->batch_active, (int)active);
    return !SDL_GetAtomicInt(&a->batch_cancel);
}
static int batch_worker(void *user) {
    app *a = user;
    a->batch_code = ps_batch_run(&a->batch_options, batch_continue, a, &a->batch_result);
    SDL_SetAtomicInt(&a->batch_done, 1);
    return 0;
}
static void start_batch(app *a) {
    if (a->project_settings_dirty)
        save_project(a);
    if (a->project_settings_dirty)
        return;
    if (a->recovery || !idle(a) || !a->built || a->dirty) {
        status(a, "Zuerst den gespeicherten Experimentcode erfolgreich bauen.");
        return;
    }
    uint64_t seed = 0;
    if (!a->batch_seed[0]) {
        status(a, "Startseed muss eine nichtnegative ganze Zahl sein.");
        return;
    }
    for (const char *p = a->batch_seed; *p; p++) {
        if (*p < '0' || *p > '9' || seed > (UINT64_MAX - (unsigned)(*p - '0')) / 10) {
            status(a, "Startseed ist keine gültige 64-Bit-Zahl.");
            return;
        }
        seed = seed * 10 + (unsigned)(*p - '0');
    }
    ps_batch_options selected={0};
    ps_batch_options *o = &selected;
    if(a->batch_target && (!isfinite(a->batch_end_time) || a->batch_end_time<=0)) {
        status(a,"Endzeit: positiven endlichen Wert eingeben.");return;
    }
    join(o->runner, sizeof o->runner, a->bin, "physim-runner" EXE_EXT);
    join(o->module, sizeof o->module, a->build_directory, "experiment" MODULE_EXT);
    join(o->source, sizeof o->source, a->project, experiment_source(a));
    unique_path(a, o->directory, sizeof o->directory, "-batch");
    snprintf(o->channel, sizeof o->channel, "%s", a->batch_channel);
    o->seed = seed;
    o->runs = (uint32_t)a->batch_runs;
    o->steps = (uint32_t)a->batch_steps;
    o->workers = (uint32_t)a->batch_workers;
    o->dt = a->batch_dt;
    o->end_time=a->batch_target?a->batch_end_time:0;
    o->adaptive=a->batch_adaptive;
    o->minimum_dt=a->batch_minimum_dt;o->maximum_dt=a->batch_maximum_dt;
    o->timeout_s = a->batch_timeout;
    o->memory_bytes = (uint64_t)a->runner_memory_mib * UINT64_C(1048576);
    if (a->batch_sweep) {
        if (!a->parameters.count || a->batch_sweep_parameter < 0 ||
            a->batch_sweep_parameter >= (int)a->parameters.count) {
            status(a, "Zuerst ein Experiment mit definierten Parametern bauen.");
            return;
        }
        const ps_parameter *parameter = &a->parameters.entries[a->batch_sweep_parameter];
        char *end;
        errno = 0;
        double start = strtod(a->batch_sweep_start, &end);
        if (errno || end == a->batch_sweep_start || *end || !isfinite(start)) {
            status(a, "Ungültiger Startwert der Parameterstudie.");
            return;
        }
        errno = 0;
        double finish = strtod(a->batch_sweep_end, &end);
        if (errno || end == a->batch_sweep_end || *end || !isfinite(finish) ||
            start == finish || start < parameter->minimum || start > parameter->maximum ||
            finish < parameter->minimum || finish > parameter->maximum) {
            status(a, "Parameterstudie: verschiedene endliche Werte innerhalb der Grenzen eingeben.");
            return;
        }
        o->sweep = true;
        snprintf(o->sweep_name, sizeof o->sweep_name, "%s", parameter->name);
        o->sweep_start = start;
        o->sweep_end = finish;
    }
    for (uint32_t i = 0; i < a->parameters.count; i++) {
        const ps_parameter *parameter = &a->parameters.entries[i];
        if (o->sweep && !strcmp(parameter->name, o->sweep_name))
            continue;
        double selected;
        if (ps_parameter_catalog_value(&a->parameters, i, &selected) != PS_OK) {
            status(a, "Experimentparameter müssen innerhalb ihrer Grenzen liegen.");
            return;
        }
        uint32_t index = o->parameter_count++;
        snprintf(o->parameters[index].name, sizeof o->parameters[index].name, "%s",
                 parameter->name);
        o->parameters[index].value = selected;
    }
    if (ps_batch_validate(o) != PS_OK) {
        status(a, "Serie: Zielzeit, Schrittbudget, adaptive Grenzen und Seed prüfen (maximal 5 Mio. Samples).");
        return;
    }
    char runs[4096];
    join(runs, sizeof runs, a->project, "runs");
    if (!ps_make_directory(runs)) {
        status(a, "Laufordner konnte nicht angelegt werden.");
        return;
    }
    a->batch_source = copy_editor_text(&a->experiment);
    if (!a->batch_source) {
        status(a, "Quellcode-Snapshot konnte nicht im Speicher gesichert werden.");
        return;
    }
    o->source_text = a->batch_source;
    o->source_size = strlen(a->batch_source);
    a->batch_options=selected;o=&a->batch_options;
    SDL_SetAtomicInt(&a->batch_cancel, 0);
    SDL_SetAtomicInt(&a->batch_done, 0);
    SDL_SetAtomicInt(&a->batch_completed, 0);
    SDL_SetAtomicInt(&a->batch_active, 0);
    a->batch_last_report[0] = 0;
    memset(&a->batch_result, 0, sizeof a->batch_result);
    a->batch_thread = SDL_CreateThread(batch_worker, "Monte Carlo", a);
    if (!a->batch_thread) {
        free(a->batch_source);
        a->batch_source = NULL;
        o->source_text = NULL;
        o->source_size = 0;
    }
    status(a, a->batch_thread ? (o->sweep ? "Parameterstudie gestartet. Wert und Seed je Lauf protokolliert."
                                          : "Laufserie gestartet. Jeder Lauf erhält den nächsten Seed.")
                              : "Laufserie konnte nicht gestartet werden.");
}
static void pump_batch(app *a) {
    if (!a->batch_thread || !SDL_GetAtomicInt(&a->batch_done))
        return;
    SDL_WaitThread(a->batch_thread, NULL);
    a->batch_thread = NULL;
    free(a->batch_source);
    a->batch_source = NULL;
    a->batch_options.source_text = NULL;
    a->batch_options.source_size = 0;
    SDL_SetAtomicInt(&a->batch_completed, (int)a->batch_result.completed);
    SDL_SetAtomicInt(&a->batch_active, 0);
    if (a->batch_code != PS_OK) {
        status(a, a->batch_result.error[0] ? a->batch_result.error : "Laufserie: Schreibfehler.");
    } else if (a->batch_result.cancelled) {
        status(a, "Laufserie abgebrochen. Bisherige Rohdaten und Seeds bleiben im Serienordner.");
    } else {
        char source[4096];
        join(source, sizeof source, a->batch_options.directory, "summary.psreport");
        unique_path(a, a->batch_last_report, sizeof a->batch_last_report, "-batch.psreport");
        if (!copy_file_exclusive(source, a->batch_last_report)) {
            a->batch_last_report[0] = 0;
            status(a,
                   "Serie fertig; Bericht liegt im Serienordner. Bibliothekskopie fehlgeschlagen.");
        } else {
            refresh_library(a);
            request_report_path(a, a->batch_last_report);
            a->tab = 2;
            a->analysis_tab = 0;
            status(a, "Laufserie vollständig. Verteilung und Kennzahlen werden geöffnet.");
        }
    }
}
