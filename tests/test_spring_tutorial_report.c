#include "physim/report.h"
#include "physim/data.h"
#include <math.h>
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Spring tutorial report %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char **argv) {
    CHECK(argc==3);ps_report *report=NULL;CHECK(ps_report_load(argv[1],&report)==PS_OK);
    uint32_t plots,tables;CHECK(ps_report_describe(report,NULL,NULL,&plots,&tables)==PS_OK && plots==3 && tables==1);
    const ps_curve_data *curve[5];unsigned next=0;
    for(unsigned p=0;p<3;p++) {
        ps_plot_info info;CHECK(ps_report_plot_read(report,p,&info)==PS_OK && info.curves==(p==2?3:1));
        CHECK(info.x_unit.dimension[2]==1 && info.y_unit.dimension[0]==(p==2?2:1) && info.y_unit.dimension[2]==(p==0?0:p==1?-1:-2));
        for(unsigned c=0;c<info.curves;c++){CHECK(ps_report_curve_view(report,p,c,&curve[next])==PS_OK && curve[next]->count==2001 && curve[next]->source_count==2001);next++;}
    }
    ps_run_reader reader;CHECK(ps_run_open(&reader,argv[2])==PS_OK);double t,v[PS_MAX_CHANNELS],initial=0,maximum=0;unsigned i=0;ps_result r;
    while((r=ps_run_next(&reader,&t,v))==PS_OK) {
        CHECK(i<2001);if(!i)initial=v[6];maximum=fmax(maximum,fabs(v[6]-initial));double expected[]={v[0],v[1],v[4],v[5],v[6]};
        for(unsigned c=0;c<5;c++)CHECK(curve[c]->x[i]==t && fabs(curve[c]->y[i]-expected[c])<=1e-12*fmax(1,fabs(expected[c])));
        i++;
    }
    CHECK(r==PS_EOF && i==2001);ps_table_info info;ps_table_row row;
    CHECK(ps_report_table_read(report,0,&info)==PS_OK && info.columns==2 && info.rows==1 && info.column[1].unit.dimension[1]==1);
    CHECK(ps_report_row_read(report,0,0,&row)==PS_OK && row.values[0]==2001 && fabs(row.values[1]-maximum)<1e-14);
    ps_run_reader_close(&reader);ps_report_destroy(report);puts("Spring tutorial report: five complete curves, SI units and exact drift statistic passed");return 0;
}
