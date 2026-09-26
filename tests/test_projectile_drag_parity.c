#include "physim/report.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define CHECK(condition)                                                                           \
    do {                                                                                           \
        if (!(condition)) {                                                                        \
            fprintf(stderr, "Projectile drag parity line %d: %s\n", __LINE__, #condition);        \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)

static int near(double a, double b) {
    return isfinite(a) && isfinite(b) && fabs(a - b) < 2e-9;
}
static int run(const char *runner, const char *module, const char *path, const char *work) {
    const char *args[] = {runner, module, path, "--steps", "200", "--dt", "0.005", "--seed",
                          "42", NULL};
    ps_process child = {0};
    CHECK(ps_process_start(&child, args, work));
    double deadline = ps_clock() + 20;
    char output[2048];
    while (ps_process_poll(&child) && ps_clock() < deadline) {
        int count = ps_process_read(&child, output, sizeof output);
        if (count > 0)
            fwrite(output, 1, (size_t)count, stdout);
        ps_sleep(1);
    }
    if (child.running)
        ps_process_kill(&child);
    int code = child.exit_code;
    ps_process_close(&child);
    CHECK(code == 0);
    return 0;
}
static int compare(const char *c_path, const char *language_path) {
    ps_run_reader c, language;
    CHECK(ps_run_open(&c, c_path) == PS_OK && ps_run_open(&language, language_path) == PS_OK);
    CHECK(c.channels == 5 && language.channels == 5);
    CHECK(strstr(c.metadata, "area_m2=0.01") && strstr(language.metadata, "area_m2=0.01"));
    CHECK(strstr(c.metadata, "integrator=RK4") && strstr(language.metadata, "integrator=RK4"));
    for (unsigned channel = 0; channel < 5; channel++) {
        CHECK(!strcmp(c.schema[channel].name, language.schema[channel].name));
        CHECK(!memcmp(c.schema[channel].dimension, language.schema[channel].dimension, 7));
    }
    double previous_energy = 14.5;
    for (unsigned frame = 0; frame <= 200; frame++) {
        double c_time, language_time, c_values[PS_MAX_CHANNELS], language_values[PS_MAX_CHANNELS];
        CHECK(ps_run_next(&c, &c_time, c_values) == PS_OK);
        CHECK(ps_run_next(&language, &language_time, language_values) == PS_OK);
        CHECK(near(c_time, frame * 0.005) && near(language_time, c_time));
        for (unsigned channel = 0; channel < 5; channel++)
            CHECK(near(c_values[channel], language_values[channel]));
        CHECK(c_values[4] <= previous_energy + 1e-10);
        CHECK(c_values[2] <= 2 && c_values[0] <= -2 + 2 * c_time + 1e-10);
        if (frame == 200) {
            const double reference[] = {-0.009597629944, 0.086074817488, 1.981051968288,
                                        -4.808006509895, 14.364852359039};
            for (unsigned channel = 0; channel < 5; channel++)
                CHECK(fabs(c_values[channel] - reference[channel]) < 1e-7);
        }
        previous_energy = c_values[4];
    }
    double time, values[PS_MAX_CHANNELS];
    CHECK(ps_run_next(&c, &time, values) == PS_EOF);
    CHECK(ps_run_next(&language, &time, values) == PS_EOF);
    CHECK(c.complete && language.complete && c.samples == 201 && language.samples == 201);
    ps_run_reader_close(&c);
    ps_run_reader_close(&language);
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
static int scenes(const char *c_path, const char *language_path) {
    void *modules[2] = {0};
    const ps_experiment_api *api[2] = {load(c_path, &modules[0]),
                                       load(language_path, &modules[1])};
    CHECK(api[0] && api[1]);
    ps_context contexts[2] = {{0}, {0}};
    for (unsigned model = 0; model < 2; model++) {
        contexts[model].struct_size = sizeof contexts[model];
        contexts[model].api_version = PS_API_VERSION;
        contexts[model].dt_s = 0.005;
        contexts[model].seed = 42;
        CHECK(api[model]->create(&contexts[model]) == PS_OK);
        for (unsigned step = 0; step < 20; step++)
            CHECK(api[model]->step(&contexts[model], 0.005) == PS_OK);
        ps_scene scene = {0};
        api[model]->build_scene(&contexts[model], &scene);
        CHECK(ps_scene_valid(&scene) && scene.count == 5);
        const uint32_t shapes[] = {PS_SPHERE, PS_ARROW, PS_POLYLINE, PS_POINT, PS_LABEL};
        for (unsigned object = 0; object < 5; object++)
            CHECK(scene.objects[object].id == object + 1 &&
                  scene.objects[object].shape == shapes[object]);
        CHECK(scene.objects[2].point_count == 3);
        CHECK(api[model]->reset(&contexts[model]) == PS_OK);
        CHECK(near(contexts[model].values[0], -2) && near(contexts[model].values[4], 14.5));
        api[model]->destroy(&contexts[model]);
        ps_module_close(modules[model]);
    }
    return 0;
}
static int analysis(const char *module_path, const char *run_path, const char *work,
                    const char *name) {
    char prefix[4096], report_path[4096], csv_path[4096];
    CHECK(snprintf(prefix, sizeof prefix, "%s/drag-%s-analysis-%.0f", work, name,
                   ps_clock() * 1e9) < (int)sizeof prefix);
    void *module = ps_module_open(module_path);
    CHECK(module);
    void *symbol = ps_module_symbol(module, "ps_get_analysis");
    ps_analysis_entry entry = NULL;
    CHECK(symbol);
    memcpy(&entry, &symbol, sizeof entry);
    const ps_analysis_api *api = entry();
    CHECK(api && api->run(run_path, prefix) == PS_OK);
    CHECK(snprintf(report_path, sizeof report_path, "%s.psreport", prefix) <
          (int)sizeof report_path);
    ps_report *report = NULL;
    CHECK(ps_report_load(report_path, &report) == PS_OK);
    const ps_curve_data *curve = NULL;
    CHECK(ps_report_curve_view(report, 0, 0, &curve) == PS_OK);
    CHECK(curve->count == 201 && curve->source_count == 201);
    ps_run_reader reader;
    CHECK(ps_run_open(&reader, run_path) == PS_OK);
    for (size_t index = 0; index < curve->count; index++) {
        double time, values[PS_MAX_CHANNELS];
        CHECK(ps_run_next(&reader, &time, values) == PS_OK);
        CHECK(near(curve->x[index], time));
        CHECK(fabs(curve->y[index] - values[2]) < 0.002);
    }
    ps_run_reader_close(&reader);
    ps_report_destroy(report);
    CHECK(snprintf(csv_path, sizeof csv_path, "%s-horizontal_velocity.csv", prefix) <
          (int)sizeof csv_path);
    FILE *csv = fopen(csv_path, "rb");
    CHECK(csv);
    fclose(csv);
    ps_module_close(module);
    return 0;
}
int main(int argc, char **argv) {
    CHECK(argc == 7);
    char c_path[4096], language_path[4096];
    CHECK(snprintf(c_path, sizeof c_path, "%s/drag-c-%.0f.psrun", argv[6], ps_clock() * 1e9) <
          (int)sizeof c_path);
    CHECK(snprintf(language_path, sizeof language_path, "%s/drag-language-%.0f.psrun", argv[6],
                   ps_clock() * 1e9) < (int)sizeof language_path);
    CHECK(run(argv[1], argv[2], c_path, argv[6]) == 0);
    CHECK(run(argv[1], argv[3], language_path, argv[6]) == 0);
    CHECK(compare(c_path, language_path) == 0);
    CHECK(scenes(argv[2], argv[3]) == 0);
    CHECK(analysis(argv[4], c_path, argv[6], "c") == 0);
    CHECK(analysis(argv[5], language_path, argv[6], "language") == 0);
    puts("C and Physim drag projectile: runner data, physics, scene and analysis agree");
    return 0;
}
