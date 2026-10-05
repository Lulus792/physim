#include "docking.h"
#include <string.h>
const ps_dock_layout PS_DOCK_DEFAULT = PS_DOCK_DEFAULT_INITIALIZER;
static bool walk(const ps_dock_layout *d, uint32_t id, uint32_t *visited, uint32_t *panels) {
    if (id >= PS_DOCK_NODES || (*visited & (1u << id))) return false;
    *visited |= 1u << id;
    const ps_dock_node *n = &d->nodes[id];
    if (n->kind == PS_DOCK_GROUP) {
        if (!n->panels || (n->panels & ~PS_DOCK_ALL) || (*panels & n->panels) ||
            n->active >= PS_DOCK_PANELS || !(n->panels & (1u << n->active)) ||
            n->first || n->second || n->ratio) return false;
        *panels |= n->panels;
        return true;
    }
    return (n->kind == PS_DOCK_X || n->kind == PS_DOCK_Y) && !n->panels && !n->active &&
           n->ratio >= 64 && n->ratio <= 960 && n->first != n->second &&
           walk(d, n->first, visited, panels) && walk(d, n->second, visited, panels);
}
bool ps_dock_valid(const ps_dock_layout *d) {
    if (!d || ((d->floating | d->hidden) & ~PS_DOCK_ALL) || (d->floating & d->hidden)) return false;
    uint32_t visited = 0, panels = 0;
    if (d->root != PS_DOCK_NONE && !walk(d, d->root, &visited, &panels)) return false;
    if ((panels & (d->floating | d->hidden)) || (panels | d->floating | d->hidden) != PS_DOCK_ALL) return false;
    for (uint32_t i = 0; i < PS_DOCK_NODES; i++)
        if (!(visited & (1u << i)) && memcmp(&d->nodes[i], &(ps_dock_node){0}, sizeof d->nodes[i])) return false;
    for (unsigned i = 0; i < PS_DOCK_PANELS; i++) {
        const ps_dock_float *r = &d->floats[i];
        if (r->x > 8192 || r->y > 8192 || r->w < 208 || r->w > 8192 || r->h < 120 || r->h > 8192) return false;
    }
    return true;
}
uint32_t ps_dock_group(const ps_dock_layout *d, uint32_t panel) {
    if (!d || panel >= PS_DOCK_PANELS) return PS_DOCK_NONE;
    for (uint32_t i = 0; i < PS_DOCK_NODES; i++)
        if (d->nodes[i].kind == PS_DOCK_GROUP && (d->nodes[i].panels & (1u << panel))) return i;
    return PS_DOCK_NONE;
}
static void detach(ps_dock_layout *d, uint32_t panel) {
    uint32_t id = ps_dock_group(d, panel), bit = 1u << panel;
    d->floating &= ~bit; d->hidden &= ~bit;
    if (id == PS_DOCK_NONE) return;
    ps_dock_node *n = &d->nodes[id];
    n->panels &= ~bit;
    if (n->panels) {
        if (n->active == panel)
            for (unsigned i = 0; i < PS_DOCK_PANELS; i++) if (n->panels & (1u << i)) { n->active = i; break; }
        return;
    }
    if (d->root == id) { d->root = PS_DOCK_NONE; memset(n, 0, sizeof *n); return; }
    for (unsigned i = 0; i < PS_DOCK_NODES; i++) {
        ps_dock_node *parent = &d->nodes[i];
        if ((parent->kind == PS_DOCK_X || parent->kind == PS_DOCK_Y) && (parent->first == id || parent->second == id)) {
            uint32_t sibling = parent->first == id ? parent->second : parent->first;
            *parent = d->nodes[sibling];
            memset(&d->nodes[sibling], 0, sizeof d->nodes[sibling]);
            memset(n, 0, sizeof *n);
            return;
        }
    }
}
static uint32_t unused(const ps_dock_layout *d) {
    for (unsigned i = 0; i < PS_DOCK_NODES; i++) if (!d->nodes[i].kind) return i;
    return PS_DOCK_NONE;
}
bool ps_dock_move(ps_dock_layout *d, uint32_t panel, uint32_t target, uint32_t side) {
    if (!ps_dock_valid(d) || panel >= PS_DOCK_PANELS || side > PS_DOCK_BOTTOM || panel == target) return false;
    if (target != PS_DOCK_NONE && ps_dock_group(d, target) == PS_DOCK_NONE) return false;
    ps_dock_layout next = *d;
    detach(&next, panel);
    uint32_t group = target == PS_DOCK_NONE ? PS_DOCK_NONE : ps_dock_group(&next, target);
    if (group == PS_DOCK_NONE) {
        if (next.root != PS_DOCK_NONE) return false;
        uint32_t id = unused(&next);
        next.nodes[id] = (ps_dock_node){.kind=PS_DOCK_GROUP,.panels=1u<<panel,.active=panel}; next.root=id;
    } else if (side == PS_DOCK_TAB) {
        next.nodes[group].panels |= 1u << panel; next.nodes[group].active=panel;
    } else {
        uint32_t old = unused(&next); if (old == PS_DOCK_NONE) return false;
        next.nodes[old] = next.nodes[group];
        uint32_t added = unused(&next); if (added == PS_DOCK_NONE) return false;
        next.nodes[added] = (ps_dock_node){.kind=PS_DOCK_GROUP,.panels=1u<<panel,.active=panel};
        bool before = side == PS_DOCK_LEFT || side == PS_DOCK_TOP;
        next.nodes[group] = (ps_dock_node){.kind=side <= PS_DOCK_RIGHT ? PS_DOCK_X : PS_DOCK_Y,
            .first=before?added:old,.second=before?old:added,.ratio=512};
    }
    if (!ps_dock_valid(&next)) return false;
    *d=next; return true;
}
bool ps_dock_float_panel(ps_dock_layout *d, uint32_t panel, ps_dock_float rect) {
    if (!ps_dock_valid(d) || panel >= PS_DOCK_PANELS) return false;
    ps_dock_layout next=*d; detach(&next,panel);
    next.floating |= 1u<<panel; next.floats[panel]=rect;
    if (!ps_dock_valid(&next)) return false;
    *d=next; return true;
}
bool ps_dock_hide(ps_dock_layout *d, uint32_t panel) {
    if (!ps_dock_valid(d) || panel >= PS_DOCK_PANELS) return false;
    ps_dock_layout next=*d; detach(&next,panel); next.hidden|=1u<<panel;
    if (!ps_dock_valid(&next)) return false;
    *d=next; return true;
}
bool ps_dock_select(ps_dock_layout *d, uint32_t panel) {
    if (!ps_dock_valid(d) || panel >= PS_DOCK_PANELS) return false;
    uint32_t id=ps_dock_group(d,panel);
    if (id==PS_DOCK_NONE) return (d->floating & (1u<<panel)) != 0;
    d->nodes[id].active=panel; return true;
}
bool ps_dock_reveal(ps_dock_layout *d, uint32_t panel, uint32_t side) {
    if(!ps_dock_valid(d) || panel>=PS_DOCK_PANELS || side>PS_DOCK_BOTTOM) return false;
    if(d->hidden&(1u<<panel)) {
        uint32_t target=PS_DOCK_NONE;
        for(unsigned p=0;p<PS_DOCK_PANELS;p++)
            if(ps_dock_group(d,p)!=PS_DOCK_NONE) { target=p;break; }
        if(!ps_dock_move(d,panel,target,side)) return false;
    }
    return ps_dock_select(d,panel);
}
