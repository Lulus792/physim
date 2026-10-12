/* Actual pre-provenance adaptive API: the runner must not read beyond its tail. */
#include "build_origin_legacy.h"
static ps_result reset(ps_context *c){c->values[0]=0;return PS_OK;}
static ps_result create(ps_context *c){return ps_channel_add(c,"value",PS_METRE,"Legacy adaptive")>=0?reset(c):PS_INVALID;}
static ps_result step(ps_context *c,double dt){c->values[0]+=dt;return PS_OK;}
static ps_result adaptive(ps_context *c,double proposed,double minimum,double maximum,ps_step_interval *out) {
 (void)minimum;(void)maximum;c->values[0]+=proposed;*out=(ps_step_interval){proposed,proposed};return PS_OK;
}
static void scene(ps_context *c,ps_scene *s){(void)c;(void)s;}
static void destroy(ps_context *c){(void)c;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
#ifdef PS_TEST_ORIGIN_LEGACY_FUTURE
 static const struct {ps_experiment_api base;uintptr_t foreign[2];} api={
   {sizeof api,PS_ABI_VERSION,PS_EXPERIMENT_ADAPTIVE_STEPS | (UINT64_C(1)<<63),"Pre-provenance adaptive",create,reset,step,scene,destroy,adaptive},{1,2}};
 return &api.base;
#else
 static const ps_experiment_api api={sizeof api,PS_ABI_VERSION,PS_EXPERIMENT_ADAPTIVE_STEPS,"Pre-provenance adaptive",create,reset,step,scene,destroy,adaptive};return &api;
#endif
}
