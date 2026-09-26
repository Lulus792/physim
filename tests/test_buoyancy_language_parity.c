#include "physim/data.h"
#include "physim/experiment.h"
#include "physim/report.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "Buoyancy language parity line %d: %s\n", __LINE__, #condition); \
    return 1; \
} } while (0)

static int run(const char *runner, const char *module, const char *path, const char *work) {
    const char *args[] = {runner, module, path, "--steps", "400", "--dt", "0.005",
                          "--seed", "42", NULL};
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
        for (unsigned step = 0; step < 20; step++)
            CHECK(api[model]->step(&contexts[model], 0.005) == PS_OK);
        api[model]->build_scene(&contexts[model], &scene[model]);
        CHECK(ps_scene_valid(&scene[model]) && scene[model].count == 26);
    }
    for (unsigned object = 0; object < 26; object++) {
        const ps_object *left = &scene[0].objects[object], *right = &scene[1].objects[object];
        CHECK(left->id == right->id && left->shape == right->shape &&
              left->color == right->color && !strcmp(left->text, right->text));
        const double a[] = {left->a.x, left->a.y, left->a.z, left->b.x, left->b.y,
                            left->b.z, left->radius};
        const double b[] = {right->a.x, right->a.y, right->a.z, right->b.x,
                            right->b.y, right->b.z, right->radius};
        for (unsigned coordinate = 0; coordinate < 7; coordinate++) {
            if (coordinate >= 3 && coordinate <= 5 &&
                left->shape != PS_LINE && left->shape != PS_ARROW)
                continue;
            if (fabs(a[coordinate] - b[coordinate]) >= 2e-9)
                fprintf(stderr, "Object %u coordinate %u: C %.17g, Physim %.17g\n",
                        object, coordinate, a[coordinate], b[coordinate]);
            CHECK(fabs(a[coordinate] - b[coordinate]) < 2e-9);
        }
    }
    for (unsigned model = 0; model < 2; model++) {
        api[model]->destroy(&contexts[model]);
        ps_module_close(modules[model]);
    }
    return 0;
}
static int analysis(const char *runner, const char *module, const char *run_path,
                    const char *work) {
    char prefix[4096], report_path[4096];
    int n = snprintf(prefix, sizeof prefix, "%s/buoyancy-analysis-%.0f", work,
                     ps_clock() * 1e9);
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
    CHECK(ps_report_describe(report, NULL, NULL, &plots, NULL) == PS_OK && plots == 3);
    const char *titles[] = {"Höhe über Wasseroberfläche", "Eingetauchter Anteil",
                             "Energiebilanz"};
    for (uint32_t i = 0; i < plots; i++) {
        ps_plot_info info;
        const ps_curve_data *curve = NULL;
        CHECK(ps_report_plot_read(report, i, &info) == PS_OK);
        CHECK(!strcmp(info.title, titles[i]));
        CHECK(ps_report_curve_view(report, i, 0, &curve) == PS_OK && curve->count == 401);
    }
    ps_report_destroy(report);
    return 0;
}

int main(int argc, char **argv) {
    CHECK(argc == 8);
    char c_path[4096], language_path[4096];
    unsigned long long stamp = (unsigned long long)(ps_clock() * 1000000);
    int n = snprintf(c_path, sizeof c_path, "%s/buoyancy-c-%llu.psrun", argv[7], stamp);
    CHECK(n > 0 && (size_t)n < sizeof c_path);
    n = snprintf(language_path, sizeof language_path, "%s/buoyancy-language-%llu.psrun",
                 argv[7], stamp);
    CHECK(n > 0 && (size_t)n < sizeof language_path);
    CHECK(!run(argv[1], argv[2], c_path, argv[7]));
    CHECK(!run(argv[1], argv[3], language_path, argv[7]));
    ps_run_reader c, language;
    CHECK(ps_run_open(&c, c_path) == PS_OK);
    CHECK(ps_run_open(&language, language_path) == PS_OK);
    CHECK(c.channels == 12 && language.channels == 12);
    CHECK(strstr(c.metadata, "model=hydrostatic floating sphere"));
    CHECK(strstr(language.metadata, "model=hydrostatic floating sphere"));
    for (uint32_t i = 0; i < 12; i++) {
        CHECK(!strcmp(c.schema[i].name, language.schema[i].name));
        CHECK(!memcmp(c.schema[i].dimension, language.schema[i].dimension, 7));
    }
    for (unsigned frame = 0; frame <= 400; frame++) {
        double c_time, language_time, cv[PS_MAX_CHANNELS], lv[PS_MAX_CHANNELS];
        CHECK(ps_run_next(&c, &c_time, cv) == PS_OK);
        CHECK(ps_run_next(&language, &language_time, lv) == PS_OK);
        CHECK(fabs(c_time - language_time) < 1e-12);
        for (uint32_t i = 0; i < 12; i++)
            CHECK(isfinite(cv[i]) && isfinite(lv[i]) &&
                  fabs(cv[i] - lv[i]) <= 2e-9 * fmax(1, fabs(cv[i])));
        CHECK(cv[2] >= 0 && cv[3] >= 0 && cv[3] <= 1);
    }
    double time, values[PS_MAX_CHANNELS];
    CHECK(ps_run_next(&c, &time, values) == PS_EOF);
    CHECK(ps_run_next(&language, &time, values) == PS_EOF);
    CHECK(c.complete && language.complete && c.samples == 401 && language.samples == 401);
    ps_run_reader_close(&c);
    ps_run_reader_close(&language);
    CHECK(!scenes(argv[2], argv[3]));
    CHECK(!analysis(argv[4], argv[5], c_path, argv[7]));
    CHECK(!analysis(argv[4], argv[6], language_path, argv[7]));
    return 0;
}
