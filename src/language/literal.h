#ifndef PS_LANGUAGE_LITERAL_H
#define PS_LANGUAGE_LITERAL_H

#include "lexer.h"
#include <stdint.h>

typedef struct {
    size_t offset;
    unsigned char bytes[4];
    unsigned count;
    unsigned next;
} ps_lang_string_cursor;

/* Consume decoded string bytes without allocation. A negative result means end of token.
 * Raw line endings in triple-quoted strings are normalized to LF; escapes stay explicit. */
static inline int ps_lang_string_next_byte_range(const unsigned char *source,
                                                 ps_lang_token token,
                                                 ps_lang_string_cursor *cursor,
                                                 size_t delimiter, int multiline) {
    if (cursor->next < cursor->count)
        return cursor->bytes[cursor->next++];
    if (cursor->offset < delimiter)
        cursor->offset = delimiter;
    size_t end = token.length - delimiter;
    if (cursor->offset >= end)
        return -1;
    unsigned byte = source[token.offset + cursor->offset++];
    if (byte == '\\') {
        byte = source[token.offset + cursor->offset++]; /* Lexer checked this escape. */
        if (byte == 'u') {
            cursor->offset++; /* Opening brace. */
            uint32_t scalar = 0;
            while (source[token.offset + cursor->offset] != '}') {
                unsigned digit = source[token.offset + cursor->offset++];
                scalar = (scalar << 4) |
                         (digit <= '9' ? digit - '0' : digit <= 'F' ? digit - 'A' + 10
                                                                  : digit - 'a' + 10);
            }
            cursor->offset++; /* Closing brace. */
            cursor->next = 1;
            if (scalar < 0x80u) {
                cursor->bytes[0] = (unsigned char)scalar;
                cursor->count = 1;
            } else if (scalar < 0x800u) {
                cursor->bytes[0] = (unsigned char)(0xC0u | (scalar >> 6));
                cursor->bytes[1] = (unsigned char)(0x80u | (scalar & 0x3Fu));
                cursor->count = 2;
            } else if (scalar < 0x10000u) {
                cursor->bytes[0] = (unsigned char)(0xE0u | (scalar >> 12));
                cursor->bytes[1] = (unsigned char)(0x80u | ((scalar >> 6) & 0x3Fu));
                cursor->bytes[2] = (unsigned char)(0x80u | (scalar & 0x3Fu));
                cursor->count = 3;
            } else {
                cursor->bytes[0] = (unsigned char)(0xF0u | (scalar >> 18));
                cursor->bytes[1] = (unsigned char)(0x80u | ((scalar >> 12) & 0x3Fu));
                cursor->bytes[2] = (unsigned char)(0x80u | ((scalar >> 6) & 0x3Fu));
                cursor->bytes[3] = (unsigned char)(0x80u | (scalar & 0x3Fu));
                cursor->count = 4;
            }
            return cursor->bytes[0];
        }
        return byte == 'n' ? '\n' : byte == 'r' ? '\r' : byte == 't' ? '\t' : (int)byte;
    }
    if (multiline && byte == '\r') {
        if (cursor->offset < end && source[token.offset + cursor->offset] == '\n')
            cursor->offset++;
        return '\n';
    }
    return (int)byte;
}
static inline int ps_lang_string_next_byte(const unsigned char *source, ps_lang_token token,
                                           ps_lang_string_cursor *cursor) {
    size_t delimiter = token.length >= 6 && source[token.offset + 1] == '"' &&
                       source[token.offset + 2] == '"' ? 3u : 1u;
    return ps_lang_string_next_byte_range(source, token, cursor, delimiter, delimiter == 3);
}
static inline int ps_lang_string_next_segment_byte(const unsigned char *source,
                                                   ps_lang_token token,
                                                   ps_lang_string_cursor *cursor) {
    return ps_lang_string_next_byte_range(source, token, cursor, 0,
                                          token.kind == PS_LANG_STRING_PART_TRIPLE);
}

static inline unsigned ps_lang_integer_base(const unsigned char *source, ps_lang_token token) {
    if (token.length < 3 || source[token.offset] != '0')
        return 10u;
    unsigned char prefix = source[token.offset + 1];
    if (prefix == 'x' || prefix == 'X')
        return 16u;
    if (prefix == 'b' || prefix == 'B')
        return 2u;
    if (prefix == 'o' || prefix == 'O')
        return 8u;
    return 10u;
}

static inline int ps_lang_prefixed_integer(const unsigned char *source, ps_lang_token token) {
    return ps_lang_integer_base(source, token) != 10u;
}

static inline unsigned ps_lang_integer_digit(unsigned char byte) {
    if (byte >= '0' && byte <= '9')
        return byte - '0';
    if (byte >= 'a' && byte <= 'f')
        return byte - 'a' + 10;
    return byte - 'A' + 10;
}

/* The checker validates the signed range before consumers call this decoder. */
static inline uint64_t ps_lang_integer_value(const unsigned char *source, ps_lang_token token) {
    unsigned base = ps_lang_integer_base(source, token);
    uint64_t value = 0;
    for (size_t i = base == 10u ? 0u : 2u; i < token.length; i++) {
        unsigned char byte = source[token.offset + i];
        if (byte != '_')
            value = value * base + ps_lang_integer_digit(byte);
    }
    return value;
}

#endif
