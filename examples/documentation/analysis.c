#include "physim/report.h"
#include <stdio.h>

static ps_result analyze(const char *input, const char *prefix) {
    ps_analysis_context *ctx = NULL;
    ps_report *report = NULL;
    ps_dataset run = {0};
    ps_series time = {0}, position = {0}, velocity = {0};
    ps_result r = ps_analysis_create(prefix, 0, &ctx);
    bool recovered = false;
    if (r == PS_OK) {
        r = ps_analysis_open_run(ctx, input, &run);
        recovered = r == PS_RECOVERED;
        if (recovered)
            r = PS_OK;
    }
    if (r == PS_OK)
        r = ps_dataset_series(ctx, run, "time", &time);
    if (r == PS_OK)
        r = ps_dataset_series(ctx, run, "position.x", &position);
    if (r == PS_OK)
        r = ps_series_derivative(ctx, position, time, &velocity);
    if (r == PS_OK)
        r = ps_report_create("Uniform motion", "Velocity from position derivative", &report);
    ps_plot_info info = {0};
    snprintf(info.title, sizeof info.title, "Velocity");
    snprintf(info.x_label, sizeof info.x_label, "Time");
    snprintf(info.y_label, sizeof info.y_label, "Velocity");
    ps_plot_handle plot = {0};
    if (r == PS_OK)
        r = ps_report_unit_from(PS_SECOND, &info.x_unit);
    if (r == PS_OK)
        r = ps_report_unit_from(PS_VELOCITY, &info.y_unit);
    if (r == PS_OK)
        r = ps_report_add_plot(report, &info, &plot);
    if (r == PS_OK)
        r = ps_report_add_series(report, plot, ctx, time, velocity, "dx/dt", PS_PLOT_LINE);
    char path[4096];
    if (r == PS_OK) {
        int n = snprintf(path, sizeof path, "%s.psreport", prefix);
        r = n < 0 || (size_t)n >= sizeof path ? PS_LIMIT : ps_report_save(report, path);
    }
    if (r == PS_OK) {
        int n = snprintf(path, sizeof path, "%s-velocity.csv", prefix);
        ps_series columns[] = {time, position, velocity};
        r = n < 0 || (size_t)n >= sizeof path ? PS_LIMIT
                                              : ps_series_export_csv(ctx, columns, 3, path);
    }
    ps_report_destroy(report);
    ps_analysis_destroy(ctx);
    return r == PS_OK && recovered ? PS_RECOVERED : r;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api = {sizeof(ps_analysis_api), PS_ABI_VERSION,
                                        "Uniform motion analysis", analyze, NULL};
    return &api;
}
