#ifndef PHYSIM_STUDENT_INTERVAL_H
#define PHYSIM_STUDENT_INTERVAL_H
#include <float.h>
#include <math.h>
/* Integer-degree normalization from the t density at zero. Gamma recurrence
 * avoids subtraction of large lgamma values. Caller bounds df<100000. */
static double ps_student_density_zero(uint64_t df) {
    uint64_t k=(df&1)?1:2;
    double density=k==1?1/PS_PI:1/(2*sqrt(2));
    for(;k<df;k+=2)density*=((double)k+1)/sqrt((double)k*((double)k+2));
    return density;
}
static ps_result ps_student_beta_fraction(double a,double b,double x,double *out) {
    const double tiny=DBL_MIN/DBL_EPSILON;
    double c=1,d=1-(a+b)*x/(a+1);
    if(fabs(d)<tiny)d=copysign(tiny,d);
    d=1/d;double h=d;
    for(unsigned m=1;m<=512;m++) {
        double twice=2.0*m;
        double aa=m*(b-m)*x/((a+twice-1)*(a+twice));
        d=1+aa*d;if(fabs(d)<tiny)d=copysign(tiny,d);
        c=1+aa/c;if(fabs(c)<tiny)c=copysign(tiny,c);
        d=1/d;h*=d*c;
        aa=-(a+m)*(a+b+m)*x/((a+twice)*(a+twice+1));
        d=1+aa*d;if(fabs(d)<tiny)d=copysign(tiny,d);
        c=1+aa/c;if(fabs(c)<tiny)c=copysign(tiny,c);
        d=1/d;double delta=d*c;h*=delta;
        if(!isfinite(h))return PS_NUMERIC;
        if(fabs(delta-1)<=8*DBL_EPSILON){*out=h;return PS_OK;}
    }
    return PS_LIMIT;
}
static ps_result ps_student_beta(double a,double b,double x,double complement,double log_beta,double *out) {
    if(x<=0){*out=0;return PS_OK;}
    if(complement<=0){*out=1;return PS_OK;}
    bool flip=x>(a+1)/(a+b+2);
    if(flip){double t=a;a=b;b=t;t=x;x=complement;complement=t;}
    double fraction;ps_result r=ps_student_beta_fraction(a,b,x,&fraction);
    if(r!=PS_OK)return r;
    double log_x=complement<.5?log1p(-complement):log(x);
    double log_complement=x<.5?log1p(-x):log(complement);
    double value=exp(a*log_x+b*log_complement-log_beta)*fraction/a;
    if(!isfinite(value) || value<0 || value>1+64*DBL_EPSILON)return PS_NUMERIC;
    value=fmin(1,value);*out=flip?1-value:value;return PS_OK;
}
static double ps_student_normal_critical(double confidence) {
    if(confidence<1e-8)return confidence*sqrt(PS_PI/2);
    double lo=0,hi=12;
    for(unsigned i=0;i<96;i++) {
        double mid=lo+(hi-lo)/2;
        if(mid==lo || mid==hi)break;
        bool below=confidence<.5?erf(mid/sqrt(2))<confidence:
                                  erfc(mid/sqrt(2))>1-confidence;
        if(below)lo=mid;else hi=mid;
    }
    return lo+(hi-lo)/2;
}
static ps_result ps_student_critical(uint64_t df,double confidence,double *out) {
    if(df==1){*out=confidence>.5?1/tan((PS_PI/2)*(1-confidence)):tan((PS_PI/2)*confidence);return PS_OK;}
    if(df==2){*out=sqrt(2)*confidence/sqrt((1-confidence)*(1+confidence));return PS_OK;}
    if(df>=100000) {
        double z=ps_student_normal_critical(confidence),z2=z*z,v=(double)df;
        *out=z+z*(z2+1)/(4*v)+z*(5*z2*z2+16*z2+3)/(96*v*v)+
             z*(3*z2*z2*z2+19*z2*z2+17*z2-15)/(384*v*v*v);
        return isfinite(*out)?PS_OK:PS_NUMERIC;
    }
    double density=ps_student_density_zero(df);
    if(confidence<1e-8){*out=confidence/(2*density);return PS_OK;}
    double degrees=(double)df,log_beta=-.5*log(degrees)-log(density);
    bool central=confidence<.5;double target=central?confidence:(1-confidence)/2;
    double lo=0,hi=1;
    for(unsigned i=0;i<160;i++) {
        double ratio=hi/sqrt(degrees),q=ratio*ratio;
        double x=1/(1+q),y=q/(1+q),value;
        ps_result r=central?ps_student_beta(.5,degrees/2,y,x,log_beta,&value):
                            ps_student_beta(degrees/2,.5,x,y,log_beta,&value);
        if(r!=PS_OK)return r;
        if(!central)value/=2;
        if(central?value>=target:value<=target)break;
        lo=hi;hi*=2;
        if(i==159 || !isfinite(hi))return PS_LIMIT;
    }
    for(unsigned i=0;i<128;i++) {
        double mid=lo+(hi-lo)/2;
        if(mid==lo || mid==hi)break;
        double ratio=mid/sqrt(degrees),q=ratio*ratio;
        double x=1/(1+q),y=q/(1+q),value;
        ps_result r=central?ps_student_beta(.5,degrees/2,y,x,log_beta,&value):
                            ps_student_beta(degrees/2,.5,x,y,log_beta,&value);
        if(r!=PS_OK)return r;
        if(!central)value/=2;
        if(central?value<target:value>target)lo=mid;else hi=mid;
    }
    *out=lo+(hi-lo)/2;return PS_OK;
}
#endif
