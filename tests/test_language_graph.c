#define PSRT_MODULE
#include "physim/language_runtime.h"
#include "physim/data.h"
#include "physim/language_constraints.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Language graph %d: %s\n", __LINE__, #x);                              \
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
    ps_body bodies[2];
    ps_constraint_graph_solution solution;
} reference;
static int solve(reference *r, double dt) {
    ps_contact_constraint contacts[3];
    size_t count = 0;
    for (unsigned i = 0; i < 2; i++) {
        ps_contact c;
        bool touching;
        CHECK(ps_contact_sphere_plane(&r->bodies[i], .25, ps_v3(0, 0, 0), ps_v3(0, 1, 0), &c,
                                      &touching) == PS_OK);
        if (touching)
            contacts[count++] = (ps_contact_constraint){i, PS_CONTACT_WORLD, c};
    }
    ps_contact c;
    bool touching;
    CHECK(ps_contact_spheres(&r->bodies[0], .25, &r->bodies[1], .25, &c, &touching) == PS_OK);
    if (touching)
        contacts[count++] = (ps_contact_constraint){0, 1, c};
    ps_distance_constraint link = {0, 1, {{0}, {0}, 1, .2}};
    ps_contact_solver solver = {128, .1, .5, .5, .00001, .8};
    CHECK(ps_constraints_resolve_graph(r->bodies, 2, contacts, count, &link, 1, &solver, dt,
                                       &r->solution) == PS_OK);
    return 0;
}
static int initialize(reference *r) {
    memset(r, 0, sizeof *r);
    for (unsigned i = 0; i < 2; i++) {
        CHECK(ps_body_sphere(1, .25, &r->bodies[i]) == PS_OK);
        r->bodies[i].position_m = ps_v3(i ? .6 : 0, i ? 1.8 : 1, 0);
        r->bodies[i].velocity_m_s.x = .5;
    }
    return solve(r, .005);
}
static int advance(reference *r, double dt) {
    for (unsigned i = 0; i < 2; i++)
        CHECK(ps_body_apply_impulse(&r->bodies[i], ps_v3(0, -9.81 * dt, 0),
                                    r->bodies[i].position_m) == PS_OK);
    CHECK(solve(r, dt) == 0);
    for (unsigned i = 0; i < 2; i++)
        CHECK(ps_body_step(&r->bodies[i], ps_v3(0, 0, 0), ps_v3(0, 0, 0), dt) == PS_OK);
    return 0;
}
static int compare(const reference *r, const double *values) {
    const ps_body *a = &r->bodies[0], *b = &r->bodies[1];
    double ea, eb, total = 0;
    CHECK(ps_body_kinetic_energy(a, &ea) == PS_OK && ps_body_kinetic_energy(b, &eb) == PS_OK);
    for (unsigned i = 0; i < r->solution.contacts.count; i++)
        total += r->solution.contacts.impulse_on_a_ns[i].y;
    const double expected[] = {a->position_m.x,
                               a->position_m.y,
                               b->position_m.x,
                               b->position_m.y,
                               a->velocity_m_s.x,
                               a->velocity_m_s.y,
                               b->velocity_m_s.x,
                               b->velocity_m_s.y,
                               ea + eb + 9.81 * (a->position_m.y + b->position_m.y),
                               r->solution.contacts.count,
                               total,
                               r->solution.joint_impulse_on_a_ns[0].y,
                               r->solution.contacts.max_normal_error_m_s,
                               r->solution.contacts.max_projection_error_m,
                               r->solution.max_joint_velocity_error_m_s,
                               r->solution.max_joint_length_error_m};
    for (unsigned i = 0; i < 16; i++) {
        if (!near(values[i], expected[i]))
            fprintf(stderr, "channel %u: %.17g expected %.17g\n", i, values[i], expected[i]);
        CHECK(near(values[i], expected[i]));
    }
    return 0;
}
static int modules(const char *path, const char *fault_path) {
    void *module;
    const ps_experiment_api *api = load(path, &module);
    CHECK(api);
    ps_context a = context(), b = context();
    CHECK(api->create(&a) == PS_OK && api->create(&b) == PS_OK);
    CHECK(a.channel_count == 16 && !strcmp(a.channels[10].name, "contact.impulse.y"));
    CHECK(a.channels[10].dimension[0] == 1 && a.channels[10].dimension[1] == 1 &&
          a.channels[10].dimension[2] == -1);
    CHECK(a.channels[15].dimension[0] == 1 && a.channels[15].dimension[2] == 0);
    double initial[16];
    memcpy(initial, a.values, sizeof initial);
    for (unsigned repeat = 0; repeat < 2; repeat++) {
        reference r;
        CHECK(initialize(&r) == 0);
        double dt = repeat ? .001 : .002;
        unsigned steps = repeat ? 4000 : 2000, active = 0;
        for (unsigned i = 0; i <= steps; i++) {
            CHECK(compare(&r, a.values) == 0);
            active += a.values[9] > 0 && a.values[10] > 0;
            if (!(i % 100)) {
                ps_scene scene = {0};
                api->build_scene(&a, &scene);
                CHECK(!a.error[0] && ps_scene_valid(&scene) && scene.count == 5);
                for (unsigned j = 0; j < 2; j++)
                    CHECK(scene.objects[j].shape == PS_SPHERE &&
                          near3(scene.objects[j].a, r.bodies[j].position_m) &&
                          scene.objects[j].radius == .25);
                CHECK(scene.objects[2].shape == PS_LINE &&
                      near3(scene.objects[2].a, r.bodies[0].position_m) &&
                      near3(scene.objects[2].b, r.bodies[1].position_m));
                CHECK(scene.objects[3].shape == PS_PLANE && scene.objects[4].shape == PS_LABEL);
            }
            if (i != steps)
                CHECK(api->step(&a, dt) == PS_OK && advance(&r, dt) == 0);
        }
        CHECK(active > 100);
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
    for (unsigned repeat = 0; repeat < 3; repeat++)
        for (unsigned mode = 1; mode <= 13; mode++) {
            CHECK(api->step(&a, mode) == PS_NUMERIC && strstr(a.error, "graph_error.phys:"));
            const char *expected = mode == 2    ? "distinct"
                                   : mode <= 3  ? "body index"
                                   : mode <= 6  ? "solver failed"
                                   : mode <= 9  ? "capacity exceeded"
                                   : mode <= 11 ? "body index"
                                   : mode == 12 ? "contact index"
                                                : "joint index";
            CHECK(strstr(a.error, expected));
            CHECK(api->step(&a, .002) == PS_NUMERIC);
            CHECK(api->reset(&a) == PS_OK && !a.error[0]);
        }
    api->destroy(&a);
    ps_module_close(module);
    return 0;
}
typedef struct {
    size_t live;
    bool deny;
} allocation_state;
static void *allocate(void *user, size_t bytes) {
    allocation_state *s = user;
    if (s->deny)
        return NULL;
    void *p = malloc(bytes);
    if (p)
        s->live += bytes;
    return p;
}
static void deallocate(void *user, void *p, size_t bytes) {
    ((allocation_state *)user)->live -= bytes;
    free(p);
}
static int ownership(void) {
    allocation_state *s = calloc(1, sizeof *s);
    CHECK(s);
    ps_allocator allocator = {s, allocate, deallocate};
    ps_body body;
    CHECK(ps_body_sphere(1, .25, &body) == PS_OK);
    psrt_constraint_result original = psrt_constraints_solve(
        allocator, PS_CONTACT_SOLVER_DEFAULT, &body, 1, NULL, 0, NULL, 0, .01, PSRT_AT(1, 1));
    psrt_constraint_result copy;
    CHECK(psrt_constraints_copy(&copy, &original) == PS_OK);
    size_t bytes = s->live;
    CHECK(bytes > sizeof(psrt_constraint_storage));
    char *error = calloc(256, 1);
    CHECK(error);
    s->deny = true;
    for (unsigned mode = 0; mode < 2; mode++) {
        memset(error, 0, 256);
        psrt_trap trap = {0};
        trap.error = error;
        trap.capacity = 256;
        psrt_current = &trap;
        if (!setjmp(trap.jump)) {
            if (mode == 0)
                (void)psrt_constraints_solve(allocator, PS_CONTACT_SOLVER_DEFAULT, &body, 1, NULL,
                                             0, NULL, 0, .01, PSRT_AT(2, 1));
            else
                (void)psrt_constraints_bodies(allocator, copy, PSRT_AT(2, 2));
            CHECK(0);
        }
        psrt_current = NULL;
        CHECK(strstr(error, "allocation") && s->live == bytes && body.velocity_m_s.y == 0);
    }
    free(error);
    psrt_constraints_drop(&original);
    CHECK(s->live == bytes && psrt_constraints_body(copy, 0, PSRT_AT(3, 1)).mass_kg == 1);
    s->deny = false;
    psrt_array bodies = psrt_constraints_bodies(allocator, copy, PSRT_AT(4, 1));
    psrt_constraints_drop(&copy);
    CHECK(psrt_array_count(&bodies) == 1 &&
          ((const ps_body *)psrt_array_data(&bodies))[0].mass_kg == 1);
    psrt_array_destroy(&bodies);
    CHECK(s->live == 0);
    free(s);
    return 0;
}
static int run_file(const char *runner, const char *module, const char *work) {
    char path[4096];
    CHECK(snprintf(path, sizeof path, "%s/graph-%.0f.psrun", work, ps_clock() * 1e6) <
          (int)sizeof path);
    const char *args[] = {runner, module, path, "--steps", "800", "--dt", "0.005", NULL};
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
    CHECK(ps_run_open(&reader, path) == PS_OK && reader.channels == 16);
    CHECK(strstr(reader.metadata, "model=coupled spheres") &&
          strstr(reader.metadata, "language=physim"));
    reference r;
    CHECK(initialize(&r) == 0);
    double time, values[PS_MAX_CHANNELS];
    for (unsigned i = 0; i <= 800; i++) {
        CHECK(ps_run_next(&reader, &time, values) == PS_OK && near(time, i * .005));
        CHECK(compare(&r, values) == 0 && advance(&r, .005) == 0);
    }
    CHECK(ps_run_next(&reader, &time, values) == PS_EOF);
    ps_run_reader_close(&reader);
    return 0;
}
int main(int argc, char **argv) {
    CHECK(argc == 5 && ownership() == 0 && modules(argv[1], argv[2]) == 0 &&
          run_file(argv[3], argv[1], argv[4]) == 0);
    puts("Language graph: C parity, coupled contacts/joints, scene, reset, ownership, allocation "
         "failure, diagnostics and stored runs passed");
    return 0;
}
