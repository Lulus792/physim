#include "physim/experiment.h"
#include "physim/numerics.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
typedef struct { double phase[2],fault; } oscillator;
static void derivative(double t,const double *y,double *dy,void *user) {
    (void)t;(void)user;dy[0]=y[1];dy[1]=-y[0];
}
static ps_result reset(ps_context *c) {
    oscillator *o=c->user;o->phase[0]=1;o->phase[1]=0;
    c->values[0]=1;c->values[1]=0;c->values[2]=c->values[3]=c->values[4]=0;
    return PS_OK;
}
static ps_result create(ps_context *c) {
    c->user=calloc(1,sizeof(oscillator));if(!c->user)return PS_MEMORY;
    ps_channel_add(c,"position",PS_METRE,"Oscillator position");
    ps_channel_add(c,"velocity",PS_VELOCITY,"Oscillator velocity reference");
    ps_channel_add(c,"interval",PS_SECOND,"Accepted interval");
    ps_unit one={{0},1,"1"};
    ps_channel_add(c,"rejected",one,"Rejected trial steps");
    ps_channel_add(c,"endpoint",PS_SECOND,"Actual endpoint");
    snprintf(c->model_metadata,sizeof c->model_metadata,"model=harmonic oscillator");
    ps_parameter_define(c,"fault","Invalid adaptive report test",0,0,8,&((oscillator*)c->user)->fault);
    if(((oscillator*)c->user)->fault==8) {
        memset(c->model_metadata,'x',sizeof c->model_metadata-1);
        c->model_metadata[sizeof c->model_metadata-1]=0;
    }
    return reset(c);
}
static ps_result step(ps_context *c,double dt) {
    oscillator *o=c->user;ps_result r=ps_ode_step(PS_RK4,derivative,NULL,c->time_s,dt,o->phase,2);
    if(r==PS_OK){c->values[0]=o->phase[0];c->values[1]=o->phase[1];c->values[2]=dt;c->values[4]=c->time_s+dt;}
    return r;
}
static ps_result adaptive(ps_context *c,double proposed,double minimum,double maximum,ps_step_interval *interval) {
    oscillator *o=c->user;ps_ode_options options=ps_ode_options_default();
    options.absolute_tolerance=1e-12;options.relative_tolerance=1e-10;
    options.initial_step=proposed;options.minimum_step=minimum;options.maximum_step=maximum;
    ps_ode_report r;ps_ode_diagnostic d;
    ps_result status=ps_ode_step_diagnosed(derivative,NULL,c->time_s,c->time_s+proposed,o->phase,2,&options,&r,&d);
    if(status!=PS_OK){snprintf(c->error,sizeof c->error,"%s",ps_ode_diagnostic_string(d.reason));return status;}
    *interval=(ps_step_interval){r.reached_time-c->time_s,r.next_step};
    c->values[0]=o->phase[0];c->values[1]=o->phase[1];c->values[2]=interval->elapsed_s;
    c->values[3]=r.rejected_steps;c->values[4]=r.reached_time;
    switch((int)o->fault) {
    case 1:interval->elapsed_s=NAN;break;
    case 2:interval->elapsed_s=0;break;
    case 3:interval->elapsed_s=proposed*2;break;
    case 4:interval->next_s=minimum/2;break;
    case 5:interval->next_s=maximum*2;break;
    case 6:interval->next_s=NAN;break;
    case 7:c->time_s+=1;break;
    default:break;
    }
    return PS_OK;
}
static void scene(ps_context *c,ps_scene *s) {
    ps_scene_add_id(s,1,PS_SPHERE,ps_v3(c->values[0],0,0),ps_v3(0,0,0),.1,UINT32_MAX);
}
static void destroy(ps_context *c) {free(c->user);c->user=NULL;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={sizeof api,PS_ABI_VERSION,PS_EXPERIMENT_ADAPTIVE_STEPS,
                                      "Oscillator",create,reset,step,scene,destroy,adaptive};
    return &api;
}
