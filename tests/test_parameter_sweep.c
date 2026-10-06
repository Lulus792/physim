#include "physim/data.h"
#include "physim/report.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "Parameter sweep failed at line %d: %s\n", __LINE__, #condition); \
    return 1; \
} } while (0)

static int read_text(const char *path, char *buffer, size_t capacity) {
    FILE *file = fopen(path, "rb");
    if (!file)
        return 0;
    size_t n = fread(buffer, 1, capacity - 1, file);
    buffer[n] = 0;
    int ok = !ferror(file);
    if (fclose(file))
        ok = 0;
    return ok;
}
int main(int argc, char **argv) {
    CHECK(argc == 2 || argc == 3);
    double gain = argc == 3 ? strtod(argv[2], NULL) : 1;
    char path[4096], text[4096];
    snprintf(path, sizeof path, "%s/series.txt", argv[1]);
    CHECK(read_text(path, text, sizeof text));
    CHECK(strstr(text, "physim_batch=5\n") &&
          strstr(text, "parameter=initialSpeed\n") &&
          strstr(text, "parameter_start=1\n") &&
          strstr(text, "parameter_end=4\n"));
    if (argc == 3)
        CHECK(strstr(text, "fixed_parameter.gain=2\n"));
    snprintf(path, sizeof path, "%s/endpoints.csv", argv[1]);
    CHECK(read_text(path, text, sizeof text));
    CHECK(strstr(text, "index,seed,file,time_s,parameter_value,value,status\n"));
    for (int i = 0; i < 3; i++) {
        double selected = i == 0 ? 1 : i == 1 ? 2.5 : 4;
        char row[128];
        snprintf(row, sizeof row, "%d,%d,run-%04d.psrun,%.17g,%.17g,%.17g,1\n",
                 i + 1, 42 + i, i + 1, 0.005, selected, selected * gain);
        CHECK(strstr(text, row));
        snprintf(path, sizeof path, "%s/run-%04d.psrun", argv[1], i + 1);
        ps_run_reader reader;
        CHECK(ps_run_open(&reader, path) == PS_OK);
        char parameter[80];
        snprintf(parameter, sizeof parameter, "\nparameter.initialSpeed=%.17g\n", selected);
        CHECK(strstr(reader.metadata, parameter));
        if (argc == 3)
            CHECK(strstr(reader.metadata, "\nparameter.gain=2\n"));
        double time, values[PS_MAX_CHANNELS];
        CHECK(ps_run_next(&reader, &time, values) == PS_OK && values[0] == selected * gain);
        CHECK(ps_run_next(&reader, &time, values) == PS_OK && values[0] == selected * gain);
        CHECK(ps_run_next(&reader, &time, values) == PS_EOF);
        ps_run_reader_close(&reader);
    }
    snprintf(path, sizeof path, "%s/summary.psreport", argv[1]);
    ps_report *report = NULL;
    CHECK(ps_report_load(path, &report) == PS_OK);
    ps_plot_info plot;
    CHECK(ps_report_plot_read(report, 0, &plot) == PS_OK);
    CHECK(strstr(plot.title, "initialSpeed") && plot.curves == 1);
    const ps_curve_data *curve = NULL;
    CHECK(ps_report_curve_view(report, 0, 0, &curve) == PS_OK);
    CHECK(curve->kind == PS_PLOT_LINE && curve->count == 3);
    for (int i = 0; i < 3; i++) {
        double selected = i == 0 ? 1 : i == 1 ? 2.5 : 4;
        CHECK(curve->x[i] == selected && curve->y[i] == selected * gain);
    }
    ps_report_destroy(report);
    return 0;
}
