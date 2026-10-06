#include "physim/data.h"
#include "physim/experiment.h"
#include "physim/report.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "Box language parity line %d: %s\n", __LINE__, #condition); \
    return 1; \
} } while (0)

static int run(const char *runner, const char *module, const char *path, const char *work,
               const char *const *parameters) {
    const char *args[20] = {runner, module, path, "--steps", "400", "--dt", "0.005",
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
    CHECK(code == 0);
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
        if (a->shape == PS_BOX) {
            CHECK(fabs(a->b.x - b->b.x) < 2e-9 && fabs(a->b.y - b->b.y) < 2e-9 &&
                  fabs(a->b.z - b->b.z) < 2e-9);
            CHECK(fabs(a->orientation.x - b->orientation.x) < 2e-9 &&
                  fabs(a->orientation.y - b->orientation.y) < 2e-9 &&
                  fabs(a->orientation.z - b->orientation.z) < 2e-9 &&
                  fabs(a->orientation.w - b->orientation.w) < 2e-9);
        }
        if (a->shape == PS_ARROW)
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
    CHECK(!compare_scene(&scene[0], &scene[1], 8));
    for (unsigned model = 0; model < 2; model++) {
        for (unsigned step = 0; step < 270; step++)
            CHECK(api[model]->step(&contexts[model], 0.005) == PS_OK);
        memset(&scene[model], 0, sizeof scene[model]);
        api[model]->build_scene(&contexts[model], &scene[model]);
    }
    CHECK(!compare_scene(&scene[0], &scene[1], 16));
    for (unsigned model = 0; model < 2; model++) {
        api[model]->destroy(&contexts[model]);
        ps_module_close(modules[model]);
    }
    return 0;
}

static int analysis(const char *runner, const char *module, const char *run_path,
                    const char *work, const char *suffix) {
    char prefix[4096], report_path[4096], csv_path[4096];
    int n = snprintf(prefix, sizeof prefix, "%s/box-analysis-%s-%.0f", work,
                     suffix, ps_clock() * 1e9);
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
    CHECK(ps_report_describe(report, NULL, NULL, &plots, NULL) == PS_OK && plots == 2);
    const ps_curve_data *curve = NULL;
    CHECK(ps_report_curve_view(report, 0, 0, &curve) == PS_OK && curve->count == 401);
    CHECK(ps_report_curve_view(report, 0, 1, &curve) == PS_OK && curve->count == 401);
    ps_report_destroy(report);
    n = snprintf(csv_path, sizeof csv_path, "%s-velocity.csv", prefix);
    CHECK(n > 0 && (size_t)n < sizeof csv_path);
    FILE *csv = fopen(csv_path, "rb");
    CHECK(csv);
    fclose(csv);
    return 0;
}

int main(int argc, char **argv) {
    CHECK(argc == 9);
    const char *inelastic[] = {"restitution=0.5", NULL};
    const char *oblique[] = {"angle=0.35", "offset=0.12", "restitution=0.6",
                             "friction=0.3", NULL};
    const char *const *parameters[] = {NULL, inelastic, oblique};
    const char *modules[] = {argv[2], argv[6], argv[7]};
    unsigned long long stamp = (unsigned long long)(ps_clock() * 1000000);
    for (unsigned mode = 0; mode < 3; mode++) {
        char c_path[4096], language_path[4096];
        int n = snprintf(c_path, sizeof c_path, "%s/box-c-%u-%llu.psrun",
                         argv[8], mode, stamp);
        CHECK(n > 0 && (size_t)n < sizeof c_path);
        n = snprintf(language_path, sizeof language_path,
                     "%s/box-language-%u-%llu.psrun", argv[8], mode, stamp);
        CHECK(n > 0 && (size_t)n < sizeof language_path);
        CHECK(!run(argv[1], modules[mode], c_path, argv[8], NULL));
        CHECK(!run(argv[1], argv[3], language_path, argv[8], parameters[mode]));
        ps_run_reader c, language;
        CHECK(ps_run_open(&c, c_path) == PS_OK &&
              ps_run_open(&language, language_path) == PS_OK);
        CHECK(c.channels == 15 && language.channels == 15);
        CHECK(strstr(c.metadata, "contact=15-axis SAT") &&
              strstr(language.metadata, "contact=15-axis SAT") &&
              strstr(language.metadata, "language=physim-0.175.0") &&
              strstr(language.metadata, "iterations=128"));
        for (uint32_t i = 0; i < 15; i++) {
            CHECK(!strcmp(c.schema[i].name, language.schema[i].name));
            CHECK(!memcmp(c.schema[i].dimension, language.schema[i].dimension, 7));
        }
        unsigned impacts = 0;
        for (unsigned frame = 0; frame <= 400; frame++) {
            double ct, lt, cv[PS_MAX_CHANNELS], lv[PS_MAX_CHANNELS];
            CHECK(ps_run_next(&c, &ct, cv) == PS_OK &&
                  ps_run_next(&language, &lt, lv) == PS_OK);
            CHECK(fabs(ct - lt) < 1e-12);
            for (uint32_t i = 0; i < 15; i++) {
                if (fabs(cv[i] - lv[i]) > 2e-9 * fmax(1, fabs(cv[i])))
                    fprintf(stderr, "Mode %u frame %u channel %u: C %.17g, Physim %.17g\n",
                            mode, frame, i, cv[i], lv[i]);
                CHECK(isfinite(cv[i]) && isfinite(lv[i]) &&
                      fabs(cv[i] - lv[i]) <= 2e-9 * fmax(1, fabs(cv[i])));
            }
            impacts += fabs(cv[13]) > 1e-9;
        }
        CHECK(impacts > 0);
        double time, values[PS_MAX_CHANNELS];
        CHECK(ps_run_next(&c, &time, values) == PS_EOF &&
              ps_run_next(&language, &time, values) == PS_EOF);
        CHECK(c.complete && language.complete && c.samples == 401 && language.samples == 401);
        ps_run_reader_close(&c);
        ps_run_reader_close(&language);
        if (mode == 0) {
            CHECK(!analysis(argv[4], argv[5], language_path, argv[8], "language"));
            CHECK(!analysis(argv[4], argv[5], c_path, argv[8], "c"));
        }
    }
    CHECK(!scenes(argv[2], argv[3]));
    return 0;
}
