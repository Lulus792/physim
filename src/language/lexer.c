#include "lexer.h"
#include "unicode_identifiers.h"
#include <string.h>

static int peek(const ps_lang_lexer *l, size_t ahead) {
    return ahead < l->size - l->offset ? l->source[l->offset + ahead] : -1;
}
static void advance(ps_lang_lexer *l, size_t n) {
    l->offset += n;
    l->column += n;
}
static ps_lang_token token(const ps_lang_lexer *l, ps_lang_kind kind) {
    return (ps_lang_token){kind, l->offset, 0, l->line, l->column, NULL, 0};
}
static ps_lang_token finish(const ps_lang_lexer *l, ps_lang_token t) {
    t.length = l->offset - t.offset;
    return t;
}
static ps_lang_token fail(ps_lang_lexer *l, ps_lang_token t, const char *message) {
    t.kind = PS_LANG_ERROR;
    t.error = message;
    l->failure = finish(l, t);
    return l->failure;
}
static int digit(int c) { return c >= '0' && c <= '9'; }
static int hex_digit(int c) {
    return digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}
static int radix_digit(int c, unsigned base) {
    if (base == 2u)
        return c == '0' || c == '1';
    if (base == 8u)
        return c >= '0' && c <= '7';
    if (base == 10u)
        return digit(c);
    return hex_digit(c);
}
/* A separator must have a digit on both sides, including in exponents. */
static int advance_number_digits(ps_lang_lexer *l, unsigned base) {
    int count = 0;
    while (radix_digit(peek(l, 0), base)) {
        advance(l, 1);
        count++;
        if (peek(l, 0) == '_') {
            if (!radix_digit(peek(l, 1), base))
                return -1;
            advance(l, 1);
        }
    }
    return count;
}
static int alpha(int c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
/* Validate exactly one Unicode scalar, rejecting overlongs and surrogates. */
static size_t utf8_width(const ps_lang_lexer *l) {
    int a = peek(l, 0), b = peek(l, 1), c = peek(l, 2), d = peek(l, 3);
    if (a >= 0xC2 && a <= 0xDF && b >= 0x80 && b <= 0xBF)
        return 2;
    if (a >= 0xE0 && a <= 0xEF && b >= 0x80 && b <= 0xBF && c >= 0x80 && c <= 0xBF &&
        !(a == 0xE0 && b < 0xA0) && !(a == 0xED && b >= 0xA0))
        return 3;
    if (a >= 0xF0 && a <= 0xF4 && b >= 0x80 && b <= 0xBF && c >= 0x80 && c <= 0xBF && d >= 0x80 &&
        d <= 0xBF && !(a == 0xF0 && b < 0x90) && !(a == 0xF4 && b >= 0x90))
        return 4;
    return 0;
}
static uint32_t utf8_scalar(const ps_lang_lexer *l, size_t width) {
    uint32_t value = (uint32_t)peek(l, 0) & (width == 2 ? 0x1Fu : width == 3 ? 0x0Fu : 0x07u);
    for (size_t i = 1; i < width; i++)
        value = (value << 6) | ((uint32_t)peek(l, i) & 0x3Fu);
    return value;
}
static int identifier_range(const ps_lang_unicode_range *ranges, size_t count, uint32_t scalar) {
    size_t low = 0, high = count;
    while (low < high) {
        size_t middle = low + (high - low) / 2;
        if (scalar < ranges[middle].first)
            high = middle;
        else if (scalar > ranges[middle].last)
            low = middle + 1;
        else
            return 1;
    }
    return 0;
}
static int identifier_start(uint32_t scalar) {
    return identifier_range(ps_lang_identifier_starts,
                            sizeof(ps_lang_identifier_starts) / sizeof(ps_lang_identifier_starts[0]),
                            scalar);
}
static int identifier_continue(uint32_t scalar) {
    return identifier_range(ps_lang_identifier_continues,
                            sizeof(ps_lang_identifier_continues) /
                                sizeof(ps_lang_identifier_continues[0]),
                            scalar);
}
size_t ps_lang_identifier_width(const char *source, size_t size, int first) {
    if (!source || !size)
        return 0;
    unsigned char byte = (unsigned char)source[0];
    if (byte < 0x80)
        return alpha(byte) || (!first && digit(byte)) ? 1u : 0u;
    ps_lang_lexer view = {0};
    view.source = (const unsigned char *)source;
    view.size = size;
    size_t width = utf8_width(&view);
    if (!width)
        return 0;
    uint32_t scalar = utf8_scalar(&view, width);
    return (first ? identifier_start(scalar) : identifier_continue(scalar)) ? width : 0;
}
static ps_lang_token newline(ps_lang_lexer *l) {
    ps_lang_token t = token(l, PS_LANG_NEWLINE);
    size_t n = peek(l, 0) == '\r' && peek(l, 1) == '\n' ? 2 : 1;
    advance(l, n);
    l->line++;
    l->column = 1;
    return finish(l, t);
}
static int text_byte(ps_lang_lexer *l) {
    int c = peek(l, 0);
    if (c >= 0x80) {
        size_t n = utf8_width(l);
        if (!n)
            return 0;
        advance(l, n);
        return 1;
    }
    if (c < 0x20 && c != '\t')
        return 0;
    if (c == 0x7F)
        return 0;
    advance(l, 1);
    return 1;
}
void ps_lang_lexer_init(ps_lang_lexer *l, const void *source, size_t size) {
    *l = (ps_lang_lexer){0};
    l->source = source;
    l->size = size;
    l->line = l->column = 1;
    if (!source && size)
        (void)fail(l, token(l, PS_LANG_ERROR), "Missing source buffer");
}
static ps_lang_token interpolation(ps_lang_lexer *l, ps_lang_token opening, int multiline) {
    if (l->interpolation_depth >= 128)
        return fail(l, opening, "String interpolation nesting limit exceeded");
    ps_lang_lexer inner = *l;
    inner.interpolation_depth++;
    size_t depth = 1;
    while (depth) {
        ps_lang_token next = ps_lang_lexer_next(&inner);
        if (next.kind == PS_LANG_ERROR) {
            l->failure = next;
            return next;
        }
        if (next.kind == PS_LANG_EOF || (!multiline && next.kind == PS_LANG_NEWLINE))
            return fail(l, opening, "Unterminated string interpolation");
        if (next.kind == PS_LANG_LPAREN) {
            if (++depth > 128)
                return fail(l, opening, "String interpolation nesting limit exceeded");
        } else if (next.kind == PS_LANG_RPAREN)
            depth--;
    }
    size_t parent_depth = l->interpolation_depth;
    *l = inner;
    l->interpolation_depth = parent_depth;
    return token(l, PS_LANG_EOF);
}
ps_lang_token ps_lang_lexer_next(ps_lang_lexer *l) {
    if (l->failure.kind == PS_LANG_ERROR)
        return l->failure;
    for (;;) {
        int c = peek(l, 0);
        if (l->comment_depth) {
            if (c < 0)
                return fail(l, token(l, PS_LANG_ERROR), "Unterminated block comment");
            if (c == '\r' || c == '\n')
                return newline(l);
            if (c == '/' && peek(l, 1) == '*') {
                l->comment_depth++;
                advance(l, 2);
            } else if (c == '*' && peek(l, 1) == '/') {
                l->comment_depth--;
                advance(l, 2);
            } else if (!text_byte(l)) {
                return fail(l, token(l, PS_LANG_ERROR), "Invalid comment byte or UTF-8");
            }
            continue;
        }
        if (c == ' ' || c == '\t') {
            advance(l, 1);
            continue;
        }
        if (c == '/' && peek(l, 1) == '/') {
            advance(l, 2);
            while (peek(l, 0) >= 0 && peek(l, 0) != '\r' && peek(l, 0) != '\n') {
                if (!text_byte(l))
                    return fail(l, token(l, PS_LANG_ERROR), "Invalid comment byte or UTF-8");
            }
            continue;
        }
        if (c == '/' && peek(l, 1) == '*') {
            l->comment_depth = 1;
            advance(l, 2);
            continue;
        }
        break;
    }
    ps_lang_token t = token(l, PS_LANG_EOF);
    int c = peek(l, 0);
    if (c < 0)
        return t;
    if (c == '\r' || c == '\n')
        return newline(l);
    if (alpha(c) || c >= 0x80) {
        size_t width = ps_lang_identifier_width((const char *)l->source + l->offset,
                                                l->size - l->offset, 1);
        if (!width)
            return fail(l, t, c >= 0x80 && !utf8_width(l)
                                  ? "Invalid UTF-8 in source"
                                  : "Unicode character cannot start an identifier");
        advance(l, width);
        static const struct {
            const char *text;
            ps_lang_kind kind;
        } words[] = {{"let", PS_LANG_LET},         {"var", PS_LANG_VAR},
                     {"func", PS_LANG_FUNC},       {"mutating", PS_LANG_MUTATING},
                     {"static", PS_LANG_STATIC},
                     {"return", PS_LANG_RETURN},   {"if", PS_LANG_IF},
                     {"guard", PS_LANG_GUARD},
                     {"else", PS_LANG_ELSE},       {"while", PS_LANG_WHILE},
                     {"for", PS_LANG_FOR},         {"in", PS_LANG_IN},
                     {"break", PS_LANG_BREAK},     {"continue", PS_LANG_CONTINUE},
                     {"struct", PS_LANG_STRUCT},   {"enum", PS_LANG_ENUM},
                     {"case", PS_LANG_CASE},       {"switch", PS_LANG_SWITCH},
                     {"default", PS_LANG_DEFAULT}, {"import", PS_LANG_IMPORT},
                     {"true", PS_LANG_TRUE},       {"false", PS_LANG_FALSE},
                     {"nil", PS_LANG_NIL}};
        for (;;) {
            int next = peek(l, 0);
            if (next < 0)
                break;
            width = ps_lang_identifier_width((const char *)l->source + l->offset,
                                             l->size - l->offset, 0);
            if (!width) {
                if (next >= 0x80 && !utf8_width(l))
                    return fail(l, token(l, PS_LANG_ERROR), "Invalid UTF-8 in identifier");
                break;
            }
            advance(l, width);
        }
        t = finish(l, t);
        t.kind = PS_LANG_IDENTIFIER;
        for (size_t i = 0; i < sizeof(words) / sizeof(words[0]); i++) {
            if (strlen(words[i].text) == t.length &&
                memcmp(l->source + t.offset, words[i].text, t.length) == 0) {
                t.kind = words[i].kind;
                break;
            }
        }
        return t;
    }
    if (digit(c)) {
        t.kind = PS_LANG_INTEGER;
        int prefix = peek(l, 1);
        if (c == '0' && (prefix == 'x' || prefix == 'X' || prefix == 'b' || prefix == 'B' ||
                         prefix == 'o' || prefix == 'O')) {
            unsigned base = prefix == 'x' || prefix == 'X' ? 16u
                            : prefix == 'b' || prefix == 'B' ? 2u : 8u;
            advance(l, 2);
            int digits = advance_number_digits(l, base);
            if (digits == 0)
                return fail(l, t, "Expected digits after integer base prefix");
            if (digits < 0)
                return fail(l, t, "Invalid numeric separator");
            if (alpha(peek(l, 0)) || digit(peek(l, 0)) || peek(l, 0) >= 0x80)
                return fail(l, t, "Invalid digit or suffix for integer base");
            return finish(l, t);
        }
        if (advance_number_digits(l, 10u) < 0)
            return fail(l, t, "Invalid numeric separator");
        if (peek(l, 0) == '.' && digit(peek(l, 1))) {
            t.kind = PS_LANG_FLOAT;
            advance(l, 1);
            if (advance_number_digits(l, 10u) < 0)
                return fail(l, t, "Invalid numeric separator");
        }
        if (peek(l, 0) == 'e' || peek(l, 0) == 'E') {
            t.kind = PS_LANG_FLOAT;
            advance(l, 1);
            if (peek(l, 0) == '+' || peek(l, 0) == '-')
                advance(l, 1);
            if (!digit(peek(l, 0)))
                return fail(l, t, "Expected exponent digits");
            if (advance_number_digits(l, 10u) < 0)
                return fail(l, t, "Invalid numeric separator");
        }
        if (alpha(peek(l, 0)) || peek(l, 0) >= 0x80)
            return fail(l, t, "Unsupported numeric suffix");
        return finish(l, t);
    }
    if (c == '"') {
        t.kind = PS_LANG_STRING;
        int multiline = peek(l, 1) == '"' && peek(l, 2) == '"';
        advance(l, multiline ? 3 : 1);
        while (multiline ? !(peek(l, 0) == '"' && peek(l, 1) == '"' && peek(l, 2) == '"')
                         : peek(l, 0) != '"') {
            c = peek(l, 0);
            if (c < 0 || (!multiline && (c == '\r' || c == '\n')))
                return fail(l, t, "Unterminated string");
            if (c == '\r' || c == '\n') {
                (void)newline(l);
                continue;
            }
            if (c == '\t')
                return fail(l, token(l, PS_LANG_ERROR), "Use an escape for a string tab");
            if (c == '\\') {
                ps_lang_token opening = token(l, PS_LANG_ERROR);
                advance(l, 1);
                c = peek(l, 0);
                if (c == '(') {
                    advance(l, 1);
                    ps_lang_token result = interpolation(l, opening, multiline);
                    if (result.kind == PS_LANG_ERROR)
                        return result;
                    continue;
                }
                if (c == 'u') {
                    advance(l, 1);
                    if (peek(l, 0) != '{')
                        return fail(l, token(l, PS_LANG_ERROR), "Expected '{' after Unicode escape");
                    advance(l, 1);
                    uint32_t scalar = 0;
                    unsigned digits = 0;
                    while (hex_digit(peek(l, 0)) && digits < 6) {
                        int digit = peek(l, 0);
                        scalar = (scalar << 4) |
                                 (uint32_t)(digit <= '9' ? digit - '0'
                                            : digit <= 'F' ? digit - 'A' + 10 : digit - 'a' + 10);
                        advance(l, 1);
                        digits++;
                    }
                    if (!digits || peek(l, 0) != '}' || !scalar || scalar > 0x10FFFFu ||
                        (scalar >= 0xD800u && scalar <= 0xDFFFu))
                        return fail(l, token(l, PS_LANG_ERROR), "Invalid Unicode scalar escape");
                    advance(l, 1);
                    continue;
                }
                if (c != '"' && c != '\\' && c != 'n' && c != 'r' && c != 't')
                    return fail(l, token(l, PS_LANG_ERROR), "Invalid string escape");
                advance(l, 1);
            } else if (!text_byte(l)) {
                return fail(l, token(l, PS_LANG_ERROR), "Invalid string byte or UTF-8");
            }
        }
        advance(l, multiline ? 3 : 1);
        return finish(l, t);
    }
    static const struct {
        const char *text;
        ps_lang_kind kind;
    } operators[] = {{"...", PS_LANG_RANGE_CLOSED},
                     {"..<", PS_LANG_RANGE_OPEN},
                     {"<<=", PS_LANG_SHIFT_LEFT_EQUAL},
                     {">>=", PS_LANG_SHIFT_RIGHT_EQUAL},
                     {"->", PS_LANG_ARROW},
                     {"==", PS_LANG_EQ},
                     {"!=", PS_LANG_NE},
                     {"<=", PS_LANG_LE},
                     {">=", PS_LANG_GE},
                     {"&&", PS_LANG_AND},
                     {"||", PS_LANG_OR},
                     {"<<", PS_LANG_SHIFT_LEFT},
                     {">>", PS_LANG_SHIFT_RIGHT},
                     {"+=", PS_LANG_PLUS_EQUAL},
                     {"-=", PS_LANG_MINUS_EQUAL},
                     {"*=", PS_LANG_STAR_EQUAL},
                     {"/=", PS_LANG_SLASH_EQUAL},
                     {"%=", PS_LANG_PERCENT_EQUAL},
                     {"&=", PS_LANG_AMP_EQUAL},
                     {"|=", PS_LANG_PIPE_EQUAL},
                     {"^=", PS_LANG_CARET_EQUAL},
                     {"(", PS_LANG_LPAREN},
                     {")", PS_LANG_RPAREN},
                     {"{", PS_LANG_LBRACE},
                     {"}", PS_LANG_RBRACE},
                     {"[", PS_LANG_LBRACKET},
                     {"]", PS_LANG_RBRACKET},
                     {",", PS_LANG_COMMA},
                     {":", PS_LANG_COLON},
                     {";", PS_LANG_SEMICOLON},
                     {".", PS_LANG_DOT},
                     {"?", PS_LANG_QUESTION},
                     {"&", PS_LANG_AMP},
                     {"|", PS_LANG_PIPE},
                     {"^", PS_LANG_CARET},
                     {"~", PS_LANG_TILDE},
                     {"+", PS_LANG_PLUS},
                     {"-", PS_LANG_MINUS},
                     {"*", PS_LANG_STAR},
                     {"/", PS_LANG_SLASH},
                     {"%", PS_LANG_PERCENT},
                     {"=", PS_LANG_EQUAL},
                     {"<", PS_LANG_LT},
                     {">", PS_LANG_GT},
                     {"!", PS_LANG_NOT}};
    for (size_t i = 0; i < sizeof(operators) / sizeof(operators[0]); i++) {
        size_t n = strlen(operators[i].text);
        if (n <= l->size - l->offset && memcmp(l->source + l->offset, operators[i].text, n) == 0) {
            advance(l, n);
            t.kind = operators[i].kind;
            return finish(l, t);
        }
    }
    advance(l, 1);
    return fail(l, t, "Unexpected source byte");
}
