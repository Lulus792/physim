/* Workspace documents own their editor and last saved snapshot independently. */
static workspace_document *active_document(app *a) {
    return a->document_active < a->document_count ? &a->documents[a->document_active] : NULL;
}
static void document_select(app *a, unsigned index) {
    if (index >= a->document_count)
        return;
    a->document_active = index;
    a->document_pending = 0;
    a->project_manager = false;
    a->tab = 8;
    snprintf(a->workspace_preview_path, sizeof a->workspace_preview_path, "%s",
             a->documents[index].path);
}
static void document_error(app *a, workspace_document *doc, ps_document_result r) {
    const char *message =
        r == PS_DOCUMENT_CONFLICT ? "Datei extern geändert oder entfernt. Änderungen bleiben im "
                                    "Editor. Datei prüfen oder neu laden."
        : r == PS_DOCUMENT_LIMIT  ? "Textdateien dürfen höchstens 256 KiB enthalten."
        : r == PS_DOCUMENT_INVALID
            ? "Kein gültiger UTF-8-Text: Binärdaten und Steuerzeichen sind nicht erlaubt."
        : r == PS_DOCUMENT_MEMORY ? "Nicht genügend Speicher. Der bisherige Inhalt bleibt erhalten."
                                  : "Datei konnte nicht gelesen oder gespeichert werden. Inhalt "
                                    "bleibt im Editor erhalten.";
    if (doc)
        snprintf(doc->error, sizeof doc->error, "%s", message);
    status(a, message);
}
static void document_changed(app *a, struct nk_text_edit *edit) {
    for (unsigned i = 0; i < a->document_count; i++) {
        workspace_document *doc = &a->documents[i];
        if (edit != &doc->edit)
            continue;
        size_t n = (size_t)nk_str_len_char(&edit->string);
        doc->dirty = n != doc->file.length ||
                     (n && memcmp(nk_str_get_const(&edit->string), doc->file.saved, n));
        /* Extra files may be headers, build scripts, or imported model inputs. */
        if (a->loaded)
            invalidate_build(a);
        return;
    }
}
static bool documents_dirty(const app *a) {
    for (unsigned i = 0; i < a->document_count; i++)
        if (a->documents[i].dirty)
            return true;
    return false;
}
static void document_drafts_start(app *a, const char *directory) {
    if (directory) {
        snprintf(a->document_drafts_directory, sizeof a->document_drafts_directory, "%s",
                 directory);
        return;
    }
    char *base = SDL_GetPrefPath("Physim", "Physim");
    if (base) {
        int n = snprintf(a->document_drafts_directory, sizeof a->document_drafts_directory,
                         "%sdocument-drafts", base);
        if (n < 0 || (size_t)n >= sizeof a->document_drafts_directory)
            a->document_drafts_directory[0] = 0;
        SDL_free(base);
    }
}
static bool document_draft_discard(app *a, workspace_document *doc, bool explicit_discard) {
    ps_result r = PS_OK;
    if (!explicit_discard) {
        ps_autosave *draft = NULL;
        r = ps_document_draft_read(a->document_drafts_directory, doc->file.path, &draft);
        ps_autosave_destroy(draft);
        if (r == PS_EOF)
            r = PS_OK;
        if (r == PS_CORRUPT)
            doc->draft_blocked = true;
    }
    if (r == PS_OK)
        r = ps_document_draft_discard(a->document_drafts_directory, doc->file.path);
    if (r != PS_OK) {
        snprintf(doc->draft_error, sizeof doc->draft_error,
                 "Sicherung konnte nicht entfernt werden (%s). Sie bleibt erhalten.",
                 ps_result_string(r));
        return false;
    }
    free(doc->draft_text);
    doc->draft_text = NULL;
    doc->draft_length = 0;
    doc->draft_present = false;
    doc->draft_blocked = false;
    doc->draft_error[0] = 0;
    return true;
}
static void document_check_recovery(app *a, workspace_document *doc) {
    doc->draft_due = ps_clock() + a->preferences.autosave_seconds;
    ps_autosave *draft = NULL;
    ps_result r = ps_document_draft_read(a->document_drafts_directory, doc->file.path, &draft);
    if (r == PS_EOF)
        return;
    if (r != PS_OK) {
        doc->draft_blocked = true;
        snprintf(doc->draft_error, sizeof doc->draft_error,
                 "Autosave pausiert: Sicherung nicht lesbar (%s). Sie bleibt erhalten.",
                 ps_result_string(r));
        return;
    }
    doc->draft_present = true;
    if (draft->length[0] == doc->file.length &&
        !memcmp(draft->text[0], doc->file.saved, doc->file.length)) {
        ps_autosave_destroy(draft);
        document_draft_discard(a, doc, false);
        return;
    }
    doc->recovery_conflict = draft->length[2] != doc->file.length ||
                             memcmp(draft->text[2], doc->file.saved, doc->file.length);
    doc->recovery = draft;
}
static void document_resolve_recovery(app *a, bool restore) {
    workspace_document *doc = active_document(a);
    if (!doc || !doc->recovery)
        return;
    if (restore) {
        if (!set_editor_text(&doc->edit, doc->recovery->text[0], doc->recovery->length[0])) {
            document_error(a, doc, PS_DOCUMENT_MEMORY);
            return;
        }
        document_changed(a, &doc->edit);
        status(a, "Entwurf wiederhergestellt. Die Datei wird erst beim Speichern geändert.");
    } else {
        if (!document_draft_discard(a, doc, true))
            return;
        status(a, "Sicherung verworfen. Die gespeicherte Datei bleibt geöffnet.");
    }
    ps_autosave_destroy(doc->recovery);
    doc->recovery = NULL;
    doc->recovery_conflict = false;
    doc->draft_due = ps_clock() + a->preferences.autosave_seconds;
}
static void documents_autosave_tick(app *a, double now) {
    for (unsigned i = 0; i < a->document_count; i++) {
        workspace_document *doc = &a->documents[i];
        if (doc->recovery || doc->draft_blocked || now < doc->draft_due)
            continue;
        doc->draft_due = now + a->preferences.autosave_seconds;
        if (!doc->dirty) {
            if (doc->draft_present)
                document_draft_discard(a, doc, false);
            continue;
        }
        size_t length = (size_t)nk_str_len_char(&doc->edit.string);
        const char *text = nk_str_get_const(&doc->edit.string);
        if (doc->draft_text && length == doc->draft_length &&
            (!length || !memcmp(text, doc->draft_text, length)))
            continue;
        char *copy = copy_editor_text(&doc->edit);
        time_t timestamp = time(NULL);
        ps_result r = copy
                          ? ps_document_draft_write(a->document_drafts_directory, &doc->file, copy,
                                                    length, timestamp > 0 ? (uint64_t)timestamp : 0)
                          : PS_MEMORY;
        if (r != PS_OK) {
            free(copy);
            doc->draft_blocked = r == PS_CORRUPT;
            snprintf(doc->draft_error, sizeof doc->draft_error,
                     "Autosave fehlgeschlagen (%s). Änderungen bleiben im Editor.",
                     ps_result_string(r));
            continue;
        }
        free(doc->draft_text);
        doc->draft_text = copy;
        doc->draft_present = true;
        doc->draft_length = length;
        doc->draft_error[0] = 0;
    }
}
/* 1: Physim, 0: C, -1: plain text. */
static int document_language(app *a, const struct nk_text_edit *edit) {
    if (edit == &a->experiment)
        return a->language_experiment ? 1 : 0;
    if (edit == &a->analysis)
        return a->language_analysis ? 1 : 0;
    for (unsigned i = 0; i < a->document_count; i++) {
        if (edit != &a->documents[i].edit)
            continue;
        const char *extension = strrchr(a->documents[i].file.path, '.');
        if (extension && !SDL_strcasecmp(extension, ".phys"))
            return 1;
        if (extension && (!SDL_strcasecmp(extension, ".c") || !SDL_strcasecmp(extension, ".h")))
            return 0;
    }
    return -1;
}
static bool document_save(app *a, unsigned index) {
    workspace_document *doc = &a->documents[index];
    if (!doc->dirty) {
        if (doc->draft_present && !doc->recovery && !doc->draft_blocked)
            document_draft_discard(a, doc, false);
        return true;
    }
    ps_document_result r = ps_text_document_save(&doc->file, nk_str_get_const(&doc->edit.string),
                                                 (size_t)nk_str_len_char(&doc->edit.string));
    if (r != PS_DOCUMENT_OK) {
        int pending = a->document_active == index ? a->document_pending : 0;
        document_select(a, index);
        a->document_pending = pending;
        document_error(a, doc, r);
        return false;
    }
    doc->dirty = false;
    doc->error[0] = 0;
    if (!doc->draft_blocked)
        document_draft_discard(a, doc, false);
    status(a, "Datei gespeichert.");
    return true;
}
static bool documents_save_all(app *a) {
    for (unsigned i = 0; i < a->document_count; i++)
        if (!document_save(a, i))
            return false;
    return true;
}
static void document_remove(app *a) {
    unsigned index = a->document_active;
    workspace_document *doc = active_document(a);
    if (!doc)
        return;
    ps_text_document_destroy(&doc->file);
    ps_autosave_destroy(doc->recovery);
    free(doc->draft_text);
    nk_textedit_free(&doc->edit);
    memmove(doc, doc + 1, (a->document_count - index - 1) * sizeof *doc);
    a->document_count--;
    memset(&a->documents[a->document_count], 0, sizeof *doc);
    a->document_pending = 0;
    a->workspace_preview_path[0] = 0;
    if (a->document_count)
        document_select(a, index < a->document_count ? index : a->document_count - 1);
    else
        a->tab = 0;
}
static void documents_clear(app *a) {
    while (a->document_count) {
        a->document_active = a->document_count - 1;
        document_remove(a);
    }
}
static bool document_reload(app *a) {
    workspace_document *doc = active_document(a);
    if (!doc)
        return false;
    ps_text_document replacement = {0};
    ps_document_result r = ps_text_document_open(&replacement, doc->file.path);
    bool changed =
        r == PS_DOCUMENT_OK && (replacement.length != doc->file.length ||
                                memcmp(replacement.saved, doc->file.saved, replacement.length));
    if (r == PS_DOCUMENT_OK && !set_editor_text(&doc->edit, replacement.saved, replacement.length))
        r = PS_DOCUMENT_MEMORY;
    if (r != PS_DOCUMENT_OK) {
        ps_text_document_destroy(&replacement);
        document_error(a, doc, r);
        return false;
    }
    ps_text_document_destroy(&doc->file);
    doc->file = replacement;
    doc->dirty = false;
    doc->error[0] = 0;
    a->document_pending = 0;
    if (changed && a->loaded)
        invalidate_build(a);
    if (!doc->draft_blocked)
        document_draft_discard(a, doc, false);
    status(a, "Datei neu geladen.");
    return true;
}
static void document_request(app *a, int action) {
    workspace_document *doc = active_document(a);
    if (!doc)
        return;
    if (doc->recovery) {
        if (action == 1)
            document_remove(a); /* Close without deciding; keep the recovery bundle. */
        return;
    }
    if (doc->dirty)
        a->document_pending = action;
    else if (action == 1) {
        if (doc->draft_present && !doc->draft_blocked && !document_draft_discard(a, doc, false))
            return;
        document_remove(a);
    } else
        document_reload(a);
}
static bool document_matches(const char *left, const char *right) {
    return !strcmp(left, right) || ps_text_document_same_file(left, right);
}
static void preview_workspace_path(app *a, const char *path) {
    if (a->recovery)
        return;
    /* Project sources always route to their existing editor, including aliases. */
    if (a->loaded) {
        char source[4096];
        join(source, sizeof source, a->project, experiment_source(a));
        bool experiment = document_matches(path, source);
        join(source, sizeof source, a->project, analysis_source(a));
        if (experiment || document_matches(path, source)) {
            a->project_manager = false;
            a->tab = experiment ? 0 : 2;
            if (!experiment)
                a->analysis_tab = 1;
            snprintf(a->workspace_preview_path, sizeof a->workspace_preview_path, "%s", path);
            return;
        }
    }
    for (unsigned i = 0; i < a->document_count; i++) {
        if (!strcmp(path, a->documents[i].path) ||
            document_matches(path, a->documents[i].file.path)) {
            document_select(a, i);
            return;
        }
    }
    if (a->document_count == WORKSPACE_DOCUMENTS) {
        status(a, "16 Dokumente geöffnet. Zuerst ein Dokument schließen.");
        return;
    }
    workspace_document *doc = &a->documents[a->document_count];
    ps_document_result r = ps_text_document_open(&doc->file, path);
    if (r != PS_DOCUMENT_OK) {
        document_error(a, NULL, r);
        return;
    }
    nk_textedit_init_default(&doc->edit);
    if (!set_editor_text(&doc->edit, doc->file.saved, doc->file.length)) {
        nk_textedit_free(&doc->edit);
        ps_text_document_destroy(&doc->file);
        document_error(a, NULL, PS_DOCUMENT_MEMORY);
        return;
    }
    a->document_count++;
    snprintf(doc->path, sizeof doc->path, "%s", path);
    document_check_recovery(a, doc);
    document_select(a, a->document_count - 1);
}
static void save_active_document(app *a) {
    if (a->tab == 8 && active_document(a))
        document_save(a, a->document_active);
    else
        save_project(a);
}
