#ifndef PHYSIM_PROTOCOL_TEXT_H
#define PHYSIM_PROTOCOL_TEXT_H
#include <stdbool.h>
#include <stddef.h>
/* Display adapter only: original process bytes belong in protocol_store.
 * Valid UTF-8, LF, CR and tabs pass unchanged. Other bytes become \xHH.
 * Zero initialize per stream; final flushes an incomplete UTF-8 suffix.
 * Output requires 4*(size+3)+1 bytes. Insufficient space changes nothing.
 * Input/output must not overlap. */
typedef struct { unsigned char pending[4]; unsigned used; } ps_protocol_text;
bool ps_protocol_text_feed(ps_protocol_text *state,const void *bytes,size_t size,
                           bool final,char *output,size_t capacity,size_t *written);
#endif
