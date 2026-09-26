#include "parser.h"
#include <string.h>

enum { PS_PARSE_DEPTH = 128 };
typedef struct parser {
    ps_lang_lexer lexer;
    ps_lang_token current, error;
    ps_lang_node *nodes;
    size_t capacity, count;
    size_t indent_line, indent;
    unsigned depth;
    unsigned index_range_depth;
    unsigned lambda_block_count;
} parser;

static void error(parser *p, const char *message) {
    if (p->error.kind != PS_LANG_ERROR) {
        p->error = p->current;
        p->error.kind = PS_LANG_ERROR;
        p->error.error = message;
    }
}
static void advance(parser *p) {
    if (p->error.kind == PS_LANG_ERROR)
        return;
    p->current = ps_lang_lexer_next(&p->lexer);
    if (p->current.kind == PS_LANG_ERROR)
        p->error = p->current;
    if (p->current.kind != PS_LANG_NEWLINE && p->current.kind != PS_LANG_EOF &&
        p->current.kind != PS_LANG_ERROR && p->indent_line != p->current.line) {
        size_t start = p->current.offset - (p->current.column - 1);
        size_t pos = start;
        while (pos < p->lexer.size && p->lexer.source[pos] == ' ')
            pos++;
        p->indent = pos - start;
        p->indent_line = p->current.line;
        if (pos < p->lexer.size && p->lexer.source[pos] == '\t')
            error(p, "Use spaces for indentation, not tabs");
    }
}
static int accept(parser *p, ps_lang_kind k) {
    if (p->error.kind == PS_LANG_ERROR || p->current.kind != k)
        return 0;
    advance(p);
    return 1;
}
static int expect(parser *p, ps_lang_kind k, const char *message) {
    if (accept(p, k))
        return 1;
    error(p, message);
    return 0;
}
/* In type position, split adjacent '>' and '=' bytes that are operators in expressions. */
static int accept_type_gt(parser *p) {
    if (p->error.kind == PS_LANG_ERROR)
        return 0;
    if (p->current.kind == PS_LANG_SHIFT_RIGHT_EQUAL) {
        p->current.kind = PS_LANG_GE;
        p->current.offset++;
        p->current.column++;
        p->current.length = 2;
        return 1;
    }
    if (p->current.kind == PS_LANG_SHIFT_RIGHT) {
        p->current.kind = PS_LANG_GT;
        p->current.offset++;
        p->current.column++;
        p->current.length = 1;
        return 1;
    }
    if (p->current.kind == PS_LANG_GE) {
        p->current.kind = PS_LANG_EQUAL;
        p->current.offset++;
        p->current.column++;
        p->current.length = 1;
        return 1;
    }
    return accept(p, PS_LANG_GT);
}
static int expect_type_gt(parser *p, const char *message) {
    if (accept_type_gt(p))
        return 1;
    error(p, message);
    return 0;
}
static void newlines(parser *p) {
    while (accept(p, PS_LANG_NEWLINE)) {
    }
}
static void separators(parser *p) {
    while (accept(p, PS_LANG_NEWLINE) || accept(p, PS_LANG_SEMICOLON)) {
    }
}
static int enter(parser *p) {
    if (p->error.kind == PS_LANG_ERROR)
        return 0;
    if (p->depth >= PS_PARSE_DEPTH) {
        error(p, "Syntax nesting limit exceeded (128)");
        return 0;
    }
    p->depth++;
    return 1;
}
static size_t node(parser *p, ps_lang_node_kind kind, ps_lang_token t, size_t a, size_t b,
                   size_t c) {
    if (p->error.kind == PS_LANG_ERROR)
        return 0;
    if (p->count >= p->capacity) {
        error(p, "Syntax node capacity exceeded");
        return 0;
    }
    size_t id = p->count++;
    p->nodes[id] = (ps_lang_node){kind, t, a, b, c, 0};
    return id;
}
static void append(parser *p, size_t *first, size_t *last, size_t id) {
    if (!id)
        return;
    if (*last)
        p->nodes[*last].next = id;
    else
        *first = id;
    *last = id;
}
static size_t expression(parser *p, int minimum, int multiline);
static size_t statement(parser *p);
static size_t block(parser *p, size_t parent_indent);
static int contextual_by(parser *p) {
    return p->current.kind == PS_LANG_IDENTIFIER && p->current.length == 2 &&
           memcmp(p->lexer.source + p->current.offset, "by", 2) == 0;
}
static int omitted_end_before_by(parser *p) {
    if (!contextual_by(p))
        return 0;
    ps_lang_lexer lookahead = p->lexer;
    ps_lang_token next;
    do {
        next = ps_lang_lexer_next(&lookahead);
    } while (next.kind == PS_LANG_NEWLINE);
    return next.kind != PS_LANG_RBRACKET;
}

static size_t type(parser *p) {
    if (!enter(p))
        return 0;
    ps_lang_token t = p->current;
    size_t id = 0;
    if (accept(p, PS_LANG_LBRACKET)) {
        newlines(p);
        size_t inner = type(p);
        newlines(p);
        expect(p, PS_LANG_RBRACKET, "Expected ']' after array element type");
        id = node(p, PS_AST_ARRAY_TYPE, t, inner, 0, 0);
    } else if (accept(p, PS_LANG_LPAREN)) {
        newlines(p);
        id = type(p);
        newlines(p);
        expect(p, PS_LANG_RPAREN, "Expected ')' after grouped type");
    } else if (accept(p, PS_LANG_FUNC)) {
        expect(p, PS_LANG_LPAREN, "Expected '(' after func in function type");
        size_t first = 0, last = 0;
        newlines(p);
        if (p->current.kind != PS_LANG_RPAREN) {
            do {
                newlines(p);
                append(p, &first, &last, type(p));
                newlines(p);
            } while (accept(p, PS_LANG_COMMA));
        }
        expect(p, PS_LANG_RPAREN, "Expected ')' after function parameter types");
        expect(p, PS_LANG_ARROW, "Expected '->' after function parameter types");
        newlines(p);
        size_t result = type(p);
        id = node(p, PS_AST_FUNCTION_TYPE, t, first, result, 0);
    } else if (expect(p, PS_LANG_IDENTIFIER, "Expected type name")) {
        id = node(p, PS_AST_TYPE, t, 0, 0, 0);
        if (accept(p, PS_LANG_DOT)) {
            ps_lang_token member = p->current;
            expect(p, PS_LANG_IDENTIFIER, "Expected imported type name after '.'");
            id = node(p, PS_AST_MEMBER, member, id, 0, 0);
        }
        if (accept(p, PS_LANG_LT)) {
            size_t first = 0, last = 0;
            do {
                append(p, &first, &last, type(p));
            } while (accept(p, PS_LANG_COMMA));
            expect_type_gt(p, "Expected '>' after generic type arguments");
            id = node(p, PS_AST_GENERIC_TYPE, t, id, first, 0);
        }
    }
    while (accept(p, PS_LANG_QUESTION))
        id = node(p, PS_AST_OPTIONAL_TYPE, t, id, 0, 0);
    p->depth--;
    return id;
}
static void parse_type_parameters(parser *p, size_t *first, size_t *last) {
    do {
        ps_lang_token name = p->current;
        expect(p, PS_LANG_IDENTIFIER, "Expected generic type parameter");
        size_t constraints = 0, last_constraint = 0;
        if (accept(p, PS_LANG_COLON)) {
            do {
                ps_lang_token constraint = p->current;
                expect(p, PS_LANG_IDENTIFIER, "Expected generic constraint name");
                append(p, &constraints, &last_constraint,
                       node(p, PS_AST_CONSTRAINT, constraint, 0, 0, 0));
            } while (accept(p, PS_LANG_AMP));
        }
        append(p, first, last,
               node(p, PS_AST_TYPE_PARAMETER, name, constraints, 0, 0));
    } while (accept(p, PS_LANG_COMMA));
}
static int precedence(ps_lang_kind k) {
    switch (k) {
    case PS_LANG_COALESCE:
        return 2;
    case PS_LANG_OR:
        return 3;
    case PS_LANG_AND:
        return 4;
    case PS_LANG_PIPE:
        return 5;
    case PS_LANG_CARET:
        return 6;
    case PS_LANG_AMP:
        return 7;
    case PS_LANG_EQ:
    case PS_LANG_NE:
        return 8;
    case PS_LANG_LT:
    case PS_LANG_LE:
    case PS_LANG_GT:
    case PS_LANG_GE:
        return 9;
    case PS_LANG_RANGE_CLOSED:
    case PS_LANG_RANGE_OPEN:
        return 10;
    case PS_LANG_SHIFT_LEFT:
    case PS_LANG_SHIFT_RIGHT:
        return 11;
    case PS_LANG_PLUS:
    case PS_LANG_MINUS:
        return 12;
    case PS_LANG_STAR:
    case PS_LANG_SLASH:
    case PS_LANG_PERCENT:
        return 13;
    default:
        return 0;
    }
}
static ps_lang_kind type_apply_ahead(parser *p) {
    parser probe = *p;
    if (!accept(&probe, PS_LANG_LT))
        return PS_LANG_EOF;
    do {
        if (!type(&probe) || probe.error.kind == PS_LANG_ERROR)
            return PS_LANG_EOF;
    } while (accept(&probe, PS_LANG_COMMA));
    if (!accept_type_gt(&probe))
        return PS_LANG_EOF;
    if (probe.current.kind == PS_LANG_LPAREN)
        return PS_LANG_LPAREN;
    /* A bare specialization is unambiguous only at an expression boundary. */
    switch (probe.current.kind) {
    case PS_LANG_DOT:
    case PS_LANG_EOF:
    case PS_LANG_NEWLINE:
    case PS_LANG_COMMA:
    case PS_LANG_RPAREN:
    case PS_LANG_RBRACKET:
        return PS_LANG_DOT;
    default:
        return PS_LANG_EOF;
    }
}
typedef struct string_position {
    size_t offset, line, column;
} string_position;
static ps_lang_token string_span(parser *p, ps_lang_token whole, string_position *position,
                                 size_t offset, size_t length, int multiline) {
    while (position->offset < offset) {
        unsigned char byte = p->lexer.source[position->offset++];
        if (byte == '\r') {
            if (position->offset < offset && p->lexer.source[position->offset] == '\n')
                position->offset++;
            position->line++;
            position->column = 1;
        } else if (byte == '\n') {
            position->line++;
            position->column = 1;
        } else
            position->column++;
    }
    whole.offset = offset;
    whole.length = length;
    whole.line = position->line;
    whole.column = position->column;
    whole.kind = multiline ? PS_LANG_STRING_PART_TRIPLE : PS_LANG_STRING;
    return whole;
}
static size_t interpolated_string(parser *p, ps_lang_token whole) {
    const unsigned char *source = p->lexer.source;
    int multiline = whole.length >= 6 && source[whole.offset + 1] == '"' &&
                    source[whole.offset + 2] == '"';
    size_t delimiter = multiline ? 3u : 1u;
    size_t end = whole.offset + whole.length - delimiter;
    size_t part_start = whole.offset + delimiter;
    size_t first = 0, last = 0;
    string_position position = {whole.offset, whole.line, whole.column};
    for (size_t at = part_start; at < end && p->error.kind != PS_LANG_ERROR;) {
        if (source[at] != '\\') {
            at++;
            continue;
        }
        if (at + 1 >= end || source[at + 1] != '(') {
            at += 2; /* The lexer already validated this escape. */
            continue;
        }
        if (at > part_start) {
            ps_lang_token part = string_span(p, whole, &position, part_start,
                                             at - part_start, multiline);
            append(p, &first, &last, node(p, PS_AST_STRING_SEGMENT, part, 0, 0, 0));
        }
        ps_lang_token start = string_span(p, whole, &position, at + 2, 0, multiline);
        parser inner = *p;
        ps_lang_lexer_init(&inner.lexer, source, p->lexer.size);
        inner.lexer.offset = start.offset;
        inner.lexer.line = start.line;
        inner.lexer.column = start.column;
        inner.lexer.interpolation_depth = p->depth;
        inner.current = (ps_lang_token){0};
        inner.indent_line = 0;
        inner.index_range_depth = 0;
        advance(&inner);
        size_t value = expression(&inner, 1, 1);
        if (inner.error.kind != PS_LANG_ERROR && inner.current.kind != PS_LANG_RPAREN)
            error(&inner, "Expected ')' after interpolated expression");
        p->count = inner.count;
        p->lambda_block_count = inner.lambda_block_count;
        if (inner.error.kind == PS_LANG_ERROR) {
            p->error = inner.error;
            return 0;
        }
        append(p, &first, &last, value);
        at = inner.current.offset + inner.current.length;
        part_start = at;
    }
    if (p->error.kind == PS_LANG_ERROR)
        return 0;
    if (!first)
        return node(p, PS_AST_LITERAL, whole, 0, 0, 0);
    if (part_start < end) {
        ps_lang_token part = string_span(p, whole, &position, part_start,
                                         end - part_start, multiline);
        append(p, &first, &last, node(p, PS_AST_STRING_SEGMENT, part, 0, 0, 0));
    }
    return node(p, PS_AST_INTERPOLATED_STRING, whole, first, 0, 0);
}
static size_t primary(parser *p, int multiline) {
    ps_lang_token t = p->current;
    size_t id = 0;
    if (t.kind == PS_LANG_MINUS || t.kind == PS_LANG_PLUS ||
        t.kind == PS_LANG_NOT || t.kind == PS_LANG_TILDE) {
        advance(p);
        newlines(p);
        size_t operand = expression(p, 14, multiline);
        id = node(p, PS_AST_UNARY, t, operand, 0, 0);
    } else if (t.kind == PS_LANG_INTEGER || t.kind == PS_LANG_FLOAT || t.kind == PS_LANG_STRING ||
               t.kind == PS_LANG_TRUE || t.kind == PS_LANG_FALSE || t.kind == PS_LANG_NIL) {
        advance(p);
        id = t.kind == PS_LANG_STRING ? interpolated_string(p, t)
                                      : node(p, PS_AST_LITERAL, t, 0, 0, 0);
    } else if (accept(p, PS_LANG_FUNC)) {
        size_t indentation = p->indent, first = 0, last = 0;
        expect(p, PS_LANG_LPAREN, "Expected '(' after anonymous func");
        newlines(p);
        if (p->current.kind != PS_LANG_RPAREN) {
            do {
                newlines(p);
                ps_lang_token param = p->current;
                expect(p, PS_LANG_IDENTIFIER, "Expected anonymous function parameter name");
                expect(p, PS_LANG_COLON, "Expected ':' and explicit parameter type");
                newlines(p);
                size_t annotation = type(p);
                append(p, &first, &last, node(p, PS_AST_PARAMETER, param, annotation, 0, 0));
                newlines(p);
            } while (accept(p, PS_LANG_COMMA));
        }
        expect(p, PS_LANG_RPAREN, "Expected ')' after anonymous function parameters");
        size_t result = 0;
        if (accept(p, PS_LANG_ARROW)) {
            newlines(p);
            result = type(p);
        }
        size_t body = block(p, indentation);
        p->lambda_block_count++;
        id = node(p, PS_AST_LAMBDA, t, first, result, body);
    } else if (accept(p, PS_LANG_IDENTIFIER)) {
        id = node(p, PS_AST_NAME, t, 0, 0, 0);
    } else if (accept(p, PS_LANG_LPAREN)) {
        newlines(p);
        id = expression(p, 1, 1);
        expect(p, PS_LANG_RPAREN, "Expected ')' after expression");
    } else if (accept(p, PS_LANG_LBRACKET)) {
        size_t first = 0, last = 0;
        newlines(p);
        if (p->current.kind != PS_LANG_RBRACKET) {
            do {
                newlines(p);
                if (p->current.kind == PS_LANG_RBRACKET)
                    break;
                append(p, &first, &last, expression(p, 1, 1));
            } while (accept(p, PS_LANG_COMMA));
        }
        expect(p, PS_LANG_RBRACKET, "Expected ']' after array elements");
        id = node(p, PS_AST_ARRAY, t, first, 0, 0);
    } else {
        error(p, "Expected expression");
    }
    for (;;) {
        if (multiline)
            newlines(p);
        t = p->current;
        size_t type_arguments = 0, last_type_argument = 0;
        ps_lang_kind type_apply = PS_LANG_EOF;
        if (id && t.kind == PS_LANG_LT &&
            (p->nodes[id].kind == PS_AST_NAME || p->nodes[id].kind == PS_AST_MEMBER) &&
            (type_apply = type_apply_ahead(p)) != PS_LANG_EOF) {
            advance(p);
            do {
                append(p, &type_arguments, &last_type_argument, type(p));
            } while (accept(p, PS_LANG_COMMA));
            expect_type_gt(p, "Expected '>' after explicit type arguments");
            t = p->current;
            if (type_apply == PS_LANG_DOT) {
                id = node(p, PS_AST_TYPE_APPLY, t, id, type_arguments, 0);
                continue;
            }
        }
        if (accept(p, PS_LANG_LPAREN)) {
            size_t first = 0, last = 0;
            newlines(p);
            if (p->current.kind != PS_LANG_RPAREN) {
                do {
                    newlines(p);
                    if (p->current.kind == PS_LANG_RPAREN)
                        break;
                    ps_lang_token label = p->current;
                    label.length = 0; /* Zero length means positional argument. */
                    if (p->current.kind == PS_LANG_IDENTIFIER ||
                        p->current.kind == PS_LANG_IN ||
                        p->current.kind == PS_LANG_WHILE) {
                        ps_lang_lexer lookahead = p->lexer;
                        if (ps_lang_lexer_next(&lookahead).kind == PS_LANG_COLON) {
                            label = p->current;
                            advance(p);
                            advance(p);
                            newlines(p);
                        }
                    }
                    size_t value = expression(p, 1, 1);
                    append(p, &first, &last, node(p, PS_AST_ARGUMENT, label, value, 0, 0));
                } while (accept(p, PS_LANG_COMMA));
            }
            expect(p, PS_LANG_RPAREN, "Expected ')' after arguments");
            id = node(p, PS_AST_CALL, t, id, first, type_arguments);
        } else if (accept(p, PS_LANG_DOT)) {
            ps_lang_token name = p->current;
            expect(p, PS_LANG_IDENTIFIER, "Expected member name after '.'");
            id = node(p, PS_AST_MEMBER, name, id, 0, 0);
        } else if (accept(p, PS_LANG_LBRACKET)) {
            newlines(p);
            p->index_range_depth++;
            size_t index;
            if (p->current.kind == PS_LANG_RANGE_OPEN ||
                p->current.kind == PS_LANG_RANGE_CLOSED) {
                ps_lang_token range = p->current;
                advance(p);
                newlines(p);
                size_t end = p->current.kind == PS_LANG_RBRACKET || omitted_end_before_by(p)
                                 ? 0 : expression(p, 12, 1);
                index = node(p, PS_AST_BINARY, range, 0, end, 0);
                if (contextual_by(p)) {
                    ps_lang_token by = p->current;
                    advance(p);
                    newlines(p);
                    size_t step = expression(p, 12, 1);
                    index = node(p, PS_AST_STRIDED_RANGE, by, index, step, 0);
                }
            } else
                index = expression(p, 1, 1);
            p->index_range_depth--;
            expect(p, PS_LANG_RBRACKET, "Expected ']' after index");
            id = node(p, PS_AST_INDEX, t, id, index, 0);
        } else {
            break;
        }
    }
    return id;
}
static size_t expression(parser *p, int minimum, int multiline) {
    if (!enter(p))
        return 0;
    size_t left = primary(p, multiline);
    for (;;) {
        if (multiline)
            newlines(p);
        if (left && p->nodes[left].kind == PS_AST_BINARY &&
            (p->nodes[left].token.kind == PS_LANG_RANGE_OPEN ||
             p->nodes[left].token.kind == PS_LANG_RANGE_CLOSED) && contextual_by(p)) {
            ps_lang_token by = p->current;
            advance(p);
            newlines(p);
            size_t step = expression(p, 12, multiline);
            left = node(p, PS_AST_STRIDED_RANGE, by, left, step, 0);
            continue;
        }
        int coalesce = p->current.kind == PS_LANG_QUESTION &&
                       p->current.offset + 1 < p->lexer.size &&
                       p->lexer.source[p->current.offset + 1] == '?';
        if (p->error.kind != PS_LANG_ERROR && p->current.kind == PS_LANG_QUESTION &&
            !coalesce && minimum <= 1) {
            ps_lang_token question = p->current;
            advance(p);
            newlines(p);
            size_t yes = expression(p, 1, multiline);
            expect(p, PS_LANG_COLON, "Expected ':' in conditional expression");
            newlines(p);
            size_t no = expression(p, 1, multiline);
            left = node(p, PS_AST_CONDITIONAL, question, left, yes, no);
            continue;
        }
        int prec = precedence(coalesce ? PS_LANG_COALESCE : p->current.kind);
        if (!prec || prec < minimum || p->error.kind == PS_LANG_ERROR)
            break;
        ps_lang_token op = p->current;
        if (coalesce) {
            op.kind = PS_LANG_COALESCE;
            op.length = 2;
        }
        advance(p);
        if (coalesce)
            expect(p, PS_LANG_QUESTION, "Expected second '?' in coalescing operator");
        newlines(p); /* An operator explicitly continues onto the next line. */
        size_t right = p->index_range_depth &&
                               (p->current.kind == PS_LANG_RBRACKET || omitted_end_before_by(p)) &&
                               (op.kind == PS_LANG_RANGE_OPEN || op.kind == PS_LANG_RANGE_CLOSED)
                           ? 0
                            : expression(p, prec + (coalesce ? 0 : 1), multiline);
        left = node(p, PS_AST_BINARY, op, left, right, 0);
    }
    p->depth--;
    return left;
}
static size_t block(parser *p, size_t parent_indent) {
    ps_lang_token t = p->current;
    if (!expect(p, PS_LANG_COLON, "Expected ':' to start an indented block") ||
        !expect(p, PS_LANG_NEWLINE, "Expected newline after ':'"))
        return 0;
    size_t first = 0, last = 0;
    newlines(p);
    if (p->current.kind == PS_LANG_EOF || p->indent <= parent_indent) {
        error(p, "Expected an indented block");
        return 0;
    }
    size_t body_indent = p->indent;
    while (p->current.kind != PS_LANG_EOF && p->indent > parent_indent &&
           p->error.kind != PS_LANG_ERROR) {
        if (p->indent != body_indent) {
            error(p, "Unexpected indentation or inconsistent dedent");
            break;
        }
        append(p, &first, &last, statement(p));
        separators(p);
    }
    return node(p, PS_AST_BLOCK, t, first, 0, 0);
}
static size_t function(parser *p, size_t parent_indent, int method);
static size_t conditional_condition(parser *p) {
    if (p->current.kind != PS_LANG_LET && p->current.kind != PS_LANG_VAR)
        return expression(p, 1, 0);
    ps_lang_kind binding_kind = p->current.kind;
    advance(p);
    ps_lang_token name = p->current;
    expect(p, PS_LANG_IDENTIFIER, "Expected conditional binding name");
    size_t annotation = accept(p, PS_LANG_COLON) ? type(p) : 0;
    expect(p, PS_LANG_EQUAL, "Expected '=' and optional initializer");
    newlines(p);
    size_t value = expression(p, 1, 0);
    name.kind = binding_kind;
    return node(p, PS_AST_OPTIONAL_BINDING, name, value, annotation, 0);
}
static size_t record(parser *p, size_t parent_indent) {
    ps_lang_token name = p->current;
    expect(p, PS_LANG_IDENTIFIER, "Expected struct name");
    size_t template_start = p->count, type_parameters = 0, last_type_parameter = 0;
    int generic = accept(p, PS_LANG_LT);
    if (generic) {
        parse_type_parameters(p, &type_parameters, &last_type_parameter);
        expect_type_gt(p, "Expected '>' after generic type parameters");
    }
    if (!expect(p, PS_LANG_COLON, "Expected ':' after struct name") ||
        !expect(p, PS_LANG_NEWLINE, "Expected newline after ':'"))
        return 0;
    newlines(p);
    if (p->current.kind == PS_LANG_EOF || p->indent <= parent_indent) {
        error(p, "Expected indented struct fields");
        return 0;
    }
    size_t indentation = p->indent, first = 0, last = 0;
    size_t methods = 0, last_method = 0;
    while (p->current.kind != PS_LANG_EOF && p->indent > parent_indent &&
           p->error.kind != PS_LANG_ERROR) {
        if (p->indent != indentation) {
            error(p, "Inconsistent struct field indentation");
            break;
        }
        ps_lang_kind mutability = p->current.kind;
        if (accept(p, PS_LANG_STATIC)) {
            expect(p, PS_LANG_FUNC, "Expected func after static");
            append(p, &methods, &last_method, function(p, indentation, 3));
            separators(p);
            continue;
        }
        if (accept(p, PS_LANG_MUTATING)) {
            expect(p, PS_LANG_FUNC, "Expected func after mutating");
            append(p, &methods, &last_method, function(p, indentation, 2));
            separators(p);
            continue;
        }
        if (accept(p, PS_LANG_FUNC)) {
            append(p, &methods, &last_method, function(p, indentation, 1));
            separators(p);
            continue;
        }
        if (mutability != PS_LANG_LET && mutability != PS_LANG_VAR) {
            error(p, "Expected let or var field declaration");
            break;
        }
        advance(p);
        ps_lang_token field = p->current;
        expect(p, PS_LANG_IDENTIFIER, "Expected field name");
        expect(p, PS_LANG_COLON, "Expected explicit field type");
        size_t annotation = type(p);
        size_t initializer = accept(p, PS_LANG_EQUAL) ? expression(p, 1, 0) : 0;
        field.kind = mutability;
        append(p, &first, &last, node(p, PS_AST_FIELD, field, annotation, initializer, 0));
        if (p->current.kind != PS_LANG_NEWLINE && p->current.kind != PS_LANG_SEMICOLON &&
            p->current.kind != PS_LANG_EOF)
            error(p, "Expected newline after struct field");
        separators(p);
    }
    size_t signature = generic ? node(p, PS_AST_GENERIC_SIGNATURE, name,
                                      type_parameters, 0, template_start) : 0;
    return node(p, generic ? PS_AST_GENERIC_STRUCT : PS_AST_STRUCT,
                name, first, methods, signature);
}
static size_t enumeration(parser *p, size_t parent_indent) {
    ps_lang_token name = p->current;
    expect(p, PS_LANG_IDENTIFIER, "Expected enum name");
    size_t template_start = p->count, type_parameters = 0, last_type_parameter = 0;
    int generic = accept(p, PS_LANG_LT);
    if (generic) {
        parse_type_parameters(p, &type_parameters, &last_type_parameter);
        expect_type_gt(p, "Expected '>' after generic type parameters");
    }
    if (!expect(p, PS_LANG_COLON, "Expected ':' after enum name") ||
        !expect(p, PS_LANG_NEWLINE, "Expected newline after ':'"))
        return 0;
    newlines(p);
    if (p->current.kind == PS_LANG_EOF || p->indent <= parent_indent) {
        error(p, "Expected indented enum cases");
        return 0;
    }
    size_t indentation = p->indent, first = 0, last = 0;
    size_t methods = 0, last_method = 0;
    while (p->current.kind != PS_LANG_EOF && p->indent > parent_indent &&
           p->error.kind != PS_LANG_ERROR) {
        if (p->indent != indentation) {
            error(p, "Inconsistent enum case indentation");
            break;
        }
        if (accept(p, PS_LANG_STATIC)) {
            expect(p, PS_LANG_FUNC, "Expected func after static");
            append(p, &methods, &last_method, function(p, indentation, 3));
            separators(p);
            continue;
        }
        if (accept(p, PS_LANG_MUTATING)) {
            expect(p, PS_LANG_FUNC, "Expected func after mutating");
            append(p, &methods, &last_method, function(p, indentation, 2));
            separators(p);
            continue;
        }
        if (accept(p, PS_LANG_FUNC)) {
            append(p, &methods, &last_method, function(p, indentation, 1));
            separators(p);
            continue;
        }
        expect(p, PS_LANG_CASE, "Expected case declaration");
        ps_lang_token item = p->current;
        expect(p, PS_LANG_IDENTIFIER, "Expected enum case name");
        size_t fields = 0, last_field = 0;
        if (accept(p, PS_LANG_LPAREN)) {
            do {
                ps_lang_token field = p->current;
                expect(p, PS_LANG_IDENTIFIER, "Expected enum payload field name");
                expect(p, PS_LANG_COLON, "Expected ':' after enum payload field name");
                size_t annotation = type(p);
                append(p, &fields, &last_field,
                       node(p, PS_AST_FIELD, field, annotation, 0, 0));
            } while (accept(p, PS_LANG_COMMA));
            expect(p, PS_LANG_RPAREN, "Expected ')' after enum payload fields");
        }
        size_t raw = 0;
        if (accept(p, PS_LANG_EQUAL))
            raw = expression(p, 1, 0);
        append(p, &first, &last, node(p, PS_AST_CASE, item, fields, raw, 0));
        if (p->current.kind != PS_LANG_NEWLINE && p->current.kind != PS_LANG_EOF)
            error(p, "Expected newline after enum case");
        separators(p);
    }
    size_t signature = generic ? node(p, PS_AST_GENERIC_SIGNATURE, name,
                                      type_parameters, 0, template_start) : 0;
    return node(p, generic ? PS_AST_GENERIC_ENUM : PS_AST_ENUM,
                name, first, methods, signature);
}
static size_t selection(parser *p, ps_lang_token keyword, size_t parent_indent) {
    size_t value = expression(p, 1, 0);
    if (!expect(p, PS_LANG_COLON, "Expected ':' after switch value") ||
        !expect(p, PS_LANG_NEWLINE, "Expected newline after ':'"))
        return 0;
    newlines(p);
    if (p->current.kind == PS_LANG_EOF || p->indent <= parent_indent) {
        error(p, "Expected indented switch cases");
        return 0;
    }
    size_t indentation = p->indent, first = 0, last = 0;
    int had_default = 0;
    while (p->current.kind != PS_LANG_EOF && p->indent > parent_indent &&
           p->error.kind != PS_LANG_ERROR) {
        if (p->indent != indentation) {
            error(p, "Inconsistent switch case indentation");
            break;
        }
        if (had_default) {
            error(p, "Default must be the last switch case");
            break;
        }
        ps_lang_token arm = p->current;
        size_t pattern = 0, last_pattern = 0;
        if (accept(p, PS_LANG_CASE)) {
            do {
                append(p, &pattern, &last_pattern, expression(p, 1, 0));
            } while (accept(p, PS_LANG_COMMA));
        }
        else if (accept(p, PS_LANG_DEFAULT))
            had_default = 1;
        else {
            error(p, "Expected case or default in switch");
            break;
        }
        size_t guard = 0;
        if (pattern && accept(p, PS_LANG_IF))
            guard = expression(p, 1, 0);
        size_t body = block(p, indentation);
        append(p, &first, &last, node(p, PS_AST_SWITCH_ARM, arm, pattern, body, guard));
        separators(p);
    }
    return node(p, PS_AST_SWITCH, keyword, value, first, 0);
}
static size_t function(parser *p, size_t parent_indent, int method) {
    ps_lang_token name = p->current;
    expect(p, PS_LANG_IDENTIFIER, "Expected function name");
    size_t template_start = p->count, type_parameters = 0, last_type_parameter = 0;
    int generic = accept(p, PS_LANG_LT);
    if (generic) {
        parse_type_parameters(p, &type_parameters, &last_type_parameter);
        expect_type_gt(p, "Expected '>' after generic type parameters");
    }
    expect(p, PS_LANG_LPAREN, "Expected '(' after function name");
    size_t first = 0, last = 0;
    if (method == 3)
        name.kind = PS_LANG_STATIC;
    if (method == 1 || method == 2) {
        ps_lang_token receiver = name;
        receiver.kind = method == 2 ? PS_LANG_VAR : PS_LANG_LET;
        append(p, &first, &last, node(p, PS_AST_SELF_PARAMETER, receiver, 0, 0, 0));
    }
    newlines(p);
    if (p->current.kind != PS_LANG_RPAREN) {
        do {
            newlines(p);
            if (p->current.kind == PS_LANG_RPAREN)
                break;
            ps_lang_token param = p->current;
            expect(p, PS_LANG_IDENTIFIER, "Expected parameter name");
            expect(p, PS_LANG_COLON, "Expected ':' and explicit parameter type");
            newlines(p);
            size_t annotation = type(p);
            append(p, &first, &last, node(p, PS_AST_PARAMETER, param, annotation, 0, 0));
            newlines(p);
        } while (accept(p, PS_LANG_COMMA));
    }
    expect(p, PS_LANG_RPAREN, "Expected ')' after parameters");
    size_t result = 0;
    if (accept(p, PS_LANG_ARROW)) {
        newlines(p);
        result = type(p);
    }
    if (generic)
        result = node(p, PS_AST_GENERIC_SIGNATURE, name, type_parameters, result,
                      template_start);
    size_t body = block(p, parent_indent);
    return node(p, generic ? PS_AST_GENERIC_FUNCTION : PS_AST_FUNCTION,
                name, first, result, body);
}
static size_t statement(parser *p) {
    if (!enter(p))
        return 0;
    ps_lang_token t = p->current;
    size_t statement_indent = p->indent;
    unsigned lambda_blocks_before = p->lambda_block_count;
    size_t id = 0;
    int compound = 0;
    if (accept(p, PS_LANG_FUNC)) {
        id = function(p, statement_indent, 0);
        if (statement_indent && id && p->nodes[id].kind == PS_AST_FUNCTION)
            p->nodes[id].kind = PS_AST_LOCAL_FUNCTION;
        compound = 1;
    } else if (accept(p, PS_LANG_STRUCT)) {
        id = record(p, statement_indent);
        compound = 1;
    } else if (accept(p, PS_LANG_ENUM)) {
        id = enumeration(p, statement_indent);
        compound = 1;
    } else if (accept(p, PS_LANG_SWITCH)) {
        id = selection(p, t, statement_indent);
        compound = 1;
    } else if (t.kind == PS_LANG_LET || t.kind == PS_LANG_VAR) {
        advance(p);
        ps_lang_token name = p->current;
        expect(p, PS_LANG_IDENTIFIER, "Expected variable name");
        size_t annotation = 0;
        if (accept(p, PS_LANG_COLON))
            annotation = type(p);
        size_t value = 0;
        if (accept(p, PS_LANG_EQUAL)) {
            newlines(p);
            value = expression(p, 1, 0);
        } else if (t.kind != PS_LANG_VAR || !annotation)
            error(p, "Variable without initializer requires 'var' and a type annotation");
        /* token.kind preserves let/var, span still points to the name. */
        name.kind = t.kind;
        id = node(p, PS_AST_VARIABLE, name, annotation, value, 0);
    } else if (accept(p, PS_LANG_RETURN)) {
        size_t value = 0;
        if (p->current.kind != PS_LANG_NEWLINE && p->current.kind != PS_LANG_SEMICOLON &&
            p->current.kind != PS_LANG_EOF)
            value = expression(p, 1, 0);
        id = node(p, PS_AST_RETURN, t, value, 0, 0);
    } else if (t.kind == PS_LANG_IF || t.kind == PS_LANG_WHILE) {
        advance(p);
        size_t condition = conditional_condition(p);
        size_t body = block(p, statement_indent), alternative = 0;
        if (t.kind == PS_LANG_IF) {
            newlines(p);
            if (p->indent == statement_indent && accept(p, PS_LANG_ELSE)) {
                alternative =
                    p->current.kind == PS_LANG_IF ? statement(p) : block(p, statement_indent);
            }
        }
        id = node(p, t.kind == PS_LANG_IF ? PS_AST_IF : PS_AST_WHILE, t, condition, body,
                  alternative);
        compound = 1;
    } else if (accept(p, PS_LANG_GUARD)) {
        size_t condition = conditional_condition(p);
        expect(p, PS_LANG_ELSE, "Expected 'else' after guard condition");
        size_t alternative = block(p, statement_indent);
        id = node(p, PS_AST_GUARD, t, condition, alternative, 0);
        compound = 1;
    } else if (accept(p, PS_LANG_FOR)) {
        ps_lang_token name = p->current;
        expect(p, PS_LANG_IDENTIFIER, "Expected iteration variable");
        expect(p, PS_LANG_IN, "Expected 'in' after iteration variable");
        size_t iterable = expression(p, 1, 0);
        size_t body = block(p, statement_indent);
        id = node(p, PS_AST_FOR, name, iterable, body, 0);
        compound = 1;
    } else if (t.kind == PS_LANG_BREAK || t.kind == PS_LANG_CONTINUE) {
        advance(p);
        id = node(p, t.kind == PS_LANG_BREAK ? PS_AST_BREAK : PS_AST_CONTINUE, t, 0, 0, 0);
    } else {
        size_t left = expression(p, 1, 0);
        ps_lang_token assignment = p->current;
        if (assignment.kind == PS_LANG_EQUAL ||
            (assignment.kind >= PS_LANG_PLUS_EQUAL &&
             assignment.kind <= PS_LANG_SHIFT_RIGHT_EQUAL)) {
            advance(p);
            newlines(p);
            size_t right = expression(p, 1, 0);
            ps_lang_kind assignment_kind = assignment.kind;
            if (assignment.kind != PS_LANG_EQUAL) {
                static const ps_lang_kind operators[] = {
                    PS_LANG_PLUS, PS_LANG_MINUS, PS_LANG_STAR, PS_LANG_SLASH, PS_LANG_PERCENT,
                    PS_LANG_AMP, PS_LANG_PIPE, PS_LANG_CARET,
                    PS_LANG_SHIFT_LEFT, PS_LANG_SHIFT_RIGHT};
                assignment.kind = operators[assignment.kind - PS_LANG_PLUS_EQUAL];
                /* Reuse the target node so code generation can snapshot it once
                 * before evaluating the compound assignment operand. */
                right = node(p, PS_AST_BINARY, assignment, left, right, 0);
            }
            assignment.kind = assignment_kind;
            id = node(p, PS_AST_ASSIGN, assignment, left, right, 0);
        } else {
            id = node(p, PS_AST_EXPRESSION, t, left, 0, 0);
        }
    }
    if (!compound && lambda_blocks_before == p->lambda_block_count &&
        p->current.kind != PS_LANG_NEWLINE && p->current.kind != PS_LANG_SEMICOLON &&
        p->current.kind != PS_LANG_EOF)
        error(p, "Expected newline or ';' after statement");
    p->depth--;
    return id;
}
ps_lang_parse_result ps_lang_parse(const void *source, size_t size, ps_lang_node *nodes,
                                   size_t capacity) {
    parser p = {0};
    p.nodes = nodes;
    p.capacity = capacity;
    p.count = capacity && nodes ? 1 : 0;
    ps_lang_lexer_init(&p.lexer, source, size);
    advance(&p);
    if (!nodes || capacity < 2)
        error(&p, "Syntax node buffer requires at least two slots");
    else
        memset(&nodes[0], 0, sizeof(nodes[0]));
    ps_lang_token start = p.current;
    size_t first = 0, last = 0;
    separators(&p);
    while (p.current.kind != PS_LANG_EOF && p.error.kind != PS_LANG_ERROR) {
        if (p.indent != 0) {
            error(&p, "Unexpected indentation at module scope");
            break;
        }
        if (p.current.kind == PS_LANG_IMPORT) {
            advance(&p);
            ps_lang_token name = p.current;
            expect(&p, PS_LANG_IDENTIFIER, "Expected module name after import");
            ps_lang_token path = name;
            while (accept(&p, PS_LANG_DOT)) {
                name = p.current;
                if (!expect(&p, PS_LANG_IDENTIFIER, "Expected module name after '.'"))
                    break;
                path.length = name.offset + name.length - path.offset;
            }
            if (p.current.kind == PS_LANG_IDENTIFIER && p.current.length == 2 &&
                !memcmp(p.lexer.source + p.current.offset, "as", 2)) {
                advance(&p);
                name = p.current;
                expect(&p, PS_LANG_IDENTIFIER, "Expected module alias after 'as'");
            }
            if (p.current.kind != PS_LANG_NEWLINE && p.current.kind != PS_LANG_SEMICOLON &&
                p.current.kind != PS_LANG_EOF)
                error(&p, "Expected newline or ';' after import");
            size_t qualified = path.offset != name.offset || path.length != name.length
                                   ? node(&p, PS_AST_NAME, path, 0, 0, 0) : 0;
            append(&p, &first, &last, node(&p, PS_AST_IMPORT, name, 0, qualified, 0));
        } else
            append(&p, &first, &last, statement(&p));
        separators(&p);
    }
    size_t root = node(&p, PS_AST_MODULE, start, first, 0, 0);
    return (ps_lang_parse_result){p.error.kind == PS_LANG_ERROR ? 0 : root, p.count, p.error};
}
