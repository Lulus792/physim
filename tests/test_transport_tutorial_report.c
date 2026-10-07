#include "physim/report.h"
#include "physim/data.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Transport report %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char **argv) {
    CHECK(argc==3);ps_report *report=NULL;CHECK(ps_report_load(argv[1],&report)==PS_OK);
    uint32_t plots,tables;CHECK(ps_report_describe(report,NULL,NULL,&plots,&tables)==PS_OK && plots==4 && tables==1);
    const ps_curve_data *curves[5];unsigned next=0;unsigned counts[]={2,1,1,1};
    const int8_t dimensions[4][7]={{-3,1,0,0,0,0,0},{-3,1,0,0,0,0,0},{-2,1,0,0,0,0,0},{3,0,-1,0,0,0,0}};
    for(unsigned p=0;p<4;p++) {
        ps_plot_info info;CHECK(ps_report_plot_read(report,p,&info)==PS_OK && info.curves==counts[p]);
        CHECK(info.x_unit.dimension[2]==1 && !memcmp(info.y_unit.dimension,dimensions[p],7));
        for(unsigned c=0;c<counts[p];c++){CHECK(ps_report_curve_view(report,p,c,&curves[next])==PS_OK);next++;}
    }
    ps_run_reader reader;CHECK(ps_run_open(&reader,argv[2])==PS_OK);double t,v[PS_MAX_CHANNELS],initial=0,drift=0;unsigned i=0;ps_result r;
    while((r=ps_run_next(&reader,&t,v))==PS_OK) {
        if(!i)initial=v[3];drift=fmax(drift,fabs(v[3]-initial));
        for(unsigned c=0;c<5;c++)CHECK(i<curves[c]->count && curves[c]->x[i]==t && fabs(curves[c]->y[i]-v[c])<=1e-12*fmax(1,fabs(v[c])));
        i++;
    }
    CHECK(r==PS_EOF && i>1);
    for(unsigned c=0;c<5;c++)CHECK(curves[c]->count==i && curves[c]->source_count==i);
    ps_table_info info;ps_table_row row;
    CHECK(ps_report_table_read(report,0,&info)==PS_OK && info.columns==2 && info.rows==1 && info.column[1].unit.dimension[1]==1);
    CHECK(ps_report_row_read(report,0,0,&row)==PS_OK && row.values[0]==i && fabs(row.values[1]-drift)<1e-10);
    ps_run_reader_close(&reader);ps_report_destroy(report);puts("Transport report: five complete curves, SI dimensions and mass drift passed");return 0;
}
