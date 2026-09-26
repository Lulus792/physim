#ifndef PHYSIM_LANGUAGE_CONSTRAINTS_H
#define PHYSIM_LANGUAGE_CONSTRAINTS_H
/* Internal compiled-language value bindings; no stable ABI. */
#include "language_array.h"
#include "language_mechanics.h"

typedef struct {
    int64_t body_a, body_b;
    ps_contact contact;
} psrt_graph_contact;
typedef struct {
    int64_t body_a, body_b;
    ps_distance_joint joint;
} psrt_graph_joint;
typedef struct {
    ps_body bodies[PS_CONTACT_GRAPH_MAX_BODIES];
    ps_constraint_graph_solution solution;
} psrt_constraint_storage;
typedef struct {
    psrt_array storage;
    int64_t body_count, contact_count, joint_count;
    double normal_error, projection_error, joint_velocity_error, joint_length_error;
} psrt_constraint_result;

static inline void psrt_constraints_drop(void *value) {
    psrt_constraint_result *r = value;
    psrt_array_destroy(&r->storage);
    memset(r, 0, sizeof *r);
}
static inline ps_result psrt_constraints_copy(void *destination, const void *source) {
    const psrt_constraint_result *s = source;
    psrt_constraint_result r = *s;
    ps_result status = psrt_array_clone(&s->storage, &r.storage);
    if (status == PS_OK)
        *(psrt_constraint_result *)destination = r;
    return status;
}
static inline void psrt_constraints_keep(psrt_constraint_result *value, psrt_site site) {
    psrt_constraint_result copy;
    if (psrt_constraints_copy(&copy, value) != PS_OK)
        psrt_fail(site, "Constraint result ownership limit exceeded");
    *value = copy;
}
static inline uint32_t psrt_constraint_index(int64_t index, bool world, psrt_site site) {
    if (world && index == -1)
        return PS_CONTACT_WORLD;
    if (index < 0 || index >= PS_CONTACT_GRAPH_MAX_BODIES)
        psrt_fail(site, "Constraint body index out of bounds");
    return (uint32_t)index;
}
static inline void psrt_constraint_pair(int64_t a, int64_t b, psrt_site site) {
    (void)psrt_constraint_index(a, false, site);
    (void)psrt_constraint_index(b, true, site);
    if (a == b)
        psrt_fail(site, "Constraint must connect distinct body indices");
}
static inline psrt_graph_contact psrt_graph_contact_make(ps_contact_manifold contacts,
                                                         int64_t index, int64_t a, int64_t b,
                                                         psrt_site site) {
    psrt_constraint_pair(a, b, site);
    return (psrt_graph_contact){a, b, psrt_contact_at(contacts, index, site)};
}
static inline psrt_graph_joint psrt_graph_joint_make(ps_distance_joint joint, int64_t a, int64_t b,
                                                     psrt_site site) {
    psrt_constraint_pair(a, b, site);
    return (psrt_graph_joint){a, b, joint};
}
static inline psrt_constraint_result
psrt_constraints_solve(ps_allocator allocator, ps_contact_solver solver, const ps_body *bodies,
                       size_t body_count, const psrt_graph_contact *contacts, size_t contact_count,
                       const psrt_graph_joint *joints, size_t joint_count, double dt,
                       psrt_site site) {
    if (body_count > PS_CONTACT_GRAPH_MAX_BODIES || contact_count > PS_CONTACT_GRAPH_MAX_CONTACTS ||
        joint_count > PS_CONSTRAINT_GRAPH_MAX_JOINTS)
        psrt_fail(site,
                  "Constraint graph capacity exceeded (128 bodies, 512 contacts, 256 joints)");
    if ((body_count && !bodies) || (contact_count && !contacts) || (joint_count && !joints))
        psrt_fail(site, "Missing constraint graph inputs");
    psrt_constraint_storage storage = {0};
    if (body_count)
        memcpy(storage.bodies, bodies, body_count * sizeof *bodies);
    ps_contact_constraint cs[PS_CONTACT_GRAPH_MAX_CONTACTS];
    ps_distance_constraint js[PS_CONSTRAINT_GRAPH_MAX_JOINTS];
    for (size_t i = 0; i < contact_count; i++) {
        psrt_constraint_pair(contacts[i].body_a, contacts[i].body_b, site);
        cs[i] = (ps_contact_constraint){psrt_constraint_index(contacts[i].body_a, false, site),
                                        psrt_constraint_index(contacts[i].body_b, true, site),
                                        contacts[i].contact};
    }
    for (size_t i = 0; i < joint_count; i++) {
        psrt_constraint_pair(joints[i].body_a, joints[i].body_b, site);
        js[i] = (ps_distance_constraint){psrt_constraint_index(joints[i].body_a, false, site),
                                         psrt_constraint_index(joints[i].body_b, true, site),
                                         joints[i].joint};
    }
    if (ps_constraints_resolve_graph(storage.bodies, body_count, cs, contact_count, js, joint_count,
                                     &solver, dt, &storage.solution) != PS_OK)
        psrt_fail(
            site,
            "Constraint graph solver failed: invalid indices, singular joint or numeric range");
    static const psrt_element_type element = {sizeof(psrt_constraint_storage), NULL, NULL};
    psrt_constraint_result result = {0};
    /* Allocate only after solving succeeds. Both operations leave no owner on error. */
    if (psrt_array_init(&element, allocator, 1, &result.storage) != PS_OK ||
        psrt_array_replace(&result.storage, 0, 0, &storage, 1) != PS_OK)
        psrt_fail(site, "Constraint result memory budget or allocation exhausted");
    result.body_count = (int64_t)body_count;
    result.contact_count = (int64_t)contact_count;
    result.joint_count = (int64_t)joint_count;
    result.normal_error = storage.solution.contacts.max_normal_error_m_s;
    result.projection_error = storage.solution.contacts.max_projection_error_m;
    result.joint_velocity_error = storage.solution.max_joint_velocity_error_m_s;
    result.joint_length_error = storage.solution.max_joint_length_error_m;
    return result;
}
static inline const psrt_constraint_storage *psrt_constraints_data(psrt_constraint_result result,
                                                                   psrt_site site) {
    if (!result.storage.block)
        psrt_fail(site, "Uninitialized constraint result");
    return psrt_array_data(&result.storage);
}
static inline ps_body psrt_constraints_body(psrt_constraint_result result, int64_t index,
                                            psrt_site site) {
    if (index < 0 || index >= result.body_count)
        psrt_fail(site, "Constraint result body index out of bounds");
    return psrt_constraints_data(result, site)->bodies[index];
}
static inline ps_vec3 psrt_constraints_contact_impulse(psrt_constraint_result result, int64_t index,
                                                       psrt_site site) {
    if (index < 0 || index >= result.contact_count)
        psrt_fail(site, "Constraint result contact index out of bounds");
    return psrt_constraints_data(result, site)->solution.contacts.impulse_on_a_ns[index];
}
static inline ps_vec3 psrt_constraints_joint_impulse(psrt_constraint_result result, int64_t index,
                                                     psrt_site site) {
    if (index < 0 || index >= result.joint_count)
        psrt_fail(site, "Constraint result joint index out of bounds");
    return psrt_constraints_data(result, site)->solution.joint_impulse_on_a_ns[index];
}
static inline psrt_array psrt_constraints_bodies(ps_allocator allocator,
                                                 psrt_constraint_result result, psrt_site site) {
    const psrt_constraint_storage *storage = psrt_constraints_data(result, site);
    static const psrt_element_type element = {sizeof(ps_body), NULL, NULL};
    psrt_array array;
    if (psrt_array_init(&element, allocator, 0, &array) != PS_OK ||
        psrt_array_replace(&array, 0, 0, storage->bodies, (size_t)result.body_count) != PS_OK)
        psrt_fail(site, "Body array memory budget or allocation exhausted");
    return array;
}
#endif
