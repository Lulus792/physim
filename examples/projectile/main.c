#include "physim/experiment.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
    double y[4];
    ps_vec3 trail[64];
    unsigned trail_count, ticks;
} projectile;
static void ode(double t, const double *y, double *d, void *u) {
    (void)t;
    (void)u;
    ps_vec3 drag = ps_drag_force(ps_v3(y[2], y[3], 0), PS_AIR, 0.47, 0.01);
    d[0] = y[2];
    d[1] = y[3];
    d[2] = drag.x;
    d[3] = -9.80665 + drag.y;
}
static void measure(ps_context *c) {
    projectile *p = c->user;
    for (int i = 0; i < 4; i++)
        c->values[i] = p->y[i];
    c->values[4] = 0.5 * (p->y[2] * p->y[2] + p->y[3] * p->y[3]) + 9.80665 * p->y[1];
}
static ps_result reset(ps_context *c) {
    projectile *p = c->user;
    p->y[0] = -2;
    p->y[1] = 0;
    p->y[2] = 2;
    p->y[3] = 5;
    p->trail_count = 1;
    p->ticks = 0;
    p->trail[0] = ps_v3(p->y[0], p->y[1], 0);
    measure(c);
    return PS_OK;
}
static ps_result create(ps_context *c) {
    c->user = calloc(1, sizeof(projectile));
    if (!c->user)
        return PS_MEMORY;
    ps_channel_add(c, "position.x", PS_METRE, "x");
    ps_channel_add(c, "position.y", PS_METRE, "y");
    ps_channel_add(c, "velocity.x", PS_VELOCITY, "vx");
    ps_channel_add(c, "velocity.y", PS_VELOCITY, "vy");
    ps_channel_add(c, "energy", PS_JOULE, "Mechanical energy");
    snprintf(c->model_metadata, sizeof c->model_metadata,
             "model=projectile, gravity and quadratic drag, no "
             "ground\nmass_kg=1\nmedium=air\ndensity_kg_m3=1.225\nCd=0.47\narea_m2=0."
             "01\nintegrator=RK4");
    return reset(c);
}
static ps_result step(ps_context *c, double dt) {
    projectile *p = c->user;
    ps_result r = ps_ode_step(PS_RK4, ode, NULL, c->time_s, dt, p->y, 4);
    measure(c);
    if (r == PS_OK && ++p->ticks % 10 == 0) {
        if (p->trail_count == 64) {
            memmove(p->trail, p->trail + 1, 63 * sizeof *p->trail);
            p->trail_count = 63;
        }
        p->trail[p->trail_count++] = ps_v3(p->y[0], p->y[1], 0);
    }
    return r;
}
static void scene(ps_context *c, ps_scene *s) {
    projectile *state = c->user;
    ps_vec3 p = ps_v3(c->values[0], c->values[1], 0);
    ps_scene_add_id(s, 1, PS_SPHERE, p, p, 0.1, 0x53dec2ff);
    ps_scene_add_id(s, 2, PS_ARROW, p,
                    ps_vadd(p, ps_v3(c->values[2] * 0.15, c->values[3] * 0.15, 0)), 0, 0xf2c572ff);
    if (state->trail_count >= 2)
        (void)ps_scene_polyline_id(s, 3, state->trail, state->trail_count, .008, 0x638aafff);
    ps_scene_add_id(s, 4, PS_POINT, ps_v3(-2, 0, 0), ps_v3(0, 0, 0), .03, 0xe2eaf2ff);
    (void)ps_scene_label_id(s, 5, p, "Wurfkoerper", 0xe2eaf2ff);
}
static void destroy(ps_context *c) { free(c->user); }
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api = {
        sizeof api, PS_ABI_VERSION, 0,      "Wurf mit Luftwiderstand", create, reset,
        step,       scene,          destroy};
    return &api;
}
