#include "physim/series.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"Series numeric %d: %s\n",__LINE__,#x); return 1; } } while (0)
static int pair(ps_analysis_context *c, const double *xx, const double *yy, size_t n,
                ps_series *x, ps_series *y) {
    CHECK(ps_series_from_values(c,xx,n,PS_SECOND,"axis",x)==PS_OK);
    CHECK(ps_series_aligned_values(c,*x,yy,n,PS_METRE,"signal",y)==PS_OK);
    return 0;
}
static int read_pair(ps_analysis_context *c, ps_series s, double a, double b) {
    double v[2];size_t got=0;
    CHECK(ps_series_read(c,s,0,v,2,&got)==PS_OK && got==2);
    CHECK(v[0]==a && fabs(v[1]/b-1)<2e-15);
    return 0;
}
int main(int argc,char **argv) {
    /* Independent polynomial oracles and order checks for the shared contract. */
    double central_error[2],edge_error[2],area_error[2];
    for(unsigned pass=0;pass<2;pass++) {
        size_t n=pass?33:17;double ax[33],cubic[33],quadratic[33],derivative[33];
        for(size_t i=0;i<n;i++) { ax[i]=(double)i/(double)(n-1);
            cubic[i]=ax[i]*ax[i]*ax[i];quadratic[i]=ax[i]*ax[i]; }
        CHECK(ps_derivative(ax,cubic,n,derivative)==PS_OK);
        central_error[pass]=fabs(derivative[n/2]-.75);
        edge_error[pass]=fabs(derivative[n-1]-3);
        area_error[pass]=fabs(ps_trapezoid(ax,quadratic,n)-1.0/3.0);
    }
    CHECK(fabs(central_error[0]/central_error[1]-4)<1e-11);
    CHECK(edge_error[0]/edge_error[1]>1.97 && edge_error[0]/edge_error[1]<2);
    CHECK(fabs(area_error[0]/area_error[1]-4)<1e-10);
    double nonuniform[3];
    CHECK(ps_derivative((double[]){0,1,3},(double[]){0,1,9},3,nonuniform)==PS_OK);
    CHECK(nonuniform[0]==1 && nonuniform[1]==3 && nonuniform[2]==4);
    CHECK(argc==2);ps_analysis_context *c=NULL;
    CHECK(ps_analysis_create(argv[1],1024*1024,&c)==PS_OK);
    ps_series x,y,out;ps_unit area;
    CHECK(ps_unit_multiply(PS_METRE,PS_SECOND,NULL,&area)==PS_OK);
    CHECK(!pair(c,(double[]){-1e308,1e308},(double[]){-1e308,1e308},2,&x,&y));
    CHECK(ps_series_derivative(c,y,x,&out)==PS_OK);
    CHECK(!read_pair(c,out,1,1));CHECK(ps_series_release(c,out)==PS_OK);
    /* The width exceeds DBL_MAX but the integral is exactly 2. */
    CHECK(ps_series_release(c,y)==PS_OK);
    CHECK(ps_series_aligned_values(c,x,(double[]){1e-308,1e-308},2,PS_METRE,"small",&y)==PS_OK);
    CHECK(ps_series_integral(c,y,x,(ps_quantity){0,area},&out)==PS_OK);
    CHECK(!read_pair(c,out,0,2));CHECK(ps_series_release(c,out)==PS_OK);
    CHECK(ps_series_release(c,x)==PS_OK && ps_series_release(c,y)==PS_OK);
    /* Half of the least subnormal rounds to zero; the area is still finite. */
    CHECK(!pair(c,(double[]){0,1e308},(double[]){DBL_TRUE_MIN,DBL_TRUE_MIN},2,&x,&y));
    CHECK(ps_series_integral(c,y,x,(ps_quantity){0,area},&out)==PS_OK);
    CHECK(!read_pair(c,out,0,DBL_TRUE_MIN*1e308));CHECK(ps_series_release(c,out)==PS_OK);
    CHECK(ps_series_release(c,x)==PS_OK && ps_series_release(c,y)==PS_OK);
    /* Genuine overflow must roll back both handle and scratch storage. */
    CHECK(!pair(c,(double[]){0,DBL_MIN},(double[]){-DBL_MAX,DBL_MAX},2,&x,&y));
    out=x;uint64_t bytes=ps_analysis_scratch_bytes(c);
    CHECK(ps_series_derivative(c,y,x,&out)==PS_NUMERIC && out.slot==x.slot &&
          out.generation==x.generation && ps_analysis_scratch_bytes(c)==bytes);
    CHECK(ps_series_release(c,x)==PS_OK && ps_series_release(c,y)==PS_OK);
    CHECK(!pair(c,(double[]){0,2},(double[]){DBL_MAX,DBL_MAX},2,&x,&y));
    out=x;bytes=ps_analysis_scratch_bytes(c);
    CHECK(ps_series_integral(c,y,x,(ps_quantity){0,area},&out)==PS_NUMERIC &&
          out.slot==x.slot && out.generation==x.generation && ps_analysis_scratch_bytes(c)==bytes);
    CHECK(ps_series_release(c,x)==PS_OK && ps_series_release(c,y)==PS_OK);
    /* Cancellation of endpoint values must not overflow before multiplication. */
    CHECK(!pair(c,(double[]){-1e308,1e308},(double[]){-DBL_MAX,DBL_MAX},2,&x,&y));
    CHECK(ps_series_integral(c,y,x,(ps_quantity){7,area},&out)==PS_OK);
    double zero_area[2];size_t got;
    CHECK(ps_series_read(c,out,0,zero_area,2,&got)==PS_OK && got==2 &&
          zero_area[0]==7 && zero_area[1]==7);
    CHECK(ps_series_release(c,out)==PS_OK);
    CHECK(ps_series_release(c,x)==PS_OK && ps_series_release(c,y)==PS_OK);
    /* The central difference straddles a streaming block and overflows in both
     * numerator and denominator. The exact slope of y=x is one everywhere. */
    double axis[513],signal[513],flags[513];
    for (size_t i=0;i<513;i++) {
        axis[i]=i<256 ? -1e308+(double)i*4e292 : 1e308+(double)(i-256)*4e292;
        signal[i]=axis[i];flags[i]=1;
    }
    CHECK(!pair(c,axis,signal,513,&x,&y));
    CHECK(ps_series_derivative(c,y,x,&out)==PS_OK);
    double block[513];CHECK(ps_series_read(c,out,0,block,513,&got)==PS_OK && got==513);
    for(size_t i=0;i<got;i++)CHECK(block[i]==1);
    CHECK(ps_series_release(c,out)==PS_OK);
    /* Masked central secants use only consecutive valid neighbors. */
    flags[255]=0;ps_series selector,masked;
    CHECK(ps_series_aligned_values(c,x,flags,513,PS_ONE,"status",&selector)==PS_OK);
    CHECK(ps_series_mask(c,y,selector,1,&masked)==PS_OK);
    CHECK(ps_series_derivative(c,masked,x,&out)==PS_OK);
    uint8_t block_mask[513];
    CHECK(ps_series_read_masked(c,out,0,block,block_mask,513,&got)==PS_OK && got==513);
    for(size_t i=0;i<got;i++)CHECK(block_mask[i]==(i!=255) && (!block_mask[i] || block[i]==1));
    CHECK(ps_series_release(c,out)==PS_OK);
    CHECK(ps_series_release(c,masked)==PS_OK);
    CHECK(ps_series_release(c,selector)==PS_OK);
    CHECK(ps_series_release(c,x)==PS_OK && ps_series_release(c,y)==PS_OK);
    /* A missing value makes all later integral values unknown. */
    CHECK(!pair(c,(double[]){-1e308,0,1e308},(double[]){1e-308,1e-308,1e-308},3,&x,&y));
    CHECK(ps_series_aligned_values(c,x,(double[]){0,1,1},3,PS_ONE,"status",&selector)==PS_OK);
    CHECK(ps_series_mask(c,y,selector,1,&masked)==PS_OK);
    CHECK(ps_series_integral(c,masked,x,(ps_quantity){0,area},&out)==PS_OK);
    uint8_t valid[3];double missing[3];
    CHECK(ps_series_read_masked(c,out,0,missing,valid,3,&got)==PS_OK && got==3);
    CHECK(!valid[0] && !valid[1] && !valid[2]);
    ps_analysis_destroy(c);
    puts("Series scaled derivative, integral, subnormal and rollback checks passed");return 0;
}
