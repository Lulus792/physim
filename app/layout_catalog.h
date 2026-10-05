#ifndef PS_LAYOUT_CATALOG_H
#define PS_LAYOUT_CATALOG_H
#include "docking.h"
#define PS_LAYOUT_MAX 8u
#define PS_LAYOUT_NAME_BYTES 64u
typedef struct {
    ps_dock_layout dock;
    uint32_t sidebar_width,inspector_width,log_height,show_log;
} ps_layout_state;
typedef struct {char name[PS_LAYOUT_NAME_BYTES];ps_layout_state state;} ps_layout_entry;
typedef struct {uint32_t count;ps_layout_entry entries[PS_LAYOUT_MAX];} ps_layout_catalog;
bool ps_layout_state_valid(const ps_layout_state *state);
bool ps_layout_catalog_valid(const ps_layout_catalog *catalog);
/* Exact, case-sensitive UTF-8 names. Existing names are updated in place.
 * All model and read mutations preserve output on error. */
ps_result ps_layout_catalog_put(ps_layout_catalog *catalog,const char *name,const ps_layout_state *state);
ps_result ps_layout_catalog_remove(ps_layout_catalog *catalog,uint32_t index);
ps_result ps_layout_catalog_read(const char *path,ps_layout_catalog *catalog);
/* Atomic sibling-file replacement; no implicit writes after corrupt reads.
 * Concurrent instances use the last completed save. Not power-loss durable. */
ps_result ps_layout_catalog_write(const char *path,const ps_layout_catalog *catalog);
#endif
