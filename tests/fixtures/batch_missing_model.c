#include "physim/experiment.h"
#include "physim/measurement.h"
static ps_sensor sensor;
static double mode,shift;
static ps_result measure(ps_context *c,double time) {
    ps_measurement sample;
    ps_result r=ps_sensor_read(&sensor,time,(ps_quantity){100+(double)c->seed+shift,PS_METRE},&sample);
    if(r!=PS_OK)return r;
    c->values[0]=sample.value.value;c->values[1]=mode==4?3:sample.state;return PS_OK;
}
static ps_result reset(ps_context *c) {
    ps_sensor_config config={0};config.unit=PS_METRE;config.rate_hz=100;
    if(mode==2 || (mode==0 && c->seed%3==0))config.rate_hz=5;
    if(mode==1 || (mode==0 && c->seed%3==2))config.dropout_probability=1;
    ps_result r=ps_sensor_init(&sensor,&config,c->seed);return r==PS_OK?measure(c,0):r;
}
static ps_result create(ps_context *c) {
    ps_channel_add(c,"sensor",PS_METRE,"Sensor endpoint");
    ps_channel_add(c,"sensor.status",PS_ONE,"0:not due, 1:valid, 2:dropped");
    ps_result r=ps_parameter_define(c,"mode","0:mixed, 1:dropped, 2:not due, 3:valid, 4:bad status",0,0,4,&mode);
    if(r==PS_OK)r=ps_parameter_define(c,"shift","Endpoint offset",1,-200,200,&shift);
    return r==PS_OK?reset(c):r;
}
static ps_result step(ps_context *c,double dt){return measure(c,c->time_s+dt);}
static ps_result adaptive(ps_context *c,double dt,double minimum,double maximum,ps_step_interval *out) {
    (void)minimum;(void)maximum;ps_result r=step(c,dt);out->elapsed_s=out->next_s=dt;return r;
}
static void scene(ps_context *c,ps_scene *s){(void)c;(void)s;}
static void destroy(ps_context *c){(void)c;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={sizeof api,PS_ABI_VERSION,PS_EXPERIMENT_ADAPTIVE_STEPS,"Missing endpoints",create,reset,step,scene,destroy,adaptive};return &api;
}
