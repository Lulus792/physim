#include "physim/experiment.h"
#include "physim/mechanics.h"
#include "physim/units.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    ps_material material;
    ps_medium medium;
    double radius, volume, mass, lambda;
    double state[3]; /* y, vertical velocity, dissipated work; SI */
} settling;
static const double gravity = 9.80665;
static ps_result forces(const settling *s, double velocity, double *weight,
                        double *buoyancy, double *drag) {
    ps_vec3 b, d;
    ps_result result = ps_buoyancy_force(s->medium.density_kg_m3, s->volume,
                                       ps_v3(0, -gravity, 0), &b);
    if (result == PS_OK)
        result = ps_sphere_drag(ps_v3(0, velocity, 0), s->medium,
                                PS_DRAG_STOKES, s->radius, 0, &d);
    if (result != PS_OK) return result;
    *weight = -s->mass * gravity; *buoyancy = b.y; *drag = d.y;
    return PS_OK;
}
static void slope(double time, const double *state, double *out, void *user) {
    (void)time;
    settling *s = user; double w, b, d;
    if (forces(s, state[1], &w, &b, &d) != PS_OK) {
        out[0] = out[1] = out[2] = NAN; return;
    }
    out[0] = state[1]; out[1] = (w + b + d) / s->mass;
    out[2] = -d * state[1];
}
static ps_result measure(ps_context *c, const double *state, double time) {
    settling *s = c->user; double w, b, d;
    ps_result result = forces(s, state[1], &w, &b, &d);
    if (result != PS_OK) return result;
    double re = s->medium.viscosity_pa_s > 0
        ? 2 * s->medium.density_kg_m3 * s->radius * fabs(state[1]) / s->medium.viscosity_pa_s : 0;
    if (!isfinite(re) || re > .1) {
        snprintf(c->error, sizeof c->error, "Stokes tutorial requires Reynolds <= 0.1"); return PS_INVALID;
    }
    double acceleration = (w + b) / s->mass, exact_y, exact_v;
    if (s->lambda == 0) {
        exact_y = .5 + .5 * acceleration * time * time; exact_v = acceleration * time;
    } else {
        double onset = -expm1(-s->lambda * time);
        exact_v = acceleration / s->lambda * onset;
        exact_y = .5 + acceleration / s->lambda * (time - onset / s->lambda);
    }
    double potential = -(w + b) * state[0], kinetic = .5 * s->mass * state[1] * state[1];
    const double values[] = {state[0], state[1], s->mass, w, b, d, re, kinetic,
                             state[2], potential, kinetic + potential + state[2], exact_y, exact_v};
    for (unsigned i = 0; i < 13; i++) {
        if (!isfinite(values[i])) return PS_NUMERIC;
    }
    for (unsigned i = 0; i < 13; i++) c->values[i] = values[i];
    return PS_OK;
}
static ps_result reset(ps_context *c) {
    settling *s = c->user; s->state[0] = .5; s->state[1] = s->state[2] = 0;
    return measure(c, s->state, 0);
}
static ps_result create(ps_context *c) {
    settling *s = calloc(1, sizeof *s); if (!s) return PS_MEMORY; c->user = s;
    s->material = (ps_material){2500, 0, 0, "custom sphere"};
    s->medium = (ps_medium){1000, 100, "custom viscous fluid"};
    ps_result r = ps_parameter_define(c, "materialDensity", "Material density in kg/m^3", 2500, 1, 10000, &s->material.density_kg_m3);
    if (r == PS_OK) r = ps_parameter_define(c, "mediumDensity", "Medium density in kg/m^3", 1000, 0, 10000, &s->medium.density_kg_m3);
    if (r == PS_OK) r = ps_parameter_define(c, "viscosity", "Dynamic viscosity in Pa s", 100, 0, 10000, &s->medium.viscosity_pa_s);
    if (r == PS_OK) r = ps_parameter_define(c, "radius", "Sphere radius in m", .05, .001, 1, &s->radius);
    if (r != PS_OK) return r;
    if (s->medium.density_kg_m3 > 0 && s->medium.viscosity_pa_s == 0) {
        snprintf(c->error, sizeof c->error, "Nonzero fluid density requires positive viscosity"); return PS_INVALID;
    }
    s->volume = 4.0 / 3.0 * PS_PI * s->radius * s->radius * s->radius;
    s->mass = s->material.density_kg_m3 * s->volume;
    s->lambda = 6 * PS_PI * s->medium.viscosity_pa_s * s->radius / s->mass;
    const char *names[] = {"position.y", "velocity.y", "mass", "force.weight", "force.buoyancy", "force.drag", "reynolds", "energy.kinetic", "energy.dissipated", "energy.potential", "energy.balance", "reference.y", "reference.velocity"};
    for (unsigned i = 0; i < 13; i++) {
        ps_unit unit = i == 0 || i == 11 ? PS_METRE : i == 1 || i == 12 ? PS_VELOCITY : i == 2 ? PS_KILOGRAM : i < 6 ? PS_NEWTON : i == 6 ? PS_ONE : PS_JOULE;
        if (ps_channel_add(c, names[i], unit, names[i]) != (int)i) return PS_LIMIT;
    }
    snprintf(c->model_metadata, sizeof c->model_metadata,
        "model=custom-material sphere in uniform custom medium\nmaterial=custom sphere\nmedium=custom viscous fluid\nmaterial_density_kg_m3=%.17g\nmedium_density_kg_m3=%.17g\nviscosity_pa_s=%.17g\nradius_m=%.17g\nmass_kg=%.17g\ngravity_m_s2=9.80665\nintegrator=RK4\nforces=weight, full-volume buoyancy, Stokes drag\nvalidity=Re<=0.1; lambda*dt<=0.25\nexcluded=walls, contacts, surface, added mass, history force, non-Newtonian rheology\n", s->material.density_kg_m3, s->medium.density_kg_m3, s->medium.viscosity_pa_s, s->radius, s->mass);
    return reset(c);
}
static ps_result step(ps_context *c, double dt) {
    settling *s = c->user;
    if (s->lambda * dt > .25) {
        snprintf(c->error, sizeof c->error, "RK4 tutorial requires lambda*dt <= 0.25"); return PS_INVALID;
    }
    double next[3] = {s->state[0], s->state[1], s->state[2]};
    ps_result r = ps_ode_step(PS_RK4, slope, s, c->time_s, dt, next, 3);
    if (r == PS_OK) r = measure(c, next, c->time_s + dt);
    if (r == PS_OK) for (unsigned i = 0; i < 3; i++) s->state[i] = next[i];
    return r;
}
static void scene(ps_context *c, ps_scene *out) {
    settling *s = c->user; ps_vec3 p = ps_v3(0, s->state[0], 0);
    ps_scene_add_id(out, 1, PS_SPHERE, p, p, s->radius, 0x53dec2ff);
    for (unsigned i = 0; i < 3; i++) {
        ps_vec3 a = ps_v3((i + 1) * .12, s->state[0], 0);
        ps_vec3 b = ps_vadd(a, ps_v3(0, .03 * c->values[3 + i], 0));
        const uint32_t colors[] = {0xf2c572ff, 0x53aeefff, 0xf57f9bff};
        ps_scene_add_id(out, i + 2, PS_ARROW, a, b, .003, colors[i]);
    }
    ps_scene_label_id(out, 5, ps_v3(-.35, .7, 0), "Custom material + medium", 0xc8d7eaff);
}
static void destroy(ps_context *c) { free(c->user); c->user = NULL; }
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api = {sizeof api, PS_ABI_VERSION, 0,
        "Custom material and medium", create, reset, step, scene, destroy, NULL}; return &api;
}
