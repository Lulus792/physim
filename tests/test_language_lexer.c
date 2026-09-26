#include "language/lexer.h"
#include <stdio.h>
#include <string.h>

#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Language lexer line %d: %s\n", __LINE__, #x);                         \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)

static int sequence(const char *source, const ps_lang_kind *kinds, size_t count) {
    ps_lang_lexer l;
    ps_lang_lexer_init(&l, source, strlen(source));
    for (size_t i = 0; i < count; i++) {
        ps_lang_token t = ps_lang_lexer_next(&l);
        CHECK(t.kind == kinds[i]);
        CHECK(t.offset <= l.size && t.length <= l.size - t.offset);
        CHECK(t.line > 0 && t.column > 0 && t.error == NULL);
    }
    CHECK(ps_lang_lexer_next(&l).kind == PS_LANG_EOF);
    CHECK(ps_lang_lexer_next(&l).kind == PS_LANG_EOF);
    return 0;
}
static int rejects(const void *source, size_t size) {
    ps_lang_lexer l;
    ps_lang_lexer_init(&l, source, size);
    for (size_t i = 0; i <= size; i++) {
        ps_lang_token t = ps_lang_lexer_next(&l);
        if (t.kind == PS_LANG_ERROR) {
            ps_lang_token again = ps_lang_lexer_next(&l);
            CHECK(t.error && again.kind == t.kind && again.offset == t.offset &&
                  again.length == t.length && again.error == t.error);
            return 0;
        }
        CHECK(t.kind != PS_LANG_EOF);
    }
    CHECK(0);
    return 1;
}
/* Independent bounds/progress oracle for arbitrary bytes and every truncated prefix. */
static int bounded(const unsigned char *source, size_t size) {
    ps_lang_lexer l;
    ps_lang_lexer_init(&l, source, size);
    size_t previous = 0;
    for (size_t i = 0; i <= size; i++) {
        ps_lang_token t = ps_lang_lexer_next(&l);
        CHECK(t.offset <= size && t.length <= size - t.offset);
        CHECK(l.offset <= size && l.offset >= previous);
        if (t.kind == PS_LANG_EOF || t.kind == PS_LANG_ERROR)
            return 0;
        CHECK(t.length > 0 && l.offset > previous);
        previous = l.offset;
    }
    CHECK(0);
    return 1;
}
int main(void) {
    const ps_lang_kind program[] = {
        PS_LANG_FUNC,       PS_LANG_IDENTIFIER, PS_LANG_LPAREN,  PS_LANG_IDENTIFIER,
        PS_LANG_COLON,      PS_LANG_IDENTIFIER, PS_LANG_RPAREN,  PS_LANG_ARROW,
        PS_LANG_IDENTIFIER, PS_LANG_LBRACE,     PS_LANG_NEWLINE, PS_LANG_LET,
        PS_LANG_IDENTIFIER, PS_LANG_EQUAL,      PS_LANG_FLOAT,   PS_LANG_STAR,
        PS_LANG_IDENTIFIER, PS_LANG_NEWLINE,    PS_LANG_RETURN,  PS_LANG_IDENTIFIER,
        PS_LANG_NEWLINE,    PS_LANG_RBRACE};
    CHECK(sequence("func energy(mass: Float64) -> Float64 {\n"
                   " let result = 0.5 * mass\n return result\n}",
                   program, sizeof(program) / sizeof(program[0])) == 0);
    const ps_lang_kind interpolated[] = {PS_LANG_STRING, PS_LANG_IDENTIFIER};
    CHECK(sequence("\"value \\(f(1))\" next", interpolated,
                   sizeof(interpolated) / sizeof(interpolated[0])) == 0);
    const ps_lang_kind words[] = {
        PS_LANG_LET,      PS_LANG_VAR,    PS_LANG_FUNC,       PS_LANG_RETURN,     PS_LANG_IF,
        PS_LANG_GUARD,    PS_LANG_ELSE,   PS_LANG_WHILE,      PS_LANG_FOR,        PS_LANG_IN,
        PS_LANG_BREAK,
        PS_LANG_CONTINUE, PS_LANG_STRUCT, PS_LANG_ENUM,       PS_LANG_IMPORT,     PS_LANG_TRUE,
        PS_LANG_FALSE,    PS_LANG_NIL,    PS_LANG_IDENTIFIER, PS_LANG_IDENTIFIER, PS_LANG_CASE,
        PS_LANG_SWITCH,   PS_LANG_DEFAULT};
    CHECK(sequence("let var func return if guard else while for in break continue struct enum "
                   "import true false nil letter _x2 case switch default",
                   words, sizeof(words) / sizeof(words[0])) == 0);
    const ps_lang_kind ops[] = {
        PS_LANG_LPAREN,      PS_LANG_RPAREN,       PS_LANG_LBRACE,      PS_LANG_RBRACE,
        PS_LANG_LBRACKET,    PS_LANG_RBRACKET,     PS_LANG_COMMA,       PS_LANG_COLON,
        PS_LANG_SEMICOLON,   PS_LANG_DOT,          PS_LANG_QUESTION,    PS_LANG_AMP,
        PS_LANG_PLUS,
        PS_LANG_MINUS,       PS_LANG_STAR,         PS_LANG_SLASH,       PS_LANG_PERCENT,
        PS_LANG_EQUAL,       PS_LANG_EQ,           PS_LANG_NE,          PS_LANG_LT,
        PS_LANG_LE,          PS_LANG_GT,           PS_LANG_GE,          PS_LANG_NOT,
        PS_LANG_AND,         PS_LANG_OR,           PS_LANG_ARROW,       PS_LANG_RANGE_CLOSED,
        PS_LANG_RANGE_OPEN,  PS_LANG_PLUS_EQUAL,   PS_LANG_MINUS_EQUAL, PS_LANG_STAR_EQUAL,
        PS_LANG_SLASH_EQUAL, PS_LANG_PERCENT_EQUAL};
    CHECK(sequence("(){}[],:;.? & + - * / % = == != < <= > >= ! && || -> ... ..< += -= *= /= %=", ops,
                   sizeof(ops) / sizeof(ops[0])) == 0);
    const ps_lang_kind bits[] = {PS_LANG_AMP, PS_LANG_PIPE, PS_LANG_CARET,
                                 PS_LANG_TILDE, PS_LANG_SHIFT_LEFT, PS_LANG_SHIFT_RIGHT};
    CHECK(sequence("& | ^ ~ << >>", bits, sizeof(bits) / sizeof(bits[0])) == 0);
    const ps_lang_kind bit_assignments[] = {
        PS_LANG_AMP_EQUAL, PS_LANG_PIPE_EQUAL, PS_LANG_CARET_EQUAL,
        PS_LANG_SHIFT_LEFT_EQUAL, PS_LANG_SHIFT_RIGHT_EQUAL, PS_LANG_GE};
    CHECK(sequence("&= |= ^= <<= >>= >=", bit_assignments,
                   sizeof(bit_assignments) / sizeof(bit_assignments[0])) == 0);
    const ps_lang_kind numbers[] = {PS_LANG_INTEGER, PS_LANG_RANGE_CLOSED, PS_LANG_INTEGER,
                                    PS_LANG_INTEGER, PS_LANG_RANGE_OPEN,   PS_LANG_INTEGER,
                                    PS_LANG_FLOAT,   PS_LANG_FLOAT,        PS_LANG_MINUS,
                                    PS_LANG_FLOAT,   PS_LANG_INTEGER,      PS_LANG_DOT};
    CHECK(sequence("1...3 0..<5 1.25 2e-3 -2E+10 4.", numbers,
                   sizeof(numbers) / sizeof(numbers[0])) == 0);
    const ps_lang_kind hex_numbers[] = {PS_LANG_INTEGER, PS_LANG_INTEGER, PS_LANG_MINUS,
                                        PS_LANG_INTEGER, PS_LANG_INTEGER};
    CHECK(sequence("0xB5C4D8FF 0Xff -0x8000000000000000 0x0", hex_numbers,
                   sizeof(hex_numbers) / sizeof(hex_numbers[0])) == 0);
    const ps_lang_kind radix_numbers[] = {PS_LANG_INTEGER, PS_LANG_INTEGER, PS_LANG_INTEGER,
                                          PS_LANG_INTEGER, PS_LANG_MINUS, PS_LANG_INTEGER};
    CHECK(sequence("0b1010 0B1 0o755 0O0 -0b10", radix_numbers,
                   sizeof(radix_numbers) / sizeof(radix_numbers[0])) == 0);
    const ps_lang_kind unicode_names[] = {PS_LANG_IDENTIFIER, PS_LANG_IDENTIFIER,
                                          PS_LANG_IDENTIFIER, PS_LANG_IDENTIFIER};
    CHECK(sequence("gr\xc3\xb6\xc3\x9f" "e_\xce\xb4" "2 e\xcc\x81 \xc3\xa9 let\xc3\xa4",
                   unicode_names, sizeof(unicode_names) / sizeof(unicode_names[0])) == 0);
    CHECK(ps_lang_identifier_width("a", 1, 1) == 1);
    CHECK(ps_lang_identifier_width("2", 1, 1) == 0);
    CHECK(ps_lang_identifier_width("2", 1, 0) == 1);
    CHECK(ps_lang_identifier_width("\xc3\xa4", 2, 1) == 2);
    CHECK(ps_lang_identifier_width("\xcc\x81", 2, 1) == 0);
    CHECK(ps_lang_identifier_width("\xcc\x81", 2, 0) == 2);
    CHECK(ps_lang_identifier_width("\xf0\x9f\x9a\x80", 4, 1) == 0);
    CHECK(ps_lang_identifier_width("\xc3", 1, 1) == 0);
    CHECK(ps_lang_identifier_width(NULL, 0, 1) == 0);
    const ps_lang_kind separated_numbers[] = {PS_LANG_INTEGER, PS_LANG_INTEGER,
                                              PS_LANG_INTEGER, PS_LANG_INTEGER,
                                              PS_LANG_FLOAT, PS_LANG_FLOAT};
    CHECK(sequence("1_000 0xFF_A0 0b1010_0101 0o7_55 1_2.3_4 1_2.3_4e+5_6",
                   separated_numbers, sizeof(separated_numbers) / sizeof(separated_numbers[0])) == 0);
    const ps_lang_kind strings[] = {PS_LANG_STRING, PS_LANG_STRING};
    CHECK(sequence("\"\" \"\xc3\xa4\xf0\x9f\x98\x80\\n\\t\\r\\\\\\\"\"", strings, 2) == 0);
    const ps_lang_kind unicode_escapes[] = {PS_LANG_STRING, PS_LANG_STRING};
    CHECK(sequence("\"\\u{41}\\u{e4}\\u{1F600}\" \"\"\"\\u{1}\"\"\"",
                   unicode_escapes, 2) == 0);
    const ps_lang_kind long_strings[] = {PS_LANG_STRING, PS_LANG_STRING};
    CHECK(sequence("\"\"\"\"\"\" \"\"\"a\nb\"\"\"", long_strings, 2) == 0);

    ps_lang_lexer l;
    ps_lang_lexer_init(&l, NULL, 0);
    CHECK(ps_lang_lexer_next(&l).kind == PS_LANG_EOF);
    ps_lang_lexer_init(&l, NULL, 1);
    CHECK(ps_lang_lexer_next(&l).kind == PS_LANG_ERROR);
    /* Exact-size buffer, mixed newline encodings, byte columns after UTF-8. */
    const char positioned[] = {'a', '\r', '\n', '\t', 'b', '\r', 'c', '\n', 'd'};
    ps_lang_lexer_init(&l, positioned, sizeof(positioned));
    ps_lang_token t = ps_lang_lexer_next(&l);
    CHECK(t.offset == 0 && t.line == 1 && t.column == 1 && t.length == 1);
    t = ps_lang_lexer_next(&l);
    CHECK(t.kind == PS_LANG_NEWLINE && t.offset == 1 && t.length == 2);
    t = ps_lang_lexer_next(&l);
    CHECK(t.offset == 4 && t.line == 2 && t.column == 2);
    CHECK(ps_lang_lexer_next(&l).kind == PS_LANG_NEWLINE);
    t = ps_lang_lexer_next(&l);
    CHECK(t.offset == 6 && t.line == 3 && t.column == 1);
    CHECK(ps_lang_lexer_next(&l).kind == PS_LANG_NEWLINE);
    t = ps_lang_lexer_next(&l);
    CHECK(t.offset == 8 && t.line == 4 && t.column == 1);
    const char *comments = "/* a /* b */\r\n */ let//x\nvar";
    ps_lang_lexer_init(&l, comments, strlen(comments));
    CHECK(ps_lang_lexer_next(&l).kind == PS_LANG_NEWLINE);
    t = ps_lang_lexer_next(&l);
    CHECK(t.kind == PS_LANG_LET && t.line == 2 && t.column == 5);
    CHECK(ps_lang_lexer_next(&l).kind == PS_LANG_NEWLINE);
    t = ps_lang_lexer_next(&l);
    CHECK(t.kind == PS_LANG_VAR && t.line == 3 && t.column == 1);
    CHECK(ps_lang_lexer_next(&l).kind == PS_LANG_EOF);
    ps_lang_lexer_init(&l, "\"\xc3\xa4\" x", 6);
    CHECK(ps_lang_lexer_next(&l).length == 4);
    CHECK(ps_lang_lexer_next(&l).column == 6);
    const char *multiline = "\"\"\"a\r\nb\nc\rd\"\"\" x";
    ps_lang_lexer_init(&l, multiline, strlen(multiline));
    t = ps_lang_lexer_next(&l);
    CHECK(t.kind == PS_LANG_STRING && t.line == 1 && t.column == 1 &&
          t.length == strlen(multiline) - 2);
    t = ps_lang_lexer_next(&l);
    CHECK(t.kind == PS_LANG_IDENTIFIER && t.line == 4 && t.column == 6);

    const char *bad[] = {"1e",
                         "1e+",
                         "1abc",
                         "0x",
                         "0Xg",
                         "0x1g",
                         "0x_1",
                         "0b",
                         "0B2",
                         "0b102",
                         "0b_1",
                         "0o",
                         "0O8",
                         "0o78",
                         "0o_7",
                         "1_",
                         "1__0",
                         "0xFF_",
                         "0b1__0",
                         "0o7_8",
                         "1_2.3_",
                         "1e_2",
                         "1e2_",
                         ("\xcc\x81" "a"),
                         "\xf0\x9f\x9a\x80",
                         "a\xc0\xaf",
                         "1\xc3\xa4",
                         "@",
                         "/*",
                         "/* /* */",
                         "\"",
                         "\"x\n\"",
                         "\"\"\"unterminated",
                         "\"\"\"bad\n\\q\"\"\"",
                         "\"\"\"bad\ttext\"\"\"",
                         "\"x\t\"",
                         "\"\\q\"",
                         "\"\\u\"",
                         "\"\\u{}\"",
                         "\"\\u{0}\"",
                         "\"\\u{xyz}\"",
                         "\"\\u{1234567}\"",
                         "\"\\u{D800}\"",
                         "\"\\u{10FFFFF}\"",
                         "\"\\u{110000}\"",
                         "\"\\u{12\"",
                         "\"\\",
                         "\"\xc0\xaf\"",
                         "\"\xed\xa0\x80\"",
                         "//\xf4\x90\x80\x80",
                         "/*\xff*/",
                         "\"\xe2\x82"};
    for (size_t i = 0; i < sizeof(bad) / sizeof(bad[0]); i++) {
        int result = rejects(bad[i], strlen(bad[i]));
        if (result) fprintf(stderr, "Rejected fixture index %zu: %s\n", i, bad[i]);
        CHECK(result == 0);
    }
    const char nul[] = {'l', 'e', 't', 0, 'x'};
    CHECK(rejects(nul, sizeof(nul)) == 0);
    const char string_nul[] = {'"', 0, '"'};
    CHECK(rejects(string_nul, sizeof(string_nul)) == 0);
    /* Diagnostic locations refer to source bytes, including the missing exponent. */
    ps_lang_lexer_init(&l, "\n  1e+", 6);
    CHECK(ps_lang_lexer_next(&l).kind == PS_LANG_NEWLINE);
    t = ps_lang_lexer_next(&l);
    CHECK(t.kind == PS_LANG_ERROR && t.offset == 3 && t.length == 3 && t.line == 2 &&
          t.column == 3);
    ps_lang_lexer_init(&l, "\"\\q\"", 4);
    t = ps_lang_lexer_next(&l);
    CHECK(t.kind == PS_LANG_ERROR && t.offset == 2 && t.column == 3);
    ps_lang_lexer_init(&l, "\"\"\"a\n\\q\"\"\"", 10);
    t = ps_lang_lexer_next(&l);
    CHECK(t.kind == PS_LANG_ERROR && t.line == 2 && t.column == 2);
    const unsigned char long_source[] = "let text = \"\"\"a\r\nb\\\"c\"\"\"\n";
    for (size_t n = 0; n < sizeof(long_source); n++)
        CHECK(bounded(long_source, n) == 0);
    const unsigned char unicode_source[] = "let text = \"\\u{1F600}\"\n";
    for (size_t n = 0; n < sizeof(unicode_source); n++)
        CHECK(bounded(unicode_source, n) == 0);
    unsigned char source[] =
        "func x(a: Float64) {\n/* hi */ let y = \"\xc3\xa4\"\nreturn 1e-2...3\n}";
    for (size_t n = 0; n < sizeof(source); n++)
        CHECK(bounded(source, n) == 0);
    for (size_t i = 0; i < sizeof(source); i++) {
        unsigned char saved = source[i];
        for (unsigned b = 0; b < 256; b++) {
            source[i] = (unsigned char)b;
            CHECK(bounded(source, sizeof(source)) == 0);
        }
        source[i] = saved;
    }
    puts("Language lexer: syntax, diagnostics, UTF-8 and bounded mutations passed");
    return 0;
}
