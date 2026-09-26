#ifndef PS_LANGUAGE_LEXER_H
#define PS_LANGUAGE_LEXER_H
#include <stddef.h>

/* Internal compiler API. Source is borrowed, length-delimited, not NUL-terminated. */
typedef enum ps_lang_kind {
    PS_LANG_EOF,
    PS_LANG_ERROR,
    PS_LANG_NEWLINE,
    PS_LANG_IDENTIFIER,
    PS_LANG_INTEGER,
    PS_LANG_FLOAT,
    PS_LANG_STRING,
    PS_LANG_LET,
    PS_LANG_VAR,
    PS_LANG_FUNC,
    PS_LANG_MUTATING,
    PS_LANG_STATIC,
    PS_LANG_RETURN,
    PS_LANG_IF,
    PS_LANG_GUARD,
    PS_LANG_ELSE,
    PS_LANG_WHILE,
    PS_LANG_FOR,
    PS_LANG_IN,
    PS_LANG_BREAK,
    PS_LANG_CONTINUE,
    PS_LANG_STRUCT,
    PS_LANG_ENUM,
    PS_LANG_CASE,
    PS_LANG_SWITCH,
    PS_LANG_DEFAULT,
    PS_LANG_IMPORT,
    PS_LANG_TRUE,
    PS_LANG_FALSE,
    PS_LANG_NIL,
    PS_LANG_LPAREN,
    PS_LANG_RPAREN,
    PS_LANG_LBRACE,
    PS_LANG_RBRACE,
    PS_LANG_LBRACKET,
    PS_LANG_RBRACKET,
    PS_LANG_COMMA,
    PS_LANG_COLON,
    PS_LANG_SEMICOLON,
    PS_LANG_DOT,
    PS_LANG_QUESTION,
    PS_LANG_PLUS,
    PS_LANG_MINUS,
    PS_LANG_STAR,
    PS_LANG_SLASH,
    PS_LANG_PERCENT,
    PS_LANG_EQUAL,
    PS_LANG_PLUS_EQUAL,
    PS_LANG_MINUS_EQUAL,
    PS_LANG_STAR_EQUAL,
    PS_LANG_SLASH_EQUAL,
    PS_LANG_PERCENT_EQUAL,
    PS_LANG_AMP_EQUAL,
    PS_LANG_PIPE_EQUAL,
    PS_LANG_CARET_EQUAL,
    PS_LANG_SHIFT_LEFT_EQUAL,
    PS_LANG_SHIFT_RIGHT_EQUAL,
    PS_LANG_EQ,
    PS_LANG_NE,
    PS_LANG_LT,
    PS_LANG_LE,
    PS_LANG_GT,
    PS_LANG_GE,
    PS_LANG_NOT,
    PS_LANG_AMP,
    PS_LANG_PIPE,
    PS_LANG_CARET,
    PS_LANG_TILDE,
    PS_LANG_SHIFT_LEFT,
    PS_LANG_SHIFT_RIGHT,
    PS_LANG_AND,
    PS_LANG_OR,
    PS_LANG_ARROW,
    PS_LANG_RANGE_CLOSED,
    PS_LANG_RANGE_OPEN,
    PS_LANG_COALESCE, /* Parser-synthesized from adjacent '?' tokens. */
    PS_LANG_STRING_PART_TRIPLE /* Parser-synthesized multiline string segment. */
} ps_lang_kind;

typedef struct ps_lang_token {
    ps_lang_kind kind;
    size_t offset, length, line, column;
    const char *error; /* Static diagnostic for ERROR; NULL otherwise. */
    size_t file; /* Source-file index, zero for a standalone parser input. */
} ps_lang_token;

typedef struct ps_lang_lexer {
    const unsigned char *source;
    size_t size, offset, line, column, comment_depth, interpolation_depth;
    ps_lang_token failure;
} ps_lang_lexer;

/* NULL source is valid only for zero size. NULL lexer/output are caller errors. */
void ps_lang_lexer_init(ps_lang_lexer *lexer, const void *source, size_t size);
ps_lang_token ps_lang_lexer_next(ps_lang_lexer *lexer);
/* Byte width of one Unicode-15.0 identifier scalar, or zero if invalid here. */
size_t ps_lang_identifier_width(const char *source, size_t size, int first);
#endif
