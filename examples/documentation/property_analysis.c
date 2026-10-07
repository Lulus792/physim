#include "physim/report.h"
#include <stdio.h>
static ps_result analyze(const char *input,const char *prefix) {
    ps_analysis_context *ctx=NULL;ps_dataset dataset;ps_series time,density;
    ps_result r=ps_analysis_create(prefix,0,&ctx);
    if(r==PS_OK)r=ps_analysis_open_run(ctx,input,&dataset);
    if(r==PS_OK || r==PS_RECOVERED)r=ps_dataset_series(ctx,dataset,"time",&time);
    if(r==PS_OK)r=ps_dataset_series(ctx,dataset,"density",&density);
    ps_report *report=NULL;ps_plot_handle plot;ps_plot_info info={0};
    snprintf(info.title,sizeof info.title,"Synthetic density sweep");
    ps_unit unit={{-3,1,0,0,0,0,0},1,"kg/m3"};ps_report_unit_from(PS_SECOND,&info.x_unit);ps_report_unit_from(unit,&info.y_unit);
    if(r==PS_OK)r=ps_report_create("Material property","Synthetic affine reference; bilinear interpolation; bounded SI domain",&report);
    if(r==PS_OK)r=ps_report_add_plot(report,&info,&plot);
    if(r==PS_OK)r=ps_report_add_series(report,plot,ctx,time,density,"density",PS_PLOT_LINE);
    char path[4096];snprintf(path,sizeof path,"%s.psreport",prefix);
    if(r==PS_OK)r=ps_report_save(report,path);
    ps_report_destroy(report);ps_analysis_destroy(ctx);return r;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,.name="Material property analysis",.run=analyze};return &api;
}
