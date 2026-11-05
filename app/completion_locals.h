#ifndef PHYSIM_COMPLETION_LOCALS_H
#define PHYSIM_COMPLETION_LOCALS_H
#include "completion.h"
#include "physim/memory.h"
#include "language/editor_bindings.h"

typedef struct {
    ps_completion_symbol symbol;
    ps_lang_editor_binding declaration;
    size_t id;
    const char *type; /* Empty means no written annotation, not an inferred type. */
} ps_completion_local;
typedef struct {
    ps_allocator allocator;
    const void *owner;
    char *source, *strings;
    size_t bytes, cursor, string_bytes, count, slot_count;
    ps_completion_local *entries;
    size_t *slots;
    ps_lang_editor_status status;
    ps_lang_editor_call call;
    bool language;
} ps_completion_locals;

/* Zero-initialize (default allocator), or set a valid allocator before use.
 * Owns the exact immutable source and all candidate strings. No I/O/execution.
 * Same owner/bytes/cursor reuses the snapshot and does not reparse or allocate.
 * Invalid requests/allocation failure preserve the previous state. Valid source
 * syntax failure publishes an empty, explicitly classified new snapshot.
 * Physim uses its real parser; C currently returns no local candidates.
 * IDs are catalog-count + 1 + declaration byte offset, never catalog aliases. */
ps_result ps_completion_locals_update(ps_completion_locals *locals,const void *owner,
    const char *source,size_t bytes,size_t cursor,bool language);
void ps_completion_locals_destroy(ps_completion_locals *locals);
const ps_completion_local *ps_completion_locals_at(const ps_completion_locals *locals,size_t id);
const ps_completion_local *ps_completion_locals_find(const ps_completion_locals *locals,const char *name,size_t bytes);
/* Complete result count, writes at most capacity IDs; never truncates at five.
 * Local scope values apply to non-member Physim queries only. */
size_t ps_completion_locals_matches(const ps_completion_locals *locals,const ps_completion_query *query,
                                   size_t *ids,size_t capacity);
/* Cheap lexical scheduling hint only. The parser's call.at_start must confirm
 * the delayed request before an automatic empty-argument popup is opened. */
bool ps_completion_locals_argument_hint(const char *source,size_t bytes,size_t cursor,bool language);
#endif
