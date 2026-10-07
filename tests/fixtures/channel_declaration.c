#include "physim/experiment.h"
#include <stdio.h>
static ps_result reset(ps_context *c) {
    ps_unit cm=PS_METRE;cm.scale=.01;cm.symbol="cm";
    ps_result r=ps_convert(125,cm,PS_METRE,&c->values[0]);c->values[1]=7;return r;
}
static ps_result create(ps_context *c) {
    ps_unit cm=PS_METRE;cm.scale=.01;cm.symbol="cm";
    if(ps_channel_add(c,"wrong_scale",cm,"cm must be explicitly converted")!=-1 ||
       ps_channel_add(c,"length",PS_METRE,"SI value converted from cm")!=0 ||
       ps_channel_add(c,"length",PS_METRE,"duplicate")!=-1 ||
       ps_channel_add(c,"after",(ps_unit){{0},1,"1"},"successful declaration after errors")!=1)return PS_INVALID;
    snprintf(c->model_metadata,sizeof c->model_metadata,"model=canonical SI channel declaration\ninput=125 cm\nstored=1.25 m\n");return reset(c);
}
static ps_result step(ps_context *c,double dt){(void)dt;return reset(c);}
static void scene(ps_context *c,ps_scene *out){(void)c;ps_scene_add_id(out,1,PS_SPHERE,ps_v3(0,0,0),ps_v3(0,0,0),.1,0x53aeefff);}
static void destroy(ps_context *c){(void)c;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,.name="Canonical SI declarations",.create=create,.reset=reset,.step=step,.build_scene=scene,.destroy=destroy};return &api;
}
