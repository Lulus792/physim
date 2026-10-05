#include "physim/experiment.h"
#include "physim/numerics.h"
#include "physim/measurement.h"
#include <stdlib.h>
#include <stdio.h>
typedef struct {double position,velocity;} model;
static void derivative(double t,const double *y,double *dy,void *user) {
    (void)t;(void)y;dy[0]=((model*)user)->velocity;
}
static void measure(ps_context *c,double endpoint,double interval) {
    model *m=c->user;c->values[0]=m->position;c->values[1]=endpoint;
    c->values[2]=interval;c->values[3]=m->velocity;
}
static ps_result reset(ps_context *c) {
    ps_rng_seed(&c->rng,c->seed);
    model *m=c->user;
    ps_result r=ps_distribution_sample((ps_distribution){PS_DIST_UNIFORM,0,1},&c->rng,&m->position);
    if(r==PS_OK)measure(c,0,0);
    return r;
}
static ps_result create(ps_context *c) {
    c->user=calloc(1,sizeof(model));if(!c->user)return PS_MEMORY;
    ps_channel_add(c,"position",PS_METRE,"Position with uncertain initial value");
    ps_channel_add(c,"endpoint",PS_SECOND,"Actual endpoint");
    ps_channel_add(c,"interval",PS_SECOND,"Accepted interval");
    ps_channel_add(c,"velocity",PS_VELOCITY,"Constant selected velocity");
    snprintf(c->model_metadata,sizeof c->model_metadata,"model=uniform translation; initial position U(0,1)");
    ps_result r=ps_parameter_define(c,"velocity","Constant velocity",1,.2,3,&((model*)c->user)->velocity);
    return r==PS_OK?reset(c):r;
}
static ps_result step(ps_context *c,double dt) {
    model *m=c->user;m->position+=m->velocity*dt;measure(c,c->time_s+dt,dt);return PS_OK;
}
static ps_result adaptive(ps_context *c,double proposed,double minimum,double maximum,ps_step_interval *interval) {
    model *m=c->user;ps_ode_options o=ps_ode_options_default();
    o.initial_step=proposed;o.minimum_step=minimum;o.maximum_step=maximum;
    ps_ode_report report;ps_ode_diagnostic diagnostic;
    ps_result r=ps_ode_step_diagnosed(derivative,m,c->time_s,c->time_s+proposed,&m->position,1,&o,&report,&diagnostic);
    if(r!=PS_OK)return r;
    *interval=(ps_step_interval){report.reached_time-c->time_s,report.next_step};
    measure(c,report.reached_time,interval->elapsed_s);return PS_OK;
}
static void scene(ps_context *c,ps_scene *s) {
    ps_scene_add_id(s,1,PS_SPHERE,ps_v3(c->values[0],0,0),ps_v3(0,0,0),.1,UINT32_MAX);
}
static void destroy(ps_context *c) {free(c->user);c->user=NULL;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={sizeof api,PS_ABI_VERSION,PS_EXPERIMENT_ADAPTIVE_STEPS,
                                      "Uniform translation",create,reset,step,scene,destroy,adaptive};return &api;
}
