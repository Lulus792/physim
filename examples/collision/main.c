#include "physim/collision.h"
#include "physim/experiment.h"
#include "physim/mechanics.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* Einstellungen ändern und F5 drücken. 0=Vakuum, 1=Luft, 2=Wasser, 3=eigenes Medium. */
#ifndef PS_COLLISION_MEDIUM
#define PS_COLLISION_MEDIUM 0
#endif
#ifndef PS_COLLISION_DRAG
#define PS_COLLISION_DRAG PS_DRAG_QUADRATIC
#endif
#ifndef PS_COLLISION_FRICTION
#define PS_COLLISION_FRICTION 0.0
#endif
#ifndef PS_COLLISION_SPIN
#define PS_COLLISION_SPIN 0.0 /* rad/s, Kugel A; mit Reibung entsteht Tangentialimpuls */
#endif
#ifndef PS_COLLISION_CCD
#define PS_COLLISION_CCD (PS_COLLISION_MEDIUM == 0) /* lineare Bewegung im Vakuum */
#endif
#ifndef PS_COLLISION_SPEED
#define PS_COLLISION_SPEED 0.6 /* m/s je Kugel, aufeinander zu */
#endif
static const double radius_m = .2, mass_kg = 1, restitution = 1, drag_coefficient = .47;
static const ps_medium custom_medium = {5, .005, "custom medium"};
static const ps_vec3 fluid_velocity_m_s = {0, 0, 0};
typedef struct {
    ps_body a, b;
    ps_medium medium;
    ps_vec3 force_a, force_b, impulse_a;
    ps_contact last_contact;
    bool had_contact;
} collision;
static ps_result measure(ps_context *c) {
    collision *p = c->user;
    double a = 0, b = 0;
    ps_result r = ps_body_kinetic_energy(&p->a, &a);
    if (r == PS_OK)
        r = ps_body_kinetic_energy(&p->b, &b);
    if (r != PS_OK)
        return r;
    c->values[0] = p->a.position_m.x;
    c->values[1] = p->b.position_m.x;
    c->values[2] = p->a.velocity_m_s.x;
    c->values[3] = p->b.velocity_m_s.x;
    c->values[4] = a + b;
    c->values[5] = p->a.angular_velocity_rad_s.z;
    c->values[6] = p->b.angular_velocity_rad_s.z;
    c->values[7] = mass_kg * (p->a.velocity_m_s.x + p->b.velocity_m_s.x);
    c->values[8] = mass_kg * (p->a.velocity_m_s.y + p->b.velocity_m_s.y);
    c->values[9] = p->force_a.x;
    c->values[10] = p->impulse_a.x;
    return PS_OK;
}
static ps_result reset(ps_context *c) {
    collision *p = c->user;
    *p = (collision){0};
    p->medium = PS_COLLISION_MEDIUM == 0   ? PS_VACUUM
                : PS_COLLISION_MEDIUM == 1 ? PS_AIR
                : PS_COLLISION_MEDIUM == 2 ? PS_WATER
                                           : custom_medium;
    ps_result r = ps_body_sphere(mass_kg, radius_m, &p->a);
    if (r == PS_OK)
        r = ps_body_sphere(mass_kg, radius_m, &p->b);
    if (r != PS_OK)
        return r;
    p->a.position_m = ps_v3(-1, -.5, 0);
    p->b.position_m = ps_v3(1, -.5, 0);
    p->a.velocity_m_s = ps_v3(PS_COLLISION_SPEED, 0, 0);
    p->b.velocity_m_s = ps_v3(-PS_COLLISION_SPEED, 0, 0);
    p->a.angular_velocity_rad_s.z = PS_COLLISION_SPIN;
    return measure(c);
}
static ps_result create(ps_context *c) {
    if (PS_COLLISION_MEDIUM < 0 || PS_COLLISION_MEDIUM > 3 || PS_COLLISION_DRAG < PS_DRAG_NONE ||
        PS_COLLISION_DRAG > PS_DRAG_QUADRATIC || !isfinite(PS_COLLISION_FRICTION) ||
        PS_COLLISION_FRICTION < 0 || !isfinite(PS_COLLISION_SPIN) ||
        !isfinite(PS_COLLISION_SPEED) || PS_COLLISION_SPEED <= 0 ||
        (PS_COLLISION_CCD != 0 && PS_COLLISION_CCD != 1) ||
        (PS_COLLISION_CCD && PS_COLLISION_MEDIUM != 0))
        return PS_INVALID;
    c->user = calloc(1, sizeof(collision));
    if (!c->user)
        return PS_MEMORY;
    ps_channel_add(c, "a.position", PS_METRE, "Sphere A X");
    ps_channel_add(c, "b.position", PS_METRE, "Sphere B X");
    ps_channel_add(c, "a.velocity", PS_VELOCITY, "Sphere A X velocity");
    ps_channel_add(c, "b.velocity", PS_VELOCITY, "Sphere B X velocity");
    ps_channel_add(c, "energy", PS_JOULE, "Total translational and rotational kinetic energy");
    const ps_unit angular_velocity = {{0, 0, -1, 0, 0, 0, 0}, 1, "rad/s"};
    const ps_unit momentum = {{1, 1, -1, 0, 0, 0, 0}, 1, "kg m/s"};
    const ps_unit force = {{1, 1, -2, 0, 0, 0, 0}, 1, "N"};
    ps_channel_add(c, "a.spin_z", angular_velocity, "Sphere A world angular velocity Z");
    ps_channel_add(c, "b.spin_z", angular_velocity, "Sphere B world angular velocity Z");
    ps_channel_add(c, "momentum.x", momentum, "Total linear momentum X");
    ps_channel_add(c, "momentum.y", momentum, "Total linear momentum Y");
    ps_channel_add(c, "a.drag_force_x", force,
                   "Force applied during this step, zero at initial sample");
    ps_channel_add(c, "a.contact_impulse_x", momentum, "Contact impulse this step, zero otherwise");
    ps_result r = reset(c);
    if (r != PS_OK)
        return r;
    collision *p = c->user;
    snprintf(
        c->model_metadata, sizeof c->model_metadata,
        "model=two homogeneous rigid spheres\nintegrator=symplectic Euler + quaternion rotation\n"
        "contact=%s single normal impulse + Coulomb tangent impulse + position projection\n"
        "gravity=none\nbuoyancy=none\nrotational_drag=none\nccd=%s\ninitial_speed_m_s=%.17g\n"
        "medium=%s\ndensity_kg_m3=%.17g\nviscosity_pa_s=%.17g\ndrag=%s\n"
        "fluid_velocity_m_s=%.17g,%.17g,%.17g\nmass_kg=%.17g\nradius_m=%.17g\n"
        "restitution=%.17g\nfriction=%.17g\na_initial_spin_rad_s=%.17g\nCd=%.17g\n",
        PS_COLLISION_CCD ? "continuous" : "discrete",
        PS_COLLISION_CCD ? "linear sphere sweep + remaining time" : "none",
        (double)PS_COLLISION_SPEED, p->medium.name, p->medium.density_kg_m3,
        p->medium.viscosity_pa_s,
        PS_COLLISION_DRAG == PS_DRAG_NONE     ? "none"
        : PS_COLLISION_DRAG == PS_DRAG_STOKES ? "Stokes"
                                              : "quadratic",
        fluid_velocity_m_s.x, fluid_velocity_m_s.y, fluid_velocity_m_s.z, mass_kg, radius_m,
        restitution, (double)PS_COLLISION_FRICTION, (double)PS_COLLISION_SPIN, drag_coefficient);
    return PS_OK;
}
static ps_result step(ps_context *c, double dt) {
    if (!isfinite(dt) || dt <= 0)
        return PS_INVALID;
    collision *p = c->user, next = *p;
    bool continuous = PS_COLLISION_CCD != 0;
    if (continuous) {
        ps_sweep_hit hit;
        bool touching;
        ps_result result =
            ps_sweep_spheres(&next.a, radius_m, ps_vscale(next.a.velocity_m_s, dt), &next.b,
                             radius_m, ps_vscale(next.b.velocity_m_s, dt), &hit, &touching);
        if (result != PS_OK)
            return result;
        double first = touching ? dt * hit.fraction : dt;
        ps_vec3 zero = {0};
        next.impulse_a = zero;
        if (first > 0)
            result = ps_body_step(&next.a, zero, zero, first);
        if (result == PS_OK && first > 0)
            result = ps_body_step(&next.b, zero, zero, first);
        if (result == PS_OK && touching) {
            result = ps_contact_resolve(&next.a, &next.b, &hit.contact, restitution,
                                        PS_COLLISION_FRICTION, &next.impulse_a);
            next.last_contact = hit.contact;
            next.had_contact = true;
            double remaining = dt - first;
            if (result == PS_OK && remaining > 0)
                result = ps_body_step(&next.a, zero, zero, remaining);
            if (result == PS_OK && remaining > 0)
                result = ps_body_step(&next.b, zero, zero, remaining);
        }
        if (result != PS_OK)
            return result;
        *p = next;
        return measure(c);
    }
    ps_result r = ps_sphere_drag(ps_vsub(next.a.velocity_m_s, fluid_velocity_m_s), next.medium,
                                 PS_COLLISION_DRAG, radius_m, drag_coefficient, &next.force_a);
    if (r == PS_OK)
        r = ps_sphere_drag(ps_vsub(next.b.velocity_m_s, fluid_velocity_m_s), next.medium,
                           PS_COLLISION_DRAG, radius_m, drag_coefficient, &next.force_b);
    if (r == PS_OK)
        r = ps_body_step(&next.a, next.force_a, ps_v3(0, 0, 0), dt);
    if (r == PS_OK)
        r = ps_body_step(&next.b, next.force_b, ps_v3(0, 0, 0), dt);
    ps_contact contact;
    bool touching = false;
    if (r == PS_OK)
        r = ps_contact_spheres(&next.a, radius_m, &next.b, radius_m, &contact, &touching);
    next.impulse_a = ps_v3(0, 0, 0);
    if (r == PS_OK && touching) {
        r = ps_contact_resolve(&next.a, &next.b, &contact, restitution, PS_COLLISION_FRICTION,
                               &next.impulse_a);
        next.last_contact = contact;
        next.had_contact = true;
    }
    if (r != PS_OK)
        return r;
    *p = next;
    return measure(c);
}
static void body_scene(ps_scene *s, const ps_body *b, ps_vec3 force, uint32_t color,
                       const char *label, uint32_t base) {
    ps_scene_add_id(s, base, PS_SPHERE, b->position_m, b->position_m, radius_m, color);
    ps_vec3 tip =
        ps_vadd(b->position_m, ps_quat_rotate(b->orientation, ps_v3(0, radius_m * 1.45, 0)));
    ps_scene_add_id(s, base + 1, PS_LINE, b->position_m, tip, .009, 0xffffffff);
    ps_scene_add_id(s, base + 2, PS_ARROW, b->position_m, ps_vadd(b->position_m, b->velocity_m_s),
                    .006, color);
    if (ps_vlength(force) > 1e-9)
        ps_scene_add_id(s, base + 3, PS_ARROW, b->position_m, ps_vadd(b->position_m, force), .008,
                        0xef879eff);
    (void)ps_scene_label_id(s, base + 4, b->position_m, label, color);
}
static void scene(ps_context *c, ps_scene *s) {
    collision *p = c->user;
    body_scene(s, &p->a, p->force_a, 0x53dec2ff, "Kugel A", 10);
    body_scene(s, &p->b, p->force_b, 0xf2c572ff, "Kugel B", 20);
    if (p->had_contact) {
        ps_scene_add_id(s, 0, PS_POINT, p->last_contact.point_m, p->last_contact.point_m, .035,
                        0xff8eafff);
        (void)ps_scene_label_id(s, 100, p->last_contact.point_m, "Letzter Kontakt", 0xff8eafff);
    }
    (void)ps_scene_label_id(s, 101, ps_v3(0, .6, 0), "Pfeile: v [1 m pro m/s], F [1 m pro N]",
                            0xb9c0cdff);
}
static void destroy(ps_context *c) { free(c->user); }
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api = {
        sizeof api, PS_ABI_VERSION, 0,      "Kugelstoß mit Rotation und Medien", create, reset,
        step,       scene,          destroy, NULL};
    return &api;
}
