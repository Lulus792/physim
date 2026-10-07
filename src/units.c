#include "physim/units.h"
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <string.h>

const ps_unit PS_ONE = {{0}, 1, "1"}, PS_AMPERE = {{0, 0, 0, 1, 0, 0, 0}, 1, "A"},
              PS_KELVIN = {{0, 0, 0, 0, 1, 0, 0}, 1, "K"},
              PS_MOLE = {{0, 0, 0, 0, 0, 1, 0}, 1, "mol"},
              PS_CANDELA = {{0, 0, 0, 0, 0, 0, 1}, 1, "cd"},
              PS_NEWTON = {{1, 1, -2, 0, 0, 0, 0}, 1, "N"},
              PS_PASCAL = {{-1, 1, -2, 0, 0, 0, 0}, 1, "Pa"},
              PS_WATT = {{2, 1, -3, 0, 0, 0, 0}, 1, "W"},
              PS_HERTZ = {{0, 0, -1, 0, 0, 0, 0}, 1, "Hz"},
              PS_ACCELERATION = {{1, 0, -2, 0, 0, 0, 0}, 1, "m/s^2"};
bool ps_unit_valid(ps_unit u) { return isfinite(u.scale) && u.scale > 0; }
bool ps_unit_compatible(ps_unit a, ps_unit b) {
    return ps_unit_valid(a) && ps_unit_valid(b) && !memcmp(a.dimension, b.dimension, 7);
}
static ps_result compose(ps_unit a, ps_unit b, int sign, const char *symbol, ps_unit *out) {
    if (!out || !ps_unit_valid(a) || !ps_unit_valid(b))
        return PS_INVALID;
    ps_unit r = {{0}, 0, symbol};
    for (int i = 0; i < 7; i++) {
        int e = a.dimension[i] + sign * b.dimension[i];
        if (e < INT8_MIN || e > INT8_MAX)
            return PS_INVALID;
        r.dimension[i] = (int8_t)e;
    }
    r.scale = sign > 0 ? a.scale * b.scale : a.scale / b.scale;
    if (!ps_unit_valid(r))
        return PS_NUMERIC;
    *out = r;
    return PS_OK;
}
ps_result ps_unit_multiply(ps_unit a, ps_unit b, const char *s, ps_unit *o) {
    return compose(a, b, 1, s, o);
}
ps_result ps_unit_divide(ps_unit a, ps_unit b, const char *s, ps_unit *o) {
    return compose(a, b, -1, s, o);
}
ps_result ps_unit_power(ps_unit a, int power, const char *symbol, ps_unit *out) {
    if (!out || !ps_unit_valid(a))
        return PS_INVALID;
    ps_unit r = {{0}, 0, symbol};
    for (int i = 0; i < 7; i++) {
        int64_t e = (int64_t)a.dimension[i] * power;
        if (e < INT8_MIN || e > INT8_MAX)
            return PS_INVALID;
        r.dimension[i] = (int8_t)e;
    }
    r.scale = pow(a.scale, power);
    if (!ps_unit_valid(r))
        return PS_NUMERIC;
    *out = r;
    return PS_OK;
}
ps_result ps_quantity_convert(ps_quantity a, ps_unit target, ps_quantity *out) {
    if (!out)
        return PS_INVALID;
    ps_quantity r = {0, target};
    ps_result status = ps_convert(a.value, a.unit, target, &r.value);
    if (status == PS_OK)
        *out = r;
    return status;
}
/* TwoSum retains the low part of addition in round-to-nearest arithmetic.
 * Ogita/Rump/Oishi (2005), Algorithm 3.1. No overflow in normalized terms. */
static void quantity_two_sum(double a, double b, double *high, double *low) {
    double sum = a + b, z = sum - a;
    *low = (a - (sum - z)) + (b - z);
    *high = sum;
}
static double quantity_rescale(double high, double low, int exponent) {
    double magnitude = fabs(scalbn(high, exponent));
    if (high != 0 && magnitude <= DBL_MIN) {
        /* Round once on the subnormal lattice. A tiny low part must break a
         * half-way tie even if adding it to the high part would round away. */
        int shift = exponent - (DBL_MIN_EXP - DBL_MANT_DIG);
        double grid = scalbn(fabs(high), shift),
               tail = scalbn(signbit(high) ? -low : low, shift);
        double integral = floor(grid), delta = (grid - integral - .5) + tail;
        if (delta > 0 || (delta == 0 && fmod(integral, 2) != 0)) integral += 1;
        return copysign(scalbn(integral, DBL_MIN_EXP - DBL_MANT_DIG), high);
    }
    return scalbn(high + low, exponent);
}
static ps_result sum(ps_quantity a, ps_quantity b, int sign, ps_quantity *out) {
    if (!out || !isfinite(a.value) || !isfinite(b.value) ||
        !ps_unit_compatible(a.unit, b.unit))
        return PS_INVALID;
    double value;
    if (b.value == 0) {
        /* Preserve the established signed-zero behavior. */
        value = a.value + sign * b.value;
    } else {
        /* Convert in normalized binary form and add before final scaling.
         * The converted operand may exceed Double or lie below its smallest
         * subnormal even when the final sum is representable. No wider type. */
        int ea, eb, esb, esa;
        double ma = frexp(a.value, &ea), mb = frexp(b.value, &eb),
               msb = frexp(b.unit.scale, &esb), msa = frexp(a.unit.scale, &esa);
        double product = mb * msb, product_error = fma(mb, msb, -product);
        double converted = product / msa;
        double conversion_error = (fma(-converted, msa, product) + product_error) / msa;
        int converted_exponent = eb + esb - esa;
        if (a.value == 0) ea = converted_exponent;
        int exponent = ea > converted_exponent ? ea : converted_exponent;
        double high, low;
        quantity_two_sum(scalbn(ma, ea - exponent),
                         sign * scalbn(converted, converted_exponent - exponent), &high, &low);
        low += sign * scalbn(conversion_error, converted_exponent - exponent);
        quantity_two_sum(high, low, &high, &low);
        value = quantity_rescale(high, low, exponent);
        if (!isfinite(value) || (value == 0 && (high != 0 || low != 0)))
            return PS_NUMERIC;
    }
    *out = (ps_quantity){value, a.unit};
    return PS_OK;
}

ps_result ps_quantity_add(ps_quantity a, ps_quantity b, ps_quantity *o) { return sum(a, b, 1, o); }
ps_result ps_quantity_subtract(ps_quantity a, ps_quantity b, ps_quantity *o) {
    return sum(a, b, -1, o);
}
static ps_result product(ps_quantity a, ps_quantity b, int sign, const char *s, ps_quantity *out) {
    if (!out || !isfinite(a.value) || !isfinite(b.value) || (sign < 0 && b.value == 0))
        return PS_INVALID;
    ps_quantity r;
    ps_result status = compose(a.unit, b.unit, sign, s, &r.unit);
    if (status != PS_OK)
        return status;
    r.value = sign > 0 ? a.value * b.value : a.value / b.value;
    if (!isfinite(r.value) || (r.value == 0 && a.value != 0 && b.value != 0))
        return PS_NUMERIC;
    *out = r;
    return PS_OK;
}
ps_result ps_quantity_multiply(ps_quantity a, ps_quantity b, const char *s, ps_quantity *o) {
    return product(a, b, 1, s, o);
}
ps_result ps_quantity_divide(ps_quantity a, ps_quantity b, const char *s, ps_quantity *o) {
    return product(a, b, -1, s, o);
}
ps_result ps_unit_format_dimension(ps_unit u, char *text, size_t capacity) {
    if (!text || !ps_unit_valid(u))
        return PS_INVALID;
    const char *names[] = {"m", "kg", "s", "A", "K", "mol", "cd"};
    char buffer[128] = {0};
    size_t used = 0;
    for (int i = 0; i < 7; i++)
        if (u.dimension[i]) {
            int n = u.dimension[i] == 1 ? snprintf(buffer + used, sizeof buffer - used, "%s%s",
                                                   used ? " " : "", names[i])
                                        : snprintf(buffer + used, sizeof buffer - used, "%s%s^%d",
                                                   used ? " " : "", names[i], u.dimension[i]);
            if (n < 0 || (size_t)n >= sizeof buffer - used)
                return PS_LIMIT;
            used += (size_t)n;
        }
    if (!used) {
        buffer[0] = '1';
        buffer[1] = 0;
        used = 1;
    }
    if (capacity <= used)
        return PS_LIMIT;
    memcpy(text, buffer, used + 1);
    return PS_OK;
}
