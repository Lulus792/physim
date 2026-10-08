#ifndef PHYSIM_LANGUAGE_SDK_H
#define PHYSIM_LANGUAGE_SDK_H
#include "language_runtime.h"
#include "language_diagnostic.h"
#include "language_run_index.h"
#include "language_array.h"
#include "experiment.h"
#include "language_measurement.h"
#include "language_mechanics.h"
#include "language_thermodynamics.h"
#include "language_electromagnetism.h"
#include "language_waves_optics.h"
#include "language_fluid.h"
#include "language_properties.h"
#include "language_constraints.h"
#include "language_contact_world.h"
#include "language_batch.h"
#include "language_collision.h"
#include "language_matrix.h"
#include "math.h"
#include "measurement.h"
#include "mechanics.h"
#include "numerics.h"
#include "units.h"
#include <limits.h>

/* Older generated C did not carry these version macros. Preserve its historical
 * provenance when compiling it against a newer SDK. */
#ifndef PSRT_LANGUAGE_VERSION
#define PSRT_LANGUAGE_VERSION "0.1.0-dev"
#endif
#ifndef PSRT_COMPILER_VERSION
#define PSRT_COMPILER_VERSION "0.1.0-dev"
#endif

static inline ps_step_interval psrt_step_interval(double elapsed,double next,psrt_site site) {
    if(!isfinite(elapsed) || elapsed<=0 || !isfinite(next) || next<=0)
        psrt_fail(site,"StepInterval requires positive finite durations");
    return (ps_step_interval){elapsed,next};
}

typedef struct psrt_channel {
    ps_context *owner;
    uint32_t index;
} psrt_channel;
typedef struct psrt_host {
    ps_context *context;
    ps_scene *scene;
    unsigned phase;
} psrt_host;
enum { PSRT_CREATE = 1, PSRT_RESET, PSRT_STEP, PSRT_SCENE };
/* Explicit streams work in programs, experiments, and analyses. Failed draws
 * leave the caller's stream unchanged via ps_distribution_sample's copy/commit. */
static inline ps_rng psrt_rng_make(int64_t seed, psrt_site site) {
    (void)site;
    ps_rng rng;
    ps_rng_seed(&rng, (uint64_t)seed);
    return rng;
}
static inline double psrt_rng_sample(ps_rng *rng, ps_distribution distribution, psrt_site site) {
    double value;
    ps_result result = ps_distribution_sample(distribution, rng, &value);
    if (result == PS_INVALID)
        psrt_fail(site, "Invalid Rng sample parameters");
    if (result != PS_OK)
        psrt_fail(site, "Non-finite Rng sample");
    return value;
}
static inline void psrt_rng_reseed(ps_rng *rng, int64_t seed, psrt_site site) {
    (void)site;
    ps_rng_seed(rng, (uint64_t)seed);
}
static inline ps_rng psrt_rng_for_run(psrt_host *host, int64_t stream, psrt_site site) {
    (void)site;
    ps_rng rng;
    ps_rng_seed(&rng, host->context->seed ^ (uint64_t)stream);
    return rng;
}
static inline void psrt_rng_reseed_for_run(psrt_host *host, ps_rng *rng, int64_t stream,
                                           psrt_site site) {
    (void)site;
    ps_rng_seed(rng, host->context->seed ^ (uint64_t)stream);
}
static inline ps_sensor psrt_sensor_for_run(psrt_host *host, ps_sensor_config config,
                                            int64_t stream, psrt_site site) {
    ps_sensor sensor;
    if (ps_sensor_init(&sensor, &config, host->context->seed ^ (uint64_t)stream) != PS_OK)
        psrt_fail(site, "Invalid sensor configuration or numeric range");
    return sensor;
}
static inline void psrt_sensor_reset_for_run(psrt_host *host, ps_sensor *sensor, int64_t stream,
                                             psrt_site site) {
    if (ps_sensor_reset(sensor, host->context->seed ^ (uint64_t)stream) != PS_OK)
        psrt_fail(site, "Sensor reset failed");
}
static inline double psrt_random(psrt_host *host, ps_distribution distribution, psrt_site site) {
    if (host->phase != PSRT_RESET && host->phase != PSRT_STEP)
        psrt_fail(site, "Random sampling requires reset or step callback");
    double value;
    ps_result result = ps_distribution_sample(distribution, &host->context->rng, &value);
    if (result == PS_INVALID)
        psrt_fail(site, "Invalid random distribution parameters");
    if (result != PS_OK)
        psrt_fail(site, "Non-finite random sample");
    return value;
}
static inline double psrt_random_uniform(psrt_host *host, double min, double max, psrt_site site) {
    return psrt_random(host, (ps_distribution){PS_DIST_UNIFORM, min, max}, site);
}
static inline double psrt_random_normal(psrt_host *host, double mean, double standard_deviation,
                                        psrt_site site) {
    return psrt_random(host, (ps_distribution){PS_DIST_NORMAL, mean, standard_deviation}, site);
}
static inline double psrt_sin(double x, psrt_site site) { return psrt_finite(sin(x), site); }
static inline double psrt_cos(double x, psrt_site site) { return psrt_finite(cos(x), site); }
static inline double psrt_tan(double x, psrt_site site) { return psrt_finite(tan(x), site); }
static inline double psrt_asin(double x, psrt_site site) { return psrt_finite(asin(x), site); }
static inline double psrt_acos(double x, psrt_site site) { return psrt_finite(acos(x), site); }
static inline double psrt_atan(double x, psrt_site site) { return psrt_finite(atan(x), site); }
static inline double psrt_atan2(double y, double x, psrt_site site) {
    if (x == 0.0 && y == 0.0)
        psrt_fail(site, "atan2 requires a nonzero x or y");
    return psrt_finite(atan2(y, x), site);
}
static inline double psrt_expm1(double x, psrt_site site) { return psrt_finite(expm1(x), site); }
static inline double psrt_exp(double x, psrt_site site) { return psrt_finite(exp(x), site); }
static inline double psrt_log(double x, psrt_site site) { return psrt_finite(log(x), site); }
static inline double psrt_log10(double x, psrt_site site) { return psrt_finite(log10(x), site); }
static inline double psrt_pow(double x, double y, psrt_site site) {
    return psrt_finite(pow(x, y), site);
}
static inline double psrt_hypot(double x, double y, psrt_site site) {
    return psrt_finite(hypot(x, y), site);
}
static inline double psrt_sqrt(double x, psrt_site site) { return psrt_finite(sqrt(x), site); }
static inline double psrt_abs(double x, psrt_site site) { return psrt_finite(fabs(x), site); }
static inline double psrt_floor(double x, psrt_site site) { return psrt_finite(floor(x), site); }
static inline double psrt_ceil(double x, psrt_site site) { return psrt_finite(ceil(x), site); }
static inline double psrt_round(double x, psrt_site site) { return psrt_finite(round(x), site); }
static inline double psrt_min(double left, double right, psrt_site site) {
    left = psrt_finite(left, site);
    right = psrt_finite(right, site);
    return left <= right ? left : right;
}
static inline double psrt_max(double left, double right, psrt_site site) {
    left = psrt_finite(left, site);
    right = psrt_finite(right, site);
    return left >= right ? left : right;
}
static inline double psrt_clamp(double value, double lower, double upper, psrt_site site) {
    value = psrt_finite(value, site);
    lower = psrt_finite(lower, site);
    upper = psrt_finite(upper, site);
    if (lower > upper)
        psrt_fail(site, "Clamp lower bound exceeds upper bound");
    return value < lower ? lower : value > upper ? upper : value;
}
static inline psrt_array psrt_linear_solve(ps_allocator allocator, const double *coefficients,
                                           size_t coefficient_count, const double *rhs,
                                           size_t rhs_count, double pivot_tolerance,
                                           psrt_site site) {
    if (!rhs_count || rhs_count > PS_NUMERIC_MAX_DIMENSION ||
        coefficient_count != rhs_count * rhs_count)
        psrt_fail(site, "Linear solve requires an n by n matrix and 1 to 32 right-hand values");
    double values[PS_NUMERIC_MAX_DIMENSION];
    ps_result result = ps_linear_solve(coefficients, rhs, rhs_count, pivot_tolerance, values);
    if (result == PS_INVALID)
        psrt_fail(site, "Invalid linear solve values or pivot tolerance");
    if (result == PS_SINGULAR)
        psrt_fail(site, "Linear system is singular at the chosen pivot tolerance");
    if (result != PS_OK)
        psrt_fail(site, "Linear solve exceeds numeric range");
    static const psrt_element_type element = {sizeof(double), NULL, NULL};
    psrt_array answer;
    if (psrt_array_init(&element, allocator, 0, &answer) != PS_OK ||
        psrt_array_replace(&answer, 0, 0, values, rhs_count) != PS_OK)
        psrt_fail(site, "Linear solve result exceeds array memory budget");
    return answer;
}
static inline ps_scalar_report psrt_scalar_search_reported(ps_scalar_fn function, void *user,
                                                            double lower, double upper,
                                                            double absolute_tolerance,
                                                            double relative_tolerance,
                                                            int64_t max_iterations,
                                                            bool minimize, psrt_site site) {
    if (max_iterations < 1 || max_iterations > 100000)
        psrt_fail(site, "Scalar search requires 1 to 100000 iterations");
    ps_scalar_report report;
    ps_result result = minimize
        ? ps_minimize_golden(function, user, lower, upper, absolute_tolerance,
                             relative_tolerance, (unsigned)max_iterations, &report)
        : ps_root_bisect(function, user, lower, upper, absolute_tolerance,
                         relative_tolerance, (unsigned)max_iterations, &report);
    if (result == PS_INVALID)
        psrt_fail(site, minimize ? "Invalid golden-section bounds or tolerances"
                                  : "Invalid bisection bracket or tolerances");
    if (result == PS_NUMERIC)
        psrt_fail(site, "Scalar callback returned a non-finite result");
    if (result == PS_LIMIT)
        psrt_fail(site, "Scalar search did not converge within maxIterations");
    if (result != PS_OK)
        psrt_fail(site, "Scalar search failed");
    psrt_finite(report.x, site);
    psrt_finite(report.value, site);
    return report;
}
static inline double psrt_scalar_search(ps_scalar_fn function, void *user, double lower,
                                        double upper, double absolute_tolerance,
                                        double relative_tolerance, int64_t max_iterations,
                                        bool minimize, psrt_site site) {
    return psrt_scalar_search_reported(function, user, lower, upper, absolute_tolerance,
                                       relative_tolerance, max_iterations, minimize, site).x;
}
static inline double psrt_root_bisect(ps_scalar_fn function, void *user, double lower,
                                      double upper, double absolute_tolerance,
                                      double relative_tolerance, int64_t max_iterations,
                                      psrt_site site) {
    return psrt_scalar_search(function, user, lower, upper, absolute_tolerance,
                              relative_tolerance, max_iterations, false, site);
}
static inline double psrt_minimize_golden(ps_scalar_fn function, void *user, double lower,
                                          double upper, double absolute_tolerance,
                                          double relative_tolerance, int64_t max_iterations,
                                          psrt_site site) {
    return psrt_scalar_search(function, user, lower, upper, absolute_tolerance,
                              relative_tolerance, max_iterations, true, site);
}
static inline ps_scalar_report psrt_root_bisect_reported(
    ps_scalar_fn function, void *user, double lower, double upper,
    double absolute_tolerance, double relative_tolerance, int64_t max_iterations,
    psrt_site site) {
    return psrt_scalar_search_reported(function, user, lower, upper, absolute_tolerance,
                                       relative_tolerance, max_iterations, false, site);
}
static inline ps_scalar_report psrt_minimize_golden_reported(
    ps_scalar_fn function, void *user, double lower, double upper,
    double absolute_tolerance, double relative_tolerance, int64_t max_iterations,
    psrt_site site) {
    return psrt_scalar_search_reported(function, user, lower, upper, absolute_tolerance,
                                       relative_tolerance, max_iterations, true, site);
}
typedef struct psrt_ode_callback_context {
    ps_allocator allocator;
    size_t count;
    psrt_site site;
    void *module_state;
    bool acceleration;
} psrt_ode_callback_context;
static inline psrt_array psrt_ode_result(ps_allocator allocator, const double *values,
                                         size_t count, psrt_site site) {
    static const psrt_element_type element = {sizeof(double), NULL, NULL};
    psrt_array answer;
    ps_result result = psrt_array_init(&element, allocator, 0, &answer);
    if (result != PS_OK)
        psrt_fail(site, "ODE result exceeds array memory budget");
    result = psrt_array_replace(&answer, 0, 0, values, count);
    if (result != PS_OK) {
        psrt_array_destroy(&answer);
        psrt_fail(site, "ODE result exceeds array memory budget");
    }
    return answer;
}
typedef struct psrt_ode_result_value {
    psrt_array state;
    int64_t accepted_steps, rejected_steps, evaluations;
    double reached_time, next_step, error_norm;
} psrt_ode_result_value;
static inline void psrt_ode_result_drop(void *value) {
    psrt_ode_result_value *result = value;
    psrt_array_destroy(&result->state);
    memset(result, 0, sizeof *result);
}
static inline ps_result psrt_ode_result_copy(void *destination, const void *source) {
    const psrt_ode_result_value *original = source;
    psrt_ode_result_value copy = *original;
    ps_result status = psrt_array_clone(&original->state, &copy.state);
    if (status == PS_OK)
        *(psrt_ode_result_value *)destination = copy;
    return status;
}
static inline void psrt_ode_result_keep(psrt_ode_result_value *value, psrt_site site) {
    psrt_ode_result_value copy;
    if (psrt_ode_result_copy(&copy, value) != PS_OK)
        psrt_fail(site, "ODE result ownership limit exceeded");
    *value = copy;
}
static inline psrt_array psrt_ode_step(ps_allocator allocator, ps_integrator method,
                                       ps_ode_fn derivative, void *user,
                                       const double *state, size_t count,
                                       double time, double dt, psrt_site site) {
    if (!count || count > PS_NUMERIC_MAX_DIMENSION || !state)
        psrt_fail(site, "ODE step requires 1 to 32 state values");
    double values[PS_NUMERIC_MAX_DIMENSION];
    memcpy(values, state, count * sizeof(double));
    ps_result result = ps_ode_step(method, derivative, user, time, dt, values, count);
    if (result == PS_INVALID)
        psrt_fail(site, "Invalid ODE state, time or positive step size");
    if (result != PS_OK)
        psrt_fail(site, "ODE step produced a non-finite value");
    return psrt_ode_result(allocator, values, count, site);
}
static inline psrt_array psrt_ode_euler(ps_allocator allocator, ps_ode_fn derivative,
                                        void *user, const double *state, size_t count,
                                        double time, double dt, psrt_site site) {
    return psrt_ode_step(allocator, PS_EULER, derivative, user, state, count, time, dt, site);
}
static inline psrt_array psrt_ode_rk4(ps_allocator allocator, ps_ode_fn derivative,
                                      void *user, const double *state, size_t count,
                                      double time, double dt, psrt_site site) {
    return psrt_ode_step(allocator, PS_RK4, derivative, user, state, count, time, dt, site);
}
static inline psrt_array psrt_ode_rk45_options(ps_allocator allocator, ps_ode_fn derivative,
                                               void *user, const double *state, size_t count,
                                               double start, double end, int64_t max_steps,
                                               ps_ode_options options, psrt_site site) {
    if (!count || count > PS_NUMERIC_MAX_DIMENSION || !state)
        psrt_fail(site, "RK45 integration requires 1 to 32 state values");
    if (max_steps <= 0 || max_steps > UINT32_MAX / 7)
        psrt_fail(site, "RK45 trial step budget is out of range");
    double values[PS_NUMERIC_MAX_DIMENSION];
    memcpy(values, state, count * sizeof(double));
    options.maximum_steps = (unsigned)max_steps;
    ps_ode_diagnostic diagnostic;
    ps_result result = ps_ode_integrate_diagnosed(derivative, user, start, end,
                                                  values, count, &options, NULL, &diagnostic);
    if (result != PS_OK)
        psrt_fail(site, ps_ode_diagnostic_string(diagnostic.reason));
    return psrt_ode_result(allocator, values, count, site);
}
static inline psrt_array psrt_ode_rk45(ps_allocator allocator, ps_ode_fn derivative,
                                       void *user, const double *state, size_t count,
                                       double start, double end, double absolute_tolerance,
                                       double relative_tolerance, int64_t max_steps,
                                       psrt_site site) {
    ps_ode_options options = ps_ode_options_default();
    options.absolute_tolerance = absolute_tolerance;
    options.relative_tolerance = relative_tolerance;
    return psrt_ode_rk45_options(allocator, derivative, user, state, count,
                                 start, end, max_steps, options, site);
}
static inline psrt_array psrt_ode_rk45_with_steps(ps_allocator allocator,
                                                  ps_ode_fn derivative, void *user,
                                                  const double *state, size_t count,
                                                  double start, double end,
                                                  double absolute_tolerance,
                                                  double relative_tolerance, int64_t max_steps,
                                                  double initial_step, double minimum_step,
                                                  double maximum_step, psrt_site site) {
    ps_ode_options options = ps_ode_options_default();
    options.absolute_tolerance = absolute_tolerance;
    options.relative_tolerance = relative_tolerance;
    options.initial_step = initial_step;
    options.minimum_step = minimum_step;
    options.maximum_step = maximum_step;
    return psrt_ode_rk45_options(allocator, derivative, user, state, count,
                                 start, end, max_steps, options, site);
}
static inline psrt_array psrt_ode_rk45_with_tolerances(
    ps_allocator allocator, ps_ode_fn derivative, void *user,
    const double *state, size_t count, double start, double end,
    const double *absolute_tolerances, size_t tolerance_count,
    double relative_tolerance, int64_t max_steps,
    double initial_step, double minimum_step, double maximum_step,
    psrt_site site) {
    if (!count || count > PS_NUMERIC_MAX_DIMENSION || !absolute_tolerances ||
        tolerance_count != count)
        psrt_fail(site, "RK45 component tolerances require one value per state component (1 to 32)");
    double tolerance_snapshot[PS_NUMERIC_MAX_DIMENSION];
    memcpy(tolerance_snapshot, absolute_tolerances, count * sizeof(double));
    ps_ode_options options = ps_ode_options_default();
    options.component_absolute_tolerance = tolerance_snapshot;
    options.relative_tolerance = relative_tolerance;
    options.initial_step = initial_step;
    options.minimum_step = minimum_step;
    options.maximum_step = maximum_step;
    return psrt_ode_rk45_options(allocator, derivative, user, state, count,
                                 start, end, max_steps, options, site);
}
static inline psrt_ode_result_value psrt_ode_rk45_reported_options(
    ps_allocator allocator, ps_ode_fn derivative, void *user,
    const double *state, size_t count, double start, double end,
    int64_t max_steps, ps_ode_options options, bool single, psrt_site site) {
    if (!count || count > PS_NUMERIC_MAX_DIMENSION || !state)
        psrt_fail(site, "RK45 integration requires 1 to 32 state values");
    if (max_steps <= 0 || max_steps > UINT32_MAX / 7)
        psrt_fail(site, "RK45 trial step budget is out of range");
    double values[PS_NUMERIC_MAX_DIMENSION];
    memcpy(values, state, count * sizeof(double));
    options.maximum_steps = (unsigned)max_steps;
    ps_ode_report report;
    ps_ode_diagnostic diagnostic;
    ps_result status = (single ? ps_ode_step_diagnosed : ps_ode_integrate_diagnosed)(
        derivative,user,start,end,values,count,&options,&report,&diagnostic);
    if (status != PS_OK)
        psrt_fail(site, ps_ode_diagnostic_string(diagnostic.reason));
    psrt_ode_result_value result = {
        psrt_ode_result(allocator, values, count, site),
        (int64_t)report.accepted_steps, (int64_t)report.rejected_steps,
        (int64_t)report.evaluations, report.reached_time,
        report.next_step, report.error_norm
    };
    return result;
}
static inline psrt_ode_result_value psrt_ode_rk45_reported(
    ps_allocator allocator, ps_ode_fn derivative, void *user,
    const double *state, size_t count, double start, double end,
    double absolute_tolerance, double relative_tolerance, int64_t max_steps,
    double initial_step, double minimum_step, double maximum_step,
    psrt_site site) {
    ps_ode_options options = ps_ode_options_default();
    options.absolute_tolerance = absolute_tolerance;
    options.relative_tolerance = relative_tolerance;
    options.initial_step = initial_step;
    options.minimum_step = minimum_step;
    options.maximum_step = maximum_step;
    return psrt_ode_rk45_reported_options(allocator, derivative, user, state, count,
                                           start, end, max_steps, options, false, site);
}
static inline psrt_ode_result_value psrt_ode_rk45_step_reported(
    ps_allocator allocator, ps_ode_fn derivative, void *user,
    const double *state, size_t count, double start, double end,
    double absolute_tolerance, double relative_tolerance, int64_t max_steps,
    double initial_step, double minimum_step, double maximum_step,
    psrt_site site) {
    ps_ode_options options = ps_ode_options_default();
    options.absolute_tolerance = absolute_tolerance;
    options.relative_tolerance = relative_tolerance;
    options.initial_step = initial_step;
    options.minimum_step = minimum_step;
    options.maximum_step = maximum_step;
    return psrt_ode_rk45_reported_options(allocator, derivative, user, state, count,
                                           start, end, max_steps, options, true, site);
}
static inline psrt_ode_result_value psrt_ode_rk45_with_tolerances_reported(
    ps_allocator allocator, ps_ode_fn derivative, void *user,
    const double *state, size_t count, double start, double end,
    const double *absolute_tolerances, size_t tolerance_count,
    double relative_tolerance, int64_t max_steps,
    double initial_step, double minimum_step, double maximum_step,
    psrt_site site) {
    if (!count || count > PS_NUMERIC_MAX_DIMENSION || !absolute_tolerances ||
        tolerance_count != count)
        psrt_fail(site, "RK45 component tolerances require one value per state component (1 to 32)");
    double tolerance_snapshot[PS_NUMERIC_MAX_DIMENSION];
    memcpy(tolerance_snapshot, absolute_tolerances, count * sizeof(double));
    ps_ode_options options = ps_ode_options_default();
    options.component_absolute_tolerance = tolerance_snapshot;
    options.relative_tolerance = relative_tolerance;
    options.initial_step = initial_step;
    options.minimum_step = minimum_step;
    options.maximum_step = maximum_step;
    return psrt_ode_rk45_reported_options(allocator, derivative, user, state, count,
                                           start, end, max_steps, options, false, site);
}
static inline psrt_array psrt_verlet_step(ps_allocator allocator,
                                         ps_acceleration_fn acceleration, void *user,
                                         const double *phase, size_t count,
                                         double time, double dt, psrt_site site) {
    if (count < 2 || count > 2 * PS_NUMERIC_MAX_DIMENSION || (count & 1) || !phase)
        psrt_fail(site, "Verlet phase requires paired position and velocity values (1 to 32 each)");
    size_t dimension = count / 2;
    double values[2 * PS_NUMERIC_MAX_DIMENSION];
    memcpy(values, phase, count * sizeof(double));
    ps_result result = ps_verlet_step(acceleration, user, time, dt,
                                     values, values + dimension, dimension);
    if (result == PS_INVALID)
        psrt_fail(site, "Invalid Verlet phase, time or nonzero step size");
    if (result != PS_OK)
        psrt_fail(site, "Verlet step produced a non-finite value");
    return psrt_ode_result(allocator, values, count, site);
}
static inline ps_vec2 psrt_vec2(double x, double y, psrt_site site) {
    return (ps_vec2){psrt_finite(x, site), psrt_finite(y, site)};
}
static inline ps_vec3 psrt_vec3(double x, double y, double z, psrt_site site) {
    return ps_v3(psrt_finite(x, site), psrt_finite(y, site), psrt_finite(z, site));
}
static inline ps_vec4 psrt_vec4(double x, double y, double z, double w, psrt_site site) {
    return ps_v4(psrt_finite(x, site), psrt_finite(y, site), psrt_finite(z, site),
                 psrt_finite(w, site));
}
static inline ps_medium psrt_medium_make(double density, double viscosity, psrt_site site) {
    if (!isfinite(density) || !isfinite(viscosity) || density < 0 || viscosity < 0)
        psrt_fail(site, "Medium density and viscosity must be finite and nonnegative");
    return (ps_medium){density, viscosity, "custom medium"};
}
static inline ps_medium psrt_medium_air(psrt_site site) {
    (void)site;
    return PS_AIR;
}
static inline ps_medium psrt_medium_water(psrt_site site) {
    (void)site;
    return PS_WATER;
}
static inline ps_medium psrt_medium_vacuum(psrt_site site) {
    (void)site;
    return PS_VACUUM;
}
static inline ps_vec3 psrt_medium_drag_force(ps_medium medium, ps_vec3 velocity,
                                              double coefficient, double area, psrt_site site) {
    if (!isfinite(medium.density_kg_m3) || medium.density_kg_m3 < 0 ||
        !isfinite(medium.viscosity_pa_s) || medium.viscosity_pa_s < 0 ||
        !isfinite(velocity.x) || !isfinite(velocity.y) || !isfinite(velocity.z) ||
        !isfinite(coefficient) || coefficient < 0 || !isfinite(area) || area < 0)
        psrt_fail(site, "Invalid medium drag parameters");
    if (medium.density_kg_m3 == 0 || coefficient == 0 || area == 0)
        return ps_v3(0, 0, 0);
    ps_vec3 force = ps_drag_force(velocity, medium, coefficient, area);
    return psrt_vec3(force.x, force.y, force.z, site);
}
static inline ps_bezier3 psrt_bezier3(ps_vec3 start, ps_vec3 control1, ps_vec3 control2,
                                     ps_vec3 end, psrt_site site) {
    return (ps_bezier3){{psrt_vec3(start.x, start.y, start.z, site),
                         psrt_vec3(control1.x, control1.y, control1.z, site),
                         psrt_vec3(control2.x, control2.y, control2.z, site),
                         psrt_vec3(end.x, end.y, end.z, site)}};
}
static inline ps_vec3 psrt_bezier_control_point(ps_bezier3 curve, int64_t index, psrt_site site) {
    if (index < 0 || index >= 4)
        psrt_fail_code(site, PS_INVALID, "Bezier control point index must be in [0, 3]");
    return curve.points[index];
}
static inline ps_curve_sample3 psrt_bezier_sample(ps_bezier3 curve, double t, psrt_site site) {
    ps_curve_sample3 sample;
    ps_result result = ps_bezier3_evaluate(&curve, t, &sample);
    if (result == PS_INVALID)
        psrt_fail(site, "Bezier parameter must be between 0 and 1");
    if (result != PS_OK)
        psrt_fail(site, "Bezier evaluation exceeds numeric range");
    return sample;
}
static inline ps_vec3 psrt_bezier_position(ps_bezier3 curve, double t, psrt_site site) {
    return psrt_bezier_sample(curve, t, site).position;
}
static inline ps_vec3 psrt_bezier_tangent(ps_bezier3 curve, double t, psrt_site site) {
    return psrt_bezier_sample(curve, t, site).tangent;
}
static inline ps_bezier3 psrt_bezier_split(ps_bezier3 curve, double t, bool right,
                                          psrt_site site) {
    ps_bezier3 left, right_curve;
    if (ps_bezier3_split(&curve, t, &left, &right_curve) != PS_OK)
        psrt_fail(site, "Bezier parameter must be between 0 and 1");
    return right ? right_curve : left;
}
static inline ps_bezier3 psrt_bezier_split_left(ps_bezier3 curve, double t, psrt_site site) {
    return psrt_bezier_split(curve, t, false, site);
}
static inline ps_bezier3 psrt_bezier_split_right(ps_bezier3 curve, double t, psrt_site site) {
    return psrt_bezier_split(curve, t, true, site);
}
static inline double psrt_dot2(ps_vec2 a, ps_vec2 b, psrt_site site) {
    return psrt_finite(ps_v2dot(a, b), site);
}
static inline double psrt_dot3(ps_vec3 a, ps_vec3 b, psrt_site site) {
    return psrt_finite(ps_vdot(a, b), site);
}
static inline double psrt_dot4(ps_vec4 a, ps_vec4 b, psrt_site site) {
    return psrt_finite(ps_v4dot(a, b), site);
}
static inline double psrt_cross2(ps_vec2 a, ps_vec2 b, psrt_site site) {
    return psrt_finite(ps_v2cross(a, b), site);
}
static inline ps_vec3 psrt_cross3(ps_vec3 a, ps_vec3 b, psrt_site site) {
    ps_vec3 v = ps_vcross(a, b);
    return psrt_vec3(v.x, v.y, v.z, site);
}
static inline double psrt_length2(ps_vec2 v, psrt_site site) {
    return psrt_finite(ps_v2length(v), site);
}
static inline double psrt_length3(ps_vec3 v, psrt_site site) {
    return psrt_finite(ps_vlength(v), site);
}
static inline double psrt_length4(ps_vec4 v, psrt_site site) {
    return psrt_finite(ps_v4length(v), site);
}
static inline ps_vec2 psrt_normalize2(ps_vec2 v, psrt_site site) {
    ps_vec2 result = ps_v2normalize(v);
    return psrt_vec2(result.x, result.y, site);
}
static inline ps_vec3 psrt_normalize3(ps_vec3 v, psrt_site site) {
    ps_vec3 result = ps_vnormalize(v);
    return psrt_vec3(result.x, result.y, result.z, site);
}
static inline ps_vec4 psrt_normalize4(ps_vec4 v, psrt_site site) {
    ps_vec4 result = ps_v4normalize(v);
    return psrt_vec4(result.x, result.y, result.z, result.w, site);
}
static inline void psrt_force_result(ps_result result, psrt_site site) {
    if (result == PS_INVALID)
        psrt_fail(site, "Invalid force parameters");
    if (result == PS_SINGULAR)
        psrt_fail(site, "Spring endpoints coincide");
    if (result != PS_OK)
        psrt_fail(site, "Non-finite force result");
}
static inline ps_vec3 psrt_spring_force(ps_vec3 position, ps_vec3 velocity, ps_vec3 anchor,
                                        ps_vec3 anchor_velocity, double stiffness,
                                        double rest_length, double damping, psrt_site site) {
    ps_vec3 force;
    psrt_force_result(ps_spring_force(position, velocity, anchor, anchor_velocity, stiffness,
                                      rest_length, damping, &force),
                      site);
    return force;
}
static inline ps_vec3 psrt_stokes_drag(ps_vec3 velocity, double viscosity, double radius,
                                       psrt_site site) {
    ps_vec3 force;
    ps_medium medium = {0};
    medium.viscosity_pa_s = viscosity;
    psrt_force_result(ps_sphere_drag(velocity, medium, PS_DRAG_STOKES, radius, 0, &force), site);
    return force;
}
static inline ps_vec3 psrt_medium_stokes_drag(ps_medium medium, ps_vec3 velocity,
                                               double radius, psrt_site site) {
    ps_vec3 force;
    psrt_force_result(ps_sphere_drag(velocity, medium, PS_DRAG_STOKES, radius, 0, &force), site);
    return force;
}
static inline ps_vec3 psrt_quadratic_drag(ps_vec3 velocity, double density, double radius,
                                          double coefficient, psrt_site site) {
    ps_vec3 force;
    ps_medium medium = {0};
    medium.density_kg_m3 = density;
    psrt_force_result(
        ps_sphere_drag(velocity, medium, PS_DRAG_QUADRATIC, radius, coefficient, &force), site);
    return force;
}
static inline ps_vec3 psrt_buoyancy_force(double density, double volume, ps_vec3 gravity,
                                          psrt_site site) {
    ps_vec3 force;
    psrt_force_result(ps_buoyancy_force(density, volume, gravity, &force), site);
    return force;
}
static inline ps_submersion psrt_sphere_submersion(double radius, double center_height,
                                                  psrt_site site) {
    ps_submersion result;
    ps_result code = ps_sphere_submersion(radius, center_height, &result);
    if (code != PS_OK)
        psrt_fail(site, code == PS_NUMERIC ? "Sphere submersion exceeds numeric range"
                                           : "Sphere submersion requires finite height and positive radius");
    return result;
}
static inline bool psrt_is_close(double left, double right, double absolute, double relative,
                                 psrt_site site) {
    (void)site;
    return ps_close(left, right, absolute, relative);
}
static inline ps_quat psrt_quat(double x, double y, double z, double w, psrt_site site) {
    return (ps_quat){psrt_finite(x, site), psrt_finite(y, site), psrt_finite(z, site),
                     psrt_finite(w, site)};
}
static inline ps_quat psrt_rotation(ps_quat q, psrt_site site) {
    ps_quat normalized;
    if (ps_quat_normalize(q, &normalized) != PS_OK)
        psrt_fail(site, "Rotation requires a finite nonzero quaternion");
    return normalized;
}
static inline ps_quat psrt_quat_conjugate(ps_quat q, psrt_site site) {
    ps_quat result = ps_quat_conjugate(q);
    return psrt_quat(result.x, result.y, result.z, result.w, site);
}
static inline ps_quat psrt_quat_multiply(ps_quat left, ps_quat right, psrt_site site) {
    ps_quat result = ps_quat_multiply(left, right);
    return psrt_quat(result.x, result.y, result.z, result.w, site);
}
static inline ps_quat psrt_quat_slerp(ps_quat start, ps_quat end, double fraction, psrt_site site) {
    if (!(fraction >= 0 && fraction <= 1))
        psrt_fail(site, "Rotation interpolation fraction must be in [0, 1]");
    /* Validate endpoints with the same diagnostic as rotate and normalizeQuat. */
    start = psrt_rotation(start, site);
    end = psrt_rotation(end, site);
    ps_quat result;
    if (ps_quat_slerp(start, end, fraction, &result) != PS_OK)
        psrt_fail(site, "Invalid rotation interpolation result");
    return psrt_quat(result.x, result.y, result.z, result.w, site);
}
static inline ps_quat psrt_axis_angle(ps_vec3 axis, double angle, psrt_site site) {
    ps_quat q = ps_quat_axis_angle(axis, angle);
    return psrt_quat(q.x, q.y, q.z, q.w, site);
}
static inline ps_vec3 psrt_rotate(ps_quat rotation, ps_vec3 vector, psrt_site site) {
    ps_vec3 v = ps_quat_rotate(psrt_rotation(rotation, site), vector);
    return psrt_vec3(v.x, v.y, v.z, site);
}
static inline ps_vec2 psrt_symplectic(ps_vec2 state, double acceleration, double dt,
                                      psrt_site site) {
    if (!(dt > 0))
        psrt_fail(site, "Integrator step must be positive");
    ps_symplectic_step(&state.x, &state.y, acceleration, dt);
    return psrt_vec2(state.x, state.y, site);
}
static inline ps_unit psrt_unit(int64_t l, int64_t m, int64_t t, int64_t i, int64_t k, int64_t n,
                                int64_t j, double scale, const char *symbol, psrt_site site) {
    int64_t exponents[] = {l, m, t, i, k, n, j};
    ps_unit value = {{0}, scale, symbol};
    for (unsigned axis = 0; axis < 7; axis++) {
        if (exponents[axis] < INT8_MIN || exponents[axis] > INT8_MAX)
            psrt_fail(site, "Unit exponent exceeds Int8 range");
        value.dimension[axis] = (int8_t)exponents[axis];
    }
    if (!ps_unit_valid(value))
        psrt_fail(site, "Invalid unit");
    return value;
}
static inline void psrt_unit_result(ps_result result, psrt_site site) {
    if (result == PS_INVALID)
        psrt_fail(site, "Unit exponent exceeds Int8 range or invalid unit");
    if (result != PS_OK)
        psrt_fail(site, "Unit scale is not positive and finite");
}
static inline ps_unit psrt_unit_multiply(ps_unit left, ps_unit right, const char *symbol,
                                        psrt_site site) {
    ps_unit result;
    psrt_unit_result(ps_unit_multiply(left, right, symbol, &result), site);
    return result;
}
static inline ps_unit psrt_unit_divide(ps_unit left, ps_unit right, const char *symbol,
                                      psrt_site site) {
    ps_unit result;
    psrt_unit_result(ps_unit_divide(left, right, symbol, &result), site);
    return result;
}
static inline ps_unit psrt_unit_power(ps_unit unit, int64_t exponent, const char *symbol,
                                     psrt_site site) {
    if (exponent < INT_MIN || exponent > INT_MAX)
        psrt_fail(site, "Unit power exceeds native integer range");
    ps_unit result;
    psrt_unit_result(ps_unit_power(unit, (int)exponent, symbol, &result), site);
    return result;
}
static inline bool psrt_unit_compatible(ps_unit left, ps_unit right, psrt_site site) {
    (void)site;
    return ps_unit_compatible(left, right);
}
static inline ps_quantity psrt_quantity(double value, ps_unit unit, psrt_site site) {
    if (!ps_unit_valid(unit))
        psrt_fail(site, "Invalid quantity unit");
    return (ps_quantity){psrt_finite(value, site), unit};
}
static inline void psrt_quantity_result(ps_result result, psrt_site site) {
    if (result == PS_INVALID)
        psrt_fail(site, "Invalid quantity operation: incompatible units, zero divisor or exponent overflow");
    if (result != PS_OK)
        psrt_fail(site, "Quantity result or scale is not representable");
}
static inline ps_quantity psrt_quantity_convert(ps_quantity value, ps_unit target, psrt_site site) {
    ps_quantity result;
    psrt_quantity_result(ps_quantity_convert(value, target, &result), site);
    return result;
}
static inline ps_quantity psrt_quantity_add(ps_quantity left, ps_quantity right, psrt_site site) {
    ps_quantity result;
    psrt_quantity_result(ps_quantity_add(left, right, &result), site);
    return result;
}
static inline ps_quantity psrt_quantity_subtract(ps_quantity left, ps_quantity right, psrt_site site) {
    ps_quantity result;
    psrt_quantity_result(ps_quantity_subtract(left, right, &result), site);
    return result;
}
static inline ps_quantity psrt_quantity_scale(ps_quantity value, double factor, psrt_site site) {
    return psrt_quantity(value.value * factor, value.unit, site);
}
static inline ps_quantity psrt_quantity_divide_scalar(ps_quantity value, double divisor,
                                                       psrt_site site) {
    return psrt_quantity(psrt_fdiv(value.value, divisor, site), value.unit, site);
}
static inline ps_quantity psrt_quantity_negate(ps_quantity value) {
    value.value = -value.value;
    return value;
}
static inline ps_quantity psrt_quantity_multiply(ps_quantity left, ps_quantity right,
                                                const char *symbol, psrt_site site) {
    ps_quantity result;
    psrt_quantity_result(ps_quantity_multiply(left, right, symbol, &result), site);
    return result;
}
static inline ps_quantity psrt_quantity_divide(ps_quantity left, ps_quantity right,
                                              const char *symbol, psrt_site site) {
    ps_quantity result;
    psrt_quantity_result(ps_quantity_divide(left, right, symbol, &result), site);
    return result;
}
static inline double psrt_convert(double value, ps_unit from, ps_unit to, psrt_site site) {
    double result;
    if (ps_convert(value, from, to, &result) != PS_OK)
        psrt_fail(site, "Incompatible unit conversion");
    return psrt_finite(result, site);
}
static inline psrt_channel psrt_add_channel(psrt_host *host, const char *name, ps_unit unit,
                                            const char *description, psrt_site site) {
    ps_context *c = host->context;
    if (host->phase != PSRT_CREATE)
        psrt_fail(site, "Channels can only be declared during create");
    int index = ps_channel_add(c, name, unit, description);
    if (index < 0)
        psrt_fail(site, "Invalid channel declaration: canonical SI unit, bounded UTF-8 metadata, unique name and free capacity required");
    return (psrt_channel){c, (uint32_t)index};
}
static inline void psrt_sample(psrt_host *host, psrt_channel channel, double value,
                               psrt_site site) {
    if (host->phase == PSRT_SCENE || channel.owner != host->context ||
        channel.index >= host->context->channel_count)
        psrt_fail(site, "Invalid channel handle or sampling phase");
    host->context->values[channel.index] = psrt_finite(value, site);
}
static inline uint32_t psrt_u32(int64_t value, psrt_site site) {
    if (value < 0 || (uint64_t)value > UINT32_MAX)
        psrt_fail(site, "Scene ID or color exceeds UInt32 range");
    return (uint32_t)value;
}
static inline void psrt_sphere(psrt_host *host, ps_vec3 center, double radius, int64_t color,
                               int64_t id, psrt_site site) {
    if (host->phase != PSRT_SCENE || !host->scene)
        psrt_fail(site, "Scene objects require scene callback");
    if (ps_scene_add_id(host->scene, psrt_u32(id, site), PS_SPHERE, center, center, radius,
                        psrt_u32(color, site)) != PS_OK)
        psrt_fail(site, "Invalid sphere or scene capacity exceeded");
}
static inline void psrt_line(psrt_host *host, ps_vec3 start, ps_vec3 end, double radius,
                             int64_t color, int64_t id, psrt_site site) {
    if (host->phase != PSRT_SCENE || !host->scene)
        psrt_fail(site, "Scene objects require scene callback");
    if (ps_scene_add_id(host->scene, psrt_u32(id, site), PS_LINE, start, end, radius,
                        psrt_u32(color, site)) != PS_OK)
        psrt_fail(site, "Invalid line or scene capacity exceeded");
}
static inline void psrt_scene_required(psrt_host *host, psrt_site site) {
    if (host->phase != PSRT_SCENE || !host->scene)
        psrt_fail(site, "Scene objects require scene callback");
}
static inline void psrt_group(psrt_host *host,const char *name,int64_t id,int64_t parent,psrt_site site) {
    psrt_scene_required(host,site);
    if(ps_scene_group(host->scene,psrt_u32(id,site),psrt_u32(parent,site),name)!=PS_OK)
        psrt_fail(site,"Invalid scene group, duplicate ID, missing parent or scene capacity exceeded");
}
static inline void psrt_scene_frame(psrt_host *host,const char *name,int64_t id,int64_t parent,
                                    ps_vec3 translation,ps_quat rotation,ps_vec3 scale,psrt_site site) {
    psrt_scene_required(host,site);
    if(ps_scene_frame(host->scene,psrt_u32(id,site),psrt_u32(parent,site),name,translation,rotation,scale)!=PS_OK)
        psrt_fail(site,"Invalid coordinate frame: ID, parent, translation, rotation, scale or composed transform");
}
static inline uint32_t psrt_scene_index(psrt_host *host,int64_t index,psrt_site site) {
    psrt_scene_required(host,site);
    if(index<0 || (uint64_t)index>=host->scene->count)psrt_fail(site,"Scene index outside published objects");
    return (uint32_t)index;
}
static inline ps_mat4 psrt_scene_transform(psrt_host *host,int64_t index,psrt_site site) {
    uint32_t slot=psrt_scene_index(host,index,site);ps_mat4 matrices[PS_MAX_OBJECTS];
    if(ps_scene_transforms(host->scene,matrices)!=PS_OK)psrt_fail(site,"Cannot compose scene coordinate frames");
    return matrices[slot];
}
static inline ps_vec3 psrt_scene_world_point(psrt_host *host,int64_t index,ps_vec3 local,psrt_site site) {
    uint32_t slot=psrt_scene_index(host,index,site);ps_vec3 result;
    if(ps_scene_world_point(host->scene,slot,local,&result)!=PS_OK)psrt_fail(site,"Invalid scene point or coordinate transform");
    return result;
}
static inline void psrt_scene_parent(psrt_host *host,int64_t child,int64_t parent,psrt_site site) {
    psrt_scene_required(host,site);
    if(ps_scene_set_parent(host->scene,psrt_u32(child,site),psrt_u32(parent,site))!=PS_OK)
        psrt_fail(site,"Invalid scene parent: missing ID, self-parent or cycle");
}
static inline void psrt_arrow(psrt_host *host, ps_vec3 start, ps_vec3 end, double radius,
                              int64_t color, int64_t id, psrt_site site) {
    psrt_scene_required(host, site);
    if (ps_scene_add_id(host->scene, psrt_u32(id, site), PS_ARROW, start, end, radius,
                        psrt_u32(color, site)) != PS_OK)
        psrt_fail(site, "Invalid arrow or scene capacity exceeded");
}
static inline void psrt_point(psrt_host *host, ps_vec3 position, double radius, int64_t color,
                              int64_t id, psrt_site site) {
    psrt_scene_required(host, site);
    if (ps_scene_add_id(host->scene, psrt_u32(id, site), PS_POINT, position, position, radius,
                        psrt_u32(color, site)) != PS_OK)
        psrt_fail(site, "Invalid point or scene capacity exceeded");
}
/* Borrows points for this call; the scene owns a copy after success. */
static inline void psrt_polyline(psrt_host *host, const ps_vec3 *points, size_t count,
                                 double radius, int64_t color, int64_t id, psrt_site site) {
    psrt_scene_required(host, site);
    if (ps_scene_polyline_id(host->scene, psrt_u32(id, site), points, count, radius,
                             psrt_u32(color, site)) != PS_OK)
        psrt_fail(site, "Invalid polyline, duplicate ID or scene capacity exceeded");
}
static inline void psrt_oriented(psrt_host *host, ps_shape shape, ps_vec3 center, ps_vec3 size,
                                 ps_quat rotation, int64_t color, int64_t id, psrt_site site) {
    psrt_scene_required(host, site);
    if (!(size.x > 0) || !(size.z > 0) || (shape == PS_BOX && !(size.y > 0)))
        psrt_fail(site, "Scene dimensions must be positive");
    ps_object object = {0};
    object.shape = shape;
    object.id = psrt_u32(id, site);
    object.color = psrt_u32(color, site);
    object.a = center;
    object.b = size;
    object.orientation = psrt_rotation(rotation, site);
    if (ps_scene_push(host->scene, &object) != PS_OK)
        psrt_fail(site, "Invalid oriented object, duplicate ID or scene capacity exceeded");
}
static inline void psrt_box(psrt_host *host, ps_vec3 center, ps_vec3 size, ps_quat rotation,
                            int64_t color, int64_t id, psrt_site site) {
    psrt_oriented(host, PS_BOX, center, size, rotation, color, id, site);
}
static inline void psrt_plane(psrt_host *host, ps_vec3 center, ps_vec2 size, ps_quat rotation,
                              int64_t color, int64_t id, psrt_site site) {
    psrt_oriented(host, PS_PLANE, center, ps_v3(size.x, 0, size.y), rotation, color, id, site);
}
static inline void psrt_label(psrt_host *host, ps_vec3 position, const char *text, int64_t color,
                              int64_t id, psrt_site site) {
    psrt_scene_required(host, site);
    if (ps_scene_label_id(host->scene, psrt_u32(id, site), position, text, psrt_u32(color, site)) !=
        PS_OK)
        psrt_fail(site, "Invalid label, duplicate ID or scene capacity exceeded");
}
static inline ps_diagnostic psrt_current_diagnostic(psrt_host *host,psrt_site site) {
    if(!host || !host->context)psrt_fail_code(site,PS_INVALID,"Diagnostic context requires an experiment");
    ps_diagnostic result;psrt_diagnostic_check(ps_experiment_diagnostic(host->context,&result),site);return result;
}
/* Logging is optional and never traps for a host budget/I/O rejection. The
 * boolean lets model code decide whether a message was accepted. */
static inline bool psrt_log_message(psrt_host *host,const char *text,ps_log_level level,psrt_site site) {
    if(!host || !host->context)psrt_fail(site,"Logging requires an experiment context");
    return ps_experiment_log(host->context,level,text)==PS_OK;
}
static inline bool psrt_log_debug(psrt_host *host,const char *text,psrt_site site){return psrt_log_message(host,text,PS_LOG_DEBUG,site);}
static inline bool psrt_log_info(psrt_host *host,const char *text,psrt_site site){return psrt_log_message(host,text,PS_LOG_INFO,site);}
static inline bool psrt_log_warning(psrt_host *host,const char *text,psrt_site site){return psrt_log_message(host,text,PS_LOG_WARNING,site);}
static inline bool psrt_log_error(psrt_host *host,const char *text,psrt_site site){return psrt_log_message(host,text,PS_LOG_ERROR,site);}
static inline void psrt_metadata(psrt_host *host, const char *text, psrt_site site) {
    if (host->phase != PSRT_CREATE)
        psrt_fail(site, "Metadata requires create callback");
    size_t used = strlen(host->context->model_metadata), size = strlen(text);
    if (used > sizeof host->context->model_metadata - 200 ||
        size + 1 > sizeof host->context->model_metadata - used - 200)
        psrt_fail(site, "Model metadata capacity exceeded");
    host->context->model_metadata[used++] = '\n';
    memcpy(host->context->model_metadata + used, text, size + 1);
}
static inline double psrt_time_step(psrt_host *host, psrt_site site) {
    if(!host || !host->context || !isfinite(host->context->dt_s) || host->context->dt_s<=0)
        psrt_fail(site,"Simulation time step requires a positive configured experiment interval");
    return host->context->dt_s;
}
static inline double psrt_time(psrt_host *host, psrt_site site) {
    return psrt_finite(host->context->time_s, site);
}
static inline int64_t psrt_run_seed(psrt_host *host, psrt_site site) {
    if (!host || !host->context)
        psrt_fail(site, "Run seed requires an experiment context");
    int64_t bits;
    uint64_t seed = host->context->seed;
    memcpy(&bits, &seed, sizeof bits);
    return bits;
}
static inline double psrt_parameter(psrt_host *host, const char *name,
                                    double default_value, double minimum, double maximum,
                                    const char *description, psrt_site site) {
    if (!host || !host->context || host->phase != PSRT_CREATE)
        psrt_fail(site, "Parameters can only be declared during create");
    double value = 0;
    ps_result result = ps_parameter_define(host->context, name, description,
                                           default_value, minimum, maximum, &value);
    if (result == PS_LIMIT)
        psrt_fail(site, "Parameter capacity exceeded");
    if (result != PS_OK)
        psrt_fail(site, "Invalid parameter definition or override");
    return value;
}
static inline double psrt_parameter_unit(psrt_host *host,const char *name,ps_unit unit,
                                          double standard,double minimum,double maximum,
                                          const char *description,psrt_site site) {
    if (!host || !host->context || host->phase!=PSRT_CREATE)
        psrt_fail(site,"Parameters can only be declared during create");
    double value=0;
    ps_result result=ps_parameter_define_unit(host->context,name,description,unit,
                                             standard,minimum,maximum,&value);
    if(result==PS_LIMIT)psrt_fail(site,"Parameter capacity exceeded");
    if(result!=PS_OK)psrt_fail(site,"Invalid parameter unit, definition or override");
    return value;
}
#endif
