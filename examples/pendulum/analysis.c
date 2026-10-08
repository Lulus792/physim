#include "physim/analysis.h"
#include "physim/report.h"
#include "physim/series.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#define STRINGIFY_VALUE_(x) #x
#define STRINGIFY_VALUE(x) STRINGIFY_VALUE_(x)
#if defined(__clang__)
#define ANALYSIS_COMPILER "Clang " __clang_version__
#elif defined(_MSC_VER)
#define ANALYSIS_COMPILER "MSVC " STRINGIFY_VALUE(_MSC_VER)
#else
#define ANALYSIS_COMPILER __VERSION__
#endif
/* Sensor rows remain aligned with the physical model in the raw run. Only
 * status=1 enters this report; no interpolation or held-value statistics. */
static ps_result add_sensor_report(ps_analysis_context *ctx, ps_dataset run, ps_report *report,
                                   const char *prefix) {
    const char *names[] = {"sensor.x.status", "sensor.time", "position.x", "nominal.x",
                           "sensor.x",        "sensor.x.u",  "time"};
    const ps_unit units[] = {PS_ONE, PS_SECOND, PS_METRE, PS_METRE, PS_METRE, PS_METRE, PS_SECOND};
    ps_series series[7] = {0};
    ps_result r = ps_dataset_series(ctx, run, names[0], &series[0]);
    if (r == PS_INVALID)
        return PS_OK; /* Templates and historical runs without sensors. */
    if (r != PS_OK)
        return r;
    ps_series_info info = {0};
    uint64_t count = 0;
    for (unsigned i = 0; i < 7 && r == PS_OK; i++) {
        if (i)
            r = ps_dataset_series(ctx, run, names[i], &series[i]);
        if (r == PS_OK)
            r = ps_series_describe(ctx, series[i], &info);
        if (!i)
            count = info.count;
        if (r == PS_OK && (info.count != count || memcmp(info.dimension, units[i].dimension, 7) ||
                           info.scale != 1))
            r = PS_INVALID;
    }
    char path[4096];
    int n = snprintf(path, sizeof path, "%s-sensor.csv", prefix);
    if (r == PS_OK && (n < 0 || (size_t)n >= sizeof path))
        r = PS_LIMIT;
    FILE *csv = r == PS_OK ? fopen(path, "wx") : NULL;
    if (r == PS_OK && !csv)
        r = PS_IO;
    if (r != PS_OK)
        return r;
    if (fputs("time_s,model_x_m,nominal_x_m,sensor_x_m,error_m,standard_uncertainty_m\n", csv) < 0)
        r = PS_IO;
    double values[7][PS_SERIES_BLOCK_SIZE];
    uint64_t states[3] = {0};
    ps_statistics errors = {0}, uncertainty = {0};
    double max_error = 0;
    for (uint64_t at = 0; at < count && r == PS_OK; at += PS_SERIES_BLOCK_SIZE) {
        size_t size =
            (size_t)(count - at < PS_SERIES_BLOCK_SIZE ? count - at : PS_SERIES_BLOCK_SIZE);
        for (unsigned c = 0; c < 7 && r == PS_OK; c++) {
            size_t got = 0;
            r = ps_series_read(ctx, series[c], at, values[c], size, &got);
            if (r == PS_OK && got != size)
                r = PS_CORRUPT;
        }
        for (size_t i = 0; i < size && r == PS_OK; i++) {
            double status = values[0][i];
            if (status != 0 && status != 1 && status != 2) {
                r = PS_CORRUPT;
                break;
            }
            states[(unsigned)status]++;
            if (status != 1)
                continue;
            if (values[5][i] < 0 ||
                fabs(values[1][i] - values[6][i]) > 1e-10 * fmax(1, fabs(values[6][i]))) {
                r = PS_INVALID;
                break;
            }
            double error = values[4][i] - values[2][i];
            if (!isfinite(error)) {
                r = PS_NUMERIC;
                break;
            }
            ps_statistics_push(&errors, error);
            ps_statistics_push(&uncertainty, values[5][i]);
            max_error = fmax(max_error, fabs(error));
            if (fprintf(csv, "%.17g,%.17g,%.17g,%.17g,%.17g,%.17g\n", values[1][i], values[2][i],
                        values[3][i], values[4][i], error, values[5][i]) < 0)
                r = PS_IO;
        }
    }
    if (fclose(csv))
        r = PS_IO;
    if (r != PS_OK)
        return r;
    ps_plot_info plot = {0};
    ps_plot_handle ph = {0};
    strcpy(plot.title, "Sollbahn, Modell und Sensor");
    strcpy(plot.x_label, "Zeit");
    strcpy(plot.y_label, "Position x");
    ps_report_unit_from(PS_SECOND, &plot.x_unit);
    ps_report_unit_from(PS_METRE, &plot.y_unit);
    r = ps_report_add_plot(report, &plot, &ph);
    if (r == PS_OK)
        r = ps_report_add_series(report, ph, ctx, series[6], series[3], "Sollbahn", PS_PLOT_LINE);
    if (r == PS_OK)
        r = ps_report_add_series(report, ph, ctx, series[6], series[2], "Physikalisches Modell",
                                 PS_PLOT_LINE);
    if (r == PS_OK && states[1]) {
        ps_series columns[] = {series[1], series[4]}, selected[2];
        r = ps_series_select(ctx, columns, 2, series[0], 1, selected);
        if (r == PS_OK) {
            r = ps_report_add_series(report, ph, ctx, selected[0], selected[1],
                                     "Sensor x · nur gültige Messungen", PS_PLOT_SCATTER);
            (void)ps_series_release(ctx, selected[0]);
            (void)ps_series_release(ctx, selected[1]);
        }
    }
    ps_table_info table = {0};
    ps_table_handle th = {0};
    strcpy(table.title, "Sensor x · Messstatus des gesamten Laufs");
    table.columns = 3;
    const char *labels[] = {"Gültig", "Ausgefallen", "Nicht fällig"};
    for (unsigned i = 0; i < 3; i++) {
        strcpy(table.column[i].label, labels[i]);
        ps_report_unit_from(PS_ONE, &table.column[i].unit);
    }
    if (r == PS_OK)
        r = ps_report_add_table(report, &table, &th);
    ps_table_row row = {"Messpunkte", {(double)states[1], (double)states[2], (double)states[0]}};
    if (r == PS_OK)
        r = ps_report_add_row(report, th, &row);
    if (r == PS_OK && states[1]) {
        memset(&table, 0, sizeof table);
        strcpy(table.title, "Sensor x · Abweichung vom Modell");
        const char *metrics[] = {"Mittlerer Fehler", "Max. absoluter Fehler",
                                 "Mittlere Standardunsicherheit", "Std.-Abw. des Fehlers"};
        table.columns = states[1] > 1 ? 4 : 3;
        for (unsigned i = 0; i < table.columns; i++) {
            strcpy(table.column[i].label, metrics[i]);
            ps_report_unit_from(PS_METRE, &table.column[i].unit);
        }
        r = ps_report_add_table(report, &table, &th);
        row = (ps_table_row){
            "Ohne Offset-/Driftkorrektur",
            {errors.mean, max_error, uncertainty.mean, ps_statistics_stddev(&errors)}};
        if (r == PS_OK)
            r = ps_report_add_row(report, th, &row);
    }
    return r;
}
static ps_result make_report(ps_analysis_context *ctx, ps_dataset run, ps_series time,
                             ps_series position, ps_series velocity, ps_series filtered,
                             const char *input, const char *prefix) {
    ps_dataset_info dataset = {0};
    ps_result result = ps_dataset_describe(ctx, run, &dataset);
    if (result != PS_OK)
        return result;
    char provenance[8192];
    int n =
        snprintf(provenance, sizeof provenance,
                 "input=%s\nanalysis_source=%s.source.c\ncompiler=%s\nbuild=%s %s\nrecovered=%d\n",
                 input, prefix, ANALYSIS_COMPILER, __DATE__, __TIME__, dataset.recovered);
    if (n < 0 || (size_t)n >= sizeof provenance)
        return PS_LIMIT;
    ps_report *report = NULL;
    result = ps_report_create("Bewegung aus Messdaten", provenance, &report);
    ps_plot_info plot = {0};
    ps_plot_handle handle = {0};
    strcpy(plot.title, "Geschwindigkeit aus der Position");
    strcpy(plot.x_label, "Zeit");
    strcpy(plot.y_label, "Geschwindigkeit");
    ps_report_unit_from(PS_SECOND, &plot.x_unit);
    ps_report_unit_from(PS_VELOCITY, &plot.y_unit);
    if (result == PS_OK)
        result = ps_report_add_plot(report, &plot, &handle);
    if (result == PS_OK)
        result =
            ps_report_add_series(report, handle, ctx, time, velocity, "Ableitung", PS_PLOT_LINE);
    if (result == PS_OK)
        result = ps_report_add_series(report, handle, ctx, time, filtered, "Gleitendes Mittel (5)",
                                      PS_PLOT_LINE);
    strcpy(plot.title, "Phasenraum");
    strcpy(plot.x_label, "Position");
    ps_report_unit_from(PS_METRE, &plot.x_unit);
    if (result == PS_OK)
        result = ps_report_add_plot(report, &plot, &handle);
    if (result == PS_OK)
        result = ps_report_add_series(report, handle, ctx, position, velocity,
                                      "Position / Geschwindigkeit", PS_PLOT_SCATTER);
    if (result == PS_OK)
        result = ps_report_add_histogram(report, ctx, velocity, "Verteilung der Geschwindigkeit",
                                         "Geschwindigkeit", 24, &handle);
    /* A dissipative model may expose its accounted energy separately. Mechanical
     * energy loss then is physical, while balance drift measures numerical error. */
    ps_series balance = {0}, mechanical = {0}, dissipated = {0};
    ps_result balance_result = ps_dataset_series(ctx, run, "energy.balance", &balance);
    if (result == PS_OK && balance_result != PS_OK && balance_result != PS_INVALID)
        result = balance_result;
    if (result == PS_OK && balance_result == PS_OK) {
        result = ps_dataset_series(ctx, run, "energy", &mechanical);
        if (result == PS_OK)
            result = ps_dataset_series(ctx, run, "energy.dissipated", &dissipated);
        memset(&plot, 0, sizeof plot);
        strcpy(plot.title, "Energiebilanz mit Dämpfung");
        strcpy(plot.x_label, "Zeit");
        strcpy(plot.y_label, "Energie");
        ps_report_unit_from(PS_SECOND, &plot.x_unit);
        ps_report_unit_from(PS_JOULE, &plot.y_unit);
        if (result == PS_OK)
            result = ps_report_add_plot(report, &plot, &handle);
        ps_series series[] = {mechanical, dissipated, balance};
        const char *labels[] = {"Mechanisch", "Dissipiert", "Gesamtbilanz"};
        for (unsigned i = 0; i < 3 && result == PS_OK; i++)
            result =
                ps_report_add_series(report, handle, ctx, time, series[i], labels[i], PS_PLOT_LINE);
    }
    if (result == PS_OK && balance_result == PS_INVALID) {
        ps_series angle = {0}, change = {0};
        ps_result present = ps_dataset_series(ctx, run, "angle", &angle);
        if (present == PS_OK)
            present = ps_dataset_series(ctx, run, "energy", &mechanical);
        if (present != PS_OK && present != PS_INVALID)
            result = present;
        if (present == PS_OK) {
            double initial = 0;
            size_t count = 0;
            result = ps_series_read(ctx, mechanical, 0, &initial, 1, &count);
            if (result == PS_OK && count != 1)
                result = PS_INVALID;
            if (result == PS_OK)
                result = ps_series_affine(ctx, mechanical, 1,
                                          (ps_quantity){-initial, PS_JOULE}, &change);
            memset(&plot, 0, sizeof plot);
            strcpy(plot.title, "Mechanische Energieänderung");
            strcpy(plot.x_label, "Zeit");
            strcpy(plot.y_label, "E - E(0)");
            ps_report_unit_from(PS_SECOND, &plot.x_unit);
            ps_report_unit_from(PS_JOULE, &plot.y_unit);
            if (result == PS_OK)
                result = ps_report_add_plot(report, &plot, &handle);
            if (result == PS_OK)
                result = ps_report_add_series(report, handle, ctx, time, change,
                                              "Energieänderung", PS_PLOT_LINE);
            char path[4096];
            int size = snprintf(path, sizeof path, "%s-energy.csv", prefix);
            ps_series columns[] = {time, mechanical, change};
            if (result == PS_OK)
                result = size < 0 || (size_t)size >= sizeof path ? PS_LIMIT
                    : ps_series_export_csv(ctx, columns, 3, path);
        }
    }
    ps_table_info table = {0};
    strcpy(table.title, "Geschwindigkeit · vollständiger Lauf");
    table.columns = 4;
    const char *names[] = {"Mittelwert", "Std.-Abw.", "Minimum", "Maximum"};
    for (unsigned i = 0; i < 4; i++) {
        strcpy(table.column[i].label, names[i]);
        ps_report_unit_from(PS_VELOCITY, &table.column[i].unit);
    }
    ps_table_handle th = {0};
    if (result == PS_OK)
        result = ps_report_add_table(report, &table, &th);
    ps_series rows[] = {velocity, filtered};
    const char *labels[] = {"Ableitung", "Gleitendes Mittel"};
    for (unsigned i = 0; i < 2 && result == PS_OK; i++) {
        ps_statistics stats = {0};
        result = ps_series_statistics(ctx, rows[i], &stats);
        ps_table_row row = {{0}, {stats.mean, ps_statistics_stddev(&stats), stats.min, stats.max}};
        strcpy(row.label, labels[i]);
        if (result == PS_OK)
            result = ps_report_add_row(report, th, &row);
    }
    /* These metrics use every original sample, independent of plot reduction. */
    if (result == PS_OK) {
        ps_run_reader reader;
        result = ps_run_open(&reader, input);
        if (result == PS_OK) {
            int energy = -1, angle = -1, balance_channel = -1;
            for (uint32_t i = 0; i < reader.channels; i++) {
                if (!strcmp(reader.schema[i].name, "energy"))
                    energy = (int)i;
                if (!strcmp(reader.schema[i].name, "angle"))
                    angle = (int)i;
                if (!strcmp(reader.schema[i].name, "energy.balance"))
                    balance_channel = (int)i;
            }
            if (balance_channel >= 0)
                energy = balance_channel;
            double t = 0, values[PS_MAX_CHANNELS], initial = 0, drift = 0, previous = 0,
                   previous_t = 0, crossing = 0, sum = 0;
            unsigned intervals = 0;
            bool had_crossing = false;
            while ((result = ps_run_next(&reader, &t, values)) == PS_OK) {
                if (energy >= 0) {
                    if (reader.samples == 1)
                        initial = values[energy];
                    drift = fmax(drift, fabs(values[energy] - initial));
                }
                if (angle >= 0) {
                    if (reader.samples > 1 && previous < 0 && values[angle] >= 0) {
                        double cross = previous_t +
                                       (t - previous_t) * (-previous) / (values[angle] - previous);
                        if (had_crossing) {
                            sum += cross - crossing;
                            intervals++;
                        }
                        crossing = cross;
                        had_crossing = true;
                    }
                    previous = values[angle];
                }
                previous_t = t;
            }
            ps_run_reader_close(&reader);
            if (result == PS_EOF || result == PS_RECOVERED)
                result = PS_OK;
            if (result == PS_OK && energy >= 0) {
                memset(&table, 0, sizeof table);
                strcpy(table.title, balance_channel >= 0 ? "Energiebilanz · inklusive Dissipation"
                                    : angle < 0          ? "Energieprüfung"
                                    : intervals          ? "Energie und Periodendauer"
                                                : "Energie · keine vollständige Periode gemessen");
                table.columns = intervals ? 2 : 1;
                strcpy(table.column[0].label,
                       balance_channel >= 0 ? "Max. Bilanzabweichung" : "Max. Energieabweichung");
                ps_report_unit_from(PS_JOULE, &table.column[0].unit);
                if (intervals) {
                    strcpy(table.column[1].label, "Periodendauer");
                    ps_report_unit_from(PS_SECOND, &table.column[1].unit);
                }
                result = ps_report_add_table(report, &table, &th);
                ps_table_row row = {"Messung", {drift, intervals ? sum / intervals : 0}};
                if (result == PS_OK)
                    result = ps_report_add_row(report, th, &row);
            }
        }
    }
    if (result == PS_OK)
        result = add_sensor_report(ctx, run, report, prefix);
    if (result == PS_OK) {
        char path[4096];
        n = snprintf(path, sizeof path, "%s.psreport", prefix);
        result = n < 0 || (size_t)n >= sizeof path ? PS_LIMIT : ps_report_save(report, path);
    }
    ps_report_destroy(report);
    return result;
}
/* Analysis runs in its own process. Change this callback to add your own report. */
static ps_result analyze(const char *input_run, const char *output_prefix) {
    ps_result result = ps_analyze_run(input_run, output_prefix);
    bool recovered = result == PS_RECOVERED;
    if (result != PS_OK && !recovered)
        return result;
    ps_analysis_context *ctx = NULL;
    result = ps_analysis_create(output_prefix, 0, &ctx);
    if (result != PS_OK)
        return result;
    ps_dataset run = {0};
    ps_series time = {0}, position = {0}, velocity = {0}, filtered = {0}, reconstructed = {0};
    result = ps_analysis_open_run(ctx, input_run, &run);
    if (result == PS_RECOVERED) {
        recovered = true;
        result = PS_OK;
    }
    if (result == PS_OK) {
        ps_dataset_info info;
        result = ps_dataset_describe(ctx, run, &info);
        if (result == PS_OK && info.samples < 2) {
            puts("Derivative export omitted: at least two samples are required.");
            ps_analysis_destroy(ctx);
            return recovered ? PS_RECOVERED : PS_OK;
        }
    }
    if (result == PS_OK)
        result = ps_dataset_series(ctx, run, "time", &time);
    if (result == PS_OK) {
        /* Prefer horizontal motion; vertical-only templates expose position.y. */
        result = ps_dataset_series(ctx, run, "position.x", &position);
        if (result != PS_OK)
            result = ps_dataset_series(ctx, run, "a.position", &position);
        if (result == PS_INVALID)
            result = ps_dataset_series(ctx, run, "position.y", &position);
        if (result == PS_INVALID) {
            puts("Derivative export omitted: no position channel; summary is available.");
            ps_analysis_destroy(ctx);
            return recovered ? PS_RECOVERED : PS_OK;
        }
    }
    if (result == PS_OK)
        result = ps_series_derivative(ctx, position, time, &velocity);
    if (result == PS_OK)
        result = ps_series_moving_average(ctx, velocity, 5, &filtered);
    if (result == PS_OK) {
        double value = 0;
        size_t count = 0;
        ps_series_info info = {0};
        result = ps_series_read(ctx, position, 0, &value, 1, &count);
        if (result == PS_OK && count != 1)
            result = PS_INVALID;
        if (result == PS_OK)
            result = ps_series_describe(ctx, position, &info);
        if (result == PS_OK) {
            ps_quantity initial = {value, {{0}, info.scale, NULL}};
            memcpy(initial.unit.dimension, info.dimension, 7);
            result = ps_series_integral(ctx, velocity, time, initial, &reconstructed);
        }
    }
    if (result == PS_OK) {
        char path[4096];
        int length = snprintf(path, sizeof path, "%s-derived.csv", output_prefix);
        if (length < 0 || (size_t)length >= sizeof path)
            result = PS_INVALID;
        else {
            ps_series columns[] = {time, position, velocity, filtered, reconstructed};
            result = ps_series_export_csv(ctx, columns, 5, path);
        }
    }
    if (result == PS_OK)
        result =
            make_report(ctx, run, time, position, velocity, filtered, input_run, output_prefix);
    ps_analysis_destroy(ctx);
    return result == PS_OK && recovered ? PS_RECOVERED : result;
}
static const char *filename(const char *path) {
    const char *name = path;
    for (const char *p = path; *p; p++)
        if (*p == '/' || *p == '\\')
            name = p + 1;
    return name;
}
static void run_label(char out[96], size_t index, const char *path) {
    int n = snprintf(out, 96, "Lauf %zu · ", index + 1);
    const char *name = filename(path);
    size_t length = strlen(name), capacity = 95 - (size_t)n;
    if (length > capacity) {
        length = capacity;
        while (length && ((unsigned char)name[length] & 0xc0) == 0x80)
            length--;
    }
    memcpy(out + n, name, length);
    out[n + length] = 0;
}
/* Dataset times are strictly increasing. Find the first value >= bound (or >). */
static ps_result time_bound(ps_analysis_context *ctx, ps_series time, uint64_t count, double bound,
                            bool upper, uint64_t *out) {
    uint64_t left = 0, right = count;
    while (left < right) {
        uint64_t middle = left + (right - left) / 2;
        double value;
        size_t got;
        ps_result r = ps_series_read(ctx, time, middle, &value, 1, &got);
        if (r != PS_OK || got != 1)
            return r == PS_OK ? PS_CORRUPT : r;
        if (value < bound || (upper && value == bound))
            left = middle + 1;
        else
            right = middle;
    }
    *out = left;
    return PS_OK;
}
static ps_result add_difference(ps_analysis_context *ctx, ps_report *report, ps_plot_handle plot,
                                ps_table_handle table, ps_series reference_time,
                                ps_series reference_position, ps_series time, ps_series position,
                                const char *label, const char *prefix, size_t run_index) {
    ps_series_info source = {0}, target = {0};
    ps_result r = ps_series_describe(ctx, reference_time, &source);
    if (r == PS_OK)
        r = ps_series_describe(ctx, time, &target);
    if (r != PS_OK || !source.count || !target.count)
        return r == PS_OK ? PS_INVALID : r;
    double start = 0, end = 0;
    size_t got = 0;
    r = ps_series_read(ctx, reference_time, 0, &start, 1, &got);
    if (r == PS_OK)
        r = ps_series_read(ctx, reference_time, source.count - 1, &end, 1, &got);
    uint64_t first = 0, stop = 0;
    if (r == PS_OK)
        r = time_bound(ctx, time, target.count, start, false, &first);
    if (r == PS_OK)
        r = time_bound(ctx, time, target.count, end, true, &stop);
    if (r != PS_OK || first == stop)
        return r; /* no samples in overlap: no invented zero-valued difference */
    ps_series series[4] = {0};
    r = ps_series_slice(ctx, time, first, stop - first, &series[0]);
    if (r == PS_OK)
        r = ps_series_slice(ctx, position, first, stop - first, &series[1]);
    if (r == PS_OK)
        r = ps_series_resample_linear(ctx, reference_position, reference_time, series[0],
                                      &series[2]);
    if (r == PS_OK)
        r = ps_series_combine(ctx, PS_SERIES_SUBTRACT, series[1], series[2], &series[3]);
    ps_statistics stats = {0};
    if (r == PS_OK)
        r = ps_series_statistics(ctx, series[3], &stats);
    if (r == PS_OK)
        r = ps_report_add_series(report, plot, ctx, series[0], series[3], label, PS_PLOT_LINE);
    ps_table_row row = {{0}, {stats.mean, ps_statistics_stddev(&stats), stats.min, stats.max}};
    strcpy(row.label, label);
    if (r == PS_OK)
        r = ps_report_add_row(report, table, &row);
    if (r == PS_OK) {
        char path[4096];
        int n = snprintf(path, sizeof path, "%s-difference-%zu.csv", prefix, run_index + 1);
        r = n < 0 || (size_t)n >= sizeof path ? PS_LIMIT
                                              : ps_series_export_csv(ctx, series, 4, path);
    }
    for (size_t i = 0; i < 4; i++)
        if (series[i].owner)
            ps_series_release(ctx, series[i]);
    return r;
}
static ps_result analyze_many(const char *const *inputs, size_t count, const char *prefix) {
    if (!inputs || count < 1 || count > PS_ANALYSIS_MAX_INPUTS)
        return PS_INVALID;
    if (count == 1)
        return analyze(inputs[0], prefix);
    char provenance[8192];
    int n =
        snprintf(provenance, sizeof provenance,
                 "Eingaben: %zu Läufe, Reihenfolge wie im "
                 "Eingabemanifest.\ninput_manifest=%s.inputs.csv\nanalysis_source=%s.source."
                 "c\ncompiler=%s\nbuild=%s %s\n"
                 "Vergleichskurven: ursprüngliche Zeitachsen.\n"
                 "Positionsdifferenz: Lauf N minus Lauf 1; Referenz linear auf die Zeitpunkte "
                 "von Lauf N interpoliert, nur im gemeinsamen Zeitbereich, keine Extrapolation.\n"
                 "Ohne gemeinsame Messzeitpunkte entfällt die Differenzkurve/-zeile. "
                 "Kennzahlen gewichten jeden Messpunkt gleich.",
                 count, filename(prefix), filename(prefix), ANALYSIS_COMPILER, __DATE__, __TIME__);
    if (n < 0 || (size_t)n >= sizeof provenance)
        return PS_LIMIT;
    ps_report *report = NULL;
    ps_analysis_context *ctx = NULL;
    ps_result result = ps_report_create("Läufe vergleichen", provenance, &report);
    if (result == PS_OK)
        result = ps_analysis_create(prefix, 0, &ctx);
    ps_plot_info plot = {0};
    ps_plot_handle velocity_plot = {0}, position_plot = {0}, difference_plot = {0};
    strcpy(plot.title, "Geschwindigkeit im Vergleich");
    strcpy(plot.x_label, "Zeit");
    strcpy(plot.y_label, "Geschwindigkeit");
    ps_report_unit_from(PS_SECOND, &plot.x_unit);
    ps_report_unit_from(PS_VELOCITY, &plot.y_unit);
    if (result == PS_OK)
        result = ps_report_add_plot(report, &plot, &velocity_plot);
    strcpy(plot.title, "Position im Vergleich");
    strcpy(plot.y_label, "Position");
    ps_report_unit_from(PS_METRE, &plot.y_unit);
    if (result == PS_OK)
        result = ps_report_add_plot(report, &plot, &position_plot);
    strcpy(plot.title, "Positionsdifferenz zu Lauf 1 · linear interpoliert");
    strcpy(plot.y_label, "Positionsdifferenz");
    if (result == PS_OK)
        result = ps_report_add_plot(report, &plot, &difference_plot);
    ps_table_info table = {0};
    table.columns = 4;
    strcpy(table.title, "Geschwindigkeit · alle Messpunkte");
    const char *columns[] = {"Mittelwert", "Std.-Abw.", "Minimum", "Maximum"};
    for (unsigned i = 0; i < 4; i++) {
        strcpy(table.column[i].label, columns[i]);
        ps_report_unit_from(PS_VELOCITY, &table.column[i].unit);
    }
    ps_table_handle statistics = {0}, difference_statistics = {0};
    if (result == PS_OK)
        result = ps_report_add_table(report, &table, &statistics);
    strcpy(table.title, "Positionsdifferenz zu Lauf 1 · gemeinsamer Zeitbereich");
    for (unsigned i = 0; i < 4; i++)
        ps_report_unit_from(PS_METRE, &table.column[i].unit);
    if (result == PS_OK)
        result = ps_report_add_table(report, &table, &difference_statistics);
    bool recovered = false;
    ps_series reference_time = {0}, reference_position = {0};
    for (size_t i = 0; i < count && result == PS_OK; i++) {
        ps_dataset run = {0};
        ps_series time = {0}, position = {0}, velocity = {0};
        result = ps_analysis_open_run(ctx, inputs[i], &run);
        if (result == PS_RECOVERED) {
            recovered = true;
            result = PS_OK;
        }
        if (result == PS_OK)
            result = ps_dataset_series(ctx, run, "time", &time);
        if (result == PS_OK) {
            result = ps_dataset_series(ctx, run, "position.x", &position);
            if (result == PS_INVALID)
                result = ps_dataset_series(ctx, run, "a.position", &position);
            if (result == PS_INVALID)
                result = ps_dataset_series(ctx, run, "position.y", &position);
        }
        if (result == PS_OK)
            result = ps_series_derivative(ctx, position, time, &velocity);
        char label[96];
        run_label(label, i, inputs[i]);
        if (result == PS_OK)
            result = ps_report_add_series(report, velocity_plot, ctx, time, velocity, label,
                                          PS_PLOT_LINE);
        if (result == PS_OK)
            result = ps_report_add_series(report, position_plot, ctx, time, position, label,
                                          PS_PLOT_LINE);
        ps_statistics stats = {0};
        if (result == PS_OK)
            result = ps_series_statistics(ctx, velocity, &stats);
        ps_table_row row = {{0}, {stats.mean, ps_statistics_stddev(&stats), stats.min, stats.max}};
        strcpy(row.label, label);
        if (result == PS_OK)
            result = ps_report_add_row(report, statistics, &row);
        if (result == PS_OK) {
            char path[4096];
            n = snprintf(path, sizeof path, "%s-run-%zu.csv", prefix, i + 1);
            ps_series series[] = {time, position, velocity};
            result = n < 0 || (size_t)n >= sizeof path ? PS_LIMIT
                                                       : ps_series_export_csv(ctx, series, 3, path);
        }
        if (!i && result == PS_OK) {
            reference_time = time;
            reference_position = position;
            ps_series_release(ctx, velocity);
        } else if (i && result == PS_OK)
            result =
                add_difference(ctx, report, difference_plot, difference_statistics, reference_time,
                               reference_position, time, position, label, prefix, i);
        if (run.owner && i)
            ps_dataset_close(ctx, run);
    }
    if (result == PS_OK) {
        char path[4096];
        n = snprintf(path, sizeof path, "%s.psreport", prefix);
        result = n < 0 || (size_t)n >= sizeof path ? PS_LIMIT : ps_report_save(report, path);
    }
    ps_analysis_destroy(ctx);
    ps_report_destroy(report);
    return result == PS_OK && recovered ? PS_RECOVERED : result;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api = {sizeof(ps_analysis_api), PS_ABI_VERSION,
                                        "Energy, period and run comparison", analyze, analyze_many};
    return &api;
}
