#ifndef PHYSIM_PREFERENCES_H
#define PHYSIM_PREFERENCES_H
#include "physim/core.h"
#include "docking.h"
enum { PS_THEME_DARK, PS_THEME_LIGHT, PS_THEME_HIGH_CONTRAST, PS_THEME_COUNT };
enum {
    PS_VIEW_VECTORS = 1,
    PS_VIEW_PATHS = 2,
    PS_VIEW_POINTS = 4,
    PS_VIEW_LABELS = 8,
    PS_VIEW_GRID = 16,
    PS_VIEW_ORTHOGRAPHIC = 32,
    PS_VIEW_LOG = 64
};
typedef struct {
    uint32_t width, height, maximized, sidebar_width, log_height;
    uint32_t editor_size, autosave_seconds, view_flags, inspector_open, workspace;
    uint32_t theme;
    ps_dock_layout dock;
} ps_preferences;
extern const ps_preferences PS_PREFERENCES_DEFAULT;
bool ps_preferences_valid(const ps_preferences *settings);
/* Private, versioned app format. Read is transactional; PS_EOF means absent.
 * Write closes a unique sibling file before replacement. Failed writes preserve
 * the previous file. Simultaneous instances use last completed write wins. */
ps_result ps_preferences_read(const char *path, ps_preferences *out);
ps_result ps_preferences_write(const char *path, const ps_preferences *settings);
#endif
