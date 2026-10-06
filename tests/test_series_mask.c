#include "physim/report.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include "test_allocator.h"
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Mask line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static bool values(ps_analysis_context *c,ps_series s,const double *expected,const uint8_t *flags,size_t n) {
    double data[1024];uint8_t mask[1024];size_t got=0;
    if(ps_series_read_masked(c,s,0,data,mask,1024,&got)!=PS_OK || got!=n)return false;
    for(size_t i=0;i<n;i++)if(mask[i]!=flags[i] || (mask[i] && fabs(data[i]-expected[i])>1e-11*fmax(1,fabs(expected[i]))))return false;
    return true;
}
int main(int argc,char **argv) {
    CHECK(argc==2);ps_analysis_context *c=NULL;char prefix[4096];snprintf(prefix,sizeof prefix,"%s/mask",argv[1]);
    CHECK(ps_analysis_create(prefix,1024*1024,&c)==PS_OK);
    double xx[]={0,1,2,3,4,5,6,7},yy[]={0,1,DBL_MAX,9,16,DBL_MAX,36,49},ff[]={1,1,0,1,1,0,1,1};
    uint8_t valid[]={1,1,0,1,1,0,1,1};ps_series x,y,f,m,out;
    CHECK(ps_series_from_values(c,xx,8,PS_SECOND,"time",&x)==PS_OK);
    CHECK(ps_series_aligned_values(c,x,yy,8,PS_METRE,"signal",&y)==PS_OK);
    CHECK(ps_series_aligned_values(c,x,ff,8,PS_ONE,"status",&f)==PS_OK);
    uint64_t before=ps_analysis_scratch_bytes(c);CHECK(ps_series_mask(c,y,f,1,&m)==PS_OK);
    CHECK(ps_analysis_scratch_bytes(c)==before+72 && values(c,m,yy,valid,8));
    CHECK(ps_series_aligned(c,x,m)==PS_OK);bool masked=false;CHECK(ps_series_is_masked(c,m,&masked)==PS_OK && masked);
    ps_statistics stats;CHECK(ps_series_statistics(c,m,&stats)==PS_OK && stats.count==6 && fabs(stats.mean-18.5)<1e-12);
    double q=-1;CHECK(ps_series_quantile(c,m,.5,&q)==PS_OK && q==12.5);
    double affine[]={1,3,0,19,33,0,73,99};CHECK(ps_series_affine(c,m,2,(ps_quantity){1,PS_METRE},&out)==PS_OK && values(c,out,affine,valid,8));
    CHECK(ps_series_release(c,out)==PS_OK);
    CHECK(ps_series_affine(c,y,2,(ps_quantity){0,PS_METRE},&out)==PS_NUMERIC);
    double dy[]={1,1,0,7,7,0,13,13};CHECK(ps_series_derivative(c,m,x,&out)==PS_OK && values(c,out,dy,valid,8));CHECK(ps_series_release(c,out)==PS_OK);
    double integral[]={0,.5,0,0,0,0,0,0};uint8_t iv[]={1,1,0,0,0,0,0,0};
    ps_unit integral_unit;CHECK(ps_unit_multiply(PS_METRE,PS_SECOND,NULL,&integral_unit)==PS_OK);
    CHECK(ps_series_integral(c,m,x,(ps_quantity){0,integral_unit},&out)==PS_OK);
    CHECK(values(c,out,integral,iv,8));CHECK(ps_series_release(c,out)==PS_OK);
    double average[]={0,.5,0,9,12.5,0,36,42.5};CHECK(ps_series_moving_average(c,m,2,&out)==PS_OK && values(c,out,average,valid,8));CHECK(ps_series_release(c,out)==PS_OK);
    CHECK(ps_series_slice(c,m,1,5,&out)==PS_OK && values(c,out,yy+1,valid+1,5));CHECK(ps_series_release(c,out)==PS_OK);
    double denominator[]={1,1,0,1,1,0,1,1};ps_series d;
    CHECK(ps_series_aligned_values(c,x,denominator,8,PS_ONE,"denominator",&d)==PS_OK);
    CHECK(ps_series_combine(c,PS_SERIES_DIVIDE,m,d,&out)==PS_OK && values(c,out,yy,valid,8));CHECK(ps_series_release(c,out)==PS_OK);
    ps_series columns[]={x,m},selected[2],flags;
    CHECK(ps_series_validity(c,m,&flags)==PS_OK && ps_series_select(c,columns,2,flags,1,selected)==PS_OK);
    double compact[]={0,1,9,16,36,49};uint8_t all[]={1,1,1,1,1,1};CHECK(values(c,selected[1],compact,all,6));
    CHECK(ps_series_release(c,selected[0])==PS_OK && ps_series_release(c,selected[1])==PS_OK);
    double target_values[]={0,.5,1,1.5,2,2.5,3,3.5,4,4.5,5,5.5,6,6.5,7};ps_series target;
    CHECK(ps_series_from_values(c,target_values,15,PS_SECOND,"target",&target)==PS_OK);
    double interpolated[]={0,.5,1,0,0,0,9,12.5,16,0,0,0,36,42.5,49};uint8_t linear[]={1,1,1,0,0,0,1,1,1,0,0,0,1,1,1};
    for(unsigned method=0;method<4;method++) {
        CHECK(ps_series_resample(c,m,x,target,(ps_resample_method)method,&out)==PS_OK);
        if(method==PS_RESAMPLE_LINEAR || method==PS_RESAMPLE_PCHIP)CHECK(values(c,out,interpolated,linear,15));
        else {
            double data[15];uint8_t mask[15];size_t got;
            CHECK(ps_series_read_masked(c,out,0,data,mask,15,&got)==PS_OK && got==15);
            CHECK(mask[4]==0 && mask[5]==0 && mask[6]==1 && mask[10]==0);
        }
        CHECK(ps_series_release(c,out)==PS_OK);
    }
    /* Masked report keeps missing rows, masks survive serialization, and SVG
     * starts a new path segment rather than connecting through a missing row. */
    ps_report *r=NULL;CHECK(ps_report_create("Masked signal","fixture",&r)==PS_OK);
    ps_plot_info plot={0};strcpy(plot.title,"Signal");ps_report_unit_from(PS_SECOND,&plot.x_unit);ps_report_unit_from(PS_METRE,&plot.y_unit);
    ps_plot_handle handle;CHECK(ps_report_add_plot(r,&plot,&handle)==PS_OK);
    CHECK(ps_report_add_series(r,handle,c,x,m,"measurement",PS_PLOT_LINE)==PS_OK);
    const uint8_t *curve_mask=NULL;CHECK(ps_report_curve_mask(r,0,0,&curve_mask)==PS_OK && curve_mask && !curve_mask[2] && curve_mask[3]==3);
    double bounds[4];CHECK(ps_report_plot_bounds(r,0,bounds)==PS_OK && bounds[3]==49);
    char file[4608];snprintf(file,sizeof file,"%s/masked.psreport",argv[1]);CHECK(ps_report_save(r,file)==PS_OK);
    FILE *saved=fopen(file,"rb");CHECK(saved);unsigned char header[12];CHECK(fread(header,1,12,saved)==12 && header[8]==2 && !fclose(saved));
    ps_report *copy=NULL;CHECK(ps_report_load(file,&copy)==PS_OK);
    const uint8_t *copied=NULL;CHECK(ps_report_curve_mask(copy,0,0,&copied)==PS_OK && !memcmp(curve_mask,copied,8));
    snprintf(file,sizeof file,"%s/masked.svg",argv[1]);CHECK(ps_report_export_svg(copy,0,file)==PS_OK);
    snprintf(file,sizeof file,"%s/plot.csv",argv[1]);CHECK(ps_report_export_plot_csv(copy,0,file)==PS_OK);
    snprintf(file,sizeof file,"%s/series.csv",argv[1]);CHECK(ps_series_export_csv(c,columns,2,file)==PS_OK);
    /* CRC-valid malformed v2 masks and unknown versions leave outputs intact. */
    snprintf(file,sizeof file,"%s/masked.psreport",argv[1]);saved=fopen(file,"rb");CHECK(saved);
    unsigned char encoded[8192];size_t size=fread(encoded,1,sizeof encoded,saved);
    CHECK(!ferror(saved) && !fclose(saved) && size>32 && size<sizeof encoded && !memcmp(encoded+size-8,curve_mask,8));
    encoded[size-1]=2;ps_put_u32(encoded+16,ps_crc32(encoded+20,size-20));
    snprintf(file,sizeof file,"%s/bad-mask.psreport",argv[1]);saved=fopen(file,"wb");CHECK(saved && fwrite(encoded,1,size,saved)==size && !fclose(saved));
    ps_report *unchanged=r;CHECK(ps_report_load(file,&unchanged)==PS_CORRUPT && unchanged==r);
    ps_put_u32(encoded+8,3);snprintf(file,sizeof file,"%s/unknown-mask.psreport",argv[1]);saved=fopen(file,"wb");
    CHECK(saved && fwrite(encoded,1,size,saved)==size && !fclose(saved));
    CHECK(ps_report_load(file,&unchanged)==PS_VERSION && unchanged==r);
    ps_report_destroy(copy);ps_report_destroy(r);
    CHECK(ps_series_release(c,m)==PS_OK);CHECK(ps_series_aligned_values(c,y,ff,8,PS_ONE,"none",&out)==PS_OK);
    ps_series empty;CHECK(ps_series_mask(c,y,out,2,&empty)==PS_OK);stats.mean=123;q=123;
    CHECK(ps_series_statistics(c,empty,&stats)==PS_INVALID && stats.mean==123);
    CHECK(ps_series_quantile(c,empty,.5,&q)==PS_INVALID && q==123);
    ps_analysis_destroy(c);
    /* Block boundaries, independent ownership, and reduced previews with more
     * gaps than a plot can display. No retained segment may cross a source gap. */
    CHECK(ps_analysis_create(prefix,1024*1024,&c)==PS_OK);
    double axis[5000],signal[5000],selector[5000];
    for(unsigned i=0;i<5000;i++){axis[i]=i;signal[i]=i;selector[i]=i%3!=2;}
    CHECK(ps_series_from_values(c,axis,5000,PS_SECOND,"axis",&x)==PS_OK);
    CHECK(ps_series_aligned_values(c,x,signal,5000,PS_METRE,"linear",&y)==PS_OK);
    CHECK(ps_series_aligned_values(c,x,selector,5000,PS_ONE,"selector",&f)==PS_OK);
    CHECK(ps_series_mask(c,y,f,1,&m)==PS_OK && ps_series_release(c,y)==PS_OK && ps_series_release(c,f)==PS_OK);
    double block[1024];uint8_t block_valid[1024];size_t got;
    CHECK(ps_series_read_masked(c,m,250,block,block_valid,1024,&got)==PS_OK && got==1024);
    for(unsigned i=0;i<1024;i++)CHECK(block_valid[i]==((250+i)%3!=2) && block[i]==250+i);
    CHECK(ps_report_create("Dense gaps","fixture",&r)==PS_OK && ps_report_add_plot(r,&plot,&handle)==PS_OK);
    CHECK(ps_report_add_series(r,handle,c,x,m,"dense",PS_PLOT_LINE)==PS_OK);
    const ps_curve_data *dense=NULL;CHECK(ps_report_curve_view(r,0,0,&dense)==PS_OK && dense->count<=2048 && dense->source_count==5000);
    CHECK(ps_report_curve_mask(r,0,0,&curve_mask)==PS_OK && curve_mask);
    for(unsigned i=1;i<dense->count;i++)if((curve_mask[i]&1) && (curve_mask[i-1]&1) && !(curve_mask[i]&2))
        CHECK((uint64_t)dense->x[i]/3==(uint64_t)dense->x[i-1]/3);
    ps_report_destroy(r);ps_analysis_destroy(c);
    /* Allocation failure while adding a mask is atomic and frees both buffers. */
    test_allocator allocator={0};CHECK(ps_report_create_with_allocator("allocation","fixture",test_domain(&allocator),&r)==PS_OK);
    CHECK(ps_report_add_plot(r,&plot,&handle)==PS_OK);ps_curve_data curve={0};curve.count=curve.source_count=2;curve.kind=PS_PLOT_LINE;
    uint8_t bad[]={1,0};size_t live=allocator.live_bytes;
    for(unsigned fail=1;fail<=2;fail++) {
        allocator.fail_on=allocator.attempts+fail;CHECK(ps_report_add_curve_masked(r,handle,&curve,bad)==PS_MEMORY);
        CHECK(allocator.live_bytes==live);ps_plot_info untouched;CHECK(ps_report_plot_read(r,0,&untouched)==PS_OK && !untouched.curves);
    }
    allocator.fail_on=0;CHECK(ps_report_add_curve_masked(r,handle,&curve,bad)==PS_OK);ps_report_destroy(r);
    CHECK(!allocator.invalid && !allocator.live_bytes && !allocator.live_blocks);
    /* Mask bytes are part of the quota; failure publishes neither a handle nor
     * scratch usage. Owned output survives release of both source handles. */
    CHECK(ps_analysis_create(prefix,8*8*2+8*9-1,&c)==PS_OK);
    CHECK(ps_series_from_values(c,xx,8,PS_SECOND,"root",&x)==PS_OK && ps_series_aligned_values(c,x,ff,8,PS_ONE,"flags",&f)==PS_OK);
    before=ps_analysis_scratch_bytes(c);out=x;CHECK(ps_series_mask(c,x,f,1,&out)==PS_LIMIT && out.slot==x.slot && ps_analysis_scratch_bytes(c)==before);ps_analysis_destroy(c);
    puts("Series masks: alignment, quota, numerics, resampling, statistics, CSV and report roundtrip passed");return 0;
}
