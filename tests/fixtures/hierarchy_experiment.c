#ifdef PS_TEST_LEGACY_SCENE
#include "legacy_experiment_abi3.h"
#else
#include "physim/experiment.h"
#endif
#include <string.h>
static ps_result create(ps_context *c) {
    return ps_channel_add(c,"position.x",PS_METRE,"Position")<0?PS_INVALID:PS_OK;
}
static ps_result reset(ps_context *c) { c->values[0]=0;return PS_OK; }
static ps_result step(ps_context *c,double dt) { c->values[0]+=dt;return PS_OK; }
static void scene(ps_context *c,ps_scene *s) {
    ps_vec3 center=ps_v3(c->values[0],0,0);
    ps_scene_add_id(s,1,PS_SPHERE,center,center,.2,0x53dec2ff);
#ifdef PS_TEST_LEGACY_SCENE
    /* An old producer may leave arbitrary bytes in its four-byte tail padding. */
    memset((unsigned char*)&s->objects[0]+172,0xa5,4);
#else
    ps_scene_group(s,100,0,"Root");ps_scene_group(s,200,100,"Model");
    ps_scene_set_parent(s,1,200);
#endif
}
static void destroy(ps_context *c) { (void)c; }
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={sizeof api,3,
#ifdef PS_TEST_LEGACY_SCENE
        0,
#else
        PS_EXPERIMENT_SCENE_HIERARCHY,
#endif
        "Hierarchy",create,reset,step,scene,destroy
#ifndef PS_TEST_LEGACY_SCENE
        ,NULL
#endif
    };
    return &api;
}
