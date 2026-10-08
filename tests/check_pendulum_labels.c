#include "physim/report.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if(!(x)){fprintf(stderr,"Report labels %d: %s\n",__LINE__,#x);return 1;} } while(0)
int main(int argc,char **argv) {
    CHECK(argc>=3 && argc<=10);ps_report *report=NULL;
    CHECK(ps_report_load(argv[1],&report)==PS_OK);uint32_t plots,tables;
    CHECK(ps_report_describe(report,NULL,NULL,&plots,&tables)==PS_OK && plots==2 && tables==3);
    for(unsigned p=0;p<2;p++) {
        ps_plot_info info;CHECK(ps_report_plot_read(report,p,&info)==PS_OK && info.curves==(unsigned)argc-2);
        for(int i=2;i<argc;i++) {
            const ps_curve_data *curve;CHECK(ps_report_curve_view(report,p,i-2,&curve)==PS_OK);
            CHECK(!strcmp(curve->label,argv[i]) && curve->source_count>100);
        }
    }
    for(unsigned t=0;t<3;t++) {
        ps_table_info info;CHECK(ps_report_table_read(report,t,&info)==PS_OK && info.rows==(unsigned)argc-2);
        for(int i=2;i<argc;i++) {
            ps_table_row row;CHECK(ps_report_row_read(report,t,i-2,&row)==PS_OK);
            CHECK(!strcmp(row.label,argv[i]));
        }
    }
    ps_report_destroy(report);puts("Report labels: both plots and all three tables identify the executed method passed");return 0;
}
