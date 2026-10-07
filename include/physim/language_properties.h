#ifndef PHYSIM_LANGUAGE_PROPERTIES_H
#define PHYSIM_LANGUAGE_PROPERTIES_H
#include "language_runtime.h"
#include "properties.h"
#include "math.h"
static inline ps_property psrt_property_descriptor(const char *name,const char *source,ps_unit unit,ps_vec4 domain) {
    ps_property p={0};p.name=name;p.source=source;p.value_unit=unit;
    p.minimum_temperature_k=domain.x;p.maximum_temperature_k=domain.y;
    p.minimum_pressure_pa=domain.z;p.maximum_pressure_pa=domain.w;return p;
}
static inline ps_quantity psrt_property_constant(const char *name,const char *source,ps_quantity value,
    ps_vec4 domain,double temperature,double pressure,psrt_site site) {
    ps_property p=psrt_property_descriptor(name,source,value.unit,domain);p.constant_value_si=value.value;
    ps_quantity result;ps_result r=ps_property_evaluate(&p,temperature,pressure,&result);
    if(r!=PS_OK)psrt_raise(site,r,"Material property: invalid SI data or query outside validity domain",NULL);
    return result;
}
static inline ps_quantity psrt_property_table(const char *name,const char *source,ps_unit unit,ps_vec4 domain,
    const double *temperatures,size_t nt,const double *pressures,size_t np,const double *values,size_t nv,
    double temperature,double pressure,psrt_site site) {
    if(!nt || !np || nt>PS_PROPERTY_MAX_AXIS || np>PS_PROPERTY_MAX_AXIS || nt*np!=nv)
        psrt_raise(site,nt>PS_PROPERTY_MAX_AXIS || np>PS_PROPERTY_MAX_AXIS?PS_LIMIT:PS_INVALID,"Material property: table shape or axis limit is invalid",NULL);
    ps_property p=psrt_property_descriptor(name,source,unit,domain);p.model=PS_PROPERTY_TABLE;
    p.temperature_k=temperatures;p.pressure_pa=pressures;p.values_si=values;p.temperature_count=nt;p.pressure_count=np;
    ps_quantity result;ps_result r=ps_property_evaluate(&p,temperature,pressure,&result);
    if(r!=PS_OK)psrt_raise(site,r,"Material property: invalid SI data or query outside validity domain",NULL);
    return result;
}
#endif
