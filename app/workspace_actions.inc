/* Restoring editor views never builds or executes project code. */
static uint32_t workspace_scroll(float value) {
    return !(value > 0) ? 0 : value >= PS_WORKSPACE_SCROLL_LIMIT
                                 ? PS_WORKSPACE_SCROLL_LIMIT : (uint32_t)value;
}
static uint32_t workspace_position(int value, int length) {
    return value < 0 ? 0 : (uint32_t)(value > length ? length : value);
}
static ps_workspace_editor workspace_editor_capture(const struct nk_text_edit *edit) {
    int length = nk_str_len(&edit->string);
    ps_workspace_editor view = {0};
    view.cursor = workspace_position(edit->cursor, length);
    view.select_start = workspace_position(edit->select_start, length);
    view.select_end = workspace_position(edit->select_end, length);
    view.scroll_x = workspace_scroll(edit->scrollbar.x);
    view.scroll_y = workspace_scroll(edit->scrollbar.y);
    return view;
}
static void workspace_editor_restore(app *a, struct nk_text_edit *edit, const ps_workspace_editor *view) {
    uint32_t length = (uint32_t)nk_str_len(&edit->string);
    edit->cursor = (int)(view->cursor < length ? view->cursor : length);
    edit->select_start = (int)(view->select_start < length ? view->select_start : length);
    edit->select_end = (int)(view->select_end < length ? view->select_end : length);
    edit->scrollbar = nk_vec2((float)view->scroll_x, (float)view->scroll_y);
    /* External edits can shorten a file. Keep the restored viewport on text. */
    const char *text = nk_str_get_const(&edit->string);
    size_t bytes = (size_t)nk_str_len_char(&edit->string), start = 0, lines = 0;
    float width = 0;
    for (size_t i = 0; i <= bytes; i++) {
        if (i < bytes && text[i] != '\n')
            continue;
        if (view->scroll_x && i > start) {
            float w = a->font_code->width(a->font_code->userdata, a->font_code->height,
                                          text + start, (int)(i - start));
            if (w > width)
                width = w;
        }
        if (i < bytes)
            lines++;
        start = i + 1;
    }
    float height = (float)lines * (a->font_code->height + a->ui->style.edit.row_padding);
    if (edit->scrollbar.y > height)
        edit->scrollbar.y = height;
    if (edit->scrollbar.x > width)
        edit->scrollbar.x = width;
    edit->has_preferred_x = 0;
}
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
        state->document_count = a->document_count;
        state->active_document = a->document_count ? a->document_active : 0;
        state->view = a->tab == 8 && a->document_count ? 3 : a->tab == 1 ? 1 : a->tab == 2 ? 2 : 0;
        state->analysis_editor = a->analysis_tab == 1;
        state->experiment = workspace_editor_capture(&a->experiment);
        state->analysis = workspace_editor_capture(&a->analysis);
        for (uint32_t i = 0; r == PS_OK && i < state->document_count; i++) {
            r = ps_workspace_absolute(a->documents[i].path, state->documents[i].path);
            state->documents[i].editor = workspace_editor_capture(&a->documents[i].edit);
        }
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
    unsigned unavailable = 0, active = 0;
    for (uint32_t i = 0; i < a->last_workspace.document_count; i++) {
        const ps_workspace_document *saved = &a->last_workspace.documents[i];
        unsigned before = a->document_count;
        preview_workspace_path(a, saved->path);
        if (a->document_count == before) {
            unavailable++;
            continue;
        }
        workspace_editor_restore(a, &a->documents[before].edit, &saved->editor);
        if (i == a->last_workspace.active_document)
            active = before;
    }
    workspace_editor_restore(a, &a->experiment, &a->last_workspace.experiment);
    workspace_editor_restore(a, &a->analysis, &a->last_workspace.analysis);
    if (a->document_count)
        document_select(a, active);
    a->analysis_tab = (int)a->last_workspace.analysis_editor;
    a->tab = a->loaded ? (int)a->last_workspace.view : 0;
    if (a->last_workspace.view == 3)
        a->tab = a->document_count ? 8 : 0;
    if (missing || unavailable) {
        char message[192];
        snprintf(message, sizeof message,
                 "Workspace geöffnet. %u zusätzliche Pfade nicht erreichbar; "
                 "%u Dokumente konnten nicht geöffnet werden.", missing, unavailable);
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
