#ifndef PS_LANGUAGE_CHECKER_H
#define PS_LANGUAGE_CHECKER_H
#include "parser.h"
#include <stdint.h>

#define PS_LANG_BUILTIN_PRINT SIZE_MAX
#define PS_LANG_BUILTIN_ASSERT (SIZE_MAX - 1)
#define PS_LANG_BUILTIN_INT64 (SIZE_MAX - 2)
#define PS_LANG_BUILTIN_FLOAT64 (SIZE_MAX - 3)
#define PS_LANG_BUILTIN_ARRAY_COUNT (SIZE_MAX - 4)
#define PS_LANG_BUILTIN_ARRAY_APPEND (SIZE_MAX - 7)
#define PS_LANG_BUILTIN_ARRAY_REMOVE (SIZE_MAX - 8)
#define PS_LANG_BUILTIN_OPTIONAL_SOME (SIZE_MAX - 9)
#define PS_LANG_BUILTIN_OPTIONAL_UNWRAP (SIZE_MAX - 10)
#define PS_LANG_BUILTIN_OPTIONAL_OR (SIZE_MAX - 11)
#define PS_LANG_BUILTIN_OPTIONAL_HAS (SIZE_MAX - 12)
#define PS_LANG_BUILTIN_ENUM_RAW_VALUE (SIZE_MAX - 13)
#define PS_LANG_BUILTIN_ENUM_FROM_RAW (SIZE_MAX - 14)
#define PS_LANG_BUILTIN_ARRAY_INSERT (SIZE_MAX - 15)
#define PS_LANG_BUILTIN_ARRAY_CONTAINS (SIZE_MAX - 17)
#define PS_LANG_BUILTIN_ARRAY_FIRST_INDEX (SIZE_MAX - 18)
#define PS_LANG_BUILTIN_STRING_COUNT (SIZE_MAX - 19)
#define PS_LANG_BUILTIN_STRING_BYTE_COUNT (SIZE_MAX - 20)
#define PS_LANG_BUILTIN_STRING_IS_EMPTY (SIZE_MAX - 21)
#define PS_LANG_BUILTIN_STRING_CONTAINS (SIZE_MAX - 22)
#define PS_LANG_BUILTIN_STRING_FIRST_INDEX (SIZE_MAX - 23)
#define PS_LANG_BUILTIN_STRING_LAST_INDEX (SIZE_MAX - 24)
#define PS_LANG_BUILTIN_STRING_HAS_PREFIX (SIZE_MAX - 25)
#define PS_LANG_BUILTIN_STRING_HAS_SUFFIX (SIZE_MAX - 26)
#define PS_LANG_BUILTIN_STRING_REPLACING (SIZE_MAX - 27)
#define PS_LANG_BUILTIN_STRING_SPLIT (SIZE_MAX - 28)
#define PS_LANG_BUILTIN_STRING_JOINED (SIZE_MAX - 29)
#define PS_LANG_BUILTIN_STRING_CAST (SIZE_MAX - 30)
#define PS_LANG_BUILTIN_ARRAY_LAST_INDEX (SIZE_MAX - 31)
#define PS_LANG_BUILTIN_ARRAY_REVERSED (SIZE_MAX - 32)
#define PS_LANG_BUILTIN_ARRAY_REVERSE (SIZE_MAX - 33)
#define PS_LANG_BUILTIN_STRING_REVERSED (SIZE_MAX - 34)
#define PS_LANG_BUILTIN_INDIRECT_CALL (SIZE_MAX - 35)
#define PS_LANG_BUILTIN_ARRAY_FILTER (SIZE_MAX - 36)
#define PS_LANG_BUILTIN_ARRAY_MAP (SIZE_MAX - 37)
#define PS_LANG_BUILTIN_ARRAY_REDUCE (SIZE_MAX - 38)
#define PS_LANG_BUILTIN_ARRAY_ANY (SIZE_MAX - 39)
#define PS_LANG_BUILTIN_ARRAY_ALL (SIZE_MAX - 40)
#define PS_LANG_BUILTIN_ARRAY_SORTED (SIZE_MAX - 41)
#define PS_LANG_BUILTIN_ARRAY_SORT (SIZE_MAX - 42)
#define PS_LANG_BUILTIN_STRING_REPEATED (SIZE_MAX - 43)
#define PS_LANG_BUILTIN_ARRAY_REPEATED (SIZE_MAX - 44)
#define PS_LANG_BUILTIN_STRING_TRIMMED (SIZE_MAX - 45)
#define PS_LANG_BUILTIN_INT64_PARSE (SIZE_MAX - 46)
#define PS_LANG_BUILTIN_FLOAT64_PARSE (SIZE_MAX - 47)
#define PS_LANG_BUILTIN_ATTEMPT (SIZE_MAX - 49)
#define PS_LANG_BUILTIN_ARRAY_POP_LAST (SIZE_MAX - 52)
#define PS_LANG_BUILTIN_ARRAY_IS_EMPTY (SIZE_MAX - 53)
#define PS_LANG_BUILTIN_ARRAY_FIRST (SIZE_MAX - 54)
#define PS_LANG_BUILTIN_ARRAY_LAST (SIZE_MAX - 55)
#define PS_LANG_BUILTIN_ARRAY_FIRST_WHERE (SIZE_MAX - 56)
#define PS_LANG_BUILTIN_ARRAY_LAST_WHERE (SIZE_MAX - 57)
#define PS_LANG_BUILTIN_ARRAY_FIRST_INDEX_WHERE (SIZE_MAX - 58)
#define PS_LANG_BUILTIN_ARRAY_LAST_INDEX_WHERE (SIZE_MAX - 59)
#define PS_LANG_BUILTIN_ARRAY_SORTED_BY (SIZE_MAX - 60)
#define PS_LANG_BUILTIN_ARRAY_SORT_BY (SIZE_MAX - 61)
#define PS_LANG_BUILTIN_ARRAY_MIN (SIZE_MAX - 62)
#define PS_LANG_BUILTIN_ARRAY_MAX (SIZE_MAX - 63)
#define PS_LANG_BUILTIN_ARRAY_MIN_BY (SIZE_MAX - 64)
#define PS_LANG_BUILTIN_ARRAY_MAX_BY (SIZE_MAX - 65)
#define PS_LANG_BUILTIN_ARRAY_REMOVE_ALL (SIZE_MAX - 66)
#define PS_LANG_BUILTIN_ARRAY_REMOVE_ALL_WHERE (SIZE_MAX - 67)
#define PS_LANG_BUILTIN_ARRAY_APPEND_CONTENTS (SIZE_MAX - 68)
#define PS_LANG_BUILTIN_ARRAY_INSERT_CONTENTS (SIZE_MAX - 69)
#define PS_LANG_BUILTIN_ARRAY_REMOVE_FIRST (SIZE_MAX - 70)
#define PS_LANG_BUILTIN_ARRAY_REMOVE_LAST (SIZE_MAX - 71)
#define PS_LANG_BUILTIN_ARRAY_REMOVE_FIRST_COUNT (SIZE_MAX - 72)
#define PS_LANG_BUILTIN_ARRAY_REMOVE_LAST_COUNT (SIZE_MAX - 73)
#define PS_LANG_BUILTIN_ARRAY_SWAP_AT (SIZE_MAX - 74)
#define PS_LANG_BUILTIN_ARRAY_REMOVE_SUBRANGE (SIZE_MAX - 75)
#define PS_LANG_BUILTIN_ARRAY_REPLACE_SUBRANGE (SIZE_MAX - 76)
#define PS_LANG_BUILTIN_ARRAY_PREFIX (SIZE_MAX - 77)
#define PS_LANG_BUILTIN_ARRAY_SUFFIX (SIZE_MAX - 78)
#define PS_LANG_BUILTIN_ARRAY_DROP_FIRST (SIZE_MAX - 79)
#define PS_LANG_BUILTIN_ARRAY_DROP_LAST (SIZE_MAX - 80)
#define PS_LANG_BUILTIN_ARRAY_COMPACT_MAP (SIZE_MAX - 81)
#define PS_LANG_BUILTIN_ARRAY_FLAT_MAP (SIZE_MAX - 82)
#define PS_LANG_BUILTIN_ARRAY_PREFIX_WHILE (SIZE_MAX - 83)
#define PS_LANG_BUILTIN_ARRAY_DROP_WHILE (SIZE_MAX - 84)
#define PS_LANG_BUILTIN_STRING_PREFIX_WHILE (SIZE_MAX - 85)
#define PS_LANG_BUILTIN_STRING_DROP_WHILE (SIZE_MAX - 86)
#define PS_LANG_BUILTIN_STRING_FILTER (SIZE_MAX - 87)
#define PS_LANG_BUILTIN_STRING_ANY (SIZE_MAX - 88)
#define PS_LANG_BUILTIN_STRING_ALL (SIZE_MAX - 89)
#define PS_LANG_BUILTIN_STRING_FIRST_WHERE (SIZE_MAX - 90)
#define PS_LANG_BUILTIN_STRING_LAST_WHERE (SIZE_MAX - 91)
#define PS_LANG_BUILTIN_STRING_FIRST_INDEX_WHERE (SIZE_MAX - 92)
#define PS_LANG_BUILTIN_STRING_LAST_INDEX_WHERE (SIZE_MAX - 93)
#define PS_LANG_BUILTIN_STRING_MAP (SIZE_MAX - 94)
#define PS_LANG_BUILTIN_STRING_COMPACT_MAP (SIZE_MAX - 95)
#define PS_LANG_BUILTIN_STRING_FLAT_MAP (SIZE_MAX - 96)
#define PS_LANG_BUILTIN_STRING_REDUCE (SIZE_MAX - 97)
#define PS_LANG_BUILTIN_ARRAY_FOR_EACH (SIZE_MAX - 98)
#define PS_LANG_BUILTIN_STRING_FOR_EACH (SIZE_MAX - 99)
/* SIZE_MAX - 100 through -173 are member bindings in builtins.h. */
#define PS_LANG_BUILTIN_STRING_FIRST (SIZE_MAX - 500)
#define PS_LANG_BUILTIN_STRING_LAST (SIZE_MAX - 501)
#define PS_LANG_BUILTIN_STRING_SORTED (SIZE_MAX - 502)
#define PS_LANG_BUILTIN_STRING_SORTED_BY (SIZE_MAX - 503)
#define PS_LANG_BUILTIN_STRING_MIN (SIZE_MAX - 504)
#define PS_LANG_BUILTIN_STRING_MAX (SIZE_MAX - 505)
#define PS_LANG_BUILTIN_STRING_MIN_BY (SIZE_MAX - 506)
#define PS_LANG_BUILTIN_STRING_MAX_BY (SIZE_MAX - 507)
#define PS_LANG_BUILTIN_INT64_IS_MULTIPLE (SIZE_MAX - 508)
#define PS_LANG_BUILTIN_INT64_SIGNUM (SIZE_MAX - 509)
#define PS_LANG_BUILTIN_ARRAY_SPLIT (SIZE_MAX - 510)
#define PS_LANG_BUILTIN_ARRAY_STARTS_WITH (SIZE_MAX - 511)
#define PS_LANG_BUILTIN_ARRAY_ELEMENTS_EQUAL (SIZE_MAX - 512)

typedef enum ps_lang_type {
    PS_TYPE_NONE,
    PS_TYPE_VOID,
    PS_TYPE_BOOL,
    PS_TYPE_INT64,
    PS_TYPE_FLOAT64,
    PS_TYPE_STRING,
    PS_TYPE_RANGE,
    PS_TYPE_FUNCTION,
    PS_TYPE_VEC2,
    PS_TYPE_VEC3,
    PS_TYPE_VEC4,
    PS_TYPE_QUAT,
    PS_TYPE_UNIT,
    PS_TYPE_QUANTITY,
    PS_TYPE_CHANNEL,
    PS_TYPE_DATASET,
    PS_TYPE_SERIES,
    PS_TYPE_PLOT,
    PS_TYPE_TABLE,
    PS_TYPE_DISTRIBUTION,
    PS_TYPE_SENSOR_CONFIG,
    PS_TYPE_SENSOR,
    PS_TYPE_BODY,
    PS_TYPE_CONTACTS,
    PS_TYPE_CONTACT_SOLVER,
    PS_TYPE_CONTACT_RESULT,
    PS_TYPE_DISTANCE_JOINT,
    PS_TYPE_JOINT_RESULT,
    PS_TYPE_CONTACT_CONSTRAINT,
    PS_TYPE_JOINT_CONSTRAINT,
    PS_TYPE_CONSTRAINT_RESULT,
    PS_TYPE_SWEEP,
    PS_TYPE_AABB,
    PS_TYPE_COLLISION_PAIR,
    PS_TYPE_MAT3,
    PS_TYPE_MAT4,
    PS_TYPE_BEZIER3,
    PS_TYPE_MEASUREMENT,
    PS_TYPE_RNG,
    PS_TYPE_ODE_RESULT,
    PS_TYPE_MEDIUM,
    PS_TYPE_MATERIAL,
    PS_TYPE_SUBMERSION,
    PS_TYPE_STEP_INTERVAL,
    PS_TYPE_SCALAR_RESULT,
    /* Nominal record types encode PS_TYPE_RECORD_BASE + struct AST index. */
    PS_TYPE_RECORD_BASE = 256,
    /* Structural function signatures encode a canonical annotation or function AST index. */
    PS_TYPE_FUNCTION_BASE = 0x04000000,
    /* Canonical optional annotation/factory AST index, with owning value storage. */
    PS_TYPE_OPTIONAL_BASE = 0x08000000,
    /* Structural arrays encode a canonical annotation/literal/library-argument/call AST index. */
    PS_TYPE_ARRAY_BASE = 0x10000000
} ps_lang_type;

static inline int ps_lang_record_type(ps_lang_type type) {
    return type >= PS_TYPE_RECORD_BASE && type < PS_TYPE_FUNCTION_BASE;
}
static inline int ps_lang_function_type(ps_lang_type type) {
    return type >= PS_TYPE_FUNCTION_BASE && type < PS_TYPE_OPTIONAL_BASE;
}
static inline int ps_lang_optional_type(ps_lang_type type) {
    return type >= PS_TYPE_OPTIONAL_BASE && type < PS_TYPE_ARRAY_BASE;
}

static inline unsigned ps_lang_vector_dimensions(ps_lang_type type) {
    return type == PS_TYPE_VEC2 ? 2 : type == PS_TYPE_VEC3 ? 3 : type == PS_TYPE_VEC4 ? 4 : 0;
}

/* Indexed like the AST. binding resolves names/calls to declarations and call
 * arguments to parameters. previous is internal symbol-stack bookkeeping. */
typedef struct ps_lang_semantic {
    ps_lang_type type;
    size_t binding, previous;
    size_t value_size;
    int64_t raw_value; /* Enum raw number or scalar switch case literal. */
    size_t receiver_slot; /* One-based library parameter occupied by a method receiver. */
    size_t method_owner;  /* Struct declaration for a user-defined method. */
    size_t generic_origin; /* Template AST index for a specialized function or method. */
    unsigned method_static;
    unsigned method_mutating;
    unsigned record_state;
    unsigned raw_enum;
    ps_lang_type array_element;
    size_t array_next;
    ps_lang_type optional_element;
    size_t optional_next;
    ps_lang_type function_result, function_type;
    size_t function_parameter, function_next;
    size_t local_function; /* Function whose body owns this local declaration. */
    size_t lambda_parent; /* Enclosing function for an anonymous function. */
    ps_lang_type lambda_result;
    size_t capture_head, capture_next;
} ps_lang_semantic;

typedef struct ps_lang_check_result {
    int ok;
    ps_lang_token diagnostic;
    size_t count; /* AST node count after generic specialization. */
} ps_lang_check_result;

/* Requires a successful, unmodified parser AST and its original source.
 * No allocation or I/O. node_capacity and info_capacity must cover parsed.count;
 * generic calls may append checked specializations within both capacities. On
 * error, partial semantic information must not be used for code generation. */
ps_lang_check_result ps_lang_check(const void *source, size_t size, ps_lang_node *nodes,
                                   ps_lang_parse_result parsed, ps_lang_semantic *info,
                                   size_t node_capacity, size_t info_capacity);
#endif
