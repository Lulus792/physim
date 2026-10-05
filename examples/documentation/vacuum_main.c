#include "physim/experiment.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

enum { X, Y, VX, VY, ENERGY, CHANNELS };
static const double gravity = 9.80665;
typedef struct {
    double x, y, vx, vy;
} vacuum_projectile;

static void measure(ps_context *context) {
    vacuum_projectile *body = context->user;
    context->values[X] = body->x;
    context->values[Y] = body->y;
    context->values[VX] = body->vx;
    context->values[VY] = body->vy;
    context->values[ENERGY] =
        0.5 * (body->vx * body->vx + body->vy * body->vy) + gravity * body->y;
}
static ps_result reset(ps_context *context) {
    vacuum_projectile *body = context->user;
    *body = (vacuum_projectile){-2, 0, 2, 5};
    measure(context);
    return PS_OK;
}
static ps_result create(ps_context *context) {
    vacuum_projectile *body = calloc(1, sizeof *body);
    if (!body)
        return PS_MEMORY;
    context->user = body;
    if (ps_channel_add(context, "position.x", PS_METRE, "Horizontal position") != X ||
        ps_channel_add(context, "position.y", PS_METRE, "Vertical position") != Y ||
        ps_channel_add(context, "velocity.x", PS_VELOCITY, "Horizontal velocity") != VX ||
        ps_channel_add(context, "velocity.y", PS_VELOCITY, "Vertical velocity") != VY ||
        ps_channel_add(context, "energy", PS_JOULE, "Mechanical energy") != ENERGY) {
        free(body);
        context->user = NULL;
        snprintf(context->error, sizeof context->error, "Could not register projectile channels");
        return PS_LIMIT;
    }
    snprintf(context->model_metadata, sizeof context->model_metadata,
             "model=projectile, vacuum\nmass_kg=1\ngravity_m_s2=9.80665\n"
             "integrator=exact constant acceleration");
    return reset(context);
}
static ps_result step(ps_context *context, double dt) {
    vacuum_projectile *body = context->user;
    body->x += body->vx * dt;
    body->y += body->vy * dt - 0.5 * gravity * dt * dt;
    body->vy -= gravity * dt;
    measure(context);
    return PS_OK;
}
static void scene(ps_context *context, ps_scene *out) {
    vacuum_projectile *body = context->user;
    ps_vec3 point = ps_v3(body->x, body->y, 0);
    ps_vec3 tip = ps_v3(body->x + 0.15 * body->vx, body->y + 0.15 * body->vy, 0);
    (void)ps_scene_add_id(out, 1, PS_SPHERE, point, point, 0.1, 0x53dec2ff);
    (void)ps_scene_add_id(out, 2, PS_LINE, point, tip, 0.01, 0xf2c572ff);
}
static void destroy(ps_context *context) {
    free(context->user);
    context->user = NULL;
}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api = {sizeof api, PS_ABI_VERSION, 0,
                                          "Vacuum projectile", create, reset, step, scene, destroy, NULL};
    return &api;
}
