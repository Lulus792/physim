#include "physim/numerics.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Adaptive line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static void growth(double time,const double *y,double *dy,void *user) {
    (void)time;(void)user;dy[0]=10*y[0];
}
static void bad(double time,const double *y,double *dy,void *user) {
    (void)time;(void)y;(void)user;dy[0]=NAN;
}
int main(void) {
    ps_ode_options o=ps_ode_options_default();o.initial_step=.5;o.maximum_step=.5;
    o.absolute_tolerance=1e-12;o.relative_tolerance=1e-10;
    double y=1;ps_ode_report r;ps_ode_diagnostic d;
    CHECK(ps_ode_step_diagnosed(growth,NULL,0,.5,&y,1,&o,&r,&d)==PS_OK);
    CHECK(r.accepted_steps==1 && r.rejected_steps>0 && r.evaluations==7*(1+r.rejected_steps));
    CHECK(r.reached_time>0 && r.reached_time<.5 && r.next_step>0 && r.error_norm<=1);
    CHECK(fabs(y-exp(10*r.reached_time))<1e-9 && d.reason==PS_ODE_DIAG_NONE);
    double t=r.reached_time,previous=y;
    o.initial_step=r.next_step;
    CHECK(ps_ode_step_diagnosed(growth,NULL,t,t+.5,&y,1,&o,&r,&d)==PS_OK && r.accepted_steps==1);
    CHECK(r.reached_time>t && y>previous && fabs(y-exp(10*r.reached_time))<1e-8);
    y=1;o.initial_step=.5;o.minimum_step=.5;
    CHECK(ps_ode_step_diagnosed(growth,NULL,0,.5,&y,1,&o,&r,&d)==PS_LIMIT);
    CHECK(y==1 && !r.accepted_steps && d.reason==PS_ODE_DIAG_MINIMUM_STEP);
    o.minimum_step=1e-14;o.maximum_steps=1;
    CHECK(ps_ode_step_diagnosed(growth,NULL,0,.5,&y,1,&o,&r,&d)==PS_LIMIT);
    CHECK(y==1 && r.rejected_steps==1 && d.reason==PS_ODE_DIAG_STEP_BUDGET);
    o.maximum_steps=10000;
    CHECK(ps_ode_step_diagnosed(bad,NULL,0,.5,&y,1,&o,&r,&d)==PS_NUMERIC && y==1);
    CHECK(d.reason==PS_ODE_DIAG_DERIVATIVE && d.component==0 && d.stage==0);
    ps_ode_report preserved=r;
    CHECK(ps_ode_step_diagnosed(growth,NULL,0,0,&y,1,&o,&r,&d)==PS_INVALID);
    CHECK(y==1 && !memcmp(&r,&preserved,sizeof r) && d.reason==PS_ODE_DIAG_ARGUMENT);
    CHECK(ps_ode_step_diagnosed(growth,NULL,.5,0,&y,1,&o,&r,&d)==PS_OK);
    CHECK(r.accepted_steps==1 && r.reached_time<.5 && r.reached_time>0 && r.next_step<0);
    CHECK(fabs(y-exp(10*(r.reached_time-.5)))<1e-9);
    puts("One accepted RK45 step, rejected trials, accuracy, limits, backward time and atomic failures passed.");
    return 0;
}
