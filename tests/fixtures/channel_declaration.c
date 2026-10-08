#include "physim/experiment.h"
#include "physim/measurement.h"
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
static ps_result publish(ps_context *c,double time) {
    ps_measurement reading;
    ps_result r=ps_sensor_read(c->user,time,(ps_quantity){1.25,PS_METRE},&reading);
    if(r!=PS_OK || reading.state!=PS_MEASUREMENT_VALID)return r==PS_OK?PS_INVALID:r;
    r=ps_channel_sample_quantity(c,0,reading.value);
    if(r==PS_OK)r=ps_channel_sample_quantity(c,2,(ps_quantity){reading.standard_uncertainty,reading.value.unit});
    if(r!=PS_OK)return r;
    if(ps_channel_sample_quantity(c,0,(ps_quantity){1,PS_SECOND})!=PS_INVALID || c->values[0]!=1.25)return PS_INVALID;
    ps_unit doubled=PS_METRE;doubled.scale=2;
    if(ps_channel_sample_quantity(c,0,(ps_quantity){DBL_MAX,doubled})!=PS_NUMERIC || c->values[0]!=1.25)return PS_INVALID;
    c->values[1]=7;return PS_OK;
}
static ps_result reset(ps_context *c) {
    ps_result r=ps_sensor_reset(c->user,4);return r==PS_OK?publish(c,0):r;
}
static ps_result create(ps_context *c) {
    ps_unit cm=PS_METRE;cm.scale=.01;cm.symbol="cm";
    if(ps_channel_add(c,"wrong_scale",cm,"cm must be explicitly converted")!=-1 ||
       ps_channel_add(c,"length",PS_METRE,"SI value converted from cm")!=0 ||
       ps_channel_add(c,"length",PS_METRE,"duplicate")!=-1 ||
       ps_channel_add(c,"after",(ps_unit){{0},1,"1"},"successful declaration after errors")!=1 ||
       ps_channel_add(c,"uncertainty",PS_METRE,"SI standard uncertainty from cm")!=2)return PS_INVALID;
    c->user=calloc(1,sizeof(ps_sensor));if(!c->user)return PS_MEMORY;
    ps_sensor_config config={0};config.unit=cm;config.rate_hz=4;config.uncertainty_absolute=1;
    ps_result r=ps_sensor_init(c->user,&config,4);if(r!=PS_OK)return r;
    snprintf(c->model_metadata,sizeof c->model_metadata,"model=canonical SI channel declaration\ninput=125 cm\nstored=1.25 m\nuncertainty=1 cm -> 0.01 m\n");return reset(c);
}
static ps_result step(ps_context *c,double dt){return publish(c,c->time_s+dt);}
static void scene(ps_context *c,ps_scene *out){(void)c;ps_scene_add_id(out,1,PS_SPHERE,ps_v3(0,0,0),ps_v3(0,0,0),.1,0x53aeefff);}
static void destroy(ps_context *c){free(c->user);c->user=NULL;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,.name="Canonical SI declarations",.create=create,.reset=reset,.step=step,.build_scene=scene,.destroy=destroy};return &api;
}
