#ifndef PHYSIM_DOCKING_H
#define PHYSIM_DOCKING_H
#include "physim/core.h"
enum { PS_DOCK_SIDEBAR, PS_DOCK_WORKSPACE, PS_DOCK_LOG, PS_DOCK_INSPECTOR, PS_DOCK_PANELS };
enum { PS_DOCK_EMPTY, PS_DOCK_GROUP, PS_DOCK_X, PS_DOCK_Y };
enum { PS_DOCK_TAB, PS_DOCK_LEFT, PS_DOCK_RIGHT, PS_DOCK_TOP, PS_DOCK_BOTTOM };
#define PS_DOCK_NODES 7u
#define PS_DOCK_ALL ((1u << PS_DOCK_PANELS)-1u)
#define PS_DOCK_NONE UINT32_MAX
#define PS_DOCK_WIRE_BYTES (PS_DOCK_NODES*24u+12u+PS_DOCK_PANELS*16u)
typedef struct { uint32_t kind, first, second, ratio, panels, active; } ps_dock_node;
typedef struct { uint32_t x, y, w, h; } ps_dock_float;
typedef struct {
    ps_dock_node nodes[PS_DOCK_NODES];
    uint32_t root, floating, hidden;
    ps_dock_float floats[PS_DOCK_PANELS];
} ps_dock_layout;
#define PS_DOCK_DEFAULT_INITIALIZER { \
    {{PS_DOCK_GROUP,0,0,0,1,0}, {PS_DOCK_GROUP,0,0,0,2,1}, \
     {PS_DOCK_GROUP,0,0,0,4,2}, {PS_DOCK_GROUP,0,0,0,8,3}, \
     {PS_DOCK_X,1,3,768,0,0}, {PS_DOCK_X,0,4,190,0,0}, \
     {PS_DOCK_Y,5,2,800,0,0}}, 6,0,0, \
    {{24,96,280,560},{300,96,720,580},{80,140,600,240},{780,96,280,560}} }
extern const ps_dock_layout PS_DOCK_DEFAULT;
bool ps_dock_valid(const ps_dock_layout *layout);
uint32_t ps_dock_group(const ps_dock_layout *layout, uint32_t panel);
/* All mutations are transactional. target is a docked panel or NONE for an empty root. */
bool ps_dock_move(ps_dock_layout *layout, uint32_t panel, uint32_t target, uint32_t side);
bool ps_dock_float_panel(ps_dock_layout *layout, uint32_t panel, ps_dock_float rect);
bool ps_dock_hide(ps_dock_layout *layout, uint32_t panel);
/* Restore a hidden panel beside the first docked panel, then select it. */
bool ps_dock_reveal(ps_dock_layout *layout, uint32_t panel, uint32_t side);
bool ps_dock_select(ps_dock_layout *layout, uint32_t panel);
/* Private fixed-width encoding used by preferences and named layouts.
 * Decode is transactional; both functions require exactly WIRE_BYTES. */
bool ps_dock_encode(const ps_dock_layout *layout,unsigned char *bytes,size_t size);
bool ps_dock_decode(const unsigned char *bytes,size_t size,ps_dock_layout *layout);
#endif
