#include "physim/series.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Select line %d: %s\n", __LINE__, #x);                                 \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
#define N 1025u
static int fixture(void) {
    remove("select.psrun");
    ps_context context = {0};
    context.dt_s = .01;
    CHECK(ps_channel_add(&context, "value", PS_METRE, "measurement") == PS_OK);
    CHECK(ps_channel_add(&context, "status", PS_ONE, "validity") == 1);
    ps_run_writer writer;
    CHECK(ps_run_create(&writer, "select.psrun", &context, "Masked measurement") == PS_OK);
    for (unsigned i = 0; i < N; i++) {
        double status = i % 3 == 0 ? 1 : i % 3 == 1 ? 0 : 2;
        double values[] = {status == 1 ? 2 * (double)i + 3 : -9999, status};
        CHECK(ps_run_append(&writer, (double)i, values) == PS_OK);
    }
    CHECK(ps_run_close(&writer) == PS_OK);
    return 0;
}
int main(void) {
    CHECK(!fixture());
    ps_analysis_context *c;
    CHECK(ps_analysis_create("select-test", 0, &c) == PS_OK);
    ps_dataset ds;
    CHECK(ps_analysis_open_run(c, "select.psrun", &ds) == PS_OK);
    ps_series t, y, mask;
    CHECK(ps_dataset_series(c, ds, "time", &t) == PS_OK);
    CHECK(ps_dataset_series(c, ds, "value", &y) == PS_OK);
    CHECK(ps_dataset_series(c, ds, "status", &mask) == PS_OK);
    uint64_t base = ps_analysis_scratch_bytes(c);
    ps_series columns[] = {t, y, mask}, chosen[3];
    CHECK(ps_series_select(c, columns, 3, mask, 1, chosen) == PS_OK);
    const size_t selected = (N + 2) / 3;
    CHECK(ps_analysis_scratch_bytes(c) == base + selected * 3 * 8);
    ps_series_info info;
    CHECK(ps_series_describe(c, chosen[1], &info) == PS_OK && info.count == selected &&
          !memcmp(info.dimension, PS_METRE.dimension, 7) && info.scale == 1);
    double values[N];
    size_t got;
    CHECK(ps_series_read(c, chosen[1], 0, values, N, &got) == PS_OK && got == selected);
    for (size_t i = 0; i < got; i++)
        CHECK(values[i] == 6 * (double)i + 3);
    CHECK(ps_series_aligned(c, chosen[0], chosen[1]) == PS_OK);
    CHECK(ps_series_aligned(c, chosen[1], y) == PS_INVALID);
    ps_series derivative;
    CHECK(ps_series_derivative(c, chosen[1], chosen[0], &derivative) == PS_OK);
    CHECK(ps_series_read(c, derivative, 0, values, N, &got) == PS_OK && got == selected);
    for (size_t i = 0; i < got; i++)
        CHECK(values[i] == 2);
    ps_statistics stats;
    CHECK(ps_series_statistics(c, chosen[1], &stats) == PS_OK && stats.count == selected &&
          stats.min == 3 && stats.max == 2049 && stats.mean == 1026);
    ps_series again[3] = {t, y, mask};
    CHECK(ps_series_select(c, again, 3, mask, 1, again) == PS_OK);
    CHECK(ps_series_aligned(c, again[0], chosen[0]) == PS_INVALID);
    ps_series slices[2];
    CHECK(ps_series_slice(c, chosen[0], 80, 200, &slices[0]) == PS_OK);
    CHECK(ps_series_slice(c, chosen[1], 80, 200, &slices[1]) == PS_OK);
    CHECK(ps_series_aligned(c, slices[0], slices[1]) == PS_OK);
    ps_series nested[2];
    CHECK(ps_series_select(c, chosen, 2, chosen[2], 1, nested) == PS_OK);
    CHECK(ps_series_aligned(c, nested[0], chosen[0]) == PS_INVALID);
    CHECK(ps_series_aligned(c, nested[0], nested[1]) == PS_OK);
    ps_series resampled;
    CHECK(ps_series_resample_linear(c, y, t, chosen[0], &resampled) == PS_OK);
    CHECK(ps_series_aligned(c, resampled, chosen[0]) == PS_OK);
    CHECK(ps_series_aligned(c, resampled, again[0]) == PS_INVALID);
    ps_series empty[3];
    CHECK(ps_series_select(c, columns, 3, mask, 7, empty) == PS_OK);
    CHECK(ps_series_read(c, empty[0], 0, NULL, 0, &got) == PS_OK && !got);
    CHECK(ps_series_aligned(c, empty[0], empty[1]) == PS_OK);
    ps_series many[32];
    for (unsigned i = 0; i < 32; i++)
        many[i] = t;
    CHECK(ps_series_select(c, many, 32, mask, 0, many) == PS_OK);
    for (unsigned i = 0; i < 32; i++) {
        CHECK(ps_series_read(c, many[i], 0, values, N, &got) == PS_OK && got == 342);
        for (size_t j = 0; j < got; j++)
            CHECK(values[j] == 3 * (double)j + 1);
        CHECK(ps_series_release(c, many[i]) == PS_OK);
    }
    ps_series zeros, full[2];
    CHECK(ps_series_affine(c, mask, 0, (ps_quantity){0, PS_ONE}, &zeros) == PS_OK);
    CHECK(ps_series_select(c, columns, 2, zeros, 0, full) == PS_OK);
    CHECK(ps_series_describe(c, full[0], &info) == PS_OK && info.count == N);
    CHECK(ps_series_aligned(c, full[0], t) == PS_INVALID);
    ps_series untouched[3];
    memcpy(untouched, chosen, sizeof chosen);
    uint64_t bytes = ps_analysis_scratch_bytes(c);
    CHECK(ps_series_select(c, columns, 0, mask, 1, untouched) == PS_INVALID);
    CHECK(ps_series_select(c, columns, 33, mask, 1, untouched) == PS_INVALID);
    CHECK(ps_series_select(c, columns, 3, mask, NAN, untouched) == PS_INVALID);
    CHECK(ps_series_select(c, columns, 3, y, 1, untouched) == PS_INVALID);
    CHECK(ps_series_select(c, slices, 2, mask, 1, untouched) == PS_INVALID);
    CHECK(ps_series_select(NULL, columns, 3, mask, 1, untouched) == PS_INVALID);
    ps_dataset second;
    ps_series foreign;
    CHECK(ps_analysis_open_run(c, "select.psrun", &second) == PS_OK);
    CHECK(ps_dataset_series(c, second, "status", &foreign) == PS_OK);
    CHECK(ps_series_select(c, columns, 3, foreign, 1, untouched) == PS_INVALID);
    CHECK(ps_dataset_close(c, second) == PS_OK);
    CHECK(!memcmp(untouched, chosen, sizeof chosen) && ps_analysis_scratch_bytes(c) == bytes);
    remove("select.csv");
    CHECK(ps_series_export_csv(c, chosen, 3, "select.csv") == PS_OK);
    FILE *csv = fopen("select.csv", "rb");
    CHECK(csv);
    char line[256];
    CHECK(fgets(line, sizeof line, csv));
    for (size_t i = 0; i < selected; i++) {
        double x, v, s;
        CHECK(fgets(line, sizeof line, csv) && sscanf(line, "%lf,%lf,%lf", &x, &v, &s) == 3);
        CHECK(x == 3 * (double)i && v == 2 * x + 3 && s == 1);
    }
    CHECK(!fgets(line, sizeof line, csv));
    fclose(csv);
    CHECK(ps_series_release(c, mask) == PS_OK && ps_series_release(c, y) == PS_OK);
    CHECK(ps_series_read(c, chosen[1], selected - 1, values, 1, &got) == PS_OK &&
          values[0] == 2049);
    CHECK(ps_series_select(c, columns, 3, mask, 1, untouched) == PS_INVALID);
    CHECK(ps_dataset_close(c, ds) == PS_OK && !ps_analysis_scratch_bytes(c));
    CHECK(ps_series_describe(c, chosen[0], &info) == PS_INVALID);
    ps_analysis_destroy(c);
    /* Exact aggregate quota: a failed group must not consume files or handles. */
    for (unsigned shortfall = 0; shortfall < 2; shortfall++) {
        CHECK(ps_analysis_create("select-quota", base + selected * 2 * 8 - shortfall, &c) == PS_OK);
        CHECK(ps_analysis_open_run(c, "select.psrun", &ds) == PS_OK);
        CHECK(ps_dataset_series(c, ds, "time", &columns[0]) == PS_OK);
        CHECK(ps_dataset_series(c, ds, "value", &columns[1]) == PS_OK);
        CHECK(ps_dataset_series(c, ds, "status", &mask) == PS_OK);
        memcpy(untouched, chosen, sizeof chosen);
        CHECK(ps_series_select(c, columns, 2, mask, 1, untouched) ==
              (shortfall ? PS_LIMIT : PS_OK));
        if (shortfall)
            CHECK(!memcmp(untouched, chosen, sizeof chosen) &&
                  ps_analysis_scratch_bytes(c) == base);
        ps_analysis_destroy(c);
    }
    /* Insufficient slots must not publish a partial group. */
    CHECK(ps_analysis_create("select-slots", 0, &c) == PS_OK);
    CHECK(ps_analysis_open_run(c, "select.psrun", &ds) == PS_OK);
    CHECK(ps_dataset_series(c, ds, "status", &mask) == PS_OK);
    for (unsigned i = 1; i < PS_ANALYSIS_MAX_SERIES - 1; i++)
        CHECK(ps_dataset_series(c, ds, "time", &t) == PS_OK);
    columns[0] = columns[1] = t;
    memcpy(untouched, chosen, sizeof chosen);
    CHECK(ps_series_select(c, columns, 2, mask, 1, untouched) == PS_LIMIT);
    CHECK(!memcmp(untouched, chosen, sizeof chosen) && ps_analysis_scratch_bytes(c) == base);
    CHECK(ps_series_select(c, columns, 1, mask, 1, untouched) == PS_OK);
    ps_analysis_destroy(c);
    remove("select.psrun");
    remove("select.csv");
    puts("Selection: gaps, alignment, units, lifetime, CSV, quota and slots passed");
    return 0;
}
