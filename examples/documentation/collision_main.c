#include "physim/collision.h"
#include "physim/experiment.h"
#include "physim/units.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
/* One central collision in vacuum; no external force, friction or initial spin. */
typedef struct {
    double mass_a,mass_b,speed_a,speed_b,restitution;
    ps_body a,b;
    double lost,event_time,impulse;
    ps_vec3 event_point;
    bool collided;
} experiment;
static const double radius=.2;
static ps_result measure(ps_context *c,const experiment *e) {
    double ka,kb;ps_result r=ps_body_kinetic_energy(&e->a,&ka);
    if(r==PS_OK)r=ps_body_kinetic_energy(&e->b,&kb);
    if(r!=PS_OK)return r;
    double values[]={e->a.position_m.x,e->b.position_m.x,e->a.velocity_m_s.x,e->b.velocity_m_s.x,
        ka+kb,e->mass_a*e->a.velocity_m_s.x+e->mass_b*e->b.velocity_m_s.x,e->lost,
        ka+kb+e->lost,e->impulse,e->event_time,e->collided?1:0};
    for(unsigned i=0;i<11;i++)if(!isfinite(values[i]))return PS_NUMERIC;
    for(unsigned i=0;i<11;i++)c->values[i]=values[i];
    return PS_OK;
}
static ps_result reset(ps_context *c) {
    experiment *e=c->user,next=*e;
    ps_result r=ps_body_sphere(e->mass_a,radius,&next.a);
    if(r==PS_OK)r=ps_body_sphere(e->mass_b,radius,&next.b);
    if(r!=PS_OK)return r;
    next.a.position_m=ps_v3(-1,0,0);next.b.position_m=ps_v3(1,0,0);
    next.a.velocity_m_s=ps_v3(e->speed_a,0,0);next.b.velocity_m_s=ps_v3(e->speed_b,0,0);
    next.lost=next.event_time=next.impulse=0;next.event_point=ps_v3(0,0,0);next.collided=false;
    r=measure(c,&next);if(r==PS_OK)*e=next;return r;
}
static ps_result create(ps_context *c) {
    experiment *e=calloc(1,sizeof *e);if(!e)return PS_MEMORY;c->user=e;
    ps_result r=ps_parameter_define_unit(c,"massA","Sphere A mass in kilograms",PS_KILOGRAM,1,.1,10,&e->mass_a);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"massB","Sphere B mass in kilograms",PS_KILOGRAM,1,.1,10,&e->mass_b);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"velocityA","Sphere A initial X velocity",PS_VELOCITY,.6,-5,5,&e->speed_a);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"velocityB","Sphere B initial X velocity",PS_VELOCITY,-.6,-5,5,&e->speed_b);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"restitution","Normal coefficient of restitution",PS_ONE,1,0,1,&e->restitution);
    if(r!=PS_OK)return r;
    ps_unit momentum={{1,1,-1,0,0,0,0},1,"kg m/s"};
    const char *names[]={"a.position","b.position","a.velocity","b.velocity","energy","momentum.x",
        "energy.dissipated","energy.balance","a.impulse","collision.time","collision.count"};
    ps_unit units[]={PS_METRE,PS_METRE,PS_VELOCITY,PS_VELOCITY,PS_JOULE,momentum,PS_JOULE,PS_JOULE,momentum,PS_SECOND,PS_ONE};
    for(unsigned i=0;i<11;i++)if(ps_channel_add(c,names[i],units[i],names[i])!=(int)i)return PS_LIMIT;
    snprintf(c->model_metadata,sizeof c->model_metadata,
        "model=one central collision of two rigid spheres\nmedium=vacuum\ngravity=none\nfriction=0\ninitial_spin=0\nradius_m=0.2\ninitial_positions_m=-1,1\nmass_a_kg=%.17g\nmass_b_kg=%.17g\nvelocity_a_m_s=%.17g\nvelocity_b_m_s=%.17g\nrestitution=%.17g\nccd=linear sphere sweep, impulse, remaining time\ndissipation=0.5*reduced_mass*(1-e^2)*closing_speed^2\nevent_time_valid=collision.count==1\nexcluded=external forces, rotation, deformation, further contacts\n",e->mass_a,e->mass_b,e->speed_a,e->speed_b,e->restitution);
    return reset(c);
}
static ps_result step(ps_context *c,double dt) {
    if(!isfinite(dt) || dt<=0 || !isfinite(c->time_s+dt) || c->time_s+dt==c->time_s)return PS_INVALID;
    experiment *e=c->user,next=*e;ps_vec3 zero=ps_v3(0,0,0);next.impulse=0;
    double first=dt;bool touching=false;ps_sweep_hit hit;ps_result r=PS_OK;
    if(!next.collided) {
        r=ps_sweep_spheres(&next.a,radius,ps_vscale(next.a.velocity_m_s,dt),&next.b,radius,
            ps_vscale(next.b.velocity_m_s,dt),&hit,&touching);
        if(r!=PS_OK)return r;
        if(touching)first=dt*hit.fraction;
    }
    if(first>0)r=ps_body_step(&next.a,zero,zero,first);
    if(r==PS_OK && first>0)r=ps_body_step(&next.b,zero,zero,first);
    if(r==PS_OK && touching) {
        double closing=next.a.velocity_m_s.x-next.b.velocity_m_s.x;
        double reduced=next.mass_a*next.mass_b/(next.mass_a+next.mass_b);
        ps_vec3 impulse;
        r=ps_contact_resolve(&next.a,&next.b,&hit.contact,next.restitution,0,&impulse);
        if(r==PS_OK) {
            next.impulse=impulse.x;next.lost=.5*reduced*(1-next.restitution*next.restitution)*closing*closing;
            next.event_time=c->time_s+first;next.event_point=hit.contact.point_m;next.collided=true;
            double remaining=dt-first;
            if(remaining>0)r=ps_body_step(&next.a,zero,zero,remaining);
            if(r==PS_OK && remaining>0)r=ps_body_step(&next.b,zero,zero,remaining);
        }
    }
    if(r==PS_OK)r=measure(c,&next);
    if(r==PS_OK)*e=next;
    return r;
}
static void scene(ps_context *c,ps_scene *s) {
    experiment *e=c->user;
    ps_scene_add_id(s,1,PS_SPHERE,e->a.position_m,e->a.position_m,radius,0x53dec2ff);
    ps_scene_add_id(s,2,PS_ARROW,e->a.position_m,ps_vadd(e->a.position_m,e->a.velocity_m_s),.006,0x53dec2ff);
    (void)ps_scene_label_id(s,3,e->a.position_m,"Kugel A",0x53dec2ff);
    ps_scene_add_id(s,4,PS_SPHERE,e->b.position_m,e->b.position_m,radius,0xf2c572ff);
    ps_scene_add_id(s,5,PS_ARROW,e->b.position_m,ps_vadd(e->b.position_m,e->b.velocity_m_s),.006,0xf2c572ff);
    (void)ps_scene_label_id(s,6,e->b.position_m,"Kugel B",0xf2c572ff);
    (void)ps_scene_label_id(s,7,ps_v3(0,.6,0),"Pfeile: Geschwindigkeit [1 m pro m/s]",0xb9c0cdff);
    (void)ps_scene_group(s,100,0,"Zentraler Stoss");
    for(unsigned i=1;i<=7;i++)(void)ps_scene_set_parent(s,i,100);
    if(e->collided) {
        ps_scene_add_id(s,8,PS_POINT,e->event_point,e->event_point,.035,0xff8eafff);
        (void)ps_scene_label_id(s,9,e->event_point,"Stosszeit siehe collision.time",0xff8eafff);
        (void)ps_scene_set_parent(s,8,100);(void)ps_scene_set_parent(s,9,100);
    }
}
static void destroy(ps_context *c){free(c->user);c->user=NULL;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,
        .capabilities=PS_EXPERIMENT_SCENE_HIERARCHY,.name="Central elastic and inelastic collision",
        .create=create,.reset=reset,.step=step,.build_scene=scene,.destroy=destroy};return &api;
}
