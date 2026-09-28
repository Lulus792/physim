#define PSRT_MODULE
#include "physim/language_runtime.h"
#include "physim/data.h"
#include "physim/language_collision.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Language sweep %d: %s\n", __LINE__, #x);                              \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static int near(double a, double b) {
    return isfinite(a) && isfinite(b) && fabs(a - b) <= 1e-9 * fmax(1, fabs(b));
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
    double time, impulse, first;
    unsigned hits, total;
} reference;
static int initialize(reference *r) {
    memset(r, 0, sizeof *r);
    CHECK(ps_body_sphere(1, .12, &r->body) == PS_OK);
    r->body.position_m = ps_v3(0, .6, 0);
    r->body.velocity_m_s.x = 80;
    r->first = 1;
    return 0;
}
static int advance(reference *r, double dt) {
    double remaining = dt;
    r->hits = 0;
    r->impulse = 0;
    r->first = 1;
    while (remaining > 0) {
        ps_sweep_hit best = {0};
        bool found = false;
        for (unsigned side = 0; side < 2; side++) {
            ps_sweep_hit hit;
            bool touching;
            CHECK(ps_sweep_sphere_plane(&r->body, .12, ps_vscale(r->body.velocity_m_s, remaining),
                                        ps_v3(side ? 1.5 : -1.5, 0, 0), ps_v3(side ? -1 : 1, 0, 0),
                                        &hit, &touching) == PS_OK);
            if (touching && ps_vdot(r->body.velocity_m_s, hit.contact.normal) > 0 &&
                (!found || hit.fraction < best.fraction)) {
                best = hit;
                found = true;
            }
        }
        double travel = found ? remaining * best.fraction : remaining;
        if (travel > 0)
            CHECK(ps_body_step(&r->body, ps_v3(0, 0, 0), ps_v3(0, 0, 0), travel) == PS_OK);
        remaining -= travel;
        if (!found)
            break;
        CHECK(r->hits < 16);
        ps_contact_manifold manifold = {0};
        manifold.count = 1;
        manifold.points[0] = best.contact;
        ps_contact_solver solver = {64, 1, 0, 0, 0, 1};
        ps_contact_solution response;
        CHECK(ps_contacts_resolve(&r->body, NULL, &manifold, &solver, &response) == PS_OK);
        r->impulse += response.impulse_on_a_ns[0].x;
        if (!r->hits)
            r->first = (dt - remaining) / dt;
        r->hits++;
    }
    r->total += r->hits;
    r->time += dt;
    return 0;
}
static int compare(const reference *r, const double *v) {
    double energy;
    CHECK(ps_body_kinetic_energy(&r->body, &energy) == PS_OK);
    const double expected[] = {r->body.position_m.x,
                               r->body.velocity_m_s.x,
                               energy,
                               r->hits,
                               r->impulse,
                               r->first,
                               r->total};
    for (unsigned i = 0; i < 7; i++) {
        if (!near(v[i], expected[i]))
            fprintf(stderr, "channel %u: %.17g expected %.17g\n", i, v[i], expected[i]);
        CHECK(near(v[i], expected[i]));
    }
    double phase = fmod(1.38 + 80 * r->time, 5.52);
    double x = phase <= 2.76 ? phase - 1.38 : 4.14 - phase;
    CHECK(fabs(v[0] - x) < 1e-8 && near(v[2], 3200) && fabs(v[0]) <= 1.38 + 1e-12);
    if (phase > 1e-8 && fabs(phase - 2.76) > 1e-8 && phase < 5.52 - 1e-8)
        CHECK(near(v[1], phase < 2.76 ? 80 : -80));
    return 0;
}
static int modules(const char *path, const char *fault_path) {
    void *module;
    const ps_experiment_api *api = load(path, &module);
    CHECK(api);
    ps_context a = context(), b = context();
    CHECK(api->create(&a) == PS_OK && api->create(&b) == PS_OK);
    CHECK(a.channel_count == 7 && !strcmp(a.channels[5].name, "first.fraction"));
    CHECK(a.channels[4].dimension[0] == 1 && a.channels[4].dimension[1] == 1 &&
          a.channels[4].dimension[2] == -1);
    double initial[7];
    memcpy(initial, a.values, sizeof initial);
    const double steps[] = {.005, .003, .05, .55};
    for (unsigned mode = 0; mode < 4; mode++) {
        reference r;
        CHECK(initialize(&r) == 0);
        unsigned count = mode < 2 ? 1000 : 40;
        for (unsigned i = 0; i <= count; i++) {
            CHECK(compare(&r, a.values) == 0);
            if (!(i % 10)) {
                ps_scene scene = {0};
                api->build_scene(&a, &scene);
                CHECK(!a.error[0] && ps_scene_valid(&scene) && scene.count == 6);
                CHECK(scene.objects[0].shape == PS_SPHERE &&
                      near3(scene.objects[0].a, r.body.position_m) &&
                      near(scene.objects[0].radius, .12));
                CHECK(scene.objects[1].shape == PS_PLANE && scene.objects[2].shape == PS_PLANE &&
                      scene.objects[3].shape == PS_ARROW && scene.objects[5].shape == PS_LABEL);
                CHECK(near3(scene.objects[3].b,
                            ps_vadd(r.body.position_m, ps_vscale(r.body.velocity_m_s, .006))));
            }
            if (i != count)
                CHECK(api->step(&a, steps[mode]) == PS_OK && advance(&r, steps[mode]) == 0);
        }
        CHECK(r.total > 30 && !memcmp(initial, b.values, sizeof initial));
        CHECK(api->reset(&a) == PS_OK && !memcmp(initial, a.values, sizeof initial));
    }
    /* The bounded event loop must fail atomically, without dropping remaining time. */
    CHECK(api->step(&a, 1) == PS_NUMERIC && strstr(a.error, "fast_sphere.phys:"));
    CHECK(!memcmp(initial, a.values, sizeof initial));
    CHECK(api->reset(&a) == PS_OK && api->step(&a, .005) == PS_OK);
    api->destroy(&a);
    api->destroy(&b);
    ps_module_close(module);
    api = load(fault_path, &module);
    CHECK(api);
    a = context();
    CHECK(api->create(&a) == PS_OK);
    const char *messages[] = {
        "no contact fraction", "Sphere sweep failed",        "Sphere-plane sweep failed",
        "Box bounds failed",   "Swept sphere bounds failed", "at most 1024",
        "Array index"};
    for (unsigned repeat = 0; repeat < 2; repeat++)
        for (unsigned mode = 1; mode <= 7; mode++) {
            CHECK(api->step(&a, mode) == PS_NUMERIC && strstr(a.error, "sweep_error.phys:") &&
                  strstr(a.error, messages[mode - 1]));
            CHECK(api->step(&a, .005) == PS_NUMERIC && api->reset(&a) == PS_OK && !a.error[0]);
        }
    api->destroy(&a);
    ps_module_close(module);
    return 0;
}
typedef struct {
    bool deny;
    size_t live;
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
    char *error = calloc(256, 1);
    CHECK(s && error);
    ps_allocator allocator = {s, allocate, deallocate};
    ps_aabb bounds[2] = {{{0, 0, 0}, {1, 1, 1}}, {{0, 0, 0}, {1, 1, 1}}};
    psrt_array pairs = psrt_collision_pairs(allocator, bounds, 2, PSRT_AT(1, 1)), copy;
    CHECK(psrt_array_count(&pairs) == 1 && psrt_array_clone(&pairs, &copy) == PS_OK);
    size_t bytes = s->live;
    s->deny = true;
    psrt_trap trap = {0};
    trap.error = error;
    trap.capacity = 256;
    psrt_current = &trap;
    if (!setjmp(trap.jump)) {
        (void)psrt_collision_pairs(allocator, bounds, 2, PSRT_AT(2, 1));
        CHECK(0);
    }
    psrt_current = NULL;
    CHECK(strstr(error, "allocation") && s->live == bytes);
    psrt_array empty = psrt_collision_pairs(allocator, NULL, 0, PSRT_AT(3, 1));
    CHECK(psrt_array_count(&empty) == 0);
    psrt_array_destroy(&pairs);
    CHECK(((const ps_collision_pair *)psrt_array_data(&copy))[0].b == 1);
    psrt_array_destroy(&copy);
    psrt_array_destroy(&empty);
    CHECK(s->live == 0);
    free(s);
    free(error);
    return 0;
}
static int run_file(const char *runner, const char *module, const char *work) {
    char path[4096];
    CHECK(snprintf(path, sizeof path, "%s/sweep-%.0f.psrun", work, ps_clock() * 1e6) <
          (int)sizeof path);
    const char *args[] = {runner, module, path, "--steps", "100", "--dt", "0.05", NULL};
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
    CHECK(ps_run_open(&reader, path) == PS_OK && reader.channels == 7 &&
          strstr(reader.metadata, "model=fast sphere"));
    reference r;
    CHECK(initialize(&r) == 0);
    double time, values[PS_MAX_CHANNELS];
    for (unsigned i = 0; i <= 100; i++) {
        CHECK(ps_run_next(&reader, &time, values) == PS_OK && near(time, i * .05));
        CHECK(compare(&r, values) == 0 && advance(&r, .05) == 0);
    }
    CHECK(ps_run_next(&reader, &time, values) == PS_EOF);
    ps_run_reader_close(&reader);
    return 0;
}
int main(int argc, char **argv) {
    CHECK(argc == 5 && ownership() == 0 && modules(argv[1], argv[2]) == 0 &&
          run_file(argv[3], argv[1], argv[4]) == 0);
    puts("Language sweeps: C and analytic parity, multiple impacts, event limits, broad phase "
         "ownership, errors, scene and runner passed");
    return 0;
}
