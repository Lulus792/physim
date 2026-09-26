#include "physim/data.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Language contact %d: %s\n", __LINE__, #x);                            \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static ps_context context(void) {
    ps_context c = {0};
    c.struct_size = sizeof c;
    c.api_version = PS_API_VERSION;
    c.seed = 42;
    return c;
}
static const ps_experiment_api *load(const char *path, void **module) {
    *module = ps_module_open(path);
    if (!*module)
        return NULL;
    void *symbol = ps_module_symbol(*module, "ps_get_experiment");
    ps_experiment_entry entry = NULL;
    memcpy(&entry, &symbol, sizeof entry);
    return entry ? entry() : NULL;
}
static int near(double a, double b) {
    return isfinite(a) && isfinite(b) && fabs(a - b) <= 1e-10 * fmax(1, fabs(b));
}
static int near3(ps_vec3 a, ps_vec3 b) {
    return near(a.x, b.x) && near(a.y, b.y) && near(a.z, b.z);
}
static int values(const double *a, const double *b) {
    for (unsigned i = 0; i < 11; i++) {
        if (!near(a[i], b[i]))
            fprintf(stderr, "channel %u: %.17g vs %.17g\n", i, a[i], b[i]);
        CHECK(near(a[i], b[i]));
    }
    return 0;
}
static int modules(const char *path, const char *reference_path, const char *fault_path) {
    void *module, *refmodule;
    const ps_experiment_api *api = load(path, &module), *ref = load(reference_path, &refmodule);
    CHECK(api && ref);
    ps_context a = context(), b = context(), independent = context();
    CHECK(api->create(&a) == PS_OK && ref->create(&b) == PS_OK &&
          api->create(&independent) == PS_OK);
    CHECK(a.channel_count == 11 && b.channel_count == 11);
    for (unsigned i = 0; i < 11; i++) {
        CHECK(!strcmp(a.channels[i].name, b.channels[i].name));
        CHECK(!memcmp(a.channels[i].dimension, b.channels[i].dimension, 7));
    }
    double initial[11];
    memcpy(initial, a.values, sizeof initial);
    unsigned impacts = 0, multiple = 0;
    for (unsigned repeat = 0; repeat < 2; repeat++) {
        for (unsigned step = 0; step <= 2000; step++) {
            CHECK(values(a.values, b.values) == 0);
            impacts += a.values[8] > 0;
            multiple += a.values[7] >= 3;
            if (!(step % 100)) {
                ps_scene scene = {0}, reference_scene = {0};
                api->build_scene(&a, &scene);
                ref->build_scene(&b, &reference_scene);
                CHECK(!a.error[0] && ps_scene_valid(&scene) &&
                      scene.count == reference_scene.count);
                for (uint32_t i = 0; i < scene.count; i++) {
                    const ps_object *x = &scene.objects[i], *y = &reference_scene.objects[i];
                    if (x->shape != y->shape || x->color != y->color || !near3(x->a, y->a))
                        fprintf(stderr,
                                "scene step %u object %u: shape %u/%u color %u/%u position "
                                "%.17g,%.17g,%.17g / %.17g,%.17g,%.17g\n",
                                step, i, x->shape, y->shape, x->color, y->color, x->a.x, x->a.y,
                                x->a.z, y->a.x, y->a.y, y->a.z);
                    CHECK(x->shape == y->shape && x->color == y->color && near3(x->a, y->a));
                    CHECK(near3(x->b, y->b) && near(x->radius, y->radius));
                    if (x->shape == PS_BOX) {
                        CHECK(near(x->orientation.x, y->orientation.x) &&
                              near(x->orientation.y, y->orientation.y));
                        CHECK(near(x->orientation.z, y->orientation.z) &&
                              near(x->orientation.w, y->orientation.w));
                    }
                }
            }
            double dt = repeat ? .001 : .002;
            CHECK(api->step(&a, dt) == PS_OK && ref->step(&b, dt) == PS_OK);
            a.time_s += dt;
            b.time_s += dt;
        }
        CHECK(!memcmp(initial, independent.values, sizeof initial));
        CHECK(api->reset(&a) == PS_OK && ref->reset(&b) == PS_OK &&
              !memcmp(initial, a.values, sizeof initial));
    }
    CHECK(impacts > 100 && multiple > 100);
    api->destroy(&a);
    api->destroy(&independent);
    ref->destroy(&b);
    ps_module_close(module);
    ps_module_close(refmodule);
    api = load(fault_path, &module);
    CHECK(api);
    a = context();
    CHECK(api->create(&a) == PS_OK);
    const char *errors[] = {"iterations",    "configuration", "Sphere-plane contact",
                            "Contact index", "Contact index", "Contact impulse index",
                            "Box contact",   "Sphere contact", "exactly one contact",
                            "Single-contact response failed"};
    for (unsigned repeat = 0; repeat < 2; repeat++)
        for (unsigned mode = 1; mode <= 10; mode++) {
            CHECK(api->step(&a, (double)mode) == PS_NUMERIC);
            CHECK(strstr(a.error, "contact_error.phys:") && strstr(a.error, errors[mode - 1]));
            CHECK(api->step(&a, .002) == PS_NUMERIC && api->reset(&a) == PS_OK && !a.error[0]);
        }
    api->destroy(&a);
    ps_module_close(module);
    return 0;
}
static int run_file(const char *runner, const char *path, const char *reference_path,
                    const char *work) {
    char file[4096];
    CHECK(snprintf(file, sizeof file, "%s/contact-%.0f.psrun", work, ps_clock() * 1e6) <
          (int)sizeof file);
    const char *args[] = {runner, path, file, "--steps", "800", "--dt", "0.002", NULL};
    ps_process child = {0};
    CHECK(ps_process_start(&child, args, work));
    double deadline = ps_clock() + 30;
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
    CHECK(ps_run_open(&reader, file) == PS_OK && reader.channels == 11);
    CHECK(strstr(reader.metadata, "model=oriented box-plane contacts") &&
          strstr(reader.metadata, "language=physim"));
    void *module;
    const ps_experiment_api *ref = load(reference_path, &module);
    CHECK(ref);
    ps_context c = context();
    CHECK(ref->create(&c) == PS_OK);
    unsigned impacts = 0;
    double time, sample[PS_MAX_CHANNELS];
    for (unsigned i = 0; i <= 800; i++) {
        CHECK(ps_run_next(&reader, &time, sample) == PS_OK && near(time, i * .002));
        CHECK(values(sample, c.values) == 0);
        impacts += sample[8] > 0;
        CHECK(ref->step(&c, .002) == PS_OK);
        c.time_s = (i + 1) * .002;
    }
    CHECK(impacts > 100 && ps_run_next(&reader, &time, sample) == PS_EOF);
    ref->destroy(&c);
    ps_module_close(module);
    ps_run_reader_close(&reader);
    return 0;
}
int main(int argc, char **argv) {
    CHECK(argc == 6);
    CHECK(modules(argv[1], argv[2], argv[3]) == 0);
    CHECK(run_file(argv[4], argv[1], argv[2], argv[5]) == 0);
    puts("Language contacts: C template parity, friction/impact, manifolds, scene, reset, "
         "diagnostics and saved runs passed");
    return 0;
}
