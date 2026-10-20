#ifndef PHYSIM_SOURCE_OUTLINE_H
#define PHYSIM_SOURCE_OUTLINE_H
#include <stdbool.h>
#include <stddef.h>

enum { PS_OUTLINE_LIMIT=512,PS_OUTLINE_SOURCE_LIMIT=1024*1024 };
typedef enum { PS_OUTLINE_FUNCTION,PS_OUTLINE_BINDING,PS_OUTLINE_TYPE } ps_outline_kind;
typedef struct {size_t offset,length,line;ps_outline_kind kind;} ps_outline_symbol;
typedef enum { PS_OUTLINE_OK,PS_OUTLINE_INCOMPLETE,PS_OUTLINE_LIMITED,PS_OUTLINE_NO_MEMORY,PS_OUTLINE_INVALID } ps_outline_status;
typedef struct {
    ps_outline_symbol symbols[PS_OUTLINE_LIMIT];
    size_t count,diagnostic_line;
    ps_outline_status status;
} ps_source_outline;
/* Borrows length-delimited UTF-8 source for this call only; symbols are byte
 * ranges into exactly that source. No I/O, preprocessing or semantic inference.
 * Physim uses its compiler AST (invalid source yields no stale symbols).
 * C indexes written file-scope declarations; macro expansion/conditional branch
 * selection and nonstandard declarator extensions require a compiler index.
 * Partial/limited results always have an explicit non-OK status. */
void ps_source_outline_build(const char *source,size_t bytes,bool language,ps_source_outline *out);
#endif
