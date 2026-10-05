#include "physim/report.h"
#include <stdio.h>
#include <string.h>
static ps_result analyze(const char *input,const char *prefix) {
    (void)input;
    ps_analysis_context *ctx=NULL;ps_report *report=NULL;
    const double x_values[]={0,1,2},y_values[]={0,1,4};double grid[129];
    for(unsigned i=0;i<129;i++)grid[i]=i/64.0;
    ps_series x,y,q,cubic,linear;ps_plot_handle plot;
    ps_result r=ps_analysis_create(prefix,0,&ctx);
    if(r==PS_OK)r=ps_series_from_values(ctx,x_values,3,PS_SECOND,"time",&x);
    if(r==PS_OK)r=ps_series_aligned_values(ctx,x,y_values,3,PS_METRE,"square",&y);
    if(r==PS_OK)r=ps_series_from_values(ctx,grid,129,PS_SECOND,"grid",&q);
    if(r==PS_OK)r=ps_series_resample(ctx,y,x,q,PS_RESAMPLE_PCHIP,&cubic);
    if(r==PS_OK)r=ps_series_resample(ctx,y,x,q,PS_RESAMPLE_LINEAR,&linear);
    if(r==PS_OK)r=ps_report_create("PCHIP comparison","kind=pchip-reference",&report);
    ps_plot_info info={0};strcpy(info.title,"PCHIP comparison");strcpy(info.x_label,"grid");strcpy(info.y_label,"pchip(square)");
    ps_report_unit_from(PS_SECOND,&info.x_unit);ps_report_unit_from(PS_METRE,&info.y_unit);
    if(r==PS_OK)r=ps_report_add_plot(report,&info,&plot);
    if(r==PS_OK)r=ps_report_add_series(report,plot,ctx,q,cubic,"PCHIP",PS_PLOT_LINE);
    if(r==PS_OK)r=ps_report_add_series(report,plot,ctx,q,linear,"linear",PS_PLOT_LINE);
    char path[4096];int n=snprintf(path,sizeof path,"%s.psreport",prefix);
    if(r==PS_OK)r=n<0 || n>=(int)sizeof path?PS_LIMIT:ps_report_save(report,path);
    ps_report_destroy(report);ps_analysis_destroy(ctx);return r;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api={sizeof(ps_analysis_api),PS_ABI_VERSION,"PCHIP comparison",analyze,NULL};
    return &api;
}
