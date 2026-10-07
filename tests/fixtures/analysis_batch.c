#include "physim/analysis.h"
#include "physim/report.h"
#include "physim/units.h"
#include <stdio.h>
#include <string.h>
static bool suffix(const char *text,const char *end){size_t n=strlen(text),m=strlen(end);return n>=m && !memcmp(text+n-m,end,m);}
static ps_result old(const char *run,const char *prefix){(void)run;(void)prefix;return PS_VERSION;}
static ps_result analyze(const char *const *inputs,size_t count,const char *prefix,const ps_analysis_services *services,ps_diagnostic *diagnostic){
    ps_diagnostic_clear(diagnostic);
    if(count!=3 || !services || services->version!=PS_ANALYSIS_SERVICES_VERSION || services->struct_size<sizeof *services || !services->run_batch || !services->resume_batch)return PS_VERSION;
    ps_batch_options options={0};ps_batch_result result={0};const char *mode=inputs[1];
    options.runs=suffix(mode,"sweep")?3:6;options.steps=suffix(mode,"fixed")?7:100;options.dt=.1;options.seed=42;
    options.workers=suffix(mode,"partial")?1:4;options.timeout_s=15;options.end_time=suffix(mode,"fixed")?0:.7;
    options.adaptive=suffix(mode,"adaptive") || suffix(mode,"sweep");options.minimum_dt=.02;options.maximum_dt=.3;
    strcpy(options.channel,"position");snprintf(options.module,sizeof options.module,"%s",inputs[0]);snprintf(options.source,sizeof options.source,"%s",inputs[2]);
    snprintf(options.directory,sizeof options.directory,"%s%s",prefix,suffix(mode,"partial")?"-paused":"-series");
    if(suffix(mode,"existing")){
        snprintf(options.directory,sizeof options.directory,"%s",inputs[0]);char *slash=NULL;
        for(char *p=options.directory;*p;p++)if(*p=='/' || *p=='\\')slash=p;
        if(!slash)return PS_INVALID;*slash=0;
    }
    options.parameter_count=1;strcpy(options.parameters[0].name,"offset");options.parameters[0].value=.4;
    if(suffix(mode,"sweep")){options.sweep=true;strcpy(options.sweep_name,"velocity");options.sweep_start=.2;options.sweep_end=3;}
    else{options.parameter_count=2;strcpy(options.parameters[1].name,"velocity");options.parameters[1].value=suffix(mode,"error")?99:1.4;}
    ps_result r=PS_OK;
    if(suffix(mode,"resume")){
        char series[4096];snprintf(series,sizeof series,"%s",inputs[0]);char *slash=NULL;
        for(char *p=series;*p;p++)if(*p=='/' || *p=='\\')slash=p;
        if(!slash)return PS_INVALID;*slash=0;
        r=services->resume_batch(services->user,series,options.directory,&options);if(r!=PS_OK)return r;
    }
    ps_result code=services->run_batch(services->user,&options,suffix(mode,"partial")?2:0,&result);
    ps_analysis_context *context=NULL;ps_report *report=NULL;
    r=ps_analysis_create(prefix,0,&context);if(r==PS_OK)r=ps_report_create("Batch binding parity","C Batch services",&report);
    ps_table_info info={0};strcpy(info.title,"Counts");info.columns=6;
    const char *labels[]={"Completed","Started","Reused","Valid","Cancelled","Code"};
    for(unsigned i=0;r==PS_OK && i<6;i++){strcpy(info.column[i].label,labels[i]);r=ps_report_unit_from(PS_ONE,&info.column[i].unit);}
    ps_table_handle table;if(r==PS_OK)r=ps_report_add_table(report,&info,&table);
    ps_table_row row={0};strcpy(row.label,"result");double counters[]={result.completed,result.started,result.reused,result.valid,result.cancelled,code};memcpy(row.values,counters,sizeof counters);
    if(r==PS_OK)r=ps_report_add_row(report,table,&row);
    if(r==PS_OK && result.valid){
        double values[PS_BATCH_MAX_RUNS];size_t n=0;for(uint32_t i=0;i<options.runs;i++)if(result.endpoint_status[i]==1)values[n++]=result.values[i];
        ps_unit unit={.scale=1,.symbol=result.channel.unit};memcpy(unit.dimension,result.channel.dimension,sizeof unit.dimension);
        ps_series endpoints;r=ps_series_from_values(context,values,n,unit,options.channel,&endpoints);
        ps_plot_handle plot;if(r==PS_OK)r=ps_report_add_histogram(report,context,endpoints,"Endpoints","value",3,&plot);
        char csv[4096];snprintf(csv,sizeof csv,"%s-values.csv",prefix);if(r==PS_OK)r=ps_series_export_csv(context,&endpoints,1,csv);
    }
    char path[4096];snprintf(path,sizeof path,"%s.psreport",prefix);if(r==PS_OK)r=ps_report_save(report,path);
    ps_report_destroy(report);ps_analysis_destroy(context);return r;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void){
    static const ps_analysis_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,.name="C Batch service parity",.run=old,.run_host=analyze};return &api;
}
