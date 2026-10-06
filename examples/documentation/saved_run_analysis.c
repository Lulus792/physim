#include "physim/report.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
static ps_result at(ps_analysis_context *c,ps_series s,uint64_t i,double *out) {
    size_t n=0;ps_result r=ps_series_read(c,s,i,out,1,&n);return r==PS_OK && n!=1?PS_INVALID:r;
}
static ps_result plot(ps_report *report,ps_analysis_context *c,ps_series time,
    ps_series *series,const char *const *labels,unsigned count,const char *title,ps_unit unit) {
    ps_plot_info info={0};snprintf(info.title,sizeof info.title,"%s",title);
    strcpy(info.x_label,"Time");snprintf(info.y_label,sizeof info.y_label,"%s",title);
    ps_result r=ps_report_unit_from(PS_SECOND,&info.x_unit);
    if(r==PS_OK)r=ps_report_unit_from(unit,&info.y_unit);
    ps_plot_handle handle;if(r==PS_OK)r=ps_report_add_plot(report,&info,&handle);
    for(unsigned i=0;r==PS_OK && i<count;i++)r=ps_report_add_series(report,handle,c,time,series[i],labels[i],PS_PLOT_LINE);
    return r;
}
static ps_result analyze(const char *input,const char *prefix) {
    ps_analysis_context *c=NULL;ps_report *report=NULL;ps_dataset run={0};ps_dataset_info info={0};
    ps_series time={0},position={0},velocity={0},reconstructed={0},residual={0};
    bool recovered=false;ps_result r=ps_analysis_create(prefix,0,&c);
    if(r==PS_OK){r=ps_analysis_open_run(c,input,&run);recovered=r==PS_RECOVERED;if(recovered)r=PS_OK;}
    if(r==PS_OK)r=ps_dataset_describe(c,run,&info);
    if(r==PS_OK && info.samples<2)r=PS_INVALID;
    if(r==PS_OK)r=ps_dataset_series(c,run,"time",&time);
    if(r==PS_OK)r=ps_dataset_series(c,run,"position.x",&position);
    /* A zero offset checks the physical dimensions; run series use SI values. */
    if(r==PS_OK)r=ps_series_affine(c,time,1,(ps_quantity){0,PS_SECOND},&time);
    if(r==PS_OK)r=ps_series_affine(c,position,1,(ps_quantity){0,PS_METRE},&position);
    double start=0,end=0,first=0,last=0;
    if(r==PS_OK)r=at(c,time,0,&start);
    if(r==PS_OK)r=at(c,time,info.samples-1,&end);
    if(r==PS_OK)r=at(c,position,0,&first);
    if(r==PS_OK)r=at(c,position,info.samples-1,&last);
    if(r==PS_OK)r=ps_series_derivative(c,position,time,&velocity);
    if(r==PS_OK)r=ps_series_integral(c,velocity,time,(ps_quantity){first,PS_METRE},&reconstructed);
    if(r==PS_OK)r=ps_series_combine(c,PS_SERIES_SUBTRACT,reconstructed,position,&residual);
    ps_statistics error={0};if(r==PS_OK)r=ps_series_statistics(c,residual,&error);
    if(r==PS_OK)r=ps_report_create("Saved run analysis",recovered?"Recovered readable prefix; recording incomplete":"Stored position, central secants and trapezoidal reconstruction",&report);
    ps_series positions[]={reconstructed,position};const char *labels[]={"reconstructed","recorded"};
    if(r==PS_OK)r=plot(report,c,time,positions,labels,2,"Position and reconstruction",PS_METRE);
    const char *v[]={"dx/dt"},*e[]={"reconstructed minus recorded"};
    if(r==PS_OK)r=plot(report,c,time,&velocity,v,1,"Derived velocity",PS_VELOCITY);
    if(r==PS_OK)r=plot(report,c,time,&residual,e,1,"Reconstruction residual",PS_METRE);
    ps_table_info table={0};strcpy(table.title,"Recording");table.columns=4;
    const char *record_labels[]={"Samples","Start","End","Recovered prefix"};ps_unit record_units[]={PS_ONE,PS_SECOND,PS_SECOND,PS_ONE};
    for(unsigned i=0;r==PS_OK && i<4;i++){strcpy(table.column[i].label,record_labels[i]);r=ps_report_unit_from(record_units[i],&table.column[i].unit);}
    ps_table_handle handle;if(r==PS_OK)r=ps_report_add_table(report,&table,&handle);
    ps_table_row row={0};strcpy(row.label,"run");row.values[0]=(double)info.samples;row.values[1]=start;row.values[2]=end;row.values[3]=recovered;
    if(r==PS_OK)r=ps_report_add_row(report,handle,&row);
    memset(&table,0,sizeof table);strcpy(table.title,"Motion");table.columns=5;
    const char *motion_labels[]={"Initial x","Final x","Displacement","Secant velocity","Max residual"};ps_unit motion_units[]={PS_METRE,PS_METRE,PS_METRE,PS_VELOCITY,PS_METRE};
    for(unsigned i=0;r==PS_OK && i<5;i++){strcpy(table.column[i].label,motion_labels[i]);r=ps_report_unit_from(motion_units[i],&table.column[i].unit);}
    if(r==PS_OK)r=ps_report_add_table(report,&table,&handle);
    row.values[0]=first;row.values[1]=last;row.values[2]=last-first;row.values[3]=(last-first)/(end-start);row.values[4]=fmax(fabs(error.min),fabs(error.max));
    if(r==PS_OK)r=ps_report_add_row(report,handle,&row);
    char path[4096];int n=snprintf(path,sizeof path,"%s.psreport",prefix);
    if(r==PS_OK)r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_report_save(report,path);
    n=snprintf(path,sizeof path,"%s-motion.csv",prefix);ps_series columns[]={time,position,velocity,reconstructed,residual};
    if(r==PS_OK)r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_series_export_csv(c,columns,5,path);
    ps_report_destroy(report);ps_analysis_destroy(c);return r==PS_OK && recovered?PS_RECOVERED:r;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,.name="Saved position analysis",.run=analyze};return &api;
}
