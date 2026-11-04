#ifndef PHYSIM_COMPLETION_HELP_H
#define PHYSIM_COMPLETION_HELP_H
#include "completion.h"
#include "documentation.h"

/* One popup owns one parsed reference page. A selection change resolves a new
 * section in that page; hiding/closing destroys it. Never aliases the inspector
 * document. Symbols have the static completion-catalog lifetime. */
typedef struct {
    ps_document *document;
    const ps_completion_symbol *symbol;
    ps_doc_api_preview preview;
    size_t first,end;
    char path[256];
    ps_result status,page_status;
    bool attempted,guide;
} ps_completion_help;
typedef ps_result (*ps_completion_help_loader)(void *user,const char *relative,ps_document **out);
void ps_completion_help_destroy(ps_completion_help *help);
/* Zero-initialize before first use. Invalid arguments leave the state intact.
 * Valid failed selections bind the new identity and expose no previous section.
 * The loader supplies a separately owned, validated UTF-8 parsed document.
 * Same-page successes and failures are cached only for this popup lifetime. */
ps_result ps_completion_help_select(ps_completion_help *help,const ps_completion_symbol *symbol,
                                    ps_completion_help_loader load,void *user);
#endif
