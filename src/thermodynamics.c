#include "physim/thermodynamics.h"
#include <math.h>

static bool positive(double x) { return isfinite(x) && x > 0; }
static bool nonnegative(double x) { return isfinite(x) && x >= 0; }
/* Keep products/quotients in normalized binary form. This works identically
 * when long double equals double (MSVC) and avoids intermediate overflow. */
static double product(double a, double b, double c, double d) {
    if (a == 0 || b == 0 || c == 0) return 0;
    int ea, eb, ec, ed;
    double ma=frexp(a,&ea), mb=frexp(b,&eb), mc=frexp(c,&ec), md=frexp(d,&ed);
    return scalbn((ma*mb)*(mc/md),ea+eb+ec-ed);
}
static ps_result scalar(double value, bool strictly_positive, double *out) {
    if (!isfinite(value) || (strictly_positive && value <= 0)) return PS_NUMERIC;
    *out=value;
    return PS_OK;
}
ps_result ps_ideal_gas_pressure(double n, double t, double v, double *out) {
    if (!out || !positive(n) || !positive(t) || !positive(v)) return PS_INVALID;
    return scalar(product(n,PS_MOLAR_GAS_CONSTANT,t,v),true,out);
}
ps_result ps_ideal_gas_volume(double n, double t, double p, double *out) {
    if (!out || !positive(n) || !positive(t) || !positive(p)) return PS_INVALID;
    return scalar(product(n,PS_MOLAR_GAS_CONSTANT,t,p),true,out);
}
ps_result ps_ideal_gas_temperature(double n, double p, double v, double *out) {
    if (!out || !positive(n) || !positive(p) || !positive(v)) return PS_INVALID;
    return scalar(product(p,v,1/PS_MOLAR_GAS_CONSTANT,n),true,out);
}
ps_result ps_ideal_gas_energy(double n, double cv, double t, double *out) {
    if (!out || !positive(n) || !positive(cv) || !positive(t)) return PS_INVALID;
    return scalar(product(n,cv,t,1),true,out);
}
static double log_ratio(double final, double initial) {
    /* Preserve tiny relative changes; logs separately avoid overflowing ratios. */
    double difference=final-initial;
    if (fabs(difference) <= initial*.5) return log1p(difference/initial);
    return log(final)-log(initial);
}
ps_result ps_ideal_gas_entropy_change(double n, double cv, double t0, double v0,
                                      double t1, double v1, double *out) {
    if (!out || !positive(n) || !positive(cv) || !positive(t0) || !positive(v0) ||
        !positive(t1) || !positive(v1)) return PS_INVALID;
    double a=log_ratio(t1,t0), b=log_ratio(v1,v0);
    /* Normalize both signed terms before their sum and final n factor. This
     * retains tiny cv terms with huge n and finite cancellation of large terms. */
    int ec,ea,er,eb,en;
    double mc=frexp(cv,&ec),ma=frexp(a,&ea),mr=frexp(PS_MOLAR_GAS_CONSTANT,&er),
           mb=frexp(b,&eb),mn=frexp(n,&en);
    int e1=ec+ea,e2=er+eb;
    if(a==0)e1=e2;
    if(b==0)e2=e1;
    int exponent=e1>e2?e1:e2;
    double sum=scalbn(mc*ma,e1-exponent)+scalbn(mr*mb,e2-exponent);
    return scalar(scalbn(mn*sum,en+exponent),false,out);
}
ps_result ps_heat_capacity(double mass, double specific, double *out) {
    if (!out || !positive(mass) || !positive(specific)) return PS_INVALID;
    return scalar(product(mass,specific,1,1),true,out);
}
ps_result ps_sensible_heat(double capacity, double t0, double t1, double *out) {
    if (!out || !positive(capacity) || !positive(t0) || !positive(t1)) return PS_INVALID;
    return scalar(product(capacity,t1-t0,1,1),false,out);
}
ps_result ps_heat_flow(double conductance, double ta, double tb, double *out) {
    if (!out || !nonnegative(conductance) || !positive(ta) || !positive(tb)) return PS_INVALID;
    return scalar(product(conductance,ta-tb,1,1),false,out);
}
static double relax(double temperature, double target, double rate) {
    double fraction=-expm1(-rate);
    /* Close temperatures use a difference, distant cooling keeps the surviving
     * exponential term even if 1-exp(-rate) rounded to one. */
    if (target >= temperature*.5) return fma(target-temperature,fraction,temperature);
    return temperature*exp(-rate)+target*fraction;
}
ps_result ps_thermal_reservoir_step(double capacity, double t, double reservoir,
                                    double conductance, double dt, double *out) {
    if (!out || !positive(capacity) || !positive(t) || !positive(reservoir) ||
        !nonnegative(conductance) || !nonnegative(dt)) return PS_INVALID;
    double rate=product(conductance,dt,1,capacity);
    if (rate < 1e-8) {
        double phi=rate==0?1:-expm1(-rate)/rate;
        return scalar(t+product(reservoir-t,conductance,dt,capacity)*phi,true,out);
    }
    return scalar(relax(t,reservoir,rate),true,out); /* infinity means equilibrium. */
}
ps_result ps_thermal_pair_step(double ca, double ta, double cb, double tb,
                               double conductance, double dt, ps_vec2 *out) {
    if (!out || !positive(ca) || !positive(cb) || !positive(ta) || !positive(tb) ||
        !nonnegative(conductance) || !nonnegative(dt)) return PS_INVALID;
    double maximum=fmax(ca,cb), a=ca/maximum, b=cb/maximum, sum=a+b;
    double equilibrium=ta<=tb ? ta+product(tb-ta,cb,1,maximum)/sum
                                 : tb+product(ta-tb,ca,1,maximum)/sum;
    double rate=product(conductance,dt,1,ca)+product(conductance,dt,1,cb);
    ps_vec2 result;
    if (rate < 1e-8) {
        double phi=rate==0?1:-expm1(-rate)/rate;
        result=(ps_vec2){ta+product(tb-ta,conductance,dt,ca)*phi,
                        tb+product(ta-tb,conductance,dt,cb)*phi};
    } else result=(ps_vec2){relax(ta,equilibrium,rate),relax(tb,equilibrium,rate)};
    if (!positive(result.x) || !positive(result.y)) return PS_NUMERIC;
    *out=result;
    return PS_OK;
}
