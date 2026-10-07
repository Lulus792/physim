#include "physim/experiment.h"
#include "physim/properties.h"
#include <math.h>
#include <stdio.h>
/* Synthetic material data for a controlled T/P sweep, not measured water. */
static const double temperatures[]={273,373},pressures[]={0,500000},density_values[]={1145.4,1145.9,1125.4,1125.9};
static const ps_unit density_unit={{-3,1,0,0,0,0,0},1,"kg/m3"};
static ps_property density_property(void) {
    ps_property p={.model=PS_PROPERTY_TABLE,.name="density",.source="synthetic affine reference",.value_unit=density_unit,
        .minimum_temperature_k=273,.maximum_temperature_k=373,.minimum_pressure_pa=0,.maximum_pressure_pa=500000,
        .temperature_k=temperatures,.pressure_pa=pressures,.values_si=density_values,.temperature_count=2,.pressure_count=2};return p;
}
static ps_result measure(ps_context *c,double time) {
    double fraction=fmin(1,time),temperature=273+100*fraction,pressure=500000*fraction;
    ps_property property=density_property();ps_quantity density;
    ps_result r=ps_property_evaluate(&property,temperature,pressure,&density);
    if(r==PS_OK){c->values[0]=temperature;c->values[1]=pressure;c->values[2]=density.value;}return r;
}
static ps_result reset(ps_context *c){return measure(c,0);}
static ps_result create(ps_context *c) {
    const char *names[]={"temperature","pressure","density"};ps_unit units[]={PS_KELVIN,PS_PASCAL,density_unit};
    for(unsigned i=0;i<3;i++)if(ps_channel_add(c,names[i],units[i],names[i])!=(int)i)return PS_LIMIT;
    snprintf(c->model_metadata,sizeof c->model_metadata,"model=synthetic material property sweep\nproperty.density.source=synthetic affine reference\nproperty.density.unit=kg/m3\nproperty.domain=273..373 K; 0..500000 Pa\nproperty.axes.temperature=273,373\nproperty.axes.pressure=0,500000\nproperty.values=1145.4,1145.9,1125.4,1125.9\nproperty.interpolation=bilinear; no extrapolation\nexcluded=measured material data,phase transitions,uncertainty\n");return reset(c);
}
static ps_result step(ps_context *c,double dt){return measure(c,c->time_s+dt);}
static void scene(ps_context *c,ps_scene *out){ps_scene_add_id(out,1,PS_SPHERE,ps_v3(c->time_s,0,0),ps_v3(0,0,0),.1,0x53aeefff);}
static void destroy(ps_context *c){(void)c;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,.name="Synthetic material property sweep",
        .create=create,.reset=reset,.step=step,.build_scene=scene,.destroy=destroy};return &api;
}
