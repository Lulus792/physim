#include "physim/report.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Monte Carlo report %d: %s\n",__LINE__,#x);return 1;}}while(0)
static int compare(const void *a,const void *b){double x=*(const double *)a,y=*(const double *)b;return (x>y)-(x<y);}
static double quantile(const double *values,double p){double at=255*p;unsigned i=(unsigned)at;return values[i]+(at-i)*(values[i+1]-values[i]);}
int main(int argc,char **argv) {
    CHECK(argc==3);ps_report *report=NULL;CHECK(ps_report_load(argv[1],&report)==PS_OK);
    uint32_t plots,tables;CHECK(ps_report_describe(report,NULL,NULL,&plots,&tables)==PS_OK && plots==4 && tables==2);
    double values[2][256],expected[4]={0};
    for(unsigned i=0;i<256;i++) {
        char path[4096];snprintf(path,sizeof path,"%s/run-%04u.psrun",argv[2],i+1);
        ps_run_reader r;CHECK(ps_run_open(&r,path)==PS_OK);double t,v[PS_MAX_CHANNELS];ps_result result;
        while((result=ps_run_next(&r,&t,v))==PS_OK){}
        CHECK(result==PS_EOF && t==1);values[0][i]=v[0];values[1][i]=v[1];if(!i)for(unsigned j=0;j<4;j++)expected[j]=v[4+j];
        ps_run_reader_close(&r);
    }
    for(unsigned axis=0;axis<2;axis++) {
        ps_plot_info info;const ps_curve_data *trace,*histogram;
        CHECK(ps_report_plot_read(report,axis*2,&info)==PS_OK && info.x_unit.dimension[0]==0 && info.y_unit.dimension[0]==1);
        CHECK(ps_report_curve_view(report,axis*2,0,&trace)==PS_OK && trace->count==256 && trace->source_count==256);
        double mean=0;for(unsigned i=0;i<256;i++){CHECK(trace->x[i]==i+1 && fabs(trace->y[i]-values[axis][i])<1e-12);mean+=values[axis][i]/256;}
        double variance=0;for(unsigned i=0;i<256;i++)variance+=(values[axis][i]-mean)*(values[axis][i]-mean)/255;
        qsort(values[axis],256,sizeof(double),compare);ps_table_row row;
        CHECK(ps_report_row_read(report,0,axis,&row)==PS_OK && fabs(row.values[0]-mean)<1e-12 && fabs(row.values[1]-sqrt(variance))<1e-12);
        CHECK(fabs(row.values[2]-quantile(values[axis],.025))<1e-12 && fabs(row.values[3]-quantile(values[axis],.5))<1e-12 && fabs(row.values[4]-quantile(values[axis],.975))<1e-12 && row.values[5]==expected[axis] && row.values[6]==expected[axis+2]);
        CHECK(ps_report_row_read(report,1,axis,&row)==PS_OK);double radius=1.959963984540054*expected[axis+2]/16;
        CHECK(fabs(row.values[0]-(mean-radius))<1e-12 && fabs(row.values[1]-(mean+radius))<1e-12);
        CHECK(ps_report_curve_view(report,axis*2+1,0,&histogram)==PS_OK && histogram->kind==PS_PLOT_HISTOGRAM && histogram->source_count==256);
        double counts[16]={0},lo=values[axis][0],hi=values[axis][255];unsigned bins=lo==hi?1:16;
        CHECK(histogram->count==bins);
        for(unsigned i=0;i<256;i++){double fraction=lo==hi?0:(values[axis][i]-lo)/(hi-lo);unsigned bin=fraction>=1?bins-1:(unsigned)(fraction*bins);counts[bin]++;}
        for(unsigned i=0;i<bins;i++)CHECK(histogram->y[i]==counts[i]);
    }
    ps_table_info info;CHECK(ps_report_table_read(report,0,&info)==PS_OK && info.columns==7 && info.rows==2);
    CHECK(ps_report_table_read(report,1,&info)==PS_OK && info.columns==2 && info.rows==2);
    ps_report_destroy(report);puts("Monte Carlo report: full traces, independent histograms, type-7 quantiles and known-sigma mean intervals passed");return 0;
}
