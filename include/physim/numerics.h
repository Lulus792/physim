#ifndef PHYSIM_NUMERICS_H
#define PHYSIM_NUMERICS_H
#include "core.h"
#include <limits.h>
#define PS_NUMERIC_MAX_DIMENSION 32u

/* Row-major A, scaled partial pivoting. A and b are preserved. x may alias b.
 * n <= 32; pivot_tolerance=0 selects n*DBL_EPSILON. Output unchanged on error. */
ps_result ps_linear_solve(const double *a, const double *b, size_t n, double pivot_tolerance,
                          double *x);
typedef double (*ps_scalar_fn)(double x, void *user);
typedef struct {
    double x, value, lower, upper;
    unsigned iterations, evaluations;
} ps_scalar_report;
/* Continuous function, opposite signs at ordered endpoints (or an endpoint root).
 * Terminates on bracket half-width <= absolute_x + relative_x*abs(x).
 * Report is also returned on PS_LIMIT; no guarantee for discontinuous functions. */
ps_result ps_root_bisect(ps_scalar_fn fn, void *user, double lower, double upper, double absolute_x,
                         double relative_x, unsigned max_iterations, ps_scalar_report *report);
/* Unimodal function on [lower,upper]; golden-section minimization, same x tolerance. */
ps_result ps_minimize_golden(ps_scalar_fn fn, void *user, double lower, double upper,
                             double absolute_x, double relative_x, unsigned max_iterations,
                             ps_scalar_report *report);

typedef void (*ps_acceleration_fn)(double time, const double *position, double *acceleration,
                                   void *user);
/* Velocity Verlet for q''=a(t,q), no velocity-dependent acceleration. Arrays must
 * be distinct, n<=32. Position/velocity unchanged if any evaluation is invalid. */
ps_result ps_verlet_step(ps_acceleration_fn fn, void *user, double time, double dt,
                         double *position, double *velocity, size_t n);
typedef struct {
    double absolute_tolerance, relative_tolerance;
    double initial_step, minimum_step, maximum_step;
    unsigned maximum_steps;                     /* accepted + rejected trial steps */
    const double *component_absolute_tolerance; /* optional n positive tolerances */
} ps_ode_options;
typedef struct {
    unsigned accepted_steps, rejected_steps, evaluations;
    double reached_time, next_step, error_norm;
} ps_ode_report;
ps_ode_options ps_ode_options_default(void);
/* Dormand-Prince 5(4), explicit non-stiff ODEs. Integrates forward or backward to
 * the requested endpoint, n<=32. Infinity norm of component-wise scaled error.
 * Local error control is not a global-error bound. State changes only on PS_OK.
 * Callbacks must be deterministic and must not change externally visible state:
 * rejected stages and trial evaluations are normal. No events/dense output yet. */
ps_result ps_ode_integrate(ps_ode_fn fn, void *user, double start, double end, double *state,
                           size_t n, const ps_ode_options *options, ps_ode_report *report);
typedef enum {
    PS_ODE_DIAG_NONE,
    PS_ODE_DIAG_ARGUMENT,
    PS_ODE_DIAG_INITIAL_STATE,
    PS_ODE_DIAG_COMPONENT_TOLERANCE,
    PS_ODE_DIAG_STEP_BUDGET,
    PS_ODE_DIAG_TIME_RESOLUTION,
    PS_ODE_DIAG_STAGE_STATE,
    PS_ODE_DIAG_DERIVATIVE,
    PS_ODE_DIAG_ERROR_ESTIMATE,
    PS_ODE_DIAG_MINIMUM_STEP
} ps_ode_diagnostic_reason;
typedef struct {
    ps_ode_diagnostic_reason reason;
    double time;      /* Failure evaluation time; reached time for limit/success. */
    size_t component; /* Zero-based, SIZE_MAX when not applicable. */
    unsigned stage;   /* Zero-based Dormand-Prince stage, UINT_MAX otherwise. */
} ps_ode_diagnostic;
/* Same integration and transactional state contract. Optional caller-owned
 * diagnostic is written on EVERY return, including invalid arguments; report
 * remains untouched on invalid arguments. No allocation or global error state.
 * Diagnostic storage, report and state must not overlap. NONE denotes success.
 * For invalid arguments, time is start (possibly nonfinite). */
ps_result ps_ode_integrate_diagnosed(ps_ode_fn fn, void *user, double start, double end,
                                     double *state, size_t n, const ps_ode_options *options,
                                     ps_ode_report *report, ps_ode_diagnostic *diagnostic);
/* Static English description; unknown reason values return "Unknown ODE diagnostic". */
const char *ps_ode_diagnostic_string(ps_ode_diagnostic_reason reason);
#endif
