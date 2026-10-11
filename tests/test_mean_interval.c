#include "physim/series.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Mean interval %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char **argv) {
    CHECK(argc==2);ps_mean_interval out;
    CHECK(ps_student_mean_interval(0,1,2,.95,&out)==PS_OK && fabs(out.critical_value-12.706204736174696)<2e-13);
    CHECK(ps_student_mean_interval(0,1,3,.95,&out)==PS_OK && fabs(out.critical_value-4.302652729749464)<2e-13);
    CHECK(ps_student_mean_interval(0,1,5,.95,&out)==PS_OK && fabs(out.critical_value-2.776445105197799)<2e-12);
    CHECK(ps_student_mean_interval(3,0,20,.95,&out)==PS_OK && out.lower==3 && out.upper==3);
    ps_mean_interval sentinel;memset(&sentinel,0x5a,sizeof sentinel);out=sentinel;
    CHECK(ps_student_mean_interval(0,1,1,.95,&out)==PS_INVALID && !memcmp(&out,&sentinel,sizeof out));
    CHECK(ps_student_mean_interval(0,-1,2,.95,&out)==PS_INVALID && !memcmp(&out,&sentinel,sizeof out));
    CHECK(ps_student_mean_interval(NAN,1,2,.95,&out)==PS_INVALID && !memcmp(&out,&sentinel,sizeof out));
    CHECK(ps_student_mean_interval(0,1,2,1,&out)==PS_INVALID && !memcmp(&out,&sentinel,sizeof out));
    CHECK(ps_student_mean_interval(0,1,2,0,&out)==PS_INVALID && !memcmp(&out,&sentinel,sizeof out));
    CHECK(ps_student_mean_interval(0,DBL_MAX,2,.99,&out)==PS_NUMERIC && !memcmp(&out,&sentinel,sizeof out));
    CHECK(ps_student_mean_interval(1,DBL_MIN,2,.95,&out)==PS_NUMERIC && !memcmp(&out,&sentinel,sizeof out));
    CHECK(ps_student_mean_interval(0,nextafter(0,1),UINT64_MAX,.95,&out)==PS_NUMERIC && !memcmp(&out,&sentinel,sizeof out));
    ps_analysis_context *context=NULL;char prefix[4096];snprintf(prefix,sizeof prefix,"%s/interval",argv[1]);
    CHECK(ps_analysis_create(prefix,0,&context)==PS_OK);
    double values[]={100,200,300,400,500};ps_unit cm={{1,0,0,0,0,0,0},.01,"cm"};ps_series data;
    CHECK(ps_series_from_values(context,values,5,cm,"measurement",&data)==PS_OK);
    CHECK(ps_series_mean_interval(context,data,.95,&out)==PS_OK && out.count==5 && out.mean==3 &&
          fabs(out.standard_deviation-sqrt(2.5))<1e-14 && fabs(out.standard_error-sqrt(.5))<1e-14);
    double flags[]={1,0,1,0,1};ps_unit one={{0},1,"1"};ps_series selector,masked;
    CHECK(ps_series_aligned_values(context,data,flags,5,one,"status",&selector)==PS_OK &&
          ps_series_mask(context,data,selector,1,&masked)==PS_OK);
    CHECK(ps_series_mean_interval(context,masked,.95,&out)==PS_OK && out.count==3 && out.mean==3 &&
          fabs(out.standard_deviation-2)<1e-14 && fabs(out.critical_value-4.302652729749464)<2e-13);
    ps_series single;CHECK(ps_series_slice(context,data,0,1,&single)==PS_OK);out=sentinel;
    CHECK(ps_series_mean_interval(context,single,.95,&out)==PS_INVALID && !memcmp(&out,&sentinel,sizeof out));
    double tiny_values[]={DBL_MIN,2*DBL_MIN};ps_series tiny;
    CHECK(ps_series_from_values(context,tiny_values,2,PS_METRE,"tiny distinct values",&tiny)==PS_OK);
    CHECK(ps_series_mean_interval(context,tiny,.95,&out)==PS_NUMERIC && !memcmp(&out,&sentinel,sizeof out));
    CHECK(ps_series_release(context,data)==PS_OK);
    CHECK(ps_series_mean_interval(context,data,.95,&out)==PS_INVALID && !memcmp(&out,&sentinel,sizeof out));
    ps_analysis_destroy(context);
    puts("Mean intervals: closed references, SI series, masks, invalid input, range failure and atomic output passed");return 0;
}
