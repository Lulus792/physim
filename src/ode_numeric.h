#ifndef PHYSIM_ODE_NUMERIC_H
#define PHYSIM_ODE_NUMERIC_H
#include <math.h>
#include <float.h>
#include <stddef.h>
/* Internal finite-input arithmetic: base + h*sum(weights[i]*values[i])/divisor.
 * Up to seven terms; positive divisor; no heap or wider floating type. Keep
 * products/sums normalized until the final state. Double rounding still applies.
 * Neumaier compensation and FMA retain ordinary product/summation residuals. */
static inline double ps_ode_weighted(double base,double h,const double *values,
                                      const double *weights,size_t count,double divisor) {
    if(h==0 || count==0)return base;
    /* Ordinary normal-range terms need no exponent decomposition. Fall back
     * on every nonfinite/subnormal product or sum so large h cannot amplify a
     * prematurely rounded tiny term. Compensation remains in the fast path. */
    double ordinary=0,ordinary_error=0;int ordinary_safe=1;
    for(size_t i=0;i<count;i++) {
        double product=values[i]*weights[i];
        if(!isfinite(product) || (values[i]!=0 && weights[i]!=0 && fabs(product)<DBL_MIN)) {
            ordinary_safe=0;break;
        }
        double next=ordinary+product;
        if(!isfinite(next)){ordinary_safe=0;break;}
        ordinary_error+=fabs(ordinary)>=fabs(product)?(ordinary-next)+product:(product-next)+ordinary;
        ordinary_error+=fma(values[i],weights[i],-product);ordinary=next;
    }
    if(ordinary_safe) {
        double total=ordinary+ordinary_error,mean=total/divisor,increment=h*mean;
        if(isfinite(total) && isfinite(mean) && isfinite(increment) &&
           (total==0 || fabs(mean)>=DBL_MIN) && (mean==0 || fabs(increment)>=DBL_MIN)) {
            double result=fma(h,mean,base);
            if(isfinite(result))return result;
        }
    }
    double mantissa[7],error[7];int exponent[7],maximum=0;int any=0;
    for(size_t i=0;i<count;i++) {
        if(values[i]==0 || weights[i]==0){mantissa[i]=error[i]=0;exponent[i]=0;continue;}
        int ev,ew;double mv=frexp(values[i],&ev),mw=frexp(weights[i],&ew);
        mantissa[i]=mv*mw;error[i]=fma(mv,mw,-mantissa[i]);exponent[i]=ev+ew;
        if(!any || exponent[i]>maximum)maximum=exponent[i];
        any=1;
    }
    if(!any)return base;
    double sum=0,compensation=0;
    for(size_t i=0;i<count;i++)if(mantissa[i]!=0) {
        double term=scalbn(mantissa[i],exponent[i]-maximum),next=sum+term;
        compensation+=fabs(sum)>=fabs(term)?(sum-next)+term:(term-next)+sum;
        compensation+=scalbn(error[i],exponent[i]-maximum);sum=next;
    }
    double total=sum+compensation;
    if(total==0)return base;
    int eh,ed,eb;double mh=frexp(h,&eh),md=frexp(divisor,&ed),mb=frexp(base,&eb);
    double increment=total*mh/md;int ei=maximum+eh-ed;
    if(base==0)eb=ei;
    int common=eb>ei?eb:ei;
    double a=scalbn(mb,eb-common),b=scalbn(increment,ei-common);
    return scalbn(a+b,common);
}
#endif
