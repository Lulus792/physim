#ifndef PHYSIM_C_BINDINGS_H
#define PHYSIM_C_BINDINGS_H
#include "c_syntax.h"

typedef struct {
    size_t name_start,name_end,line,column;
    size_t type_start,type_end,declarator_start,declarator_end;
    bool parameter;
} ps_c_binding;
typedef struct {
    ps_result status;
    size_t count;
    bool has_error;
} ps_c_bindings_result;

/* Original full-source C grammar, never rewritten or preprocessed. Complete
 * object declarators and definition parameters in scopes active at cursor,
 * including actual ERROR recovery nodes. Written base/declarator spans only;
 * no include/type/value resolution. Declaration order, outer shadowed names
 * retained for the caller's nearest-name merge. Own unfinished initializer,
 * prototypes, fields, nested callback parameters and future/closed scopes excluded.
 * Names/types borrow the exact caller-owned source. Private scratch and parser
 * objects are freed before returning; no IO or workers. On failure count=0,
 * discard partial output. Allocator zero selects default, source max256KiB,
 * max65536 nodes and default c_syntax budgets. Cursor must be a UTF-8 boundary. */
ps_c_bindings_result ps_c_bindings(const char *source,size_t bytes,size_t cursor,
    ps_c_binding *bindings,size_t capacity,ps_allocator allocator);

/* Required bytes INCLUDING NUL, or zero for invalid spans/arguments. At most
 * capacity bytes written, always terminated if capacity>0. Original written
 * specifier/declarator text, with the declared identifier removed; no inference.
 * Caller detects insufficient capacity from return value > capacity. */
size_t ps_c_binding_type(const char *source,size_t bytes,ps_c_binding binding,
                         char *output,size_t capacity);
#endif
