#ifndef PHYSIM_LANGUAGE_MEASUREMENT_H
#define PHYSIM_LANGUAGE_MEASUREMENT_H
/* Experimental value bindings for generated C17; not a stable public ABI. */
#include "language_runtime.h"
#include "measurement.h"

static inline ps_distribution psrt_distribution(ps_distribution_kind kind, double a, double b,
                                                psrt_site site) {
    ps_distribution value = {kind, a, b};
    if (ps_distribution_validate(value) != PS_OK)
        psrt_fail(site, "Invalid distribution parameters");
    return value;
}
static inline ps_distribution psrt_distribution_constant(double value, psrt_site site) {
    return psrt_distribution(PS_DIST_CONSTANT, value, 0, site);
}
static inline ps_distribution psrt_distribution_uniform(double min, double max, psrt_site site) {
    return psrt_distribution(PS_DIST_UNIFORM, min, max, site);
}
static inline ps_distribution psrt_distribution_normal(double mean, double deviation,
                                                       psrt_site site) {
    return psrt_distribution(PS_DIST_NORMAL, mean, deviation, site);
}
static inline double psrt_distribution_mean(ps_distribution value, psrt_site site) {
    double mean, deviation;
    if (ps_distribution_moments(value, &mean, &deviation) != PS_OK)
        psrt_fail(site, "Distribution moments are not representable");
    return mean;
}
static inline double psrt_distribution_deviation(ps_distribution value, psrt_site site) {
    double mean, deviation;
    if (ps_distribution_moments(value, &mean, &deviation) != PS_OK)
        psrt_fail(site, "Distribution moments are not representable");
    return deviation;
}
static inline ps_sensor_config psrt_sensor_config(ps_unit unit, double rate_hz, double start_time,
                                                  double resolution, double offset, double drift,
                                                  ps_distribution noise, double dropout,
                                                  double uncertainty_absolute,
                                                  double uncertainty_relative, psrt_site site) {
    ps_sensor_config config = {
        unit,  rate_hz, start_time, resolution,           offset,
        drift, noise,   dropout,    uncertainty_absolute, uncertainty_relative};
    if (ps_sensor_config_validate(&config) != PS_OK)
        psrt_fail(site, "Invalid sensor configuration or numeric range");
    return config;
}
static inline ps_sensor psrt_sensor_init(ps_sensor_config config, int64_t seed, psrt_site site) {
    ps_sensor sensor;
    if (ps_sensor_init(&sensor, &config, (uint64_t)seed) != PS_OK)
        psrt_fail(site, "Invalid sensor configuration or numeric range");
    return sensor;
}
static inline void psrt_sensor_reset(ps_sensor *sensor, int64_t seed, psrt_site site) {
    if (ps_sensor_reset(sensor, (uint64_t)seed) != PS_OK)
        psrt_fail(site, "Sensor reset failed");
}
static inline double psrt_sensor_next_time(ps_sensor sensor, psrt_site site) {
    double time;
    if (ps_sensor_next_time(&sensor, &time) != PS_OK)
        psrt_fail(site, "Sensor next time is not representable");
    return time;
}
static inline ps_measurement psrt_sensor_read(ps_sensor *sensor, double time, ps_quantity truth,
                                              psrt_site site) {
    ps_measurement measurement;
    if (ps_sensor_read(sensor, time, truth, &measurement) != PS_OK)
        psrt_fail(site, "Sensor read failed: invalid time, dimensions or numeric range");
    return measurement;
}
static inline bool psrt_measurement_valid(ps_measurement value, psrt_site site) {
    (void)site;
    return value.state == PS_MEASUREMENT_VALID;
}
static inline bool psrt_measurement_due(ps_measurement value, psrt_site site) {
    (void)site;
    return value.state != PS_MEASUREMENT_NOT_DUE;
}
static inline bool psrt_measurement_dropped(ps_measurement value, psrt_site site) {
    (void)site;
    return value.state == PS_MEASUREMENT_DROPPED;
}
#endif
