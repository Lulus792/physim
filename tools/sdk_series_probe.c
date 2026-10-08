/* Independent public-API consumer for target-time series from a relocated SDK. */
#include "physim/data.h"
#include "physim/report.h"
#include "physim/series.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"SDK target series line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char **argv) {
    CHECK(argc==3 && (!strcmp(argv[2],"installed") || !strcmp(argv[2],"rebuilt")));
    ps_analysis_context *ctx=NULL;
    char prefix[4096];snprintf(prefix,sizeof prefix,"%s/pchip-%s",argv[1],argv[2]);
    CHECK(ps_analysis_create(prefix,0,&ctx)==PS_OK);
    const double axis[]={0,1,2},ordinate[]={0,1,4},grid[]={.5,1.5};
    ps_series x,y,q,pchip;double cubic[2];size_t got;
    CHECK(ps_series_from_values(ctx,axis,3,PS_SECOND,"x",&x)==PS_OK &&
          ps_series_aligned_values(ctx,x,ordinate,3,PS_METRE,"y",&y)==PS_OK &&
          ps_series_from_values(ctx,grid,2,PS_SECOND,"q",&q)==PS_OK &&
          ps_series_resample(ctx,y,x,q,PS_RESAMPLE_PCHIP,&pchip)==PS_OK &&
          ps_series_aligned(ctx,pchip,q)==PS_OK &&
          ps_series_read(ctx,pchip,0,cubic,2,&got)==PS_OK && got==2 &&
          fabs(cubic[0]-.3125)<2e-15 && fabs(cubic[1]-2.1875)<2e-15);
    ps_analysis_destroy(ctx);
    double endpoints[3];unsigned counts[3];char path[4096];
    for(unsigned i=0;i<3;i++) {
        snprintf(path,sizeof path,"%s/run-%04u.psrun",argv[1],i+1);
        ps_run_reader r;CHECK(ps_run_open(&r,path)==PS_OK && r.channels==9);
        const char *names[]={"angle","angular_velocity","position.x","position.y","energy",
                             "sensor.angle","velocity.x","velocity.y","speed"};
        for(unsigned channel=0;channel<9;channel++)CHECK(!strcmp(r.schema[channel].name,names[channel]));
        const int8_t dimension[]={1,0,-1,0,0,0,0};
        for(unsigned channel=6;channel<9;channel++)
            CHECK(!memcmp(r.schema[channel].dimension,dimension,7) && !strcmp(r.schema[channel].unit,"m/s"));
        ps_parameter_unit unit;
        CHECK(ps_parameter_unit_parse(r.metadata,"length",&unit)==PS_OK && unit.declared &&
              unit.scale==1 && unit.dimension[0]==1 && !strcmp(unit.symbol,"m"));
        char expected[96];snprintf(expected,sizeof expected,"\nparameter.length=%.17g\n",.5+i);
        CHECK(strstr(r.metadata,expected) && strstr(r.metadata,"\nstep_mode=adaptive\n") &&
              strstr(r.metadata,"\nend_time_s=0.69999999999999996\n"));
        double t,v[PS_MAX_CHANNELS],previous=-1,energy=0;unsigned n=0;ps_result status;
        while((status=ps_run_next(&r,&t,v))==PS_OK) {
            CHECK(t>previous && t<=.7);
            if(!n){CHECK(t==0);energy=v[4];}else CHECK(fabs(v[4]-energy)<1e-6);
            double length_m=.5+i;
            CHECK(fabs(v[6]-length_m*cos(v[0])*v[1])<1e-10 &&
                  fabs(v[7]-length_m*sin(v[0])*v[1])<1e-10 && v[8]>=0 &&
                  fabs(v[8]-hypot(v[6],v[7]))<1e-10);
            previous=t;endpoints[i]=v[0];n++;
        }
        CHECK(status==PS_EOF && previous==.7 && n>5 && n<=1001);counts[i]=n;ps_run_reader_close(&r);
    }
    CHECK(counts[0]!=counts[2]);
    snprintf(path,sizeof path,"%s/summary.psreport",argv[1]);ps_report *report=NULL;
    CHECK(ps_report_load(path,&report)==PS_OK);const ps_curve_data *curve;
    ps_plot_info plot;CHECK(ps_report_plot_read(report,0,&plot)==PS_OK && !strcmp(plot.x_unit.symbol,"m") && plot.x_unit.dimension[0]==1 && plot.x_unit.scale==1);
    CHECK(ps_report_curve_view(report,0,0,&curve)==PS_OK && curve->kind==PS_PLOT_LINE && curve->count==3);
    for(unsigned i=0;i<3;i++)CHECK(curve->x[i]==.5+i && curve->y[i]==endpoints[i]);
    snprintf(path,sizeof path,"%s/study-%s.svg",argv[1],argv[2]);CHECK(ps_report_export_svg(report,0,path)==PS_OK);
    FILE *svg_file=fopen(path,"rb");char svg[65536];CHECK(svg_file);
    size_t length=fread(svg,1,sizeof svg-1,svg_file);svg[length]=0;CHECK(!fclose(svg_file));
    CHECK(strstr(svg,">length [m]</text>") && strstr(svg,">angle [rad]</text>"));
    ps_report_destroy(report);
    puts("Installed target-time series: variable step counts, exact common endpoint, energy and study report passed");return 0;
}
