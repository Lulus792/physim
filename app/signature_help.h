#ifndef PHYSIM_SIGNATURE_HELP_H
#define PHYSIM_SIGNATURE_HELP_H
#include "completion_locals.h"
#include "language/editor_signatures.h"

typedef struct {
    size_t begin,end,name_begin,name_end; /* UTF-8 byte spans in display->text. */
    bool type_missing;
} ps_signature_parameter;
typedef struct ps_signature_display {
    char *text;
    ps_signature_parameter *parameters;
    size_t text_bytes,count,name_begin,name_end,allocation_bytes;
    size_t variadic_begin,variadic_end;
    bool unspecified;
} ps_signature_display;
typedef enum { PS_SIGNATURE_NONE,PS_SIGNATURE_UNKNOWN,PS_SIGNATURE_MASKED,
               PS_SIGNATURE_AMBIGUOUS,PS_SIGNATURE_READY,PS_SIGNATURE_INVALID } ps_signature_state;
typedef struct {
    ps_allocator allocator;
    const void *owner;
    char *source;
    size_t bytes,cursor;
    bool language,module_ready;
    ps_lang_editor_call call;
    ps_lang_editor_function *functions;
    ps_lang_editor_parameter *parameters;
    size_t function_count,parameter_count;
    ps_signature_display *display;
    const ps_completion_symbol *api_symbol; /* Static catalog lifetime only. */
    size_t declaration,active_parameter;
    ps_signature_state state;
} ps_signature_help;
/* Zero-init/default allocator, or supply an allocator. Consumes an exact current
 * locals snapshot, including its actual call path. Owns source/module metadata/
 * display strings; borrows locals.c_source only during a query. No I/O/worker.
 * Same exact owner/language/source/cursor reuses state without allocation.
 * Same document with a different cursor reuses owned module headers. Failure
 * preserves old state: caller must compare identity to current input and hide a
 * stale display. No user initializer, include, value/type/overload inference.
 * API fallback is forbidden for an actually shadowing declaration. Unknown or
 * ambiguous targets publish no display. active_parameter is SIZE_MAX for excess
 * arguments, unknown C prototypes, and variadic tail; never clamps to the last.
 * Source/module limits follow existing editor APIs; display limit1MiB. */
ps_result ps_signature_help_update(ps_signature_help *help,const ps_completion_locals *locals);
void ps_signature_help_destroy(ps_signature_help *help);
bool ps_signature_help_matches(const ps_signature_help *help,const ps_completion_locals *locals);
#endif
