#include "physim/units.h"
#include <math.h>
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
static ps_result sum(ps_quantity a, ps_quantity b, int sign, ps_quantity *out) {
    if (!out || !isfinite(a.value))
        return PS_INVALID;
    double converted;
    ps_result status = ps_convert(b.value, b.unit, a.unit, &converted);
    if (status != PS_OK)
        return status;
    ps_quantity r = {a.value + sign * converted, a.unit};
    if (!isfinite(r.value))
        return PS_NUMERIC;
    *out = r;
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
