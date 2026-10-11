#include "physim/experiment.h"
#include "platform.h"
#include <stdio.h>
#ifndef PS_BACKPRESSURE_STEP_DELAY_MS
#define PS_BACKPRESSURE_STEP_DELAY_MS 1
#endif
static unsigned steps;
static ps_result create(ps_context *c) {
    steps=0;c->values[0]=c->values[1]=0;
    if(ps_channel_add(c,"count",PS_ONE,"Accepted steps")<0 || ps_channel_add(c,"twice",PS_ONE,"Independent second channel")<0)return PS_INVALID;
    (void)ps_experiment_log(c,PS_LOG_INFO,"Created backpressure probe");return PS_OK;
}
static ps_result reset(ps_context *c){steps=0;c->values[0]=c->values[1]=0;return PS_OK;}
static ps_result step(ps_context *c,double dt) {
    (void)dt;steps++;c->values[0]=steps;c->values[1]=2*steps;
    if(steps==64 || steps==128)(void)ps_experiment_log(c,PS_LOG_WARNING,steps==64?"Warning 64":"Warning 128");
    if(steps%32==0){FILE *f=fopen("backpressure-progress.txt","wb");if(!f)return PS_IO;
        bool ok=fprintf(f,"%u\n",steps)>0;ok &= fclose(f)==0;if(!ok)return PS_IO;}
    ps_sleep(PS_BACKPRESSURE_STEP_DELAY_MS);return PS_OK;
}
static void scene(ps_context *c,ps_scene *s) {
    for(unsigned i=0;i<PS_MAX_OBJECTS;i++)ps_scene_add(s,PS_POINT,ps_v3(i,c->values[0],0),ps_v3(0,0,0),.1,0xffffffff);
    s->point_count=PS_MAX_SCENE_POINTS;for(unsigned i=0;i<s->point_count;i++)s->points[i]=ps_v3(i,0,0);
}
static void destroy(ps_context *c){(void)ps_experiment_log(c,PS_LOG_INFO,"Destroyed backpressure probe");}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,.name="Backpressure probe",
        .create=create,.reset=reset,.step=step,.build_scene=scene,.destroy=destroy};return &api;
}
