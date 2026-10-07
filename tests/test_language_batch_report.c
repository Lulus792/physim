#include "physim/report.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Batch report line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static int order(const void *a,const void *b){double x=*(const double*)a,y=*(const double*)b;return (x>y)-(x<y);}
static int demo(const char *path,const char *directory){
    ps_report *report=NULL;CHECK(ps_report_load(path,&report)==PS_OK);
    uint32_t plots,tables;CHECK(ps_report_describe(report,NULL,NULL,&plots,&tables)==PS_OK && plots==1 && tables==1);
    double values[256],mean=0;
    for(unsigned i=0;i<256;i++){
        char file[4096];snprintf(file,sizeof file,"%s/run-%04u.psrun",directory,i+1);ps_run_reader r;
        CHECK(ps_run_open(&r,file)==PS_OK && r.channels>=4 && !strcmp(r.schema[0].name,"position.x") && r.schema[0].dimension[0]==1);
        double t,v[PS_MAX_CHANNELS];unsigned count=0;ps_result status;
        while((status=ps_run_next(&r,&t,v))==PS_OK){CHECK(fabs(v[0]-(-2+v[2]*t))<2e-12);count++;}
        CHECK(status==PS_EOF && count==201 && t==1);values[i]=v[0];mean+=v[0]/256;ps_run_reader_close(&r);
    }
    double variance=0;for(unsigned i=0;i<256;i++)variance+=(values[i]-mean)*(values[i]-mean)/255;
    qsort(values,256,sizeof *values,order);
    ps_table_info info;ps_table_row row;CHECK(ps_report_table_read(report,0,&info)==PS_OK && info.columns==6 && info.rows==1);
    CHECK(ps_report_row_read(report,0,0,&row)==PS_OK && row.values[0]==256);
    CHECK(fabs(row.values[1]-mean)<5e-12 && fabs(row.values[2]-sqrt(variance))<5e-12);
    const double probabilities[]={.5,.025,.975};
    for(unsigned i=0;i<3;i++){double at=255*probabilities[i];unsigned lower=(unsigned)at;double q=values[lower]+(at-lower)*(values[lower+1]-values[lower]);CHECK(fabs(row.values[3+i]-q)<5e-12);}
    for(unsigned i=1;i<6;i++)CHECK(info.column[i].unit.dimension[0]==1 && info.column[i].unit.scale==1);
    const ps_curve_data *hist;CHECK(ps_report_curve_view(report,0,0,&hist)==PS_OK && hist->kind==PS_PLOT_HISTOGRAM && hist->count==16 && hist->source_count==256);
    double counts[16]={0};for(unsigned i=0;i<256;i++){double fraction=(values[i]-values[0])/(values[255]-values[0]);unsigned bin=fraction>=1?15:(unsigned)(16*fraction);counts[bin]++;}
    for(unsigned i=0;i<16;i++)CHECK(hist->y[i]==counts[i]);
    ps_report_destroy(report);puts("Batch documentation: all 256 physical trajectories, independent mean/variance/type-7 quantiles, SI units and histogram bins passed");return 0;
}
int main(int argc,char **argv){
    if(argc==4 && !strcmp(argv[2],"--demo"))return demo(argv[1],argv[3]);
    CHECK(argc==8); /* report, completed, started, reused, valid, cancelled, code */
    ps_report *report=NULL;CHECK(ps_report_load(argv[1],&report)==PS_OK);
    uint32_t plots,tables;CHECK(ps_report_describe(report,NULL,NULL,&plots,&tables)==PS_OK && tables==1);
    ps_table_info info;ps_table_row row;CHECK(ps_report_table_read(report,0,&info)==PS_OK && info.columns==6 && info.rows==1);
    CHECK(ps_report_row_read(report,0,0,&row)==PS_OK);
    for(unsigned i=0;i<6;i++)CHECK(row.values[i]==strtod(argv[2+i],NULL) && info.column[i].unit.scale==1);
    unsigned valid=(unsigned)row.values[3];CHECK(plots==(valid?1u:0u));
    if(valid){ps_plot_info plot;const ps_curve_data *curve;CHECK(ps_report_plot_read(report,0,&plot)==PS_OK && plot.x_unit.dimension[0]==1 && plot.x_unit.scale==1);
        CHECK(ps_report_curve_view(report,0,0,&curve)==PS_OK && curve->source_count==valid && curve->kind==PS_PLOT_HISTOGRAM);
        double sum=0;for(uint32_t i=0;i<curve->count;i++){CHECK(isfinite(curve->x[i]) && curve->y[i]>=0);sum+=curve->y[i];}CHECK(sum==valid);
    }
    ps_report_destroy(report);puts("Batch report: exact completion/reuse/status counts, SI schema and independently counted histogram passed");return 0;
}
