#ifndef PHYSIM_WORKSPACE_STATE_H
#define PHYSIM_WORKSPACE_STATE_H
#include "physim/core.h"
enum { PS_WORKSPACE_PATH = 4096, PS_WORKSPACE_ADDITIONS = 32,
       PS_WORKSPACE_DOCUMENTS = 16, PS_WORKSPACE_TEXT_LIMIT = 256 * 1024,
       PS_WORKSPACE_SCROLL_LIMIT = 16 * 1024 * 1024 };
typedef struct {
    uint32_t cursor, select_start, select_end, scroll_x, scroll_y;
} ps_workspace_editor;
typedef struct {
    char path[PS_WORKSPACE_PATH];
    ps_workspace_editor editor;
} ps_workspace_document;
typedef struct {
    char root[PS_WORKSPACE_PATH];
    char additions[PS_WORKSPACE_ADDITIONS][PS_WORKSPACE_PATH];
    uint32_t count;
    ps_workspace_document documents[PS_WORKSPACE_DOCUMENTS];
    uint32_t document_count, active_document;
    uint32_t view; /* 0: develop, 1: simulate, 2: analyze, 3: extra document. */
    uint32_t analysis_editor;
    ps_workspace_editor experiment, analysis;
} ps_workspace_state;
/* Private app format. Paths are absolute UTF-8; an empty root represents no
 * saved workspace. Version 1 path-only files remain readable. Editor positions
 * are Unicode scalar indices, clamped against current text on restoration.
 * Read is transactional. Missing files return PS_EOF.
 * Replacement uses a closed, exclusive sibling file; failed writes preserve
 * the previous file. Concurrent instances use the last completed write. */
ps_result ps_workspace_state_read(const char *path, ps_workspace_state *out);
ps_result ps_workspace_state_write(const char *path, const ps_workspace_state *state);
/* Resolve a relative path against the current directory without requiring the
 * target to exist. Windows drive-relative and root-relative paths are rejected.
 * The output is unchanged on error. */
ps_result ps_workspace_absolute(const char *path, char out[PS_WORKSPACE_PATH]);
#endif
