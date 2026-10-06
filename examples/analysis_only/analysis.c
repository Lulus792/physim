#include "physim/report.h"
#include <stdio.h>
#include <string.h>
/* An editable preview: first two channels per input, at most 16 plots. Scatter
 * points retain original timestamps and never bridge missing sensor readings. */
static ps_result analyze_many(const char *const *inputs,size_t count,const char *prefix) {
    if(!inputs || !count || count>PS_ANALYSIS_MAX_INPUTS || !prefix)return PS_INVALID;
    ps_analysis_context *ctx=NULL;ps_report *report=NULL;
    ps_result result=ps_analysis_create(prefix,0,&ctx);bool recovered=false;
    if(result==PS_OK)result=ps_report_create("First two channels per input run",
        "First two channels of each input; original time axes; only sensor status=1.\n"
        "Editable C17 analysis; source and input hashes are recorded by the runner.",&report);
    for(size_t input=0;input<count && result==PS_OK;input++) {
        ps_dataset run={0};result=ps_analysis_open_run(ctx,inputs[input],&run);
        if(result==PS_RECOVERED){recovered=true;result=PS_OK;}
        ps_dataset_info info;
        if(result==PS_OK)result=ps_dataset_describe(ctx,run,&info);
        ps_series time;
        if(result==PS_OK)result=ps_dataset_series(ctx,run,"time",&time);
        for(uint32_t channel=0;result==PS_OK && channel<info.channel_count && channel<2;channel++) {
            ps_series columns[2]={time},selected[2];
            result=ps_dataset_series(ctx,run,info.channels[channel].name,&columns[1]);
            int status=-1;
            if(result==PS_OK)result=ps_channel_status_index(info.channels,info.channel_count,channel,&status);
            if(result==PS_OK && status>=0) {
                ps_series selector;result=ps_dataset_series(ctx,run,info.channels[status].name,&selector);
                if(result==PS_OK)result=ps_series_select(ctx,columns,2,selector,1,selected);
                if(result==PS_OK)memcpy(columns,selected,sizeof columns);
            }
            ps_series_info values;
            if(result==PS_OK)result=ps_series_describe(ctx,columns[1],&values);
            if(result==PS_OK && values.count) {
                ps_plot_info plot={0};snprintf(plot.title,sizeof plot.title,"Run %zu · %s",input+1,info.channels[channel].name);
                strcpy(plot.x_label,"time");snprintf(plot.y_label,sizeof plot.y_label,"%s",info.channels[channel].name);
                ps_report_unit_from(PS_SECOND,&plot.x_unit);
                memcpy(plot.y_unit.dimension,values.dimension,7);plot.y_unit.scale=values.scale;
                snprintf(plot.y_unit.symbol,sizeof plot.y_unit.symbol,"%s",values.symbol);
                ps_plot_handle handle;result=ps_report_add_plot(report,&plot,&handle);
                ps_series first[2];
                if(result==PS_OK)result=ps_series_slice(ctx,columns[0],0,1,&first[0]);
                if(result==PS_OK)result=ps_series_slice(ctx,columns[1],0,1,&first[1]);
                if(result==PS_OK)result=ps_report_add_series(report,handle,ctx,first[0],first[1],"First valid sample",PS_PLOT_LINE);
                if(result==PS_OK)result=ps_report_add_series(report,handle,ctx,columns[0],columns[1],"Measurements",PS_PLOT_SCATTER);
            }
        }
        if(run.owner)ps_dataset_close(ctx,run);
    }
    char path[4096];int n=snprintf(path,sizeof path,"%s.psreport",prefix);
    if(result==PS_OK)result=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_report_save(report,path);
    ps_report_destroy(report);ps_analysis_destroy(ctx);
    return result==PS_OK && recovered?PS_RECOVERED:result;
}
static ps_result analyze(const char *input,const char *prefix) {return analyze_many(&input,1,prefix);}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api={sizeof api,PS_ABI_VERSION,"Saved run preview",analyze,analyze_many};return &api;
}
