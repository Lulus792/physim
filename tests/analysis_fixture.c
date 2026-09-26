#include "physim/analysis.h"
#include "physim/experiment.h"
#include <stdlib.h>
#include <string.h>
static ps_result run(const char *input, const char *prefix) {
    (void)input;
    (void)prefix;
#if FIXTURE_MODE == 3
    abort();
#elif FIXTURE_MODE == 4
    volatile int stay = 1;
    while (stay) {
    }
#endif
#if FIXTURE_MODE != 3
    return PS_OK;
#endif
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
#if FIXTURE_MODE == 1
    /* Current ABI with the optional run_many tail omitted. */
    typedef struct {
        uint32_t struct_size, abi_version;
        const char *name;
        ps_result (*run)(const char *, const char *);
    } old_api;
    static const old_api api = {sizeof(old_api), PS_ABI_VERSION, "Legacy", run};
    return (const ps_analysis_api *)&api;
#else
    static const ps_analysis_api api = {
        sizeof(ps_analysis_api), FIXTURE_MODE == 2 ? 2 : PS_ABI_VERSION, "Fixture", run, NULL};
    return &api;
#endif
}
