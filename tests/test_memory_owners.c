#include "physim/report.h"
#include "test_allocator.h"
#include <stdio.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Memory owners line %d: %s\n", __LINE__, #x);                          \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static ps_plot_info plot_info(void) {
    ps_plot_info p = {0};
    strcpy(p.title, "Allocation fixture");
    ps_report_unit_from(PS_SECOND, &p.x_unit);
    ps_report_unit_from(PS_METRE, &p.y_unit);
    return p;
}
static ps_table_info table_info(void) {
    ps_table_info t = {0};
    strcpy(t.title, "Table");
    t.columns = 1;
    strcpy(t.column[0].label, "Value");
    ps_report_unit_from(PS_METRE, &t.column[0].unit);
    return t;
}
static ps_curve_data curve_data(void) {
    ps_curve_data c = {0};
    c.kind = PS_PLOT_LINE;
    c.count = 2;
    c.source_count = 2;
    strcpy(c.label, "Curve");
    c.x[1] = 1;
    c.y[0] = 3;
    c.y[1] = 4;
    return c;
}
/* Representative report includes every heap-allocating mutation and save. */
static ps_result report_workflow(ps_allocator a, ps_analysis_context *ctx, ps_series x, ps_series y,
                                 const char *path) {
    ps_report *report = NULL;
    ps_result result = ps_report_create_with_allocator("Memory ä", "fixture", a, &report);
    ps_plot_info pi = plot_info();
    ps_plot_handle plot = {0};
    ps_table_info ti = table_info();
    ps_table_handle table = {0};
    ps_curve_data curve = curve_data();
    ps_table_row row = {"row", {3}};
    if (result == PS_OK)
        result = ps_report_add_plot(report, &pi, &plot);
    if (result == PS_OK)
        result = ps_report_add_curve(report, plot, &curve);
    if (result == PS_OK)
        result = ps_report_add_curve(report, plot, &curve);
    if (result == PS_OK)
        result = ps_report_add_table(report, &ti, &table);
    if (result == PS_OK)
        result = ps_report_add_row(report, table, &row);
    if (result == PS_OK)
        result = ps_report_add_series(report, plot, ctx, x, y, "Series", PS_PLOT_LINE);
    if (result == PS_OK)
        result = ps_report_add_histogram(report, ctx, y, "Histogram", "x", 4, &plot);
    if (result == PS_OK)
        result = ps_report_save(report, path);
    ps_report_destroy(report);
    return result;
}
static int same_files(const char *a, const char *b) {
    FILE *fa = fopen(a, "rb"), *fb = fopen(b, "rb");
    CHECK(fa && fb);
    int ca, cb;
    do {
        ca = fgetc(fa);
        cb = fgetc(fb);
        if (ca != cb) {
            fclose(fa);
            fclose(fb);
            return 1;
        }
    } while (ca != EOF);
    CHECK(!ferror(fa) && !ferror(fb));
    CHECK(fclose(fa) == 0);
    CHECK(fclose(fb) == 0);
    return 0;
}
static int mutation_failures(ps_analysis_context *ctx, ps_series x, ps_series y) {
    test_allocator tracker = {0};
    ps_allocator a = test_domain(&tracker);
    ps_report *r = NULL;
    CHECK(ps_report_create_with_allocator("Preserve", "", a, &r) == PS_OK);
    ps_plot_info pi = plot_info();
    ps_plot_handle plot;
    CHECK(ps_report_add_plot(r, &pi, &plot) == PS_OK);
    ps_curve_data curve = curve_data();
    CHECK(ps_report_add_curve(r, plot, &curve) == PS_OK);
    CHECK(ps_report_save(r, "memory-before.psreport") == PS_OK);
    for (int mode = 0; mode < 5; mode++) {
        size_t bytes = tracker.live_bytes, blocks = tracker.live_blocks;
        tracker.fail_on = tracker.attempts + (mode == 3 ? 2 : 1);
        ps_result result;
        ps_table_info ti = table_info();
        ps_table_handle th = {r, 99};
        ps_plot_handle ph = {r, 99};
        if (mode == 0)
            result = ps_report_add_curve(r, plot, &curve);
        else if (mode == 1)
            result = ps_report_add_table(r, &ti, &th);
        else if (mode == 2 || mode == 3)
            result = ps_report_add_series(r, plot, ctx, x, y, "Fail", PS_PLOT_LINE);
        else
            result = ps_report_add_histogram(r, ctx, y, "Fail", "x", 4, &ph);
        CHECK(result == PS_MEMORY);
        CHECK(th.owner == r && th.index == 99 && ph.owner == r && ph.index == 99);
        CHECK(tracker.live_bytes == bytes && tracker.live_blocks == blocks && !tracker.invalid);
        tracker.fail_on = 0;
        remove("memory-after.psreport");
        CHECK(ps_report_save(r, "memory-after.psreport") == PS_OK);
        CHECK(same_files("memory-before.psreport", "memory-after.psreport") == 0);
    }
    /* A too-small budget rejects the encoding buffer before creating a file. */
    tracker.budget = tracker.live_bytes + 1024;
    CHECK(ps_report_save(r, "memory-failed.psreport") == PS_MEMORY);
    FILE *f = fopen("memory-failed.psreport", "rb");
    CHECK(!f);
    tracker.budget = 0;
    for (int svg = 0; svg < 2; svg++) {
        const char *path = svg ? "memory-export.svg" : "memory-export.csv";
        remove(path);
        size_t bytes = tracker.live_bytes, blocks = tracker.live_blocks;
        tracker.fail_on = tracker.attempts + 1;
        ps_result result =
            svg ? ps_report_export_svg(r, 0, path) : ps_report_export_plot_csv(r, 0, path);
        CHECK(result == PS_MEMORY);
        CHECK(tracker.live_bytes == bytes && tracker.live_blocks == blocks);
        f = fopen(path, "rb");
        CHECK(!f);
        tracker.fail_on = 0;
        result = svg ? ps_report_export_svg(r, 0, path) : ps_report_export_plot_csv(r, 0, path);
        CHECK(result == PS_OK);
        result = svg ? ps_report_export_svg(r, 0, path) : ps_report_export_plot_csv(r, 0, path);
        CHECK(result == PS_IO); /* Existing target: temporary storage also released. */
        CHECK(tracker.live_bytes == bytes && tracker.live_blocks == blocks && !tracker.invalid);
        CHECK(remove(path) == 0);
    }
    ps_report_destroy(r);
    CHECK(!tracker.live_blocks && !tracker.live_bytes && !tracker.invalid);
    return 0;
}
static int report_failures(ps_analysis_context *ctx, ps_series x, ps_series y) {
    test_allocator baseline = {0};
    CHECK(report_workflow(test_domain(&baseline), ctx, x, y, "memory-report.psreport") == PS_OK);
    CHECK(!baseline.live_blocks && !baseline.live_bytes && !baseline.invalid &&
          baseline.attempts >= 8);
    for (size_t fail = 1; fail <= baseline.attempts; fail++) {
        test_allocator tracker = {0};
        tracker.fail_on = fail;
        CHECK(report_workflow(test_domain(&tracker), ctx, x, y, "memory-failed.psreport") ==
              PS_MEMORY);
        CHECK(!tracker.live_blocks && !tracker.live_bytes && !tracker.invalid);
        FILE *f = fopen("memory-failed.psreport", "rb");
        CHECK(!f);
    }
    ps_report *sentinel = NULL;
    CHECK(ps_report_create("Sentinel", "unchanged", &sentinel) == PS_OK);
    ps_report *loaded = sentinel;
    test_allocator loader = {0};
    CHECK(ps_report_load_with_allocator("memory-report.psreport", test_domain(&loader), &loaded) ==
          PS_OK);
    CHECK(loader.live_blocks > 1 && loader.live_bytes > 0 && !loader.invalid);
    size_t allocations = loader.attempts;
    ps_report_destroy(loaded);
    CHECK(!loader.live_blocks && !loader.live_bytes && !loader.invalid && allocations >= 7);
    for (size_t fail = 1; fail <= allocations; fail++) {
        test_allocator tracker = {0};
        tracker.fail_on = fail;
        loaded = sentinel;
        CHECK(ps_report_load_with_allocator("memory-report.psreport", test_domain(&tracker),
                                            &loaded) == PS_MEMORY);
        CHECK(loaded == sentinel && !tracker.live_blocks && !tracker.live_bytes &&
              !tracker.invalid);
        /* Recovery after failure uses the same domain, including its unchanged user state. */
        tracker.fail_on = 0;
        CHECK(ps_report_load_with_allocator("memory-report.psreport", test_domain(&tracker),
                                            &loaded) == PS_OK);
        ps_report_destroy(loaded);
        CHECK(!tracker.live_blocks && !tracker.live_bytes && !tracker.invalid);
    }
    loaded = sentinel;
    CHECK(ps_report_create_with_allocator("x", "", (ps_allocator){0}, &loaded) == PS_INVALID &&
          loaded == sentinel);
    CHECK(ps_report_load_with_allocator("memory-report.psreport", (ps_allocator){0}, &loaded) ==
              PS_INVALID &&
          loaded == sentinel);
    /* Every truncated prefix must unwind its custom allocation domain. */
    FILE *f = fopen("memory-report.psreport", "rb");
    CHECK(f);
    unsigned char encoded[8192];
    size_t length = fread(encoded, 1, sizeof encoded, f);
    CHECK(length > 20 && length < sizeof encoded && !ferror(f));
    CHECK(fclose(f) == 0);
    for (size_t n = 0; n < length; n++) {
        f = fopen("memory-truncated.psreport", "wb");
        CHECK(f);
        CHECK(fwrite(encoded, 1, n, f) == n);
        CHECK(fclose(f) == 0);
        test_allocator tracker = {0};
        loaded = sentinel;
        CHECK(ps_report_load_with_allocator("memory-truncated.psreport", test_domain(&tracker),
                                            &loaded) != PS_OK);
        CHECK(loaded == sentinel && !tracker.live_blocks && !tracker.live_bytes &&
              !tracker.invalid);
    }
    ps_report_destroy(sentinel);
    printf("Memory owners: %zu report failure sites, %zu load failure sites, %zu truncated "
           "prefixes passed\n",
           baseline.attempts, allocations, length);
    return 0;
}
static int arena_owners(void) {
    /* All owner allocations fit here; no dynamic allocation from the arena itself. */
    size_t capacity = 2 * 1024 * 1024;
    void *buffer = malloc(capacity);
    CHECK(buffer);
    ps_arena arena;
    CHECK(ps_arena_init(&arena, buffer, capacity) == PS_OK);
    ps_allocator a = ps_arena_allocator(&arena);
    for (int pass = 0; pass < 2; pass++) {
        ps_report *r = NULL;
        ps_analysis_context *ctx = NULL;
        CHECK(ps_report_load_with_allocator("memory-report.psreport", a, &r) == PS_OK);
        CHECK(ps_analysis_create_with_allocator("memory-arena", 0, a, &ctx) == PS_OK);
        ps_dataset data;
        ps_series y;
        CHECK(ps_analysis_open_run(ctx, "memory-source.psrun", &data) == PS_OK);
        CHECK(ps_dataset_series(ctx, data, "position", &y) == PS_OK);
        ps_statistics stats = {0};
        CHECK(ps_series_statistics(ctx, y, &stats) == PS_OK && stats.count == 9);
        ps_plot_handle histogram;
        CHECK(ps_report_add_histogram(r, ctx, y, "Arena", "position", 4, &histogram) == PS_OK);
        ps_curve_data curve;
        CHECK(ps_report_curve_read(r, histogram.index, 0, &curve) == PS_OK);
        double sum = 0;
        for (uint32_t i = 0; i < curve.count; i++)
            sum += curve.y[i];
        CHECK(sum == 9);
        size_t used = arena.used;
        ps_analysis_destroy(ctx);
        ps_report_destroy(r);
        CHECK(arena.used == used);
        ps_arena_reset(&arena);
        CHECK(arena.used == 0);
    }
    free(buffer);
    return 0;
}
int main(void) {
    const char *files[] = {"memory-source.psrun",    "memory-report.psreport",
                           "memory-failed.psreport", "memory-before.psreport",
                           "memory-after.psreport",  "memory-truncated.psreport"};
    for (size_t i = 0; i < sizeof files / sizeof *files; i++)
        remove(files[i]);
    ps_context context = {0};
    context.dt_s = .25;
    CHECK(ps_channel_add(&context, "position", PS_METRE, "allocator fixture") == 0);
    ps_run_writer writer;
    CHECK(ps_run_create(&writer, files[0], &context, "Memory") == PS_OK);
    for (int i = 0; i < 9; i++) {
        double value = (double)i * i;
        CHECK(ps_run_append(&writer, i * .25, &value) == PS_OK);
    }
    CHECK(ps_run_close(&writer) == PS_OK);
    test_allocator tracker = {0};
    tracker.fail_on = 1;
    ps_analysis_context *ctx = NULL;
    CHECK(ps_analysis_create_with_allocator("memory-work", 0, test_domain(&tracker), &ctx) ==
              PS_MEMORY &&
          ctx == NULL);
    CHECK(!tracker.live_bytes && !tracker.invalid);
    tracker.fail_on = 0;
    CHECK(ps_analysis_create_with_allocator("memory-work", 0, test_domain(&tracker), &ctx) ==
          PS_OK);
    ps_analysis_context *saved = ctx;
    CHECK(ps_analysis_create_with_allocator("memory-work", 0, (ps_allocator){0}, &ctx) ==
              PS_INVALID &&
          ctx == saved);
    ps_dataset dataset;
    ps_series x, y, derived;
    CHECK(ps_analysis_open_run(ctx, files[0], &dataset) == PS_OK);
    CHECK(ps_dataset_series(ctx, dataset, "time", &x) == PS_OK);
    CHECK(ps_dataset_series(ctx, dataset, "position", &y) == PS_OK);
    CHECK(ps_series_affine(ctx, y, 2, (ps_quantity){1, PS_METRE}, &derived) == PS_OK);
    CHECK(ps_analysis_scratch_bytes(ctx) > 0);
    ps_statistics stats = {0};
    CHECK(ps_series_statistics(ctx, derived, &stats) == PS_OK);
    CHECK(stats.count == 9 && stats.min == 1 && stats.max == 129);
    CHECK(mutation_failures(ctx, x, y) == 0);
    CHECK(report_failures(ctx, x, y) == 0);
    ps_analysis_destroy(ctx);
    CHECK(!tracker.live_blocks && !tracker.live_bytes && !tracker.invalid);
    CHECK(arena_owners() == 0);
    for (size_t i = 0; i < sizeof files / sizeof *files; i++)
        CHECK(remove(files[i]) == 0 || i == 2);
    return 0;
}
