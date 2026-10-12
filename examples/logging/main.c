#include "physim/experiment.h"
#include <stdio.h>
#include <stdlib.h>
typedef struct { double burst; } model;
static ps_result reset(ps_context *c);
static ps_result create(ps_context *c) {snprintf(c->model_metadata,sizeof c->model_metadata,"layer.geometry=omitted|Logging demonstration, no geometry model\nlayer.laws=omitted|Prescribed scalar progression, not physics\nlayer.medium=omitted|No material or medium\nlayer.losses=omitted|No loss model\nlayer.numerics=active|Value incremented by dt\nlayer.measurement=ideal|Exact scalar channel\nlayer.sampling=omitted|Deterministic model; external ensembles separate\n");
    model *state=calloc(1,sizeof *state);
    if(!state)return PS_MEMORY;
    c->user=state;
    (void)ps_experiment_log(c,PS_LOG_INFO,"Created: α");
    ps_channel_add(c,"value",PS_METRE,"Logging leaves physics unchanged");
    ps_result r=ps_parameter_define(c,"burst","Burst log count",0,0,5000,&state->burst);
    if(r==PS_OK)r=reset(c);
    return r;
}
static ps_result reset(ps_context *c){c->values[0]=100+(double)c->seed;(void)ps_experiment_log(c,PS_LOG_DEBUG,"Reset \"quoted\"\t☃");return PS_OK;}
static ps_result step(ps_context *c,double dt) {
    model *state=c->user;
    (void)ps_experiment_log(c,PS_LOG_WARNING,"Step\nnext line");
    if(c->time_s==0)for(unsigned i=0;i<(unsigned)state->burst;i++)(void)ps_experiment_log(c,PS_LOG_DEBUG,"burst α☃");
    c->values[0]+=dt;return PS_OK;
}
static void scene(ps_context *c,ps_scene *s){(void)c;(void)s;}
static void destroy(ps_context *c){(void)ps_experiment_log(c,PS_LOG_ERROR,"Destroyed");free(c->user);c->user=NULL;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={sizeof api,PS_ABI_VERSION, (0) | PS_EXPERIMENT_BUILD_INFO,"Logging example",create,reset,step,scene,destroy,NULL, PS_EXPERIMENT_BUILD_ORIGIN};return &api;
}
