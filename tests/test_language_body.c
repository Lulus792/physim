#define PSRT_MODULE
#include "physim/language_runtime.h"
#include "physim/data.h"
#include "physim/language_mechanics.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Language body %d: %s\n", __LINE__, #x);                               \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static int near(double a, double b) {
    return isfinite(a) && isfinite(b) && fabs(a - b) <= 1e-11 * fmax(1, fabs(b));
}
static int near3(ps_vec3 a, ps_vec3 b) {
    return near(a.x, b.x) && near(a.y, b.y) && near(a.z, b.z);
}
static ps_context context(void) {
    ps_context c = {0};
    c.struct_size = sizeof c;
    c.api_version = PS_API_VERSION;
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
static ps_vec3 application_point(const ps_body *b) {
    return ps_vadd(b->position_m, ps_quat_rotate(b->orientation, ps_v3(.3, .1, -.2)));
}
static int initialize(ps_body *b) {
    CHECK(ps_body_box(2, ps_v3(.8, .5, .3), b) == PS_OK);
    b->position_m = ps_v3(-1, 1, 0);
    b->velocity_m_s = ps_v3(.5, .2, -.1);
    b->orientation = ps_quat_axis_angle(ps_v3(1, 2, 3), .2);
    b->angular_velocity_rad_s = ps_v3(.2, .3, .5);
    CHECK(ps_body_apply_impulse(b, ps_v3(.1, 0, .2), ps_vadd(b->position_m, ps_v3(0, .2, 0))) ==
          PS_OK);
    return 0;
}
static int advance(ps_body *b, double dt) {
    ps_vec3 force = ps_v3(.4, -1, .2), torque;
    CHECK(ps_body_force_torque(b, force, application_point(b), &torque) == PS_OK);
    CHECK(ps_body_step(b, force, torque, dt) == PS_OK);
    return 0;
}
static int compare(const ps_body *b, const double *values) {
    double energy;
    ps_vec3 velocity;
    CHECK(ps_body_kinetic_energy(b, &energy) == PS_OK);
    CHECK(ps_body_point_velocity(b, application_point(b), &velocity) == PS_OK);
    const double expected[] = {b->position_m.x,
                               b->position_m.y,
                               b->position_m.z,
                               b->velocity_m_s.x,
                               b->velocity_m_s.y,
                               b->velocity_m_s.z,
                               b->orientation.x,
                               b->orientation.y,
                               b->orientation.z,
                               b->orientation.w,
                               b->angular_velocity_rad_s.x,
                               b->angular_velocity_rad_s.y,
                               b->angular_velocity_rad_s.z,
                               energy,
                               velocity.x};
    for (unsigned i = 0; i < 15; i++)
        CHECK(near(values[i], expected[i]));
    return 0;
}
static int modules(const char *path, const char *fault_path) {
    void *module;
    const ps_experiment_api *api = load(path, &module);
    CHECK(api);
    ps_context a = context(), b = context();
    CHECK(api->create(&a) == PS_OK && api->create(&b) == PS_OK);
    CHECK(a.channel_count == 15 && !strcmp(a.channels[6].name, "orientation.x"));
    CHECK(!strcmp(a.channels[13].unit, "J"));
    CHECK(a.channels[13].dimension[0] == 2 && a.channels[13].dimension[1] == 1 &&
          a.channels[13].dimension[2] == -2);
    double initial[15];
    memcpy(initial, a.values, sizeof initial);
    for (unsigned repeat = 0; repeat < 2; repeat++) {
        ps_body reference;
        CHECK(initialize(&reference) == 0);
        for (unsigned step = 0; step <= 500; step++) {
            CHECK(compare(&reference, a.values) == 0);
            if (!(step % 50)) {
                ps_scene scene;
                api->build_scene(&a, &scene);
                CHECK(!a.error[0] && ps_scene_valid(&scene) && scene.count == 3);
                CHECK(scene.objects[0].shape == PS_BOX &&
                      near3(scene.objects[0].a, reference.position_m));
                CHECK(near(scene.objects[0].orientation.x, reference.orientation.x) &&
                      near(scene.objects[0].orientation.y, reference.orientation.y) &&
                      near(scene.objects[0].orientation.z, reference.orientation.z) &&
                      near(scene.objects[0].orientation.w, reference.orientation.w));
                CHECK(scene.objects[1].shape == PS_ARROW &&
                      near3(scene.objects[1].a, application_point(&reference)));
            }
            double dt = step == 250 ? .01 : .002;
            CHECK(api->step(&a, dt) == PS_OK && advance(&reference, dt) == 0);
        }
        CHECK(!memcmp(initial, b.values, sizeof initial));
        CHECK(api->reset(&a) == PS_OK && !memcmp(initial, a.values, sizeof initial));
    }
    api->destroy(&a);
    api->destroy(&b);
    ps_module_close(module);
    api = load(fault_path, &module);
    CHECK(api);
    a = context();
    CHECK(api->create(&a) == PS_OK);
    const char *errors[] = {"Invalid sphere body", "Invalid box body",   "Body step failed",
                            "Invalid body state",  "Invalid body state", "Body impulse failed"};
    for (unsigned repeat = 0; repeat < 2; repeat++)
        for (unsigned mode = 1; mode <= 6; mode++) {
            CHECK(api->step(&a, (double)mode) == PS_NUMERIC);
            CHECK(strstr(a.error, errors[mode - 1]) && strstr(a.error, "body_error.phys:"));
            CHECK(api->step(&a, .002) == PS_NUMERIC);
            CHECK(api->reset(&a) == PS_OK && !a.error[0]);
        }
    api->destroy(&a);
    ps_module_close(module);
    return 0;
}
static int atomic_errors(void) {
    ps_body *body = malloc(sizeof *body);
    char *error = malloc(256);
    CHECK(body && error && ps_body_sphere(2, .5, body) == PS_OK);
    const ps_body original = *body;
    for (unsigned mode = 0; mode < 3; mode++) {
        memset(error, 0, 256);
        psrt_trap trap = {0};
        trap.error = error;
        trap.capacity = 256;
        psrt_current = &trap;
        if (!setjmp(trap.jump)) {
            if (mode == 0)
                psrt_body_set_state(body, ps_v3(1, 2, 3), ps_v3(0, 0, 0), (ps_quat){0, 0, 0, 2},
                                    ps_v3(0, 0, 0), PSRT_AT(1, 1));
            else if (mode == 1)
                psrt_body_apply_impulse(body, ps_v3(1e308, 0, 0), ps_v3(0, 1e308, 0),
                                        PSRT_AT(2, 1));
            else
                psrt_body_step(body, ps_v3(1e308, 0, 0), ps_v3(0, 0, 0), 1e308, PSRT_AT(3, 1));
            CHECK(0);
        }
        psrt_current = NULL;
        CHECK(error[0] && !memcmp(body, &original, sizeof original));
    }
    free(body);
    free(error);
    return 0;
}
static int run_file(const char *runner, const char *module, const char *work) {
    char path[4096];
    CHECK(snprintf(path, sizeof path, "%s/body-%.0f.psrun", work, ps_clock() * 1e6) <
          (int)sizeof path);
    const char *args[] = {runner, module, path, "--steps", "200", "--dt", "0.005", NULL};
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
    CHECK(strstr(reader.metadata, "model=driven rigid box") &&
          strstr(reader.metadata, "language=physim"));
    ps_body reference;
    CHECK(initialize(&reference) == 0);
    double time, values[PS_MAX_CHANNELS];
    for (unsigned i = 0; i <= 200; i++) {
        CHECK(ps_run_next(&reader, &time, values) == PS_OK && near(time, i * .005));
        CHECK(compare(&reference, values) == 0 && advance(&reference, .005) == 0);
    }
    CHECK(ps_run_next(&reader, &time, values) == PS_EOF);
    ps_run_reader_close(&reader);
    return 0;
}
int main(int argc, char **argv) {
    CHECK(argc == 5);
    CHECK(atomic_errors() == 0 && modules(argv[1], argv[2]) == 0);
    CHECK(run_file(argv[3], argv[1], argv[4]) == 0);
    puts("Language bodies: C parity, orientation, scene, state isolation, atomic errors and runner "
         "passed");
    return 0;
}
