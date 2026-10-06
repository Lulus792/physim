#include "physim/report.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
static const char *methods[]={"Euler","symplectic Euler","RK4","velocity Verlet","Dormand-Prince 5(4)"};
static ps_result label(const ps_dataset_info *info,size_t index,char text[96]) {
    for(unsigned i=0;i<5;i++) {
        char token[64];snprintf(token,sizeof token,"\nintegrator=%s\n",methods[i]);
        if(strstr(info->metadata,token)) {snprintf(text,96,"Run %u / %s",(unsigned)index+1,methods[i]);return PS_OK;}
    }
    return PS_INVALID;
}
static ps_result plot(ps_report *report,const char *title,ps_unit unit,ps_plot_handle *handle) {
    ps_plot_info info={0};snprintf(info.title,sizeof info.title,"%s",title);
    snprintf(info.x_label,sizeof info.x_label,"Time");snprintf(info.y_label,sizeof info.y_label,"%s",title);
    ps_result r=ps_report_unit_from(PS_SECOND,&info.x_unit);if(r==PS_OK)r=ps_report_unit_from(unit,&info.y_unit);
    return r==PS_OK?ps_report_add_plot(report,&info,handle):r;
}
static ps_result scalar(ps_analysis_context *c,ps_series s,uint64_t at,double *value) {
    size_t n=0;ps_result r=ps_series_read(c,s,at,value,1,&n);return r==PS_OK && n!=1?PS_INVALID:r;
}
static ps_result analyze_many(const char *const *inputs,size_t count,const char *prefix) {
    if(!count || count>8)return PS_INVALID;
    ps_analysis_context *c=NULL;ps_report *report=NULL;bool recovered=false;
    ps_result r=ps_analysis_create(prefix,0,&c);
    if(r==PS_OK)r=ps_report_create("Pendulum integrators","Vacuum motion, energy drift and crossing periods",&report);
    ps_plot_handle angle_plot={0},energy_plot={0};ps_table_handle summary={0},periods={0};
    if(r==PS_OK)r=plot(report,"Angle",PS_RADIAN,&angle_plot);
    if(r==PS_OK)r=plot(report,"Energy drift",PS_JOULE,&energy_plot);
    ps_table_info info={0};strcpy(info.title,"Run comparison");info.columns=4;
    const char *titles[]={"Samples","Maximum energy drift","Duration","Period intervals"};
    ps_unit units[]={PS_ONE,PS_JOULE,PS_SECOND,PS_ONE};
    for(unsigned i=0;r==PS_OK && i<4;i++){strcpy(info.column[i].label,titles[i]);r=ps_report_unit_from(units[i],&info.column[i].unit);}
    if(r==PS_OK)r=ps_report_add_table(report,&info,&summary);
    memset(&info,0,sizeof info);strcpy(info.title,"Measured periods");info.columns=1;
    strcpy(info.column[0].label,"Mean positive-crossing period");
    if(r==PS_OK)r=ps_report_unit_from(PS_SECOND,&info.column[0].unit);
    if(r==PS_OK)r=ps_report_add_table(report,&info,&periods);
    for(size_t i=0;r==PS_OK && i<count;i++) {
        ps_dataset dataset={0};ps_series time={0},angle={0},energy={0},drift={0};ps_dataset_info details;
        r=ps_analysis_open_run(c,inputs[i],&dataset);if(r==PS_RECOVERED){recovered=true;r=PS_OK;}
        if(r==PS_OK)r=ps_dataset_describe(c,dataset,&details);
        char name[96];if(r==PS_OK)r=label(&details,i,name);
        if(r==PS_OK)r=ps_dataset_series(c,dataset,"time",&time);
        if(r==PS_OK)r=ps_dataset_series(c,dataset,"angle",&angle);
        if(r==PS_OK)r=ps_dataset_series(c,dataset,"energy",&energy);
        double initial=0,first=0,last=0;
        if(r==PS_OK)r=scalar(c,energy,0,&initial);
        if(r==PS_OK)r=scalar(c,time,0,&first);
        if(r==PS_OK)r=scalar(c,time,details.samples-1,&last);
        if(r==PS_OK)r=ps_series_affine(c,energy,1,(ps_quantity){-initial,PS_JOULE},&drift);
        if(r==PS_OK)r=ps_report_add_series(report,angle_plot,c,time,angle,name,PS_PLOT_LINE);
        if(r==PS_OK)r=ps_report_add_series(report,energy_plot,c,time,drift,name,PS_PLOT_LINE);
        ps_statistics stats={0};if(r==PS_OK)r=ps_series_statistics(c,drift,&stats);
        double previous_a=0,previous_t=0,last_cross=0,sum=0;unsigned intervals=0;bool crossed=false;
        for(uint64_t at=0;r==PS_OK && at<details.samples;at++) {
            double t,a;r=scalar(c,time,at,&t);if(r==PS_OK)r=scalar(c,angle,at,&a);
            if(r!=PS_OK)break;
            if(at && previous_a<0 && a>=0) {
                double cross=previous_t+(t-previous_t)*(-previous_a)/(a-previous_a);
                if(crossed){sum+=cross-last_cross;intervals++;}crossed=true;last_cross=cross;
            }
            previous_t=t;previous_a=a;
        }
        ps_table_row row={0};snprintf(row.label,sizeof row.label,"%s",name);
        row.values[0]=(double)details.samples;row.values[1]=fmax(fabs(stats.min),fabs(stats.max));
        row.values[2]=last-first;row.values[3]=intervals;
        if(r==PS_OK)r=ps_report_add_row(report,summary,&row);
        if(r==PS_OK && intervals){row.values[0]=sum/intervals;r=ps_report_add_row(report,periods,&row);}
        char path[4096];int n=snprintf(path,sizeof path,"%s-pendulum_%u.csv",prefix,(unsigned)i+1);
        ps_series columns[]={time,angle,energy,drift};
        if(r==PS_OK)r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_series_export_csv(c,columns,4,path);
        if(dataset.owner)ps_dataset_close(c,dataset);
    }
    if(r==PS_OK){char path[4096];int n=snprintf(path,sizeof path,"%s.psreport",prefix);r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_report_save(report,path);}
    ps_report_destroy(report);ps_analysis_destroy(c);return r==PS_OK && recovered?PS_RECOVERED:r;
}
static ps_result analyze(const char *input,const char *prefix){return analyze_many(&input,1,prefix);}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,
        .name="Pendulum integrator comparison",.run=analyze,.run_many=analyze_many};return &api;
}
