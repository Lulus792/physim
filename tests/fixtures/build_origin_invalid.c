#include "physim/experiment.h"
#include <string.h>
#ifndef PS_TEST_ORIGIN_CASE
#define PS_TEST_ORIGIN_CASE 0
#endif
static ps_result reset(ps_context *c){c->values[0]=0;return PS_OK;}
static ps_result create(ps_context *c){
 if(PS_TEST_ORIGIN_CASE==2)strcpy(c->model_metadata,"module_compiler=forged");
 if(PS_TEST_ORIGIN_CASE==3)strcpy(c->model_metadata,"core_build=forged");
 return ps_channel_add(c,"value",PS_METRE,"Origin guard")>=0?reset(c):PS_INVALID;
}
static ps_result step(ps_context *c,double dt){c->values[0]+=dt;return PS_OK;}
static void scene(ps_context *c,ps_scene *s){(void)c;(void)s;}
static void destroy(ps_context *c){(void)c;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
 static const ps_experiment_api api={sizeof api,PS_ABI_VERSION,PS_EXPERIMENT_BUILD_INFO,PS_TEST_ORIGIN_CASE==4?"bad\nmodule_compiler=forged":"Origin guard",create,reset,step,scene,destroy,NULL,
 .compiler=PS_TEST_ORIGIN_CASE==0?"bad\ncompiler":PS_BUILD_COMPILER,
 .build=PS_TEST_ORIGIN_CASE==1?"":PS_BUILD_STAMP};return &api;
}
