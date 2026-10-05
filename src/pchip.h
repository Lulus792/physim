#ifndef PS_PCHIP_H
#define PS_PCHIP_H
#include <math.h>
#include <stdbool.h>
/* Positive values as mantissa/exponent pairs. Secants of finite double inputs
 * can lie outside double's range; Windows long double has no wider range. */
typedef struct { double f; int e; } ps_pchip_scaled;
typedef struct { ps_pchip_scaled magnitude; int sign; } ps_pchip_slope;
static ps_pchip_scaled ps_pchip_normal(double f, int e) {
    int shift; double m=frexp(f,&shift);
    return (ps_pchip_scaled){m,m?e+shift:0};
}
static ps_pchip_scaled ps_pchip_span(double a,double b) {
    double d=b-a;
    return isfinite(d)?ps_pchip_normal(fabs(d),0):ps_pchip_normal(fabs(b*.5-a*.5),1);
}
static int ps_pchip_compare(ps_pchip_scaled a,ps_pchip_scaled b) {
    if(!a.f || !b.f)return (a.f>0)-(b.f>0);
    if(a.e!=b.e)return (a.e>b.e)?1:-1;
    return (a.f>b.f)-(a.f<b.f);
}
static ps_pchip_scaled ps_pchip_multiply(ps_pchip_scaled a,ps_pchip_scaled b) {
    return ps_pchip_normal(a.f*b.f,a.e+b.e);
}
static ps_pchip_scaled ps_pchip_divide(ps_pchip_scaled a,ps_pchip_scaled b) {
    return ps_pchip_normal(a.f/b.f,a.e-b.e);
}
static double ps_pchip_value(ps_pchip_scaled a) { return ldexp(a.f,a.e); }
static ps_pchip_scaled ps_pchip_sum(ps_pchip_scaled a,ps_pchip_scaled b,bool subtract) {
    if(!a.f)return subtract?(ps_pchip_scaled){0}:b;
    if(!b.f)return a;
    int e=a.e>b.e?a.e:b.e;
    double f=ldexp(a.f,a.e-e)+(subtract?-1:1)*ldexp(b.f,b.e-e);
    return f>0?ps_pchip_normal(f,e):(ps_pchip_scaled){0};
}
static ps_pchip_scaled ps_pchip_fraction(ps_pchip_scaled a,ps_pchip_scaled b) {
    /* a/(a+b), retained in scaled form for the one-sided endpoint formula. */
    return ps_pchip_divide(a,ps_pchip_sum(a,b,false));
}
static ps_pchip_slope ps_pchip_secant(double xa,double xb,double ya,double yb) {
    ps_pchip_slope s={ps_pchip_divide(ps_pchip_span(ya,yb),ps_pchip_span(xa,xb)),(yb>ya)-(yb<ya)};
    return s;
}
static ps_pchip_slope ps_pchip_interior(ps_pchip_scaled h0,ps_pchip_scaled h1,
                                       ps_pchip_slope d0,ps_pchip_slope d1) {
    if(!d0.sign || d0.sign!=d1.sign)return (ps_pchip_slope){0};
    double t=ps_pchip_value(ps_pchip_fraction(h0,h1));
    double w0=(2-t)/3,w1=(1+t)/3;
    ps_pchip_scaled small=d0.magnitude,big=d1.magnitude;
    if(ps_pchip_compare(small,big)>0) {
        small=d1.magnitude;big=d0.magnitude;
        double temporary=w0;w0=w1;w1=temporary;
    }
    double ratio=ps_pchip_value(ps_pchip_divide(small,big));
    return (ps_pchip_slope){ps_pchip_divide(small,ps_pchip_normal(w0+w1*ratio,0)),d0.sign};
}
static ps_pchip_slope ps_pchip_endpoint(ps_pchip_scaled h0,ps_pchip_scaled h1,
                                       ps_pchip_slope d0,ps_pchip_slope d1) {
    if(!d0.sign)return (ps_pchip_slope){0};
    ps_pchip_scaled t=ps_pchip_fraction(h0,h1);
    ps_pchip_scaled a=ps_pchip_multiply(d0.magnitude,ps_pchip_normal(1+ps_pchip_value(t),0));
    ps_pchip_scaled b=ps_pchip_multiply(d1.magnitude,t);
    ps_pchip_scaled m=ps_pchip_sum(a,b,d0.sign==d1.sign);
    if(d0.sign!=d1.sign) {
        ps_pchip_scaled limit=ps_pchip_multiply(d0.magnitude,ps_pchip_normal(3,0));
        if(ps_pchip_compare(m,limit)>0)m=limit;
    }
    return (ps_pchip_slope){m,m.f?d0.sign:0};
}
static double ps_pchip_control(double anchor,double other,ps_pchip_scaled h,
                               ps_pchip_slope slope,int direction) {
    ps_pchip_scaled term=ps_pchip_multiply(h,slope.magnitude);
    term=ps_pchip_divide(term,ps_pchip_normal(3,0));
    ps_pchip_scaled span=ps_pchip_span(anchor,other);
    if(ps_pchip_compare(term,span)>0)term=span;
    double delta=ps_pchip_value(term),value;
    int sign=slope.sign*direction;
    if(isfinite(delta))value=anchor+sign*delta;
    else value=2*(anchor*.5+sign*ldexp(term.f,term.e-1));
    return fmax(fmin(anchor,other),fmin(fmax(anchor,other),value));
}
static void ps_pchip_controls(const double *x,const double *y,unsigned count,unsigned left,double out[4]) {
    unsigned right=left+1;
    ps_pchip_scaled h=ps_pchip_span(x[left],x[right]);
    ps_pchip_slope d=ps_pchip_secant(x[left],x[right],y[left],y[right]),a=d,b=d;
    if(count>2) {
        if(left) a=ps_pchip_interior(ps_pchip_span(x[left-1],x[left]),h,
            ps_pchip_secant(x[left-1],x[left],y[left-1],y[left]),d);
        else a=ps_pchip_endpoint(h,ps_pchip_span(x[right],x[right+1]),d,
            ps_pchip_secant(x[right],x[right+1],y[right],y[right+1]));
        if(right+1<count) b=ps_pchip_interior(h,ps_pchip_span(x[right],x[right+1]),d,
            ps_pchip_secant(x[right],x[right+1],y[right],y[right+1]));
        else b=ps_pchip_endpoint(h,ps_pchip_span(x[left-1],x[left]),d,
            ps_pchip_secant(x[left-1],x[left],y[left-1],y[left]));
    }
    out[0]=y[left];out[1]=ps_pchip_control(y[left],y[right],h,a,1);
    out[2]=ps_pchip_control(y[right],y[left],h,b,-1);out[3]=y[right];
}
static double ps_pchip_blend(double a,double b,double t) {
    return (a<0)!=(b<0)?(1-t)*a+t*b:a+t*(b-a);
}
static double ps_pchip_evaluate(const double controls[4],double t) {
    /* de Casteljau uses only convex combinations of finite controls. */
    double a=ps_pchip_blend(controls[0],controls[1],t);
    double b=ps_pchip_blend(controls[1],controls[2],t);
    double d=ps_pchip_blend(controls[2],controls[3],t);
    double value=ps_pchip_blend(ps_pchip_blend(a,b,t),ps_pchip_blend(b,d,t),t);
    return fmax(fmin(controls[0],controls[3]),fmin(fmax(controls[0],controls[3]),value));
}
#endif
