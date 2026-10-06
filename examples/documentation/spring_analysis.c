#include "physim/report.h"
#include <math.h>
#include <stdio.h>
static ps_result add_plot(ps_report *report,ps_analysis_context *context,ps_series time,
    ps_series *values,const char *const *labels,unsigned count,const char *title,ps_unit unit) {
    ps_plot_info info={0};snprintf(info.title,sizeof info.title,"%s",title);
    snprintf(info.x_label,sizeof info.x_label,"Time");snprintf(info.y_label,sizeof info.y_label,"%s",title);
    ps_result r=ps_report_unit_from(PS_SECOND,&info.x_unit);if(r==PS_OK)r=ps_report_unit_from(unit,&info.y_unit);
    ps_plot_handle handle;if(r==PS_OK)r=ps_report_add_plot(report,&info,&handle);
    for(unsigned i=0;r==PS_OK && i<count;i++)r=ps_report_add_series(report,handle,context,time,values[i],labels[i],PS_PLOT_LINE);
    return r;
}
static ps_result analyze(const char *input,const char *prefix) {
    ps_analysis_context *context=NULL;ps_report *report=NULL;ps_dataset run={0};
    ps_series time={0},values[5]={0};bool recovered=false;ps_result r=ps_analysis_create(prefix,0,&context);
    if(r==PS_OK){r=ps_analysis_open_run(context,input,&run);recovered=r==PS_RECOVERED;if(recovered)r=PS_OK;}
    if(r==PS_OK)r=ps_dataset_series(context,run,"time",&time);
    const char *names[]={"position.x","velocity.x","energy","energy.dissipated","energy.balance"};
    for(unsigned i=0;r==PS_OK && i<5;i++)r=ps_dataset_series(context,run,names[i],&values[i]);
    if(r==PS_OK)r=ps_report_create("Spring mass damper","Motion and energy accounting",&report);
    const char *position[]={"extension"},*velocity[]={"velocity"},*energies[]={"mechanical","dissipated","total"};
    if(r==PS_OK)r=add_plot(report,context,time,&values[0],position,1,"Extension",PS_METRE);
    if(r==PS_OK)r=add_plot(report,context,time,&values[1],velocity,1,"Velocity",PS_VELOCITY);
    if(r==PS_OK)r=add_plot(report,context,time,&values[2],energies,3,"Energy accounting",PS_JOULE);
    ps_statistics stats={0};double initial=0;size_t read=0;
    if(r==PS_OK)r=ps_series_statistics(context,values[4],&stats);
    if(r==PS_OK)r=ps_series_read(context,values[4],0,&initial,1,&read);
    if(r==PS_OK && read!=1)r=PS_INVALID;
    ps_table_info table={0};snprintf(table.title,sizeof table.title,"Energy balance");table.columns=2;
    snprintf(table.column[0].label,sizeof table.column[0].label,"Samples");snprintf(table.column[1].label,sizeof table.column[1].label,"Maximum drift");
    if(r==PS_OK)r=ps_report_unit_from(PS_ONE,&table.column[0].unit);
    if(r==PS_OK)r=ps_report_unit_from(PS_JOULE,&table.column[1].unit);
    ps_table_handle handle;if(r==PS_OK)r=ps_report_add_table(report,&table,&handle);
    ps_table_row row={0};snprintf(row.label,sizeof row.label,"complete run");row.values[0]=(double)stats.count;
    row.values[1]=fmax(fabs(stats.min-initial),fabs(stats.max-initial));
    if(r==PS_OK)r=ps_report_add_row(report,handle,&row);
    char path[4096];if(r==PS_OK){int n=snprintf(path,sizeof path,"%s.psreport",prefix);r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_report_save(report,path);}
    if(r==PS_OK){int n=snprintf(path,sizeof path,"%s-spring_check.csv",prefix);ps_series columns[]={time,values[0],values[1],values[2],values[3],values[4]};r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_series_export_csv(context,columns,6,path);}
    ps_report_destroy(report);ps_analysis_destroy(context);return r==PS_OK && recovered?PS_RECOVERED:r;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,.name="Spring mass damper analysis",.run=analyze};return &api;
}
