#include "physim/data.h"
#include "physim/report.h"
#include "platform.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Floating line %d: %s\n", __LINE__, #x);                               \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static int run(const char *const *args) {
    ps_process p = {0};
    if (!ps_process_start(&p, args, NULL))
        return -1;
    double until = ps_clock() + 20;
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
/* Independent polynomial acceleration in the partially submerged domain. */
static double acceleration(double y, double v) {
    double fraction = .5 - 7.5 * y + 250 * y * y * y;
    return 9.81 * (2 * fraction - 1) - 1.2 * fraction * v / (2 * PS_PI / 3);
}
static void reference_step(double *y, double *v, double h) {
    double k1 = acceleration(*y, *v);
    double v2 = *v + h * k1 / 2, k2 = acceleration(*y + h * *v / 2, v2);
    double v3 = *v + h * k2 / 2, k3 = acceleration(*y + h * v2 / 2, v3);
    double v4 = *v + h * k3, k4 = acceleration(*y + h * v3, v4);
    *y += h * (*v + 2 * v2 + 2 * v3 + v4) / 6;
    *v += h * (k1 + 2 * k2 + 2 * k3 + k4) / 6;
}
int main(int argc, char **argv) {
    if (argc != 9)
        return 2;
    void *module = ps_module_open(argv[2]);
    CHECK(module);
    void *symbol = ps_module_symbol(module, "ps_get_experiment");
    ps_experiment_entry entry = NULL;
    memcpy(&entry, &symbol, sizeof entry);
    CHECK(entry);
    const ps_experiment_api *api = entry();
    ps_context c = {0};
    c.struct_size = sizeof c;
    c.api_version = PS_API_VERSION;
    CHECK(api->create(&c) == PS_OK && c.channel_count == 12);
    double initial[PS_MAX_CHANNELS], advanced[PS_MAX_CHANNELS];
    memcpy(initial, c.values, sizeof initial);
    CHECK(api->step(&c, -1) == PS_INVALID && !memcmp(initial, c.values, sizeof initial));
    CHECK(api->step(&c, .005) == PS_OK);
    memcpy(advanced, c.values, sizeof advanced);
    CHECK(api->reset(&c) == PS_OK && !memcmp(initial, c.values, sizeof initial));
    CHECK(api->step(&c, .005) == PS_OK && !memcmp(advanced, c.values, sizeof advanced));
    ps_scene scene = {0};
    api->build_scene(&c, &scene);
    CHECK(ps_scene_valid(&scene) && scene.count == 26);
    unsigned labels = 0;
    for (unsigned i = 0; i < scene.count; i++)
        labels += scene.objects[i].shape == PS_LABEL;
    CHECK(labels == 3);
    api->destroy(&c);
    ps_module_close(module);
    char root[256], paths[5][320];
    snprintf(root, sizeof root, "floating-%llu", (unsigned long long)(ps_clock() * 1e9));
    CHECK(ps_make_directory(root));
    const double density[] = {500, 500, 1000, 1500, 500};
    const double height[] = {.04, 0, -.5, -.5, -.5};
    const double volume = 4 * PS_PI * .001 / 3;
    double coarse_error = 0;
    const char *names[] = {"position.y",         "velocity.y",       "submerged.volume",
                           "submerged.fraction", "force.buoyancy.y", "force.weight.y",
                           "force.damping.y",    "force.total.y",    "energy",
                           "energy.dissipated",  "energy.balance",   "buoyancy.offset.y"};
    for (unsigned mode = 0; mode < 5; mode++) {
        snprintf(paths[mode], sizeof paths[mode], "%s/run-%u.psrun", root, mode);
        const char *args[] = {argv[1], argv[mode + 2], paths[mode], "--steps", mode ? "20" : "2000",
                              "--dt",  ".005",         "--seed",    "42",      NULL};
        CHECK(run(args) == 0);
        ps_run_reader reader;
        CHECK(ps_run_open(&reader, paths[mode]) == PS_OK && reader.channels == 12);
        CHECK(strstr(reader.metadata, "model=hydrostatic floating sphere") &&
              strstr(reader.metadata, "damping_model=-c*immersed_volume_fraction*v") &&
              strstr(reader.metadata, "excluded=waves"));
        for (unsigned j = 0; j < 12; j++)
            CHECK(!strcmp(reader.schema[j].name, names[j]));
        double t, values[PS_MAX_CHANNELS], initial_energy = 0, previous_work = 0;
        double ry = .04, rv = 0;
        unsigned count = 0;
        ps_result r;
        while ((r = ps_run_next(&reader, &t, values)) == PS_OK) {
            CHECK(fabs(t - count * .005) < 1e-11);
            double y = values[0], v = values[1], weight = -density[mode] * volume * 9.81;
            double fraction = mode >= 2 ? 1 : .5 - 7.5 * y + 250 * y * y * y;
            CHECK(values[3] >= 0 && values[3] <= 1 && fabs(values[3] - fraction) < 1e-13);
            CHECK(fabs(values[2] - fraction * volume) < 1e-14);
            CHECK(fabs(values[4] - 1000 * volume * fraction * 9.81) < 1e-11);
            CHECK(fabs(values[5] - weight) < 1e-11);
            CHECK(fabs(values[6] - (mode ? 0 : -1.2 * fraction * v)) < 1e-12);
            CHECK(fabs(values[7] - values[4] - values[5] - values[6]) < 1e-11);
            if (!mode) {
                /* At dt=.005: sub-micrometre position and 10 micrometres/s
                 * velocity budgets. Refinement below checks fourth-order convergence. */
                CHECK(fabs(y) < .1 && fabs(y - ry) < 1e-6 && fabs(v - rv) < 1e-5);
                coarse_error = fmax(coarse_error, fabs(v - rv));
                for (unsigned sub = 0; sub < 20; sub++)
                    reference_step(&ry, &rv, .00025);
                double potential = 1000 * 9.81 * PS_PI * (.005 * y * y - y * y * y * y / 12);
                CHECK(fabs(values[8] - .5 * density[mode] * volume * v * v - potential) < 1e-12);
            } else {
                double acc = mode == 1 ? 0 : 9.81 * (1000 / density[mode] - 1);
                CHECK(fabs(y - height[mode] - .5 * acc * t * t) < 1e-12);
                CHECK(fabs(v - acc * t) < 1e-12);
            }
            if (!count)
                initial_energy = values[10];
            CHECK(fabs(values[10] - initial_energy) < 2e-7);
            CHECK(fabs(values[10] - values[8] - values[9]) < 1e-12);
            CHECK(values[9] >= previous_work - 1e-13);
            previous_work = values[9];
            count++;
        }
        CHECK(r == PS_EOF && count == (mode ? 21u : 2001u));
        ps_run_reader_close(&reader);
    }
    char refined[320];
    snprintf(refined, sizeof refined, "%s/refined.psrun", root);
    const char *refine_args[] = {argv[1], argv[2], refined, "--steps",
                                 "4000",  "--dt",  ".0025", NULL};
    CHECK(run(refine_args) == 0);
    ps_run_reader fine_reader;
    CHECK(ps_run_open(&fine_reader, refined) == PS_OK);
    double fine_t, fine_values[PS_MAX_CHANNELS], ry = .04, rv = 0, fine_error = 0;
    unsigned fine_count = 0;
    ps_result fine_result;
    while ((fine_result = ps_run_next(&fine_reader, &fine_t, fine_values)) == PS_OK) {
        fine_error = fmax(fine_error, fabs(fine_values[1] - rv));
        for (unsigned sub = 0; sub < 20; sub++)
            reference_step(&ry, &rv, .000125);
        fine_count++;
    }
    CHECK(fine_result == PS_EOF && fine_count == 4001);
    ps_run_reader_close(&fine_reader);
    CHECK(fine_error > 0 && coarse_error / fine_error > 14 && coarse_error / fine_error < 18);
    printf("Motion max velocity error %.9g -> %.9g m/s; refinement ratio %.6g\n", coarse_error,
           fine_error, coarse_error / fine_error);
    char prefix[320], report_path[360];
    snprintf(prefix, sizeof prefix, "%s/report", root);
    const char *analysis[] = {argv[7], argv[8], paths[0], prefix, NULL};
    CHECK(run(analysis) == 0);
    snprintf(report_path, sizeof report_path, "%s.psreport", prefix);
    ps_report *report = NULL;
    CHECK(ps_report_load(report_path, &report) == PS_OK);
    uint32_t plots, tables;
    CHECK(ps_report_describe(report, NULL, NULL, &plots, &tables) == PS_OK && plots == 4 &&
          tables == 2);
    ps_table_row metrics;
    CHECK(ps_report_row_read(report, 1, 0, &metrics) == PS_OK && metrics.values[0] < 2e-7);
    ps_curve_data *curve = malloc(sizeof *curve);
    CHECK(curve);
    CHECK(ps_report_curve_read(report, 3, 2, curve) == PS_OK && curve->count == 2001);
    for (unsigned i = 0; i < curve->count; i++)
        CHECK(fabs(curve->y[i] - curve->y[0]) < 2e-7);
    free(curve);
    ps_report_destroy(report);
    puts("Floating sphere: scene, reset, 6086 samples, independent motion reference, energy and "
         "analysis passed");
    return 0;
}
