#ifndef PHYSIM_PROPERTIES_H
#define PHYSIM_PROPERTIES_H
#include "units.h"
#define PS_PROPERTY_MAX_AXIS 64u
/* Caller-owned property data in SI. Strings/arrays/unit symbol must outlive all
 * evaluations. Keep them unchanged while evaluating; synchronize shared mutation.
 * No allocation, implicit catalog or material selection. Domain is
 * closed, Kelvin >= 0, Pascal >= 0. value_unit must have scale 1.
 * Constant uses constant_value_si; table uses 1..64 strictly increasing finite
 * axes and temperature-major values (pressure varies fastest).
 * A one-point axis means independence of that coordinate throughout the domain;
 * its reference coordinate must lie inside the domain. Multi-point axes must
 * cover the declared domain. No extrapolation, phase transitions or uncertainty
 * model is inferred. Negative property values are permitted; each consuming
 * physical model validates the properties it actually needs. */
typedef enum { PS_PROPERTY_CONSTANT, PS_PROPERTY_TABLE } ps_property_model;
typedef struct {
    ps_property_model model;
    const char *name, *source; /* optional, bounded UTF-8; NULL means absent */
    ps_unit value_unit;
    double minimum_temperature_k, maximum_temperature_k;
    double minimum_pressure_pa, maximum_pressure_pa;
    double constant_value_si;
    const double *temperature_k, *pressure_pa, *values_si;
    size_t temperature_count, pressure_count;
} ps_property;
/* Invalid descriptor -> PS_INVALID, excessive axis size -> PS_LIMIT. */
ps_result ps_property_validate(const ps_property *property);
/* Bilinear interpolation (linear/constant on one-point axes), no extrapolation.
 * Nonfinite/out-of-domain query -> PS_INVALID; nonfinite arithmetic -> PS_NUMERIC.
 * All failures preserve out. Unit symbol is borrowed from the property. */
ps_result ps_property_evaluate(const ps_property *property,double temperature_k,
                                double pressure_pa,ps_quantity *out);
#endif
