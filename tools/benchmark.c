#include "benchmark_build.h"
#include "physim/report.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* A private, exclusive directory preserves failed evidence and never overwrites
 * user data. All timed operations use the production public API. */
static char directory[256];
static volatile double sink;
static int failed;
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Benchmark failed at line %d: %s (files: %s)\n", __LINE__, #x,         \
                    directory);                                                                    \
            failed = 1;                                                                            \
            goto cleanup;                                                                          \
        }                                                                                          \
    } while (0)

static void row(const char *name, unsigned repeat, uint64_t units, double start, uint64_t scratch) {
    double seconds = ps_clock() - start;
    printf("%s,%u,%llu,%.9f,%.3f,%llu,%s,%s\n", name, repeat, (unsigned long long)units, seconds,
           seconds > 0 ? (double)units / seconds : 0, (unsigned long long)scratch,
           PS_BENCH_COMPILER, PS_BENCH_CONFIG);
}

static void run(unsigned repeat, uint64_t count) {
    char path[320], prefix[320];
    snprintf(path, sizeof path, "%s/run-%u.psrun", directory, repeat);
    snprintf(prefix, sizeof prefix, "%s/scratch-%u", directory, repeat);
    ps_context context = {0};
    ps_run_writer writer = {0};
    ps_run_reader reader = {0};
    ps_analysis_context *analysis = NULL;
    ps_report *report = NULL;
    ps_curve_data *curve = NULL;
    context.dt_s = 0.125;
    context.seed = 42;
    for (unsigned c = 0; c < PS_MAX_CHANNELS; ++c) {
        char name[32];
        snprintf(name, sizeof name, "channel%u", c);
        CHECK(ps_channel_add(&context, name, PS_METRE, "linear reference") == (int)c);
    }
    double start = ps_clock();
    CHECK(ps_run_create(&writer, path, &context, "performance-reference") == PS_OK);
    for (uint64_t i = 0; i < count; ++i) {
        double values[PS_MAX_CHANNELS];
        for (unsigned c = 0; c < PS_MAX_CHANNELS; ++c)
            values[c] = (double)i * (double)(c + 1) * 0.125;
        CHECK(ps_run_append(&writer, (double)i * 0.125, values) == PS_OK);
    }
    CHECK(ps_run_close(&writer) == PS_OK);
    row("write_16_channels", repeat, count, start, 0);

    /* Full roundtrip validation, including every channel and final marker. */
    start = ps_clock();
    CHECK(ps_run_open(&reader, path) == PS_OK);
    CHECK(reader.channels == PS_MAX_CHANNELS);
    for (uint64_t i = 0; i < count; ++i) {
        double time, values[PS_MAX_CHANNELS];
        CHECK(ps_run_next(&reader, &time, values) == PS_OK);
        CHECK(time == (double)i * 0.125);
        for (unsigned c = 0; c < PS_MAX_CHANNELS; ++c)
            CHECK(values[c] == (double)i * (double)(c + 1) * 0.125);
    }
    double time, values[PS_MAX_CHANNELS];
    CHECK(ps_run_next(&reader, &time, values) == PS_EOF);
    CHECK(reader.complete && reader.samples == count);
    ps_run_reader_close(&reader);
    row("read_validate_16_channels", repeat, count, start, 0);

    CHECK(ps_analysis_create(prefix, 0, &analysis) == PS_OK);
    ps_dataset dataset;
    start = ps_clock();
    CHECK(ps_analysis_open_run(analysis, path, &dataset) == PS_OK);
    row("analysis_snapshot", repeat, count, start, ps_analysis_scratch_bytes(analysis));
    ps_series x, y, derivative;
    CHECK(ps_dataset_series(analysis, dataset, "time", &x) == PS_OK);
    CHECK(ps_dataset_series(analysis, dataset, "channel15", &y) == PS_OK);
    ps_statistics stats = {0};
    start = ps_clock();
    CHECK(ps_series_statistics(analysis, y, &stats) == PS_OK);
    CHECK(stats.count == count && stats.min == 0 && stats.max == 2.0 * (double)(count - 1));
    CHECK(fabs(stats.mean - (double)(count - 1)) < 1e-9);
    row("series_statistics", repeat, count, start, ps_analysis_scratch_bytes(analysis));
    start = ps_clock();
    CHECK(ps_series_derivative(analysis, y, x, &derivative) == PS_OK);
    row("series_derivative", repeat, count, start, ps_analysis_scratch_bytes(analysis));
    CHECK(ps_series_statistics(analysis, derivative, &stats) == PS_OK);
    CHECK(stats.count == count && stats.min == 16 && stats.max == 16);

    CHECK(ps_report_create("Benchmark", "seed=42", &report) == PS_OK);
    ps_plot_info info = {0};
    strcpy(info.title, "Linear reference");
    strcpy(info.x_label, "Time");
    strcpy(info.y_label, "Position");
    CHECK(ps_report_unit_from(PS_SECOND, &info.x_unit) == PS_OK);
    CHECK(ps_report_unit_from(PS_METRE, &info.y_unit) == PS_OK);
    ps_plot_handle plot;
    CHECK(ps_report_add_plot(report, &info, &plot) == PS_OK);
    start = ps_clock();
    for (unsigned c = 0; c < PS_REPORT_MAX_CURVES; ++c) {
        char label[32];
        snprintf(label, sizeof label, "curve%u", c);
        CHECK(ps_report_add_series(report, plot, analysis, x, y, label, PS_PLOT_LINE) == PS_OK);
    }
    row("report_preview_8_curves", repeat, count * PS_REPORT_MAX_CURVES, start,
        ps_analysis_scratch_bytes(analysis));
    curve = malloc(sizeof *curve);
    CHECK(curve != NULL);
    CHECK(ps_report_curve_read(report, 0, 0, curve) == PS_OK);
    CHECK(curve->source_count == count && curve->count <= PS_REPORT_MAX_POINTS);
    CHECK(curve->x[0] == 0 && curve->y[curve->count - 1] == 2.0 * (double)(count - 1));
    start = ps_clock();
    for (unsigned frame = 0; frame < 10000; ++frame)
        for (unsigned c = 0; c < PS_REPORT_MAX_CURVES; ++c) {
            CHECK(ps_report_curve_read(report, 0, c, curve) == PS_OK);
            sink += curve->y[curve->count - 1];
        }
    row("report_curve_copy", repeat, 10000 * PS_REPORT_MAX_CURVES, start, 0);
    for (unsigned c = 0; c < PS_REPORT_MAX_CURVES; ++c) {
        const ps_curve_data *view = NULL;
        CHECK(ps_report_curve_view(report, 0, c, &view) == PS_OK);
        CHECK(ps_report_curve_read(report, 0, c, curve) == PS_OK);
        CHECK(!memcmp(view, curve, sizeof *curve));
    }
    start = ps_clock();
    for (unsigned frame = 0; frame < 10000; ++frame)
        for (unsigned c = 0; c < PS_REPORT_MAX_CURVES; ++c) {
            const ps_curve_data *view = NULL;
            CHECK(ps_report_curve_view(report, 0, c, &view) == PS_OK);
            sink += view->y[view->count - 1];
        }
    row("report_curve_view", repeat, 10000 * PS_REPORT_MAX_CURVES, start, 0);
cleanup:
    free(curve);
    ps_report_destroy(report);
    ps_analysis_destroy(analysis);
    if (reader.file)
        ps_run_reader_close(&reader);
    if (writer.file)
        ps_run_close(&writer);
    if (!failed && remove(path)) {
        fprintf(stderr, "Cannot remove benchmark data: %s\n", path);
        failed = 1;
    }
}

static bool parse(const char *text, unsigned long maximum, unsigned long *out) {
    if (!text || !*text)
        return false;
    unsigned long value = 0;
    for (const char *p = text; *p; ++p) {
        if (*p < '0' || *p > '9' || value > (maximum - (unsigned long)(*p - '0')) / 10)
            return false;
        value = value * 10 + (unsigned long)(*p - '0');
    }
    if (!value || value > maximum)
        return false;
    *out = value;
    return true;
}

int main(int argc, char **argv) {
    unsigned long samples = 100000, repeats = 5;
    if (argc > 3 || (argc > 1 && !parse(argv[1], 1000000, &samples)) || samples < 2 ||
        (argc > 2 && !parse(argv[2], 30, &repeats))) {
        fprintf(stderr, "Usage: physim-benchmark [samples:2..1000000] [repeats:1..30]\n");
        return 2;
    }
    bool created = false;
    for (unsigned attempt = 0; attempt < 100; ++attempt) {
        snprintf(directory, sizeof directory, "physim-benchmark-%.0f-%u", ps_clock() * 1e6,
                 attempt);
        if (ps_make_directory_exclusive(directory)) {
            created = true;
            break;
        }
    }
    if (!created) {
        fprintf(stderr, "Cannot create benchmark directory\n");
        return 1;
    }
    printf("workload,repeat,units,seconds,units_per_second,scratch_bytes,compiler,configuration\n");
    for (unsigned i = 0; i < repeats && !failed; ++i)
        run(i, samples);
    fprintf(stderr, "Benchmark %s; private directory: %s\n", failed ? "FAILED" : "verified",
            directory);
    return failed || fflush(stdout) != 0 ? 1 : 0;
}
