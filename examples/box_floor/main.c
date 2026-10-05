#include "physim/experiment.h"
#include "physim/mechanics.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* Volle Kantenlängen, kg, m/s². Einstellungen ändern und mit F5 neu bauen. */
static const ps_vec3 box_size_m = {.8, .5, .6};
static const double mass_kg = 1, gravity_m_s2 = 9.81;
typedef struct {
    ps_body box;
    ps_contact_solver solver;
    ps_contact_manifold contacts;
    ps_contact_solution solution;
} experiment;
static ps_result measure(ps_context *c) {
    experiment *e = c->user;
    double kinetic = 0;
    ps_result r = ps_body_kinetic_energy(&e->box, &kinetic);
    if (r != PS_OK)
        return r;
    ps_quat q = e->box.orientation;
    double support_y = fabs(ps_quat_rotate(q, ps_v3(box_size_m.x * .5, 0, 0)).y) +
                       fabs(ps_quat_rotate(q, ps_v3(0, box_size_m.y * .5, 0)).y) +
                       fabs(ps_quat_rotate(q, ps_v3(0, 0, box_size_m.z * .5)).y);
    double impulse_y = 0;
    for (uint32_t i = 0; i < e->solution.count; i++)
        impulse_y += e->solution.impulse_on_a_ns[i].y;
    c->values[0] = e->box.position_m.x;
    c->values[1] = e->box.position_m.y;
    c->values[2] = e->box.position_m.z;
    c->values[3] = e->box.velocity_m_s.y;
    c->values[4] = kinetic + mass_kg * gravity_m_s2 * e->box.position_m.y;
    c->values[5] = kinetic;
    c->values[6] = ps_vlength(e->box.angular_velocity_rad_s);
    c->values[7] = (double)e->contacts.count;
    c->values[8] = impulse_y;
    c->values[9] = e->box.position_m.y - support_y;
    c->values[10] = e->solution.max_normal_error_m_s;
    return PS_OK;
}
static ps_result reset(ps_context *c) {
    experiment *e = c->user;
    *e = (experiment){0};
    ps_result r = ps_body_box(mass_kg, box_size_m, &e->box);
    if (r != PS_OK)
        return r;
    e->box.position_m = ps_v3(0, 1.5, 0);
    e->box.velocity_m_s = ps_v3(.7, 0, .2);
    e->box.angular_velocity_rad_s = ps_v3(0, .3, .15);
    e->box.orientation = ps_quat_axis_angle(ps_v3(1, 0, 1), .22);
    e->solver = PS_CONTACT_SOLVER_DEFAULT;
    e->solver.iterations = 64;
    e->solver.restitution = .1;
    e->solver.friction = .6;
    return measure(c);
}
static ps_result create(ps_context *c) {
    c->user = calloc(1, sizeof(experiment));
    if (!c->user)
        return PS_MEMORY;
    const ps_unit angular = {{0, 0, -1, 0, 0, 0, 0}, 1, "rad/s"};
    const ps_unit count = {{0}, 1, "1"};
    const ps_unit impulse = {{1, 1, -1, 0, 0, 0, 0}, 1, "N s"};
    ps_channel_add(c, "position.x", PS_METRE, "Center X");
    ps_channel_add(c, "position.y", PS_METRE, "Center Y");
    ps_channel_add(c, "position.z", PS_METRE, "Center Z");
    ps_channel_add(c, "velocity.y", PS_VELOCITY, "Vertical center velocity");
    ps_channel_add(c, "energy", PS_JOULE, "Kinetic plus gravitational potential, plane Y=0");
    ps_channel_add(c, "kinetic.energy", PS_JOULE, "Translation plus rotation");
    ps_channel_add(c, "angular.speed", angular, "World angular speed magnitude");
    ps_channel_add(c, "contacts", count, "Contact vertices before position projection");
    ps_channel_add(c, "contact.impulse.y", impulse, "Total upward impulse this step");
    ps_channel_add(c, "clearance", PS_METRE, "Lowest vertex height after projection");
    ps_channel_add(c, "contact.error", PS_VELOCITY, "Normal solver residual before projection");
    ps_result r = reset(c);
    if (r != PS_OK)
        return r;
    experiment *e = c->user;
    snprintf(c->model_metadata, sizeof c->model_metadata,
             "model=oriented box-plane contacts\nmedium=vacuum\nccd=none\n"
             "solver=accumulated normal impulses and projected Coulomb cone, single body pair\n"
             "integrator=symplectic Euler + explicit gyroscopic rotation\n"
             "mass_kg=%.17g\nsize_m=%.17g,%.17g,%.17g\ngravity_m_s2=%.17g\n"
             "initial_position_m=0,1.5,0\ninitial_velocity_m_s=0.7,0,0.2\n"
             "initial_angular_velocity_rad_s=0,0.3,0.15\ninitial_axis_angle=1,0,1;0.22\n"
             "iterations=%u\nrestitution=%.17g\nfriction=%.17g\nbounce_threshold_m_s=%.17g\n"
             "penetration_slop_m=%.17g\ncorrection_fraction=%.17g\n",
             mass_kg, box_size_m.x, box_size_m.y, box_size_m.z, gravity_m_s2, e->solver.iterations,
             e->solver.restitution, e->solver.friction, e->solver.bounce_threshold_m_s,
             e->solver.penetration_slop_m, e->solver.correction_fraction);
    return PS_OK;
}
static ps_result step(ps_context *c, double dt) {
    experiment *e = c->user, next = *e;
    ps_result r = ps_body_step(&next.box, ps_v3(0, -mass_kg * gravity_m_s2, 0), ps_v3(0, 0, 0), dt);
    if (r == PS_OK)
        r = ps_contacts_box_plane(&next.box, box_size_m, ps_v3(0, 0, 0), ps_v3(0, 1, 0),
                                  &next.contacts);
    if (r == PS_OK)
        r = ps_contacts_resolve(&next.box, NULL, &next.contacts, &next.solver, &next.solution);
    if (r != PS_OK)
        return r;
    *e = next;
    return measure(c);
}
static void scene(ps_context *c, ps_scene *s) {
    experiment *e = c->user;
    ps_object box = {0};
    box.shape = PS_BOX;
    box.id = 1;
    box.a = e->box.position_m;
    box.b = box_size_m;
    box.orientation = e->box.orientation;
    box.color = 0x70b1eeff;
    (void)ps_scene_push(s, &box);
    ps_scene_add_id(s, 2, PS_PLANE, ps_v3(0, 0, 0), ps_v3(8, 0, 8), 0, 0x364452ff);
    ps_scene_add_id(s, 3, PS_ARROW, box.a, ps_vadd(box.a, e->box.velocity_m_s), .008, 0x6dcf94ff);
    ps_scene_add_id(s, 4, PS_ARROW, box.a,
                    ps_vadd(box.a, ps_v3(0, -mass_kg * gravity_m_s2 * .05, 0)), .008, 0xf2a657ff);
    for (uint32_t i = 0; i < e->contacts.count; i++) {
        ps_vec3 point = e->contacts.points[i].point_m;
        point.y = .015; /* plane projection, lifted slightly for visibility */
        ps_scene_add_id(s, 0, PS_POINT, point, point, .025, 0xff8eafff);
    }
    (void)ps_scene_label_id(s, 5, box.a, "Box · Kontaktpunkte rosa", 0x70b1eeff);
    (void)ps_scene_label_id(s, 6, ps_v3(-1, .8, 0), "v: 1 m pro m/s · Gewicht: 0.05 m pro N",
                            0xc0c6cfff);
}
static void destroy(ps_context *c) { free(c->user); }
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api = {
        sizeof api, PS_ABI_VERSION, 0, "Box auf Ebene", create, reset, step, scene, destroy, NULL};
    return &api;
}
