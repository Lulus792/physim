static bool document_recovery_test_start(app *a, const char *root, const char *mode) {
    char workspace[4096], path[4096];
    join(workspace, sizeof workspace, root, "workspace");
    bool writer = !strcmp(mode, "drafts-write");
    if (!SDL_CreateDirectory(workspace))
        return false;
    const char *names[] = {"notes α.txt", "second.txt"};
    const char *originals[] = {"original α\n", "second original\n"};
    if (writer) {
        for (unsigned i = 0; i < 2; i++) {
            join(path, sizeof path, workspace, names[i]);
            if (!document_test_write(path, originals[i]))
                return false;
        }
    }
    if (!open_workspace_path(a, workspace))
        return false;
    for (unsigned i = 0; i < 2; i++) {
        join(path, sizeof path, workspace, names[i]);
        preview_workspace_path(a, path);
        if (a->document_count != i + 1)
            return false;
        if (writer && !document_test_text(a, i ? "second draft γ\n" : "first draft β 1\n"))
            return false;
    }
    document_select(a, 0);
    if (!writer)
        return true;
    double due = ps_clock() + a->preferences.autosave_seconds;
    for (unsigned i = 0; i < 2; i++)
        a->documents[i].draft_due = due;
    documents_autosave_tick(a, due - 1);
    if (a->documents[0].draft_text || a->documents[1].draft_text)
        return false;
    documents_autosave_tick(a, due);
    if (!a->documents[0].draft_text || !a->documents[1].draft_text ||
        !document_test_text(a, "first draft β 2\n"))
        return false;
    documents_autosave_tick(a, due + a->preferences.autosave_seconds);
    ps_autosave *draft = NULL;
    if (ps_document_draft_read(a->document_drafts_directory, a->documents[0].file.path, &draft) !=
        PS_OK)
        return false;
    bool ok =
        !strcmp(draft->text[0], "first draft β 2\n") && !strcmp(draft->text[2], "original α\n");
    ps_autosave_destroy(draft);
    fflush(stdout);
    _Exit(ok ? 0 : 1); /* Real abrupt exit: no normal save or cleanup. */
}
static void document_recovery_test_frame(app *a, const char *mode, int *stage, int *code,
                                         const char **capture) {
    bool reset = !strcmp(mode, "drafts-reset");
    bool revert = !strcmp(mode, "drafts-revert");
    bool late_corrupt = !strcmp(mode, "drafts-late-corrupt");
    bool corrupt = !strcmp(mode, "drafts-corrupt") || reset;
    bool conflict = !strcmp(mode, "drafts-conflict");
    bool discard = !strcmp(mode, "drafts-discard");
    bool cancel = !strcmp(mode, "drafts-cancel");
    bool ok = true;
    workspace_document *doc = active_document(a);
    switch ((*stage)++) {
    case 1:
        ok = doc && !doc->dirty && !a->job.running && !a->runner.running && !a->loaded;
        if (corrupt) {
            ok = ok && !doc->recovery && doc->draft_blocked && doc->draft_error[0];
            ok = ok && document_test_text(a, "still in editor\n");
            documents_autosave_tick(a, doc->draft_due + 1);
            ok = ok && !doc->draft_text;
            *capture = "drafts-corrupt.bmp";
        } else {
            ok = ok && doc->recovery && doc->recovery_conflict == conflict;
            *capture = conflict ? "drafts-conflict.bmp" : "drafts-recovery.bmp";
        }
        if (reset)
            test_mouse(a, a->document_bounds[9], true);
        else if (cancel || corrupt) {
            /* Closing an unresolved recovery preserves the bundle and the source. */
            SDL_Event quit = {0};
            quit.type = SDL_EVENT_QUIT;
            SDL_PushEvent(&quit);
        } else {
            test_mouse(a, a->document_bounds[discard ? 8 : 7], true);
        }
        break;
    case 2:
        if (reset)
            test_mouse(a, a->document_bounds[9], false);
        else if (!cancel && !corrupt)
            test_mouse(a, a->document_bounds[discard ? 8 : 7], false);
        break;
    case 4:
        if (reset) {
            ok = doc && !doc->draft_blocked && !doc->draft_error[0] && doc->dirty;
            documents_autosave_tick(a, doc->draft_due + 1);
            ps_autosave *draft = NULL;
            ok = ok && doc->draft_text &&
                 ps_document_draft_read(a->document_drafts_directory, doc->file.path, &draft) ==
                     PS_OK;
            ok = ok && draft && !strcmp(draft->text[0], "still in editor\n");
            ps_autosave_destroy(draft);
            ok = ok && document_save(a, 0);
            a->quitting = true;
            break;
        }
        ok = doc && !doc->recovery && doc->dirty == !discard;
        if (!discard) {
            const char *expected = "first draft β 2\n";
            ok = ok && nk_str_len_char(&doc->edit.string) == (int)strlen(expected) &&
                 !memcmp(nk_str_get_const(&doc->edit.string), expected, strlen(expected));
            ok = ok && (conflict ? (!strcmp(doc->file.saved, "external α\n") ||
                                    !strcmp(doc->file.saved, "external α\r\n"))
                                 : !strcmp(doc->file.saved, "original α\n"));
            if (revert) {
                ok = ok && doc->draft_present && document_test_text(a, doc->file.saved) &&
                     !doc->dirty;
                documents_autosave_tick(a, doc->draft_due + 1);
                ok = ok && !doc->draft_present;
                break;
            }
            if (late_corrupt) {
                char path[4096];
                ok = ok &&
                     ps_document_draft_path(a->document_drafts_directory, doc->file.path, path) ==
                         PS_OK &&
                     document_test_write(path, "damaged snapshot");
            }
            test_key(a, SDLK_S);
        }
        break;
    case 6: {
        ps_autosave *draft = NULL;
        ok = doc && !doc->dirty && !a->job.running && !a->runner.running &&
             ps_document_draft_read(a->document_drafts_directory, doc->file.path, &draft) ==
                 (late_corrupt ? PS_CORRUPT : PS_EOF);
        if (late_corrupt)
            ok = ok && doc->draft_blocked;
        ps_autosave_destroy(draft);
        document_select(a, 1);
        ok = ok && active_document(a)->recovery && !active_document(a)->recovery_conflict;
        document_resolve_recovery(a, true);
        ok = ok && document_save(a, 1);
        a->quitting = true;
        break;
    }
    }
    if (!ok) {
        if (doc)
            fprintf(stderr,
                    "Recovery state: active=%u pending=%d dirty=%d recovery=%d text=[%.*s] "
                    "saved=[%s]\n",
                    a->document_active, a->document_pending, doc->dirty, doc->recovery != NULL,
                    nk_str_len_char(&doc->edit.string), nk_str_get_const(&doc->edit.string),
                    doc->file.saved);
        *code = 1;
        a->quitting = true;
    }
    if (a->quitting)
        printf("DOCUMENT RECOVERY %s: %s stage %d\n", mode, *code ? "FAILED" : "PASSED", *stage);
}
