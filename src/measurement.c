#include "physim/measurement.h"
#include <float.h>
#include <math.h>
#include <string.h>
#define SENSOR_INITIALIZED UINT32_C(0x50534d31)
#define MAX_INDEX UINT64_C(4503599627370495)
ps_result ps_distribution_validate(ps_distribution d) {
    if (!isfinite(d.a) || !isfinite(d.b))
        return PS_INVALID;
    switch (d.kind) {
    case PS_DIST_CONSTANT:
        return d.b == 0 ? PS_OK : PS_INVALID;
    case PS_DIST_UNIFORM:
        return d.a <= d.b ? PS_OK : PS_INVALID;
    case PS_DIST_NORMAL:
        return d.b >= 0 ? PS_OK : PS_INVALID;
    default:
        return PS_INVALID;
    }
}
ps_result ps_distribution_sample(ps_distribution d, ps_rng *rng, double *out) {
    if (!out || !rng || !(rng->increment & 1u) || ps_distribution_validate(d) != PS_OK)
        return PS_INVALID;
    ps_rng next = *rng;
    double value = d.a;
    if (d.kind == PS_DIST_UNIFORM && d.a != d.b) {
        double u = ps_rng_uniform(&next);
        value = (1 - u) * d.a + u * d.b; /* Finite interval may have an overflowing width. */
    } else if (d.kind == PS_DIST_NORMAL && d.b)
        value = ps_rng_normal(&next, d.a, d.b);
    if (!isfinite(value))
        return PS_NUMERIC;
    *rng = next;
    *out = value;
    return PS_OK;
}
ps_result ps_distribution_moments(ps_distribution d, double *mean, double *stddev) {
    if (!mean || !stddev || mean == stddev || ps_distribution_validate(d) != PS_OK)
        return PS_INVALID;
    double m = d.a, s = 0;
    if (d.kind == PS_DIST_UNIFORM) {
        m = d.a / 2 + d.b / 2;
        s = (d.b / 2 - d.a / 2) / sqrt(3);
    } else if (d.kind == PS_DIST_NORMAL)
        s = d.b;
    if (!isfinite(m) || !isfinite(s))
        return PS_NUMERIC;
    *mean = m;
    *stddev = s;
    return PS_OK;
}
ps_result ps_sensor_config_validate(const ps_sensor_config *c) {
    if (!c || !ps_unit_valid(c->unit) || !isfinite(c->rate_hz) || c->rate_hz <= 0 ||
        !isfinite(c->start_time_s) || !isfinite(c->resolution) || c->resolution < 0 ||
        !isfinite(c->offset) || !isfinite(c->drift_per_s) ||
        ps_distribution_validate(c->noise) != PS_OK || !isfinite(c->dropout_probability) ||
        c->dropout_probability < 0 || c->dropout_probability > 1 ||
        !isfinite(c->uncertainty_absolute) || c->uncertainty_absolute < 0 ||
        !isfinite(c->uncertainty_relative) || c->uncertainty_relative < 0)
        return PS_INVALID;
    double period = 1 / c->rate_hz, next = c->start_time_s + period;
    if (!isfinite(period) || period <= 0 || !isfinite(next) || next <= c->start_time_s)
        return PS_NUMERIC;
    return PS_OK;
}
ps_result ps_sensor_init(ps_sensor *sensor, const ps_sensor_config *config, uint64_t seed) {
    if (!sensor)
        return PS_INVALID;
    ps_result r = ps_sensor_config_validate(config);
    if (r != PS_OK)
        return r;
    ps_sensor next = {0};
    next.config = *config;
    next.seed = seed;
    next.next_time_s = next.last_time_s = config->start_time_s;
    next.initialized = SENSOR_INITIALIZED;
    *sensor = next;
    return PS_OK;
}
ps_result ps_sensor_reset(ps_sensor *sensor, uint64_t seed) {
    if (!sensor || sensor->initialized != SENSOR_INITIALIZED)
        return PS_INVALID;
    return ps_sensor_init(sensor, &sensor->config, seed);
}
ps_result ps_sensor_next_time(const ps_sensor *sensor, double *time_s) {
    if (!sensor || !time_s || sensor->initialized != SENSOR_INITIALIZED ||
        !isfinite(sensor->next_time_s))
        return PS_INVALID;
    *time_s = sensor->next_time_s;
    return PS_OK;
}
/* SplitMix64's bijective mixing operations derive local PCG seeds. Noise and
 * dropout use different domains. This is a reproducibility mechanism, not crypto. */
static uint64_t mix(uint64_t x) {
    x = (x ^ (x >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    x = (x ^ (x >> 27)) * UINT64_C(0x94d049bb133111eb);
    return x ^ (x >> 31);
}
ps_result ps_sensor_read(ps_sensor *sensor, double time, ps_quantity truth, ps_measurement *out) {
    if (!sensor || !out || sensor->initialized != SENSOR_INITIALIZED || !isfinite(time) ||
        time < sensor->config.start_time_s || time < sensor->last_time_s ||
        !isfinite(sensor->next_time_s) || sensor->next_index > MAX_INDEX)
        return PS_INVALID;
    const ps_sensor_config *c = &sensor->config;
    ps_result r = ps_sensor_config_validate(c);
    if (r != PS_OK)
        return r;
    double ideal;
    r = ps_convert(truth.value, truth.unit, c->unit, &ideal);
    if (r != PS_OK)
        return r;
    double period = 1 / c->rate_hz;
    double tolerance =
        fmin(period * 1e-6, 8 * DBL_EPSILON * fmax(fabs(time), fabs(sensor->next_time_s)));
    ps_measurement reading = {0};
    reading.value.unit = c->unit;
    reading.time_s = time;
    reading.index = sensor->next_index;
    if (time < sensor->next_time_s && sensor->next_time_s - time > tolerance) {
        *out = reading;
        return PS_OK;
    }
    double elapsed = time - c->start_time_s, slots = floor(elapsed / period);
    if (!isfinite(slots) || slots < 0 || slots >= (double)MAX_INDEX)
        return PS_LIMIT;
    uint64_t index = (uint64_t)slots;
    double following = c->start_time_s + (double)(index + 1) * period;
    if (following <= time || following - time <= tolerance)
        index++;
    if (index < sensor->next_index)
        index = sensor->next_index;
    if (index >= MAX_INDEX)
        return PS_LIMIT;
    double next_time = c->start_time_s + (double)(index + 1) * period;
    if (!isfinite(next_time) || next_time <= time)
        return PS_NUMERIC;
    ps_rng noise_rng, dropout_rng;
    uint64_t indexed = mix(sensor->seed ^ mix(index + UINT64_C(0x9e3779b97f4a7c15)));
    ps_rng_seed(&noise_rng, mix(indexed ^ UINT64_C(0xa0761d6478bd642f)));
    ps_rng_seed(&dropout_rng, mix(indexed ^ UINT64_C(0xe7037ed1a0b428db)));
    double noise = 0, noise_mean = 0, noise_sd = 0;
    r = ps_distribution_sample(c->noise, &noise_rng, &noise);
    if (r == PS_OK)
        r = ps_distribution_moments(c->noise, &noise_mean, &noise_sd);
    if (r != PS_OK)
        return r;
    double value = ideal + c->offset + c->drift_per_s * elapsed + noise;
    if (!isfinite(value))
        return PS_NUMERIC;
    if (c->resolution)
        value -= remainder(value, c->resolution);
    double relative = fabs(ideal) * c->uncertainty_relative;
    double uncertainty =
        hypot(hypot(c->uncertainty_absolute, relative), hypot(noise_sd, c->resolution / sqrt(12)));
    if (!isfinite(value) || !isfinite(uncertainty))
        return PS_NUMERIC;
    reading.state = ps_rng_uniform(&dropout_rng) < c->dropout_probability ? PS_MEASUREMENT_DROPPED
                                                                          : PS_MEASUREMENT_VALID;
    if (reading.state == PS_MEASUREMENT_VALID) {
        reading.value.value = value;
        reading.standard_uncertainty = uncertainty;
    }
    reading.index = index;
    reading.skipped = index - sensor->next_index;
    sensor->next_index = index + 1;
    sensor->next_time_s = next_time;
    sensor->last_time_s = time;
    *out = reading;
    return PS_OK;
}
