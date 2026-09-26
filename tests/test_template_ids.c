#include "physim/experiment.h"
#include "platform.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Template IDs line %d: %s\n", __LINE__, #x);                           \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
int main(int argc, char **argv) {
    CHECK(argc == 9);
    unsigned moved = 0;
    for (int model = 1; model < argc; model++) {
        void *module = ps_module_open(argv[model]);
        CHECK(module);
        void *symbol = ps_module_symbol(module, "ps_get_experiment");
        ps_experiment_entry entry;
        CHECK(symbol && sizeof entry == sizeof symbol);
        memcpy(&entry, &symbol, sizeof entry);
        const ps_experiment_api *api = entry();
        CHECK(api && api->abi_version == PS_ABI_VERSION);
        ps_context c = {0};
        c.struct_size = sizeof c;
        c.api_version = PS_API_VERSION;
        c.dt_s = 1.0 / 240;
        c.seed = 42;
        ps_rng_seed(&c.rng, c.seed);
        CHECK(api->create(&c) == PS_OK);
        uint32_t shape[128] = {0}, color[128] = {0}, position[128] = {0};
        bool seen[128] = {0};
        for (unsigned step = 0; step <= 480; step++) {
            ps_scene scene = {0};
            api->build_scene(&c, &scene);
            CHECK(ps_scene_valid(&scene) && scene.count);
            for (uint32_t i = 0; i < scene.count; i++) {
                const ps_object *o = &scene.objects[i];
                CHECK(o->id < 128);
                if (!o->id) {
                    CHECK(o->shape == PS_POINT || o->shape == PS_ARROW);
                    continue;
                }
                if (seen[o->id]) {
                    CHECK(shape[o->id] == o->shape && color[o->id] == o->color);
                    if (position[o->id] != i)
                        moved++;
                }
                shape[o->id] = o->shape;
                color[o->id] = o->color;
                position[o->id] = i;
                seen[o->id] = true;
            }
            if (step < 480) {
                CHECK(api->step(&c, c.dt_s) == PS_OK);
                c.time_s = (step + 1) * c.dt_s;
            }
        }
        printf("%s: 481 valid scenes with stable object IDs\n", api->name);
        api->destroy(&c);
        ps_module_close(module);
    }
    CHECK(moved > 0); /* Conditional sensor/path/contact entries really changed ordering. */
    puts("Conditional entries changed list positions without changing object identities");
    return 0;
}
