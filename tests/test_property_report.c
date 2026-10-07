#include "physim/report.h"
#include <math.h>
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Property report %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char **argv) {
    CHECK(argc==2);ps_report *r=NULL;CHECK(ps_report_load(argv[1],&r)==PS_OK);
    uint32_t plots,tables;CHECK(ps_report_describe(r,NULL,NULL,&plots,&tables)==PS_OK && plots==1 && !tables);
    ps_curve_data data;ps_plot_info info;CHECK(ps_report_curve_read(r,0,0,&data)==PS_OK && data.count==5);
    CHECK(ps_report_plot_read(r,0,&info)==PS_OK && info.y_unit.dimension[0]==-3 && info.y_unit.dimension[1]==1);
    for(unsigned i=0;i<5;i++) {
        double time=.25*i,temperature=273+100*time,pressure=500000*time;
        CHECK(data.x[i]==time && fabs(data.y[i]-(1200-.2*temperature+1e-6*pressure))<5e-13);
    }
    ps_report_destroy(r);puts("Property report: independent affine values and SI dimensions passed");return 0;
}
