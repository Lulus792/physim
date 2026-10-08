#include "physim/experiment.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "Pendulum forces %d: %s\n", __LINE__, #x); return 1; \
} } while (0)

static const ps_object *object(const ps_scene *scene, unsigned id) {
    for (unsigned i = 0; i < scene->count; i++)
        if (scene->objects[i].id == id) return &scene->objects[i];
    return NULL;
}
static int near(double a, double b) {
    return isfinite(a) && isfinite(b) && fabs(a - b) < 2e-10 * fmax(1, fabs(b));
}
static int forces(const ps_experiment_api *api, ps_context *c, double length,
                  double density) {
    ps_scene scene = {0};
    api->build_scene(c, &scene);
    CHECK(!c->error[0] && ps_scene_valid(&scene));
    const ps_object *bob = object(&scene, 3), *weight = object(&scene, 7),
                    *rod = object(&scene, 8), *drag = object(&scene, 9);
    CHECK(bob && weight && rod && weight->shape == PS_ARROW && rod->shape == PS_ARROW);
    CHECK(weight->parent_id == 3 && rod->parent_id == 3);
    CHECK(weight->color == 0xe87979ff && rod->color == 0x91d28aff);
    double angle = c->values[0], omega = c->values[1];
    double x = length * sin(angle), y = -length * cos(angle);
    CHECK(near(bob->a.x, x) && near(bob->a.y, y));
    double fx = 0, fy = 0;
    const ps_object *vectors[] = {weight, rod, drag};
    for (unsigned i = 0; i < 3; i++) {
        const ps_object *v = vectors[i];
        if (!v) continue;
        CHECK(v->shape == PS_ARROW && v->parent_id == 3);
        CHECK(near(v->a.x, x) && near(v->a.y, y) && v->a.z == 0 && v->b.z == 0);
        fx += (v->b.x - v->a.x) / .05;
        fy += (v->b.y - v->a.y) / .05;
        const ps_object *label = object(&scene, i == 2 ? 12 : 10 + i);
        CHECK(label && label->shape == PS_LABEL && label->parent_id == v->id);
        CHECK(label->color == v->color && strstr(label->text, "0.05 m/N"));
        CHECK(near(label->a.x, v->b.x) && near(label->a.y, v->b.y));
    }
    CHECK(near((weight->b.x - x) / .05, 0));
    CHECK(near((weight->b.y - y) / .05, -9.80665));
    /* Independent Newton oracle: gravity tangent plus centripetal acceleration.
       The one-kilogram rigid rod model has a radial constraint, not free fall. */
    double tangent_x = cos(angle), tangent_y = sin(angle);
    double speed = length * omega;
    double drag_tangent = -.5 * density * .47 * .01 * speed * fabs(speed);
    double tangent_acceleration = -9.80665 * sin(angle) + drag_tangent;
    CHECK(near(fx, tangent_acceleration * tangent_x - omega * omega * x));
    CHECK(near(fy, tangent_acceleration * tangent_y - omega * omega * y));
    if (density) {
        CHECK(drag && drag->color == 0xc499e8ff);
        double dx = (drag->b.x - x) / .05, dy = (drag->b.y - y) / .05;
        CHECK(near(dx, drag_tangent * tangent_x) && near(dy, drag_tangent * tangent_y));
        CHECK(dx * speed * tangent_x + dy * speed * tangent_y <= 1e-12);
    } else CHECK(!drag);
    return 0;
}
int main(int argc, char **argv) {
    CHECK(argc == 10);
    for (int model = 1; model < argc; model++) {
        void *module = ps_module_open(argv[model]); CHECK(module);
        void *symbol = ps_module_symbol(module, "ps_get_experiment");
        ps_experiment_entry entry = NULL; memcpy(&entry, &symbol, sizeof entry); CHECK(entry);
        const ps_experiment_api *api = entry(); CHECK(api && api->abi_version == PS_ABI_VERSION);
        const double lengths[] = {.3, 1.5, 4}, angles[] = {-.8, 0, .6};
        for (unsigned setting = 0; setting < 3; setting++) {
            ps_context c = {.struct_size = sizeof c, .api_version = PS_API_VERSION, .seed = 42};
            CHECK(ps_parameter_override(&c, "length", lengths[setting]) == PS_OK);
            CHECK(ps_parameter_override(&c, "initialAngle", angles[setting]) == PS_OK);
            CHECK(api->create(&c) == PS_OK && ps_parameter_finalize(&c) == PS_OK);
            for (unsigned step = 0; step <= 400; step++) {
                CHECK(!forces(api, &c, lengths[setting], model == 2 ? 1.225 : 0));
                if (step < 400) {
                    CHECK(api->step(&c, .005) == PS_OK); c.time_s = (step + 1) * .005;
                }
            }
            CHECK(api->reset(&c) == PS_OK);
            CHECK(!forces(api, &c, lengths[setting], model == 2 ? 1.225 : 0));
            api->destroy(&c);
        }
        ps_module_close(module);
    }
    puts("Pendulum forces: C/Physim gravity, rod constraint, dissipative drag, units, hierarchy and reset passed");
    return 0;
}
