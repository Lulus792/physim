static size_t workspace_tree_test_find(app *a, const char *name, unsigned root) {
    for (size_t i = 0; i < a->workspace_tree.count; i++)
        if (a->workspace_tree.entries[i].root == root &&
            !strcmp(workspace_name(a->workspace_tree.entries[i].path), name))
            return i;
    return SIZE_MAX;
}
static bool workspace_tree_test_target(app *a, const char *name, unsigned root) {
    size_t index = workspace_tree_test_find(a, name, root);
    if (index == SIZE_MAX)
        return false;
    snprintf(a->workspace_tree_test_path, sizeof a->workspace_tree_test_path, "%s",
             a->workspace_tree.entries[index].path);
    return true;
}
static bool workspace_tree_test_write(const char *path) {
    FILE *f = fopen(path, "wb");
    if (!f)
        return false;
    bool ok = fputs("Tree nested preview: α = 42\n", f) >= 0;
    return !fclose(f) && ok;
}
static bool workspace_tree_test_start(app *a, const char *directory) {
    char root[4096], folder[4096], path[4096], extra[4096];
    join(root, sizeof root, directory, "tree-root");
    join(folder, sizeof folder, root, "models/oscillator");
    join(path, sizeof path, folder, "notes.txt");
    if (!SDL_CreateDirectory(folder) || !workspace_tree_test_write(path))
        return false;
    join(extra, sizeof extra, directory, "references");
    join(path, sizeof path, extra, "reference.txt");
    if (!SDL_CreateDirectory(extra) || !workspace_tree_test_write(path))
        return false;
    if (!open_workspace_path(a, root))
        return false;
    add_workspace_path(a, extra);
    return !a->loaded && workspace_tree_test_target(a, "models", 0);
}
static void workspace_tree_test_frame(app *a, int *stage, int *code, const char **capture) {
    bool ok = true;
    switch ((*stage)++) {
    case 0:
        break;
    case 1:
    case 5:
    case 9:
    case 14:
    case 18:
        ok = a->workspace_tree_test_bounds.w > 0;
        test_mouse(a, a->workspace_tree_test_bounds, true);
        break;
    case 2:
    case 6:
    case 10:
    case 15:
    case 19:
        test_mouse(a, a->workspace_tree_test_bounds, false);
        break;
    case 4:
        ok = workspace_tree_test_target(a, "oscillator", 0);
        break;
    case 8:
        ok = workspace_tree_test_target(a, "notes.txt", 0);
        break;
    case 12:
        ok = a->tab == 8 && strstr(active_document(a)->file.saved, "Tree nested preview: α = 42") &&
             !a->loaded && !a->job.running && !a->runner.running;
        refresh_workspace_entries(a);
        ok = ok && workspace_tree_test_find(a, "notes.txt", 0) != SIZE_MAX;
        test_mouse(a, nk_rect(900, 700, 1, 1), false);
        break;
    case 13:
        *capture = "workspace-tree-open.bmp";
        ok = workspace_tree_test_target(a, "models", 0);
        break;
    case 17:
        ok = workspace_tree_test_find(a, "notes.txt", 0) == SIZE_MAX &&
             strstr(active_document(a)->file.saved, "Tree nested preview") &&
             workspace_tree_test_target(a, "references", 1);
        break;
    case 21:
        ok = workspace_tree_test_find(a, "reference.txt", 1) != SIZE_MAX;
        test_mouse(a, nk_rect(900, 700, 1, 1), false);
        break;
    case 22:
        *capture = "workspace-tree-added.bmp";
        a->quitting = true;
        break;
    }
    if (!ok) {
        *code = 1;
        a->quitting = true;
    }
    if (a->quitting)
        printf("WORKSPACE TREE UI: %s (stage %d)\n", *code ? "FAILED" : "PASSED", *stage);
}
