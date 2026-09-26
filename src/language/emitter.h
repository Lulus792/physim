#ifndef PS_LANGUAGE_EMITTER_H
#define PS_LANGUAGE_EMITTER_H
#include "checker.h"
#include <stdio.h>

/* Requires the original source, successful parser AST and checker information.
 * Emits a standalone C17 program using physim/language_runtime.h. Does not close
 * output. Failed output is incomplete and must not be compiled. */
ps_lang_check_result ps_lang_emit_c(FILE *output, const char *source_path,
                                    const char *const *paths, size_t path_count, const void *source,
                                    const ps_lang_node *nodes, ps_lang_parse_result parsed,
                                    const ps_lang_semantic *info);
ps_lang_check_result ps_lang_emit_experiment(FILE *output, const char *source_path,
                                             const char *const *paths, size_t path_count,
                                             const void *source, size_t source_size,
                                             const ps_lang_node *nodes, ps_lang_parse_result parsed,
                                             const ps_lang_semantic *info);
ps_lang_check_result ps_lang_emit_analysis(FILE *output, const char *source_path,
                                           const char *const *paths, size_t path_count,
                                           const void *source, size_t source_size,
                                           const ps_lang_node *nodes, ps_lang_parse_result parsed,
                                           const ps_lang_semantic *info);
#endif
