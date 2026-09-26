#ifndef PS_LANGUAGE_PARSER_H
#define PS_LANGUAGE_PARSER_H
#include "lexer.h"

/* Zero is the absent-node index. Nodes and source are owned by the caller.
 * Node token gives the relevant source location (operator/name/keyword).
 * a/b/c contain child indices; next links siblings, never expression operands. */
typedef enum ps_lang_node_kind {
    PS_AST_MODULE,
    PS_AST_IMPORT, /* token is local alias; a=loaded root, b=optional qualified path name. */
    PS_AST_BLOCK,
    PS_AST_TYPE,
    PS_AST_ARRAY_TYPE,
    PS_AST_OPTIONAL_TYPE,
    PS_AST_FUNCTION_TYPE, /* a=parameter types, b=result type. */
    PS_AST_GENERIC_TYPE, /* a=generic nominal type name, b=type arguments. */
    PS_AST_RESOLVED_TYPE, /* Checker-created concrete annotation in a specialization. */
    PS_AST_FUNCTION,
    PS_AST_LOCAL_FUNCTION, /* Named function declaration inside a block. */
    PS_AST_LAMBDA, /* a=typed parameters, b=result type, c=indented body. */
    PS_AST_CAPTURE, /* Checker-created capture; binding is the source declaration. */
    PS_AST_GENERIC_FUNCTION, /* a=parameters, b=generic signature, c=body. */
    PS_AST_GENERIC_SIGNATURE, /* a=type parameters, b=result type, c=first template node. */
    PS_AST_TYPE_PARAMETER, /* a=constraint list. */
    PS_AST_CONSTRAINT,
    PS_AST_PARAMETER,
    PS_AST_SELF_PARAMETER,
    PS_AST_VARIABLE,
    PS_AST_OPTIONAL_BINDING, /* token is let/var name; a=initializer, b=payload annotation. */
    PS_AST_RETURN,
    PS_AST_IF,
    PS_AST_GUARD, /* a=Bool or optional binding, b=else block that returns. */
    PS_AST_WHILE,
    PS_AST_FOR,
    PS_AST_BREAK,
    PS_AST_CONTINUE,
    PS_AST_EXPRESSION,
    PS_AST_ASSIGN, /* token is = or a compound assignment operator. */
    PS_AST_LITERAL,
    PS_AST_STRING_SEGMENT, /* Raw bytes within one string literal, without delimiters. */
    PS_AST_INTERPOLATED_STRING, /* a=ordered String segments and embedded expressions. */
    PS_AST_NAME,
    PS_AST_UNARY,
    PS_AST_BINARY,
    PS_AST_CONDITIONAL, /* a=Bool condition, b=true value, c=false value. */
    PS_AST_STRIDED_RANGE, /* a=range expression, b=Int64 stride expression. */
    PS_AST_CALL, /* a=callee, b=arguments, c=explicit type arguments. */
    PS_AST_ARGUMENT,
    PS_AST_MEMBER,
    PS_AST_TYPE_APPLY, /* a=generic nominal type name, b=type arguments; used for members. */
    PS_AST_INDEX,
    PS_AST_ARRAY,
    PS_AST_STRUCT,
    PS_AST_GENERIC_STRUCT, /* a=fields, b=methods, c=generic signature. */
    PS_AST_GENERIC_ENUM, /* a=cases, b=methods, c=generic signature. */
    PS_AST_FIELD, /* a=type annotation, b=optional initializer for struct fields. */
    PS_AST_ENUM, /* a=cases, b=methods. */
    PS_AST_CASE, /* a=payload fields, b=optional explicit raw integer expression. */
    PS_AST_SWITCH,
    PS_AST_SWITCH_ARM /* a=patterns, b=body, c=optional Bool guard. */
} ps_lang_node_kind;

typedef struct ps_lang_node {
    ps_lang_node_kind kind;
    ps_lang_token token;
    size_t a, b, c, next;
} ps_lang_node;

typedef struct ps_lang_parse_result {
    size_t root, count;       /* root is zero on failure; partial nodes are not a valid AST. */
    ps_lang_token diagnostic; /* ERROR with static message on failure; zero otherwise. */
} ps_lang_parse_result;

/* capacity includes the reserved slot zero. No allocation or I/O. */
ps_lang_parse_result ps_lang_parse(const void *source, size_t size, ps_lang_node *nodes,
                                   size_t capacity);
#endif
