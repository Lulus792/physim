#include "batch.h"
#include "platform.h"
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Batch line %d: %s\n", __LINE__, #x);                                  \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)

static bool after_one(uint32_t completed, uint32_t active, void *user) {
    (void)active;
    (void)user;
    return completed < 1;
}
static bool timed(uint32_t completed, uint32_t active, void *user) {
    (void)active;
    (void)completed;
    return ps_clock() < *(double *)user;
}
static bool file_contains(const char *directory, const char *name, const char *text) {
    char path[4096], content[8192];
    snprintf(path, sizeof path, "%s/%s", directory, name);
    FILE *f = fopen(path, "rb");
    if (!f)
        return false;
    size_t n = fread(content, 1, sizeof content - 1, f);
    content[n] = 0;
    fclose(f);
    return strstr(content, text) != NULL;
}
static bool absent_report(const char *directory) {
    char path[4096];
    snprintf(path, sizeof path, "%s/summary.psreport", directory);
    FILE *f = fopen(path, "rb");
    if (f)
        fclose(f);
    return f == NULL;
}
static bool same_file(const char *a, const char *b, const char *name) {
    char path[4096];
    snprintf(path, sizeof path, "%s/%s", a, name);
    FILE *first = fopen(path, "rb");
    snprintf(path, sizeof path, "%s/%s", b, name);
    FILE *second = fopen(path, "rb");
    bool same = first && second;
    if (same) {
        unsigned char x[4096], y[4096];
        for (;;) {
            size_t nx = fread(x, 1, sizeof x, first), ny = fread(y, 1, sizeof y, second);
            if (nx != ny || memcmp(x, y, nx)) {
                same = false;
                break;
            }
            if (!nx)
                break;
        }
        same = same && !ferror(first) && !ferror(second);
    }
    if (first)
        fclose(first);
    if (second)
        fclose(second);
    return same;
}
static bool same_summary(const char *a, const char *b) {
    char path[4096];
    ps_report *first = NULL, *second = NULL;
    snprintf(path, sizeof path, "%s/summary.psreport", a);
    bool same = ps_report_load(path, &first) == PS_OK;
    snprintf(path, sizeof path, "%s/summary.psreport", b);
    same = same && ps_report_load(path, &second) == PS_OK;
    if (same) {
        ps_curve_data x, y;
        same = ps_report_curve_read(first, 0, 0, &x) == PS_OK &&
               ps_report_curve_read(second, 0, 0, &y) == PS_OK && x.count == y.count &&
               x.source_count == y.source_count && x.bar_width == y.bar_width &&
               !memcmp(x.x, y.x, x.count * sizeof(double)) &&
               !memcmp(x.y, y.y, x.count * sizeof(double));
        for (unsigned table = 0; same && table < 2; table++) {
            ps_table_info info;
            same = ps_report_table_read(first, table, &info) == PS_OK;
            for (unsigned row = 0; same && row < info.rows; row++) {
                ps_table_row u, v;
                same = ps_report_row_read(first, table, row, &u) == PS_OK &&
                       ps_report_row_read(second, table, row, &v) == PS_OK &&
                       !memcmp(u.values, v.values, info.columns * sizeof(double));
            }
        }
    }
    ps_report_destroy(first);
    ps_report_destroy(second);
    return same;
}
static int statistics(void) {
    ps_channel channel = {0};
    strcpy(channel.name, "position.x");
    strcpy(channel.unit, "m");
    channel.dimension[0] = 1;
    double values[256];
    for (unsigned i = 0; i < 200; i++)
        values[i] = i;
    ps_report *report = NULL;
    CHECK(ps_batch_report(values, 200, &channel, "reference", &report) == PS_OK);
    uint32_t plots, tables;
    CHECK(ps_report_describe(report, NULL, NULL, &plots, &tables) == PS_OK && plots == 1 &&
          tables == 2);
    ps_table_row row;
    CHECK(ps_report_row_read(report, 0, 0, &row) == PS_OK && fabs(row.values[0] - 99.5) < 1e-12);
    CHECK(ps_report_row_read(report, 0, 2, &row) == PS_OK && fabs(row.values[0] - 4.975) < 1e-12);
    CHECK(ps_report_row_read(report, 0, 4, &row) == PS_OK && fabs(row.values[0] - 194.025) < 1e-12);
    double deviation = sqrt(200.0 * 201 / 12);
    CHECK(ps_report_row_read(report, 0, 6, &row) == PS_OK &&
          fabs(row.values[0] - deviation) < 1e-12);
    CHECK(ps_report_row_read(report, 1, 0, &row) == PS_OK &&
          fabs(row.values[0] - (99.5 - 1.959963984540054 * deviation / sqrt(200))) < 1e-12);
    ps_curve_data curve;
    CHECK(ps_report_curve_read(report, 0, 0, &curve) == PS_OK && curve.count == 15 &&
          curve.source_count == 200);
    double total = 0;
    for (unsigned i = 0; i < curve.count; i++)
        total += curve.y[i];
    CHECK(total == 200 && curve.y[0] == 14 && curve.y[14] == 14);
    ps_report_destroy(report);
    CHECK(ps_batch_report(values, 199, &channel, "reference", &report) == PS_OK);
    CHECK(ps_report_describe(report, NULL, NULL, &plots, &tables) == PS_OK && tables == 1);
    ps_report_destroy(report);
    for (unsigned i = 0; i < 256; i++)
        values[i] = 1e150;
    CHECK(ps_batch_report(values, 256, &channel, "constant", &report) == PS_OK);
    CHECK(ps_report_curve_read(report, 0, 0, &curve) == PS_OK && curve.count == 1 &&
          curve.y[0] == 256);
    CHECK(ps_report_row_read(report, 0, 6, &row) == PS_OK && row.values[0] == 0);
    ps_report_destroy(report);
    CHECK(ps_batch_report(values, 1, &channel, "single", &report) == PS_OK);
    ps_table_info info;
    CHECK(ps_report_table_read(report, 0, &info) == PS_OK && info.rows == 6);
    ps_report *unchanged = report;
    values[0] = NAN;
    CHECK(ps_batch_report(values, 1, &channel, "nan", &report) == PS_NUMERIC &&
          report == unchanged);
    values[0] = -DBL_MAX;
    values[1] = DBL_MAX;
    CHECK(ps_batch_report(values, 2, &channel, "overflow", &report) == PS_NUMERIC &&
          report == unchanged);
    ps_report_destroy(report);
    return 0;
}
int main(int argc, char **argv) {
    /* runner, uncertain module, build directory, crash, hang, schema fixture */
    CHECK(argc == 7 && statistics() == 0);
    void *module = ps_module_open(argv[2]);
    CHECK(module);
    void *symbol = ps_module_symbol(module, "ps_get_experiment");
    ps_experiment_entry entry;
    memcpy(&entry, &symbol, sizeof entry);
    CHECK(entry);
    const ps_experiment_api *api = entry();
    ps_context context = {0};
    context.struct_size = sizeof context;
    context.api_version = PS_API_VERSION;
    context.seed = 42;
    CHECK(api->create(&context) == PS_OK);
    double initial[PS_MAX_CHANNELS], next[PS_MAX_CHANNELS];
    memcpy(initial, context.values, sizeof initial);
    CHECK(api->step(&context, -.1) == PS_INVALID &&
          !memcmp(initial, context.values, sizeof initial));
    CHECK(api->step(&context, .005) == PS_OK);
    memcpy(next, context.values, sizeof next);
    CHECK(api->reset(&context) == PS_OK && !memcmp(initial, context.values, sizeof initial));
    CHECK(api->step(&context, .005) == PS_OK && !memcmp(next, context.values, sizeof next));
    api->destroy(&context);
    ps_module_close(module);
    ps_batch_options o = {0};
    snprintf(o.runner, sizeof o.runner, "%s", argv[1]);
    snprintf(o.module, sizeof o.module, "%s", argv[2]);
    snprintf(o.channel, sizeof o.channel, "position.x");
    o.seed = 42;
    o.workers = 1;
    o.source_text = "/* frozen source snapshot */\n";
    o.source_size = strlen(o.source_text);
    o.runs = 256;
    o.steps = 200;
    o.dt = .005;
    o.timeout_s = 15;
    char root[4096], first[4096];
    snprintf(root, sizeof root, "%s/batch-reference-%.0f", argv[3], ps_clock() * 1e9);
    CHECK(ps_make_directory_exclusive(root));
    snprintf(first, sizeof first, "%s/first", root);
    strcpy(o.directory, first);
    ps_batch_result result, original;
    CHECK(ps_batch_run(&o, NULL, NULL, &original) == PS_OK && original.completed == 256 &&
          !original.cancelled);
    CHECK(file_contains(first, "experiment.c", "/* frozen source snapshot */") &&
          file_contains(first, "status.txt", "status=complete") &&
          file_contains(first, "endpoints.csv", "1,42,run-0001.psrun,1,"));
    double sum = 0, squared = 0;
    for (unsigned i = 0; i < 256; i++) {
        sum += original.values[i];
        squared += (original.values[i] - 1) * (original.values[i] - 1);
    }
    CHECK(fabs(sum / 256 - 1) < .03 && fabs(sqrt(squared / 256) - .15) < .025);
    CHECK(ps_batch_run(&o, NULL, NULL, &result) == PS_IO);
    CHECK(file_contains(first, "status.txt", "status=complete"));
    snprintf(o.directory, sizeof o.directory, "%s/parallel", root);
    o.workers = 4;
    CHECK(ps_batch_run(&o, NULL, NULL, &result) == PS_OK && result.completed == 256 &&
          result.active == 0 && result.peak_active == 4 && result.started == 256 &&
          !memcmp(result.values, original.values, 256 * sizeof(double)));
    CHECK(same_file(first, o.directory, "endpoints.csv") && same_summary(first, o.directory));
    snprintf(o.directory, sizeof o.directory, "%s/repeat", root);
    snprintf(o.source, sizeof o.source, "%s/main.phys", root);
    o.runs = 8;
    o.workers = 8;
    CHECK(ps_batch_run(&o, NULL, NULL, &result) == PS_OK && result.completed == 8 &&
          !memcmp(result.values, original.values, 8 * sizeof(double)));
    CHECK(file_contains(o.directory, "experiment.phys", "/* frozen source snapshot */"));
    o.source[0] = 0;
    for (unsigned i = 0; i < 8; i++) {
        ps_run_reader a, b;
        char path[4096], seed[64];
        snprintf(path, sizeof path, "%s/run-%04u.psrun", first, i + 1);
        CHECK(ps_run_open(&a, path) == PS_OK);
        snprintf(seed, sizeof seed, "seed=%u\n", 42 + i);
        CHECK(strstr(a.metadata, seed));
        snprintf(path, sizeof path, "%s/run-%04u.psrun", o.directory, i + 1);
        CHECK(ps_run_open(&b, path) == PS_OK);
        double ta, tb, va[PS_MAX_CHANNELS], vb[PS_MAX_CHANNELS], vx = 0, vy = 0, energy = 0;
        for (unsigned sample = 0; sample <= o.steps; sample++) {
            CHECK(ps_run_next(&a, &ta, va) == PS_OK && ps_run_next(&b, &tb, vb) == PS_OK);
            CHECK(ta == tb && !memcmp(va, vb, a.channels * sizeof(double)));
            if (!sample) {
                vx = va[2];
                vy = va[3];
                energy = va[4];
            }
            CHECK(fabs(va[0] - (-2 + vx * ta)) < 1e-12);
            CHECK(fabs(va[1] - (vy * ta - .5 * 9.80665 * ta * ta)) < 1e-12);
            CHECK(fabs(va[4] - energy) < 1e-11);
            CHECK(fabs(va[5] - (-2 + 3 * ta)) < 1e-12);
            CHECK(fabs(va[6] - (5 * ta - .5 * 9.80665 * ta * ta)) < 1e-12);
        }
        CHECK(ps_run_next(&a, &ta, va) == PS_EOF && ps_run_next(&b, &tb, vb) == PS_EOF);
        ps_run_reader_close(&a);
        ps_run_reader_close(&b);
    }
    snprintf(o.directory, sizeof o.directory, "%s/one-long-run", root);
    o.runs = 1;
    o.workers = 8;
    o.steps = 3000;
    CHECK(ps_batch_run(&o, NULL, NULL, &result) == PS_OK && result.completed == 1 &&
          result.peak_active == 1 && result.started == 1 && result.finished[0] &&
          !result.finished[1]);
    CHECK(fabs(result.values[0] - (-2 + (original.values[0] + 2) * 15)) < 1e-8);
    o.runs = 8;
    o.steps = 200;
    o.seed = 1042;
    o.workers = 1;
    snprintf(o.directory, sizeof o.directory, "%s/different", root);
    CHECK(ps_batch_run(&o, NULL, NULL, &result) == PS_OK &&
          memcmp(result.values, original.values, 8 * sizeof(double)));
    snprintf(o.directory, sizeof o.directory, "%s/cancelled", root);
    CHECK(ps_batch_run(&o, after_one, NULL, &result) == PS_OK && result.cancelled &&
          result.completed == 1);
    CHECK(absent_report(o.directory) &&
          file_contains(o.directory, "status.txt", "status=cancelled"));
    snprintf(o.directory, sizeof o.directory, "%s/missing-channel", root);
    strcpy(o.channel, "missing");
    CHECK(ps_batch_run(&o, NULL, NULL, &result) == PS_INVALID && result.completed == 0 &&
          absent_report(o.directory));
    strcpy(o.channel, "test");
    o.runs = 2;
    o.workers = 2;
    for (unsigned failure = 0; failure < 3; failure++) {
        strcpy(o.module, argv[4 + failure]);
        snprintf(o.directory, sizeof o.directory, "%s/failure-%u", root, failure);
        o.timeout_s = .25;
        double begin = ps_clock();
        CHECK(ps_batch_run(&o, NULL, NULL, &result) != PS_OK && !result.cancelled &&
              absent_report(o.directory));
        CHECK(result.completed == (failure == 2 ? 1u : 0u) && ps_clock() - begin < 5);
        CHECK(file_contains(o.directory, "status.txt", "status=failed"));
    }
    strcpy(o.module, argv[5]);
    snprintf(o.directory, sizeof o.directory, "%s/cancel-hang", root);
    o.timeout_s = 15;
    double until = ps_clock() + .15;
    CHECK(ps_batch_run(&o, timed, &until, &result) == PS_OK && result.cancelled &&
          result.completed == 0);
    CHECK(ps_clock() < until + 2 && absent_report(o.directory));
    o.seed = UINT64_MAX;
    CHECK(ps_batch_validate(&o) == PS_INVALID);
    o.seed = 0;
    o.runs = 1000;
    o.steps = 100000;
    CHECK(ps_batch_validate(&o) == PS_INVALID);
    o.runs = 1;
    o.steps = 1;
    o.dt = NAN;
    CHECK(ps_batch_validate(&o) == PS_INVALID);
    printf("Batch reference passed: mean=%.9g, spread about true mean=%.9g; raw files: %s\n",
           sum / 256, sqrt(squared / 256), root);
    return 0;
}
