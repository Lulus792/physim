#include "physim/analysis.h"
#ifdef _WIN32
#define PS_EXPORT __declspec(dllexport)
#else
#define PS_EXPORT __attribute__((visibility("default")))
#endif
/* Frozen ABI-3 descriptor through run_many; no diagnostic callback/tail. */
typedef struct {
    uint32_t struct_size,abi_version;
    const char *name;
    ps_result (*run)(const char *,const char *);
    ps_result (*run_many)(const char *const *,size_t,const char *);
} legacy_api;
static ps_result one(const char *input,const char *prefix){(void)input;(void)prefix;return PS_OK;}
static ps_result many(const char *const *inputs,size_t count,const char *prefix){(void)inputs;(void)count;(void)prefix;return PS_INVALID;}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void){static const legacy_api api={sizeof api,3,"Legacy analysis",one,many};return (const ps_analysis_api *)&api;}
