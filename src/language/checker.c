#ifndef _WIN32
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
#endif
#include "checker.h"
#include "builtins.h"
#include "literal.h"
#include <float.h>
#include <limits.h>
#include <locale.h>
#ifdef __APPLE__
#include <xlocale.h>
#endif
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

_Static_assert(sizeof(double) == sizeof(int64_t) && DBL_MANT_DIG == 53,
               "Physim Float64 patterns require binary64");
_Static_assert(PS_LANG_BUILTIN_STRING_FIRST < PS_LANG_MEMBER_SUBMERSION_CENTROID &&
               PS_LANG_BUILTIN_STRING_LAST > PS_LANG_LIBRARY_BASE,
               "String endpoint bindings must stay between member and library bindings");

enum { GENERIC_TYPE_LIMIT = 8, GENERIC_SPECIALIZATION_LIMIT = 256 };
typedef struct specialization {
    size_t original, concrete;
    ps_lang_type types[GENERIC_TYPE_LIMIT];
    unsigned count;
} specialization;
typedef struct checker {
    const unsigned char *source;
    ps_lang_node *nodes;
    ps_lang_semantic *info;
    size_t head, scope_base, function, work, array_head, optional_head, function_head,
           self_parameter;
    size_t count, node_capacity, info_capacity, specialization_count;
    specialization specializations[GENERIC_SPECIALIZATION_LIMIT];
    unsigned depth, loops;
    int specializing;
    int *switch_break;
    ps_lang_token error;
} checker;

static void fail(checker *c, size_t id, const char *message) {
    if (c->error.kind != PS_LANG_ERROR) {
        c->error = c->nodes[id].token;
        c->error.kind = PS_LANG_ERROR;
        c->error.error = message;
    }
}
static int enter(checker *c, size_t id) {
    if (c->error.kind == PS_LANG_ERROR)
        return 0;
    if (c->depth == 128) {
        fail(c, id, "Semantic nesting limit exceeded (128)");
        return 0;
    }
    if (!c->work) {
        fail(c, id, "Semantic work budget exceeded");
        return 0;
    }
    c->work--;
    c->depth++;
    return 1;
}
static int same(checker *c, ps_lang_token a, ps_lang_token b) {
    if (c->error.kind == PS_LANG_ERROR)
        return 0;
    /* Bound symbol-table comparisons, including long identifier prefixes. */
    size_t cost = a.length == b.length ? a.length + 1 : 1;
    if (!cost || cost > c->work) {
        c->error = a;
        c->error.kind = PS_LANG_ERROR;
        c->error.error = "Semantic work budget exceeded";
        return 0;
    }
    c->work -= cost;
    return a.length == b.length &&
           memcmp(c->source + a.offset, c->source + b.offset, a.length) == 0;
}
static int word(checker *c, ps_lang_token t, const char *s) {
    return t.length == strlen(s) && memcmp(c->source + t.offset, s, t.length) == 0;
}
static int overload_work(checker *c, size_t id) {
    if (!c->work) {
        fail(c, id, "Semantic work budget exceeded");
        return 0;
    }
    c->work--;
    return 1;
}
static int builtin_type_name(checker *c, ps_lang_token t) {
    static const char *const names[] = {
        "Int64", "Float64", "Bool", "String", "Void", "Optional", "Vec2", "Vec3",
        "Vec4", "Quat", "Mat3", "Mat4", "Bezier3", "Unit", "Quantity", "Medium", "Material",
        "Submersion", "Channel", "Dataset",
        "Series", "Plot", "Table", "Distribution", "SensorConfig", "Sensor",
        "Measurement", "Rng", "OdeResult", "StepInterval", "ScalarResult", "Diagnostic", "RunIndex", "RunBlock", "RunSnapshot", "Collider", "ContactWorld", "Body", "Contacts", "ContactSolver", "ContactResult",
        "DistanceJoint", "JointResult", "ContactConstraint", "JointConstraint",
        "ConstraintResult", "Sweep", "Aabb", "CollisionPair"
    };
    for (size_t i = 0; i < sizeof names / sizeof names[0]; i++)
        if (word(c, t, names[i]))
            return 1;
    return 0;
}
static int reserved_nominal_type_name(checker *c, ps_lang_token t) {
    static const char *const names[] = {
        "Int64", "Float64", "Bool", "String", "Void", "Vec2", "Vec3", "Vec4",
        "Quat", "Mat3", "Mat4", "Bezier3", "Optional", "Rng", "Unit", "Medium", "Material",
        "Submersion", "Channel",
        "Dataset", "Series", "Plot", "OdeResult", "StepInterval", "ScalarResult", "Diagnostic", "RunIndex", "RunBlock", "RunSnapshot", "Collider", "ContactWorld"
    };
    for (size_t i = 0; i < sizeof(names) / sizeof(names[0]); ++i)
        if (word(c, t, names[i]))
            return 1;
    return 0;
}
static size_t lookup(checker *c, ps_lang_token name) {
    for (size_t id = c->head; id && c->error.kind != PS_LANG_ERROR; id = c->info[id].previous)
        if (name.file == c->nodes[id].token.file && same(c, name, c->nodes[id].token))
            return id;
    return 0;
}
static void declare(checker *c, size_t id) {
    if (word(c, c->nodes[id].token, "self")) {
        fail(c, id, "self is reserved for the method receiver");
        return;
    }
    for (size_t other = c->head; other != c->scope_base; other = c->info[other].previous) {
        if (c->nodes[id].token.file == c->nodes[other].token.file &&
            same(c, c->nodes[id].token, c->nodes[other].token)) {
            if (c->nodes[id].kind == PS_AST_OPTIONAL_BINDING && c->nodes[id].a &&
                c->nodes[c->nodes[id].a].kind == PS_AST_NAME &&
                c->info[c->nodes[id].a].binding == other)
                continue; /* guard let value = value may replace the optional name. */
            if (!c->function &&
                (c->nodes[id].kind == PS_AST_FUNCTION ||
                 c->nodes[id].kind == PS_AST_GENERIC_FUNCTION) &&
                (c->nodes[other].kind == PS_AST_FUNCTION ||
                 c->nodes[other].kind == PS_AST_GENERIC_FUNCTION) &&
                !c->info[other].method_owner)
                continue;
            fail(c, id, "Duplicate declaration in this scope");
            return;
        }
        if (c->error.kind == PS_LANG_ERROR)
            return;
    }
    c->info[id].previous = c->head;
    c->info[id].local_function = c->function;
    c->head = id;
}
static void capture_local(checker *c, size_t lambda, size_t decl, size_t use) {
    for (size_t capture = c->info[lambda].capture_head; capture;
         capture = c->info[capture].capture_next) {
        if (!enter(c, use))
            return;
        c->depth--;
        if (c->info[capture].binding == decl)
            return;
    }
    if (c->count >= c->node_capacity || c->count >= c->info_capacity ||
        c->count >= PS_TYPE_FUNCTION_BASE - PS_TYPE_RECORD_BASE) {
        fail(c, use, "Closure capture exceeds semantic node capacity");
        return;
    }
    size_t capture = c->count++;
    c->nodes[capture] = (ps_lang_node){PS_AST_CAPTURE, c->nodes[use].token, 0, 0, 0, 0};
    memset(&c->info[capture], 0, sizeof c->info[capture]);
    c->info[capture].binding = decl;
    c->info[capture].type = c->info[decl].type;
    c->info[capture].capture_next = c->info[lambda].capture_head;
    c->info[lambda].capture_head = capture;
}
static void capture_reference(checker *c, size_t decl, size_t use) {
    if (decl == c->function && c->nodes[decl].kind == PS_AST_LOCAL_FUNCTION)
        return;
    size_t owner = c->info[decl].local_function;
    if (!owner || !c->function || owner == c->function)
        return;
    for (size_t fn = c->function; fn && fn != owner;
         fn = c->info[fn].lambda_parent) {
        if (fn == decl && c->nodes[fn].kind == PS_AST_LOCAL_FUNCTION)
            return;
        if (c->nodes[fn].kind != PS_AST_LAMBDA &&
            c->nodes[fn].kind != PS_AST_LOCAL_FUNCTION) {
            fail(c, use, "Local value cannot cross this function boundary");
            return;
        }
        capture_local(c, fn, decl, use);
        if (c->error.kind == PS_LANG_ERROR)
            return;
    }
}
static int numeric(ps_lang_type t) { return t == PS_TYPE_INT64 || t == PS_TYPE_FLOAT64; }
static int vector_type(ps_lang_type t) { return ps_lang_vector_dimensions(t) != 0; }
static int value_type(ps_lang_type t) {
    return t == PS_TYPE_BOOL || numeric(t) || t == PS_TYPE_STRING || t >= PS_TYPE_RECORD_BASE ||
           (t >= PS_TYPE_VEC2 && t <= PS_TYPE_CONTACT_WORLD);
}
static int scalar_type(ps_lang_type t) {
    return t == PS_TYPE_BOOL || numeric(t) || t == PS_TYPE_STRING;
}
static int enum_type(checker *c, ps_lang_type t) {
    return ps_lang_record_type(t) && c->nodes[t - PS_TYPE_RECORD_BASE].kind == PS_AST_ENUM;
}
static int enum_payload(checker *c, ps_lang_type t) {
    if (!enum_type(c, t)) return 0;
    for (size_t f = c->nodes[t - PS_TYPE_RECORD_BASE].a; f; f = c->nodes[f].next)
        if (c->nodes[f].a) return 1;
    return 0;
}
static ps_lang_type array_element(checker *c, ps_lang_type t) {
    return t >= PS_TYPE_ARRAY_BASE ? c->info[t - PS_TYPE_ARRAY_BASE].array_element : PS_TYPE_NONE;
}
static ps_lang_type array_type(checker *c, size_t id, ps_lang_type element) {
    if (!value_type(element)) {
        fail(c, id, "Array elements must be values");
        return PS_TYPE_NONE;
    }
    for (size_t type = c->array_head; type; type = c->info[type].array_next) {
        if (!enter(c, id))
            return PS_TYPE_NONE;
        c->depth--;
        if (c->info[type].array_element == element)
            return (ps_lang_type)(PS_TYPE_ARRAY_BASE + type);
    }
    c->info[id].array_element = element;
    c->info[id].array_next = c->array_head;
    c->array_head = id;
    return (ps_lang_type)(PS_TYPE_ARRAY_BASE + id);
}
static ps_lang_type function_type(checker *c, size_t id, size_t parameters,
                                  ps_lang_type result) {
    if (!result)
        return PS_TYPE_NONE;
    for (size_t previous = c->function_head; previous;
         previous = c->info[previous].function_next) {
        if (!enter(c, id))
            return PS_TYPE_NONE;
        c->depth--;
        if (c->info[previous].function_result != result)
            continue;
        size_t left = parameters, right = c->info[previous].function_parameter;
        while (left && right && c->info[left].type == c->info[right].type) {
            left = c->nodes[left].next;
            right = c->nodes[right].next;
        }
        if (!left && !right)
            return (ps_lang_type)(PS_TYPE_FUNCTION_BASE + previous);
    }
    c->info[id].function_result = result;
    c->info[id].function_parameter = parameters;
    c->info[id].function_next = c->function_head;
    c->info[id].function_type = (ps_lang_type)(PS_TYPE_FUNCTION_BASE + id);
    c->function_head = id;
    return c->info[id].function_type;
}
static ps_lang_type optional_element(checker *c, ps_lang_type t) {
    return ps_lang_optional_type(t) ? c->info[t - PS_TYPE_OPTIONAL_BASE].optional_element
                                    : PS_TYPE_NONE;
}
static int equatable_inner(checker *c, ps_lang_type t, size_t *seen, size_t depth) {
    if (scalar_type(t) || vector_type(t)) return 1;
    if (depth >= 128) return 0;
    ps_lang_type element = optional_element(c, t);
    if (!element) element = array_element(c, t);
    if (element) return equatable_inner(c, element, seen, depth + 1);
    if (!ps_lang_record_type(t)) return 0;
    size_t id = (size_t)(t - PS_TYPE_RECORD_BASE);
    for (size_t i = 0; i < depth; i++)
        if (seen[i] == id) return 1; /* Recursion is only possible through a container. */
    seen[depth] = id;
    if (c->nodes[id].kind == PS_AST_STRUCT) {
        for (size_t f = c->nodes[id].a; f; f = c->nodes[f].next)
            if (!equatable_inner(c, c->info[f].type, seen, depth + 1)) return 0;
        return 1;
    }
    if (c->nodes[id].kind == PS_AST_ENUM) {
        for (size_t item = c->nodes[id].a; item; item = c->nodes[item].next)
            for (size_t f = c->nodes[item].a; f; f = c->nodes[f].next)
                if (!equatable_inner(c, c->info[f].type, seen, depth + 1)) return 0;
        return 1;
    }
    return 0;
}
static int equatable(checker *c, ps_lang_type t) {
    size_t seen[128] = {0};
    return equatable_inner(c, t, seen, 0);
}
static size_t imported_member(checker *c, size_t import, ps_lang_token name) {
    if (!import || c->nodes[import].kind != PS_AST_IMPORT || !c->nodes[import].a)
        return 0;
    size_t root = c->nodes[import].a;
    size_t file = c->nodes[root].token.file;
    for (size_t id = c->nodes[root].a; id && c->nodes[id].token.file == file;
         id = c->nodes[id].next)
        if (c->nodes[id].kind != PS_AST_IMPORT && same(c, name, c->nodes[id].token))
            return id;
    return 0;
}
static size_t imported_type(checker *c, size_t member) {
    if (c->nodes[member].kind != PS_AST_MEMBER ||
        c->nodes[c->nodes[member].a].kind != PS_AST_NAME)
        return 0;
    size_t alias = lookup(c, c->nodes[c->nodes[member].a].token);
    size_t decl = imported_member(c, alias, c->nodes[member].token);
    return decl && (c->nodes[decl].kind == PS_AST_STRUCT ||
                    c->nodes[decl].kind == PS_AST_ENUM) ? decl : 0;
}
static ps_lang_type optional_type(checker *c, size_t id, ps_lang_type element) {
    if (!value_type(element)) {
        fail(c, id, "Optional content must be a value");
        return PS_TYPE_NONE;
    }
    for (size_t type = c->optional_head; type; type = c->info[type].optional_next) {
        if (!enter(c, id))
            return PS_TYPE_NONE;
        c->depth--;
        if (c->info[type].optional_element == element)
            return (ps_lang_type)(PS_TYPE_OPTIONAL_BASE + type);
    }
    c->info[id].optional_element = element;
    c->info[id].optional_next = c->optional_head;
    c->optional_head = id;
    return (ps_lang_type)(PS_TYPE_OPTIONAL_BASE + id);
}
static int comparable(ps_lang_type type) {
    return type == PS_TYPE_INT64 || type == PS_TYPE_FLOAT64 ||
           type == PS_TYPE_STRING;
}
static int constraint_name(checker *c, ps_lang_token name) {
    return word(c, name, "Numeric") || word(c, name, "Scalar") ||
           word(c, name, "Equatable") || word(c, name, "Comparable") ||
           word(c, name, "Vector");
}
static void validate_constraints(checker *c, size_t parameter) {
    for (size_t id = c->nodes[parameter].a; id && c->error.kind != PS_LANG_ERROR;
         id = c->nodes[id].next) {
        if (!constraint_name(c, c->nodes[id].token))
            fail(c, id, "Unknown generic constraint");
        for (size_t prior = c->nodes[parameter].a; prior != id;
             prior = c->nodes[prior].next)
            if (same(c, c->nodes[prior].token, c->nodes[id].token))
                fail(c, id, "Duplicate generic constraint");
    }
}
static int generic_record_kind(ps_lang_node_kind kind) {
    return kind == PS_AST_GENERIC_STRUCT || kind == PS_AST_GENERIC_ENUM;
}
static void check_constraints(checker *c, size_t generic, const ps_lang_type *types,
                              size_t at) {
    size_t signature = generic_record_kind(c->nodes[generic].kind)
                           ? c->nodes[generic].c : c->nodes[generic].b;
    size_t index = 0;
    for (size_t p = c->nodes[signature].a; p && c->error.kind != PS_LANG_ERROR;
         p = c->nodes[p].next, index++) {
        ps_lang_type type = types[index];
        for (size_t id = c->nodes[p].a; id && c->error.kind != PS_LANG_ERROR;
             id = c->nodes[id].next) {
            ps_lang_token name = c->nodes[id].token;
            if (word(c, name, "Numeric") && !numeric(type))
                fail(c, at, "Type argument must satisfy Numeric constraint");
            else if (word(c, name, "Scalar") && !scalar_type(type))
                fail(c, at, "Type argument must satisfy Scalar constraint");
            else if (word(c, name, "Vector") && !vector_type(type))
                fail(c, at, "Type argument must satisfy Vector constraint");
            else if (word(c, name, "Comparable") && !comparable(type))
                fail(c, at, "Type argument must satisfy Comparable constraint");
            else if (word(c, name, "Equatable")) {
                if (!equatable(c, type))
                    fail(c, at, "Type argument must satisfy Equatable constraint");
            }
        }
    }
}
static int constraints_match(checker *c, size_t generic,
                             const ps_lang_type *types) {
    size_t signature = c->nodes[generic].b;
    size_t index = 0;
    for (size_t p = c->nodes[signature].a; p;
         p = c->nodes[p].next, index++) {
        ps_lang_type type = types[index];
        for (size_t id = c->nodes[p].a; id; id = c->nodes[id].next) {
            ps_lang_token name = c->nodes[id].token;
            if ((word(c, name, "Numeric") && !numeric(type)) ||
                (word(c, name, "Scalar") && !scalar_type(type)) ||
                (word(c, name, "Vector") && !vector_type(type)) ||
                (word(c, name, "Comparable") && !comparable(type)) ||
                (word(c, name, "Equatable") && !equatable(c, type)))
                return 0;
        }
    }
    return 1;
}
static size_t specialize_record(checker *c, size_t generic, size_t type_arguments,
                                const ps_lang_type *inferred, size_t at);
static size_t generic_record_name(checker *c, size_t id) {
    const ps_lang_node *n = &c->nodes[id];
    size_t decl = 0;
    if (n->kind == PS_AST_TYPE || n->kind == PS_AST_NAME)
        decl = lookup(c, n->token);
    else if (n->kind == PS_AST_MEMBER &&
             (c->nodes[n->a].kind == PS_AST_TYPE ||
              c->nodes[n->a].kind == PS_AST_NAME)) {
        size_t alias = lookup(c, c->nodes[n->a].token);
        decl = imported_member(c, alias, n->token);
        c->info[n->a].binding = alias;
    }
    if (decl && generic_record_kind(c->nodes[decl].kind))
        c->info[id].binding = decl;
    else
        decl = 0;
    return decl;
}
static size_t expected_record_specialization(checker *c, size_t generic,
                                             ps_lang_type expected) {
    if (!generic || !ps_lang_record_type(expected))
        return 0;
    size_t concrete = (size_t)(expected - PS_TYPE_RECORD_BASE);
    for (size_t i = 0; i < c->specialization_count; i++)
        if (c->specializations[i].original == generic &&
            c->specializations[i].concrete == concrete)
            return concrete;
    return 0;
}
static ps_lang_type annotation(checker *c, size_t id, int allow_void) {
    if (!id)
        return PS_TYPE_VOID;
    if (!enter(c, id))
        return PS_TYPE_NONE;
    const ps_lang_node *n = &c->nodes[id];
    ps_lang_type t = PS_TYPE_NONE;
    if (n->kind == PS_AST_RESOLVED_TYPE) {
        t = c->info[id].type;
    } else if (n->kind == PS_AST_GENERIC_TYPE) {
        size_t generic = generic_record_name(c, n->a);
        if (!generic)
            fail(c, n->a, "Type arguments require a generic struct");
        else {
            size_t concrete = specialize_record(c, generic, n->b, NULL, id);
            if (concrete)
                t = c->info[concrete].type;
        }
    } else if (n->kind == PS_AST_FUNCTION_TYPE) {
        for (size_t parameter = n->a; parameter && c->error.kind != PS_LANG_ERROR;
             parameter = c->nodes[parameter].next)
            c->info[parameter].type = annotation(c, parameter, 0);
        ps_lang_type result = annotation(c, n->b, 1);
        if (c->error.kind != PS_LANG_ERROR)
            t = function_type(c, id, n->a, result);
    } else if (n->kind == PS_AST_OPTIONAL_TYPE) {
        ps_lang_type element = annotation(c, n->a, 0);
        if (element)
            t = optional_type(c, id, element);
    } else if (n->kind == PS_AST_ARRAY_TYPE) {
        ps_lang_type element = annotation(c, n->a, 0);
        if (element)
            t = array_type(c, id, element);
    } else if (n->kind == PS_AST_MEMBER && c->nodes[n->a].kind == PS_AST_TYPE) {
        size_t imported = lookup(c, c->nodes[n->a].token);
        size_t decl = imported_member(c, imported, n->token);
        if (decl && (c->nodes[decl].kind == PS_AST_STRUCT ||
                     c->nodes[decl].kind == PS_AST_ENUM)) {
            t = c->info[decl].type;
            c->info[id].binding = decl;
        }
        if (decl && generic_record_kind(c->nodes[decl].kind))
            fail(c, id, c->nodes[decl].kind == PS_AST_GENERIC_ENUM
                            ? "Generic enum requires type arguments"
                            : "Generic struct requires type arguments");
    } else if (n->kind == PS_AST_TYPE) {
        if (word(c, n->token, "Int64"))
            t = PS_TYPE_INT64;
        else if (word(c, n->token, "Float64"))
            t = PS_TYPE_FLOAT64;
        else if (word(c, n->token, "Bool"))
            t = PS_TYPE_BOOL;
        else if (word(c, n->token, "String"))
            t = PS_TYPE_STRING;
        else if (word(c, n->token, "Void") && allow_void)
            t = PS_TYPE_VOID;
        else if (word(c, n->token, "Vec2"))
            t = PS_TYPE_VEC2;
        else if (word(c, n->token, "Vec3"))
            t = PS_TYPE_VEC3;
        else if (word(c, n->token, "Vec4"))
            t = PS_TYPE_VEC4;
        else if (word(c, n->token, "Quat"))
            t = PS_TYPE_QUAT;
        else if (word(c, n->token, "Mat3"))
            t = PS_TYPE_MAT3;
        else if (word(c, n->token, "Mat4"))
            t = PS_TYPE_MAT4;
        else if (word(c, n->token, "Bezier3"))
            t = PS_TYPE_BEZIER3;
        else if (word(c, n->token, "Unit"))
            t = PS_TYPE_UNIT;
        else if (word(c, n->token, "Quantity"))
            t = PS_TYPE_QUANTITY;
        else if (word(c, n->token, "Medium"))
            t = PS_TYPE_MEDIUM;
        else if (word(c,n->token,"Diagnostic"))
            t=PS_TYPE_DIAGNOSTIC;
        else if (word(c, n->token, "Material"))
            t = PS_TYPE_MATERIAL;
        else if (word(c, n->token, "Submersion"))
            t = PS_TYPE_SUBMERSION;
        else if (word(c,n->token,"RunIndex"))
            t=PS_TYPE_RUN_INDEX;
        else if (word(c,n->token,"RunBlock"))
            t=PS_TYPE_RUN_BLOCK;
        else if (word(c,n->token,"RunSnapshot"))
            t=PS_TYPE_RUN_SNAPSHOT;
        else if (word(c,n->token,"Collider"))
            t=PS_TYPE_COLLIDER;
        else if (word(c,n->token,"ContactWorld"))
            t=PS_TYPE_CONTACT_WORLD;
        else if (word(c, n->token, "Channel"))
            t = PS_TYPE_CHANNEL;
        else if (word(c, n->token, "Dataset"))
            t = PS_TYPE_DATASET;
        else if (word(c, n->token, "Series"))
            t = PS_TYPE_SERIES;
        else if (word(c, n->token, "Plot"))
            t = PS_TYPE_PLOT;
        else if (word(c, n->token, "Table"))
            t = PS_TYPE_TABLE;
        else if (word(c, n->token, "Distribution"))
            t = PS_TYPE_DISTRIBUTION;
        else if (word(c, n->token, "SensorConfig"))
            t = PS_TYPE_SENSOR_CONFIG;
        else if (word(c, n->token, "Sensor"))
            t = PS_TYPE_SENSOR;
        else if (word(c, n->token, "Measurement"))
            t = PS_TYPE_MEASUREMENT;
        else if (word(c, n->token, "Rng"))
            t = PS_TYPE_RNG;
        else if (word(c, n->token, "OdeResult"))
            t = PS_TYPE_ODE_RESULT;
        else if (word(c, n->token, "StepInterval"))
            t = PS_TYPE_STEP_INTERVAL;
        else if (word(c, n->token, "ScalarResult"))
            t = PS_TYPE_SCALAR_RESULT;
        else if (word(c, n->token, "Body"))
            t = PS_TYPE_BODY;
        else if (word(c, n->token, "Contacts"))
            t = PS_TYPE_CONTACTS;
        else if (word(c, n->token, "ContactSolver"))
            t = PS_TYPE_CONTACT_SOLVER;
        else if (word(c, n->token, "ContactResult"))
            t = PS_TYPE_CONTACT_RESULT;
        else if (word(c, n->token, "DistanceJoint"))
            t = PS_TYPE_DISTANCE_JOINT;
        else if (word(c, n->token, "JointResult"))
            t = PS_TYPE_JOINT_RESULT;
        else if (word(c, n->token, "ContactConstraint"))
            t = PS_TYPE_CONTACT_CONSTRAINT;
        else if (word(c, n->token, "JointConstraint"))
            t = PS_TYPE_JOINT_CONSTRAINT;
        else if (word(c, n->token, "ConstraintResult"))
            t = PS_TYPE_CONSTRAINT_RESULT;
        else if (word(c, n->token, "Sweep"))
            t = PS_TYPE_SWEEP;
        else if (word(c, n->token, "Aabb"))
            t = PS_TYPE_AABB;
        else if (word(c, n->token, "CollisionPair"))
            t = PS_TYPE_COLLISION_PAIR;
        else {
            size_t decl = lookup(c, n->token);
            if (decl &&
                (c->nodes[decl].kind == PS_AST_STRUCT || c->nodes[decl].kind == PS_AST_ENUM)) {
                t = c->info[decl].type;
                c->info[id].binding = decl;
            }
            if (decl && generic_record_kind(c->nodes[decl].kind))
                fail(c, id, c->nodes[decl].kind == PS_AST_GENERIC_ENUM
                                ? "Generic enum requires type arguments"
                                : "Generic struct requires type arguments");
        }
    }
    if (!t)
        fail(c, id, "Unknown or unsupported type in this compiler stage");
    c->info[id].type = t;
    c->depth--;
    return t;
}
static void integer(checker *c, size_t id, int negative) {
    ps_lang_token t = c->nodes[id].token;
    uint64_t value = 0, limit = (uint64_t)INT64_MAX + (negative ? 1u : 0u);
    unsigned base = ps_lang_integer_base(c->source, t);
    for (size_t i = base == 10u ? 0u : 2u; i < t.length; i++) {
        unsigned char byte = c->source[t.offset + i];
        if (byte == '_')
            continue;
        unsigned digit = ps_lang_integer_digit(byte);
        if (value > (limit - digit) / base) {
            fail(c, id, "Integer literal is outside Int64 range");
            return;
        }
        value = value * base + digit;
    }
}
static ps_lang_type expression(checker *c, size_t id, ps_lang_type expected);
static int sequence(checker *c, size_t first);
static void function_signature(checker *c, size_t id, size_t owner);
static void generic_signature(checker *c, size_t id, size_t owner);
static size_t field_lookup(checker *c, ps_lang_type type, ps_lang_token name) {
    if (type == PS_TYPE_STEP_INTERVAL)
        return word(c,name,"elapsed") ? PS_LANG_MEMBER_STEP_ELAPSED
               : word(c,name,"nextStep") ? PS_LANG_MEMBER_STEP_NEXT : 0;
    if (type == PS_TYPE_SCALAR_RESULT)
        return word(c, name, "x") ? PS_LANG_MEMBER_SCALAR_X
               : word(c, name, "value") ? PS_LANG_MEMBER_SCALAR_VALUE
               : word(c, name, "lower") ? PS_LANG_MEMBER_SCALAR_LOWER
               : word(c, name, "upper") ? PS_LANG_MEMBER_SCALAR_UPPER
               : word(c, name, "iterations") ? PS_LANG_MEMBER_SCALAR_ITERATIONS
               : word(c, name, "evaluations") ? PS_LANG_MEMBER_SCALAR_EVALUATIONS : 0;
    if (type == PS_TYPE_ODE_RESULT)
        return word(c, name, "state") ? PS_LANG_MEMBER_ODE_STATE
               : word(c, name, "acceptedSteps") ? PS_LANG_MEMBER_ODE_ACCEPTED
               : word(c, name, "rejectedSteps") ? PS_LANG_MEMBER_ODE_REJECTED
               : word(c, name, "evaluations") ? PS_LANG_MEMBER_ODE_EVALUATIONS
               : word(c, name, "reachedTime") ? PS_LANG_MEMBER_ODE_REACHED_TIME
               : word(c, name, "nextStep") ? PS_LANG_MEMBER_ODE_NEXT_STEP
               : word(c, name, "errorNorm") ? PS_LANG_MEMBER_ODE_ERROR_NORM : 0;
    if (type == PS_TYPE_SWEEP)
        return word(c, name, "hit") ? PS_LANG_MEMBER_SWEEP_HIT : 0;
    if (type == PS_TYPE_AABB)
        return word(c, name, "minimum") ? PS_LANG_MEMBER_AABB_MIN
               : word(c, name, "maximum") ? PS_LANG_MEMBER_AABB_MAX : 0;
    if (type == PS_TYPE_COLLISION_PAIR)
        return word(c, name, "bodyA") ? PS_LANG_MEMBER_PAIR_A
               : word(c, name, "bodyB") ? PS_LANG_MEMBER_PAIR_B : 0;
    if (type == PS_TYPE_CONTACT_CONSTRAINT || type == PS_TYPE_JOINT_CONSTRAINT) {
        if (word(c, name, "bodyA")) return PS_LANG_MEMBER_CONSTRAINT_A;
        if (word(c, name, "bodyB")) return PS_LANG_MEMBER_CONSTRAINT_B;
        if (type == PS_TYPE_JOINT_CONSTRAINT)
            return word(c, name, "joint") ? PS_LANG_MEMBER_CONSTRAINT_JOINT : 0;
        return word(c, name, "point") ? PS_LANG_MEMBER_CONSTRAINT_POINT
               : word(c, name, "normal") ? PS_LANG_MEMBER_CONSTRAINT_NORMAL
               : word(c, name, "penetration") ? PS_LANG_MEMBER_CONSTRAINT_DEPTH : 0;
    }
    if (type == PS_TYPE_CONSTRAINT_RESULT)
        return word(c, name, "bodyCount") ? PS_LANG_MEMBER_GRAPH_BODY_COUNT
               : word(c, name, "contactCount") ? PS_LANG_MEMBER_GRAPH_CONTACT_COUNT
               : word(c, name, "jointCount") ? PS_LANG_MEMBER_GRAPH_JOINT_COUNT
               : word(c, name, "maxNormalError") ? PS_LANG_MEMBER_GRAPH_NORMAL_ERROR
               : word(c, name, "maxProjectionError") ? PS_LANG_MEMBER_GRAPH_PROJECTION_ERROR
               : word(c, name, "maxJointVelocityError") ? PS_LANG_MEMBER_GRAPH_JOINT_VELOCITY_ERROR
               : word(c, name, "maxJointLengthError") ? PS_LANG_MEMBER_GRAPH_JOINT_LENGTH_ERROR : 0;
    if (type == PS_TYPE_DISTANCE_JOINT)
        return word(c, name, "anchorA") ? PS_LANG_MEMBER_JOINT_ANCHOR_A
               : word(c, name, "anchorB") ? PS_LANG_MEMBER_JOINT_ANCHOR_B
               : word(c, name, "length") ? PS_LANG_MEMBER_JOINT_LENGTH
               : word(c, name, "stabilization") ? PS_LANG_MEMBER_JOINT_STABILIZATION : 0;
    if (type == PS_TYPE_JOINT_RESULT)
        return word(c, name, "bodyA") ? PS_LANG_MEMBER_RESULT_A
               : word(c, name, "bodyB") ? PS_LANG_MEMBER_RESULT_B
               : word(c, name, "impulse") ? PS_LANG_MEMBER_JOINT_IMPULSE
               : word(c, name, "lengthError") ? PS_LANG_MEMBER_JOINT_LENGTH_ERROR
               : word(c, name, "velocityError") ? PS_LANG_MEMBER_JOINT_VELOCITY_ERROR : 0;
    if (type == PS_TYPE_CONTACTS)
        return word(c, name, "count") ? PS_LANG_MEMBER_CONTACT_COUNT : 0;
    if (type == PS_TYPE_CONTACT_RESULT)
        return word(c, name, "bodyA") ? PS_LANG_MEMBER_RESULT_A
               : word(c, name, "bodyB") ? PS_LANG_MEMBER_RESULT_B
               : word(c, name, "count") ? PS_LANG_MEMBER_RESULT_COUNT
               : word(c, name, "maxNormalError") ? PS_LANG_MEMBER_RESULT_ERROR : 0;
    if (type == PS_TYPE_CONTACT_SOLVER)
        return word(c, name, "iterations") ? PS_LANG_MEMBER_SOLVER_ITERATIONS
               : word(c, name, "restitution") ? PS_LANG_MEMBER_SOLVER_RESTITUTION
               : word(c, name, "friction") ? PS_LANG_MEMBER_SOLVER_FRICTION
               : word(c, name, "bounceThreshold") ? PS_LANG_MEMBER_SOLVER_THRESHOLD
               : word(c, name, "penetrationSlop") ? PS_LANG_MEMBER_SOLVER_SLOP
               : word(c, name, "correctionFraction") ? PS_LANG_MEMBER_SOLVER_CORRECTION : 0;
    if (type == PS_TYPE_BODY)
        return word(c, name, "position") ? PS_LANG_MEMBER_BODY_POSITION
               : word(c, name, "velocity") ? PS_LANG_MEMBER_BODY_VELOCITY
               : word(c, name, "orientation") ? PS_LANG_MEMBER_BODY_ORIENTATION
               : word(c, name, "angularVelocity") ? PS_LANG_MEMBER_BODY_ANGULAR_VELOCITY
               : word(c, name, "mass") ? PS_LANG_MEMBER_BODY_MASS
               : word(c, name, "inertia") ? PS_LANG_MEMBER_BODY_INERTIA : 0;
    if (type == PS_TYPE_MEASUREMENT)
        return word(c, name, "value") ? PS_LANG_MEMBER_MEASUREMENT_VALUE
               : word(c, name, "state") ? PS_LANG_MEMBER_STATE
               : word(c, name, "time") ? PS_LANG_MEMBER_TIME
               : word(c, name, "standardUncertainty") ? PS_LANG_MEMBER_UNCERTAINTY
               : word(c, name, "index") ? PS_LANG_MEMBER_INDEX
               : word(c, name, "skipped") ? PS_LANG_MEMBER_SKIPPED : 0;
    if (type == PS_TYPE_QUANTITY)
        return word(c, name, "value") ? PS_LANG_MEMBER_VALUE
               : word(c, name, "unit") ? PS_LANG_MEMBER_UNIT : 0;
    if (type == PS_TYPE_MEDIUM)
        return word(c, name, "density") ? PS_LANG_MEMBER_MEDIUM_DENSITY
               : word(c, name, "viscosity") ? PS_LANG_MEMBER_MEDIUM_VISCOSITY : 0;
    if (type == PS_TYPE_MATERIAL)
        return word(c, name, "density") ? PS_LANG_MEMBER_MATERIAL_DENSITY
               : word(c, name, "restitution") ? PS_LANG_MEMBER_MATERIAL_RESTITUTION
               : word(c, name, "friction") ? PS_LANG_MEMBER_MATERIAL_FRICTION : 0;
    if (type == PS_TYPE_SUBMERSION)
        return word(c, name, "volume") ? PS_LANG_MEMBER_SUBMERSION_VOLUME
               : word(c, name, "centroidOffset") ? PS_LANG_MEMBER_SUBMERSION_CENTROID : 0;
    if (vector_type(type) || type == PS_TYPE_QUAT) {
        if (word(c, name, "x"))
            return PS_LANG_MEMBER_X;
        if (word(c, name, "y"))
            return PS_LANG_MEMBER_Y;
        if (type != PS_TYPE_VEC2 && word(c, name, "z"))
            return PS_LANG_MEMBER_Z;
        if ((type == PS_TYPE_QUAT || type == PS_TYPE_VEC4) && word(c, name, "w"))
            return PS_LANG_MEMBER_W;
        return 0;
    }
    if (!ps_lang_record_type(type))
        return 0;
    size_t record = (size_t)(type - PS_TYPE_RECORD_BASE);
    for (size_t f = c->nodes[record].a; f && c->error.kind != PS_LANG_ERROR; f = c->nodes[f].next)
        if (same(c, name, c->nodes[f].token))
            return f;
    return 0;
}
static ps_lang_type field_type(checker *c, size_t field, size_t member) {
    if (field == PS_LANG_MEMBER_SCALAR_ITERATIONS ||
        field == PS_LANG_MEMBER_SCALAR_EVALUATIONS)
        return PS_TYPE_INT64;
    if (field == PS_LANG_MEMBER_ODE_STATE)
        return array_type(c, member, PS_TYPE_FLOAT64);
    if (field <= PS_LANG_MEMBER_ODE_ACCEPTED && field >= PS_LANG_MEMBER_ODE_EVALUATIONS)
        return PS_TYPE_INT64;
    if (field == PS_LANG_MEMBER_SWEEP_HIT) return PS_TYPE_BOOL;
    if (field == PS_LANG_MEMBER_AABB_MIN || field == PS_LANG_MEMBER_AABB_MAX) return PS_TYPE_VEC3;
    if (field == PS_LANG_MEMBER_PAIR_A || field == PS_LANG_MEMBER_PAIR_B) return PS_TYPE_INT64;
    if (field == PS_LANG_MEMBER_CONSTRAINT_A || field == PS_LANG_MEMBER_CONSTRAINT_B ||
        (field <= PS_LANG_MEMBER_GRAPH_BODY_COUNT && field >= PS_LANG_MEMBER_GRAPH_JOINT_COUNT))
        return PS_TYPE_INT64;
    if (field == PS_LANG_MEMBER_CONSTRAINT_POINT || field == PS_LANG_MEMBER_CONSTRAINT_NORMAL)
        return PS_TYPE_VEC3;
    if (field == PS_LANG_MEMBER_CONSTRAINT_JOINT)
        return PS_TYPE_DISTANCE_JOINT;
    if (field == PS_LANG_MEMBER_JOINT_ANCHOR_A || field == PS_LANG_MEMBER_JOINT_ANCHOR_B ||
        field == PS_LANG_MEMBER_JOINT_IMPULSE)
        return PS_TYPE_VEC3;
    if (field == PS_LANG_MEMBER_RESULT_A || field == PS_LANG_MEMBER_RESULT_B)
        return PS_TYPE_BODY;
    if (field == PS_LANG_MEMBER_CONTACT_COUNT || field == PS_LANG_MEMBER_RESULT_COUNT ||
        field == PS_LANG_MEMBER_SOLVER_ITERATIONS)
        return PS_TYPE_INT64;
    if (field <= PS_LANG_MEMBER_BODY_POSITION && field >= PS_LANG_MEMBER_BODY_INERTIA)
        return field == PS_LANG_MEMBER_BODY_MASS ? PS_TYPE_FLOAT64
               : field == PS_LANG_MEMBER_BODY_ORIENTATION ? PS_TYPE_QUAT : PS_TYPE_VEC3;
    if (field == PS_LANG_MEMBER_MEASUREMENT_VALUE)
        return PS_TYPE_QUANTITY;
    if (field == PS_LANG_MEMBER_STATE || field == PS_LANG_MEMBER_INDEX ||
        field == PS_LANG_MEMBER_SKIPPED)
        return PS_TYPE_INT64;
    if (field == PS_LANG_MEMBER_UNIT)
        return PS_TYPE_UNIT;
    return ps_lang_member_name(field) ? PS_TYPE_FLOAT64 : c->info[field].type;
}
static size_t method_lookup(checker *c, ps_lang_type type, ps_lang_token name) {
    if (!ps_lang_record_type(type))
        return 0;
    size_t owner = (size_t)(type - PS_TYPE_RECORD_BASE);
    if (c->nodes[owner].kind != PS_AST_STRUCT && c->nodes[owner].kind != PS_AST_ENUM)
        return 0;
    for (size_t method = c->nodes[owner].b; method && c->error.kind != PS_LANG_ERROR;
         method = c->nodes[method].next) {
        if (!overload_work(c, method))
            return 0;
        if (same(c, name, c->nodes[method].token))
            return method;
    }
    return 0;
}
static int range_node(checker *c, size_t id) {
    if (c->nodes[id].kind == PS_AST_STRIDED_RANGE)
        return range_node(c, c->nodes[id].a);
    return c->nodes[id].kind == PS_AST_BINARY && (c->nodes[id].token.kind == PS_LANG_RANGE_OPEN ||
                                                  c->nodes[id].token.kind == PS_LANG_RANGE_CLOSED);
}
static size_t initializer(checker *c, size_t record) {
    for (size_t method = c->nodes[record].b; method; method = c->nodes[method].next)
        if (word(c, c->nodes[method].token, "init"))
            return method;
    return 0;
}
static size_t initializer_for_value(checker *c, size_t record, size_t value_id,
                                    ps_lang_type expected) {
    size_t selected = 0;
    for (size_t method = c->nodes[record].b; method; method = c->nodes[method].next) {
        if (!word(c, c->nodes[method].token, "init") ||
            c->nodes[method].kind == PS_AST_GENERIC_FUNCTION ||
            (expected && c->info[method].function_type != expected))
            continue;
        if (selected) {
            fail(c, value_id, "Ambiguous struct initializer value; provide a distinct function type");
            return 0;
        }
        selected = method;
    }
    if (!selected)
        fail(c, value_id, expected
                 ? "No struct initializer matches the required function type"
                 : "Generic initializer values require explicit type arguments");
    return selected;
}
static size_t generic_initializer_for_value(checker *c, size_t record, size_t value_id,
                                            ps_lang_type expected);
static ps_lang_type known_type(checker *c, size_t id, unsigned depth);
static ps_lang_type initializer_argument_type(checker *c, size_t id, unsigned depth) {
    if (!id || depth >= 128 || c->error.kind == PS_LANG_ERROR)
        return PS_TYPE_NONE;
    const ps_lang_node *n = &c->nodes[id];
    if (n->kind == PS_AST_LITERAL) {
        if (n->token.kind == PS_LANG_INTEGER) return PS_TYPE_INT64;
        if (n->token.kind == PS_LANG_FLOAT) return PS_TYPE_FLOAT64;
        if (n->token.kind == PS_LANG_TRUE || n->token.kind == PS_LANG_FALSE)
            return PS_TYPE_BOOL;
        if (n->token.kind == PS_LANG_STRING) return PS_TYPE_STRING;
    }
    if (n->kind == PS_AST_STRING_SEGMENT || n->kind == PS_AST_INTERPOLATED_STRING)
        return PS_TYPE_STRING;
    ps_lang_type known = known_type(c, id, depth);
    if (known) return known;
    if (n->kind == PS_AST_UNARY)
        return n->token.kind == PS_LANG_NOT ? PS_TYPE_BOOL
                                           : initializer_argument_type(c, n->a, depth + 1);
    if (n->kind == PS_AST_BINARY) {
        if (n->token.kind == PS_LANG_COALESCE) {
            ps_lang_type left = optional_element(c, known_type(c, n->a, depth + 1));
            return left ? left : initializer_argument_type(c, n->b, depth + 1);
        }
        if (n->token.kind == PS_LANG_EQ || n->token.kind == PS_LANG_NE ||
            n->token.kind == PS_LANG_LT || n->token.kind == PS_LANG_LE ||
            n->token.kind == PS_LANG_GT || n->token.kind == PS_LANG_GE ||
            n->token.kind == PS_LANG_AND || n->token.kind == PS_LANG_OR)
            return PS_TYPE_BOOL;
        ps_lang_type left = initializer_argument_type(c, n->a, depth + 1);
        ps_lang_type right = initializer_argument_type(c, n->b, depth + 1);
        if (n->token.kind == PS_LANG_PLUS && left == PS_TYPE_STRING &&
            right == PS_TYPE_STRING)
            return PS_TYPE_STRING;
        if (numeric(left) && numeric(right))
            return left == PS_TYPE_FLOAT64 || right == PS_TYPE_FLOAT64
                       ? PS_TYPE_FLOAT64 : PS_TYPE_INT64;
    }
    if (n->kind == PS_AST_CONDITIONAL) {
        ps_lang_type yes = initializer_argument_type(c, n->b, depth + 1);
        ps_lang_type no = initializer_argument_type(c, n->c, depth + 1);
        return yes == no ? yes : PS_TYPE_NONE;
    }
    return PS_TYPE_NONE;
}
static int initializer_integer_literal(checker *c, size_t id, unsigned depth) {
    if (!id || depth >= 128)
        return 0;
    const ps_lang_node *n = &c->nodes[id];
    if (n->kind == PS_AST_LITERAL)
        return n->token.kind == PS_LANG_INTEGER;
    return n->kind == PS_AST_UNARY &&
           (n->token.kind == PS_LANG_MINUS || n->token.kind == PS_LANG_PLUS) &&
           initializer_integer_literal(c, n->a, depth + 1);
}
static size_t initializer_optional_some_value(checker *c, size_t id) {
    const ps_lang_node *call = &c->nodes[id];
    if (call->kind != PS_AST_CALL || call->c || !call->b ||
        c->nodes[call->b].next)
        return 0;
    const ps_lang_node *member = &c->nodes[call->a];
    if (member->kind != PS_AST_MEMBER || !word(c, member->token, "some") ||
        c->nodes[member->a].kind != PS_AST_NAME ||
        !word(c, c->nodes[member->a].token, "Optional") ||
        lookup(c, c->nodes[member->a].token))
        return 0;
    size_t argument = call->b;
    return (!c->nodes[argument].token.length ||
            word(c, c->nodes[argument].token, "value"))
               ? c->nodes[argument].a : 0;
}
static int initializer_shape_same(checker *c, size_t left, size_t right) {
    size_t left_count = 0, right_count = 0;
    for (size_t p = c->nodes[left].a; p; p = c->nodes[p].next)
        left_count++;
    for (size_t p = c->nodes[right].a; p; p = c->nodes[p].next)
        right_count++;
    if (left_count != right_count)
        return 0;
    for (size_t p = c->nodes[left].a; p; p = c->nodes[p].next) {
        size_t match = c->nodes[right].a;
        while (match && !same(c, c->nodes[p].token, c->nodes[match].token))
            match = c->nodes[match].next;
        if (!match)
            return 0;
    }
    return 1;
}
static int type_parameter(checker *c, size_t generic, ps_lang_token name);
static int initializer_annotation_same(checker *c, size_t left_method, size_t right_method,
                                       size_t left, size_t right,
                                       unsigned depth) {
    if (!left || !right || depth >= 128 || c->error.kind == PS_LANG_ERROR)
        return left == right;
    const ps_lang_node *a = &c->nodes[left], *b = &c->nodes[right];
    if (a->kind == PS_AST_RESOLVED_TYPE && b->kind == PS_AST_TYPE) {
        if (c->nodes[right_method].kind == PS_AST_GENERIC_FUNCTION &&
            type_parameter(c, right_method, b->token) >= 0)
            return 0;
        return c->info[left].type == annotation(c, right, 0);
    }
    if (a->kind == PS_AST_TYPE && b->kind == PS_AST_RESOLVED_TYPE) {
        if (c->nodes[left_method].kind == PS_AST_GENERIC_FUNCTION &&
            type_parameter(c, left_method, a->token) >= 0)
            return 0;
        return annotation(c, left, 0) == c->info[right].type;
    }
    if (a->kind != b->kind)
        return 0;
    if (a->kind == PS_AST_TYPE) {
        int left_index = c->nodes[left_method].kind == PS_AST_GENERIC_FUNCTION
                             ? type_parameter(c, left_method, a->token) : -1;
        int right_index = c->nodes[right_method].kind == PS_AST_GENERIC_FUNCTION
                              ? type_parameter(c, right_method, b->token) : -1;
        return left_index >= 0 || right_index >= 0
                   ? left_index >= 0 && left_index == right_index
                   : same(c, a->token, b->token);
    }
    if (a->kind == PS_AST_RESOLVED_TYPE)
        return c->info[left].type == c->info[right].type;
    if (a->kind == PS_AST_ARRAY_TYPE || a->kind == PS_AST_OPTIONAL_TYPE)
        return initializer_annotation_same(c, left_method, right_method,
                                           a->a, b->a, depth + 1);
    if (a->kind == PS_AST_FUNCTION_TYPE) {
        size_t left_parameter = a->a, right_parameter = b->a;
        while (left_parameter && right_parameter) {
            if (!initializer_annotation_same(c, left_method, right_method,
                                             left_parameter, right_parameter, depth + 1))
                return 0;
            left_parameter = c->nodes[left_parameter].next;
            right_parameter = c->nodes[right_parameter].next;
        }
        return !left_parameter && !right_parameter &&
               initializer_annotation_same(c, left_method, right_method,
                                           a->b, b->b, depth + 1);
    }
    if (a->kind == PS_AST_MEMBER)
        return same(c, a->token, b->token) &&
               initializer_annotation_same(c, left_method, right_method,
                                           a->a, b->a, depth + 1);
    if (a->kind == PS_AST_GENERIC_TYPE) {
        if (!initializer_annotation_same(c, left_method, right_method,
                                         a->a, b->a, depth + 1))
            return 0;
        size_t left_arg = a->b, right_arg = b->b;
        while (left_arg && right_arg) {
            if (!initializer_annotation_same(c, left_method, right_method,
                                             left_arg, right_arg, depth + 1))
                return 0;
            left_arg = c->nodes[left_arg].next;
            right_arg = c->nodes[right_arg].next;
        }
        return !left_arg && !right_arg;
    }
    return 0;
}
static int initializer_template_signature_same(checker *c, size_t left, size_t right) {
    if (!initializer_shape_same(c, left, right))
        return 0;
    for (size_t p = c->nodes[left].a; p; p = c->nodes[p].next) {
        size_t match = c->nodes[right].a;
        while (match && !same(c, c->nodes[p].token, c->nodes[match].token))
            match = c->nodes[match].next;
        if (!match || !initializer_annotation_same(c, left, right,
                                                    c->nodes[p].a, c->nodes[match].a, 0))
            return 0;
    }
    return 1;
}
static int initializer_signature_same(checker *c, size_t left, size_t right) {
    if (!initializer_shape_same(c, left, right))
        return 0;
    for (size_t p = c->nodes[left].a; p; p = c->nodes[p].next) {
        size_t match = c->nodes[right].a;
        while (match && !same(c, c->nodes[p].token, c->nodes[match].token))
            match = c->nodes[match].next;
        if (!match || c->info[p].type != c->info[match].type)
            return 0;
    }
    return 1;
}
static int initializer_shape_matches(checker *c, size_t method, size_t call_id) {
    size_t first = c->nodes[call_id].b;
    int named = first && c->nodes[first].token.length != 0;
    size_t arguments = 0, parameters = 0;
    for (size_t a = first; a; a = c->nodes[a].next)
        arguments++;
    for (size_t p = c->nodes[method].a; p; p = c->nodes[p].next)
        parameters++;
    if (arguments != parameters)
        return 0;
    for (size_t a = first; a; a = c->nodes[a].next) {
        if ((c->nodes[a].token.length != 0) != named)
            return 0;
        if (named) {
            size_t p = c->nodes[method].a;
            while (p && !same(c, c->nodes[a].token, c->nodes[p].token))
                p = c->nodes[p].next;
            if (!p)
                return 0;
            for (size_t prior = first; prior != a; prior = c->nodes[prior].next)
                if (same(c, c->nodes[prior].token, c->nodes[a].token))
                    return 0;
        }
    }
    return 1;
}
static int initializer_value_matches(checker *c, size_t value_id, ps_lang_type wanted,
                                     int *score, unsigned depth) {
    if (c->error.kind == PS_LANG_ERROR)
        return 0;
    if (depth >= 128) {
        fail(c, value_id, "Semantic nesting limit exceeded (128)");
        return 0;
    }
    if (!c->work) {
        fail(c, value_id, "Semantic work budget exceeded");
        return 0;
    }
    c->work--;
    const ps_lang_node *value = &c->nodes[value_id];
    if (value->kind == PS_AST_LITERAL && value->token.kind == PS_LANG_NIL) {
        if (!optional_element(c, wanted))
            return 0;
        (*score)++;
        return 1;
    }
    size_t some_value = initializer_optional_some_value(c, value_id);
    if (some_value) {
        ps_lang_type element = optional_element(c, wanted);
        if (!element)
            return 0;
        (*score)++;
        return initializer_value_matches(c, some_value, element, score, depth + 1);
    }
    if (value->kind == PS_AST_ARRAY) {
        ps_lang_type element = array_element(c, wanted);
        if (!element)
            return 0;
        (*score)++;
        for (size_t item = value->a; item && c->error.kind != PS_LANG_ERROR;
             item = c->nodes[item].next)
            if (!initializer_value_matches(c, item, element, score, depth + 1))
                return 0;
        return c->error.kind != PS_LANG_ERROR;
    }
    ps_lang_type actual = initializer_argument_type(c, value_id, depth);
    if (!actual || !wanted)
        return c->error.kind != PS_LANG_ERROR;
    if (actual == wanted) {
        *score += 2;
        return 1;
    }
    if (actual == PS_TYPE_INT64 && wanted == PS_TYPE_FLOAT64 &&
        initializer_integer_literal(c, value_id, 0)) {
        (*score)++;
        return 1;
    }
    return 0;
}
static int initializer_template_argument(checker *c, size_t generic, size_t inner,
                                         size_t annotation_id, ps_lang_type actual,
                                         ps_lang_type *types, size_t value_id,
                                         int *score, unsigned depth);
static size_t initializer_for_call(checker *c, size_t record, size_t call_id,
                                   int method_type_arguments) {
    size_t selected = 0, candidates = 0, only_candidate = 0;
    int best_score = -1, tied = 0;
    for (size_t method = c->nodes[record].b; method; method = c->nodes[method].next) {
        if (!word(c, c->nodes[method].token, "init"))
            continue;
        if (method_type_arguments && c->nodes[method].kind != PS_AST_GENERIC_FUNCTION)
            continue;
        candidates++;
        only_candidate = method;
        if (initializer_shape_matches(c, method, call_id)) {
            int score = 0, compatible = 1;
            int generic_method = c->nodes[method].kind == PS_AST_GENERIC_FUNCTION;
            ps_lang_type types[GENERIC_TYPE_LIMIT] = {0};
            size_t positional = c->nodes[method].a;
            int named = c->nodes[call_id].b && c->nodes[c->nodes[call_id].b].token.length;
            for (size_t a = c->nodes[call_id].b; a && compatible; a = c->nodes[a].next) {
                size_t p = positional;
                if (named) {
                    for (p = c->nodes[method].a; p; p = c->nodes[p].next)
                        if (same(c, c->nodes[a].token, c->nodes[p].token))
                            break;
                }
                if (generic_method) {
                    int argument_score = 0;
                    ps_lang_type actual = initializer_argument_type(c, c->nodes[a].a, 0);
                    compatible = initializer_template_argument(c, method, 0,
                                                                c->nodes[p].a, actual,
                                                                types, c->nodes[a].a,
                                                                &argument_score, 0);
                    score += argument_score / 2;
                } else
                    compatible = initializer_value_matches(c, c->nodes[a].a,
                                                           c->info[p].type, &score, 0);
                if (!named)
                    positional = c->nodes[p].next;
            }
            if (!compatible)
                continue;
            if (score > best_score) {
                best_score = score;
                tied = 0;
                selected = method;
            } else if (score == best_score) {
                tied = 1;
            }
        }
    }
    if (tied) {
        fail(c, call_id, "Ambiguous struct initializer overload; use named or typed arguments");
        return 0;
    }
    if (!candidates && method_type_arguments)
        fail(c, call_id, "Explicit type arguments require a generic function or method");
    if (!selected && candidates > 1)
        fail(c, call_id, "No matching struct initializer overload");
    return selected ? selected : candidates == 1 ? only_candidate : 0;
}
static int is_initializer(checker *c, size_t record, size_t method) {
    if (method && c->info[method].generic_origin &&
        c->info[method].method_owner == record)
        method = c->info[method].generic_origin;
    for (size_t m = c->nodes[record].b; m; m = c->nodes[m].next)
        if (m == method && word(c, c->nodes[m].token, "init"))
            return 1;
    return 0;
}
static ps_lang_type signature_type(checker *c, size_t id, ps_lang_type type) {
    ps_lang_type element = ps_lang_signature_element(type);
    return element ? array_type(c, id, element) : type;
}
static size_t free_overload_for_call(checker *c, size_t first, size_t call_id,
                                     size_t imported, ps_lang_type expected,
                                     int report);
static ps_lang_type known_type(checker *c, size_t id, unsigned depth) {
    if (!id || depth >= 128 || c->error.kind == PS_LANG_ERROR)
        return PS_TYPE_NONE;
    if (!c->work) {
        fail(c, id, "Semantic work budget exceeded");
        return PS_TYPE_NONE;
    }
    c->work--;
    const ps_lang_node *n = &c->nodes[id];
    if (n->kind == PS_AST_INDEX) {
        if (range_node(c, n->b))
            return known_type(c, n->a, depth + 1);
        ps_lang_type element = array_element(c, known_type(c, n->a, depth + 1));
        if (!element && c->nodes[n->a].kind == PS_AST_ARRAY && c->nodes[n->a].a)
            element = known_type(c, c->nodes[n->a].a, depth + 1);
        return element;
    }
    if (n->kind == PS_AST_LITERAL && n->token.kind == PS_LANG_FLOAT)
        return PS_TYPE_FLOAT64;
    if (n->kind == PS_AST_STRING_SEGMENT || n->kind == PS_AST_INTERPOLATED_STRING)
        return PS_TYPE_STRING;
    if (n->kind == PS_AST_UNARY && n->token.kind != PS_LANG_NOT)
        return known_type(c, n->a, depth + 1);
    if (n->kind == PS_AST_CONDITIONAL) {
        ps_lang_type yes = known_type(c, n->b, depth + 1);
        ps_lang_type no = known_type(c, n->c, depth + 1);
        return yes == no ? yes : PS_TYPE_NONE;
    }
    if (n->kind == PS_AST_BINARY) {
        ps_lang_type left = known_type(c, n->a, depth + 1);
        ps_lang_type right = known_type(c, n->b, depth + 1);
        if (n->token.kind == PS_LANG_COALESCE) {
            ps_lang_type element = optional_element(c, left);
            return element ? element : right;
        }
        if ((n->token.kind == PS_LANG_PLUS || n->token.kind == PS_LANG_MINUS ||
             n->token.kind == PS_LANG_STAR || n->token.kind == PS_LANG_SLASH) &&
            (left == PS_TYPE_QUANTITY || right == PS_TYPE_QUANTITY))
            return PS_TYPE_QUANTITY;
        if (n->token.kind == PS_LANG_STAR)
            return vector_type(left) ? left : vector_type(right) ? right : PS_TYPE_NONE;
        if (n->token.kind == PS_LANG_PLUS || n->token.kind == PS_LANG_MINUS ||
            n->token.kind == PS_LANG_SLASH)
            return vector_type(left) ? left : PS_TYPE_NONE;
        return PS_TYPE_NONE;
    }
    if (n->kind == PS_AST_NAME) {
        if (word(c, n->token, "self"))
            return c->self_parameter ? c->info[c->self_parameter].type : PS_TYPE_NONE;
        size_t decl = lookup(c, n->token);
        if (decl && (c->nodes[decl].kind == PS_AST_FUNCTION ||
                     c->nodes[decl].kind == PS_AST_GENERIC_FUNCTION))
            for (size_t prior = c->info[decl].previous; prior;
                 prior = c->info[prior].previous)
                if ((c->nodes[prior].kind == PS_AST_FUNCTION ||
                     c->nodes[prior].kind == PS_AST_GENERIC_FUNCTION) &&
                    c->nodes[prior].token.file == c->nodes[decl].token.file &&
                    same(c, c->nodes[prior].token, c->nodes[decl].token))
                    return PS_TYPE_NONE;
        return decl ? c->nodes[decl].kind == PS_AST_FUNCTION &&
                              !c->info[decl].method_owner
                          ? c->info[decl].function_type : c->info[decl].type
                    : PS_TYPE_NONE;
    }
    if (n->kind == PS_AST_CALL && c->nodes[n->a].kind == PS_AST_MEMBER) {
        const ps_lang_node *member = &c->nodes[n->a];
        const ps_lang_node *root = &c->nodes[member->a];
        if (root->kind == PS_AST_NAME && !lookup(c, root->token)) {
            size_t binding = 0;
            const ps_lang_builtin *factory = ps_lang_static_method_find(
                c->source + root->token.offset, root->token.length,
                c->source + member->token.offset, member->token.length, &binding);
            if (factory)
                return signature_type(c, id, factory->result);
        }
        if (root->kind == PS_AST_NAME) {
            size_t alias = lookup(c, root->token);
            if (alias && c->nodes[alias].kind == PS_AST_IMPORT) {
                size_t fn = imported_member(c, alias, member->token);
                if (fn && (c->nodes[fn].kind == PS_AST_FUNCTION ||
                           c->nodes[fn].kind == PS_AST_GENERIC_FUNCTION)) {
                    fn = free_overload_for_call(c, fn, id, alias,
                                                PS_TYPE_NONE, 0);
                    return fn ? c->info[fn].type : PS_TYPE_NONE;
                }
            }
        }
        ps_lang_type owner = known_type(c, member->a, depth + 1);
        if (optional_element(c, owner) &&
            (word(c, member->token, "unwrap") || word(c, member->token, "valueOr")))
            return optional_element(c, owner);
        /* No optional interning during lookahead: a later expected Float64?
         * may still specialize an integer literal passed to some. */
        if ((word(c, member->token, "unwrap") || word(c, member->token, "valueOr")) &&
            c->nodes[member->a].kind == PS_AST_CALL) {
            const ps_lang_node *call = &c->nodes[member->a];
            const ps_lang_node *some = &c->nodes[call->a];
            if (some->kind == PS_AST_MEMBER && word(c, some->token, "some") &&
                c->nodes[some->a].kind == PS_AST_NAME &&
                word(c, c->nodes[some->a].token, "Optional") &&
                !lookup(c, c->nodes[some->a].token) && call->b)
                return known_type(c, c->nodes[call->b].a, depth + 1);
        }
        size_t user_method = method_lookup(c, owner, member->token);
        if (user_method) {
            for (size_t method = c->nodes[user_method].next; method;
                 method = c->nodes[method].next)
                if (same(c, c->nodes[method].token, member->token))
                    return PS_TYPE_NONE;
            return c->info[user_method].type;
        }
        if (array_element(c, owner)) {
            if (word(c, member->token, "contains") ||
                word(c, member->token, "allSatisfy") ||
                word(c, member->token, "starts") ||
                word(c, member->token, "elementsEqual"))
                return PS_TYPE_BOOL;
            if (word(c, member->token, "remove"))
                return array_element(c, owner);
        }
        size_t binding = 0;
        const ps_lang_method *method = ps_lang_method_find(owner, c->source + member->token.offset,
                                                           member->token.length, &binding);
        return method ? signature_type(c, id, ps_lang_builtin_get(binding)->result) : PS_TYPE_NONE;
    }
    if (n->kind == PS_AST_CALL && c->nodes[n->a].kind == PS_AST_NAME) {
        size_t decl = lookup(c, c->nodes[n->a].token);
        if (decl && (c->nodes[decl].kind == PS_AST_FUNCTION ||
                     c->nodes[decl].kind == PS_AST_GENERIC_FUNCTION))
            for (size_t prior = c->info[decl].previous; prior;
                 prior = c->info[prior].previous)
                if ((c->nodes[prior].kind == PS_AST_FUNCTION ||
                     c->nodes[prior].kind == PS_AST_GENERIC_FUNCTION) &&
                    c->nodes[prior].token.file == c->nodes[decl].token.file &&
                    same(c, c->nodes[prior].token, c->nodes[decl].token)) {
                    size_t selected = free_overload_for_call(c, decl, id, 0,
                                                             PS_TYPE_NONE, 0);
                    return selected ? c->info[selected].type : PS_TYPE_NONE;
                }
        if (!decl) {
            size_t binding = 0;
            ps_lang_token name = c->nodes[n->a].token;
            if (word(c, name, "Int64"))
                return PS_TYPE_INT64;
            if (word(c, name, "arrayCount"))
                return PS_TYPE_INT64;
            if ((word(c, name, "arrayAppending") || word(c, name, "arrayRemoving")) && n->b)
                return known_type(c, c->nodes[n->b].a, depth + 1);
            if (word(c, name, "Float64"))
                return PS_TYPE_FLOAT64;
            const ps_lang_builtin *builtin =
                ps_lang_builtin_find(c->source + name.offset, name.length, &binding);
            if (builtin)
                return signature_type(c, id, builtin->result);
        }
        return decl && (c->nodes[decl].kind == PS_AST_FUNCTION ||
                        c->nodes[decl].kind == PS_AST_STRUCT)
                   ? c->info[decl].type
                   : PS_TYPE_NONE;
    }
    if (n->kind == PS_AST_MEMBER) {
        if (word(c, n->token, "count") && array_element(c, known_type(c, n->a, depth + 1)))
            return PS_TYPE_INT64;
        size_t f = field_lookup(c, known_type(c, n->a, depth + 1), n->token);
        return f ? field_type(c, f, id) : PS_TYPE_NONE;
    }
    return PS_TYPE_NONE;
}
/* Propagate an already-known numeric context to literals on either side of an
 * operator. This does not convert Int64 variables to Float64. The actual walk
 * below still checks every operand and enforces its own nesting limit. */
static ps_lang_type numeric_hint(checker *c, size_t id, unsigned depth) {
    if (!id || depth >= 128 || c->error.kind == PS_LANG_ERROR)
        return PS_TYPE_NONE;
    if (!c->work) {
        fail(c, id, "Semantic work budget exceeded");
        return PS_TYPE_NONE;
    }
    c->work--;
    const ps_lang_node *n = &c->nodes[id];
    if (n->kind == PS_AST_INDEX && c->nodes[n->a].kind == PS_AST_ARRAY && !range_node(c, n->b)) {
        for (size_t item = c->nodes[n->a].a; item && c->error.kind != PS_LANG_ERROR;
             item = c->nodes[item].next)
            if (numeric_hint(c, item, depth + 1) == PS_TYPE_FLOAT64)
                return PS_TYPE_FLOAT64;
    }
    if (n->kind == PS_AST_MEMBER || n->kind == PS_AST_INDEX)
        return known_type(c, id, depth) == PS_TYPE_FLOAT64 ? PS_TYPE_FLOAT64 : PS_TYPE_NONE;
    if (n->kind == PS_AST_LITERAL)
        return n->token.kind == PS_LANG_FLOAT ? PS_TYPE_FLOAT64 : PS_TYPE_NONE;
    if (n->kind == PS_AST_NAME) {
        return known_type(c, id, depth) == PS_TYPE_FLOAT64 ? PS_TYPE_FLOAT64 : PS_TYPE_NONE;
    }
    if (n->kind == PS_AST_CALL) {
        return known_type(c, id, depth) == PS_TYPE_FLOAT64 ? PS_TYPE_FLOAT64 : PS_TYPE_NONE;
    }
    if (n->kind == PS_AST_UNARY && n->token.kind != PS_LANG_NOT)
        return numeric_hint(c, n->a, depth + 1);
    if (n->kind == PS_AST_CONDITIONAL &&
        (numeric_hint(c, n->b, depth + 1) == PS_TYPE_FLOAT64 ||
         numeric_hint(c, n->c, depth + 1) == PS_TYPE_FLOAT64))
        return PS_TYPE_FLOAT64;
    if (n->kind == PS_AST_BINARY &&
        (n->token.kind == PS_LANG_PLUS || n->token.kind == PS_LANG_MINUS ||
          n->token.kind == PS_LANG_STAR || n->token.kind == PS_LANG_SLASH ||
          n->token.kind == PS_LANG_PERCENT)) {
        if (numeric_hint(c, n->a, depth + 1) == PS_TYPE_FLOAT64 ||
            numeric_hint(c, n->b, depth + 1) == PS_TYPE_FLOAT64)
            return PS_TYPE_FLOAT64;
    }
    if (n->kind == PS_AST_BINARY && n->token.kind == PS_LANG_COALESCE &&
        (optional_element(c, known_type(c, n->a, depth + 1)) == PS_TYPE_FLOAT64 ||
         numeric_hint(c, n->b, depth + 1) == PS_TYPE_FLOAT64))
        return PS_TYPE_FLOAT64;
    return PS_TYPE_NONE;
}
/* Resolve only immutable, syntax-known dimensions. Unknown values keep the
 * shared runtime checks. The local budget bounds repeated let-alias expansion. */
static size_t library_argument(checker *c, size_t call, unsigned slot) {
    size_t callee = c->nodes[call].a;
    if (c->info[callee].receiver_slot == slot &&
        c->nodes[callee].kind == PS_AST_MEMBER)
        return c->nodes[callee].a;
    for (size_t arg = c->nodes[call].b; arg; arg = c->nodes[arg].next)
        if (c->info[arg].binding == slot)
            return c->nodes[arg].a;
    return 0;
}
static int known_integer(checker *c, size_t id, int64_t *out, unsigned depth,
                         size_t *budget) {
    if (!id || depth >= 64 || !*budget)
        return 0;
    --*budget;
    if (!c->work) {
        fail(c, id, "Semantic work budget exceeded");
        return 0;
    }
    --c->work;
    const ps_lang_node *node = &c->nodes[id];
    if (node->kind == PS_AST_NAME || node->kind == PS_AST_MEMBER) {
        size_t decl = c->info[id].binding;
        if (decl && decl < c->count && c->nodes[decl].kind == PS_AST_VARIABLE &&
            c->nodes[decl].token.kind == PS_LANG_LET && c->nodes[decl].b)
            return known_integer(c, c->nodes[decl].b, out, depth + 1, budget);
        return 0;
    }
    if (node->kind == PS_AST_UNARY &&
        (node->token.kind == PS_LANG_MINUS || node->token.kind == PS_LANG_PLUS)) {
        const ps_lang_node *literal = &c->nodes[node->a];
        if (literal->kind == PS_AST_LITERAL && literal->token.kind == PS_LANG_INTEGER &&
            literal->token.length <= 64) {
            uint64_t magnitude = ps_lang_integer_value(c->source, literal->token);
            if (node->token.kind == PS_LANG_MINUS && magnitude <= INT64_MAX) {
                *out = -(int64_t)magnitude;
                return 1;
            }
            if (node->token.kind == PS_LANG_MINUS && magnitude == (uint64_t)INT64_MAX + 1u) {
                *out = INT64_MIN;
                return 1;
            }
        }
        if (!known_integer(c, node->a, out, depth + 1, budget))
            return 0;
        if (node->token.kind == PS_LANG_MINUS) {
            if (*out == INT64_MIN)
                return 0;
            *out = -*out;
        }
        return 1;
    }
    if (node->kind == PS_AST_BINARY) {
        ps_lang_kind op = node->token.kind;
        if (op != PS_LANG_PLUS && op != PS_LANG_MINUS && op != PS_LANG_STAR &&
            op != PS_LANG_SLASH && op != PS_LANG_PERCENT)
            return 0;
        int64_t left, right;
        if (!known_integer(c, node->a, &left, depth + 1, budget) ||
            !known_integer(c, node->b, &right, depth + 1, budget))
            return 0;
        if (op == PS_LANG_PLUS) {
            if ((right > 0 && left > INT64_MAX - right) ||
                (right < 0 && left < INT64_MIN - right))
                return 0;
            *out = left + right;
        } else if (op == PS_LANG_MINUS) {
            if ((right < 0 && left > INT64_MAX + right) ||
                (right > 0 && left < INT64_MIN + right))
                return 0;
            *out = left - right;
        } else if (op == PS_LANG_STAR) {
            if ((left > 0 && right > 0 && left > INT64_MAX / right) ||
                (left > 0 && right < 0 && right < INT64_MIN / left) ||
                (left < 0 && right > 0 && left < INT64_MIN / right) ||
                (left < 0 && right < 0 && left < INT64_MAX / right))
                return 0;
            *out = left * right;
        } else {
            if (!right || (left == INT64_MIN && right == -1))
                return 0;
            *out = op == PS_LANG_SLASH ? left / right : left % right;
        }
        return 1;
    }
    if (node->kind != PS_AST_LITERAL || node->token.kind != PS_LANG_INTEGER ||
        node->token.length > 64)
        return 0;
    uint64_t value = ps_lang_integer_value(c->source, node->token);
    if (value > INT64_MAX)
        return 0;
    *out = (int64_t)value;
    return 1;
}
static int known_dimension(checker *c, size_t id, int8_t out[7], unsigned depth,
                           size_t *budget) {
    if (!id || depth >= 64 || !*budget)
        return 0;
    --*budget;
    if (!c->work) {
        fail(c, id, "Semantic work budget exceeded");
        return 0;
    }
    --c->work;
    const ps_lang_node *node = &c->nodes[id];
    if (node->kind == PS_AST_UNARY &&
        (node->token.kind == PS_LANG_PLUS || node->token.kind == PS_LANG_MINUS))
        return known_dimension(c, node->a, out, depth + 1, budget);
    if (node->kind == PS_AST_BINARY &&
        (node->token.kind == PS_LANG_PLUS || node->token.kind == PS_LANG_MINUS))
        return known_dimension(c, node->a, out, depth + 1, budget);
    if (node->kind == PS_AST_BINARY && node->token.kind == PS_LANG_STAR)
        return known_dimension(c,
                               c->info[node->a].type == PS_TYPE_QUANTITY ? node->a : node->b,
                               out, depth + 1, budget);
    if (node->kind == PS_AST_BINARY && node->token.kind == PS_LANG_SLASH &&
        c->info[node->a].type == PS_TYPE_QUANTITY)
        return known_dimension(c, node->a, out, depth + 1, budget);
    if (node->kind == PS_AST_NAME || node->kind == PS_AST_MEMBER) {
        if (node->kind == PS_AST_MEMBER && c->info[id].binding == PS_LANG_MEMBER_UNIT)
            return known_dimension(c, node->a, out, depth + 1, budget);
        size_t decl = c->info[id].binding;
        if (decl && decl < c->count && c->nodes[decl].kind == PS_AST_VARIABLE &&
            c->nodes[decl].token.kind == PS_LANG_LET && c->nodes[decl].b)
            return known_dimension(c, c->nodes[decl].b, out, depth + 1, budget);
        return 0;
    }
    if (node->kind != PS_AST_CALL)
        return 0;
    const ps_lang_builtin *builtin = ps_lang_builtin_get(c->info[id].binding);
    if (!builtin)
        return 0;
    const char *name = builtin->name;
    if (!strcmp(name, "Unit")) {
        for (unsigned axis = 0; axis < 7; axis++) {
            int64_t exponent;
            if (!known_integer(c, library_argument(c, id, axis + 1), &exponent,
                               depth + 1, budget) || exponent < INT8_MIN || exponent > INT8_MAX)
                return 0;
            out[axis] = (int8_t)exponent;
        }
        return 1;
    }
    if (!strcmp(name, "Quantity"))
        return known_dimension(c, library_argument(c, id, 2), out, depth + 1, budget);
    if (!strcmp(name, "convertQuantity"))
        return known_dimension(c, library_argument(c, id, 2), out, depth + 1, budget);
    if (!strcmp(name, "addQuantity") || !strcmp(name, "subtractQuantity"))
        return known_dimension(c, library_argument(c, id, 1), out, depth + 1, budget);
    int multiply = !strcmp(name, "multiplyQuantity") || !strcmp(name, "multiplyUnit");
    int divide = !strcmp(name, "divideQuantity") || !strcmp(name, "divideUnit");
    if (multiply || divide) {
        int8_t left[7], right[7];
        if (!known_dimension(c, library_argument(c, id, 1), left, depth + 1, budget) ||
            !known_dimension(c, library_argument(c, id, 2), right, depth + 1, budget))
            return 0;
        for (unsigned axis = 0; axis < 7; axis++) {
            int dimension = left[axis] + (multiply ? right[axis] : -right[axis]);
            if (dimension < INT8_MIN || dimension > INT8_MAX)
                return 0;
            out[axis] = (int8_t)dimension;
        }
        return 1;
    }
    if (!strcmp(name, "powerUnit")) {
        int8_t base[7];
        int64_t exponent;
        if (!known_dimension(c, library_argument(c, id, 1), base, depth + 1, budget) ||
            !known_integer(c, library_argument(c, id, 2), &exponent, depth + 1, budget))
            return 0;
        for (unsigned axis = 0; axis < 7; axis++) {
            if (exponent && (exponent > INT8_MAX || exponent < INT8_MIN))
                return 0;
            int dimension = base[axis] * (int)exponent;
            if (dimension < INT8_MIN || dimension > INT8_MAX)
                return 0;
            out[axis] = (int8_t)dimension;
        }
        return 1;
    }
    return 0;
}
static void check_known_unit_call(checker *c, size_t id, const char *name) {
    unsigned left_slot = 0, right_slot = 0;
    if (!strcmp(name, "convert")) {
        left_slot = 2;
        right_slot = 3;
    } else if (!strcmp(name, "convertQuantity") || !strcmp(name, "addQuantity") ||
               !strcmp(name, "subtractQuantity")) {
        left_slot = 1;
        right_slot = 2;
    }
    if (!left_slot)
        return;
    int8_t left[7], right[7];
    size_t budget = 4096;
    if (known_dimension(c, library_argument(c, id, left_slot), left, 0, &budget) &&
        known_dimension(c, library_argument(c, id, right_slot), right, 0, &budget) &&
        memcmp(left, right, sizeof left) != 0)
        fail(c, id, "Statically incompatible unit dimensions");
}
static ps_lang_type library_call(checker *c, size_t id, const ps_lang_builtin *builtin,
                                 size_t binding, size_t receiver_slot) {
    const ps_lang_node *n = &c->nodes[id];
    if (n->c) {
        fail(c, n->c, "Explicit type arguments require a generic function or method");
        return PS_TYPE_NONE;
    }
    unsigned seen = receiver_slot ? 1u << (receiver_slot - 1) : 0;
    unsigned count = receiver_slot ? 1 : 0;
    int named = n->b && c->nodes[n->b].token.length;
    for (size_t a = n->b; a && c->error.kind != PS_LANG_ERROR; a = c->nodes[a].next) {
        const ps_lang_node *arg = &c->nodes[a];
        unsigned slot = count - (receiver_slot ? 1 : 0);
        if (receiver_slot && slot >= receiver_slot - 1)
            slot++;
        if ((arg->token.length != 0) != named) {
            fail(c, a, "Use either positional or named arguments, not both");
            break;
        }
        if (named) {
            for (slot = 0; slot < builtin->count; slot++)
                if (word(c, arg->token, builtin->labels[slot]))
                    break;
        }
        if (slot >= builtin->count) {
            fail(c, a, "Unknown parameter name or too many arguments");
            break;
        }
        if (seen & (1u << slot)) {
            fail(c, a, "Parameter supplied more than once");
            break;
        }
        seen |= 1u << slot;
        c->info[a].binding = slot + 1;
        ps_lang_type expected = signature_type(c, a, builtin->types[slot]);
        if (builtin->types[slot] == PS_TYPE_FUNCTION ||
            builtin->types[slot] == PS_LANG_ODE_CALLBACK) {
            size_t value = arg->a;
            ps_lang_type actual = expression(c, value, PS_TYPE_NONE);
            size_t signature = ps_lang_function_type(actual)
                                   ? (size_t)(actual - PS_TYPE_FUNCTION_BASE) : 0;
            size_t parameter = signature ? c->info[signature].function_parameter : 0;
            size_t second = parameter ? c->nodes[parameter].next : 0;
            int ode = builtin->types[slot] == PS_LANG_ODE_CALLBACK;
            if (!signature || !parameter ||
                c->info[parameter].type != PS_TYPE_FLOAT64 ||
                (ode ? !second || c->nodes[second].next ||
                           array_element(c, c->info[second].type) != PS_TYPE_FLOAT64 ||
                           array_element(c, c->info[signature].function_result) != PS_TYPE_FLOAT64
                     : second || c->info[signature].function_result != PS_TYPE_FLOAT64)) {
                fail(c, value, ode
                    ? "ODE callback requires func(Float64, [Float64]) -> [Float64]"
                    : "Scalar callback requires func(Float64) -> Float64");
                break;
            }
            c->info[a].type = actual;
        } else
            c->info[a].type = expression(c, arg->a, expected);
        count++;
    }
    if (count != builtin->count)
        fail(c, id, "Missing library arguments");
    c->info[id].binding = binding;
    c->info[n->a].binding = binding;
    c->info[n->a].type = PS_TYPE_FUNCTION;
    c->info[n->a].receiver_slot = receiver_slot;
    if (c->error.kind != PS_LANG_ERROR)
        check_known_unit_call(c, id, builtin->name);
    return signature_type(c, id, builtin->result);
}
static int mutable_target(checker *c, size_t id);
static size_t specialize(checker *c, size_t generic, size_t call_id,
                         ps_lang_type expected, int record_type_arguments);
static size_t infer_record_constructor(checker *c, size_t generic, size_t call_id,
                                       ps_lang_type expected, size_t *selected_initializer);
static ps_lang_type indirect_call(checker *c, size_t id, ps_lang_type function_value) {
    const ps_lang_node *n = &c->nodes[id];
    if (n->c) {
        fail(c, n->c, "Function values do not accept type arguments");
        return PS_TYPE_NONE;
    }
    size_t signature = (size_t)(function_value - PS_TYPE_FUNCTION_BASE);
    size_t parameter = c->info[signature].function_parameter;
    for (size_t a = n->b; a && c->error.kind != PS_LANG_ERROR; a = c->nodes[a].next) {
        if (c->nodes[a].token.length) {
            fail(c, a, "Function values use positional arguments");
            break;
        }
        if (!parameter) {
            fail(c, a, "Too many function-value arguments");
            break;
        }
        c->info[a].type = expression(c, c->nodes[a].a, c->info[parameter].type);
        parameter = c->nodes[parameter].next;
    }
    if (parameter && c->error.kind != PS_LANG_ERROR)
        fail(c, id, "Missing function-value arguments");
    c->info[id].binding = PS_LANG_BUILTIN_INDIRECT_CALL;
    return c->info[signature].function_result;
}
static size_t inferred_enum_case_owner(checker *c, size_t generic, size_t call_id,
                                       ps_lang_token case_name) {
    size_t item = 0;
    for (size_t candidate = c->nodes[generic].a; candidate;
         candidate = c->nodes[candidate].next)
        if (same(c, c->nodes[candidate].token, case_name)) {
            item = candidate;
            break;
        }
    if (!item || !c->nodes[item].a) {
        fail(c, call_id, "Generic enum case needs explicit type arguments or an expected type");
        return 0;
    }
    ps_lang_type types[GENERIC_TYPE_LIMIT] = {0};
    int score = 0;
    size_t field = c->nodes[item].a;
    int named = c->nodes[call_id].b && c->nodes[c->nodes[call_id].b].token.length;
    for (size_t a = c->nodes[call_id].b; a && c->error.kind != PS_LANG_ERROR;
         a = c->nodes[a].next) {
        size_t selected = field;
        if (named)
            for (; selected && !same(c, c->nodes[a].token, c->nodes[selected].token);
                 selected = c->nodes[selected].next) {}
        if (!selected) {
            fail(c, a, "Unknown enum payload field or too many arguments");
            return 0;
        }
        size_t value = c->nodes[a].a;
        ps_lang_type actual = initializer_argument_type(c, value, 0);
        if (!initializer_template_argument(c, generic, 0, c->nodes[selected].a,
                                           actual, types, value, &score, 0)) {
            if (c->error.kind != PS_LANG_ERROR)
                fail(c, value, "Enum payload does not match generic type arguments");
            return 0;
        }
        if (!named)
            field = c->nodes[selected].next;
    }
    size_t signature = c->nodes[generic].c;
    size_t index = 0;
    for (size_t p = c->nodes[signature].a; p; p = c->nodes[p].next, index++)
        if (index >= GENERIC_TYPE_LIMIT || !types[index]) {
            fail(c, call_id, "Cannot infer every generic enum type parameter");
            return 0;
        }
    return specialize_record(c, generic, 0, types, call_id);
}
static void generic_seed_float_types(checker *c, size_t generic, size_t method,
                                     size_t call_id, ps_lang_type *types);
static int method_shape_matches(checker *c, size_t first_parameter,
                                size_t call_id) {
    size_t arguments = 0, parameters = 0;
    int named = c->nodes[call_id].b &&
                c->nodes[c->nodes[call_id].b].token.length != 0;
    for (size_t a = c->nodes[call_id].b; a; a = c->nodes[a].next)
        arguments++;
    for (size_t p = first_parameter; p; p = c->nodes[p].next)
        parameters++;
    if (arguments != parameters)
        return 0;
    for (size_t a = c->nodes[call_id].b; a; a = c->nodes[a].next) {
        if ((c->nodes[a].token.length != 0) != named)
            return 0;
        if (named) {
            size_t p = first_parameter;
            while (p && !same(c, c->nodes[a].token, c->nodes[p].token))
                p = c->nodes[p].next;
            if (!p)
                return 0;
            for (size_t prior = c->nodes[call_id].b; prior != a;
                 prior = c->nodes[prior].next)
                if (same(c, c->nodes[prior].token, c->nodes[a].token))
                    return 0;
        }
    }
    return 1;
}
static int infer_overload_result(checker *c, size_t generic,
                                 size_t annotation_id, ps_lang_type expected,
                                 ps_lang_type *types, unsigned depth) {
    if (!annotation_id || !expected || depth >= 128)
        return 0;
    const ps_lang_node *n = &c->nodes[annotation_id];
    if (n->kind == PS_AST_TYPE) {
        int index = type_parameter(c, generic, n->token);
        if (index < 0)
            return 1;
        if (types[index] && types[index] != expected)
            return 0;
        types[index] = expected;
        return 1;
    }
    if (n->kind == PS_AST_ARRAY_TYPE)
        return infer_overload_result(c, generic, n->a,
                                     array_element(c, expected), types,
                                     depth + 1);
    if (n->kind == PS_AST_OPTIONAL_TYPE)
        return infer_overload_result(c, generic, n->a,
                                     optional_element(c, expected), types,
                                     depth + 1);
    return 1;
}
static int free_overload_candidate(checker *c, size_t fn, size_t call_id,
                                   ps_lang_type expected, int *score) {
    int generic = c->nodes[fn].kind == PS_AST_GENERIC_FUNCTION;
    if (!generic && c->nodes[call_id].c)
        return 0;
    size_t first_parameter = c->nodes[fn].a;
    if (c->info[fn].method_owner && !c->info[fn].method_static &&
        first_parameter)
        first_parameter = c->nodes[first_parameter].next;
    if (!method_shape_matches(c, first_parameter, call_id))
        return 0;
    ps_lang_type types[GENERIC_TYPE_LIMIT] = {0};
    size_t type_count = 0;
    if (generic) {
        size_t signature = c->nodes[fn].b;
        for (size_t p = c->nodes[signature].a; p; p = c->nodes[p].next)
            type_count++;
        if (!type_count || type_count > GENERIC_TYPE_LIMIT)
            return 0;
        if (c->nodes[call_id].c) {
            size_t index = 0;
            for (size_t t = c->nodes[call_id].c; t; t = c->nodes[t].next) {
                if (index == type_count)
                    return 0;
                types[index++] = annotation(c, t, 0);
                if (!value_type(types[index - 1]))
                    return 0;
            }
            if (index != type_count)
                return 0;
        } else
            generic_seed_float_types(c, fn, fn, call_id, types);
    }
    int named = c->nodes[call_id].b &&
                c->nodes[c->nodes[call_id].b].token.length != 0;
    for (int pass = 0; pass < 2; pass++) {
        size_t positional = first_parameter;
        for (size_t a = c->nodes[call_id].b; a; a = c->nodes[a].next) {
            size_t p = positional;
            if (named)
                for (p = first_parameter; p; p = c->nodes[p].next)
                    if (same(c, c->nodes[a].token, c->nodes[p].token))
                        break;
            int ignored = 0;
            ps_lang_type actual = initializer_argument_type(c, c->nodes[a].a, 0);
            if (!initializer_template_argument(c, generic ? fn : 0, 0,
                                               c->nodes[p].a, actual, types,
                                               c->nodes[a].a,
                                               pass ? &ignored : score, 0))
                return 0;
            if (!named)
                positional = c->nodes[p].next;
        }
    }
    if (generic) {
        int missing = 0;
        for (size_t i = 0; i < type_count; i++)
            missing |= types[i] == PS_TYPE_NONE;
        if (missing && expected &&
            !infer_overload_result(c, fn, c->nodes[c->nodes[fn].b].b,
                                   expected, types, 0))
            return 0;
        for (size_t i = 0; i < type_count; i++)
            if (!types[i])
                return 0;
        if (!constraints_match(c, fn, types))
            return 0;
    }
    return c->error.kind != PS_LANG_ERROR;
}
static size_t free_overload_for_call(checker *c, size_t first, size_t call_id,
                                     size_t imported, ps_lang_type expected,
                                     int report) {
    ps_lang_token name = c->nodes[first].token;
    size_t start = imported ? c->nodes[c->nodes[imported].a].a : first;
    size_t candidates = 0, selected = 0;
    int best_score = -1, tied = 0;
    for (size_t fn = start; fn && c->error.kind != PS_LANG_ERROR;
         fn = imported ? c->nodes[fn].next : c->info[fn].previous) {
        if (imported && c->nodes[fn].token.file != name.file)
            break;
        if (!overload_work(c, call_id))
            return 0;
        if ((c->nodes[fn].kind != PS_AST_FUNCTION &&
             c->nodes[fn].kind != PS_AST_GENERIC_FUNCTION) ||
            c->info[fn].method_owner ||
            c->nodes[fn].token.file != name.file ||
            !same(c, c->nodes[fn].token, name))
            continue;
        candidates++;
        int score = 0;
        if (!free_overload_candidate(c, fn, call_id, expected, &score))
            continue;
        if (score > best_score) {
            best_score = score;
            selected = fn;
            tied = 0;
        } else if (score == best_score) {
            if (c->nodes[selected].kind == PS_AST_GENERIC_FUNCTION &&
                c->nodes[fn].kind == PS_AST_FUNCTION) {
                selected = fn;
                tied = 0;
            } else if (c->nodes[selected].kind == c->nodes[fn].kind)
                tied = 1;
        }
    }
    if (candidates <= 1)
        return first;
    if (tied && report)
        fail(c, call_id, "Ambiguous free function overload; use typed arguments");
    else if (!selected && report && c->error.kind != PS_LANG_ERROR)
        fail(c, call_id, "No matching free function overload");
    return c->error.kind == PS_LANG_ERROR || tied ? 0 : selected;
}
static size_t free_overload_for_value(checker *c, size_t first,
                                      ps_lang_type expected, size_t imported,
                                      size_t at) {
    ps_lang_token name = c->nodes[first].token;
    size_t start = imported ? c->nodes[c->nodes[imported].a].a : first;
    size_t candidates = 0, selected = 0;
    for (size_t fn = start; fn; fn = imported ? c->nodes[fn].next
                                             : c->info[fn].previous) {
        if (imported && c->nodes[fn].token.file != name.file)
            break;
        if (!overload_work(c, at))
            return 0;
        if ((c->nodes[fn].kind != PS_AST_FUNCTION &&
             c->nodes[fn].kind != PS_AST_GENERIC_FUNCTION) ||
            c->info[fn].method_owner ||
            c->nodes[fn].token.file != name.file ||
            !same(c, c->nodes[fn].token, name))
            continue;
        candidates++;
        if (c->nodes[fn].kind == PS_AST_FUNCTION &&
            c->info[fn].function_type == expected)
            selected = fn;
    }
    if (candidates <= 1)
        return first;
    if (!ps_lang_function_type(expected))
        fail(c, at, "Overloaded function value requires an expected function type");
    else if (!selected)
        fail(c, at, "No free function overload matches the required function type");
    return c->error.kind == PS_LANG_ERROR ? 0 : selected;
}
static size_t generic_overload_for_reference(checker *c, size_t first,
                                             size_t reference,
                                             ps_lang_type expected,
                                             size_t imported,
                                             size_t method_owner,
                                             int static_context) {
    ps_lang_token name = c->nodes[first].token;
    size_t start = method_owner ? c->nodes[method_owner].b
                   : imported ? c->nodes[c->nodes[imported].a].a : first;
    size_t selected = 0, candidates = 0, total_generic = 0, only_generic = 0;
    size_t expected_signature = ps_lang_function_type(expected)
                                    ? (size_t)(expected - PS_TYPE_FUNCTION_BASE) : 0;
    for (size_t fn = start; fn && c->error.kind != PS_LANG_ERROR;
         fn = method_owner || imported ? c->nodes[fn].next
                                        : c->info[fn].previous) {
        if (imported && c->nodes[fn].token.file != name.file)
            break;
        if (!overload_work(c, reference))
            return 0;
        if (c->nodes[fn].kind != PS_AST_GENERIC_FUNCTION ||
            c->info[fn].method_owner != method_owner ||
            (method_owner &&
             (c->info[fn].method_static != 0) != static_context) ||
            c->nodes[fn].token.file != name.file ||
            !same(c, c->nodes[fn].token, name))
            continue;
        total_generic++;
        only_generic = fn;
        ps_lang_type types[GENERIC_TYPE_LIMIT] = {0};
        size_t signature = c->nodes[fn].b, type_count = 0;
        for (size_t p = c->nodes[signature].a; p; p = c->nodes[p].next)
            type_count++;
        if (!type_count || type_count > GENERIC_TYPE_LIMIT)
            continue;
        size_t index = 0, t = c->nodes[reference].b;
        for (; t && index < type_count; t = c->nodes[t].next)
            types[index++] = annotation(c, t, 0);
        if (index != type_count || t ||
            !constraints_match(c, fn, types))
            continue;
        candidates++;
        if (expected_signature) {
            size_t parameter = c->nodes[fn].a;
            if (method_owner && !static_context && parameter)
                parameter = c->nodes[parameter].next;
            size_t wanted = c->info[expected_signature].function_parameter;
            int matches = 1, score = 0;
            while (parameter && wanted && matches) {
                matches = initializer_template_argument(
                    c, fn, 0, c->nodes[parameter].a, c->info[wanted].type,
                    types, reference, &score, 0);
                parameter = c->nodes[parameter].next;
                wanted = c->nodes[wanted].next;
            }
            size_t result = c->nodes[signature].b;
            if (parameter || wanted || !matches ||
                (result ? !initializer_template_argument(
                              c, fn, 0, result,
                              c->info[expected_signature].function_result,
                              types, reference, &score, 0)
                        : c->info[expected_signature].function_result != PS_TYPE_VOID))
                continue;
        }
        if (selected) {
            fail(c, reference, method_owner
                     ? "Ambiguous generic method reference"
                     : "Ambiguous generic free function reference");
            return 0;
        }
        selected = fn;
    }
    if (!selected && total_generic == 1 && c->error.kind != PS_LANG_ERROR)
        return only_generic; /* Keep precise specialization diagnostics. */
    if (!selected && c->error.kind != PS_LANG_ERROR)
        fail(c, reference, method_owner
                 ? candidates
                       ? "No generic method matches the required function type"
                       : "No generic method matches the type arguments"
                 : candidates
                       ? "No generic free function matches the required function type"
                       : "No generic free function matches the type arguments");
    return selected;
}
static size_t method_overload_for_call(checker *c, size_t first,
                                       size_t call_id, ps_lang_type expected,
                                       int static_context) {
    size_t owner = c->info[first].method_owner;
    ps_lang_token name = c->nodes[first].token;
    size_t candidates = 0, selected = 0;
    int best_score = -1, tied = 0;
    for (size_t fn = c->nodes[owner].b; fn && c->error.kind != PS_LANG_ERROR;
         fn = c->nodes[fn].next) {
        if (!overload_work(c, call_id))
            return 0;
        if (!same(c, c->nodes[fn].token, name) ||
            (c->info[fn].method_static != 0) != static_context)
            continue;
        candidates++;
        int score = 0;
        if (!free_overload_candidate(c, fn, call_id, expected, &score))
            continue;
        if (score > best_score) {
            best_score = score;
            selected = fn;
            tied = 0;
        } else if (score == best_score) {
            if (c->nodes[selected].kind == PS_AST_GENERIC_FUNCTION &&
                c->nodes[fn].kind == PS_AST_FUNCTION) {
                selected = fn;
                tied = 0;
            } else if (c->nodes[selected].kind == c->nodes[fn].kind)
                tied = 1;
        }
    }
    if (!candidates) {
        fail(c, call_id, static_context
                 ? c->nodes[owner].kind == PS_AST_ENUM
                       ? "Instance method must be called on an enum value"
                       : "Expected a static method on this type"
                 : "Static method must be called on its type");
        return 0;
    }
    if (candidates == 1)
        return selected ? selected : first;
    if (tied)
        fail(c, call_id, "Ambiguous method overload; use typed arguments");
    else if (!selected && c->error.kind != PS_LANG_ERROR)
        fail(c, call_id, "No matching method overload");
    return c->error.kind == PS_LANG_ERROR ? 0 : selected;
}
static size_t method_overload_for_value(checker *c, size_t first,
                                        ps_lang_type expected,
                                        int static_context, size_t at) {
    size_t owner = c->info[first].method_owner;
    ps_lang_token name = c->nodes[first].token;
    size_t candidates = 0, selected = 0, only_candidate = 0;
    for (size_t fn = c->nodes[owner].b; fn && c->error.kind != PS_LANG_ERROR;
         fn = c->nodes[fn].next) {
        if (!overload_work(c, at))
            return 0;
        if (!same(c, c->nodes[fn].token, name) ||
            (c->info[fn].method_static != 0) != static_context)
            continue;
        candidates++;
        only_candidate = fn;
        if (c->nodes[fn].kind == PS_AST_FUNCTION &&
            c->info[fn].function_type == expected)
            selected = fn;
    }
    if (!candidates) {
        fail(c, at, static_context
                 ? "Instance method value requires a bound receiver"
                 : "Static method value requires its type");
        return 0;
    }
    if (candidates == 1)
        return only_candidate;
    if (!ps_lang_function_type(expected))
        fail(c, at, "Overloaded method value requires an expected function type");
    else if (!selected)
        fail(c, at, "No method overload matches the required function type");
    return c->error.kind == PS_LANG_ERROR ? 0 : selected;
}
static ps_lang_type call(checker *c, size_t id, ps_lang_type expected) {
    const ps_lang_node *n = &c->nodes[id];
    const ps_lang_node *callee = &c->nodes[n->a];
    size_t fn = 0;
    if (callee->kind == PS_AST_MEMBER) {
        size_t owner = c->nodes[callee->a].kind == PS_AST_NAME
                           ? lookup(c, c->nodes[callee->a].token)
                           : imported_type(c, callee->a);
        if (!owner && c->nodes[callee->a].kind == PS_AST_MEMBER) {
            size_t generic = generic_record_name(c, callee->a);
            if (generic && c->nodes[generic].kind == PS_AST_GENERIC_ENUM)
                owner = generic;
        }
        if (c->nodes[callee->a].kind == PS_AST_TYPE_APPLY) {
            size_t applied = callee->a;
            size_t generic = generic_record_name(c, c->nodes[applied].a);
            if (generic && c->nodes[generic].kind == PS_AST_GENERIC_ENUM) {
                owner = specialize_record(c, generic, c->nodes[applied].b, NULL, applied);
                if (owner) {
                    c->info[applied].type = c->info[owner].type;
                    c->info[applied].binding = owner;
                }
            }
        }
        if (owner && c->nodes[owner].kind == PS_AST_GENERIC_ENUM) {
            size_t generic = owner;
            owner = expected_record_specialization(c, generic, expected);
            if (!owner)
                owner = inferred_enum_case_owner(c, generic, id, callee->token);
            if (owner) {
                c->info[callee->a].type = c->info[owner].type;
                c->info[callee->a].binding = owner;
            }
        }
        if (owner && c->nodes[owner].kind == PS_AST_ENUM) {
            fn = method_lookup(c, c->info[owner].type, callee->token);
            if (fn) {
                c->info[callee->a].type = c->info[owner].type;
                c->info[callee->a].binding = owner;
                goto user_call;
            }
            if (word(c, callee->token, "fromRawValue")) {
                size_t arg = n->b;
                if (!c->info[owner].raw_enum) {
                    fail(c, id, "Enum does not define raw values");
                    return PS_TYPE_NONE;
                }
                if (n->c || !arg || c->nodes[arg].next ||
                    (c->nodes[arg].token.length &&
                     !word(c, c->nodes[arg].token, "rawValue"))) {
                    fail(c, id, "fromRawValue expects one Int64 argument");
                    return PS_TYPE_NONE;
                }
                c->info[arg].type = expression(c, c->nodes[arg].a, PS_TYPE_INT64);
                c->info[callee->a].type = c->info[owner].type;
                c->info[callee->a].binding = owner;
                c->info[n->a].type = PS_TYPE_FUNCTION;
                c->info[n->a].binding = c->info[id].binding =
                    PS_LANG_BUILTIN_ENUM_FROM_RAW;
                return optional_type(c, id, c->info[owner].type);
            }
            size_t item = field_lookup(c, c->info[owner].type, callee->token);
            if (!item || !c->nodes[item].a) {
                fail(c, id, "Enum case has no payload constructor");
                return PS_TYPE_NONE;
            }
            if (n->c) {
                fail(c, n->c, "Enum cases do not accept type arguments");
                return PS_TYPE_NONE;
            }
            size_t field = c->nodes[item].a;
            size_t count = 0, required = 0;
            int named = n->b && c->nodes[n->b].token.length != 0;
            for (size_t f = field; f; f = c->nodes[f].next) required++;
            for (size_t a = n->b; a && c->error.kind != PS_LANG_ERROR; a = c->nodes[a].next) {
                const ps_lang_node *arg = &c->nodes[a];
                if ((arg->token.length != 0) != named) {
                    fail(c, a, "Use either positional or named arguments, not both");
                    break;
                }
                size_t selected = field;
                if (named)
                    for (; selected && !same(c, arg->token, c->nodes[selected].token);
                         selected = c->nodes[selected].next) {}
                if (!selected) {
                    fail(c, a, "Unknown enum payload field or too many arguments");
                    break;
                }
                for (size_t prior = n->b; prior != a; prior = c->nodes[prior].next)
                    if (c->info[prior].binding == selected)
                        fail(c, a, "Enum payload field supplied more than once");
                c->info[a].binding = selected;
                c->info[a].type = expression(c, arg->a, c->info[selected].type);
                count++;
                if (!named) field = c->nodes[selected].next;
            }
            if (count != required) fail(c, id, "Missing enum payload fields");
            c->info[callee->a].type = c->info[owner].type;
            c->info[callee->a].binding = owner;
            c->info[n->a].type = PS_TYPE_FUNCTION;
            c->info[n->a].binding = item;
            c->info[id].binding = item;
            return c->info[owner].type;
        }
    }
    if (callee->kind == PS_AST_MEMBER &&
        c->nodes[callee->a].kind == PS_AST_TYPE_APPLY) {
        size_t applied = callee->a;
        size_t generic = generic_record_name(c, c->nodes[applied].a);
        if (!generic) {
            fail(c, applied, "Type arguments require a generic struct");
            return PS_TYPE_NONE;
        }
        size_t owner = specialize_record(c, generic, c->nodes[applied].b, NULL, applied);
        if (!owner)
            return PS_TYPE_NONE;
        fn = method_lookup(c, c->info[owner].type, callee->token);
        if (!fn) {
            fail(c, n->a, "Expected a static method on this type");
            return PS_TYPE_NONE;
        }
        c->info[applied].type = c->info[owner].type;
        c->info[applied].binding = owner;
        goto user_call;
    }
    if (callee->kind == PS_AST_MEMBER && c->nodes[callee->a].kind == PS_AST_NAME) {
        size_t alias = lookup(c, c->nodes[callee->a].token);
        if (alias && c->nodes[alias].kind == PS_AST_IMPORT) {
            fn = imported_member(c, alias, callee->token);
            if (!fn) {
                fail(c, n->a, "Unknown exported function or type in imported module");
                return PS_TYPE_NONE;
            }
            c->info[callee->a].binding = alias;
            if (c->nodes[fn].kind == PS_AST_VARIABLE &&
                ps_lang_function_type(c->info[fn].type))
                return indirect_call(c, id, expression(c, n->a, PS_TYPE_NONE));
            goto user_call;
        }
    }
    if (callee->kind == PS_AST_MEMBER) {
        size_t owner = imported_type(c, callee->a);
        if (owner && (c->nodes[owner].kind == PS_AST_STRUCT ||
                      c->nodes[owner].kind == PS_AST_ENUM)) {
            fn = method_lookup(c, c->info[owner].type, callee->token);
            if (!fn) {
                fail(c, n->a, "Expected a static method on imported type");
                return PS_TYPE_NONE;
            }
            c->info[callee->a].binding = owner;
            c->info[callee->a].type = c->info[owner].type;
            goto user_call;
        }
    }
    if (callee->kind == PS_AST_MEMBER && c->nodes[callee->a].kind == PS_AST_NAME &&
        word(c, c->nodes[callee->a].token, "Optional") && !lookup(c, c->nodes[callee->a].token)) {
        if (n->c) {
            fail(c, n->c, "Explicit type arguments require a generic function or method");
            return PS_TYPE_NONE;
        }
        size_t arg = n->b;
        if (!word(c, callee->token, "some") || !arg || c->nodes[arg].next ||
            (c->nodes[arg].token.length && !word(c, c->nodes[arg].token, "value"))) {
            fail(c, id, "Optional.some expects one value argument");
            return PS_TYPE_NONE;
        }
        ps_lang_type element = expression(c, c->nodes[arg].a, optional_element(c, expected));
        c->info[arg].type = element;
        c->info[id].binding = c->info[n->a].binding = PS_LANG_BUILTIN_OPTIONAL_SOME;
        c->info[n->a].type = PS_TYPE_FUNCTION;
        return optional_type(c, id, element);
    }
    if (callee->kind == PS_AST_MEMBER && c->nodes[callee->a].kind == PS_AST_NAME) {
        size_t owner = lookup(c, c->nodes[callee->a].token);
        if (owner && (c->nodes[owner].kind == PS_AST_STRUCT ||
                      c->nodes[owner].kind == PS_AST_ENUM)) {
            fn = method_lookup(c, c->info[owner].type, callee->token);
            if (!fn) {
                fail(c, n->a, "Expected a static method on this type");
                return PS_TYPE_NONE;
            }
            c->info[callee->a].binding = owner;
            c->info[callee->a].type = c->info[owner].type;
            goto user_call;
        }
    }
    if (callee->kind == PS_AST_MEMBER && c->nodes[callee->a].kind == PS_AST_NAME &&
        !lookup(c, c->nodes[callee->a].token)) {
        ps_lang_token root = c->nodes[callee->a].token;
        if ((word(c, root, "Int64") || word(c, root, "Float64")) &&
            word(c, callee->token, "parse")) {
            if (n->c) {
                fail(c, n->c, "Explicit type arguments require a generic function or method");
                return PS_TYPE_NONE;
            }
            size_t arg = n->b;
            if (!arg || c->nodes[arg].next ||
                (c->nodes[arg].token.length && !word(c, c->nodes[arg].token, "text"))) {
                fail(c, id, "Numeric parse expects one String argument");
                return PS_TYPE_NONE;
            }
            c->info[arg].type = expression(c, c->nodes[arg].a, PS_TYPE_STRING);
            c->info[id].binding = c->info[n->a].binding =
                word(c, root, "Int64") ? PS_LANG_BUILTIN_INT64_PARSE
                                        : PS_LANG_BUILTIN_FLOAT64_PARSE;
            c->info[n->a].type = PS_TYPE_FUNCTION;
            return optional_type(c, id, word(c, root, "Int64") ? PS_TYPE_INT64
                                                                : PS_TYPE_FLOAT64);
        }
        size_t binding = 0;
        const ps_lang_builtin *factory = ps_lang_static_method_find(
            c->source + root.offset, root.length, c->source + callee->token.offset,
            callee->token.length, &binding);
        if (factory)
            return library_call(c, id, factory, binding, 0);
    }
    ps_lang_type member_owner = PS_TYPE_NONE;
    if (callee->kind == PS_AST_MEMBER) {
        member_owner = expression(c, callee->a, PS_TYPE_NONE);
        ps_lang_type element = optional_element(c, member_owner);
        if (element) {
            if (n->c) {
                fail(c, n->c, "Explicit type arguments require a generic function or method");
                return PS_TYPE_NONE;
            }
            int unwrap = word(c, callee->token, "unwrap");
            size_t arg = n->b;
            if ((!unwrap && !word(c, callee->token, "valueOr")) || (unwrap && arg) ||
                (!unwrap && (!arg || c->nodes[arg].next ||
                 (c->nodes[arg].token.length && !word(c, c->nodes[arg].token, "fallback"))))) {
                fail(c, id, "Optional methods are unwrap() and valueOr(fallback)");
                return PS_TYPE_NONE;
            }
            if (arg)
                c->info[arg].type = expression(c, c->nodes[arg].a, element);
            c->info[id].binding = c->info[n->a].binding = unwrap ? PS_LANG_BUILTIN_OPTIONAL_UNWRAP
                                                                : PS_LANG_BUILTIN_OPTIONAL_OR;
            c->info[n->a].type = PS_TYPE_FUNCTION;
            c->info[n->a].receiver_slot = 1;
            return element;
        }
        fn = method_lookup(c, member_owner, callee->token);
        if (fn) {
            c->info[n->a].receiver_slot = 1;
            goto user_call;
        }
    }
    if (callee->kind == PS_AST_MEMBER && member_owner == PS_TYPE_INT64 &&
        (word(c, callee->token, "isMultiple") || word(c, callee->token, "signum"))) {
        if (n->c) {
            fail(c, n->c, "Explicit type arguments require a generic function or method");
            return PS_TYPE_NONE;
        }
        int multiple = word(c, callee->token, "isMultiple");
        size_t arg = n->b;
        if (multiple ? (!arg || c->nodes[arg].next ||
                        !word(c, c->nodes[arg].token, "of")) : arg) {
            fail(c, id, multiple ? "Int64.isMultiple expects one of: argument"
                                 : "Int64.signum expects no arguments");
            return PS_TYPE_NONE;
        }
        if (arg)
            c->info[arg].type = expression(c, c->nodes[arg].a, PS_TYPE_INT64);
        c->info[id].binding = c->info[n->a].binding =
            multiple ? PS_LANG_BUILTIN_INT64_IS_MULTIPLE : PS_LANG_BUILTIN_INT64_SIGNUM;
        c->info[n->a].type = PS_TYPE_FUNCTION;
        c->info[n->a].receiver_slot = 1;
        return multiple ? PS_TYPE_BOOL : PS_TYPE_INT64;
    }
    if (callee->kind == PS_AST_MEMBER &&
        (word(c, callee->token, "prefix") || word(c, callee->token, "suffix") ||
         word(c, callee->token, "dropFirst") || word(c, callee->token, "dropLast")) &&
        !(word(c, callee->token, "prefix") && n->b &&
          word(c, c->nodes[n->b].token, "while"))) {
        if (n->c) {
            fail(c, n->c, "Explicit type arguments require a generic function or method");
            return PS_TYPE_NONE;
        }
        if (!array_element(c, member_owner) && member_owner != PS_TYPE_STRING) {
            fail(c, n->a, "Edge selection requires an array or String value");
            return PS_TYPE_NONE;
        }
        int prefix = word(c, callee->token, "prefix");
        int suffix = word(c, callee->token, "suffix");
        int first = word(c, callee->token, "dropFirst");
        size_t arg = n->b;
        if (((prefix || suffix) && !arg) ||
            (arg && (c->nodes[arg].next || c->nodes[arg].token.length))) {
            fail(c, id, "prefix and suffix require one positional count; dropFirst and dropLast accept zero or one");
            return PS_TYPE_NONE;
        }
        if (arg)
            c->info[arg].type = expression(c, c->nodes[arg].a, PS_TYPE_INT64);
        c->info[id].binding = c->info[n->a].binding =
            prefix ? PS_LANG_BUILTIN_ARRAY_PREFIX
            : suffix ? PS_LANG_BUILTIN_ARRAY_SUFFIX
            : first ? PS_LANG_BUILTIN_ARRAY_DROP_FIRST
                    : PS_LANG_BUILTIN_ARRAY_DROP_LAST;
        c->info[n->a].type = PS_TYPE_FUNCTION;
        c->info[n->a].receiver_slot = 1;
        return member_owner;
    }
    if (callee->kind == PS_AST_MEMBER && word(c, callee->token, "popLast")) {
        if (n->c) {
            fail(c, n->c, "Explicit type arguments require a generic function or method");
            return PS_TYPE_NONE;
        }
        ps_lang_type element = array_element(c, member_owner);
        if (!element) {
            fail(c, n->a, "Safe removal requires an array value");
            return PS_TYPE_NONE;
        }
        if (!mutable_target(c, callee->a))
            fail(c, callee->a, "Safe removal requires a mutable var binding");
        if (n->b) {
            fail(c, id, "Array popLast expects no arguments");
            return PS_TYPE_NONE;
        }
        c->info[id].binding = c->info[n->a].binding = PS_LANG_BUILTIN_ARRAY_POP_LAST;
        c->info[n->a].type = PS_TYPE_FUNCTION;
        c->info[n->a].receiver_slot = 1;
        return optional_type(c, id, element);
    }
    if (callee->kind == PS_AST_MEMBER &&
        (word(c, callee->token, "removeFirst") ||
         word(c, callee->token, "removeLast"))) {
        if (n->c) {
            fail(c, n->c, "Explicit type arguments require a generic function or method");
            return PS_TYPE_NONE;
        }
        ps_lang_type element = array_element(c, member_owner);
        if (!element) {
            fail(c, n->a, "Array edge removal requires an array value");
            return PS_TYPE_NONE;
        }
        if (!mutable_target(c, callee->a))
            fail(c, callee->a, "Array edge removal requires a mutable var binding");
        int first = word(c, callee->token, "removeFirst");
        size_t arg = n->b;
        if (arg && (c->nodes[arg].next || c->nodes[arg].token.length)) {
            fail(c, id, "Array edge removal expects zero or one positional count");
            return PS_TYPE_NONE;
        }
        if (arg)
            c->info[arg].type = expression(c, c->nodes[arg].a, PS_TYPE_INT64);
        c->info[id].binding = c->info[n->a].binding =
            arg ? (first ? PS_LANG_BUILTIN_ARRAY_REMOVE_FIRST_COUNT
                         : PS_LANG_BUILTIN_ARRAY_REMOVE_LAST_COUNT)
                : (first ? PS_LANG_BUILTIN_ARRAY_REMOVE_FIRST
                         : PS_LANG_BUILTIN_ARRAY_REMOVE_LAST);
        c->info[n->a].type = PS_TYPE_FUNCTION;
        c->info[n->a].receiver_slot = 1;
        return arg ? PS_TYPE_VOID : element;
    }
    if (callee->kind == PS_AST_MEMBER &&
        word(c, callee->token, "reduce")) {
        if (n->c) {
            fail(c, n->c, "Explicit type arguments require a generic function or method");
            return PS_TYPE_NONE;
        }
        int string = member_owner == PS_TYPE_STRING;
        ps_lang_type element = string ? PS_TYPE_STRING : array_element(c, member_owner);
        if (!element) {
            fail(c, n->a, "reduce requires an Array or String value");
            return PS_TYPE_NONE;
        }
        size_t initial = n->b, combine = initial ? c->nodes[initial].next : 0;
        if (!initial || !combine || c->nodes[combine].next ||
            c->nodes[initial].token.length || c->nodes[combine].token.length) {
            fail(c, id, string
                ? "String reduce expects initial value and positional combine function"
                : "Array reduce expects initial value and positional combine function");
            return PS_TYPE_NONE;
        }
        ps_lang_type function = expression(c, c->nodes[combine].a, PS_TYPE_NONE);
        c->info[combine].type = function;
        if (!ps_lang_function_type(function)) {
            fail(c, combine, string
                ? "String reduce combine must be func(Accumulator, String) -> Accumulator"
                : "Array reduce combine must be func(Accumulator, Element) -> Accumulator");
            return PS_TYPE_NONE;
        }
        size_t signature = (size_t)(function - PS_TYPE_FUNCTION_BASE);
        size_t first = c->info[signature].function_parameter;
        size_t second = first ? c->nodes[first].next : 0;
        ps_lang_type accumulator = c->info[signature].function_result;
        if (!first || !second || c->nodes[second].next ||
            c->info[first].type != accumulator || c->info[second].type != element ||
            !value_type(accumulator)) {
            fail(c, combine, string
                ? "String reduce combine must be func(Accumulator, String) -> Accumulator"
                : "Array reduce combine must be func(Accumulator, Element) -> Accumulator");
            return PS_TYPE_NONE;
        }
        c->info[initial].type = expression(c, c->nodes[initial].a, accumulator);
        c->info[id].binding = c->info[n->a].binding = string
            ? PS_LANG_BUILTIN_STRING_REDUCE : PS_LANG_BUILTIN_ARRAY_REDUCE;
        c->info[n->a].type = PS_TYPE_FUNCTION;
        c->info[n->a].receiver_slot = 1;
        return accumulator;
    }
    if (callee->kind == PS_AST_MEMBER && word(c, callee->token, "removeAll") &&
        !n->b) {
        if (n->c) {
            fail(c, n->c, "Explicit type arguments require a generic function or method");
            return PS_TYPE_NONE;
        }
        if (!array_element(c, member_owner)) {
            fail(c, n->a, "Array removeAll requires an array value");
            return PS_TYPE_NONE;
        }
        if (!mutable_target(c, callee->a))
            fail(c, callee->a, "Array removeAll requires a mutable var binding");
        c->info[id].binding = c->info[n->a].binding = PS_LANG_BUILTIN_ARRAY_REMOVE_ALL;
        c->info[n->a].type = PS_TYPE_FUNCTION;
        c->info[n->a].receiver_slot = 1;
        return PS_TYPE_VOID;
    }
    if (callee->kind == PS_AST_MEMBER && word(c, callee->token, "forEach")) {
        if (n->c) {
            fail(c, n->c, "Explicit type arguments require a generic function or method");
            return PS_TYPE_NONE;
        }
        int string = member_owner == PS_TYPE_STRING;
        ps_lang_type element = string ? PS_TYPE_STRING : array_element(c, member_owner);
        if (!element) {
            fail(c, n->a, "forEach requires an Array or String value");
            return PS_TYPE_NONE;
        }
        size_t arg = n->b;
        if (!arg || c->nodes[arg].next || c->nodes[arg].token.length) {
            fail(c, id, string ? "String forEach expects one positional function"
                               : "Array forEach expects one positional function");
            return PS_TYPE_NONE;
        }
        ps_lang_type action = expression(c, c->nodes[arg].a, PS_TYPE_NONE);
        c->info[arg].type = action;
        size_t signature = ps_lang_function_type(action)
            ? (size_t)(action - PS_TYPE_FUNCTION_BASE) : 0;
        size_t parameter = signature ? c->info[signature].function_parameter : 0;
        if (!parameter || c->nodes[parameter].next ||
            c->info[parameter].type != element ||
            c->info[signature].function_result != PS_TYPE_VOID) {
            fail(c, arg, string ? "String forEach body must be func(String) -> Void"
                                : "Array forEach body must be func(Element) -> Void");
            return PS_TYPE_NONE;
        }
        c->info[id].binding = c->info[n->a].binding = string
            ? PS_LANG_BUILTIN_STRING_FOR_EACH : PS_LANG_BUILTIN_ARRAY_FOR_EACH;
        c->info[n->a].type = PS_TYPE_FUNCTION;
        c->info[n->a].receiver_slot = 1;
        return PS_TYPE_VOID;
    }
    if (callee->kind == PS_AST_MEMBER &&
        (word(c, callee->token, "filter") || word(c, callee->token, "map") ||
         word(c, callee->token, "compactMap") ||
         word(c, callee->token, "flatMap") ||
         word(c, callee->token, "drop") ||
         word(c, callee->token, "removeAll") ||
         word(c, callee->token, "allSatisfy") ||
         (word(c, callee->token, "prefix") && n->b &&
          word(c, c->nodes[n->b].token, "while")) ||
         ((word(c, callee->token, "first") ||
           word(c, callee->token, "last") ||
           word(c, callee->token, "firstIndex") ||
           word(c, callee->token, "lastIndex")) && n->b &&
          word(c, c->nodes[n->b].token, "where")) ||
         (word(c, callee->token, "contains") && n->b &&
          word(c, c->nodes[n->b].token, "where")))) {
        int mapping = word(c, callee->token, "map");
        int compact = word(c, callee->token, "compactMap");
        int flattening = word(c, callee->token, "flatMap");
        int prefixing = word(c, callee->token, "prefix");
        int dropping = word(c, callee->token, "drop");
        int removing = word(c, callee->token, "removeAll");
        int any = word(c, callee->token, "contains");
        int all = word(c, callee->token, "allSatisfy");
        int first_match = word(c, callee->token, "first");
        int last_match = word(c, callee->token, "last");
        int first_index_match = word(c, callee->token, "firstIndex");
        int last_index_match = word(c, callee->token, "lastIndex");
        int searched = first_match || last_match || first_index_match || last_index_match;
        int indexed = first_index_match || last_index_match;
        int quantified = any || all;
        if (n->c) {
            fail(c, n->c, "Explicit type arguments require a generic function or method");
            return PS_TYPE_NONE;
        }
        int string_while = member_owner == PS_TYPE_STRING && (prefixing || dropping);
        int string_filter = member_owner == PS_TYPE_STRING &&
            word(c, callee->token, "filter");
        int string_transforming = member_owner == PS_TYPE_STRING &&
            (mapping || compact || flattening);
        int string_mapping = string_transforming && mapping;
        int string_predicate = member_owner == PS_TYPE_STRING &&
            (string_filter || quantified);
        int string_searched = member_owner == PS_TYPE_STRING && searched;
        ps_lang_type element = string_while || string_predicate || string_searched ||
            string_transforming
            ? PS_TYPE_STRING : array_element(c, member_owner);
        if (!element) {
            fail(c, n->a, removing ? "Array removeAll requires an array value"
                         : mapping ? "Array map requires an array value"
                         : compact ? "Array compactMap requires an array value"
                         : flattening ? "Array flatMap requires an array value"
                         : prefixing ? "prefix(while:) requires an Array or String value"
                         : dropping ? "drop(while:) requires an Array or String value"
                         : quantified || searched ? "Array predicate query requires an array value"
                                      : "Array filter requires an array value");
            return PS_TYPE_NONE;
        }
        if (removing && !mutable_target(c, callee->a))
            fail(c, callee->a, "Array removeAll requires a mutable var binding");
        size_t arg = n->b;
        if (!arg || c->nodes[arg].next ||
            (removing || any || searched ? !word(c, c->nodes[arg].token, "where")
                 : prefixing || dropping ? !word(c, c->nodes[arg].token, "while")
                 : c->nodes[arg].token.length != 0)) {
            fail(c, id, removing ? "Array removeAll expects where: predicate"
                       : mapping ? (string_mapping ? "String map expects one positional transform"
                                                   : "Array map expects one positional transform")
                       : compact ? (string_transforming
                             ? "String compactMap expects one positional transform"
                             : "Array compactMap expects one positional transform")
                       : flattening ? (string_transforming
                             ? "String flatMap expects one positional transform"
                             : "Array flatMap expects one positional transform")
                       : prefixing ? (string_while ? "String prefix expects while: predicate"
                                                   : "Array prefix expects while: predicate")
                       : dropping ? (string_while ? "String drop expects while: predicate"
                                                   : "Array drop expects while: predicate")
                       : searched ? (string_searched
                             ? "String predicate search expects where: predicate"
                             : "Array predicate query expects where: predicate")
                       : quantified ? (string_predicate
                             ? (any ? "String contains expects where: predicate"
                                    : "String allSatisfy expects one positional predicate")
                             : "Array predicate query expects one positional predicate")
                       : string_filter ? "String filter expects one positional predicate"
                                       : "Array filter expects one positional predicate");
            return PS_TYPE_NONE;
        }
        ps_lang_type predicate = expression(c, c->nodes[arg].a, PS_TYPE_NONE);
        c->info[arg].type = predicate;
        if (!ps_lang_function_type(predicate)) {
            fail(c, arg, removing ? "Array removeAll predicate must be func(Element) -> Bool"
                         : mapping ? (string_mapping
                               ? "String map transform must be func(String) -> Value"
                               : "Array map transform must be func(Element) -> Value")
                         : compact ? (string_transforming
                               ? "String compactMap transform must be func(String) -> Value?"
                               : "Array compactMap transform must be func(Element) -> Value?")
                         : flattening ? (string_transforming
                               ? "String flatMap transform must be func(String) -> [Value]"
                               : "Array flatMap transform must be func(Element) -> [Value]")
                         : prefixing || dropping ? (string_while
                               ? "String while predicate must be func(String) -> Bool"
                               : "Array while predicate must be func(Element) -> Bool")
                         : quantified || searched ? (string_predicate || string_searched
                               ? "String predicate query requires func(String) -> Bool"
                               : "Array predicate query requires func(Element) -> Bool")
                         : string_filter ? "String filter predicate must be func(String) -> Bool"
                                         : "Array filter predicate must be func(Element) -> Bool");
            return PS_TYPE_NONE;
        }
        size_t signature = (size_t)(predicate - PS_TYPE_FUNCTION_BASE);
        size_t parameter = c->info[signature].function_parameter;
        ps_lang_type result = c->info[signature].function_result;
        if (!parameter || c->nodes[parameter].next ||
            c->info[parameter].type != element ||
            (mapping ? !value_type(result)
                     : compact ? !optional_element(c, result)
                     : flattening ? !array_element(c, result) : result != PS_TYPE_BOOL)) {
            fail(c, arg, removing ? "Array removeAll predicate must be func(Element) -> Bool"
                         : mapping ? (string_mapping
                               ? "String map transform must be func(String) -> Value"
                               : "Array map transform must be func(Element) -> Value")
                         : compact ? (string_transforming
                               ? "String compactMap transform must be func(String) -> Value?"
                               : "Array compactMap transform must be func(Element) -> Value?")
                         : flattening ? (string_transforming
                               ? "String flatMap transform must be func(String) -> [Value]"
                               : "Array flatMap transform must be func(Element) -> [Value]")
                         : prefixing || dropping ? (string_while
                               ? "String while predicate must be func(String) -> Bool"
                               : "Array while predicate must be func(Element) -> Bool")
                         : quantified || searched ? (string_predicate || string_searched
                               ? "String predicate query requires func(String) -> Bool"
                               : "Array predicate query requires func(Element) -> Bool")
                         : string_filter ? "String filter predicate must be func(String) -> Bool"
                                         : "Array filter predicate must be func(Element) -> Bool");
            return PS_TYPE_NONE;
        }
        c->info[id].binding = c->info[n->a].binding =
            removing ? PS_LANG_BUILTIN_ARRAY_REMOVE_ALL_WHERE
                    : mapping ? (string_mapping ? PS_LANG_BUILTIN_STRING_MAP
                                                : PS_LANG_BUILTIN_ARRAY_MAP)
                    : compact ? (string_transforming ? PS_LANG_BUILTIN_STRING_COMPACT_MAP
                                                     : PS_LANG_BUILTIN_ARRAY_COMPACT_MAP)
                    : flattening ? (string_transforming ? PS_LANG_BUILTIN_STRING_FLAT_MAP
                                                        : PS_LANG_BUILTIN_ARRAY_FLAT_MAP)
                    : prefixing ? (string_while ? PS_LANG_BUILTIN_STRING_PREFIX_WHILE
                                               : PS_LANG_BUILTIN_ARRAY_PREFIX_WHILE)
                    : dropping ? (string_while ? PS_LANG_BUILTIN_STRING_DROP_WHILE
                                               : PS_LANG_BUILTIN_ARRAY_DROP_WHILE)
                    : any ? (string_predicate ? PS_LANG_BUILTIN_STRING_ANY
                                              : PS_LANG_BUILTIN_ARRAY_ANY)
                    : all ? (string_predicate ? PS_LANG_BUILTIN_STRING_ALL
                                              : PS_LANG_BUILTIN_ARRAY_ALL)
                    : first_match ? (string_searched ? PS_LANG_BUILTIN_STRING_FIRST_WHERE
                                                     : PS_LANG_BUILTIN_ARRAY_FIRST_WHERE)
                    : last_match ? (string_searched ? PS_LANG_BUILTIN_STRING_LAST_WHERE
                                                    : PS_LANG_BUILTIN_ARRAY_LAST_WHERE)
                    : first_index_match ? (string_searched
                          ? PS_LANG_BUILTIN_STRING_FIRST_INDEX_WHERE
                          : PS_LANG_BUILTIN_ARRAY_FIRST_INDEX_WHERE)
                    : last_index_match ? (string_searched
                          ? PS_LANG_BUILTIN_STRING_LAST_INDEX_WHERE
                          : PS_LANG_BUILTIN_ARRAY_LAST_INDEX_WHERE)
                    : string_filter ? PS_LANG_BUILTIN_STRING_FILTER
                                    : PS_LANG_BUILTIN_ARRAY_FILTER;
        c->info[n->a].type = PS_TYPE_FUNCTION;
        c->info[n->a].receiver_slot = 1;
        return removing ? PS_TYPE_VOID
               : mapping ? array_type(c, id, result)
               : compact ? array_type(c, id, optional_element(c, result))
               : flattening ? array_type(c, id, array_element(c, result))
               : quantified ? PS_TYPE_BOOL
               : searched ? optional_type(c, id, indexed ? PS_TYPE_INT64 : element)
                          : member_owner;
    }
    if (callee->kind == PS_AST_MEMBER && array_element(c, member_owner) &&
        (word(c, callee->token, "starts") ||
         word(c, callee->token, "elementsEqual"))) {
        if (n->c) {
            fail(c, n->c, "Explicit type arguments require a generic function or method");
            return PS_TYPE_NONE;
        }
        int starts = word(c, callee->token, "starts");
        ps_lang_type element = array_element(c, member_owner);
        if (!equatable(c, element)) {
            fail(c, n->a, "Array sequence comparison element type must satisfy Equatable");
            return PS_TYPE_NONE;
        }
        size_t arg = n->b;
        if (!arg || c->nodes[arg].next ||
            (starts ? !word(c, c->nodes[arg].token, "with")
                    : c->nodes[arg].token.length != 0)) {
            fail(c, id, starts ? "Array starts expects with: prefix"
                               : "Array elementsEqual expects one positional array");
            return PS_TYPE_NONE;
        }
        c->info[arg].type = expression(c, c->nodes[arg].a, member_owner);
        c->info[id].binding = c->info[n->a].binding =
            starts ? PS_LANG_BUILTIN_ARRAY_STARTS_WITH : PS_LANG_BUILTIN_ARRAY_ELEMENTS_EQUAL;
        c->info[n->a].type = PS_TYPE_FUNCTION;
        c->info[n->a].receiver_slot = 1;
        return PS_TYPE_BOOL;
    }
    if (callee->kind == PS_AST_MEMBER &&
        (word(c, callee->token, "contains") || word(c, callee->token, "firstIndex") ||
         word(c, callee->token, "lastIndex") || word(c, callee->token, "hasPrefix") ||
         word(c, callee->token, "hasSuffix"))) {
        if (n->c) {
            fail(c, n->c, "Explicit type arguments require a generic function or method");
            return PS_TYPE_NONE;
        }
        int string = member_owner == PS_TYPE_STRING;
        int first_index = word(c, callee->token, "firstIndex");
        int last_index = word(c, callee->token, "lastIndex");
        int prefix = word(c, callee->token, "hasPrefix");
        int suffix = word(c, callee->token, "hasSuffix");
        if (!string && (prefix || suffix)) {
            fail(c, n->a, "String method requires a String value");
            return PS_TYPE_NONE;
        }
        ps_lang_type element = string ? PS_TYPE_STRING : array_element(c, member_owner);
        if (!element) {
            fail(c, n->a, "Array method requires an array value or String");
            return PS_TYPE_NONE;
        }
        if (!string && !equatable(c, element)) {
            fail(c, n->a, "Array element type does not support equality");
            return PS_TYPE_NONE;
        }
        int indexed = first_index || last_index;
        size_t arg = n->b;
        if (!arg || c->nodes[arg].next ||
            (indexed ? !word(c, c->nodes[arg].token, "of")
                     : c->nodes[arg].token.length != 0)) {
            fail(c, id, last_index ? (string ? "String lastIndex expects of: value"
                                              : "Array lastIndex expects of: value")
                       : first_index ? (string ? "String firstIndex expects of: value"
                                               : "Array firstIndex expects of: value")
                       : prefix ? "String hasPrefix expects exactly one positional argument"
                       : suffix ? "String hasSuffix expects exactly one positional argument"
                       : string ? "String contains expects exactly one positional argument"
                                : "Array contains expects exactly one positional argument");
            return PS_TYPE_NONE;
        }
        c->info[arg].type = expression(c, c->nodes[arg].a, element);
        c->info[id].binding = c->info[n->a].binding =
            string ? (first_index ? PS_LANG_BUILTIN_STRING_FIRST_INDEX
                      : last_index ? PS_LANG_BUILTIN_STRING_LAST_INDEX
                      : prefix ? PS_LANG_BUILTIN_STRING_HAS_PREFIX
                      : suffix ? PS_LANG_BUILTIN_STRING_HAS_SUFFIX
                               : PS_LANG_BUILTIN_STRING_CONTAINS)
                   : (first_index ? PS_LANG_BUILTIN_ARRAY_FIRST_INDEX
                      : last_index ? PS_LANG_BUILTIN_ARRAY_LAST_INDEX
                                   : PS_LANG_BUILTIN_ARRAY_CONTAINS);
        c->info[n->a].type = PS_TYPE_FUNCTION;
        c->info[n->a].receiver_slot = 1;
        return indexed ? optional_type(c, id, PS_TYPE_INT64) : PS_TYPE_BOOL;
    }
    if (callee->kind == PS_AST_MEMBER &&
        word(c, callee->token, "replacingOccurrences")) {
        if (n->c) {
            fail(c, n->c, "Explicit type arguments require a generic function or method");
            return PS_TYPE_NONE;
        }
        if (member_owner != PS_TYPE_STRING) {
            fail(c, n->a, "String method requires a String value");
            return PS_TYPE_NONE;
        }
        size_t search = n->b, replacement = search ? c->nodes[search].next : 0;
        if (!search || !replacement || c->nodes[replacement].next ||
            !word(c, c->nodes[search].token, "of") ||
            !word(c, c->nodes[replacement].token, "with")) {
            fail(c, id, "String replacingOccurrences expects of: search, with: replacement");
            return PS_TYPE_NONE;
        }
        c->info[search].type = expression(c, c->nodes[search].a, PS_TYPE_STRING);
        c->info[replacement].type = expression(c, c->nodes[replacement].a, PS_TYPE_STRING);
        c->info[id].binding = c->info[n->a].binding = PS_LANG_BUILTIN_STRING_REPLACING;
        c->info[n->a].type = PS_TYPE_FUNCTION;
        c->info[n->a].receiver_slot = 1;
        return PS_TYPE_STRING;
    }
    if (callee->kind == PS_AST_MEMBER && member_owner == PS_TYPE_STRING &&
        word(c, callee->token, "trimmingCharacters")) {
        if (n->c) {
            fail(c, n->c, "Explicit type arguments require a generic function or method");
            return PS_TYPE_NONE;
        }
        size_t set = n->b;
        if (!set || c->nodes[set].next || !word(c, c->nodes[set].token, "in") ||
            c->nodes[c->nodes[set].a].kind != PS_AST_MEMBER ||
            !word(c, c->nodes[c->nodes[set].a].token, "whitespacesAndNewlines") ||
            c->nodes[c->nodes[c->nodes[set].a].a].kind != PS_AST_NAME ||
            !word(c, c->nodes[c->nodes[c->nodes[set].a].a].token, "CharacterSet")) {
            fail(c, id, "String trimmingCharacters expects in: CharacterSet.whitespacesAndNewlines");
            return PS_TYPE_NONE;
        }
        c->info[id].binding = c->info[n->a].binding = PS_LANG_BUILTIN_STRING_TRIMMED;
        c->info[n->a].type = PS_TYPE_FUNCTION;
        c->info[n->a].receiver_slot = 1;
        return PS_TYPE_STRING;
    }
    if (callee->kind == PS_AST_MEMBER && word(c, callee->token, "split")) {
        if (n->c) {
            fail(c, n->c, "Explicit type arguments require a generic function or method");
            return PS_TYPE_NONE;
        }
        int string = member_owner == PS_TYPE_STRING;
        ps_lang_type element = array_element(c, member_owner);
        if (!string && !element) {
            fail(c, n->a, "split requires an Array or String value");
            return PS_TYPE_NONE;
        }
        if (!string && !equatable(c, element)) {
            fail(c, n->a, "Array split element type must satisfy Equatable");
            return PS_TYPE_NONE;
        }
        size_t separator = n->b;
        size_t second = separator ? c->nodes[separator].next : 0;
        size_t max_splits = second && word(c, c->nodes[second].token, "maxSplits")
                                ? second : 0;
        size_t omit_empty = max_splits ? c->nodes[max_splits].next : second;
        if (!separator || !word(c, c->nodes[separator].token, "separator") ||
            (second && !max_splits && !word(c, c->nodes[second].token,
                                            "omittingEmptySubsequences")) ||
            (omit_empty && (!word(c, c->nodes[omit_empty].token,
                                      "omittingEmptySubsequences") ||
                            c->nodes[omit_empty].next))) {
            fail(c, id, string
                ? "String split expects separator:, optional maxSplits: and omittingEmptySubsequences:"
                : "Array split expects separator:, optional maxSplits: and omittingEmptySubsequences:");
            return PS_TYPE_NONE;
        }
        c->info[separator].type = expression(c, c->nodes[separator].a,
                                             string ? PS_TYPE_STRING : element);
        if (max_splits)
            c->info[max_splits].type = expression(c, c->nodes[max_splits].a, PS_TYPE_INT64);
        if (omit_empty)
            c->info[omit_empty].type = expression(c, c->nodes[omit_empty].a, PS_TYPE_BOOL);
        c->info[id].binding = c->info[n->a].binding =
            string ? PS_LANG_BUILTIN_STRING_SPLIT : PS_LANG_BUILTIN_ARRAY_SPLIT;
        c->info[n->a].type = PS_TYPE_FUNCTION;
        c->info[n->a].receiver_slot = 1;
        return array_type(c, id, string ? PS_TYPE_STRING : member_owner);
    }
    if (callee->kind == PS_AST_MEMBER && word(c, callee->token, "joined")) {
        if (n->c) {
            fail(c, n->c, "Explicit type arguments require a generic function or method");
            return PS_TYPE_NONE;
        }
        if (array_element(c, member_owner) != PS_TYPE_STRING) {
            fail(c, n->a, "joined requires a [String] value");
            return PS_TYPE_NONE;
        }
        size_t separator = n->b;
        if (!separator || c->nodes[separator].next ||
            !word(c, c->nodes[separator].token, "separator")) {
            fail(c, id, "String array joined expects separator: value");
            return PS_TYPE_NONE;
        }
        c->info[separator].type = expression(c, c->nodes[separator].a, PS_TYPE_STRING);
        c->info[id].binding = c->info[n->a].binding = PS_LANG_BUILTIN_STRING_JOINED;
        c->info[n->a].type = PS_TYPE_FUNCTION;
        c->info[n->a].receiver_slot = 1;
        return PS_TYPE_STRING;
    }
    if (callee->kind == PS_AST_MEMBER &&
        (word(c, callee->token, "sort") || word(c, callee->token, "sorted"))) {
        if (n->c) {
            fail(c, n->c, "Explicit type arguments require a generic function or method");
            return PS_TYPE_NONE;
        }
        int string = member_owner == PS_TYPE_STRING;
        ps_lang_type element = string ? PS_TYPE_STRING : array_element(c, member_owner);
        if (!element) {
            fail(c, n->a, "Array sorting requires an array value");
            return PS_TYPE_NONE;
        }
        int mutate = word(c, callee->token, "sort");
        if (mutate && string) {
            fail(c, n->a, "String is immutable; use sorted()");
            return PS_TYPE_NONE;
        }
        if (mutate && !mutable_target(c, callee->a))
            fail(c, callee->a, "Array sort requires a mutable var binding");
        if (n->b) {
            size_t arg = n->b;
            if (c->nodes[arg].next || !word(c, c->nodes[arg].token, "by")) {
                fail(c, id, string ? "String sorting expects by: comparator"
                                   : "Array sorting expects by: comparator");
                return PS_TYPE_NONE;
            }
            ps_lang_type comparator = expression(c, c->nodes[arg].a, PS_TYPE_NONE);
            c->info[arg].type = comparator;
            if (!ps_lang_function_type(comparator)) {
                fail(c, arg, string ? "String comparator must be func(String, String) -> Bool"
                                    : "Array comparator must be func(Element, Element) -> Bool");
                return PS_TYPE_NONE;
            }
            size_t signature = (size_t)(comparator - PS_TYPE_FUNCTION_BASE);
            size_t first = c->info[signature].function_parameter;
            size_t second = first ? c->nodes[first].next : 0;
            if (!first || !second || c->nodes[second].next ||
                c->info[first].type != element || c->info[second].type != element ||
                c->info[signature].function_result != PS_TYPE_BOOL) {
                fail(c, arg, string ? "String comparator must be func(String, String) -> Bool"
                                    : "Array comparator must be func(Element, Element) -> Bool");
                return PS_TYPE_NONE;
            }
            c->info[id].binding = c->info[n->a].binding =
                mutate ? PS_LANG_BUILTIN_ARRAY_SORT_BY
                : string ? PS_LANG_BUILTIN_STRING_SORTED_BY
                         : PS_LANG_BUILTIN_ARRAY_SORTED_BY;
        } else {
            if (element != PS_TYPE_INT64 && element != PS_TYPE_FLOAT64 &&
                element != PS_TYPE_STRING) {
                fail(c, n->a, "Array sorting requires [Int64], [Float64] or [String]");
                return PS_TYPE_NONE;
            }
            c->info[id].binding = c->info[n->a].binding =
                mutate ? PS_LANG_BUILTIN_ARRAY_SORT
                : string ? PS_LANG_BUILTIN_STRING_SORTED
                         : PS_LANG_BUILTIN_ARRAY_SORTED;
        }
        c->info[n->a].type = PS_TYPE_FUNCTION;
        c->info[n->a].receiver_slot = 1;
        return mutate ? PS_TYPE_VOID
             : string ? array_type(c, id, PS_TYPE_STRING) : member_owner;
    }
    if (callee->kind == PS_AST_MEMBER &&
        (word(c, callee->token, "min") || word(c, callee->token, "max"))) {
        if (n->c) {
            fail(c, n->c, "Explicit type arguments require a generic function or method");
            return PS_TYPE_NONE;
        }
        int string = member_owner == PS_TYPE_STRING;
        ps_lang_type element = string ? PS_TYPE_STRING : array_element(c, member_owner);
        if (!element) {
            fail(c, n->a, "Array extrema require an array value");
            return PS_TYPE_NONE;
        }
        int minimum = word(c, callee->token, "min");
        if (n->b) {
            size_t arg = n->b;
            if (c->nodes[arg].next || !word(c, c->nodes[arg].token, "by")) {
                fail(c, id, string ? "String extrema expect by: comparator"
                                   : "Array extrema expect by: comparator");
                return PS_TYPE_NONE;
            }
            ps_lang_type comparator = expression(c, c->nodes[arg].a, PS_TYPE_NONE);
            c->info[arg].type = comparator;
            if (!ps_lang_function_type(comparator)) {
                fail(c, arg, string ? "String comparator must be func(String, String) -> Bool"
                                    : "Array comparator must be func(Element, Element) -> Bool");
                return PS_TYPE_NONE;
            }
            size_t signature = (size_t)(comparator - PS_TYPE_FUNCTION_BASE);
            size_t first = c->info[signature].function_parameter;
            size_t second = first ? c->nodes[first].next : 0;
            if (!first || !second || c->nodes[second].next ||
                c->info[first].type != element || c->info[second].type != element ||
                c->info[signature].function_result != PS_TYPE_BOOL) {
                fail(c, arg, string ? "String comparator must be func(String, String) -> Bool"
                                    : "Array comparator must be func(Element, Element) -> Bool");
                return PS_TYPE_NONE;
            }
            c->info[id].binding = c->info[n->a].binding =
                string ? (minimum ? PS_LANG_BUILTIN_STRING_MIN_BY
                                  : PS_LANG_BUILTIN_STRING_MAX_BY)
                       : (minimum ? PS_LANG_BUILTIN_ARRAY_MIN_BY
                                  : PS_LANG_BUILTIN_ARRAY_MAX_BY);
        } else {
            if (element != PS_TYPE_INT64 && element != PS_TYPE_FLOAT64 &&
                element != PS_TYPE_STRING) {
                fail(c, n->a, "Array extrema require [Int64], [Float64] or [String]");
                return PS_TYPE_NONE;
            }
            c->info[id].binding = c->info[n->a].binding =
                string ? (minimum ? PS_LANG_BUILTIN_STRING_MIN
                                  : PS_LANG_BUILTIN_STRING_MAX)
                       : (minimum ? PS_LANG_BUILTIN_ARRAY_MIN
                                  : PS_LANG_BUILTIN_ARRAY_MAX);
        }
        c->info[n->a].type = PS_TYPE_FUNCTION;
        c->info[n->a].receiver_slot = 1;
        return optional_type(c, id, element);
    }
    if (callee->kind == PS_AST_MEMBER &&
        (word(c, callee->token, "reverse") || word(c, callee->token, "reversed"))) {
        if (n->c) {
            fail(c, n->c, "Explicit type arguments require a generic function or method");
            return PS_TYPE_NONE;
        }
        int mutate = word(c, callee->token, "reverse");
        if (member_owner == PS_TYPE_STRING) {
            if (mutate) {
                fail(c, n->a, "String is immutable; use reversed()");
                return PS_TYPE_NONE;
            }
            if (n->b) {
                fail(c, id, "String reversed expects no arguments");
                return PS_TYPE_NONE;
            }
            c->info[id].binding = c->info[n->a].binding = PS_LANG_BUILTIN_STRING_REVERSED;
            c->info[n->a].type = PS_TYPE_FUNCTION;
            c->info[n->a].receiver_slot = 1;
            return PS_TYPE_STRING;
        }
        if (!array_element(c, member_owner)) {
            fail(c, n->a, "Array reverse requires an array value");
            return PS_TYPE_NONE;
        }
        if (mutate && !mutable_target(c, callee->a))
            fail(c, callee->a, "Array reverse requires a mutable var binding");
        if (n->b) {
            fail(c, id, "Array reverse expects no arguments");
            return PS_TYPE_NONE;
        }
        c->info[id].binding = c->info[n->a].binding =
            mutate ? PS_LANG_BUILTIN_ARRAY_REVERSE : PS_LANG_BUILTIN_ARRAY_REVERSED;
        c->info[n->a].type = PS_TYPE_FUNCTION;
        c->info[n->a].receiver_slot = 1;
        return mutate ? PS_TYPE_VOID : member_owner;
    }
    if (callee->kind == PS_AST_MEMBER &&
        (word(c, callee->token, "removeSubrange") ||
         word(c, callee->token, "replaceSubrange"))) {
        if (n->c) {
            fail(c, n->c, "Explicit type arguments require a generic function or method");
            return PS_TYPE_NONE;
        }
        int replacing = word(c, callee->token, "replaceSubrange");
        if (!array_element(c, member_owner)) {
            fail(c, n->a, "Array subrange method requires an array value");
            return PS_TYPE_NONE;
        }
        if (!mutable_target(c, callee->a))
            fail(c, callee->a, "Array subrange method requires a mutable var binding");
        size_t bounds_arg = n->b, values_arg = bounds_arg ? c->nodes[bounds_arg].next : 0;
        size_t bounds = bounds_arg ? c->nodes[bounds_arg].a : 0;
        if (!bounds_arg || c->nodes[bounds_arg].token.length ||
            !bounds || c->nodes[bounds].kind != PS_AST_BINARY ||
            (c->nodes[bounds].token.kind != PS_LANG_RANGE_OPEN &&
             c->nodes[bounds].token.kind != PS_LANG_RANGE_CLOSED) ||
            !c->nodes[bounds].a || !c->nodes[bounds].b ||
            (replacing ? (!values_arg || c->nodes[values_arg].next ||
                          !word(c, c->nodes[values_arg].token, "with")) : values_arg)) {
            fail(c, id, replacing
                            ? "Array replaceSubrange expects a bounded range and with: array"
                            : "Array removeSubrange expects one bounded range");
            return PS_TYPE_NONE;
        }
        (void)expression(c, c->nodes[bounds].a, PS_TYPE_INT64);
        (void)expression(c, c->nodes[bounds].b, PS_TYPE_INT64);
        c->info[bounds].type = c->info[bounds_arg].type = PS_TYPE_RANGE;
        if (replacing)
            c->info[values_arg].type = expression(c, c->nodes[values_arg].a, member_owner);
        c->info[id].binding = c->info[n->a].binding =
            replacing ? PS_LANG_BUILTIN_ARRAY_REPLACE_SUBRANGE
                      : PS_LANG_BUILTIN_ARRAY_REMOVE_SUBRANGE;
        c->info[n->a].type = PS_TYPE_FUNCTION;
        c->info[n->a].receiver_slot = 1;
        return PS_TYPE_VOID;
    }
    if (callee->kind == PS_AST_MEMBER && word(c, callee->token, "swapAt")) {
        if (n->c) {
            fail(c, n->c, "Explicit type arguments require a generic function or method");
            return PS_TYPE_NONE;
        }
        if (!array_element(c, member_owner)) {
            fail(c, n->a, "Array swapAt requires an array value");
            return PS_TYPE_NONE;
        }
        if (!mutable_target(c, callee->a))
            fail(c, callee->a, "Array swapAt requires a mutable var binding");
        size_t first = n->b, second = first ? c->nodes[first].next : 0;
        if (!first || !second || c->nodes[second].next ||
            c->nodes[first].token.length || c->nodes[second].token.length) {
            fail(c, id, "Array swapAt expects two positional indices");
            return PS_TYPE_NONE;
        }
        c->info[first].type = expression(c, c->nodes[first].a, PS_TYPE_INT64);
        c->info[second].type = expression(c, c->nodes[second].a, PS_TYPE_INT64);
        c->info[id].binding = c->info[n->a].binding = PS_LANG_BUILTIN_ARRAY_SWAP_AT;
        c->info[n->a].type = PS_TYPE_FUNCTION;
        c->info[n->a].receiver_slot = 1;
        return PS_TYPE_VOID;
    }
    if (callee->kind == PS_AST_MEMBER &&
        word(c, callee->token, "insert")) {
        if (n->c) {
            fail(c, n->c, "Explicit type arguments require a generic function or method");
            return PS_TYPE_NONE;
        }
        ps_lang_type owner = member_owner, element = array_element(c, owner);
        if (!element) {
            fail(c, n->a, "Array method requires an array value");
            return PS_TYPE_NONE;
        }
        if (!mutable_target(c, callee->a))
            fail(c, callee->a, "Array method requires a mutable var binding");
        size_t value = n->b, position = value ? c->nodes[value].next : 0;
        int contents = value && word(c, c->nodes[value].token, "contentsOf");
        if (!value || !position || c->nodes[position].next ||
            (c->nodes[value].token.length && !contents) ||
            !word(c, c->nodes[position].token, "at")) {
            fail(c, id, "Array insert expects value and at: index, or contentsOf: array and at: index");
            return PS_TYPE_NONE;
        }
        c->info[value].type = expression(c, c->nodes[value].a, contents ? owner : element);
        c->info[position].type = expression(c, c->nodes[position].a, PS_TYPE_INT64);
        c->info[id].binding = c->info[n->a].binding =
            contents ? PS_LANG_BUILTIN_ARRAY_INSERT_CONTENTS : PS_LANG_BUILTIN_ARRAY_INSERT;
        c->info[n->a].type = PS_TYPE_FUNCTION;
        c->info[n->a].receiver_slot = 1;
        return PS_TYPE_VOID;
    }
    if (callee->kind == PS_AST_MEMBER &&
        (word(c, callee->token, "append") || word(c, callee->token, "remove"))) {
        if (n->c) {
            fail(c, n->c, "Explicit type arguments require a generic function or method");
            return PS_TYPE_NONE;
        }
        int append = word(c, callee->token, "append");
        ps_lang_type owner = member_owner;
        ps_lang_type element = array_element(c, owner);
        if (!element) {
            fail(c, n->a, "Array method requires an array value");
            return PS_TYPE_NONE;
        }
        if (!mutable_target(c, callee->a))
            fail(c, callee->a, "Array method requires a mutable var binding");
        size_t arg = n->b;
        int contents = append && arg && word(c, c->nodes[arg].token, "contentsOf");
        if (!arg || c->nodes[arg].next ||
            (c->nodes[arg].token.length &&
             (append ? !contents : !word(c, c->nodes[arg].token, "at")))) {
            fail(c, id,
                 "Array method expects exactly one positional argument; append also accepts contentsOf: array and removal also accepts at:");
            return PS_TYPE_NONE;
        }
        c->info[arg].type = expression(c, c->nodes[arg].a,
                                       contents ? owner : append ? element : PS_TYPE_INT64);
        c->info[id].binding = c->info[n->a].binding =
            contents ? PS_LANG_BUILTIN_ARRAY_APPEND_CONTENTS
                     : append ? PS_LANG_BUILTIN_ARRAY_APPEND : PS_LANG_BUILTIN_ARRAY_REMOVE;
        c->info[n->a].type = PS_TYPE_FUNCTION;
        c->info[n->a].receiver_slot = 1;
        return append ? PS_TYPE_VOID : element;
    }
    if (callee->kind == PS_AST_MEMBER) {
        ps_lang_type owner = member_owner;
        size_t field = field_lookup(c, owner, callee->token);
        if (field && ps_lang_function_type(field_type(c, field, n->a)))
            return indirect_call(c, id, expression(c, n->a, PS_TYPE_NONE));
        size_t binding = 0;
        const ps_lang_method *method = ps_lang_method_find(owner, c->source + callee->token.offset,
                                                           callee->token.length, &binding);
        if (!method) {
            fail(c, n->a, "Unknown method for this receiver type");
            return PS_TYPE_NONE;
        }
        c->info[n->a].method_mutating = (method->receiver & PS_LANG_METHOD_MUTATING) != 0;
        if (c->info[n->a].method_mutating && !mutable_target(c, callee->a))
            fail(c, callee->a, "Mutating method requires a mutable var binding");
        return library_call(c, id, ps_lang_builtin_get(binding), binding,
                            (method->receiver & PS_LANG_METHOD_RECEIVER_MASK) + 1);
    }
    if (c->nodes[n->a].kind != PS_AST_NAME) {
        ps_lang_type value = expression(c, n->a, PS_TYPE_NONE);
        if (ps_lang_function_type(value))
            return indirect_call(c, id, value);
        fail(c, id, "Expected a named function or function value");
        return PS_TYPE_NONE;
    }
    fn = lookup(c, c->nodes[n->a].token);
    if (fn && ps_lang_function_type(c->info[fn].type) &&
        c->nodes[fn].kind != PS_AST_FUNCTION &&
        c->nodes[fn].kind != PS_AST_LOCAL_FUNCTION)
        return indirect_call(c, id, expression(c, n->a, PS_TYPE_NONE));
    if (!fn && word(c, c->nodes[n->a].token, "attempt")) {
        if (n->c) {
            fail(c, n->c, "Explicit type arguments require a generic function or method");
            return PS_TYPE_NONE;
        }
        size_t arg = n->b;
        if (!arg || c->nodes[arg].next || c->nodes[arg].token.length) {
            fail(c, id, "attempt expects exactly one positional value expression");
            return PS_TYPE_NONE;
        }
        ps_lang_type value = expression(c, c->nodes[arg].a, optional_element(c, expected));
        if (value == PS_TYPE_VOID || value == PS_TYPE_RANGE || value == PS_TYPE_NONE) {
            fail(c, arg, "attempt requires a value expression");
            return PS_TYPE_NONE;
        }
        c->info[arg].type = value;
        c->info[id].binding = c->info[n->a].binding = PS_LANG_BUILTIN_ATTEMPT;
        c->info[n->a].type = PS_TYPE_FUNCTION;
        return optional_type(c, id, value);
    }
    if (!fn && word(c, c->nodes[n->a].token, "Array") && n->b &&
        word(c, c->nodes[n->b].token, "repeating")) {
        if (n->c) {
            fail(c, n->c, "Explicit type arguments require a generic function or method");
            return PS_TYPE_NONE;
        }
        size_t repeated = n->b, count = c->nodes[repeated].next;
        if (!count || c->nodes[count].next ||
            !word(c, c->nodes[count].token, "count")) {
            fail(c, id, "Array(repeating:count:) expects a value and Int64 count");
            return PS_TYPE_NONE;
        }
        ps_lang_type element = expression(c, c->nodes[repeated].a, PS_TYPE_NONE);
        c->info[repeated].type = element;
        c->info[count].type = expression(c, c->nodes[count].a, PS_TYPE_INT64);
        c->info[id].binding = c->info[n->a].binding = PS_LANG_BUILTIN_ARRAY_REPEATED;
        c->info[n->a].type = PS_TYPE_FUNCTION;
        return array_type(c, id, element);
    }
    if (!fn && word(c, c->nodes[n->a].token, "String") && n->b &&
        word(c, c->nodes[n->b].token, "repeating")) {
        if (n->c) {
            fail(c, n->c, "Explicit type arguments require a generic function or method");
            return PS_TYPE_NONE;
        }
        size_t repeated = n->b, count = c->nodes[repeated].next;
        if (!count || c->nodes[count].next ||
            !word(c, c->nodes[count].token, "count")) {
            fail(c, id, "String(repeating:count:) expects a String and Int64 count");
            return PS_TYPE_NONE;
        }
        c->info[repeated].type = expression(c, c->nodes[repeated].a, PS_TYPE_STRING);
        c->info[count].type = expression(c, c->nodes[count].a, PS_TYPE_INT64);
        c->info[id].binding = c->info[n->a].binding = PS_LANG_BUILTIN_STRING_REPEATED;
        c->info[n->a].type = PS_TYPE_FUNCTION;
        return PS_TYPE_STRING;
    }
    if (!fn &&
        (word(c, c->nodes[n->a].token, "Int64") ||
         word(c, c->nodes[n->a].token, "Float64") ||
         word(c, c->nodes[n->a].token, "String"))) {
        if (n->c) {
            fail(c, n->c, "Explicit type arguments require a generic function or method");
            return PS_TYPE_NONE;
        }
        size_t arg = n->b;
        if (!arg || c->nodes[arg].next || c->nodes[arg].token.length) {
            fail(c, id, "Numeric conversion expects exactly one positional argument");
            return PS_TYPE_NONE;
        }
        ps_lang_type input = expression(c, c->nodes[arg].a, PS_TYPE_NONE);
        ps_lang_type result = word(c, c->nodes[n->a].token, "Int64") ? PS_TYPE_INT64
                              : word(c, c->nodes[n->a].token, "Float64") ? PS_TYPE_FLOAT64
                                                                           : PS_TYPE_STRING;
        if (result == PS_TYPE_STRING ? !scalar_type(input) : !numeric(input) && input != PS_TYPE_STRING)
            fail(c, arg, result == PS_TYPE_STRING
                             ? "String conversion requires Bool, Int64, Float64 or String"
                             : "Numeric conversion requires Int64, Float64 or String");
        c->info[arg].type = input;
        c->info[id].binding = result == PS_TYPE_INT64 ? PS_LANG_BUILTIN_INT64
                              : result == PS_TYPE_FLOAT64 ? PS_LANG_BUILTIN_FLOAT64
                                                          : PS_LANG_BUILTIN_STRING_CAST;
        c->info[n->a].type = PS_TYPE_FUNCTION;
        c->info[n->a].binding = c->info[id].binding;
        return result;
    }
    if (!fn) {
        ps_lang_token name = c->nodes[n->a].token;
        size_t binding = 0;
        const ps_lang_builtin *builtin =
            ps_lang_builtin_find(c->source + name.offset, name.length, &binding);
        if (builtin)
            return library_call(c, id, builtin, binding, 0);
    }
    if (!fn &&
        (word(c, c->nodes[n->a].token, "print") || word(c, c->nodes[n->a].token, "assert"))) {
        if (n->c) {
            fail(c, n->c, "Explicit type arguments require a generic function or method");
            return PS_TYPE_NONE;
        }
        int assertion = word(c, c->nodes[n->a].token, "assert");
        size_t arg = n->b;
        size_t message = arg ? c->nodes[arg].next : 0;
        if (!arg || c->nodes[arg].token.length ||
            (message && (!assertion || c->nodes[message].next ||
                         (c->nodes[message].token.length &&
                          !word(c, c->nodes[message].token, "message"))))) {
            fail(c, id, assertion ? "assert expects a Bool and optional String message"
                                  : "Builtin expects exactly one positional argument");
            return PS_TYPE_NONE;
        }
        ps_lang_type t = expression(c, c->nodes[arg].a, assertion ? PS_TYPE_BOOL : PS_TYPE_NONE);
        if (!value_type(t))
            fail(c, arg, "Builtin argument must produce a value");
        else if (!scalar_type(t))
            fail(c, arg, "Builtin requires a scalar value, not a struct");
        c->info[arg].type = t;
        if (message)
            c->info[message].type = expression(c, c->nodes[message].a, PS_TYPE_STRING);
        c->info[id].binding = assertion ? PS_LANG_BUILTIN_ASSERT : PS_LANG_BUILTIN_PRINT;
        c->info[n->a].type = PS_TYPE_FUNCTION;
        c->info[n->a].binding = c->info[id].binding;
        return PS_TYPE_VOID;
    }
user_call:
    if (fn && c->info[fn].method_owner && callee->kind == PS_AST_MEMBER &&
        !word(c, c->nodes[fn].token, "init")) {
        int static_context = !c->info[n->a].receiver_slot;
        fn = method_overload_for_call(c, fn, id, expected, static_context);
        if (!fn)
            return PS_TYPE_NONE;
        c->info[n->a].method_mutating = c->info[fn].method_mutating;
        if (!static_context && c->info[fn].method_mutating &&
            !mutable_target(c, callee->a))
            fail(c, callee->a, "Mutating method requires a mutable var binding");
    }
    if (fn && (c->nodes[fn].kind == PS_AST_FUNCTION ||
               c->nodes[fn].kind == PS_AST_GENERIC_FUNCTION) &&
        !c->info[fn].method_owner &&
        (callee->kind == PS_AST_NAME || callee->kind == PS_AST_MEMBER)) {
        size_t imported = 0;
        if (callee->kind == PS_AST_MEMBER &&
            c->nodes[callee->a].kind == PS_AST_NAME) {
            size_t alias = lookup(c, c->nodes[callee->a].token);
            if (alias && c->nodes[alias].kind == PS_AST_IMPORT)
                imported = alias;
        }
        fn = free_overload_for_call(c, fn, id, imported, expected, 1);
        if (!fn)
            return PS_TYPE_NONE;
    }
    int generic_record_call = fn && c->nodes[fn].kind == PS_AST_GENERIC_STRUCT;
    size_t inferred_initializer = 0;
    if (generic_record_call && c->function && !n->c) {
        size_t owner = c->info[c->function].method_owner;
        for (size_t i = 0; i < c->specialization_count; i++)
            if (owner && c->specializations[i].original == fn &&
                c->specializations[i].concrete == owner &&
                is_initializer(c, owner, c->function)) {
                fn = owner;
                generic_record_call = 0;
                break;
            }
    }
    if (generic_record_call) {
        size_t contextual_record = 0;
        if (!n->c && ps_lang_record_type(expected))
            for (size_t i = 0; i < c->specialization_count; i++)
                if (c->specializations[i].original == fn &&
                    c->specializations[i].concrete ==
                        (size_t)(expected - PS_TYPE_RECORD_BASE)) {
                    contextual_record = c->specializations[i].concrete;
                    break;
                }
        fn = n->c ? specialize_record(c, fn, n->c, NULL, id)
                  : contextual_record ? contextual_record
                    : infer_record_constructor(c, fn, id, expected, &inferred_initializer);
        if (!fn)
            return PS_TYPE_NONE;
    }
    if (fn && c->nodes[fn].kind == PS_AST_STRUCT) {
        size_t custom = initializer(c, fn);
        if (custom && !is_initializer(c, fn, c->function)) {
            fn = inferred_initializer ? inferred_initializer
                                      : initializer_for_call(c, fn, id,
                                                             n->c && !generic_record_call);
            if (!fn)
                return PS_TYPE_NONE;
        }
    } else if (fn && c->nodes[fn].kind == PS_AST_FUNCTION &&
               word(c, c->nodes[fn].token, "init") && c->info[fn].method_owner) {
        fn = initializer_for_call(c, c->info[fn].method_owner, id, n->c != 0);
        if (!fn)
            return PS_TYPE_NONE;
    }
    int generic_call = fn && c->nodes[fn].kind == PS_AST_GENERIC_FUNCTION;
    if (n->c && !generic_call && !generic_record_call) {
        fail(c, n->c, "Explicit type arguments require a generic function or method");
        return PS_TYPE_NONE;
    }
    if (generic_call) {
        fn = specialize(c, fn, id, expected, generic_record_call);
        if (!fn)
            return PS_TYPE_NONE;
    }
    if (!fn || (c->nodes[fn].kind != PS_AST_FUNCTION &&
                c->nodes[fn].kind != PS_AST_LOCAL_FUNCTION &&
                c->nodes[fn].kind != PS_AST_STRUCT)) {
        fail(c, n->a, "Unknown function or value is not callable");
        return PS_TYPE_NONE;
    }
    c->info[n->a].type = c->nodes[fn].kind == PS_AST_LOCAL_FUNCTION
                                ? c->info[fn].function_type : PS_TYPE_FUNCTION;
    c->info[n->a].binding = fn;
    c->info[id].binding = fn;
    if (c->nodes[fn].kind == PS_AST_LOCAL_FUNCTION)
        capture_reference(c, fn, n->a);
    size_t parameter = c->nodes[fn].a;
    if (c->info[fn].method_owner && !c->info[fn].method_static)
        parameter = c->nodes[parameter].next;
    size_t first_parameter = parameter;
    int named = n->b && c->nodes[n->b].token.length != 0;
    size_t arguments = 0, parameters = 0;
    for (size_t p = parameter; p; p = c->nodes[p].next)
        parameters++;
    for (size_t a = n->b; a && c->error.kind != PS_LANG_ERROR; a = c->nodes[a].next) {
        const ps_lang_node *arg = &c->nodes[a];
        if ((arg->token.length != 0) != named) {
            fail(c, a, "Use either positional or named arguments, not both");
            break;
        }
        size_t p = parameter;
        if (named) {
            for (p = first_parameter; p; p = c->nodes[p].next)
                if (same(c, arg->token, c->nodes[p].token))
                    break;
        }
        if (!p) {
            fail(c, a, "Unknown parameter name or too many arguments");
            break;
        }
        for (size_t prior = n->b; prior != a; prior = c->nodes[prior].next)
            if (c->info[prior].binding == p)
                fail(c, a, "Parameter supplied more than once");
        c->info[a].binding = p;
        c->info[a].type = (generic_call || generic_record_call) && c->info[arg->a].type
                              ? c->info[arg->a].type
                              : expression(c, arg->a, c->info[p].type);
        if ((generic_call || generic_record_call) && c->info[a].type != c->info[p].type)
            fail(c, a, "Generic argument type does not match specialized parameter");
        arguments++;
        if (!named)
            parameter = c->nodes[p].next;
    }
    if (c->nodes[fn].kind == PS_AST_STRUCT) {
        for (size_t p = first_parameter; p && c->error.kind != PS_LANG_ERROR;
             p = c->nodes[p].next) {
            int supplied = 0;
            for (size_t a = n->b; a; a = c->nodes[a].next)
                supplied |= c->info[a].binding == p;
            if (!supplied && !c->nodes[p].b)
                fail(c, id, "Missing struct fields");
        }
    } else if (arguments != parameters)
        fail(c, id, "Missing function arguments");
    return c->nodes[fn].kind == PS_AST_LOCAL_FUNCTION
               ? c->info[fn].lambda_result : c->info[fn].type;
}
static ps_lang_type binary(checker *c, size_t id, ps_lang_type expected) {
    const ps_lang_node *n = &c->nodes[id];
    ps_lang_kind op = n->token.kind;
    if (op == PS_LANG_COALESCE) {
        ps_lang_type element = expected;
        if (c->nodes[n->a].kind == PS_AST_LITERAL &&
            c->nodes[n->a].token.kind == PS_LANG_NIL) {
            if (!element)
                element = expression(c, n->b, PS_TYPE_NONE);
            if (!element)
                return PS_TYPE_NONE;
            (void)expression(c, n->a, optional_type(c, n->a, element));
            if (expected)
                (void)expression(c, n->b, element);
        } else {
            ps_lang_type hint = element ? optional_type(c, id, element) : PS_TYPE_NONE;
            ps_lang_type source = expression(c, n->a, hint);
            element = optional_element(c, source);
            if (!element) {
                fail(c, id, "Left operand of coalescing operator must be optional");
                return PS_TYPE_NONE;
            }
            (void)expression(c, n->b, element);
        }
        return element;
    }
    int left_nil = c->nodes[n->a].kind == PS_AST_LITERAL && c->nodes[n->a].token.kind == PS_LANG_NIL;
    int right_nil = c->nodes[n->b].kind == PS_AST_LITERAL && c->nodes[n->b].token.kind == PS_LANG_NIL;
    if ((op == PS_LANG_EQ || op == PS_LANG_NE) && (left_nil || right_nil)) {
        if (left_nil && right_nil) {
            fail(c, id, "nil comparison requires an optional value");
            return PS_TYPE_NONE;
        }
        size_t value = left_nil ? n->b : n->a, empty = left_nil ? n->a : n->b;
        ps_lang_type optional = expression(c, value, PS_TYPE_NONE);
        if (!optional_element(c, optional))
            fail(c, id, "nil comparison requires an optional value");
        else
            (void)expression(c, empty, optional);
        return PS_TYPE_BOOL;
    }
    if (op == PS_LANG_AMP || op == PS_LANG_PIPE || op == PS_LANG_CARET ||
        op == PS_LANG_SHIFT_LEFT || op == PS_LANG_SHIFT_RIGHT) {
        ps_lang_type left = expression(c, n->a, PS_TYPE_INT64);
        ps_lang_type right = expression(c, n->b, PS_TYPE_INT64);
        if (left != PS_TYPE_INT64 || right != PS_TYPE_INT64)
            fail(c, id, "Bitwise operators require Int64 operands");
        return PS_TYPE_INT64;
    }
    int arithmetic = op == PS_LANG_PLUS || op == PS_LANG_MINUS || op == PS_LANG_STAR ||
                     op == PS_LANG_SLASH || op == PS_LANG_PERCENT;
    int logic = op == PS_LANG_AND || op == PS_LANG_OR;
    int range = op == PS_LANG_RANGE_OPEN || op == PS_LANG_RANGE_CLOSED;
    ps_lang_type left_hint = known_type(c, n->a, 0), right_hint = known_type(c, n->b, 0);
    if (vector_type(left_hint) || vector_type(right_hint)) {
        ps_lang_type left = expression(
            c, n->a,
            !vector_type(left_hint) && op == PS_LANG_STAR ? PS_TYPE_FLOAT64 : PS_TYPE_NONE);
        ps_lang_type right = expression(
            c, n->b,
            vector_type(left) && (op == PS_LANG_STAR || op == PS_LANG_SLASH) ? PS_TYPE_FLOAT64
                                                                             : PS_TYPE_NONE);
        if ((op == PS_LANG_EQ || op == PS_LANG_NE) && vector_type(left) && left == right)
            return PS_TYPE_BOOL;
        if ((op == PS_LANG_PLUS || op == PS_LANG_MINUS) && vector_type(left) && left == right)
            return left;
        if ((op == PS_LANG_STAR || op == PS_LANG_SLASH) && vector_type(left) &&
            right == PS_TYPE_FLOAT64)
            return left;
        if (op == PS_LANG_STAR && left == PS_TYPE_FLOAT64 && vector_type(right))
            return right;
        fail(c, id, "Invalid vector operator or incompatible operand types");
        return PS_TYPE_NONE;
    }
    if (left_hint == PS_TYPE_QUANTITY || right_hint == PS_TYPE_QUANTITY) {
        ps_lang_type left = expression(c, n->a,
                                       left_hint == PS_TYPE_QUANTITY ? PS_TYPE_QUANTITY
                                                                     : PS_TYPE_FLOAT64);
        ps_lang_type right = expression(c, n->b,
                                        (op == PS_LANG_PLUS || op == PS_LANG_MINUS ||
                                         right_hint == PS_TYPE_QUANTITY)
                                            ? PS_TYPE_QUANTITY : PS_TYPE_FLOAT64);
        if ((op == PS_LANG_PLUS || op == PS_LANG_MINUS) &&
            left == PS_TYPE_QUANTITY && right == PS_TYPE_QUANTITY) {
            int8_t left_dimension[7], right_dimension[7];
            size_t budget = 4096;
            if (known_dimension(c, n->a, left_dimension, 0, &budget) &&
                known_dimension(c, n->b, right_dimension, 0, &budget) &&
                memcmp(left_dimension, right_dimension, sizeof left_dimension) != 0)
                fail(c, id, "Statically incompatible unit dimensions");
            return PS_TYPE_QUANTITY;
        }
        if ((op == PS_LANG_STAR &&
             ((left == PS_TYPE_QUANTITY && right == PS_TYPE_FLOAT64) ||
              (left == PS_TYPE_FLOAT64 && right == PS_TYPE_QUANTITY))) ||
            (op == PS_LANG_SLASH && left == PS_TYPE_QUANTITY && right == PS_TYPE_FLOAT64))
            return PS_TYPE_QUANTITY;
        fail(c, id, "Invalid Quantity operator or operand types");
        return PS_TYPE_NONE;
    }
    ps_lang_type want = logic                             ? PS_TYPE_BOOL
                        : range                           ? PS_TYPE_INT64
                        : op == PS_LANG_PLUS && array_element(c, expected) ? expected
                        : arithmetic && numeric(expected) ? expected
                                                          : PS_TYPE_NONE;
    if (!want && (numeric_hint(c, n->a, 0) == PS_TYPE_FLOAT64 ||
                  numeric_hint(c, n->b, 0) == PS_TYPE_FLOAT64))
        want = PS_TYPE_FLOAT64;
    ps_lang_type left, right;
    if (op == PS_LANG_PLUS && c->nodes[n->a].kind == PS_AST_ARRAY &&
        !c->nodes[n->a].a && !want) {
        right = expression(c, n->b, PS_TYPE_NONE);
        left = expression(c, n->a, right);
    } else {
        left = expression(c, n->a, want);
        right = expression(c, n->b, left);
    }
    if (logic)
        return PS_TYPE_BOOL;
    if (range)
        return PS_TYPE_RANGE;
    if (op == PS_LANG_PLUS && array_element(c, left)) {
        if (left != right)
            fail(c, id, "Array concatenation requires arrays of the same type");
        return left;
    }
    if (op == PS_LANG_PLUS && left == PS_TYPE_STRING) {
        if (right != PS_TYPE_STRING)
            fail(c, id, "String concatenation requires two String values");
        return PS_TYPE_STRING;
    }
    if (op == PS_LANG_EQ || op == PS_LANG_NE) {
        if (optional_element(c, left) || optional_element(c, right)) {
            if (left != right)
                fail(c, id, "Equality requires values of the same type");
            else {
                ps_lang_type element = optional_element(c, left);
                if (!equatable(c, element))
                    fail(c, id, "Optional content type does not support equality");
            }
            return PS_TYPE_BOOL;
        }
        if (left != right || !equatable(c, left))
            fail(c, id, "Equality requires values of the same type");
        return PS_TYPE_BOOL;
    }
    if (left == PS_TYPE_STRING && right == PS_TYPE_STRING &&
        (op == PS_LANG_LT || op == PS_LANG_LE ||
         op == PS_LANG_GT || op == PS_LANG_GE))
        return PS_TYPE_BOOL;
    if (!numeric(left) || left != right)
        fail(c, id, "Operator requires numbers of the same type");
    if (op == PS_LANG_PERCENT && left != PS_TYPE_INT64)
        fail(c, id, "Remainder requires Int64 operands");
    return arithmetic ? left : PS_TYPE_BOOL;
}
static ps_lang_type expression(checker *c, size_t id, ps_lang_type expected) {
    if (!enter(c, id))
        return PS_TYPE_NONE;
    const ps_lang_node *n = &c->nodes[id];
    ps_lang_type t = PS_TYPE_NONE;
    switch (n->kind) {
    case PS_AST_LAMBDA: {
        size_t saved_head = c->head, saved_base = c->scope_base;
        size_t saved_function = c->function, saved_self = c->self_parameter;
        unsigned saved_loops = c->loops;
        int *saved_break = c->switch_break;
        c->info[id].lambda_parent = saved_function;
        function_signature(c, id, 0);
        c->info[id].lambda_result = c->info[id].type;
        t = c->info[id].function_type;
        c->scope_base = c->head;
        c->function = id;
        c->loops = 0;
        c->switch_break = NULL;
        for (size_t p = n->a; p && c->error.kind != PS_LANG_ERROR; p = c->nodes[p].next)
            declare(c, p);
        int returns = sequence(c, c->nodes[n->c].a);
        if (c->info[id].type != PS_TYPE_VOID && !returns)
            fail(c, id, "Not all anonymous function paths return a value");
        c->head = saved_head;
        c->scope_base = saved_base;
        c->function = saved_function;
        c->self_parameter = saved_self;
        c->loops = saved_loops;
        c->switch_break = saved_break;
        break;
    }
    case PS_AST_ARRAY: {
        ps_lang_type element = array_element(c, expected);
        if (!element && !n->a) {
            fail(c, id, "Empty array requires an explicit element type");
            break;
        }
        if (!element) {
            for (size_t item = n->a; item && c->error.kind != PS_LANG_ERROR;
                 item = c->nodes[item].next)
                if (numeric_hint(c, item, 0) == PS_TYPE_FLOAT64)
                    element = PS_TYPE_FLOAT64;
        }
        for (size_t item = n->a; item && c->error.kind != PS_LANG_ERROR;
             item = c->nodes[item].next) {
            ps_lang_type current = expression(c, item, element);
            if (!element)
                element = current;
        }
        if (c->error.kind != PS_LANG_ERROR)
            t = array_type(c, id, element);
        break;
    }
    case PS_AST_INDEX: {
        ps_lang_type owner = expression(c, n->a, PS_TYPE_NONE);
        t = owner == PS_TYPE_STRING ? PS_TYPE_STRING : array_element(c, owner);
        if (!t)
            fail(c, id, "Indexing requires an array value or String");
        if (range_node(c, n->b)) {
            size_t base = c->nodes[n->b].kind == PS_AST_STRIDED_RANGE ? c->nodes[n->b].a : n->b;
            const ps_lang_node *range = &c->nodes[base];
            if (range->a)
                (void)expression(c, range->a, PS_TYPE_INT64);
            if (range->b)
                (void)expression(c, range->b, PS_TYPE_INT64);
            if (base != n->b)
                (void)expression(c, c->nodes[n->b].b, PS_TYPE_INT64);
            c->info[base].type = PS_TYPE_RANGE;
            c->info[n->b].type = PS_TYPE_RANGE;
            t = owner;
        } else
            (void)expression(c, n->b, PS_TYPE_INT64);
        break;
    }
    case PS_AST_LITERAL:
        switch (n->token.kind) {
        case PS_LANG_INTEGER:
            t = expected == PS_TYPE_FLOAT64 ? PS_TYPE_FLOAT64 : PS_TYPE_INT64;
            if (t == PS_TYPE_INT64 || ps_lang_prefixed_integer(c->source, n->token))
                integer(c, id, 0);
            break;
        case PS_LANG_FLOAT:
            t = PS_TYPE_FLOAT64;
            break;
        case PS_LANG_TRUE:
        case PS_LANG_FALSE:
            t = PS_TYPE_BOOL;
            break;
        case PS_LANG_STRING:
            t = PS_TYPE_STRING;
            break;
        case PS_LANG_NIL:
            if (!optional_element(c, expected))
                fail(c, id, "nil requires an explicit optional type");
            else
                t = expected;
            break;
        default:
            fail(c, id, "Unsupported literal");
            break;
        }
        break;
    case PS_AST_NAME: {
        if (word(c, n->token, "self")) {
            if (!c->self_parameter)
                fail(c, id, "self is only available inside a method");
            else {
                t = c->info[c->self_parameter].type;
                c->info[id].binding = c->self_parameter;
                capture_reference(c, c->self_parameter, id);
            }
            break;
        }
        size_t decl = lookup(c, n->token);
        if (!decl)
            fail(c, id, "Unknown name or use before declaration");
        else if (c->nodes[decl].kind == PS_AST_FUNCTION ||
                 c->nodes[decl].kind == PS_AST_GENERIC_FUNCTION) {
            if (c->info[decl].method_owner)
                fail(c, id, "Methods require a receiver and cannot be used as values");
            else {
                decl = free_overload_for_value(c, decl, expected, 0, id);
                if (decl && c->nodes[decl].kind == PS_AST_FUNCTION) {
                    t = c->info[decl].function_type;
                    c->info[id].binding = decl;
                } else if (decl && c->error.kind != PS_LANG_ERROR)
                    fail(c, id, "Generic function values require specialization");
            }
        }
        else if (c->nodes[decl].kind == PS_AST_STRUCT || c->nodes[decl].kind == PS_AST_ENUM)
            fail(c, id, "Types must be constructed, not used as values");
        else {
            t = c->info[decl].type;
            c->info[id].binding = decl;
            capture_reference(c, decl, id);
        }
        break;
    }
    case PS_AST_CALL:
        t = call(c, id, expected);
        break;
    case PS_AST_STRING_SEGMENT:
        t = PS_TYPE_STRING;
        break;
    case PS_AST_INTERPOLATED_STRING:
        for (size_t part = n->a; part && c->error.kind != PS_LANG_ERROR;
             part = c->nodes[part].next) {
            ps_lang_type item = expression(c, part, PS_TYPE_NONE);
            if (item != PS_TYPE_STRING && item != PS_TYPE_BOOL &&
                item != PS_TYPE_INT64 && item != PS_TYPE_FLOAT64)
                fail(c, part, "String interpolation requires Bool, Int64, Float64 or String");
        }
        t = PS_TYPE_STRING;
        break;
    case PS_AST_TYPE_APPLY: {
        size_t generic = 0, imported = 0;
        int static_context = 0;
        const ps_lang_node *target = &c->nodes[n->a];
        if (target->kind == PS_AST_NAME)
            generic = lookup(c, target->token);
        else if (target->kind == PS_AST_MEMBER) {
            size_t owner = c->nodes[target->a].kind == PS_AST_NAME
                               ? lookup(c, c->nodes[target->a].token)
                               : imported_type(c, target->a);
            if (c->nodes[target->a].kind == PS_AST_TYPE_APPLY) {
                size_t record = generic_record_name(c, c->nodes[target->a].a);
                if (record)
                    owner = specialize_record(c, record, c->nodes[target->a].b,
                                              NULL, target->a);
            }
            if (owner && c->nodes[owner].kind == PS_AST_IMPORT) {
                generic = imported_member(c, owner, target->token);
                imported = owner;
            }
            else if (owner && (c->nodes[owner].kind == PS_AST_STRUCT ||
                               c->nodes[owner].kind == PS_AST_ENUM)) {
                generic = method_lookup(c, c->info[owner].type, target->token);
                static_context = 1;
                if (c->nodes[owner].kind == PS_AST_STRUCT &&
                    word(c, target->token, "init"))
                    generic = generic_initializer_for_value(c, owner, id, expected);
            }
            else if (c->error.kind != PS_LANG_ERROR) {
                ps_lang_type receiver = expression(c, target->a, PS_TYPE_NONE);
                generic = method_lookup(c, receiver, target->token);
            }
        }
        if (generic && c->info[generic].method_owner &&
            !word(c, target->token, "init"))
            generic = generic_overload_for_reference(
                c, generic, id, expected, 0,
                c->info[generic].method_owner, static_context);
        else if (generic && (target->kind == PS_AST_NAME || imported) &&
                 !c->info[generic].method_owner)
            generic = generic_overload_for_reference(c, generic, id,
                                                     expected, imported, 0, 0);
        if (!generic || c->nodes[generic].kind != PS_AST_GENERIC_FUNCTION) {
            if (c->error.kind != PS_LANG_ERROR)
                fail(c, id, "Explicit function reference requires a generic function");
        }
        else {
            size_t concrete = specialize(c, generic, id, PS_TYPE_NONE, 0);
            if (concrete) {
                t = c->info[concrete].function_type;
                c->info[id].binding = concrete;
                if (c->nodes[concrete].kind == PS_AST_LOCAL_FUNCTION)
                    capture_reference(c, concrete, id);
            }
        }
        break;
    }
    case PS_AST_MEMBER: {
        if (word(c, n->token, "count") &&
            c->nodes[n->a].kind == PS_AST_MEMBER &&
            word(c, c->nodes[n->a].token, "utf8")) {
            ps_lang_type utf8_owner = expression(c, c->nodes[n->a].a, PS_TYPE_NONE);
            if (utf8_owner == PS_TYPE_STRING) {
                t = PS_TYPE_INT64;
                c->info[id].binding = PS_LANG_BUILTIN_STRING_BYTE_COUNT;
                break;
            }
        }
        if (c->nodes[n->a].kind == PS_AST_NAME) {
            size_t alias = lookup(c, c->nodes[n->a].token);
            if (alias && c->nodes[alias].kind == PS_AST_IMPORT) {
                size_t exported = imported_member(c, alias, n->token);
                if (exported && (c->nodes[exported].kind == PS_AST_FUNCTION ||
                                 c->nodes[exported].kind == PS_AST_GENERIC_FUNCTION))
                    exported = free_overload_for_value(c, exported, expected,
                                                       alias, id);
                if (!exported || (c->nodes[exported].kind != PS_AST_VARIABLE &&
                                  c->nodes[exported].kind != PS_AST_FUNCTION)) {
                    if (c->error.kind != PS_LANG_ERROR)
                        fail(c, id, exported &&
                             c->nodes[exported].kind == PS_AST_GENERIC_FUNCTION
                                 ? "Generic function values require specialization"
                                 : "Unknown exported value in imported module");
                } else {
                    t = c->nodes[exported].kind == PS_AST_FUNCTION
                            ? c->info[exported].function_type : c->info[exported].type;
                    c->info[id].binding = exported;
                    c->info[n->a].binding = alias;
                }
                break;
            }
        }
        size_t declaration = c->nodes[n->a].kind == PS_AST_NAME
                                 ? lookup(c, c->nodes[n->a].token)
                                 : imported_type(c, n->a);
        if (!declaration && c->nodes[n->a].kind == PS_AST_MEMBER) {
            size_t generic = generic_record_name(c, n->a);
            if (generic && c->nodes[generic].kind == PS_AST_GENERIC_ENUM)
                declaration = generic;
        }
        if (c->nodes[n->a].kind == PS_AST_TYPE_APPLY) {
            size_t generic = generic_record_name(c, c->nodes[n->a].a);
            if (!generic)
                fail(c, n->a, "Type arguments require a generic struct");
            else
                declaration = specialize_record(c, generic, c->nodes[n->a].b,
                                                NULL, n->a);
        }
        if (declaration && c->nodes[declaration].kind == PS_AST_GENERIC_ENUM) {
            ps_lang_type result = ps_lang_function_type(expected)
                                      ? c->info[expected - PS_TYPE_FUNCTION_BASE].function_result
                                      : expected;
            declaration = expected_record_specialization(c, declaration, result);
            if (!declaration)
                fail(c, id, "Generic enum case needs explicit type arguments or an expected type");
        }
        if (declaration && (c->nodes[declaration].kind == PS_AST_STRUCT ||
                            c->nodes[declaration].kind == PS_AST_ENUM)) {
            size_t method = method_lookup(c, c->info[declaration].type, n->token);
            if (method) {
                if (c->nodes[declaration].kind == PS_AST_STRUCT &&
                    word(c, n->token, "init"))
                    method = initializer_for_value(c, declaration, id, expected);
                else
                    method = method_overload_for_value(c, method, expected, 1, id);
                if (!method)
                    break;
                if (!c->info[method].method_static)
                    fail(c, id, "Instance method value requires a bound receiver");
                else if (c->nodes[method].kind == PS_AST_GENERIC_FUNCTION)
                    fail(c, id, "Generic method values require specialization");
                else {
                    t = c->info[method].function_type;
                    c->info[id].binding = method;
                    c->info[n->a].binding = declaration;
                    c->info[n->a].type = c->info[declaration].type;
                }
                break;
            }
        }
        int enum_case = declaration && c->nodes[declaration].kind == PS_AST_ENUM;
        ps_lang_type owner =
            enum_case ? c->info[declaration].type : expression(c, n->a, PS_TYPE_NONE);
        if (!enum_case && ps_lang_record_type(owner)) {
            size_t method = method_lookup(c, owner, n->token);
            if (method) {
                method = method_overload_for_value(c, method, expected, 0, id);
                if (!method)
                    break;
                if (c->info[method].method_static)
                    fail(c, id, "Static method value requires its type");
                else if (c->nodes[method].kind == PS_AST_GENERIC_FUNCTION)
                    fail(c, id, "Generic method values require specialization");
                else {
                    t = c->info[method].function_type;
                    c->info[id].binding = method;
                }
                break;
            }
        }
        ps_lang_type array_member = array_element(c, owner);
        if (array_member) {
            if (word(c, n->token, "count")) {
                t = PS_TYPE_INT64;
                c->info[id].binding = PS_LANG_BUILTIN_ARRAY_COUNT;
                break;
            }
            if (word(c, n->token, "isEmpty")) {
                t = PS_TYPE_BOOL;
                c->info[id].binding = PS_LANG_BUILTIN_ARRAY_IS_EMPTY;
                break;
            }
            if (word(c, n->token, "first") || word(c, n->token, "last")) {
                t = optional_type(c, id, array_member);
                c->info[id].binding = word(c, n->token, "first")
                                          ? PS_LANG_BUILTIN_ARRAY_FIRST
                                          : PS_LANG_BUILTIN_ARRAY_LAST;
                break;
            }
        }
        if (owner == PS_TYPE_STRING) {
            if (word(c, n->token, "count")) {
                t = PS_TYPE_INT64;
                c->info[id].binding = PS_LANG_BUILTIN_STRING_COUNT;
                break;
            }
            if (word(c, n->token, "isEmpty")) {
                t = PS_TYPE_BOOL;
                c->info[id].binding = PS_LANG_BUILTIN_STRING_IS_EMPTY;
                break;
            }
            if (word(c, n->token, "first") || word(c, n->token, "last")) {
                t = optional_type(c, id, PS_TYPE_STRING);
                c->info[id].binding = word(c, n->token, "first")
                                          ? PS_LANG_BUILTIN_STRING_FIRST
                                          : PS_LANG_BUILTIN_STRING_LAST;
                break;
            }
        }
        if (!enum_case && enum_type(c, owner) && word(c, n->token, "rawValue")) {
            if (!c->info[owner - PS_TYPE_RECORD_BASE].raw_enum)
                fail(c, id, "Enum does not define raw values");
            else {
                t = PS_TYPE_INT64;
                c->info[id].binding = PS_LANG_BUILTIN_ENUM_RAW_VALUE;
            }
            break;
        }
        if (!enum_case && enum_type(c, owner)) {
            fail(c, id, "Enum cases require the type name, not a value");
            break;
        }
        if (optional_element(c, owner) && word(c, n->token, "hasValue")) {
            t = PS_TYPE_BOOL;
            c->info[id].binding = PS_LANG_BUILTIN_OPTIONAL_HAS;
            break;
        }
        if (enum_case) {
            c->info[n->a].binding = declaration;
            c->info[n->a].type = owner;
        }
        size_t field = field_lookup(c, owner, n->token);
        if (!field)
            fail(c, id, "Unknown field or receiver is not a struct");
        else {
            t = enum_case && c->nodes[field].a
                    ? c->info[field].function_type : field_type(c, field, id);
            c->info[id].binding = field;
        }
        break;
    }
    case PS_AST_BINARY:
        t = binary(c, id, expected);
        break;
    case PS_AST_CONDITIONAL: {
        (void)expression(c, n->a, PS_TYPE_BOOL);
        size_t first = n->b, second = n->c;
        ps_lang_type branch_expected = expected;
        if (!branch_expected && numeric_hint(c, id, 0) == PS_TYPE_FLOAT64)
            branch_expected = PS_TYPE_FLOAT64;
        if (!branch_expected &&
            ((c->nodes[first].kind == PS_AST_LITERAL &&
              c->nodes[first].token.kind == PS_LANG_NIL) ||
             (c->nodes[first].kind == PS_AST_ARRAY && !c->nodes[first].a))) {
            first = n->c;
            second = n->b;
        }
        t = expression(c, first, branch_expected);
        (void)expression(c, second, t);
        if (!value_type(t))
            fail(c, id, "Conditional branches must produce a value");
        break;
    }
    case PS_AST_STRIDED_RANGE:
        (void)expression(c, n->a, PS_TYPE_RANGE);
        (void)expression(c, n->b, PS_TYPE_INT64);
        t = PS_TYPE_RANGE;
        break;
    case PS_AST_UNARY:
        if (n->token.kind == PS_LANG_MINUS && c->nodes[n->a].kind == PS_AST_LITERAL &&
            c->nodes[n->a].token.kind == PS_LANG_INTEGER &&
            (expected != PS_TYPE_FLOAT64 ||
             ps_lang_prefixed_integer(c->source, c->nodes[n->a].token))) {
            integer(c, n->a, 1);
            t = c->info[n->a].type = expected == PS_TYPE_FLOAT64 ? PS_TYPE_FLOAT64 : PS_TYPE_INT64;
        } else {
            t = expression(c, n->a, n->token.kind == PS_LANG_NOT ? PS_TYPE_BOOL
                                     : n->token.kind == PS_LANG_TILDE ? PS_TYPE_INT64 : expected);
        }
        if (n->token.kind == PS_LANG_NOT ? t != PS_TYPE_BOOL
            : n->token.kind == PS_LANG_TILDE ? t != PS_TYPE_INT64
                                             : !numeric(t) && !vector_type(t) &&
                                                   !(t == PS_TYPE_QUANTITY &&
                                                     (n->token.kind == PS_LANG_PLUS ||
                                                      n->token.kind == PS_LANG_MINUS)))
            fail(c, id, "Invalid unary operand type");
        break;
    default:
        fail(c, id, "Expression is not implemented by the type checker yet");
        break;
    }
    if (expected && t && expected != t)
        fail(c, id, "Expression type does not match required type");
    c->info[id].type = t;
    c->depth--;
    return t;
}

static int statement(checker *c, size_t id);
static int mutable_target(checker *c, size_t id) {
    while (c->nodes[id].kind == PS_AST_MEMBER || c->nodes[id].kind == PS_AST_INDEX) {
        if (c->nodes[id].kind == PS_AST_INDEX &&
            c->info[c->nodes[id].a].type == PS_TYPE_STRING)
            return 0;
        if (c->nodes[id].kind == PS_AST_INDEX && range_node(c, c->nodes[id].b))
            return 0;
        if (c->nodes[id].kind == PS_AST_MEMBER) {
            size_t field = c->info[id].binding;
            if (!field || field == PS_LANG_BUILTIN_ARRAY_COUNT ||
                field == PS_LANG_BUILTIN_ARRAY_IS_EMPTY ||
                field == PS_LANG_BUILTIN_ARRAY_FIRST ||
                field == PS_LANG_BUILTIN_ARRAY_LAST ||
                field == PS_LANG_BUILTIN_STRING_COUNT ||
                field == PS_LANG_BUILTIN_STRING_BYTE_COUNT ||
                field == PS_LANG_BUILTIN_STRING_IS_EMPTY ||
                field == PS_LANG_BUILTIN_STRING_FIRST ||
                field == PS_LANG_BUILTIN_STRING_LAST ||
                field == PS_LANG_BUILTIN_OPTIONAL_HAS ||
                field == PS_LANG_MEMBER_VALUE || field == PS_LANG_MEMBER_UNIT ||
                field == PS_LANG_MEMBER_MEDIUM_DENSITY ||
                field == PS_LANG_MEMBER_MEDIUM_VISCOSITY ||
                field == PS_LANG_MEMBER_SUBMERSION_VOLUME ||
                field == PS_LANG_MEMBER_SUBMERSION_CENTROID ||
                field == PS_LANG_MEMBER_STEP_ELAPSED || field == PS_LANG_MEMBER_STEP_NEXT ||
                (field <= PS_LANG_MEMBER_MATERIAL_DENSITY &&
                 field >= PS_LANG_MEMBER_MATERIAL_FRICTION) ||
                (field <= PS_LANG_MEMBER_MEASUREMENT_VALUE && field >= PS_LANG_MEMBER_SKIPPED) ||
                (field <= PS_LANG_MEMBER_BODY_POSITION && field >= PS_LANG_MEMBER_BODY_INERTIA) ||
                (field <= PS_LANG_MEMBER_CONTACT_COUNT && field >= PS_LANG_MEMBER_PAIR_B) ||
                (field <= PS_LANG_MEMBER_ODE_STATE && field >= PS_LANG_MEMBER_ODE_ERROR_NORM) ||
                (field <= PS_LANG_MEMBER_SCALAR_X &&
                 field >= PS_LANG_MEMBER_SCALAR_EVALUATIONS) ||
                (!ps_lang_member_name(field) &&
                 (field >= c->count || c->nodes[field].token.kind != PS_LANG_VAR)))
                return 0;
        }
        id = c->nodes[id].a;
    }
    size_t decl = c->info[id].binding;
    return c->nodes[id].kind == PS_AST_NAME && decl &&
           (!c->info[decl].local_function ||
            c->info[decl].local_function == c->function) &&
           (c->nodes[decl].kind == PS_AST_VARIABLE ||
            c->nodes[decl].kind == PS_AST_OPTIONAL_BINDING ||
            c->nodes[decl].kind == PS_AST_SELF_PARAMETER) &&
           c->nodes[decl].token.kind == PS_LANG_VAR;
}
static int sequence(checker *c, size_t first) {
    int returns = 0;
    for (size_t id = first; id && c->error.kind != PS_LANG_ERROR; id = c->nodes[id].next)
        returns |= statement(c, id);
    return returns;
}
static int block(checker *c, size_t id) {
    size_t saved_head = c->head, saved_base = c->scope_base;
    c->scope_base = c->head;
    int returns = sequence(c, c->nodes[id].a);
    c->head = saved_head;
    c->scope_base = saved_base;
    return returns;
}
static void condition(checker *c, size_t id) {
    if (c->nodes[id].kind != PS_AST_OPTIONAL_BINDING) {
        (void)expression(c, id, PS_TYPE_BOOL);
        return;
    }
    const ps_lang_node *n = &c->nodes[id];
    ps_lang_type expected = PS_TYPE_NONE;
    if (n->b) {
        ps_lang_type payload = annotation(c, n->b, 0);
        if (payload)
            expected = optional_type(c, id, payload);
    }
    ps_lang_type optional = expression(c, n->a, expected);
    ps_lang_type payload = optional_element(c, optional);
    if (!payload)
        fail(c, id, "Conditional binding requires an optional value");
    c->info[id].type = payload;
}
static int conditional_block(checker *c, size_t binding, size_t body) {
    if (c->nodes[binding].kind != PS_AST_OPTIONAL_BINDING)
        return block(c, body);
    size_t saved_head = c->head, saved_base = c->scope_base;
    c->scope_base = c->head;
    declare(c, binding);
    int returns = sequence(c, c->nodes[body].a);
    c->head = saved_head;
    c->scope_base = saved_base;
    return returns;
}
static int guard_else_escapes(checker *c, size_t id, unsigned loops, unsigned switches,
                              unsigned depth);
static int guard_block_escapes(checker *c, size_t block_id, unsigned loops,
                               unsigned switches, unsigned depth) {
    if (depth >= 128)
        return 1;
    for (size_t item = c->nodes[block_id].a; item; item = c->nodes[item].next)
        if (guard_else_escapes(c, item, loops, switches, depth + 1))
            return 1;
    return 0;
}
static int guard_else_escapes(checker *c, size_t id, unsigned loops, unsigned switches,
                              unsigned depth) {
    if (depth >= 128)
        return 1;
    const ps_lang_node *n = &c->nodes[id];
    switch (n->kind) {
    case PS_AST_BREAK:
        return !loops && !switches;
    case PS_AST_CONTINUE:
        return !loops;
    case PS_AST_BLOCK:
        return guard_block_escapes(c, id, loops, switches, depth + 1);
    case PS_AST_IF:
        return guard_block_escapes(c, n->b, loops, switches, depth + 1) ||
               (n->c && guard_else_escapes(c, n->c, loops, switches, depth + 1));
    case PS_AST_GUARD:
        return guard_block_escapes(c, n->b, loops, switches, depth + 1);
    case PS_AST_WHILE:
    case PS_AST_FOR:
        return guard_block_escapes(c, n->b, loops + 1, switches, depth + 1);
    case PS_AST_SWITCH:
        for (size_t arm = n->b; arm; arm = c->nodes[arm].next)
            if (guard_block_escapes(c, c->nodes[arm].b, loops, switches + 1, depth + 1))
                return 1;
        return 0;
    default:
        return 0;
    }
}
static int compare_switch_strings(checker *c, size_t left, size_t right) {
    ps_lang_token a = c->nodes[left].token, b = c->nodes[right].token;
    ps_lang_string_cursor i = {0}, j = {0};
    for (;;) {
        if (!c->work) {
            fail(c, right, "Semantic work budget exceeded");
            return 0;
        }
        c->work--;
        int left_byte = ps_lang_string_next_byte(c->source, a, &i);
        int right_byte = ps_lang_string_next_byte(c->source, b, &j);
        if (left_byte != right_byte)
            return left_byte < right_byte ? -1 : 1;
        if (left_byte < 0)
            return 0;
    }
}
static int same_switch_pattern(checker *c, size_t left, size_t right, ps_lang_type type) {
    if (!c->work) {
        fail(c, right, "Semantic work budget exceeded");
        return 0;
    }
    c->work--;
    if (type == PS_TYPE_FLOAT64) {
        double a, b;
        memcpy(&a, &c->info[left].raw_value, sizeof a);
        memcpy(&b, &c->info[right].raw_value, sizeof b);
        return a == b;
    }
    if (type != PS_TYPE_STRING)
        return c->info[left].raw_value == c->info[right].raw_value;
    return compare_switch_strings(c, left, right) == 0;
}
static size_t enum_pattern_owner(checker *c, size_t root) {
    if (c->nodes[root].kind == PS_AST_NAME)
        return lookup(c, c->nodes[root].token);
    if (c->nodes[root].kind == PS_AST_TYPE_APPLY) {
        size_t generic = generic_record_name(c, c->nodes[root].a);
        if (!generic || c->nodes[generic].kind != PS_AST_GENERIC_ENUM)
            return 0;
        size_t concrete = specialize_record(c, generic, c->nodes[root].b, NULL, root);
        if (concrete) {
            c->info[root].type = c->info[concrete].type;
            c->info[root].binding = concrete;
        }
        return concrete;
    }
    return imported_type(c, root);
}
static void int64_switch_literal(checker *c, size_t id) {
    const ps_lang_node *pattern = &c->nodes[id];
    int signed_literal = pattern->kind == PS_AST_UNARY &&
                         (pattern->token.kind == PS_LANG_MINUS ||
                          pattern->token.kind == PS_LANG_PLUS);
    int negative = signed_literal && pattern->token.kind == PS_LANG_MINUS;
    size_t literal = signed_literal ? pattern->a : id;
    if (c->nodes[literal].kind != PS_AST_LITERAL ||
        c->nodes[literal].token.kind != PS_LANG_INTEGER) {
        fail(c, id, "Int64 switch case must be an integer literal");
        return;
    }
    integer(c, literal, negative);
    if (c->error.kind == PS_LANG_ERROR)
        return;
    uint64_t magnitude = ps_lang_integer_value(c->source, c->nodes[literal].token);
    c->info[id].raw_value = negative
        ? magnitude == (uint64_t)INT64_MAX + 1u ? INT64_MIN : -(int64_t)magnitude
        : (int64_t)magnitude;
    c->info[id].type = PS_TYPE_INT64;
}
static int int64_switch_range(checker *c, size_t id) {
    return c->nodes[id].kind == PS_AST_BINARY &&
           (c->nodes[id].token.kind == PS_LANG_RANGE_CLOSED ||
            c->nodes[id].token.kind == PS_LANG_RANGE_OPEN);
}
static void int64_switch_bounds(checker *c, size_t id, int64_t *lower, int64_t *upper) {
    if (int64_switch_range(c, id)) {
        *lower = c->info[c->nodes[id].a].raw_value;
        *upper = c->info[c->nodes[id].b].raw_value;
        if (c->nodes[id].token.kind == PS_LANG_RANGE_OPEN)
            (*upper)--; /* The checker rejects empty ranges, including INT64_MIN..<INT64_MIN. */
    } else
        *lower = *upper = c->info[id].raw_value;
}
static double float64_switch_value(checker *c, size_t id) {
    double value;
    memcpy(&value, &c->info[id].raw_value, sizeof value);
    return value;
}
static void float64_switch_bounds(checker *c, size_t id, double *lower, double *upper,
                                  int *open) {
    if (int64_switch_range(c, id)) {
        *lower = float64_switch_value(c, c->nodes[id].a);
        *upper = float64_switch_value(c, c->nodes[id].b);
        *open = c->nodes[id].token.kind == PS_LANG_RANGE_OPEN;
    } else {
        *lower = *upper = float64_switch_value(c, id);
        *open = 0;
    }
}
static int payload_literal_pattern(checker *c, size_t id, ps_lang_type type);
static void scalar_switch_pattern(checker *c, size_t id, ps_lang_type type) {
    const ps_lang_node *pattern = &c->nodes[id];
    if (type == PS_TYPE_STRING) {
        if (int64_switch_range(c, id)) {
            size_t lower = pattern->a, upper = pattern->b;
            if (!lower || !upper || int64_switch_range(c, lower) ||
                int64_switch_range(c, upper) ||
                c->nodes[lower].kind != PS_AST_LITERAL ||
                c->nodes[upper].kind != PS_AST_LITERAL ||
                c->nodes[lower].token.kind != PS_LANG_STRING ||
                c->nodes[upper].token.kind != PS_LANG_STRING) {
                fail(c, id, "String switch range requires two string literals");
                return;
            }
            int order = compare_switch_strings(c, lower, upper);
            if (c->error.kind == PS_LANG_ERROR) return;
            if (order > 0 || (order == 0 && pattern->token.kind == PS_LANG_RANGE_OPEN)) {
                fail(c, id, "String switch range must contain at least one value");
                return;
            }
            c->info[lower].type = c->info[upper].type = c->info[id].type = type;
            return;
        }
        if (pattern->kind != PS_AST_LITERAL || pattern->token.kind != PS_LANG_STRING) {
            fail(c, id, "String switch case must be a string literal");
            return;
        }
        c->info[id].type = type;
    } else if (type == PS_TYPE_BOOL) {
        if (pattern->kind != PS_AST_LITERAL ||
            (pattern->token.kind != PS_LANG_TRUE && pattern->token.kind != PS_LANG_FALSE)) {
            fail(c, id, "Bool switch case must be true or false");
            return;
        }
        c->info[id].raw_value = pattern->token.kind == PS_LANG_TRUE;
    } else if (type == PS_TYPE_FLOAT64) {
        if (int64_switch_range(c, id)) {
            if (!pattern->a || !pattern->b ||
                int64_switch_range(c, pattern->a) || int64_switch_range(c, pattern->b)) {
                fail(c, id, "Float64 switch range requires two numeric literals");
                return;
            }
            scalar_switch_pattern(c, pattern->a, type);
            if (c->error.kind == PS_LANG_ERROR) return;
            scalar_switch_pattern(c, pattern->b, type);
            if (c->error.kind == PS_LANG_ERROR) return;
            double lower = float64_switch_value(c, pattern->a);
            double upper = float64_switch_value(c, pattern->b);
            if (pattern->token.kind == PS_LANG_RANGE_OPEN ? lower >= upper : lower > upper) {
                fail(c, id, "Float64 switch range must contain at least one value");
                return;
            }
            c->info[id].type = type;
            return;
        }
        int signed_literal = pattern->kind == PS_AST_UNARY &&
                             (pattern->token.kind == PS_LANG_MINUS ||
                              pattern->token.kind == PS_LANG_PLUS);
        const ps_lang_node *literal = &c->nodes[signed_literal ? pattern->a : id];
        if (literal->kind != PS_AST_LITERAL ||
            (literal->token.kind != PS_LANG_FLOAT &&
             literal->token.kind != PS_LANG_INTEGER)) {
            fail(c, id, "Float64 switch case must be a numeric literal");
            return;
        }
        (void)payload_literal_pattern(c, id, type);
        return;
    } else if (int64_switch_range(c, id)) {
        if (!pattern->a || !pattern->b) {
            fail(c, id, "Int64 switch range requires two integer literals");
            return;
        }
        int64_switch_literal(c, pattern->a);
        if (c->error.kind == PS_LANG_ERROR) return;
        int64_switch_literal(c, pattern->b);
        if (c->error.kind == PS_LANG_ERROR) return;
        int64_t lower = c->info[pattern->a].raw_value;
        int64_t upper = c->info[pattern->b].raw_value;
        if (pattern->token.kind == PS_LANG_RANGE_OPEN ? lower >= upper : lower > upper) {
            fail(c, id, "Int64 switch range must contain at least one value");
            return;
        }
    } else {
        int64_switch_literal(c, id);
        return;
    }
    c->info[id].type = type;
}
static int overlapping_switch_pattern(checker *c, size_t left, size_t right,
                                      ps_lang_type type) {
    if (type == PS_TYPE_STRING) {
        size_t a = int64_switch_range(c, left) ? c->nodes[left].a : left;
        size_t b = int64_switch_range(c, left) ? c->nodes[left].b : left;
        size_t x = int64_switch_range(c, right) ? c->nodes[right].a : right;
        size_t y = int64_switch_range(c, right) ? c->nodes[right].b : right;
        int left_open = int64_switch_range(c, left) &&
                        c->nodes[left].token.kind == PS_LANG_RANGE_OPEN;
        int right_open = int64_switch_range(c, right) &&
                         c->nodes[right].token.kind == PS_LANG_RANGE_OPEN;
        int lower_against_upper = compare_switch_strings(c, a, y);
        if (c->error.kind == PS_LANG_ERROR) return 0;
        int other_against_upper = compare_switch_strings(c, x, b);
        return (lower_against_upper < 0 ||
                (lower_against_upper == 0 && !right_open)) &&
               (other_against_upper < 0 ||
                (other_against_upper == 0 && !left_open));
    }
    if (type != PS_TYPE_INT64 && type != PS_TYPE_FLOAT64)
        return same_switch_pattern(c, left, right, type);
    if (!c->work) {
        fail(c, right, "Semantic work budget exceeded");
        return 0;
    }
    c->work--;
    if (type == PS_TYPE_FLOAT64) {
        double a, b, x, y;
        int left_open, right_open;
        float64_switch_bounds(c, left, &a, &b, &left_open);
        float64_switch_bounds(c, right, &x, &y, &right_open);
        return (a < y || (a == y && !right_open)) &&
               (x < b || (x == b && !left_open));
    }
    int64_t a, b, x, y;
    int64_switch_bounds(c, left, &a, &b);
    int64_switch_bounds(c, right, &x, &y);
    return a <= y && x <= b;
}
static void float_payload_literal(checker *c, size_t id) {
    const ps_lang_node *n = &c->nodes[id];
    int signed_literal = n->kind == PS_AST_UNARY;
    int negative = signed_literal && n->token.kind == PS_LANG_MINUS;
    ps_lang_token token = c->nodes[signed_literal ? n->a : id].token;
    double value;
    if (ps_lang_prefixed_integer(c->source, token))
        value = (double)ps_lang_integer_value(c->source, token);
    else {
        if (token.length > c->work || token.length > SIZE_MAX - 1) {
            fail(c, id, "Semantic work budget exceeded");
            return;
        }
        c->work -= token.length;
        char *text = malloc(token.length + 1);
        if (!text) {
            fail(c, id, "Cannot allocate Float64 pattern text");
            return;
        }
        size_t length = 0;
        for (size_t i = 0; i < token.length; i++) {
            unsigned char byte = c->source[token.offset + i];
            if (byte != '_') text[length++] = (char)byte;
        }
        text[length] = '\0';
        char *end = text;
        const struct lconv *current = localeconv();
        if (current && current->decimal_point && !strcmp(current->decimal_point, "."))
            value = strtod(text, &end);
        else {
#ifdef _WIN32
            _locale_t numeric = _create_locale(LC_NUMERIC, "C");
            if (!numeric) {
                free(text);
                fail(c, id, "Cannot initialize C numeric locale");
                return;
            }
            value = _strtod_l(text, &end, numeric);
            _free_locale(numeric);
#else
            locale_t numeric = newlocale(LC_NUMERIC_MASK, "C", (locale_t)0);
            if (!numeric) {
                free(text);
                fail(c, id, "Cannot initialize C numeric locale");
                return;
            }
            value = strtod_l(text, &end, numeric);
            freelocale(numeric);
#endif
        }
        int valid = end == text + length;
        free(text);
        if (!valid) {
            fail(c, id, "Invalid Float64 pattern literal");
            return;
        }
    }
    if (negative) value = -value;
    if (!isfinite(value)) {
        fail(c, id, "Non-finite Float64 pattern literal");
        return;
    }
    memcpy(&c->info[id].raw_value, &value, sizeof value);
}
static int payload_literal_pattern(checker *c, size_t id, ps_lang_type type) {
    if ((type == PS_TYPE_INT64 || type == PS_TYPE_FLOAT64 ||
         type == PS_TYPE_STRING) &&
        int64_switch_range(c, id)) {
        scalar_switch_pattern(c, id, type);
        return c->error.kind != PS_LANG_ERROR;
    }
    const ps_lang_node *n = &c->nodes[id];
    int signed_literal = n->kind == PS_AST_UNARY &&
                         (n->token.kind == PS_LANG_MINUS || n->token.kind == PS_LANG_PLUS);
    const ps_lang_node *literal = &c->nodes[signed_literal ? n->a : id];
    int valid = literal->kind == PS_AST_LITERAL &&
        (optional_element(c, type) ? !signed_literal && literal->token.kind == PS_LANG_NIL :
         type == PS_TYPE_BOOL ? !signed_literal &&
             (literal->token.kind == PS_LANG_TRUE || literal->token.kind == PS_LANG_FALSE) :
         type == PS_TYPE_STRING ? !signed_literal && literal->token.kind == PS_LANG_STRING :
         type == PS_TYPE_INT64 ? literal->token.kind == PS_LANG_INTEGER :
         type == PS_TYPE_FLOAT64 ? literal->token.kind == PS_LANG_FLOAT ||
                                  literal->token.kind == PS_LANG_INTEGER : 0);
    if (!valid) {
        fail(c, id, "Payload pattern requires a matching scalar literal, Int64, Float64 or String range or nil for an optional field");
        return 0;
    }
    (void)expression(c, id, type);
    if (c->error.kind != PS_LANG_ERROR) {
        if (type == PS_TYPE_FLOAT64)
            float_payload_literal(c, id);
        else if (!optional_element(c, type))
            scalar_switch_pattern(c, id, type);
    }
    return c->error.kind != PS_LANG_ERROR;
}
static int optional_some_member(checker *c, size_t id) {
    return c->nodes[id].kind == PS_AST_MEMBER &&
           word(c, c->nodes[id].token, "some") && c->nodes[id].a &&
           c->nodes[c->nodes[id].a].kind == PS_AST_NAME &&
           word(c, c->nodes[c->nodes[id].a].token, "Optional") &&
           !lookup(c, c->nodes[c->nodes[id].a].token);
}
static int payload_value_pattern(checker *c, size_t id, ps_lang_type type,
                                 unsigned depth) {
    if (depth >= 128 || !c->work) {
        fail(c, id, "Payload pattern nesting or work limit exceeded");
        return 0;
    }
    c->work--;
    const ps_lang_node *n = &c->nodes[id];
    if (n->kind == PS_AST_NAME) {
        c->info[id].type = type;
        if (!word(c, n->token, "_")) declare(c, id);
        return c->error.kind != PS_LANG_ERROR;
    }
    if (optional_element(c, type) &&
        (n->kind == PS_AST_MEMBER || n->kind == PS_AST_CALL)) {
        size_t member = n->kind == PS_AST_CALL ? n->a : id;
        if (!optional_some_member(c, member)) {
            fail(c, id, "Optional payload pattern must be nil or Optional.some");
            return 0;
        }
        c->info[id].type = type;
        if (n->kind == PS_AST_CALL) {
            size_t arg = n->b;
            if (n->c || !arg || !c->nodes[arg].a || c->nodes[arg].next ||
                c->nodes[arg].token.length) {
                fail(c, id, "Optional.some payload pattern requires one positional value");
                return 0;
            }
            return payload_value_pattern(c, c->nodes[arg].a, optional_element(c, type),
                                         depth + 1);
        }
        return 1;
    }
    return payload_literal_pattern(c, id, type);
}
static int partial_payload_pattern(checker *c, size_t id) {
    if (c->nodes[id].kind != PS_AST_CALL)
        return 0;
    for (size_t arg = c->nodes[id].b; arg; arg = c->nodes[arg].next)
        if (c->nodes[c->nodes[arg].a].kind != PS_AST_NAME)
            return 1;
    return 0;
}
static int payload_value_covers(checker *c, size_t prior, size_t current,
                                ps_lang_type type, unsigned depth) {
    if (depth >= 128 || !c->work) {
        fail(c, current, "Payload pattern nesting or work limit exceeded");
        return 0;
    }
    c->work--;
    if (c->nodes[prior].kind == PS_AST_NAME) return 1;
    if (c->nodes[current].kind == PS_AST_NAME) return 0;
    if (optional_element(c, type)) {
        int left_nil = c->nodes[prior].kind == PS_AST_LITERAL;
        int right_nil = c->nodes[current].kind == PS_AST_LITERAL;
        if (left_nil || right_nil) return left_nil && right_nil;
        if (c->nodes[prior].kind == PS_AST_MEMBER) return 1;
        if (c->nodes[current].kind == PS_AST_MEMBER)
            return c->nodes[c->nodes[prior].b].a &&
                   c->nodes[c->nodes[c->nodes[prior].b].a].kind == PS_AST_NAME;
        return payload_value_covers(c, c->nodes[c->nodes[prior].b].a,
                                    c->nodes[c->nodes[current].b].a,
                                    optional_element(c, type), depth + 1);
    }
    if (type == PS_TYPE_INT64) {
        int64_t lower, upper, wanted_lower, wanted_upper;
        int64_switch_bounds(c, prior, &lower, &upper);
        int64_switch_bounds(c, current, &wanted_lower, &wanted_upper);
        return lower <= wanted_lower && upper >= wanted_upper;
    }
    if (type == PS_TYPE_FLOAT64) {
        double lower, upper, wanted_lower, wanted_upper;
        int open, wanted_open;
        float64_switch_bounds(c, prior, &lower, &upper, &open);
        float64_switch_bounds(c, current, &wanted_lower, &wanted_upper,
                              &wanted_open);
        return lower <= wanted_lower &&
               (upper > wanted_upper ||
                (upper == wanted_upper && (!open || wanted_open)));
    }
    if (type == PS_TYPE_STRING) {
        int prior_range = int64_switch_range(c, prior);
        int current_range = int64_switch_range(c, current);
        size_t lower = prior_range ? c->nodes[prior].a : prior;
        size_t upper = prior_range ? c->nodes[prior].b : prior;
        size_t wanted_lower = current_range ? c->nodes[current].a : current;
        size_t wanted_upper = current_range ? c->nodes[current].b : current;
        int lower_order = compare_switch_strings(c, lower, wanted_lower);
        if (c->error.kind == PS_LANG_ERROR) return 0;
        int upper_order = compare_switch_strings(c, upper, wanted_upper);
        return lower_order <= 0 &&
               (upper_order > 0 ||
                (upper_order == 0 &&
                 (!prior_range || c->nodes[prior].token.kind != PS_LANG_RANGE_OPEN ||
                  (current_range &&
                   c->nodes[current].token.kind == PS_LANG_RANGE_OPEN))));
    }
    return same_switch_pattern(c, prior, current, type);
}
static int payload_pattern_covers(checker *c, size_t prior, size_t current,
                                  ps_lang_type optional_type) {
    if (!partial_payload_pattern(c, prior)) return 1;
    if (c->nodes[current].kind != PS_AST_CALL) return 0;
    size_t left = c->nodes[prior].b, right = c->nodes[current].b;
    for (; left && right; left = c->nodes[left].next, right = c->nodes[right].next) {
        size_t a = c->nodes[left].a, b = c->nodes[right].a;
        if (c->nodes[a].kind == PS_AST_NAME) continue;
        ps_lang_type type = optional_type ? optional_element(c, optional_type)
                                          : c->info[c->info[left].binding].type;
        if (!payload_value_covers(c, a, b, type, 0))
            return 0;
    }
    return !left && !right;
}
static void optional_switch_pattern(checker *c, size_t id, size_t arm,
                                    ps_lang_type type) {
    const ps_lang_node *pattern = &c->nodes[id];
    if (pattern->kind == PS_AST_LITERAL && pattern->token.kind == PS_LANG_NIL) {
        c->info[id].raw_value = 0;
        c->info[id].type = type;
        return;
    }
    size_t member = pattern->kind == PS_AST_CALL ? pattern->a : id;
    if (!optional_some_member(c, member)) {
        fail(c, id, "Optional switch case must be nil or Optional.some");
        return;
    }
    if (pattern->kind == PS_AST_CALL) {
        size_t arg = pattern->b;
        if (pattern->c || !arg || !c->nodes[arg].a || c->nodes[arg].next ||
            c->nodes[arg].token.length || pattern->next ||
            c->nodes[arm].a != id) {
            fail(c, id, "Optional.some pattern requires one positional value and its own arm");
            return;
        }
        size_t value = c->nodes[arg].a;
        (void)payload_value_pattern(c, value, optional_element(c, type), 0);
    }
    c->info[id].raw_value = 1;
    c->info[id].type = type;
}
static size_t optional_pattern_at_depth(checker *c, size_t pattern, unsigned depth) {
    for (unsigned i = 0; i < depth; i++) {
        if (c->nodes[pattern].kind != PS_AST_CALL || !c->nodes[pattern].b)
            return 0;
        pattern = c->nodes[c->nodes[pattern].b].a;
    }
    return pattern;
}
static int optional_patterns_cover(checker *c, size_t first_arm, ps_lang_type type,
                                   unsigned depth) {
    if (depth >= 128 || !c->work) {
        fail(c, first_arm, "Optional pattern coverage limit exceeded");
        return 0;
    }
    c->work--;
    int nil = 0, some = 0, yes = 0, no = 0, nested = 0;
    for (size_t arm = first_arm; arm; arm = c->nodes[arm].next) {
        if (c->nodes[arm].c) continue;
        for (size_t root = c->nodes[arm].a; root; root = c->nodes[root].next) {
            if (!c->work) {
                fail(c, root, "Semantic work budget exceeded");
                return 0;
            }
            c->work--;
            size_t id = optional_pattern_at_depth(c, root, depth);
            if (!id) continue;
            const ps_lang_node *pattern = &c->nodes[id];
            if (pattern->kind == PS_AST_NAME) return 1;
            if (optional_element(c, type)) {
                if (pattern->kind == PS_AST_LITERAL)
                    nil = 1;
                else if (pattern->kind == PS_AST_MEMBER)
                    some = 1;
                else if (pattern->kind == PS_AST_CALL) {
                    size_t child = c->nodes[pattern->b].a;
                    if (c->nodes[child].kind == PS_AST_NAME)
                        some = 1;
                    else
                        nested = 1;
                }
            } else if (type == PS_TYPE_BOOL && pattern->kind == PS_AST_LITERAL) {
                yes |= pattern->token.kind == PS_LANG_TRUE;
                no |= pattern->token.kind == PS_LANG_FALSE;
            }
        }
    }
    if (optional_element(c, type))
        return nil && (some || (nested && optional_patterns_cover(
            c, first_arm, optional_element(c, type), depth + 1)));
    return type == PS_TYPE_BOOL && yes && no;
}
typedef enum enum_coverage_branch {
    ENUM_COVER_NIL,
    ENUM_COVER_SOME,
    ENUM_COVER_TRUE,
    ENUM_COVER_FALSE,
    ENUM_COVER_ANY
} enum_coverage_branch;
typedef struct enum_coverage_choice {
    const struct enum_coverage_choice *previous;
    size_t field;
    unsigned optional_depth;
    enum_coverage_branch branch;
} enum_coverage_choice;
static size_t enum_pattern_field_value(checker *c, size_t pattern, size_t field) {
    for (size_t arg = c->nodes[pattern].b; arg; arg = c->nodes[arg].next) {
        if (!c->work) {
            fail(c, arg, "Semantic work budget exceeded");
            return 0;
        }
        c->work--;
        if (c->info[arg].binding == field)
            return c->nodes[arg].a;
    }
    return 0;
}
static int enum_value_covers_choice(checker *c, size_t value,
                                    const enum_coverage_choice *choice) {
    for (unsigned i = 0; i < choice->optional_depth; i++) {
        if (c->nodes[value].kind == PS_AST_NAME ||
            c->nodes[value].kind == PS_AST_MEMBER)
            return 1;
        if (c->nodes[value].kind != PS_AST_CALL)
            return 0;
        value = c->nodes[c->nodes[value].b].a;
    }
    const ps_lang_node *pattern = &c->nodes[value];
    if (pattern->kind == PS_AST_NAME) return 1;
    switch (choice->branch) {
    case ENUM_COVER_NIL:
        return pattern->kind == PS_AST_LITERAL && pattern->token.kind == PS_LANG_NIL;
    case ENUM_COVER_SOME:
        return pattern->kind == PS_AST_MEMBER || pattern->kind == PS_AST_CALL;
    case ENUM_COVER_TRUE:
        return pattern->kind == PS_AST_LITERAL && pattern->token.kind == PS_LANG_TRUE;
    case ENUM_COVER_FALSE:
        return pattern->kind == PS_AST_LITERAL && pattern->token.kind == PS_LANG_FALSE;
    case ENUM_COVER_ANY:
        return 0;
    }
    return 0;
}
static int enum_case_has_covering_pattern(checker *c, size_t first_arm, size_t item,
                                           const enum_coverage_choice *choice) {
    for (size_t arm = first_arm; arm; arm = c->nodes[arm].next) {
        if (c->nodes[arm].c) continue;
        for (size_t pattern = c->nodes[arm].a; pattern;
             pattern = c->nodes[pattern].next) {
            if (!c->work) {
                fail(c, pattern, "Semantic work budget exceeded");
                return 0;
            }
            c->work--;
            if (c->info[pattern].binding != item) continue;
            if (c->nodes[pattern].kind == PS_AST_MEMBER) return 1;
            int matches = 1;
            for (const enum_coverage_choice *current = choice; current;
                 current = current->previous) {
                if (!c->work) {
                    fail(c, pattern, "Semantic work budget exceeded");
                    return 0;
                }
                c->work--;
                size_t value = enum_pattern_field_value(c, pattern, current->field);
                if (!value || !enum_value_covers_choice(c, value, current)) {
                    matches = 0;
                    break;
                }
            }
            if (matches) return 1;
        }
    }
    return 0;
}
static int enum_case_branches_cover(checker *c, size_t first_arm, size_t item,
                                    size_t field, ps_lang_type type,
                                    unsigned optional_depth,
                                    const enum_coverage_choice *previous,
                                    unsigned depth) {
    if (depth >= 128 || !c->work) {
        fail(c, item, "Enum payload coverage limit exceeded");
        return 0;
    }
    c->work--;
    if (!field)
        return enum_case_has_covering_pattern(c, first_arm, item, previous);
    size_t next = c->nodes[field].next;
    ps_lang_type next_type = next ? c->info[next].type : PS_TYPE_NONE;
    enum_coverage_choice choice = {previous, field, optional_depth, ENUM_COVER_ANY};
    ps_lang_type element = optional_element(c, type);
    if (element) {
        choice.branch = ENUM_COVER_NIL;
        if (!enum_case_branches_cover(c, first_arm, item, next, next_type,
                                      0, &choice, depth + 1))
            return 0;
        choice.branch = ENUM_COVER_SOME;
        return enum_case_branches_cover(c, first_arm, item, field, element,
                                        optional_depth + 1, &choice, depth + 1);
    }
    if (type == PS_TYPE_BOOL) {
        choice.branch = ENUM_COVER_TRUE;
        if (!enum_case_branches_cover(c, first_arm, item, next, next_type,
                                      0, &choice, depth + 1))
            return 0;
        choice.branch = ENUM_COVER_FALSE;
        return enum_case_branches_cover(c, first_arm, item, next, next_type,
                                        0, &choice, depth + 1);
    }
    return enum_case_branches_cover(c, first_arm, item, next, next_type,
                                    0, &choice, depth + 1);
}
static int enum_patterns_cover(checker *c, size_t first_arm, ps_lang_type type) {
    for (size_t item = c->nodes[type - PS_TYPE_RECORD_BASE].a; item;
         item = c->nodes[item].next) {
        if (!c->work) {
            fail(c, item, "Semantic work budget exceeded");
            return 0;
        }
        c->work--;
        int full_pattern = 0;
        for (size_t arm = first_arm; arm && !full_pattern; arm = c->nodes[arm].next)
            if (!c->nodes[arm].c)
                for (size_t pattern = c->nodes[arm].a; pattern;
                     pattern = c->nodes[pattern].next) {
                    if (!c->work) {
                        fail(c, pattern, "Semantic work budget exceeded");
                        return 0;
                    }
                    c->work--;
                    if (c->info[pattern].binding == item &&
                        !partial_payload_pattern(c, pattern)) {
                        full_pattern = 1;
                        break;
                    }
                }
        if (full_pattern) continue;
        size_t field = c->nodes[item].a;
        if (!field || !enum_case_branches_cover(c, first_arm, item, field,
                                                c->info[field].type, 0, NULL, 0))
            return 0;
    }
    return 1;
}
static int statement(checker *c, size_t id) {
    if (!enter(c, id))
        return 0;
    const ps_lang_node *n = &c->nodes[id];
    int returns = 0;
    switch (n->kind) {
    case PS_AST_VARIABLE: {
        ps_lang_type want = n->a ? annotation(c, n->a, 0) : PS_TYPE_NONE;
        ps_lang_type t = n->b ? expression(c, n->b, want) : want;
        if (!value_type(t))
            fail(c, id, n->b ? "Variable initializer must produce a value"
                              : "Variable type annotation must name a value type");
        c->info[id].type = t;
        declare(c, id);
        break;
    }
    case PS_AST_ASSIGN: {
        ps_lang_type t = expression(c, n->a, PS_TYPE_NONE);
        size_t target = n->a;
        if (c->nodes[target].kind == PS_AST_INDEX &&
            c->info[c->nodes[target].a].type == PS_TYPE_STRING)
            fail(c, target, "String indices and slices are immutable");
        if (c->nodes[target].kind == PS_AST_INDEX && range_node(c, c->nodes[target].b)) {
            if (c->nodes[c->nodes[target].b].kind == PS_AST_STRIDED_RANGE &&
                n->token.kind != PS_LANG_EQUAL)
                fail(c, id, "Compound assignment to a stepped range is not supported");
            target = c->nodes[target].a;
        }
        if (!mutable_target(c, target))
            fail(c, n->a, "Assignment requires a mutable var binding");
        (void)expression(c, n->b, t);
        break;
    }
    case PS_AST_RETURN:
        if (!c->function)
            fail(c, id, "Return is only allowed inside a function");
        else if (n->a) {
            ps_lang_type result = c->nodes[c->function].kind == PS_AST_LOCAL_FUNCTION
                                      ? c->info[c->function].lambda_result
                                      : c->info[c->function].type;
            ps_lang_type t = expression(c, n->a, result);
            if (t == PS_TYPE_VOID)
                fail(c, id, "Void functions use return without a value");
        } else if ((c->nodes[c->function].kind == PS_AST_LOCAL_FUNCTION
                         ? c->info[c->function].lambda_result
                         : c->info[c->function].type) != PS_TYPE_VOID)
            fail(c, id, "Expected a return value");
        returns = 1;
        break;
    case PS_AST_IF:
        condition(c, n->a);
        returns = conditional_block(c, n->a, n->b);
        if (n->c) {
            int alternative =
                c->nodes[n->c].kind == PS_AST_IF ? statement(c, n->c) : block(c, n->c);
            returns &= alternative;
        } else
            returns = 0;
        break;
    case PS_AST_GUARD: {
        if (!c->function)
            fail(c, id, "Guard is only allowed inside a function");
        condition(c, n->a);
        int alternative_returns = block(c, n->b);
        if (guard_block_escapes(c, n->b, 0, 0, 0))
            fail(c, id, "Guard else cannot break or continue the enclosing flow");
        if (!alternative_returns)
            fail(c, id, "Guard else must return on every path");
        if (c->nodes[n->a].kind == PS_AST_OPTIONAL_BINDING && c->error.kind != PS_LANG_ERROR)
            declare(c, n->a);
        break;
    }
    case PS_AST_SWITCH: {
        ps_lang_type t = expression(c, n->a, PS_TYPE_NONE);
        int scalar = t == PS_TYPE_BOOL || t == PS_TYPE_INT64 || t == PS_TYPE_FLOAT64 ||
                     t == PS_TYPE_STRING;
        int optional = optional_element(c, t) != PS_TYPE_NONE;
        if (!enum_type(c, t) && !scalar && !optional) {
            fail(c, n->a, "Switch requires an enum, optional, Bool, Int64, Float64 or String value");
            break;
        }
        size_t total = t == PS_TYPE_BOOL || optional ? 2 : 0, covered = 0;
        if (!scalar && !optional)
            for (size_t f = c->nodes[t - PS_TYPE_RECORD_BASE].a; f; f = c->nodes[f].next) {
                if (!enter(c, id))
                    break;
                c->depth--;
                total++;
            }
        int has_default = 0, broken = 0;
        int *saved_break = c->switch_break;
        c->switch_break = &broken;
        returns = 1;
        for (size_t arm = n->b; arm && c->error.kind != PS_LANG_ERROR; arm = c->nodes[arm].next) {
            size_t saved_head = c->head, saved_base = c->scope_base;
            c->scope_base = c->head;
            if (c->nodes[arm].a) {
                for (size_t pattern = c->nodes[arm].a;
                     pattern && c->error.kind != PS_LANG_ERROR;
                     pattern = c->nodes[pattern].next) {
                    if (scalar || optional) {
                        if (optional)
                            optional_switch_pattern(c, pattern, arm, t);
                        else
                            scalar_switch_pattern(c, pattern, t);
                        if (c->error.kind == PS_LANG_ERROR)
                            break;
                        for (size_t other = n->b; other != arm && c->error.kind != PS_LANG_ERROR;
                             other = c->nodes[other].next)
                            if (!c->nodes[other].c)
                                for (size_t prior = c->nodes[other].a;
                                     prior && c->error.kind != PS_LANG_ERROR;
                                     prior = c->nodes[prior].next)
                                    if ((optional ? same_switch_pattern(c, prior, pattern,
                                                                        PS_TYPE_INT64)
                                                  : overlapping_switch_pattern(c, prior, pattern,
                                                                               t)) &&
                                        (!optional || payload_pattern_covers(c, prior, pattern, t)))
                                        fail(c, pattern, "Duplicate switch case");
                        for (size_t prior = c->nodes[arm].a;
                             prior != pattern && c->error.kind != PS_LANG_ERROR;
                             prior = c->nodes[prior].next)
                            if (optional ? same_switch_pattern(c, prior, pattern,
                                                                PS_TYPE_INT64)
                                         : overlapping_switch_pattern(c, prior, pattern, t))
                                fail(c, pattern, "Duplicate switch case");
                        if (!c->nodes[arm].c &&
                            (!optional || !partial_payload_pattern(c, pattern))) covered++;
                        continue;
                    }
                    const ps_lang_node *pat = &c->nodes[pattern];
                    if (pat->kind == PS_AST_CALL) {
                        size_t member = pat->a;
                        if (c->nodes[member].kind != PS_AST_MEMBER) {
                            fail(c, pattern, "Switch pattern must name an enum case");
                            break;
                        }
                        size_t root = c->nodes[member].a;
                        size_t owner = enum_pattern_owner(c, root);
                        if (!owner || c->nodes[owner].kind != PS_AST_ENUM ||
                            c->info[owner].type != t) {
                            fail(c, pattern, "Switch pattern must name a case of this enum");
                            break;
                        }
                        size_t item = field_lookup(c, t, c->nodes[member].token);
                        if (!item || !c->nodes[item].a || pat->c || pat->next ||
                            c->nodes[arm].a != pattern) {
                            fail(c, pattern, "Invalid enum payload pattern");
                            break;
                        }
                        c->info[member].binding = c->info[pattern].binding = item;
                        c->info[pattern].type = t;
                        size_t field = c->nodes[item].a;
                        for (size_t a = pat->b; a && c->error.kind != PS_LANG_ERROR;
                             a = c->nodes[a].next) {
                            size_t value = c->nodes[a].a;
                            if (!field || c->nodes[a].token.length) {
                                fail(c, a, "Enum pattern requires positional values");
                                break;
                            }
                            c->info[a].binding = field;
                            (void)payload_value_pattern(c, value, c->info[field].type, 0);
                            field = c->nodes[field].next;
                        }
                        if (field) fail(c, pattern, "Missing enum payload bindings");
                    } else if (pat->kind == PS_AST_MEMBER && enum_payload(c, t)) {
                        size_t root = pat->a;
                        size_t owner = enum_pattern_owner(c, root);
                        size_t item = owner && c->nodes[owner].kind == PS_AST_ENUM &&
                                              c->info[owner].type == t
                                          ? field_lookup(c, t, pat->token) : 0;
                        if (!item) fail(c, pattern, "Switch case must name an enum case");
                        c->info[pattern].binding = item;
                        c->info[pattern].type = t;
                    } else
                        (void)expression(c, pattern, t);
                    if (c->error.kind == PS_LANG_ERROR)
                        break;
                    size_t binding = c->info[pattern].binding;
                    if ((c->nodes[pattern].kind != PS_AST_MEMBER &&
                         c->nodes[pattern].kind != PS_AST_CALL) || !binding ||
                        c->nodes[binding].kind != PS_AST_CASE) {
                        fail(c, pattern, "Switch case must name an enum case");
                        break;
                    }
                    for (size_t other = n->b; other != arm && c->error.kind != PS_LANG_ERROR;
                         other = c->nodes[other].next)
                        if (!c->nodes[other].c)
                            for (size_t prior = c->nodes[other].a;
                                 prior && c->error.kind != PS_LANG_ERROR;
                                 prior = c->nodes[prior].next) {
                                if (!enter(c, pattern))
                                    break;
                                c->depth--;
                                if (c->info[prior].binding == binding &&
                                    payload_pattern_covers(c, prior, pattern, PS_TYPE_NONE))
                                    fail(c, pattern, "Duplicate switch case");
                            }
                    for (size_t prior = c->nodes[arm].a;
                         prior != pattern && c->error.kind != PS_LANG_ERROR;
                         prior = c->nodes[prior].next) {
                        if (!enter(c, pattern))
                            break;
                        c->depth--;
                        if (c->info[prior].binding == binding)
                            fail(c, pattern, "Duplicate switch case");
                    }
                    if (!c->nodes[arm].c && !partial_payload_pattern(c, pattern)) covered++;
                }
            } else
                has_default = 1;
            if (c->nodes[arm].c)
                (void)expression(c, c->nodes[arm].c, PS_TYPE_BOOL);
            returns &= sequence(c, c->nodes[c->nodes[arm].b].a);
            c->head = saved_head;
            c->scope_base = saved_base;
        }
        if (optional && !has_default && covered != total &&
            c->error.kind != PS_LANG_ERROR &&
            optional_patterns_cover(c, n->b, t, 0))
            covered = total;
        if (!scalar && !optional && !has_default && covered != total &&
            c->error.kind != PS_LANG_ERROR && enum_patterns_cover(c, n->b, t))
            covered = total;
        if (!has_default && (t == PS_TYPE_INT64 || t == PS_TYPE_FLOAT64 ||
                             t == PS_TYPE_STRING || covered != total))
            fail(c, id, t == PS_TYPE_INT64 ? "Int64 switch requires default"
                                         : t == PS_TYPE_FLOAT64 ? "Float64 switch requires default"
                                         : t == PS_TYPE_STRING ? "String switch requires default"
                                         : optional ? "Optional switch must cover nil and some or provide default"
                                         : t == PS_TYPE_BOOL ? "Bool switch must cover true and false or provide default"
                                                             : "Switch must cover every enum case or provide default");
        returns &= !broken;
        c->switch_break = saved_break;
        break;
    }
    case PS_AST_WHILE: {
        condition(c, n->a);
        int *saved_break = c->switch_break;
        c->switch_break = NULL;
        c->loops++;
        (void)conditional_block(c, n->a, n->b);
        c->loops--;
        c->switch_break = saved_break;
        break;
    }
    case PS_AST_FOR: {
        ps_lang_type iterable = expression(c, n->a, PS_TYPE_NONE);
        ps_lang_type element = iterable == PS_TYPE_STRING ? PS_TYPE_STRING
                               : array_element(c, iterable);
        if (!element && iterable != PS_TYPE_RANGE)
            fail(c, n->a, "For loop requires an array, String or range of the required type");
        size_t saved_head = c->head, saved_base = c->scope_base;
        c->scope_base = c->head;
        c->info[id].type = element ? element : PS_TYPE_INT64;
        declare(c, id);
        int *saved_break = c->switch_break;
        c->switch_break = NULL;
        c->loops++;
        (void)sequence(c, c->nodes[n->b].a);
        c->loops--;
        c->switch_break = saved_break;
        c->head = saved_head;
        c->scope_base = saved_base;
        break;
    }
    case PS_AST_BREAK:
        if (c->switch_break)
            *c->switch_break = 1;
        else if (!c->loops)
            fail(c, id, "Break and continue require an enclosing loop; break also accepts switch");
        break;
    case PS_AST_CONTINUE:
        if (!c->loops)
            fail(c, id, "Break and continue require an enclosing loop");
        break;
    case PS_AST_EXPRESSION:
        (void)expression(c, n->a, PS_TYPE_NONE);
        break;
    case PS_AST_LOCAL_FUNCTION: {
        size_t saved_function = c->function, saved_self = c->self_parameter;
        unsigned saved_loops = c->loops;
        int *saved_break = c->switch_break;
        c->info[id].lambda_parent = saved_function;
        function_signature(c, id, 0);
        c->info[id].lambda_result = c->info[id].type;
        c->info[id].type = c->info[id].function_type;
        size_t saved_head = c->head, saved_base = c->scope_base;
        c->scope_base = c->head;
        c->function = id;
        c->loops = 0;
        c->switch_break = NULL;
        for (size_t p = n->a; p && c->error.kind != PS_LANG_ERROR; p = c->nodes[p].next)
            declare(c, p);
        int local_returns = sequence(c, c->nodes[n->c].a);
        if (c->info[id].lambda_result != PS_TYPE_VOID && !local_returns)
            fail(c, id, "Not all local function paths return a value");
        c->head = saved_head;
        c->scope_base = saved_base;
        c->function = saved_function;
        c->self_parameter = saved_self;
        c->loops = saved_loops;
        c->switch_break = saved_break;
        break;
    }
    case PS_AST_GENERIC_FUNCTION:
        c->info[id].lambda_parent = c->function;
        generic_signature(c, id, 0);
        break;
    case PS_AST_STRUCT:
    case PS_AST_ENUM:
    case PS_AST_GENERIC_STRUCT:
    case PS_AST_GENERIC_ENUM:
        fail(c, id, "Type declarations are only supported at module scope");
        break;
    default:
        fail(c, id, "Unsupported statement in type checker");
        break;
    }
    c->depth--;
    return returns;
}
static void check_record(checker *c, size_t id) {
    if (c->info[id].record_state == 2 || !enter(c, id))
        return;
    if (c->info[id].record_state == 1) {
        fail(c, id, c->nodes[id].kind == PS_AST_ENUM
                        ? "Recursive enum values have infinite size"
                        : "Recursive struct values have infinite size");
        c->depth--;
        return;
    }
    c->info[id].record_state = 1;
    if (c->nodes[id].kind == PS_AST_ENUM) {
        size_t maximum = 0;
        for (size_t item = c->nodes[id].a; item && c->error.kind != PS_LANG_ERROR;
             item = c->nodes[item].next) {
            size_t size = 0;
            for (size_t f = c->nodes[item].a; f && c->error.kind != PS_LANG_ERROR;
                 f = c->nodes[f].next) {
                ps_lang_type t = c->info[f].type;
                size_t part = 512; /* Conservative upper bound for SDK value types. */
                if (scalar_type(t) || vector_type(t)) part = 32;
                if (t >= PS_TYPE_OPTIONAL_BASE) part = 64;
                if (ps_lang_record_type(t)) {
                    size_t child = (size_t)(t - PS_TYPE_RECORD_BASE);
                    check_record(c, child);
                    part = c->info[child].value_size;
                }
                if (part > 1024u * 1024u - 8u - size) {
                    fail(c, f, "Enum payload exceeds 1 MiB layout budget");
                    break;
                }
                size += part;
            }
            if (size > maximum) maximum = size;
        }
        c->info[id].value_size = 8 + maximum;
        c->info[id].record_state = 2;
        c->depth--;
        return;
    }
    size_t size = 0;
    for (size_t f = c->nodes[id].a; f && c->error.kind != PS_LANG_ERROR; f = c->nodes[f].next) {
        size_t part = 8; /* Conservative size including padding for current scalar types. */
        if (c->info[f].type == PS_TYPE_VEC2 ||
            (c->info[f].type >= PS_TYPE_CHANNEL && c->info[f].type <= PS_TYPE_TABLE))
            part = 16;
        if (c->info[f].type == PS_TYPE_VEC3)
            part = 24;
        if (c->info[f].type == PS_TYPE_UNIT || c->info[f].type == PS_TYPE_QUAT ||
            c->info[f].type == PS_TYPE_VEC4)
            part = 32;
        if (c->info[f].type >= PS_TYPE_OPTIONAL_BASE)
            part = 64; /* Conservative owning handle layout, independent of elements. */
        if (c->info[f].type == PS_TYPE_QUANTITY)
            part = 40;
        if (c->info[f].type == PS_TYPE_DISTRIBUTION)
            part = 24;
        if (c->info[f].type == PS_TYPE_SENSOR_CONFIG)
            part = 128;
        if (c->info[f].type == PS_TYPE_SENSOR)
            part = 192;
        if (c->info[f].type == PS_TYPE_MEASUREMENT)
            part = 96;
        if (c->info[f].type == PS_TYPE_STEP_INTERVAL)
            part = 16;
        if (c->info[f].type == PS_TYPE_RNG)
            part = 16;
        if (c->info[f].type == PS_TYPE_ODE_RESULT)
            part = 128;
        if (c->info[f].type == PS_TYPE_CONTACT_WORLD) part=64;
        if (c->info[f].type == PS_TYPE_COLLIDER) part=64;
        if (c->info[f].type == PS_TYPE_RUN_INDEX) part=8;
        if (c->info[f].type == PS_TYPE_RUN_BLOCK) part=64;
        if (c->info[f].type == PS_TYPE_RUN_SNAPSHOT) part=8192;
        if (c->info[f].type == PS_TYPE_SCALAR_RESULT)
            part = 48;
        if (c->info[f].type == PS_TYPE_BODY)
            part = 136;
        if (c->info[f].type == PS_TYPE_CONTACTS)
            part = 456;
        if (c->info[f].type == PS_TYPE_CONTACT_SOLVER)
            part = 48;
        if (c->info[f].type == PS_TYPE_CONTACT_RESULT)
            part = 480;
        if (c->info[f].type == PS_TYPE_DISTANCE_JOINT)
            part = 64;
        if (c->info[f].type == PS_TYPE_JOINT_RESULT)
            part = 312;
        if (c->info[f].type == PS_TYPE_CONTACT_CONSTRAINT)
            part = 72;
        if (c->info[f].type == PS_TYPE_JOINT_CONSTRAINT)
            part = 80;
        if (c->info[f].type == PS_TYPE_CONSTRAINT_RESULT)
            part = 120;
        if (c->info[f].type == PS_TYPE_SWEEP) part = 72;
        if (c->info[f].type == PS_TYPE_AABB) part = 48;
        if (c->info[f].type == PS_TYPE_MAT3) part = 72;
        if (c->info[f].type == PS_TYPE_MAT4) part = 128;
        if (c->info[f].type == PS_TYPE_BEZIER3) part = 96;
        if (c->info[f].type == PS_TYPE_COLLISION_PAIR) part = 8;
        if (ps_lang_record_type(c->info[f].type)) {
            size_t child = (size_t)(c->info[f].type - PS_TYPE_RECORD_BASE);
            check_record(c, child);
            part = c->info[child].value_size;
        }
        if (part > 1024u * 1024u - size) {
            fail(c, f, "Struct value exceeds 1 MiB layout budget");
            break;
        }
        size += part;
    }
    c->info[id].value_size = size ? size : 1;
    c->info[id].record_state = 2;
    c->depth--;
}
static void check_raw_enum(checker *c, size_t id) {
    for (size_t item = c->nodes[id].a; item; item = c->nodes[item].next)
        if (c->nodes[item].b) c->info[id].raw_enum = 1;
    if (!c->info[id].raw_enum) return;
    int64_t next = 0;
    int next_valid = 1;
    for (size_t item = c->nodes[id].a; item && c->error.kind != PS_LANG_ERROR;
         item = c->nodes[item].next) {
        if (c->nodes[item].a) {
            fail(c, item, "Enum raw values cannot be combined with payload fields");
            break;
        }
        if (word(c, c->nodes[item].token, "fromRawValue") ||
            word(c, c->nodes[item].token, "rawValue")) {
            fail(c, item, "Enum raw value case name is reserved");
            break;
        }
        int64_t value = next;
        size_t raw = c->nodes[item].b;
        if (raw) {
            int signed_literal = c->nodes[raw].kind == PS_AST_UNARY &&
                                 (c->nodes[raw].token.kind == PS_LANG_MINUS ||
                                  c->nodes[raw].token.kind == PS_LANG_PLUS);
            int negative = signed_literal && c->nodes[raw].token.kind == PS_LANG_MINUS;
            size_t literal = signed_literal ? c->nodes[raw].a : raw;
            if (c->nodes[literal].kind != PS_AST_LITERAL ||
                c->nodes[literal].token.kind != PS_LANG_INTEGER) {
                fail(c, raw, "Enum raw value must be an Int64 literal");
                break;
            }
            integer(c, literal, negative);
            if (c->error.kind == PS_LANG_ERROR) break;
            uint64_t magnitude = ps_lang_integer_value(c->source, c->nodes[literal].token);
            value = negative ? magnitude == (uint64_t)INT64_MAX + 1u
                                   ? INT64_MIN : -(int64_t)magnitude
                             : (int64_t)magnitude;
        } else if (!next_valid) {
            fail(c, item, "Implicit enum raw value exceeds Int64 range");
            break;
        }
        for (size_t prior = c->nodes[id].a; prior != item; prior = c->nodes[prior].next)
            if (c->info[prior].raw_value == value)
                fail(c, item, "Duplicate enum raw value");
        c->info[item].raw_value = value;
        next_valid = value != INT64_MAX;
        if (next_valid) next = value + 1;
    }
}
static void function_signature(checker *c, size_t id, size_t owner) {
    c->info[id].type = annotation(c, c->nodes[id].b, 1);
    c->info[id].method_owner = owner;
    c->info[id].method_static = owner && c->nodes[id].token.kind == PS_LANG_STATIC;
    c->info[id].method_mutating = owner && !c->info[id].method_static &&
                                  c->nodes[c->nodes[id].a].token.kind == PS_LANG_VAR;
    if (!owner && !c->specializing && c->nodes[id].kind != PS_AST_LAMBDA)
        declare(c, id);
    size_t first = c->nodes[id].a;
    for (size_t p = first; p && c->error.kind != PS_LANG_ERROR; p = c->nodes[p].next) {
        if (c->nodes[p].kind == PS_AST_SELF_PARAMETER) {
            c->info[p].type = c->info[owner].type;
            continue;
        }
        if (word(c, c->nodes[p].token, "self"))
            fail(c, p, "self is reserved for the method receiver");
        c->info[p].type = annotation(c, c->nodes[p].a, 0);
        for (size_t prior = first; prior != p; prior = c->nodes[prior].next)
            if (c->nodes[prior].kind != PS_AST_SELF_PARAMETER &&
                same(c, c->nodes[prior].token, c->nodes[p].token))
                fail(c, p, "Duplicate parameter name");
    }
    if (c->error.kind != PS_LANG_ERROR)
        c->info[id].function_type = function_type(
            c, id, owner && !c->info[id].method_static && first
                       ? c->nodes[first].next : first,
            c->info[id].type);
    if (owner && word(c, c->nodes[id].token, "init")) {
        int enumeration = c->nodes[owner].kind == PS_AST_ENUM;
        if (enumeration)
            fail(c, id, "Enum methods cannot be named init");
        else if (!c->info[id].method_static)
            fail(c, id, "Struct initializer must be a static function");
        else if (c->info[id].type != c->info[owner].type)
            fail(c, id, "Struct initializer must return its struct type");
    }
}
static int type_parameter(checker *c, size_t generic, ps_lang_token name) {
    size_t signature = generic_record_kind(c->nodes[generic].kind)
                           ? c->nodes[generic].c : c->nodes[generic].b;
    int index = 0;
    for (size_t p = c->nodes[signature].a; p; p = c->nodes[p].next, index++)
        if (same(c, name, c->nodes[p].token))
            return index;
    return -1;
}
static int has_type_parameter(checker *c, size_t generic, size_t annotation_id) {
    const ps_lang_node *n = &c->nodes[annotation_id];
    if (n->kind == PS_AST_TYPE)
        return type_parameter(c, generic, n->token) >= 0;
    if (n->kind == PS_AST_ARRAY_TYPE || n->kind == PS_AST_OPTIONAL_TYPE)
        return has_type_parameter(c, generic, n->a);
    if (n->kind == PS_AST_FUNCTION_TYPE) {
        for (size_t p = n->a; p; p = c->nodes[p].next)
            if (has_type_parameter(c, generic, p))
                return 1;
        return has_type_parameter(c, generic, n->b);
    }
    if (n->kind == PS_AST_GENERIC_TYPE) {
        for (size_t a = n->b; a; a = c->nodes[a].next)
            if (has_type_parameter(c, generic, a))
                return 1;
    }
    return 0;
}
static int infer_annotation(checker *c, size_t generic, size_t annotation_id,
                            ps_lang_type actual, ps_lang_type *types, size_t at) {
    if (!annotation_id || !actual) {
        fail(c, at, "Cannot infer generic type from this argument");
        return 0;
    }
    const ps_lang_node *n = &c->nodes[annotation_id];
    if (n->kind == PS_AST_TYPE) {
        int parameter = type_parameter(c, generic, n->token);
        if (parameter >= 0) {
            if (types[parameter] && types[parameter] != actual)
                fail(c, at, "Conflicting types for generic parameter");
            else if (!value_type(actual))
                fail(c, at, "Generic type parameter requires a value type");
            else
                types[parameter] = actual;
            return c->error.kind != PS_LANG_ERROR;
        }
    }
    if (n->kind == PS_AST_ARRAY_TYPE) {
        ps_lang_type element = array_element(c, actual);
        if (!element)
            fail(c, at, "Generic argument requires an array");
        else
            return infer_annotation(c, generic, n->a, element, types, at);
        return 0;
    }
    if (n->kind == PS_AST_OPTIONAL_TYPE) {
        ps_lang_type element = optional_element(c, actual);
        if (!element)
            fail(c, at, "Generic argument requires an optional value");
        else
            return infer_annotation(c, generic, n->a, element, types, at);
        return 0;
    }
    if (n->kind == PS_AST_FUNCTION_TYPE) {
        if (!ps_lang_function_type(actual)) {
            fail(c, at, "Generic argument requires a function value");
            return 0;
        }
        size_t signature = (size_t)(actual - PS_TYPE_FUNCTION_BASE);
        size_t parameter = c->info[signature].function_parameter;
        for (size_t p = n->a; p && c->error.kind != PS_LANG_ERROR;
             p = c->nodes[p].next) {
            if (!parameter) {
                fail(c, at, "Generic function signature has too many parameters");
                return 0;
            }
            (void)infer_annotation(c, generic, p, c->info[parameter].type, types, at);
            parameter = c->nodes[parameter].next;
        }
        if (parameter && c->error.kind != PS_LANG_ERROR)
            fail(c, at, "Generic function signature has too few parameters");
        if (c->error.kind != PS_LANG_ERROR)
            (void)infer_annotation(c, generic, n->b,
                                   c->info[signature].function_result, types, at);
        return c->error.kind != PS_LANG_ERROR;
    }
    if (n->kind == PS_AST_GENERIC_TYPE) {
        size_t original = generic_record_name(c, n->a);
        size_t concrete = ps_lang_record_type(actual)
                              ? (size_t)(actual - PS_TYPE_RECORD_BASE) : 0;
        specialization *entry = NULL;
        for (size_t i = 0; i < c->specialization_count; i++)
            if (c->specializations[i].original == original &&
                c->specializations[i].concrete == concrete) {
                entry = &c->specializations[i];
                break;
            }
        if (!original || !entry) {
            fail(c, at, "Generic argument requires a matching struct specialization");
            return 0;
        }
        size_t index = 0;
        for (size_t a = n->b; a && c->error.kind != PS_LANG_ERROR;
             a = c->nodes[a].next) {
            if (index >= entry->count) {
                fail(c, at, "Generic struct type argument count does not match declaration");
                return 0;
            }
            (void)infer_annotation(c, generic, a, entry->types[index++], types, at);
        }
        return c->error.kind != PS_LANG_ERROR;
    }
    ps_lang_type required = annotation(c, annotation_id, actual == PS_TYPE_VOID);
    if (required && required != actual)
        fail(c, at, "Generic argument type does not match parameter");
    return c->error.kind != PS_LANG_ERROR;
}
static int initializer_template_argument(checker *c, size_t generic, size_t inner,
                                         size_t annotation_id,
                                         ps_lang_type actual, ps_lang_type *types,
                                         size_t value_id, int *score, unsigned depth) {
    if (c->error.kind == PS_LANG_ERROR)
        return 0;
    if (!annotation_id || depth >= 128 || !c->work) {
        fail(c, value_id, depth >= 128 ? "Semantic nesting limit exceeded (128)"
                                       : "Semantic work budget exceeded");
        return 0;
    }
    c->work--;
    const ps_lang_node *n = &c->nodes[annotation_id];
    if (!actual) {
        const ps_lang_node *value = &c->nodes[value_id];
        size_t some_value = initializer_optional_some_value(c, value_id);
        if (some_value) {
            if (n->kind == PS_AST_OPTIONAL_TYPE) {
                (*score)++;
                ps_lang_type element = initializer_argument_type(c, some_value, depth + 1);
                return initializer_template_argument(c, generic, inner, n->a, element,
                                                     types, some_value, score, depth + 1);
            }
            if (n->kind == PS_AST_TYPE) {
                int index = type_parameter(c, generic, n->token);
                if (index >= 0)
                    return !types[index] || initializer_value_matches(c, value_id,
                                                                       types[index], score, depth);
                if (inner && type_parameter(c, inner, n->token) >= 0)
                    return 1;
            }
            if (n->kind == PS_AST_ARRAY_TYPE || n->kind == PS_AST_GENERIC_TYPE)
                return 0;
            ps_lang_type required = annotation(c, annotation_id, 0);
            return required && initializer_value_matches(c, value_id, required, score, depth);
        }
        if (value->kind == PS_AST_LITERAL && value->token.kind == PS_LANG_NIL) {
            if (n->kind == PS_AST_OPTIONAL_TYPE) {
                (*score)++;
                return 1;
            }
            if (n->kind == PS_AST_TYPE) {
                int index = type_parameter(c, generic, n->token);
                if (index >= 0)
                    return !types[index] || initializer_value_matches(c, value_id,
                                                                       types[index], score, depth);
                if (inner && type_parameter(c, inner, n->token) >= 0)
                    return 1;
            }
            if (n->kind == PS_AST_ARRAY_TYPE || n->kind == PS_AST_GENERIC_TYPE)
                return 0;
            ps_lang_type required = annotation(c, annotation_id, 0);
            return required && initializer_value_matches(c, value_id, required, score, depth);
        }
        if (value->kind == PS_AST_ARRAY) {
            if (n->kind == PS_AST_ARRAY_TYPE) {
                if (c->nodes[n->a].kind == PS_AST_TYPE) {
                    int index = type_parameter(c, generic, c->nodes[n->a].token);
                    if (index >= 0 && !types[index])
                        for (size_t item = value->a;
                             item && c->error.kind != PS_LANG_ERROR;
                             item = c->nodes[item].next)
                            if (numeric_hint(c, item, depth + 1) == PS_TYPE_FLOAT64) {
                                types[index] = PS_TYPE_FLOAT64;
                                break;
                            }
                }
                (*score)++;
                for (size_t item = value->a; item && c->error.kind != PS_LANG_ERROR;
                     item = c->nodes[item].next) {
                    ps_lang_type element = initializer_argument_type(c, item, depth + 1);
                    if (!initializer_template_argument(c, generic, inner, n->a, element,
                                                       types, item, score, depth + 1))
                        return 0;
                }
                return c->error.kind != PS_LANG_ERROR;
            }
            if (n->kind == PS_AST_TYPE) {
                int index = type_parameter(c, generic, n->token);
                if (index >= 0)
                    return !types[index] || initializer_value_matches(c, value_id,
                                                                       types[index], score, depth);
                if (inner && type_parameter(c, inner, n->token) >= 0)
                    return 1;
            }
            if (n->kind == PS_AST_OPTIONAL_TYPE || n->kind == PS_AST_GENERIC_TYPE)
                return 0;
            ps_lang_type required = annotation(c, annotation_id, 0);
            return required && initializer_value_matches(c, value_id, required, score, depth);
        }
        return 1;
    }
    if (n->kind == PS_AST_TYPE) {
        int index = type_parameter(c, generic, n->token);
        if (index >= 0) {
            if (types[index] && types[index] != actual) {
                if (types[index] == PS_TYPE_FLOAT64 && actual == PS_TYPE_INT64 &&
                    initializer_integer_literal(c, value_id, 0)) {
                    (*score)++;
                    return 1;
                }
                return 0;
            }
            int already_bound = types[index] != PS_TYPE_NONE;
            if (!already_bound)
                types[index] = actual;
            *score += already_bound ? 4 : 2;
            return 1;
        }
        if (inner && type_parameter(c, inner, n->token) >= 0)
            return 1;
    }
    if (n->kind == PS_AST_ARRAY_TYPE || n->kind == PS_AST_OPTIONAL_TYPE) {
        ps_lang_type element = n->kind == PS_AST_ARRAY_TYPE ? array_element(c, actual)
                                                              : optional_element(c, actual);
        if (!element || !initializer_template_argument(c, generic, inner, n->a, element,
                                                       types, value_id, score, depth + 1))
            return 0;
        (*score)++;
        return 1;
    }
    if (n->kind == PS_AST_FUNCTION_TYPE) {
        if (!ps_lang_function_type(actual))
            return 0;
        size_t signature = (size_t)(actual - PS_TYPE_FUNCTION_BASE);
        size_t parameter = c->info[signature].function_parameter;
        for (size_t p = n->a; p; p = c->nodes[p].next) {
            if (!parameter || !initializer_template_argument(
                    c, generic, inner, p, c->info[parameter].type,
                    types, value_id, score, depth + 1))
                return 0;
            parameter = c->nodes[parameter].next;
        }
        if (parameter || !initializer_template_argument(
                c, generic, inner, n->b, c->info[signature].function_result,
                types, value_id, score, depth + 1))
            return 0;
        (*score)++;
        return 1;
    }
    if (n->kind == PS_AST_GENERIC_TYPE && has_type_parameter(c, generic, annotation_id)) {
        size_t original = generic_record_name(c, n->a);
        size_t concrete = ps_lang_record_type(actual)
                              ? (size_t)(actual - PS_TYPE_RECORD_BASE) : 0;
        specialization *entry = NULL;
        for (size_t i = 0; i < c->specialization_count; i++)
            if (c->specializations[i].original == original &&
                c->specializations[i].concrete == concrete) {
                entry = &c->specializations[i];
                break;
            }
        if (!entry)
            return 0;
        size_t index = 0;
        for (size_t a = n->b; a && c->error.kind != PS_LANG_ERROR;
             a = c->nodes[a].next, index++) {
            if (index >= entry->count ||
                !initializer_template_argument(c, generic, inner, a, entry->types[index],
                                               types, value_id, score, depth + 1))
                return 0;
        }
        if (index != entry->count)
            return 0;
        (*score)++;
        return 1;
    }
    ps_lang_type required = annotation(c, annotation_id, actual == PS_TYPE_VOID);
    if (required == actual) {
        *score += 4;
        return 1;
    }
    if (required == PS_TYPE_FLOAT64 && actual == PS_TYPE_INT64 &&
        initializer_integer_literal(c, value_id, 0)) {
        (*score)++;
        return 1;
    }
    return 0;
}
static size_t generic_initializer_for_value(checker *c, size_t record, size_t value_id,
                                            ps_lang_type expected) {
    size_t type_arguments = c->nodes[value_id].b, type_count = 0;
    ps_lang_type explicit_types[GENERIC_TYPE_LIMIT] = {0};
    for (size_t t = type_arguments; t; t = c->nodes[t].next) {
        if (type_count == GENERIC_TYPE_LIMIT) {
            fail(c, value_id, "Too many explicit type arguments");
            return 0;
        }
        explicit_types[type_count++] = annotation(c, t, 0);
    }
    if (c->error.kind == PS_LANG_ERROR)
        return 0;
    size_t signature = ps_lang_function_type(expected)
                           ? (size_t)(expected - PS_TYPE_FUNCTION_BASE) : 0;
    size_t selected = 0;
    for (size_t method = c->nodes[record].b; method; method = c->nodes[method].next) {
        if (!word(c, c->nodes[method].token, "init") ||
            c->nodes[method].kind != PS_AST_GENERIC_FUNCTION)
            continue;
        size_t generic_parameter = c->nodes[c->nodes[method].b].a;
        size_t argument_count = 0;
        for (size_t p = generic_parameter; p; p = c->nodes[p].next)
            argument_count++;
        if (argument_count != type_count)
            continue;
        if (signature) {
            if (c->info[signature].function_result != c->info[record].type)
                continue;
            ps_lang_type types[GENERIC_TYPE_LIMIT] = {0};
            memcpy(types, explicit_types, type_count * sizeof types[0]);
            size_t parameter = c->nodes[method].a;
            size_t wanted = c->info[signature].function_parameter;
            int compatible = 1, score = 0;
            while (parameter && wanted && compatible) {
                compatible = initializer_template_argument(
                    c, method, 0, c->nodes[parameter].a, c->info[wanted].type,
                    types, value_id, &score, 0);
                parameter = c->nodes[parameter].next;
                wanted = c->nodes[wanted].next;
            }
            if (c->error.kind == PS_LANG_ERROR)
                return 0;
            if (!compatible || parameter || wanted)
                continue;
        }
        if (selected) {
            fail(c, value_id, "Ambiguous generic initializer value");
            return 0;
        }
        selected = method;
    }
    if (!selected)
        fail(c, value_id, "No generic initializer matches the type arguments and function type");
    return selected;
}
/* A later Float64 value gives an earlier integer literal its numeric context.
 * Seed that context before checking arguments in source order. */
static void generic_float_context(checker *c, size_t generic, size_t annotation_id,
                                  size_t value_id, ps_lang_type *types, unsigned depth) {
    if (!annotation_id || !value_id || c->error.kind == PS_LANG_ERROR)
        return;
    if (depth >= 128 || !c->work) {
        fail(c, value_id, depth >= 128 ? "Semantic nesting limit exceeded (128)"
                                        : "Semantic work budget exceeded");
        return;
    }
    c->work--;
    const ps_lang_node *annotation_node = &c->nodes[annotation_id];
    const ps_lang_node *value = &c->nodes[value_id];
    if (annotation_node->kind == PS_AST_TYPE) {
        int index = type_parameter(c, generic, annotation_node->token);
        if (index >= 0 && !types[index] &&
            initializer_argument_type(c, value_id, depth + 1) == PS_TYPE_FLOAT64)
            types[index] = PS_TYPE_FLOAT64;
    } else if (annotation_node->kind == PS_AST_OPTIONAL_TYPE) {
        size_t some_value = initializer_optional_some_value(c, value_id);
        if (some_value)
            generic_float_context(c, generic, annotation_node->a, some_value,
                                  types, depth + 1);
    } else if (annotation_node->kind == PS_AST_ARRAY_TYPE && value->kind == PS_AST_ARRAY) {
        for (size_t item = value->a; item && c->error.kind != PS_LANG_ERROR;
             item = c->nodes[item].next)
            generic_float_context(c, generic, annotation_node->a, item,
                                  types, depth + 1);
    }
}
static void generic_seed_float_types(checker *c, size_t generic, size_t method,
                                     size_t call_id, ps_lang_type *types) {
    size_t positional = c->nodes[method].a;
    if (positional && c->nodes[positional].kind == PS_AST_SELF_PARAMETER)
        positional = c->nodes[positional].next;
    size_t first_parameter = positional;
    int named = c->nodes[call_id].b && c->nodes[c->nodes[call_id].b].token.length;
    for (size_t a = c->nodes[call_id].b; a && c->error.kind != PS_LANG_ERROR;
         a = c->nodes[a].next) {
        size_t p = positional;
        if (named) {
            for (p = first_parameter; p; p = c->nodes[p].next)
                if (same(c, c->nodes[a].token, c->nodes[p].token))
                    break;
        }
        if (p)
            generic_float_context(c, generic, c->nodes[p].a, c->nodes[a].a,
                                  types, 0);
        if (!named && p)
            positional = c->nodes[p].next;
    }
}
static size_t initializer_template_for_call(checker *c, size_t generic, size_t call_id,
                                            ps_lang_type *known_types,
                                            size_t type_count) {
    size_t selected = 0, only_shape = 0, shapes = 0;
    size_t only_method = 0, methods = 0;
    size_t deferred_method = 0, deferred_methods = 0;
    ps_lang_type selected_types[GENERIC_TYPE_LIMIT] = {0};
    ps_lang_type deferred_types[GENERIC_TYPE_LIMIT] = {0};
    int best_score = -1, tied = 0;
    int named = c->nodes[call_id].b && c->nodes[c->nodes[call_id].b].token.length;
    for (size_t method = c->nodes[generic].b; method && c->error.kind != PS_LANG_ERROR;
         method = c->nodes[method].next) {
        if (!word(c, c->nodes[method].token, "init"))
            continue;
        methods++;
        only_method = method;
        if (!initializer_shape_matches(c, method, call_id))
            continue;
        shapes++;
        only_shape = method;
        ps_lang_type types[GENERIC_TYPE_LIMIT] = {0};
        for (size_t i = 0; i < type_count; i++)
            types[i] = known_types[i];
        generic_seed_float_types(c, generic, method, call_id, types);
        int score = 0, compatible = 1;
        size_t positional = c->nodes[method].a;
        for (size_t a = c->nodes[call_id].b; a && compatible; a = c->nodes[a].next) {
            size_t p = positional;
            if (named) {
                for (p = c->nodes[method].a; p; p = c->nodes[p].next)
                    if (same(c, c->nodes[a].token, c->nodes[p].token))
                        break;
            }
            ps_lang_type actual = initializer_argument_type(c, c->nodes[a].a, 0);
            compatible = initializer_template_argument(c, generic,
                                                       c->nodes[method].kind == PS_AST_GENERIC_FUNCTION
                                                           ? method : 0,
                                                       c->nodes[p].a,
                                                       actual, types, c->nodes[a].a,
                                                       &score, 0);
            if (!named)
                positional = c->nodes[p].next;
        }
        /* A later argument may bind T for an earlier nil or array literal. */
        positional = c->nodes[method].a;
        for (size_t a = c->nodes[call_id].b; a && compatible; a = c->nodes[a].next) {
            size_t p = positional;
            if (named) {
                for (p = c->nodes[method].a; p; p = c->nodes[p].next)
                    if (same(c, c->nodes[a].token, c->nodes[p].token))
                        break;
            }
            int ignored_score = 0;
            ps_lang_type actual = initializer_argument_type(c, c->nodes[a].a, 0);
            compatible = initializer_template_argument(c, generic,
                                                       c->nodes[method].kind == PS_AST_GENERIC_FUNCTION
                                                           ? method : 0,
                                                       c->nodes[p].a,
                                                       actual, types, c->nodes[a].a,
                                                       &ignored_score, 0);
            if (!named)
                positional = c->nodes[p].next;
        }
        if (!compatible)
            continue;
        int inferred = 1;
        for (size_t i = 0; i < type_count; i++)
            inferred &= types[i] != PS_TYPE_NONE;
        if (!inferred) {
            deferred_method = method;
            deferred_methods++;
            if (deferred_methods == 1)
                memcpy(deferred_types, types, type_count * sizeof types[0]);
            continue;
        }
        if (score > best_score) {
            best_score = score;
            selected = method;
            tied = 0;
            memcpy(selected_types, types, type_count * sizeof types[0]);
        } else if (score == best_score)
            tied = 1;
    }
    if (c->error.kind == PS_LANG_ERROR)
        return 0;
    if (tied)
        fail(c, call_id, "Ambiguous generic struct initializer overload; use typed arguments");
    else if (selected) {
        memcpy(known_types, selected_types, type_count * sizeof known_types[0]);
        return selected;
    } else if (deferred_methods == 1) {
        memcpy(known_types, deferred_types, type_count * sizeof known_types[0]);
        return deferred_method; /* Expression checking may still infer its type. */
    }
    else if (shapes == 1 || methods == 1)
        return shapes == 1 ? only_shape : only_method; /* Preserve detailed errors. */
    else if (deferred_methods)
        fail(c, call_id, "Cannot infer every generic struct type parameter");
    else
        fail(c, call_id, "No matching generic struct initializer overload");
    return 0;
}
static size_t infer_record_constructor(checker *c, size_t generic, size_t call_id,
                                       ps_lang_type expected, size_t *selected_initializer) {
    size_t signature = c->nodes[generic].c;
    size_t type_count = 0;
    for (size_t p = c->nodes[signature].a; p; p = c->nodes[p].next)
        type_count++;
    if (!type_count || type_count > GENERIC_TYPE_LIMIT) {
        fail(c, generic, "Generic struct requires 1 to 8 type parameters");
        return 0;
    }
    ps_lang_type types[GENERIC_TYPE_LIMIT] = {0};
    if (ps_lang_record_type(expected)) {
        size_t wanted = (size_t)(expected - PS_TYPE_RECORD_BASE);
        for (size_t i = 0; i < c->specialization_count; i++)
            if (c->specializations[i].original == generic &&
                c->specializations[i].concrete == wanted)
                for (size_t j = 0; j < type_count; j++)
                    types[j] = c->specializations[i].types[j];
    }
    size_t custom = initializer(c, generic);
    size_t concrete = 0;
    if (custom) {
        custom = initializer_template_for_call(c, generic, call_id, types, type_count);
        if (!custom)
            return 0;
        generic_seed_float_types(c, generic, custom, call_id, types);
        int inferred = 1;
        for (size_t i = 0; i < type_count; i++)
            inferred &= types[i] != PS_TYPE_NONE;
        if (inferred) {
            concrete = specialize_record(c, generic, 0, types, call_id);
            if (!concrete)
                return 0;
        }
        if (c->nodes[custom].kind == PS_AST_GENERIC_FUNCTION && !concrete) {
            fail(c, call_id, "Cannot infer every generic struct type parameter");
            return 0;
        }
    }
    size_t first_field = custom ? c->nodes[custom].a : c->nodes[generic].a;
    size_t field = first_field;
    size_t first_argument = c->nodes[call_id].b;
    int named = first_argument && c->nodes[first_argument].token.length != 0;
    for (size_t a = first_argument; a && c->error.kind != PS_LANG_ERROR;
         a = c->nodes[a].next) {
        const ps_lang_node *arg = &c->nodes[a];
        if ((arg->token.length != 0) != named) {
            fail(c, a, "Use either positional or named arguments, not both");
            break;
        }
        size_t f = field;
        if (named) {
            for (f = first_field; f; f = c->nodes[f].next)
                if (same(c, arg->token, c->nodes[f].token))
                    break;
        }
        if (!f) {
            fail(c, a, "Unknown field name or too many constructor arguments");
            break;
        }
        for (size_t prior = first_argument; prior != a;
             prior = c->nodes[prior].next)
            if (c->info[prior].binding == f)
                fail(c, a, "Field supplied more than once");
        c->info[a].binding = f;
        if (custom && c->nodes[custom].kind == PS_AST_GENERIC_FUNCTION) {
            /* The specialized method checks its own type parameters and arguments. */
            if (!named)
                field = c->nodes[f].next;
            continue;
        }
        size_t field_annotation = c->nodes[f].a;
        ps_lang_type wanted = concrete ? c->info[f + concrete - generic].type : PS_TYPE_NONE;
        if (!concrete && c->nodes[field_annotation].kind == PS_AST_TYPE) {
            int index = type_parameter(c, generic, c->nodes[field_annotation].token);
            wanted = index >= 0 ? types[index] : annotation(c, field_annotation, 0);
        } else if (!concrete && !has_type_parameter(c, generic, field_annotation))
            wanted = annotation(c, field_annotation, 0);
        const ps_lang_node *value = &c->nodes[arg->a];
        int deferred = !wanted &&
            ((value->kind == PS_AST_ARRAY && !value->a) ||
             (value->kind == PS_AST_LITERAL && value->token.kind == PS_LANG_NIL));
        if (!deferred) {
            ps_lang_type actual = expression(c, arg->a, wanted);
            c->info[a].type = actual;
            (void)infer_annotation(c, generic, field_annotation, actual, types, a);
        }
        if (!named)
            field = c->nodes[f].next;
    }
    for (size_t f = first_field; f && c->error.kind != PS_LANG_ERROR;
         f = c->nodes[f].next) {
        int supplied = 0;
        for (size_t a = first_argument; a; a = c->nodes[a].next)
            supplied |= c->info[a].binding == f;
        if (!supplied && (custom || !c->nodes[f].b))
            fail(c, call_id, custom ? "Missing generic struct initializer arguments"
                                    : "Missing or excess generic struct fields");
    }
    for (size_t i = 0; i < type_count && c->error.kind != PS_LANG_ERROR; i++)
        if (!types[i])
            fail(c, call_id, "Cannot infer every generic struct type parameter");
    if (c->error.kind == PS_LANG_ERROR)
        return 0;
    if (!concrete)
        concrete = specialize_record(c, generic, 0, types, call_id);
    if (concrete && selected_initializer && custom)
        *selected_initializer = custom + concrete - generic;
    return concrete;
}
static size_t specialize(checker *c, size_t generic, size_t call_id,
                         ps_lang_type expected, int record_type_arguments) {
    size_t signature = c->nodes[generic].b;
    size_t type_count = 0;
    for (size_t p = c->nodes[signature].a; p; p = c->nodes[p].next)
        type_count++;
    if (!type_count || type_count > GENERIC_TYPE_LIMIT) {
        fail(c, generic, "Generic function requires 1 to 8 type parameters");
        return 0;
    }
    ps_lang_type types[GENERIC_TYPE_LIMIT] = {0};
    int function_reference = c->nodes[call_id].kind == PS_AST_TYPE_APPLY;
    size_t explicit_type = record_type_arguments ? 0 : function_reference
        ? c->nodes[call_id].b : c->nodes[call_id].c;
    if (explicit_type) {
        size_t parameter_type = c->nodes[signature].a;
        size_t index = 0;
        for (size_t t = explicit_type; t && c->error.kind != PS_LANG_ERROR;
             t = c->nodes[t].next) {
            if (!parameter_type) {
                fail(c, t, "Too many explicit type arguments");
                break;
            }
            ps_lang_type concrete = annotation(c, t, 0);
            if (concrete && !value_type(concrete))
                fail(c, t, "Generic type argument requires a value type");
            types[index++] = concrete;
            parameter_type = c->nodes[parameter_type].next;
        }
        if (parameter_type && c->error.kind != PS_LANG_ERROR)
            fail(c, call_id, "Missing explicit type arguments");
    }
    if (expected && c->nodes[signature].b)
        (void)infer_annotation(c, generic, c->nodes[signature].b, expected, types, call_id);
    if (c->error.kind == PS_LANG_ERROR)
        return 0;
    size_t parameter = c->nodes[generic].a;
    if (c->info[generic].method_owner && !c->info[generic].method_static)
        parameter = c->nodes[parameter].next;
    size_t first_parameter = parameter;
    size_t first_argument = function_reference ? 0 : c->nodes[call_id].b;
    int named = first_argument && c->nodes[first_argument].token.length != 0;
    size_t arguments = 0, parameters = 0;
    for (size_t p = parameter; p; p = c->nodes[p].next)
        parameters++;
    int inferred_before_check = 0;
    if (!explicit_type) {
        ps_lang_type preview[GENERIC_TYPE_LIMIT] = {0};
        memcpy(preview, types, type_count * sizeof types[0]);
        generic_seed_float_types(c, generic, generic, call_id, preview);
        size_t positional = first_parameter, preview_arguments = 0;
        int compatible = c->error.kind != PS_LANG_ERROR;
        for (size_t a = first_argument; a && compatible; a = c->nodes[a].next) {
            if ((c->nodes[a].token.length != 0) != named) {
                compatible = 0;
                break;
            }
            size_t p = positional;
            if (named)
                for (p = first_parameter; p; p = c->nodes[p].next)
                    if (same(c, c->nodes[a].token, c->nodes[p].token))
                        break;
            if (!p) {
                compatible = 0;
                break;
            }
            int score = 0;
            ps_lang_type actual = initializer_argument_type(c, c->nodes[a].a, 0);
            compatible = initializer_template_argument(c, generic, 0, c->nodes[p].a,
                                                       actual, preview, c->nodes[a].a,
                                                       &score, 0);
            preview_arguments++;
            if (!named)
                positional = c->nodes[p].next;
        }
        if (compatible && preview_arguments == parameters &&
            c->error.kind != PS_LANG_ERROR) {
            memcpy(types, preview, type_count * sizeof types[0]);
            inferred_before_check = 1;
            for (size_t i = 0; i < type_count; i++)
                inferred_before_check &= types[i] != PS_TYPE_NONE;
        }
    }
    if (c->error.kind == PS_LANG_ERROR)
        return 0;
    for (size_t a = explicit_type || inferred_before_check ? 0 : first_argument;
         a && c->error.kind != PS_LANG_ERROR;
         a = c->nodes[a].next) {
        const ps_lang_node *arg = &c->nodes[a];
        if ((arg->token.length != 0) != named) {
            fail(c, a, "Use either positional or named arguments, not both");
            break;
        }
        size_t p = parameter;
        if (named) {
            for (p = first_parameter; p; p = c->nodes[p].next)
                if (same(c, arg->token, c->nodes[p].token))
                    break;
        }
        if (!p) {
            fail(c, a, "Unknown parameter name or too many arguments");
            break;
        }
        for (size_t prior = first_argument; prior != a; prior = c->nodes[prior].next)
            if (c->info[prior].binding == p)
                fail(c, a, "Parameter supplied more than once");
        c->info[a].binding = p;
        ps_lang_type wanted = PS_TYPE_NONE;
        size_t annotation_id = c->nodes[p].a;
        if (c->nodes[annotation_id].kind == PS_AST_TYPE) {
            int index = type_parameter(c, generic, c->nodes[annotation_id].token);
            if (index >= 0)
                wanted = types[index];
            else
                wanted = annotation(c, annotation_id, 0);
        } else if (!has_type_parameter(c, generic, annotation_id))
            wanted = annotation(c, annotation_id, 0);
        const ps_lang_node *value = &c->nodes[arg->a];
        int deferred = !wanted &&
            ((value->kind == PS_AST_ARRAY && !value->a) ||
             (value->kind == PS_AST_LITERAL && value->token.kind == PS_LANG_NIL));
        if (!deferred) {
            ps_lang_type actual = expression(c, arg->a, wanted);
            c->info[a].type = actual;
            (void)infer_annotation(c, generic, annotation_id, actual, types, a);
        }
        arguments++;
        if (!named)
            parameter = c->nodes[p].next;
    }
    if (!explicit_type && !inferred_before_check && arguments != parameters &&
        c->error.kind != PS_LANG_ERROR)
        fail(c, call_id, "Missing or excess generic function arguments");
    for (size_t i = 0; i < type_count && c->error.kind != PS_LANG_ERROR; i++)
        if (!types[i])
            fail(c, call_id, "Cannot infer every generic type parameter");
    if (c->error.kind == PS_LANG_ERROR)
        return 0;
    check_constraints(c, generic, types, call_id);
    if (c->error.kind == PS_LANG_ERROR)
        return 0;
    for (size_t i = 0; i < c->specialization_count; i++) {
        specialization *existing = &c->specializations[i];
        if (existing->original != generic || existing->count != type_count)
            continue;
        int equal = 1;
        for (size_t j = 0; j < type_count; j++)
            equal &= existing->types[j] == types[j];
        if (equal)
            return existing->concrete;
    }
    if (c->specialization_count == GENERIC_SPECIALIZATION_LIMIT) {
        fail(c, call_id, "Generic specialization limit exceeded (256)");
        return 0;
    }
    size_t start = c->nodes[signature].c;
    if (!start || start > generic || generic >= c->count) {
        fail(c, generic, "Invalid generic function template");
        return 0;
    }
    size_t span = generic - start + 1;
    if (span > c->node_capacity - c->count || span > c->info_capacity - c->count ||
        span > c->work) {
        fail(c, call_id, "Generic specialization exceeds compiler capacity");
        return 0;
    }
    c->work -= span;
    size_t shift = c->count - start;
    for (size_t old = start; old <= generic; old++) {
        size_t fresh = old + shift;
        ps_lang_node n = c->nodes[old];
        if (n.a >= start && n.a <= generic)
            n.a += shift;
        if (n.b >= start && n.b <= generic)
            n.b += shift;
        if (n.c >= start && n.c <= generic)
            n.c += shift;
        if (n.next >= start && n.next <= generic)
            n.next += shift;
        else if (old == generic)
            n.next = 0;
        c->nodes[fresh] = n;
        memset(&c->info[fresh], 0, sizeof c->info[fresh]);
        if (n.kind == PS_AST_RESOLVED_TYPE)
            c->info[fresh].type = c->info[old].type;
        if (n.kind == PS_AST_TYPE) {
            int index = type_parameter(c, generic, n.token);
            if (index >= 0) {
                c->nodes[fresh].kind = PS_AST_RESOLVED_TYPE;
                c->info[fresh].type = types[index];
            }
        }
    }
    size_t concrete = generic + shift;
    int local = c->info[generic].local_function != 0;
    c->nodes[concrete].kind = local ? PS_AST_LOCAL_FUNCTION : PS_AST_FUNCTION;
    c->nodes[concrete].b = c->nodes[signature + shift].b;
    c->count += span;
    specialization *entry = &c->specializations[c->specialization_count++];
    entry->original = generic;
    entry->concrete = concrete;
    entry->count = (unsigned)type_count;
    for (size_t i = 0; i < type_count; i++)
        entry->types[i] = types[i];
    c->specializing = 1;
    function_signature(c, concrete, c->info[generic].method_owner);
    c->specializing = 0;
    c->info[concrete].generic_origin = generic;
    if (local && c->error.kind != PS_LANG_ERROR) {
        size_t saved_head = c->head, saved_base = c->scope_base;
        size_t saved_function = c->function, saved_self = c->self_parameter;
        unsigned saved_loops = c->loops;
        int *saved_break = c->switch_break;
        c->info[concrete].local_function = c->info[generic].local_function;
        c->info[concrete].lambda_parent = c->info[generic].local_function;
        c->info[concrete].lambda_result = c->info[concrete].type;
        c->info[concrete].type = c->info[concrete].function_type;
        c->head = c->scope_base = generic;
        c->function = concrete;
        c->loops = 0;
        c->switch_break = NULL;
        for (size_t p = c->nodes[concrete].a; p && c->error.kind != PS_LANG_ERROR;
             p = c->nodes[p].next)
            declare(c, p);
        int returns = sequence(c, c->nodes[c->nodes[concrete].c].a);
        if (c->info[concrete].lambda_result != PS_TYPE_VOID && !returns)
            fail(c, concrete, "Not all generic local function paths return a value");
        c->head = saved_head;
        c->scope_base = saved_base;
        c->function = saved_function;
        c->self_parameter = saved_self;
        c->loops = saved_loops;
        c->switch_break = saved_break;
    }
    return c->error.kind == PS_LANG_ERROR ? 0 : concrete;
}
static void validate_scoped_annotation(checker *c, size_t outer, size_t inner,
                                       size_t id, int allow_void) {
    if (!id || c->error.kind == PS_LANG_ERROR)
        return;
    const ps_lang_node *n = &c->nodes[id];
    if (n->kind == PS_AST_TYPE &&
        ((outer && type_parameter(c, outer, n->token) >= 0) ||
         (inner && type_parameter(c, inner, n->token) >= 0)))
        return;
    if (n->kind == PS_AST_ARRAY_TYPE || n->kind == PS_AST_OPTIONAL_TYPE) {
        validate_scoped_annotation(c, outer, inner, n->a, 0);
        return;
    }
    if (n->kind == PS_AST_FUNCTION_TYPE) {
        for (size_t p = n->a; p && c->error.kind != PS_LANG_ERROR;
             p = c->nodes[p].next)
            validate_scoped_annotation(c, outer, inner, p, 0);
        validate_scoped_annotation(c, outer, inner, n->b, 1);
        return;
    }
    if (n->kind == PS_AST_GENERIC_TYPE) {
        size_t decl = generic_record_name(c, n->a);
        if (!decl) {
            fail(c, n->a, "Type arguments require a generic struct");
            return;
        }
        size_t parameters = 0, arguments = 0;
        for (size_t p = c->nodes[c->nodes[decl].c].a; p; p = c->nodes[p].next)
            parameters++;
        for (size_t a = n->b; a && c->error.kind != PS_LANG_ERROR;
             a = c->nodes[a].next) {
            validate_scoped_annotation(c, outer, inner, a, 0);
            arguments++;
        }
        if (arguments != parameters && c->error.kind != PS_LANG_ERROR)
            fail(c, id, "Generic struct type argument count does not match declaration");
        return;
    }
    (void)annotation(c, id, allow_void);
}
static void validate_generic_annotation(checker *c, size_t generic, size_t id,
                                        int allow_void) {
    validate_scoped_annotation(c, generic, 0, id, allow_void);
}
static void generic_signature(checker *c, size_t id, size_t owner) {
    size_t signature = c->nodes[id].b;
    size_t parameters = 0;
    for (size_t p = c->nodes[signature].a; p && c->error.kind != PS_LANG_ERROR;
         p = c->nodes[p].next) {
        parameters++;
        if (parameters > GENERIC_TYPE_LIMIT)
            fail(c, p, "Generic function supports at most 8 type parameters");
        if (builtin_type_name(c, c->nodes[p].token))
            fail(c, p, "Generic type parameter conflicts with a builtin type");
        if (word(c, c->nodes[p].token, "self"))
            fail(c, p, "self is reserved for the method receiver");
        for (size_t prior = c->nodes[signature].a; prior != p;
             prior = c->nodes[prior].next)
            if (same(c, c->nodes[prior].token, c->nodes[p].token))
                fail(c, p, "Duplicate generic type parameter");
        validate_constraints(c, p);
    }
    validate_generic_annotation(c, id, c->nodes[signature].b, 1);
    if (owner && word(c, c->nodes[id].token, "init")) {
        if (c->nodes[owner].kind == PS_AST_ENUM)
            fail(c, id, "Enum methods cannot be named init");
        if (c->nodes[id].token.kind != PS_LANG_STATIC)
            fail(c, id, "Struct initializer must be a static function");
        size_t result = c->nodes[signature].b;
        if (!result || has_type_parameter(c, id, result) ||
            annotation(c, result, 0) != c->info[owner].type)
            fail(c, id, "Struct initializer must return its struct type");
    }
    for (size_t p = c->nodes[id].a; p && c->error.kind != PS_LANG_ERROR;
         p = c->nodes[p].next) {
        if (c->nodes[p].kind != PS_AST_SELF_PARAMETER)
            validate_generic_annotation(c, id, c->nodes[p].a, 0);
        if (word(c, c->nodes[p].token, "self") &&
            c->nodes[p].kind != PS_AST_SELF_PARAMETER)
            fail(c, p, "self is reserved for the method receiver");
        for (size_t prior = c->nodes[id].a; prior != p;
             prior = c->nodes[prior].next)
            if (c->nodes[prior].kind != PS_AST_SELF_PARAMETER &&
                same(c, c->nodes[prior].token, c->nodes[p].token))
                fail(c, p, "Duplicate parameter name");
    }
    c->info[id].type = PS_TYPE_FUNCTION;
    c->info[id].method_owner = owner;
    c->info[id].method_static = owner && c->nodes[id].token.kind == PS_LANG_STATIC;
    c->info[id].method_mutating = owner && !c->info[id].method_static &&
                                   c->nodes[c->nodes[id].a].token.kind == PS_LANG_VAR;
    if (!owner)
        declare(c, id);
}
static size_t specialize_record(checker *c, size_t generic, size_t type_arguments,
                                const ps_lang_type *inferred, size_t at) {
    int enumeration = c->nodes[generic].kind == PS_AST_GENERIC_ENUM;
    size_t signature = c->nodes[generic].c;
    ps_lang_type types[GENERIC_TYPE_LIMIT] = {0};
    size_t type_count = 0;
    for (size_t p = c->nodes[signature].a; p; p = c->nodes[p].next)
        type_count++;
    if ((!type_arguments && !inferred) || !type_count || type_count > GENERIC_TYPE_LIMIT) {
        fail(c, at, enumeration ? "Generic enum requires 1 to 8 type arguments"
                                : "Generic struct requires 1 to 8 type arguments");
        return 0;
    }
    size_t index = 0;
    if (inferred) {
        for (index = 0; index < type_count; index++) {
            types[index] = inferred[index];
            if (!value_type(types[index]))
                fail(c, at, enumeration ? "Generic enum type argument requires a value type"
                                        : "Generic struct type argument requires a value type");
        }
    } else {
        for (size_t a = type_arguments; a && c->error.kind != PS_LANG_ERROR;
             a = c->nodes[a].next) {
            if (index == type_count) {
                fail(c, a, enumeration ? "Too many generic enum type arguments"
                                       : "Too many generic struct type arguments");
                return 0;
            }
            types[index] = annotation(c, a, 0);
            if (types[index] && !value_type(types[index]))
                fail(c, a, enumeration ? "Generic enum type argument requires a value type"
                                       : "Generic struct type argument requires a value type");
            index++;
        }
    }
    if (index != type_count && c->error.kind != PS_LANG_ERROR)
        fail(c, at, enumeration ? "Missing generic enum type arguments"
                                : "Missing generic struct type arguments");
    if (c->error.kind == PS_LANG_ERROR)
        return 0;
    check_constraints(c, generic, types, at);
    if (c->error.kind == PS_LANG_ERROR)
        return 0;
    for (size_t i = 0; i < c->specialization_count; i++) {
        specialization *existing = &c->specializations[i];
        if (existing->original != generic || existing->count != type_count)
            continue;
        int equal = 1;
        for (size_t j = 0; j < type_count; j++)
            equal &= existing->types[j] == types[j];
        if (equal)
            return existing->concrete;
    }
    if (c->specialization_count == GENERIC_SPECIALIZATION_LIMIT) {
        fail(c, at, "Generic specialization limit exceeded (256)");
        return 0;
    }
    size_t start = c->nodes[signature].c;
    if (!start || start > generic || generic >= c->count) {
        fail(c, generic, enumeration ? "Invalid generic enum template"
                                     : "Invalid generic struct template");
        return 0;
    }
    size_t span = generic - start + 1;
    if (span > c->node_capacity - c->count || span > c->info_capacity - c->count ||
        span > c->work) {
        fail(c, at, enumeration ? "Generic enum specialization exceeds compiler capacity"
                                : "Generic struct specialization exceeds compiler capacity");
        return 0;
    }
    c->work -= span;
    size_t shift = c->count - start;
    for (size_t old = start; old <= generic; old++) {
        size_t fresh = old + shift;
        ps_lang_node n = c->nodes[old];
        if (n.a >= start && n.a <= generic)
            n.a += shift;
        if (n.b >= start && n.b <= generic)
            n.b += shift;
        if (n.c >= start && n.c <= generic)
            n.c += shift;
        if (n.next >= start && n.next <= generic)
            n.next += shift;
        else if (old == generic)
            n.next = 0;
        c->nodes[fresh] = n;
        memset(&c->info[fresh], 0, sizeof c->info[fresh]);
        if (n.kind == PS_AST_TYPE) {
            int parameter = type_parameter(c, generic, n.token);
            if (parameter >= 0) {
                c->nodes[fresh].kind = PS_AST_RESOLVED_TYPE;
                c->info[fresh].type = types[parameter];
            }
        }
    }
    size_t concrete = generic + shift;
    c->nodes[concrete].kind = enumeration ? PS_AST_ENUM : PS_AST_STRUCT;
    c->nodes[concrete].c = 0;
    c->count += span;
    c->info[concrete].type = (ps_lang_type)(PS_TYPE_RECORD_BASE + concrete);
    specialization *entry = &c->specializations[c->specialization_count++];
    entry->original = generic;
    entry->concrete = concrete;
    entry->count = (unsigned)type_count;
    for (size_t i = 0; i < type_count; i++)
        entry->types[i] = types[i];
    for (size_t f = c->nodes[concrete].a; f && c->error.kind != PS_LANG_ERROR;
         f = c->nodes[f].next) {
        c->info[f].type = enumeration ? c->info[concrete].type
                                      : annotation(c, c->nodes[f].a, 0);
        if (enumeration)
            for (size_t payload = c->nodes[f].a;
                 payload && c->error.kind != PS_LANG_ERROR;
                 payload = c->nodes[payload].next)
                c->info[payload].type = annotation(c, c->nodes[payload].a, 0);
        if (enumeration && c->nodes[f].a && c->error.kind != PS_LANG_ERROR)
            c->info[f].function_type = function_type(c, f, c->nodes[f].a,
                                                      c->info[concrete].type);
    }
    if (enumeration) {
        if (!c->nodes[concrete].a)
            fail(c, at, "Enum requires at least one case");
        else if (c->error.kind != PS_LANG_ERROR)
            check_raw_enum(c, concrete);
    }
    if (c->error.kind != PS_LANG_ERROR)
        check_record(c, concrete);
    for (size_t m = c->nodes[concrete].b; m && c->error.kind != PS_LANG_ERROR;
         m = c->nodes[m].next) {
        if (c->nodes[m].kind == PS_AST_GENERIC_FUNCTION)
            generic_signature(c, m, concrete);
        else
            function_signature(c, m, concrete);
        if (!enumeration && word(c, c->nodes[m].token, "init"))
            for (size_t prior = c->nodes[concrete].b;
                 prior != m && c->error.kind != PS_LANG_ERROR;
                 prior = c->nodes[prior].next)
                if (word(c, c->nodes[prior].token, "init") &&
                    (c->nodes[prior].kind == PS_AST_GENERIC_FUNCTION ||
                     c->nodes[m].kind == PS_AST_GENERIC_FUNCTION
                         ? initializer_template_signature_same(c, prior, m)
                         : initializer_signature_same(c, prior, m)))
                    fail(c, at, "Generic struct initializer signatures collide after specialization");
    }
    return c->error.kind == PS_LANG_ERROR ? 0 : concrete;
}
static void generic_record_signature(checker *c, size_t id) {
    int enumeration = c->nodes[id].kind == PS_AST_GENERIC_ENUM;
    size_t signature = c->nodes[id].c;
    size_t parameters = 0;
    for (size_t p = c->nodes[signature].a; p && c->error.kind != PS_LANG_ERROR;
         p = c->nodes[p].next) {
        parameters++;
        if (parameters > GENERIC_TYPE_LIMIT)
            fail(c, p, "Generic type supports at most 8 type parameters");
        if (builtin_type_name(c, c->nodes[p].token))
            fail(c, p, "Generic type parameter conflicts with a builtin type");
        if (word(c, c->nodes[p].token, "self") ||
            same(c, c->nodes[p].token, c->nodes[id].token))
            fail(c, p, "Generic type parameter conflicts with a reserved name");
        for (size_t prior = c->nodes[signature].a; prior != p;
             prior = c->nodes[prior].next)
            if (same(c, c->nodes[prior].token, c->nodes[p].token))
                fail(c, p, "Duplicate generic type parameter");
        validate_constraints(c, p);
    }
    for (size_t f = c->nodes[id].a; f && c->error.kind != PS_LANG_ERROR;
         f = c->nodes[f].next) {
        if (enumeration) {
            for (size_t payload = c->nodes[f].a;
                 payload && c->error.kind != PS_LANG_ERROR;
                 payload = c->nodes[payload].next) {
                validate_generic_annotation(c, id, c->nodes[payload].a, 0);
                for (size_t prior = c->nodes[f].a; prior != payload;
                     prior = c->nodes[prior].next)
                    if (same(c, c->nodes[prior].token, c->nodes[payload].token))
                        fail(c, payload, "Duplicate enum payload field");
            }
        } else
            validate_generic_annotation(c, id, c->nodes[f].a, 0);
        for (size_t prior = c->nodes[id].a; prior != f;
             prior = c->nodes[prior].next)
            if (same(c, c->nodes[prior].token, c->nodes[f].token))
                fail(c, f, enumeration ? "Duplicate enum case" : "Duplicate struct field");
    }
    if (enumeration) {
        if (!c->nodes[id].a)
            fail(c, id, "Enum requires at least one case");
        else if (c->error.kind != PS_LANG_ERROR)
            check_raw_enum(c, id);
    }
    for (size_t m = c->nodes[id].b; m && c->error.kind != PS_LANG_ERROR;
         m = c->nodes[m].next) {
        if (enumeration && word(c, c->nodes[m].token, "init"))
            fail(c, m, "Enum methods cannot be named init");
        else if (word(c, c->nodes[m].token, "init")) {
            if (c->nodes[m].token.kind != PS_LANG_STATIC)
                fail(c, m, "Struct initializer must be a static function");
        }
        for (size_t f = c->nodes[id].a; f; f = c->nodes[f].next)
            if (same(c, c->nodes[f].token, c->nodes[m].token))
                fail(c, m, enumeration ? "Method name conflicts with an enum case"
                                       : "Method name conflicts with a field");
        if (enumeration && c->info[id].raw_enum &&
            (word(c, c->nodes[m].token, "rawValue") ||
             word(c, c->nodes[m].token, "fromRawValue")))
            fail(c, m, "Method name conflicts with an enum raw-value member");
        for (size_t prior = c->nodes[id].b; prior != m;
             prior = c->nodes[prior].next)
            if (same(c, c->nodes[prior].token, c->nodes[m].token) &&
                (c->nodes[prior].token.kind == PS_LANG_STATIC) ==
                    (c->nodes[m].token.kind == PS_LANG_STATIC) &&
                initializer_template_signature_same(c, prior, m))
                fail(c, m, word(c, c->nodes[m].token, "init")
                               ? "Duplicate struct initializer signature"
                               : "Duplicate method signature");
        size_t inner = c->nodes[m].kind == PS_AST_GENERIC_FUNCTION ? m : 0;
        size_t result = c->nodes[m].b;
        if (inner) {
            size_t method_signature = c->nodes[m].b;
            result = c->nodes[method_signature].b;
            size_t count = 0;
            for (size_t p = c->nodes[method_signature].a; p;
                 p = c->nodes[p].next) {
                count++;
                if (count > GENERIC_TYPE_LIMIT)
                    fail(c, p, "Generic function supports at most 8 type parameters");
                if (builtin_type_name(c, c->nodes[p].token))
                    fail(c, p, "Generic type parameter conflicts with a builtin type");
                if (word(c, c->nodes[p].token, "self"))
                    fail(c, p, "self is reserved for the method receiver");
                if (type_parameter(c, id, c->nodes[p].token) >= 0)
                    fail(c, p, enumeration
                                  ? "Method type parameter shadows an enum type parameter"
                                  : "Method type parameter shadows a struct type parameter");
                for (size_t prior = c->nodes[method_signature].a; prior != p;
                     prior = c->nodes[prior].next)
                    if (same(c, c->nodes[prior].token, c->nodes[p].token))
                        fail(c, p, "Duplicate generic type parameter");
                validate_constraints(c, p);
            }
        }
        if (!enumeration && word(c, c->nodes[m].token, "init")) {
            int own_result = result && c->nodes[result].kind == PS_AST_GENERIC_TYPE &&
                             c->nodes[result].a &&
                             c->nodes[c->nodes[result].a].kind == PS_AST_TYPE &&
                             same(c, c->nodes[c->nodes[result].a].token, c->nodes[id].token);
            size_t argument = own_result ? c->nodes[result].b : 0;
            size_t parameter = c->nodes[signature].a;
            while (own_result && argument && parameter) {
                own_result = c->nodes[argument].kind == PS_AST_TYPE &&
                             same(c, c->nodes[argument].token, c->nodes[parameter].token);
                argument = c->nodes[argument].next;
                parameter = c->nodes[parameter].next;
            }
            if (!own_result || argument || parameter)
                fail(c, m, "Struct initializer must return its struct type");
        }
        validate_scoped_annotation(c, id, inner, result, 1);
        for (size_t p = c->nodes[m].a; p && c->error.kind != PS_LANG_ERROR;
             p = c->nodes[p].next) {
            if (c->nodes[p].kind == PS_AST_SELF_PARAMETER)
                continue;
            validate_scoped_annotation(c, id, inner, c->nodes[p].a, 0);
            if (word(c, c->nodes[p].token, "self"))
                fail(c, p, "self is reserved for the method receiver");
            for (size_t prior = c->nodes[m].a; prior != p;
                 prior = c->nodes[prior].next)
                if (c->nodes[prior].kind != PS_AST_SELF_PARAMETER &&
                    same(c, c->nodes[prior].token, c->nodes[p].token))
                    fail(c, p, "Duplicate parameter name");
        }
    }
}
static void default_calls(checker *c, size_t id, size_t *stack, size_t count,
                          unsigned depth) {
    if (!id || c->error.kind == PS_LANG_ERROR)
        return;
    if (depth >= 128) {
        fail(c, id, "Struct field default nesting limit exceeded");
        return;
    }
    if (!c->work) {
        fail(c, id, "Semantic work budget exceeded");
        return;
    }
    c->work--;
    const ps_lang_node *n = &c->nodes[id];
    if (n->kind == PS_AST_CALL) {
        default_calls(c, n->a, stack, count, depth + 1);
        for (size_t a = n->b; a && c->error.kind != PS_LANG_ERROR; a = c->nodes[a].next)
            default_calls(c, c->nodes[a].a, stack, count, depth + 1);
        size_t record = c->info[id].binding;
        if (record && record < c->count && c->nodes[record].kind == PS_AST_STRUCT &&
            c->info[record].type == c->info[id].type)
            for (size_t f = c->nodes[record].a; f && c->error.kind != PS_LANG_ERROR;
                 f = c->nodes[f].next) {
                if (!c->nodes[f].b)
                    continue;
                int supplied = 0;
                for (size_t a = n->b; a; a = c->nodes[a].next)
                    supplied |= c->info[a].binding == f;
                if (supplied)
                    continue;
                for (size_t i = 0; i < count; i++)
                    if (stack[i] == f) {
                        fail(c, id, "Recursive struct field default");
                        return;
                    }
                if (count >= 128) {
                    fail(c, id, "Struct field default nesting limit exceeded");
                    return;
                }
                stack[count] = f;
                default_calls(c, c->nodes[f].b, stack, count + 1, depth + 1);
            }
        return;
    }
    if (n->kind == PS_AST_ARRAY || n->kind == PS_AST_INTERPOLATED_STRING)
        for (size_t item = n->a; item && c->error.kind != PS_LANG_ERROR;
             item = c->nodes[item].next)
            default_calls(c, item, stack, count, depth + 1);
    else if (n->kind == PS_AST_UNARY || n->kind == PS_AST_MEMBER ||
             n->kind == PS_AST_EXPRESSION)
        default_calls(c, n->a, stack, count, depth + 1);
    else if (n->kind == PS_AST_BINARY || n->kind == PS_AST_INDEX ||
             n->kind == PS_AST_STRIDED_RANGE) {
        default_calls(c, n->a, stack, count, depth + 1);
        default_calls(c, n->b, stack, count, depth + 1);
    } else if (n->kind == PS_AST_CONDITIONAL) {
        default_calls(c, n->a, stack, count, depth + 1);
        default_calls(c, n->b, stack, count, depth + 1);
        default_calls(c, n->c, stack, count, depth + 1);
    }
}

ps_lang_check_result ps_lang_check(const void *source, size_t size, ps_lang_node *nodes,
                                   ps_lang_parse_result parsed, ps_lang_semantic *info,
                                   size_t node_capacity, size_t info_capacity) {
    ps_lang_token bad = {PS_LANG_ERROR, 0, 0, 1, 1, "Invalid semantic input or capacity", 0};
    if (!parsed.root)
        return (ps_lang_check_result){0, parsed.diagnostic.kind == PS_LANG_ERROR ? parsed.diagnostic
                                                                                 : bad, parsed.count};
    if ((!source && size) || !nodes || !info || parsed.root >= parsed.count ||
        node_capacity < parsed.count || info_capacity < parsed.count ||
        parsed.count > SIZE_MAX / sizeof(*info) ||
        parsed.count >= PS_TYPE_OPTIONAL_BASE - PS_TYPE_RECORD_BASE ||
        parsed.count > INT_MAX - PS_TYPE_ARRAY_BASE)
        return (ps_lang_check_result){0, bad, parsed.count};
    for (size_t i = 1; i < parsed.count; i++) {
        const ps_lang_node *n = &nodes[i];
        if (n->token.offset > size || n->token.length > size - n->token.offset || n->a >= i ||
            n->b >= i || n->c >= i || (n->next && (n->next <= i || n->next >= parsed.count)))
            return (ps_lang_check_result){0, bad, parsed.count};
    }
    if (nodes[parsed.root].kind != PS_AST_MODULE)
        return (ps_lang_check_result){0, bad, parsed.count};
    memset(info, 0, parsed.count * sizeof(*info));
    checker c = {0};
    c.source = source;
    c.nodes = nodes;
    c.info = info;
    c.work = 4000000;
    c.count = parsed.count;
    c.node_capacity = node_capacity;
    c.info_capacity = info_capacity;
    size_t first = nodes[parsed.root].a;
    for (size_t id = first; id && c.error.kind != PS_LANG_ERROR; id = nodes[id].next)
        if (nodes[id].kind == PS_AST_IMPORT) {
            if (!nodes[id].a || nodes[nodes[id].a].kind != PS_AST_MODULE)
                fail(&c, id, "Import requires a loaded module");
            else
                declare(&c, id);
        }
    /* Nominal types are registered before any field or function signature. */
    for (size_t id = first; id && c.error.kind != PS_LANG_ERROR; id = nodes[id].next) {
        if (nodes[id].kind != PS_AST_STRUCT && nodes[id].kind != PS_AST_ENUM &&
            !generic_record_kind(nodes[id].kind))
            continue;
        ps_lang_token name = nodes[id].token;
        if (reserved_nominal_type_name(&c, name))
            fail(&c, id, "Type name conflicts with a builtin type");
        if (!generic_record_kind(nodes[id].kind))
            info[id].type = (ps_lang_type)(PS_TYPE_RECORD_BASE + id);
        declare(&c, id);
    }
    for (size_t id = first; id && c.error.kind != PS_LANG_ERROR; id = nodes[id].next)
        if (generic_record_kind(nodes[id].kind))
            generic_record_signature(&c, id);
    for (size_t id = first; id && c.error.kind != PS_LANG_ERROR; id = nodes[id].next) {
        if (nodes[id].kind != PS_AST_STRUCT && nodes[id].kind != PS_AST_ENUM)
            continue;
        for (size_t f = nodes[id].a; f && c.error.kind != PS_LANG_ERROR; f = nodes[f].next) {
            info[f].type =
                nodes[id].kind == PS_AST_ENUM ? info[id].type : annotation(&c, nodes[f].a, 0);
            if (nodes[id].kind == PS_AST_ENUM)
                for (size_t payload = nodes[f].a; payload && c.error.kind != PS_LANG_ERROR;
                     payload = nodes[payload].next) {
                    info[payload].type = annotation(&c, nodes[payload].a, 0);
                    ps_lang_type pt = info[payload].type;
                    if (!value_type(pt))
                        fail(&c, payload, "Enum payload field must be a value type");
                    for (size_t prior = nodes[f].a; prior != payload; prior = nodes[prior].next)
                        if (same(&c, nodes[prior].token, nodes[payload].token))
                            fail(&c, payload, "Duplicate enum payload field");
                }
            if (nodes[id].kind == PS_AST_ENUM && nodes[f].a && c.error.kind != PS_LANG_ERROR)
                info[f].function_type = function_type(&c, f, nodes[f].a, info[id].type);
            for (size_t prior = nodes[id].a; prior != f; prior = nodes[prior].next)
                if (same(&c, nodes[prior].token, nodes[f].token))
                    fail(&c, f,
                         nodes[id].kind == PS_AST_ENUM ? "Duplicate enum case"
                                                       : "Duplicate struct field");
        }
    }
    for (size_t id = first; id && c.error.kind != PS_LANG_ERROR; id = nodes[id].next)
        if (nodes[id].kind == PS_AST_ENUM) {
            if (!nodes[id].a)
                fail(&c, id, "Enum requires at least one case");
            else
                check_raw_enum(&c, id);
        }
    for (size_t id = first; id && c.error.kind != PS_LANG_ERROR; id = nodes[id].next)
        if (nodes[id].kind == PS_AST_STRUCT || nodes[id].kind == PS_AST_ENUM)
            check_record(&c, id);
    /* Predeclare signatures for forward calls and recursion. */
    for (size_t id = first; id && c.error.kind != PS_LANG_ERROR; id = nodes[id].next) {
        if (nodes[id].kind == PS_AST_FUNCTION)
            function_signature(&c, id, 0);
        if (nodes[id].kind == PS_AST_GENERIC_FUNCTION)
            generic_signature(&c, id, 0);
        if (nodes[id].kind == PS_AST_FUNCTION ||
            nodes[id].kind == PS_AST_GENERIC_FUNCTION) {
            for (size_t prior = info[id].previous; prior && c.error.kind != PS_LANG_ERROR;
                 prior = info[prior].previous)
                if ((nodes[prior].kind == PS_AST_FUNCTION ||
                     nodes[prior].kind == PS_AST_GENERIC_FUNCTION) &&
                    nodes[prior].token.file == nodes[id].token.file &&
                    same(&c, nodes[prior].token, nodes[id].token) &&
                    (nodes[prior].kind == PS_AST_GENERIC_FUNCTION ||
                     nodes[id].kind == PS_AST_GENERIC_FUNCTION
                         ? initializer_template_signature_same(&c, prior, id)
                         : initializer_signature_same(&c, prior, id)))
                    fail(&c, id, "Duplicate free function signature");
        }
        if (nodes[id].kind == PS_AST_STRUCT || nodes[id].kind == PS_AST_ENUM) {
            for (size_t m = nodes[id].b; m && c.error.kind != PS_LANG_ERROR; m = nodes[m].next) {
                for (size_t f = nodes[id].a; f && c.error.kind != PS_LANG_ERROR; f = nodes[f].next)
                    if (same(&c, nodes[f].token, nodes[m].token))
                        fail(&c, m, nodes[id].kind == PS_AST_ENUM
                                        ? "Method name conflicts with an enum case"
                                        : "Method name conflicts with a field");
                if (nodes[id].kind == PS_AST_ENUM && info[id].raw_enum &&
                    (word(&c, nodes[m].token, "rawValue") ||
                     word(&c, nodes[m].token, "fromRawValue")))
                    fail(&c, m, "Method name conflicts with an enum raw-value member");
                if (word(&c, nodes[m].token, "self"))
                    fail(&c, m, "self is reserved for the method receiver");
                if (nodes[m].kind == PS_AST_GENERIC_FUNCTION)
                    generic_signature(&c, m, id);
                else
                    function_signature(&c, m, id);
                for (size_t prior = nodes[id].b; prior != m && c.error.kind != PS_LANG_ERROR;
                     prior = nodes[prior].next)
                    if (same(&c, nodes[prior].token, nodes[m].token) &&
                        info[prior].method_static == info[m].method_static &&
                            (nodes[prior].kind == PS_AST_GENERIC_FUNCTION ||
                             nodes[m].kind == PS_AST_GENERIC_FUNCTION
                                 ? initializer_template_signature_same(&c, prior, m)
                                 : initializer_signature_same(&c, prior, m)))
                        fail(&c, m, word(&c, nodes[m].token, "init")
                                        ? "Duplicate struct initializer signature"
                                        : "Duplicate method signature");
            }
        }
    }
    /* Module statements are checked in source order. */
    for (size_t id = first; id && c.error.kind != PS_LANG_ERROR; id = nodes[id].next)
        if (nodes[id].kind != PS_AST_FUNCTION && nodes[id].kind != PS_AST_STRUCT &&
            nodes[id].kind != PS_AST_ENUM && nodes[id].kind != PS_AST_IMPORT &&
            !generic_record_kind(nodes[id].kind) &&
            nodes[id].kind != PS_AST_GENERIC_FUNCTION)
            (void)statement(&c, id);
    size_t globals = c.head;
    for (size_t id = 1; id < c.count && c.error.kind != PS_LANG_ERROR; id++) {
        if (nodes[id].kind == PS_AST_FIELD && nodes[id].b && info[id].type) {
            /* Defaults resolve names in their declaring module, not the caller's locals. */
            c.head = c.scope_base = globals;
            c.function = c.self_parameter = 0;
            (void)expression(&c, nodes[id].b, info[id].type);
            continue;
        }
        if (nodes[id].kind != PS_AST_FUNCTION || info[id].type == PS_TYPE_NONE)
            continue;
        c.head = c.scope_base = globals;
        c.function = id;
        c.self_parameter = info[id].method_owner && !info[id].method_static ? nodes[id].a : 0;
        if (c.self_parameter)
            info[c.self_parameter].local_function = id;
        for (size_t p = nodes[id].a; p; p = nodes[p].next)
            if (nodes[p].kind != PS_AST_SELF_PARAMETER)
                declare(&c, p);
        int returns = sequence(&c, nodes[nodes[id].c].a);
        if (info[id].type != PS_TYPE_VOID && !returns)
            fail(&c, id, "Not all function paths return a value");
    }
    /* Constructors inline omitted defaults. Reject cycles before C emission. */
    for (size_t id = 1; id < c.count && c.error.kind != PS_LANG_ERROR; id++)
        if (nodes[id].kind == PS_AST_FIELD && nodes[id].b && info[id].type) {
            size_t stack[128] = {id};
            default_calls(&c, nodes[id].b, stack, 1, 0);
        }
    return (ps_lang_check_result){c.error.kind != PS_LANG_ERROR, c.error, c.count};
}
