#include "physim/series.h"
#include "physim/report.h"
#include "platform.h"
#include "test_allocator.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "Streaming quantile line %d: %s\n", __LINE__, #x); return 1; } } while (0)
static int compare(const void *left, const void *right) {
    double a = *(const double *)left, b = *(const double *)right;
    return (a > b) - (a < b);
}
static int reference_cases(ps_analysis_context *ctx) {
    const size_t sizes[] = {1, 255, 256, 257, 513, 65537};
    const double probabilities[] = {0, .001, .1, .25, .5, .731, .9, nextafter(1., 0.), 1};
    const double special[] = {-DBL_MAX, DBL_MAX, -DBL_MIN, DBL_MIN, -DBL_TRUE_MIN,
                              DBL_TRUE_MIN, -0., 0., -1., 1.};
    for (unsigned variant = 0; variant < 2; variant++)
        for (size_t case_index = 0; case_index < sizeof sizes / sizeof *sizes; case_index++) {
            size_t count = sizes[case_index];
            double *values = malloc(count * sizeof *values), *flags = malloc(count * sizeof *flags);
            double *sorted = malloc(count * sizeof *sorted);
            CHECK(values && flags && sorted);
            size_t kept = 0;
            for (size_t i = 0; i < count; i++) {
                values[i] = i % 11 == 0 ? special[(i / 11) % 10]
                    : ldexp((double)((i * 104729) % 65537) - 32768., (int)(i % 31) - 15);
                flags[i] = !variant || i % 3 != 1;
                if (flags[i]) sorted[kept++] = values[i];
            }
            qsort(sorted, kept, sizeof *sorted, compare);
            ps_series input, selector, masked;
            CHECK(ps_series_from_values(ctx, values, count, PS_ONE, "reference", &input) == PS_OK);
            CHECK(ps_series_aligned_values(ctx, input, flags, count, PS_ONE, "valid", &selector) == PS_OK);
            CHECK(ps_series_mask(ctx, input, selector, 1, &masked) == PS_OK);
            uint64_t scratch = ps_analysis_scratch_bytes(ctx);
            test_allocator temporary = {.budget = 6400};
            for (size_t j = 0; j < sizeof probabilities / sizeof *probabilities; j++) {
                double p = probabilities[j], at = (double)(kept - 1) * p;
                size_t lower = (size_t)at;
                double fraction = at - (double)lower;
                double expected = lower >= kept - 1 || fraction == 0 ? sorted[lower]
                    : (1 - fraction) * sorted[lower] + fraction * sorted[lower + 1];
                double actual = 42;
                CHECK(ps_series_quantile_with_allocator(ctx, masked, p, test_domain(&temporary), &actual) == PS_OK);
                CHECK(actual == expected);
                CHECK(!temporary.invalid && !temporary.live_bytes && temporary.peak_bytes <= 6400);
                CHECK(ps_analysis_scratch_bytes(ctx) == scratch);
            }
            double preserved = 42;
            if (count > 256) {
                temporary.budget = 6399;
                CHECK(ps_series_quantile_with_allocator(ctx, masked, .5, test_domain(&temporary), &preserved) == PS_MEMORY);
                CHECK(preserved == 42 && !temporary.live_bytes && ps_analysis_scratch_bytes(ctx) == scratch);
                temporary.budget = 6400;
            }
            temporary.fail_on = temporary.attempts + 1;
            CHECK(ps_series_quantile_with_allocator(ctx, masked, .5, test_domain(&temporary), &preserved) == PS_MEMORY);
            CHECK(preserved == 42 && !temporary.live_bytes && ps_analysis_scratch_bytes(ctx) == scratch);
            CHECK(ps_series_release(ctx, masked) == PS_OK);
            CHECK(ps_series_release(ctx, selector) == PS_OK);
            CHECK(ps_series_release(ctx, input) == PS_OK);
            free(values); free(flags); free(sorted);
        }
    return 0;
}
static int constant_cases(ps_analysis_context *ctx) {
    double values[257], flags[257] = {0};
    for (unsigned i = 0; i < 257; i++) values[i] = 7;
    ps_series input, selector, masked;
    CHECK(ps_series_from_values(ctx, values, 257, PS_ONE, "constant", &input) == PS_OK);
    CHECK(ps_series_aligned_values(ctx, input, flags, 257, PS_ONE, "missing", &selector) == PS_OK);
    CHECK(ps_series_mask(ctx, input, selector, 1, &masked) == PS_OK);
    double result = 42;
    CHECK(ps_series_quantile(ctx, masked, .5, &result) == PS_INVALID && result == 42);
    CHECK(ps_series_quantile(ctx, input, .731, &result) == PS_OK && result == 7);
    CHECK(ps_series_release(ctx, masked) == PS_OK && ps_series_release(ctx, selector) == PS_OK);
    flags[256] = 1;
    CHECK(ps_series_aligned_values(ctx, input, flags, 257, PS_ONE, "one valid", &selector) == PS_OK);
    CHECK(ps_series_mask(ctx, input, selector, 1, &masked) == PS_OK);
    CHECK(ps_series_quantile(ctx, masked, .731, &result) == PS_OK && result == 7);
    CHECK(ps_series_release(ctx, masked) == PS_OK && ps_series_release(ctx, selector) == PS_OK);
    CHECK(ps_series_release(ctx, input) == PS_OK);
    return 0;
}
static int language_analysis(const char *runner, const char *module) {
    const char *args[] = {runner, module, "large.psrun", "language-quantile", NULL};
    ps_process child = {0};
    CHECK(ps_process_start(&child, args, NULL));
    double until = ps_clock() + 90;
    char output[4096];
    while (ps_clock() < until) {
        int running = ps_process_poll(&child), n = ps_process_read(&child, output, sizeof output);
        if (n > 0) fwrite(output, 1, (size_t)n, stdout);
        if (!running && n <= 0) break;
        ps_sleep(1);
    }
    bool ok = !child.running && child.exit_code == 0;
    if (child.running) ps_process_kill(&child);
    ps_process_close(&child);
    CHECK(ok);
    ps_report *report = NULL;
    CHECK(ps_report_load("language-quantile.psreport", &report) == PS_OK);
    uint32_t plots = 0, tables = 0;
    CHECK(ps_report_describe(report, NULL, NULL, &plots, &tables) == PS_OK && plots == 1);
    ps_curve_data histogram;
    CHECK(ps_report_curve_read(report, 0, 0, &histogram) == PS_OK && histogram.count == 17);
    uint64_t count = 0;
    for (unsigned i = 0; i < histogram.count; i++) count += (uint64_t)histogram.y[i];
    CHECK(count == 524289);
    ps_report_destroy(report);
    return 0;
}
static int large_dataset(ps_analysis_context *ctx, test_allocator *allocation,
                          const char *runner, const char *module) {
    const unsigned count = 1048577;
    ps_context context = {.struct_size = sizeof context, .api_version = PS_API_VERSION, .dt_s = 1, .seed = 17};
    CHECK(ps_channel_add(&context, "position.x", PS_METRE, "Position α") == 0);
    CHECK(ps_channel_add(&context, "valid", PS_ONE, "Observation mask") == 1);
    ps_run_writer writer;
    CHECK(ps_run_create(&writer, "large.psrun", &context, "Large block analysis α") == PS_OK);
    for (unsigned i = 0; i < count; i++) {
        double values[] = {(double)(count - 1 - i), i % 2 == 0};
        CHECK(ps_run_append(&writer, i, values) == PS_OK);
    }
    CHECK(ps_run_close(&writer) == PS_OK);
    ps_dataset dataset;
    CHECK(ps_analysis_open_run(ctx, "large.psrun", &dataset) == PS_OK);
    ps_dataset_info info;
    CHECK(ps_dataset_describe(ctx, dataset, &info) == PS_OK && info.samples == count &&
          info.channel_count == 2 && !info.recovered && strstr(info.metadata, "seed=17\n"));
    CHECK(!strcmp(info.channels[0].name, "position.x") && !strcmp(info.channels[0].description, "Position α") &&
          !strcmp(info.channels[0].unit, "m") && !memcmp(info.channels[0].dimension, PS_METRE.dimension, 7));
    strcpy(info.channels[0].name, "changed copy"); info.metadata[0] = 0;
    CHECK(ps_dataset_describe(ctx, dataset, &info) == PS_OK &&
          !strcmp(info.channels[0].name, "position.x") && strstr(info.metadata, "seed=17\n"));
    ps_series position, time, valid, masked;
    CHECK(ps_dataset_series(ctx, dataset, "position.x", &position) == PS_OK);
    CHECK(ps_dataset_series(ctx, dataset, "time", &time) == PS_OK);
    CHECK(ps_dataset_series(ctx, dataset, "valid", &valid) == PS_OK);
    CHECK(ps_series_mask(ctx, position, valid, 1, &masked) == PS_OK);
    double block[258]; size_t got = 0;
    CHECK(ps_series_read(ctx, position, 255, block, 258, &got) == PS_OK && got == 258);
    for (size_t i = 0; i < got; i++) CHECK(block[i] == (double)(count - 1 - 255 - i));
    CHECK(ps_series_read(ctx, time, count - 3, block, 258, &got) == PS_OK && got == 3);
    CHECK(block[0] == count - 3 && block[2] == count - 1);
    uint64_t scratch = ps_analysis_scratch_bytes(ctx);
    size_t baseline = allocation->live_bytes;
    allocation->budget = baseline + 6400;
    ps_series derivative, integral, average, transformed;
    CHECK(ps_series_derivative(ctx, position, time, &derivative) == PS_OK);
    ps_statistics statistics;
    CHECK(ps_series_statistics(ctx, derivative, &statistics) == PS_OK &&
          statistics.count == count && statistics.mean == -1 && statistics.m2 == 0);
    ps_unit area_unit;
    CHECK(ps_unit_multiply(PS_METRE, PS_SECOND, "m*s", &area_unit) == PS_OK);
    CHECK(ps_series_integral(ctx, position, time, (ps_quantity){0, area_unit}, &integral) == PS_OK);
    CHECK(ps_series_read(ctx, integral, count - 1, block, 1, &got) == PS_OK && got == 1 &&
          block[0] == .5 * (count - 1) * (count - 1));
    CHECK(ps_series_moving_average(ctx, position, 5, &average) == PS_OK);
    CHECK(ps_series_read(ctx, average, 255, block, 258, &got) == PS_OK && got == 258);
    for (size_t i = 0; i < got; i++) CHECK(block[i] == (double)(count + 1 - 255 - i));
    CHECK(ps_series_affine(ctx, position, 2, (ps_quantity){3, PS_METRE}, &transformed) == PS_OK);
    CHECK(ps_series_read(ctx, transformed, count - 1, block, 1, &got) == PS_OK && got == 1 && block[0] == 3);
    CHECK(ps_series_release(ctx, derivative) == PS_OK && ps_series_release(ctx, integral) == PS_OK &&
          ps_series_release(ctx, average) == PS_OK && ps_series_release(ctx, transformed) == PS_OK);
    CHECK(ps_analysis_scratch_bytes(ctx) == scratch);
    double result = -1;
    CHECK(ps_series_quantile(ctx, position, .5, &result) == PS_OK && result == 524288);
    CHECK(ps_series_quantile(ctx, masked, .731, &result) == PS_OK && fabs(result - .731 * 1048576) < 1e-9);
    CHECK(allocation->live_bytes == baseline && allocation->peak_bytes <= baseline + 6400 &&
          !allocation->invalid && ps_analysis_scratch_bytes(ctx) == scratch);
    CHECK(ps_dataset_close(ctx, dataset) == PS_OK && ps_analysis_scratch_bytes(ctx) == 0);
    CHECK(ps_series_quantile(ctx, position, .5, &result) == PS_INVALID);
    if (runner) CHECK(language_analysis(runner, module) == 0);
    CHECK(remove("large.psrun") == 0);
    printf("Streaming quantile: %u rows; context %zu bytes, peak %zu bytes; scratch restored\n", count, baseline, allocation->peak_bytes);
    return 0;
}
int main(int argc, char **argv) {
    CHECK(argc == 1 || argc == 3);
    test_allocator allocation = {0}; ps_analysis_context *ctx = NULL;
    CHECK(ps_analysis_create_with_allocator("quantile", UINT64_C(134217728), test_domain(&allocation), &ctx) == PS_OK);
    CHECK(reference_cases(ctx) == 0);
    CHECK(constant_cases(ctx) == 0);
    CHECK(large_dataset(ctx, &allocation, argc == 3 ? argv[1] : NULL, argc == 3 ? argv[2] : NULL) == 0);
    ps_analysis_destroy(ctx);
    CHECK(!allocation.invalid && !allocation.live_bytes && !allocation.live_blocks);
    puts("Streaming quantile: independently sorted references, masks, allocation failure and large dataset passed");
    return 0;
}
