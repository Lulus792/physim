#ifndef PHYSIM_APP_COMPLETION_H
#define PHYSIM_APP_COMPLETION_H
#include "physim/core.h"
typedef enum { PS_COMPLETION_FUNCTION,PS_COMPLETION_TYPE,PS_COMPLETION_VALUE } ps_completion_kind;
typedef struct {
    const char *name,*module,*signature;
    ps_completion_kind kind;
    bool language,instance;
} ps_completion_symbol;
typedef struct {
    size_t start,end;
    char prefix[128],receiver[128];
    bool language,member;
} ps_completion_query;
/* Byte offsets, UTF-8 boundary required. Comments/strings and unsupported C
 * members produce PS_INVALID. Every failure preserves the output query. */
ps_result ps_completion_prepare(const char *text,size_t bytes,size_t cursor,bool language,ps_completion_query *out);
size_t ps_completion_count(void);
const ps_completion_symbol *ps_completion_at(size_t index);
/* Returns all matches; writes at most capacity indices. No allocation. */
size_t ps_completion_matches(const ps_completion_query *query,size_t *indices,size_t capacity);
/* Member completion inserts the leaf name, preserving an existing receiver. */
const char *ps_completion_insertion(const ps_completion_query *query,const ps_completion_symbol *symbol);
#endif
