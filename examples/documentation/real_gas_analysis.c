#include "physim/units.h"
#include "physim/report.h"
#include <stdio.h>
static ps_result analyze(const char *input,const char *prefix) {
    ps_analysis_context *ctx=NULL;ps_dataset dataset;ps_series volume,pressure;
    ps_result r=ps_analysis_create(prefix,0,&ctx);
    if(r==PS_OK)r=ps_analysis_open_run(ctx,input,&dataset);
    if(r==PS_OK || r==PS_RECOVERED)r=ps_dataset_series(ctx,dataset,"volume",&volume);
    ps_report *report=NULL;
    if(r==PS_OK)r=ps_report_create("Real gas comparison","Synthetic coefficients; homogeneous isothermal algebra; no phase coexistence",&report);
    const char *names[]={"ideal_pressure","vdw_pressure"};
    for(unsigned i=0;i<2 && r==PS_OK;i++) {
        ps_plot_handle plot;ps_plot_info info={0};
        snprintf(info.title,sizeof info.title,"%s",names[i]);
        ps_unit unit={{3,0,0,0,0,0,0},1,"m3"};ps_report_unit_from(unit,&info.x_unit);ps_report_unit_from(PS_PASCAL,&info.y_unit);
        r=ps_dataset_series(ctx,dataset,names[i],&pressure);
        if(r==PS_OK)r=ps_report_add_plot(report,&info,&plot);
        if(r==PS_OK)r=ps_report_add_series(report,plot,ctx,volume,pressure,names[i],PS_PLOT_LINE);
    }
    char path[4096];snprintf(path,sizeof path,"%s.psreport",prefix);
    if(r==PS_OK)r=ps_report_save(report,path);
    ps_report_destroy(report);ps_analysis_destroy(ctx);return r;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,.name="Real gas analysis",.run=analyze};return &api;
}
