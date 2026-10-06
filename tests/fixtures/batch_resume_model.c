#include "physim/experiment.h"
#include "platform.h"
static double rate;
static ps_result create(ps_context *c) {
    ps_channel_add(c,"value",PS_METRE,"Seed and parameter endpoint");
    ps_result result=ps_parameter_define(c,"rate","Rate",1,.5,2,&rate);if(result!=PS_OK)return result;
    c->values[0]=100+(double)c->seed;return PS_OK;
}
static ps_result step(ps_context *c,double dt) {
    if(c->seed==0 && c->time_s==0)ps_sleep(200);
    c->values[0]+=rate*dt;return PS_OK;
}
static void scene(ps_context *c,ps_scene *s){(void)c;(void)s;}
static void destroy(ps_context *c){(void)c;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={sizeof api,PS_ABI_VERSION,0,"Resume fixture",create,create,step,scene,destroy,NULL};return &api;
}
