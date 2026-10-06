#include "physim/report.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Saved report %d: %s\n",__LINE__,#x);return 1;}}while(0)
static bool near(double a,double b){return isfinite(a) && isfinite(b) && fabs(a-b)<=2e-11*fmax(1,fmax(fabs(a),fabs(b)));}
int main(int argc,char **argv) {
    CHECK(argc==3);ps_run_reader reader;CHECK(ps_run_open(&reader,argv[2])==PS_OK && reader.channels==1);
    double t[4096],x[4096],v[4096],reconstruction[4096],residual[4096],values[PS_MAX_CHANNELS];unsigned n=0;ps_result r;
    while(n<4096 && (r=ps_run_next(&reader,&t[n],values))==PS_OK)x[n++]=values[0];
    CHECK(n>=2 && n<4096 && (r==PS_EOF || r==PS_RECOVERED));bool recovered=r==PS_RECOVERED;ps_run_reader_close(&reader);
    for(unsigned i=0;i<n;i++){unsigned a=i?i-1:0,b=i+1<n?i+1:i;v[i]=(x[b]-x[a])/(t[b]-t[a]);}
    reconstruction[0]=x[0];residual[0]=0;double maximum=0;
    for(unsigned i=1;i<n;i++){reconstruction[i]=reconstruction[i-1]+.5*(v[i-1]+v[i])*(t[i]-t[i-1]);residual[i]=reconstruction[i]-x[i];maximum=fmax(maximum,fabs(residual[i]));}
    ps_report *report=NULL;CHECK(ps_report_load(argv[1],&report)==PS_OK);uint32_t plots,tables;
    CHECK(ps_report_describe(report,NULL,NULL,&plots,&tables)==PS_OK && plots==3 && tables==2);
    double *sources[]={reconstruction,x,v,residual};unsigned curve_index=0;
    for(unsigned plot=0;plot<3;plot++) {
        ps_plot_info info;CHECK(ps_report_plot_read(report,plot,&info)==PS_OK && info.x_unit.dimension[2]==1 && info.y_unit.dimension[0]==1 && info.y_unit.dimension[2]==(plot==1?-1:0) && info.x_unit.scale==1 && info.y_unit.scale==1 && info.y_unit.symbol[0]=='m');
        for(unsigned c=0;c<(plot==0?2u:1u);c++,curve_index++) {
            const ps_curve_data *curve;CHECK(ps_report_curve_view(report,plot,c,&curve)==PS_OK && curve->source_count==n && curve->count<=2048 && curve->x[0]==t[0] && curve->x[curve->count-1]==t[n-1]);
            unsigned at=0;for(unsigned i=0;i<curve->count;i++){while(at<n && t[at]<curve->x[i])at++;CHECK(at<n && curve->x[i]==t[at] && near(curve->y[i],sources[curve_index][at]));}
        }
    }
    ps_table_info info;ps_table_row row;
    CHECK(ps_report_table_read(report,0,&info)==PS_OK && info.columns==4 && info.rows==1 && ps_report_row_read(report,0,0,&row)==PS_OK);
    CHECK(row.values[0]==n && row.values[1]==t[0] && row.values[2]==t[n-1] && row.values[3]==recovered);
    CHECK(ps_report_table_read(report,1,&info)==PS_OK && info.columns==5 && info.rows==1 && ps_report_row_read(report,1,0,&row)==PS_OK);
    CHECK(near(row.values[0],x[0]) && near(row.values[1],x[n-1]) && near(row.values[2],x[n-1]-x[0]) && near(row.values[3],(x[n-1]-x[0])/(t[n-1]-t[0])) && near(row.values[4],maximum));
    ps_report_destroy(report);puts("Saved report: archived values, independent secants/trapezoids, bounded previews and both tables passed");return 0;
}
