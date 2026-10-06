#include "physim/analysis.h"
static ps_result run(const char *input,const char *prefix){(void)input;(void)prefix;return PS_INVALID;}
static ps_result diagnose(const char *const *inputs,size_t count,const char *prefix,ps_diagnostic *out) {
    (void)inputs;(void)count;(void)prefix;
    ps_result result=ps_diagnostic_set(out,PS_INVALID,"analyze","input",__FILE__,__LINE__,1,"Deliberate analysis error α");
    return result==PS_OK?PS_INVALID:result;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api={sizeof api,PS_ABI_VERSION,"Diagnostic analysis",run,NULL,diagnose};return &api;
}
