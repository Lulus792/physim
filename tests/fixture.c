#include "physim/experiment.h"
#include <stdlib.h>
#ifndef FIXTURE_MODE
#define FIXTURE_MODE 0
#endif
static ps_result create(ps_context *c) {
    ps_channel_add(c, "test", PS_METRE, "test");
    return PS_OK;
}
static ps_result step(ps_context *c, double dt) {
    (void)c;
    (void)dt;
#if FIXTURE_MODE == 1
    abort();
#elif FIXTURE_MODE == 2
    volatile int stay = 1;
    while (stay) {
    }
#endif
#if FIXTURE_MODE != 1
    return PS_OK;
#endif
}
static void scene(ps_context *c, ps_scene *s) {
    (void)c;
    (void)s;
}
static void destroy(ps_context *c) { (void)c; }
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api = {sizeof api, FIXTURE_MODE == 3 ? 2 : PS_ABI_VERSION,
                                          0,          "fixture",
                                          create,     create,
                                          step,       scene,
                                          destroy, NULL};
    return &api;
}
