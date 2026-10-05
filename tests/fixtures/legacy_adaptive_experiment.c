/* A claimed extension with only the frozen ABI-3 base must be rejected safely. */
#include "legacy_experiment_abi3.h"
static ps_result create(ps_context *c) {return ps_channel_add(c,"x",PS_METRE,"x")<0?PS_INVALID:PS_OK;}
static ps_result step(ps_context *c,double dt) {c->values[0]+=dt;return PS_OK;}
static void scene(ps_context *c,ps_scene *s) {(void)c;(void)s;}
static void destroy(ps_context *c) {(void)c;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={sizeof api,3,2,"Short API",create,create,step,scene,destroy};
    return &api;
}
