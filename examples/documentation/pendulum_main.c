#include "physim/experiment.h"
#include "physim/numerics.h"
#include "physim/units.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
/* One vacuum model; integrator=0 Euler, 1 symplectic, 2 RK4, 3 Verlet, 4 RK45. */
typedef struct { double state[2], length, angle, method; } pendulum;
static const double gravity=9.80665;
static const char *methods[]={"Euler","symplectic Euler","RK4","velocity Verlet","Dormand-Prince 5(4)"};
static void slope(double time,const double *state,double *out,void *user) {
    (void)time;const pendulum *p=user;out[0]=state[1];out[1]=-gravity/p->length*sin(state[0]);
}
static void acceleration(double time,const double *q,double *out,void *user) {
    (void)time;const pendulum *p=user;out[0]=-gravity/p->length*sin(q[0]);
}
static ps_result measure(ps_context *c,const double state[2]) {
    pendulum *p=c->user;double a=state[0],w=state[1];
    double values[]={a,w,p->length*sin(a),-p->length*cos(a),
        .5*p->length*p->length*w*w+gravity*p->length*(1-cos(a)),a,
        p->length*cos(a)*w,p->length*sin(a)*w,p->length*fabs(w)};
    for(unsigned i=0;i<9;i++)if(!isfinite(values[i]))return PS_NUMERIC;
    for(unsigned i=0;i<9;i++)c->values[i]=values[i];
    return PS_OK;
}
static ps_result reset(ps_context *c) {
    pendulum *p=c->user;double initial[]={p->angle,0};ps_result r=measure(c,initial);
    if(r==PS_OK){p->state[0]=initial[0];p->state[1]=0;}return r;
}
static ps_result create(ps_context *c) {
    pendulum *p=calloc(1,sizeof *p);if(!p)return PS_MEMORY;c->user=p;
    ps_result r=ps_parameter_define_unit(c,"length","Pendulum length in metres",PS_METRE,1.5,.1,10,&p->length);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"initialAngle","Initial angle in radians",PS_RADIAN,.45,-1.5,1.5,&p->angle);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"integrator","0 Euler; 1 symplectic; 2 RK4; 3 Verlet; 4 RK45",PS_ONE,2,0,4,&p->method);
    if(r!=PS_OK)return r;
    if(p->method!=floor(p->method)){snprintf(c->error,sizeof c->error,"Integrator must be an integer from 0 to 4");return PS_INVALID;}
    ps_unit rate={{0,0,-1,0,0,0,0},1,"rad/s"};
    ps_unit velocity={{1,0,-1,0,0,0,0},1,"m/s"};
    const char *names[]={"angle","angular_velocity","position.x","position.y","energy","sensor.angle","velocity.x","velocity.y","speed"};
    ps_unit units[]={PS_RADIAN,rate,PS_METRE,PS_METRE,PS_JOULE,PS_RADIAN,velocity,velocity,velocity};
    for(unsigned i=0;i<9;i++)if(ps_channel_add(c,names[i],units[i],names[i])!=(int)i)return PS_LIMIT;
    snprintf(c->model_metadata,sizeof c->model_metadata,
        "model=point pendulum, massless rigid rod, vacuum\nlength_m=%.17g\ninitial_angle_rad=%.17g\nmass_kg=1\ngravity_m_s2=9.80665\nintegrator=%s\nrk45_absolute_tolerance=1e-10\nrk45_relative_tolerance=1e-8\nexcluded=drag, drive, rod inertia, contacts, sensor noise\n",p->length,p->angle,methods[(unsigned)p->method]);
    return reset(c);
}
static ps_ode_options options(double proposed,double minimum,double maximum) {
    ps_ode_options o=ps_ode_options_default();o.absolute_tolerance=1e-10;o.relative_tolerance=1e-8;
    o.initial_step=proposed;o.minimum_step=minimum;o.maximum_step=maximum;return o;
}
static ps_result step(ps_context *c,double dt) {
    if(!isfinite(dt) || dt<=0 || !isfinite(c->time_s+dt) || c->time_s+dt==c->time_s)return PS_INVALID;
    pendulum *p=c->user;double next[]={p->state[0],p->state[1]};ps_result r=PS_OK;
    if(p->method==PS_SYMPLECTIC) {
        double d[2];slope(c->time_s,next,d,p);ps_symplectic_step(&next[0],&next[1],d[1],dt);
    } else if(p->method==PS_VERLET)r=ps_verlet_step(acceleration,p,c->time_s,dt,&next[0],&next[1],1);
    else if(p->method==PS_RK45) {
        ps_ode_options o=options(dt,fmin(1e-14,dt),dt);
        r=ps_ode_integrate(slope,p,c->time_s,c->time_s+dt,next,2,&o,NULL);
    } else r=ps_ode_step((ps_integrator)(unsigned)p->method,slope,p,c->time_s,dt,next,2);
    if(r==PS_OK)r=measure(c,next);
    if(r==PS_OK){p->state[0]=next[0];p->state[1]=next[1];}return r;
}
static ps_result adaptive_step(ps_context *c,double dt,double minimum,double maximum,ps_step_interval *interval) {
    pendulum *p=c->user;
    if(p->method!=PS_RK45){snprintf(c->error,sizeof c->error,"Adaptive mode requires integrator=4");return PS_INVALID;}
    double next[]={p->state[0],p->state[1]};ps_ode_options o=options(dt,minimum,maximum);
    ps_ode_report report;ps_ode_diagnostic diagnostic;
    ps_result r=ps_ode_step_diagnosed(slope,p,c->time_s,c->time_s+dt,next,2,&o,&report,&diagnostic);
    if(r!=PS_OK){snprintf(c->error,sizeof c->error,"%s",ps_ode_diagnostic_string(diagnostic.reason));return r;}
    r=measure(c,next);
    if(r==PS_OK){p->state[0]=next[0];p->state[1]=next[1];*interval=(ps_step_interval){report.reached_time-c->time_s,report.next_step};}
    return r;
}
static void scene(ps_context *c,ps_scene *s) {
    pendulum *p=c->user;ps_vec3 origin=ps_v3(0,0,0),bob=ps_v3(c->values[2],c->values[3],0);
    ps_scene_add_id(s,1,PS_LINE,origin,bob,0,0xb5c4d8ff);
    ps_scene_add_id(s,2,PS_SPHERE,origin,origin,.045,0xe6edf3ff);
    ps_scene_add_id(s,3,PS_SPHERE,bob,bob,.12,0x53dec2ff);
    ps_vec3 v=ps_v3(p->length*cos(c->values[0])*c->values[1],p->length*sin(c->values[0])*c->values[1],0);
    ps_scene_add_id(s,4,PS_ARROW,bob,ps_vadd(bob,ps_vscale(v,.3)),0,0xf2c572ff);
    (void)ps_scene_label_id(s,5,origin,"Aufhaengung",0xb5c4d8ff);
    (void)ps_scene_label_id(s,6,bob,"Pendelmasse",0x53dec2ff);
    (void)ps_scene_group(s,100,0,"Pendel");(void)ps_scene_group(s,101,100,"Bewegte Masse");
    (void)ps_scene_set_parent(s,1,100);(void)ps_scene_set_parent(s,2,100);(void)ps_scene_set_parent(s,3,101);
    (void)ps_scene_set_parent(s,4,3);(void)ps_scene_set_parent(s,5,2);(void)ps_scene_set_parent(s,6,3);
    /* One kilogram; force arrows use 0.05 metres per newton. */
    ps_vec3 weight_end=ps_vadd(bob,ps_v3(0,-.05*gravity,0));
    double constraint=gravity*cos(c->values[0])+p->length*c->values[1]*c->values[1];
    ps_vec3 rod_end=ps_vadd(bob,ps_vscale(bob,-.05*constraint/p->length));
    ps_scene_add_id(s,7,PS_ARROW,bob,weight_end,0,0xe87979ff);
    ps_scene_add_id(s,8,PS_ARROW,bob,rod_end,0,0x91d28aff);
    (void)ps_scene_label_id(s,10,weight_end,"Gewicht · 0.05 m/N",0xe87979ff);
    (void)ps_scene_label_id(s,11,rod_end,"Stangenkraft · 0.05 m/N",0x91d28aff);
    (void)ps_scene_set_parent(s,7,3);(void)ps_scene_set_parent(s,8,3);
    (void)ps_scene_set_parent(s,10,7);(void)ps_scene_set_parent(s,11,8);
}
static void destroy(ps_context *c){free(c->user);c->user=NULL;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,
        .capabilities=PS_EXPERIMENT_SCENE_HIERARCHY|PS_EXPERIMENT_ADAPTIVE_STEPS,.name="Pendulum integrator comparison",
        .create=create,.reset=reset,.step=step,.build_scene=scene,.destroy=destroy,.adaptive_step=adaptive_step};return &api;
}
