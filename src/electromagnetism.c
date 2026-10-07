#include "physim/electromagnetism.h"
#include <math.h>
static bool positive(double x) { return isfinite(x) && x>0; }
static bool finite3(ps_vec3 v) { return isfinite(v.x) && isfinite(v.y) && isfinite(v.z); }
/* Binary normalization avoids intermediate overflow even on MSVC, where
 * long double has no wider exponent range. Factors are finite; divisors nonzero. */
static double ratio(const double *n,size_t nc,const double *d,size_t dc) {
    double m=1;int exponent=0;
    for(size_t i=0;i<nc;i++){if(n[i]==0)return 0;int e;m*=frexp(n[i],&e);exponent+=e;}
    for(size_t i=0;i<dc;i++){int e;m/=frexp(d[i],&e);exponent-=e;}
    return scalbn(m,exponent);
}
static ps_result scalar(double value,bool require_positive,double *out) {
    if(!isfinite(value) || (require_positive && value<=0))return PS_NUMERIC;
    *out=value;return PS_OK;
}
/* delta=factor*offset. A halved subtraction handles opposite huge coordinates
 * without losing nearby-coordinate subtraction precision on the ordinary path. */
static ps_result separation(ps_vec3 source,ps_vec3 point,ps_vec3 *offset,double *scale,
                              double *length,double *factor) {
    if(!finite3(source) || !finite3(point))return PS_INVALID;
    *offset=ps_vsub(point,source);*factor=1;
    if(!finite3(*offset)) {
        *offset=ps_v3(point.x*.5-source.x*.5,point.y*.5-source.y*.5,point.z*.5-source.z*.5);
        *factor=2;
    }
    *scale=fmax(fabs(offset->x),fmax(fabs(offset->y),fabs(offset->z)));
    if(*scale==0)return PS_SINGULAR;
    ps_vec3 unit=ps_v3(offset->x/ *scale,offset->y/ *scale,offset->z/ *scale);
    *length=hypot(hypot(unit.x,unit.y),unit.z);
    return PS_OK;
}
ps_result ps_point_charge_field(double q,ps_vec3 source,ps_vec3 point,double epsilon,ps_vec3 *out) {
    if(!out || !isfinite(q) || !positive(epsilon))return PS_INVALID;
    ps_vec3 delta;double scale,length,factor;
    ps_result r=separation(source,point,&delta,&scale,&length,&factor);if(r!=PS_OK)return r;
    double denominator[]={4*PS_PI,epsilon,scale,scale,scale,length,length,length,factor,factor};
    double nx[]={q,delta.x},ny[]={q,delta.y},nz[]={q,delta.z};
    ps_vec3 result=ps_v3(ratio(nx,2,denominator,10),ratio(ny,2,denominator,10),ratio(nz,2,denominator,10));
    if(!finite3(result))return PS_NUMERIC;
    *out=result;return PS_OK;
}
ps_result ps_point_charge_potential(double q,ps_vec3 source,ps_vec3 point,double epsilon,double *out) {
    if(!out || !isfinite(q) || !positive(epsilon))return PS_INVALID;
    ps_vec3 delta;double scale,length,factor;
    ps_result r=separation(source,point,&delta,&scale,&length,&factor);if(r!=PS_OK)return r;
    double n[]={q},d[]={4*PS_PI,epsilon,scale,length,factor};
    return scalar(ratio(n,1,d,5),false,out);
}
typedef struct { double m;int e; } scaled;
static scaled term(double a,double b) {
    int ea,eb;double ma=frexp(a,&ea),mb=frexp(b,&eb);return (scaled){ma*mb,ea+eb};
}
static double force_component(double q,double e,double va,double bb,double vb,double ba) {
    scaled terms[]={term(e,1),term(va,bb),term(-vb,ba)};
    /* Add largest exponents first, renormalizing after cancellation before the
     * smaller electric term. Zero products do not impose an irrelevant exponent. */
    for(unsigned i=0;i<3;i++)for(unsigned j=i+1;j<3;j++)
        if((terms[j].m!=0 && terms[i].m==0) || (terms[j].m!=0 && terms[j].e>terms[i].e)) {
            scaled swap=terms[i];terms[i]=terms[j];terms[j]=swap;
        }
    scaled sum=terms[0];
    for(unsigned i=1;i<3;i++) {
        if(terms[i].m==0)continue;
        if(sum.m==0){sum=terms[i];continue;}
        int exponent=sum.e>terms[i].e?sum.e:terms[i].e;
        double value=scalbn(sum.m,sum.e-exponent)+scalbn(terms[i].m,terms[i].e-exponent);
        int extra;sum.m=frexp(value,&extra);sum.e=exponent+extra;
    }
    int eq;double mq=frexp(q,&eq);return scalbn(sum.m*mq,sum.e+eq);
}
ps_result ps_lorentz_force(double q,ps_vec3 e,ps_vec3 v,ps_vec3 b,ps_vec3 *out) {
    if(!out || !isfinite(q) || !finite3(e) || !finite3(v) || !finite3(b))return PS_INVALID;
    ps_vec3 result=ps_v3(force_component(q,e.x,v.y,b.z,v.z,b.y),
                         force_component(q,e.y,v.z,b.x,v.x,b.z),
                         force_component(q,e.z,v.x,b.y,v.y,b.x));
    if(!finite3(result))return PS_NUMERIC;*out=result;return PS_OK;
}
ps_result ps_resistor_current(double v,double resistance,double *out) {
    if(!out || !isfinite(v) || !positive(resistance))return PS_INVALID;
    double n[]={v},d[]={resistance};return scalar(ratio(n,1,d,1),false,out);
}
ps_result ps_resistor_voltage(double current,double resistance,double *out) {
    if(!out || !isfinite(current) || !positive(resistance))return PS_INVALID;
    double n[]={current,resistance};return scalar(ratio(n,2,NULL,0),false,out);
}
ps_result ps_resistor_power(double v,double resistance,double *out) {
    if(!out || !isfinite(v) || !positive(resistance))return PS_INVALID;
    double n[]={v,v},d[]={resistance};return scalar(ratio(n,2,d,1),false,out);
}
ps_result ps_resistance_series(double a,double b,double *out) {
    if(!out || !positive(a) || !positive(b))return PS_INVALID;
    return scalar(a+b,true,out);
}
ps_result ps_resistance_parallel(double a,double b,double *out) {
    if(!out || !positive(a) || !positive(b))return PS_INVALID;
    return scalar(fmin(a,b)/(1+fmin(a,b)/fmax(a,b)),true,out);
}
ps_result ps_capacitor_energy(double capacitance,double v,double *out) {
    if(!out || !positive(capacitance) || !isfinite(v))return PS_INVALID;
    double n[]={.5,capacitance,v,v};return scalar(ratio(n,4,NULL,0),false,out);
}
ps_result ps_rc_voltage_step(double resistance,double capacitance,double v,double source,double dt,double *out) {
    if(!out || !positive(resistance) || !positive(capacitance) || !isfinite(v) ||
        !isfinite(source) || !isfinite(dt) || dt<0)return PS_INVALID;
    if(dt==0 || v==source){*out=v;return PS_OK;}
    double n[]={dt},d[]={resistance,capacitance};double rate=ratio(n,1,d,2);
    double fraction=-expm1(-rate);
    if(rate<1e-8) {
        /* A tiny rate can underflow while a huge source still causes a finite
         * change. Normalize source and initial contributions independently. */
        double ns[]={source,dt},nv[]={v,dt};double phi=rate==0?1:fraction/rate;
        return scalar(v+(ratio(ns,2,d,2)-ratio(nv,2,d,2))*phi,false,out);
    }
    double value=(signbit(v)==signbit(source) && fabs(source)>=fabs(v)*.5)
        ? fma(source-v,fraction,v):v*exp(-rate)+source*fraction;
    return scalar(value,false,out);
}
