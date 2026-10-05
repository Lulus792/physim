#include "physim/experiment.h"
static ps_result create(ps_context *c) {
    ps_channel_add(c, "test", c->seed % 2 ? PS_SECOND : PS_METRE, "Changing unit");
    return PS_OK;
}
static ps_result step(ps_context *c, double dt) {
    (void)c;
    (void)dt;
    return PS_OK;
}
static void scene(ps_context *c, ps_scene *s) {
    (void)c;
    (void)s;
}
static void destroy(ps_context *c) { (void)c; }
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api = {
        sizeof api, PS_ABI_VERSION, 0, "schema fixture", create, create, step, scene, destroy, NULL};
    return &api;
}
