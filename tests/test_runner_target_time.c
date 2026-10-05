#include "physim/data.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Target runner line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static int launch(const char *const *args,const char *work,int expected) {
    ps_process p={0};CHECK(ps_process_start(&p,args,work));double until=ps_clock()+15;char out[4096];
    while(ps_process_poll(&p) && ps_clock()<until){while(ps_process_read(&p,out,sizeof out)>0){}ps_sleep(1);}
    if(p.running)ps_process_kill(&p);
    int code=p.exit_code;ps_process_close(&p);CHECK(code==expected);return 0;
}
static int valid(const char *path,double target,unsigned expected,bool clipped) {
    ps_run_reader r;CHECK(ps_run_open(&r,path)==PS_OK);
    CHECK(strstr(r.metadata,"\nend_time_s=") && strstr(r.metadata,"\nmaximum_accepted_steps=20\n"));
    ps_parameter_unit unit;
    CHECK(ps_parameter_unit_parse(r.metadata,"velocity",&unit)==PS_OK && unit.declared &&
          unit.scale==.01 && unit.dimension[0]==1 && unit.dimension[2]==-1 && !strcmp(unit.symbol,"cm/s"));
    double t,v[PS_MAX_CHANNELS],initial=0,previous=0,last_dt=0;unsigned n=0;ps_result status;
    while((status=ps_run_next(&r,&t,v))==PS_OK) {
        if(!n){CHECK(t==0);initial=v[0];}
        else {CHECK(t>previous && t<=target);last_dt=t-previous;}
        CHECK(fabs(v[0]-(initial+v[3]*t))<1e-12 && fabs(v[1]-t)<1e-14);
        previous=t;n++;
    }
    CHECK(status==PS_EOF && previous==target && n==expected && (!clipped || last_dt<.08));
    ps_run_reader_close(&r);return 0;
}
static int same(const char *a,const char *b) {
    ps_run_reader x,y;CHECK(ps_run_open(&x,a)==PS_OK && ps_run_open(&y,b)==PS_OK);
    double tx,ty,vx[PS_MAX_CHANNELS],vy[PS_MAX_CHANNELS];ps_result r;
    while((r=ps_run_next(&x,&tx,vx))==PS_OK) {
        CHECK(ps_run_next(&y,&ty,vy)==PS_OK && tx==ty && !memcmp(vx,vy,x.channels*sizeof(double)));
    }
    CHECK(r==PS_EOF && ps_run_next(&y,&ty,vy)==PS_EOF);
    ps_run_reader_close(&x);ps_run_reader_close(&y);return 0;
}
int main(int argc,char **argv) {
    CHECK(argc==5);char paths[2][4096];
    for(unsigned adaptive=0;adaptive<2;adaptive++) {
        for(unsigned language=0;language<2;language++) {
            snprintf(paths[language],sizeof paths[language],"%s/target-%u-%u.psrun",argv[4],adaptive,language);
            const char *args[24]={argv[1],argv[2+language],paths[language],"--dt",".1","--steps","20","--until",".25","--record-scenes",NULL};
            unsigned n=10;if(adaptive){args[n++]="--adaptive";args[n++]="--min-dt";args[n++]=".08";args[n++]="--max-dt";args[n++]=".1";}args[n]=NULL;
            CHECK(!launch(args,argv[4],0));CHECK(!valid(paths[language],.25,4,true));
        }
        CHECK(!same(paths[0],paths[1]));
    }
    const char *targets[]={".03",".7"};const unsigned counts[]={2,8};
    for(unsigned i=0;i<2;i++) {
        char path[4096];snprintf(path,sizeof path,"%s/fixed-end-%u.psrun",argv[4],i);
        const char *args[]={argv[1],argv[2],path,"--dt",".1","--steps","20","--until",targets[i],NULL};
        CHECK(!launch(args,argv[4],0));CHECK(!valid(path,i?.7:.03,counts[i],i==0));
    }
    for(unsigned adaptive=0;adaptive<2;adaptive++) {
        char path[4096];snprintf(path,sizeof path,"%s/short-budget-%u.psrun",argv[4],adaptive);
        const char *args[18]={argv[1],argv[2],path,"--dt",".1","--steps","1","--until",".7",NULL};
        unsigned n=9;if(adaptive){args[n++]="--adaptive";args[n++]="--max-dt";args[n++]=".1";}args[n]=NULL;
        CHECK(!launch(args,argv[4],7));ps_run_reader r;CHECK(ps_run_open(&r,path)==PS_OK);
        double t,v[PS_MAX_CHANNELS];CHECK(ps_run_next(&r,&t,v)==PS_OK && t==0);
        CHECK(ps_run_next(&r,&t,v)==PS_OK && t==.1 && ps_run_next(&r,&t,v)==PS_RECOVERED);ps_run_reader_close(&r);
    }
    const char *bad[]={"0","-1","nan","inf","1e-999","1000000001",".1suffix"};
    for(unsigned i=0;i<sizeof bad/sizeof *bad;i++) {
        const char *args[]={argv[1],argv[2],"unused.psrun","--until",bad[i],NULL};CHECK(!launch(args,argv[4],2));
    }
    const char *interactive[]={argv[1],argv[2],"unused.psrun","--interactive","--until",".7",NULL};CHECK(!launch(interactive,argv[4],2));
    for(unsigned language=0;language<2;language++) {
        char path[4096];snprintf(path,sizeof path,"%s/subnormal-%u.psrun",argv[4],language);
        const char *args[]={argv[1],argv[2+language],path,"--steps","1","--param","offset=1e-310",NULL};
        CHECK(!launch(args,argv[4],0));ps_run_reader r;CHECK(ps_run_open(&r,path)==PS_OK);
        double t,v[PS_MAX_CHANNELS];CHECK(r.channels==5);
        CHECK(ps_run_next(&r,&t,v)==PS_OK && v[4]==1e-310);
        CHECK(ps_run_next(&r,&t,v)==PS_OK && v[4]==1e-310 && ps_run_next(&r,&t,v)==PS_EOF);ps_run_reader_close(&r);
    }
    const char *underflow[]={argv[1],argv[2],"unused.psrun","--param","offset=1e-999",NULL};
    CHECK(!launch(underflow,argv[4],2));
    puts("Exact target times, clipped terminal intervals, C/Physim parity, bounded steps and preserved incomplete prefixes passed.");return 0;
}
