#ifndef PHYSIM_LANGUAGE_ANALYSIS_H
#define PHYSIM_LANGUAGE_ANALYSIS_H
#ifdef PSRT_FN_ANALYZE
static ps_result psra_run_diagnostic(const char *const *inputs, size_t count, const char *prefix,ps_diagnostic *diagnostic) {
    if(diagnostic)ps_diagnostic_clear(diagnostic);
    if ((!inputs && count) || count > PS_ANALYSIS_MAX_INPUTS || !prefix || !*prefix)
        return PS_INVALID;
    for (size_t i = 0; i < count; i++)
        if (!inputs[i] || !*inputs[i])
            return PS_INVALID;
    ps_module_state *state = calloc(1, sizeof *state);
    if (!state)
        return PS_MEMORY;
    const char *provenance =
        "language=physim-" PSRT_LANGUAGE_VERSION
        "\ncompiler=physimc-" PSRT_COMPILER_VERSION
        "\nbackend=C17\nsource_fnv1a64=" PSRT_SOURCE_HASH;
    state->host.inputs = inputs;
    state->host.count = count;
    state->host.prefix = prefix;
    state->host.provenance = provenance;
    state->host.result = PS_NUMERIC;
    ps_result result = ps_analysis_create(prefix, 0, &state->host.context);
    if (result != PS_OK) {
        free(state);
        return result;
    }
    state->trap.previous = psrt_current;
    state->trap.error = state->error;
    state->trap.capacity = sizeof state->error;
    state->trap.diagnostic=diagnostic;state->trap.result=&state->host.result;
    state->trap.operation="analyze";state->trap.argument=NULL;state->trap.failure_code=PS_NUMERIC;
    psrt_current = &state->trap;
    if (setjmp(state->trap.jump)) {
        fprintf(stderr, "%s\n", state->error);
        result = state->trap.failure_code;
    } else {
        ps_module_init(state);
        PSRT_FN_ANALYZE(state);
        if (!state->host.report)
            psrt_fail(PSRT_AT(1, 1), "Analysis must create a report");
        char path[4096];
        int n = snprintf(path, sizeof path, "%s.psreport", prefix);
        if (n < 0 || (size_t)n >= sizeof path)
            psra_check(&state->host, PS_LIMIT, PSRT_AT(1, 1));
        psra_check(&state->host, ps_report_save(state->host.report, path), PSRT_AT(1, 1));
        result = state->host.recovered ? PS_RECOVERED : PS_OK;
    }
    psrt_current = state->trap.previous;
#ifdef PSRT_HAS_ARRAYS
    ps_module_values_destroy(state);
#endif
    ps_report_destroy(state->host.report);
    ps_analysis_destroy(state->host.context);
    free(state);
    return result;
}
static ps_result psra_run_many(const char *const *inputs,size_t count,const char *prefix) {
    return psra_run_diagnostic(inputs,count,prefix,NULL);
}
static ps_result psra_run(const char *input, const char *prefix) {
    return psra_run_many(&input, 1, prefix);
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api = {sizeof api, PS_ABI_VERSION, PSRT_SOURCE, psra_run,
                                        psra_run_many,psra_run_diagnostic};
    return &api;
}
#endif
#endif
