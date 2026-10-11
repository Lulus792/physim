#include "physim/data.h"
#include "physim/experiment.h"
#include "physim/report.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "Collision language parity line %d: %s\n", __LINE__, #condition); \
    return 1; \
} } while (0)

static int run(const char *runner, const char *module, const char *path, const char *work,
               const char *const *parameters, int expected_code) {
    const char *args[24] = {runner, module, path, "--steps", "400", "--dt", "0.005",
                            "--seed", "42"};
    unsigned count = 9;
    if (parameters)
        for (unsigned i = 0; parameters[i]; i++) {
            CHECK(count + 2 < sizeof args / sizeof args[0]);
            args[count++] = "--param";
            args[count++] = parameters[i];
        }
    args[count] = NULL;
    ps_process child = {0};
    CHECK(ps_process_start(&child, args, work));
    double deadline = ps_clock() + 30;
    char output[2048];
    while (ps_process_poll(&child) && ps_clock() < deadline) {
        while (ps_process_read(&child, output, sizeof output) > 0) {}
        ps_sleep(1);
    }
    if (child.running)
        ps_process_kill(&child);
    int code = child.exit_code;
    ps_process_close(&child);
    CHECK(code == expected_code);
    return 0;
}
static const ps_experiment_api *load(const char *path, void **module) {
    *module = ps_module_open(path);
    if (!*module)
        return NULL;
    void *symbol = ps_module_symbol(*module, "ps_get_experiment");
    ps_experiment_entry entry = NULL;
    if (symbol)
        memcpy(&entry, &symbol, sizeof entry);
    return entry ? entry() : NULL;
}
static int instances(const char *path) {
    void *module=NULL;const ps_experiment_api *api=load(path,&module);CHECK(api);
    ps_context c[2]={{0},{0}};
    const char *names[]={"speed","mediumMode","dragMode","friction","spin","customDensity","customViscosity"};
    const double values[]={1.2,3,1,.3,1,7.5,.0125};
    for(unsigned i=0;i<2;i++) {
        c[i].struct_size=sizeof c[i];c[i].api_version=PS_API_VERSION;c[i].dt_s=.005;c[i].seed=42;
        if(i)for(unsigned j=0;j<7;j++)CHECK(ps_parameter_override(&c[i],names[j],values[j])==PS_OK);
        CHECK(api->create(&c[i])==PS_OK && ps_parameter_finalize(&c[i])==PS_OK);
    }
    for(unsigned cycle=0;cycle<3;cycle++) {
        for(unsigned i=0;i<2;i++) {
            CHECK(c[i].channel_count==11 && fabs(c[i].values[2]-(i?1.2:.6))<1e-12 && c[i].values[5]==(i?1:0));
            CHECK(strstr(c[i].model_metadata,i?"medium=custom medium\n":"medium=vacuum\n"));
        }
        if(cycle==2)break;
        for(unsigned step=0;step<200;step++)for(unsigned i=0;i<2;i++) {
            CHECK(api->step(&c[i],.005)==PS_OK);c[i].time_s=(step+1)*.005;
        }
        CHECK(fabs(c[0].values[2]-.6)<1e-12 && c[1].values[2]<0);
        for(unsigned i=0;i<2;i++){CHECK(api->reset(&c[i])==PS_OK);c[i].time_s=0;}
    }
    for(unsigned i=0;i<2;i++)api->destroy(&c[i]);ps_module_close(module);return 0;
}
static int compare_scene(const ps_scene *c, const ps_scene *language, unsigned count) {
    CHECK(ps_scene_valid(c) && ps_scene_valid(language));
    CHECK(c->count == count && language->count == count);
    for (unsigned i = 0; i < count; i++) {
        const ps_object *a = &c->objects[i], *b = &language->objects[i];
        CHECK(a->shape == b->shape && a->id == b->id && a->color == b->color &&
              !strcmp(a->text, b->text));
        CHECK(fabs(a->radius - b->radius) < 2e-9);
        CHECK(fabs(a->a.x - b->a.x) < 2e-9 && fabs(a->a.y - b->a.y) < 2e-9 &&
              fabs(a->a.z - b->a.z) < 2e-9);
        if (a->shape == PS_LINE || a->shape == PS_ARROW)
            CHECK(fabs(a->b.x - b->b.x) < 2e-9 && fabs(a->b.y - b->b.y) < 2e-9 &&
                  fabs(a->b.z - b->b.z) < 2e-9);
    }
    return 0;
}
static int scenes(const char *c_path, const char *language_path) {
    void *modules[2] = {0};
    const ps_experiment_api *api[2] = {load(c_path, &modules[0]),
                                       load(language_path, &modules[1])};
    CHECK(api[0] && api[1]);
    ps_context contexts[2] = {{0}, {0}};
    ps_scene scene[2] = {{0}, {0}};
    for (unsigned model = 0; model < 2; model++) {
        contexts[model].struct_size = sizeof contexts[model];
        contexts[model].api_version = PS_API_VERSION;
        contexts[model].dt_s = 0.005;
        contexts[model].seed = 42;
        CHECK(api[model]->create(&contexts[model]) == PS_OK);
        api[model]->build_scene(&contexts[model], &scene[model]);
    }
    CHECK(!compare_scene(&scene[0], &scene[1], 9));
    for (unsigned model = 0; model < 2; model++) {
        for (unsigned step = 0; step < 270; step++)
            CHECK(api[model]->step(&contexts[model], 0.005) == PS_OK);
        memset(&scene[model], 0, sizeof scene[model]);
        api[model]->build_scene(&contexts[model], &scene[model]);
    }
    CHECK(!compare_scene(&scene[0], &scene[1], 11));
    for (unsigned model = 0; model < 2; model++) {
        api[model]->destroy(&contexts[model]);
        ps_module_close(modules[model]);
    }
    return 0;
}
static int analysis(const char *runner, const char *module, const char *run_path,
                    const char *work, const char *csv_suffix) {
    char prefix[4096], report_path[4096], csv_path[4096];
    int n = snprintf(prefix, sizeof prefix, "%s/collision-analysis-%s-%.0f", work,
                     csv_suffix, ps_clock() * 1e9);
    CHECK(n > 0 && (size_t)n < sizeof prefix);
    const char *args[] = {runner, module, run_path, prefix, NULL};
    ps_process child = {0};
    CHECK(ps_process_start(&child, args, work));
    double deadline = ps_clock() + 20;
    char output[2048];
    while (ps_process_poll(&child) && ps_clock() < deadline) {
        while (ps_process_read(&child, output, sizeof output) > 0) {}
        ps_sleep(1);
    }
    if (child.running)
        ps_process_kill(&child);
    int code = child.exit_code;
    ps_process_close(&child);
    CHECK(code == 0);
    n = snprintf(report_path, sizeof report_path, "%s.psreport", prefix);
    CHECK(n > 0 && (size_t)n < sizeof report_path);
    ps_report *report = NULL;
    CHECK(ps_report_load(report_path, &report) == PS_OK);
    uint32_t plots = 0;
    CHECK(ps_report_describe(report, NULL, NULL, &plots, NULL) == PS_OK && plots >= 2);
    const ps_curve_data *curve = NULL;
    CHECK(ps_report_curve_view(report, 0, 0, &curve) == PS_OK && curve->count == 401);
    ps_report_destroy(report);
    n = snprintf(csv_path, sizeof csv_path, "%s-%s.csv", prefix, csv_suffix);
    CHECK(n > 0 && (size_t)n < sizeof csv_path);
    FILE *csv = fopen(csv_path, "rb");
    CHECK(csv);
    fclose(csv);
    return 0;
}
int main(int argc, char **argv) {
    CHECK(argc == 11);
    char c_path[4096], language_path[4096];
    unsigned long long stamp = (unsigned long long)(ps_clock() * 1000000);
    const char *air[] = {"mediumMode=1", NULL};
    const char *custom[] = {"mediumMode=3", NULL};
    const char *friction[] = {"friction=0.3", "spin=1", NULL};
    const char *water[] = {"mediumMode=2","dragMode=0",NULL};
    const char *stokes[] = {"mediumMode=3","dragMode=1","customDensity=7.5","customViscosity=0.0125",NULL};
    const char *discrete[] = {"speed=1.2","ccdMode=0",NULL};
    const char *const *parameters[] = {NULL, air, custom, friction, air, custom, friction, water, stokes, discrete};
    const char *const *c_parameters[] = {NULL,NULL,NULL,NULL,air,custom,friction,water,stokes,discrete};
    const char *modules[] = {argv[2],argv[7],argv[8],argv[9],argv[2],argv[2],argv[2],argv[2],argv[2],argv[2]};
    const double medium_modes[]={0,1,3,0,1,3,0,2,3,0};
    const double drag_modes[]={2,2,2,2,2,2,2,0,1,2};
    const double frictions[]={0,0,0,.3,0,0,.3,0,0,0};
    const double spins[]={0,0,0,1,0,0,1,0,0,0};
    for (unsigned mode = 0; mode < 10; mode++) {
        int n = snprintf(c_path, sizeof c_path, "%s/collision-c-%u-%llu.psrun",
                         argv[10], mode, stamp);
        CHECK(n > 0 && (size_t)n < sizeof c_path);
        n = snprintf(language_path, sizeof language_path,
                     "%s/collision-language-%u-%llu.psrun", argv[10], mode, stamp);
        CHECK(n > 0 && (size_t)n < sizeof language_path);
        CHECK(!run(argv[1], modules[mode], c_path, argv[10], c_parameters[mode],0));
        CHECK(!run(argv[1], argv[3], language_path, argv[10], parameters[mode],0));
        ps_run_reader c, language;
        CHECK(ps_run_open(&c, c_path) == PS_OK &&
              ps_run_open(&language, language_path) == PS_OK);
        CHECK(c.channels == 11 && language.channels == 11);
        CHECK(strstr(c.metadata, "model=two homogeneous rigid spheres") &&
              strstr(language.metadata, "model=two homogeneous rigid spheres") &&
              strstr(language.metadata, "language=physim-0.182.0") &&
              strstr(c.metadata,"gravity=none") && strstr(language.metadata,"gravity=none"));
        const char *names[]={"speed","mediumMode","dragMode","friction","spin","ccdMode","customDensity","customViscosity"};
        const char *symbols[]={"m/s","1","1","1","rad/s","1","kg/m^3","Pa s"};
        const int8_t dimensions[][7]={{1,0,-1,0,0,0,0},{0},{0},{0},{0,0,-1,0,0,0,0},{0},{-3,1,0,0,0,0,0},{-1,1,-1,0,0,0,0}};
        const double selected[]={mode==9?1.2:.6,medium_modes[mode],drag_modes[mode],frictions[mode],spins[mode],mode==9?0:-1,mode==8?7.5:5,mode==8?.0125:.005};
        const char *media[]={"medium=vacuum\n","medium=air at 15 C, sea level\n","medium=water at 20 C\n","medium=custom medium\n"};
        bool continuous=mode!=9 && medium_modes[mode]==0;
        for(unsigned model=0;model<2;model++) {
            const char *metadata=model?language.metadata:c.metadata;char expected[128];
            CHECK(strstr(metadata,media[(unsigned)medium_modes[mode]]) &&
                  strstr(metadata,continuous?"ccd=linear sphere sweep + remaining time\n":"ccd=none\n"));
            for(unsigned parameter=0;parameter<8;parameter++) {
                snprintf(expected,sizeof expected,"\nparameter.%s=%.17g\n",names[parameter],selected[parameter]);
                CHECK(strstr(metadata,expected));ps_parameter_unit unit;
                CHECK(ps_parameter_unit_parse(metadata,names[parameter],&unit)==PS_OK && unit.declared && unit.scale==1 &&
                      !strcmp(unit.symbol,symbols[parameter]) && !memcmp(unit.dimension,dimensions[parameter],7));
            }
            const char *keys[]={"initial_speed_m_s","friction","a_initial_spin_rad_s","density_kg_m3","viscosity_pa_s"};
            const double density[]={0,1.225,998.2,selected[6]},viscosity[]={0,1.81e-5,1.002e-3,selected[7]};
            const double values[]={selected[0],selected[3],selected[4],density[(unsigned)selected[1]],viscosity[(unsigned)selected[1]]};
            for(unsigned property=0;property<5;property++) {
                snprintf(expected,sizeof expected,"\n%s=%.17g\n",keys[property],values[property]);CHECK(strstr(metadata,expected));
            }
            const char *drag_names[]={"drag=none\n","drag=Stokes\n","drag=quadratic\n"};
            CHECK(strstr(metadata,drag_names[(unsigned)selected[2]]));
        }
        for (uint32_t i = 0; i < 11; i++) {
            CHECK(!strcmp(c.schema[i].name, language.schema[i].name));
            CHECK(!memcmp(c.schema[i].dimension, language.schema[i].dimension, 7));
        }
        unsigned impacts = 0;
        for (unsigned frame = 0; frame <= 400; frame++) {
            double ct, lt, cv[PS_MAX_CHANNELS], lv[PS_MAX_CHANNELS];
            CHECK(ps_run_next(&c, &ct, cv) == PS_OK &&
                  ps_run_next(&language, &lt, lv) == PS_OK);
            CHECK(fabs(ct - lt) < 1e-12);
            for (uint32_t i = 0; i < 11; i++) {
                if (fabs(cv[i] - lv[i]) > 2e-9 * fmax(1, fabs(cv[i])))
                    fprintf(stderr, "Mode %u frame %u channel %u: C %.17g, Physim %.17g\n",
                            mode, frame, i, cv[i], lv[i]);
                CHECK(isfinite(cv[i]) && isfinite(lv[i]) &&
                      fabs(cv[i] - lv[i]) <= 2e-9 * fmax(1, fabs(cv[i])));
            }
            impacts += fabs(cv[10]) > 1e-9;
            CHECK(fabs(cv[7]) < 2e-9 && fabs(cv[8]) < 2e-9);
        }
        CHECK(impacts == 1);
        double time, values[PS_MAX_CHANNELS];
        CHECK(ps_run_next(&c, &time, values) == PS_EOF &&
              ps_run_next(&language, &time, values) == PS_EOF);
        CHECK(c.complete && language.complete && c.samples == 401 && language.samples == 401);
        ps_run_reader_close(&c);
        ps_run_reader_close(&language);
        if (mode == 0) {
            CHECK(!analysis(argv[4], argv[5], language_path, argv[10], "derived"));
            CHECK(!analysis(argv[4], argv[6], c_path, argv[10], "velocity"));
        }
    }
    CHECK(!scenes(argv[2], argv[3]));
    CHECK(!instances(argv[2]) && !instances(argv[3]));
    const char *invalid[]={"speed=0","speed=1000.1","mediumMode=-1","mediumMode=4","dragMode=-1","dragMode=3",
        "friction=-0.001","friction=10.01","spin=-1001","spin=1001","ccdMode=-2","ccdMode=2",
        "customDensity=-1","customDensity=100001","customViscosity=-1","customViscosity=100001",
        "mediumMode=1.5","dragMode=0.5","ccdMode=-0.5"};
    for(unsigned model=0;model<2;model++)for(unsigned i=0;i<20;i++) {
        char path[4096];snprintf(path,sizeof path,"%s/sphere-invalid-%u-%u.psrun",argv[10],model,i);
        const char *bad[]={i<19?invalid[i]:"mediumMode=1",i<19?NULL:"ccdMode=1",NULL};
        CHECK(!run(argv[1],argv[model+2],path,argv[10],bad,5));FILE *file=fopen(path,"rb");CHECK(!file);
    }
    return 0;
}
