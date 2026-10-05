/* Independent public-API consumer for target-time series from a relocated SDK. */
#include "physim/data.h"
#include "physim/report.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"SDK target series line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char **argv) {
    CHECK(argc==3 && (!strcmp(argv[2],"installed") || !strcmp(argv[2],"rebuilt")));
    double endpoints[3];unsigned counts[3];char path[4096];
    for(unsigned i=0;i<3;i++) {
        snprintf(path,sizeof path,"%s/run-%04u.psrun",argv[1],i+1);
        ps_run_reader r;CHECK(ps_run_open(&r,path)==PS_OK && r.channels==6);
        char expected[96];snprintf(expected,sizeof expected,"\nparameter.length=%.17g\n",.5+i);
        CHECK(strstr(r.metadata,expected) && strstr(r.metadata,"\nstep_mode=adaptive\n") &&
              strstr(r.metadata,"\nend_time_s=0.69999999999999996\n"));
        double t,v[PS_MAX_CHANNELS],previous=-1,energy=0;unsigned n=0;ps_result status;
        while((status=ps_run_next(&r,&t,v))==PS_OK) {
            CHECK(t>previous && t<=.7);
            if(!n){CHECK(t==0);energy=v[4];}else CHECK(fabs(v[4]-energy)<1e-6);
            previous=t;endpoints[i]=v[0];n++;
        }
        CHECK(status==PS_EOF && previous==.7 && n>5 && n<=1001);counts[i]=n;ps_run_reader_close(&r);
    }
    CHECK(counts[0]!=counts[2]);
    snprintf(path,sizeof path,"%s/summary.psreport",argv[1]);ps_report *report=NULL;
    CHECK(ps_report_load(path,&report)==PS_OK);const ps_curve_data *curve;
    ps_plot_info plot;CHECK(ps_report_plot_read(report,0,&plot)==PS_OK && !plot.x_unit.symbol[0]);
    CHECK(ps_report_curve_view(report,0,0,&curve)==PS_OK && curve->kind==PS_PLOT_LINE && curve->count==3);
    for(unsigned i=0;i<3;i++)CHECK(curve->x[i]==.5+i && curve->y[i]==endpoints[i]);
    snprintf(path,sizeof path,"%s/study-%s.svg",argv[1],argv[2]);CHECK(ps_report_export_svg(report,0,path)==PS_OK);
    FILE *svg_file=fopen(path,"rb");char svg[65536];CHECK(svg_file);
    size_t length=fread(svg,1,sizeof svg-1,svg_file);svg[length]=0;CHECK(!fclose(svg_file));
    CHECK(strstr(svg,">length</text>") && strstr(svg,">angle [rad]</text>"));
    ps_report_destroy(report);
    puts("Installed target-time series: variable step counts, exact common endpoint, energy and study report passed");return 0;
}
