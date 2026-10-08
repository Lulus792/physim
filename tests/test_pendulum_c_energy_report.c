#include "physim/report.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"C pendulum energy %d: %s\n",__LINE__,#x);return 1; } } while (0)
static bool near(double a,double b) {
    return isfinite(a) && isfinite(b) && fabs(a-b)<1e-12*fmax(1,fabs(b));
}
int main(int argc,char **argv) {
    CHECK(argc==3);
    ps_run_reader reader;CHECK(ps_run_open(&reader,argv[2])==PS_OK);
    int angle=-1,energy=-1;
    for(unsigned i=0;i<reader.channels;i++) {
        if(!strcmp(reader.schema[i].name,"angle"))angle=(int)i;
        if(!strcmp(reader.schema[i].name,"energy"))energy=(int)i;
    }
    CHECK(angle>=0 && energy>=0);
    double t,v[PS_MAX_CHANNELS],first=0,last=0,initial=0,maximum=0,previous=0,
           last_cross=0,period_sum=0;
    uint64_t samples=0;unsigned intervals=0;bool crossed=false;ps_result result;
    while((result=ps_run_next(&reader,&t,v))==PS_OK) {
        if(!samples){first=t;initial=v[energy];}
        maximum=fmax(maximum,fabs(v[energy]-initial));
        if(samples && previous<0 && v[angle]>=0) {
            double crossing=last+(t-last)*(-previous)/(v[angle]-previous);
            if(crossed){period_sum+=crossing-last_cross;intervals++;}
            crossed=true;last_cross=crossing;
        }
        previous=v[angle];last=t;samples++;
    }
    CHECK(result==PS_EOF && samples>1);
    ps_run_reader_close(&reader);
    ps_report *report=NULL;CHECK(ps_report_load(argv[1],&report)==PS_OK);
    uint32_t plots,tables;CHECK(ps_report_describe(report,NULL,NULL,&plots,&tables)==PS_OK);
    CHECK(plots==4 && tables==2);
    ps_plot_info plot;const ps_curve_data *curve;
    CHECK(ps_report_plot_read(report,3,&plot)==PS_OK && plot.curves==1 &&
          !strcmp(plot.title,"Mechanische Energieänderung"));
    CHECK(plot.x_unit.dimension[2]==1 && plot.x_unit.scale==1 &&
          plot.y_unit.dimension[0]==2 && plot.y_unit.dimension[1]==1 &&
          plot.y_unit.dimension[2]==-2 && plot.y_unit.scale==1);
    CHECK(ps_report_curve_view(report,3,0,&curve)==PS_OK &&
          curve->source_count==samples && curve->x[0]==first &&
          curve->x[curve->count-1]==last && near(curve->y[0],0));
    ps_table_info metrics;ps_table_row row;
    CHECK(ps_report_table_read(report,1,&metrics)==PS_OK && metrics.rows==1 &&
          metrics.columns==(intervals?2u:1u) &&
          !strcmp(metrics.column[0].label,"Max. Energieabweichung"));
    CHECK(ps_report_row_read(report,1,0,&row)==PS_OK && near(row.values[0],maximum));
    if(intervals)CHECK(near(row.values[1],period_sum/intervals));
    if(samples<=PS_REPORT_MAX_POINTS) {
        CHECK(curve->count==samples && ps_run_open(&reader,argv[2])==PS_OK);
        for(unsigned i=0;i<samples;i++) {
            CHECK(ps_run_next(&reader,&t,v)==PS_OK);
            CHECK(curve->x[i]==t && near(curve->y[i],v[energy]-initial));
        }
        ps_run_reader_close(&reader);
    }
    ps_report_destroy(report);
    puts("C pendulum: energy plot samples, SI axes, full-source energy metrics and measured periods passed");
    return 0;
}
