#include "physim/report.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Series SI %d: %s\n",__LINE__,#x);return 1;}}while(0)
static int same_handle(ps_series a,ps_series b){return a.owner==b.owner && a.slot==b.slot && a.generation==b.generation;}
static int read_one(ps_analysis_context *c,ps_series s,size_t i,double expected) {
    double value;size_t count;return ps_series_read(c,s,i,&value,1,&count)==PS_OK && count==1 && fabs(value-expected)<=2e-12*fmax(1,fabs(expected));
}
int main(int argc,char **argv) {
    CHECK(argc==2);char prefix[4096];snprintf(prefix,sizeof prefix,"%s/series-si",argv[1]);
    ps_analysis_context *c=NULL;CHECK(ps_analysis_create(prefix,0,&c)==PS_OK);
    ps_unit cm=PS_METRE,ms=PS_SECOND;cm.scale=.01;cm.symbol="cm";ms.scale=.001;ms.symbol="ms";
    double one=1,fifty=50;ps_series a,b,sum;CHECK(ps_series_from_values(c,&one,1,PS_METRE,"a",&a)==PS_OK);
    CHECK(ps_series_aligned_values(c,a,&fifty,1,cm,"b",&b)==PS_OK);
    CHECK(ps_series_combine(c,PS_SERIES_ADD,a,b,&sum)==PS_OK && read_one(c,sum,0,1.5));
    ps_series_info info;CHECK(ps_series_describe(c,b,&info)==PS_OK && info.scale==1 && !strcmp(info.symbol,"m"));
    CHECK(read_one(c,b,0,.5) && fifty==50);
    enum {N=513};double raw_x[N],raw_y[N],constant[N],flags[N];
    for(unsigned i=0;i<N;i++){raw_x[i]=250*i;raw_y[i]=100+12.5*i;constant[i]=50;flags[i]=i==256?0:100;}
    ps_series x,y,half,add,sub,mul,div,affine,rate,integral,selector,masked,valid;
    CHECK(ps_series_from_values(c,raw_x,N,ms,"time",&x)==PS_OK);
    CHECK(ps_series_aligned_values(c,x,raw_y,N,cm,"length",&y)==PS_OK);
    CHECK(ps_series_aligned_values(c,x,constant,N,cm,"half",&half)==PS_OK);
    CHECK(ps_series_combine(c,PS_SERIES_ADD,y,half,&add)==PS_OK);
    CHECK(ps_series_combine(c,PS_SERIES_SUBTRACT,y,half,&sub)==PS_OK);
    CHECK(ps_series_combine(c,PS_SERIES_MULTIPLY,y,half,&mul)==PS_OK);
    CHECK(ps_series_combine(c,PS_SERIES_DIVIDE,y,half,&div)==PS_OK);
    CHECK(ps_series_affine(c,y,2,(ps_quantity){25,cm},&affine)==PS_OK);
    CHECK(ps_series_derivative(c,y,x,&rate)==PS_OK);
    ps_unit cms={{1,0,1,0,0,0,0},.01,"cm s"};CHECK(ps_series_integral(c,y,x,(ps_quantity){10,cms},&integral)==PS_OK);
    ps_unit percent=PS_ONE;percent.scale=.01;percent.symbol="%";
    CHECK(ps_series_aligned_values(c,x,flags,N,percent,"selector",&selector)==PS_OK);
    CHECK(ps_series_mask(c,y,selector,1,&masked)==PS_OK && ps_series_validity(c,masked,&valid)==PS_OK);
    for(unsigned i=0;i<N;i++) {
        double t=.25*i;CHECK(read_one(c,x,i,t) && read_one(c,y,i,1+.5*t));
        CHECK(read_one(c,add,i,1.5+.5*t) && read_one(c,sub,i,.5+.5*t));
        CHECK(read_one(c,mul,i,.5+.25*t) && read_one(c,div,i,2+t));
        CHECK(read_one(c,affine,i,2.25+t) && read_one(c,rate,i,.5));
        CHECK(read_one(c,integral,i,.1+t+.25*t*t) && read_one(c,valid,i,i==256?0:1));
        CHECK(raw_x[i]==250*i && raw_y[i]==100+12.5*i);
    }
    ps_series list[]={x,y,add,sub,mul,div,affine,rate,integral,selector};
    for(unsigned i=0;i<sizeof list/sizeof *list;i++){CHECK(ps_series_describe(c,list[i],&info)==PS_OK && info.scale==1);}
    CHECK(ps_series_describe(c,rate,&info)==PS_OK && info.dimension[0]==1 && info.dimension[2]==-1);
    CHECK(ps_series_describe(c,integral,&info)==PS_OK && info.dimension[0]==1 && info.dimension[2]==1);
    uint64_t bytes=ps_analysis_scratch_bytes(c);ps_series preserved=y;
    double late[N];for(unsigned i=0;i<N;i++)late[i]=1;late[N-1]=DBL_MAX;
    ps_unit huge=PS_METRE;huge.scale=2;
    CHECK(ps_series_aligned_values(c,x,late,N,huge,"overflow",&preserved)==PS_NUMERIC && same_handle(preserved,y) && ps_analysis_scratch_bytes(c)==bytes);
    double tiny=DBL_TRUE_MIN;ps_unit small=PS_METRE;small.scale=.5;
    CHECK(ps_series_from_values(c,&tiny,1,small,"underflow",&preserved)==PS_NUMERIC && same_handle(preserved,y) && ps_analysis_scratch_bytes(c)==bytes);
    /* Offset need not be separately representable when the final sum is finite. */
    double negative=-0x1p1023;ps_series large,result;CHECK(ps_series_from_values(c,&negative,1,PS_METRE,"large",&large)==PS_OK);
    CHECK(ps_series_affine(c,large,1,(ps_quantity){0x1p1023,huge},&result)==PS_OK && read_one(c,result,0,0x1p1023));
    double query_values[1025];for(unsigned i=0;i<1025;i++)query_values[i]=125*i;
    ps_series query,linear,pchip,nearest,previous;
    CHECK(ps_series_from_values(c,query_values,1025,ms,"target",&query)==PS_OK);
    CHECK(ps_series_resample(c,y,x,query,PS_RESAMPLE_LINEAR,&linear)==PS_OK);
    CHECK(ps_series_resample(c,y,x,query,PS_RESAMPLE_PCHIP,&pchip)==PS_OK);
    CHECK(ps_series_resample(c,y,x,query,PS_RESAMPLE_NEAREST,&nearest)==PS_OK);
    CHECK(ps_series_resample(c,y,x,query,PS_RESAMPLE_PREVIOUS,&previous)==PS_OK);
    for(unsigned i=0;i<1025;i++) {
        CHECK(read_one(c,linear,i,1+.0625*i) && read_one(c,pchip,i,1+.0625*i));
        CHECK(read_one(c,nearest,i,1+.125*(i/2)) && read_one(c,previous,i,1+.125*(i/2)));
    }
    ps_series empty;CHECK(ps_series_from_values(c,NULL,0,cm,"empty",&empty)==PS_OK && ps_series_describe(c,empty,&info)==PS_OK && info.scale==1 && info.count==0);
    double zero=0;ps_series mask_none,all_missing,skipped;
    CHECK(ps_series_aligned_values(c,a,&zero,1,PS_ONE,"none",&mask_none)==PS_OK);
    CHECK(ps_series_mask(c,a,mask_none,1,&all_missing)==PS_OK);
    ps_unit four=PS_METRE;four.scale=4;
    CHECK(ps_series_affine(c,all_missing,1,(ps_quantity){0x1p1023,four},&skipped)==PS_OK);
    uint8_t present;double placeholder;size_t returned;
    CHECK(ps_series_read_masked(c,skipped,0,&placeholder,&present,1,&returned)==PS_OK && returned==1 && !present);
    char csv[4096],report_path[4096];snprintf(csv,sizeof csv,"%s/series-si.csv",argv[1]);snprintf(report_path,sizeof report_path,"%s/series-si.psreport",argv[1]);
    ps_series columns[]={x,y,add,rate,integral};CHECK(ps_series_export_csv(c,columns,5,csv)==PS_OK);
    ps_report *report=NULL;ps_plot_handle plot;ps_plot_info plot_info={0};strcpy(plot_info.title,"SI length");
    ps_report_unit_from(PS_SECOND,&plot_info.x_unit);ps_report_unit_from(PS_METRE,&plot_info.y_unit);
    CHECK(ps_report_create("Series SI","Explicit cm/ms import, SI operations and output",&report)==PS_OK);
    CHECK(ps_report_add_plot(report,&plot_info,&plot)==PS_OK && ps_report_add_series(report,plot,c,x,add,"sum",PS_PLOT_LINE)==PS_OK);
    CHECK(ps_report_save(report,report_path)==PS_OK);ps_report_destroy(report);report=NULL;
    CHECK(ps_report_load(report_path,&report)==PS_OK);ps_curve_data curve;CHECK(ps_report_curve_read(report,0,0,&curve)==PS_OK && curve.count==N);
    CHECK(curve.x[512]==128 && fabs(curve.y[512]-65.5)<1e-12);ps_report_destroy(report);
    ps_analysis_destroy(c);
    /* Failed conversion and quota keep the source, handles and scratch quota intact. */
    snprintf(prefix,sizeof prefix,"%s/limited-si",argv[1]);CHECK(ps_analysis_create(prefix,8,&c)==PS_OK);
    CHECK(ps_series_from_values(c,&fifty,1,cm,"half",&a)==PS_OK && read_one(c,a,0,.5));
    preserved=a;CHECK(ps_series_aligned_values(c,a,&fifty,1,cm,"over quota",&preserved)==PS_LIMIT && same_handle(preserved,a) && ps_analysis_scratch_bytes(c)==8);
    ps_analysis_destroy(c);puts("Series SI: canonical imports, multiblock algebra/calculus/masks, affine cancellation, report/CSV and atomic range/quota failures passed");return 0;
}
