#include "physim/data.h"
#include "physim/measurement.h"
#include "platform.h"
#include "protocol.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Language experiment %d: %s\n", __LINE__, #x);                         \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static ps_context context(void) {
    ps_context c = {0};
    c.struct_size = sizeof c;
    c.api_version = PS_API_VERSION;
    c.dt_s = 0.005;
    c.seed = 42;
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
static int near(double a, double b) { return isfinite(a) && isfinite(b) && fabs(a - b) < 5e-11; }
static int spring_scene_parity(const ps_experiment_api *language, ps_context *language_context,
                               const ps_experiment_api *reference, ps_context *reference_context) {
    ps_scene scene = {0}, reference_scene = {0};
    language->build_scene(language_context, &scene);
    reference->build_scene(reference_context, &reference_scene);
    CHECK(ps_scene_valid(&scene) && ps_scene_valid(&reference_scene));
    CHECK(scene.count == 13 && scene.count == reference_scene.count);
    CHECK(scene.point_count == 65 && scene.point_count == reference_scene.point_count);
    for (unsigned object = 0; object < scene.count; object++) {
        const ps_object *a = &scene.objects[object], *b = &reference_scene.objects[object];
        CHECK(a->id == b->id && a->shape == b->shape && a->color == b->color);
        CHECK(!strcmp(a->text, b->text) && near(a->radius, b->radius));
        CHECK(near(a->a.x, b->a.x) && near(a->a.y, b->a.y) && near(a->a.z, b->a.z));
        if (a->shape == PS_BOX || a->shape == PS_LINE || a->shape == PS_ARROW)
            CHECK(near(a->b.x, b->b.x) && near(a->b.y, b->b.y) && near(a->b.z, b->b.z));
        CHECK(a->point_count == b->point_count);
    }
    for (unsigned i = 0; i < scene.point_count; i++)
        CHECK(near(scene.points[i].x, reference_scene.points[i].x) &&
              near(scene.points[i].y, reference_scene.points[i].y) &&
              near(scene.points[i].z, reference_scene.points[i].z));
    return 0;
}
static int pendulum_api(const char *path, const char *reference_path,
                        const char *integrator_name) {
    void *module, *reference_module;
    const ps_experiment_api *api = load(path, &module);
    const ps_experiment_api *reference = load(reference_path, &reference_module);
    CHECK(api && reference);
    ps_context a = context(), b = context();
    CHECK(api->create(&a) == PS_OK && reference->create(&b) == PS_OK);
    CHECK(a.channel_count == 6 && a.channel_count == b.channel_count);
    CHECK(strstr(a.model_metadata, integrator_name));
    for (unsigned step = 0; step <= 4000; step++) {
        for (uint32_t channel = 0; channel < a.channel_count; channel++) {
            CHECK(!strcmp(a.channels[channel].name, b.channels[channel].name));
            CHECK(!memcmp(a.channels[channel].dimension, b.channels[channel].dimension, 7));
            CHECK(near(a.values[channel], b.values[channel]));
        }
        if (step == 4000)
            break;
        CHECK(api->step(&a, 0.005) == PS_OK && reference->step(&b, 0.005) == PS_OK);
        a.time_s = b.time_s = (step + 1) * 0.005;
    }
    ps_scene scene = {0};
    api->build_scene(&a, &scene);
    CHECK(ps_scene_valid(&scene) && scene.count == 9);
    CHECK(scene.objects[0].parent_id==100 && scene.objects[1].parent_id==100 &&
          scene.objects[2].parent_id==101 && scene.objects[3].shape==PS_GROUP &&
          scene.objects[3].id==100 && scene.objects[4].parent_id==100);
    ps_scene reference_scene = {0};
    reference->build_scene(&b, &reference_scene);
    CHECK(ps_scene_valid(&reference_scene) && reference_scene.count >= 3);
    for (unsigned object = 0; object < 3; object++) {
        CHECK(scene.objects[object].shape == reference_scene.objects[object].shape);
        CHECK(scene.objects[object].color == reference_scene.objects[object].color);
        CHECK(scene.objects[object].id == reference_scene.objects[object].id);
    }
    CHECK(scene.objects[2].shape == PS_SPHERE && scene.objects[2].id == 3);
    CHECK(near(scene.objects[2].a.x, a.values[2]) && near(scene.objects[2].a.y, a.values[3]));
    CHECK(api->reset(&a) == PS_OK && reference->reset(&b) == PS_OK);
    for (uint32_t channel = 0; channel < a.channel_count; channel++)
        CHECK(near(a.values[channel], b.values[channel]));
    CHECK(api->step(&a, 0) == PS_INVALID && api->step(&a, NAN) == PS_INVALID);
    api->destroy(&a);
    reference->destroy(&b);
    CHECK(!a.user && !b.user);
    ps_module_close(module);
    ps_module_close(reference_module);
    return 0;
}
static int spring_api(const char *path, const char *reference_path) {
    void *module, *reference_module;
    const ps_experiment_api *api = load(path, &module);
    const ps_experiment_api *reference = load(reference_path, &reference_module);
    CHECK(api && reference);
    ps_context a = context(), b = context();
    CHECK(api->create(&a) == PS_OK && reference->create(&b) == PS_OK);
    CHECK(a.channel_count == 11 && b.channel_count == 11);
    for (unsigned channel = 0; channel < 11; channel++) {
        CHECK(!strcmp(a.channels[channel].name, b.channels[channel].name));
        CHECK(!memcmp(a.channels[channel].dimension, b.channels[channel].dimension, 7));
    }
    for (unsigned step = 0; step <= 2000; step++) {
        for (unsigned channel = 0; channel < 11; channel++)
            CHECK(near(a.values[channel], b.values[channel]));
        CHECK(fabs(a.values[6] - 0.98) < 2e-8);
        double t = step * 0.002, frequency = sqrt(16 - 0.36);
        double expected =
            0.35 * exp(-0.6 * t) * (cos(frequency * t) + 0.6 / frequency * sin(frequency * t));
        CHECK(fabs(a.values[0] - expected) < 2e-9);
        CHECK(api->step(&a, 0.002) == PS_OK && reference->step(&b, 0.002) == PS_OK);
        a.time_s = b.time_s = (step + 1) * 0.002;
    }
    CHECK(!spring_scene_parity(api, &a, reference, &b));
    CHECK(api->reset(&a) == PS_OK && reference->reset(&b) == PS_OK);
    CHECK(near(a.values[0], 0.35) && a.values[5] == 0);
    CHECK(!spring_scene_parity(api, &a, reference, &b));
    api->destroy(&a);
    reference->destroy(&b);
    ps_module_close(module);
    ps_module_close(reference_module);
    return 0;
}
static int shapes_api(const char *path, const char *fault_path, const char *empty_path) {
    void *module;
    const ps_experiment_api *api = load(path, &module);
    CHECK(api);
    ps_context c = context();
    CHECK(api->create(&c) == PS_OK);
    CHECK(api->step(&c, 1.5707963267948966) == PS_OK);
    ps_scene scene = {0};
    api->build_scene(&c, &scene);
    CHECK(!c.error[0] && ps_scene_valid(&scene) && scene.count == 8);
    const uint32_t shapes[] = {PS_BOX, PS_PLANE, PS_ARROW, PS_POINT, PS_LABEL, PS_SPHERE, PS_LINE,
                               PS_POLYLINE};
    for (unsigned i = 0; i < 8; i++)
        CHECK(scene.objects[i].shape == shapes[i] && scene.objects[i].id == i + 1);
    ps_object *box = &scene.objects[0], *plane = &scene.objects[1];
    CHECK(box->b.x == 2 && box->b.y == 1 && box->b.z == 0.5);
    CHECK(near(box->orientation.z, sqrt(0.5)) && near(box->orientation.w, sqrt(0.5)));
    CHECK(plane->a.y == -1 && plane->b.x == 6 && plane->b.z == 4);
    CHECK(plane->orientation.w == 1 && plane->color == UINT32_C(3049576576));
    CHECK(near(scene.objects[2].b.x, 0) && near(scene.objects[2].b.y, 2));
    CHECK(scene.objects[2].radius == 0.02 && scene.objects[3].a.x == -2);
    CHECK(!strcmp(scene.objects[4].text, "Rotation \xc2\xb7 Box"));
    CHECK(scene.point_count == 3 && scene.objects[7].point_first == 0 &&
          scene.objects[7].point_count == 3 && scene.objects[7].radius == 0.015);
    CHECK(scene.points[0].x == -1 && scene.points[0].y == 0 && scene.points[0].z == 1);
    CHECK(scene.points[1].x == 0 && scene.points[1].y == 1 && scene.points[1].z == 1);
    CHECK(scene.points[2].x == 1 && scene.points[2].y == 0 && scene.points[2].z == 1);
    CHECK(api->reset(&c) == PS_OK);
    api->build_scene(&c, &scene);
    CHECK(scene.count == 8 && scene.objects[0].orientation.w == 1);
    api->destroy(&c);
    ps_module_close(module);
    api = load(fault_path, &module);
    CHECK(api);
    c = context();
    CHECK(api->create(&c) == PS_OK);
    const char *errors[] = {"dimensions must be positive",
                            "nonzero quaternion",
                            "Invalid label",
                            "duplicate ID",
                            "UInt32 range",
                            "require scene callback",
                            "require scene callback",
                            "Invalid polyline", "Invalid polyline", "Invalid polyline",
                            "duplicate ID", "UInt32 range", "scene capacity", "scene capacity"};
    for (unsigned mode = 1; mode <= sizeof errors / sizeof errors[0]; mode++) {
        CHECK(api->reset(&c) == PS_OK);
        CHECK(api->step(&c, (double)mode) == (mode == 6 || mode == 7 ? PS_NUMERIC : PS_OK));
        api->build_scene(&c, &scene);
        CHECK(!scene.count && !scene.point_count);
        CHECK(strstr(c.error, errors[mode - 1]) && strstr(c.error, "scene_shapes_error.phys:"));
        CHECK(api->step(&c, 0.01) == PS_NUMERIC);
    }
    CHECK(api->reset(&c) == PS_OK && api->step(&c, 15) == PS_OK);
    api->build_scene(&c, &scene);
    CHECK(!c.error[0] && ps_scene_valid(&scene) && scene.count == 3);
    CHECK(scene.point_count == PS_MAX_SCENE_POINTS && scene.objects[2].point_first == 2 &&
          scene.objects[2].point_count == 94 && scene.objects[2].radius == 0);
    CHECK(scene.points[2].x == 0 && scene.points[3].x == 1);
    for (unsigned i = 0; i < 92; i++)
        CHECK(scene.points[i + 4].x == (double)i && scene.points[i + 4].y == 0);
    CHECK(api->reset(&c) == PS_OK);
    api->build_scene(&c, &scene);
    CHECK(!c.error[0] && scene.count == 1 && ps_scene_valid(&scene));
    api->destroy(&c);
    ps_module_close(module);
    api = load(empty_path, &module);
    CHECK(api);
    c = context();
    CHECK(api->create(&c) == PS_OK);
    for (unsigned attempt = 0; attempt < 3; attempt++) {
        CHECK(api->reset(&c) == PS_OK);
        api->build_scene(&c, &scene);
        CHECK(!scene.count && !scene.point_count && strstr(c.error, "Invalid polyline") &&
              strstr(c.error, "polyline_empty.phys:"));
        CHECK(api->step(&c, 0.01) == PS_NUMERIC);
    }
    api->destroy(&c);
    ps_module_close(module);
    return 0;
}
static int random_reference(ps_context *c, ps_rng *rng) {
    double uniform, normal;
    CHECK(ps_distribution_sample((ps_distribution){PS_DIST_UNIFORM, -2, 3}, rng, &uniform) ==
          PS_OK);
    CHECK(ps_distribution_sample((ps_distribution){PS_DIST_NORMAL, 5, 0.25}, rng, &normal) ==
          PS_OK);
    CHECK(c->values[0] == uniform && near(c->values[1], normal));
    CHECK(c->rng.state == rng->state && c->rng.increment == rng->increment);
    return 0;
}
static int random_api(const char *path, const char *fault_path, const char *create_path) {
    void *module;
    const ps_experiment_api *api = load(path, &module);
    CHECK(api);
    const uint64_t seeds[] = {0, 42, 43, UINT64_MAX};
    for (unsigned seed = 0; seed < 4; seed++) {
        ps_context a = context(), b = context();
        a.seed = b.seed = seeds[seed];
        CHECK(api->create(&a) == PS_OK && api->create(&b) == PS_OK);
        CHECK(a.channel_count == 2 && b.channel_count == 2);
        double first[] = {a.values[0], a.values[1]};
        ps_rng reference;
        ps_rng_seed(&reference, a.seed);
        CHECK(random_reference(&a, &reference) == 0);
        for (unsigned frame = 0; frame < 64; frame++) {
            ps_scene scene = {0};
            ps_rng before = a.rng;
            api->build_scene(&a, &scene);
            CHECK(ps_scene_valid(&scene) && scene.count == 1);
            CHECK(a.rng.state == before.state && a.rng.increment == before.increment);
            CHECK(api->step(&a, 0.01) == PS_OK);
            CHECK(random_reference(&a, &reference) == 0);
        }
        CHECK(b.values[0] == first[0] && b.values[1] == first[1]);
        CHECK(api->reset(&a) == PS_OK);
        CHECK(a.values[0] == first[0] && a.values[1] == first[1]);
        a.seed = seeds[seed] ^ UINT64_C(12345);
        CHECK(api->reset(&a) == PS_OK);
        ps_rng_seed(&reference, a.seed);
        CHECK(random_reference(&a, &reference) == 0);
        api->destroy(&a);
        api->destroy(&b);
    }
    ps_module_close(module);
    api = load(fault_path, &module);
    CHECK(api);
    ps_context bad = context();
    CHECK(api->create(&bad) == PS_OK);
    for (unsigned mode = 1; mode <= 3; mode++) {
        CHECK(api->reset(&bad) == PS_OK);
        ps_rng before = bad.rng;
        if (mode < 3) {
            CHECK(api->step(&bad, (double)mode) == PS_NUMERIC);
            CHECK(strstr(bad.error, "Invalid random distribution"));
        } else {
            CHECK(api->step(&bad, 3) == PS_OK);
            ps_scene scene = {0};
            api->build_scene(&bad, &scene);
            CHECK(!scene.count && strstr(bad.error, "requires reset or step"));
        }
        CHECK(strstr(bad.error, "random_error.phys:"));
        CHECK(bad.rng.state == before.state && bad.rng.increment == before.increment);
        CHECK(api->step(&bad, 0.01) == PS_NUMERIC);
    }
    api->destroy(&bad);
    ps_module_close(module);
    api = load(create_path, &module);
    CHECK(api);
    bad = context();
    CHECK(api->create(&bad) == PS_NUMERIC && !bad.user && !bad.channel_count);
    CHECK(strstr(bad.error, "requires reset or step"));
    api->destroy(&bad);
    ps_module_close(module);
    return 0;
}
static int runner_fault(const char *runner_path, const char *module_path, const char *work,
                        const char *expected, int create_failure) {
    char path[4096];
    CHECK(snprintf(path, sizeof path, "%s/fault-%.0f.psrun", work, ps_clock() * 1e6) <
          (int)sizeof path);
    const char *args[] = {runner_path, module_path, path, "--interactive", NULL};
    ps_process child = {0};
    CHECK(ps_process_start(&child, args, work));
    if (!create_failure) {
        unsigned char command[64], version[4];
        ps_put_u32(version, PS_ABI_VERSION);
        size_t n = ps_wire_encode(command, PS_MSG_HELLO, 0, version, 4);
        CHECK(n && ps_process_write(&child, command, n));
        n = ps_wire_encode(command, PS_MSG_STEP, 1, NULL, 0);
        CHECK(n && ps_process_write(&child, command, n));
    }
    ps_wire_buffer wire = {0};
    int found = 0;
    double deadline = ps_clock() + 20;
    for (;;) {
        int running = ps_process_poll(&child);
        int got = ps_process_read(&child, wire.data + wire.used, sizeof wire.data - wire.used);
        if (got > 0)
            wire.used += (size_t)got;
        uint32_t type, size;
        const unsigned char *payload;
        int status;
        while ((status = ps_wire_peek(&wire, &type, &payload, &size)) > 0) {
            if (type == PS_MSG_ERROR) {
                char error[PS_WIRE_MAX + 1];
                memcpy(error, payload, size);
                error[size] = 0;
                CHECK(strstr(error, expected) && strstr(error, ".phys:"));
                found++;
            }
            ps_wire_consume(&wire, size);
        }
        CHECK(status >= 0);
        if ((!running && got <= 0) || ps_clock() > deadline)
            break;
        ps_sleep(1);
    }
    if (child.running)
        ps_process_kill(&child);
    int code = child.exit_code;
    ps_process_close(&child);
    CHECK(found == 1 && code != 0 && wire.used == 0);
    return 0;
}
static int runner(const char *runner_path, const char *module_path, const char *work,
                  int pendulum) {
    char path[4096];
    CHECK(snprintf(path, sizeof path, "%s/%s-%.0f.psrun", work,
                   pendulum == 2 ? "random"
                   : pendulum == 3 ? "pendulum-rk45"
                   : pendulum == 4 ? "pendulum-verlet"
                   : pendulum    ? "pendulum"
                                 : "projectile",
                   ps_clock() * 1e6) < (int)sizeof path);
    const char *args[] = {runner_path, module_path, path,     "--steps", "200",
                          "--dt",      "0.005",     "--seed", "42",      NULL};
    ps_process child = {0};
    CHECK(ps_process_start(&child, args, work));
    double deadline = ps_clock() + 20;
    char output[4096];
    while (ps_process_poll(&child) && ps_clock() < deadline) {
        int n = ps_process_read(&child, output, sizeof output);
        if (n > 0)
            fwrite(output, 1, (size_t)n, stdout);
        ps_sleep(1);
    }
    if (child.running)
        ps_process_kill(&child);
    int code = child.exit_code;
    ps_process_close(&child);
    CHECK(code == 0);
    ps_run_reader reader;
    CHECK(ps_run_open(&reader, path) == PS_OK);
    CHECK(reader.channels == (pendulum == 2 ? 2u : pendulum ? 6u : 5u));
    CHECK(strstr(reader.metadata, "language=physim-0.182.0\ncompiler=physimc-0.1.0-dev"));
    CHECK(strstr(reader.metadata, "source_fnv1a64=") && strstr(reader.metadata, "module_fnv1a64="));
    if (pendulum == 3)
        CHECK(strstr(reader.metadata, "integrator=Dormand-Prince 5(4)"));
    if (pendulum == 4)
        CHECK(strstr(reader.metadata, "integrator=velocity Verlet"));
    double theta = 0.45, omega = 0;
    ps_rng random;
    ps_rng_seed(&random, 42);
    for (unsigned frame = 0; frame <= 200; frame++) {
        double time, values[PS_MAX_CHANNELS];
        CHECK(ps_run_next(&reader, &time, values) == PS_OK);
        CHECK(near(time, frame * 0.005));
        if (pendulum == 2) {
            double uniform, normal;
            CHECK(ps_distribution_sample((ps_distribution){PS_DIST_UNIFORM, -2, 3}, &random,
                                         &uniform) == PS_OK);
            CHECK(ps_distribution_sample((ps_distribution){PS_DIST_NORMAL, 5, 0.25}, &random,
                                         &normal) == PS_OK);
            CHECK(values[0] == uniform && near(values[1], normal));
        } else if (pendulum == 3 || pendulum == 4) {
            CHECK(isfinite(values[0]) && isfinite(values[1]));
            CHECK(near(values[2], 1.5 * sin(values[0])));
            CHECK(near(values[3], -1.5 * cos(values[0])));
            CHECK(near(values[5], values[0]));
            CHECK(fabs(values[4] - 9.80665 * 1.5 * (1 - cos(0.45))) <
                  (pendulum == 3 ? 1e-7 : 1e-3));
        } else if (pendulum) {
            if (frame)
                ps_symplectic_step(&theta, &omega, -9.80665 / 1.5 * sin(theta), 0.005);
            CHECK(near(values[0], theta) && near(values[1], omega));
            CHECK(near(values[2], 1.5 * sin(theta)) && near(values[3], -1.5 * cos(theta)));
        } else {
            CHECK(near(values[0], -2 + 2 * time));
            CHECK(near(values[1], 5 * time - 0.5 * 9.80665 * time * time));
            CHECK(near(values[2], 2) && near(values[3], 5 - 9.80665 * time));
            CHECK(near(values[4], 14.5));
        }
    }
    double time, values[PS_MAX_CHANNELS];
    CHECK(ps_run_next(&reader, &time, values) == PS_EOF);
    CHECK(reader.complete && reader.samples == 201);
    ps_run_reader_close(&reader);
    return 0;
}
static int rng_streams_api(const char *path) {
    void *module;
    const ps_experiment_api *api = load(path, &module);
    CHECK(api);
    const uint64_t seeds[] = {0, 42, UINT64_MAX};
    for (unsigned s = 0; s < 3; s++) {
        ps_context c = context();
        c.seed = seeds[s];
        CHECK(api->create(&c) == PS_OK && c.channel_count == 2);
        ps_rng host_before = c.rng;
        ps_rng one, two;
        ps_rng_seed(&one, c.seed ^ UINT64_C(1));
        ps_rng_seed(&two, c.seed ^ UINT64_C(2));
        double first[2] = {0};
        for (unsigned frame = 0; frame < 64; frame++) {
            if (frame)
                CHECK(api->step(&c, 0.01) == PS_OK);
            double a, b;
            CHECK(ps_distribution_sample((ps_distribution){PS_DIST_UNIFORM, 0, 1}, &one, &a) ==
                  PS_OK);
            CHECK(ps_distribution_sample((ps_distribution){PS_DIST_UNIFORM, 0, 1}, &two, &b) ==
                  PS_OK);
            CHECK(c.values[0] == a && c.values[1] == b);
            CHECK(c.rng.state == host_before.state && c.rng.increment == host_before.increment);
            if (!frame) {
                first[0] = a;
                first[1] = b;
            }
        }
        CHECK(api->reset(&c) == PS_OK);
        CHECK(c.values[0] == first[0] && c.values[1] == first[1]);
        c.seed ^= UINT64_C(12345);
        CHECK(api->reset(&c) == PS_OK);
        ps_rng_seed(&one, c.seed ^ UINT64_C(1));
        ps_rng_seed(&two, c.seed ^ UINT64_C(2));
        double a, b;
        CHECK(ps_distribution_sample((ps_distribution){PS_DIST_UNIFORM, 0, 1}, &one, &a) == PS_OK);
        CHECK(ps_distribution_sample((ps_distribution){PS_DIST_UNIFORM, 0, 1}, &two, &b) == PS_OK);
        CHECK(c.values[0] == a && c.values[1] == b);
        api->destroy(&c);
    }
    ps_module_close(module);
    return 0;
}
int main(int argc, char **argv) {
    CHECK(argc == 25);
    CHECK(pendulum_api(argv[18], argv[19], "integrator=RK4") == 0);
    CHECK(pendulum_api(argv[20], argv[19], "integrator=RK4") == 0);
    CHECK(pendulum_api(argv[21], argv[22], "integrator=Dormand-Prince 5(4)") == 0);
    CHECK(pendulum_api(argv[23], argv[24], "integrator=velocity Verlet") == 0);
    CHECK(spring_api(argv[14], argv[15]) == 0);
    CHECK(shapes_api(argv[12], argv[13], argv[16]) == 0);
    CHECK(random_api(argv[9], argv[10], argv[11]) == 0);
    CHECK(rng_streams_api(argv[17]) == 0);
    void *modules[6] = {0};
    const ps_experiment_api *api[6];
    for (unsigned i = 0; i < 6; i++) {
        api[i] = load(argv[i + 1], &modules[i]);
        CHECK(api[i] && api[i]->abi_version == PS_ABI_VERSION &&
              api[i]->struct_size == sizeof *api[i]);
    }
    ps_context a = context(), b = context(), reference = context();
    CHECK(api[0]->create(&a) == PS_OK && api[0]->create(&b) == PS_OK);
    CHECK(a.user && b.user && a.user != b.user);
    CHECK(api[1]->create(&reference) == PS_OK);
    CHECK(a.channel_count == reference.channel_count && a.channel_count == 6);
    for (uint32_t ch = 0; ch < a.channel_count; ch++) {
        CHECK(!strcmp(a.channels[ch].name, reference.channels[ch].name));
        CHECK(!memcmp(a.channels[ch].dimension, reference.channels[ch].dimension, 7));
        CHECK(near(a.values[ch], reference.values[ch]));
    }
    for (unsigned frame = 0; frame < 800; frame++) {
        CHECK(api[0]->step(&a, 0.005) == PS_OK);
        CHECK(api[1]->step(&reference, 0.005) == PS_OK);
        a.time_s = reference.time_s = (frame + 1) * 0.005;
        for (uint32_t ch = 0; ch < a.channel_count; ch++)
            CHECK(near(a.values[ch], reference.values[ch]));
    }
    CHECK(near(b.values[0], 0.45) && near(b.values[1], 0));
    CHECK(api[0]->step(&b, 0.01) == PS_OK);
    double other_angle = b.values[0];
    CHECK(api[0]->reset(&a) == PS_OK && near(a.values[0], 0.45));
    CHECK(b.values[0] == other_angle);
    ps_scene scene = {0};
    api[0]->build_scene(&a, &scene);
    CHECK(ps_scene_valid(&scene) && scene.count == 9 && scene.objects[2].parent_id==101);
    CHECK(scene.objects[2].shape == PS_SPHERE && scene.objects[2].id == 3);
    CHECK(near(scene.objects[2].a.x, a.values[2]) && near(scene.objects[2].a.y, a.values[3]));
    CHECK(api[0]->step(&a, 0) == PS_INVALID && api[0]->step(&a, NAN) == PS_INVALID);
    CHECK(api[0]->create(&a) == PS_INVALID);
    api[0]->destroy(&a);
    api[0]->destroy(&b);
    api[1]->destroy(&reference);
    CHECK(!a.user && !b.user && !reference.user);
    api[0]->destroy(&a);

    ps_context projectile = context();
    CHECK(api[2]->create(&projectile) == PS_OK && projectile.channel_count == 5);
    for (unsigned i = 0; i < 200; i++)
        CHECK(api[2]->step(&projectile, 0.005) == PS_OK);
    CHECK(near(projectile.values[0], 0) && near(projectile.values[1], 0.096675));
    CHECK(near(projectile.values[3], -4.80665) && near(projectile.values[4], 14.5));
    api[2]->build_scene(&projectile, &scene);
    CHECK(ps_scene_valid(&scene) && scene.count == 2);
    api[2]->destroy(&projectile);

    ps_context bad = context();
    CHECK(api[3]->create(&bad) == PS_OK);
    CHECK(api[3]->step(&bad, 0.005) == PS_NUMERIC);
    CHECK(strstr(bad.error, "Division by zero") && strstr(bad.error, "experiment_error.phys:"));
    CHECK(api[3]->step(&bad, 0.005) == PS_NUMERIC);
    CHECK(api[3]->reset(&bad) == PS_OK && bad.error[0] == 0 && bad.values[0] == 1);
    api[3]->destroy(&bad);
    bad = context();
    CHECK(api[4]->create(&bad) == PS_OK);
    api[4]->build_scene(&bad, &scene);
    CHECK(!scene.count && strstr(bad.error, "Invalid sphere"));
    CHECK(api[4]->step(&bad, 0.005) == PS_NUMERIC);
    CHECK(api[4]->reset(&bad) == PS_OK);
    api[4]->destroy(&bad);
    bad = context();
    CHECK(api[5]->create(&bad) == PS_NUMERIC);
    CHECK(!bad.user && !bad.channel_count && strstr(bad.error, "Assertion failed"));
    api[5]->destroy(&bad);
    CHECK(api[5]->create(&bad) == PS_NUMERIC && !bad.user);
    /* A trapped failure must not leave another module's trap/depth active. */
    a = context();
    CHECK(api[0]->create(&a) == PS_OK && api[0]->step(&a, 0.005) == PS_OK);
    api[0]->destroy(&a);
    for (unsigned i = 0; i < 6; i++)
        ps_module_close(modules[i]);
    CHECK(runner(argv[7], argv[1], argv[8], 1) == 0);
    CHECK(runner(argv[7], argv[3], argv[8], 0) == 0);
    CHECK(runner(argv[7], argv[9], argv[8], 2) == 0);
    CHECK(runner(argv[7], argv[21], argv[8], 3) == 0);
    CHECK(runner(argv[7], argv[23], argv[8], 4) == 0);
    CHECK(runner_fault(argv[7], argv[11], argv[8], "requires reset or step", 1) == 0);
    CHECK(runner_fault(argv[7], argv[4], argv[8], "Division by zero", 0) == 0);
    CHECK(runner_fault(argv[7], argv[5], argv[8], "Invalid sphere", 0) == 0);
    CHECK(runner_fault(argv[7], argv[6], argv[8], "Assertion failed", 1) == 0);
    puts("Language experiments: symplectic/RK4/RK45/Verlet C references, isolated instances, lifecycle, trapped faults and "
         "real runner files passed");
    return 0;
}
