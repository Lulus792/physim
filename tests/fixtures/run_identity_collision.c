#include "physim/experiment.h"
#include <string.h>
static ps_result create(ps_context *c) {
    strcpy(c->model_metadata,c->seed==43?"other=1\rproject_id=forged":"experiment_id=forged");
    return ps_channel_add(c,"value",PS_METRE,"identity collision")>=0?PS_OK:PS_INVALID;
}
static ps_result reset(ps_context *c){c->values[0]=1;return PS_OK;}
static ps_result step(ps_context *c,double dt){(void)c;(void)dt;return PS_OK;}
static void scene(ps_context *c,ps_scene *s){(void)c;(void)s;}
static void destroy(ps_context *c){(void)c;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={sizeof api,PS_ABI_VERSION,0,"identity collision",create,reset,step,scene,destroy,NULL};return &api;
}
