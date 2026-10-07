#include "physim/analysis.h"
#include "platform.h"
#include "batch.h"
#include <string.h>
typedef struct {char runner[4096];} analysis_services_state;
static bool host_batch_continue(uint32_t completed,uint32_t active,void *user) {
    (void)active;return completed<*(const uint32_t *)user;
}
static ps_result host_batch(void *user,const ps_batch_options *request,uint32_t stop_after,ps_batch_result *result) {
    analysis_services_state *state=user;
    if(!request || !result || !state->runner[0])return PS_INVALID;
    ps_batch_options options=*request;
    snprintf(options.runner,sizeof options.runner,"%s",state->runner);
    if(stop_after>=options.runs)return PS_INVALID;
    return ps_batch_run(&options,stop_after?host_batch_continue:NULL,&stop_after,result);
}
static ps_result host_resume(void *user,const char *series,const char *directory,ps_batch_options *options) {
    analysis_services_state *state=user;
    if(!state->runner[0])return PS_IO;
    return ps_batch_resume_load(series,state->runner,directory,options);
}
static void host_services_init(analysis_services_state *state) {
    char executable[4096];
    if(!ps_executable_path(executable,sizeof executable))return;
    char *slash=NULL;
    for(char *p=executable;*p;p++)if(*p=='/' || *p=='\\')slash=p;
    if(!slash)return;
    slash[1]=0;
#ifdef _WIN32
    const char *name="physim-runner.exe";
#else
    const char *name="physim-runner";
#endif
    int n=snprintf(state->runner,sizeof state->runner,"%s%s",executable,name);
    if(n<0 || (size_t)n>=sizeof state->runner)state->runner[0]=0;
}
static void csv_text(FILE *f, const char *text) {
    fputc('"', f);
    for (; *text; text++) {
        if (*text == '"')
            fputc('"', f);
        fputc(*text, f);
    }
    fputc('"', f);
}
static bool identity(FILE *out, const char *role, size_t index, const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f)
        return false;
    uint64_t hash = UINT64_C(14695981039346656037), size = 0;
    unsigned char block[65536];
    size_t n;
    while ((n = fread(block, 1, sizeof block, f)) != 0) {
        if (size > UINT64_MAX - n) {
            fclose(f);
            return false;
        }
        size += n;
        for (size_t i = 0; i < n; i++) {
            hash ^= block[i];
            hash *= UINT64_C(1099511628211);
        }
    }
    bool ok = !ferror(f);
    fclose(f);
    if (!ok)
        return false;
    fprintf(out, "%s,%zu,", role, index);
    csv_text(out, path);
    fprintf(out, ",%llu,%016llx\n", (unsigned long long)size, (unsigned long long)hash);
    return !ferror(out);
}
static bool manifest(const char *module, const char *const *runs, size_t count,
                     const char *prefix) {
    char path[4096];
    int n = snprintf(path, sizeof path, "%s.inputs.csv", prefix);
    if (n < 0 || (size_t)n >= sizeof path)
        return false;
    FILE *f = fopen(path, "wbx");
    if (!f)
        return false;
    fputs("role,index,path,bytes,fnv1a64\n", f);
    bool ok = identity(f, "module", 0, module);
    for (size_t i = 0; i < count && ok; i++)
        ok = identity(f, "input", i + 1, runs[i]);
    if (fclose(f))
        ok = false;
    return ok;
}
static void analysis_diagnostic(const char *prefix,ps_result code,const char *operation,
                                const char *message,const ps_diagnostic *provided) {
    ps_diagnostic record;
    if(provided && ps_diagnostic_valid(provided) && provided->code==code)record=*provided;
    else if(ps_diagnostic_set(&record,code,operation,NULL,NULL,0,0,message)!=PS_OK)return;
    char path[4096];int n=snprintf(path,sizeof path,"%s.psdiag",prefix);
    if(n>0 && (size_t)n<sizeof path)(void)ps_diagnostic_save(path,&record);
}
int main(int argc, char **argv) {
    ps_binary_stdio();
    if (argc == 4 && !strcmp(argv[1], "--csv")) {
        ps_result r = ps_run_export_csv(argv[2], argv[3]);
        if (r != PS_OK && r != PS_RECOVERED) {
            fprintf(stderr, "%s\n", ps_result_string(r));
            analysis_diagnostic(argv[3],r,"export.csv",ps_result_string(r),NULL);
        }
        return r == PS_OK || r == PS_RECOVERED ? 0 : 1;
    }
    bool many = argc >= 4 && !strcmp(argv[2], "--runs");
    size_t count = many ? (size_t)argc - 4 : 1;
    if ((!many && argc != 4) || count > PS_ANALYSIS_MAX_INPUTS) {
        fprintf(stderr, "Usage: physim-analysis-runner module run.psrun output-prefix\n       "
                        "physim-analysis-runner module --runs output-prefix [run1.psrun ...]\n       "
                        "physim-analysis-runner --csv run.psrun output.csv\n");
        return 2;
    }
    const char *single[] = {argv[2]};
    const char *inputs[PS_ANALYSIS_MAX_INPUTS];
    for (size_t i = 0; i < count; i++)
        inputs[i] = many ? argv[i + 4] : single[0];
    const char *prefix = argv[3];
    for (size_t i = 0; i < count; i++)
        for (size_t j = 0; j < i; j++)
            if (!strcmp(inputs[i], inputs[j])) {
                fprintf(stderr, "Duplicate input run\n");
                analysis_diagnostic(prefix,PS_INVALID,"inputs","Duplicate input run",NULL);
                return 2;
            }
    void *module = ps_module_open(argv[1]);
    if (!module) {
        fprintf(stderr, "Cannot load analysis module\n");
        analysis_diagnostic(prefix,PS_IO,"module.load","Cannot load analysis module",NULL);
        return 3;
    }
    ps_analysis_entry entry = NULL;
    void *symbol = ps_module_symbol(module, "ps_get_analysis");
    memcpy(&entry, &symbol, sizeof entry);
    const ps_analysis_api *api = entry ? entry() : NULL;
    if (!api || api->struct_size < PS_ANALYSIS_API_BASE_SIZE ||
        api->abi_version != PS_ABI_VERSION || !api->run) {
        fprintf(stderr, "Analysis ABI mismatch\n");
        analysis_diagnostic(prefix,PS_VERSION,"module.abi","Analysis ABI mismatch",NULL);
        ps_module_close(module);
        return 4;
    }
    bool hosted=api->struct_size>=sizeof *api && api->run_host;
    bool typed=api->struct_size>=PS_ANALYSIS_API_DIAGNOSTIC_SIZE && api->run_diagnostic;
    if (count != 1 && !hosted && !typed && (api->struct_size < PS_ANALYSIS_API_MANY_SIZE || !api->run_many)) {
        fprintf(
            stderr,
            "This analysis module supports one run only. Implement run_many for zero or multiple runs.\n");
        analysis_diagnostic(prefix,PS_VERSION,"run_many","Analysis module supports one input only",NULL);
        ps_module_close(module);
        return 6;
    }
    if (!manifest(argv[1], inputs, count, prefix)) {
        fprintf(stderr, "Cannot create input manifest or read an input file\n");
        analysis_diagnostic(prefix,PS_IO,"manifest","Cannot create input manifest or read an input file",NULL);
        ps_module_close(module);
        return 7;
    }
    ps_diagnostic record;ps_diagnostic_clear(&record);
    analysis_services_state service_state={0};host_services_init(&service_state);
    const ps_analysis_services services={sizeof services,PS_ANALYSIS_SERVICES_VERSION,&service_state,host_batch,host_resume};
    ps_result result = hosted?api->run_host(count?inputs:NULL,count,prefix,&services,&record):
        typed?api->run_diagnostic(count?inputs:NULL,count,prefix,&record):
        count != 1 ? api->run_many(count ? inputs : NULL, count, prefix):api->run(inputs[0],prefix);
    if(result!=PS_OK && result!=PS_RECOVERED)
        analysis_diagnostic(prefix,result,"analyze",ps_result_string(result),&record);
    printf("Analysis: %s\n", ps_result_string(result));
    ps_module_close(module);
    return result == PS_OK || result == PS_RECOVERED ? 0 : 5;
}
