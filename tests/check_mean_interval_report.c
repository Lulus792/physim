#include "physim/report.h"
#include <math.h>
#include <stdio.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Interval report %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char **argv) {
    CHECK(argc==2);ps_report *report=NULL;
    CHECK(ps_report_load(argv[1],&report)==PS_OK);uint32_t plots,tables;
    CHECK(ps_report_describe(report,NULL,NULL,&plots,&tables)==PS_OK && plots==0 && tables==1);
    ps_table_info info;ps_table_row row;
    CHECK(ps_report_table_read(report,0,&info)==PS_OK && info.columns==4 && info.rows==1);
    CHECK(ps_report_row_read(report,0,0,&row)==PS_OK && row.values[0]==3 && row.values[3]==.95);
    double half=2.776445105197799*sqrt(.5);
    CHECK(fabs(row.values[1]-(3-half))<2e-12 && fabs(row.values[2]-(3+half))<2e-12);
    for(unsigned i=0;i<3;i++)CHECK(info.column[i].unit.dimension[0]==1 && info.column[i].unit.dimension[2]==0);
    CHECK(info.column[3].unit.dimension[0]==0);
    ps_report_destroy(report);puts("Student-t analysis report: independent interval endpoints and SI columns passed");return 0;
}
