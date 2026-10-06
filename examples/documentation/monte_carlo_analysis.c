#include "physim/report.h"
#include "physim/run_index.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
/* Match the tutorial's 256-run series. Change this and the Physim constant
 * together when deliberately analyzing a different-size archived series. */
#define TUTORIAL_RUNS 256u
static ps_result analyze(const char *input,const char *prefix) {
    char directory[4096];size_t size=strlen(input);if(size>=sizeof directory)return PS_LIMIT;
    memcpy(directory,input,size+1);char *slash=strrchr(directory,'/'),*backslash=strrchr(directory,'\\');
    if(backslash && (!slash || backslash>slash))slash=backslash;
    if(!slash)return PS_INVALID;
    *slash=0;
    double indices[TUTORIAL_RUNS],x[TUTORIAL_RUNS],y[TUTORIAL_RUNS],expected[4]={0},endpoint=0;
    const char *names[]={"position.x","position.y","velocity.x","velocity.y","nominal.x","nominal.y","uncertainty.x","uncertainty.y","energy"};
    ps_result r=PS_OK;
    for(unsigned i=0;r==PS_OK && i<TUTORIAL_RUNS;i++) {
        char path[4096];int n=snprintf(path,sizeof path,"%s/run-%04u.psrun",directory,i+1);
        if(n<0 || (size_t)n>=sizeof path)return PS_LIMIT;
        ps_run_index *run=NULL;r=ps_run_index_open(path,ps_allocator_default(),4096,&run);
        if(r==PS_RECOVERED)r=PS_CORRUPT;
        ps_run_index_info info={.struct_size=sizeof info,.version=PS_RUN_INDEX_VERSION};
        if(r==PS_OK)r=ps_run_index_get_info(run,&info);
        if(r==PS_OK && (!info.complete || !info.samples || info.channels!=9))r=PS_INVALID;
        for(unsigned k=0;r==PS_OK && k<9;k++) {
            ps_unit unit=k==2 || k==3?PS_VELOCITY:k==8?PS_JOULE:PS_METRE;
            if(strcmp(info.schema[k].name,names[k]) || memcmp(info.schema[k].dimension,unit.dimension,7))r=PS_INVALID;
        }
        double time=0,values[PS_MAX_CHANNELS];if(r==PS_OK)r=ps_run_index_read(run,info.samples-1,1,&time,values);
        if(r==PS_OK) {
            if(!i){endpoint=time;for(unsigned k=0;k<4;k++)expected[k]=values[4+k];}
            if(time!=endpoint || endpoint<=0 || values[6]<0 || values[7]<0)r=PS_INVALID;
            for(unsigned k=0;r==PS_OK && k<4;k++)if(values[4+k]!=expected[k])r=PS_INVALID;
            indices[i]=i+1;x[i]=values[0];y[i]=values[1];
        }
        ps_run_index_destroy(run);
    }
    if(r!=PS_OK)return r;
    ps_analysis_context *c=NULL;ps_report *report=NULL;ps_series ordinal={0},series[2]={0};
    r=ps_analysis_create(prefix,0,&c);
    if(r==PS_OK)r=ps_series_from_values(c,indices,TUTORIAL_RUNS,PS_ONE,"run",&ordinal);
    if(r==PS_OK)r=ps_series_aligned_values(c,ordinal,x,TUTORIAL_RUNS,PS_METRE,"position.x endpoint",&series[0]);
    if(r==PS_OK)r=ps_series_aligned_values(c,ordinal,y,TUTORIAL_RUNS,PS_METRE,"position.y endpoint",&series[1]);
    if(r==PS_OK)r=ps_report_create("Monte Carlo initial velocities","256 archived endpoints at a common positive time",&report);
    ps_table_info info={0};strcpy(info.title,"Endpoint statistics");info.columns=7;
    const char *labels[]={"Mean","Sample standard deviation","Q2.5%","Median","Q97.5%","Model mean","Model standard deviation"};
    for(unsigned k=0;r==PS_OK && k<7;k++){strcpy(info.column[k].label,labels[k]);r=ps_report_unit_from(PS_METRE,&info.column[k].unit);}
    ps_table_handle summary={0},confidence={0};if(r==PS_OK)r=ps_report_add_table(report,&info,&summary);
    memset(&info,0,sizeof info);strcpy(info.title,"95% mean interval, known model sigma");info.columns=2;
    strcpy(info.column[0].label,"Lower");strcpy(info.column[1].label,"Upper");
    if(r==PS_OK)r=ps_report_unit_from(PS_METRE,&info.column[0].unit);
    if(r==PS_OK)r=ps_report_unit_from(PS_METRE,&info.column[1].unit);
    if(r==PS_OK)r=ps_report_add_table(report,&info,&confidence);
    for(unsigned axis=0;r==PS_OK && axis<2;axis++) {
        const char *title=axis?"Vertical endpoints":"Horizontal endpoints";ps_plot_info plot={0};
        strcpy(plot.title,title);strcpy(plot.x_label,"Run");strcpy(plot.y_label,"Position");
        r=ps_report_unit_from(PS_ONE,&plot.x_unit);if(r==PS_OK)r=ps_report_unit_from(PS_METRE,&plot.y_unit);
        ps_plot_handle handle;if(r==PS_OK)r=ps_report_add_plot(report,&plot,&handle);
        if(r==PS_OK)r=ps_report_add_series(report,handle,c,ordinal,series[axis],axis?"y":"x",PS_PLOT_LINE);
        if(r==PS_OK)r=ps_report_add_histogram(report,c,series[axis],axis?"Vertical distribution":"Horizontal distribution","Position",16,&handle);
        ps_statistics stats={0};if(r==PS_OK)r=ps_series_statistics(c,series[axis],&stats);
        ps_table_row row={0};strcpy(row.label,axis?"y":"x");row.values[0]=stats.mean;row.values[1]=ps_statistics_stddev(&stats);
        const double probabilities[]={.025,.5,.975};
        for(unsigned k=0;r==PS_OK && k<3;k++)r=ps_series_quantile(c,series[axis],probabilities[k],&row.values[2+k]);
        row.values[5]=expected[axis];row.values[6]=expected[2+axis];if(r==PS_OK)r=ps_report_add_row(report,summary,&row);
        double radius=1.959963984540054*expected[2+axis]/sqrt((double)TUTORIAL_RUNS);
        row.values[0]=stats.mean-radius;row.values[1]=stats.mean+radius;if(r==PS_OK)r=ps_report_add_row(report,confidence,&row);
    }
    char path[4096];int n=snprintf(path,sizeof path,"%s.psreport",prefix);
    if(r==PS_OK)r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_report_save(report,path);
    n=snprintf(path,sizeof path,"%s-endpoints.csv",prefix);ps_series columns[]={ordinal,series[0],series[1]};
    if(r==PS_OK)r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_series_export_csv(c,columns,3,path);
    ps_report_destroy(report);ps_analysis_destroy(c);return r;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,
        .name="Monte Carlo archived series analysis",.run=analyze};return &api;
}
