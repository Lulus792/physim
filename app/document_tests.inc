static char document_test_directory[4096];
static bool document_test_write(const char *path, const char *text) {
    FILE *f = fopen(path, "wb");
    if (!f)
        return false;
    bool ok = fputs(text, f) >= 0;
    return !fclose(f) && ok;
}
static bool document_test_text(app *a, const char *text) {
    workspace_document *doc = active_document(a);
    if (!doc || !set_editor_text(&doc->edit, text, strlen(text)))
        return false;
    document_changed(a, &doc->edit);
    return true;
}
static bool document_test_start(app *a, const char *root) {
    snprintf(document_test_directory, sizeof document_test_directory, "%s", root);
    char path[4096];
    join(path, sizeof path, root, "notes α.txt");
    if (!document_test_write(path, "Workspace notes: α = 1\n"))
        return false;
    join(path, sizeof path, root, "second.txt");
    if (!document_test_write(path, "second\n") || !open_workspace_path(a, root))
        return false;
    return workspace_tree_test_target(a, "notes α.txt", 0);
}
static bool document_build_test_start(app *a, const char *root) {
    join(a->project_input, sizeof a->project_input, root, "project with spaces");
    new_project(a);
    if (!a->loaded)
        return false;
    char path[4096];
    join(path, sizeof path, a->project, "helper.h");
    if (!document_test_write(path, "#define DOCUMENT_TEST_VERSION 1\n"))
        return false;
    const char *source = "\n#include \"helper.h\"\n"
                         "_Static_assert(DOCUMENT_TEST_VERSION >= 1, \"header version\");\n";
    nk_str_append_text_char(&a->experiment.string, source, (int)strlen(source));
    a->dirty = true;
    preview_workspace_path(a, path);
    return a->document_count == 1;
}
static void document_build_test_frame(app *a, int *stage, int *code) {
    bool ok = true;
    workspace_document *doc = active_document(a);
    switch (*stage) {
    case 0:
        if (a->library_thread)
            return;
        build_project(a);
        ok = a->job.running && a->job_kind == 2;
        break;
    case 1:
        /* Save while the compiler is running: dirty flags alone cannot detect this. */
        ok = a->job.running && document_test_text(a, "#define DOCUMENT_TEST_VERSION 2\n") &&
             document_save(a, 0) && !doc->dirty;
        break;
    case 2:
        if (a->job.running)
            return;
        ok = !a->built && !a->diagnostic_count && strstr(a->status, "Erneut bauen");
        build_project(a);
        break;
    case 3:
        if (a->job.running && a->job_kind != 5)
            return;
        ok = a->job.running && a->job_kind == 5;
        snprintf(a->find, sizeof a->find, ">= 1");
        snprintf(a->replace, sizeof a->replace, ">= 2");
        find_text(a, &a->experiment, true);
        ok = ok && a->dirty;
        save_project(a);
        ok = ok && !a->dirty;
        break;
    case 4:
        if (a->job.running)
            return;
        ok = !a->built && strstr(a->status, "Erneut bauen");
        build_project(a);
        break;
    case 5:
        if (a->job.running)
            return;
        ok = a->built && !a->diagnostic_count;
        ok = ok && document_test_text(a, "#define DOCUMENT_TEST_VERSION 3\n") && !a->built;
        test_window_key(a->window, SDLK_F6);
        break;
    case 6:
        ok = !a->runner.running && !a->last_run[0] && doc->dirty;
        ok = ok && document_save(a, 0) && !a->built;
        build_project(a);
        break;
    case 7:
        if (a->job.running)
            return;
        ok = a->built;
        ok = ok && document_test_write(doc->path, "#define DOCUMENT_TEST_VERSION 4\n") &&
             document_reload(a) && !a->built && !doc->dirty;
        start_run(a);
        ok = ok && !a->runner.running && !a->last_run[0];
        a->quitting = true;
        break;
    }
    if (!ok) {
        *code = 1;
        a->quitting = true;
    }
    if (a->quitting)
        printf("DOCUMENT BUILD: %s stage %d: %s\n", *code ? "FAILED" : "PASSED", *stage, a->status);
    ++*stage;
}
static void document_test_frame(app *a, const char *root, int *stage, int *code,
                                const char **capture) {
    bool ok = true;
    char directory[4096];
    snprintf(directory, sizeof directory, "%s", document_test_directory);
    root = directory;
    char path[4096];
    workspace_document *doc = active_document(a);
    switch ((*stage)++) {
    case 1:
        test_mouse(a, a->workspace_tree_test_bounds, true);
        break;
    case 2:
        test_mouse(a, a->workspace_tree_test_bounds, false);
        break;
    case 4:
        ok = doc && a->document_count == 1 && a->tab == 8 && !doc->dirty;
        test_mouse(a, a->document_bounds[6], true);
        break;
    case 5:
        test_mouse(a, a->document_bounds[6], false);
        break;
    case 7:
        test_key(a, SDLK_A);
        break;
    case 9: {
        SDL_Event input = {0};
        input.type = SDL_EVENT_TEXT_INPUT;
        input.text.windowID = SDL_GetWindowID(a->window);
        input.text.text = "Text über SDL: β = 2\n";
        SDL_PushEvent(&input);
        break;
    }
    case 11:
        ok = doc && doc->dirty && !a->analysis_dirty && !a->dirty;
        test_key(a, SDLK_S);
        break;
    case 13:
        ok = doc && !doc->dirty && !strcmp(doc->file.saved, "Text über SDL: β = 2\n");
        join(path, sizeof path, root, "second.txt");
        preview_workspace_path(a, path);
        ok = ok && a->document_count == 2 && document_test_text(a, "local draft");
        document_select(a, 0);
        break;
    case 15:
        test_mouse(a, a->document_list_bounds[1], true);
        break;
    case 16:
        test_mouse(a, a->document_list_bounds[1], false);
        break;
    case 18:
        ok = a->document_active == 1 && doc && doc->dirty;
        join(path, sizeof path, root, "second.txt");
        ok = ok && document_test_write(path, "external");
        test_key(a, SDLK_S);
        break;
    case 20: {
        ok = doc && doc->dirty && strstr(doc->error, "extern");
        *capture = "documents-conflict.bmp";
        SDL_Event quit = {0};
        quit.type = SDL_EVENT_QUIT;
        SDL_PushEvent(&quit);
        break;
    }
    case 22:
        ok = !a->quitting && doc && doc->dirty;
        ok = ok && !open_workspace_path(a, a->root) && a->document_count == 2 &&
             !strcmp(a->workspace, root);
        test_mouse(a, a->document_bounds[1], true);
        break;
    case 23:
        test_mouse(a, a->document_bounds[1], false);
        break;
    case 25:
        ok = a->document_pending == 2;
        *capture = "documents-reload.bmp";
        test_mouse(a, a->document_bounds[5], true);
        break;
    case 26:
        test_mouse(a, a->document_bounds[5], false);
        break;
    case 28:
        ok = doc && doc->dirty && !a->document_pending;
        test_key(a, SDLK_W);
        break;
    case 30:
        ok = a->document_pending == 1;
        test_mouse(a, a->document_bounds[3], true);
        break;
    case 31:
        test_mouse(a, a->document_bounds[3], false);
        break;
    case 33:
        ok = a->document_count == 2 && doc && doc->dirty && a->document_pending == 1;
        document_request(a, 2);
        break;
    case 35:
        test_mouse(a, a->document_bounds[4], true);
        break;
    case 36:
        test_mouse(a, a->document_bounds[4], false);
        break;
    case 38:
        ok = doc && !doc->dirty && !strcmp(doc->file.saved, "external");
        snprintf(a->find, sizeof a->find, "external");
        snprintf(a->replace, sizeof a->replace, "internal");
        if (doc)
            find_text(a, &doc->edit, true);
        ok = ok && doc->dirty && !a->analysis_dirty && !a->dirty;
        test_key(a, SDLK_W);
        break;
    case 40:
        ok = a->document_pending == 1;
        test_mouse(a, a->document_bounds[4], true);
        break;
    case 41:
        test_mouse(a, a->document_bounds[4], false);
        break;
    case 43:
        ok = a->document_count == 1 && doc && !doc->dirty;
        join(path, sizeof path, root, "binary.txt");
        ok = ok && document_test_write(path, "\xff");
        preview_workspace_path(a, path);
        ok = ok && a->document_count == 1 && active_document(a) == doc;
        join(path, sizeof path, root, "./notes α.txt");
        preview_workspace_path(a, path);
        ok = ok && a->document_count == 1;
        *capture = "documents-editor.bmp";
        *stage = 60;
        break;
    case 60:
        join(path, sizeof path, root, "scratch.phys");
        ok = document_test_write(path, "");
        preview_workspace_path(a, path);
        ok = ok && a->document_count == 2;
        break;
    case 61:
        test_mouse(a, a->document_bounds[6], true);
        break;
    case 62:
        test_mouse(a, a->document_bounds[6], false);
        break;
    case 64: {
        SDL_Event key = {0};
        key.type = SDL_EVENT_KEY_DOWN;
        key.key.windowID = SDL_GetWindowID(a->window);
        key.key.key = SDLK_TAB;
        key.key.down = true;
        SDL_PushEvent(&key);
        key.type = SDL_EVENT_KEY_UP;
        key.key.down = false;
        SDL_PushEvent(&key);
        break;
    }
    case 66:
        ok = doc && doc->dirty && nk_str_len_char(&doc->edit.string) == 4 &&
             !memcmp(nk_str_get_const(&doc->edit.string), "    ", 4);
        test_key(a, SDLK_Z);
        break;
    case 68:
        ok = doc && !doc->dirty && nk_str_len_char(&doc->edit.string) == 0;
        test_key(a, SDLK_R);
        break;
    case 70:
        ok = doc && doc->dirty && nk_str_len_char(&doc->edit.string) == 4;
        test_key(a, SDLK_S);
        break;
    case 72:
        ok = doc && !doc->dirty && !strcmp(doc->file.saved, "    ");
        test_key(a, SDLK_W);
        break;
    case 74:
        ok = a->document_count == 1;
        *stage = 44;
        break;
    case 44:
        join(a->project_input, sizeof a->project_input, root, "project");
        new_project(a);
        ok = a->loaded && !a->document_count;
        join(path, sizeof path, a->project, experiment_source(a));
        preview_workspace_path(a, path);
        ok = ok && !a->document_count && a->tab == 0;
        join(path, sizeof path, a->project, analysis_source(a));
        preview_workspace_path(a, path);
        ok = ok && !a->document_count && a->tab == 2 && a->analysis_tab == 1;
        break;
    case 45:
        if (a->library_thread) {
            --*stage;
            break;
        }
        ok = open_workspace_path(a, root) && !a->loaded;
        join(path, sizeof path, root, "notes α.txt");
        preview_workspace_path(a, path);
        ok = ok && document_test_text(a, "Saved on close: γ\n");
        break;
    case 48: {
        SDL_Event quit = {0};
        quit.type = SDL_EVENT_QUIT;
        SDL_PushEvent(&quit);
        break;
    }
    case 51:
        ok = false;
        break;
    }
    if (!ok) {
        *code = 1;
        a->quitting = true;
    }
    if (a->quitting)
        printf("DOCUMENT WORKFLOW: %s stage %d: %s\n", *code ? "FAILED" : "PASSED", *stage,
               a->status);
}
