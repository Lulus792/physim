#include "physim/experiment.h"
#include "physim/units.h"
static ps_result create(ps_context *c){double scene;ps_result result=ps_parameter_define(c,"scene","Fail while building scene",0,0,1,&scene);if(result!=PS_OK)return result;if(ps_channel_add(c,"value",PS_ONE,"Initial value")<0)return PS_INVALID;c->values[0]=1;return PS_OK;}
static ps_result reset(ps_context *c){c->values[0]=1;return PS_OK;}
static ps_result step(ps_context *c,double dt) {
    (void)dt;
    ps_diagnostic diagnostic;
    ps_result result=ps_diagnostic_set(&diagnostic,PS_SINGULAR,"solve","matrix",__FILE__,__LINE__,1,
        "Singular matrix α\nChoose independent equations");
    return result==PS_OK?ps_experiment_fail(c,&diagnostic):result;
}
static void scene(ps_context *c,ps_scene *s){if(c->parameters[0].value==1){(void)step(c,0);return;}ps_scene_add_id(s,1,PS_SPHERE,ps_v3(0,0,0),ps_v3(0,0,0),.2,UINT32_MAX);}
static void destroy(ps_context *c){(void)c;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void){static const ps_experiment_api api={sizeof api,PS_ABI_VERSION,0,"Diagnostic example",create,reset,step,scene,destroy,NULL};return &api;}
