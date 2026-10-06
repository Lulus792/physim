#include "batch.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Missing endpoint line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static bool cancel(uint32_t count,uint32_t active,void *user){(void)active;return count<*(uint32_t*)user;}
static bool text(const char *directory,const char *file,char out[8192]) {
    char path[4608];snprintf(path,sizeof path,"%s/%s",directory,file);FILE *f=fopen(path,"rb");if(!f)return false;
    size_t n=fread(out,1,8191,f);out[n]=0;bool ok=!ferror(f);return !fclose(f) && ok;
}
static int report(const ps_batch_options *o,unsigned valid,unsigned plots,unsigned tables,double mean) {
    char path[4608];snprintf(path,sizeof path,"%s/summary.psreport",o->directory);ps_report *r=NULL;
    CHECK(ps_report_load(path,&r)==PS_OK);uint32_t p,t;char provenance[8192];
    CHECK(ps_report_describe(r,NULL,provenance,&p,&t)==PS_OK && p==plots && t==tables);
    CHECK(strstr(provenance,"selektive Ausfälle"));
    if(valid && !o->sweep) {
        ps_table_row row;CHECK(ps_report_row_read(r,0,0,&row)==PS_OK && fabs(row.values[0]-mean)<=1e-12*fmax(1,fabs(mean)));
        const ps_curve_data *curve;CHECK(ps_report_curve_view(r,0,0,&curve)==PS_OK && curve->source_count==valid);
        double count=0;for(unsigned i=0;i<curve->count;i++)count+=curve->y[i];CHECK(count==valid);
    }
    if(valid<o->runs) {
        ps_table_info table;CHECK(ps_report_table_read(r,t-1,&table)==PS_OK && !strcmp(table.title,"Messabdeckung · Endwerte"));
        ps_table_row row;CHECK(ps_report_row_read(r,t-1,1,&row)==PS_OK && row.values[0]==valid && fabs(row.values[1]-100.0*valid/o->runs)<1e-12);
        snprintf(path,sizeof path,"%s/coverage.csv",o->directory);CHECK(ps_report_export_table_csv(r,t-1,path)==PS_OK);
    }
    if(o->sweep && valid) {
        const ps_curve_data *curve;CHECK(ps_report_curve_view(r,0,0,&curve)==PS_OK && curve->count==valid &&
            curve->source_count==o->runs && curve->kind==(valid<o->runs?PS_PLOT_SCATTER:PS_PLOT_LINE));
        unsigned point=0;
        for(unsigned i=0;i<o->runs;i++)if(i%3==1) {
            double shift=o->sweep_start+(o->sweep_end-o->sweep_start)*i/(o->runs-1);
            CHECK(fabs(curve->x[point]-shift)<1e-12 && fabs(curve->y[point]-(100+i+shift))<1e-12);point++;
        }
    }
    ps_report_destroy(r);return 0;
}
int main(int argc,char **argv) {
    CHECK(argc==5); /* runner, C sensor, Physim sensor, work */
    ps_batch_options o={0};snprintf(o.runner,sizeof o.runner,"%s",argv[1]);strcpy(o.channel,"sensor");
    o.runs=6;o.steps=3;o.workers=4;o.dt=.01;o.timeout_s=10;
    ps_batch_result a,b;char csv[8192],original[4096];
    for(unsigned lang=0;lang<2;lang++) {
        snprintf(o.module,sizeof o.module,"%s",argv[2+lang]);
        snprintf(o.directory,sizeof o.directory,"%s/mixed-%u",argv[4],lang);
        CHECK(ps_batch_run(&o,NULL,NULL,&a)==PS_OK && a.completed==6 && a.valid==2);
        for(unsigned i=0;i<6;i++)CHECK(a.finished[i] && a.endpoint_status[i]==i%3 && (i%3!=1 || a.values[i]==101+i));
        CHECK(!report(&o,2,1,2,103.5));CHECK(text(o.directory,"endpoints.csv",csv));
        CHECK(strstr(csv,"index,seed,file,time_s,value,status\n") && strstr(csv,"run-0001.psrun,0.029999999999999999,,0\n") &&
            strstr(csv,"run-0003.psrun,0.029999999999999999,,2\n"));
        strcpy(original,o.directory);o.workers=1;snprintf(o.directory,sizeof o.directory,"%s/serial-%u",argv[4],lang);
        CHECK(ps_batch_run(&o,NULL,NULL,&b)==PS_OK && b.valid==a.valid && !memcmp(a.values,b.values,sizeof a.values) &&
            !memcmp(a.endpoint_status,b.endpoint_status,sizeof a.endpoint_status));
        char serial[8192];CHECK(text(o.directory,"endpoints.csv",serial) && !strcmp(csv,serial));o.workers=4;
        /* Invalid endpoints are successful journaled runs and are reusable. */
        snprintf(o.directory,sizeof o.directory,"%s/cancelled-%u",argv[4],lang);uint32_t after=2;
        CHECK(ps_batch_run(&o,cancel,&after,&a)==PS_OK && a.cancelled && a.completed==2);strcpy(original,o.directory);
        char next[4096];snprintf(next,sizeof next,"%s/resumed-%u",argv[4],lang);ps_batch_options saved;
        CHECK(ps_batch_resume_load(original,o.runner,next,&saved)==PS_OK);
        CHECK(ps_batch_run(&saved,NULL,NULL,&b)==PS_OK && b.completed==6 && b.valid==2 && b.reused==2 && b.started==4);
        CHECK(!report(&saved,2,1,2,103.5));
        snprintf(o.directory,sizeof o.directory,"%s/adaptive-%u",argv[4],lang);
        o.adaptive=true;o.end_time=.03;o.minimum_dt=.005;o.maximum_dt=.01;
        CHECK(ps_batch_run(&o,NULL,NULL,&a)==PS_OK && a.valid==2);CHECK(!report(&o,2,1,2,103.5));
        snprintf(o.directory,sizeof o.directory,"%s/clipped-%u",argv[4],lang);o.end_time=.035;o.steps=4;
        CHECK(ps_batch_run(&o,NULL,NULL,&a)==PS_OK && a.completed==6 && !a.valid);CHECK(!report(&o,0,0,1,0));
        o.adaptive=false;o.end_time=0;o.steps=3;
        o.sweep=true;strcpy(o.sweep_name,"shift");o.sweep_start=0;o.sweep_end=5;
        snprintf(o.directory,sizeof o.directory,"%s/sweep-%u",argv[4],lang);
        CHECK(ps_batch_run(&o,NULL,NULL,&a)==PS_OK && a.valid==2);CHECK(!report(&o,2,1,1,0));o.sweep=false;
        o.parameter_count=1;strcpy(o.parameters[0].name,"mode");
        for(unsigned mode=1;mode<=2;mode++) {
            o.parameters[0].value=mode;snprintf(o.directory,sizeof o.directory,"%s/empty-%u-%u",argv[4],lang,mode);
            CHECK(ps_batch_run(&o,NULL,NULL,&a)==PS_OK && a.completed==6 && !a.valid);CHECK(!report(&o,0,0,1,0));
            for(unsigned i=0;i<6;i++)CHECK(a.endpoint_status[i]==(mode==1?2:0));
        }
        o.parameters[0].value=4;snprintf(o.directory,sizeof o.directory,"%s/bad-status-%u",argv[4],lang);
        CHECK(ps_batch_run(&o,NULL,NULL,&a)==PS_CORRUPT && a.completed==0);
        o.parameter_count=2;o.parameters[0].value=3;strcpy(o.parameters[1].name,"shift");o.parameters[1].value=-100;
        o.runs=1;snprintf(o.directory,sizeof o.directory,"%s/valid-zero-%u",argv[4],lang);
        CHECK(ps_batch_run(&o,NULL,NULL,&a)==PS_OK && a.valid==1 && a.endpoint_status[0]==1 && a.values[0]==0);
        CHECK(!report(&o,1,1,1,0));o.runs=6;o.parameter_count=0;
    }
    /* The CI threshold uses valid measurements, never the requested run count. */
    o.runs=210;snprintf(o.directory,sizeof o.directory,"%s/ci-threshold",argv[4]);
    CHECK(ps_batch_run(&o,NULL,NULL,&a)==PS_OK && a.completed==210 && a.valid==70);
    CHECK(!report(&o,70,1,2,205.5));
    puts("Missing endpoints: sensor states, valid-only statistics, empty reports, gaps, continuation, adaptive clipping and C/Physim parity passed");return 0;
}
