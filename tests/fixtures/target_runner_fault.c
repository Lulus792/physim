/* Genuine CRC-valid run files with deliberately invalid target-time semantics. */
#include "physim/data.h"
#include "platform.h"
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv) {
    if(argc<3)return 2;
    FILE *config=fopen(argv[1],"rb");if(!config)return 3;int mode=fgetc(config);fclose(config);
    double target=.7,dt=.1,minimum=.02,maximum=.2;unsigned steps=8;uint64_t seed=42;
    for(int i=3;i+1<argc;i++) {
        if(!strcmp(argv[i],"--until"))target=strtod(argv[++i],NULL);
        else if(!strcmp(argv[i],"--dt"))dt=strtod(argv[++i],NULL);
        else if(!strcmp(argv[i],"--min-dt"))minimum=strtod(argv[++i],NULL);
        else if(!strcmp(argv[i],"--max-dt"))maximum=strtod(argv[++i],NULL);
        else if(!strcmp(argv[i],"--steps"))steps=(unsigned)strtoul(argv[++i],NULL,10);
        else if(!strcmp(argv[i],"--seed"))seed=strtoull(argv[++i],NULL,10);
    }
    ps_context c={0};c.struct_size=sizeof c;c.api_version=PS_API_VERSION;c.dt_s=dt;c.seed=seed;
    ps_channel_add(&c,"position",PS_METRE,"Position");
    snprintf(c.model_metadata,sizeof c.model_metadata,
             "step_mode=%s\nend_time_s=%.17g\nmaximum_accepted_steps=%u\nminimum_dt_s=%.17g\nmaximum_dt_s=%.17g",
             mode=='4'?"fixed":"adaptive",mode=='3'?target+.1:target,steps,minimum,maximum);
    ps_run_writer w;if(ps_run_create(&w,argv[2],&c,"timing fault")!=PS_OK)return 4;
    double times[32]={0,.1,.2,.3,.4,.5,.6,0};times[7]=target;unsigned count=8;
    switch(mode) {
    case '0':times[7]=target-.01;break;
    case '1':times[3]=times[2];break;
    case '2':count=12;for(unsigned i=0;i<count;i++)times[i]=target*i/(count-1);break;
    case '5':times[1]=minimum/2;break;
    case '6':times[1]=maximum+.05;times[2]=.3;times[3]=.4;times[4]=.5;times[5]=.6;times[6]=.65;break;
    case '7':times[0]=.01;break;
    default:break;
    }
    if(mode=='h') {ps_run_append(&w,0,c.values);ps_sleep(10000);fclose(w.file);return 0;}
    for(unsigned i=0;i<count;i++){c.values[0]=times[i];if(ps_run_append(&w,times[i],c.values)!=PS_OK)return 5;}
    if(mode=='8'){fclose(w.file);return 0;}
    return ps_run_close(&w)==PS_OK?0:6;
}
