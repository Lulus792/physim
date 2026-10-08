#include "physim/experiment.h"
#include "physim/contact_world.h"
#include "physim/units.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {ps_body bodies[3];ps_ccd_collider models[3];ps_contact_solver solver;ps_ccd_step_result last;double speed;} experiment;
static ps_result measure(ps_context *c){
    experiment *e=c->user;double energy=0;
    for(unsigned i=0;i<3;i++){double k;ps_result result=ps_body_kinetic_energy(&e->bodies[i],&k);if(result!=PS_OK)return result;energy+=k;c->values[i]=e->bodies[i].position_m.x;c->values[i+3]=e->bodies[i].velocity_m_s.x;}
    c->values[6]=energy;c->values[7]=e->last.events;c->values[8]=e->last.contacts;c->values[9]=e->last.max_normal_error_m_s;c->values[10]=e->last.max_projection_error_m;return PS_OK;
}
static ps_result reset(ps_context *c){
    experiment *e=c->user;memset(&e->last,0,sizeof e->last);
    for(unsigned i=0;i<3;i++){ps_result result=ps_body_sphere(1,.5,&e->bodies[i]);if(result!=PS_OK)return result;e->bodies[i].position_m.x=-4+4*(double)i;e->bodies[i].velocity_m_s.x=i?0:e->speed;e->models[i]=(ps_ccd_collider){{3-i,i,PS_COLLIDER_SPHERE,{.5,0,0},{0,0,0}},NULL};}
    return measure(c);
}
static ps_result create(ps_context *c){
    experiment *e=calloc(1,sizeof *e);if(!e)return PS_MEMORY;c->user=e;
    ps_result result=ps_parameter_define(c,"speed","Initial speed",10,0,100,&e->speed);if(result!=PS_OK)return result;
    e->solver=PS_CONTACT_SOLVER_DEFAULT;e->solver.iterations=200;e->solver.restitution=1;e->solver.friction=0;e->solver.bounce_threshold_m_s=0;
    const char *names[]={"a.position","b.position","c.position","a.velocity","b.velocity","c.velocity","energy.kinetic","events","contacts","normal.error","projection.error"};
    for(unsigned i=0;i<11;i++){ps_unit unit=i<3 || i==10?PS_METRE:i<6 || i==9?PS_VELOCITY:i==6?PS_JOULE:PS_ONE;if(ps_channel_add(c,names[i],unit,names[i])<0)return PS_INVALID;}
    snprintf(c->model_metadata,sizeof c->model_metadata,"model=three equal homogeneous spheres in vacuum\nmass_kg=1\nradius_m=0.5\ncentres_m=-4,0,4\nrestitution=1\nfriction=0\nexternal_forces=none\nccd=earliest event batches with canonical IDs\ncontact_offset_m=0.000001\n");return reset(c);
}
static ps_result step(ps_context *c,double dt){
    experiment *e=c->user;ps_body candidate[3];memcpy(candidate,e->bodies,sizeof candidate);ps_ccd_step_result report;
    ps_result result=ps_ccd_step(candidate,3,e->models,3,NULL,NULL,dt,&e->solver,NULL,&report);
    if(result!=PS_OK){snprintf(c->error,sizeof c->error,"CCD event step failed: %s",ps_result_string(result));return result;}
    memcpy(e->bodies,candidate,sizeof candidate);e->last=report;return measure(c);
}
static void scene(ps_context *c,ps_scene *s){experiment *e=c->user;const uint32_t colors[]={0x3589ddff,0xf2a83fff,0x42b897ff};for(unsigned i=0;i<3;i++){ps_vec3 p=e->bodies[i].position_m;ps_scene_add_id(s,i+1,PS_SPHERE,p,p,.5,colors[i]);ps_scene_add_id(s,10+i,PS_ARROW,p,ps_vadd(p,ps_vscale(e->bodies[i].velocity_m_s,.1)),.008,colors[i]);}}
static void destroy(ps_context *c){free(c->user);c->user=NULL;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void){static const ps_experiment_api api={sizeof api,PS_ABI_VERSION,0,"Continuous three-body events",create,reset,step,scene,destroy,NULL};return &api;}
