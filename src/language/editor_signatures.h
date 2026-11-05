#ifndef PS_LANGUAGE_EDITOR_SIGNATURES_H
#define PS_LANGUAGE_EDITOR_SIGNATURES_H
#include "editor_bindings.h"

typedef struct {
    size_t name_start,name_end,type_start,type_end;
} ps_lang_editor_parameter;
typedef struct {
    size_t name_start,name_end,header_start,header_end,result_start,result_end;
    size_t parameter_first,parameter_count,scope_start,scope_end,scope_depth,visible_from;
    size_t owner_start,owner_end;
    int method,generic; /* 0 free/local, 1 instance, 2 mutating, 3 static. */
} ps_lang_editor_function;
typedef struct {
    ps_lang_editor_status status;
    size_t function_count,parameter_count;
    ps_lang_token diagnostic;
} ps_lang_editor_signatures_result;
/* Real parser productions publish complete written headers before their bodies.
 * No arbitrary partial AST is interpreted as valid. Original spans, no type or
 * overload resolution. Implicit self is not a call parameter; result span zero
 * means the language's omitted return annotation, not inferred return values.
 * Lexical declaration scope is [scope_start,scope_end); module depth=0 and end
 * is size. Local functions become visible at their header; module/method names
 * have visible_from=0. Owner names come from actual struct/enum productions.
 * OK/PREFIX (EOF-only body/expression error) permit the completed headers.
 * INVALID/resource exhaustion: counts=0, discard output. Capacity-only LIMIT
 * reports required counts; never consume a truncated result. Both capacities0
 * query counts only. Nodes need >=2 slots. Source max256KiB, no NUL, no I/O/heap. */
ps_lang_editor_signatures_result ps_lang_editor_signatures(
    const void *source,size_t size,ps_lang_node *nodes,size_t node_capacity,
    ps_lang_editor_function *functions,size_t function_capacity,
    ps_lang_editor_parameter *parameters,size_t parameter_capacity);
/* Document lookup for hoisted module functions/methods while another body is
 * being edited. Uses real productions and original lexer tokens to resume at
 * module declaration boundaries or the recorded struct/enum method indentation.
 * Only complete written headers are published; local functions are deliberately
 * excluded (use the cursor-prefix query above for their actual lexical scopes).
 * PREFIX means a syntax error was recovered anywhere, NOT a valid AST. Lexical
 * failure/resource exhaustion still discard all output. Same ownership/limits/
 * count-query/capacity contracts. Strict compiler and prefix APIs are unchanged. */
ps_lang_editor_signatures_result ps_lang_editor_module_signatures(
    const void *source,size_t size,ps_lang_node *nodes,size_t node_capacity,
    ps_lang_editor_function *functions,size_t function_capacity,
    ps_lang_editor_parameter *parameters,size_t parameter_capacity);
#endif
