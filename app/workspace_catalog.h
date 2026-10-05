#ifndef PS_WORKSPACE_CATALOG_H
#define PS_WORKSPACE_CATALOG_H
#include "workspace_state.h"
#define PS_WORKSPACE_MAX 8u
#define PS_WORKSPACE_NAME_BYTES 64u
typedef struct { char name[PS_WORKSPACE_NAME_BYTES]; ps_workspace_state state; } ps_named_workspace_entry;
/* Large snapshots belong on the heap, including transaction copies. */
typedef struct { uint32_t count; ps_named_workspace_entry entries[PS_WORKSPACE_MAX]; } ps_workspace_catalog;
bool ps_workspace_catalog_valid(const ps_workspace_catalog *catalog);
ps_result ps_workspace_catalog_put(ps_workspace_catalog *catalog, const char *name, const ps_workspace_state *state);
ps_result ps_workspace_catalog_remove(ps_workspace_catalog *catalog, uint32_t index);
/* Versioned personal catalog. Mutations and reads preserve outputs on error.
 * Writes replace a closed sibling file; concurrent instances use last-save wins. */
ps_result ps_workspace_catalog_read(const char *path, ps_workspace_catalog *catalog);
ps_result ps_workspace_catalog_write(const char *path, const ps_workspace_catalog *catalog);
#endif
