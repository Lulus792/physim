/* App-level recovery regression: writer exits without the normal save/cleanup;
 * a fresh process exercises the recovery buttons through SDL events. */
static const char *autosave_main_marker = "\n/* autosave main ä 1 */\n";
static const char *autosave_analysis_marker = "\n/* autosave analysis ä */\n";
static bool test_source_has(app *a, const char *name, const char *needle) {
    char path[4096];
    join(path, sizeof path, a->project, name);
    size_t length = 0;
    char *source = load_utf8_text(path, &length);
    bool found = source && strstr(source, needle);
    free(source);
    return found;
}
static bool test_autosave_crash(app *a) {
    if (!a->loaded || a->recovery)
        return false;
    nk_str_append_text_char(&a->experiment.string, autosave_main_marker,
                            (int)strlen(autosave_main_marker));
    nk_str_append_text_char(&a->analysis.string, autosave_analysis_marker,
                            (int)strlen(autosave_analysis_marker));
    a->dirty = a->analysis_dirty = true;
    char path[4096];
    autosave_path(a, path, sizeof path);
    double due = a->autosave_due;
    autosave_tick(a, due - 1);
    if (exists(path))
        return false;
    autosave_tick(a, due);
    ps_autosave *snapshot = NULL;
    if (ps_autosave_read(path, &snapshot) != PS_OK)
        return false;
    bool ok = strstr(snapshot->text[0], autosave_main_marker) &&
              strstr(snapshot->text[1], autosave_analysis_marker) &&
              !strstr(snapshot->text[2], autosave_main_marker) &&
              !strstr(snapshot->text[3], autosave_analysis_marker);
    ps_autosave_destroy(snapshot);
    /* Change one byte, preserving the length: timer must replace the generation. */
    char *source = nk_str_get(&a->experiment.string);
    char *marker = strstr(source, "main ä 1");
    if (!marker)
        return false;
    marker[strlen("main ä ")] = '2';
    autosave_tick(a, due + 30);
    snapshot = NULL;
    if (ps_autosave_read(path, &snapshot) != PS_OK)
        return false;
    ok = ok && strstr(snapshot->text[0], "autosave main ä 2") &&
         !test_source_has(a, "main.c", "autosave main") &&
         !test_source_has(a, "analysis.c", "autosave analysis");
    ps_autosave_destroy(snapshot);
    return ok;
}
static bool test_recovery_result(app *a, bool restore) {
    char path[4096];
    autosave_path(a, path, sizeof path);
    if (a->recovery || a->built || a->job.running || a->runner.running ||
        test_source_has(a, "main.c", "autosave main") ||
        test_source_has(a, "analysis.c", "autosave analysis"))
        return false;
    if (!restore)
        return !a->dirty && !a->analysis_dirty && !exists(path);
    if (!a->dirty || !a->analysis_dirty || !exists(path) ||
        !strstr(nk_str_get_const(&a->experiment.string), "autosave main ä 2") ||
        !strstr(nk_str_get_const(&a->analysis.string), "autosave analysis ä"))
        return false;
    save_project(a);
    return !a->dirty && !a->analysis_dirty && !exists(path) &&
           test_source_has(a, "main.c", "autosave main ä 2") &&
           test_source_has(a, "analysis.c", "autosave analysis ä");
}
