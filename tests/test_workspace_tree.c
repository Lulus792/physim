#include "workspace_tree.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Workspace tree line %d: %s\n", __LINE__, #x);                         \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static bool file_write(const char *path) {
    FILE *f = fopen(path, "wb");
    if (!f)
        return false;
    bool ok = fputs("workspace\n", f) >= 0;
    return !fclose(f) && ok;
}
static bool path_equal(const char *a, const char *b) {
    while (*a && *b) {
        char x = *a++, y = *b++;
#ifdef _WIN32
        if (x == '\\')
            x = '/';
        if (y == '\\')
            y = '/';
#endif
        if (x != y)
            return false;
    }
    return *a == *b;
}
static size_t find(const ps_workspace_tree *tree, const char *path, unsigned root) {
    for (size_t i = 0; i < tree->count; i++)
        if (tree->entries[i].root == root && path_equal(tree->entries[i].path, path))
            return i;
    return SIZE_MAX;
}
int main(int argc, char **argv) {
    CHECK(argc == 2);
    char base[4096], root[4096], extra[4096], missing[4096], attached[4096];
    snprintf(base, sizeof base, "%s/tree-%llu ä", argv[1], (unsigned long long)SDL_GetTicksNS());
    snprintf(root, sizeof root, "%s/main", base);
    snprintf(extra, sizeof extra, "%s/extra", base);
    snprintf(missing, sizeof missing, "%s/missing", base);
    snprintf(attached, sizeof attached, "%s/attached.txt", base);
    CHECK(SDL_CreateDirectory(root) && SDL_CreateDirectory(extra) && file_write(attached));
    char folder[4096], nested[4096], document[4096], path[4096];
    snprintf(folder, sizeof folder, "%s/aaa", root);
    snprintf(nested, sizeof nested, "%s/nested", folder);
    snprintf(document, sizeof document, "%s/λ.txt", nested);
    CHECK(SDL_CreateDirectory(nested) && file_write(document));
    for (unsigned i = 0; i < 200; i++) {
        snprintf(path, sizeof path, "%s/file-%03u.txt", root, 199 - i);
        CHECK(file_write(path));
    }
    const char *roots[] = {root, attached, extra, missing};
    ps_workspace_tree tree = {0};
    CHECK(ps_workspace_tree_refresh(&tree, roots, 4) == PS_IO);
    CHECK(tree.count == 205 && tree.entries[0].expanded);
    CHECK(tree.entries[1].directory && path_equal(tree.entries[1].path, folder));
    for (size_t i = 3; i < 202; i++)
        CHECK(strcmp(tree.entries[i - 1].path, tree.entries[i].path) < 0);
    CHECK(find(&tree, document, 0) == SIZE_MAX); /* Lazy: no recursive scan yet. */
    CHECK(find(&tree, attached, 1) != SIZE_MAX && find(&tree, extra, 2) != SIZE_MAX);
    CHECK(tree.entries[find(&tree, missing, 3)].missing);
    CHECK(ps_workspace_tree_toggle(&tree, 1) == PS_OK);
    CHECK(ps_workspace_tree_toggle(&tree, find(&tree, nested, 0)) == PS_OK);
    CHECK(find(&tree, document, 0) != SIZE_MAX);
    CHECK(ps_workspace_tree_toggle(&tree, find(&tree, document, 0)) == PS_INVALID);
    snprintf(path, sizeof path, "%s/new.txt", nested);
    CHECK(file_write(path));
    CHECK(ps_workspace_tree_refresh(&tree, roots, 4) == PS_IO);
    CHECK(find(&tree, document, 0) != SIZE_MAX && find(&tree, path, 0) != SIZE_MAX);
    CHECK(tree.entries[find(&tree, nested, 0)].expanded);
    CHECK(SDL_RemovePath(document));
    CHECK(ps_workspace_tree_refresh(&tree, roots, 4) == PS_IO);
    CHECK(find(&tree, document, 0) == SIZE_MAX && find(&tree, path, 0) != SIZE_MAX);
    CHECK(ps_workspace_tree_toggle(&tree, find(&tree, folder, 0)) == PS_OK);
    CHECK(find(&tree, nested, 0) == SIZE_MAX && tree.count == 205);
    CHECK(ps_workspace_tree_refresh(&tree, roots, 4) == PS_IO);
    CHECK(!tree.entries[find(&tree, folder, 0)].expanded);
    CHECK(ps_workspace_tree_toggle(&tree, 0) == PS_OK && tree.count == 4);
    CHECK(ps_workspace_tree_refresh(&tree, roots, 4) == PS_IO && tree.count == 4);
    /* Empty directory and a vanished directory are different states. */
    CHECK(ps_workspace_tree_toggle(&tree, 2) == PS_OK && tree.entries[2].expanded &&
          tree.count == 4);
    CHECK(ps_workspace_tree_toggle(&tree, 2) == PS_OK);
    CHECK(SDL_RemovePath(extra));
    CHECK(ps_workspace_tree_toggle(&tree, 2) == PS_IO && !tree.entries[2].expanded);
    CHECK(ps_workspace_tree_refresh(&tree, roots, 4) == PS_IO && tree.entries[2].missing);
    const char *invalid[] = {root, "\xff"};
    ps_workspace_entry *before = tree.entries;
    CHECK(ps_workspace_tree_refresh(&tree, invalid, 2) == PS_INVALID && tree.entries == before);
    CHECK(ps_workspace_tree_refresh(&tree, NULL, 0) == PS_INVALID && tree.entries == before);
    ps_workspace_tree_destroy(&tree);
    /* Added roots always retain slots, even when the main folder is huge. */
    tree.limit = 12;
    CHECK(ps_workspace_tree_refresh(&tree, roots, 4) != PS_OK);
    CHECK(tree.count == 12 && tree.entries[0].error == PS_LIMIT);
    CHECK(find(&tree, attached, 1) != SIZE_MAX && find(&tree, missing, 3) != SIZE_MAX);
    CHECK(ps_workspace_tree_toggle(&tree, 0) == PS_OK && tree.count == 4);
    ps_workspace_tree_destroy(&tree);
    /* A directory can be the main root and an independent added root. */
    const char *duplicates[] = {root, root};
    CHECK(ps_workspace_tree_refresh(&tree, duplicates, 2) == PS_OK);
    CHECK(!tree.entries[find(&tree, root, 1)].expanded);
    CHECK(ps_workspace_tree_toggle(&tree, find(&tree, root, 1)) == PS_OK);
    CHECK(ps_workspace_tree_toggle(&tree, 0) == PS_OK);
    CHECK(find(&tree, folder, 0) == SIZE_MAX && find(&tree, folder, 1) != SIZE_MAX);
    CHECK(ps_workspace_tree_refresh(&tree, duplicates, 2) == PS_OK);
    CHECK(find(&tree, folder, 0) == SIZE_MAX && find(&tree, folder, 1) != SIZE_MAX);
    ps_workspace_tree_destroy(&tree);
    /* Bound depth even when each folder has only one child (or follows a link). */
    snprintf(root, sizeof root, "%s/deep", base);
    snprintf(path, sizeof path, "%s", root);
    for (unsigned i = 0; i < PS_WORKSPACE_TREE_DEPTH + 2; i++) {
        CHECK(SDL_CreateDirectory(path));
        strcat(path, "/d");
    }
    CHECK(ps_workspace_tree_refresh(&tree, roots, 1) == PS_OK);
    for (size_t i = 1; i < PS_WORKSPACE_TREE_DEPTH; i++)
        CHECK(ps_workspace_tree_toggle(&tree, i) == PS_OK);
    CHECK(tree.count == PS_WORKSPACE_TREE_DEPTH + 1);
    CHECK(ps_workspace_tree_toggle(&tree, PS_WORKSPACE_TREE_DEPTH) == PS_LIMIT);
    CHECK(tree.entries[PS_WORKSPACE_TREE_DEPTH].error == PS_LIMIT);
    ps_workspace_tree_destroy(&tree);
    CHECK(!tree.entries && !tree.count);
    puts("Workspace tree: sorting, lazy nesting, refresh, multiple roots, missing paths and limits "
         "passed");
    return 0;
}
