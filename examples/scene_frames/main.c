#include "physim/experiment.h"
#include "physim/math.h"
static ps_result create(ps_context *c){return ps_channel_add(c,"position.x",PS_METRE,"Local position")<0?PS_INVALID:PS_OK;}
static ps_result reset(ps_context *c){c->values[0]=0;return PS_OK;}
static ps_result step(ps_context *c,double dt){c->values[0]+=dt;return PS_OK;}
static void scene(ps_context *c,ps_scene *s) {
    ps_quat rotation=ps_quat_axis_angle(ps_v3(0,0,1),PS_PI/2);
    ps_scene_frame(s,100,0,"Translated/rotated frame",ps_v3(.4,-.2,0),rotation,ps_v3(1.5,1,.5));
    ps_scene_group(s,101,100,"Model");
    ps_scene_frame(s,200,101,"Nested scaled frame",ps_v3(.15,0,0),rotation,ps_v3(.75,1.25,1));
    ps_scene_add_id(s,1,PS_BOX,ps_v3(c->values[0],0,0),ps_v3(.5,.3,.3),0,0x53dec2ff);
    ps_scene_set_parent(s,1,200);
    ps_scene_add_id(s,2,PS_SPHERE,ps_v3(-.45,0,0),ps_v3(-.45,0,0),.16,0xf6b966a0);
    ps_scene_set_parent(s,2,200);
    ps_scene_label_id(s,3,ps_v3(c->values[0],.3,0),"Local body α",UINT32_MAX);ps_scene_set_parent(s,3,200);
    ps_vec3 points[]={ps_v3(-.6,-.35,0),ps_v3(0,-.2,0),ps_v3(.6,-.35,0)};
    ps_scene_polyline_id(s,4,points,3,.015,0x66aafaff);ps_scene_set_parent(s,4,200);
    ps_scene_add_id(s,5,PS_ARROW,ps_v3(0,0,0),ps_v3(.5,0,0),.012,0xef6670ff);ps_scene_set_parent(s,5,200);
    ps_scene_add_id(s,6,PS_POINT,ps_v3(0,.45,0),ps_v3(0,.45,0),.025,UINT32_MAX);ps_scene_set_parent(s,6,200);
    ps_object plane={.shape=PS_PLANE,.color=0x49607950,.a={0,0,-.3},.b={1.3,0,.7},.orientation={.7071067811865475,0,0,.7071067811865476},.id=7,.parent_id=200};
    ps_quat_normalize(ps_quat_axis_angle(ps_v3(1,0,0),PS_PI/2),&plane.orientation);
    ps_scene_push(s,&plane);
    ps_scene_add_id(s,8,PS_LINE,ps_v3(-.5,.5,0),ps_v3(.5,.5,0),.01,0xaaaaffff);ps_scene_set_parent(s,8,200);
}
static void destroy(ps_context *c){(void)c;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={sizeof api,PS_ABI_VERSION,
#ifdef PS_TEST_FRAME_CAP_MISSING
        PS_EXPERIMENT_SCENE_HIERARCHY,
#else
        PS_EXPERIMENT_SCENE_HIERARCHY|PS_EXPERIMENT_SCENE_FRAMES,
#endif
        "Coordinate frames",create,reset,step,scene,destroy,NULL};return &api;
}
