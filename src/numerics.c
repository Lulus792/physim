#include "physim/numerics.h"
#include <float.h>
#include <limits.h>
#include <math.h>
#include <string.h>

static bool finite_vector(const double *v, size_t n) {
    for (size_t i = 0; i < n; i++)
        if (!isfinite(v[i]))
            return false;
    return true;
}
ps_result ps_linear_solve(const double *a, const double *b, size_t n, double tol, double *x) {
    if (!a || !b || !x || !n || n > 32 || !isfinite(tol) || tol < 0 || tol >= 1)
        return PS_INVALID;
    if (!finite_vector(a, n * n) || !finite_vector(b, n))
        return PS_INVALID;
    double m[32][33], result[32];
    if (tol == 0)
        tol = n * DBL_EPSILON;
    for (size_t i = 0; i < n; i++) {
        double scale = 0;
        for (size_t j = 0; j < n; j++)
            scale = fmax(scale, fabs(a[i * n + j]));
        if (scale == 0)
            return PS_SINGULAR;
        for (size_t j = 0; j < n; j++)
            m[i][j] = a[i * n + j] / scale;
        m[i][n] = b[i] / scale;
        if (!isfinite(m[i][n]))
            return PS_NUMERIC;
    }
    for (size_t k = 0; k < n; k++) {
        size_t pivot = k;
        for (size_t i = k + 1; i < n; i++)
            if (fabs(m[i][k]) > fabs(m[pivot][k]))
                pivot = i;
        if (fabs(m[pivot][k]) <= tol)
            return PS_SINGULAR;
        if (pivot != k)
            for (size_t j = k; j <= n; j++) {
                double tmp = m[k][j];
                m[k][j] = m[pivot][j];
                m[pivot][j] = tmp;
            }
        for (size_t i = k + 1; i < n; i++) {
            double factor = m[i][k] / m[k][k];
            for (size_t j = k + 1; j <= n; j++) {
                m[i][j] -= factor * m[k][j];
                if (!isfinite(m[i][j]))
                    return PS_NUMERIC;
            }
        }
    }
    for (size_t i = n; i-- > 0;) {
        double v = m[i][n];
        for (size_t j = i + 1; j < n; j++)
            v -= m[i][j] * result[j];
        result[i] = v / m[i][i];
        if (!isfinite(result[i]))
            return PS_NUMERIC;
    }
    memcpy(x, result, n * sizeof *x);
    return PS_OK;
}
static bool scalar_args(ps_scalar_fn fn, double lo, double hi, double abs_x, double rel_x,
                        unsigned limit, ps_scalar_report *r) {
    return fn && r && isfinite(lo) && isfinite(hi) && lo < hi && isfinite(abs_x) && abs_x > 0 &&
           isfinite(rel_x) && rel_x >= 0 && rel_x < 1 && limit > 0;
}
static bool bracket_small(double lo, double hi, double x, double abs_x, double rel_x) {
    return hi * .5 - lo * .5 <= abs_x + rel_x * fabs(x);
}
ps_result ps_root_bisect(ps_scalar_fn fn, void *u, double lo, double hi, double abs_x, double rel_x,
                         unsigned limit, ps_scalar_report *out) {
    if (!scalar_args(fn, lo, hi, abs_x, rel_x, limit, out))
        return PS_INVALID;
    double fl = fn(lo, u), fh = fn(hi, u);
    if (!isfinite(fl) || !isfinite(fh))
        return PS_NUMERIC;
    ps_scalar_report r = {0, 0, lo, hi, 0, 2};
    if (fl == 0 || fh == 0) {
        r.x = fl == 0 ? lo : hi;
        r.value = 0;
        *out = r;
        return PS_OK;
    }
    if (signbit(fl) == signbit(fh))
        return PS_INVALID;
    for (unsigned iter = 0; iter < limit; iter++) {
        double mid = lo * .5 + hi * .5, fm = fn(mid, u);
        if (!isfinite(fm))
            return PS_NUMERIC;
        r.x = mid;
        r.value = fm;
        r.lower = lo;
        r.upper = hi;
        r.iterations = iter + 1;
        r.evaluations++;
        if (fm == 0 || bracket_small(lo, hi, mid, abs_x, rel_x)) {
            *out = r;
            return PS_OK;
        }
        if (mid == lo || mid == hi)
            break;
        if (signbit(fl) == signbit(fm)) {
            lo = mid;
            fl = fm;
        } else
            hi = mid;
    }
    *out = r;
    return PS_LIMIT;
}
ps_result ps_minimize_golden(ps_scalar_fn fn, void *u, double lo, double hi, double abs_x,
                             double rel_x, unsigned limit, ps_scalar_report *out) {
    if (!scalar_args(fn, lo, hi, abs_x, rel_x, limit, out))
        return PS_INVALID;
    const double g = 0.6180339887498948482;
    double a = g * lo + (1 - g) * hi, b = (1 - g) * lo + g * hi, fa = fn(a, u), fb = fn(b, u);
    if (!isfinite(fa) || !isfinite(fb))
        return PS_NUMERIC;
    ps_scalar_report r = {0, 0, lo, hi, 0, 2};
    for (unsigned iter = 0;; iter++) {
        r.x = fa < fb ? a : b;
        r.value = fmin(fa, fb);
        r.lower = lo;
        r.upper = hi;
        r.iterations = iter;
        if (bracket_small(lo, hi, r.x, abs_x, rel_x)) {
            *out = r;
            return PS_OK;
        }
        if (iter == limit || !(lo < a && a < b && b < hi)) {
            *out = r;
            return PS_LIMIT;
        }
        if (fa < fb) {
            hi = b;
            b = a;
            fb = fa;
            a = g * lo + (1 - g) * hi;
            fa = fn(a, u);
        } else {
            lo = a;
            a = b;
            fa = fb;
            b = (1 - g) * lo + g * hi;
            fb = fn(b, u);
        }
        r.evaluations++;
        if (!isfinite(fa) || !isfinite(fb))
            return PS_NUMERIC;
    }
}
ps_result ps_verlet_step(ps_acceleration_fn fn, void *u, double t, double dt, double *q, double *v,
                         size_t n) {
    if (!fn || !q || !v || !n || n > 32 || !isfinite(t) || !isfinite(dt) || dt == 0 ||
        !isfinite(t + dt))
        return PS_INVALID;
    uintptr_t qp = (uintptr_t)q, vp = (uintptr_t)v;
    if ((qp >= vp ? qp - vp : vp - qp) < n * sizeof(double))
        return PS_INVALID;
    if (!finite_vector(q, n) || !finite_vector(v, n))
        return PS_INVALID;
    double a[32], b[32], qn[32], vn[32];
    for (size_t i = 0; i < n; i++)
        a[i] = b[i] = NAN;
    fn(t, q, a, u);
    if (!finite_vector(a, n))
        return PS_NUMERIC;
    for (size_t i = 0; i < n; i++)
        qn[i] = q[i] + dt * (v[i] + .5 * dt * a[i]);
    if (!finite_vector(qn, n))
        return PS_NUMERIC;
    fn(t + dt, qn, b, u);
    if (!finite_vector(b, n))
        return PS_NUMERIC;
    for (size_t i = 0; i < n; i++)
        vn[i] = v[i] + dt * (.5 * a[i] + .5 * b[i]);
    if (!finite_vector(vn, n))
        return PS_NUMERIC;
    memcpy(q, qn, n * sizeof *q);
    memcpy(v, vn, n * sizeof *v);
    return PS_OK;
}
ps_ode_options ps_ode_options_default(void) {
    ps_ode_options o = {1e-9, 1e-7, .001, 1e-14, 1, 100000, NULL};
    return o;
}
ps_result ps_ode_integrate(ps_ode_fn fn, void *u, double start, double end, double *state, size_t n,
                           const ps_ode_options *options, ps_ode_report *out) {
    return ps_ode_integrate_diagnosed(fn, u, start, end, state, n, options, out, NULL);
}
ps_result ps_ode_integrate_diagnosed(ps_ode_fn fn, void *u, double start, double end, double *state,
                                     size_t n, const ps_ode_options *options, ps_ode_report *out,
                                     ps_ode_diagnostic *diagnostic) {
    ps_ode_diagnostic local;
    if (!diagnostic)
        diagnostic = &local;
    *diagnostic = (ps_ode_diagnostic){PS_ODE_DIAG_ARGUMENT, start, SIZE_MAX, UINT_MAX};
    ps_ode_options o = options ? *options : ps_ode_options_default();
    if (!fn || !state || !n || n > 32 || !isfinite(start) || !isfinite(end) ||
        !isfinite(end - start) || !isfinite(o.absolute_tolerance) || o.absolute_tolerance <= 0 ||
        !isfinite(o.relative_tolerance) || o.relative_tolerance < 0 || o.relative_tolerance >= 1 ||
        !isfinite(o.initial_step) || o.initial_step <= 0 || !isfinite(o.minimum_step) ||
        o.minimum_step <= 0 || !isfinite(o.maximum_step) || o.maximum_step < o.minimum_step ||
        !o.maximum_steps || o.maximum_steps > UINT32_MAX / 7)
        return PS_INVALID;
    for (size_t i = 0; i < n; i++)
        if (!isfinite(state[i])) {
            diagnostic->reason = PS_ODE_DIAG_INITIAL_STATE;
            diagnostic->component = i;
            return PS_INVALID;
        }
    if (o.component_absolute_tolerance)
        for (size_t i = 0; i < n; i++)
            if (!isfinite(o.component_absolute_tolerance[i]) ||
                o.component_absolute_tolerance[i] <= 0) {
                diagnostic->reason = PS_ODE_DIAG_COMPONENT_TOLERANCE;
                diagnostic->component = i;
                return PS_INVALID;
            }
    diagnostic->reason = PS_ODE_DIAG_NONE;
    /* Dormand-Prince tableau, fifth-order solution and embedded fourth-order error. */
    static const double c[7] = {0, 1. / 5, 3. / 10, 4. / 5, 8. / 9, 1, 1};
    static const double a[7][6] = {
        {0},
        {1. / 5},
        {3. / 40, 9. / 40},
        {44. / 45, -56. / 15, 32. / 9},
        {19372. / 6561, -25360. / 2187, 64448. / 6561, -212. / 729},
        {9017. / 3168, -355. / 33, 46732. / 5247, 49. / 176, -5103. / 18656},
        {35. / 384, 0, 500. / 1113, 125. / 192, -2187. / 6784, 11. / 84}};
    static const double e[7] = {-71. / 57600,    0,          71. / 16695, -71. / 1920,
                                17253. / 339200, -22. / 525, 1. / 40};
    double y[32], z[32], k[7][32], t = start;
    memcpy(y, state, n * sizeof *y);
    double direction = end >= start ? 1 : -1,
           step = fmin(o.maximum_step, fmax(o.minimum_step, o.initial_step));
    ps_ode_report r = {0, 0, 0, start, direction * step, 0};
    ps_result status = PS_OK;
    for (unsigned trial = 0; t != end; trial++) {
        if (trial == o.maximum_steps) {
            diagnostic->reason = PS_ODE_DIAG_STEP_BUDGET;
            status = PS_LIMIT;
            break;
        }
        double remaining = fabs(end - t), h = direction * fmin(step, remaining);
        bool last = step >= remaining;
        double next = last ? end : t + h;
        if (next == t) {
            diagnostic->reason = PS_ODE_DIAG_TIME_RESOLUTION;
            status = PS_LIMIT;
            break;
        }
        for (size_t stage = 0; stage < 7; stage++) {
            for (size_t i = 0; i < n; i++) {
                double sum = 0;
                for (size_t j = 0; j < stage; j++)
                    sum += a[stage][j] * k[j][i];
                z[i] = y[i] + h * sum;
                k[stage][i] = NAN;
            }
            double stage_time = stage >= 5 ? next : t + c[stage] * h;
            for (size_t i = 0; i < n; i++)
                if (!isfinite(z[i])) {
                    *diagnostic = (ps_ode_diagnostic){PS_ODE_DIAG_STAGE_STATE, stage_time, i,
                                                      (unsigned)stage};
                    status = PS_NUMERIC;
                    goto done;
                }
            fn(stage_time, z, k[stage], u);
            r.evaluations++;
            for (size_t i = 0; i < n; i++)
                if (!isfinite(k[stage][i])) {
                    *diagnostic =
                        (ps_ode_diagnostic){PS_ODE_DIAG_DERIVATIVE, stage_time, i, (unsigned)stage};
                    status = PS_NUMERIC;
                    goto done;
                }
        }
        double norm = 0;
        for (size_t i = 0; i < n; i++) {
            double err = 0;
            for (size_t j = 0; j < 7; j++)
                err += e[j] * k[j][i];
            double at = o.component_absolute_tolerance ? o.component_absolute_tolerance[i]
                                                       : o.absolute_tolerance;
            double scale = at + o.relative_tolerance * fmax(fabs(y[i]), fabs(z[i]));
            double scaled = fabs(h * err) / scale;
            if (!isfinite(scale) || !isfinite(scaled)) {
                *diagnostic = (ps_ode_diagnostic){PS_ODE_DIAG_ERROR_ESTIMATE, next, i, UINT_MAX};
                status = PS_NUMERIC;
                goto done;
            }
            norm = fmax(norm, scaled);
        }
        r.error_norm = norm;
        double factor = norm == 0 ? 5 : fmax(.2, fmin(5, .9 * pow(norm, -.2)));
        if (norm <= 1) {
            memcpy(y, z, n * sizeof *y);
            t = next;
            r.accepted_steps++;
            r.reached_time = t;
        } else {
            r.rejected_steps++;
            factor = fmin(factor, .9);
            if (fabs(h) <= o.minimum_step) {
                diagnostic->reason = PS_ODE_DIAG_MINIMUM_STEP;
                status = PS_LIMIT;
                break;
            }
        }
        step = fmin(o.maximum_step, fmax(o.minimum_step, fabs(h) * factor));
        r.next_step = direction * step;
    }
done:
    if (diagnostic->component == SIZE_MAX)
        diagnostic->time = r.reached_time;
    if (out)
        *out = r;
    if (status == PS_OK)
        memcpy(state, y, n * sizeof *state);
    return status;
}

const char *ps_ode_diagnostic_string(ps_ode_diagnostic_reason reason) {
    switch (reason) {
    case PS_ODE_DIAG_NONE:
        return "Integration completed";
    case PS_ODE_DIAG_ARGUMENT:
        return "Invalid integration argument or option";
    case PS_ODE_DIAG_INITIAL_STATE:
        return "Nonfinite initial state component";
    case PS_ODE_DIAG_COMPONENT_TOLERANCE:
        return "Invalid component absolute tolerance";
    case PS_ODE_DIAG_STEP_BUDGET:
        return "Trial step budget exhausted";
    case PS_ODE_DIAG_TIME_RESOLUTION:
        return "Step cannot advance floating-point time";
    case PS_ODE_DIAG_STAGE_STATE:
        return "Nonfinite intermediate state component";
    case PS_ODE_DIAG_DERIVATIVE:
        return "Nonfinite or unset derivative component";
    case PS_ODE_DIAG_ERROR_ESTIMATE:
        return "Nonfinite error estimate or scale";
    case PS_ODE_DIAG_MINIMUM_STEP:
        return "Tolerance unmet at minimum step";
    default:
        return "Unknown ODE diagnostic";
    }
}
