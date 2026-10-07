#include "physim/experiment.h"
#include "physim/waves.h"
#include "physim/units.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_NODES 65
/* Linear taut string, fixed endpoints, one standing mode initially at rest. */
typedef struct {
    unsigned count,mode;double length,amplitude,tension,density,speed,dx,dt,theta;
    double previous[MAX_NODES],current[MAX_NODES];
} string_state;
static ps_result measure(ps_context *c,const double *previous,const double *current,double time) {
    string_state *s=c->user;double energy=0;
    for(unsigned j=1;j+1<s->count;j++) {
        double velocity=(current[j]-previous[j])/s->dt;
        energy+=.5*s->density*s->dx*velocity*velocity;
    }
    for(unsigned j=0;j+1<s->count;j++)
        energy+=.5*s->tension/s->dx*(current[j+1]-current[j])*(previous[j+1]-previous[j]);
    double reference=s->amplitude*sin(s->mode*PS_PI*.5)*cos(s->speed*s->mode*PS_PI/s->length*time);
    double center=current[s->count/2];
    double values[]={center,reference,center-reference,energy,s->speed*s->dt/s->dx,(double)s->count};
    for(unsigned j=0;j<6;j++)if(!isfinite(values[j]))return PS_NUMERIC;
    for(unsigned j=0;j<6;j++)c->values[j]=values[j];return PS_OK;
}
static ps_result reset(ps_context *c) {
    string_state *s=c->user;
    for(unsigned j=0;j<s->count;j++) {
        s->current[j]=j==0 || j+1==s->count?0:s->amplitude*sin(s->mode*PS_PI*j/(s->count-1));
        s->previous[j]=s->current[j]*cos(s->theta);
    }
    return measure(c,s->previous,s->current,0);
}
static ps_result create(ps_context *c) {
    string_state *s=calloc(1,sizeof *s);if(!s)return PS_MEMORY;c->user=s;
    double count,mode;ps_unit density={{-1,1,0,0,0,0,0},1,"kg/m"};
    ps_result r=ps_parameter_define_unit(c,"nodes","Odd nodes across string (17..65)",PS_ONE,65,17,65,&count);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"mode","Standing wave mode (1..4)",PS_ONE,1,1,4,&mode);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"length","String length in m",PS_METRE,1,.1,10,&s->length);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"amplitude","Initial amplitude in m",PS_METRE,.05,0,1,&s->amplitude);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"tension","Constant tension in N",PS_NEWTON,1,.0001,100,&s->tension);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"linearDensity","Linear density in kg/m",density,1,.0001,100,&s->density);
    if(r!=PS_OK)return r;
    if(count!=floor(count) || ((unsigned)count)%2==0 || mode!=floor(mode))return PS_INVALID;
    s->count=(unsigned)count;s->mode=(unsigned)mode;s->dx=s->length/(s->count-1);s->dt=c->dt_s;
    r=ps_string_wave_speed(s->tension,s->density,&s->speed);if(r!=PS_OK)return r;
    double courant=s->speed*s->dt/s->dx;
    if(!isfinite(s->dt) || s->dt<=0 || !isfinite(courant) || courant>1)return PS_INVALID;
    s->theta=2*asin(courant*sin(s->mode*PS_PI/(2*(s->count-1))));
    const char *names[]={"displacement.center","reference.center","error.center","energy.discrete","courant","nodes"};
    ps_unit units[]={PS_METRE,PS_METRE,PS_METRE,PS_JOULE,PS_ONE,PS_ONE};
    for(unsigned j=0;j<6;j++)if(ps_channel_add(c,names[j],units[j],names[j])!=(int)j)return PS_LIMIT;
    snprintf(c->model_metadata,sizeof c->model_metadata,"model=linear fixed-endpoint string\nintegrator=centered second-order leapfrog\ninitial_velocity=zero; previous mode initialized with cos(theta)\nstability=c*dt/dx<=1; constant dt\nenergy=half-step kinetic plus cross-time gradient potential\nexcluded=damping,forcing,variable medium,nonlinear stretch,2D/3D PDE\n");
    return reset(c);
}
static ps_result step(ps_context *c,double dt) {
    string_state *s=c->user;
    if(!isfinite(dt) || fabs(dt-s->dt)>8*DBL_EPSILON*s->dt)return PS_INVALID;
    double next[MAX_NODES];ps_result r=ps_string_wave_step(s->previous,s->current,s->count,s->speed,s->dx,dt,next);
    if(r==PS_OK)r=measure(c,s->current,next,c->time_s+dt);
    if(r==PS_OK){memcpy(s->previous,s->current,s->count*sizeof(double));memcpy(s->current,next,s->count*sizeof(double));}
    return r;
}
static void scene(ps_context *c,ps_scene *out) {
    string_state *s=c->user;ps_vec3 points[MAX_NODES];
    for(unsigned j=0;j<s->count;j++)points[j]=ps_v3(j*s->dx-s->length*.5,s->current[j],0);
    ps_scene_polyline_id(out,1,points,s->count,.002,0x53aeefff);
}
static void destroy(ps_context *c){free(c->user);c->user=NULL;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,
        .name="Standing wave on a fixed string",.create=create,.reset=reset,.step=step,.build_scene=scene,.destroy=destroy};return &api;
}
