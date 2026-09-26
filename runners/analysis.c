#include "physim/analysis.h"
#include "platform.h"
#include <string.h>
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
int main(int argc, char **argv) {
    ps_binary_stdio();
    if (argc == 4 && !strcmp(argv[1], "--csv")) {
        ps_result r = ps_run_export_csv(argv[2], argv[3]);
        if (r != PS_OK)
            fprintf(stderr, "%s\n", ps_result_string(r));
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
                return 2;
            }
    void *module = ps_module_open(argv[1]);
    if (!module) {
        fprintf(stderr, "Cannot load analysis module\n");
        return 3;
    }
    ps_analysis_entry entry = NULL;
    void *symbol = ps_module_symbol(module, "ps_get_analysis");
    memcpy(&entry, &symbol, sizeof entry);
    const ps_analysis_api *api = entry ? entry() : NULL;
    if (!api || api->struct_size < PS_ANALYSIS_API_BASE_SIZE ||
        api->abi_version != PS_ABI_VERSION || !api->run) {
        fprintf(stderr, "Analysis ABI mismatch\n");
        ps_module_close(module);
        return 4;
    }
    if (count != 1 && (api->struct_size < sizeof *api || !api->run_many)) {
        fprintf(
            stderr,
            "This analysis module supports one run only. Implement run_many for zero or multiple runs.\n");
        ps_module_close(module);
        return 6;
    }
    if (!manifest(argv[1], inputs, count, prefix)) {
        fprintf(stderr, "Cannot create input manifest or read an input file\n");
        ps_module_close(module);
        return 7;
    }
    ps_result result = count != 1 ? api->run_many(count ? inputs : NULL, count, prefix)
                                  : api->run(inputs[0], prefix);
    printf("Analysis: %s\n", ps_result_string(result));
    ps_module_close(module);
    return result == PS_OK || result == PS_RECOVERED ? 0 : 5;
}
