#ifndef PHYSIM_WORKSPACE_TREE_H
#define PHYSIM_WORKSPACE_TREE_H
#include "physim/core.h"
enum { PS_WORKSPACE_TREE_LIMIT = 8192, PS_WORKSPACE_TREE_DEPTH = 64 };
typedef struct {
    char *path;
    unsigned depth, root;
    bool directory, expanded, missing;
    ps_result error;
} ps_workspace_entry;
/* Visible entries in preorder. All paths are owned by the tree. Entry pointers
 * and indices must be reacquired after a successful mutation. Initialize to 0;
 * limit=0 selects the default. Collapsing releases descendant storage. */
typedef struct {
    ps_workspace_entry *entries;
    size_t count, capacity, limit;
} ps_workspace_tree;
void ps_workspace_tree_destroy(ps_workspace_tree *tree);
/* Main root first, followed by added files or directories. Refresh preserves
 * expanded paths within each root. A new main root starts expanded. Missing
 * roots remain visible. PS_MEMORY/PS_INVALID leave the old tree unchanged;
 * PS_IO/PS_LIMIT may return a partial tree with explicit per-entry errors. */
ps_result ps_workspace_tree_refresh(ps_workspace_tree *tree, const char *const *roots,
                                    size_t count);
/* Toggle directories only. File entries return PS_INVALID. Enumeration happens
 * on expansion, never during rendering. Limits also bound cyclic symlinks. */
ps_result ps_workspace_tree_toggle(ps_workspace_tree *tree, size_t index);
#endif
