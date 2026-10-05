#ifndef PS_TEXT_VALIDATION_H
#define PS_TEXT_VALIDATION_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static inline bool ps_text_valid(const char *s, size_t capacity, bool multiline) {
    size_t length = 0;
    while (length < capacity && s[length])
        length++;
    if (length == capacity)
        return false;
    const char *end = s + length;
    const unsigned char *p = (const unsigned char *)s;
    while (p < (const unsigned char *)end) {
        uint32_t c = *p++;
        unsigned extra = 0;
        uint32_t minimum = 0;
        if (c >= 0xc2 && c <= 0xdf) {
            extra = 1;
            minimum = 0x80;
            c &= 31;
        } else if (c >= 0xe0 && c <= 0xef) {
            extra = 2;
            minimum = 0x800;
            c &= 15;
        } else if (c >= 0xf0 && c <= 0xf4) {
            extra = 3;
            minimum = 0x10000;
            c &= 7;
        } else if (c >= 0x80)
            return false;
        if ((size_t)((const unsigned char *)end - p) < extra)
            return false;
        for (unsigned i = 0; i < extra; i++) {
            if ((*p & 0xc0) != 0x80)
                return false;
            c = (c << 6) | (*p++ & 63);
        }
        if (c < minimum || c > 0x10ffff || (c >= 0xd800 && c <= 0xdfff) ||
            (c >= 0x7f && c <= 0x9f) ||
            (c < 32 && !(multiline && (c == '\n' || c == '\r' || c == '\t'))))
            return false;
    }
    return true;
}
#endif
