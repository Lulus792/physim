#ifndef PHYSIM_LANGUAGE_ELECTROMAGNETISM_H
#define PHYSIM_LANGUAGE_ELECTROMAGNETISM_H
#include "language_runtime.h"
#include "electromagnetism.h"
/* Pure SI bindings; attempt() catches source-site failures. */
static inline double psrt_vacuum_permittivity(psrt_site site) { (void)site;return PS_VACUUM_PERMITTIVITY; }
static inline ps_vec3 psrt_charge_field(double q,ps_vec3 source,ps_vec3 point,double epsilon,psrt_site site) {
    ps_vec3 value={0};ps_result r=ps_point_charge_field(q,source,point,epsilon,&value);
    if(r==PS_SINGULAR)psrt_fail(site,"Point charge field is singular at its source");
    if(r!=PS_OK)psrt_fail(site,"Electromagnetism: invalid SI inputs or numeric range");
    return value;
}
static inline double psrt_charge_potential(double q,ps_vec3 source,ps_vec3 point,double epsilon,psrt_site site) {
    double value=0;ps_result r=ps_point_charge_potential(q,source,point,epsilon,&value);
    if(r==PS_SINGULAR)psrt_fail(site,"Point charge potential is singular at its source");
    if(r!=PS_OK)psrt_fail(site,"Electromagnetism: invalid SI inputs or numeric range");
    return value;
}
static inline ps_vec3 psrt_lorentz_force(double q,ps_vec3 electric,ps_vec3 velocity,ps_vec3 magnetic,psrt_site site) {
    ps_vec3 value={0};if(ps_lorentz_force(q,electric,velocity,magnetic,&value)!=PS_OK)
        psrt_fail(site,"Electromagnetism: invalid SI inputs or numeric range");return value;
}
#define PSRT_ELECTRIC_TWO(name,call) \
    static inline double name(double a,double b,psrt_site site) { \
        double value=0;if(call(a,b,&value)!=PS_OK) \
            psrt_fail(site,"Electromagnetism: invalid SI inputs or numeric range");return value; }
PSRT_ELECTRIC_TWO(psrt_resistor_current,ps_resistor_current)
PSRT_ELECTRIC_TWO(psrt_resistor_voltage,ps_resistor_voltage)
PSRT_ELECTRIC_TWO(psrt_resistor_power,ps_resistor_power)
PSRT_ELECTRIC_TWO(psrt_resistance_series,ps_resistance_series)
PSRT_ELECTRIC_TWO(psrt_resistance_parallel,ps_resistance_parallel)
PSRT_ELECTRIC_TWO(psrt_capacitor_energy,ps_capacitor_energy)
#undef PSRT_ELECTRIC_TWO
static inline double psrt_rc_step(double resistance,double capacitance,double voltage,double source,double dt,psrt_site site) {
    double value=0;if(ps_rc_voltage_step(resistance,capacitance,voltage,source,dt,&value)!=PS_OK)
        psrt_fail(site,"Electromagnetism: invalid SI inputs or numeric range");return value;
}
#endif
