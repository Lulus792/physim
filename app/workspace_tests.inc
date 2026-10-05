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
    if (!strcmp(mode, "native-dialogs") || !strcmp(mode, "native-dialog-error"))
        return native_dialog_test_start(root);
    if (!strncmp(mode, "reset-", 6))
        return reset_test_start(a, root, mode);
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
    if (!strcmp(mode, "toolbar") || !strcmp(mode, "toolbar-noise"))
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
        preview_workspace_path(a, path);
        if (a->document_count != 1)
            return false;
        a->documents[0].edit.cursor = 7;
        a->documents[0].edit.select_start = 2;
        a->documents[0].edit.select_end = 9;
        join(path, sizeof path, root, "view λ.txt");
        f = fopen(path, "w");
        if (!f)
            return false;
        for (unsigned i = 0; i < 120; i++)
            if (fputs("View λ: saved editor position\n", f) < 0)
                ok = false;
        if (fclose(f))
            ok = false;
        if (!ok)
            return false;
        preview_workspace_path(a, path);
        if (a->document_count != 2)
            return false;
        a->documents[1].edit.cursor = 300;
        a->documents[1].edit.select_start = 290;
        a->documents[1].edit.select_end = 310;
        a->documents[1].edit.scrollbar.y = 500;
        if (a->loaded) {
            a->experiment.cursor = 42;
            a->analysis.cursor = 13;
        }
        document_select(a, 1);
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
    if (!strcmp(mode, "native-dialogs") || !strcmp(mode, "native-dialog-error")) {
        native_dialog_test_frame(a, !strcmp(mode, "native-dialog-error"), stage, code, capture);
        return;
    }
    if (!strncmp(mode, "reset-", 6)) {
        reset_test_frame(a, mode, stage, code, capture);
        return;
    }
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
    if (!strcmp(mode, "toolbar") || !strcmp(mode, "toolbar-noise")) {
        toolbar_test_frame(a, stage, code, capture);
        if (!strcmp(mode, "toolbar-noise"))
            test_pointer_noise(a);
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
            bool expect_project = strncmp(mode, "read-folder", 11) != 0;
            if (!a->workspace_open || a->loaded != expect_project ||
                a->workspace_addition_count != 2 || a->built || a->runner.running ||
                a->job.running || strcmp(a->workspace, a->last_workspace.root) ||
                strcmp(a->workspace_additions[0], a->last_workspace.additions[0]) ||
                strcmp(a->workspace_additions[1], a->last_workspace.additions[1]))
                *code = 1;
            if (!strcmp(mode, "missing-addition") && !strstr(a->status, "nicht erreichbar"))
                *code = 1;
            if (a->last_workspace.document_count) {
                bool missing_document = !strcmp(mode, "missing-addition");
                unsigned expected = missing_document ? 1 : a->last_workspace.document_count;
                unsigned active = missing_document ? 0 : a->last_workspace.active_document;
                int expected_tab = a->last_workspace.view == 3 ? 8 : (int)a->last_workspace.view;
                if (a->document_count != expected || a->tab != expected_tab || a->document_active != active ||
                    a->analysis_tab != (int)a->last_workspace.analysis_editor)
                    *code = 1;
                for (unsigned i = 0; i < a->document_count; i++) {
                    const ps_workspace_document *saved = &a->last_workspace.documents[i + missing_document];
                    ps_workspace_editor restored = workspace_editor_capture(&a->documents[i].edit);
                    ps_workspace_editor expected_view = saved->editor;
                    if (!strcmp(mode, "read-folder-shortened") && i == 1) {
                        expected_view.cursor = expected_view.select_start = expected_view.select_end = 1;
                        expected_view.scroll_y = 0;
                    }
                    if (strcmp(a->documents[i].path, saved->path) ||
                        memcmp(&restored, &expected_view, sizeof restored) || a->documents[i].dirty) {
                        fprintf(stderr, "Document view %u: cursor=%u selection=%u:%u scroll=%u:%u; "
                                "expected %u %u:%u %u:%u\n", i, restored.cursor, restored.select_start,
                                restored.select_end, restored.scroll_x, restored.scroll_y,
                                expected_view.cursor, expected_view.select_start, expected_view.select_end,
                                expected_view.scroll_x, expected_view.scroll_y);
                        *code = 1;
                    }
                }
                if (a->loaded && (a->experiment.cursor != (int)a->last_workspace.experiment.cursor ||
                                 a->analysis.cursor != (int)a->last_workspace.analysis.cursor))
                    *code = 1;
            }
            if (!strcmp(mode, "read")) {
                document_select(a, 0);
                a->tab = 2;
                a->analysis_tab = 1;
            } else if (!strcmp(mode, "read-analysis")) {
                if (a->tab != 2 || a->analysis_tab != 1)
                    *code = 1;
                a->tab = 1;
            } else if (!strcmp(mode, "read-simulate")) {
                if (a->tab != 1 || a->runner.running)
                    *code = 1;
                document_select(a, a->document_count - 1);
            }
            if (!strcmp(mode, "read-folder-shortened")) {
                workspace_document *doc = active_document(a);
                if (!doc || ps_document_draft_write(a->document_drafts_directory, &doc->file,
                                                    "Autosave λ\n", strlen("Autosave λ\n"), 123) != PS_OK)
                    *code = 1;
            }
            if (!strcmp(mode, "read-folder-recovery")) {
                workspace_document *doc = active_document(a);
                if (!doc || !doc->recovery || doc->recovery_conflict ||
                    strcmp(doc->recovery->text[0], "Autosave λ\n") || doc->file.length != 2 ||
                    nk_str_len_char(&doc->edit.string) != 2 ||
                    memcmp(nk_str_get_const(&doc->edit.string), "λ", 2))
                    *code = 1;
            }
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
