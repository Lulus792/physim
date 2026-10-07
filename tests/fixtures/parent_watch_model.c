#include "physim/experiment.h"
#include "physim/units.h"
#include "platform.h"
#include <stdio.h>
#ifdef _WIN32
#include <process.h>
#define getpid _getpid
#else
#include <unistd.h>
#endif
static void hang(void) {
    FILE *heartbeat=fopen("heartbeat.txt","ab");
    if(!heartbeat)return;
    for(;;){fputc('x',heartbeat);fflush(heartbeat);ps_sleep(10);}
}
static ps_result create(ps_context *c) {
    double phase=2;
    ps_result r=ps_parameter_define(c,"hangPhase","0=create, 1=step, 2=complete seed zero",2,0,2,&phase);
    if(r!=PS_OK)return r;
    if(ps_channel_add(c,"phase",PS_ONE,"Lifecycle fixture")!=0)return PS_INVALID;
    FILE *identity=fopen("identity.txt","wbx");if(!identity)return PS_IO;
    fprintf(identity,"%d\n%d\n",(int)getpid(),(int)phase);fclose(identity);
    if(ps_channel_add(c,"position",PS_METRE,"Position")!=1)return PS_INVALID;
    c->values[0]=phase;c->values[1]=0;
    if(phase==0)hang();
    return PS_OK;
}
static ps_result reset(ps_context *c){(void)c;return PS_OK;}
static ps_result step(ps_context *c,double dt) {
    int phase=(int)c->values[0];
    if(phase==1 || (phase==2 && c->seed!=0))hang();
    c->values[1]=c->time_s+dt;return PS_OK;
}
static void scene(ps_context *c,ps_scene *s){(void)c;(void)s;}
static void destroy(ps_context *c){(void)c;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,
        .name="Parent lifetime fixture",.create=create,.reset=reset,.step=step,.build_scene=scene,.destroy=destroy};return &api;
}
