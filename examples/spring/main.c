#include "physim/experiment.h"
#include "physim/mechanics.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Horizontal slider: SI parameters. Edit and rebuild with F5. */
#ifndef PS_SPRING_DAMPING
#define PS_SPRING_DAMPING 1.2
#endif
static const double mass_kg = 1, stiffness_n_m = 16, default_damping_ns_m = PS_SPRING_DAMPING;
static const double rest_length_m = 1, initial_extension_m = .35, initial_velocity_m_s = 0;
typedef struct {
    double y[3];
    double damping_ns_m;
} experiment; /* extension, velocity, dissipated work */
typedef struct { const experiment *state; ps_result error; } derivative_context;
static ps_result force(const experiment *e, double x, double v, ps_vec3 *out) {
    /* Crossing the anchor leaves this one-dimensional axial model's domain. */
    if (x <= -rest_length_m)
        return PS_INVALID;
    return ps_spring_force(ps_v3(x, 0, 0), ps_v3(v, 0, 0), ps_v3(-rest_length_m, 0, 0),
                           ps_v3(0, 0, 0), stiffness_n_m, rest_length_m, e->damping_ns_m, out);
}
static void derivative(double t, const double *y, double *dy, void *user) {
    (void)t;
    derivative_context *context = user;
    const experiment *e = context->state;
    ps_vec3 f;
    ps_result r = force(e, y[0], y[1], &f);
    if (r != PS_OK) {
        context->error = r;
        dy[0] = dy[1] = dy[2] = NAN;
        return;
    }
    dy[0] = y[1];
    dy[1] = f.x / mass_kg;
    dy[2] = e->damping_ns_m * y[1] * y[1];
}
static ps_result measure(ps_context *c, const double *y) {
    const experiment *e = c->user;
    ps_vec3 f;
    ps_result r = force(e, y[0], y[1], &f);
    if (r != PS_OK)
        return r;
    double kinetic = .5 * mass_kg * y[1] * y[1], potential = .5 * stiffness_n_m * y[0] * y[0];
    double values[] = {y[0],
                       y[1],
                       kinetic,
                       potential,
                       kinetic + potential,
                       y[2],
                       kinetic + potential + y[2],
                       -stiffness_n_m * y[0],
                       -e->damping_ns_m * y[1],
                       f.x,
                       e->damping_ns_m * y[1] * y[1]};
    for (unsigned i = 0; i < sizeof values / sizeof values[0]; i++)
        if (!isfinite(values[i]))
            return PS_NUMERIC;
    memcpy(c->values, values, sizeof values);
    return PS_OK;
}
static ps_result reset(ps_context *c) {
    experiment *e = c->user;
    double y[] = {initial_extension_m, initial_velocity_m_s, 0};
    ps_result r = measure(c, y);
    if (r == PS_OK)
        memcpy(e->y, y, sizeof y);
    return r;
}
static ps_result create(ps_context *c) {
    if (!isfinite(mass_kg) || mass_kg <= 0 || !isfinite(stiffness_n_m) || stiffness_n_m <= 0 ||
        !isfinite(default_damping_ns_m) || default_damping_ns_m < 0 || !isfinite(rest_length_m) ||
        rest_length_m <= 0 || !isfinite(initial_extension_m) || !isfinite(initial_velocity_m_s))
        return PS_INVALID;
    c->user = calloc(1, sizeof(experiment));
    if (!c->user)
        return PS_MEMORY;
    experiment *e = c->user;
    const ps_unit damping_unit = {{0, 1, -1, 0, 0, 0, 0}, 1, "N s/m"};
    ps_result parameter = ps_parameter_define_unit(c, "damping", "Viscous damping coefficient", damping_unit,
                                                  default_damping_ns_m, 0, 64, &e->damping_ns_m);
    if (parameter != PS_OK) return parameter;
    const ps_unit newton = {{1, 1, -2, 0, 0, 0, 0}, 1, "N"};
    const ps_unit watt = {{2, 1, -3, 0, 0, 0, 0}, 1, "W"};
    ps_channel_add(c, "position.x", PS_METRE, "Extension from equilibrium at X=0");
    ps_channel_add(c, "velocity.x", PS_VELOCITY, "Slider velocity");
    ps_channel_add(c, "energy.kinetic", PS_JOULE, "Kinetic energy");
    ps_channel_add(c, "energy.spring", PS_JOULE, "Elastic potential energy");
    ps_channel_add(c, "energy", PS_JOULE, "Mechanical energy; damping removes energy");
    ps_channel_add(c, "energy.dissipated", PS_JOULE,
                   "Integral of c*v^2, same RK4 stages as motion");
    ps_channel_add(c, "energy.balance", PS_JOULE,
                   "Mechanical plus dissipated energy; compare with initial");
    ps_channel_add(c, "force.spring.x", newton, "Elastic spring force");
    ps_channel_add(c, "force.damper.x", newton, "Viscous damping force");
    ps_channel_add(c, "force.total.x", newton, "Total axial force from mechanics API");
    ps_channel_add(c, "power.dissipated", watt, "Nonnegative damping power c*v^2");
    snprintf(c->model_metadata, sizeof c->model_metadata,
             "model=horizontal spring-mass-damper\nintegrator=RK4\n"
             "mass_kg=%.17g\nstiffness_n_m=%.17g\ndamping_ns_m=%.17g\n"
             "rest_length_m=%.17g\ninitial_extension_m=%.17g\ninitial_velocity_m_s=%.17g\n"
             "anchor_x_m=%.17g\nequilibrium_x_m=0\n"
             "constraints=prescribed 1D horizontal guide, no constraint solver\n"
             "gravity=none\ncontact=none\nspring_mass=0\nthermal_model=none\n"
             "dissipated_work=integrated c*v^2\ndomain=slider right of anchor\n",
             mass_kg, stiffness_n_m, e->damping_ns_m, rest_length_m, initial_extension_m,
             initial_velocity_m_s, -rest_length_m);
    return reset(c);
}
static ps_result step(ps_context *c, double dt) {
    experiment *e = c->user;
    double y[3];
    memcpy(y, e->y, sizeof y);
    derivative_context context = {e, PS_OK};
    ps_result r = ps_ode_step(PS_RK4, derivative, &context, c->time_s, dt, y, 3);
    if (context.error != PS_OK) {
        snprintf(c->error, sizeof c->error,
                 "Spring domain exceeded or invalid force; refine dt and check initial energy.");
        return context.error;
    }
    if (r == PS_OK)
        r = measure(c, y);
    if (r == PS_OK)
        memcpy(e->y, y, sizeof y);
    return r;
}
static void scene(ps_context *c, ps_scene *s) {
    double x = c->values[0], left = -rest_length_m;
    ps_vec3 coil[65];
    for (unsigned i = 0; i < 65; i++) {
        double u = (double)i / 64, radius = .075 * fmin(1, 12 * fmin(u, 1 - u));
        coil[i] = ps_v3(left + (x - left) * u, radius * sin(12 * PS_PI * u),
                        radius * cos(12 * PS_PI * u));
    }
    (void)ps_scene_polyline_id(s, 1, coil, 65, .009, 0x91a9c5ff);
    ps_scene_add_id(s, 2, PS_BOX, ps_v3(left - .06, 0, 0), ps_v3(.12, .7, .25), 0, 0x596675ff);
    ps_scene_add_id(s, 3, PS_BOX, ps_v3(x, 0, 0), ps_v3(.3, .3, .3), 0, 0x70b1eeff);
    ps_scene_add_id(s, 4, PS_LINE, ps_v3(left, -.18, 0), ps_v3(.7, -.18, 0), .008, 0x596675ff);
    /* Parallel damper symbol; geometry is illustrative, not a cylinder model. */
    ps_scene_add_id(s, 5, PS_LINE, ps_v3(left, -.3, 0), ps_v3(x, -.3, 0), .008, 0xa2b0c1ff);
    ps_scene_add_id(s, 6, PS_BOX, ps_v3(left * .6, -.3, 0), ps_v3(.3, .09, .09), 0, 0x596675ff);
    ps_scene_add_id(s, 7, PS_LINE, ps_v3(x, -.3, 0), ps_v3(x, -.15, 0), .008, 0xa2b0c1ff);
    ps_scene_add_id(s, 8, PS_POINT, ps_v3(0, -.18, .1), ps_v3(0, -.18, .1), .025, 0xe6edf3ff);
    const double y[] = {.25, .42, .59}, scale[] = {.2, .06, .06};
    const unsigned channel[] = {1, 7, 8};
    const uint32_t color[] = {0x6dcf94ff, 0xf2c572ff, 0xff8eafff};
    for (unsigned i = 0; i < 3; i++)
        ps_scene_add_id(s, 20 + i, PS_ARROW, ps_v3(x, y[i], 0),
                        ps_v3(x + c->values[channel[i]] * scale[i], y[i], 0), .008, color[i]);
    (void)ps_scene_label_id(s, 30, ps_v3(left, .85, 0), "Feder–Masse–Dämpfer", 0xe6edf3ff);
    (void)ps_scene_label_id(s, 31, ps_v3(left, -.6, 0), "v: 0.2 m pro m/s · Kräfte: 0.06 m pro N",
                            0xc0c6cfff);
}
static void destroy(ps_context *c) {
    free(c->user);
    c->user = NULL;
}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api = {
        sizeof api, PS_ABI_VERSION, 0, "Feder–Masse–Dämpfer", create, reset, step, scene, destroy, NULL};
    return &api;
}
