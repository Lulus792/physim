static bool workspace_test_start(app *a, const char *root, const char *mode) {
    if (!ps_make_directory(root))
        return false;
    char path[4096];
    join(path, sizeof path, root, "workspace.bin");
    workspace_state_start(a, path);
    join(path, sizeof path, root, "app-data/document-drafts");
    document_drafts_start(a, path);
    if (a->workspace_open || a->loaded || a->project_input[0])
        return false;
    if (!strncmp(mode, "project-settings-", 17))
        return project_settings_test_start(a, root, mode);
    if (!strncmp(mode, "drafts-", 7))
        return document_recovery_test_start(a, root, mode);
    if (!strcmp(mode, "tree"))
        return workspace_tree_test_start(a, root);
    if (!strcmp(mode, "documents"))
        return document_test_start(a, root);
    if (!strcmp(mode, "documents-build"))
        return document_build_test_start(a, root);
    if (!strcmp(mode, "toolbar"))
        return true;
    if (!strcmp(mode, "write") || !strcmp(mode, "write-folder")) {
        if (!strcmp(mode, "write")) {
            join(a->project_input, sizeof a->project_input, root, "project");
            new_project(a);
            if (!a->loaded)
                return false;
        } else if (!open_workspace_path(a, root) || a->loaded)
            return false;
        join(path, sizeof path, root, "notes ä.txt");
        FILE *f = fopen(path, "w");
        if (!f)
            return false;
        bool ok = fputs("Workspace restart test\n", f) >= 0;
        if (fclose(f))
            ok = false;
        if (!ok)
            return false;
        add_workspace_path(a, path);
        add_workspace_path(a, root);
        return a->workspace_addition_count == 2;
    }
    if (!strcmp(mode, "corrupt"))
        return !a->workspace_state_writable && a->workspace_state_error[0] &&
               !a->last_workspace.root[0];
    if (!strcmp(mode, "empty"))
        return a->workspace_state_writable && !a->last_workspace.root[0];
    return a->workspace_state_writable && a->last_workspace.root[0] && a->last_workspace.count == 2;
}
static void workspace_test_frame(app *a, const char *mode, int *stage, int *code,
                                 const char **capture) {
    if (!strncmp(mode, "project-settings-", 17)) {
        project_settings_test_frame(a, mode, stage, code);
        return;
    }
    if (!strncmp(mode, "drafts-", 7)) {
        document_recovery_test_frame(a, mode, stage, code, capture);
        return;
    }
    if (!strcmp(mode, "documents-build")) {
        document_build_test_frame(a, stage, code);
        return;
    }
    if (!strcmp(mode, "documents")) {
        document_test_frame(a, a->workspace, stage, code, capture);
        return;
    }
    if (!strcmp(mode, "toolbar")) {
        toolbar_test_frame(a, stage, code, capture);
        return;
    }
    if (!strcmp(mode, "tree")) {
        workspace_tree_test_frame(a, stage, code, capture);
        return;
    }
    if (!strcmp(mode, "write") || !strcmp(mode, "write-folder") || !strcmp(mode, "corrupt") ||
        !strcmp(mode, "empty")) {
        a->quitting = true;
        return;
    }
    if (!strcmp(mode, "forget")) {
        forget_workspace(a);
        if (a->last_workspace.root[0] || a->workspace_open || a->workspace_state_error[0])
            *code = 1;
        a->quitting = true;
        return;
    }
    switch ((*stage)++) {
    case 0:
        break; /* First frame establishes button geometry. */
    case 1: {
        *capture = "workspace-restore.bmp";
        test_mouse(a, a->workspace_restore_bounds, true);
        break;
    }
    case 2:
        test_mouse(a, a->workspace_restore_bounds, false);
        break;
    case 3:
        break;
    case 4:
        if (!strcmp(mode, "missing-root")) {
            if (a->workspace_open || a->loaded || !a->last_workspace.root[0])
                *code = 1;
        } else {
            bool expect_project = strcmp(mode, "read-folder") != 0;
            if (!a->workspace_open || a->loaded != expect_project ||
                a->workspace_addition_count != 2 || a->built || a->runner.running ||
                a->job.running || strcmp(a->workspace, a->last_workspace.root) ||
                strcmp(a->workspace_additions[0], a->last_workspace.additions[0]) ||
                strcmp(a->workspace_additions[1], a->last_workspace.additions[1]))
                *code = 1;
            if (!strcmp(mode, "missing-addition") && !strstr(a->status, "nicht erreichbar"))
                *code = 1;
            /* A blocked restore must preserve the current additions. */
            a->dialog_pending = true;
            restore_workspace(a);
            a->dialog_pending = false;
            if (a->workspace_addition_count != 2)
                *code = 1;
            a->job.running = true;
            restore_workspace(a);
            a->job.running = false;
            if (a->workspace_addition_count != 2 || strcmp(a->workspace, a->last_workspace.root))
                *code = 1;
        }
        printf("WORKSPACE RESTART %s: %s\n", mode, *code ? "FAILED" : "PASSED");
        a->quitting = true;
        break;
    }
}
