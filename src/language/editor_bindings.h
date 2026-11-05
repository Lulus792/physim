#ifndef PS_LANGUAGE_EDITOR_BINDINGS_H
#define PS_LANGUAGE_EDITOR_BINDINGS_H
#include "parser.h"

typedef enum {
    PS_LANG_EDITOR_VARIABLE, PS_LANG_EDITOR_PARAMETER, PS_LANG_EDITOR_ITERATOR,
    PS_LANG_EDITOR_PATTERN, PS_LANG_EDITOR_SELF
} ps_lang_editor_binding_kind;
typedef struct {
    ps_lang_token name; /* Exact declaration location, not the current use. */
    ps_lang_editor_binding_kind kind;
    size_t type_start, type_end; /* Written annotation; both zero if inferred. */
} ps_lang_editor_binding;
typedef enum {
    PS_LANG_EDITOR_OK, PS_LANG_EDITOR_PREFIX, PS_LANG_EDITOR_INVALID, PS_LANG_EDITOR_LIMIT
} ps_lang_editor_status;
typedef struct {
    size_t open, argument; /* Byte offset of '(', zero-based argument ordinal. */
    int active, at_start; /* Current grammar path is inside a call argument. */
} ps_lang_editor_call;
typedef struct {
    size_t count;
    ps_lang_editor_status status;
    ps_lang_token diagnostic;
    ps_lang_editor_call call; /* No callee/signature/type resolution claim. */
} ps_lang_editor_bindings_result;

/* Parse only the original [0,cursor) with the real Physim grammar. A blank
 * cursor line uses the next real token's indentation to retain the enclosing
 * block; lookahead never publishes declarations after the cursor. No I/O,
 * allocation, source rewriting, execution or type-inference claim. Caller owns
 * nodes/output; output spans borrow exactly the supplied source revision.
 * Complete declarations in active lexical scopes are returned in declaration
 * order, including shadowed outer bindings: reverse lookup chooses the nearest.
 * Self has its method declaration location and its written receiver-type span;
 * its actual spelling is "self", not source[name.offset..name.length].
 * A declaration is published after its newline/semicolon, never inside its own
 * initializer. Parameters/iterators enter their body scopes; payload patterns
 * also cover the arm guard, matching the compiler's lexical binding rule.
 * PREFIX means syntax is unfinished exactly at the cursor, not a valid AST.
 * Earlier syntax errors or exhausted buffers produce no candidate bindings.
 * The strict compiler parser and its failure contract remain independent. */
ps_lang_editor_bindings_result ps_lang_editor_bindings(
    const void *source, size_t size, size_t cursor, ps_lang_node *nodes, size_t node_capacity,
    ps_lang_editor_binding *bindings, size_t binding_capacity);
#endif
