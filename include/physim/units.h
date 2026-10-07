#ifndef PHYSIM_UNITS_H
#define PHYSIM_UNITS_H
#include "core.h"
extern const ps_unit PS_ONE, PS_AMPERE, PS_KELVIN, PS_MOLE, PS_CANDELA, PS_NEWTON, PS_PASCAL,
    PS_WATT, PS_HERTZ, PS_ACCELERATION;
bool ps_unit_valid(ps_unit unit);
bool ps_unit_compatible(ps_unit a, ps_unit b);
/* Algebra returns a borrowed symbol pointer (caller supplies its lifetime).
 * Exponent overflow and nonpositive/unrepresentable scales are rejected.
 * All checked operations leave output unchanged on failure. No affine units. */
ps_result ps_unit_multiply(ps_unit a, ps_unit b, const char *symbol, ps_unit *out);
ps_result ps_unit_divide(ps_unit a, ps_unit b, const char *symbol, ps_unit *out);
ps_result ps_unit_power(ps_unit a, int power, const char *symbol, ps_unit *out);
typedef struct {
    double value;
    ps_unit unit;
} ps_quantity;
ps_result ps_quantity_convert(ps_quantity value, ps_unit target, ps_quantity *out);
/* Addition/subtraction return a's unit and combine normalized operands before
 * final scaling, including when b alone cannot be represented in a's unit.
 * Assumes default round-to-nearest/ties-to-even; compensated terms retain
 * conversion/sum residuals and subnormals round once on their final lattice.
 * Exact normalized cancellation may return zero; a nonzero normalized result
 * rounding to zero or a nonfinite final result yields PS_NUMERIC. Double
 * rounding still applies, especially near cancellation; no exact-arithmetic
 * guarantee. Inputs/output may alias. Product/quotient compose dimensions. */
ps_result ps_quantity_add(ps_quantity a, ps_quantity b, ps_quantity *out);
ps_result ps_quantity_subtract(ps_quantity a, ps_quantity b, ps_quantity *out);
ps_result ps_quantity_multiply(ps_quantity a, ps_quantity b, const char *symbol, ps_quantity *out);
ps_result ps_quantity_divide(ps_quantity a, ps_quantity b, const char *symbol, ps_quantity *out);
/* Canonical SI dimension spelling; ignores unit scale and custom symbol. */
ps_result ps_unit_format_dimension(ps_unit unit, char *text, size_t capacity);
#endif
