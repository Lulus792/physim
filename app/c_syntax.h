#ifndef PHYSIM_C_SYNTAX_H
#define PHYSIM_C_SYNTAX_H
#include "physim/memory.h"

typedef struct {
    const char *type, *field; /* Immutable compiled grammar strings, not tree pointers. */
    size_t start, end, line, column, parent;
    bool named, missing, error;
} ps_c_node;
typedef struct {size_t heap_bytes, progress_checks;} ps_c_limits;
typedef struct {
    ps_result status;
    size_t count, heap_peak, allocations, progress_checks;
    bool has_error;
} ps_c_syntax_result;

/* Actual C grammar, including recovery nodes; root may itself be ERROR.
 * has_error is NOT compiler acceptance.
 * No preprocessing, type checking, scope classification or source rewriting.
 * source may be NULL only for empty text; embedded NUL and >256KiB are rejected.
 * Output is preorder, parent SIZE_MAX for root, original byte spans, 1-based line/
 * byte-column. On failure count=0 and caller must discard partial output. Source and
 * output remain caller-owned; no library object/pointer escapes. No I/O/workers.
 * Zero allocator chooses default; zero limits choose 32MiB/10000 checks. Limits
 * count all private allocations (including tracking overhead), not caller output.
 * Per-thread private allocation boundary converts any allocation failure to
 * PS_MEMORY, budget/cancellation/capacity exhaustion to PS_LIMIT; never returns
 * NULL into Tree-sitter or changes its global allocator. Reentrant/parallel calls
 * use distinct allocation domains. Allocator callbacks follow memory.h.
 * Progress checks bound parser operations, NOT a measured wall-clock deadline. */
ps_c_syntax_result ps_c_syntax(const char *source,size_t bytes,ps_c_node *nodes,size_t capacity,
                             ps_allocator allocator,ps_c_limits limits);
#endif
