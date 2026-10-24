/* Logging-layout fixture: prescribed scalar, deliberately no physics or scene. */
#include "physim/experiment.h"
#include <stdio.h>
static ps_result reset(ps_context *c){c->values[0]=100+(double)c->seed;return ps_experiment_log(c,PS_LOG_DEBUG,"Reset \"quoted\"\t☃");}
static ps_result create(ps_context *c){
    snprintf(c->model_metadata,sizeof c->model_metadata,"layer.laws=omitted|Prescribed scalar for native UI tests\nlayer.geometry=omitted|No geometry\n");
    c->values[0]=100+(double)c->seed;
    if(ps_channel_add(c,"value",PS_METRE,"Prescribed scalar; no physical model")<0)return PS_LIMIT;
    ps_result r=ps_experiment_log(c,PS_LOG_INFO,"Created: α");
    if(r==PS_OK)r=reset(c);return r;
}
static ps_result step(ps_context *c,double dt){
    c->values[0]+=dt;
    return ps_experiment_log(c,PS_LOG_WARNING,"Eine lange Warnung mit echten Unicode-Zeichen α und mehreren Worten zur Prüfung.\nTabs:\t☃ Ende");
}
static void scene(ps_context *c,ps_scene *s){(void)c;(void)s;}
static void destroy(ps_context *c){(void)ps_experiment_log(c,PS_LOG_ERROR,"Destroyed");}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void){
    static const ps_experiment_api api={sizeof api,PS_ABI_VERSION,PS_EXPERIMENT_BUILD_INFO,"Native inspector fixture",create,reset,step,scene,destroy,NULL,PS_EXPERIMENT_BUILD_ORIGIN};return &api;
}
