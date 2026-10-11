#include "physim/experiment.h"
#include <stdlib.h>
typedef struct {double distance,gain;} state;
static ps_result reset(ps_context *c){state *s=c->user;c->values[0]=s->distance;c->values[1]=s->gain;return PS_OK;}
static ps_result create(ps_context *c) {
    state *s=calloc(1,sizeof *s);if(!s)return PS_MEMORY;c->user=s;
    ps_unit cm={{1},.01,"cm"};
    ps_result result=ps_parameter_define_unit(c,"distance","Abstand in Zentimetern für die Messung",cm,.5,.1,2,&s->distance);
    if(result==PS_OK)result=ps_parameter_define(c,"gain","Verstärkung ohne Einheit",2,0,5,&s->gain);
    if(result!=PS_OK)return result;
    if(ps_channel_add(c,"distance",PS_METRE,"Selected distance in SI")<0 ||
       ps_channel_add(c,"gain",PS_ONE,"Selected gain")<0)return PS_INVALID;
    return reset(c);
}
static ps_result step(ps_context *c,double dt){(void)dt;return reset(c);}
static void scene(ps_context *c,ps_scene *s){(void)c;(void)s;}
static void destroy(ps_context *c){free(c->user);c->user=NULL;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,.name="Parameter form fixture",
        .create=create,.reset=reset,.step=step,.build_scene=scene,.destroy=destroy};return &api;
}
