#include "physim/analysis.h"
#include "physim/units.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"Analysis extremes %d: %s\n",__LINE__,#x);return 1;} } while(0)
static int contains(const char *path,const char *needle) {
    FILE *f=fopen(path,"rb"); if(!f)return 0;
    char text[8192];size_t n=fread(text,1,sizeof text-1,f);fclose(f);text[n]=0;
    return strstr(text,needle)!=NULL;
}
static ps_result analyze(const char *root,const char *name,const char *channel,
                         const double *values,const double *mask,size_t count,char prefix[4096]) {
    char path[4096];snprintf(path,sizeof path,"%s/%s.psrun",root,name);
    snprintf(prefix,4096,"%s/%s",root,name);
    ps_context c={.struct_size=sizeof c,.api_version=PS_API_VERSION,.dt_s=.01};
    if(ps_channel_add(&c,channel,PS_JOULE,"test")!=0)return PS_INVALID;
    char status[64];snprintf(status,sizeof status,"%s.status",channel);
    if(mask && ps_channel_add(&c,status,PS_ONE,"status")!=1)return PS_INVALID;
    ps_run_writer w;ps_result r=ps_run_create(&w,path,&c,"extreme regression");if(r!=PS_OK)return r;
    for(size_t i=0;i<count;i++) {double v[]={values[i],mask?mask[i]:1};r=ps_run_append(&w,(double)i*.01,v);if(r!=PS_OK)return r;}
    r=ps_run_close(&w);return r==PS_OK?ps_analyze_run(path,prefix):r;
}
int main(int argc,char **argv) {
    CHECK(argc==2);double x[]={-1e308,1e308},y[]={-1e308,1e308},out[]={42,42};
    CHECK(ps_derivative(x,y,2,out)==PS_OK && out[0]==1 && out[1]==1);
    CHECK(ps_derivative(x,y,2,y)==PS_OK && y[0]==1 && y[1]==1);
    double tiny[]={0,1e-200},large[]={1e308,1e308};
    CHECK(fabs(ps_trapezoid(tiny,large,2)/1e108-1)<1e-15);
    double impossible[]={0,DBL_MIN},signal[]={-DBL_MAX,DBL_MAX};out[0]=42;out[1]=43;
    CHECK(ps_derivative(impossible,signal,2,out)==PS_NUMERIC && out[0]==42 && out[1]==43);
    double partial_x[]={0,1,1+DBL_EPSILON},partial_y[]={0,1,DBL_MAX},partial_out[]={41,42,43};
    CHECK(ps_derivative(partial_x,partial_y,3,partial_out)==PS_NUMERIC &&
          partial_out[0]==41 && partial_out[1]==42 && partial_out[2]==43);
    CHECK(isnan(ps_trapezoid((double[]){1,0},large,2)));
    char prefix[4096],file[4120];double constants[]={0,1,-1,1e20,-1e20,DBL_MAX,-DBL_MAX};
    for(unsigned i=0;i<7;i++) {
        char name[64];snprintf(name,sizeof name,"constant-%u",i);double v[]={constants[i],constants[i]};
        CHECK(analyze(argv[1],name,"energy",v,NULL,2,prefix)==PS_OK);
        snprintf(file,sizeof file,"%s-plot.svg",prefix);
        CHECK(!contains(file,"nan") && !contains(file,"inf") && contains(file,"325.000"));
    }
    for(unsigned i=0;i<3;i++) {
        double magnitude=i==0?1:i==1?1e150:1e200;double v[]={magnitude,-magnitude};
        char name[64];snprintf(name,sizeof name,"variance-%u",i);
        ps_result r=analyze(argv[1],name,"signal",v,NULL,2,prefix);
        CHECK(r==(i==2?PS_NUMERIC:PS_OK));
        if(i==2){snprintf(file,sizeof file,"%s-summary.csv",prefix);CHECK(fopen(file,"rb")==NULL);}
        else {ps_statistics s={0};ps_statistics_push(&s,magnitude);ps_statistics_push(&s,-magnitude);
            CHECK(fabs(ps_statistics_stddev(&s)/(sqrt(2)*magnitude)-1)<1e-15);}
    }
    double e[]={0,100,0,100,0},m[]={0,1,2,1,0};
    CHECK(analyze(argv[1],"masked","energy",e,m,5,prefix)==PS_OK);
    snprintf(file,sizeof file,"%s-manifest.txt",prefix);CHECK(contains(file,"max_energy_drift_J=0\n"));
    double no[]={0,2,0,2,0};CHECK(analyze(argv[1],"missing","energy",e,no,5,prefix)==PS_OK);
    snprintf(file,sizeof file,"%s-manifest.txt",prefix);CHECK(contains(file,"max_energy_drift_J=nan\n"));
    double angles[]={-1,1,-1,0,1,-1,1},gap[]={1,1,1,2,1,1,1};
    CHECK(analyze(argv[1],"angle-gap","angle",angles,gap,7,prefix)==PS_OK);
    snprintf(file,sizeof file,"%s-manifest.txt",prefix);CHECK(contains(file,"period_intervals=0\n"));
    CHECK(analyze(argv[1],"angle-valid","angle",angles,NULL,7,prefix)==PS_OK);
    snprintf(file,sizeof file,"%s-manifest.txt",prefix);CHECK(contains(file,"period_intervals=2\n"));
    puts("Analysis extremes: finite plots, masked metrics, numeric rejection, scaled helpers and atomic aliases passed");return 0;
}
