#include "physim/collision.h"
#include "physim/data.h"
#include "physim/report.h"
#include "physim/numerics.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "SDK probe line %d: %s\n", __LINE__, #x);                              \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static void growth(double t,const double *y,double *dy,void *user) {
    (void)t;(void)user;dy[0]=y[0];
}
int main(int argc, char **argv) {
    CHECK(argc == 3 || (argc == 4 && (!strcmp(argv[3], "language") ||
                                    !strcmp(argv[3], "sensor-language") ||
                                    !strcmp(argv[3],"adaptive") || !strcmp(argv[3],"adaptive-language"))));
    bool adaptive=argc==4 && !strncmp(argv[3],"adaptive",8);
    double integrated=1;ps_ode_options options=ps_ode_options_default();
    options.initial_step=.5;options.maximum_step=.5;
    ps_ode_report numeric;ps_ode_diagnostic diagnostic;
    CHECK(ps_ode_step_diagnosed(growth,NULL,0,.5,&integrated,1,&options,&numeric,&diagnostic)==PS_OK);
    CHECK(numeric.accepted_steps==1 && numeric.reached_time>0 && numeric.reached_time<.5 &&
          fabs(integrated-exp(numeric.reached_time))<1e-7);
    const ps_aabb bounds[] = {{{0, 0, 0}, {1, 1, 1}}, {{1, 1, 1}, {2, 2, 2}}};
    ps_collision_pair pair;
    size_t candidates = 0;
    CHECK(ps_broad_phase(bounds, 2, &pair, 1, &candidates) == PS_OK && candidates == 1 &&
          pair.a == 0 && pair.b == 1);
    ps_scene hierarchy={0};
    CHECK(ps_scene_group(&hierarchy,100,0,"SDK Root")==PS_OK);
    CHECK(ps_scene_add_id(&hierarchy,1,PS_POINT,ps_v3(1,2,3),ps_v3(1,2,3),.1,UINT32_MAX)==PS_OK);
    CHECK(ps_scene_set_parent(&hierarchy,1,100)==PS_OK && ps_scene_parent_index(&hierarchy,1)==0);
    CHECK(ps_scene_set_parent(&hierarchy,100,1)==PS_INVALID && ps_scene_valid(&hierarchy));
    unsigned char bytes[PS_SNAPSHOT_MAX];ps_context state={0};ps_snapshot restored={0};
    size_t length=ps_snapshot_encode(bytes,&state,&hierarchy,false);
    CHECK(length && ps_snapshot_decode_version(2,bytes,(uint32_t)length,&restored.time,restored.values,
                                               &restored.count,&restored.scene,&restored.paused));
    CHECK(restored.scene.objects[1].parent_id==100 && restored.scene.objects[0].shape==PS_GROUP);
    ps_run_reader reader;
    CHECK(ps_run_open(&reader, argv[1]) == PS_OK);
    CHECK(reader.channels > 0 && reader.channels <= PS_MAX_CHANNELS);
    double time, values[PS_MAX_CHANNELS];
    unsigned rows = 0;
    double previous=0,first=0,energy=0;bool varied=false;
    if(adaptive)CHECK(reader.channels==9 && strstr(reader.metadata,"step_mode=adaptive\n"));
    if(adaptive) {
        const char *names[]={"velocity.x","velocity.y","speed"};
        const int8_t dimension[]={1,0,-1,0,0,0,0};
        for(unsigned i=0;i<3;i++)CHECK(!strcmp(reader.schema[6+i].name,names[i]) &&
            !memcmp(reader.schema[6+i].dimension,dimension,7) && !strcmp(reader.schema[6+i].unit,"m/s"));
    }
    ps_result result;
    while ((result = ps_run_next(&reader, &time, values)) == PS_OK) {
        CHECK(rows <= (adaptive?500u:200u));
        if(adaptive) {
            if(!rows)energy=values[4];
            else {
                CHECK(time>previous && time<=previous+.1);
                if(rows==1)first=time;else varied |= fabs(time-previous-first)>1e-6;
                CHECK(fabs(values[4]-energy)<1e-6);
            }
            CHECK(fabs(values[6]-1.5*cos(values[0])*values[1])<1e-10 &&
                  fabs(values[7]-1.5*sin(values[0])*values[1])<1e-10 &&
                  values[8]>=0 && fabs(values[8]-hypot(values[6],values[7]))<1e-10);
            previous=time;
        } else CHECK(fabs(time - rows * .005) < 1e-12);
        for (unsigned i = 0; i < reader.channels; i++)
            CHECK(isfinite(values[i]));
        rows++;
    }
    ps_run_reader_close(&reader);
    CHECK(result == PS_EOF && rows == (adaptive?501u:201u) && (!adaptive || varied));
    ps_report *report = NULL;
    CHECK(ps_report_load(argv[2], &report) == PS_OK);
    char title[192], provenance[8192];
    uint32_t plots, tables;
    CHECK(ps_report_describe(report, title, provenance, &plots, &tables) == PS_OK);
    CHECK(title[0]);
    if (argc == 4 && !strcmp(argv[3], "sensor-language")) {
        CHECK(plots == 2 && tables == 2);
        ps_table_info info;
        ps_table_row row;
        CHECK(ps_report_table_read(report, 0, &info) == PS_OK && info.rows == 1);
        CHECK(ps_report_row_read(report, 0, 0, &row) == PS_OK && row.values[1] == rows);
        double valid = row.values[0];
        CHECK(valid > 1 && valid < rows);
        const ps_curve_data *curve;
        CHECK(ps_report_curve_view(report, 0, 2, &curve) == PS_OK);
        CHECK(curve->kind == PS_PLOT_SCATTER && curve->source_count == valid);
        CHECK(ps_report_table_read(report, 1, &info) == PS_OK && info.rows == 1);
        CHECK(ps_report_row_read(report, 1, 0, &row) == PS_OK);
        CHECK(row.values[0] == valid && row.values[3] > 0);
    } else if (argc == 4 && strcmp(argv[3],"adaptive")) {
        CHECK(plots == 2 && tables == 0);
    } else {
        CHECK(plots > 0 && tables > 0);
    }
    ps_report_destroy(report);
    puts("Installed SDK run and report validated");
    return 0;
}
