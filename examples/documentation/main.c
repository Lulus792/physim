#include "physim/experiment.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    double position, velocity;
    int channel;
} motion;

static ps_result reset(ps_context *c) {
    motion *m = c->user;
    m->position = 0;
    m->velocity = 1.5;
    c->values[m->channel] = m->position;
    return PS_OK;
}
static ps_result create(ps_context *c) {
    motion *m = calloc(1, sizeof *m);
    if (!m)
        return PS_MEMORY;
    c->user = m;
    m->channel = ps_channel_add(c, "position.x", PS_METRE, "Position along X");
    if (m->channel < 0) {
        snprintf(c->error, sizeof c->error, "Could not register position.x");
        return PS_LIMIT;
    }
    snprintf(c->model_metadata, sizeof c->model_metadata,
             "model=uniform motion\nvelocity_m_s=1.5\nintegrator=exact");
    return reset(c);
}
static ps_result step(ps_context *c, double dt) {
    motion *m = c->user;
    m->position += m->velocity * dt;
    c->values[m->channel] = m->position;
    return PS_OK;
}
static void scene(ps_context *c, ps_scene *s) {
    motion *m = c->user;
    (void)ps_scene_add_id(s, 1, PS_SPHERE, ps_v3(m->position, 0, 0), ps_v3(0, 0, 0), 0.15,
                          0x64aaffff);
}
static void destroy(ps_context *c) {
    free(c->user);
    c->user = NULL;
}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api = {sizeof(ps_experiment_api),
                                          PS_ABI_VERSION,
                                          0,
                                          "Uniform motion",
                                          create,
                                          reset,
                                          step,
                                          scene,
                                          destroy};
    return &api;
}
