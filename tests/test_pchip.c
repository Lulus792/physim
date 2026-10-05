#include "physim/series.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"PCHIP line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static ps_result interpolate(ps_analysis_context *c,const double *x,const double *y,size_t n,
                              const double *q,size_t m,ps_unit axis,ps_unit target,ps_series *result) {
    ps_series xx,yy,qq;ps_result r=ps_series_from_values(c,x,n,axis,"axis",&xx);
    if(r==PS_OK)r=ps_series_aligned_values(c,xx,y,n,PS_METRE,"value",&yy);
    if(r==PS_OK)r=ps_series_from_values(c,q,m,target,"target",&qq);
    if(r==PS_OK)r=ps_series_resample(c,yy,xx,qq,PS_RESAMPLE_PCHIP,result);
    if(r==PS_OK)r=ps_series_aligned(c,qq,*result);
    return r;
}
int main(void) {
    ps_analysis_context *c=NULL;CHECK(ps_analysis_create("pchip-work",0,&c)==PS_OK);
    double x[]={0,1,2},y[]={0,1,4},q[]={0,.5,1,1.5,2},values[4097];size_t got;
    ps_series out;
    CHECK(interpolate(c,x,y,3,q,5,PS_SECOND,PS_SECOND,&out)==PS_OK);
    CHECK(ps_series_read(c,out,0,values,5,&got)==PS_OK && got==5);
    const double expected[]={0,.3125,1,2.1875,4};
    for(unsigned i=0;i<5;i++)CHECK(fabs(values[i]-expected[i])<2e-15);
    double around[]={1-1e-6,1,1+1e-6};
    CHECK(interpolate(c,x,y,3,around,3,PS_SECOND,PS_SECOND,&out)==PS_OK);
    CHECK(ps_series_read(c,out,0,values,3,&got)==PS_OK && got==3 &&
          fabs((values[1]-values[0])/1e-6-1.5)<1e-5 &&
          fabs((values[2]-values[1])/1e-6-1.5)<1e-5);
    ps_series_info info;CHECK(ps_series_describe(c,out,&info)==PS_OK && info.dimension[0]==1 && info.scale==1 && !strcmp(info.name,"pchip(value)"));
    ps_analysis_destroy(c);CHECK(ps_analysis_create("pchip-units",0,&c)==PS_OK);
    double nx[]={0,1000,4000},ny[]={0,2,3},nq[]={0,.5,1,2.5,4};ps_unit ms=PS_SECOND;ms.scale=.001;
    CHECK(interpolate(c,nx,ny,3,nq,5,ms,PS_SECOND,&out)==PS_OK);
    CHECK(ps_series_read(c,out,0,values,5,&got)==PS_OK && got==5);
    CHECK(values[0]==0 && values[2]==2 && values[4]==3);
    CHECK(fabs(values[1]-4337.0/3552)<2e-14 && fabs(values[3]-203.0/74)<2e-14);
    ps_analysis_destroy(c);CHECK(ps_analysis_create("pchip-shape",0,&c)==PS_OK);
    double sx[17],sy[17],sq[4097];
    for(unsigned i=0;i<17;i++){sx[i]=i*i*.125;sy[i]=i<5?0:i<10?(double)(i-4):5;}
    for(unsigned i=0;i<4097;i++)sq[i]=sx[16]*i/4096;
    CHECK(interpolate(c,sx,sy,17,sq,4097,PS_SECOND,PS_SECOND,&out)==PS_OK);
    CHECK(ps_series_read(c,out,0,values,4097,&got)==PS_OK && got==4097);
    for(unsigned i=0,k=0;i<4097;i++) {
        while(k<15 && sq[i]>sx[k+1])k++;
        CHECK(values[i]>=sy[k] && values[i]<=sy[k+1] && (!i || values[i]>=values[i-1]));
    }
    /* Decreasing data and extrema must not acquire new peaks or undershoots. */
    for(unsigned i=0;i<17;i++)sy[i]=-sy[i];
    CHECK(interpolate(c,sx,sy,17,sq,4097,PS_SECOND,PS_SECOND,&out)==PS_OK);
    CHECK(ps_series_read(c,out,0,values,4097,&got)==PS_OK && got==4097);
    for(unsigned i=1;i<4097;i++)CHECK(values[i]<=values[i-1]);
    for(unsigned i=0;i<17;i++)sy[i]=i%2?-10:10;
    CHECK(interpolate(c,sx,sy,17,sq,4097,PS_SECOND,PS_SECOND,&out)==PS_OK);
    CHECK(ps_series_read(c,out,0,values,4097,&got)==PS_OK && got==4097);
    for(unsigned i=0;i<4097;i++)CHECK(values[i]>=-10 && values[i]<=10);
    ps_analysis_destroy(c);CHECK(ps_analysis_create("pchip-blocks",0,&c)==PS_OK);
    double bx[769],by[769],bq[1537];
    for(unsigned i=0;i<769;i++){bx[i]=i*.5;by[i]=3*bx[i]-2;}
    for(unsigned i=0;i<1537;i++)bq[i]=i*.25;
    CHECK(interpolate(c,bx,by,769,bq,1537,PS_SECOND,PS_SECOND,&out)==PS_OK);
    CHECK(ps_series_read(c,out,0,values,1537,&got)==PS_OK && got==1537);
    for(unsigned i=0;i<1537;i++)CHECK(fabs(values[i]-(3*bq[i]-2))<3e-13);
    ps_analysis_destroy(c);CHECK(ps_analysis_create("pchip-failures",0,&c)==PS_OK);
    ps_series xx,yy,qq,keep={0};CHECK(ps_series_from_values(c,bx,769,PS_SECOND,"x",&xx)==PS_OK);
    CHECK(ps_series_aligned_values(c,xx,by,769,PS_METRE,"y",&yy)==PS_OK);
    bq[1536]+=1;CHECK(ps_series_from_values(c,bq,1537,PS_SECOND,"q",&qq)==PS_OK);
    uint64_t bytes=ps_analysis_scratch_bytes(c);
    CHECK(ps_series_resample(c,yy,xx,qq,PS_RESAMPLE_PCHIP,&keep)==PS_INVALID && !keep.owner && ps_analysis_scratch_bytes(c)==bytes);
    bx[700]=0;CHECK(ps_series_from_values(c,bx,769,PS_SECOND,"bad x",&xx)==PS_OK);
    CHECK(ps_series_aligned_values(c,xx,by,769,PS_METRE,"y",&yy)==PS_OK);
    CHECK(ps_series_from_values(c,q,1,PS_SECOND,"prefix",&qq)==PS_OK);bytes=ps_analysis_scratch_bytes(c);
    CHECK(ps_series_resample(c,yy,xx,qq,PS_RESAMPLE_PCHIP,&keep)==PS_INVALID && !keep.owner && ps_analysis_scratch_bytes(c)==bytes);
    ps_analysis_destroy(c);
    CHECK(ps_analysis_create("pchip-quota",(769*2+1537)*8,&c)==PS_OK);bx[700]=350;bq[1536]-=1;
    CHECK(interpolate(c,bx,by,769,bq,1537,PS_SECOND,PS_SECOND,&keep)==PS_LIMIT && !keep.owner);
    CHECK(ps_analysis_scratch_bytes(c)==(769*2+1537)*8);ps_analysis_destroy(c);
    CHECK(ps_analysis_create("pchip-extremes",0,&c)==PS_OK);
    double ex[]={-DBL_MAX,0,DBL_MAX},ey[]={-DBL_MAX,0,DBL_MAX},eq[]={-DBL_MAX,-DBL_MAX/2,0,DBL_MAX/2,DBL_MAX};
    CHECK(interpolate(c,ex,ey,3,eq,5,PS_SECOND,PS_SECOND,&out)==PS_OK);
    CHECK(ps_series_read(c,out,0,values,5,&got)==PS_OK && got==5);
    for(unsigned i=0;i<5;i++)CHECK(isfinite(values[i]) && fabs(values[i]/DBL_MAX-eq[i]/DBL_MAX)<8e-16);
    double tiny[]={0,DBL_TRUE_MIN,2*DBL_TRUE_MIN},large[]={0,DBL_MAX/2,DBL_MAX},tq[]={0,DBL_TRUE_MIN,2*DBL_TRUE_MIN};
    CHECK(interpolate(c,tiny,large,3,tq,3,PS_SECOND,PS_SECOND,&out)==PS_OK);
    CHECK(ps_series_read(c,out,0,values,3,&got)==PS_OK && got==3 && values[1]==large[1]);
    double mixx[]={0,DBL_TRUE_MIN,DBL_MAX},mixy[]={0,1,2},mixq[]={0,DBL_TRUE_MIN,DBL_MAX/2,DBL_MAX};
    CHECK(interpolate(c,mixx,mixy,3,mixq,4,PS_SECOND,PS_SECOND,&out)==PS_OK);
    CHECK(ps_series_read(c,out,0,values,4,&got)==PS_OK && got==4 && values[2]>=1 && values[2]<=2);
    double two[]={-DBL_MAX,DBL_MAX},middle[]={0};
    CHECK(interpolate(c,two,two,2,middle,1,PS_SECOND,PS_SECOND,&out)==PS_OK);
    CHECK(ps_series_read(c,out,0,values,1,&got)==PS_OK && values[0]==0);
    double one[]={-0.0};CHECK(interpolate(c,one,one,1,one,1,PS_SECOND,PS_SECOND,&out)==PS_OK);
    CHECK(ps_series_read(c,out,0,values,1,&got)==PS_OK && signbit(values[0]));
    ps_analysis_destroy(c);puts("PCHIP: analytic/nonuniform/unit references, shape, blocks, finite extremes and transactional failures passed");return 0;
}
