#include "physim/numerics.h"
#include "physim/units.h"
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Numerics line %d: %s\n", __LINE__, #x);                               \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static double root(double x, void *u) {
    (void)u;
    return x * x - 2;
}
static double bowl(double x, void *u) {
    (void)u;
    return (x - 2.3) * (x - 2.3);
}
static double bad_scalar(double x, void *u) {
    (void)x;
    (void)u;
    return NAN;
}
static void exponential(double t, const double *y, double *d, void *u) {
    (void)t;
    (void)u;
    d[0] = y[0];
}
static void harmonic(double t, const double *y, double *d, void *u) {
    (void)t;
    (void)u;
    d[0] = y[1];
    d[1] = -y[0];
}
static void acceleration(double t, const double *q, double *a, void *u) {
    (void)t;
    (void)u;
    a[0] = -q[0];
}
static void forced(double t, const double *y, double *d, void *u) {
    (void)y;
    (void)u;
    d[0] = 3 * t * t;
}
static void incomplete(double t, const double *y, double *d, void *u) {
    (void)t;
    (void)y;
    (void)u;
    d[0] = 0;
}
static void scaled(double t, const double *y, double *d, void *u) {
    (void)t;
    (void)u;
    d[0] = y[0];
    d[1] = y[1];
}
static void late_invalid(double t, const double *y, double *d, void *u) {
    (void)y;
    (void)u;
    d[0] = 0;
    d[1] = t >= 2.25 ? INFINITY : 0;
}
static int diagnostics(void) {
    double y[] = {1, 7};
    ps_ode_report r = {9, 8, 7, 6, 5, 4}, saved = r;
    ps_ode_diagnostic d;
    CHECK(ps_ode_integrate_diagnosed(NULL, NULL, 0, 1, y, 2, NULL, &r, &d) == PS_INVALID);
    CHECK(d.reason == PS_ODE_DIAG_ARGUMENT && d.time == 0 && d.component == SIZE_MAX &&
          d.stage == UINT_MAX);
    CHECK(!memcmp(&r, &saved, sizeof r));
    y[1] = NAN;
    CHECK(ps_ode_integrate_diagnosed(harmonic, NULL, 0, 1, y, 2, NULL, &r, &d) == PS_INVALID);
    CHECK(d.reason == PS_ODE_DIAG_INITIAL_STATE && d.component == 1 &&
          !memcmp(&r, &saved, sizeof r));
    y[1] = 7;
    double tolerance[] = {1e-9, 0};
    ps_ode_options o = ps_ode_options_default();
    o.component_absolute_tolerance = tolerance;
    CHECK(ps_ode_integrate_diagnosed(harmonic, NULL, 0, 1, y, 2, &o, &r, &d) == PS_INVALID);
    CHECK(d.reason == PS_ODE_DIAG_COMPONENT_TOLERANCE && d.component == 1);
    o = ps_ode_options_default();
    o.maximum_steps = 1;
    CHECK(ps_ode_integrate_diagnosed(exponential, NULL, 0, 5, y, 1, &o, &r, &d) == PS_LIMIT);
    CHECK(d.reason == PS_ODE_DIAG_STEP_BUDGET && d.time == r.reached_time && d.time > 0 &&
          y[0] == 1);
    CHECK(d.component == SIZE_MAX && d.stage == UINT_MAX);
    CHECK(ps_ode_integrate_diagnosed(exponential, NULL, 1e16, 1e16 + 4, y, 1, NULL, &r, &d) ==
          PS_LIMIT);
    CHECK(d.reason == PS_ODE_DIAG_TIME_RESOLUTION && d.time == 1e16);
    o = ps_ode_options_default();
    o.minimum_step = o.maximum_step = o.initial_step = 1;
    o.absolute_tolerance = 1e-15;
    o.relative_tolerance = 0;
    CHECK(ps_ode_integrate_diagnosed(exponential, NULL, 0, 5, y, 1, &o, &r, &d) == PS_LIMIT);
    CHECK(d.reason == PS_ODE_DIAG_MINIMUM_STEP && d.time == 0 && r.rejected_steps == 1 &&
          y[0] == 1);
    CHECK(ps_ode_integrate_diagnosed(incomplete, NULL, 0, 1, y, 2, NULL, &r, &d) == PS_NUMERIC);
    CHECK(d.reason == PS_ODE_DIAG_DERIVATIVE && d.component == 1 && d.stage == 0 && d.time == 0);
    o = ps_ode_options_default();
    o.initial_step = 1;
    CHECK(ps_ode_integrate_diagnosed(late_invalid, NULL, 2, 3, y, 2, &o, &r, &d) == PS_NUMERIC);
    CHECK(d.reason == PS_ODE_DIAG_DERIVATIVE && d.component == 1 && d.stage == 2 &&
          fabs(d.time - 2.3) < 1e-15);
    CHECK(r.evaluations == 3 && r.reached_time == 2 && y[0] == 1 && y[1] == 7);
    y[0] = DBL_MAX;
    CHECK(ps_ode_integrate_diagnosed(exponential, NULL, 0, 1, y, 1, &o, &r, &d) == PS_NUMERIC);
    CHECK(d.reason == PS_ODE_DIAG_STAGE_STATE && d.component == 0 && d.stage == 1 && d.time == .2 &&
          y[0] == DBL_MAX);
    o.absolute_tolerance = DBL_MAX;
    o.relative_tolerance = .5;
    CHECK(ps_ode_integrate_diagnosed(incomplete, NULL, 0, 1, y, 1, &o, &r, &d) == PS_NUMERIC);
    CHECK(d.reason == PS_ODE_DIAG_ERROR_ESTIMATE && d.component == 0 && d.stage == UINT_MAX &&
          d.time == 1);
    y[0] = 1;
    CHECK(ps_ode_integrate_diagnosed(exponential, NULL, 1, 0, y, 1, NULL, &r, &d) == PS_OK);
    CHECK(d.reason == PS_ODE_DIAG_NONE && d.time == 0 && d.component == SIZE_MAX &&
          d.stage == UINT_MAX);
    CHECK(fabs(y[0] - exp(-1)) < 1e-8);
    CHECK(ps_ode_integrate_diagnosed(exponential, NULL, 4, 4, y, 1, NULL, &r, &d) == PS_OK);
    CHECK(d.reason == PS_ODE_DIAG_NONE && d.time == 4 && !r.evaluations);
    CHECK(
        !strcmp(ps_ode_diagnostic_string((ps_ode_diagnostic_reason)999), "Unknown ODE diagnostic"));
    for (int i = PS_ODE_DIAG_NONE; i <= PS_ODE_DIAG_MINIMUM_STEP; i++)
        CHECK(strcmp(ps_ode_diagnostic_string((ps_ode_diagnostic_reason)i),
                     "Unknown ODE diagnostic"));
    return 0;
}
int main(void) {
    CHECK(!diagnostics());
    double matrix[] = {0, 2, 1, 1, -2, -3, 3, -1, 2}, rhs[] = {7, -12, 7}, x[] = {99, 98, 97};
    CHECK(ps_linear_solve(matrix, rhs, 3, 0, x) == PS_OK);
    for (int i = 0; i < 3; i++)
        CHECK(fabs(x[i] - (i + 1)) < 1e-12);
    CHECK(ps_linear_solve(matrix, rhs, 3, 0, rhs) == PS_OK);
    CHECK(fabs(rhs[2] - 3) < 1e-12);
    double singular[] = {1, 2, 2, 4}, b[] = {3, 6};
    x[0] = 99;
    CHECK(ps_linear_solve(singular, b, 2, 0, x) == PS_SINGULAR && x[0] == 99);
    double near[] = {1, 1, 1, 1 + DBL_EPSILON};
    CHECK(ps_linear_solve(near, b, 2, 0, x) == PS_SINGULAR);
    double huge[] = {1e300, 0, 0, 1e-300}, hb[] = {2e300, 3e-300};
    CHECK(ps_linear_solve(huge, hb, 2, 0, x) == PS_OK && x[0] == 2 && fabs(x[1] - 3) < 1e-14);
    double dense[32 * 32], right[32] = {0}, solution[32];
    for (size_t i = 0; i < 32; i++)
        for (size_t j = 0; j < 32; j++) {
            dense[i * 32 + j] = i == j ? 40 : sin((double)(i * 32 + j));
            right[i] += dense[i * 32 + j] * cos((double)j);
        }
    CHECK(ps_linear_solve(dense, right, 32, 0, solution) == PS_OK);
    for (size_t i = 0; i < 32; i++)
        CHECK(fabs(solution[i] - cos((double)i)) < 1e-13);
    ps_scalar_report r;
    CHECK(ps_root_bisect(root, NULL, 0, 2, 1e-12, 0, 100, &r) == PS_OK &&
          fabs(r.x - sqrt(2)) < 1e-12);
    CHECK(ps_root_bisect(root, NULL, 2, 3, 1e-12, 0, 100, &r) == PS_INVALID);
    CHECK(ps_root_bisect(root, NULL, 0, 2, 1e-12, 0, 1, &r) == PS_LIMIT && r.iterations == 1);
    CHECK(ps_root_bisect(bad_scalar, NULL, 0, 2, 1e-12, 0, 100, &r) == PS_NUMERIC);
    CHECK(ps_minimize_golden(bowl, NULL, -10, 10, 1e-10, 0, 100, &r) == PS_OK &&
          fabs(r.x - 2.3) < 2e-10);
    CHECK(ps_minimize_golden(bowl, NULL, -10, 10, 1e-10, 0, 1, &r) == PS_LIMIT);
    double errors[2];
    for (int pass = 0; pass < 2; pass++) {
        double q = 1, v = 0, dt = pass ? .005 : .01;
        int steps = pass ? 2000 : 1000;
        for (int i = 0; i < steps; i++)
            CHECK(ps_verlet_step(acceleration, NULL, i * dt, dt, &q, &v, 1) == PS_OK);
        errors[pass] = fabs(q - cos(10));
        CHECK(fabs(.5 * (q * q + v * v) - .5) < dt * dt);
        for (int i = steps; i > 0; i--)
            CHECK(ps_verlet_step(acceleration, NULL, i * dt, -dt, &q, &v, 1) == PS_OK);
        CHECK(fabs(q - 1) < 1e-12 && fabs(v) < 1e-12);
    }
    CHECK(errors[0] / errors[1] > 3.9 && errors[0] / errors[1] < 4.1);
    double q = 1, v = 2;
    CHECK(ps_verlet_step(incomplete, NULL, 0, .1, &q, &q, 1) == PS_INVALID && q == 1);
    double qp[] = {1, 2}, vp[] = {3, 4};
    CHECK(ps_ode_step(PS_RK4, incomplete, NULL, 0, .1, qp, 2) == PS_NUMERIC && qp[0] == 1 &&
          qp[1] == 2);
    CHECK(ps_ode_step(PS_EULER, incomplete, NULL, 0, .1, qp, 2) == PS_NUMERIC && qp[0] == 1 &&
          qp[1] == 2);
    CHECK(ps_verlet_step(incomplete, NULL, 0, .1, qp, vp, 2) == PS_NUMERIC && qp[0] == 1 &&
          vp[1] == 4);
    ps_ode_report report;
    ps_ode_options o = ps_ode_options_default();
    o.initial_step = 1;
    o.absolute_tolerance = 1e-12;
    o.relative_tolerance = 1e-11;
    double y[] = {1, 0};
    CHECK(ps_ode_integrate(exponential, NULL, 0, 5, y, 1, &o, &report) == PS_OK);
    CHECK(fabs(y[0] - exp(5)) < 3e-9 && report.reached_time == 5 && report.rejected_steps > 0);
    CHECK(report.evaluations == 7 * (report.accepted_steps + report.rejected_steps));
    CHECK(ps_ode_integrate(exponential, NULL, 5, 0, y, 1, &o, NULL) == PS_OK &&
          fabs(y[0] - 1) < 3e-11);
    y[0] = 1;
    y[1] = 0;
    CHECK(ps_ode_integrate(harmonic, NULL, 0, 20, y, 2, &o, NULL) == PS_OK);
    CHECK(fabs(y[0] - cos(20)) < 1e-10 && fabs(y[1] + sin(20)) < 1e-10);
    y[0] = 8;
    CHECK(ps_ode_integrate(forced, NULL, 2, 3, y, 1, &o, NULL) == PS_OK && fabs(y[0] - 27) < 1e-12);
    y[0] = 1;
    y[1] = 7;
    o.maximum_steps = 1;
    CHECK(ps_ode_integrate(exponential, NULL, 0, 5, y, 1, &o, &report) == PS_LIMIT && y[0] == 1);
    o = ps_ode_options_default();
    o.maximum_steps = 1;
    y[0] = 1;
    CHECK(ps_ode_integrate(exponential, NULL, 0, 5, y, 1, &o, &report) == PS_LIMIT && y[0] == 1 &&
          report.accepted_steps == 1 && report.reached_time > 0);
    o = ps_ode_options_default();
    CHECK(ps_ode_integrate(exponential, NULL, 1e16, 1e16 + 4, y, 1, &o, &report) == PS_LIMIT &&
          y[0] == 1);
    o = ps_ode_options_default();
    o.minimum_step = 1;
    o.maximum_step = 1;
    o.initial_step = 1;
    o.absolute_tolerance = 1e-15;
    o.relative_tolerance = 0;
    CHECK(ps_ode_integrate(exponential, NULL, 0, 5, y, 1, &o, &report) == PS_LIMIT && y[0] == 1);
    CHECK(ps_ode_integrate(incomplete, NULL, 0, 1, y, 2, NULL, &report) == PS_NUMERIC &&
          y[0] == 1 && y[1] == 7);
    double atol[] = {1e-20, 1e-2};
    y[0] = 1e-10;
    y[1] = 1e10;
    o = ps_ode_options_default();
    o.component_absolute_tolerance = atol;
    o.relative_tolerance = 1e-10;
    CHECK(ps_ode_integrate(scaled, NULL, 0, 2, y, 2, &o, NULL) == PS_OK);
    CHECK(fabs(y[0] / 1e-10 - exp(2)) < 1e-8 && fabs(y[1] / 1e10 - exp(2)) < 1e-8);
    CHECK(ps_ode_integrate(exponential, NULL, 0, 0, y, 1, NULL, &report) == PS_OK &&
          report.evaluations == 0);
    ps_unit u;
    CHECK(ps_unit_multiply(PS_KILOGRAM, PS_ACCELERATION, "N", &u) == PS_OK &&
          ps_unit_compatible(u, PS_NEWTON));
    CHECK(ps_unit_divide(PS_JOULE, PS_SECOND, "W", &u) == PS_OK && ps_unit_compatible(u, PS_WATT));
    CHECK(ps_unit_power(PS_SECOND, -1, "Hz", &u) == PS_OK && ps_unit_compatible(u, PS_HERTZ));
    CHECK(ps_unit_power(PS_METRE, INT_MIN, NULL, &u) == PS_INVALID);
    ps_unit cm = PS_METRE;
    cm.scale = .01;
    cm.symbol = "cm";
    ps_quantity qa = {1, PS_METRE}, qb = {50, cm}, result = {99, PS_ONE};
    CHECK(ps_quantity_add(qa, qb, &result) == PS_OK && result.value == 1.5);
    qb.unit = PS_SECOND;
    CHECK(ps_quantity_add(qa, qb, &result) == PS_INVALID && result.value == 1.5);
    CHECK(ps_quantity_divide(qa, qb, "m/s", &result) == PS_OK &&
          ps_unit_compatible(result.unit, PS_VELOCITY));
    qb.value = 0;
    CHECK(ps_quantity_divide(qa, qb, NULL, &result) == PS_INVALID);
    double converted = 123;
    ps_unit a = PS_METRE, big = PS_METRE;
    a.scale = 1e300;
    big.scale = 1e300;
    CHECK(ps_convert(1e300, a, big, &converted) == PS_OK && converted == 1e300);
    CHECK(ps_convert(1e300, a, PS_METRE, &converted) == PS_NUMERIC && converted == 1e300);
    char text[128] = "untouched";
    CHECK(ps_unit_format_dimension(PS_NEWTON, text, sizeof text) == PS_OK &&
          !strcmp(text, "m kg s^-2"));
    CHECK(ps_unit_format_dimension(PS_NEWTON, text, 2) == PS_LIMIT && !strcmp(text, "m kg s^-2"));
    CHECK(ps_unit_format_dimension(PS_ONE, text, sizeof text) == PS_OK && !strcmp(text, "1"));
    (void)v;
    puts("Numerical reference, failure atomicity and unit algebra checks passed");
    return 0;
}
