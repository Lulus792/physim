#include "physim/experiment.h"
#include "physim/measurement.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct { double mean_x,sigma_x,mean_y,sigma_y,vx,vy,time; } projectile;
static const double gravity=9.80665;
static ps_result measure(ps_context *c,const projectile *p,double time) {
    double x=-2+p->vx*time,y=p->vy*time-.5*gravity*time*time,vy=p->vy-gravity*time;
    double values[]={x,y,p->vx,vy,-2+p->mean_x*time,p->mean_y*time-.5*gravity*time*time,
        p->sigma_x*time,p->sigma_y*time,.5*(p->vx*p->vx+vy*vy)+gravity*y};
    for(unsigned i=0;i<9;i++)if(!isfinite(values[i]))return PS_NUMERIC;
    for(unsigned i=0;i<9;i++)c->values[i]=values[i];
    return PS_OK;
}
static ps_result reset(ps_context *c) {
    projectile *p=c->user,next=*p;ps_rng rng;ps_rng_seed(&rng,c->seed);
    ps_result r=ps_distribution_sample((ps_distribution){PS_DIST_NORMAL,p->mean_x,p->sigma_x},&rng,&next.vx);
    if(r==PS_OK)r=ps_distribution_sample((ps_distribution){PS_DIST_NORMAL,p->mean_y,p->sigma_y},&rng,&next.vy);
    if(r==PS_OK)r=measure(c,&next,0);
    if(r==PS_OK){next.time=0;*p=next;c->rng=rng;}return r;
}
static ps_result create(ps_context *c) {
    projectile *p=calloc(1,sizeof *p);if(!p)return PS_MEMORY;c->user=p;
    ps_result r=ps_parameter_define_unit(c,"meanVx","Mean initial horizontal velocity",PS_VELOCITY,3,-20,20,&p->mean_x);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"sigmaVx","Initial horizontal standard deviation",PS_VELOCITY,.15,0,5,&p->sigma_x);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"meanVy","Mean initial vertical velocity",PS_VELOCITY,5,-20,20,&p->mean_y);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"sigmaVy","Initial vertical standard deviation",PS_VELOCITY,.25,0,5,&p->sigma_y);
    if(r!=PS_OK)return r;
    const char *names[]={"position.x","position.y","velocity.x","velocity.y","nominal.x","nominal.y","uncertainty.x","uncertainty.y","energy"};
    for(unsigned i=0;i<9;i++)if(ps_channel_add(c,names[i],i==2 || i==3?PS_VELOCITY:i==8?PS_JOULE:PS_METRE,names[i])!=(int)i)return PS_LIMIT;
    snprintf(c->model_metadata,sizeof c->model_metadata,
        "model=uncertain initial velocities, exact vacuum projectile\nmass_kg=1\ngravity_m_s2=9.80665\ninitial_position_m=-2,0\nvx_distribution=normal\nvy_distribution=normal\nvelocity_dependence=independent model draws\nintegrator=exact ballistic propagation\nuncertainty_channels=population standard deviations, not sensor errors\nexcluded=ground, drag, sensor noise, parameter drift\n");
    return reset(c);
}
static ps_result step(ps_context *c,double dt) {
    if(!isfinite(dt) || dt<=0 || !isfinite(c->time_s+dt) || c->time_s+dt==c->time_s)return PS_INVALID;
    projectile *p=c->user;double time=c->time_s+dt;ps_result r=measure(c,p,time);
    if(r==PS_OK)p->time=time;
    return r;
}
static void scene(ps_context *c,ps_scene *s) {
    projectile *p=c->user;ps_vec3 actual=ps_v3(c->values[0],c->values[1],0),nominal=ps_v3(c->values[4],c->values[5],0);
    ps_scene_add_id(s,1,PS_SPHERE,actual,actual,.09,0x53dec2ff);
    ps_scene_add_id(s,2,PS_POINT,nominal,nominal,.05,0x779bccff);
    ps_scene_add_id(s,3,PS_ARROW,actual,ps_vadd(actual,ps_v3(.12*c->values[2],.12*c->values[3],0)),0,0x53dec2ff);
    (void)ps_scene_label_id(s,4,actual,"Einzellauf",0x53dec2ff);
    (void)ps_scene_label_id(s,5,nominal,"Erwartungswert",0x779bccff);
    ps_scene_add_id(s,6,PS_LINE,ps_vadd(nominal,ps_v3(-c->values[6],0,0)),ps_vadd(nominal,ps_v3(c->values[6],0,0)),.006,0xf2c572ff);
    ps_scene_add_id(s,7,PS_LINE,ps_vadd(nominal,ps_v3(0,-c->values[7],0)),ps_vadd(nominal,ps_v3(0,c->values[7],0)),.006,0xf2c572ff);
    (void)ps_scene_label_id(s,8,ps_v3(-.5,1.6,0),"Kreuz: eine Populations-Standardabweichung",0xf2c572ff);
    if(p->time>0) {
        ps_vec3 a[32],b[32];for(unsigned i=0;i<32;i++) {
            double t=p->time*i/31;a[i]=ps_v3(-2+p->vx*t,p->vy*t-.5*gravity*t*t,0);
            b[i]=ps_v3(-2+p->mean_x*t,p->mean_y*t-.5*gravity*t*t,0);
        }
        (void)ps_scene_polyline_id(s,9,a,32,.008,0x53dec2ff);(void)ps_scene_polyline_id(s,10,b,32,.004,0x779bccff);
    }
    (void)ps_scene_group(s,100,0,"Unsicherer Vakuumwurf");
    for(unsigned i=1;i<=8;i++)(void)ps_scene_set_parent(s,i,100);
    if(p->time>0){(void)ps_scene_set_parent(s,9,100);(void)ps_scene_set_parent(s,10,100);}
}
static void destroy(ps_context *c){free(c->user);c->user=NULL;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,.capabilities=PS_EXPERIMENT_SCENE_HIERARCHY,
        .name="Monte Carlo initial velocities",.create=create,.reset=reset,.step=step,.build_scene=scene,.destroy=destroy};return &api;
}
