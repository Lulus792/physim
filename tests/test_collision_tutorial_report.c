#include "physim/report.h"
#include <math.h>
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Collision report %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char **argv) {
    CHECK(argc==3);ps_report *report=NULL;CHECK(ps_report_load(argv[1],&report)==PS_OK);
    uint32_t plots,tables;CHECK(ps_report_describe(report,NULL,NULL,&plots,&tables)==PS_OK && plots==4 && tables==2);
    const ps_curve_data *curves[8];unsigned next=0;
    const unsigned channels[]={0,1,2,3,5,4,6,7},counts[]={2,2,1,3};
    for(unsigned p=0;p<4;p++) {
        ps_plot_info info;CHECK(ps_report_plot_read(report,p,&info)==PS_OK && info.curves==counts[p] && info.x_unit.dimension[2]==1);
        CHECK(info.y_unit.dimension[0]==(p<2?1:p==2?1:2) && info.y_unit.dimension[1]==(p<2?0:1) && info.y_unit.dimension[2]==(p==0?0:p==3?-2:-1));
        for(unsigned c=0;c<counts[p];c++)CHECK(ps_report_curve_view(report,p,c,&curves[next++])==PS_OK);
    }
    ps_run_reader reader;CHECK(ps_run_open(&reader,argv[2])==PS_OK);
    double t,v[PS_MAX_CHANNELS],p0=0,b0=0,pdrift=0,bdrift=0,sum=0;unsigned n=0;ps_result r;
    while((r=ps_run_next(&reader,&t,v))==PS_OK) {
        if(!n){p0=v[5];b0=v[7];}pdrift=fmax(pdrift,fabs(v[5]-p0));bdrift=fmax(bdrift,fabs(v[7]-b0));sum+=v[8];
        for(unsigned c=0;c<8;c++)CHECK(n<curves[c]->count && curves[c]->x[n]==t && fabs(curves[c]->y[n]-v[channels[c]])<1e-12);
        n++;
    }
    CHECK(r==PS_EOF && n);for(unsigned c=0;c<8;c++)CHECK(curves[c]->count==n && curves[c]->source_count==n);
    ps_table_info info;ps_table_row row;
    CHECK(ps_report_table_read(report,0,&info)==PS_OK && info.columns==5 && info.rows==1 && info.column[1].unit.dimension[1]==1);
    CHECK(ps_report_row_read(report,0,0,&row)==PS_OK && row.values[0]==n && fabs(row.values[1]-pdrift)<1e-12 && fabs(row.values[2]-v[6])<1e-12 && fabs(row.values[3]-bdrift)<1e-12 && row.values[4]==v[10]);
    CHECK(ps_report_table_read(report,1,&info)==PS_OK && info.columns==2 && info.rows==(v[10]==1?1u:0u) && info.column[0].unit.dimension[2]==1 && info.column[1].unit.dimension[0]==1);
    if(v[10]==1)CHECK(ps_report_row_read(report,1,0,&row)==PS_OK && fabs(row.values[0]-v[9])<1e-12 && fabs(row.values[1]-sum)<1e-12);
    ps_run_reader_close(&reader);ps_report_destroy(report);puts("Collision report: eight complete curves, units, conservation and event tables passed");return 0;
}
