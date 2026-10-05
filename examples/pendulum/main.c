#include "physim/experiment.h"
#include "physim/numerics.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* Edit these SI parameters, then Build. One state, fixed/adaptive steps, explicit seed. */
static const double length_m = 1.5;
static const double mass_kg = 1.0;
static const double gravity_m_s2 = 9.80665;
static const double initial_angle_rad = 0.45;
static const double air_density_kg_m3 = 0.0; /* 1.225 enables air drag */
static const double drag_coefficient = 0.47;
static const double area_m2 = 0.01;
static const double sensor_noise_rad = 0.0;
#ifndef PS_PENDULUM_METHOD
#define PS_PENDULUM_METHOD PS_RK4
#endif
static const ps_integrator integrator = PS_PENDULUM_METHOD;
static const double absolute_tolerance = 1e-10, relative_tolerance = 1e-8;
typedef struct {
    double y[2], length, initial_angle;
} pendulum;
static void derivative(double t, const double *y, double *dy, void *u) {
    (void)t;
    const pendulum *p=u;
    dy[0] = y[1];
    dy[1] = -gravity_m_s2 / p->length * sin(y[0]) - 0.5 * air_density_kg_m3 * drag_coefficient *
                                                       area_m2 * p->length / mass_kg * y[1] *
                                                       fabs(y[1]);
}
static void measure(ps_context *c) {
    pendulum *p = c->user;
    double a = p->y[0], w = p->y[1];
    c->values[0] = a;
    c->values[1] = w;
    c->values[2] = p->length * sin(a);
    c->values[3] = -p->length * cos(a);
    c->values[4] = 0.5 * mass_kg * p->length * p->length * w * w +
                   mass_kg * gravity_m_s2 * p->length * (1 - cos(a));
    c->values[5] = a + (sensor_noise_rad ? ps_rng_normal(&c->rng, 0, sensor_noise_rad) : 0);
}
static void acceleration(double t, const double *q, double *a, void *user) {
    (void)t;
    const pendulum *p=user;
    a[0] = -gravity_m_s2 / p->length * sin(q[0]);
}
static ps_result reset(ps_context *c) {
    pendulum *p = c->user;
    p->y[0] = p->initial_angle;
    p->y[1] = 0;
    ps_rng_seed(&c->rng, c->seed);
    measure(c);
    return PS_OK;
}
static ps_result create(ps_context *c) {
    if (length_m <= 0 || mass_kg <= 0 || air_density_kg_m3 < 0 || sensor_noise_rad < 0)
        return PS_INVALID;
    if (integrator < PS_EULER || integrator > PS_RK45 ||
        (integrator == PS_VERLET && air_density_kg_m3 != 0))
        return PS_INVALID;
    c->user = calloc(1, sizeof(pendulum));
    if (!c->user)
        return PS_MEMORY;
    pendulum *p=c->user;
    ps_result parameter=ps_parameter_define(c,"length","Pendulum length in metres",length_m,.1,10,&p->length);
    if(parameter==PS_OK)
        parameter=ps_parameter_define(c,"initialAngle","Initial angle in radians",initial_angle_rad,-1.5,1.5,&p->initial_angle);
    if(parameter!=PS_OK) return parameter;
    ps_unit angular_velocity = {{0, 0, -1, 0, 0, 0, 0}, 1, "rad/s"};
    ps_channel_add(c, "angle", PS_RADIAN, "True pendulum angle");
    ps_channel_add(c, "angular_velocity", angular_velocity, "Angular velocity");
    ps_channel_add(c, "position.x", PS_METRE, "Horizontal position");
    ps_channel_add(c, "position.y", PS_METRE, "Vertical position");
    ps_channel_add(c, "energy", PS_JOULE, "Kinetic plus gravitational potential energy");
    ps_channel_add(c, "sensor.angle", PS_RADIAN, "Angle with independent Gaussian sensor noise");
    snprintf(
        c->model_metadata, sizeof c->model_metadata,
        "model=point pendulum, massless rigid rod, uniform "
        "gravity\nlength_m=%.17g\nmass_kg=%.17g\ngravity_m_s2=%.17g\ninitial_angle_rad=%."
        "17g\nmedium_density_kg_m3=%.17g\ndrag_coefficient=%.17g\narea_m2=%.17g\nsensor_noise_"
        "rad=%.17g\nintegrator=%s\nadaptive_integrator=Dormand-Prince 5(4)\nrk45_absolute_tolerance=%.17g\nrk45_relative_tolerance=%.17g",
        p->length, mass_kg, gravity_m_s2, p->initial_angle, air_density_kg_m3, drag_coefficient,
        area_m2, sensor_noise_rad,
        integrator == PS_RK4      ? "RK4"
        : integrator == PS_EULER  ? "Euler"
        : integrator == PS_VERLET ? "velocity Verlet"
        : integrator == PS_RK45   ? "Dormand-Prince 5(4)"
                                  : "symplectic Euler",
        absolute_tolerance, relative_tolerance);
    return reset(c);
}
static ps_result step(ps_context *c, double dt) {
    pendulum *p = c->user;
    ps_result r = PS_OK;
    if (integrator == PS_SYMPLECTIC) {
        double d[2];
        derivative(c->time_s, p->y, d, p);
        ps_symplectic_step(&p->y[0], &p->y[1], d[1], dt);
    } else if (integrator == PS_VERLET) {
        r = ps_verlet_step(acceleration, p, c->time_s, dt, &p->y[0], &p->y[1], 1);
    } else if (integrator == PS_RK45) {
        ps_ode_options options = ps_ode_options_default();
        options.absolute_tolerance = absolute_tolerance;
        options.relative_tolerance = relative_tolerance;
        options.initial_step = options.maximum_step = dt;
        r = ps_ode_integrate(derivative, p, c->time_s, c->time_s + dt, p->y, 2, &options, NULL);
    } else
        r = ps_ode_step(integrator, derivative, p, c->time_s, dt, p->y, 2);
    if (r == PS_OK)
        measure(c);
    return r;
}
static ps_result adaptive_step(ps_context *c,double proposed,double minimum,double maximum,
                                ps_step_interval *interval) {
    pendulum *p=c->user;
    ps_ode_options options=ps_ode_options_default();
    options.absolute_tolerance=absolute_tolerance;options.relative_tolerance=relative_tolerance;
    options.initial_step=proposed;options.minimum_step=minimum;options.maximum_step=maximum;
    ps_ode_report report;ps_ode_diagnostic diagnostic;
    ps_result result=ps_ode_step_diagnosed(derivative,p,c->time_s,c->time_s+proposed,
                                          p->y,2,&options,&report,&diagnostic);
    if(result!=PS_OK) {
        snprintf(c->error,sizeof c->error,"%s",ps_ode_diagnostic_string(diagnostic.reason));
        return result;
    }
    measure(c);*interval=(ps_step_interval){report.reached_time-c->time_s,report.next_step};
    return PS_OK;
}
static void scene(ps_context *c, ps_scene *s) {
    const pendulum *p=c->user;
    ps_vec3 origin = ps_v3(0, 0, 0), bob = ps_v3(c->values[2], c->values[3], 0);
    ps_scene_add_id(s, 1, PS_LINE, origin, bob, 0, 0xb5c4d8ff);
    ps_scene_add_id(s, 2, PS_SPHERE, origin, origin, 0.045, 0xe6edf3ff);
    ps_scene_add_id(s, 3, PS_SPHERE, bob, bob, 0.12, 0x53dec2ff);
    ps_vec3 velocity = ps_v3(p->length * cos(c->values[0]) * c->values[1],
                             p->length * sin(c->values[0]) * c->values[1], 0);
    ps_scene_add_id(s, 4, PS_ARROW, bob, ps_vadd(bob, ps_vscale(velocity, 0.3)), 0, 0xf2c572ff);
    (void)ps_scene_label_id(s, 5, origin, "Aufhaengung", 0xb5c4d8ff);
    (void)ps_scene_label_id(s, 6, bob, "Pendelmasse", 0x53dec2ff);
    (void)ps_scene_group(s,100,0,"Pendel");
    (void)ps_scene_group(s,101,100,"Bewegte Masse");
    (void)ps_scene_set_parent(s,1,100);
    (void)ps_scene_set_parent(s,2,100);
    (void)ps_scene_set_parent(s,3,101);
    (void)ps_scene_set_parent(s,4,3);
    (void)ps_scene_set_parent(s,5,2);
    (void)ps_scene_set_parent(s,6,3);
}
static void destroy(ps_context *c) {
    free(c->user);
    c->user = NULL;
}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api = {sizeof(ps_experiment_api),
                                          PS_ABI_VERSION,
                                          PS_EXPERIMENT_SCENE_HIERARCHY | PS_EXPERIMENT_ADAPTIVE_STEPS,
                                          "Pendel",
                                          create,
                                          reset,
                                          step,
                                          scene,
                                          destroy, adaptive_step};
    return &api;
}
