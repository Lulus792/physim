#ifndef PHYSIM_LINEAR_NUMERIC_H
#define PHYSIM_LINEAR_NUMERIC_H
#include "physim/core.h"
#include <float.h>
#include <math.h>
#include <string.h>
static inline bool linear_finite_vector(const double *v, size_t n) {
    for (size_t i = 0; i < n; i++)
        if (!isfinite(v[i]))
            return false;
    return true;
}
/* Private mantissa/exponent values keep each right-hand component independent.
 * They extend range, not precision: cancellation still follows Double rounding. */
typedef struct {
    double mantissa;
    int exponent;
} linear_value;
static inline linear_value linear_normalize(double value, int exponent) {
    int shift;
    double mantissa = frexp(value, &shift);
    return (linear_value){mantissa, value == 0 ? 0 : exponent + shift};
}
static inline linear_value linear_divide(linear_value value, double divisor) {
    int exponent;
    double mantissa = frexp(divisor, &exponent);
    return linear_normalize(value.mantissa / mantissa, value.exponent - exponent);
}
/* Up to 32 finite scaled terms with finite weights. */
static inline linear_value linear_sum(const linear_value *values, const double *weights, size_t n) {
    double terms[32], errors[32];
    int exponents[32], maximum = 0;
    bool any = false;
    for (size_t i = 0; i < n; i++) {
        if (values[i].mantissa == 0 || weights[i] == 0) {
            terms[i] = errors[i] = 0;
            exponents[i] = 0;
            continue;
        }
        int e;
        double w = frexp(weights[i], &e);
        terms[i] = values[i].mantissa * w;
        errors[i] = fma(values[i].mantissa, w, -terms[i]);
        exponents[i] = values[i].exponent + e;
        if (!any || exponents[i] > maximum)
            maximum = exponents[i];
        any = true;
    }
    if (!any)
        return (linear_value){0, 0};
    double sum = 0, compensation = 0;
    for (size_t i = 0; i < n; i++)
        if (terms[i] != 0) {
            double term = scalbn(terms[i], exponents[i] - maximum), next = sum + term;
            compensation += fabs(sum) >= fabs(term) ? (sum - next) + term : (term - next) + sum;
            compensation += scalbn(errors[i], exponents[i] - maximum);
            sum = next;
        }
    return linear_normalize(sum + compensation, maximum);
}
static inline ps_result linear_solve_scaled(const double *a, const double *b, size_t n, double tol,
                                            linear_value *x) {
    if (!a || !b || !x || !n || n > 32 || !isfinite(tol) || tol < 0 || tol >= 1)
        return PS_INVALID;
    if (!linear_finite_vector(a, n * n) || !linear_finite_vector(b, n))
        return PS_INVALID;
    double m[32][32];
    linear_value rhs[32], solved[32];
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
        rhs[i] = linear_divide(linear_normalize(b[i], 0), scale);
    }
    for (size_t k = 0; k < n; k++) {
        size_t pivot = k;
        for (size_t i = k + 1; i < n; i++)
            if (fabs(m[i][k]) > fabs(m[pivot][k]))
                pivot = i;
        if (fabs(m[pivot][k]) <= tol)
            return PS_SINGULAR;
        if (pivot != k) {
            for (size_t j = k; j < n; j++) {
                double tmp = m[k][j];
                m[k][j] = m[pivot][j];
                m[pivot][j] = tmp;
            }
            linear_value tmp = rhs[k];
            rhs[k] = rhs[pivot];
            rhs[pivot] = tmp;
        }
        for (size_t i = k + 1; i < n; i++) {
            double factor = m[i][k] / m[k][k];
            for (size_t j = k + 1; j < n; j++) {
                m[i][j] = fma(-factor, m[k][j], m[i][j]);
                if (!isfinite(m[i][j]))
                    return PS_NUMERIC;
            }
            rhs[i] =
                linear_sum((const linear_value[]){rhs[i], rhs[k]}, (const double[]){1, -factor}, 2);
        }
    }
    for (size_t i = n; i-- > 0;) {
        linear_value values[32];
        double weights[32];
        size_t count = 1;
        values[0] = rhs[i];
        weights[0] = 1;
        for (size_t j = i + 1; j < n; j++) {
            values[count] = solved[j];
            weights[count++] = -m[i][j];
        }
        solved[i] = linear_divide(linear_sum(values, weights, count), m[i][i]);
    }
    memcpy(x, solved, n * sizeof *x);
    return PS_OK;
}
static inline double linear_double(linear_value value) {
    return scalbn(value.mantissa, value.exponent);
}
#endif
