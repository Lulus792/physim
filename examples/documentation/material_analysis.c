#include "physim/report.h"
#include <stdio.h>
static ps_result plot(ps_report *report, ps_analysis_context *context, ps_series time,
                      ps_series *values, const char *const *labels, unsigned count,
                      const char *title, ps_unit unit) {
    ps_plot_info info = {0}; snprintf(info.title, sizeof info.title, "%s", title);
    snprintf(info.x_label, sizeof info.x_label, "Time"); snprintf(info.y_label, sizeof info.y_label, "%s", title);
    ps_result r = ps_report_unit_from(PS_SECOND, &info.x_unit);
    if (r == PS_OK) r = ps_report_unit_from(unit, &info.y_unit);
    ps_plot_handle handle;
    if (r == PS_OK) r = ps_report_add_plot(report, &info, &handle);
    for (unsigned i = 0; r == PS_OK && i < count; i++)
        r = ps_report_add_series(report, handle, context, time, values[i], labels[i], PS_PLOT_LINE);
    return r;
}
static ps_result analyze(const char *input, const char *prefix) {
    ps_analysis_context *context = NULL; ps_report *report = NULL; ps_dataset run = {0};
    ps_series time = {0}, values[7] = {0}; bool recovered = false;
    ps_result r = ps_analysis_create(prefix, 0, &context);
    if (r == PS_OK) { r = ps_analysis_open_run(context, input, &run); recovered = r == PS_RECOVERED; if (recovered) r = PS_OK; }
    if (r == PS_OK) r = ps_dataset_series(context, run, "time", &time);
    const char *names[] = {"velocity.y", "reference.velocity", "energy.kinetic", "energy.potential", "energy.dissipated", "energy.balance"};
    for (unsigned i = 0; r == PS_OK && i < 6; i++) r = ps_dataset_series(context, run, names[i], &values[i]);
    if (r == PS_OK) r = ps_series_combine(context, PS_SERIES_SUBTRACT, values[0], values[1], &values[6]);
    if (r == PS_OK) r = ps_report_create("Custom material and medium", "Stokes settling and energy accounting", &report);
    const char *velocity_labels[] = {"RK4", "analytic"}, *energy_labels[] = {"kinetic", "effective potential", "dissipated", "total"}, *error_labels[] = {"RK4 minus analytic"};
    if (r == PS_OK) r = plot(report, context, time, values, velocity_labels, 2, "Vertical velocity", PS_VELOCITY);
    if (r == PS_OK) r = plot(report, context, time, &values[6], error_labels, 1, "Velocity error", PS_VELOCITY);
    if (r == PS_OK) r = plot(report, context, time, &values[2], energy_labels, 4, "Energy accounting", PS_JOULE);
    char path[4096];
    if (r == PS_OK) {
        int n = snprintf(path, sizeof path, "%s.psreport", prefix);
        r = n < 0 || (size_t)n >= sizeof path ? PS_LIMIT : ps_report_save(report, path);
    }
    if (r == PS_OK) {
        int n = snprintf(path, sizeof path, "%s-material_check.csv", prefix);
        ps_series columns[] = {time, values[0], values[1], values[6], values[5]};
        r = n < 0 || (size_t)n >= sizeof path ? PS_LIMIT : ps_series_export_csv(context, columns, 5, path);
    }
    ps_report_destroy(report); ps_analysis_destroy(context); return r == PS_OK && recovered ? PS_RECOVERED : r;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api = {.struct_size=sizeof api, .abi_version=PS_ABI_VERSION, .name="Custom material and medium analysis", .run=analyze}; return &api;
}
