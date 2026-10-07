#include "physim/report.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Series SI report %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char **argv) {
    CHECK(argc==2);ps_report *r=NULL;CHECK(ps_report_load(argv[1],&r)==PS_OK);
    uint32_t plots,tables;CHECK(ps_report_describe(r,NULL,NULL,&plots,&tables)==PS_OK && plots==1 && tables==0);
    ps_plot_info info;ps_curve_data data;CHECK(ps_report_plot_read(r,0,&info)==PS_OK && ps_report_curve_read(r,0,0,&data)==PS_OK && data.count==513);
    const int8_t second[]={0,0,1,0,0,0,0},metre[]={1,0,0,0,0,0,0};
    CHECK(!memcmp(info.x_unit.dimension,second,7) && !memcmp(info.y_unit.dimension,metre,7) && info.x_unit.scale==1 && info.y_unit.scale==1);
    for(unsigned i=0;i<513;i++){CHECK(data.x[i]==.25*i && fabs(data.y[i]-(1.5+.125*i))<2e-12);}
    ps_report_destroy(r);puts("Series SI report: independent SI dimensions, scale 1 and every curve point passed");return 0;
}
