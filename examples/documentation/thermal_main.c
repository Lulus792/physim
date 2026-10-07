#include "physim/experiment.h"
#include "physim/thermodynamics.h"
#include "physim/units.h"
#include <stdio.h>
#include <stdlib.h>
/* Isolated well-mixed bodies, constant capacities; gas A at fixed volume. */
typedef struct { double ca,cb,g,ta0,tb0,n,cv,volume;ps_vec2 temperatures; } thermal;
static ps_result measure(ps_context *c,ps_vec2 temperatures) {
    thermal *s=c->user;double power=0,pressure=0,energy=0;
    ps_result r=ps_heat_flow(s->g,temperatures.x,temperatures.y,&power);
    if(r==PS_OK)r=ps_ideal_gas_pressure(s->n,temperatures.x,s->volume,&pressure);
    if(r==PS_OK)r=ps_ideal_gas_energy(s->n,s->cv,temperatures.x,&energy);
    double values[]={temperatures.x,temperatures.y,power,pressure,energy,
                    s->ca*temperatures.x+s->cb*temperatures.y};
    if(r==PS_OK)for(unsigned i=0;i<6;i++)c->values[i]=values[i];
    return r;
}
static ps_result reset(ps_context *c) {
    thermal *s=c->user;ps_vec2 initial={s->ta0,s->tb0};
    ps_result r=measure(c,initial);if(r==PS_OK)s->temperatures=initial;return r;
}
static ps_result create(ps_context *c) {
    thermal *s=calloc(1,sizeof *s);if(!s)return PS_MEMORY;c->user=s;
    ps_unit capacity={{2,1,-2,0,-1,0,0},1,"J/K"};
    ps_unit conductance={{2,1,-3,0,-1,0,0},1,"W/K"};
    ps_unit watt={{2,1,-3,0,0,0,0},1,"W"};
    ps_result r=ps_parameter_define_unit(c,"capacityA","Gas A heat capacity in J/K",capacity,100,1,10000,&s->ca);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"capacityB","Body B heat capacity in J/K",capacity,300,1,10000,&s->cb);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"conductance","Constant coupling in W/K",conductance,5,0,10000,&s->g);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"temperatureA","Initial A temperature in K",PS_KELVIN,400,1,10000,&s->ta0);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"temperatureB","Initial B temperature in K",PS_KELVIN,300,1,10000,&s->tb0);
    s->cv=12.5;s->n=s->ca/s->cv;s->volume=.1;
    const char *names[]={"temperature.a","temperature.b","heat.power","gas.pressure","gas.energy","energy.balance"};
    ps_unit units[]={PS_KELVIN,PS_KELVIN,watt,PS_PASCAL,PS_JOULE,PS_JOULE};
    for(unsigned i=0;r==PS_OK && i<6;i++)if(ps_channel_add(c,names[i],units[i],names[i])!=(int)i)r=PS_LIMIT;
    if(r!=PS_OK)return r;
    snprintf(c->model_metadata,sizeof c->model_metadata,"model=isolated constant-capacity thermal pair\nunits=Kelvin,SI\nintegrator=exact exponential\ngas=ideal; fixed volume 0.1 m^3; cv=12.5 J/(mol K); n=capacityA/cv\nexcluded=spatial gradients,radiation,latent heat,real gas\n");
    return reset(c);
}
static ps_result step(ps_context *c,double dt) {
    thermal *s=c->user;ps_vec2 next;
    ps_result r=ps_thermal_pair_step(s->ca,s->temperatures.x,s->cb,s->temperatures.y,s->g,dt,&next);
    if(r==PS_OK)r=measure(c,next);if(r==PS_OK)s->temperatures=next;return r;
}
static void scene(ps_context *c,ps_scene *out) {
    thermal *s=c->user;
    ps_vec3 a=ps_v3(-.6,(s->temperatures.x-300)*.005,0),b=ps_v3(.6,(s->temperatures.y-300)*.005,0);
    ps_scene_add_id(out,1,PS_SPHERE,a,a,.2,0xf2a052ff);
    ps_scene_add_id(out,2,PS_SPHERE,b,b,.2,0x53aeefff);
    ps_scene_add_id(out,3,PS_LINE,a,b,.015,0xc8d7eaff);
}
static void destroy(ps_context *c) { free(c->user);c->user=NULL; }
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,
        .name="Ideal gas and isolated thermal exchange",.create=create,.reset=reset,.step=step,.build_scene=scene,.destroy=destroy};return &api;
}
