#include "batch.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Timed batch line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static bool absent(const char *directory,const char *name) {
    char path[4096];snprintf(path,sizeof path,"%s/%s",directory,name);FILE *f=fopen(path,"rb");if(f)fclose(f);return f==NULL;
}
static bool contains(const char *directory,const char *name,const char *value) {
    char path[4096],text[8192];snprintf(path,sizeof path,"%s/%s",directory,name);FILE *f=fopen(path,"rb");if(!f)return false;
    size_t n=fread(text,1,sizeof text-1,f);text[n]=0;fclose(f);return strstr(text,value)!=NULL;
}
static int data(const char *a,const char *b,unsigned index,double target,double velocity,unsigned *count) {
    char path[4096];ps_run_reader x,y;
    snprintf(path,sizeof path,"%s/run-%04u.psrun",a,index+1);CHECK(ps_run_open(&x,path)==PS_OK);
    snprintf(path,sizeof path,"%s/run-%04u.psrun",b,index+1);CHECK(ps_run_open(&y,path)==PS_OK);
    double tx,ty,vx[PS_MAX_CHANNELS],vy[PS_MAX_CHANNELS],initial=0,previous=0;unsigned n=0;ps_result r;
    while((r=ps_run_next(&x,&tx,vx))==PS_OK) {
        CHECK(ps_run_next(&y,&ty,vy)==PS_OK && tx==ty && x.channels==y.channels && !memcmp(vx,vy,x.channels*sizeof(double)));
        if(!n)initial=vx[0];else CHECK(tx>previous);
        CHECK(tx<=target && fabs(vx[0]-(initial+velocity*tx))<1e-12 && vx[3]==velocity);
        previous=tx;n++;
    }
    CHECK(r==PS_EOF && previous==target && n>=2 && ps_run_next(&y,&ty,vy)==PS_EOF);
    ps_run_reader_close(&x);ps_run_reader_close(&y);*count=n;return 0;
}
static bool same_summary(const char *a,const char *b) {
    char path[4096];ps_report *x=NULL,*y=NULL;snprintf(path,sizeof path,"%s/summary.psreport",a);
    bool ok=ps_report_load(path,&x)==PS_OK;snprintf(path,sizeof path,"%s/summary.psreport",b);ok &= ps_report_load(path,&y)==PS_OK;
    uint32_t plots=0,tables=0;if(ok)ok=ps_report_describe(x,NULL,NULL,&plots,&tables)==PS_OK;
    for(uint32_t p=0;ok && p<plots;p++) {
        const ps_curve_data *u,*v;ok=ps_report_curve_view(x,p,0,&u)==PS_OK && ps_report_curve_view(y,p,0,&v)==PS_OK &&
            u->count==v->count && !memcmp(u->x,v->x,u->count*sizeof(double)) && !memcmp(u->y,v->y,u->count*sizeof(double));
    }
    for(uint32_t t=0;ok && t<tables;t++) {
        ps_table_info info;ok=ps_report_table_read(x,t,&info)==PS_OK;
        for(unsigned row=0;ok && row<info.rows;row++) {
            ps_table_row u,v;ok=ps_report_row_read(x,t,row,&u)==PS_OK && ps_report_row_read(y,t,row,&v)==PS_OK &&
                !memcmp(u.values,v.values,info.columns*sizeof(double));
        }
    }
    ps_report_destroy(x);ps_report_destroy(y);return ok;
}
static bool cancel_completed(uint32_t completed,uint32_t active,void *user){(void)active;return completed<*(uint32_t*)user;}
static bool cancel(uint32_t completed,uint32_t active,void *user) {
    (void)completed;(void)active;return ps_clock()<*(double*)user;
}
static int cli(const char *const *args,const char *work,int expected) {
    ps_process p={0};CHECK(ps_process_start(&p,args,work));char out[4096];double until=ps_clock()+15;
    while(ps_process_poll(&p) && ps_clock()<until){while(ps_process_read(&p,out,sizeof out)>0){}ps_sleep(1);}
    if(p.running)ps_process_kill(&p);
    int code=p.exit_code;ps_process_close(&p);CHECK(code==expected);return 0;
}
int main(int argc,char **argv) {
    CHECK(argc==8); /* runner, C, Physim, batch CLI, fake runner, work, legacy module */
    ps_batch_options o={0};snprintf(o.runner,sizeof o.runner,"%s",argv[1]);snprintf(o.module,sizeof o.module,"%s",argv[2]);
    strcpy(o.channel,"position");o.seed=42;o.runs=6;o.steps=100;o.dt=.1;o.timeout_s=15;
    o.adaptive=true;o.end_time=.7;o.minimum_dt=.02;o.maximum_dt=.3;o.workers=1;
    char first[4096],parallel[4096],language[4096];
    snprintf(first,sizeof first,"%s/serial",argv[6]);snprintf(parallel,sizeof parallel,"%s/parallel",argv[6]);snprintf(language,sizeof language,"%s/language",argv[6]);
    strcpy(o.directory,first);ps_batch_result a,b;
    CHECK(ps_batch_run(&o,NULL,NULL,&a)==PS_OK && a.completed==6 && a.values[0]!=a.values[1]);
    CHECK(contains(first,"series.txt","physim_batch=5\n") && contains(first,"series.txt","step_mode=adaptive\n"));
    strcpy(o.directory,parallel);o.workers=4;CHECK(ps_batch_run(&o,NULL,NULL,&b)==PS_OK && b.peak_active==4 && !memcmp(a.values,b.values,6*sizeof(double)));
    CHECK(same_summary(first,parallel));
    /* Continuation retains nonuniform accepted time grids for C and Physim. */
    for(unsigned lang=0;lang<2;lang++) {
        ps_batch_options original=o,resumed;ps_batch_result partial,continued,all;
        snprintf(original.module,sizeof original.module,"%s",argv[2+lang]);
        snprintf(original.directory,sizeof original.directory,"%s/resume-%u-original",argv[6],lang);
        uint32_t cancel_after=2;
        CHECK(ps_batch_run(&original,cancel_completed,&cancel_after,&partial)==PS_OK && partial.cancelled && partial.completed==2);
        char output[4096];snprintf(output,sizeof output,"%s/resume-%u-next",argv[6],lang);
        CHECK(ps_batch_resume_load(original.directory,original.runner,output,&resumed)==PS_OK);
        CHECK(ps_batch_run(&resumed,NULL,NULL,&continued)==PS_OK && continued.completed==6 && continued.reused==2 && continued.started==4);
        snprintf(original.directory,sizeof original.directory,"%s/resume-%u-reference",argv[6],lang);
        CHECK(ps_batch_run(&original,NULL,NULL,&all)==PS_OK && !memcmp(all.values,continued.values,6*sizeof(double)));
        CHECK(same_summary(output,original.directory));
    }

    strcpy(o.directory,language);strcpy(o.module,argv[3]);CHECK(ps_batch_run(&o,NULL,NULL,&b)==PS_OK && !memcmp(a.values,b.values,6*sizeof(double)));
    CHECK(same_summary(first,language));
    unsigned count;
    for(unsigned i=0;i<6;i++) {CHECK(!data(first,parallel,i,.7,1,&count));CHECK(!data(first,language,i,.7,1,&count));CHECK(count<101);}
    o.sweep=true;strcpy(o.sweep_name,"velocity");o.sweep_start=.2;o.sweep_end=3;o.runs=3;
    strcpy(o.module,argv[2]);snprintf(first,sizeof first,"%s/sweep-c",argv[6]);strcpy(o.directory,first);CHECK(ps_batch_run(&o,NULL,NULL,&a)==PS_OK && a.completed==3);
    strcpy(o.module,argv[3]);snprintf(language,sizeof language,"%s/sweep-phys",argv[6]);strcpy(o.directory,language);CHECK(ps_batch_run(&o,NULL,NULL,&b)==PS_OK && !memcmp(a.values,b.values,3*sizeof(double)));
    CHECK(same_summary(first,language));
    for(unsigned i=0;i<3;i++)CHECK(!data(first,language,i,.7,i==0?.2:i==1?1.6:3,&count));
    CHECK(a.sweep_unit.declared && a.sweep_unit.scale==.01 && !strcmp(a.sweep_unit.symbol,"cm/s") &&
          a.sweep_unit.dimension[0]==1 && a.sweep_unit.dimension[2]==-1);
    CHECK(contains(first,"series.txt","parameter_value_storage=SI\n") &&
          contains(first,"series.txt","parameter_unit.velocity=cm/s\n"));
    char study_path[4096];snprintf(study_path,sizeof study_path,"%s/summary.psreport",first);
    ps_report *study=NULL;CHECK(ps_report_load(study_path,&study)==PS_OK);
    ps_plot_info plot;const ps_curve_data *curve;
    CHECK(ps_report_plot_read(study,0,&plot)==PS_OK && plot.x_unit.scale==.01 && !strcmp(plot.x_unit.symbol,"cm/s"));
    CHECK(ps_report_curve_view(study,0,0,&curve)==PS_OK && curve->x[0]==20 && curve->x[1]==160 && curve->x[2]==300);
    ps_report_destroy(study);
    o.sweep=false;o.runs=2;o.workers=2;o.end_time=.25;o.minimum_dt=.08;o.maximum_dt=.1;
    snprintf(o.directory,sizeof o.directory,"%s/clipped-series",argv[6]);CHECK(ps_batch_run(&o,NULL,NULL,&a)==PS_OK);
    char path[4096];snprintf(path,sizeof path,"%s/run-0001.psrun",o.directory);ps_run_reader r;CHECK(ps_run_open(&r,path)==PS_OK);
    double time,values[PS_MAX_CHANNELS],last=0;ps_result status;
    while((status=ps_run_next(&r,&time,values))==PS_OK)last=time;
    CHECK(status==PS_EOF && last==.25 && values[2]<.08);ps_run_reader_close(&r);
    o.steps=1;o.end_time=.7;o.minimum_dt=.02;o.maximum_dt=.3;snprintf(o.directory,sizeof o.directory,"%s/short-budget",argv[6]);
    CHECK(ps_batch_run(&o,NULL,NULL,&a)==PS_IO && a.completed==0 && absent(o.directory,"summary.psreport") && contains(o.directory,"status.txt","status=failed\n"));
    o.steps=100;strcpy(o.module,argv[7]);snprintf(o.directory,sizeof o.directory,"%s/unsupported",argv[6]);
    CHECK(ps_batch_run(&o,NULL,NULL,&a)==PS_IO && a.completed==0 && absent(o.directory,"summary.psreport"));
    strcpy(o.runner,argv[5]);o.runs=2;o.workers=2;o.steps=8;o.minimum_dt=.02;o.maximum_dt=.2;
    for(unsigned fault=0;fault<9;fault++) {
        snprintf(o.module,sizeof o.module,"%s/fault-%u.data",argv[6],fault);FILE *f=fopen(o.module,"wb");CHECK(f && fputc('0'+(int)fault,f)!=EOF && !fclose(f));
        snprintf(o.directory,sizeof o.directory,"%s/fault-%u",argv[6],fault);
        CHECK(ps_batch_run(&o,NULL,NULL,&a)==PS_CORRUPT && !a.completed && !a.active && absent(o.directory,"summary.psreport"));
    }
    o.sweep=true;strcpy(o.sweep_name,"velocity");o.sweep_start=.2;o.sweep_end=3;
    const char unit_faults[]={'u','d'};
    for(unsigned i=0;i<2;i++) {
        snprintf(o.module,sizeof o.module,"%s/unit-fault-%c.data",argv[6],unit_faults[i]);
        FILE *config=fopen(o.module,"wb");CHECK(config && fputc(unit_faults[i],config)!=EOF && !fclose(config));
        snprintf(o.directory,sizeof o.directory,"%s/unit-fault-%c",argv[6],unit_faults[i]);
        CHECK(ps_batch_run(&o,NULL,NULL,&a)==PS_CORRUPT && a.completed<=1 && !a.active && absent(o.directory,"summary.psreport"));
    }
    o.sweep=false;
    snprintf(o.module,sizeof o.module,"%s/hang.data",argv[6]);FILE *f=fopen(o.module,"wb");CHECK(f && fputc('h',f)!=EOF && !fclose(f));
    snprintf(o.directory,sizeof o.directory,"%s/cancelled",argv[6]);double until=ps_clock()+.3;
    CHECK(ps_batch_run(&o,cancel,&until,&a)==PS_OK && a.cancelled && a.started>0 && a.active==0 && absent(o.directory,"summary.psreport"));
    CHECK(ps_clock()<until+2);
    snprintf(path,sizeof path,"%s/cli",argv[6]);
    const char *command[]={argv[4],argv[1],argv[2],path,"position","3","100",".1","42","--until",".7","--adaptive","--min-dt",".02","--max-dt",".3","--workers","2",NULL};
    CHECK(!cli(command,argv[6],0) && contains(path,"status.txt","status=complete\n"));
    snprintf(path,sizeof path,"%s/cli-subnormal",argv[6]);
    const char *tiny[]={argv[4],argv[1],argv[2],path,"position","3","100",".1","42","--until",".7",
                       "--adaptive","--min-dt",".02","--max-dt",".3","--param","offset=1e-310",NULL};
    CHECK(!cli(tiny,argv[6],0));
    char tiny_path[4096];snprintf(tiny_path,sizeof tiny_path,"%s/run-0001.psrun",path);
    CHECK(ps_run_open(&r,tiny_path)==PS_OK && strstr(r.metadata,"parameter.offset=9.9999999999999694e-311\n"));
    CHECK(ps_run_next(&r,&time,values)==PS_OK && values[4]==1e-310);ps_run_reader_close(&r);
    const char *bad[]={argv[4],argv[1],argv[2],path,"position","3","100",".1","42","--adaptive",NULL};CHECK(!cli(bad,argv[6],2));
    const char *bad_target[]={argv[4],argv[1],argv[2],path,"position","3","100",".1","42","--until","0",NULL};CHECK(!cli(bad_target,argv[6],2));
    o.end_time=NAN;CHECK(ps_batch_validate(&o)==PS_INVALID);o.end_time=.7;o.minimum_dt=.2;CHECK(ps_batch_validate(&o)==PS_INVALID);
    o.minimum_dt=.02;o.maximum_dt=.01;CHECK(ps_batch_validate(&o)==PS_INVALID);o.maximum_dt=.2;o.end_time=0;CHECK(ps_batch_validate(&o)==PS_INVALID);
    puts("Common target time, seeded C/Physim series, parallel order, sweep, clipped tail, budget, cancellation and CRC-valid timing faults passed.");return 0;
}
