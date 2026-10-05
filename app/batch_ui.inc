static void batch_view(app *a) {
    struct nk_context *ui = a->ui;
    bool running = a->batch_thread != NULL;
    nk_style_push_font(ui, a->font_title);
    nk_layout_row_dynamic(ui, 36, 1);
    nk_label(ui, "Laufserien", NK_TEXT_LEFT);
    nk_style_pop_font(ui);
    nk_layout_row_dynamic(ui, 46, 1);
    nk_label_wrap(ui, "Wiederhole dein Experiment mit aufeinanderfolgenden Seeds oder variiere "
                      "einen definierten Parameter. Vergleiche den Endwert eines Messkanals.");
    nk_layout_row_dynamic(ui, 38, 3);
    nk_begin_disabled(ui, !idle(a) || !a->built || a->dirty);
    a->batch_start_bounds = nk_widget_bounds(ui);
    if (action_button(ui, a->batch_sweep ? "Studie starten" : "Serie starten", true))
        start_batch(a);
    nk_end_disabled(ui);
    nk_begin_disabled(ui, !running);
    a->batch_cancel_bounds = nk_widget_bounds(ui);
    if (nk_button_label(ui, "Abbrechen"))
        SDL_SetAtomicInt(&a->batch_cancel, 1);
    nk_end_disabled(ui);
    if (nk_button_label(ui, "Anleitung"))
        open_documentation(a, DOCUMENTATION_MONTE_CARLO_TOPIC);
    nk_begin_disabled(ui, running);
    nk_layout_row_dynamic(ui, 30, 1);
    a->batch_sweep_bounds = nk_widget_bounds(ui);
    nk_checkbox_label(ui, "Parameterstudie", &a->batch_sweep);
    if (a->batch_sweep) {
        if (a->parameters.count) {
            if (a->batch_sweep_parameter < 0 ||
                a->batch_sweep_parameter >= (int)a->parameters.count)
                a->batch_sweep_parameter = 0;
            const char *names[PS_MAX_PARAMETERS];
            for (uint32_t i = 0; i < a->parameters.count; i++)
                names[i] = a->parameters.entries[i].name;
            nk_layout_row_dynamic(ui, 24, 1);
            nk_label(ui, "Experimentparameter", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ui, 32, 1);
            int previous = a->batch_sweep_parameter;
            a->batch_sweep_parameter = nk_combo(ui, names, (int)a->parameters.count,
                                                 a->batch_sweep_parameter, 28, nk_vec2(280, 240));
            const ps_parameter *parameter = &a->parameters.entries[a->batch_sweep_parameter];
            if (!a->batch_sweep_ready || previous != a->batch_sweep_parameter) {
                snprintf(a->batch_sweep_start, sizeof a->batch_sweep_start, "%.17g",
                         parameter->minimum);
                snprintf(a->batch_sweep_end, sizeof a->batch_sweep_end, "%.17g",
                         parameter->maximum);
                a->batch_sweep_ready = true;
            }
            char description[256];
            snprintf(description, sizeof description, "%s · Bereich %.6g bis %.6g",
                     parameter->description, parameter->minimum, parameter->maximum);
            nk_layout_row_dynamic(ui, 40, 1);
            nk_label_wrap(ui, description);
            nk_layout_row_dynamic(ui, 22, 2);
            nk_label(ui, "Startwert", NK_TEXT_LEFT);
            nk_label(ui, "Endwert", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ui, 34, 2);
            nk_edit_string_zero_terminated(ui, NK_EDIT_FIELD, a->batch_sweep_start,
                                           sizeof a->batch_sweep_start, nk_filter_default);
            nk_edit_string_zero_terminated(ui, NK_EDIT_FIELD, a->batch_sweep_end,
                                           sizeof a->batch_sweep_end, nk_filter_default);
        } else {
            nk_layout_row_dynamic(ui, 42, 1);
            nk_label_wrap(ui, "Dieses Experiment definiert keine Parameter. "
                              "Füge parameter(...) hinzu und baue es erneut.");
        }
    }
    nk_layout_row_dynamic(ui, 22, 2);
    nk_label(ui, "Läufe", NK_TEXT_LEFT);
    nk_label(ui, a->batch_target?"Maximale akzeptierte Schritte":"Schritte pro Lauf", NK_TEXT_LEFT);
    nk_layout_row_dynamic(ui, 34, 2);
    nk_property_int(ui, "#Anzahl", 1, &a->batch_runs, 1000, 1, 1);
    nk_property_int(ui, "#Schritte", 1, &a->batch_steps, 100000, 10, 1);
    nk_layout_row_dynamic(ui, 22, 2);
    nk_label(ui, a->batch_adaptive?"Startschritt in Sekunden":"Zeitschritt in Sekunden", NK_TEXT_LEFT);
    nk_label(ui, "Startseed", NK_TEXT_LEFT);
    nk_layout_row_dynamic(ui, 34, 2);
    a->batch_timing_bounds[0]=nk_widget_bounds(ui);
    if(a->batch_adaptive)
        timing_text_field(ui,a->batch_timing_text[0],&a->batch_timing_displayed[0],&a->batch_dt);
    else nk_property_double(ui, "#dt", .000001, &a->batch_dt, 1, .001, .0001f);
    nk_edit_string_zero_terminated(ui, NK_EDIT_FIELD, a->batch_seed, sizeof a->batch_seed,
                                   nk_filter_decimal);
    nk_layout_row_dynamic(ui, 22, 2);
    nk_label(ui, "Endwert-Kanal (Name aus dem Experiment)", NK_TEXT_LEFT);
    nk_label(ui, "Gleichzeitige Läufe", NK_TEXT_LEFT);
    nk_layout_row_dynamic(ui, 34, 2);
    nk_edit_string_zero_terminated(ui, NK_EDIT_FIELD, a->batch_channel, sizeof a->batch_channel,
                                   nk_filter_ascii);
    a->batch_workers_bounds = nk_widget_bounds(ui);
    nk_property_int(ui, "#Parallel", 1, &a->batch_workers, PS_BATCH_MAX_WORKERS, 1, 1);
    nk_layout_row_dynamic(ui, 22, 2);
    nk_label(ui, "Zeitlimit je Lauf (Sekunden)", NK_TEXT_LEFT);
    nk_label(ui, "Speicher je Lauf (MiB, 0 = ohne)", NK_TEXT_LEFT);
    nk_layout_row_dynamic(ui, 34, 2);
    nk_property_double(ui, "#Limit", 1, &a->batch_timeout, 3600, 5, 1);
    nk_property_int(ui, "#Speicher", 0, &a->runner_memory_mib, 16384, 128, 16);
    nk_layout_row_dynamic(ui,30,2);
    a->batch_target_bounds=nk_widget_bounds(ui);
    nk_begin_disabled(ui,a->batch_adaptive);
    nk_checkbox_label(ui,"Gemeinsame Endzeit",&a->batch_target);
    nk_end_disabled(ui);
    a->batch_adaptive_bounds=nk_widget_bounds(ui);
    nk_checkbox_label(ui,"Adaptive Schritte",&a->batch_adaptive);
    if(a->batch_adaptive) a->batch_target=true;
    if(a->batch_target) {
        nk_layout_row_dynamic(ui,32,2);nk_label(ui,"Endzeit (s)",NK_TEXT_LEFT);
        a->batch_timing_bounds[1]=nk_widget_bounds(ui);
        timing_text_field(ui,a->batch_timing_text[1],&a->batch_timing_displayed[1],&a->batch_end_time);
    }
    if(a->batch_adaptive) {
        nk_layout_row_dynamic(ui,22,2);
        nk_label(ui,"Minimum (s)",NK_TEXT_LEFT);nk_label(ui,"Maximum (s)",NK_TEXT_LEFT);
        nk_layout_row_dynamic(ui,32,2);
        a->batch_timing_bounds[2]=nk_widget_bounds(ui);
        timing_text_field(ui,a->batch_timing_text[2],&a->batch_timing_displayed[2],&a->batch_minimum_dt);
        a->batch_timing_bounds[3]=nk_widget_bounds(ui);
        timing_text_field(ui,a->batch_timing_text[3],&a->batch_timing_displayed[3],&a->batch_maximum_dt);
        nk_layout_row_dynamic(ui,52,1);
        nk_label_wrap(ui,"Minimum ≤ Startschritt ≤ Maximum ≤ 1 s. Das Modell muss adaptive Schritte unterstützen; der letzte Schritt endet an der Zielzeit.");
    }
    nk_end_disabled(ui);
    char text[256];
    snprintf(text, sizeof text, "Endzeit %.6g s · %s %llu Samples · Seed pro Lauf: Startseed + Index",
             a->batch_target?a->batch_end_time:a->batch_steps * a->batch_dt,a->batch_target?"höchstens":"",
             (unsigned long long)a->batch_runs * ((unsigned)a->batch_steps + 1u));
    nk_layout_row_dynamic(ui, 32, 1);
    nk_label_wrap(ui, text);
    if (a->batch_options.runs) {
        unsigned completed = (unsigned)SDL_GetAtomicInt(&a->batch_completed);
        snprintf(text, sizeof text, "%u / %u abgeschlossen · %d aktiv · bis zu %u gleichzeitig",
                 completed, a->batch_options.runs, SDL_GetAtomicInt(&a->batch_active),
                 a->batch_options.workers);
        nk_layout_row_dynamic(ui, 28, 1);
        nk_label(ui, text, NK_TEXT_LEFT);
        nk_size progress = completed;
        nk_layout_row_dynamic(ui, 8, 1);
        nk_progress(ui, &progress, a->batch_options.runs, false);
    }
    nk_layout_row_dynamic(ui, 64, 1);
    nk_label_wrap(ui, a->batch_sweep
                      ? "Die Studie variiert den Parameter linear. Auch der Seed steigt je Lauf; "
                        "bei stochastischen Modellen können beide Effekte die Kurve verändern."
                      : "Das Experiment muss den Seed verwenden, damit Ergebnisse variieren. "
                        "Die Vorlage „Wurf mit Unsicherheit“ zeigt Sollbahn, zufälligen "
                        "Anfangszustand und verrauschte Sensormessung.");
    if (!running && a->batch_options.directory[0]) {
        nk_layout_row_dynamic(ui, 32, 2);
        if (nk_button_label(ui, "Serienordner öffnen")) {
            char uri[4200];
            snprintf(uri, sizeof uri, "file:///%s", a->batch_options.directory);
            SDL_OpenURL(uri);
        }
        nk_begin_disabled(ui, !a->batch_last_report[0] || !idle(a));
        if (nk_button_label(ui, "Bericht öffnen")) {
            request_report_path(a, a->batch_last_report);
            a->tab = 2;
            a->analysis_tab = 0;
        }
        nk_end_disabled(ui);
    }
}
