#include "physim/report.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
static ps_result plot(ps_report *r,ps_analysis_context *c,ps_series time,ps_series *series,
    const char *const *labels,unsigned count,const char *title,ps_unit unit) {
    ps_plot_info info={0};snprintf(info.title,sizeof info.title,"%s",title);
    strcpy(info.x_label,"Time");snprintf(info.y_label,sizeof info.y_label,"%s",title);
    ps_result result=ps_report_unit_from(PS_SECOND,&info.x_unit);
    if(result==PS_OK)result=ps_report_unit_from(unit,&info.y_unit);
    ps_plot_handle handle;if(result==PS_OK)result=ps_report_add_plot(r,&info,&handle);
    for(unsigned i=0;result==PS_OK && i<count;i++)result=ps_report_add_series(r,handle,c,time,series[i],labels[i],PS_PLOT_LINE);
    return result;
}
static ps_result value(ps_analysis_context *c,ps_series s,uint64_t index,double *out) {
    size_t n=0;ps_result r=ps_series_read(c,s,index,out,1,&n);return r==PS_OK && n!=1?PS_INVALID:r;
}
static ps_result analyze(const char *input,const char *prefix) {
    ps_analysis_context *c=NULL;ps_report *report=NULL;ps_dataset dataset={0};ps_dataset_info info={0};
    ps_series series[12]={0};bool recovered=false;ps_result r=ps_analysis_create(prefix,0,&c);
    if(r==PS_OK){r=ps_analysis_open_run(c,input,&dataset);recovered=r==PS_RECOVERED;if(recovered)r=PS_OK;}
    if(r==PS_OK)r=ps_dataset_describe(c,dataset,&info);
    const char *names[]={"time","a.position","b.position","a.velocity","b.velocity","energy","momentum.x",
        "energy.dissipated","energy.balance","a.impulse","collision.time","collision.count"};
    for(unsigned i=0;r==PS_OK && i<12;i++)r=ps_dataset_series(c,dataset,names[i],&series[i]);
    if(r==PS_OK)r=ps_report_create("Elastic and inelastic collision","Central vacuum collision: momentum and energy accounting",&report);
    const char *bodies[]={"Sphere A","Sphere B"},*p[]={"total"},*e[]={"kinetic","dissipated","balance"};
    ps_unit momentum={{1,1,-1,0,0,0,0},1,"kg m/s"};
    if(r==PS_OK)r=plot(report,c,series[0],&series[1],bodies,2,"Positions",PS_METRE);
    if(r==PS_OK)r=plot(report,c,series[0],&series[3],bodies,2,"Velocities",PS_VELOCITY);
    if(r==PS_OK)r=plot(report,c,series[0],&series[6],p,1,"Momentum",momentum);
    ps_series energies[]={series[5],series[7],series[8]};
    if(r==PS_OK)r=plot(report,c,series[0],energies,e,3,"Energy accounting",PS_JOULE);
    double initial_p=0,initial_balance=0,final_lost=0,count=0,event_time=0;
    ps_statistics ps={0},balance={0},impulse={0};
    if(r==PS_OK)r=value(c,series[6],0,&initial_p);
    if(r==PS_OK)r=value(c,series[8],0,&initial_balance);
    if(r==PS_OK)r=value(c,series[7],info.samples-1,&final_lost);
    if(r==PS_OK)r=value(c,series[11],info.samples-1,&count);
    if(r==PS_OK)r=value(c,series[10],info.samples-1,&event_time);
    if(r==PS_OK)r=ps_series_statistics(c,series[6],&ps);
    if(r==PS_OK)r=ps_series_statistics(c,series[8],&balance);
    if(r==PS_OK)r=ps_series_statistics(c,series[9],&impulse);
    ps_table_info table={0};strcpy(table.title,"Conservation checks");table.columns=5;
    const char *labels[]={"Samples","Maximum momentum drift","Dissipated energy","Maximum balance drift","Collisions"};
    ps_unit units[]={PS_ONE,momentum,PS_JOULE,PS_JOULE,PS_ONE};
    for(unsigned i=0;r==PS_OK && i<5;i++){strcpy(table.column[i].label,labels[i]);r=ps_report_unit_from(units[i],&table.column[i].unit);}
    ps_table_handle handle;if(r==PS_OK)r=ps_report_add_table(report,&table,&handle);
    ps_table_row row={0};strcpy(row.label,"complete run");row.values[0]=(double)info.samples;
    row.values[1]=fmax(fabs(ps.min-initial_p),fabs(ps.max-initial_p));row.values[2]=final_lost;
    row.values[3]=fmax(fabs(balance.min-initial_balance),fabs(balance.max-initial_balance));row.values[4]=count;
    if(r==PS_OK)r=ps_report_add_row(report,handle,&row);
    memset(&table,0,sizeof table);strcpy(table.title,"Collision event");table.columns=2;
    strcpy(table.column[0].label,"Contact time");strcpy(table.column[1].label,"Impulse on A");
    if(r==PS_OK)r=ps_report_unit_from(PS_SECOND,&table.column[0].unit);
    if(r==PS_OK)r=ps_report_unit_from(momentum,&table.column[1].unit);
    if(r==PS_OK)r=ps_report_add_table(report,&table,&handle);
    if(r==PS_OK && count==1){strcpy(row.label,"central collision");row.values[0]=event_time;row.values[1]=impulse.mean*(double)impulse.count;r=ps_report_add_row(report,handle,&row);}
    char path[4096];int n=snprintf(path,sizeof path,"%s.psreport",prefix);
    if(r==PS_OK)r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_report_save(report,path);
    n=snprintf(path,sizeof path,"%s-collision.csv",prefix);
    if(r==PS_OK)r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_series_export_csv(c,series,12,path);
    ps_report_destroy(report);ps_analysis_destroy(c);return r==PS_OK && recovered?PS_RECOVERED:r;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,
        .name="Elastic and inelastic collision analysis",.run=analyze};return &api;
}
