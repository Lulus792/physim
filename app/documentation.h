#ifndef PHYSIM_DOCUMENTATION_H
#define PHYSIM_DOCUMENTATION_H
#include "physim/core.h"
#define PS_DOC_MAX_BLOCKS 4096u
#define PS_DOC_MAX_BYTES (256u * 1024u)
typedef enum {
    PS_DOC_TEXT,
    PS_DOC_HEADING,
    PS_DOC_CODE,
    PS_DOC_LINK,
    PS_DOC_RULE,
    PS_DOC_TABLE_ROW
} ps_doc_kind;
typedef struct {
    ps_doc_kind kind;
    unsigned level;
    size_t text, length, target;
    float height;
} ps_doc_block;
typedef struct {
    size_t count, used;
    float layout_width;
    ps_doc_block blocks[PS_DOC_MAX_BLOCKS];
    char text[PS_DOC_MAX_BYTES * 2];
} ps_document;
/* Small, inert Markdown reader for the bundled documentation. Caller validates
 * UTF-8. No HTML, scripts, images, network access or automatic URL navigation.
 * Transactional parse: out is unchanged on failure; free successful documents. */
ps_result ps_document_parse(const char *source, size_t size, bool code, ps_document **out);
const char *ps_document_find(const char *text, const char *query);
#endif
