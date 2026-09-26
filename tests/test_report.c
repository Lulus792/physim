#include "physim/report.h"
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Report line %d: %s\n", __LINE__, #x);                                 \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static bool write_bytes(const unsigned char *b, size_t n) {
    FILE *f = fopen("report-invalid.psreport", "wb");
    if (!f)
        return false;
    bool ok = fwrite(b, 1, n, f) == n;
    return fclose(f) == 0 && ok;
}
int main(void) {
    const char *files[] = {"report-test.psreport", "report-invalid.psreport",
                           "report-test.svg",      "report-table.csv",
                           "report-plot.csv",      "report-source.psrun"};
    for (size_t i = 0; i < sizeof files / sizeof *files; i++)
        remove(files[i]);
    ps_report *r = NULL, *loaded = NULL;
    CHECK(ps_report_create("Analyse <&> ä", "input=reference\ncompiler=test", &r) == PS_OK);
    ps_report *unchanged = r;
    CHECK(ps_report_create("bad\xc0\xaf", "", &unchanged) == PS_INVALID && unchanged == r);
    ps_plot_info info = {0};
    strcpy(info.title, "Geschwindigkeit <&>");
    strcpy(info.x_label, "Zeit");
    strcpy(info.y_label, "Geschwindigkeit");
    CHECK(ps_report_unit_from(PS_SECOND, &info.x_unit) == PS_OK);
    CHECK(ps_report_unit_from(PS_VELOCITY, &info.y_unit) == PS_OK);
    ps_plot_handle plot;
    CHECK(ps_report_add_plot(r, &info, &plot) == PS_OK);
    ps_curve_data *curve = calloc(1, sizeof *curve);
    CHECK(curve != NULL);
    strcpy(curve->label, "a,\"b\" <&>");
    curve->kind = PS_PLOT_LINE;
    curve->count = 3;
    curve->source_count = 3;
    curve->x[0] = 0;
    curve->x[1] = 1;
    curve->x[2] = 2;
    curve->y[0] = -3;
    curve->y[1] = 2;
    curve->y[2] = 1;
    CHECK(ps_report_add_curve(r, plot, curve) == PS_OK);
    const ps_curve_data *borrowed = NULL;
    CHECK(ps_report_curve_view(r, 0, 0, &borrowed) == PS_OK);
    CHECK(borrowed && borrowed != curve && !memcmp(borrowed, curve, sizeof *curve));
    const ps_curve_data *same = borrowed;
    CHECK(ps_report_curve_view(NULL, 0, 0, &same) == PS_INVALID && same == borrowed);
    CHECK(ps_report_curve_view(r, UINT32_MAX, 0, &same) == PS_INVALID && same == borrowed);
    CHECK(ps_report_curve_view(r, 0, UINT32_MAX, &same) == PS_INVALID && same == borrowed);
    CHECK(ps_report_curve_view(r, 0, 0, NULL) == PS_INVALID);
    curve->y[1] = NAN;
    CHECK(ps_report_add_curve(r, plot, curve) == PS_INVALID);
    curve->y[1] = 2;
    curve->kind = PS_PLOT_SCATTER;
    CHECK(ps_report_add_curve(r, plot, curve) == PS_OK);
    CHECK(ps_report_curve_view(r, 0, 0, &same) == PS_OK && same == borrowed);
    CHECK(borrowed->kind == PS_PLOT_LINE && borrowed->y[1] == 2);
    CHECK(ps_report_add_curve(r, (ps_plot_handle){NULL, 0}, curve) == PS_INVALID);
    double bounds[4];
    CHECK(ps_report_plot_bounds(r, 0, bounds) == PS_OK);
    CHECK(bounds[0] == 0 && bounds[1] == 2 && bounds[2] == -3 && bounds[3] == 2);
    CHECK(ps_report_axis_fraction(0, -DBL_MAX, DBL_MAX) == .5);
    CHECK(ps_report_axis_fraction(4, 4, 4) == .5);
    ps_table_info table = {0};
    strcpy(table.title, "Kennzahlen");
    table.columns = 2;
    strcpy(table.column[0].label, "Mittelwert");
    strcpy(table.column[1].label, "Streuung");
    table.column[0].unit = table.column[1].unit = info.y_unit;
    ps_table_handle th;
    CHECK(ps_report_add_table(r, &table, &th) == PS_OK);
    ps_table_row row = {"Wert,\"ä\"", {2, 3}};
    CHECK(ps_report_add_row(r, th, &row) == PS_OK);
    CHECK(ps_report_curve_view(r, 0, 0, &same) == PS_OK && same == borrowed);
    CHECK(borrowed->y[0] == -3 && borrowed->y[1] == 2);
    row.values[1] = INFINITY;
    CHECK(ps_report_add_row(r, th, &row) == PS_INVALID);
    CHECK(ps_report_save(r, files[0]) == PS_OK);
    CHECK(ps_report_save(r, files[0]) == PS_IO);
    CHECK(ps_report_load(files[0], &loaded) == PS_OK);
    CHECK(ps_report_curve_read(loaded, 0, 0, curve) == PS_OK && curve->count == 3 &&
          curve->y[0] == -3);
    const ps_curve_data *loaded_view = NULL;
    CHECK(ps_report_curve_view(loaded, 0, 0, &loaded_view) == PS_OK);
    CHECK(loaded_view != borrowed && !memcmp(loaded_view, curve, sizeof *curve));
    CHECK(ps_report_row_read(loaded, 0, 0, &row) == PS_OK && row.values[1] == 3);
    CHECK(ps_report_export_svg(loaded, 0, files[2]) == PS_OK);
    const char *region_path = "report-region.svg";
    remove(region_path);
    double region[] = {0, 1, -1, 1};
    CHECK(ps_report_export_svg_region(loaded, 0, region, region_path) == PS_OK);
    CHECK(ps_report_export_svg_region(loaded, 0, region, region_path) == PS_IO);
    remove(region_path);
    region[1] = 0;
    CHECK(ps_report_export_svg_region(loaded, 0, region, region_path) == PS_INVALID);
    region[1] = NAN;
    CHECK(ps_report_export_svg_region(loaded, 0, region, region_path) == PS_INVALID);
    region[1] = DBL_MIN;
    CHECK(ps_report_export_svg_region(loaded, 0, region, region_path) == PS_LIMIT);
    FILE *invalid_region = fopen(region_path, "rb");
    CHECK(!invalid_region);
    CHECK(ps_report_export_table_csv(loaded, 0, files[3]) == PS_OK);
    CHECK(ps_report_export_plot_csv(loaded, 0, files[4]) == PS_OK);
    FILE *f = fopen(files[2], "rb");
    CHECK(f != NULL);
    char exported[8192] = {0};
    CHECK(fread(exported, 1, sizeof exported - 1, f) > 0);
    fclose(f);
    CHECK(strstr(exported, "&lt;&amp;&gt;") && strstr(exported, "<circle") &&
          strstr(exported, "<polyline"));
    f = fopen(files[3], "rb");
    CHECK(f != NULL);
    memset(exported, 0, sizeof exported);
    CHECK(fread(exported, 1, sizeof exported - 1, f) > 0);
    fclose(f);
    CHECK(strstr(exported, "\"Wert,\"\"ä\"\"\",2,3"));
    f = fopen(files[0], "rb");
    CHECK(f != NULL);
    CHECK(!fseek(f, 0, SEEK_END));
    long length = ftell(f);
    CHECK(length > 20 && length < 4096);
    rewind(f);
    unsigned char *bytes = malloc((size_t)length + 1), *mutant = malloc((size_t)length + 1);
    CHECK(bytes && mutant && fread(bytes, 1, (size_t)length, f) == (size_t)length);
    fclose(f);
    for (size_t n = 0; n < (size_t)length; n++) {
        CHECK(write_bytes(bytes, n));
        unchanged = loaded;
        CHECK(ps_report_load(files[1], &unchanged) != PS_OK && unchanged == loaded);
    }
    for (unsigned i = 0; i < 300; i++) {
        memcpy(mutant, bytes, (size_t)length);
        size_t at = 20 + ((size_t)i * 7919 % ((size_t)length - 20));
        mutant[at] ^= (unsigned char)(1 + i % 255);
        if (i % 2)
            ps_put_u32(mutant + 16, ps_crc32(mutant + 20, (size_t)length - 20));
        CHECK(write_bytes(mutant, (size_t)length));
        unchanged = loaded;
        ps_result result = ps_report_load(files[1], &unchanged);
        if (result == PS_OK)
            ps_report_destroy(unchanged);
        else
            CHECK(unchanged == loaded);
    }
    memcpy(mutant, bytes, (size_t)length);
    ps_put_u32(mutant + 12, 9 * 1024 * 1024);
    CHECK(write_bytes(mutant, (size_t)length));
    unchanged = loaded;
    CHECK(ps_report_load(files[1], &unchanged) == PS_LIMIT && unchanged == loaded);
    memcpy(mutant, bytes, (size_t)length);
    ps_put_u32(mutant + 8, 99);
    CHECK(write_bytes(mutant, (size_t)length));
    CHECK(ps_report_load(files[1], &unchanged) == PS_VERSION);
    memcpy(mutant, bytes, (size_t)length);
    mutant[length] = 1;
    CHECK(write_bytes(mutant, (size_t)length + 1));
    CHECK(ps_report_load(files[1], &unchanged) == PS_CORRUPT);
    free(bytes);
    free(mutant);
    ps_report_destroy(loaded);

    ps_context context = {0};
    context.dt_s = .01;
    ps_channel_add(&context, "velocity", PS_VELOCITY, "spike reference");
    ps_run_writer writer;
    CHECK(ps_run_create(&writer, files[5], &context, "test") == PS_OK);
    for (unsigned i = 0; i < 6001; i++) {
        double v[] = {i == 2783 ? 1000 : i == 2784 ? -1000 : (double)(i % 10)};
        CHECK(ps_run_append(&writer, i * .01, v) == PS_OK);
    }
    CHECK(ps_run_close(&writer) == PS_OK);
    ps_analysis_context *ctx = NULL;
    ps_dataset dataset;
    ps_series time, velocity;
    CHECK(ps_analysis_create("report-work", 0, &ctx) == PS_OK);
    CHECK(ps_analysis_open_run(ctx, files[5], &dataset) == PS_OK);
    CHECK(ps_dataset_series(ctx, dataset, "time", &time) == PS_OK);
    CHECK(ps_dataset_series(ctx, dataset, "velocity", &velocity) == PS_OK);
    CHECK(ps_report_add_series(r, plot, ctx, time, velocity, "Spitzen", PS_PLOT_LINE) == PS_OK);
    CHECK(ps_report_curve_read(r, 0, 2, curve) == PS_OK && curve->source_count == 6001 &&
          curve->count <= 2048);
    bool high = false, low = false;
    for (uint32_t i = 0; i < curve->count; i++) {
        high |= curve->y[i] == 1000;
        low |= curve->y[i] == -1000;
    }
    CHECK(high && low && curve->x[0] == 0 && curve->x[curve->count - 1] == 60);
    CHECK(ps_report_add_series(r, plot, ctx, time, velocity, "Punkte", PS_PLOT_SCATTER) == PS_OK);
    CHECK(ps_report_curve_read(r, 0, 3, curve) == PS_OK && curve->count == 2048 &&
          curve->x[2047] == 60);
    CHECK(ps_report_add_series(r, plot, ctx, velocity, time, "Falsche Dimension", PS_PLOT_LINE) ==
          PS_INVALID);
    ps_series slice;
    CHECK(ps_series_slice(ctx, time, 1, 6000, &slice) == PS_OK);
    CHECK(ps_report_add_series(r, plot, ctx, slice, velocity, "Falsche Zuordnung", PS_PLOT_LINE) ==
          PS_INVALID);
    ps_plot_handle histogram;
    CHECK(ps_report_add_histogram(r, ctx, velocity, "Verteilung", "Geschwindigkeit", 20,
                                  &histogram) == PS_OK);
    CHECK(ps_report_curve_read(r, histogram.index, 0, curve) == PS_OK && curve->count == 20);
    double sum = 0;
    for (uint32_t i = 0; i < curve->count; i++)
        sum += curve->y[i];
    CHECK(sum == 6001 && curve->y[0] == 1 && curve->y[19] == 1);
    ps_series constant;
    CHECK(ps_series_affine(ctx, velocity, 0, (ps_quantity){3, PS_VELOCITY}, &constant) == PS_OK);
    CHECK(ps_report_add_histogram(r, ctx, constant, "Konstante", "Geschwindigkeit", 128,
                                  &histogram) == PS_OK);
    CHECK(ps_report_curve_read(r, histogram.index, 0, curve) == PS_OK && curve->count == 1 &&
          curve->y[0] == 6001);
    CHECK(ps_report_plot_bounds(r, histogram.index, bounds) == PS_OK && bounds[0] < 3 &&
          bounds[1] > 3 && bounds[2] == 0);
    ps_series pair;
    CHECK(ps_series_slice(ctx, velocity, 0, 2, &pair) == PS_OK);
    CHECK(ps_report_add_histogram(r, ctx, pair, "Mehr Klassen als Werte", "Geschwindigkeit", 128,
                                  &histogram) == PS_OK);
    CHECK(ps_report_curve_read(r, histogram.index, 0, curve) == PS_OK && curve->count == 128 &&
          curve->source_count == 2);
    sum = 0;
    for (uint32_t i = 0; i < curve->count; i++)
        sum += curve->y[i];
    CHECK(sum == 2 && curve->y[0] == 1 && curve->y[127] == 1);
    ps_plot_info converted = info;
    converted.x_unit.scale = 60;
    strcpy(converted.x_unit.symbol, "min");
    converted.y_unit.scale = 1000;
    strcpy(converted.y_unit.symbol, "km/s");
    ps_plot_handle conversion;
    CHECK(ps_report_add_plot(r, &converted, &conversion) == PS_OK);
    CHECK(ps_report_add_series(r, conversion, ctx, time, velocity, "Andere Skalen", PS_PLOT_LINE) ==
          PS_OK);
    CHECK(ps_report_curve_read(r, conversion.index, 0, curve) == PS_OK &&
          curve->x[curve->count - 1] == 1);
    high = false;
    low = false;
    for (uint32_t i = 0; i < curve->count; i++) {
        high |= curve->y[i] == 1;
        low |= curve->y[i] == -1;
    }
    CHECK(high && low);
    ps_analysis_destroy(ctx);
    ps_report_destroy(r);
    free(curve);
    for (size_t i = 0; i < sizeof files / sizeof *files; i++)
        remove(files[i]);
    puts("Reports: roundtrip, hostile files, units, extrema, histograms and exports passed");
    return 0;
}
