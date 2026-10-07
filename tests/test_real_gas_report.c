#include "physim/report.h"
#include <math.h>
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Real gas report %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char **argv) {
    CHECK(argc==2);ps_report *r=NULL;CHECK(ps_report_load(argv[1],&r)==PS_OK);
    uint32_t plots,tables;CHECK(ps_report_describe(r,NULL,NULL,&plots,&tables)==PS_OK && plots==2 && !tables);
    for(unsigned j=0;j<2;j++) {
        ps_curve_data data;ps_plot_info info;CHECK(ps_report_curve_read(r,j,0,&data)==PS_OK && data.count==7);
        CHECK(ps_report_plot_read(r,j,&info)==PS_OK && info.x_unit.dimension[0]==3 && info.y_unit.dimension[0]==-1 && info.y_unit.dimension[1]==1 && info.y_unit.dimension[2]==-2);
        for(unsigned i=0;i<7;i++) {
            double volume=.005-.004*fmin(1,.25*i),pressure=j?8.31446261815324*450/(volume-4e-5)-.4/(volume*volume):8.31446261815324*450/volume;
            CHECK(fabs(data.x[i]-volume)<1e-17 && fabs(data.y[i]-pressure)<3e-12*pressure);
        }
    }
    ps_report_destroy(r);puts("Real gas report: ideal and vdW pressure curves with SI dimensions passed");return 0;
}
