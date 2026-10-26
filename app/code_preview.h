#ifndef PS_CODE_PREVIEW_H
#define PS_CODE_PREVIEW_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
typedef float (*ps_code_measure)(void *user,const char *text,size_t bytes);
typedef struct {size_t begin,end;float width;bool hyphen;} ps_code_preview_line;
typedef struct {
    const char *source;size_t length,count,capacity;
    ps_code_preview_line *lines;
    float width,maximum,tab;
} ps_code_preview;
/* Source is borrowed immutable UTF-8. On failure *output is unchanged.
 * Lines retain original byte spans, including preserved hanging whitespace. */
bool ps_code_preview_build(const char *source,size_t bytes,float width,ps_code_measure measure,void *user,ps_code_preview **output);
/* pre-wrap + overflow-wrap:anywhere. Emergency breaks are only between
 * complete extended grapheme clusters. An indivisible wider cluster may
 * overflow; maximum reports that honestly. Source bytes are never rewritten.
 * A trailing hard break retains the final empty visual row for the caret. */
bool ps_code_preview_build_anywhere(const char *source,size_t bytes,float width,ps_code_measure measure,void *user,ps_code_preview **output);
void ps_code_preview_destroy(ps_code_preview *preview);
/* Validated source helpers shared with the renderer. */
size_t ps_code_preview_rune(const char *source,size_t bytes,uint32_t *rune);
bool ps_code_preview_invisible(uint32_t rune);
float ps_code_preview_advance(uint32_t rune,float x,float tab,float glyph);
#endif
