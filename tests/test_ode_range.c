#include "physim/numerics.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"ODE range %d: %s\n",__LINE__,#x);return 1;}}while(0)
typedef struct {double value;size_t n;unsigned calls;} constant_state;
static void constant(double t,const double *y,double *out,void *u) {
    (void)t;(void)y;constant_state *s=u;s->calls++;for(size_t i=0;i<s->n;i++)out[i]=s->value;
}
static void constant_acceleration(double t,const double *q,double *out,void *u){constant(t,q,out,u);}
int main(void) {
    constant_state s={1e308,1,0};double y=0;
    CHECK(ps_ode_step(PS_RK4,constant,&s,0,1e-308,&y,1)==PS_OK && fabs(y-1)<1e-14 && s.calls==4);
    s.calls=0;y=-1e308;CHECK(ps_ode_step(PS_EULER,constant,&s,0,2,&y,1)==PS_OK && fabs(y/1e308-1)<1e-14 && s.calls==1);
    y=-1e308;s.calls=0;CHECK(ps_ode_step(PS_RK4,constant,&s,0,2,&y,1)==PS_OK && fabs(y/1e308-1)<1e-14 && s.calls==4);
    s.value=DBL_TRUE_MIN;y=0;CHECK(ps_ode_step(PS_RK4,constant,&s,0,DBL_MAX,&y,1)==PS_OK && fabs(y/(DBL_MAX*DBL_TRUE_MIN)-1)<1e-14);
    s.value=1e308;ps_ode_options o=ps_ode_options_default();o.minimum_step=1e-310;o.initial_step=o.maximum_step=1e-308;
    ps_ode_report report;y=0;s.calls=0;
    CHECK(ps_ode_integrate(constant,&s,0,1e-308,&y,1,&o,&report)==PS_OK && fabs(y-1)<1e-14 && report.reached_time==1e-308 && s.calls==7);
    y=1;s.value=-1e308;CHECK(ps_ode_integrate(constant,&s,0,1e-308,&y,1,&o,&report)==PS_OK && fabs(y)<1e-14);
    double sp=-1e308,sv=-1e308;ps_symplectic_step(&sp,&sv,1e308,2);
    CHECK(fabs(sp/1e308-1)<1e-14 && fabs(sv/1e308-1)<1e-14);
    s.value=1e308;double q=0,v=0;
    CHECK(ps_verlet_step(constant_acceleration,&s,0,1e-308,&q,&v,1)==PS_OK && fabs(v-1)<1e-14 && q>0 && fabs(q/5e-309-1)<1e-14);
    s.value=DBL_TRUE_MIN;q=v=0;CHECK(ps_verlet_step(constant_acceleration,&s,0,1,&q,&v,1)==PS_OK && v==DBL_TRUE_MIN);
    double states[32]={0};s.n=32;s.value=1e308;
    CHECK(ps_ode_step(PS_RK4,constant,&s,0,1e-308,states,32)==PS_OK);for(unsigned i=0;i<32;i++)CHECK(fabs(states[i]-1)<1e-14);
    s.n=1;s.value=DBL_MAX;y=DBL_MAX;
    CHECK(ps_ode_step(PS_RK4,constant,&s,0,1,&y,1)==PS_NUMERIC && y==DBL_MAX);
    y=DBL_MAX;CHECK(ps_ode_step(PS_EULER,constant,&s,0,1,&y,1)==PS_NUMERIC && y==DBL_MAX);
    q=DBL_MAX;v=DBL_MAX;CHECK(ps_verlet_step(constant_acceleration,&s,0,1,&q,&v,1)==PS_NUMERIC && q==DBL_MAX && v==DBL_MAX);
    puts("ODE range: finite constant solutions, weighted stages, 32 states, RK45/Verlet and real overflow rollback passed");return 0;
}
