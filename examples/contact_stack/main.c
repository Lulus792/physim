#include "physim/experiment.h"
#include "physim/contact_world.h"
#include "physim/units.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
    ps_body bodies[5];
    ps_collider colliders[5];
    ps_contact_world world;
    ps_contact_world_result last;
    ps_contact_solver solver;
    double warm_fraction;
} experiment;
static ps_result measure(ps_context *c) {
    experiment *e=c->user;double energy=0;
    for(unsigned i=1;i<5;i++){double k;ps_result result=ps_body_kinetic_energy(&e->bodies[i],&k);if(result!=PS_OK)return result;energy+=k;}
    c->values[0]=e->bodies[1].position_m.y;c->values[1]=e->bodies[4].position_m.y;
    c->values[2]=e->bodies[4].velocity_m_s.y;c->values[3]=energy;
    c->values[4]=e->last.count;c->values[5]=e->last.matched;c->values[6]=e->last.warmed;
    c->values[7]=e->last.created;c->values[8]=e->last.ended;
    c->values[9]=e->last.solution.max_normal_error_m_s;c->values[10]=e->last.solution.max_projection_error_m;
    return PS_OK;
}
static ps_result reset(ps_context *c) {
    experiment *e=c->user;ps_contact_world_settings settings=PS_CONTACT_WORLD_DEFAULT;settings.warm_fraction=e->warm_fraction;
    ps_result result=ps_contact_world_init(&e->world,sizeof e->world,&settings);if(result!=PS_OK)return result;
    memset(&e->last,0,sizeof e->last);
    result=ps_body_sphere(0,.5,&e->bodies[0]);if(result!=PS_OK)return result;
    e->colliders[0]=(ps_collider){1,0,PS_COLLIDER_PLANE,{0,0,0},{0,1,0}};
    for(unsigned i=1;i<5;i++) {
        result=ps_body_box(1,ps_v3(1,1,1),&e->bodies[i]);if(result!=PS_OK)return result;
        e->bodies[i].position_m.y=i-.5;e->colliders[i]=(ps_collider){i+1,i,PS_COLLIDER_BOX,{1,1,1},{0,0,0}};
    }
    return measure(c);
}
static ps_result create(ps_context *c) {
    experiment *e=calloc(1,sizeof *e);if(!e)return PS_MEMORY;c->user=e;
    e->solver=PS_CONTACT_SOLVER_DEFAULT;double iterations=4;
    ps_result result=ps_parameter_define(c,"warmStart","Warmstart fraction",1,0,1,&e->warm_fraction);
    if(result==PS_OK)result=ps_parameter_define(c,"iterations","Velocity/projection iterations",4,1,256,&iterations);
    if(result!=PS_OK)return result;
    if(iterations!=floor(iterations))return PS_INVALID;
    e->solver.iterations=(uint32_t)iterations;
    const char *names[]={"bottom.height","top.height","top.velocity","energy.kinetic","contacts","contacts.matched","contacts.warmed","contacts.created","contacts.ended","solver.normalError","solver.projectionError"};
    for(unsigned i=0;i<11;i++) {
        ps_unit unit=i<2 || i==10?PS_METRE:i==2 || i==9?PS_VELOCITY:i==3?PS_JOULE:PS_ONE;
        if(ps_channel_add(c,names[i],unit,names[i])<0)return PS_INVALID;
    }
    snprintf(c->model_metadata,sizeof c->model_metadata,"model=four homogeneous unit boxes on a static plane\ngravity_m_s2=-9.80665\ncontacts=automatic sphere/box/plane discrete\ncontact_ids=stable collider IDs with local anchors\nsolver=projected graph impulses with warm history\nccd=none\n");
    return reset(c);
}
static ps_result step(ps_context *c,double dt) {
    experiment *e=c->user;
    for(unsigned i=1;i<5;i++){ps_result result=ps_body_step(&e->bodies[i],ps_v3(0,-9.80665,0),ps_v3(0,0,0),dt);if(result!=PS_OK)return result;}
    ps_result result=ps_contact_world_solve(&e->world,e->bodies,5,e->colliders,5,&e->solver,dt,&e->last);
    if(result!=PS_OK){snprintf(c->error,sizeof c->error,"Contact world failed: %s",ps_result_string(result));return result;}
    return measure(c);
}
static void scene(ps_context *c,ps_scene *s) {
    experiment *e=c->user;ps_scene_add_id(s,1,PS_PLANE,ps_v3(0,0,0),ps_v3(8,0,8),0,0x667582ff);
    const uint32_t colors[]={0x318eefff,0x46b49fff,0x70c1e8ff,0x928bdfff};
    for(unsigned i=1;i<5;i++) {
        ps_scene_add_id(s,i+1,PS_BOX,e->bodies[i].position_m,ps_v3(1,1,1),0,colors[i-1]);
        s->objects[s->count-1].orientation=e->bodies[i].orientation;
    }
    for(unsigned i=0;i<e->world.count && i<10;i++) {
        const ps_contact *contact=&e->world.contacts[i].constraint.contact;
        ps_scene_add_id(s,100+i,PS_POINT,contact->point_m,ps_v3(0,0,0),.03,0xffb84aff);
        ps_scene_add_id(s,200+i,PS_ARROW,contact->point_m,ps_vadd(contact->point_m,ps_vscale(contact->normal,.2)),.008,0xffb84aff);
    }
}
static void destroy(ps_context *c){free(c->user);c->user=NULL;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void){static const ps_experiment_api api={sizeof api,PS_ABI_VERSION,0,"Persistent contact stack",create,reset,step,scene,destroy,NULL};return &api;}
