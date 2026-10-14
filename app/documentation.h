#ifndef PHYSIM_DOCUMENTATION_H
#define PHYSIM_DOCUMENTATION_H
#include "physim/core.h"
#define PS_DOC_MAX_BLOCKS 4096u
#define PS_DOC_MAX_BYTES (256u * 1024u)
#define PS_DOC_MAX_CODE_BYTES (2u * PS_DOC_MAX_BYTES - 2u)
#define PS_DOC_MAX_SPANS 16384u
#define PS_DOC_MAX_CELLS 16384u
#define PS_DOC_MAX_READING_BYTES (PS_DOC_MAX_CODE_BYTES+2u*PS_DOC_MAX_BLOCKS)
#define PS_DOC_HISTORY_LIMIT 64u
typedef struct {
    int topic, track, match;
    unsigned scroll_x, scroll_y;
    bool contents;
    char query[128], filter[128];
} ps_doc_location;
typedef struct {
    ps_doc_location entries[PS_DOC_HISTORY_LIMIT];
    unsigned count, index;
} ps_doc_history;
/* UI-owned navigation state. A new visit discards the forward branch; when
 * full, only the oldest entry expires. Failed operations preserve the history. */
bool ps_doc_history_visit(ps_doc_history *history,const ps_doc_location *location);
bool ps_doc_history_store(ps_doc_history *history,const ps_doc_location *location);
const ps_doc_location *ps_doc_history_peek(const ps_doc_history *history,int direction);
bool ps_doc_history_move(ps_doc_history *history,int direction);
typedef enum {
    PS_DOC_TEXT,
    PS_DOC_HEADING,
    PS_DOC_CODE,
    PS_DOC_LINK,
    PS_DOC_RULE,
    PS_DOC_TABLE_ROW,
    PS_DOC_IMAGE
} ps_doc_kind;
typedef struct {
    ps_doc_kind kind;
    unsigned level;
    size_t text, length, target;
    size_t first_span, span_count, inline_parent;
    size_t reading_begin, reading_length;
    size_t first_cell, cell_count;
    size_t table_first, table_row; /* first row block and zero-based row; SIZE_MAX outside tables */
    float height;
    float code_line_height; /* renderer cache: native box-cell pitch, zero for ordinary text */
    float hit[4];
} ps_doc_block;
enum { PS_DOC_BOLD=1, PS_DOC_INLINE_CODE=2, PS_DOC_INLINE_LINK=4,
       PS_DOC_TABLE_SEPARATOR=8 };
/* Trimmed cell ranges in document.text, in row-major order. Delimiters are
 * recognized before inline decoration is removed, never in code/link labels.
 * Empty cells retain a zero-length range. Reading offsets are obtained from
 * their row's reading_begin + cell.text - row.text. */
typedef struct {size_t text,length;} ps_doc_cell;
typedef struct {
    size_t text, length, link;
    unsigned style;
} ps_doc_span;
typedef struct {
    size_t count, used, span_count, cell_count;
    float layout_width;
    ps_doc_block blocks[PS_DOC_MAX_BLOCKS];
    ps_doc_span spans[PS_DOC_MAX_SPANS];
    ps_doc_cell cells[PS_DOC_MAX_CELLS];
    char text[PS_DOC_MAX_BYTES * 2];
    size_t reading_length;
    char reading[PS_DOC_MAX_READING_BYTES+1];
} ps_document;
/* Small, inert Markdown reader for the bundled documentation. Caller validates
 * UTF-8. Images retain their alt text and inert target for the native renderer.
 * No HTML, scripts, network access or automatic URL navigation.
 * Transactional parse: out is unchanged on failure; free successful documents. */
ps_result ps_document_parse(const char *source, size_t size, bool code, ps_document **out);
const char *ps_document_find(const char *text, const char *query);
/* Resolve a Markdown heading fragment (optional leading #, percent-encoded UTF-8).
 * Lowercase heading IDs remove punctuation, turn spaces into hyphens and use
 * unique -1/-2 suffixes. Missing/invalid fragments preserve the output. */
ps_result ps_document_anchor(const ps_document *document,const char *fragment,size_t *block);
/* Locate API entries, including adjacent language overloads/type members.
 * C constants may live in a shared declaration. Failure preserves outputs. */
ps_result ps_document_api_section(const ps_document *document,const char *name,bool language,size_t *first,size_t *end);
#endif
