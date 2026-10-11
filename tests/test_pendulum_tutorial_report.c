#include "physim/report.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Pendulum report %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char **argv) {
    CHECK(argc>=3 && argc<=10);ps_report *report=NULL;CHECK(ps_report_load(argv[1],&report)==PS_OK);
    uint32_t plots,tables;CHECK(ps_report_describe(report,NULL,NULL,&plots,&tables)==PS_OK &&
        ((plots==2 && tables==3) || (plots==3 && tables==4)));
    unsigned period_row=0,balance_row=0;
    for(int i=2;i<argc;i++) {
        const ps_curve_data *a,*e;ps_plot_info info;
        CHECK(ps_report_plot_read(report,0,&info)==PS_OK && info.curves==(unsigned)argc-2 && info.y_unit.dimension[2]==0);
        CHECK(ps_report_curve_view(report,0,i-2,&a)==PS_OK && ps_report_curve_view(report,1,i-2,&e)==PS_OK);
        ps_run_reader reader;CHECK(ps_run_open(&reader,argv[i])==PS_OK);
        int balance=-1,loss=-1;
        for(unsigned k=0;k<reader.channels;k++) {
            if(!strcmp(reader.schema[k].name,"energy.balance"))balance=(int)k;
            if(!strcmp(reader.schema[k].name,"energy.dissipated"))loss=(int)k;
        }
        double t,v[PS_MAX_CHANNELS],initial=0,maximum=0,first=0,previous_a=0,previous_t=0,last_cross=0,sum=0;
        double initial_balance=0,max_balance=0,last_loss=0;
        uint64_t count=0;unsigned intervals=0;bool crossed=false;ps_result r;
        while((r=ps_run_next(&reader,&t,v))==PS_OK) {
            if(!count){initial=v[4];first=t;}maximum=fmax(maximum,fabs(v[4]-initial));
            if(balance>=0) {
                CHECK(loss>=0);if(!count)initial_balance=v[balance];
                max_balance=fmax(max_balance,fabs(v[balance]-initial_balance));last_loss=v[loss];
            }
            if(count && previous_a<0 && v[0]>=0) {
                double crossing=previous_t+(t-previous_t)*(-previous_a)/(v[0]-previous_a);
                if(crossed){sum+=crossing-last_cross;intervals++;}last_cross=crossing;crossed=true;
            }
            previous_t=t;previous_a=v[0];count++;
        }
        CHECK(r==PS_EOF && a->source_count==count && e->source_count==count);
        /* Full CSV checks cover every row; report previews may reduce >4096 rows. */
        CHECK(a->x[0]==first && a->x[a->count-1]==previous_t);
        CHECK(fabs(e->y[0])<1e-14);
        if(balance>=0) {
            const ps_curve_data *error;ps_table_row metric;
            CHECK(plots==3 && tables==4 && ps_report_curve_view(report,2,balance_row,&error)==PS_OK && error->source_count==count);
            CHECK(ps_report_row_read(report,3,balance_row++,&metric)==PS_OK && fabs(metric.values[0]-last_loss)<1e-12 && fabs(metric.values[1]-max_balance)<1e-12);
            CHECK(fabs(error->y[0])<1e-14 && fabs(error->y[error->count-1]-(v[balance]-initial_balance))<1e-12);
        }
        ps_table_row row;CHECK(ps_report_row_read(report,0,i-2,&row)==PS_OK && row.values[0]==count &&
            fabs(row.values[1]-maximum)<1e-12 && row.values[2]==previous_t-first && row.values[3]==intervals);
        if(intervals){CHECK(ps_report_row_read(report,1,period_row++,&row)==PS_OK && fabs(row.values[0]-sum/intervals)<1e-12);}
        CHECK(ps_report_plot_read(report,1,&info)==PS_OK && info.y_unit.dimension[0]==2 && info.y_unit.dimension[1]==1 && info.y_unit.dimension[2]==-2);
        if(count<=PS_REPORT_MAX_POINTS) {
            CHECK(a->count==count && e->count==count);ps_run_reader_close(&reader);CHECK(ps_run_open(&reader,argv[i])==PS_OK);
            for(unsigned j=0;j<count;j++){CHECK(ps_run_next(&reader,&t,v)==PS_OK);CHECK(a->x[j]==t && a->y[j]==v[0] && e->x[j]==t && fabs(e->y[j]-(v[4]-initial))<1e-12);}
        }
        ps_run_reader_close(&reader);
    }
    ps_table_info info;CHECK(ps_report_table_read(report,1,&info)==PS_OK && info.rows==period_row);
    if(plots==3) {
        ps_plot_info extra;CHECK(ps_report_plot_read(report,2,&extra)==PS_OK && extra.curves==balance_row &&
            !strcmp(extra.title,"Energy balance error") && ps_report_table_read(report,3,&info)==PS_OK && info.rows==balance_row);
    }
    ps_report_destroy(report);puts("Pendulum report: independent axes, complete curves, SI units, energy and periods passed");return 0;
}
