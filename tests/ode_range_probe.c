#include "physim/numerics.h"
#include <math.h>
#include <stdio.h>
static void slope(double t,const double *y,double *out,void *u){(void)t;(void)y;out[0]=*(double *)u;}
int main(void) {
    int method;double q,v,rate,dt;
    while(scanf("%d %la %la %la %la",&method,&q,&v,&rate,&dt)==5) {
        ps_result result=PS_OK;
        if(method==PS_EULER || method==PS_RK4)result=ps_ode_step((ps_integrator)method,slope,&rate,0,dt,&q,1);
        else if(method==PS_SYMPLECTIC){ps_symplectic_step(&q,&v,rate,dt);if(!isfinite(q)||!isfinite(v))result=PS_NUMERIC;}
        else if(method==PS_VERLET)result=ps_verlet_step(slope,&rate,0,dt,&q,&v,1);
        else if(method==PS_RK45){ps_ode_options o=ps_ode_options_default();o.initial_step=o.minimum_step=o.maximum_step=dt;o.maximum_steps=8;o.absolute_tolerance=1e-8;o.relative_tolerance=1e-6;result=ps_ode_integrate(slope,&rate,0,dt,&q,1,&o,NULL);}
        else return 2;
        printf("%d %.17g %.17g\n",result,q,v);
    }
    return ferror(stdin)||ferror(stdout)?3:0;
}
