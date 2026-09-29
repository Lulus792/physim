/* This workflow uses the real platform dialogs. An external UI driver selects
 * paths or cancels; no callback or file-list result is injected by this test. */
static char native_dialog_root[4096];
static char native_dialog_previous_status[sizeof ((app *)0)->status];
static bool native_dialog_test_path(const char *actual, const char *relative) {
    char expected[4096], absolute_actual[PS_WORKSPACE_PATH];
    char absolute_expected[PS_WORKSPACE_PATH];
    join(expected, sizeof expected, native_dialog_root, relative);
    if (ps_workspace_absolute(actual, absolute_actual) != PS_OK ||
        ps_workspace_absolute(expected, absolute_expected) != PS_OK)
        return false;
#ifdef _WIN32
    /* Windows dialogs return backslashes; the test CLI also accepts slashes. */
    for (char *p = absolute_actual; *p; ++p)
        if (*p == '\\') *p = '/';
    for (char *p = absolute_expected; *p; ++p)
        if (*p == '\\') *p = '/';
#endif
    if (!strcmp(absolute_actual, absolute_expected))
        return true;
    fprintf(stderr, "Native dialog path mismatch: expected '%s', got '%s'\n",
            absolute_expected, absolute_actual);
    return false;
}
static bool native_dialog_test_start(const char *root) {
    snprintf(native_dialog_root, sizeof native_dialog_root, "%s", root);
    char path[4096];
    join(path, sizeof path, root, "Selected folder ä");
    if (!ps_make_directory(path))
        return false;
    join(path, sizeof path, root, "Added folder ä");
    if (!ps_make_directory(path))
        return false;
    join(path, sizeof path, root, "External file ä.txt");
    FILE *file = fopen(path, "wb");
    if (!file)
        return false;
    bool ok = fputs("Native dialog selection\n", file) >= 0;
    return fclose(file) == 0 && ok;
}
static void native_dialog_test_frame(app *a, bool error_test, int *stage, int *code,
                                     const char **capture) {
    bool ok = true;
    if ((*stage & 1) && a->dialog_pending)
        return;
    switch (*stage) {
    case 0:
        puts("NATIVE DIALOG REQUEST: folder");
        fflush(stdout);
        choose_workspace_path(a, PS_DIALOG_OPEN_FOLDER);
        break;
    case 1:
        if (error_test) {
            ok = !a->workspace_open && !a->workspace_addition_count &&
                 strstr(a->status, "Dateiauswahl fehlgeschlagen") != NULL;
            puts(ok ? "NATIVE DIALOG ERROR: PASSED" : "NATIVE DIALOG ERROR: FAILED");
            a->quitting = true;
        } else
            ok = a->workspace_open && !a->loaded &&
                 native_dialog_test_path(a->workspace, "Selected folder ä");
        break;
    case 2:
        puts("NATIVE DIALOG REQUEST: file");
        fflush(stdout);
        choose_workspace_path(a, PS_DIALOG_ADD_FILE);
        break;
    case 3:
        ok = a->workspace_addition_count == 1 &&
             native_dialog_test_path(a->workspace_additions[0], "External file ä.txt");
        break;
    case 4:
        puts("NATIVE DIALOG REQUEST: addition");
        fflush(stdout);
        choose_workspace_path(a, PS_DIALOG_ADD_FOLDER);
        break;
    case 5:
        ok = a->workspace_addition_count == 2 &&
             native_dialog_test_path(a->workspace_additions[1], "Added folder ä");
        break;
    case 6:
        snprintf(native_dialog_previous_status, sizeof native_dialog_previous_status, "%s",
                 a->status);
        puts("NATIVE DIALOG REQUEST: cancel");
        fflush(stdout);
        choose_workspace_path(a, PS_DIALOG_OPEN_FOLDER);
        break;
    case 7:
        ok = a->workspace_open && a->workspace_addition_count == 2 &&
             native_dialog_test_path(a->workspace, "Selected folder ä") &&
             !strcmp(native_dialog_previous_status, a->status);
        *capture = "native-dialogs.bmp";
        puts(ok ? "NATIVE DIALOGS: PASSED" : "NATIVE DIALOGS: FAILED");
        a->quitting = true;
        break;
    }
    fflush(stdout);
    if (!ok) {
        fprintf(stderr, "Native dialog stage %d failed: %s\n", *stage, a->status);
        *code = 1;
        a->quitting = true;
    }
    ++*stage;
}
