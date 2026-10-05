#ifndef PHYSIM_LANGUAGE_EXPERIMENT_H
#define PHYSIM_LANGUAGE_EXPERIMENT_H
/* Included after generated state and callbacks. Not an independent public ABI. */
#ifdef PSRT_EXPERIMENT_ADAPTER
static ps_result psbridge_invoke(ps_context *c, ps_scene *scene, double dt, unsigned phase) {
    if (!c || !c->user)
        return PS_INVALID;
    ps_module_state *state = c->user;
    if (state->failed && phase != PSRT_RESET)
        return PS_NUMERIC;
    if (phase == PSRT_STEP && (!isfinite(dt) || dt <= 0))
        return PS_INVALID;
    state->host = (psrt_host){c, scene, phase};
    state->trap.previous = psrt_current;
    state->trap.error = c->error;
    state->trap.capacity = sizeof c->error;
    state->trap.depth = 0;
    c->error[0] = 0;
    psrt_current = &state->trap;
    if (setjmp(state->trap.jump)) {
        psrt_current = state->trap.previous;
        state->failed = true;
        if (scene)
            memset(scene, 0, sizeof *scene);
        return PS_NUMERIC;
    }
    if (phase == PSRT_CREATE) {
        ps_module_init(state);
        PSRT_FN_CREATE(state);
        state->host.phase = PSRT_RESET;
        ps_rng_seed(&c->rng, c->seed);
        PSRT_FN_RESET(state);
    } else if (phase == PSRT_RESET) {
        ps_rng_seed(&c->rng, c->seed);
        PSRT_FN_RESET(state);
    } else if (phase == PSRT_STEP)
        PSRT_FN_STEP(state, dt);
    else if (phase == PSRT_SCENE)
        PSRT_FN_SCENE(state);
    psrt_current = state->trap.previous;
    state->host.scene = NULL;
    state->failed = false;
    return PS_OK;
}
static ps_result psbridge_create(ps_context *c) {
    if (!c || c->struct_size < sizeof *c || c->api_version != PS_API_VERSION)
        return PS_VERSION;
    if (c->user || c->channel_count)
        return PS_INVALID;
    c->user = calloc(1, sizeof(ps_module_state));
    if (!c->user)
        return PS_MEMORY;
    snprintf(c->model_metadata, sizeof c->model_metadata,
             "language=physim-" PSRT_LANGUAGE_VERSION
             "\ncompiler=physimc-" PSRT_COMPILER_VERSION
             "\nbackend=C17\nsource_fnv1a64=%s", PSRT_SOURCE_HASH);
    ps_result result = psbridge_invoke(c, NULL, 0, PSRT_CREATE);
    if (result != PS_OK) {
#ifdef PSRT_HAS_ARRAYS
        ps_module_values_destroy(c->user);
#endif
        free(c->user);
        c->user = NULL;
        c->channel_count = 0;
        memset(c->channels, 0, sizeof c->channels);
        memset(c->values, 0, sizeof c->values);
        c->model_metadata[0] = 0;
    }
    return result;
}
static ps_result psbridge_reset(ps_context *c) { return psbridge_invoke(c, NULL, 0, PSRT_RESET); }
static ps_result psbridge_step(ps_context *c, double dt) {
    return psbridge_invoke(c, NULL, dt, PSRT_STEP);
}
static void psbridge_scene(ps_context *c, ps_scene *scene) {
    if (!scene)
        return;
    memset(scene, 0, sizeof *scene);
    (void)psbridge_invoke(c, scene, 0, PSRT_SCENE);
}
static void psbridge_destroy(ps_context *c) {
    if (c) {
#ifdef PSRT_HAS_ARRAYS
        if (c->user)
            ps_module_values_destroy(c->user);
#endif
        free(c->user);
        c->user = NULL;
    }
}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api = {sizeof api,           PS_ABI_VERSION,  PS_EXPERIMENT_SCENE_HIERARCHY,
                                          PSRT_EXPERIMENT_NAME, psbridge_create, psbridge_reset,
                                          psbridge_step,        psbridge_scene,  psbridge_destroy};
    return &api;
}
#endif
#endif
