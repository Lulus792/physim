#ifndef PS_TEXT_WRAP_H
#define PS_TEXT_WRAP_H
#include "code_preview.h"
#include "../third_party/nuklear_text_layout.h"
typedef struct {
    char *source;
    size_t bytes;
    float width;
    struct nk_text_visual visual;
    struct nk_text_visual_glyph *glyphs;
    struct nk_text_visual_row *rows;
} ps_text_wrap;
/* Owns an exact source copy and the row/glyph map. Original byte offsets,
 * grapheme boundaries and tab positions are shared by drawing, hit testing,
 * selection and native accessibility. Failure leaves *output unchanged. */
bool ps_text_wrap_build(const char *source,size_t bytes,float width,float line_height,
                       ps_code_measure measure,void *user,ps_text_wrap **output);
void ps_text_wrap_destroy(ps_text_wrap *layout);
#endif
