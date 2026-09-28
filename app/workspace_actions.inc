/* Remember paths only. Restoring never builds or executes project code. */
static void workspace_state_start(app *a, const char *override_path) {
    if (override_path)
        snprintf(a->workspace_state_path, sizeof a->workspace_state_path, "%s", override_path);
    else {
        char *directory = SDL_GetPrefPath("Physim", "Physim");
        if (directory) {
            int n = snprintf(a->workspace_state_path, sizeof a->workspace_state_path,
                             "%sworkspace.bin", directory);
            if (n < 0 || (size_t)n >= sizeof a->workspace_state_path)
                a->workspace_state_path[0] = 0;
            SDL_free(directory);
        }
    }
    memset(&a->last_workspace, 0, sizeof a->last_workspace);
    ps_result r = ps_workspace_state_read(a->workspace_state_path, &a->last_workspace);
    a->workspace_state_writable = r == PS_OK || r == PS_EOF;
    a->workspace_state_error[0] = 0;
    if (!a->workspace_state_writable)
        snprintf(
            a->workspace_state_error, sizeof a->workspace_state_error,
            "Gespeicherter Workspace konnte nicht gelesen werden (%s). Die Datei bleibt erhalten.",
            ps_result_string(r));
}
static ps_result workspace_state_save(app *a) {
    if (!a->workspace_state_writable || !a->workspace_open || !a->workspace_state_path[0])
        return PS_OK;
    ps_workspace_state *state = calloc(1, sizeof *state);
    ps_result r = state ? ps_workspace_absolute(a->workspace, state->root) : PS_MEMORY;
    if (state) {
        state->count = a->workspace_addition_count;
        for (uint32_t i = 0; r == PS_OK && i < state->count; i++)
            r = ps_workspace_absolute(a->workspace_additions[i], state->additions[i]);
        if (r == PS_OK)
            r = ps_workspace_state_write(a->workspace_state_path, state);
        free(state);
    }
    if (r != PS_OK)
        snprintf(a->workspace_state_error, sizeof a->workspace_state_error,
                 "Workspace konnte nicht gespeichert werden: %s.", ps_result_string(r));
    return r;
}
static void restore_workspace(app *a) {
    if (!a->last_workspace.root[0] || a->dialog_pending)
        return;
    if (!open_workspace_path(a, a->last_workspace.root))
        return;
    unsigned missing = 0;
    for (uint32_t i = 0; i < a->last_workspace.count; i++) {
        const char *path = a->last_workspace.additions[i];
        SDL_PathInfo info;
        if (!SDL_GetPathInfo(path, &info))
            missing++;
        /* Keep missing paths so temporarily unavailable drives are not forgotten. */
        snprintf(a->workspace_additions[a->workspace_addition_count++],
                 sizeof a->workspace_additions[0], "%s", path);
    }
    refresh_workspace_entries(a);
    if (missing) {
        char message[192];
        snprintf(message, sizeof message,
                 "Workspace geöffnet. %u zusätzliche Pfade sind derzeit nicht erreichbar.",
                 missing);
        status(a, message);
    }
}
static void forget_workspace(app *a) {
    ps_workspace_state *empty = calloc(1, sizeof *empty);
    ps_result r = empty ? ps_workspace_state_write(a->workspace_state_path, empty) : PS_MEMORY;
    free(empty);
    if (r == PS_OK) {
        memset(&a->last_workspace, 0, sizeof a->last_workspace);
        a->workspace_state_writable = true;
        a->workspace_state_error[0] = 0;
        status(a, "Gespeicherten Workspace vergessen.");
    } else {
        char message[192];
        snprintf(message, sizeof message, "Workspace konnte nicht vergessen werden: %s.",
                 ps_result_string(r));
        status(a, message);
    }
}
