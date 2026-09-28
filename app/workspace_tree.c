#include "workspace_tree.h"
#include "autosave.h"
#include <SDL3/SDL_filesystem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t tree_limit(const ps_workspace_tree *tree) {
    return tree->limit && tree->limit < PS_WORKSPACE_TREE_LIMIT ? tree->limit
                                                                : PS_WORKSPACE_TREE_LIMIT;
}
void ps_workspace_tree_destroy(ps_workspace_tree *tree) {
    if (!tree)
        return;
    for (size_t i = 0; i < tree->count; i++)
        free(tree->entries[i].path);
    free(tree->entries);
    *tree = (ps_workspace_tree){0};
}
static ps_result reserve(ps_workspace_tree *tree, size_t count) {
    if (count > tree_limit(tree))
        return PS_LIMIT;
    if (count <= tree->capacity)
        return PS_OK;
    size_t capacity = tree->capacity ? tree->capacity * 2 : 32;
    if (capacity < count)
        capacity = count;
    if (capacity > tree_limit(tree))
        capacity = tree_limit(tree);
    ps_workspace_entry *entries = realloc(tree->entries, capacity * sizeof *entries);
    if (!entries)
        return PS_MEMORY;
    tree->entries = entries;
    tree->capacity = capacity;
    return PS_OK;
}
static ps_result append(ps_workspace_tree *tree, const char *path, unsigned depth, unsigned root) {
    size_t length = strlen(path);
    if (!length || length >= 4096 || !ps_source_text_valid(path, length))
        return PS_INVALID;
    ps_result r = reserve(tree, tree->count + 1);
    if (r != PS_OK)
        return r;
    char *copy = malloc(length + 1);
    if (!copy)
        return PS_MEMORY;
    memcpy(copy, path, length + 1);
    SDL_PathInfo info;
    bool exists = SDL_GetPathInfo(path, &info);
    tree->entries[tree->count++] =
        (ps_workspace_entry){.path = copy,
                             .depth = depth,
                             .root = root,
                             .directory = exists && info.type == SDL_PATHTYPE_DIRECTORY,
                             .missing = !exists,
                             .error = exists ? PS_OK : PS_IO};
    return PS_OK;
}
static int entry_order(const void *left, const void *right) {
    const ps_workspace_entry *a = left, *b = right;
    if (a->directory != b->directory)
        return a->directory ? -1 : 1;
    return strcmp(a->path, b->path);
}
typedef struct {
    ps_workspace_tree children;
    size_t available;
    unsigned depth, root;
    ps_result result;
} listing;
static SDL_EnumerationResult SDLCALL enumerate(void *userdata, const char *dirname,
                                               const char *name) {
    listing *list = userdata;
    if (!strcmp(name, ".") || !strcmp(name, ".."))
        return SDL_ENUM_CONTINUE;
    if (list->children.count == list->available) {
        list->result = PS_LIMIT;
        return SDL_ENUM_SUCCESS;
    }
    char path[4096];
    int length = snprintf(path, sizeof path, "%s%s", dirname, name);
    if (length < 0 || (size_t)length >= sizeof path) {
        list->result = PS_LIMIT;
        return SDL_ENUM_CONTINUE;
    }
    ps_result r = append(&list->children, path, list->depth, list->root);
    if (r != PS_OK) {
        list->result = r == PS_INVALID ? PS_CORRUPT : r;
        return r == PS_MEMORY ? SDL_ENUM_FAILURE : SDL_ENUM_CONTINUE;
    }
    return SDL_ENUM_CONTINUE;
}
ps_result ps_workspace_tree_toggle(ps_workspace_tree *tree, size_t index) {
    if (!tree || index >= tree->count || !tree->entries[index].directory)
        return PS_INVALID;
    ps_workspace_entry *entry = &tree->entries[index];
    if (entry->expanded) {
        size_t end = index + 1;
        while (end < tree->count && tree->entries[end].depth > entry->depth) {
            free(tree->entries[end].path);
            end++;
        }
        memmove(tree->entries + index + 1, tree->entries + end,
                (tree->count - end) * sizeof *tree->entries);
        tree->count -= end - index - 1;
        entry->expanded = false;
        entry->error = PS_OK;
        return PS_OK;
    }
    if (entry->depth >= PS_WORKSPACE_TREE_DEPTH) {
        entry->error = PS_LIMIT;
        return PS_LIMIT;
    }
    listing list = {.available = tree_limit(tree) - tree->count,
                    .depth = entry->depth + 1,
                    .root = entry->root};
    bool ok = SDL_EnumerateDirectory(entry->path, enumerate, &list);
    ps_result r = ok ? list.result : list.result == PS_MEMORY ? PS_MEMORY : PS_IO;
    if (!ok) {
        entry->error = r;
        ps_workspace_tree_destroy(&list.children);
        return r;
    }
    ps_result allocated = reserve(tree, tree->count + list.children.count);
    if (allocated != PS_OK) {
        tree->entries[index].error = allocated;
        ps_workspace_tree_destroy(&list.children);
        return allocated;
    }
    if (list.children.count > 1)
        qsort(list.children.entries, list.children.count, sizeof *list.children.entries,
              entry_order);
    memmove(tree->entries + index + 1 + list.children.count, tree->entries + index + 1,
            (tree->count - index - 1) * sizeof *tree->entries);
    if (list.children.count)
        memcpy(tree->entries + index + 1, list.children.entries,
               list.children.count * sizeof *tree->entries);
    tree->count += list.children.count;
    free(list.children.entries); /* Paths transferred to the destination. */
    tree->entries[index].expanded = true;
    tree->entries[index].error = r;
    return r;
}
static int expanded_order(const void *left, const void *right) {
    const ps_workspace_entry *a = *(const ps_workspace_entry *const *)left;
    const ps_workspace_entry *b = *(const ps_workspace_entry *const *)right;
    if (a->root != b->root)
        return a->root < b->root ? -1 : 1;
    return strcmp(a->path, b->path);
}
ps_result ps_workspace_tree_refresh(ps_workspace_tree *tree, const char *const *roots,
                                    size_t count) {
    if (!tree || !roots || !count || count > 33 || count > tree_limit(tree))
        return PS_INVALID;
    for (size_t i = 0; i < count; i++)
        if (!roots[i])
            return PS_INVALID;
    ps_workspace_tree next = {.limit = tree->limit};
    ps_result r = PS_OK, result = PS_OK;
    for (size_t i = 0; i < count; i++) {
        r = append(&next, roots[i], 0, (unsigned)i);
        if (r != PS_OK)
            goto fail;
    }
    /* Binary lookup keeps refreshing many expanded directories bounded. Old
     * entries remain alive until the complete new tree has been constructed. */
    const ps_workspace_entry **expanded = malloc((tree->count + 1) * sizeof *expanded);
    if (!expanded) {
        r = PS_MEMORY;
        goto fail;
    }
    size_t expanded_count = 0;
    for (size_t i = 0; i < tree->count; i++)
        if (tree->entries[i].expanded)
            expanded[expanded_count++] = &tree->entries[i];
    qsort(expanded, expanded_count, sizeof *expanded, expanded_order);
    bool new_main = !tree->count || strcmp(tree->entries[0].path, roots[0]);
    for (size_t i = 0; i < next.count; i++) {
        const ps_workspace_entry *entry = &next.entries[i];
        bool open = (i == 0 && new_main) ||
                    bsearch(&entry, expanded, expanded_count, sizeof *expanded, expanded_order);
        if (entry->missing)
            result = PS_IO;
        if (entry->directory && open) {
            r = ps_workspace_tree_toggle(&next, i);
            if (r == PS_MEMORY) {
                free(expanded);
                goto fail;
            }
            if (r != PS_OK)
                result = r;
        }
    }
    free(expanded);
    ps_workspace_tree_destroy(tree);
    *tree = next;
    return result;
fail:
    ps_workspace_tree_destroy(&next);
    return r;
}
