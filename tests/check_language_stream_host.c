#include "physim/report.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Stream host %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char **argv) {
    CHECK(argc==3);ps_run_reader run;CHECK(ps_run_open(&run,argv[1])==PS_OK);
    CHECK(run.channels==1 && !strcmp(run.schema[0].name,"replayed") && run.schema[0].dimension[0]==1);
    double t,v[PS_MAX_CHANNELS],expected[]={0,10,12,14};
    for(unsigned i=0;i<4;i++)CHECK(ps_run_next(&run,&t,v)==PS_OK && t==i*.125 && v[0]==expected[i]);
    CHECK(ps_run_next(&run,&t,v)==PS_EOF);ps_run_reader_close(&run);
    ps_report *report=NULL;CHECK(ps_report_load(argv[2],&report)==PS_OK);
    uint32_t plots,tables;CHECK(ps_report_describe(report,NULL,NULL,&plots,&tables)==PS_OK && plots==0 && tables==1);
    ps_table_info info;CHECK(ps_report_table_read(report,0,&info)==PS_OK && info.rows==3 && info.columns==2);
    CHECK(info.column[0].unit.dimension[2]==1 && info.column[1].unit.dimension[0]==1);
    for(unsigned i=0;i<3;i++){ps_table_row row;CHECK(ps_report_row_read(report,0,i,&row)==PS_OK && row.values[0]==i*.125 && row.values[1]==10+2*i);}
    ps_report_destroy(report);puts("Stream hosts: actual experiment replay and complete SI analysis table passed");return 0;
}
