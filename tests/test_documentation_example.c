#include "physim/report.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Documentation example line %d: %s\n", __LINE__, #x);                  \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
int main(int argc, char **argv) {
    CHECK(argc == 4);
    char directory[4096], path[4096], prefix[4096];
    int n = snprintf(directory, sizeof directory, "%s/documentation-example-%.0f", argv[3],
                     ps_clock() * 1e9);
    CHECK(n > 0 && (size_t)n < sizeof directory && ps_make_directory_exclusive(directory));
    n = snprintf(path, sizeof path, "%s/motion.psrun", directory);
    CHECK(n > 0 && (size_t)n < sizeof path);
    void *module = ps_module_open(argv[1]);
    CHECK(module);
    void *symbol = ps_module_symbol(module, "ps_get_experiment");
    ps_experiment_entry entry;
    CHECK(symbol && sizeof entry == sizeof symbol);
    memcpy(&entry, &symbol, sizeof entry);
    const ps_experiment_api *api = entry();
    CHECK(api->abi_version == PS_ABI_VERSION);
    ps_context c = {0};
    c.struct_size = sizeof c;
    c.api_version = PS_API_VERSION;
    c.dt_s = .01;
    c.seed = 42;
    ps_rng_seed(&c.rng, c.seed);
    CHECK(api->create(&c) == PS_OK && c.channel_count == 1);
    CHECK(c.values[0] == 0 && !strcmp(c.channels[0].name, "position.x"));
    ps_run_writer writer;
    CHECK(ps_run_create(&writer, path, &c, api->name) == PS_OK);
    CHECK(ps_run_append(&writer, 0, c.values) == PS_OK);
    for (unsigned i = 1; i <= 200; i++) {
        CHECK(api->step(&c, c.dt_s) == PS_OK);
        c.time_s = i * c.dt_s;
        CHECK(fabs(c.values[0] - 1.5 * c.time_s) < 1e-12);
        ps_scene scene = {0};
        api->build_scene(&c, &scene);
        CHECK(ps_scene_valid(&scene) && scene.count == 1 && scene.objects[0].id == 1);
        CHECK(scene.objects[0].a.x == c.values[0]);
        CHECK(ps_run_append(&writer, c.time_s, c.values) == PS_OK);
    }
    CHECK(ps_run_close(&writer) == PS_OK);
    CHECK(api->reset(&c) == PS_OK && c.channel_count == 1 && c.values[0] == 0);
    api->destroy(&c);
    ps_module_close(module);
    module = ps_module_open(argv[2]);
    CHECK(module);
    symbol = ps_module_symbol(module, "ps_get_analysis");
    ps_analysis_entry analysis_entry;
    CHECK(symbol && sizeof analysis_entry == sizeof symbol);
    memcpy(&analysis_entry, &symbol, sizeof analysis_entry);
    const ps_analysis_api *analysis = analysis_entry();
    n = snprintf(prefix, sizeof prefix, "%s/analysis", directory);
    CHECK(n > 0 && (size_t)n < sizeof prefix);
    CHECK(analysis->run(path, prefix) == PS_OK);
    n = snprintf(path, sizeof path, "%s.psreport", prefix);
    CHECK(n > 0 && (size_t)n < sizeof path);
    ps_report *report = NULL;
    CHECK(ps_report_load(path, &report) == PS_OK);
    const ps_curve_data *curve;
    CHECK(ps_report_curve_view(report, 0, 0, &curve) == PS_OK);
    CHECK(curve->count == 201 && curve->source_count == 201);
    for (unsigned i = 0; i < curve->count; i++)
        CHECK(fabs(curve->y[i] - 1.5) < 1e-10);
    ps_report_destroy(report);
    ps_module_close(module);
    puts("Documented experiment: 201 positions, reset, scene and derived velocity verified");
    return 0;
}
