#include "physim/report.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Language report %d: %s\n", __LINE__, #x);                             \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static const ps_analysis_api *load(const char *path, void **module) {
    *module = ps_module_open(path);
    if (!*module)
        return NULL;
    void *symbol = ps_module_symbol(*module, "ps_get_analysis");
    ps_analysis_entry entry = NULL;
    memcpy(&entry, &symbol, sizeof entry);
    return entry ? entry() : NULL;
}
static int input(const char *path, int sensor_mode) {
    ps_context c = {0};
    c.struct_size = sizeof c;
    c.api_version = PS_API_VERSION;
    if (sensor_mode < 0) {
        CHECK(ps_channel_add(&c, "value", PS_METRE, "reference") == 0);
        CHECK(ps_channel_add(&c, "status", PS_ONE, "selector") == 1);
    } else {
        CHECK(ps_channel_add(&c, "position.x", PS_METRE, "model") == 0);
        CHECK(ps_channel_add(&c, "nominal.x", PS_METRE, "nominal") == 1);
        CHECK(ps_channel_add(&c, "sensor.x", PS_METRE, "measured") == 2);
        CHECK(ps_channel_add(&c, "sensor.x.u", PS_METRE, "uncertainty") == 3);
        CHECK(ps_channel_add(&c, "sensor.x.status", PS_ONE, "status") == 4);
    }
    ps_run_writer writer;
    CHECK(ps_run_create(&writer, path, &c, "reference") == PS_OK);
    for (unsigned i = 0; i < 513; i++) {
        double values[PS_MAX_CHANNELS] = {0};
        if (sensor_mode < 0) {
            values[0] = .5 * i;
            values[1] = i % 3;
        } else {
            int valid = sensor_mode == 2 ? i % 3 == 1 : sensor_mode == 1 && i == 256;
            values[0] = .5 * i;
            values[1] = .5 * i - .1;
            values[2] = valid ? values[0] + .25 : 0;
            values[3] = valid ? .1 : 0;
            values[4] = valid ? 1 : 2;
        }
        CHECK(ps_run_append(&writer, .01 * i, values) == PS_OK);
    }
    CHECK(ps_run_close(&writer) == PS_OK);
    return 0;
}
static int selected_report(const char *prefix) {
    char path[4096];
    snprintf(path, sizeof path, "%s.psreport", prefix);
    ps_report *report;
    CHECK(ps_report_load(path, &report) == PS_OK);
    uint32_t plots, tables;
    CHECK(ps_report_describe(report, NULL, NULL, &plots, &tables) == PS_OK && plots == 1 &&
          tables == 1);
    const ps_curve_data *curve;
    CHECK(ps_report_curve_view(report, 0, 1, &curve) == PS_OK);
    CHECK(curve->kind == PS_PLOT_SCATTER && curve->count == 171 && curve->source_count == 171);
    ps_statistics stats = {0};
    for (unsigned i = 0; i < 171; i++) {
        unsigned index = 1 + 3 * i;
        CHECK(fabs(curve->x[i] - .01 * index) < 1e-12 && curve->y[i] == .5 * index);
        ps_statistics_push(&stats, .5 * index);
    }
    ps_table_info info;
    ps_table_row row;
    CHECK(ps_report_table_read(report, 0, &info) == PS_OK && info.columns == 3 && info.rows == 1);
    CHECK(info.column[1].unit.scale == 1 && info.column[1].unit.dimension[0] == 1);
    CHECK(ps_report_row_read(report, 0, 0, &row) == PS_OK && !strcmp(row.label, "Valid"));
    CHECK(row.values[0] == 171 && fabs(row.values[1] - stats.mean) < 1e-12 &&
          fabs(row.values[2] - ps_statistics_stddev(&stats)) < 1e-12);
    ps_report_destroy(report);
    snprintf(path, sizeof path, "%s-selected.csv", prefix);
    FILE *csv = fopen(path, "rb");
    CHECK(csv);
    char line[1024];
    CHECK(fgets(line, sizeof line, csv));
    unsigned rows = 0;
    while (fgets(line, sizeof line, csv))
        rows++;
    fclose(csv);
    CHECK(rows == 171);
    snprintf(path, sizeof path, "%s-statistics.csv", prefix);
    csv = fopen(path, "rb");
    CHECK(csv);
    CHECK(fgets(line, sizeof line, csv) && fgets(line, sizeof line, csv) && strstr(line, "Valid"));
    fclose(csv);
    return 0;
}
static int sensor_report(const char *prefix, int mode) {
    char path[4096];
    snprintf(path, sizeof path, "%s.psreport", prefix);
    ps_report *report;
    CHECK(ps_report_load(path, &report) == PS_OK);
    uint32_t plots, tables;
    CHECK(ps_report_describe(report, NULL, NULL, &plots, &tables) == PS_OK && tables == 2 &&
          plots == (mode ? 2u : 1u));
    ps_table_info info;
    ps_table_row row;
    CHECK(ps_report_row_read(report, 0, 0, &row) == PS_OK);
    CHECK(row.values[0] == (mode == 2 ? 171 : mode) && row.values[1] == 513);
    CHECK(ps_report_table_read(report, 1, &info) == PS_OK && info.rows == (mode == 2 ? 1u : 0u));
    if (mode == 2) {
        CHECK(ps_report_row_read(report, 1, 0, &row) == PS_OK);
        CHECK(row.values[0] == 171 && row.values[1] == .25 && row.values[2] == 0 &&
              fabs(row.values[3] - .1) < 1e-14);
    }
    if (mode) {
        const ps_curve_data *curve;
        CHECK(ps_report_curve_view(report, 0, 2, &curve) == PS_OK &&
              curve->kind == PS_PLOT_SCATTER);
        CHECK(curve->source_count == (mode == 2 ? 171u : 1u));
        CHECK(ps_report_curve_view(report, 1, 0, &curve) == PS_OK &&
              curve->kind == PS_PLOT_HISTOGRAM);
        double total = 0;
        for (unsigned i = 0; i < curve->count; i++)
            total += curve->y[i];
        CHECK(total == (mode == 2 ? 171 : 1));
    }
    ps_report_destroy(report);
    return 0;
}
int main(int argc, char **argv) {
    CHECK(argc == 5);
    char work[4096], path[4096], prefix[4096];
    snprintf(work, sizeof work, "%s/report-language-%.0f", argv[4], ps_clock() * 1e6);
    CHECK(ps_make_directory_exclusive(work));
    snprintf(path, sizeof path, "%s/input.psrun", work);
    CHECK(input(path, -1) == 0);
    void *module;
    const ps_analysis_api *api = load(argv[1], &module);
    CHECK(api);
    for (unsigned i = 0; i < 2; i++) {
        snprintf(prefix, sizeof prefix, "%s/selection-%u", work, i);
        CHECK(api->run(path, prefix) == PS_OK && selected_report(prefix) == 0);
    }
    ps_module_close(module);
    api = load(argv[2], &module);
    CHECK(api);
    const char *inputs[] = {path, path, path, path, path, path, path, path};
    for (unsigned repeat = 0; repeat < 2; repeat++)
        for (size_t count = 1; count <= 8; count++) {
            snprintf(prefix, sizeof prefix, "%s/error-%u-%zu", work, repeat, count);
            CHECK(api->run_many(inputs, count, prefix) ==
                  (count == 6 || count == 7 ? PS_LIMIT : PS_INVALID));
            char report_path[4200];
            snprintf(report_path, sizeof report_path, "%s.psreport", prefix);
            FILE *f = fopen(report_path, "rb");
            CHECK(!f);
        }
    ps_module_close(module);
    api = load(argv[3], &module);
    CHECK(api);
    for (int mode = 0; mode < 3; mode++) {
        snprintf(path, sizeof path, "%s/sensor-%d.psrun", work, mode);
        CHECK(input(path, mode) == 0);
        snprintf(prefix, sizeof prefix, "%s/sensor-%d", work, mode);
        CHECK(api->run(path, prefix) == PS_OK && sensor_report(prefix, mode) == 0);
    }
    ps_module_close(module);
    puts("Language reports: aligned selections, empty/singleton data, units, tables, limits and "
         "lifecycle passed");
    return 0;
}
