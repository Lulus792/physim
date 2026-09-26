#include "physim/data.h"
#include "physim/report.h"
#include "platform.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Spring reference line %d: %s\n", __LINE__, #x);                       \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static int run(const char *const *args) {
    ps_process p = {0};
    if (!ps_process_start(&p, args, NULL))
        return -1;
    double until = ps_clock() + 15;
    char output[4096];
    while (ps_process_poll(&p) && ps_clock() < until) {
        while (ps_process_read(&p, output, sizeof output) > 0) {
        }
        ps_sleep(1);
    }
    int code = p.running ? -1 : p.exit_code;
    ps_process_close(&p);
    return code;
}
/* Closed solutions of m*x'' + c*x' + k*x = 0, m=1, k=16, x(0)=.35, v(0)=0.
 * Independent of the experiment's force helper, RK stages and work quadrature. */
static void exact(double c, double t, double *x, double *v) {
    double alpha = c / 2, decay = exp(-alpha * t);
    if (c < 8) {
        double w = sqrt(16 - alpha * alpha);
        *x = .35 * decay * (cos(w * t) + alpha / w * sin(w * t));
        *v = -.35 * decay * 16 / w * sin(w * t);
    } else if (c == 8) {
        *x = .35 * (1 + 4 * t) * decay;
        *v = -.35 * 16 * t * decay;
    } else {
        double root = sqrt(alpha * alpha - 16), r1 = -alpha + root, r2 = -alpha - root;
        double a = -.35 * r2 / (r1 - r2), b = .35 - a;
        *x = a * exp(r1 * t) + b * exp(r2 * t);
        *v = r1 * a * exp(r1 * t) + r2 * b * exp(r2 * t);
    }
}
int main(int argc, char **argv) {
    if (argc != 8)
        return 2;
    /* A rejected RK stage must not leak a partially advanced state or samples. */
    void *module = ps_module_open(argv[2]);
    CHECK(module != NULL);
    void *symbol = ps_module_symbol(module, "ps_get_experiment");
    ps_experiment_entry entry = NULL;
    memcpy(&entry, &symbol, sizeof entry);
    CHECK(entry != NULL);
    const ps_experiment_api *api = entry();
    ps_context context = {0};
    context.struct_size = sizeof context;
    context.api_version = PS_API_VERSION;
    CHECK(api && api->create(&context) == PS_OK);
    double initial[PS_MAX_CHANNELS], advanced[PS_MAX_CHANNELS];
    memcpy(initial, context.values, sizeof initial);
    CHECK(api->step(&context, -1) == PS_INVALID &&
          !memcmp(initial, context.values, sizeof initial));
    CHECK(api->step(&context, 1) == PS_INVALID && !memcmp(initial, context.values, sizeof initial));
    CHECK(api->step(&context, .005) == PS_OK);
    memcpy(advanced, context.values, sizeof advanced);
    CHECK(api->reset(&context) == PS_OK && !memcmp(initial, context.values, sizeof initial));
    CHECK(api->step(&context, .005) == PS_OK && !memcmp(advanced, context.values, sizeof advanced));
    api->destroy(&context);
    ps_module_close(module);
    const double damping[] = {1.2, 0, 8, 12};
    const char *paths[] = {"spring-under.psrun", "spring-undamped.psrun", "spring-critical.psrun",
                           "spring-over.psrun"};
    const char *names[] = {"position.x",     "velocity.x",      "energy.kinetic",
                           "energy.spring",  "energy",          "energy.dissipated",
                           "energy.balance", "force.spring.x",  "force.damper.x",
                           "force.total.x",  "power.dissipated"};
    for (unsigned mode = 0; mode < 4; mode++) {
        remove(paths[mode]);
        const char *args[] = {argv[1], argv[mode + 2], paths[mode], "--steps",
                              "2000",  "--dt",         ".005",      NULL};
        CHECK(run(args) == 0);
        ps_run_reader reader;
        CHECK(ps_run_open(&reader, paths[mode]) == PS_OK && reader.channels == 11);
        CHECK(strstr(reader.metadata, "model=horizontal spring-mass-damper") &&
              strstr(reader.metadata, "integrator=RK4") &&
              strstr(reader.metadata, "damping_ns_m=") &&
              strstr(reader.metadata, "initial_extension_m=") &&
              strstr(reader.metadata, "gravity=none"));
        for (unsigned i = 0; i < 11; i++)
            CHECK(!strcmp(reader.schema[i].name, names[i]));
        double t, y[PS_MAX_CHANNELS], previous_energy = .98, previous_work = 0;
        unsigned count = 0;
        ps_result r;
        while ((r = ps_run_next(&reader, &t, y)) == PS_OK) {
            double x, v;
            exact(damping[mode], t, &x, &v);
            double energy = .5 * v * v + 8 * x * x;
            CHECK(fabs(t - count * .005) < 1e-12);
            CHECK(fabs(y[0] - x) < 3e-8 && fabs(y[1] - v) < 1e-7);
            CHECK(fabs(y[2] - .5 * v * v) < 1e-7 && fabs(y[3] - 8 * x * x) < 1e-7);
            CHECK(fabs(y[4] - energy) < 1e-7 && fabs(y[5] - (.98 - energy)) < 1e-7);
            CHECK(fabs(y[6] - .98) < 1e-7 && fabs(y[6] - y[4] - y[5]) < 1e-14);
            CHECK(y[4] <= previous_energy + 1e-10 && y[5] >= previous_work - 1e-13 && y[10] >= 0);
            CHECK(fabs(y[7] + 16 * y[0]) < 1e-12 && fabs(y[8] + damping[mode] * y[1]) < 1e-12 &&
                  fabs(y[9] - y[7] - y[8]) < 1e-12 &&
                  fabs(y[10] - damping[mode] * y[1] * y[1]) < 1e-12);
            previous_energy = y[4];
            previous_work = y[5];
            count++;
        }
        CHECK(r == PS_EOF && count == 2001);
        ps_run_reader_close(&reader);
    }
    double error[2];
    for (unsigned refinement = 0; refinement < 2; refinement++) {
        const char *path = "spring-convergence.psrun";
        remove(path);
        const char *args[] = {argv[1],
                              argv[2],
                              path,
                              "--steps",
                              refinement ? "40" : "20",
                              "--dt",
                              refinement ? ".025" : ".05",
                              NULL};
        CHECK(run(args) == 0);
        ps_run_reader reader;
        CHECK(ps_run_open(&reader, path) == PS_OK);
        double t = 0, y[PS_MAX_CHANNELS], x, v;
        ps_result r;
        while ((r = ps_run_next(&reader, &t, y)) == PS_OK) {
        }
        CHECK(r == PS_EOF && fabs(t - 1) < 1e-14);
        exact(1.2, 1, &x, &v);
        error[refinement] = hypot(y[0] - x, (y[1] - v) / 4);
        ps_run_reader_close(&reader);
        remove(path);
    }
    CHECK(error[1] < 1e-5 && error[0] / error[1] > 14 && error[0] / error[1] < 18);
    const char *outputs[] = {"spring-report-summary.csv", "spring-report-manifest.txt",
                             "spring-report-plot.svg",    "spring-report-derived.csv",
                             "spring-report.psreport",    "spring-report.inputs.csv"};
    for (unsigned i = 0; i < sizeof outputs / sizeof outputs[0]; i++)
        remove(outputs[i]);
    const char *args[] = {argv[6], argv[7], paths[0], "spring-report", NULL};
    CHECK(run(args) == 0);
    ps_report *report = NULL;
    uint32_t plots, tables;
    CHECK(ps_report_load("spring-report.psreport", &report) == PS_OK);
    CHECK(ps_report_describe(report, NULL, NULL, &plots, &tables) == PS_OK && plots == 4 &&
          tables == 2);
    ps_plot_info plot;
    CHECK(ps_report_plot_read(report, 3, &plot) == PS_OK && plot.curves == 3 &&
          !strcmp(plot.y_unit.symbol, "J"));
    ps_curve_data *curve = malloc(sizeof *curve);
    CHECK(curve != NULL);
    for (unsigned j = 0; j < 3; j++) {
        CHECK(ps_report_curve_read(report, 3, j, curve) == PS_OK && curve->count == 2001);
        for (unsigned i = 0; i < curve->count; i++) {
            double x, v;
            exact(1.2, curve->x[i], &x, &v);
            double energy = .5 * v * v + 8 * x * x;
            CHECK(fabs(curve->y[i] - (j == 0 ? energy : j == 1 ? .98 - energy : .98)) < 1e-7);
        }
    }
    free(curve);
    ps_table_row row;
    ps_table_info table;
    CHECK(ps_report_table_read(report, 1, &table) == PS_OK &&
          !strcmp(table.column[0].label, "Max. Bilanzabweichung"));
    CHECK(ps_report_row_read(report, 1, 0, &row) == PS_OK && row.values[0] < 1e-7);
    ps_report_destroy(report);
    FILE *f = fopen("spring-report-manifest.txt", "rb");
    CHECK(f != NULL);
    char manifest[8192] = {0};
    size_t n = fread(manifest, 1, sizeof manifest - 1, f);
    CHECK(!ferror(f) && !fclose(f) && n > 0);
    CHECK(strstr(manifest, "energy_metric_channel=energy.balance"));
    double drift = 0;
    const char *metric = strstr(manifest, "max_energy_drift_J=");
    CHECK(metric && sscanf(metric, "max_energy_drift_J=%lf", &drift) == 1 && drift < 1e-7);
    printf("Spring: four damping regimes, 8004 samples, energy/work, RK4 ratio %.4g and balance "
           "report passed\n",
           error[0] / error[1]);
    return 0;
}
