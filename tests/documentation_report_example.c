/* Appended after the exact published function in docs/reports.md. */
#include <stdlib.h>
#include <string.h>
#define REQUIRE(x) do { if(!(x)){fprintf(stderr,"Report guide line %d: %s\n",__LINE__,#x);goto done;} } while(0)
static bool guide_file_exists(const char *path) {
    FILE *file=fopen(path,"rb");if(!file)return false;fclose(file);return true;
}
int main(void) {
    int result=1;ps_analysis_context *context=NULL;ps_report *loaded=NULL;
    ps_curve_data *curve=calloc(1,sizeof *curve);
    ps_series time={0},velocity={0},independent={0},wrong_dimension={0},replacement={0};
    const double times[]={0,.5,1},values[]={-2,0,2},other[]={100,200,300};
    REQUIRE(curve);
    REQUIRE(ps_analysis_create("report-guide-series",0,&context)==PS_OK);
    REQUIRE(ps_series_from_values(context,times,3,PS_SECOND,"time",&time)==PS_OK);
    REQUIRE(ps_series_aligned_values(context,time,values,3,PS_VELOCITY,"velocity",&velocity)==PS_OK);
    REQUIRE(ps_series_from_values(context,values,3,PS_VELOCITY,"independent",&independent)==PS_OK);
    REQUIRE(velocity_plot(context,time,independent,"unaligned.psreport")==PS_INVALID);
    REQUIRE(!guide_file_exists("unaligned.psreport"));
    REQUIRE(ps_series_aligned_values(context,time,values,3,PS_METRE,"position",&wrong_dimension)==PS_OK);
    REQUIRE(velocity_plot(context,time,wrong_dimension,"wrong-unit.psreport")==PS_INVALID);
    REQUIRE(!guide_file_exists("wrong-unit.psreport"));
    REQUIRE(velocity_plot(context,time,velocity,"velocity.psreport")==PS_OK);
    REQUIRE(ps_series_aligned_values(context,time,other,3,PS_VELOCITY,"replacement",&replacement)==PS_OK);
    REQUIRE(velocity_plot(context,time,replacement,"velocity.psreport")==PS_IO);
    ps_analysis_destroy(context);context=NULL;
    REQUIRE(ps_report_load("velocity.psreport",&loaded)==PS_OK);
    char title[192],provenance[8192];uint32_t plots=0,tables=0;
    REQUIRE(ps_report_describe(loaded,title,provenance,&plots,&tables)==PS_OK);
    REQUIRE(!strcmp(title,"Meine Auswertung") && !strcmp(provenance,"Methode: zentrale Sekanten"));
    REQUIRE(plots==1 && tables==0);
    ps_plot_info info={0};ps_report_unit seconds={0},speed={0};
    REQUIRE(ps_report_plot_read(loaded,0,&info)==PS_OK);
    REQUIRE(ps_report_unit_from(PS_SECOND,&seconds)==PS_OK && ps_report_unit_from(PS_VELOCITY,&speed)==PS_OK);
    REQUIRE(!strcmp(info.title,"Geschwindigkeit") && !strcmp(info.x_label,"Zeit") && !strcmp(info.y_label,"Geschwindigkeit"));
    REQUIRE(!memcmp(info.x_unit.dimension,seconds.dimension,sizeof seconds.dimension));
    REQUIRE(info.x_unit.scale==seconds.scale && !strcmp(info.x_unit.symbol,seconds.symbol));
    REQUIRE(!memcmp(info.y_unit.dimension,speed.dimension,sizeof speed.dimension));
    REQUIRE(info.y_unit.scale==speed.scale && !strcmp(info.y_unit.symbol,speed.symbol));
    REQUIRE(info.curves==1 && ps_report_curve_read(loaded,0,0,curve)==PS_OK);
    REQUIRE(curve->kind==PS_PLOT_LINE && curve->count==3 && curve->source_count==3 && !strcmp(curve->label,"Ableitung"));
    for(size_t i=0;i<3;i++)REQUIRE(curve->x[i]==times[i] && curve->y[i]==values[i]);
    puts("Published report example: aligned values, units, saved points and exclusive output passed");
    result=0;
done:
    ps_report_destroy(loaded);ps_analysis_destroy(context);free(curve);return result;
}
