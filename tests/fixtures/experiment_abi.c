#include "physim/experiment.h"
#include <stdio.h>
#ifndef PS_TEST_ABI_CASE
#define PS_TEST_ABI_CASE 0
#endif
static ps_result reset(ps_context *c) { fputs("ABI_CALLBACK reset\n",stderr);c->values[0]=0;return PS_OK; }
static ps_result create(ps_context *c) {
    fputs("ABI_CALLBACK create\n",stderr);
    if(PS_TEST_ABI_CASE==16)return PS_MEMORY;
    if(ps_channel_add(c,"position",PS_METRE,"ABI contract probe")<0)return PS_INVALID;
    return reset(c);
}
static ps_result step(ps_context *c,double dt) { fputs("ABI_CALLBACK step\n",stderr);c->values[0]+=dt;return PS_OK; }
static void scene(ps_context *c,ps_scene *s) { (void)c;(void)s;fputs("ABI_CALLBACK scene\n",stderr); }
static void destroy(ps_context *c) { (void)c;fputs("ABI_CALLBACK destroy\n",stderr); }
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const struct { ps_experiment_api base; uint64_t future; } api={.base={
        .struct_size=PS_TEST_ABI_CASE==1 ? PS_EXPERIMENT_API_BASE_SIZE-1 :
                     PS_TEST_ABI_CASE==11 || PS_TEST_ABI_CASE==15 ? PS_EXPERIMENT_API_BASE_SIZE :
                     PS_TEST_ABI_CASE==14 ? sizeof api : sizeof api.base,
        .abi_version=PS_TEST_ABI_CASE==2 ? PS_ABI_VERSION+1 : PS_ABI_VERSION,
        .capabilities=PS_TEST_ABI_CASE==9 ? PS_EXPERIMENT_SCENE_FRAMES :
                      PS_TEST_ABI_CASE==10 || PS_TEST_ABI_CASE==11 ? PS_EXPERIMENT_ADAPTIVE_STEPS :
                      PS_TEST_ABI_CASE==13 ? UINT64_C(1)<<63 : 0,
        .name=PS_TEST_ABI_CASE==3 ? NULL : "Experiment ABI contract",
        .create=PS_TEST_ABI_CASE==4 ? NULL : create,
        .reset=PS_TEST_ABI_CASE==5 ? NULL : reset,
        .step=PS_TEST_ABI_CASE==6 ? NULL : step,
        .build_scene=PS_TEST_ABI_CASE==7 ? NULL : scene,
        .destroy=PS_TEST_ABI_CASE==8 ? NULL : destroy
    },.future=UINT64_C(123)};
    return PS_TEST_ABI_CASE==12 ? NULL : &api.base;
}
