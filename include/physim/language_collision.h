#ifndef PHYSIM_LANGUAGE_COLLISION_H
#define PHYSIM_LANGUAGE_COLLISION_H
/* Internal value bindings for the shared collision library. */
#include "collision.h"
#include "language_array.h"
#include "language_mechanics.h"
typedef struct {
    bool hit;
    ps_sweep_hit value;
} psrt_sweep;
static inline psrt_sweep psrt_sweep_spheres(ps_body a, double ra, ps_vec3 da, ps_body b, double rb,
                                            ps_vec3 db, psrt_site site) {
    psrt_sweep result = {0};
    if (ps_sweep_spheres(&a, ra, da, &b, rb, db, &result.value, &result.hit) != PS_OK)
        psrt_fail(site, "Sphere sweep failed: invalid inputs or numeric range");
    return result;
}
static inline psrt_sweep psrt_sweep_plane(ps_body body, double radius, ps_vec3 displacement,
                                          ps_vec3 point, ps_vec3 normal, psrt_site site) {
    psrt_sweep result = {0};
    if (ps_sweep_sphere_plane(&body, radius, displacement, point, normal, &result.value,
                              &result.hit) != PS_OK)
        psrt_fail(site, "Sphere-plane sweep failed: invalid inputs or numeric range");
    return result;
}
static inline double psrt_sweep_fraction(psrt_sweep sweep, psrt_site site) {
    if (!sweep.hit)
        psrt_fail(site, "Sweep has no contact fraction");
    return sweep.value.fraction;
}
static inline ps_contact_manifold psrt_sweep_contacts(psrt_sweep sweep, psrt_site site) {
    (void)site;
    ps_contact_manifold contacts = {0};
    if (sweep.hit) {
        contacts.count = 1;
        contacts.points[0] = sweep.value.contact;
    }
    return contacts;
}
static inline ps_aabb psrt_aabb_sphere(ps_body body, double radius, psrt_site site) {
    ps_aabb result;
    if (ps_aabb_sphere(&body, radius, &result) != PS_OK)
        psrt_fail(site, "Sphere bounds failed: invalid inputs or numeric range");
    return result;
}
static inline ps_aabb psrt_aabb_box(ps_body body, ps_vec3 size, psrt_site site) {
    ps_aabb result;
    if (ps_aabb_box(&body, size, &result) != PS_OK)
        psrt_fail(site, "Box bounds failed: invalid inputs or numeric range");
    return result;
}
static inline ps_aabb psrt_aabb_swept_sphere(ps_body body, double radius, ps_vec3 displacement,
                                             psrt_site site) {
    ps_aabb result;
    if (ps_aabb_swept_sphere(&body, radius, displacement, &result) != PS_OK)
        psrt_fail(site, "Swept sphere bounds failed: invalid inputs or numeric range");
    return result;
}
static inline psrt_array psrt_collision_pairs(ps_allocator allocator, const ps_aabb *bounds,
                                              size_t count, psrt_site site) {
    size_t required = 0;
    if (count > PS_BROAD_PHASE_MAX_BODIES)
        psrt_fail(site, "Broad phase supports at most 1024 bodies");
    ps_result status = ps_broad_phase(bounds, count, NULL, 0, &required);
    if (status != PS_OK && status != PS_LIMIT)
        psrt_fail(site, "Broad phase bounds are invalid");
    static const psrt_element_type element = {sizeof(ps_collision_pair), NULL, NULL};
    psrt_array result;
    if (psrt_array_init(&element, allocator, 0, &result) != PS_OK ||
        psrt_array_build_begin(&result, required) != PS_OK)
        psrt_fail(site, "Collision pair memory budget or allocation exhausted");
    if (required) {
        /* Exclusive, unpublished trivial block: construct directly without a
         * second large buffer. On failure there are no live elements. */
        ps_collision_pair *pairs = (ps_collision_pair *)(result.block + 1);
        size_t written = 0;
        status = ps_broad_phase(bounds, count, pairs, required, &written);
        if (status != PS_OK || written != required) {
            psrt_array_destroy(&result);
            psrt_fail(site, "Broad phase result construction failed");
        }
        result.block->value.count = written;
    }
    return result;
}
#endif
