#include "physim/report.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define CHECK(condition)                                                                           \
    do {                                                                                           \
        if (!(condition)) {                                                                        \
            fprintf(stderr, "Vacuum tutorial line %d: %s\n", __LINE__, #condition);                \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)

static int close_to(double a, double b) { return fabs(a - b) < 1e-9; }
static int run_pair(const char *experiment_path, const char *analysis_path,
                    const char *directory, const char *name) {
    char run_path[4096], prefix[4096], report_path[4096];
    CHECK(snprintf(run_path, sizeof run_path, "%s/%s.psrun", directory, name) <
          (int)sizeof run_path);
    CHECK(snprintf(prefix, sizeof prefix, "%s/%s-analysis", directory, name) <
          (int)sizeof prefix);
    void *module = ps_module_open(experiment_path);
    CHECK(module);
    void *symbol = ps_module_symbol(module, "ps_get_experiment");
    ps_experiment_entry entry;
    CHECK(symbol && sizeof entry == sizeof symbol);
    memcpy(&entry, &symbol, sizeof entry);
    const ps_experiment_api *api = entry();
    CHECK(api && api->abi_version == PS_ABI_VERSION);
    ps_context context = {0};
    context.struct_size = sizeof context;
    context.api_version = PS_API_VERSION;
    context.dt_s = 0.005;
    context.seed = 42;
    ps_rng_seed(&context.rng, context.seed);
    CHECK(api->create(&context) == PS_OK && context.channel_count == 5);
    const char *channels[] = {"position.x", "position.y", "velocity.x", "velocity.y", "energy"};
    for (int channel = 0; channel < 5; channel++)
        CHECK(!strcmp(context.channels[channel].name, channels[channel]));
    CHECK(strstr(context.model_metadata, "vacuum"));
    ps_run_writer writer;
    CHECK(ps_run_create(&writer, run_path, &context, api->name) == PS_OK);
    CHECK(ps_run_append(&writer, 0, context.values) == PS_OK);
    for (unsigned frame = 1; frame <= 200; frame++) {
        double time = frame * context.dt_s;
        CHECK(api->step(&context, context.dt_s) == PS_OK);
        context.time_s = time;
        CHECK(close_to(context.values[0], -2 + 2 * time));
        CHECK(close_to(context.values[1], 5 * time - 0.5 * 9.80665 * time * time));
        CHECK(close_to(context.values[2], 2));
        CHECK(close_to(context.values[3], 5 - 9.80665 * time));
        CHECK(close_to(context.values[4], 14.5));
        ps_scene scene = {0};
        api->build_scene(&context, &scene);
        CHECK(ps_scene_valid(&scene) && scene.count == 2);
        CHECK(scene.objects[0].id == 1 && scene.objects[1].id == 2);
        CHECK(scene.objects[0].shape == PS_SPHERE && scene.objects[1].shape == PS_LINE);
        CHECK(close_to(scene.objects[0].a.x, context.values[0]));
        CHECK(close_to(scene.objects[1].a.x, context.values[0]) &&
              close_to(scene.objects[1].a.y, context.values[1]));
        CHECK(close_to(scene.objects[1].b.x, context.values[0] + 0.15 * context.values[2]) &&
              close_to(scene.objects[1].b.y, context.values[1] + 0.15 * context.values[3]));
        CHECK(ps_run_append(&writer, time, context.values) == PS_OK);
    }
    CHECK(ps_run_close(&writer) == PS_OK);
    CHECK(api->reset(&context) == PS_OK);
    CHECK(close_to(context.values[0], -2) && close_to(context.values[1], 0));
    api->destroy(&context);
    ps_module_close(module);

    module = ps_module_open(analysis_path);
    CHECK(module);
    symbol = ps_module_symbol(module, "ps_get_analysis");
    ps_analysis_entry analysis_entry;
    CHECK(symbol && sizeof analysis_entry == sizeof symbol);
    memcpy(&analysis_entry, &symbol, sizeof analysis_entry);
    const ps_analysis_api *analysis = analysis_entry();
    CHECK(analysis && analysis->abi_version == PS_ABI_VERSION);
    CHECK(analysis->run(run_path, prefix) == PS_OK);
    CHECK(snprintf(report_path, sizeof report_path, "%s.psreport", prefix) <
          (int)sizeof report_path);
    ps_report *report = NULL;
    CHECK(ps_report_load(report_path, &report) == PS_OK);
    const ps_curve_data *curve = NULL;
    CHECK(ps_report_curve_view(report, 0, 0, &curve) == PS_OK);
    CHECK(curve->count == 201 && curve->source_count == 201);
    for (size_t index = 0; index < curve->count; index++)
        CHECK(close_to(curve->y[index], 2));
    ps_report_destroy(report);
    ps_module_close(module);
    return 0;
}

int main(int argc, char **argv) {
    CHECK(argc == 6);
    char directory[4096];
    CHECK(snprintf(directory, sizeof directory, "%s/vacuum-tutorial-%.0f", argv[5],
                   ps_clock() * 1e9) < (int)sizeof directory);
    CHECK(ps_make_directory_exclusive(directory));
    CHECK(run_pair(argv[1], argv[3], directory, "c") == 0);
    CHECK(run_pair(argv[2], argv[4], directory, "physim") == 0);
    puts("Vacuum projectile: C and Physim runs, five channels, scenes and analyses verified");
    return 0;
}
