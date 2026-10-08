#ifndef PS_TRANSFORM_NUMERIC_H
#define PS_TRANSFORM_NUMERIC_H
#include "linear_numeric.h"
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
_Static_assert(FLT_RADIX == 2 && DBL_MANT_DIG == 53 && DBL_MAX_EXP == 1024 && DBL_MIN_EXP == -1021,
               "Scalar search requires binary64 doubles");
/* Four exact binary64 products span exponents -2148 through 2049. Signed
 * integer accumulation preserves arbitrarily small cancellation remainders.
 * Convert only the final magnitude to a normalized 53-bit value. */
#define TRANSFORM_WORDS 132
#define TRANSFORM_BASE (-2148)
typedef struct {
    uint64_t significand;
    int exponent;
} transform_binary;
static inline transform_binary transform_parts(double value) {
    int exponent;
    double mantissa = frexp(fabs(value), &exponent);
    uint64_t significand = (uint64_t)ldexp(mantissa, 53);
    exponent -= 53;
    if (significand)
        while (!(significand & 1)) {
            significand >>= 1;
            exponent++;
        }
    return (transform_binary){significand, exponent};
}
static inline void transform_add(uint32_t sum[TRANSFORM_WORDS], const uint32_t *words, size_t count,
                                 int exponent) {
    while (count && words[count - 1] == 0)
        count--;
    if (!count)
        return;
    unsigned position = (unsigned)(exponent - TRANSFORM_BASE);
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
static inline void transform_add_double(uint32_t sum[TRANSFORM_WORDS], double value,
                                        int extra_exponent) {
    if (value == 0)
        return;
    transform_binary p = transform_parts(value);
    uint32_t words[2] = {(uint32_t)p.significand, (uint32_t)(p.significand >> 32)};
    transform_add(sum, words, 2, p.exponent + extra_exponent);
}
static inline void transform_add_product(uint32_t sum[TRANSFORM_WORDS], double a, double b) {
    if (a == 0 || b == 0)
        return;
    transform_binary x = transform_parts(a), y = transform_parts(b);
    uint64_t xl = (uint32_t)x.significand, yl = (uint32_t)y.significand;
    uint64_t xh = x.significand >> 32, yh = y.significand >> 32;
    uint64_t low = xl * yl;
    uint64_t middle = xh * yl + xl * yh + (low >> 32);
    uint64_t high = xh * yh + (middle >> 32);
    uint32_t words[4] = {(uint32_t)low, (uint32_t)middle, (uint32_t)high, (uint32_t)(high >> 32)};
    transform_add(sum, words, 4, x.exponent + y.exponent);
}
static inline int transform_compare(const uint32_t *a, const uint32_t *b) {
    for (size_t i = TRANSFORM_WORDS; i > 0; i--)
        if (a[i - 1] != b[i - 1])
            return a[i - 1] > b[i - 1] ? 1 : -1;
    return 0;
}
static inline unsigned transform_bit(const uint32_t *a, size_t bit) {
    return (a[bit / 32] >> (bit % 32)) & 1;
}
typedef struct {
    uint32_t magnitude[TRANSFORM_WORDS];
    int sign;
} transform_exact;
/* Ordinary products form short exact floating expansions via FMA/TwoSum.
 * Small products or overflowing expansion stages use the integer fallback. */
static inline bool transform_direct(const double *a, const double *b, size_t count, double *out) {
    double expansion[8];
    size_t length = 0;
    for (size_t i = 0; i < count; i++) {
        if (a[i] == 0 || b[i] == 0)
            continue;
        double product = a[i] * b[i];
        if (!isfinite(product) || fabs(product) < 0x1p-968)
            return false;
        double terms[2] = {fma(a[i], b[i], -product), product};
        for (size_t term = 0; term < 2; term++) {
            double q = terms[term];
            size_t next_length = 0;
            if (q == 0)
                continue;
            for (size_t j = 0; j < length; j++) {
                double x = expansion[j], sum = q + x;
                if (!isfinite(sum))
                    return false;
                double bv = sum - q, av = sum - bv;
                double error = (q - av) + (x - bv);
                if (error != 0)
                    expansion[next_length++] = error;
                q = sum;
            }
            if (q != 0)
                expansion[next_length++] = q;
            length = next_length;
        }
    }
    if (!length) {
        *out = 0;
        return true;
    }
    double high = expansion[--length], low = 0;
    while (length) {
        double x = high, y = expansion[--length];
        high = x + y;
        if (!isfinite(high))
            return false;
        low = y - (high - x);
        if (low != 0)
            break;
    }
    /* If the first discarded part is exactly a half-way remainder, the
     * remaining expansion determines which side of that midpoint is exact. */
    if (length &&
        ((low > 0 && expansion[length - 1] > 0) || (low < 0 && expansion[length - 1] < 0))) {
        double doubled = low * 2, next = high + doubled;
        if (doubled == next - high)
            high = next;
    }
    *out = high;
    return true;
}
static inline transform_exact transform_dot(const double *a, const double *b, size_t count) {
    uint32_t positive[TRANSFORM_WORDS] = {0}, negative[TRANSFORM_WORDS] = {0};
    for (size_t i = 0; i < count; i++)
        transform_add_product(signbit(a[i]) == signbit(b[i]) ? positive : negative, a[i], b[i]);
    int sign = transform_compare(positive, negative);
    if (!sign)
        return (transform_exact){{0}, 0};
    uint32_t *large = sign > 0 ? positive : negative, *small = sign > 0 ? negative : positive;
    uint64_t borrow = 0;
    for (size_t i = 0; i < TRANSFORM_WORDS; i++) {
        uint64_t sub = (uint64_t)small[i] + borrow;
        uint64_t current = large[i];
        large[i] = (uint32_t)(current - sub);
        borrow = current < sub;
    }
    transform_exact result = {{0}, sign};
    memcpy(result.magnitude, large, sizeof result.magnitude);
    return result;
}
#define TRANSFORM_DIV_WORDS 268
static inline size_t transform_top(const uint32_t *a, size_t count) {
    size_t top = count * 32;
    while (top && !((a[(top - 1) / 32] >> ((top - 1) % 32)) & 1))
        top--;
    return top;
}
static inline void transform_shift_copy(uint32_t *out, const uint32_t *input, unsigned shift) {
    unsigned word = shift / 32, bits = shift % 32;
    for (size_t i = 0; i < TRANSFORM_WORDS; i++) {
        uint64_t value = (uint64_t)input[i] << bits;
        out[word + i] |= (uint32_t)value;
        if (bits)
            out[word + i + 1] |= (uint32_t)(value >> 32);
    }
}
static inline int transform_div_compare(const uint32_t *a, const uint32_t *b, unsigned shift) {
    unsigned word = shift / 32, bits = shift % 32;
    for (size_t i = TRANSFORM_DIV_WORDS; i > 0; i--) {
        size_t j = i - 1;
        uint32_t value = 0;
        if (j >= word) {
            size_t k = j - word;
            value = b[k] << bits;
            if (bits && k)
                value |= b[k - 1] >> (32 - bits);
        }
        if (a[j] != value)
            return a[j] > value ? 1 : -1;
    }
    return 0;
}
static inline void transform_div_subtract(uint32_t *a, const uint32_t *b, unsigned shift) {
    unsigned word = shift / 32, bits = shift % 32;
    uint64_t borrow = 0;
    for (size_t j = 0; j < TRANSFORM_DIV_WORDS; j++) {
        uint32_t value = 0;
        if (j >= word) {
            size_t k = j - word;
            value = b[k] << bits;
            if (bits && k)
                value |= b[k - 1] >> (32 - bits);
        }
        uint64_t sub = (uint64_t)value + borrow, current = a[j];
        a[j] = (uint32_t)(current - sub);
        borrow = current < sub;
    }
}
static inline double transform_quotient(transform_exact numerator, transform_exact denominator) {
    if (!numerator.sign)
        return 0;
    size_t nt = transform_top(numerator.magnitude, TRANSFORM_WORDS),
           dt = transform_top(denominator.magnitude, TRANSFORM_WORDS);
    int exponent = (int)nt - (int)dt;
    uint32_t n[TRANSFORM_DIV_WORDS] = {0}, d[TRANSFORM_DIV_WORDS] = {0};
    transform_shift_copy(n, numerator.magnitude, exponent < 0 ? (unsigned)-exponent : 0);
    transform_shift_copy(d, denominator.magnitude, exponent > 0 ? (unsigned)exponent : 0);
    if (transform_div_compare(n, d, 0) < 0)
        exponent--;
    int shift = exponent - 52;
    if (shift < -1074)
        shift = -1074;
    if (exponent > 1023)
        return numerator.sign == denominator.sign ? INFINITY : -INFINITY;
    memset(n, 0, sizeof n);
    memset(d, 0, sizeof d);
    transform_shift_copy(n, numerator.magnitude, shift < 0 ? (unsigned)-shift : 0);
    transform_shift_copy(d, denominator.magnitude, shift > 0 ? (unsigned)shift : 0);
    size_t nbits = transform_top(n, TRANSFORM_DIV_WORDS),
           dbits = transform_top(d, TRANSFORM_DIV_WORDS);
    uint64_t significand = 0;
    if (nbits >= dbits)
        for (size_t i = nbits - dbits + 1; i > 0; i--) {
            unsigned bit = (unsigned)(i - 1);
            if (transform_div_compare(n, d, bit) >= 0) {
                transform_div_subtract(n, d, bit);
                significand |= UINT64_C(1) << bit;
            }
        }
    /* Compare 2*remainder with denominator without losing the half-way bit. */
    uint64_t carry = 0;
    for (size_t i = 0; i < TRANSFORM_DIV_WORDS; i++) {
        uint64_t value = ((uint64_t)n[i] << 1) + carry;
        n[i] = (uint32_t)value;
        carry = value >> 32;
    }
    int rounding = transform_div_compare(n, d, 0);
    if (rounding > 0 || (rounding == 0 && (significand & 1)))
        significand++;
    double value = scalbn((double)significand, shift);
    return numerator.sign == denominator.sign ? value : -value;
}
static inline double transform_double(transform_exact value) {
    if (!value.sign)
        return 0;
    size_t top = transform_top(value.magnitude, TRANSFORM_WORDS);
    size_t shift = top > 53 ? top - 53 : 0;
    if (shift < (size_t)(-1074 - TRANSFORM_BASE))
        shift = (size_t)(-1074 - TRANSFORM_BASE);
    uint64_t significand = 0;
    for (size_t i = top; i > shift; i--)
        significand = (significand << 1) | transform_bit(value.magnitude, i - 1);
    if (shift) {
        bool sticky = false;
        for (size_t i = 0; i + 1 < shift; i++)
            if (transform_bit(value.magnitude, i)) {
                sticky = true;
                break;
            }
        if (transform_bit(value.magnitude, shift - 1) && (sticky || (significand & 1)))
            significand++;
    }
    double result = scalbn((double)significand, TRANSFORM_BASE + (int)shift);
    return value.sign < 0 ? -result : result;
}
#endif
