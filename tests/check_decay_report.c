#include "physim/report.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"Decay report %d: %s\n",__LINE__,#x);return 1; } } while (0)
int main(int argc,char **argv) {
    CHECK(argc==5);double expected=strtod(argv[2],NULL),period=strtod(argv[3],NULL);
    unsigned intervals=(unsigned)strtoul(argv[4],NULL,10);
    ps_report *report=NULL;CHECK(ps_report_load(argv[1],&report)==PS_OK);
    uint32_t tables;CHECK(ps_report_describe(report,NULL,NULL,NULL,&tables)==PS_OK);
    unsigned found=0;
    for(unsigned i=0;i<tables;i++) {
        ps_table_info info;CHECK(ps_report_table_read(report,i,&info)==PS_OK);
        if(strcmp(info.title,"Observed amplitude decay"))continue;
        found++;CHECK(info.columns==5 && info.rows==(intervals?1u:0u));
        CHECK(info.column[2].unit.dimension[2]==-1 && info.column[2].unit.scale==1);
        if(intervals) {
            ps_table_row row;CHECK(ps_report_row_read(report,i,0,&row)==PS_OK);
            CHECK(row.values[0]==intervals && fabs(row.values[1]-expected*period)<2e-12);
            for(unsigned j=2;j<5;j++)CHECK(fabs(row.values[j]-expected)<2e-12);
        }
    }
    CHECK(found==1);ps_report_destroy(report);
    puts("Decay report: known signed rate, log decrement, interval count and unavailable states passed");return 0;
}
