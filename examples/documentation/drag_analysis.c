#include "physim/report.h"
#include <stdio.h>

static ps_result analyze(const char *input, const char *prefix) {
    ps_analysis_context *context = NULL;
    ps_report *report = NULL;
    ps_dataset run = {0};
    ps_series time = {0}, position = {0}, velocity = {0};
    ps_result result = ps_analysis_create(prefix, 0, &context);
    bool recovered = false;
    if (result == PS_OK) {
        result = ps_analysis_open_run(context, input, &run);
        recovered = result == PS_RECOVERED;
        if (recovered)
            result = PS_OK;
    }
    if (result == PS_OK)
        result = ps_dataset_series(context, run, "time", &time);
    if (result == PS_OK)
        result = ps_dataset_series(context, run, "position.x", &position);
    if (result == PS_OK)
        result = ps_series_derivative(context, position, time, &velocity);
    if (result == PS_OK)
        result = ps_report_create("Projectile with quadratic air drag", "Horizontal velocity", &report);
    ps_plot_info info = {0};
    snprintf(info.title, sizeof info.title, "Horizontal velocity");
    snprintf(info.x_label, sizeof info.x_label, "Time");
    snprintf(info.y_label, sizeof info.y_label, "Velocity");
    ps_plot_handle plot = {0};
    if (result == PS_OK)
        result = ps_report_unit_from(PS_SECOND, &info.x_unit);
    if (result == PS_OK)
        result = ps_report_unit_from(PS_VELOCITY, &info.y_unit);
    if (result == PS_OK)
        result = ps_report_add_plot(report, &info, &plot);
    if (result == PS_OK)
        result = ps_report_add_series(report, plot, context, time, velocity, "dx/dt", PS_PLOT_LINE);
    char path[4096];
    if (result == PS_OK) {
        int size = snprintf(path, sizeof path, "%s.psreport", prefix);
        result = size < 0 || (size_t)size >= sizeof path ? PS_LIMIT : ps_report_save(report, path);
    }
    if (result == PS_OK) {
        int size = snprintf(path, sizeof path, "%s-horizontal_velocity.csv", prefix);
        ps_series columns[] = {time, position, velocity};
        result = size < 0 || (size_t)size >= sizeof path
                     ? PS_LIMIT
                     : ps_series_export_csv(context, columns, 3, path);
    }
    ps_report_destroy(report);
    ps_analysis_destroy(context);
    return result == PS_OK && recovered ? PS_RECOVERED : result;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api = {sizeof api, PS_ABI_VERSION,
                                        "Projectile with quadratic air drag analysis", analyze, NULL};
    return &api;
}
