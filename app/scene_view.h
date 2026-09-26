#ifndef PHYSIM_SCENE_VIEW_H
#define PHYSIM_SCENE_VIEW_H
#include "physim/experiment.h"
/* Stable IDs follow objects across order changes; anonymous slots retain the legacy policy. */
typedef struct {
    uint32_t hidden, count, shapes[PS_MAX_OBJECTS], ids[PS_MAX_OBJECTS];
} ps_scene_view;
/* Requires a validated scene (including count <= PS_MAX_OBJECTS). */
static inline void ps_scene_view_sync(ps_scene_view *view, const ps_scene *scene) {
    bool changed = view->count != scene->count;
    for (uint32_t i = 0; i < scene->count; i++)
        changed |= view->shapes[i] != scene->objects[i].shape;
    uint32_t hidden = 0;
    for (uint32_t i = 0; i < scene->count; i++) {
        uint32_t id = scene->objects[i].id;
        if (id) {
            for (uint32_t j = 0; j < view->count; j++)
                if (view->ids[j] == id && (view->hidden & (UINT32_C(1) << j)))
                    hidden |= UINT32_C(1) << i;
        } else if (!changed && !view->ids[i] && (view->hidden & (UINT32_C(1) << i)))
            hidden |= UINT32_C(1) << i;
    }
    for (uint32_t i = 0; i < scene->count; i++) {
        view->shapes[i] = scene->objects[i].shape;
        view->ids[i] = scene->objects[i].id;
    }
    view->hidden = hidden;
    view->count = scene->count;
}
static inline bool ps_scene_view_visible(const ps_scene_view *view, uint32_t slot) {
    return slot < PS_MAX_OBJECTS && !(view->hidden & (UINT32_C(1) << slot));
}
#endif
