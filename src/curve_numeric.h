#ifndef PS_CURVE_NUMERIC_H
#define PS_CURVE_NUMERIC_H
#include "transform_numeric.h"
/* Exact cubic binary64 polynomials: at most four significands (212 bits),
 * small integer coefficients and exponents from -4296 through 1030. */
#define CURVE_WORDS 168
#define CURVE_BASE (-4296)
static inline void curve_product_multiply(uint32_t value[8], uint64_t factor) {
    uint32_t out[8] = {0};
    uint32_t parts[2] = {(uint32_t)factor, (uint32_t)(factor >> 32)};
    for (unsigned j = 0; j < 2; j++) {
        uint64_t carry = 0;
        for (unsigned i = 0; i + j < 8; i++) {
            uint64_t next = (uint64_t)value[i] * parts[j] + out[i + j] + carry;
            out[i + j] = (uint32_t)next;
            carry = next >> 32;
        }
    }
    memcpy(value, out, sizeof out);
}
static inline void curve_add_term(uint32_t sum[CURVE_WORDS], double point,
                                  transform_binary t, unsigned power, unsigned coefficient) {
    if (point == 0 || !coefficient || (power && !t.significand))
        return;
    transform_binary p = transform_parts(point);
    uint32_t product[8] = {(uint32_t)p.significand, (uint32_t)(p.significand >> 32)};
    for (unsigned i = 0; i < power; i++)
        curve_product_multiply(product, t.significand);
    curve_product_multiply(product, coefficient);
    unsigned position = (unsigned)(p.exponent + (int)power * t.exponent - CURVE_BASE);
    unsigned word = position / 32, shift = position % 32, count = 8;
    while (count && !product[count - 1])
        count--;
    uint64_t carry = 0;
    for (unsigned i = 0; i < count; i++) {
        uint64_t next = ((uint64_t)product[i] << shift) + carry + sum[word + i];
        sum[word + i] = (uint32_t)next;
        carry = next >> 32;
    }
    word += count;
    while (carry) {
        uint64_t next = (uint64_t)sum[word] + carry;
        sum[word++] = (uint32_t)next;
        carry = next >> 32;
    }
}
static inline unsigned curve_bit(const uint32_t *value, size_t bit) {
    return (value[bit / 32] >> (bit % 32)) & 1u;
}
static inline size_t curve_top(const uint32_t *value) {
    size_t word = CURVE_WORDS;
    while (word && !value[word - 1])
        word--;
    if (!word)
        return 0;
    unsigned bits = 32;
    while (!((value[word - 1] >> (bits - 1)) & 1u))
        bits--;
    return (word - 1) * 32 + bits;
}
static inline double curve_polynomial(const double points[4], double parameter,
                                      const int coefficients[4][4], unsigned degree) {
    uint32_t positive[CURVE_WORDS] = {0}, negative[CURVE_WORDS] = {0};
    transform_binary t = transform_parts(parameter);
    for (unsigned power = 0; power <= degree; power++)
        for (unsigned i = 0; i < 4; i++) {
            int coefficient = coefficients[power][i];
            bool minus = (coefficient < 0) != (points[i] < 0);
            curve_add_term(minus ? negative : positive, points[i], t, power,
                           (unsigned)(coefficient < 0 ? -coefficient : coefficient));
        }
    int sign = 0;
    for (size_t i = CURVE_WORDS; i > 0; i--)
        if (positive[i - 1] != negative[i - 1]) {
            sign = positive[i - 1] > negative[i - 1] ? 1 : -1;
            break;
        }
    if (!sign)
        return 0;
    uint32_t *large = sign > 0 ? positive : negative, *small = sign > 0 ? negative : positive;
    uint64_t borrow = 0;
    for (size_t i = 0; i < CURVE_WORDS; i++) {
        uint64_t sub = (uint64_t)small[i] + borrow, current = large[i];
        large[i] = (uint32_t)(current - sub);
        borrow = current < sub;
    }
    size_t top = curve_top(large), shift = top > 53 ? top - 53 : 0;
    if (shift < (size_t)(-1074 - CURVE_BASE))
        shift = (size_t)(-1074 - CURVE_BASE);
    uint64_t significand = 0;
    for (size_t i = top; i > shift; i--)
        significand = (significand << 1) | curve_bit(large, i - 1);
    bool sticky = false;
    size_t words = (shift - 1) / 32;
    unsigned bits = (unsigned)((shift - 1) % 32);
    for (size_t i = 0; i < words; i++)
        if (large[i]) {
            sticky = true;
            break;
        }
    if (bits && (large[words] & ((UINT32_C(1) << bits) - 1u)))
        sticky = true;
    if (curve_bit(large, shift - 1) && (sticky || (significand & 1)))
        significand++;
    double result = scalbn((double)significand, CURVE_BASE + (int)shift);
    return sign < 0 ? -result : result;
}
#endif
