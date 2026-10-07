#ifndef PHYSIM_LANGUAGE_THERMODYNAMICS_H
#define PHYSIM_LANGUAGE_THERMODYNAMICS_H
#include "language_runtime.h"
#include "thermodynamics.h"
/* Pure value bindings: source-site errors can be caught with attempt(). */
#define PSRT_THERMAL_THREE(name, call) \
    static inline double name(double a,double b,double c,psrt_site site) { \
        double value=0; if(call(a,b,c,&value)!=PS_OK) \
            psrt_fail(site,"Thermodynamics: invalid SI inputs or numeric range"); \
        return value; }
PSRT_THERMAL_THREE(psrt_gas_pressure,ps_ideal_gas_pressure)
PSRT_THERMAL_THREE(psrt_gas_volume,ps_ideal_gas_volume)
PSRT_THERMAL_THREE(psrt_gas_temperature,ps_ideal_gas_temperature)
PSRT_THERMAL_THREE(psrt_gas_energy,ps_ideal_gas_energy)
PSRT_THERMAL_THREE(psrt_sensible_heat,ps_sensible_heat)
PSRT_THERMAL_THREE(psrt_heat_flow,ps_heat_flow)
#undef PSRT_THERMAL_THREE
static inline double psrt_heat_capacity(double mass,double specific,psrt_site site) {
    double value=0;
    if(ps_heat_capacity(mass,specific,&value)!=PS_OK)
        psrt_fail(site,"Thermodynamics: invalid SI inputs or numeric range");
    return value;
}
static inline double psrt_gas_entropy(double n,double cv,double t0,double v0,double t1,double v1,psrt_site site) {
    double value=0;
    if(ps_ideal_gas_entropy_change(n,cv,t0,v0,t1,v1,&value)!=PS_OK)
        psrt_fail(site,"Thermodynamics: invalid SI inputs or numeric range");
    return value;
}
static inline double psrt_thermal_reservoir(double capacity,double temperature,double reservoir,
                                           double conductance,double dt,psrt_site site) {
    double value=0;
    if(ps_thermal_reservoir_step(capacity,temperature,reservoir,conductance,dt,&value)!=PS_OK)
        psrt_fail(site,"Thermodynamics: invalid SI inputs or numeric range");
    return value;
}
static inline ps_vec2 psrt_thermal_pair(double ca,double ta,double cb,double tb,
                                       double conductance,double dt,psrt_site site) {
    ps_vec2 value={0};
    if(ps_thermal_pair_step(ca,ta,cb,tb,conductance,dt,&value)!=PS_OK)
        psrt_fail(site,"Thermodynamics: invalid SI inputs or numeric range");
    return value;
}
#endif
