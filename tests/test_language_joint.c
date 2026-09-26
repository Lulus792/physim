#include "physim/data.h"
#include "physim/mechanics.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Language joint %d: %s\n", __LINE__, #x);                              \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static int near(double a, double b) {
    return isfinite(a) && isfinite(b) && fabs(a - b) <= 1e-10 * fmax(1, fabs(b));
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
typedef struct {
    ps_body body;
    ps_distance_joint joint;
    ps_distance_joint_solution solution;
} reference;
static ps_vec3 anchor_point(const reference *r) {
    return ps_vadd(r->body.position_m, ps_quat_rotate(r->body.orientation, r->joint.anchor_a_m));
}
static double distance(ps_vec3 a, ps_vec3 b) {
    ps_vec3 d = ps_vsub(a, b);
    return hypot(hypot(d.x, d.y), d.z);
}
static int initialize(reference *r) {
    memset(r, 0, sizeof *r);
    CHECK(ps_body_box(1, ps_v3(.3, .8, .3), &r->body) == PS_OK);
    r->body.position_m = ps_v3(0, .6, 0);
    r->body.velocity_m_s = ps_v3(.5, 0, 0);
    r->body.orientation = ps_quat_axis_angle(ps_v3(0, 0, 1), .35);
    r->body.angular_velocity_rad_s = ps_v3(0, 0, .4);
    r->joint.anchor_a_m = ps_v3(0, .4, 0);
    r->joint.anchor_b_m = ps_v3(0, 2, 0);
    r->joint.length_m = distance(anchor_point(r), r->joint.anchor_b_m);
    r->joint.stabilization = .2;
    /* NULL world in C must agree with the explicit mass-zero Body in .phys. */
    CHECK(ps_distance_joint_resolve(&r->body, NULL, &r->joint, .005, &r->solution) == PS_OK);
    return 0;
}
static int advance(reference *r, double dt) {
    CHECK(ps_body_apply_impulse(&r->body, ps_v3(0, -9.81 * dt, 0), r->body.position_m) == PS_OK);
    CHECK(ps_distance_joint_resolve(&r->body, NULL, &r->joint, dt, &r->solution) == PS_OK);
    CHECK(ps_body_step(&r->body, ps_v3(0, 0, 0), ps_v3(0, 0, 0), dt) == PS_OK);
    return 0;
}
static int compare(const reference *r, const double *values) {
    const ps_body *b = &r->body;
    double kinetic;
    CHECK(ps_body_kinetic_energy(b, &kinetic) == PS_OK);
    const double expected[] = {b->position_m.x,
                               b->position_m.y,
                               b->position_m.z,
                               b->velocity_m_s.x,
                               b->velocity_m_s.y,
                               b->orientation.z,
                               b->orientation.w,
                               b->angular_velocity_rad_s.z,
                               kinetic + 9.81 * b->position_m.y,
                               distance(anchor_point(r), r->joint.anchor_b_m) - r->joint.length_m,
                               r->solution.impulse_on_a_ns.x,
                               r->solution.impulse_on_a_ns.y,
                               r->solution.length_error_m,
                               r->solution.velocity_error_m_s};
    for (unsigned i = 0; i < sizeof expected / sizeof expected[0]; i++) {
        if (!near(values[i], expected[i]))
            fprintf(stderr, "channel %u: %.17g expected %.17g\n", i, values[i], expected[i]);
        CHECK(near(values[i], expected[i]));
    }
    return 0;
}
static int scene_matches(const reference *r, const ps_scene *scene) {
    CHECK(ps_scene_valid(scene) && scene->count == 6);
    const ps_object *o = scene->objects;
    CHECK(o[0].shape == PS_BOX && near3(o[0].a, r->body.position_m));
    CHECK(near3(o[0].b, ps_v3(.3, .8, .3)));
    CHECK(near(o[0].orientation.x, r->body.orientation.x) &&
          near(o[0].orientation.y, r->body.orientation.y) &&
          near(o[0].orientation.z, r->body.orientation.z) &&
          near(o[0].orientation.w, r->body.orientation.w));
    CHECK(o[1].shape == PS_LINE && near3(o[1].a, r->joint.anchor_b_m) &&
          near3(o[1].b, anchor_point(r)));
    CHECK(o[2].shape == PS_POINT && near3(o[2].a, r->joint.anchor_b_m));
    CHECK(o[3].shape == PS_POINT && near3(o[3].a, anchor_point(r)));
    CHECK(o[4].shape == PS_ARROW && near3(o[4].a, r->body.position_m) &&
          near3(o[4].b, ps_vadd(r->body.position_m, ps_vscale(r->body.velocity_m_s, .3))));
    CHECK(o[5].shape == PS_LABEL);
    return 0;
}
static int modules(const char *path, const char *fault_path) {
    void *module;
    const ps_experiment_api *api = load(path, &module);
    CHECK(api);
    ps_context a = context(), b = context();
    CHECK(api->create(&a) == PS_OK && api->create(&b) == PS_OK);
    CHECK(a.channel_count == 14 && !strcmp(a.channels[10].name, "joint.impulse.x"));
    CHECK(!strcmp(a.channels[12].name, "joint.lengthError") &&
          !strcmp(a.channels[13].name, "joint.velocityError"));
    CHECK(a.channels[10].dimension[0] == 1 && a.channels[10].dimension[1] == 1 &&
          a.channels[10].dimension[2] == -1);
    CHECK(a.channels[12].dimension[0] == 1 && a.channels[12].dimension[1] == 0 &&
          a.channels[12].dimension[2] == 0);
    CHECK(a.channels[13].dimension[0] == 1 && a.channels[13].dimension[2] == -1);
    double initial[14];
    memcpy(initial, a.values, sizeof initial);
    double drift[2] = {0};
    for (unsigned repeat = 0; repeat < 2; repeat++) {
        reference r;
        CHECK(initialize(&r) == 0);
        double dt = repeat ? .002 : .004;
        unsigned steps = repeat ? 2000 : 1000;
        for (unsigned i = 0; i <= steps; i++) {
            CHECK(compare(&r, a.values) == 0);
            drift[repeat] = fmax(drift[repeat], fabs(a.values[9]));
            if (!(i % 100)) {
                ps_scene scene = {0};
                api->build_scene(&a, &scene);
                CHECK(!a.error[0] && scene_matches(&r, &scene) == 0);
            }
            if (i != steps)
                CHECK(api->step(&a, dt) == PS_OK && advance(&r, dt) == 0);
        }
        CHECK(!memcmp(initial, b.values, sizeof initial));
        CHECK(api->reset(&a) == PS_OK && !memcmp(initial, a.values, sizeof initial));
    }
    CHECK(drift[0] > 0 && drift[1] < drift[0] * .4);
    api->destroy(&a);
    api->destroy(&b);
    ps_module_close(module);
    api = load(fault_path, &module);
    CHECK(api);
    a = context();
    CHECK(api->create(&a) == PS_OK);
    for (unsigned repeat = 0; repeat < 2; repeat++)
        for (unsigned mode = 1; mode <= 7; mode++) {
            CHECK(api->step(&a, (double)mode) == PS_NUMERIC);
            const char *expected = mode <= 3   ? "Invalid distance joint"
                                   : mode == 4 ? "anchors coincide"
                                               : "Distance joint solver failed";
            CHECK(strstr(a.error, expected) && strstr(a.error, "joint_error.phys:"));
            CHECK(api->step(&a, .002) == PS_NUMERIC);
            CHECK(api->reset(&a) == PS_OK && !a.error[0]);
        }
    api->destroy(&a);
    ps_module_close(module);
    return 0;
}
static int run_file(const char *runner, const char *module, const char *work) {
    char path[4096];
    CHECK(snprintf(path, sizeof path, "%s/joint-%.0f.psrun", work, ps_clock() * 1e6) <
          (int)sizeof path);
    const char *args[] = {runner, module, path, "--steps", "400", "--dt", "0.005", NULL};
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
    CHECK(ps_run_open(&reader, path) == PS_OK && reader.channels == 14);
    CHECK(strstr(reader.metadata, "model=rigid joint pendulum") &&
          strstr(reader.metadata, "language=physim"));
    reference r;
    CHECK(initialize(&r) == 0);
    double time, values[PS_MAX_CHANNELS];
    for (unsigned i = 0; i <= 400; i++) {
        CHECK(ps_run_next(&reader, &time, values) == PS_OK && near(time, i * .005));
        CHECK(compare(&r, values) == 0 && advance(&r, .005) == 0);
    }
    CHECK(ps_run_next(&reader, &time, values) == PS_EOF);
    ps_run_reader_close(&reader);
    return 0;
}
int main(int argc, char **argv) {
    CHECK(argc == 5);
    CHECK(modules(argv[1], argv[2]) == 0 && run_file(argv[3], argv[1], argv[4]) == 0);
    puts("Language joints: C parity, local anchors, drift refinement, scene, independent states, "
         "reset, diagnostics and runner passed");
    return 0;
}
