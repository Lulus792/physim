#include "physim/data.h"
#include "physim/report.h"
#include "test_allocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned long long cases, accepted;
static ps_report *sentinel;
static void require(bool ok) {
    if (ok)
        return;
    fprintf(stderr, "Report invariant failed at case %llu; input: report-mutation.psreport\n",
            cases);
    abort();
}
static void inspect(ps_report *report) {
    char title[192], provenance[8192];
    uint32_t plots, tables;
    require(ps_report_describe(report, title, provenance, &plots, &tables) == PS_OK);
    require(plots <= PS_REPORT_MAX_PLOTS && tables <= PS_REPORT_MAX_TABLES);
    ps_report *copy = NULL;
    require(ps_report_create(title, provenance, &copy) == PS_OK);
    for (uint32_t i = 0; i < plots; i++) {
        ps_plot_info info;
        require(ps_report_plot_read(report, i, &info) == PS_OK);
        uint32_t curves = info.curves;
        require(curves <= PS_REPORT_MAX_CURVES);
        info.curves = 0;
        ps_plot_handle handle;
        require(ps_report_add_plot(copy, &info, &handle) == PS_OK);
        for (uint32_t j = 0; j < curves; j++) {
            const ps_curve_data *curve = NULL;
            require(ps_report_curve_view(report, i, j, &curve) == PS_OK && curve != NULL);
            require(ps_report_add_curve(copy, handle, curve) == PS_OK);
        }
    }
    for (uint32_t i = 0; i < tables; i++) {
        ps_table_info info;
        require(ps_report_table_read(report, i, &info) == PS_OK);
        uint32_t rows = info.rows;
        require(rows <= PS_REPORT_MAX_ROWS);
        info.rows = 0;
        ps_table_handle handle;
        require(ps_report_add_table(copy, &info, &handle) == PS_OK);
        for (uint32_t j = 0; j < rows; j++) {
            ps_table_row row;
            require(ps_report_row_read(report, i, j, &row) == PS_OK);
            require(ps_report_add_row(copy, handle, &row) == PS_OK);
        }
    }
    ps_report_destroy(copy);
}
static size_t exercise(const unsigned char *data, size_t size, size_t fail_on) {
    FILE *f = fopen("report-mutation.psreport", "wb");
    require(f != NULL);
    require(fwrite(data, 1, size, f) == size);
    require(fclose(f) == 0);
    cases++;
    test_allocator memory = {0};
    memory.budget = 32u * 1024u * 1024u;
    memory.fail_on = fail_on;
    ps_report *report = sentinel;
    ps_result result =
        ps_report_load_with_allocator("report-mutation.psreport", test_domain(&memory), &report);
    if (result == PS_OK) {
        require(!fail_on && report != sentinel && report != NULL);
        accepted++;
        inspect(report);
        ps_report_destroy(report);
    } else {
        require(report == sentinel);
        require(result == PS_CORRUPT || result == PS_VERSION || result == PS_LIMIT ||
                result == PS_MEMORY);
        if (fail_on)
            require(result == PS_MEMORY);
    }
    require(!memory.invalid && memory.live_blocks == 0 && memory.live_bytes == 0);
    return memory.attempts;
}
static void reject(const unsigned char *data, size_t size) {
    unsigned long long before = accepted;
    exercise(data, size, 0);
    require(accepted == before);
}
int main(int argc, char **argv) {
    unsigned char seed[8192], changed[8193];
    require(ps_report_create("existing", "", &sentinel) == PS_OK);
    if (argc == 3 && !strcmp(argv[1], "--replay")) {
        FILE *f = fopen(argv[2], "rb");
        if (!f)
            return 2;
        size_t n = fread(seed, 1, sizeof seed, f);
        int tail = fgetc(f);
        fclose(f);
        if (tail != EOF)
            return 2;
        exercise(seed, n, 0);
        ps_report_destroy(sentinel);
        puts("Report replay passed");
        return 0;
    }
    if (argc != 1)
        return 2;
    ps_report *report = NULL;
    require(ps_report_create("Pr\xc3\xbc" "fung", "seed=42\n", &report) == PS_OK);
    ps_plot_info info = {0};
    strcpy(info.title, "Plot");
    strcpy(info.x_label, "x");
    strcpy(info.y_label, "n");
    ps_unit scalar = {{0}, 1, "1"};
    require(ps_report_unit_from(PS_SECOND, &info.x_unit) == PS_OK);
    require(ps_report_unit_from(scalar, &info.y_unit) == PS_OK);
    ps_plot_handle plot;
    require(ps_report_add_plot(report, &info, &plot) == PS_OK);
    ps_curve_data curve = {0};
    strcpy(curve.label, "Data");
    curve.count = 3;
    curve.source_count = 3;
    curve.bar_width = .25;
    for (unsigned i = 0; i < 3; i++) {
        curve.x[i] = i;
        curve.y[i] = i + 1;
    }
    for (unsigned kind = 1; kind <= 3; kind++) {
        curve.kind = (ps_plot_kind)kind;
        require(ps_report_add_curve(report, plot, &curve) == PS_OK);
    }
    ps_table_info table = {0};
    strcpy(table.title, "Table");
    table.columns = 2;
    for (unsigned i = 0; i < 2; i++) {
        strcpy(table.column[i].label, i ? "b" : "a");
        require(ps_report_unit_from(scalar, &table.column[i].unit) == PS_OK);
    }
    ps_table_handle handle;
    require(ps_report_add_table(report, &table, &handle) == PS_OK);
    ps_table_row row = {"row", {1, 2}};
    require(ps_report_add_row(report, handle, &row) == PS_OK);
    remove("report-seed.psreport");
    require(ps_report_save(report, "report-seed.psreport") == PS_OK);
    ps_report_destroy(report);
    FILE *f = fopen("report-seed.psreport", "rb");
    require(f != NULL);
    size_t n = fread(seed, 1, sizeof seed, f);
    require(fgetc(f) == EOF);
    require(fclose(f) == 0);
    require(n > 20 && ps_get_u32(seed + 12) == n - 20);
    size_t allocations = exercise(seed, n, 0);
    for (size_t fail = 1; fail <= allocations; fail++)
        exercise(seed, n, fail);
    for (size_t cut = 0; cut < n; cut++)
        reject(seed, cut);
    for (size_t byte = 0; byte < n; byte++)
        for (unsigned bit = 0; bit < 8; bit++) {
            memcpy(changed, seed, n);
            changed[byte] ^= (unsigned char)(1u << bit);
            reject(changed, n);
        }
    for (size_t byte = 20; byte < n; byte++)
        for (unsigned bit = 0; bit < 8; bit++) {
            memcpy(changed, seed, n);
            changed[byte] ^= (unsigned char)(1u << bit);
            ps_put_u32(changed + 16, ps_crc32(changed + 20, n - 20));
            exercise(changed, n, 0);
        }
    memcpy(changed, seed, n);
    changed[n] = 0;
    reject(changed, n + 1);
    const uint32_t extremes[] = {0,         1, 8192, 8u * 1024u * 1024u, 8u * 1024u * 1024u + 1,
                                 UINT32_MAX};
    for (unsigned i = 0; i < 6; i++) {
        memcpy(changed, seed, n);
        ps_put_u32(changed + 12, extremes[i]);
        reject(changed, n);
    }
    require(accepted > 100);
    ps_report_destroy(sentinel);
    remove("report-seed.psreport");
    remove("report-mutation.psreport");
    printf("Report mutation campaign: %llu cases, %llu accepted; %zu allocation failures checked\n",
           cases, accepted, allocations);
    return 0;
}

