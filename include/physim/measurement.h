#ifndef PHYSIM_MEASUREMENT_H
#define PHYSIM_MEASUREMENT_H
#include "units.h"

typedef enum { PS_DIST_CONSTANT, PS_DIST_UNIFORM, PS_DIST_NORMAL } ps_distribution_kind;
typedef struct {
    ps_distribution_kind kind;
    /* CONSTANT: a=value, b=0. UNIFORM: a=min, b=max. NORMAL: a=mean, b=stddev. */
    double a, b;
} ps_distribution;
ps_result ps_distribution_validate(ps_distribution distribution);
/* Caller seeds the RNG. Constant/degenerate distributions consume no draws.
 * Checked functions leave all outputs, RNGs and sensor state unchanged on error. */
ps_result ps_distribution_sample(ps_distribution distribution, ps_rng *rng, double *out);
ps_result ps_distribution_moments(ps_distribution distribution, double *mean, double *stddev);

typedef struct {
    ps_unit unit; /* Symbol is borrowed for the lifetime of sensor and samples. */
    double rate_hz, start_time_s;
    double resolution, offset, drift_per_s; /* In unit, unit, unit/second. */
    ps_distribution noise;                  /* Additive noise in unit. */
    double dropout_probability;             /* Independent Bernoulli trial per scheduled index. */
    double uncertainty_absolute, uncertainty_relative; /* Standard uncertainty components. */
} ps_sensor_config;
typedef enum {
    PS_MEASUREMENT_NOT_DUE,
    PS_MEASUREMENT_VALID,
    PS_MEASUREMENT_DROPPED
} ps_measurement_state;
typedef struct {
    ps_measurement_state state;
    ps_quantity value;
    double time_s, standard_uncertainty;
    uint64_t index, skipped;
} ps_measurement;
/* Caller-owned, allocation-free; treat fields as private and configure via init.
 * An instance is not thread-safe. Separate instances may be used concurrently. */
typedef struct {
    ps_sensor_config config;
    uint64_t seed, next_index;
    double next_time_s, last_time_s;
    uint32_t initialized;
} ps_sensor;
ps_result ps_sensor_config_validate(const ps_sensor_config *config);
ps_result ps_sensor_init(ps_sensor *sensor, const ps_sensor_config *config, uint64_t seed);
ps_result ps_sensor_reset(ps_sensor *sensor, uint64_t seed);
ps_result ps_sensor_next_time(const ps_sensor *sensor, double *time_s);
/* truth is the ideal signal at time_s, converted to the configured unit.
 * Calls before start/older than the last attempted sample are invalid. A call
 * before the next grid time returns NOT_DUE without advancing state. A late
 * call samples at its ACTUAL time and reports skipped grid slots; no fabricated
 * past values or interpolation. Slight FP grid mismatch is tolerated (8 ulps
 * in the time scale, capped at 1e-6 of the period). At most 2^52-1 grid indices.
 * Noise/dropout are indexed by seed and grid index, independent of call count.
 * VALID value = quantize(truth + offset + drift*(time-start) + noise).
 * Quantization rounds to the nearest multiple of resolution, ties to even.
 * Standard uncertainty combines supplied absolute/relative components, noise
 * stddev and resolution/sqrt(12) by RSS, assuming independent components and
 * uniform quantization error. Relative component uses abs(truth), not the noisy
 * output. Known biases are NOT uncertainty and are NOT corrected by this model.
 * NOT_DUE/DROPPED return zero numeric placeholders: never analyze as measurements.
 * No confidence/coverage probability is implied by standard_uncertainty. */
ps_result ps_sensor_read(ps_sensor *sensor, double time_s, ps_quantity truth, ps_measurement *out);
#endif
