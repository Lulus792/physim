#include "physim/experiment.h"
#include "physim/measurement.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef PS_SENSOR_IDEAL
#define PS_SENSOR_IDEAL 0
#endif
#ifndef PS_SENSOR_DROPOUT
#define PS_SENSOR_DROPOUT .05
#endif
/* Vacuum projectile, 1 kg, no ground. Uncertainty is sampled once per run;
 * sensor noise uses a separate RNG, so sampling never changes the trajectory. */
typedef struct {
    double state[4], time, time_correction, initial_vx, initial_vy;
    ps_sensor sensor[2];
    ps_vec3 last_sensor;
    double last_sensor_time;
    bool has_sensor;
    ps_vec3 trail[64];
    unsigned count, ticks;
} projectile;
static void ode(double t, const double *y, double *d, void *user) {
    (void)t;
    (void)user;
    d[0] = y[2];
    d[1] = y[3];
    d[2] = 0;
    d[3] = -9.80665;
}
static ps_result measure(projectile *p, double values[PS_MAX_CHANNELS]) {
    ps_measurement samples[2];
    for (unsigned i = 0; i < 2; i++) {
        ps_result r = ps_sensor_read(&p->sensor[i], p->time, (ps_quantity){p->state[i], PS_METRE},
                                     &samples[i]);
        if (r != PS_OK)
            return r;
    }
    for (unsigned i = 0; i < 4; i++)
        values[i] = p->state[i];
    values[4] =
        .5 * (p->state[2] * p->state[2] + p->state[3] * p->state[3]) + 9.80665 * p->state[1];
    values[5] = -2 + 3 * p->time;
    values[6] = 5 * p->time - .5 * 9.80665 * p->time * p->time;
    values[7] = samples[0].value.value;
    values[8] = samples[1].value.value;
    values[9] = samples[0].state;
    values[10] = samples[1].state;
    values[11] = p->time;
    values[12] = samples[0].standard_uncertainty;
    values[13] = samples[1].standard_uncertainty;
    values[14] = (double)samples[0].skipped;
    if (samples[0].state == PS_MEASUREMENT_VALID && samples[1].state == PS_MEASUREMENT_VALID) {
        p->last_sensor = ps_v3(values[7], values[8], 0);
        p->last_sensor_time = p->time;
        p->has_sensor = true;
    }
    return PS_OK;
}
static ps_result reset(ps_context *c) {
    projectile next = {0}, *p = &next;
    ps_rng rng;
    ps_rng_seed(&rng, c->seed);
    ps_sensor_config sensor = {0};
    sensor.unit = PS_METRE;
    sensor.rate_hz = 100;
    sensor.resolution = PS_SENSOR_IDEAL ? 0 : .005;
    sensor.offset = PS_SENSOR_IDEAL ? 0 : .01;
    sensor.drift_per_s = PS_SENSOR_IDEAL ? 0 : .002;
    sensor.noise = (ps_distribution){PS_DIST_NORMAL, 0, PS_SENSOR_IDEAL ? 0 : .02};
    sensor.dropout_probability = PS_SENSOR_IDEAL ? 0 : PS_SENSOR_DROPOUT;
    sensor.uncertainty_absolute = PS_SENSOR_IDEAL ? 0 : .003;
    sensor.uncertainty_relative = PS_SENSOR_IDEAL ? 0 : .001;
    ps_result r = ps_sensor_init(&p->sensor[0], &sensor, c->seed ^ UINT64_C(0xa0761d6478bd642f));
    if (r == PS_OK)
        r = ps_sensor_init(&p->sensor[1], &sensor, c->seed ^ UINT64_C(0xe7037ed1a0b428db));
    if (r != PS_OK)
        return r;
    p->state[0] = -2;
    r = ps_distribution_sample((ps_distribution){PS_DIST_NORMAL, 3, .15}, &rng, &p->initial_vx);
    if (r == PS_OK)
        r = ps_distribution_sample((ps_distribution){PS_DIST_NORMAL, 5, .25}, &rng, &p->initial_vy);
    if (r != PS_OK)
        return r;
    p->state[2] = p->initial_vx;
    p->state[3] = p->initial_vy;
    p->trail[0] = ps_v3(-2, 0, 0);
    p->count = 1;
    double values[PS_MAX_CHANNELS] = {0};
    r = measure(p, values);
    if (r != PS_OK)
        return r;
    snprintf(
        c->model_metadata, sizeof c->model_metadata,
        "model=uncertain vacuum projectile\nmass_kg=1\ng_m_s2=9.80665\nground=none\n"
        "integrator=RK4\nvx_distribution=normal\nvx_mean_m_s=3\nvx_stddev_m_s=0.15\n"
        "vy_distribution=normal\nvy_mean_m_s=5\nvy_stddev_m_s=0.25\n"
        "sampled_vx_m_s=%.17g\nsampled_vy_m_s=%.17g\n"
        "measurement_api=1\nsensor_rate_hz=100\nsensor_distribution=normal\nsensor_stddev_m=%.17g\n"
        "sensor_seed_x=seed XOR 0xa0761d6478bd642f\nsensor_seed_y=seed XOR 0xe7037ed1a0b428db\n"
        "sensor_offset_m=%.17g\nsensor_drift_m_s=%.17g\nsensor_resolution_m=%.17g\n"
        "sensor_dropout_probability=%.17g\nsensor_u_absolute_m=%.17g\nsensor_u_relative=%.17g\n"
        "sensor_status=0:not_due,1:valid,2:dropped; invalid values are zero placeholders\n",
        p->initial_vx, p->initial_vy, sensor.noise.b, sensor.offset, sensor.drift_per_s,
        sensor.resolution, sensor.dropout_probability, sensor.uncertainty_absolute,
        sensor.uncertainty_relative);
    *(projectile *)c->user = next;
    c->rng = rng;
    memcpy(c->values, values, sizeof values);
    return PS_OK;
}
static ps_result create(ps_context *c) {
    c->user = calloc(1, sizeof(projectile));
    if (!c->user)
        return PS_MEMORY;
    ps_channel_add(c, "position.x", PS_METRE, "Physical x with uncertain initial velocity");
    ps_channel_add(c, "position.y", PS_METRE, "Physical y with uncertain initial velocity");
    ps_channel_add(c, "velocity.x", PS_VELOCITY, "Physical vx");
    ps_channel_add(c, "velocity.y", PS_VELOCITY, "Physical vy");
    ps_channel_add(c, "energy", PS_JOULE, "Physical mechanical energy");
    ps_channel_add(c, "nominal.x", PS_METRE, "Nominal trajectory, mean initial velocity");
    ps_channel_add(c, "nominal.y", PS_METRE, "Nominal trajectory, mean initial velocity");
    ps_channel_add(c, "sensor.x", PS_METRE, "Sensor x; use only rows with sensor.x.status = 1");
    ps_channel_add(c, "sensor.y", PS_METRE, "Sensor y; use only rows with sensor.y.status = 1");
    ps_channel_add(c, "sensor.x.status", PS_ONE, "0:not due, 1:valid, 2:dropped");
    ps_channel_add(c, "sensor.y.status", PS_ONE, "0:not due, 1:valid, 2:dropped");
    ps_channel_add(c, "sensor.time", PS_SECOND,
                   "Actual sample time, meaningful only when status is nonzero");
    ps_channel_add(c, "sensor.x.u", PS_METRE,
                   "Combined standard uncertainty, meaningful only for valid x");
    ps_channel_add(c, "sensor.y.u", PS_METRE,
                   "Combined standard uncertainty, meaningful only for valid y");
    ps_channel_add(c, "sensor.skipped", PS_ONE, "Skipped scheduled slots since previous attempt");
    return reset(c);
}
static ps_result step(ps_context *c, double dt) {
    if (!isfinite(dt) || dt <= 0 || dt > 1)
        return PS_INVALID;
    projectile *p = c->user, next = *p;
    ps_result r = ps_ode_step(PS_RK4, ode, NULL, p->time, dt, next.state, 4);
    if (r != PS_OK)
        return r;
    double corrected_dt = dt - next.time_correction;
    next.time = p->time + corrected_dt;
    next.time_correction = (next.time - p->time) - corrected_dt;
    if (!isfinite(next.time) || next.time > 100000)
        return PS_LIMIT;
    double values[PS_MAX_CHANNELS] = {0};
    r = measure(&next, values);
    if (r != PS_OK)
        return r;
    if (++next.ticks % 10 == 0) {
        if (next.count == 64) {
            memmove(next.trail, next.trail + 1, 63 * sizeof *next.trail);
            next.count = 63;
        }
        next.trail[next.count++] = ps_v3(next.state[0], next.state[1], 0);
    }
    *p = next;
    memcpy(c->values, values, sizeof values);
    return PS_OK;
}
static void scene(ps_context *c, ps_scene *s) {
    projectile *p = c->user;
    ps_vec3 actual = ps_v3(p->state[0], p->state[1], 0);
    ps_vec3 nominal = ps_v3(c->values[5], c->values[6], 0);
    ps_scene_add_id(s, 1, PS_SPHERE, actual, actual, .09, 0x53dec2ff);
    ps_scene_add_id(s, 2, PS_POINT, nominal, nominal, .05, 0x779bccff);
    if (p->has_sensor) {
        ps_scene_add_id(s, 3, PS_POINT, p->last_sensor, actual, .035, 0xf2c572ff);
        char label[64];
        snprintf(label, sizeof label, "Sensor · %.2f s", p->last_sensor_time);
        (void)ps_scene_label_id(s, 4, p->last_sensor, label, 0xf2c572ff);
    }
    ps_scene_add_id(s, 5, PS_ARROW, actual,
                    ps_vadd(actual, ps_v3(p->state[2] * .12, p->state[3] * .12, 0)), 0, 0x53dec2ff);
    if (p->count > 1)
        (void)ps_scene_polyline_id(s, 6, p->trail, p->count, .008, 0x53dec2ff);
    if (p->time > 0) {
        ps_vec3 path[32];
        for (unsigned i = 0; i < 32; i++) {
            double t = p->time * i / 31;
            path[i] = ps_v3(-2 + 3 * t, 5 * t - .5 * 9.80665 * t * t, 0);
        }
        (void)ps_scene_polyline_id(s, 7, path, 32, .004, 0x779bccff);
    }
    (void)ps_scene_label_id(s, 8, actual, "Zufälliger Anfangszustand", 0xe2eaf2ff);
    (void)ps_scene_label_id(s, 9, nominal, "Sollbahn", 0x779bccff);
}
static void destroy(ps_context *c) { free(c->user); }
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api = {sizeof api, PS_ABI_VERSION, 0,    "Wurf mit Unsicherheit",
                                          create,     reset,          step, scene,
                                          destroy, NULL};
    return &api;
}
