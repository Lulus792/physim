#include "physim/data.h"
#include "platform.h"
#include "protocol.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Adaptive runner line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static int finish(ps_process *child,double deadline) {
    char bytes[4096];
    while(ps_process_poll(child) && ps_clock()<deadline) {
        while(ps_process_read(child,bytes,sizeof bytes)>0){}ps_sleep(1);
    }
    if(child->running)ps_process_kill(child);
    int code=child->exit_code;ps_process_close(child);return code;
}
static int launch(const char *const *args,const char *work,int expected) {
    ps_process p={0};CHECK(ps_process_start(&p,args,work));
    CHECK(finish(&p,ps_clock()+15)==expected);return 0;
}
static bool send(ps_process *child,uint32_t *sequence,uint32_t type,double speed) {
    unsigned char frame[32],payload[8];uint32_t n=0;
    if(type==PS_MSG_HELLO){n=4;ps_put_u32(payload,PS_ABI_VERSION);}
    if(type==PS_MSG_SPEED){n=8;ps_put_f64(payload,speed);}
    size_t size=ps_wire_encode(frame,type,(*sequence)++,payload,n);
    return size && ps_process_write(child,frame,size);
}
static bool frame(ps_process *child,ps_wire_buffer *wire,uint32_t *type,double *time,
                  bool *paused,double deadline) {
    while(ps_process_poll(child) && ps_clock()<deadline) {
        const unsigned char *p;uint32_t n;int status=ps_wire_peek(wire,type,&p,&n);
        if(status<0)return false;
        if(status>0) {
            bool valid=true;
            if(*type==PS_MSG_SNAPSHOT) {
                double values[PS_MAX_CHANNELS];uint32_t count;ps_scene scene;
                valid=ps_snapshot_decode(p,n,time,values,&count,&scene,paused);
            }
            if(*type==PS_MSG_ERROR)fprintf(stderr,"Runner error: %.*s\n",(int)n,p);
            ps_wire_consume(wire,n);
            if(*type!=PS_MSG_HEARTBEAT)return valid;
            continue;
        }
        int got=ps_process_read(child,wire->data+wire->used,sizeof wire->data-wire->used);
        if(got<0)return false;wire->used+=(size_t)got;ps_sleep(1);
    }
    return false;
}
static int compare(const char *actual,const char *reference,bool whole) {
    ps_run_reader a,b;CHECK(ps_run_open(&a,actual)==PS_OK && ps_run_open(&b,reference)==PS_OK);
    CHECK(a.channels==5 && b.channels==5 && strstr(a.metadata,"step_mode=adaptive\n"));
    double ta,tb,va[PS_MAX_CHANNELS],vb[PS_MAX_CHANNELS],previous=0,first=0;
    unsigned samples=0;bool varied=false,rejected=false;ps_result status;
    while((status=ps_run_next(&a,&ta,va))==PS_OK) {
        CHECK(ps_run_next(&b,&tb,vb)==PS_OK && ta==tb);
        for(unsigned i=0;i<5;i++)CHECK(va[i]==vb[i]);
        CHECK(fabs(va[0]-cos(ta))<2e-8 && fabs(va[1]+sin(ta))<2e-8 && va[4]==ta);
        if(samples) {
            CHECK(ta>previous && ta-previous==va[2] && va[2]>=1e-8 && va[2]<=.25);
            if(samples==1)first=va[2];else varied |= fabs(va[2]-first)>1e-6;
            rejected |= va[3]>0;
        }
        previous=ta;samples++;
    }
    CHECK(status==PS_EOF && samples>10 && varied && rejected);
    if(whole)CHECK(samples==201 && ps_run_next(&b,&tb,vb)==PS_EOF);
    ps_run_reader_close(&a);ps_run_reader_close(&b);return 0;
}
static int interactive(const char *runner,const char *module,const char *path,const char *work,
                       const char *speed,bool slow,const char *reference) {
    const char *args[]={runner,module,path,"--interactive","--adaptive","--dt",".1",
                        "--min-dt","1e-8","--max-dt",".25","--speed",speed,NULL};
    ps_process child={0};CHECK(ps_process_start(&child,args,work));
    double deadline=ps_clock()+12,time=0;bool paused;uint32_t type,sequence=0;ps_wire_buffer wire={0};
    CHECK(frame(&child,&wire,&type,&time,&paused,deadline) && type==PS_MSG_HELLO);
    CHECK(send(&child,&sequence,PS_MSG_HELLO,0));
    CHECK(frame(&child,&wire,&type,&time,&paused,deadline) && type==PS_MSG_SNAPSHOT && paused && time==0);
    CHECK(send(&child,&sequence,PS_MSG_STEP,0));
    CHECK(frame(&child,&wire,&type,&time,&paused,deadline) && type==PS_MSG_SNAPSHOT && paused && time>0 && time<.1);
    CHECK(send(&child,&sequence,PS_MSG_RUN,0));
    CHECK(frame(&child,&wire,&type,&time,&paused,deadline) && type==PS_MSG_SNAPSHOT && !paused);
    if(slow)CHECK(send(&child,&sequence,PS_MSG_SPEED,4));
    while(time<.8) {
        if(slow)ps_sleep(100);
        CHECK(frame(&child,&wire,&type,&time,&paused,deadline) && type==PS_MSG_SNAPSHOT && !paused);
    }
    CHECK(send(&child,&sequence,PS_MSG_PAUSE,0));
    do {CHECK(frame(&child,&wire,&type,&time,&paused,deadline) && type==PS_MSG_SNAPSHOT);}while(!paused);
    double held=time;ps_sleep(100);
    CHECK(send(&child,&sequence,PS_MSG_SPEED,0) && send(&child,&sequence,PS_MSG_STEP,0));
    CHECK(frame(&child,&wire,&type,&time,&paused,deadline) && type==PS_MSG_SNAPSHOT && paused && time>held && time<=held+.25);
    CHECK(send(&child,&sequence,PS_MSG_STOP,0));CHECK(finish(&child,deadline)==0);
    /* Offline speed can pass many samples before the next wall-clock frame.
     * Size the independent CLI reference to the actual saved prefix. */
    ps_run_reader run;CHECK(ps_run_open(&run,path)==PS_OK);
    double t,v[PS_MAX_CHANNELS];unsigned count=0;ps_result status;
    while((status=ps_run_next(&run,&t,v))==PS_OK)count++;
    CHECK(status==PS_EOF && count>10);ps_run_reader_close(&run);
    char extended[4096],steps[32];
    if(count>201) {
        snprintf(extended,sizeof extended,"%s.reference.psrun",path);
        snprintf(steps,sizeof steps,"%u",count-1);
        const char *baseline[]={runner,module,extended,"--steps",steps,"--adaptive","--dt",".1",
                                "--min-dt","1e-8","--max-dt",".25",NULL};
        CHECK(!launch(baseline,work,0));reference=extended;
    }
    CHECK(!compare(path,reference,false));return 0;
}
static int errors(const char *runner,const char *module,const char *legacy,const char *work) {
    char path[4096];
    const char *bad[]={"nan","0","-1","1e-999","1.01",".2suffix"};
    for(unsigned i=0;i<sizeof bad/sizeof *bad;i++) {
        snprintf(path,sizeof path,"%s/bad-option-%u.psrun",work,i);
        const char *args[]={runner,module,path,"--adaptive","--min-dt",bad[i],NULL};
        CHECK(!launch(args,work,2));
    }
    const char *wrong[]={runner,legacy,"unused.psrun","--adaptive",NULL};
    CHECK(!launch(wrong,work,4));
    const char *bounds[]={runner,module,"unused.psrun","--min-dt","1e-8",NULL};
    CHECK(!launch(bounds,work,2));
    snprintf(path,sizeof path,"%s/metadata-too-long.psrun",work);
    const char *metadata[]={runner,module,path,"--adaptive","--param","fault=8",NULL};
    CHECK(!launch(metadata,work,5));
    ps_run_reader absent;CHECK(ps_run_open(&absent,path)==PS_IO);
    for(unsigned fault=1;fault<=7;fault++) {
        char parameter[32];snprintf(parameter,sizeof parameter,"fault=%u",fault);
        snprintf(path,sizeof path,"%s/bad-interval-%u.psrun",work,fault);
        const char *args[]={runner,module,path,"--steps","3","--adaptive","--dt",".1",
                            "--max-dt",".25","--param",parameter,NULL};
        CHECK(!launch(args,work,7));
        ps_run_reader reader;CHECK(ps_run_open(&reader,path)==PS_OK);
        double t,v[PS_MAX_CHANNELS];CHECK(ps_run_next(&reader,&t,v)==PS_OK && t==0 && v[0]==1);
        CHECK(ps_run_next(&reader,&t,v)==PS_RECOVERED);ps_run_reader_close(&reader);
    }
    return 0;
}
int main(int argc,char **argv) {
    CHECK(argc==6); /* runner, C, Physim, short legacy API, work */
    CHECK(!errors(argv[1],argv[2],argv[4],argv[5]));
    char references[2][4096],path[4096];
    for(unsigned language=0;language<2;language++) {
        snprintf(references[language],sizeof references[language],"%s/reference-%u.psrun",argv[5],language);
        const char *args[]={argv[1],argv[2+language],references[language],"--steps","200","--adaptive",
                            "--dt",".1","--min-dt","1e-8","--max-dt",".25","--record-scenes",NULL};
        CHECK(!launch(args,argv[5],0));
    }
    CHECK(!compare(references[0],references[1],true));
    for(unsigned language=0;language<2;language++) {
        const char *speeds[]={".5","4","0",".5"};
        for(unsigned mode=0;mode<4;mode++) {
            snprintf(path,sizeof path,"%s/interactive-%u-%u.psrun",argv[5],language,mode);
            CHECK(!interactive(argv[1],argv[2+language],path,argv[5],speeds[mode],mode==3,references[language]));
        }
    }
    puts("Adaptive C/Physim data match, variable intervals meet analytic reference; pacing, pause, single-step, slow reader and invalid reports passed.");
    return 0;
}
