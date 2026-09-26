#include "physim/experiment.h"
#include "physim/mechanics.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* Winkel in rad, Versatz in m. Ändern und mit F5 neu bauen. */
#ifndef PS_BOX_ANGLE
#define PS_BOX_ANGLE 0.0
#endif
#ifndef PS_BOX_OFFSET
#define PS_BOX_OFFSET 0.0
#endif
#ifndef PS_BOX_RESTITUTION
#define PS_BOX_RESTITUTION 1.0
#endif
#ifndef PS_BOX_FRICTION
#define PS_BOX_FRICTION 0.0
#endif
static const ps_vec3 size_m = {.8, .6, .6};
static const double mass_kg = 1;
typedef struct {
    ps_body a, b;
    ps_contact_solver solver;
    ps_contact_manifold contacts, last_contacts;
    ps_contact_solution solution, last_solution;
} experiment;
static ps_result measure(ps_context *c) {
    experiment *e = c->user;
    double ka = 0, kb = 0;
    ps_result r = ps_body_kinetic_energy(&e->a, &ka);
    if (r == PS_OK)
        r = ps_body_kinetic_energy(&e->b, &kb);
    if (r != PS_OK)
        return r;
    ps_vec3 momentum = ps_vscale(ps_vadd(e->a.velocity_m_s, e->b.velocity_m_s), mass_kg);
    double impulse = 0;
    for (unsigned i = 0; i < e->solution.count; i++)
        impulse += e->solution.impulse_on_a_ns[i].x;
    c->values[0] = e->a.position_m.x;
    c->values[1] = e->a.position_m.y;
    c->values[2] = e->a.position_m.z;
    c->values[3] = e->a.velocity_m_s.x;
    c->values[4] = ka + kb;
    c->values[5] = e->b.position_m.x;
    c->values[6] = e->b.velocity_m_s.x;
    c->values[7] = ps_vlength(e->a.angular_velocity_rad_s);
    c->values[8] = ps_vlength(e->b.angular_velocity_rad_s);
    c->values[9] = momentum.x;
    c->values[10] = momentum.y;
    c->values[11] = momentum.z;
    c->values[12] = e->contacts.count;
    c->values[13] = impulse;
    c->values[14] = e->solution.max_normal_error_m_s;
    return PS_OK;
}
static ps_result reset(ps_context *c) {
    experiment *e = c->user;
    *e = (experiment){0};
    ps_result r = ps_body_box(mass_kg, size_m, &e->a);
    if (r == PS_OK)
        r = ps_body_box(mass_kg, size_m, &e->b);
    if (r != PS_OK)
        return r;
    e->a.position_m = ps_v3(-1, 0, 0);
    e->b.position_m = ps_v3(1, PS_BOX_OFFSET, 0);
    e->a.velocity_m_s = ps_v3(.6, 0, 0);
    e->b.velocity_m_s = ps_v3(-.6, 0, 0);
    e->b.orientation = ps_quat_axis_angle(ps_v3(0, 0, 1), PS_BOX_ANGLE);
    e->solver = PS_CONTACT_SOLVER_DEFAULT;
    e->solver.iterations = 128;
    e->solver.restitution = PS_BOX_RESTITUTION;
    e->solver.friction = PS_BOX_FRICTION;
    e->solver.penetration_slop_m = 0;
    e->solver.correction_fraction = 1;
    return measure(c);
}
static ps_result create(ps_context *c) {
    if (!isfinite(PS_BOX_ANGLE) || !isfinite(PS_BOX_OFFSET) || !isfinite(PS_BOX_RESTITUTION) ||
        PS_BOX_RESTITUTION < 0 || PS_BOX_RESTITUTION > 1 || !isfinite(PS_BOX_FRICTION) ||
        PS_BOX_FRICTION < 0)
        return PS_INVALID;
    c->user = calloc(1, sizeof(experiment));
    if (!c->user)
        return PS_MEMORY;
    const ps_unit angular = {{0, 0, -1, 0, 0, 0, 0}, 1, "rad/s"};
    const ps_unit momentum = {{1, 1, -1, 0, 0, 0, 0}, 1, "kg m/s"};
    const ps_unit one = {{0}, 1, "1"};
    ps_channel_add(c, "position.x", PS_METRE, "Box A X");
    ps_channel_add(c, "position.y", PS_METRE, "Box A Y");
    ps_channel_add(c, "position.z", PS_METRE, "Box A Z");
    ps_channel_add(c, "velocity.x", PS_VELOCITY, "Box A velocity X");
    ps_channel_add(c, "energy", PS_JOULE, "Total translational and rotational kinetic energy");
    ps_channel_add(c, "b.position.x", PS_METRE, "Box B X");
    ps_channel_add(c, "b.velocity.x", PS_VELOCITY, "Box B velocity X");
    ps_channel_add(c, "a.spin", angular, "Box A angular speed");
    ps_channel_add(c, "b.spin", angular, "Box B angular speed");
    ps_channel_add(c, "momentum.x", momentum, "Total linear momentum X");
    ps_channel_add(c, "momentum.y", momentum, "Total linear momentum Y");
    ps_channel_add(c, "momentum.z", momentum, "Total linear momentum Z");
    ps_channel_add(c, "contacts", one, "Contact points detected this step");
    ps_channel_add(c, "a.impulse.x", momentum, "Impulse on A this step, not force");
    ps_channel_add(c, "contact.error", PS_VELOCITY, "Normal solver residual before projection");
    ps_result r = reset(c);
    if (r != PS_OK)
        return r;
    experiment *e = c->user;
    snprintf(c->model_metadata, sizeof c->model_metadata,
             "model=two homogeneous oriented rigid boxes\nmedium=vacuum\ngravity=none\n"
             "contact=15-axis SAT + clipped face / closest edge manifold\n"
             "solver=projected pair impulses, Coulomb friction, translation projection\n"
             "integrator=symplectic Euler + quaternion rotation\nccd=none\nbody_graph=none\n"
             "mass_kg=%.17g\nsize_m=%.17g,%.17g,%.17g\na_initial_position_m=-1,0,0\n"
             "b_initial_position_m=1,%.17g,0\na_initial_velocity_m_s=0.6,0,0\n"
             "b_initial_velocity_m_s=-0.6,0,0\nb_angle_z_rad=%.17g\n"
             "initial_spin_rad_s=0\nrestitution=%.17g\nfriction=%.17g\niterations=%u\n"
             "bounce_threshold_m_s=%.17g\npenetration_slop_m=0\ncorrection_fraction=1\n",
             mass_kg, size_m.x, size_m.y, size_m.z, (double)PS_BOX_OFFSET, (double)PS_BOX_ANGLE,
             e->solver.restitution, e->solver.friction, e->solver.iterations,
             e->solver.bounce_threshold_m_s);
    return PS_OK;
}
static ps_result step(ps_context *c, double dt) {
    experiment *e = c->user, next = *e;
    const ps_vec3 zero = {0};
    ps_result r = ps_body_step(&next.a, zero, zero, dt);
    if (r == PS_OK)
        r = ps_body_step(&next.b, zero, zero, dt);
    if (r == PS_OK)
        r = ps_contacts_boxes(&next.a, size_m, &next.b, size_m, &next.contacts);
    if (r == PS_OK)
        r = ps_contacts_resolve(&next.a, &next.b, &next.contacts, &next.solver, &next.solution);
    if (r != PS_OK)
        return r;
    if (next.contacts.count) {
        next.last_contacts = next.contacts;
        next.last_solution = next.solution;
    }
    *e = next;
    return measure(c);
}
static void body_scene(ps_scene *s, const ps_body *b, uint32_t color, const char *label,
                       uint32_t base) {
    ps_object object = {0};
    object.shape = PS_BOX;
    object.id = base;
    object.a = b->position_m;
    object.b = size_m;
    object.orientation = b->orientation;
    object.color = color;
    (void)ps_scene_push(s, &object);
    ps_scene_add_id(s, base + 1, PS_ARROW, b->position_m, ps_vadd(b->position_m, b->velocity_m_s),
                    .009, color);
    (void)ps_scene_label_id(s, base + 2, ps_vadd(b->position_m, ps_v3(0, .48, 0)), label, color);
}
static void scene(ps_context *c, ps_scene *s) {
    experiment *e = c->user;
    body_scene(s, &e->a, 0x53dec2ff, "Box A", 10);
    body_scene(s, &e->b, 0xf2c572ff, "Box B", 20);
    for (unsigned i = 0; i < e->last_contacts.count; i++) {
        ps_vec3 point = e->last_contacts.points[i].point_m;
        ps_scene_add_id(s, 0, PS_POINT, point, point, .025, 0xff8eafff);
        ps_scene_add_id(s, 0, PS_ARROW, point, ps_vadd(point, e->last_solution.impulse_on_a_ns[i]),
                        .006, 0xff8eafff);
    }
    (void)ps_scene_label_id(s, 100, ps_v3(-1, -.95, 0), "Geschwindigkeit: 1 m pro m/s", 0xb9c0cdff);
    (void)ps_scene_label_id(s, 101, ps_v3(-1, -1.15, 0), "Rosa: letzter Stoß, J [1 m pro Ns]",
                            0xff8eafff);
}
static void destroy(ps_context *c) { free(c->user); }
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api = {
        sizeof api, PS_ABI_VERSION, 0,    "Boxstoß mit Flächen- und Kantenkontakten",
        create,     reset,          step, scene,
        destroy};
    return &api;
}
