#include "physim/experiment.h"
#include "platform.h"
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "Language safe removal module line %d: %s\n", __LINE__, #condition); \
        return 1; \
    } \
} while (0)

int main(int argc, char **argv) {
    CHECK(argc == 2);
    void *module = ps_module_open(argv[1]);
    CHECK(module);
    void *symbol = ps_module_symbol(module, "ps_get_experiment");
    ps_experiment_entry entry = NULL;
    memcpy(&entry, &symbol, sizeof entry);
    CHECK(entry);
    const ps_experiment_api *api = entry();
    CHECK(api && api->create && api->reset && api->step && api->destroy);
    ps_context context = {0};
    context.struct_size = sizeof context;
    context.api_version = PS_API_VERSION;
    context.dt_s = 0.1;
    context.seed = 42;
    CHECK(api->create(&context) == PS_OK);
    CHECK(context.channel_count == 1 && !context.error[0]);
    CHECK(api->reset(&context) == PS_OK);
    CHECK(context.values[0] == 2);
    CHECK(api->step(&context, 0.1) == PS_OK && context.values[0] == 1);
    CHECK(api->step(&context, 0.1) == PS_OK && context.values[0] == 0);
    CHECK(api->step(&context, 0.1) == PS_OK && context.values[0] == -1);
    CHECK(!context.error[0]);
    CHECK(api->reset(&context) == PS_OK && context.values[0] == 2);
    CHECK(api->step(&context, 0.1) == PS_OK && context.values[0] == 1);
    ps_scene scene = {0};
    api->build_scene(&context, &scene);
    CHECK(ps_scene_valid(&scene) && scene.count == 1);
    CHECK(scene.objects[0].a.x == 1);
    api->destroy(&context);
    ps_module_close(module);
    return 0;
}
