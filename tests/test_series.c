#include "physim/series.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Series line %d: %s\n", __LINE__, #x);                                 \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static const char *run_path = "series-fixture.psrun";
static void *deny_quantile_allocation(void *user, size_t bytes) {
    (void)user;
    (void)bytes;
    return NULL;
}
static void release_quantile_allocation(void *user, void *pointer, size_t bytes) {
    (void)user;
    (void)pointer;
    (void)bytes;
}
static ps_result fixture(bool monotone) {
    remove(run_path);
    ps_context c = {0};
    c.dt_s = .01;
    c.seed = 42;
    ps_channel_add(&c, "position.x", PS_METRE, "linear position");
    ps_channel_add(&c, "velocity.x", PS_VELOCITY, "constant velocity");
    ps_channel_add(&c, "offset,\"quoted\"", PS_METRE, "CSV label");
    ps_run_writer writer;
    ps_result r = ps_run_create(&writer, run_path, &c, "Series reference");
    if (r != PS_OK)
        return r;
    for (unsigned i = 0; i < 1025; i++) {
        double t = i * .01 + (i % 2) * .001;
        double v[] = {2 * t + 3, 2, 0};
        r = ps_run_append(&writer, !monotone && i == 512 ? 0 : t, v);
        if (r != PS_OK)
            break;
    }
    ps_result end = ps_run_close(&writer);
    return r == PS_OK ? end : r;
}
int main(int argc, char **argv) {
    CHECK(fixture(true) == PS_OK);
    ps_analysis_context *c = NULL, *other = NULL;
    CHECK(ps_analysis_create("series-test-ä", 0, &c) == PS_OK);
    CHECK(ps_analysis_create("series-other", 0, &other) == PS_OK);
    const double generated[] = {1, 2, 3}, doubled[] = {2, 4, 6};
    ps_series synthetic, aligned_values, unrelated, generated_sum;
    CHECK(ps_series_from_values(c, generated, 3, PS_METRE, "generated", &synthetic) == PS_OK);
    double quantile = -1;
    CHECK(ps_series_quantile(c, synthetic, 0, &quantile) == PS_OK && quantile == 1);
    CHECK(ps_series_quantile(c, synthetic, .25, &quantile) == PS_OK && quantile == 1.5);
    CHECK(ps_series_quantile(c, synthetic, .5, &quantile) == PS_OK && quantile == 2);
    CHECK(ps_series_quantile(c, synthetic, 1, &quantile) == PS_OK && quantile == 3);
    CHECK(ps_series_quantile(c, synthetic, -.1, &quantile) == PS_INVALID && quantile == 3);
    CHECK(ps_series_quantile(c, synthetic, NAN, &quantile) == PS_INVALID && quantile == 3);
    CHECK(ps_series_quantile(c, synthetic, INFINITY, &quantile) == PS_INVALID && quantile == 3);
    ps_series empty_series;
    CHECK(ps_series_slice(c, synthetic, 3, 0, &empty_series) == PS_OK);
    CHECK(ps_series_quantile(c, empty_series, .5, &quantile) == PS_INVALID && quantile == 3);
    CHECK(ps_series_release(c, empty_series) == PS_OK);
    ps_allocator denied = {NULL, deny_quantile_allocation, release_quantile_allocation};
    CHECK(ps_series_quantile_with_allocator(c, synthetic, .5, denied, &quantile) == PS_MEMORY &&
          quantile == 3);
    const double extremes[] = {-DBL_MAX, DBL_MAX};
    ps_series extreme_series;
    CHECK(ps_series_from_values(c, extremes, 2, PS_METRE, "extremes", &extreme_series) == PS_OK);
    CHECK(ps_series_quantile(c, extreme_series, .5, &quantile) == PS_OK && quantile == 0);
    CHECK(ps_series_release(c, extreme_series) == PS_OK);
    CHECK(ps_series_aligned_values(c, synthetic, doubled, 3, PS_METRE, "doubled",
                                   &aligned_values) == PS_OK);
    CHECK(ps_series_aligned(c, synthetic, aligned_values) == PS_OK);
    CHECK(ps_series_combine(c, PS_SERIES_ADD, synthetic, aligned_values, &generated_sum) == PS_OK);
    double generated_read[3];
    size_t generated_count;
    CHECK(ps_series_read(c, generated_sum, 0, generated_read, 3, &generated_count) == PS_OK &&
          generated_count == 3 && generated_read[0] == 3 && generated_read[2] == 9);
    CHECK(ps_series_from_values(c, generated, 3, PS_METRE, "other", &unrelated) == PS_OK);
    CHECK(ps_series_aligned(c, synthetic, unrelated) == PS_INVALID);
    CHECK(ps_series_aligned_values(other, synthetic, doubled, 3, PS_METRE, "wrong owner",
                                   &unrelated) == PS_INVALID);
    uint64_t bytes_before = ps_analysis_scratch_bytes(c);
    ps_series unchanged_synthetic = synthetic;
    const double invalid[] = {1, NAN, 3};
    CHECK(ps_series_from_values(c, invalid, 3, PS_METRE, "invalid", &unchanged_synthetic) ==
          PS_NUMERIC);
    CHECK(ps_series_aligned_values(c, synthetic, doubled, 2, PS_METRE, "short",
                                   &unchanged_synthetic) == PS_INVALID);
    CHECK(ps_analysis_scratch_bytes(c) == bytes_before &&
          !memcmp(&unchanged_synthetic, &synthetic, sizeof synthetic));
    CHECK(ps_series_release(c, synthetic) == PS_OK);
    CHECK(ps_series_describe(c, synthetic, &(ps_series_info){0}) == PS_INVALID);
    CHECK(ps_series_quantile(c, synthetic, .5, &quantile) == PS_INVALID && quantile == 0);
    CHECK(ps_series_aligned_values(c, synthetic, doubled, 3, PS_METRE, "stale",
                                   &unchanged_synthetic) == PS_INVALID);
    CHECK(ps_series_read(c, aligned_values, 2, generated_read, 1, &generated_count) == PS_OK &&
          generated_count == 1 && generated_read[0] == 6);
    CHECK(ps_series_release(c, aligned_values) == PS_OK);
    CHECK(ps_series_release(c, unrelated) == PS_OK);
    CHECK(ps_series_release(c, generated_sum) == PS_OK);
    CHECK(ps_analysis_scratch_bytes(c) == 0);
    ps_dataset ds;
    CHECK(ps_analysis_open_run(c, run_path, &ds) == PS_OK);
    if (argc > 1 && !strcmp(argv[1], "--exit-open")) {
        puts("open scratch file left to OS cleanup");
        return 0;
    }
    ps_dataset_info info;
    CHECK(ps_dataset_describe(c, ds, &info) == PS_OK && info.samples == 1025 &&
          info.channel_count == 3 && !info.recovered);
    CHECK(strstr(info.metadata, "seed=42") != NULL);
    CHECK(ps_analysis_scratch_bytes(c) == 1025 * 4 * 8);
    ps_series t, x, v, z, d, integral, smooth, tx, xx, shift, combined;
    CHECK(ps_dataset_series(c, ds, "time", &t) == PS_OK);
    CHECK(ps_dataset_series(c, ds, "position.x", &x) == PS_OK);
    CHECK(ps_dataset_series(c, ds, "velocity.x", &v) == PS_OK);
    CHECK(ps_dataset_series(c, ds, "offset,\"quoted\"", &z) == PS_OK);
    ps_series unchanged = x;
    CHECK(ps_dataset_series(c, ds, "missing", &unchanged) == PS_INVALID &&
          !memcmp(&unchanged, &x, sizeof x));
    double values[1025];
    size_t got = 77;
    CHECK(ps_series_read(c, t, 0, values, 1025, &got) == PS_OK && got == 1025);
    CHECK(ps_series_quantile(c, t, .5, &quantile) == PS_OK &&
          fabs(quantile - values[512]) < 1e-14);
    CHECK(fabs(values[256] - 2.56) < 1e-14 && fabs(values[257] - 2.571) < 1e-14);
    CHECK(ps_series_read(c, t, 1025, NULL, 0, &got) == PS_OK && got == 0);
    CHECK(ps_series_read(c, t, 1026, values, 1, &got) == PS_INVALID);
    CHECK(ps_series_derivative(c, x, t, &d) == PS_OK);
    CHECK(ps_series_read(c, d, 0, values, 1025, &got) == PS_OK);
    for (size_t i = 0; i < got; i++)
        CHECK(fabs(values[i] - 2) < 1e-11);
    ps_series_info si;
    CHECK(ps_series_describe(c, d, &si) == PS_OK &&
          !memcmp(si.dimension, PS_VELOCITY.dimension, 7));
    CHECK(ps_series_integral(c, v, t, (ps_quantity){3, PS_METRE}, &integral) == PS_OK);
    CHECK(ps_series_read(c, integral, 0, values, 1025, &got) == PS_OK);
    for (size_t i = 0; i < got; i++)
        CHECK(fabs(values[i] - (3 + 2 * (i * .01 + (i % 2) * .001))) < 1e-12);
    CHECK(ps_series_moving_average(c, x, 7, &smooth) == PS_OK);
    CHECK(ps_series_read(c, smooth, 0, values, 1025, &got) == PS_OK);
    for (size_t i = 0; i < got; i++) {
        double expected = 0;
        size_t begin = i >= 6 ? i - 6 : 0;
        for (size_t j = begin; j <= i; j++)
            expected += 3 + 2 * (j * .01 + (j % 2) * .001);
        CHECK(fabs(values[i] - expected / (i - begin + 1)) < 1e-12);
    }
    CHECK(ps_series_slice(c, t, 250, 520, &tx) == PS_OK &&
          ps_series_slice(c, x, 250, 520, &xx) == PS_OK);
    CHECK(ps_series_derivative(c, xx, tx, &combined) == PS_OK);
    CHECK(ps_series_read(c, combined, 255, values, 4, &got) == PS_OK && got == 4 &&
          fabs(values[2] - 2) < 1e-11);
    CHECK(ps_series_release(c, combined) == PS_OK);
    CHECK(ps_series_affine(c, x, 2, (ps_quantity){-1, PS_METRE}, &shift) == PS_OK);
    CHECK(ps_series_combine(c, PS_SERIES_SUBTRACT, x, integral, &combined) == PS_OK);
    ps_statistics stats;
    CHECK(ps_series_statistics(c, combined, &stats) == PS_OK && stats.count == 1025 &&
          fabs(stats.min) < 1e-12 && fabs(stats.max) < 1e-12);
    CHECK(ps_series_release(c, combined) == PS_OK);
    uint64_t bytes = ps_analysis_scratch_bytes(c);
    unchanged = x;
    CHECK(ps_series_combine(c, PS_SERIES_ADD, x, v, &unchanged) == PS_INVALID &&
          !memcmp(&unchanged, &x, sizeof x));
    CHECK(ps_series_combine(c, PS_SERIES_DIVIDE, x, z, &unchanged) == PS_NUMERIC &&
          !memcmp(&unchanged, &x, sizeof x));
    CHECK(ps_series_derivative(c, x, z, &unchanged) == PS_INVALID);
    CHECK(ps_series_integral(c, v, t, (ps_quantity){0, PS_SECOND}, &unchanged) == PS_INVALID);
    CHECK(ps_series_combine(c, PS_SERIES_SUBTRACT, x, xx, &unchanged) == PS_INVALID);
    CHECK(ps_analysis_scratch_bytes(c) == bytes);
    CHECK(ps_series_describe(other, x, &si) == PS_INVALID);
    ps_dataset ds2;
    ps_series x2;
    CHECK(ps_analysis_open_run(c, run_path, &ds2) == PS_OK &&
          ps_dataset_series(c, ds2, "position.x", &x2) == PS_OK);
    CHECK(ps_series_combine(c, PS_SERIES_ADD, x, x2, &unchanged) == PS_INVALID);
    CHECK(ps_dataset_close(c, ds2) == PS_OK);
    CHECK(ps_series_describe(c, x2, &si) == PS_INVALID);
    remove("series-export.csv");
    ps_series columns[] = {t, x, d, integral, z};
    CHECK(ps_series_export_csv(c, columns, 5, "series-export.csv") == PS_OK);
    CHECK(ps_series_export_csv(c, columns, 5, "series-export.csv") == PS_IO);
    FILE *csv = fopen("series-export.csv", "rb");
    CHECK(csv != NULL);
    char line[1024];
    unsigned rows = 0;
    CHECK(fgets(line, sizeof line, csv) != NULL && strstr(line, "offset,\"\"quoted\"\"") != NULL);
    while (fgets(line, sizeof line, csv))
        rows++;
    fclose(csv);
    CHECK(rows == 1025);
    ps_series stale = x;
    CHECK(ps_series_release(c, x) == PS_OK && ps_dataset_series(c, ds, "position.x", &x) == PS_OK);
    CHECK(ps_series_describe(c, stale, &si) == PS_INVALID);
    CHECK(ps_series_read(c, d, 256, values, 1, &got) == PS_OK && fabs(values[0] - 2) < 1e-11);
    CHECK(fixture(false) == PS_OK);
    CHECK(ps_series_read(c, t, 512, values, 1, &got) == PS_OK && fabs(values[0] - 5.12) < 1e-12);
    CHECK(fixture(true) == PS_OK);
    ps_series bound, persistent;
    CHECK(ps_series_aligned_values(c, t, values, 1025, PS_SECOND, "bound", &bound) == PS_OK);
    const double persistent_value = 7;
    CHECK(ps_series_from_values(c, &persistent_value, 1, PS_METRE, "persistent", &persistent) ==
          PS_OK);
    CHECK(ps_dataset_close(c, ds) == PS_OK && ps_analysis_scratch_bytes(c) == 8);
    CHECK(ps_series_describe(c, bound, &si) == PS_INVALID);
    CHECK(ps_series_read(c, persistent, 0, values, 1, &got) == PS_OK && got == 1 && values[0] == 7);
    CHECK(ps_series_release(c, persistent) == PS_OK && ps_analysis_scratch_bytes(c) == 0);
    CHECK(ps_series_read(c, d, 0, values, 1, &got) == PS_INVALID);
    CHECK(ps_analysis_open_run(c, run_path, &ds2) == PS_OK &&
          ps_dataset_describe(c, ds, &info) == PS_INVALID);
    ps_analysis_destroy(c);
    ps_analysis_destroy(other);
    CHECK(ps_analysis_create("series-quota", 128, &c) == PS_OK);
    ps_series limited = {0};
    CHECK(ps_series_from_values(c, values, 17, PS_METRE, "too many", &limited) == PS_LIMIT &&
          !ps_analysis_scratch_bytes(c) && !limited.owner);
    ps_dataset old = ds2;
    CHECK(ps_analysis_open_run(c, run_path, &ds2) == PS_LIMIT && !memcmp(&old, &ds2, sizeof ds2) &&
          !ps_analysis_scratch_bytes(c));
    ps_analysis_destroy(c);
    CHECK(fixture(false) == PS_OK && ps_analysis_create("series-invalid", 0, &c) == PS_OK);
    CHECK(ps_analysis_open_run(c, run_path, &ds) == PS_CORRUPT && !ps_analysis_scratch_bytes(c));
    ps_analysis_destroy(c);
    CHECK(fixture(true) == PS_OK);
    FILE *source = fopen(run_path, "rb");
    CHECK(source != NULL);
    CHECK(fseek(source, 0, SEEK_END) == 0);
    long size = ftell(source);
    CHECK(size > 20);
    rewind(source);
    remove("series-partial.psrun");
    FILE *partial = fopen("series-partial.psrun", "wbx");
    CHECK(partial != NULL);
    for (long i = 0; i < size - 20; i++) {
        int ch = fgetc(source);
        CHECK(ch != EOF && fputc(ch, partial) != EOF);
    }
    fclose(source);
    CHECK(!fclose(partial));
    CHECK(ps_analysis_create("series-recovery", 1025 * 4 * 8, &c) == PS_OK);
    CHECK(ps_analysis_open_run(c, "series-partial.psrun", &ds) == PS_RECOVERED);
    CHECK(ps_dataset_describe(c, ds, &info) == PS_OK && info.recovered && info.samples == 1025);
    CHECK(ps_dataset_series(c, ds, "position.x", &x) == PS_OK);
    unchanged = x;
    CHECK(ps_series_moving_average(c, x, 2, &unchanged) == PS_LIMIT &&
          !memcmp(&unchanged, &x, sizeof x));
    ps_analysis_destroy(c);
    remove("series-partial.psrun");
    remove(run_path);
    remove("series-export.csv");
    puts("Series: references across blocks, units, lifetime, alignment and quota passed");
    return 0;
}
