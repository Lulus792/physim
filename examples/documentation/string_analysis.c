#include "physim/report.h"
#include "physim/units.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
static ps_result analyze(const char *input,const char *prefix) {
    ps_analysis_context *c=NULL;ps_report *report=NULL;ps_dataset dataset={0};ps_dataset_info info={0};
    ps_series series[7]={0};bool recovered=false;ps_result r=ps_analysis_create(prefix,0,&c);
    if(r==PS_OK){r=ps_analysis_open_run(c,input,&dataset);recovered=r==PS_RECOVERED;if(recovered)r=PS_OK;}
    if(r==PS_OK)r=ps_dataset_describe(c,dataset,&info);
    const char *names[]={"time","displacement.center","reference.center","error.center","energy.discrete","courant","nodes"};
    for(unsigned i=0;r==PS_OK && i<7;i++)r=ps_dataset_series(c,dataset,names[i],&series[i]);
    if(r==PS_OK)r=ps_report_create("Standing wave on a fixed string","Leapfrog dispersion and discrete conserved energy",&report);
    ps_unit units[]={PS_METRE,PS_METRE,PS_JOULE,PS_ONE};
    const char *titles[]={"Center and continuum","Center error","Discrete energy","Courant number"};
    unsigned indices[]={1,3,4,5},counts[]={2,1,1,1};
    for(unsigned p=0;r==PS_OK && p<4;p++) {
        ps_plot_info plot={0};snprintf(plot.title,sizeof plot.title,"%s",titles[p]);
        strcpy(plot.x_label,"Time");snprintf(plot.y_label,sizeof plot.y_label,"%s",titles[p]);
        r=ps_report_unit_from(PS_SECOND,&plot.x_unit);
        if(r==PS_OK)r=ps_report_unit_from(units[p],&plot.y_unit);
        ps_plot_handle handle;if(r==PS_OK)r=ps_report_add_plot(report,&plot,&handle);
        for(unsigned j=0;r==PS_OK && j<counts[p];j++)r=ps_report_add_series(report,handle,c,series[0],series[indices[p]+j],names[indices[p]+j],PS_PLOT_LINE);
    }
    double initial=0;size_t count=0;ps_statistics balance={0};
    if(r==PS_OK)r=ps_series_read(c,series[4],0,&initial,1,&count);
    if(r==PS_OK && count!=1)r=PS_INVALID;
    if(r==PS_OK)r=ps_series_statistics(c,series[4],&balance);
    ps_table_info table={0};strcpy(table.title,"Conservation checks");table.columns=2;
    strcpy(table.column[0].label,"Samples");strcpy(table.column[1].label,"Maximum energy drift");
    if(r==PS_OK)r=ps_report_unit_from(PS_ONE,&table.column[0].unit);
    if(r==PS_OK)r=ps_report_unit_from(PS_JOULE,&table.column[1].unit);
    ps_table_handle handle;if(r==PS_OK)r=ps_report_add_table(report,&table,&handle);
    ps_table_row row={0};strcpy(row.label,"complete run");row.values[0]=(double)info.samples;
    row.values[1]=fmax(fabs(balance.min-initial),fabs(balance.max-initial));
    if(r==PS_OK)r=ps_report_add_row(report,handle,&row);
    char path[4096];int n=snprintf(path,sizeof path,"%s.psreport",prefix);
    if(r==PS_OK)r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_report_save(report,path);
    n=snprintf(path,sizeof path,"%s-string.csv",prefix);
    if(r==PS_OK)r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_series_export_csv(c,series,7,path);
    ps_report_destroy(report);ps_analysis_destroy(c);return r==PS_OK && recovered?PS_RECOVERED:r;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,
        .name="Standing wave on a fixed string analysis",.run=analyze};return &api;
}
