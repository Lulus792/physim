#include "physim/report.h"
#include <stdio.h>
#include <string.h>
static ps_result plot(ps_report *report,ps_analysis_context *ctx,ps_series x,ps_series y,
                       ps_unit unit,const char *title,const char *label,ps_plot_handle *out) {
    ps_plot_info info={0};snprintf(info.title,sizeof info.title,"%s",title);
    ps_series_info xi,yi;ps_series_describe(ctx,x,&xi);ps_series_describe(ctx,y,&yi);
    snprintf(info.x_label,sizeof info.x_label,"%s",xi.name);snprintf(info.y_label,sizeof info.y_label,"%s",yi.name);
    ps_report_unit_from(PS_SECOND,&info.x_unit);ps_report_unit_from(unit,&info.y_unit);
    ps_result r=ps_report_add_plot(report,&info,out);
    return r==PS_OK?ps_report_add_series(report,*out,ctx,x,y,label,PS_PLOT_LINE):r;
}
static ps_result analyze(const char *input,const char *prefix) {
    (void)input;ps_analysis_context *ctx=NULL;ps_report *report=NULL;
    double xx[]={0,1,2,3,4,5,6,7},yy[]={0,1,9999,9,16,9999,36,49},ff[]={1,1,0,1,1,0,1,1};
    double grid[]={0,.5,1,1.5,2,2.5,3,3.5,4,4.5,5,5.5,6,6.5,7};
    ps_series x,raw,status,y,derivative,q,resampled;ps_plot_handle handle;
    ps_result r=ps_analysis_create(prefix,0,&ctx);
    if(r==PS_OK)r=ps_series_from_values(ctx,xx,8,PS_SECOND,"time",&x);
    if(r==PS_OK)r=ps_series_aligned_values(ctx,x,yy,8,PS_METRE,"signal",&raw);
    if(r==PS_OK)r=ps_series_aligned_values(ctx,x,ff,8,PS_ONE,"status",&status);
    if(r==PS_OK)r=ps_series_mask(ctx,raw,status,1,&y);
    if(r==PS_OK)r=ps_series_derivative(ctx,y,x,&derivative);
    if(r==PS_OK)r=ps_series_from_values(ctx,grid,15,PS_SECOND,"grid",&q);
    if(r==PS_OK)r=ps_series_resample(ctx,y,x,q,PS_RESAMPLE_PCHIP,&resampled);
    if(r==PS_OK)r=ps_report_create("Masked measurement segments","Explicit sample masks; no interpolation through missing observations.",&report);
    if(r==PS_OK)r=plot(report,ctx,x,y,PS_METRE,"Masked signal","measurement",&handle);
    if(r==PS_OK)r=plot(report,ctx,x,derivative,PS_VELOCITY,"Derivative within segments","derivative",&handle);
    if(r==PS_OK)r=plot(report,ctx,q,resampled,PS_METRE,"PCHIP within segments","PCHIP",&handle);
    char path[4096];ps_series columns[]={x,y};
    snprintf(path,sizeof path,"%s-masked-data.csv",prefix);if(r==PS_OK)r=ps_series_export_csv(ctx,columns,2,path);
    snprintf(path,sizeof path,"%s.psreport",prefix);if(r==PS_OK)r=ps_report_save(report,path);
    ps_report_destroy(report);ps_analysis_destroy(ctx);return r;
}
static ps_result many(const char *const *inputs,size_t count,const char *prefix){(void)inputs;(void)count;return analyze(NULL,prefix);}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api={sizeof api,PS_ABI_VERSION,"Masked measurement segments",analyze,many};return &api;
}
