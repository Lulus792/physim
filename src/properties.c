#include "physim/properties.h"
#include "text_validation.h"
#include <math.h>
static bool domain(double lo,double hi) {
    return isfinite(lo) && isfinite(hi) && lo<=hi && lo>=0;
}
static bool axis(const double *values,size_t n,double lo,double hi) {
    if(!values)return false;
    for(size_t i=0;i<n;i++)if(!isfinite(values[i]) || values[i]<0 ||
        (i && values[i]<=values[i-1]))return false;
    return n==1?(values[0]>=lo && values[0]<=hi):(values[0]<=lo && values[n-1]>=hi);
}
ps_result ps_property_validate(const ps_property *p) {
    if(!p || !ps_unit_valid(p->value_unit) || p->value_unit.scale!=1 ||
       !domain(p->minimum_temperature_k,p->maximum_temperature_k) ||
       !domain(p->minimum_pressure_pa,p->maximum_pressure_pa) ||
       (p->name && !ps_text_valid(p->name,128,false)) ||
       (p->source && !ps_text_valid(p->source,1024,false)))return PS_INVALID;
    if(p->model==PS_PROPERTY_CONSTANT)return isfinite(p->constant_value_si)?PS_OK:PS_INVALID;
    if(p->model!=PS_PROPERTY_TABLE || !p->temperature_count || !p->pressure_count)return PS_INVALID;
    if(p->temperature_count>PS_PROPERTY_MAX_AXIS || p->pressure_count>PS_PROPERTY_MAX_AXIS)return PS_LIMIT;
    if(!p->values_si || !axis(p->temperature_k,p->temperature_count,p->minimum_temperature_k,p->maximum_temperature_k) ||
       !axis(p->pressure_pa,p->pressure_count,p->minimum_pressure_pa,p->maximum_pressure_pa))return PS_INVALID;
    for(size_t i=0;i<p->temperature_count*p->pressure_count;i++)if(!isfinite(p->values_si[i]))return PS_INVALID;
    return PS_OK;
}
static size_t interval(const double *x,size_t n,double value,double *fraction) {
    if(n==1){*fraction=0;return 0;}
    size_t i=0;while(i+2<n && value>x[i+1])i++;
    *fraction=(value-x[i])/(x[i+1]-x[i]);return i;
}
static double blend(double a,double b,double t) {
    if(t==0)return a;if(t==1)return b;
    double value=signbit(a)!=signbit(b)?(1-t)*a+t*b:a+t*(b-a);
    return fmax(fmin(a,b),fmin(fmax(a,b),value));
}
ps_result ps_property_evaluate(const ps_property *p,double temperature,double pressure,ps_quantity *out) {
    if(!out)return PS_INVALID;
    ps_result r=ps_property_validate(p);if(r!=PS_OK)return r;
    if(!isfinite(temperature) || !isfinite(pressure) || temperature<p->minimum_temperature_k ||
       temperature>p->maximum_temperature_k || pressure<p->minimum_pressure_pa || pressure>p->maximum_pressure_pa)return PS_INVALID;
    double value=p->constant_value_si;
    if(p->model==PS_PROPERTY_TABLE) {
        double t,w;size_t i=interval(p->temperature_k,p->temperature_count,temperature,&t),
                         j=interval(p->pressure_pa,p->pressure_count,pressure,&w);
        size_t i1=i+(p->temperature_count>1),j1=j+(p->pressure_count>1),n=p->pressure_count;
        value=blend(blend(p->values_si[i*n+j],p->values_si[i*n+j1],w),
                    blend(p->values_si[i1*n+j],p->values_si[i1*n+j1],w),t);
    }
    if(!isfinite(value))return PS_NUMERIC;
    *out=(ps_quantity){value,p->value_unit};return PS_OK;
}
