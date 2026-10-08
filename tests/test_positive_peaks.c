#include "physim/series.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"Positive peaks %d: %s\n",__LINE__,#x);return 1; } } while (0)
static double value(ps_analysis_context *c,ps_series s,unsigned at) {
    double result=NAN;size_t n=0;
    return ps_series_read(c,s,at,&result,1,&n)==PS_OK && n==1?result:NAN;
}
static int check(ps_analysis_context *c,ps_series out[3],uint64_t count) {
    for(unsigned i=0;i<3;i++) {
        ps_series_info info;CHECK(ps_series_describe(c,out[i],&info)==PS_OK && info.count==count && info.scale==1);
        CHECK(ps_series_aligned(c,out[0],out[i])==PS_OK);
    }
    return 0;
}
int main(int argc,char **argv) {
    CHECK(argc==2);ps_analysis_context *c=NULL;CHECK(ps_analysis_create(argv[1],0,&c)==PS_OK);
    double times[]={0,1,2,3,4,5,6,7,8},signal[]={0,1,2,2,1,-1,0,3,0};
    ps_series x,y,out[3];CHECK(ps_series_from_values(c,times,9,PS_SECOND,"t",&x)==PS_OK);
    CHECK(ps_series_aligned_values(c,x,signal,9,PS_RADIAN,"angle",&y)==PS_OK);
    CHECK(ps_series_positive_peaks(c,y,x,out)==PS_OK && !check(c,out,2));
    CHECK(value(c,out[0],0)==2.5 && value(c,out[1],0)==2 && value(c,out[2],0)==0);
    CHECK(value(c,out[0],1)==7 && value(c,out[1],1)==3 && value(c,out[2],1)==0);
    CHECK(ps_series_aligned(c,y,out[0])==PS_INVALID);
    for(unsigned i=0;i<3;i++)CHECK(ps_series_release(c,out[i])==PS_OK);
    double flags[]={1,1,1,0,1,1,1,1,1};ps_series mask,masked;
    CHECK(ps_series_aligned_values(c,x,flags,9,PS_ONE,"valid",&mask)==PS_OK);
    CHECK(ps_series_mask(c,y,mask,1,&masked)==PS_OK);
    CHECK(ps_series_positive_peaks(c,masked,x,out)==PS_OK && !check(c,out,1));
    CHECK(value(c,out[0],0)==7 && value(c,out[2],0)==1);
    for(unsigned i=0;i<3;i++)CHECK(ps_series_release(c,out[i])==PS_OK);
    ps_series saved[]={x,y,mask};memcpy(out,saved,sizeof out);uint64_t used=ps_analysis_scratch_bytes(c);
    CHECK(ps_series_positive_peaks(c,y,y,out)==PS_INVALID && !memcmp(saved,out,sizeof out) && ps_analysis_scratch_bytes(c)==used);
    double duplicate[]={0,1,2,2,4,5,6,7,8};ps_series bad_time;
    CHECK(ps_series_aligned_values(c,x,duplicate,9,PS_SECOND,"bad time",&bad_time)==PS_OK);
    used=ps_analysis_scratch_bytes(c);
    CHECK(ps_series_positive_peaks(c,y,bad_time,out)==PS_INVALID && !memcmp(saved,out,sizeof out) && ps_analysis_scratch_bytes(c)==used);
    /* Output handles may overwrite the caller's input-handle array. */
    ps_series alias[]={y,x,mask};CHECK(ps_series_positive_peaks(c,alias[0],alias[1],alias)==PS_OK && !check(c,alias,2));
    for(unsigned i=0;i<3;i++)CHECK(ps_series_release(c,alias[i])==PS_OK);
    double zero[9]={0};ps_series rest;
    CHECK(ps_series_aligned_values(c,x,zero,9,PS_RADIAN,"rest",&rest)==PS_OK);
    CHECK(ps_series_positive_peaks(c,rest,x,out)==PS_OK && !check(c,out,0));
    for(unsigned i=0;i<3;i++)CHECK(ps_series_release(c,out[i])==PS_OK);
    double tx[520],sy[520]={0};for(unsigned i=0;i<520;i++)tx[i]=i;
    sy[511]=sy[512]=sy[513]=2;ps_series bx,by;
    CHECK(ps_series_from_values(c,tx,520,PS_SECOND,"block time",&bx)==PS_OK);
    CHECK(ps_series_aligned_values(c,bx,sy,520,PS_RADIAN,"block signal",&by)==PS_OK);
    CHECK(ps_series_positive_peaks(c,by,bx,out)==PS_OK && !check(c,out,1) && value(c,out[0],0)==512);
    for(unsigned i=0;i<3;i++)CHECK(ps_series_release(c,out[i])==PS_OK);
    double extreme[]={-DBL_MAX,-DBL_MAX/2,DBL_MAX/2,DBL_MAX},plateau[]={0,2,2,0};ps_series ex,ey;
    CHECK(ps_series_from_values(c,extreme,4,PS_SECOND,"extreme time",&ex)==PS_OK);
    CHECK(ps_series_aligned_values(c,ex,plateau,4,PS_RADIAN,"plateau",&ey)==PS_OK);
    CHECK(ps_series_positive_peaks(c,ey,ex,out)==PS_OK && !check(c,out,1) && value(c,out[0],0)==0);
    ps_analysis_destroy(c);
    /* Inputs consume 144 bytes; two peaks need 48 more, beyond this quota. */
    CHECK(ps_analysis_create(argv[1],191,&c)==PS_OK);
    CHECK(ps_series_from_values(c,times,9,PS_SECOND,"t",&x)==PS_OK &&
          ps_series_aligned_values(c,x,signal,9,PS_RADIAN,"a",&y)==PS_OK);
    memcpy(out,saved,sizeof out);used=ps_analysis_scratch_bytes(c);
    CHECK(ps_series_positive_peaks(c,y,x,out)==PS_LIMIT && !memcmp(out,saved,sizeof out) && ps_analysis_scratch_bytes(c)==used);
    ps_analysis_destroy(c);
    puts("Positive peaks: flat tops, block boundaries, missing segments, strict time, empty results, aliases and quota passed");
    return 0;
}
