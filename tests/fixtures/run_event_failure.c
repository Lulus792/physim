#include "physim/experiment.h"
#include <stdlib.h>
static ps_result create(ps_context *c) {
    (void)ps_experiment_log(c,PS_LOG_INFO,"initial event");
    return ps_channel_add(c,"value",PS_METRE,"failure fixture")>=0?PS_OK:PS_INVALID;
}
static ps_result reset(ps_context *c){c->values[0]=1;return PS_OK;}
static ps_result step(ps_context *c,double dt) {
    (void)dt;(void)ps_experiment_log(c,PS_LOG_WARNING,"before failure α");
    if(c->seed==43)abort();
    return PS_NUMERIC;
}
static void scene(ps_context *c,ps_scene *s){(void)c;(void)s;}
static void destroy(ps_context *c){(void)ps_experiment_log(c,PS_LOG_ERROR,"destroy after failure");}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={sizeof api,PS_ABI_VERSION,0,"event failure",create,reset,step,scene,destroy,NULL};return &api;
}
