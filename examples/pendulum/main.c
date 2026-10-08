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
#ifndef PS_PENDULUM_AIR_DENSITY
#define PS_PENDULUM_AIR_DENSITY 0.0 /* 1.225 enables air drag */
#endif
static const double air_density_kg_m3 = PS_PENDULUM_AIR_DENSITY;
static const double drag_coefficient = 0.47;
static const double area_m2 = 0.01;
static const double sensor_noise_rad = 0.0;
#ifndef PS_PENDULUM_METHOD
#define PS_PENDULUM_METHOD PS_RK4
#endif
static const ps_integrator integrator = PS_PENDULUM_METHOD;
static const double absolute_tolerance = 1e-10, relative_tolerance = 1e-8;
typedef struct {
    double y[2], length, initial_angle, mass, density, coefficient, area, noise;
} pendulum;
static void derivative(double t, const double *y, double *dy, void *u) {
    (void)t;
    const pendulum *p=u;
    dy[0] = y[1];
    dy[1] = -gravity_m_s2 / p->length * sin(y[0]);
    if (p->density > 0 && p->coefficient > 0 && p->area > 0) {
        ps_medium medium = {p->density, 0, "uniform pendulum medium"};
        ps_vec3 drag = ps_drag_force(ps_v3(p->length * y[1], 0, 0), medium, p->coefficient, p->area);
        dy[1] += drag.x / (p->mass * p->length);
    }
}
static ps_result measure(ps_context *c, const double state[2]) {
    pendulum *p = c->user;
    double a = state[0], w = state[1];
    double values[] = {a, w, p->length * sin(a), -p->length * cos(a),
        0.5 * p->mass * p->length * p->length * w * w +
        p->mass * gravity_m_s2 * p->length * (1 - cos(a)), a,
        p->length * cos(a) * w, p->length * sin(a) * w, p->length * fabs(w)};
    for (unsigned i = 0; i < 9; i++)
        if (!isfinite(values[i])) return PS_NUMERIC;
    ps_rng rng = c->rng;
    if (p->noise) values[5] += ps_rng_normal(&rng, 0, p->noise);
    if (!isfinite(values[5])) return PS_NUMERIC;
    for (unsigned i = 0; i < 9; i++) c->values[i] = values[i];
    c->rng = rng;
    return PS_OK;
}
static void acceleration(double t, const double *q, double *a, void *user) {
    (void)t;
    const pendulum *p=user;
    a[0] = -gravity_m_s2 / p->length * sin(q[0]);
}
static ps_result reset(ps_context *c) {
    pendulum *p = c->user;
    double initial[] = {p->initial_angle, 0};
    ps_rng_seed(&c->rng, c->seed);
    ps_result result = measure(c, initial);
    if (result == PS_OK) { p->y[0] = initial[0]; p->y[1] = 0; }
    return result;
}
static ps_result create(ps_context *c) {
    if (length_m <= 0 || mass_kg <= 0 || air_density_kg_m3 < 0 || sensor_noise_rad < 0)
        return PS_INVALID;
    if (integrator < PS_EULER || integrator > PS_RK45)
        return PS_INVALID;
    c->user = calloc(1, sizeof(pendulum));
    if (!c->user)
        return PS_MEMORY;
    pendulum *p=c->user;
    ps_result parameter=ps_parameter_define_unit(c,"length","Pendulum length in metres",PS_METRE,length_m,.1,10,&p->length);
    if(parameter==PS_OK)
        parameter=ps_parameter_define_unit(c,"initialAngle","Initial angle in radians",PS_RADIAN,initial_angle_rad,-1.5,1.5,&p->initial_angle);
    ps_unit kilogram = {{0,1,0,0,0,0,0},1,"kg"};
    ps_unit density = {{-3,1,0,0,0,0,0},1,"kg/m3"};
    ps_unit area = {{2,0,0,0,0,0,0},1,"m2"};
    if(parameter==PS_OK)
        parameter=ps_parameter_define_unit(c,"mass","Bob mass in kilograms",kilogram,mass_kg,.001,1000,&p->mass);
    if(parameter==PS_OK)
        parameter=ps_parameter_define_unit(c,"airDensity","Medium density; zero disables drag",density,air_density_kg_m3,0,1000,&p->density);
    if(parameter==PS_OK)
        parameter=ps_parameter_define_unit(c,"dragCoefficient","Quadratic drag coefficient",PS_ONE,drag_coefficient,0,2,&p->coefficient);
    if(parameter==PS_OK)
        parameter=ps_parameter_define_unit(c,"area","Drag cross-section in square metres",area,area_m2,0,1,&p->area);
    if(parameter==PS_OK)
        parameter=ps_parameter_define_unit(c,"sensorNoise","Gaussian angle sensor standard deviation",PS_RADIAN,sensor_noise_rad,0,.5,&p->noise);
    if(parameter!=PS_OK) return parameter;
    if(integrator==PS_VERLET && p->density>0 && p->coefficient>0 && p->area>0) {
        snprintf(c->error,sizeof c->error,"Velocity Verlet requires zero velocity-dependent drag");
        return PS_INVALID;
    }
    ps_unit angular_velocity = {{0, 0, -1, 0, 0, 0, 0}, 1, "rad/s"};
    ps_channel_add(c, "angle", PS_RADIAN, "True pendulum angle");
    ps_channel_add(c, "angular_velocity", angular_velocity, "Angular velocity");
    ps_channel_add(c, "position.x", PS_METRE, "Horizontal position");
    ps_channel_add(c, "position.y", PS_METRE, "Vertical position");
    ps_channel_add(c, "energy", PS_JOULE, "Kinetic plus gravitational potential energy");
    ps_channel_add(c, "sensor.angle", PS_RADIAN, "Angle with independent Gaussian sensor noise");
    ps_unit velocity = {{1, 0, -1, 0, 0, 0, 0}, 1, "m/s"};
    ps_channel_add(c, "velocity.x", velocity, "Horizontal velocity");
    ps_channel_add(c, "velocity.y", velocity, "Vertical velocity");
    ps_channel_add(c, "speed", velocity, "Nonnegative tangential speed");
    snprintf(
        c->model_metadata, sizeof c->model_metadata,
        "model=point pendulum, massless rigid rod, uniform "
        "gravity\nlength_m=%.17g\nmass_kg=%.17g\ngravity_m_s2=%.17g\ninitial_angle_rad=%."
        "17g\nmedium_density_kg_m3=%.17g\ndrag_coefficient=%.17g\narea_m2=%.17g\nsensor_noise_"
        "rad=%.17g\nintegrator=%s\nadaptive_integrator=Dormand-Prince 5(4)\nrk45_absolute_tolerance=%.17g\nrk45_relative_tolerance=%.17g",
        p->length, p->mass, gravity_m_s2, p->initial_angle, p->density, p->coefficient,
        p->area, p->noise,
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
    if (!isfinite(dt) || dt <= 0) return PS_INVALID;
    double next[] = {p->y[0], p->y[1]};
    ps_result r = PS_OK;
    if (integrator == PS_SYMPLECTIC) {
        double d[2];
        derivative(c->time_s, next, d, p);
        ps_symplectic_step(&next[0], &next[1], d[1], dt);
    } else if (integrator == PS_VERLET) {
        r = ps_verlet_step(acceleration, p, c->time_s, dt, &next[0], &next[1], 1);
    } else if (integrator == PS_RK45) {
        ps_ode_options options = ps_ode_options_default();
        options.absolute_tolerance = absolute_tolerance;
        options.relative_tolerance = relative_tolerance;
        options.initial_step = options.maximum_step = dt;
        r = ps_ode_integrate(derivative, p, c->time_s, c->time_s + dt, next, 2, &options, NULL);
    } else
        r = ps_ode_step(integrator, derivative, p, c->time_s, dt, next, 2);
    if (r == PS_OK) r = measure(c, next);
    if (r == PS_OK) { p->y[0] = next[0]; p->y[1] = next[1]; }
    return r;
}
static ps_result adaptive_step(ps_context *c,double proposed,double minimum,double maximum,
                                ps_step_interval *interval) {
    pendulum *p=c->user;
    double next[] = {p->y[0], p->y[1]};
    ps_ode_options options=ps_ode_options_default();
    options.absolute_tolerance=absolute_tolerance;options.relative_tolerance=relative_tolerance;
    options.initial_step=proposed;options.minimum_step=minimum;options.maximum_step=maximum;
    ps_ode_report report;ps_ode_diagnostic diagnostic;
    ps_result result=ps_ode_step_diagnosed(derivative,p,c->time_s,c->time_s+proposed,
                                          next,2,&options,&report,&diagnostic);
    if(result!=PS_OK) {
        snprintf(c->error,sizeof c->error,"%s",ps_ode_diagnostic_string(diagnostic.reason));
        return result;
    }
    result = measure(c, next);
    if (result == PS_OK) {
        p->y[0] = next[0]; p->y[1] = next[1];
        *interval=(ps_step_interval){report.reached_time-c->time_s,report.next_step};
    }
    return result;
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
    /* Force arrows use 0.05 metres per newton; velocity keeps its own scale. */
    const double force_scale = 0.05;
    ps_vec3 weight = ps_v3(0, -p->mass * gravity_m_s2, 0);
    double constraint = p->mass * (gravity_m_s2 * cos(c->values[0]) +
                                  p->length * c->values[1] * c->values[1]);
    ps_vec3 rod_force = ps_vscale(bob, -constraint / p->length);
    ps_vec3 weight_end = ps_vadd(bob, ps_vscale(weight, force_scale));
    ps_vec3 rod_end = ps_vadd(bob, ps_vscale(rod_force, force_scale));
    ps_scene_add_id(s, 7, PS_ARROW, bob, weight_end, 0, 0xe87979ff);
    ps_scene_add_id(s, 8, PS_ARROW, bob, rod_end, 0, 0x91d28aff);
    (void)ps_scene_label_id(s, 10, weight_end, "Gewicht · 0.05 m/N", 0xe87979ff);
    (void)ps_scene_label_id(s, 11, rod_end, "Stangenkraft · 0.05 m/N", 0x91d28aff);
    (void)ps_scene_set_parent(s, 7, 3);
    (void)ps_scene_set_parent(s, 8, 3);
    (void)ps_scene_set_parent(s, 10, 7);
    (void)ps_scene_set_parent(s, 11, 8);
    if (p->density > 0 && p->coefficient > 0 && p->area > 0) {
        ps_medium medium = {p->density, 0, "uniform pendulum medium"};
        ps_vec3 drag = ps_drag_force(velocity, medium, p->coefficient, p->area);
        ps_vec3 drag_end = ps_vadd(bob, ps_vscale(drag, force_scale));
        ps_scene_add_id(s, 9, PS_ARROW, bob, drag_end, 0, 0xc499e8ff);
        (void)ps_scene_label_id(s, 12, drag_end, "Luftwiderstand · 0.05 m/N", 0xc499e8ff);
        (void)ps_scene_set_parent(s, 9, 3);
        (void)ps_scene_set_parent(s, 12, 9);
    }
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
