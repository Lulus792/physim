#include "physim/data.h"
#include "physim/report.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
int main(int argc, char **argv) {
    if (argc != 4)
        return 2;
    ps_run_reader run;
    if (ps_run_open(&run, argv[1]) != PS_OK)
        return 1;
    FILE *file = fopen(argv[2], "rb");
    if (!file) {
        ps_run_reader_close(&run);
        return 1;
    }
    char line[1024];
    bool ok = fgets(line, sizeof line, file) != NULL && strstr(line, "m s^-1") != NULL;
    uint64_t count = 0;
    double max_error = 0, max_reconstruction = 0, time, values[PS_MAX_CHANNELS];
    ps_result result = PS_OK;
    while (ok && (result = ps_run_next(&run, &time, values)) == PS_OK) {
        double t, x, v, filtered, reconstructed;
        if (!fgets(line, sizeof line, file) ||
            sscanf(line, "%lf,%lf,%lf,%lf,%lf", &t, &x, &v, &filtered, &reconstructed) != 5 ||
            !isfinite(v) || !isfinite(filtered) || !isfinite(reconstructed) ||
            fabs(t - time) > 1e-12 || x != values[2]) {
            ok = false;
            break;
        }
        double reference = 1.5 * cos(values[0]) * values[1];
        if (count > 0 && count < 4000)
            max_error = fmax(max_error, fabs(reference - v));
        max_reconstruction = fmax(max_reconstruction, fabs(x - reconstructed));
        count++;
    }
    ok = ok && result == PS_EOF && count == 4001 && !fgets(line, sizeof line, file) &&
         max_error < 1e-4 && max_reconstruction < 1e-4;
    printf("Derived CSV: %llu samples, velocity error %.9g m/s, reconstruction error %.9g m\n",
           (unsigned long long)count, max_error, max_reconstruction);
    fclose(file);
    ps_run_reader_close(&run);
    ps_report *report = NULL;
    if (ps_report_load(argv[3], &report) != PS_OK)
        return 1;
    uint32_t plots = 0, tables = 0;
    ps_report_describe(report, NULL, NULL, &plots, &tables);
    ps_curve_data curve;
    ok = ok && plots == 3 && tables == 2;
    if (ps_report_curve_read(report, 0, 0, &curve) != PS_OK)
        ok = false;
    else {
        ok = ok && curve.source_count == 4001 && curve.count <= PS_REPORT_MAX_POINTS &&
             curve.x[0] == 0 && fabs(curve.x[curve.count - 1] - 20) < 1e-10;
        FILE *csv = fopen(argv[2], "rb");
        if (!csv)
            ok = false;
        else {
            fgets(line, sizeof line, csv);
            uint32_t at = 0;
            while (at < curve.count && fgets(line, sizeof line, csv)) {
                double t, x, v;
                if (sscanf(line, "%lf,%lf,%lf", &t, &x, &v) != 3) {
                    ok = false;
                    break;
                }
                if (t == curve.x[at]) {
                    ok = ok && v == curve.y[at];
                    at++;
                }
            }
            ok = ok && at == curve.count;
            fclose(csv);
        }
    }
    if (ps_report_curve_read(report, 2, 0, &curve) != PS_OK)
        ok = false;
    else {
        double sum = 0;
        for (uint32_t i = 0; i < curve.count; i++)
            sum += curve.y[i];
        ok = ok && sum == 4001;
    }
    ps_table_row metrics;
    if (ps_report_row_read(report, 1, 0, &metrics) != PS_OK)
        ok = false;
    else
        ok = ok && metrics.values[0] < 1e-8 && fabs(metrics.values[1] - 2.488805869) < 2e-6;
    ps_report_destroy(report);
    puts(ok ? "App report agrees with full derived CSV, histogram and pendulum metrics"
            : "App report reference FAILED");
    return ok ? 0 : 1;
}
