#ifndef PHYSIM_UI_GEOMETRY_H
#define PHYSIM_UI_GEOMETRY_H
#include "physim/memory.h"
#include "ui.h"

typedef struct {
    float p[2], uv[2];
    nk_byte color[4];
} ps_ui_vertex;

/* Thread-confined conversion storage. Initialize once; destroy before its owner.
 * Fixed Nuklear views prevent unchecked allocation inside its vertex allocator.
 * Only growth allocates; storage is discarded, never copied, on growth. Commands
 * belong to the caller and are cleared before each conversion attempt. Geometry
 * is readable only after PS_OK, until the next conversion or destroy. No custom
 * draw callbacks with side effects: growth can repeat conversion of commands. */
typedef struct {
    ps_allocator allocator;
    size_t byte_limit, capacity[2];
    void *memory[2];
    struct nk_buffer vertices, indices;
    uint64_t allocations;
} ps_ui_geometry;

/* 0 selects a 64-MiB combined retained-capacity limit; larger limits are clamped.
 * Growth temporarily holds old and new storage. No allocation at init. */
ps_result ps_ui_geometry_init(ps_ui_geometry *geometry, ps_allocator allocator, size_t byte_limit);
void ps_ui_geometry_destroy(ps_ui_geometry *geometry);
void ps_ui_geometry_config(struct nk_convert_config *config,
                           const struct nk_draw_null_texture *null_texture);
ps_result ps_ui_geometry_convert(ps_ui_geometry *geometry, struct nk_context *ui,
                                 struct nk_buffer *commands,
                                 const struct nk_draw_null_texture *null_texture);
#endif
