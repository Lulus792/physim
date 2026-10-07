#include "physim/experiment.h"
#include "physim/fluid.h"
#include "physim/units.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CELLS 64
/* Passive steady pipe split prescribes velocity for a separate periodic tracer.
 * This is not a coupled Navier-Stokes branch flow or an open-boundary tracer. */
typedef struct {
    double radius,pipe_length,viscosity,density,inlet,diffusion,conductance;
    double pressures[4],flows[3],velocity,reynolds,concentration[CELLS];
} transport;
static ps_result measure(ps_context *c,const double *profile,double time,double dt) {
    transport *s=c->user;double dx=1.0/CELLS,mass=0;
    for(unsigned j=0;j<CELLS;j++)mass+=profile[j]*dx;
    double k=2*PS_PI,reference=.2+.1*cos(k*(.5-s->velocity*time))*exp(-s->diffusion*k*k*time);
    double stability=fabs(s->velocity)*dt/dx+2*s->diffusion*dt/(dx*dx);
    double values[]={profile[CELLS/2],reference,profile[CELLS/2]-reference,mass,s->flows[0],s->pressures[1],s->reynolds,stability};
    for(unsigned j=0;j<8;j++)if(!isfinite(values[j]))return PS_NUMERIC;
    for(unsigned j=0;j<8;j++)c->values[j]=values[j];return PS_OK;
}
static ps_result reset(ps_context *c) {
    transport *s=c->user;
    for(unsigned j=0;j<CELLS;j++)s->concentration[j]=.2+.1*cos(2*PS_PI*j/CELLS);
    return measure(c,s->concentration,0,c->dt_s);
}
static ps_result create(ps_context *c) {
    transport *s=calloc(1,sizeof *s);if(!s)return PS_MEMORY;c->user=s;
    ps_unit viscosity={{-1,1,-1,0,0,0,0},1,"Pa s"},density={{-3,1,0,0,0,0,0},1,"kg/m^3"};
    ps_unit diffusivity={{2,0,-1,0,0,0,0},1,"m^2/s"};
    ps_result r=ps_parameter_define_unit(c,"radius","Identical pipe radius in m",PS_METRE,.005,.0001,.01,&s->radius);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"pipeLength","Each pipe length in m",PS_METRE,1,.1,10,&s->pipe_length);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"viscosity","Newtonian viscosity in Pa s",viscosity,.01,.001,1,&s->viscosity);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"density","Fluid density for Reynolds in kg/m^3",density,1000,1,10000,&s->density);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"inletPressure","Signed inlet gauge pressure in Pa",PS_PASCAL,100,-100,100,&s->inlet);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"diffusivity","Tracer diffusivity in m^2/s",diffusivity,.0001,0,.0005,&s->diffusion);
    if(r==PS_OK)r=ps_pipe_conductance(s->radius,s->pipe_length,s->viscosity,&s->conductance);
    uint8_t fixed[]={1,0,1,1};double boundary[]={s->inlet,0,0,0};
    ps_pipe_edge edges[]={{0,1,s->conductance},{1,2,s->conductance},{1,3,s->conductance}};
    if(r==PS_OK)r=ps_pipe_network_solve(fixed,boundary,4,edges,3,s->pressures,s->flows);
    if(r==PS_OK)s->velocity=s->flows[0]/(PS_PI*s->radius*s->radius);
    if(r==PS_OK)r=ps_reynolds_number(s->density,s->velocity,2*s->radius,s->viscosity,&s->reynolds);
    if(r==PS_OK && s->reynolds>1000)r=PS_INVALID;
    double dx=1.0/CELLS;
    if(r==PS_OK && (!isfinite(c->dt_s) || c->dt_s<=0 || fabs(s->velocity)*c->dt_s/dx+2*s->diffusion*c->dt_s/(dx*dx)>1))r=PS_INVALID;
    if(r!=PS_OK)return r;
    ps_unit mass_area={{-2,1,0,0,0,0,0},1,"kg/m^2"},flow={{3,0,-1,0,0,0,0},1,"m^3/s"};
    const char *names[]={"concentration.center","reference.center","error.center","mass.perArea","flow.inlet","pressure.junction","reynolds","stability"};
    ps_unit units[]={density,density,density,mass_area,flow,PS_PASCAL,PS_ONE,PS_ONE};
    for(unsigned j=0;j<8;j++)if(ps_channel_add(c,names[j],units[j],names[j])!=(int)j)return PS_LIMIT;
    snprintf(c->model_metadata,sizeof c->model_metadata,"model=passive three-pipe split plus separate periodic tracer\nintegrator=upwind advection and explicit centered diffusion\nnetwork=fixed nodes 0,2,3; free node 1; edges 0->1,1->2,1->3\ntransport=64 periodic cells over 1 m; mean velocity from inlet pipe\nvalidity=Re<=1000; abs(v)*dt/dx+2*D*dt/dx^2<=1\nexcluded=turbulence,momentum evolution,compressibility,open tracer boundaries,CFD\n");
    return reset(c);
}
static ps_result step(ps_context *c,double dt) {
    transport *s=c->user;double next[CELLS];
    ps_result r=ps_transport_periodic_step(s->concentration,CELLS,s->velocity,s->diffusion,1.0/CELLS,dt,next);
    if(r==PS_OK)r=measure(c,next,c->time_s+dt,dt);
    if(r==PS_OK)memcpy(s->concentration,next,sizeof next);return r;
}
static void scene(ps_context *c,ps_scene *out) {
    transport *s=c->user;ps_vec3 profile[CELLS+1];
    /* Visual height scale 1 m per (kg/m^3), not a second spatial dimension. */
    for(unsigned j=0;j<=CELLS;j++)profile[j]=ps_v3((double)j/CELLS-.5,s->concentration[j%CELLS],0);
    ps_scene_polyline_id(out,1,profile,CELLS+1,.002,0x53aeefff);
    ps_vec3 nodes[]={ps_v3(-.6,-.4,0),ps_v3(-.2,-.4,0),ps_v3(.3,-.2,0),ps_v3(.3,-.6,0)};
    unsigned a[]={0,1,1},b[]={1,2,3};
    for(unsigned e=0;e<3;e++) {
        ps_vec3 from=nodes[s->flows[e]>=0?a[e]:b[e]],to=nodes[s->flows[e]>=0?b[e]:a[e]];
        ps_scene_add_id(out,e+2,s->flows[e]==0?PS_LINE:PS_ARROW,from,to,.012,0xf2a052ff);
    }
}
static void destroy(ps_context *c){free(c->user);c->user=NULL;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,
        .name="Pipe network and periodic tracer",.create=create,.reset=reset,.step=step,.build_scene=scene,.destroy=destroy};return &api;
}
