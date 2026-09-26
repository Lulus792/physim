#include "physim/collision.h"
#include "physim/data.h"
#include "physim/report.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "SDK probe line %d: %s\n", __LINE__, #x);                              \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
int main(int argc, char **argv) {
    CHECK(argc == 3 || (argc == 4 && (!strcmp(argv[3], "language") ||
                                    !strcmp(argv[3], "sensor-language"))));
    const ps_aabb bounds[] = {{{0, 0, 0}, {1, 1, 1}}, {{1, 1, 1}, {2, 2, 2}}};
    ps_collision_pair pair;
    size_t candidates = 0;
    CHECK(ps_broad_phase(bounds, 2, &pair, 1, &candidates) == PS_OK && candidates == 1 &&
          pair.a == 0 && pair.b == 1);
    ps_run_reader reader;
    CHECK(ps_run_open(&reader, argv[1]) == PS_OK);
    CHECK(reader.channels > 0 && reader.channels <= PS_MAX_CHANNELS);
    double time, values[PS_MAX_CHANNELS];
    unsigned rows = 0;
    ps_result result;
    while ((result = ps_run_next(&reader, &time, values)) == PS_OK) {
        CHECK(rows <= 200 && fabs(time - rows * .005) < 1e-12);
        for (unsigned i = 0; i < reader.channels; i++)
            CHECK(isfinite(values[i]));
        rows++;
    }
    ps_run_reader_close(&reader);
    CHECK(result == PS_EOF && rows == 201);
    ps_report *report = NULL;
    CHECK(ps_report_load(argv[2], &report) == PS_OK);
    char title[192], provenance[8192];
    uint32_t plots, tables;
    CHECK(ps_report_describe(report, title, provenance, &plots, &tables) == PS_OK);
    CHECK(title[0]);
    if (argc == 4 && !strcmp(argv[3], "sensor-language")) {
        CHECK(plots == 2 && tables == 2);
        ps_table_info info;
        ps_table_row row;
        CHECK(ps_report_table_read(report, 0, &info) == PS_OK && info.rows == 1);
        CHECK(ps_report_row_read(report, 0, 0, &row) == PS_OK && row.values[1] == rows);
        double valid = row.values[0];
        CHECK(valid > 1 && valid < rows);
        const ps_curve_data *curve;
        CHECK(ps_report_curve_view(report, 0, 2, &curve) == PS_OK);
        CHECK(curve->kind == PS_PLOT_SCATTER && curve->source_count == valid);
        CHECK(ps_report_table_read(report, 1, &info) == PS_OK && info.rows == 1);
        CHECK(ps_report_row_read(report, 1, 0, &row) == PS_OK);
        CHECK(row.values[0] == valid && row.values[3] > 0);
    } else if (argc == 4) {
        CHECK(plots == 2 && tables == 0);
    } else {
        CHECK(plots > 0 && tables > 0);
    }
    ps_report_destroy(report);
    puts("Installed SDK run and report validated");
    return 0;
}
