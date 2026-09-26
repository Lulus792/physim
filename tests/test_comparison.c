#include "physim/report.h"
#include "platform.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Comparison line %d: %s\n", __LINE__, #x);                             \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static ps_result fixture(const char *path, unsigned count, double dt, double slope, ps_unit unit,
                         double start) {
    remove(path);
    ps_context context = {0};
    context.dt_s = dt;
    ps_channel_add(&context, "position.x", unit, "linear reference");
    ps_run_writer writer;
    ps_result result = ps_run_create(&writer, path, &context, "comparison reference");
    if (result != PS_OK)
        return result;
    for (unsigned i = 0; i < count && result == PS_OK; i++) {
        double time = start + i * dt, x = 3 + slope * time;
        result = ps_run_append(&writer, time, &x);
    }
    ps_result close = ps_run_close(&writer);
    return result == PS_OK ? close : result;
}
static int run(const char *const *args) {
    ps_process process = {0};
    if (!ps_process_start(&process, args, NULL))
        return -1;
    double until = ps_clock() + 10;
    char output[2048];
    while (ps_process_poll(&process) && ps_clock() < until) {
        while (ps_process_read(&process, output, sizeof output) > 0) {
        }
        ps_sleep(1);
    }
    int code = process.running ? -1 : process.exit_code;
    ps_process_close(&process);
    return code;
}
int main(int argc, char **argv) {
    if (argc != 3)
        return 2;
    const char *files[] = {"comparison A ä.psrun",         "comparison B ä.psrun",
                           "comparison wrong.psrun",       "comparison result.inputs.csv",
                           "comparison result.psreport",   "comparison result-run-1.csv",
                           "comparison result-run-2.csv",  "comparison invalid.inputs.csv",
                           "comparison invalid.psreport",  "comparison invalid-run-1.csv",
                           "comparison invalid-run-2.csv", "comparison result-difference-2.csv"};
    for (size_t i = 0; i < sizeof files / sizeof *files; i++)
        remove(files[i]);
    CHECK(fixture(files[0], 17, .25, 1, PS_METRE, 0) == PS_OK);
    CHECK(fixture(files[1], 33, .1, 2, PS_METRE, 0) == PS_OK);
    CHECK(fixture(files[2], 17, .25, 1, PS_SECOND, 0) == PS_OK);
    const char *args[] = {argv[1],  argv[2],  "--runs", "comparison result",
                          files[0], files[1], NULL};
    CHECK(run(args) == 0);
    ps_report *report = NULL;
    CHECK(ps_report_load(files[4], &report) == PS_OK);
    uint32_t plots = 0, tables = 0;
    ps_report_describe(report, NULL, NULL, &plots, &tables);
    CHECK(plots == 3 && tables == 2);
    ps_curve_data curve;
    for (unsigned i = 0; i < 2; i++) {
        CHECK(ps_report_curve_read(report, 0, i, &curve) == PS_OK);
        CHECK(curve.count == (i ? 33u : 17u));
        CHECK(fabs(curve.x[curve.count - 1] - (i ? 3.2 : 4)) < 1e-12);
        for (uint32_t j = 0; j < curve.count; j++)
            CHECK(fabs(curve.y[j] - (double)(i + 1)) < 1e-12);
        ps_table_row row;
        CHECK(ps_report_row_read(report, 0, i, &row) == PS_OK &&
              fabs(row.values[0] - (double)(i + 1)) < 1e-12 && row.values[1] < 1e-12);
    }
    CHECK(ps_report_curve_read(report, 2, 0, &curve) == PS_OK && curve.count == 33);
    for (uint32_t j = 0; j < curve.count; j++)
        CHECK(fabs(curve.y[j] - curve.x[j]) < 1e-12);
    ps_table_row difference;
    CHECK(ps_report_row_read(report, 1, 0, &difference) == PS_OK &&
          fabs(difference.values[0] - 1.6) < 1e-12 &&
          fabs(difference.values[1] - sqrt(.935)) < 1e-12 && fabs(difference.values[2]) < 1e-12 &&
          fabs(difference.values[3] - 3.2) < 1e-12);
    FILE *f = fopen("comparison result-difference-2.csv", "rb");
    CHECK(f != NULL);
    char line[512];
    CHECK(fgets(line, sizeof line, f) != NULL);
    unsigned rows = 0;
    double time, input, reference, delta;
    while (fgets(line, sizeof line, f)) {
        CHECK(sscanf(line, "%lf,%lf,%lf,%lf", &time, &input, &reference, &delta) == 4);
        CHECK(fabs(input - (3 + 2 * time)) < 1e-12 && fabs(reference - (3 + time)) < 1e-12 &&
              fabs(delta - time) < 1e-12);
        rows++;
    }
    fclose(f);
    CHECK(rows == 33);
    f = fopen(files[3], "rb");
    CHECK(f != NULL);
    char manifest[8192] = {0};
    CHECK(fread(manifest, 1, sizeof manifest - 1, f) > 0);
    fclose(f);
    CHECK(strstr(manifest, files[0]) && strstr(manifest, files[1]) && strstr(manifest, "fnv1a64") &&
          strstr(manifest, "module,0,"));
    ps_report_destroy(report);
    const char *wrong[] = {argv[1],  argv[2],  "--runs", "comparison invalid",
                           files[0], files[2], NULL};
    CHECK(run(wrong) == 5);
    f = fopen(files[8], "rb");
    CHECK(f == NULL);
    /* Real runner reports: partial overlap, one common sample, no common samples. */
    const double starts[] = {1.5, 4, 5};
    const uint32_t counts[] = {26, 1, 0};
    for (size_t test = 0; test < 3; test++) {
        remove(files[3]);
        remove(files[4]);
        remove(files[5]);
        remove(files[6]);
        remove(files[11]);
        CHECK(fixture(files[1], 33, .1, 2, PS_METRE, starts[test]) == PS_OK);
        CHECK(run(args) == 0);
        report = NULL;
        CHECK(ps_report_load(files[4], &report) == PS_OK);
        ps_plot_info plot;
        ps_table_info table;
        CHECK(ps_report_plot_read(report, 2, &plot) == PS_OK &&
              plot.curves == (counts[test] ? 1u : 0u));
        CHECK(ps_report_table_read(report, 1, &table) == PS_OK && table.rows == plot.curves);
        if (counts[test]) {
            CHECK(ps_report_curve_read(report, 2, 0, &curve) == PS_OK &&
                  curve.count == counts[test]);
            for (uint32_t j = 0; j < curve.count; j++) {
                CHECK(fabs(curve.x[j] - starts[test] - j * .1) < 1e-12);
                CHECK(fabs(curve.y[j] - curve.x[j]) < 1e-12 && curve.x[j] <= 4);
            }
            CHECK(ps_report_row_read(report, 1, 0, &difference) == PS_OK &&
                  fabs(difference.values[0] - (starts[test] + 4) * .5) < 1e-12);
        } else {
            f = fopen(files[11], "rb");
            CHECK(f == NULL);
        }
        ps_report_destroy(report);
    }
    for (size_t i = 0; i < sizeof files / sizeof *files; i++)
        remove(files[i]);
    puts("Comparison: independent time grids, all samples, units, metadata and runner output "
         "passed");
    return 0;
}
