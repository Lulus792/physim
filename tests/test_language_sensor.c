#include "physim/data.h"
#include "physim/measurement.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Language sensor %d: %s\n", __LINE__, #x);                             \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static ps_context context(uint64_t seed) {
    ps_context c = {0};
    c.struct_size = sizeof c;
    c.api_version = PS_API_VERSION;
    c.seed = seed;
    return c;
}
static const ps_experiment_api *load(const char *path, void **module) {
    *module = ps_module_open(path);
    if (!*module)
        return NULL;
    void *address = ps_module_symbol(*module, "ps_get_experiment");
    ps_experiment_entry entry = NULL;
    memcpy(&entry, &address, sizeof entry);
    return entry ? entry() : NULL;
}
typedef struct {
    ps_sensor sensors[2];
    ps_rng rng;
    double velocity, time;
} reference;
static int initialize(reference *r, uint64_t seed) {
    memset(r, 0, sizeof *r);
    ps_sensor_config config = {PS_METRE, 100,  0,   .005, .01, .002, {PS_DIST_NORMAL, 0, .02},
                               .2,       .003, .001};
    CHECK(ps_sensor_init(&r->sensors[0], &config, seed ^ UINT64_C(17)) == PS_OK);
    CHECK(ps_sensor_init(&r->sensors[1], &config, seed ^ UINT64_C(29)) == PS_OK);
    ps_rng_seed(&r->rng, seed);
    CHECK(ps_distribution_sample((ps_distribution){PS_DIST_NORMAL, 3, .15}, &r->rng,
                                 &r->velocity) == PS_OK);
    return 0;
}
static int near(double a, double b) { return isfinite(a) && isfinite(b) && fabs(a - b) < 1e-12; }
static int compare(reference *r, const double *values, unsigned states[3]) {
    double truth[] = {-2 + r->velocity * r->time, 5 * r->time - .5 * 9.80665 * r->time * r->time};
    for (unsigned axis = 0; axis < 2; axis++) {
        ps_measurement sample;
        CHECK(ps_sensor_read(&r->sensors[axis], r->time, (ps_quantity){truth[axis], PS_METRE},
                             &sample) == PS_OK);
        CHECK(near(values[axis], truth[axis]));
        CHECK(near(values[axis + 2], sample.value.value));
        CHECK(values[axis + 4] == (double)sample.state);
        CHECK(near(values[axis + 6], sample.standard_uncertainty));
        states[sample.state]++;
        if (!axis) {
            CHECK(values[9] == (double)sample.skipped && values[10] == (double)sample.index);
            CHECK(values[8] == sample.time_s);
        }
    }
    CHECK(values[11] == r->velocity);
    CHECK(near(values[12], -2 + 3 * r->time) && near(values[13], truth[1]));
    CHECK(fabs(values[14] - (.5 * r->velocity * r->velocity + 12.5)) < 1e-11);
    return 0;
}
static int modules(const char *path, const char *fault_path) {
    void *module;
    const ps_experiment_api *api = load(path, &module);
    CHECK(api);
    const uint64_t seeds[] = {0, 42, UINT64_MAX, UINT64_C(0x8000000000000000)};
    unsigned states[3] = {0};
    for (unsigned seed = 0; seed < 4; seed++) {
        ps_context a = context(seeds[seed]), b = context(seeds[seed]);
        reference r;
        CHECK(initialize(&r, a.seed) == 0);
        CHECK(api->create(&a) == PS_OK && api->create(&b) == PS_OK);
        CHECK(a.channel_count == 15 && !strcmp(a.channels[4].name, "sensor.x.status"));
        double initial[15];
        memcpy(initial, a.values, sizeof initial);
        for (unsigned step = 0; step <= 1000; step++) {
            CHECK(compare(&r, a.values, states) == 0);
            CHECK(a.rng.state == r.rng.state && a.rng.increment == r.rng.increment);
            if (!(step % 50)) {
                ps_scene scene;
                api->build_scene(&a, &scene);
                CHECK(!a.error[0] && ps_scene_valid(&scene) && scene.count >= 1);
            }
            double dt = step == 600 ? .035 : .005;
            CHECK(api->step(&a, dt) == PS_OK);
            r.time += dt;
        }
        CHECK(!memcmp(initial, b.values, sizeof initial));
        CHECK(api->reset(&a) == PS_OK && !memcmp(initial, a.values, sizeof initial));
        a.seed ^= UINT64_MAX;
        CHECK(api->reset(&a) == PS_OK && initialize(&r, a.seed) == 0);
        CHECK(compare(&r, a.values, states) == 0);
        api->destroy(&a);
        api->destroy(&b);
    }
    CHECK(states[0] > 0 && states[1] > 0 && states[2] > 0);
    ps_module_close(module);
    api = load(fault_path, &module);
    CHECK(api);
    ps_context c = context(42);
    CHECK(api->create(&c) == PS_OK);
    for (unsigned repeat = 0; repeat < 2; repeat++)
        for (unsigned mode = 1; mode <= 4; mode++) {
            CHECK(api->step(&c, (double)mode) == PS_NUMERIC);
            CHECK(strstr(c.error, mode == 4 ? "Cannot unwrap nil optional" : "Sensor read failed") &&
                  strstr(c.error, "sensor_error.phys:"));
            CHECK(api->step(&c, .1) == PS_NUMERIC);
            CHECK(api->reset(&c) == PS_OK && !c.error[0]);
        }
    api->destroy(&c);
    ps_module_close(module);
    return 0;
}
static int run_file(const char *runner, const char *module, const char *work) {
    char path[4096];
    CHECK(snprintf(path, sizeof path, "%s/sensors-%.0f.psrun", work, ps_clock() * 1e6) <
          (int)sizeof path);
    const char *args[] = {
        runner, module, path, "--steps", "200", "--dt", "0.005", "--seed", "18446744073709551615",
        NULL};
    ps_process child = {0};
    CHECK(ps_process_start(&child, args, work));
    double deadline = ps_clock() + 20;
    char output[4096];
    while (ps_process_poll(&child) && ps_clock() < deadline) {
        (void)ps_process_read(&child, output, sizeof output);
        ps_sleep(1);
    }
    if (child.running)
        ps_process_kill(&child);
    int code = child.exit_code;
    ps_process_close(&child);
    CHECK(code == 0);
    ps_run_reader reader;
    CHECK(ps_run_open(&reader, path) == PS_OK && reader.channels == 15);
    CHECK(strstr(reader.metadata, "language=physim-0.179.0\ncompiler=physimc-0.1.0-dev") &&
          strstr(reader.metadata, "sensor_seed_x=run seed XOR 17"));
    reference r;
    CHECK(initialize(&r, UINT64_MAX) == 0);
    unsigned states[3] = {0};
    for (unsigned step = 0; step <= 200; step++) {
        double time, values[PS_MAX_CHANNELS];
        CHECK(ps_run_next(&reader, &time, values) == PS_OK && near(time, r.time));
        CHECK(compare(&r, values, states) == 0);
        r.time += .005;
    }
    double time, values[PS_MAX_CHANNELS];
    CHECK(ps_run_next(&reader, &time, values) == PS_EOF);
    CHECK(states[0] && states[1] && states[2]);
    ps_run_reader_close(&reader);
    return 0;
}
int main(int argc, char **argv) {
    CHECK(argc == 5);
    CHECK(modules(argv[1], argv[2]) == 0);
    CHECK(run_file(argv[3], argv[1], argv[4]) == 0);
    puts(
        "Language sensors: C parity, copies, 64-bit seeds, recovery and saved measurements passed");
    return 0;
}
