#include "physim/analysis.h"
#include "analysis_numeric.h"
#include <float.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
void ps_statistics_push(ps_statistics *s, double x) {
    if (!s || !isfinite(x))
        return;
    if (!s->count)
        s->min = s->max = x;
    double d = x - s->mean;
    s->count++;
    s->mean += d / (double)s->count;
    s->m2 += d * (x - s->mean);
    if (x < s->min)
        s->min = x;
    if (x > s->max)
        s->max = x;
}
double ps_statistics_stddev(const ps_statistics *s) {
    return s && s->count > 1 ? sqrt(s->m2 / (double)(s->count - 1)) : 0;
}
ps_result ps_derivative(const double *x, const double *y, size_t n, double *out) {
    if (!x || !y || !out || n < 2) return PS_INVALID;
    for (size_t i = 0; i < n; i++)
        if (!isfinite(x[i]) || !isfinite(y[i]) || (i && x[i] <= x[i - 1]))
            return PS_INVALID;
    if (n > SIZE_MAX / sizeof(double)) return PS_LIMIT;
    double *values = malloc(n * sizeof *values);
    if (!values) return PS_MEMORY;
    for (size_t i = 0; i < n; i++) {
        size_t a = i ? i - 1 : 0, b = i + 1 < n ? i + 1 : n - 1;
        values[i] = ps_numeric_secant(x[a], x[b], y[a], y[b]);
        if (!isfinite(values[i])) { free(values); return PS_NUMERIC; }
    }
    memcpy(out, values, n * sizeof *out);
    free(values);
    return PS_OK;
}
double ps_trapezoid(const double *x, const double *y, size_t n) {
    if (!x || !y || n < 2) return NAN;
    for (size_t i = 0; i < n; i++)
        if (!isfinite(x[i]) || !isfinite(y[i]) || (i && x[i] <= x[i - 1])) return NAN;
    double sum = 0;
    for (size_t i = 1; i < n; i++) {
        double area = ps_numeric_trapezoid(x[i-1], x[i], y[i-1], y[i]);
        sum += area;
        if (!isfinite(area) || !isfinite(sum)) return NAN;
    }
    return sum;
}
/* Fraction on a finite axis without overflowing its span. Constants sit in
 * the middle; labels retain their exact finite value rather than a fake span. */
static double axis_fraction(double value, double lo, double hi) {
    if (lo == hi) return .5;
    double scale = fmax(fabs(lo), fabs(hi));
    if (scale > 1) return (value / scale - lo / scale) / (hi / scale - lo / scale);
    return (value - lo) / (hi - lo);
}
static void xml(FILE *f, const char *s) {
    for (; *s; s++) {
        if (*s == '&')
            fputs("&amp;", f);
        else if (*s == '<')
            fputs("&lt;", f);
        else if (*s == '>')
            fputs("&gt;", f);
        else if (*s == '\"')
            fputs("&quot;", f);
        else
            fputc(*s, f);
    }
}
static void csv_label(FILE *f, const char *s) {
    fputc('"', f);
    for (; *s; s++) {
        if (*s == '"')
            fputc('"', f);
        fputc(*s, f);
    }
    fputc('"', f);
}
ps_result ps_analyze_run(const char *input, const char *prefix) {
    ps_run_reader r;
    ps_result result = ps_run_open(&r, input);
    if (result != PS_OK)
        return result;
    ps_statistics stats[PS_MAX_CHANNELS] = {{0}};
    int status_channels[PS_MAX_CHANNELS];
    for (uint32_t i = 0; i < r.channels; i++) {
        result = ps_channel_status_index(r.schema, r.channels, i, &status_channels[i]);
        if (result != PS_OK) {
            ps_run_reader_close(&r);
            return result;
        }
    }
    double t, v[PS_MAX_CHANNELS], initial_energy = 0, max_drift = 0, previous_t = 0,
                                  previous_angle = 0, last_cross = 0, period_sum = 0;
    unsigned periods = 0;
    bool had_cross = false, had_angle = false, had_energy = false;
    int energy = -1, angle = -1, balance = -1;
    for (uint32_t i = 0; i < r.channels; i++) {
        if (!strcmp(r.schema[i].name, "energy"))
            energy = (int)i;
        if (!strcmp(r.schema[i].name, "energy.balance"))
            balance = (int)i;
        if (!strcmp(r.schema[i].name, "angle"))
            angle = (int)i;
    }
    if (balance >= 0)
        energy = balance;
    while ((result = ps_run_next(&r, &t, v)) == PS_OK) {
        for (uint32_t i = 0; i < r.channels; i++) {
            int status = status_channels[i];
            if (status >= 0 && v[status] != 0 && v[status] != 1 && v[status] != 2) {
                ps_run_reader_close(&r);
                return PS_CORRUPT;
            }
            if (status < 0 || v[status] == 1) {
                ps_statistics_push(&stats[i], v[i]);
                if (!isfinite(stats[i].mean) || !isfinite(stats[i].m2)) {
                    ps_run_reader_close(&r);
                    return PS_NUMERIC;
                }
            }
        }
        if (energy >= 0 && (status_channels[energy] < 0 || v[status_channels[energy]] == 1)) {
            if (!had_energy) initial_energy = v[energy];
            had_energy = true;
            double drift = fabs(v[energy] - initial_energy);
            if (!isfinite(drift)) { ps_run_reader_close(&r); return PS_NUMERIC; }
            if (drift > max_drift)
                max_drift = drift;
        }
        if (angle >= 0 && (status_channels[angle] < 0 || v[status_channels[angle]] == 1)) {
            if (had_angle && previous_angle < 0 && v[angle] >= 0) {
                double cross =
                    previous_t + (t - previous_t) * axis_fraction(0, previous_angle, v[angle]);
                if (had_cross) {
                    period_sum += cross - last_cross;
                    periods++;
                }
                last_cross = cross;
                had_cross = true;
            }
            previous_angle = v[angle];
            had_angle = true;
        } else {
            had_angle = false;
            had_cross = false; /* Never interpolate or count intervals through a gap. */
        }
        previous_t = t;
    }
    bool recovered = result == PS_RECOVERED;
    uint64_t count = r.samples;
    if ((result != PS_EOF && !recovered) || !count) {
        ps_run_reader_close(&r);
        return count ? result : PS_INVALID;
    }
    char name[4096];
    if (strlen(prefix) > sizeof name - 32) {
        ps_run_reader_close(&r);
        return PS_INVALID;
    }
    snprintf(name, sizeof name, "%s-summary.csv", prefix);
    FILE *table = fopen(name, "wx");
    if (!table) {
        ps_run_reader_close(&r);
        return PS_IO;
    }
    fputs("channel,unit,count,mean,stddev,min,max\n", table);
    for (uint32_t i = 0; i < r.channels; i++) {
        csv_label(table, r.schema[i].name);
        fputc(',', table);
        csv_label(table, r.schema[i].unit);
        if (!stats[i].count)
            fputs(",0,,,,\n", table);
        else if (stats[i].count == 1)
            fprintf(table, ",1,%.17g,,%.17g,%.17g\n", stats[i].mean, stats[i].min, stats[i].max);
        else
            fprintf(table, ",%llu,%.17g,%.17g,%.17g,%.17g\n", (unsigned long long)stats[i].count,
                    stats[i].mean, ps_statistics_stddev(&stats[i]), stats[i].min, stats[i].max);
    }
    bool bad = ferror(table) != 0;
    if (fclose(table))
        bad = true;
    snprintf(name, sizeof name, "%s-manifest.txt", prefix);
    FILE *manifest = fopen(name, "wx");
    if (!manifest) {
        ps_run_reader_close(&r);
        return PS_IO;
    }
    fprintf(manifest,
            "analysis_api=%u\ninput=%s\nsamples=%llu\nrecovered=%d\nmax_energy_drift_J=%."
            "17g\nperiod_s=%.17g\nperiod_intervals=%u\nenergy_metric_channel=%s\n%s",
            PS_API_VERSION, input, (unsigned long long)count, recovered, had_energy ? max_drift : NAN,
            periods ? period_sum / periods : NAN, periods,
            energy >= 0 ? r.schema[energy].name : "none", r.metadata);
    if (ferror(manifest))
        bad = true;
    if (fclose(manifest))
        bad = true;
    printf("Samples: %llu%s\n", (unsigned long long)count, recovered ? " (recovered)" : "");
    if (had_energy)
        printf("Maximum %s deviation: %.9g J\n", balance >= 0 ? "energy balance" : "energy",
               max_drift);
    if (periods)
        printf("Period: %.9g s (%u intervals)\n", period_sum / periods, periods);
    uint32_t index = energy >= 0 ? (uint32_t)energy : 0;
    double lo = stats[index].min, hi = stats[index].max;
    snprintf(name, sizeof name, "%s-plot.svg", prefix);
    FILE *svg = fopen(name, "wx");
    if (!svg) {
        ps_run_reader_close(&r);
        return PS_IO;
    }
    fprintf(svg,
            "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"1100\" height=\"600\" viewBox=\"0 0 "
            "1100 600\"><rect width=\"1100\" height=\"600\" fill=\"#101b28\"/><g fill=\"#e6edf3\" "
            "font-family=\"sans-serif\"><text x=\"70\" y=\"45\" font-size=\"24\">Physim / ");
    xml(svg, r.schema[index].name);
    fprintf(svg, "</text><text x=\"70\" y=\"80\">%.9g ... %.9g ", lo, hi);
    xml(svg, r.schema[index].unit);
    fprintf(svg,
            "</text><text x=\"70\" y=\"565\">0 ... %.5g s / %llu samples</text></g><path d=\"M70 "
            "100 V530 H1040\" fill=\"none\" stroke=\"#63758a\"/><polyline fill=\"none\" "
            "stroke=\"#53dec2\" stroke-width=\"2\" points=\"",
            previous_t, (unsigned long long)count);
    ps_run_reader_close(&r);
    result = ps_run_open(&r, input);
    if (result != PS_OK) {
        fclose(svg);
        return result;
    }
    uint64_t stride = count / 4000 + 1;
    while (ps_run_next(&r, &t, v) == PS_OK) {
        if ((status_channels[index] < 0 || v[status_channels[index]] == 1) &&
            ((r.samples - 1) % stride == 0 || r.samples == count))
            fprintf(svg, "%.3f,%.3f ", 70 + (previous_t > 0 ? t / previous_t : 0) * 970,
                    530 - axis_fraction(v[index], lo, hi) * 410);
    }
    fputs("\"/></svg>\n", svg);
    if (ferror(svg))
        bad = true;
    if (fclose(svg))
        bad = true;
    ps_run_reader_close(&r);
    return bad ? PS_IO : (recovered ? PS_RECOVERED : PS_OK);
}
