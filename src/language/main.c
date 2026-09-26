#include "emitter.h"
#include "loader.h"
#include "version.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { SOURCE_LIMIT = 1024 * 1024, NODE_LIMIT = 65536 };
static const char *usage =
    "Usage: physimc --check|--deps|--emit-c|--emit-experiment|--emit-analysis "
    "[--module-path directory ...] source.phys";

static int diagnostic(const char *path, ps_lang_token t) {
    fprintf(stderr, "%s:%zu:%zu: error: %s\n", path, t.line ? t.line : 1,
            t.column ? t.column : 1, t.error ? t.error : "Compiler error");
    return 1;
}
int main(int argc, char **argv) {
    if (argc == 2 && strcmp(argv[1], "--version") == 0) {
        puts("physimc " PS_COMPILER_VERSION_TEXT "; Physim language " PS_LANGUAGE_VERSION_TEXT
             " (C17 backend)");
        return 0;
    }
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        puts(usage);
        puts("Imports resolve beside the importing file, then through up to 16 module paths "
             "in order; total limit: 1 MiB source, 65536 AST slots, 128 modules.");
        return 0;
    }
    if (argc < 3 ||
        (strcmp(argv[1], "--check") != 0 && strcmp(argv[1], "--deps") != 0 &&
         strcmp(argv[1], "--emit-c") != 0 && strcmp(argv[1], "--emit-experiment") != 0 &&
         strcmp(argv[1], "--emit-analysis") != 0)) {
        fprintf(stderr, "%s\n", usage);
        return 2;
    }
    ps_lang_loader *l = calloc(1, sizeof(*l));
    ps_lang_semantic *info = NULL;
    int status = 2;
    if (!l)
        return 2;
    const char *source_path = argv[argc - 1];
    for (int arg = 2; arg < argc - 1; arg++) {
        if (strcmp(argv[arg], "--module-path") != 0 || ++arg >= argc - 1 ||
            !argv[arg][0] || l->module_path_count >= PS_MODULE_PATH_LIMIT) {
            fprintf(stderr, "%s\n", usage);
            goto cleanup;
        }
        l->module_paths[l->module_path_count++] = argv[arg];
    }
    if (source_path[0] == '-' && source_path[1] == '-') {
        fprintf(stderr, "%s\n", usage);
        goto cleanup;
    }
    l->source = malloc(SOURCE_LIMIT + 1u);
    l->nodes = calloc(NODE_LIMIT, sizeof(*l->nodes));
    info = calloc(NODE_LIMIT, sizeof(*info));
    l->count = 1;
    if (!l->source || !l->nodes || !info) {
        fprintf(stderr, "physimc: error: Cannot allocate compiler buffers\n");
        goto cleanup;
    }
    ps_lang_token origin = {PS_LANG_ERROR, 0, 0, 1, 1, "Cannot open source file", 0};
    size_t main_root = ps_lang_load(l, source_path, origin);
    if (!main_root) {
        size_t file = l->error.file;
        (void)diagnostic(file < l->files ? l->paths[file] : source_path, l->error);
        status = l->operational_error ? 2 : 1;
        goto cleanup;
    }
    if (strcmp(argv[1], "--deps") == 0) {
        for (size_t i = 0; i < l->files; i++)
            puts(l->paths[i]);
        status = ferror(stdout) ? 2 : 0;
        goto cleanup;
    }
    if (l->count >= NODE_LIMIT) {
        status = diagnostic(source_path, origin);
        goto cleanup;
    }
    size_t root = l->count++;
    l->nodes[root] = l->nodes[main_root];
    l->nodes[root].a = l->first;
    ps_lang_parse_result parsed = {root, l->count, {0}};
    ps_lang_check_result checked =
        ps_lang_check(l->source, l->source_size, l->nodes, parsed, info,
                      NODE_LIMIT, NODE_LIMIT);
    if (!checked.ok) {
        status = diagnostic(l->paths[checked.diagnostic.file], checked.diagnostic);
        goto cleanup;
    }
    parsed.count = checked.count;
    if (strcmp(argv[1], "--check") != 0) {
        const char *const *paths = (const char *const *)l->paths;
        ps_lang_check_result emitted =
            strcmp(argv[1], "--emit-analysis") == 0
                ? ps_lang_emit_analysis(stdout, source_path, paths, l->files, l->source,
                                        l->source_size, l->nodes, parsed, info)
            : strcmp(argv[1], "--emit-experiment") == 0
                ? ps_lang_emit_experiment(stdout, source_path, paths, l->files, l->source,
                                          l->source_size, l->nodes, parsed, info)
                : ps_lang_emit_c(stdout, source_path, paths, l->files, l->source, l->nodes,
                                 parsed, info);
        if (!emitted.ok) {
            status = diagnostic(l->paths[emitted.diagnostic.file], emitted.diagnostic);
            goto cleanup;
        }
    } else
        printf("%s: check passed\n", source_path);
    if (fflush(stdout) != 0) {
        fprintf(stderr, "physimc: error: Cannot flush output\n");
        goto cleanup;
    }
    status = 0;
cleanup:
    free(info);
    ps_lang_loader_destroy(l);
    free(l);
    return status;
}
