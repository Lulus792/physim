#include "physim/analysis.h"
#include <stdio.h>
static ps_result legacy(const char *input,const char *prefix){(void)input;(void)prefix;return PS_VERSION;}
static ps_result diagnostic(const char *const *inputs,size_t count,const char *prefix,ps_diagnostic *out){
    (void)inputs;ps_diagnostic_clear(out);if(count)return PS_INVALID;
    FILE *f=fopen(prefix,"wbx");if(!f)return PS_IO;
    int result=fputs("old diagnostic tail selected\n",f),closed=fclose(f);return result>=0 && !closed?PS_OK:PS_IO;
}
static ps_result forbidden(const char *const *inputs,size_t count,const char *prefix,const ps_analysis_services *services,ps_diagnostic *out){
    (void)inputs;(void)count;(void)prefix;(void)services;(void)out;return PS_VERSION;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void){
    static const ps_analysis_api api={.struct_size=PS_ANALYSIS_API_DIAGNOSTIC_SIZE,.abi_version=PS_ABI_VERSION,
        .name="old ABI-3 diagnostic tail",.run=legacy,.run_diagnostic=diagnostic,.run_host=forbidden};return &api;
}
