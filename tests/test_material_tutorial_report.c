#include "physim/report.h"
#include "physim/data.h"
#include <math.h>
#include <stdio.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Material report line %d: %s\n",__LINE__,#x);return 1;}} while(0)
int main(int argc,char **argv) {
    CHECK(argc==3);ps_report *report=NULL;CHECK(ps_report_load(argv[1],&report)==PS_OK);
    uint32_t plots,tables;CHECK(ps_report_describe(report,NULL,NULL,&plots,&tables)==PS_OK && plots==3 && tables==0);
    const ps_curve_data *curves[7];unsigned at=0;
    for(unsigned p=0;p<3;p++) {
        ps_plot_info info;CHECK(ps_report_plot_read(report,p,&info)==PS_OK);
        CHECK(info.x_unit.dimension[2]==1 && info.y_unit.dimension[0]==(p==2?2:1) && info.y_unit.dimension[2]==(p==2?-2:-1));
        unsigned count=p==0?2:p==1?1:4;
        for(unsigned c=0;c<count;c++) {CHECK(ps_report_curve_view(report,p,c,&curves[at])==PS_OK);CHECK(curves[at]->count==1001 && curves[at]->source_count==1001 && curves[at]->kind==PS_PLOT_LINE);at++;}
        const ps_curve_data *extra;CHECK(ps_report_curve_view(report,p,count,&extra)!=PS_OK);
    }
    ps_run_reader run;CHECK(ps_run_open(&run,argv[2])==PS_OK);double time,values[PS_MAX_CHANNELS];unsigned row=0;ps_result r;
    while((r=ps_run_next(&run,&time,values))==PS_OK) {
        CHECK(row<1001);double expected[]={values[1],values[12],values[1]-values[12],values[7],values[9],values[8],values[10]};
        for(unsigned c=0;c<7;c++) CHECK(curves[c]->x[row]==time && isfinite(curves[c]->y[row]) && fabs(curves[c]->y[row]-expected[c])<=1e-12*fmax(1,fabs(expected[c])));
        row++;
    }
    CHECK(r==PS_EOF && row==1001);ps_run_reader_close(&run);ps_report_destroy(report);
    puts("Material report: all three plots, seven full curves, SI units and original measurements passed");return 0;
}
