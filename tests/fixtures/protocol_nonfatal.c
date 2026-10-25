#include "physim/experiment.h"
#include <stdlib.h>
static ps_result reset(ps_context *c) {
    c->values[0]=100+(double)c->seed;
    (void)ps_experiment_log(c,PS_LOG_DEBUG,"Reset \"quoted\"\t☃");return PS_OK;
}
static ps_result create(ps_context *c) {
    (void)ps_experiment_log(c,PS_LOG_INFO,"Created: α");
    if(ps_channel_add(c,"value",PS_METRE,"Protocol does not change measurements")<0)return PS_LIMIT;
    return reset(c);
}
static ps_result step(ps_context *c,double dt) {
    (void)ps_experiment_log(c,PS_LOG_WARNING,"Step\nnext line");
    (void)ps_experiment_log(c,PS_LOG_ERROR,"Nonfatal error: α\n\toriginal text");
    c->values[0]+=dt;return PS_OK;
}
static void scene(ps_context *c,ps_scene *s){(void)c;(void)s;}
static void destroy(ps_context *c){(void)ps_experiment_log(c,PS_LOG_ERROR,"Destroyed");}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={sizeof api,PS_ABI_VERSION,PS_EXPERIMENT_BUILD_INFO,"Protocol nonfatal",create,reset,step,scene,destroy,NULL,PS_EXPERIMENT_BUILD_ORIGIN};return &api;
}
