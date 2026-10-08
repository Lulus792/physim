#ifndef PHYSIM_LANGUAGE_MECHANICS_H
#define PHYSIM_LANGUAGE_MECHANICS_H
/* Experimental SI value bindings for generated C17; not a stable public ABI. */
#include "language_runtime.h"
#include "mechanics.h"

typedef struct psrt_joint_result {
    ps_body body_a, body_b;
    ps_distance_joint_solution solution;
} psrt_joint_result;
static inline ps_distance_joint psrt_distance_joint(ps_vec3 anchor_a, ps_vec3 anchor_b,
                                                     double length, double stabilization,
                                                     psrt_site site) {
    ps_distance_joint joint = {anchor_a, anchor_b, length, stabilization};
    if (ps_distance_joint_validate(&joint) != PS_OK)
        psrt_fail(site, "Invalid distance joint anchors, length or stabilization");
    return joint;
}
static inline psrt_joint_result psrt_joint_resolve(ps_distance_joint joint, ps_body a, ps_body b,
                                                   double dt, psrt_site site) {
    psrt_joint_result result = {a, b, {0}};
    ps_result status = ps_distance_joint_resolve(&result.body_a, &result.body_b, &joint, dt,
                                                &result.solution);
    if (status == PS_SINGULAR)
        psrt_fail(site, "Distance joint anchors coincide");
    if (status != PS_OK)
        psrt_fail(site, "Distance joint solver failed: invalid inputs or numeric range");
    return result;
}

/* Solving copies both input bodies. The caller explicitly adopts the results;
 * no user storage is changed if detection/solving traps. A mass-zero Body can
 * represent the static world, including a plane described by a manifold. */
typedef struct psrt_contact_result {
    ps_body body_a, body_b;
    ps_contact_solution solution;
} psrt_contact_result;
static inline ps_contact_solver psrt_contact_solver_default(psrt_site site) {
    (void)site;
    return PS_CONTACT_SOLVER_DEFAULT;
}
static inline ps_contact_solver psrt_contact_solver_make(int64_t iterations, double restitution,
                                                         double friction, double threshold,
                                                         double slop, double correction,
                                                         psrt_site site) {
    if (iterations < 1 || iterations > 256)
        psrt_fail(site, "Contact solver iterations must be in 1..256");
    ps_contact_solver settings = {
        (uint32_t)iterations, restitution, friction, threshold, slop, correction};
    ps_body fixed = {0};
    fixed.orientation.w = 1;
    ps_contact_manifold empty = {0};
    /* Let the common solver validate its configuration, including empty cases. */
    if (ps_contacts_resolve(&fixed, NULL, &empty, &settings, NULL) != PS_OK)
        psrt_fail(site, "Invalid contact solver configuration");
    return settings;
}
static inline ps_material psrt_material_make(double density, double restitution,
                                              double friction, psrt_site site) {
    if (!isfinite(density) || density < 0 || !isfinite(restitution) ||
        restitution < 0 || restitution > 1 || !isfinite(friction) || friction < 0)
        psrt_fail(site, "Material density, restitution or friction is invalid");
    return (ps_material){density, restitution, friction, "custom material"};
}
static inline ps_contact_solver psrt_material_contact_solver(ps_material material,
                                                              int64_t iterations,
                                                              double threshold, double slop,
                                                              double correction,
                                                              psrt_site site) {
    return psrt_contact_solver_make(iterations, material.restitution, material.friction,
                                    threshold, slop, correction, site);
}
static inline ps_contact_manifold psrt_contacts_spheres(ps_body a, double radius_a, ps_body b,
                                                        double radius_b, psrt_site site) {
    ps_contact_manifold contacts = {0};
    bool touching;
    if (ps_contact_spheres(&a, radius_a, &b, radius_b, &contacts.points[0], &touching) != PS_OK)
        psrt_fail(site, "Sphere contact detection failed");
    contacts.count = touching ? 1 : 0;
    return contacts;
}
static inline ps_contact_manifold psrt_contacts_sphere_plane(ps_body body, double radius,
                                                             ps_vec3 point, ps_vec3 normal,
                                                             psrt_site site) {
    ps_contact_manifold contacts = {0};
    bool touching;
    if (ps_contact_sphere_plane(&body, radius, point, normal, &contacts.points[0], &touching) !=
        PS_OK)
        psrt_fail(site, "Sphere-plane contact detection failed");
    contacts.count = touching ? 1 : 0;
    return contacts;
}
static inline ps_contact_manifold
psrt_contacts_sphere_box(ps_body sphere, double radius, ps_body box, ps_vec3 size, psrt_site site) {
    ps_contact_manifold contacts = {0};
    bool touching;
    if (ps_contact_sphere_box(&sphere, radius, &box, size, &contacts.points[0], &touching) != PS_OK)
        psrt_fail(site, "Sphere-box contact detection failed");
    contacts.count = touching ? 1 : 0;
    return contacts;
}
static inline ps_contact_manifold psrt_contacts_box_plane(ps_body body, ps_vec3 size, ps_vec3 point,
                                                          ps_vec3 normal, psrt_site site) {
    ps_contact_manifold contacts;
    if (ps_contacts_box_plane(&body, size, point, normal, &contacts) != PS_OK)
        psrt_fail(site, "Box-plane contact detection failed");
    return contacts;
}
static inline ps_contact_manifold psrt_contacts_boxes(ps_body a, ps_vec3 size_a, ps_body b,
                                                      ps_vec3 size_b, psrt_site site) {
    ps_contact_manifold contacts;
    if (ps_contacts_boxes(&a, size_a, &b, size_b, &contacts) != PS_OK)
        psrt_fail(site, "Box contact detection failed");
    return contacts;
}
static inline ps_contact psrt_contact_at(ps_contact_manifold contacts, int64_t index,
                                         psrt_site site) {
    if (index < 0 || (uint64_t)index >= contacts.count)
        psrt_fail(site, "Contact index out of bounds");
    return contacts.points[index];
}
static inline ps_vec3 psrt_contact_point(ps_contact_manifold contacts, int64_t index,
                                         psrt_site site) {
    return psrt_contact_at(contacts, index, site).point_m;
}
static inline ps_vec3 psrt_contact_normal(ps_contact_manifold contacts, int64_t index,
                                          psrt_site site) {
    return psrt_contact_at(contacts, index, site).normal;
}
static inline double psrt_contact_penetration(ps_contact_manifold contacts, int64_t index,
                                              psrt_site site) {
    return psrt_contact_at(contacts, index, site).penetration_m;
}
static inline psrt_contact_result psrt_contacts_resolve(ps_contact_manifold contacts, ps_body a,
                                                        ps_body b, ps_contact_solver settings,
                                                        psrt_site site) {
    psrt_contact_result result = {a, b, {0}};
    if (ps_contacts_resolve(&result.body_a, &result.body_b, &contacts, &settings,
                            &result.solution) != PS_OK)
        psrt_fail(site, "Contact solver failed: invalid inputs or numeric range");
    return result;
}
/* Exact value-semantic binding of the C single-contact impulse response. */
static inline psrt_contact_result psrt_contact_resolve_single(ps_contact_manifold contacts,
                                                              ps_body a, ps_body b,
                                                              double restitution,
                                                              double friction, psrt_site site) {
    if (contacts.count != 1)
        psrt_fail(site, "Single-contact response requires exactly one contact");
    psrt_contact_result result = {a, b, {0}};
    ps_vec3 impulse = {0};
    if (ps_contact_resolve(&result.body_a, &result.body_b, &contacts.points[0], restitution,
                           friction, &impulse) != PS_OK)
        psrt_fail(site, "Single-contact response failed: invalid inputs or numeric range");
    result.solution.count = 1;
    result.solution.impulse_on_a_ns[0] = impulse;
    return result;
}
static inline ps_vec3 psrt_contact_impulse(psrt_contact_result result, int64_t index,
                                           psrt_site site) {
    if (index < 0 || (uint64_t)index >= result.solution.count)
        psrt_fail(site, "Contact impulse index out of bounds");
    return result.solution.impulse_on_a_ns[index];
}

static inline ps_body psrt_body_with_inertia(double mass, ps_vec3 inertia, psrt_site site) {
    ps_body result;
    ps_result status = ps_body_with_inertia(mass, inertia, &result);
    if (status != PS_OK)
        psrt_fail_code(site, status, "Invalid explicit body mass or principal inertia");
    return result;
}
static inline ps_body psrt_body_sphere(double mass, double radius, psrt_site site) {
    ps_body body;
    if (ps_body_sphere(mass, radius, &body) != PS_OK)
        psrt_fail(site, "Invalid sphere body mass, radius or numeric range");
    return body;
}
static inline ps_body psrt_body_box(double mass, ps_vec3 size, psrt_site site) {
    ps_body body;
    if (ps_body_box(mass, size, &body) != PS_OK)
        psrt_fail(site, "Invalid box body mass, size or numeric range");
    return body;
}
static inline void psrt_body_set_state(ps_body *body, ps_vec3 position, ps_vec3 velocity,
                                       ps_quat orientation, ps_vec3 angular_velocity,
                                       psrt_site site) {
    ps_body candidate = *body;
    candidate.position_m = position;
    candidate.velocity_m_s = velocity;
    candidate.orientation = orientation;
    candidate.angular_velocity_rad_s = angular_velocity;
    if (ps_body_validate(&candidate) != PS_OK)
        psrt_fail(site,
                  "Invalid body state: finite SI values, unit rotation and static rest required");
    *body = candidate;
}
static inline void psrt_body_apply_impulse(ps_body *body, ps_vec3 impulse, ps_vec3 point,
                                           psrt_site site) {
    if (ps_body_apply_impulse(body, impulse, point) != PS_OK)
        psrt_fail(site, "Body impulse failed: invalid input or numeric range");
}
static inline void psrt_body_step(ps_body *body, ps_vec3 force, ps_vec3 torque, double dt,
                                  psrt_site site) {
    if (ps_body_step(body, force, torque, dt) != PS_OK)
        psrt_fail(site, "Body step failed: positive dt and representable motion required");
}
static inline double psrt_body_kinetic_energy(ps_body body, psrt_site site) {
    double value;
    if (ps_body_kinetic_energy(&body, &value) != PS_OK)
        psrt_fail(site, "Body kinetic energy is not representable");
    return value;
}
static inline ps_vec3 psrt_body_point_velocity(ps_body body, ps_vec3 point, psrt_site site) {
    ps_vec3 value;
    if (ps_body_point_velocity(&body, point, &value) != PS_OK)
        psrt_fail(site, "Body point velocity is not representable");
    return value;
}
static inline ps_vec3 psrt_body_force_torque(ps_body body, ps_vec3 force, ps_vec3 point,
                                             psrt_site site) {
    ps_vec3 value;
    if (ps_body_force_torque(&body, force, point, &value) != PS_OK)
        psrt_fail(site, "Body force torque is not representable");
    return value;
}
#endif
