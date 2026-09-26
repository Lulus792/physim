#include "physim/series.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Resample line %d: %s\n", __LINE__, #x);                               \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static ps_result fixture(const char *path, size_t count, double step, int mode) {
    remove(path);
    ps_context context = {0};
    context.dt_s = 1;
    ps_channel_add(&context, "axis", PS_SECOND, "interpolation coordinate");
    ps_channel_add(&context, "value", PS_METRE, "linear reference");
    ps_run_writer writer;
    ps_result r = ps_run_create(&writer, path, &context, "Resampling reference");
    if (r != PS_OK)
        return r;
    for (size_t i = 0; i < count && r == PS_OK; i++) {
        double x = (double)i * step, y = 3 * x - 2;
        if (mode == 1 && i == 700)
            x = 0; /* defect after the requested prefix */
        if (mode == 2) {
            x = i ? DBL_MAX : -DBL_MAX;
            y = x;
        }
        if (mode == 3) {
            x = 0;
            y = 7;
        }
        if (mode == 4) {
            y = x * x;
        }
        double values[] = {x, y};
        r = ps_run_append(&writer, (double)i, values);
    }
    ps_result closed = ps_run_close(&writer);
    return r == PS_OK ? closed : r;
}
static ps_result open_series(ps_analysis_context *c, const char *path, ps_dataset *d, ps_series *x,
                             ps_series *y) {
    ps_result r = ps_analysis_open_run(c, path, d);
    if (r == PS_OK)
        r = ps_dataset_series(c, *d, "axis", x);
    if (r == PS_OK)
        r = ps_dataset_series(c, *d, "value", y);
    return r;
}
int main(void) {
    const char *source = "resample-source.psrun", *target = "resample-target.psrun";
    CHECK(fixture(source, 769, .5, 0) == PS_OK);
    CHECK(fixture(target, 1537, .25, 0) == PS_OK);
    ps_analysis_context *c = NULL;
    CHECK(ps_analysis_create("resample-work", 0, &c) == PS_OK);
    ps_dataset a, b;
    ps_series x, y, q, reference, interpolated;
    CHECK(open_series(c, source, &a, &x, &y) == PS_OK);
    CHECK(open_series(c, target, &b, &q, &reference) == PS_OK);
    uint64_t bytes = ps_analysis_scratch_bytes(c);
    CHECK(ps_series_resample_linear(c, y, x, q, &interpolated) == PS_OK);
    CHECK(ps_analysis_scratch_bytes(c) == bytes + 1537 * 8);
    CHECK(ps_series_aligned(c, q, interpolated) == PS_OK);
    CHECK(ps_series_aligned(c, x, interpolated) == PS_INVALID);
    ps_series_info info;
    CHECK(ps_series_describe(c, interpolated, &info) == PS_OK && info.count == 1537 &&
          info.dimension[0] == 1 && info.scale == 1);
    ps_series difference;
    CHECK(ps_series_combine(c, PS_SERIES_SUBTRACT, interpolated, reference, &difference) == PS_OK);
    ps_statistics stats;
    CHECK(ps_series_statistics(c, difference, &stats) == PS_OK && stats.min == 0 && stats.max == 0);
    CHECK(ps_series_release(c, difference) == PS_OK);
    ps_series sliced_x, sliced_y, sliced_q, sliced;
    CHECK(ps_series_slice(c, x, 100, 400, &sliced_x) == PS_OK);
    CHECK(ps_series_slice(c, y, 100, 400, &sliced_y) == PS_OK);
    CHECK(ps_series_slice(c, q, 201, 700, &sliced_q) == PS_OK);
    CHECK(ps_series_resample_linear(c, sliced_y, sliced_x, sliced_q, &sliced) == PS_OK);
    CHECK(ps_series_aligned(c, sliced, sliced_q) == PS_OK);
    double values[1537];
    size_t got;
    CHECK(ps_series_read(c, sliced, 0, values, 1537, &got) == PS_OK && got == 700);
    for (size_t i = 0; i < got; i++)
        CHECK(values[i] == 3 * ((double)i + 201) * .25 - 2);
    ps_series keep = interpolated;
    bytes = ps_analysis_scratch_bytes(c);
    CHECK(ps_series_resample_linear(c, y, x, reference, &keep) == PS_INVALID);
    CHECK(ps_series_resample_linear(c, sliced_y, x, q, &keep) == PS_INVALID);
    CHECK(ps_series_resample_linear(c, y, x, (ps_series){0}, &keep) == PS_INVALID);
    CHECK(ps_series_resample_linear(c, y, x, q, NULL) == PS_INVALID);
    CHECK(ps_series_resample(c, y, x, q, (ps_resample_method)99, &keep) == PS_INVALID);
    CHECK(!memcmp(&keep, &interpolated, sizeof keep) && ps_analysis_scratch_bytes(c) == bytes);
    ps_series shifted, constant, empty;
    CHECK(ps_series_affine(c, q, 1, (ps_quantity){.01, PS_SECOND}, &shifted) == PS_OK);
    bytes = ps_analysis_scratch_bytes(c);
    CHECK(ps_series_resample_linear(c, y, x, shifted, &keep) == PS_INVALID);
    CHECK(ps_series_resample(c, y, x, shifted, PS_RESAMPLE_NEAREST, &keep) == PS_INVALID);
    CHECK(ps_series_resample(c, y, x, shifted, PS_RESAMPLE_PREVIOUS, &keep) == PS_INVALID);
    CHECK(ps_analysis_scratch_bytes(c) == bytes); /* failure after several output blocks */
    CHECK(ps_series_release(c, shifted) == PS_OK);
    CHECK(ps_series_affine(c, q, 1, (ps_quantity){-.01, PS_SECOND}, &shifted) == PS_OK);
    CHECK(ps_series_resample_linear(c, y, x, shifted, &keep) == PS_INVALID);
    CHECK(ps_series_affine(c, q, 0, (ps_quantity){0, PS_SECOND}, &constant) == PS_OK);
    CHECK(ps_series_resample_linear(c, y, x, constant, &keep) == PS_INVALID);
    CHECK(ps_series_slice(c, q, 0, 0, &empty) == PS_OK);
    CHECK(ps_series_resample_linear(c, y, x, empty, &keep) == PS_INVALID);
    CHECK(!memcmp(&keep, &interpolated, sizeof keep));
    CHECK(ps_dataset_close(c, a) == PS_OK);
    CHECK(ps_series_read(c, interpolated, 0, values, 1537, &got) == PS_OK && got == 1537);
    for (size_t i = 0; i < got; i++)
        CHECK(values[i] == 3 * (double)i * .25 - 2);
    CHECK(ps_dataset_close(c, b) == PS_OK && ps_analysis_scratch_bytes(c) == 0);
    CHECK(ps_series_describe(c, interpolated, &info) == PS_INVALID);
    ps_analysis_destroy(c);

    CHECK(ps_analysis_create("resample-limit", (769 + 1537) * 3 * 8, &c) == PS_OK);
    CHECK(open_series(c, source, &a, &x, &y) == PS_OK);
    CHECK(open_series(c, target, &b, &q, &reference) == PS_OK);
    CHECK(ps_series_resample_linear(c, y, x, q, &keep) == PS_LIMIT);
    CHECK(ps_series_resample(c, y, x, q, PS_RESAMPLE_NEAREST, &keep) == PS_LIMIT);
    CHECK(ps_series_resample(c, y, x, q, PS_RESAMPLE_PREVIOUS, &keep) == PS_LIMIT);
    CHECK(!memcmp(&keep, &interpolated, sizeof keep));
    ps_analysis_destroy(c);

    CHECK(fixture(source, 769, .5, 1) == PS_OK);
    CHECK(fixture(target, 3, .25, 0) == PS_OK);
    CHECK(ps_analysis_create("resample-invalid", 0, &c) == PS_OK);
    CHECK(open_series(c, source, &a, &x, &y) == PS_OK);
    CHECK(open_series(c, target, &b, &q, &reference) == PS_OK);
    CHECK(ps_series_resample_linear(c, y, x, q, &keep) == PS_INVALID);
    CHECK(ps_series_resample(c, y, x, q, PS_RESAMPLE_NEAREST, &keep) == PS_INVALID);
    CHECK(ps_series_resample(c, y, x, q, PS_RESAMPLE_PREVIOUS, &keep) == PS_INVALID);
    ps_analysis_destroy(c);

    CHECK(fixture(source, 2, 1, 2) == PS_OK);
    CHECK(fixture(target, 1, 1, 3) == PS_OK);
    CHECK(ps_analysis_create("resample-extreme", 0, &c) == PS_OK);
    CHECK(open_series(c, source, &a, &x, &y) == PS_OK);
    CHECK(open_series(c, target, &b, &q, &reference) == PS_OK);
    CHECK(ps_series_resample_linear(c, y, x, q, &interpolated) == PS_OK);
    CHECK(ps_series_read(c, interpolated, 0, values, 1, &got) == PS_OK && got == 1 &&
          values[0] == 0);
    for (int method = PS_RESAMPLE_NEAREST; method <= PS_RESAMPLE_PREVIOUS; method++) {
        CHECK(ps_series_resample(c, y, x, q, (ps_resample_method)method, &interpolated) == PS_OK);
        CHECK(ps_series_read(c, interpolated, 0, values, 1, &got) == PS_OK &&
              values[0] == -DBL_MAX);
        CHECK(ps_series_release(c, interpolated) == PS_OK);
        CHECK(ps_series_resample(c, reference, q, q, (ps_resample_method)method, &interpolated) ==
              PS_OK);
        CHECK(ps_series_read(c, interpolated, 0, values, 1, &got) == PS_OK && values[0] == 7);
        CHECK(ps_series_release(c, interpolated) == PS_OK);
    }
    CHECK(ps_series_resample_linear(c, reference, q, q, &interpolated) == PS_OK);
    CHECK(ps_series_read(c, interpolated, 0, values, 1, &got) == PS_OK && values[0] == 7);
    CHECK(ps_series_affine(c, q, 1, (ps_quantity){DBL_MAX * .75, PS_SECOND}, &shifted) == PS_OK);
    CHECK(ps_series_resample(c, y, x, shifted, PS_RESAMPLE_NEAREST, &interpolated) == PS_OK);
    CHECK(ps_series_read(c, interpolated, 0, values, 1, &got) == PS_OK && values[0] == DBL_MAX);
    CHECK(ps_series_resample(c, y, x, shifted, PS_RESAMPLE_PREVIOUS, &interpolated) == PS_OK);
    CHECK(ps_series_read(c, interpolated, 0, values, 1, &got) == PS_OK && values[0] == -DBL_MAX);
    ps_analysis_destroy(c);

    CHECK(fixture(source, 17, .5, 4) == PS_OK);
    CHECK(fixture(target, 33, .25, 4) == PS_OK);
    CHECK(ps_analysis_create("resample-quadratic", 0, &c) == PS_OK);
    CHECK(open_series(c, source, &a, &x, &y) == PS_OK);
    CHECK(open_series(c, target, &b, &q, &reference) == PS_OK);
    CHECK(ps_series_resample_linear(c, y, x, q, &interpolated) == PS_OK);
    CHECK(ps_series_read(c, interpolated, 0, values, 33, &got) == PS_OK && got == 33);
    for (size_t i = 0; i < got; i++) {
        double time = (double)i * .25;
        CHECK(fabs(values[i] - time * time - (i % 2 ? .0625 : 0)) < 1e-12);
    }
    ps_analysis_destroy(c);
    CHECK(fixture(source, 769, .5, 4) == PS_OK);
    CHECK(fixture(target, 3073, .125, 0) == PS_OK);
    CHECK(ps_analysis_create("resample-methods", 0, &c) == PS_OK);
    CHECK(open_series(c, source, &a, &x, &y) == PS_OK);
    CHECK(open_series(c, target, &b, &q, &reference) == PS_OK);
    ps_series outputs[2];
    for (int mode = 0; mode < 2; mode++) {
        ps_resample_method method = mode ? PS_RESAMPLE_PREVIOUS : PS_RESAMPLE_NEAREST;
        CHECK(ps_series_resample(c, y, x, q, method, &outputs[mode]) == PS_OK);
        CHECK(ps_series_aligned(c, q, outputs[mode]) == PS_OK);
        CHECK(ps_series_describe(c, outputs[mode], &info) == PS_OK && info.dimension[0] == 1 &&
              info.scale == 1);
    }
    CHECK(ps_dataset_close(c, a) == PS_OK);
    for (int mode = 0; mode < 2; mode++) {
        for (size_t at = 0; at < 3073; at += got) {
            CHECK(ps_series_read(c, outputs[mode], at, values, 127, &got) == PS_OK && got);
            for (size_t i = 0; i < got; i++) {
                size_t index = (at + i + (mode ? 0 : 1)) / 4;
                double coordinate = (double)index * .5;
                CHECK(values[i] == coordinate * coordinate);
            }
        }
    }
    CHECK(ps_dataset_close(c, b) == PS_OK && ps_analysis_scratch_bytes(c) == 0);
    CHECK(ps_series_describe(c, outputs[0], &info) == PS_INVALID);
    ps_analysis_destroy(c);
    remove(source);
    remove(target);
    puts("Resampling: linear/nearest/previous references, blocks, ranges, units, lifetime and "
         "limits "
         "passed");
    return 0;
}
