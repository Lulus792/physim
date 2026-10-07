#ifndef PS_SCALAR_NUMERIC_H
#define PS_SCALAR_NUMERIC_H
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
_Static_assert(FLT_RADIX == 2 && DBL_MANT_DIG == 53 && DBL_MAX_EXP == 1024 && DBL_MIN_EXP == -1021,
               "Scalar search requires binary64 doubles");
/* Exact sign of hi - lo - 2*absolute - 2*relative*abs(x). Products contain
 * at most 106 significand bits, with lowest exponent -2147. A fixed array
 * spans the complete binary64 product range; no allocation or wider type. */
#define SCALAR_WORDS 100
#define SCALAR_BASE (-2148)
typedef struct {
    uint64_t significand;
    int exponent;
} scalar_binary;
static scalar_binary scalar_parts(double value) {
    int exponent;
    double mantissa = frexp(fabs(value), &exponent);
    uint64_t significand = (uint64_t)ldexp(mantissa, 53);
    exponent -= 53;
    if (significand)
        while (!(significand & 1)) {
            significand >>= 1;
            exponent++;
        }
    return (scalar_binary){significand, exponent};
}
static void scalar_add(uint32_t sum[SCALAR_WORDS], const uint32_t *words, size_t count,
                       int exponent) {
    while (count && words[count - 1] == 0)
        count--;
    if (!count)
        return;
    unsigned position = (unsigned)(exponent - SCALAR_BASE);
    size_t index = position / 32;
    unsigned shift = position % 32;
    uint64_t carry = 0;
    for (size_t i = 0; i < count; i++) {
        uint64_t total = ((uint64_t)words[i] << shift) + carry + sum[index + i];
        sum[index + i] = (uint32_t)total;
        carry = total >> 32;
    }
    index += count;
    while (carry) {
        uint64_t total = (uint64_t)sum[index] + carry;
        sum[index++] = (uint32_t)total;
        carry = total >> 32;
    }
}
static void scalar_add_double(uint32_t sum[SCALAR_WORDS], double value, int extra_exponent) {
    if (value == 0)
        return;
    scalar_binary p = scalar_parts(value);
    uint32_t words[2] = {(uint32_t)p.significand, (uint32_t)(p.significand >> 32)};
    scalar_add(sum, words, 2, p.exponent + extra_exponent);
}
static void scalar_add_product(uint32_t sum[SCALAR_WORDS], double a, double b) {
    if (a == 0 || b == 0)
        return;
    scalar_binary x = scalar_parts(a), y = scalar_parts(b);
    uint64_t xl = (uint32_t)x.significand, yl = (uint32_t)y.significand;
    uint64_t xh = x.significand >> 32, yh = y.significand >> 32;
    uint64_t low = xl * yl;
    uint64_t middle = xh * yl + xl * yh + (low >> 32);
    uint64_t high = xh * yh + (middle >> 32);
    uint32_t words[4] = {(uint32_t)low, (uint32_t)middle, (uint32_t)high, (uint32_t)(high >> 32)};
    scalar_add(sum, words, 4, x.exponent + y.exponent + 1);
}
static bool ps_scalar_bracket_small(double lo, double hi, double x, double absolute,
                                    double relative) {
    /* A wide error enclosure settles ordinary cases cheaply. Endpoint halves,
     * product, sum and final difference contribute at most six epsilon*scale
     * plus subnormal rounding errors; sixteen leaves a conservative margin.
     * Only use a normal margin, so underflow cannot erase the enclosure. */
    double scale = fmax(fmax(fabs(lo), fabs(hi)), fmax(absolute, fabs(x)));
    double margin = (16 * DBL_EPSILON) * scale;
    double width = hi * .5 - lo * .5;
    double tolerance = absolute + relative * fabs(x);
    if (margin >= DBL_MIN && isfinite(tolerance)) {
        double difference = width - tolerance;
        if (difference > margin)
            return false;
        if (difference < -margin)
            return true;
    }
    uint32_t positive[SCALAR_WORDS] = {0}, negative[SCALAR_WORDS] = {0};
    scalar_add_double(hi >= 0 ? positive : negative, hi, 0);
    scalar_add_double(lo >= 0 ? negative : positive, lo, 0);
    scalar_add_double(negative, absolute, 1);
    scalar_add_product(negative, relative, fabs(x));
    for (size_t i = SCALAR_WORDS; i > 0; i--)
        if (positive[i - 1] != negative[i - 1])
            return positive[i - 1] < negative[i - 1];
    return true;
}
#endif
