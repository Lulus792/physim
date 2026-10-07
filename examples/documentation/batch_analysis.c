#include "physim/analysis.h"
#include "physim/report.h"
#include "physim/units.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static ps_result legacy(const char *input,const char *prefix){(void)input;(void)prefix;return PS_VERSION;}
static int compare(const void *a,const void *b){double x=*(const double *)a,y=*(const double *)b;return (x>y)-(x<y);}
static double quantile(const double *values,size_t count,double p){double at=(count-1)*p;size_t lower=(size_t)at;if(lower==count-1)return values[lower];return (1-(at-lower))*values[lower]+(at-lower)*values[lower+1];}
static ps_result analyze(const char *const *inputs,size_t count,const char *prefix,const ps_analysis_services *services,ps_diagnostic *diagnostic){
    if(diagnostic)ps_diagnostic_clear(diagnostic);
    if(!inputs || count!=1 || !services || services->struct_size<offsetof(ps_analysis_services,resume_batch) || services->version!=PS_ANALYSIS_SERVICES_VERSION || !services->run_batch)return PS_VERSION;
    ps_batch_options options={0};ps_batch_result result={0};
    snprintf(options.module,sizeof options.module,"%s",inputs[0]);
    int n=snprintf(options.directory,sizeof options.directory,"%s-series",prefix);if(n<0 || (size_t)n>=sizeof options.directory)return PS_LIMIT;
    strcpy(options.channel,"position.x");options.runs=256;options.steps=200;options.dt=.005;options.seed=42;options.workers=4;options.timeout_s=30;
    ps_result r=services->run_batch(services->user,&options,0,&result);
    if(r!=PS_OK || result.cancelled || result.completed!=256 || result.valid!=256)return r==PS_OK?PS_INVALID:r;
    ps_analysis_context *context=NULL;ps_report *report=NULL;
    r=ps_analysis_create(prefix,0,&context);if(r==PS_OK)r=ps_report_create("Archived Monte Carlo from C","C17 Batch host service",&report);
    double values[256];for(unsigned i=0;i<256;i++)values[i]=result.values[i];
    ps_unit unit={.scale=1,.symbol=result.channel.unit};memcpy(unit.dimension,result.channel.dimension,sizeof unit.dimension);
    ps_series endpoints;if(r==PS_OK)r=ps_series_from_values(context,values,256,unit,"position.x",&endpoints);
    ps_plot_handle plot;if(r==PS_OK)r=ps_report_add_histogram(report,context,endpoints,"Final horizontal positions","value",16,&plot);
    ps_statistics stats={0};if(r==PS_OK)r=ps_series_statistics(context,endpoints,&stats);
    qsort(values,256,sizeof *values,compare);
    ps_table_info info={0};strcpy(info.title,"Final values");info.columns=6;
    const char *labels[]={"Runs","Mean","Stddev","Median","Lower 2.5%","Upper 97.5%"};
    for(unsigned i=0;r==PS_OK && i<6;i++){strcpy(info.column[i].label,labels[i]);r=ps_report_unit_from(i?unit:PS_ONE,&info.column[i].unit);}
    ps_table_handle table;if(r==PS_OK)r=ps_report_add_table(report,&info,&table);
    ps_table_row row={0};strcpy(row.label,"seeded ensemble");double summary[]={256,stats.mean,ps_statistics_stddev(&stats),quantile(values,256,.5),quantile(values,256,.025),quantile(values,256,.975)};memcpy(row.values,summary,sizeof summary);
    if(r==PS_OK)r=ps_report_add_row(report,table,&row);
    char path[4096];n=snprintf(path,sizeof path,"%s.psreport",prefix);
    if(r==PS_OK)r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_report_save(report,path);
    ps_report_destroy(report);ps_analysis_destroy(context);return r;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void){
    static const ps_analysis_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,.name="Archived Monte Carlo from C",.run=legacy,.run_host=analyze};return &api;
}
