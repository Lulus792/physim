#ifndef PS_BODY_NUMERIC_H
#define PS_BODY_NUMERIC_H
#include "transform_numeric.h"
/* Positive inertia/energy products: up to five significands, including two
 * bounded normalized angular components. Exponents -5370..3076, no heap. */
#define BODY_WORDS 280
#define BODY_BASE (-5372)
static inline void body_multiply_words(uint32_t value[16], uint64_t factor) {
    uint32_t result[16] = {0};
    uint32_t parts[2] = {(uint32_t)factor, (uint32_t)(factor >> 32)};
    for (unsigned j = 0; j < 2; j++) {
        uint64_t carry = 0;
        for (unsigned i = 0; i + j < 16; i++) {
            uint64_t next = (uint64_t)value[i] * parts[j] + result[i + j] + carry;
            result[i + j] = (uint32_t)next;
            carry = next >> 32;
        }
    }
    memcpy(value, result, sizeof result);
}
static inline void body_add_product(uint32_t sum[BODY_WORDS], const double *values,
                                    unsigned count, unsigned coefficient) {
    uint32_t product[16] = {coefficient};
    int exponent = 0;
    for (unsigned i = 0; i < count; i++) {
        if (values[i] == 0)
            return;
        transform_binary p = transform_parts(values[i]);
        body_multiply_words(product, p.significand);
        exponent += p.exponent;
    }
    unsigned position = (unsigned)(exponent - BODY_BASE);
    unsigned word = position / 32, shift = position % 32, length = 16;
    while (length && !product[length - 1])
        length--;
    uint64_t carry = 0;
    for (unsigned i = 0; i < length; i++) {
        uint64_t next = ((uint64_t)product[i] << shift) + carry + sum[word + i];
        sum[word + i] = (uint32_t)next;
        carry = next >> 32;
    }
    word += length;
    while (carry) {
        uint64_t next = (uint64_t)sum[word] + carry;
        sum[word++] = (uint32_t)next;
        carry = next >> 32;
    }
}
static inline unsigned body_bit(const uint32_t *value, size_t bit) {
    return (value[bit / 32] >> (bit % 32)) & 1u;
}
/* Signed momentum terms use the same exact product lattice. Rotation factors
 * are bounded; four factors and cancellation fit without intermediate overflow. */
static inline void body_add_signed_product(uint32_t positive[BODY_WORDS],
                                           uint32_t negative[BODY_WORDS],
                                           const double *values, unsigned count, bool minus) {
    for (unsigned i = 0; i < count; i++)
        minus ^= signbit(values[i]) != 0;
    body_add_product(minus ? negative : positive, values, count, 1);
}
/* Exact division by the small analytic denominator, followed by one rounding.
 * The division remainder breaks ties below the integer accumulation lattice. */
static inline double body_value(uint32_t sum[BODY_WORDS], unsigned denominator) {
    uint64_t remainder = 0;
    for (size_t i = BODY_WORDS; i > 0; i--) {
        uint64_t next = (remainder << 32) | sum[i - 1];
        sum[i - 1] = (uint32_t)(next / denominator);
        remainder = next % denominator;
    }
    size_t word = BODY_WORDS;
    while (word && !sum[word - 1])
        word--;
    if (!word)
        return 0;
    unsigned bits = 32;
    while (!((sum[word - 1] >> (bits - 1)) & 1u))
        bits--;
    size_t top = (word - 1) * 32 + bits;
    size_t shift = top > 53 ? top - 53 : 0;
    if (shift < (size_t)(-1074 - BODY_BASE))
        shift = (size_t)(-1074 - BODY_BASE);
    uint64_t significand = 0;
    for (size_t i = top; i > shift; i--)
        significand = (significand << 1) | body_bit(sum, i - 1);
    bool sticky = remainder != 0;
    size_t words = (shift - 1) / 32;
    bits = (unsigned)((shift - 1) % 32);
    for (size_t i = 0; i < words; i++)
        if (sum[i]) {
            sticky = true;
            break;
        }
    if (bits && (sum[words] & ((UINT32_C(1) << bits) - 1u)))
        sticky = true;
    if (body_bit(sum, shift - 1) && (sticky || (significand & 1)))
        significand++;
    return scalbn((double)significand, BODY_BASE + (int)shift);
}
static inline double body_signed_value(uint32_t positive[BODY_WORDS],
                                       uint32_t negative[BODY_WORDS]) {
    int comparison = 0;
    for (size_t i = BODY_WORDS; i > 0; i--)
        if (positive[i - 1] != negative[i - 1]) {
            comparison = positive[i - 1] > negative[i - 1] ? 1 : -1;
            break;
        }
    if (!comparison)
        return 0;
    uint32_t *large = comparison > 0 ? positive : negative;
    uint32_t *small = comparison > 0 ? negative : positive;
    uint64_t borrow = 0;
    for (size_t i = 0; i < BODY_WORDS; i++) {
        uint64_t sub = (uint64_t)small[i] + borrow;
        borrow = (uint64_t)large[i] < sub;
        large[i] = (uint32_t)((uint64_t)large[i] - sub);
    }
    double value = body_value(large, 1);
    return comparison < 0 ? -value : value;
}
static inline double body_inertia(double mass, double a, double b, bool sphere) {
    uint32_t sum[BODY_WORDS] = {0};
    double factors[3] = {mass, a, a};
    body_add_product(sum, factors, 3, sphere ? 2u : 1u);
    if (!sphere) {
        factors[1] = factors[2] = b;
        body_add_product(sum, factors, 3, 1);
    }
    return body_value(sum, sphere ? 5u : 12u);
}
#endif
