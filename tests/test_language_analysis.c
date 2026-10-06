#include "physim/report.h"
#include "physim/measurement.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Language analysis %d: %s\n", __LINE__, #x);                           \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static int process(const char *const *args, const char *work, int expected,
                   const char *diagnostic) {
    ps_process child = {0};
    CHECK(ps_process_start(&child, args, work));
    char output[16384] = {0};
    size_t used = 0;
    double until = ps_clock() + 30;
    for (;;) {
        int running = ps_process_poll(&child);
        int n = ps_process_read(&child, output + used, sizeof output - used - 1);
        if (n > 0)
            used += (size_t)n;
        if ((!running && n <= 0) || ps_clock() > until || used == sizeof output - 1)
            break;
        ps_sleep(1);
    }
    if (child.running)
        ps_process_kill(&child);
    int code = child.exit_code;
    ps_process_close(&child);
    if (code != expected)
        fprintf(stderr, "%s\n", output);
    CHECK(code == expected);
    CHECK(!diagnostic || strstr(output, diagnostic));
    return 0;
}
static int report(const char *prefix, unsigned inputs) {
    char path[4096], provenance[8192];
    snprintf(path, sizeof path, "%s.psreport", prefix);
    ps_report *r = NULL;
    CHECK(ps_report_load(path, &r) == PS_OK);
    uint32_t plots, tables;
    CHECK(ps_report_describe(r, NULL, provenance, &plots, &tables) == PS_OK);
    CHECK(plots == inputs * 2 && tables == 0 &&
          strstr(provenance, "language=physim-0.176.0\ncompiler=physimc-0.1.0-dev") &&
          strstr(provenance, "source_fnv1a64="));
    ps_curve_data curve;
    CHECK(ps_report_curve_read(r, 1, 0, &curve) == PS_OK && curve.count == 5);
    const double expected[] = {0.25, 0.5, 1, 1.5, 1.75};
    for (unsigned i = 0; i < 5; i++)
        CHECK(fabs(curve.y[i] - expected[i]) < 1e-12);
    ps_plot_info info;
    CHECK(ps_report_plot_read(r, 1, &info) == PS_OK);
    CHECK(info.y_unit.dimension[0] == 1 && info.y_unit.dimension[2] == -1);
    if (inputs == 2) {
        CHECK(ps_report_curve_read(r, 3, 0, &curve) == PS_OK && curve.count == 5);
        for (unsigned i = 0; i < 5; i++)
            CHECK(fabs(curve.y[i] - 2) < 1e-12);
    }
    ps_report_destroy(r);
    snprintf(path, sizeof path, "%s-velocity.csv", prefix);
    FILE *f = fopen(path, "rb");
    CHECK(f);
    fclose(f);
    snprintf(path, sizeof path, "%s-position.svg", prefix);
    f = fopen(path, "rb");
    CHECK(f);
    fclose(f);
    return 0;
}
static int monte_carlo_report(const char *prefix) {
    char path[4096];
    snprintf(path, sizeof path, "%s.psreport", prefix);
    ps_report *report_data = NULL;
    CHECK(ps_report_load(path, &report_data) == PS_OK);
    uint32_t plots = 0, tables = 0;
    CHECK(ps_report_describe(report_data, NULL, NULL, &plots, &tables) == PS_OK &&
          plots == 2 && tables == 1);
    ps_curve_data trace, histogram;
    CHECK(ps_report_curve_read(report_data, 0, 0, &trace) == PS_OK &&
          trace.count == 1024 && trace.source_count == 1024);
    CHECK(ps_report_curve_read(report_data, 1, 0, &histogram) == PS_OK &&
          histogram.count == 24 && histogram.source_count == 1024);
    ps_rng rng;
    ps_rng_seed(&rng, 1234);
    ps_statistics reference = {0};
    for (unsigned i = 0; i < 1024; i++) {
        double value;
        CHECK(ps_distribution_sample((ps_distribution){PS_DIST_NORMAL, 0, 1}, &rng, &value) ==
              PS_OK);
        CHECK(trace.x[i] == i && trace.y[i] == value);
        ps_statistics_push(&reference, value);
    }
    uint64_t counted = 0;
    for (unsigned i = 0; i < histogram.count; i++)
        counted += (uint64_t)histogram.y[i];
    CHECK(counted == 1024);
    ps_table_row row;
    CHECK(ps_report_row_read(report_data, 0, 0, &row) == PS_OK);
    CHECK(fabs(row.values[0] - reference.mean) < 1e-13 &&
          fabs(row.values[1] - ps_statistics_stddev(&reference)) < 1e-13);
    ps_report_destroy(report_data);
    const char *suffixes[] = {"draws.csv", "trace.svg", "histogram.svg", "summary.csv"};
    for (unsigned i = 0; i < 4; i++) {
        snprintf(path, sizeof path, "%s-%s", prefix, suffixes[i]);
        FILE *file = fopen(path, "rb");
        CHECK(file);
        fclose(file);
    }
    snprintf(path, sizeof path, "%s.inputs.csv", prefix);
    FILE *manifest = fopen(path, "rb");
    CHECK(manifest);
    char line[4096];
    CHECK(fgets(line, sizeof line, manifest) && strstr(line, "role,index,path"));
    CHECK(fgets(line, sizeof line, manifest) && !strncmp(line, "module,0,", 9));
    CHECK(!fgets(line, sizeof line, manifest));
    fclose(manifest);
    return 0;
}
static int batch_endpoint_report(const char *prefix) {
    char path[4096];
    snprintf(path, sizeof path, "%s.psreport", prefix);
    ps_report *report_data = NULL;
    CHECK(ps_report_load(path, &report_data) == PS_OK);
    uint32_t plots, tables;
    CHECK(ps_report_describe(report_data, NULL, NULL, &plots, &tables) == PS_OK &&
          plots == 2 && tables == 1);
    ps_curve_data trace, histogram;
    CHECK(ps_report_curve_read(report_data, 0, 0, &trace) == PS_OK && trace.count == 2);
    CHECK(trace.x[0] == 0 && trace.x[1] == 1 && trace.y[0] == 1 && trace.y[1] == 0);
    CHECK(ps_report_curve_read(report_data, 1, 0, &histogram) == PS_OK);
    uint64_t counted = 0;
    for (unsigned i = 0; i < histogram.count; i++)
        counted += (uint64_t)histogram.y[i];
    CHECK(counted == 2);
    ps_table_row row;
    CHECK(ps_report_row_read(report_data, 0, 0, &row) == PS_OK &&
          row.values[0] == 2 && row.values[1] == 0.5);
    ps_report_destroy(report_data);
    snprintf(path, sizeof path, "%s-endpoints.csv", prefix);
    FILE *file = fopen(path, "rb");
    CHECK(file);
    fclose(file);
    return 0;
}
int main(int argc, char **argv) {
    CHECK(argc == 12);
    char work[4096], input[4096], language[4096], prefix[4096];
    snprintf(work, sizeof work, "%s/language-analysis-%.0f", argv[7], ps_clock() * 1e6);
    CHECK(ps_make_directory_exclusive(work));
    snprintf(input, sizeof input, "%s/c.psrun", work);
    snprintf(language, sizeof language, "%s/language.psrun", work);
    ps_context c = {0};
    c.struct_size = sizeof c;
    c.api_version = PS_API_VERSION;
    CHECK(ps_channel_add(&c, "position.x", PS_METRE, "Polynomial reference") == 0);
    ps_run_writer writer;
    CHECK(ps_run_create(&writer, input, &c, "C reference") == PS_OK);
    for (unsigned i = 0; i < 5; i++) {
        double t = i * 0.25, value = t * t;
        CHECK(ps_run_append(&writer, t, &value) == PS_OK);
    }
    CHECK(ps_run_close(&writer) == PS_OK);
    snprintf(prefix, sizeof prefix, "%s/statistics", work);
    const char *reference[] = {argv[4], argv[8], input, prefix, NULL};
    CHECK(process(reference, work, 0, NULL) == 0);
    const char *experiment[] = {argv[5], argv[6], language, "--steps", "4", "--dt", "0.25", NULL};
    CHECK(process(experiment, work, 0, NULL) == 0);
    snprintf(prefix, sizeof prefix, "%s/resampled", work);
    const char *resampled[] = {argv[4], argv[8], "--runs", prefix, input, language, NULL};
    CHECK(process(resampled, work, 0, NULL) == 0);
    snprintf(prefix, sizeof prefix, "%s/mixed", work);
    const char *analysis[] = {argv[4], argv[1], "--runs", prefix, input, language, NULL};
    CHECK(process(analysis, work, 0, NULL) == 0);
    CHECK(report(prefix, 2) == 0);
    snprintf(prefix, sizeof prefix, "%s/monte-carlo", work);
    const char *monte_carlo[] = {argv[4], argv[10], "--runs", prefix, NULL};
    CHECK(process(monte_carlo, work, 0, NULL) == 0);
    CHECK(monte_carlo_report(prefix) == 0);
    snprintf(prefix, sizeof prefix, "%s/batch-endpoints", work);
    const char *batch_endpoints[] = {argv[4], argv[11], "--runs", prefix, input, language, NULL};
    CHECK(process(batch_endpoints, work, 0, NULL) == 0);
    CHECK(batch_endpoint_report(prefix) == 0);
    for (unsigned i = 2; i <= 3; i++) {
        snprintf(prefix, sizeof prefix, "%s/failure-%u", work, i);
        const char *bad[] = {argv[4], argv[i], input, prefix, NULL};
        CHECK(process(bad, work, 5, ".phys:") == 0);
        char path[4096];
        snprintf(path, sizeof path, "%s.psreport", prefix);
        FILE *f = fopen(path, "rb");
        CHECK(!f);
        void *bad_module = ps_module_open(argv[i]);
        CHECK(bad_module);
        void *bad_symbol = ps_module_symbol(bad_module, "ps_get_analysis");
        ps_analysis_entry bad_entry = NULL;
        memcpy(&bad_entry, &bad_symbol, sizeof bad_entry);
        CHECK(bad_entry);
        for (unsigned repeat = 0; repeat < 2; repeat++) {
            snprintf(prefix, sizeof prefix, "%s/direct-failure-%u-%u", work, i, repeat);
            CHECK(bad_entry()->run(input, prefix) == PS_INVALID);
        }
        ps_module_close(bad_module);
    }
    void *module = ps_module_open(argv[1]);
    CHECK(module);
    void *symbol = ps_module_symbol(module, "ps_get_analysis");
    ps_analysis_entry entry = NULL;
    memcpy(&entry, &symbol, sizeof entry);
    CHECK(entry);
    const ps_analysis_api *api = entry();
    CHECK(api->abi_version == PS_ABI_VERSION && api->struct_size == sizeof *api);
    CHECK(api->run_many(NULL, 0, NULL) == PS_INVALID);
    for (unsigned i = 0; i < 2; i++) {
        snprintf(prefix, sizeof prefix, "%s/reentrant-%u", work, i);
        CHECK(api->run(input, prefix) == PS_OK);
        CHECK(report(prefix, 1) == 0);
    }
    ps_module_close(module);
    module = ps_module_open(argv[9]);
    CHECK(module);
    symbol = ps_module_symbol(module, "ps_get_analysis");
    memcpy(&entry, &symbol, sizeof entry);
    CHECK(entry);
    const char *repeated[] = {input, input, input, input, input, input, input, input};
    for (size_t count = 1; count <= 8; count++) {
        snprintf(prefix, sizeof prefix, "%s/operations-error-%zu", work, count);
        CHECK(entry()->run_many(repeated, count, prefix) ==
              ((count == 4 || count == 5) ? PS_NUMERIC : PS_INVALID));
    }
    ps_module_close(module);
    puts("Language analysis: C/language runs, dimensions, derived data, exports, stale handles and "
         "lifecycle passed");
    return 0;
}
