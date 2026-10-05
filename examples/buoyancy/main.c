#include "physim/experiment.h"
#include "physim/mechanics.h"
#include "physim/units.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Uniform sphere on a prescribed vertical guide, infinite horizontal water
 * surface y=0. SI parameters; edit and rebuild with F5. Damping is empirical,
 * proportional to immersed volume fraction, not a fluid-dynamics drag law. */
#ifndef PS_FLOAT_DENSITY
#define PS_FLOAT_DENSITY 500
#endif
#ifndef PS_FLOAT_HEIGHT
#define PS_FLOAT_HEIGHT .04
#endif
#ifndef PS_FLOAT_DAMPING
#define PS_FLOAT_DAMPING 1.2
#endif
static const double radius = .1, fluid_density = 1000, gravity = 9.81;
static const double body_density = PS_FLOAT_DENSITY, initial_height = PS_FLOAT_HEIGHT;
static const double damping = PS_FLOAT_DAMPING;
typedef struct {
    double state[3];
} experiment; /* height, velocity, dissipated work */
static double full_volume(void) {
    ps_submersion full;
    return ps_sphere_submersion(radius, -radius, &full) == PS_OK ? full.volume_m3 : NAN;
}
static double mass(void) { return body_density * full_volume(); }
static ps_result forces(double height, double velocity, ps_submersion *s, double *up,
                        double *drag) {
    ps_result r = ps_sphere_submersion(radius, height, s);
    if (r != PS_OK)
        return r;
    ps_vec3 force;
    r = ps_buoyancy_force(fluid_density, s->volume_m3, ps_v3(0, -gravity, 0), &force);
    if (r != PS_OK)
        return r;
    *up = force.y;
    *drag = -damping * (s->volume_m3 / full_volume()) * velocity;
    return isfinite(*drag) ? PS_OK : PS_NUMERIC;
}
/* Effective potential for body + ideal hydrostatic reservoir, U(0)=0.
 * Its derivative is weight minus buoyancy, including dry/full continuations. */
static double potential(double height) {
    double h = fmax(-radius, fmin(radius, height));
    double h2 = h * h, r2 = radius * radius;
    double integral = PS_PI * (2 * radius * r2 * h / 3 - r2 * h2 / 2 + h2 * h2 / 12);
    if (height < -radius)
        integral += full_volume() * (height + radius);
    return mass() * gravity * height - fluid_density * gravity * integral;
}
static void derivative(double t, const double *y, double *dy, void *user) {
    (void)t;
    ps_result *error = user;
    ps_submersion s;
    double up, drag;
    ps_result r = forces(y[0], y[1], &s, &up, &drag);
    if (r != PS_OK) {
        *error = r;
        dy[0] = dy[1] = dy[2] = NAN;
        return;
    }
    dy[0] = y[1];
    dy[1] = (up + drag) / mass() - gravity;
    dy[2] = -drag * y[1];
}
static ps_result measure(ps_context *c, const double *y) {
    ps_submersion s;
    double up, drag;
    ps_result r = forces(y[0], y[1], &s, &up, &drag);
    if (r != PS_OK)
        return r;
    double energy = .5 * mass() * y[1] * y[1] + potential(y[0]);
    double values[] = {y[0],
                       y[1],
                       s.volume_m3,
                       s.volume_m3 / full_volume(),
                       up,
                       -mass() * gravity,
                       drag,
                       up - mass() * gravity + drag,
                       energy,
                       y[2],
                       energy + y[2],
                       s.centroid_offset_m};
    for (unsigned i = 0; i < sizeof values / sizeof values[0]; i++)
        if (!isfinite(values[i]))
            return PS_NUMERIC;
    memcpy(c->values, values, sizeof values);
    return PS_OK;
}
static ps_result reset(ps_context *c) {
    experiment *e = c->user;
    double y[] = {initial_height, 0, 0};
    ps_result r = measure(c, y);
    if (r == PS_OK)
        memcpy(e->state, y, sizeof y);
    return r;
}
static ps_result create(ps_context *c) {
    if (!isfinite(body_density) || body_density <= 0 || !isfinite(initial_height) ||
        !isfinite(damping) || damping < 0 || !isfinite(mass()) || mass() <= 0)
        return PS_INVALID;
    c->user = calloc(1, sizeof(experiment));
    if (!c->user)
        return PS_MEMORY;
    const ps_unit cubic_metre = {{3, 0, 0, 0, 0, 0, 0}, 1, "m^3"};
    const ps_unit newton = {{1, 1, -2, 0, 0, 0, 0}, 1, "N"};
    ps_channel_add(c, "position.y", PS_METRE, "Sphere center above water surface y=0");
    ps_channel_add(c, "velocity.y", PS_VELOCITY, "Vertical velocity");
    ps_channel_add(c, "submerged.volume", cubic_metre, "Displaced water volume");
    ps_channel_add(c, "submerged.fraction", PS_ONE, "Immersed volume / full sphere volume");
    ps_channel_add(c, "force.buoyancy.y", newton, "Archimedes force");
    ps_channel_add(c, "force.weight.y", newton, "Weight, negative Y");
    ps_channel_add(c, "force.damping.y", newton, "Empirical -c*fraction*v");
    ps_channel_add(c, "force.total.y", newton, "Sum of buoyancy, weight and damping");
    ps_channel_add(c, "energy", PS_JOULE, "Kinetic plus body/reservoir potential, U(0)=0");
    ps_channel_add(c, "energy.dissipated", PS_JOULE, "Integrated damping work");
    ps_channel_add(c, "energy.balance", PS_JOULE, "Mechanical plus dissipated energy");
    ps_channel_add(c, "buoyancy.offset.y", PS_METRE, "Displaced-fluid centroid relative to center");
    snprintf(c->model_metadata, sizeof c->model_metadata,
             "model=hydrostatic floating sphere\nintegrator=RK4\nradius_m=%.17g\n"
             "body_density_kg_m3=%.17g\nfluid_density_kg_m3=%.17g\nmass_kg=%.17g\n"
             "gravity_m_s2=%.17g\ninitial_height_m=%.17g\ndamping_ns_m=%.17g\n"
             "surface_y_m=0\ndamping_model=-c*immersed_volume_fraction*v\n"
             "energy=body plus ideal hydrostatic reservoir, U(0)=0\n"
             "constraints=prescribed vertical guide\nfluid=uniform infinite reservoir\n"
             "excluded=waves, surface tension, added mass, rotational drag, floor contact\n",
             radius, body_density, fluid_density, mass(), gravity, initial_height, damping);
    return reset(c);
}
static ps_result step(ps_context *c, double dt) {
    experiment *e = c->user;
    double y[3];
    memcpy(y, e->state, sizeof y);
    ps_result error = PS_OK;
    ps_result r = ps_ode_step(PS_RK4, derivative, &error, c->time_s, dt, y, 3);
    if (error != PS_OK)
        return error;
    if (r == PS_OK)
        r = measure(c, y);
    if (r == PS_OK)
        memcpy(e->state, y, sizeof y);
    return r;
}
static void scene(ps_context *c, ps_scene *s) {
    double y = c->values[0];
    /* Open surface grid leaves the underwater sphere visible without transparency. */
    for (int i = -4; i <= 4; i++) {
        double p = i * .1;
        ps_scene_add_id(s, (uint32_t)(1 + (i + 4) * 2), PS_LINE, ps_v3(-.4, 0, p), ps_v3(.4, 0, p),
                        .001, 0x4d9dc5ff);
        ps_scene_add_id(s, (uint32_t)(2 + (i + 4) * 2), PS_LINE, ps_v3(p, 0, -.4), ps_v3(p, 0, .4),
                        .001, 0x4d9dc5ff);
    }
    ps_scene_add_id(s, 20, PS_SPHERE, ps_v3(0, y, 0), ps_v3(0, 0, 0), radius, 0xf2c572ff);
    ps_scene_add_id(s, 21, PS_POINT, ps_v3(0, y + c->values[11], radius + .005), ps_v3(0, 0, 0),
                    .006, 0x6dcf94ff);
    const double x[] = {-.16, .16, .23};
    const unsigned channel[] = {4, 5, 6};
    const uint32_t color[] = {0x6dcf94ff, 0xff8eafff, 0xbab1f4ff};
    for (unsigned i = 0; i < 3; i++)
        ps_scene_add_id(s, 30 + i, PS_ARROW, ps_v3(x[i], y, 0),
                        ps_v3(x[i], y + .006 * c->values[channel[i]], 0), .003, color[i]);
    (void)ps_scene_label_id(s, 40, ps_v3(-.4, .35, 0), "Auftrieb · schwimmende Kugel", 0xe6edf3ff);
    (void)ps_scene_label_id(s, 41, ps_v3(-.4, -.28, 0),
                            "Grün: Auftrieb · Rosa: Gewicht · Violett: Dämpfer", 0xc0c6cfff);
    (void)ps_scene_label_id(s, 42, ps_v3(-.4, -.34, 0),
                            "Kräfte: 0.006 m pro N · Wasseroberfläche: y=0", 0xc0c6cfff);
}
static void destroy(ps_context *c) {
    free(c->user);
    c->user = NULL;
}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api = {sizeof api, PS_ABI_VERSION, 0,     "Auftrieb", create,
                                          reset,      step,           scene, destroy, NULL};
    return &api;
}
