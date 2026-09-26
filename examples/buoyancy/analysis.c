#include "physim/analysis.h"
#include "physim/report.h"
#include "physim/series.h"
#include <stdio.h>
#include <string.h>

static ps_result add_plot(ps_report *report, ps_analysis_context *context, ps_series time,
                          ps_series value, const char *title, const char *label,
                          ps_unit unit) {
    ps_plot_info info = {0};
    snprintf(info.title, sizeof info.title, "%s", title);
    snprintf(info.x_label, sizeof info.x_label, "Time");
    snprintf(info.y_label, sizeof info.y_label, "%s", label);
    ps_result result = ps_report_unit_from(PS_SECOND, &info.x_unit);
    if (result == PS_OK)
        result = ps_report_unit_from(unit, &info.y_unit);
    ps_plot_handle plot = {0};
    if (result == PS_OK)
        result = ps_report_add_plot(report, &info, &plot);
    if (result == PS_OK)
        result = ps_report_add_series(report, plot, context, time, value, label, PS_PLOT_LINE);
    return result;
}
static ps_result analyze(const char *input, const char *prefix) {
    ps_analysis_context *context = NULL;
    ps_report *report = NULL;
    ps_dataset run = {0};
    ps_series time = {0}, position = {0}, fraction = {0}, balance = {0};
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
        result = ps_dataset_series(context, run, "position.y", &position);
    if (result == PS_OK)
        result = ps_dataset_series(context, run, "submerged.fraction", &fraction);
    if (result == PS_OK)
        result = ps_dataset_series(context, run, "energy.balance", &balance);
    if (result == PS_OK)
        result = ps_report_create("Auftrieb · schwimmende Kugel", "Höhe, Eintauchen und Energiebilanz", &report);
    if (result == PS_OK)
        result = add_plot(report, context, time, position, "Höhe über Wasseroberfläche",
                          "position.y", PS_METRE);
    if (result == PS_OK)
        result = add_plot(report, context, time, fraction, "Eingetauchter Anteil",
                          "submerged.fraction", PS_ONE);
    if (result == PS_OK)
        result = add_plot(report, context, time, balance, "Energiebilanz",
                          "energy.balance", PS_JOULE);
    char path[4096];
    if (result == PS_OK) {
        int n = snprintf(path, sizeof path, "%s.psreport", prefix);
        result = n < 0 || (size_t)n >= sizeof path ? PS_LIMIT : ps_report_save(report, path);
    }
    if (result == PS_OK) {
        int n = snprintf(path, sizeof path, "%s-position.csv", prefix);
        ps_series columns[] = {time, position};
        result = n < 0 || (size_t)n >= sizeof path
                     ? PS_LIMIT : ps_series_export_csv(context, columns, 2, path);
    }
    ps_report_destroy(report);
    ps_analysis_destroy(context);
    return result == PS_OK && recovered ? PS_RECOVERED : result;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api = {sizeof api, PS_ABI_VERSION,
                                        "Auftriebsauswertung", analyze, NULL};
    return &api;
}
