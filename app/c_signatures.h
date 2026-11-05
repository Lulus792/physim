#ifndef PHYSIM_C_SIGNATURES_H
#define PHYSIM_C_SIGNATURES_H
#include "c_bindings.h"

typedef struct {
    ps_c_binding declaration; /* Written typed declaration; name may be absent. */
    size_t start,end; /* Actual parameter entry in the original header. */
    bool type_missing; /* Old-style identifier without an unambiguous written type. */
} ps_c_parameter;

typedef struct {
    ps_c_binding declaration;
    size_t parameters_start,parameters_end,parameter_first,parameter_count;
    size_t variadic_start,variadic_end;
    size_t scope_start,scope_end,scope_depth,visible_from;
    bool pointer,definition,unspecified,old_style,scope_open;
} ps_c_function;

typedef struct {
    ps_result status;
    size_t function_count,parameter_count;
    bool has_error;
} ps_c_signatures_result;

/* Projection of actual complete written C declarators from the immutable source
 * snapshot. No new parse, preprocessing, alias/type/value/include resolution or
 * worker. Functions, prototypes, one-level function-pointer objects and callable
 * definition parameters; no fields, nested prototype parameters or array/double-
 * pointer objects treated as directly callable. Returned names/types/headers are
 * original source spans, including callback/return declarator structure. Unnamed
 * parameters remain unnamed. A sole plain void means zero; an empty prototype is
 * unspecified, not zero expected arguments. Ellipsis is separate from fixed count.
 * Old-style types come only from actual matching declarations; missing/duplicate
 * annotations remain type_missing (never invented implicit int).
 * Scope [start,end), with end inclusive only when scope_open, including module
 * EOF or an unfinished scope. visible_from is also required for C declaration
 * order; callback parameters are visible only in the actual definition body.
 * Typed old-style declarations are not separately published as file-scope names.
 * All counts are complete; both capacities0 query counts. Output-only PS_LIMIT
 * reports required counts: discard truncated output. Other failures count0 and
 * discard output. Scratch is private and freed on every path. The source remains
 * unchanged/reusable after failure. Zero allocator selects default. */
ps_c_signatures_result ps_c_bindings_source_signatures(
    const ps_c_bindings_source *source,ps_c_function *functions,size_t function_capacity,
    ps_c_parameter *parameters,size_t parameter_capacity,ps_allocator allocator);

typedef enum { PS_C_NAME_NONE,PS_C_NAME_DECLARATION,PS_C_NAME_TYPEDEF,PS_C_NAME_ENUMERATOR,PS_C_NAME_PARAMETER } ps_c_name_kind;
typedef struct {
    ps_result status;
    bool has_error,found,callable;
    ps_c_name_kind kind;
    ps_c_binding declaration;
    ps_c_function function; /* Valid only when callable: actual written header. */
    size_t parameter_count;
} ps_c_lookup_result;
/* Exact ordinary-identifier lookup in the original snapshot at cursor. Nearest
 * active declaration wins, including typedefs/enumerators and a completed object
 * declarator inside its unfinished initializer. This is intentionally distinct
 * from completed variable suggestions. Tags/fields/labels are other namespaces;
 * no preprocessing, alias inference, external includes or overload resolution.
 * Unresolved ordinary objects still set found and prevent API-name fallback.
 * callable means a complete directly written function/function-pointer header,
 * not proof that an opaque object/typedef cannot be called. Parameter entries
 * follow the signature API. NULL/0 queries the complete required count. Capacity
 * PS_LIMIT returns only parameter_count/has_error; every other failure clears
 * usable results/counts. Scratch is private/freed; immutable source unchanged. */
ps_c_lookup_result ps_c_bindings_source_lookup(const ps_c_bindings_source *source,
    size_t cursor,const char *name,size_t name_bytes,
    ps_c_parameter *parameters,size_t parameter_capacity,ps_allocator allocator);
#endif
