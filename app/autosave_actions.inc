/* Private recovery actions, after editor IO and before project actions. */
static char *copy_editor_text(const struct nk_text_edit *edit) {
    int length = nk_str_len_char(&edit->string);
    const char *text = nk_str_get_const(&edit->string);
    if (length < 0 || !ps_source_text_valid(text, (size_t)length))
        return NULL;
    char *copy = malloc((size_t)length + 1);
    if (copy) {
        if (length)
            memcpy(copy, text, (size_t)length);
        copy[length] = 0;
    }
    return copy;
}
static void autosave_path(app *a, char *out, size_t capacity) {
    int n = snprintf(out, capacity, "%s/.physim-autosave", a->project);
    if (n < 0 || (size_t)n >= capacity)
        out[0] = 0;
}
static void check_recovery(app *a) {
    a->autosave_current = a->autosave_blocked = false;
    a->autosave_error[0] = 0;
    a->autosave_due = ps_clock() + a->preferences.autosave_seconds;
    char path[4096];
    autosave_path(a, path, sizeof path);
    ps_autosave *snapshot = NULL;
    ps_result r = ps_autosave_read(path, &snapshot);
    if (r == PS_EOF)
        return;
    if (r != PS_OK) {
        a->autosave_blocked = true;
        snprintf(a->autosave_error, sizeof a->autosave_error,
                 "Autosave pausiert: Sicherung nicht lesbar (%s). Datei bleibt erhalten.",
                 ps_result_string(r));
        status(a, a->autosave_error);
        return;
    }
    if(a->analysis_only && (snapshot->length[0] || snapshot->length[2])) {
        ps_autosave_destroy(snapshot);a->autosave_blocked=true;
        snprintf(a->autosave_error,sizeof a->autosave_error,"Sicherung gehört zu einem Experimentprojekt. Datei bleibt erhalten.");status(a,a->autosave_error);return;
    }
    bool equal = !strcmp(snapshot->text[0], a->saved_source[0]) &&
                 !strcmp(snapshot->text[1], a->saved_source[1]);
    if (equal) {
        ps_autosave_destroy(snapshot);
        /* A crash after saving but before removing the snapshot is harmless. */
        return;
    }
    a->recovery = snapshot;
    a->recovery_conflict = strcmp(snapshot->text[2], a->saved_source[0]) ||
                           strcmp(snapshot->text[3], a->saved_source[1]);
    status(a, "Ungespeicherte Änderungen gefunden. Wiederherstellung auswählen.");
}
static void resolve_recovery(app *a, bool restore) {
    if (!a->recovery)
        return;
    if (restore) {
        struct nk_text_edit editors[2];
        nk_textedit_init_default(&editors[0]);
        nk_textedit_init_default(&editors[1]);
        bool ok = set_editor_text(&editors[0], a->recovery->text[0], a->recovery->length[0]) &&
                  set_editor_text(&editors[1], a->recovery->text[1], a->recovery->length[1]);
        if (!ok) {
            nk_textedit_free(&editors[0]);
            nk_textedit_free(&editors[1]);
            status(a, "Wiederherstellung fehlgeschlagen: Speicher erschöpft.");
            return;
        }
        nk_textedit_free(&a->experiment);
        nk_textedit_free(&a->analysis);
        a->experiment = editors[0];
        a->analysis = editors[1];
        a->dirty = strcmp(a->recovery->text[0], a->saved_source[0]) != 0;
        a->analysis_dirty = strcmp(a->recovery->text[1], a->saved_source[1]) != 0;
        a->built = false;
        a->tab = 0;
        status(a,
               "Änderungen wiederhergestellt. Quelldateien werden erst beim Speichern geändert.");
    } else {
        char path[4096];
        autosave_path(a, path, sizeof path);
        if (!SDL_RemovePath(path)) {
            status(a, "Sicherung konnte nicht verworfen werden. Datei bleibt erhalten.");
            return;
        }
        status(a, "Gespeicherte Quelldateien geöffnet. Ungespeicherte Sicherung verworfen.");
    }
    ps_autosave_destroy(a->recovery);
    a->recovery = NULL;
    a->autosave_due = ps_clock() + a->preferences.autosave_seconds;
}
static void autosave_tick(app *a, double now) {
    if (!a->loaded || a->recovery || a->autosave_blocked || (!a->dirty && !a->analysis_dirty) ||
        now < a->autosave_due)
        return;
    a->autosave_due = now + a->preferences.autosave_seconds;
    ps_autosave snapshot = {0};
    snapshot.text[0] = (char *)nk_str_get_const(&a->experiment.string);
    snapshot.text[1] = (char *)nk_str_get_const(&a->analysis.string);
    snapshot.length[0] = (uint32_t)nk_str_len_char(&a->experiment.string);
    snapshot.length[1] = (uint32_t)nk_str_len_char(&a->analysis.string);
    snapshot.text[2] = a->saved_source[0];
    snapshot.text[3] = a->saved_source[1];
    if (!snapshot.text[2] || !snapshot.text[3])
        return;
    snapshot.length[2] = (uint32_t)strlen(snapshot.text[2]);
    snapshot.length[3] = (uint32_t)strlen(snapshot.text[3]);
    uint32_t hash[2];
    for (unsigned i = 0; i < 2; i++)
        hash[i] = ps_crc32((unsigned char *)snapshot.text[i], snapshot.length[i]);
    if (a->autosave_current && hash[0] == a->autosave_hash[0] && hash[1] == a->autosave_hash[1] &&
        snapshot.length[0] == a->autosave_length[0] && snapshot.length[1] == a->autosave_length[1])
        return;
    time_t timestamp = time(NULL);
    snapshot.saved_at_s = timestamp > 0 ? (uint64_t)timestamp : 0;
    char path[4096];
    autosave_path(a, path, sizeof path);
    ps_result r = ps_autosave_write(path, &snapshot);
    if (r != PS_OK) {
        snprintf(a->autosave_error, sizeof a->autosave_error,
                 "Autosave fehlgeschlagen (%s). Änderungen sind noch im Editor.",
                 ps_result_string(r));
        status(a, a->autosave_error);
        return;
    }
    memcpy(a->autosave_hash, hash, sizeof hash);
    memcpy(a->autosave_length, snapshot.length, sizeof a->autosave_length);
    a->autosave_current = true;
    a->autosave_error[0] = 0;
}
