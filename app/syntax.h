#ifndef PHYSIM_SYNTAX_H
#define PHYSIM_SYNTAX_H
#include <stdbool.h>
#include <stddef.h>

typedef enum {
    PS_SYNTAX_TEXT, PS_SYNTAX_COMMENT, PS_SYNTAX_STRING, PS_SYNTAX_KEYWORD,
    PS_SYNTAX_TYPE, PS_SYNTAX_PREPROCESSOR, PS_SYNTAX_NUMBER,
    PS_SYNTAX_FUNCTION, PS_SYNTAX_VARIABLE, PS_SYNTAX_CONTROL, PS_SYNTAX_COUNT
} ps_syntax_role;
typedef struct {size_t begin,end;ps_syntax_role role;} ps_syntax_token;
/* An optional revision-owned declaration index overrides catalog/call syntax
 * for identifiers. TEXT means unknown. No scope or type inference is implied
 * by the lexical fallback; completion's scope provider remains independent. */
typedef ps_syntax_role (*ps_syntax_resolver)(void *user,size_t offset,size_t bytes);
typedef struct {
    const char *source;size_t bytes,offset,block_depth;
    bool language,line_comment,triple_string,line_start,directive,type_name;
    char quote;
    ps_syntax_resolver resolve;void *user;
} ps_syntax_scan;
/* Borrows exactly these immutable bytes. Transactional on invalid arguments.
 * Tokens partition all bytes, never straddle physical line breaks, and never
 * allocate. A CRLF is one TEXT token. Exhaustion preserves the output token. */
bool ps_syntax_init(ps_syntax_scan *scan,const char *source,size_t bytes,bool language);
bool ps_syntax_next(ps_syntax_scan *scan,ps_syntax_token *token);
ps_syntax_role ps_syntax_catalog_role(const char *name,size_t bytes,bool language);
#endif
