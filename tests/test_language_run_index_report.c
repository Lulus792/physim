#include "physim/report.h"
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"RunIndex report line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char **argv) {
    CHECK(argc==2);ps_report *report=NULL;CHECK(ps_report_load(argv[1],&report)==PS_OK);
    ps_plot_info info;CHECK(ps_report_plot_read(report,0,&info)==PS_OK && info.x_unit.dimension[2]==1 && info.y_unit.dimension[0]==1);
    const ps_curve_data *curve;CHECK(ps_report_curve_view(report,0,0,&curve)==PS_OK && curve->count==256);
    for(unsigned i=0;i<256;i++)CHECK(curve->x[i]==(999744+i)*.001 && curve->y[i]==999744+i);
    ps_report_destroy(report);puts("RunIndex analysis: all 256 tail rows, times and SI units preserved");return 0;
}
