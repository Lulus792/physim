#include "physim/experiment.h"
#include "physim/electromagnetism.h"
#include "physim/units.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
/* Constant-source series RC, ideal linear components; all values in SI. */
typedef struct { double resistance,capacitance,source,initial,voltage,initial_energy; } circuit;
static ps_result measure(ps_context *c,double voltage,double elapsed) {
    circuit *s=c->user;double current=0,power=0,energy=0;
    ps_result r=ps_resistor_current(s->source-voltage,s->resistance,&current);
    if(r==PS_OK)r=ps_resistor_power(s->source-voltage,s->resistance,&power);
    if(r==PS_OK)r=ps_capacitor_energy(s->capacitance,voltage,&energy);
    double rate=elapsed/(s->resistance*s->capacitance);
    double difference=s->initial-s->source;
    double work=s->source*s->capacitance*(-difference)*(-expm1(-rate));
    double heat=.5*s->capacitance*difference*difference*(-expm1(-2*rate));
    double values[]={voltage,current,power,energy,heat,work,energy+heat-work};
    if(r==PS_OK)for(unsigned i=0;i<7;i++)c->values[i]=values[i];
    return r;
}
static ps_result reset(ps_context *c) {
    circuit *s=c->user;ps_result r=measure(c,s->initial,0);if(r==PS_OK)s->voltage=s->initial;return r;
}
static ps_result create(ps_context *c) {
    circuit *s=calloc(1,sizeof *s);if(!s)return PS_MEMORY;c->user=s;
    ps_unit ohm={{2,1,-3,-2,0,0,0},1,"ohm"},farad={{-2,-1,4,2,0,0,0},1,"F"};
    ps_unit volt={{2,1,-3,-1,0,0,0},1,"V"},watt={{2,1,-3,0,0,0,0},1,"W"};
    ps_result r=ps_parameter_define_unit(c,"resistance","Series resistance in ohm",ohm,1000,1,1e6,&s->resistance);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"capacitance","Capacitance in farad",farad,.002,1e-6,1,&s->capacitance);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"sourceVoltage","Constant supply in V",volt,12,-1000,1000,&s->source);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"initialVoltage","Initial capacitor voltage in V",volt,0,-1000,1000,&s->initial);
    if(r==PS_OK)r=ps_capacitor_energy(s->capacitance,s->initial,&s->initial_energy);
    const char *names[]={"voltage","current","power.resistor","energy.capacitor","energy.dissipated","energy.source","energy.balance"};
    ps_unit units[]={volt,PS_AMPERE,watt,PS_JOULE,PS_JOULE,PS_JOULE,PS_JOULE};
    for(unsigned i=0;r==PS_OK && i<7;i++)if(ps_channel_add(c,names[i],units[i],names[i])!=(int)i)r=PS_LIMIT;
    if(r!=PS_OK)return r;
    snprintf(c->model_metadata,sizeof c->model_metadata,"model=constant-source series RC\nunits=SI\nintegrator=exact exponential\nenergy_reference=zero capacitor energy at zero voltage\nsource_work=Vs*C*(V-V0); positive into circuit\nheat=exact integral of I^2*R; stable expm1\nexcluded=inductance,parasitics,nonlinear components,source switching\n");
    return reset(c);
}
static ps_result step(ps_context *c,double dt) {
    circuit *s=c->user;double next;
    ps_result r=ps_rc_voltage_step(s->resistance,s->capacitance,s->voltage,s->source,dt,&next);
    if(r==PS_OK)r=measure(c,next,c->time_s+dt);if(r==PS_OK)s->voltage=next;return r;
}
static void scene(ps_context *c,ps_scene *out) {
    circuit *s=c->user;ps_vec3 a=ps_v3(-.6,s->voltage*.03,0),b=ps_v3(.6,0,0);
    ps_scene_add_id(out,1,PS_SPHERE,a,a,.2,0xf2a052ff);
    ps_scene_add_id(out,2,PS_SPHERE,b,b,.2,0x53aeefff);
    ps_scene_add_id(out,3,PS_LINE,a,b,.015,0xc8d7eaff);
}
static void destroy(ps_context *c) { free(c->user);c->user=NULL; }
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,
        .name="Series RC: voltage and energy",.create=create,.reset=reset,.step=step,.build_scene=scene,.destroy=destroy};return &api;
}
