#include "physim/report.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
static const char *methods[]={"Euler","symplectic Euler","RK4","velocity Verlet","Dormand-Prince 5(4)"};
static ps_result label(const ps_dataset_info *info,size_t index,char text[96]) {
    bool adaptive=strstr(info->metadata,"\nstep_mode=adaptive\n")!=NULL;
    const char *key=adaptive?"adaptive_integrator":"integrator";
    for(unsigned i=0;i<5;i++) {
        char token[80];snprintf(token,sizeof token,"\n%s=%s\n",key,methods[i]);
        if(strstr(info->metadata,token)) {
            snprintf(text,96,"Run %u / %s%s",(unsigned)index+1,methods[i],adaptive?" (adaptive)":"");
            return PS_OK;
        }
    }
    /* Older vacuum tutorial runs used integrator=RK45 for their adaptive-only
     * entry. Do not infer an adaptive method from another fixed selection. */
    if(adaptive && !strstr(info->metadata,"\nadaptive_integrator=") &&
       strstr(info->metadata,"\nintegrator=Dormand-Prince 5(4)\n")) {
        snprintf(text,96,"Run %u / Dormand-Prince 5(4) (adaptive)",(unsigned)index+1);
        return PS_OK;
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
static ps_result decay_table(ps_report *report,ps_table_handle *out) {
    ps_table_info info={0};strcpy(info.title,"Observed amplitude decay");info.columns=5;
    const char *labels[]={"Peak intervals","Mean log decrement","Mean rate","Minimum rate","Maximum rate"};
    ps_unit units[]={PS_ONE,PS_ONE,PS_HERTZ,PS_HERTZ,PS_HERTZ};
    for(unsigned i=0;i<5;i++) {strcpy(info.column[i].label,labels[i]);ps_report_unit_from(units[i],&info.column[i].unit);}
    return ps_report_add_table(report,&info,out);
}
static ps_result add_decay(ps_analysis_context *c,ps_series angle,ps_series time,
                           ps_report *report,ps_table_handle table,const char *name,
                           const char *prefix,unsigned index) {
    ps_series peaks[3];ps_result r=ps_series_positive_peaks(c,angle,time,peaks);
    if(r!=PS_OK)return r;
    ps_series_info info;r=ps_series_describe(c,peaks[0],&info);
    char path[4096];int n=snprintf(path,sizeof path,"%s-peaks_%u.csv",prefix,index+1);
    if(r==PS_OK)r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_series_export_csv(c,peaks,3,path);
    n=snprintf(path,sizeof path,"%s-decay_%u.csv",prefix,index+1);
    if(r==PS_OK && (n<0 || (size_t)n>=sizeof path))r=PS_LIMIT;
    FILE *csv=r==PS_OK?fopen(path,"wx"):NULL;
    if(r==PS_OK && !csv)r=PS_IO;
    if(r==PS_OK && fputs("start_s,end_s,start_amplitude_rad,end_amplitude_rad,log_decrement,rate_per_s,segment\n",csv)<0)r=PS_IO;
    ps_statistics decrements={0},rates={0};double previous[3]={0};
    for(uint64_t at=0;r==PS_OK && at<info.count;at++) {
        double current[3];
        for(unsigned i=0;i<3 && r==PS_OK;i++) {size_t count=0;r=ps_series_read(c,peaks[i],at,&current[i],1,&count);if(r==PS_OK && count!=1)r=PS_CORRUPT;}
        if(r!=PS_OK)break;
        if(at && current[2]==previous[2]) {
            double interval=current[0]-previous[0];
            double decrement=log(previous[1])-log(current[1]);
            double rate=decrement/interval;
            if(interval<=0 || !isfinite(interval) || !isfinite(rate)){r=PS_NUMERIC;break;}
            ps_statistics_push(&decrements,decrement);ps_statistics_push(&rates,rate);
            if(fprintf(csv,"%.17g,%.17g,%.17g,%.17g,%.17g,%.17g,%.17g\n",previous[0],current[0],previous[1],current[1],decrement,rate,current[2])<0)r=PS_IO;
        }
        memcpy(previous,current,sizeof previous);
    }
    if(csv && fclose(csv))r=PS_IO;
    if(r==PS_OK && decrements.count) {
        ps_table_row row={0};snprintf(row.label,sizeof row.label,"%s",name);
        row.values[0]=(double)decrements.count;row.values[1]=decrements.mean;
        row.values[2]=rates.mean;row.values[3]=rates.min;row.values[4]=rates.max;
        r=ps_report_add_row(report,table,&row);
    }
    for(unsigned i=0;i<3;i++)(void)ps_series_release(c,peaks[i]);
    return r;
}
static ps_result analyze_many(const char *const *inputs,size_t count,const char *prefix) {
    if(!count || count>8)return PS_INVALID;
    ps_analysis_context *c=NULL;ps_report *report=NULL;bool recovered=false;
    ps_result r=ps_analysis_create(prefix,0,&c);
    if(r==PS_OK)r=ps_report_create("Pendulum integrators","Mechanical energy changes and positive crossing periods",&report);
    ps_plot_handle angle_plot={0},energy_plot={0};ps_table_handle summary={0},periods={0},decay={0};
    if(r==PS_OK)r=plot(report,"Angle",PS_RADIAN,&angle_plot);
    if(r==PS_OK)r=plot(report,"Energy change",PS_JOULE,&energy_plot);
    ps_table_info info={0};strcpy(info.title,"Run comparison");info.columns=4;
    const char *titles[]={"Samples","Maximum energy change","Duration","Period intervals"};
    ps_unit units[]={PS_ONE,PS_JOULE,PS_SECOND,PS_ONE};
    for(unsigned i=0;r==PS_OK && i<4;i++){strcpy(info.column[i].label,titles[i]);r=ps_report_unit_from(units[i],&info.column[i].unit);}
    if(r==PS_OK)r=ps_report_add_table(report,&info,&summary);
    memset(&info,0,sizeof info);strcpy(info.title,"Measured periods");info.columns=1;
    strcpy(info.column[0].label,"Mean positive-crossing period");
    if(r==PS_OK)r=ps_report_unit_from(PS_SECOND,&info.column[0].unit);
    if(r==PS_OK)r=ps_report_add_table(report,&info,&periods);
    if(r==PS_OK)r=decay_table(report,&decay);
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
        if(r==PS_OK)r=add_decay(c,angle,time,report,decay,name,prefix,(unsigned)i);
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
