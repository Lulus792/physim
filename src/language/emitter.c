#include "emitter.h"
#include "builtins.h"
#include "literal.h"
#include "version.h"
#include <inttypes.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

typedef struct emitter {
    FILE *out;
    const unsigned char *source;
    const char *source_path;
    const char *const *paths;
    size_t path_count;
    const ps_lang_node *nodes;
    const ps_lang_semantic *info;
    size_t node_count;
    unsigned char *global, *record_done, *owned;
    unsigned char *equal_record, *equal_optional, *equal_array;
    char type_name[40];
    size_t bytes, cached, break_mark, continue_mark, self_parameter, current_function;
    uint64_t primitive_types;
    unsigned depth;
    int experiment, sdk, arrays, strings;
    ps_lang_token error;
} emitter;
static void fail(emitter *e, size_t id, const char *message) {
    if (e->error.kind != PS_LANG_ERROR) {
        e->error = e->nodes[id].token;
        if (!e->error.line)
            e->error.line = 1;
        if (!e->error.column)
            e->error.column = 1;
        e->error.kind = PS_LANG_ERROR;
        e->error.error = message;
    }
}
static void out(emitter *e, const char *format, ...) {
    if (e->error.kind == PS_LANG_ERROR)
        return;
    va_list args;
    va_start(args, format);
    int n = vfprintf(e->out, format, args);
    va_end(args);
    if (n < 0)
        fail(e, 0, "Cannot write generated C");
    else if ((size_t)n > 32u * 1024u * 1024u - e->bytes)
        fail(e, 0, "Generated C exceeds 32 MiB output limit");
    else
        e->bytes += (size_t)n;
}
static void quoted(emitter *e, const unsigned char *text, size_t size, int decode) {
    out(e, "\"");
    for (size_t i = 0; i < size && e->error.kind != PS_LANG_ERROR; i++) {
        if (i && i % 256 == 0)
            out(e, "\" \\\n\"");
        unsigned char b = text[i];
        if (decode && b == '\\') {
            b = text[++i]; /* Parser validated the complete string. */
            if (b == 'n')
                b = '\n';
            else if (b == 'r')
                b = '\r';
            else if (b == 't')
                b = '\t';
        }
        out(e, "\\%03o", (unsigned)b);
    }
    out(e, "\"");
}
/* Byte arrays avoid implementation limits on the length of C string literals. */
static void literal_bytes(emitter *e, size_t id, const unsigned char *text, size_t size) {
    out(e, "static const unsigned char pslit_%zu[] = {\n", id);
    for (size_t i = 0; i < size && e->error.kind != PS_LANG_ERROR; i++) {
        unsigned char b = text[i];
        if (b == '_')
            continue;
        out(e, "%u,%s", (unsigned)b, i % 32 == 31 ? "\n" : "");
    }
    out(e, "0};\n");
}
static void string_literal(emitter *e, size_t id, ps_lang_token token) {
    out(e, "static const unsigned char pslit_%zu[] = {\n", id);
    ps_lang_string_cursor cursor = {0};
    size_t count = 0;
    for (int byte; e->error.kind != PS_LANG_ERROR &&
                   (byte = ps_lang_string_next_byte(e->source, token, &cursor)) >= 0; count++)
        out(e, "%u,%s", (unsigned)byte, count % 32 == 31 ? "\n" : "");
    out(e, "0};\n");
}
static void string_segment_literal(emitter *e, size_t id, ps_lang_token token) {
    out(e, "static const unsigned char pslit_%zu[] = {\n", id);
    ps_lang_string_cursor cursor = {0};
    size_t count = 0;
    for (int byte; e->error.kind != PS_LANG_ERROR &&
                   (byte = ps_lang_string_next_segment_byte(e->source, token, &cursor)) >= 0;
         count++)
        out(e, "%u,%s", (unsigned)byte, count % 32 == 31 ? "\n" : "");
    out(e, "0};\n");
}
static const char *type(emitter *e, ps_lang_type t) {
    if (t >= PS_TYPE_OPTIONAL_BASE)
        return "psrt_array";
    if (ps_lang_function_type(t))
        return "psrt_function_value";
    if (t >= PS_TYPE_RECORD_BASE) {
        snprintf(e->type_name, sizeof e->type_name, "pst_%zu", (size_t)(t - PS_TYPE_RECORD_BASE));
        return e->type_name;
    }
    switch (t) {
    case PS_TYPE_VEC2:
        return "ps_vec2";
    case PS_TYPE_VEC3:
        return "ps_vec3";
    case PS_TYPE_VEC4:
        return "ps_vec4";
    case PS_TYPE_QUAT:
        return "ps_quat";
    case PS_TYPE_MAT3:
        return "ps_mat3";
    case PS_TYPE_MAT4:
        return "ps_mat4";
    case PS_TYPE_BEZIER3:
        return "ps_bezier3";
    case PS_TYPE_UNIT:
        return "ps_unit";
    case PS_TYPE_QUANTITY:
        return "ps_quantity";
    case PS_TYPE_MEDIUM:
        return "ps_medium";
    case PS_TYPE_MATERIAL:
        return "ps_material";
    case PS_TYPE_SUBMERSION:
        return "ps_submersion";
    case PS_TYPE_DISTRIBUTION:
        return "ps_distribution";
    case PS_TYPE_SENSOR_CONFIG:
        return "ps_sensor_config";
    case PS_TYPE_SENSOR:
        return "ps_sensor";
    case PS_TYPE_MEASUREMENT:
        return "ps_measurement";
    case PS_TYPE_RNG:
        return "ps_rng";
    case PS_TYPE_BODY:
        return "ps_body";
    case PS_TYPE_CONTACTS:
        return "ps_contact_manifold";
    case PS_TYPE_CONTACT_SOLVER:
        return "ps_contact_solver";
    case PS_TYPE_CONTACT_RESULT:
        return "psrt_contact_result";
    case PS_TYPE_DISTANCE_JOINT:
        return "ps_distance_joint";
    case PS_TYPE_JOINT_RESULT:
        return "psrt_joint_result";
    case PS_TYPE_CONTACT_CONSTRAINT:
        return "psrt_graph_contact";
    case PS_TYPE_JOINT_CONSTRAINT:
        return "psrt_graph_joint";
    case PS_TYPE_CONSTRAINT_RESULT:
        return "psrt_constraint_result";
    case PS_TYPE_ODE_RESULT:
        return "psrt_ode_result_value";
    case PS_TYPE_SCALAR_RESULT:
        return "ps_scalar_report";
    case PS_TYPE_SWEEP: return "psrt_sweep";
    case PS_TYPE_AABB: return "ps_aabb";
    case PS_TYPE_COLLISION_PAIR: return "ps_collision_pair";
    case PS_TYPE_CHANNEL:
        return "psrt_channel";
    case PS_TYPE_DATASET:
        return "ps_dataset";
    case PS_TYPE_SERIES:
        return "ps_series";
    case PS_TYPE_PLOT:
        return "ps_plot_handle";
    case PS_TYPE_TABLE:
        return "ps_table_handle";
    case PS_TYPE_INT64:
        return "int64_t";
    case PS_TYPE_FLOAT64:
        return "double";
    case PS_TYPE_BOOL:
        return "bool";
    case PS_TYPE_STRING:
        return "psrt_string";
    case PS_TYPE_VOID:
        return "void";
    default:
        return "void";
    }
}
static void site(emitter *e, size_t id) {
    size_t file = e->nodes[id].token.file;
    if (file >= e->path_count)
        file = 0;
    out(e, "((psrt_site){");
    quoted(e, (const unsigned char *)e->paths[file], strlen(e->paths[file]), 0);
    out(e, ", %zu, %zu})", e->nodes[id].token.line, e->nodes[id].token.column);
}
/* Keep native compiler diagnostics and debug symbols tied to the Physim source.
 * The runtime still uses PSRT_AT for exact expression positions. */
static void source_line(emitter *e, size_t id) {
    out(e, "\n#line %zu ", e->nodes[id].token.line ? e->nodes[id].token.line : 1);
    size_t file = e->nodes[id].token.file;
    if (file >= e->path_count)
        file = 0;
    quoted(e, (const unsigned char *)e->paths[file], strlen(e->paths[file]), 0);
    out(e, "\n");
}
static int owns(emitter *e, ps_lang_type t) {
    if (!e->arrays)
        return 0;
    if (ps_lang_function_type(t) || t >= PS_TYPE_OPTIONAL_BASE ||
        t == PS_TYPE_CONSTRAINT_RESULT ||
        t == PS_TYPE_ODE_RESULT || t == PS_TYPE_STRING)
        return 1;
    if (!ps_lang_record_type(t))
        return 0;
    size_t id = (size_t)(t - PS_TYPE_RECORD_BASE);
    if (!e->owned[id]) {
        e->owned[id] = 1;
        if (e->nodes[id].kind == PS_AST_STRUCT)
            for (size_t f = e->nodes[id].a; f; f = e->nodes[f].next)
                if (owns(e, e->info[f].type))
                    e->owned[id] = 2;
        if (e->nodes[id].kind == PS_AST_ENUM)
            for (size_t item = e->nodes[id].a; item; item = e->nodes[item].next)
                for (size_t f = e->nodes[item].a; f; f = e->nodes[f].next)
                    if (owns(e, e->info[f].type))
                        e->owned[id] = 2;
    }
    return e->owned[id] == 2;
}
static int enum_payload(emitter *e, ps_lang_type t) {
    if (!ps_lang_record_type(t)) return 0;
    size_t id = (size_t)(t - PS_TYPE_RECORD_BASE);
    if (e->nodes[id].kind != PS_AST_ENUM) return 0;
    for (size_t c = e->nodes[id].a; c; c = e->nodes[c].next)
        if (e->nodes[c].a) return 1;
    return 0;
}
static unsigned char *equal_state(emitter *e, ps_lang_type t) {
    if (ps_lang_optional_type(t))
        return &e->equal_optional[t - PS_TYPE_OPTIONAL_BASE];
    if (t >= PS_TYPE_ARRAY_BASE)
        return &e->equal_array[t - PS_TYPE_ARRAY_BASE];
    if (ps_lang_record_type(t))
        return &e->equal_record[t - PS_TYPE_RECORD_BASE];
    return NULL;
}
static void mark_equal_type(emitter *e, ps_lang_type t) {
    unsigned char *state = equal_state(e, t);
    if (!state || *state) return;
    *state = 1;
    if (ps_lang_optional_type(t)) {
        mark_equal_type(e, e->info[t - PS_TYPE_OPTIONAL_BASE].optional_element);
    } else if (t >= PS_TYPE_ARRAY_BASE) {
        mark_equal_type(e, e->info[t - PS_TYPE_ARRAY_BASE].array_element);
    } else {
        size_t id = (size_t)(t - PS_TYPE_RECORD_BASE);
        if (e->nodes[id].kind == PS_AST_STRUCT)
            for (size_t f = e->nodes[id].a; f; f = e->nodes[f].next)
                mark_equal_type(e, e->info[f].type);
        else if (e->nodes[id].kind == PS_AST_ENUM)
            for (size_t item = e->nodes[id].a; item; item = e->nodes[item].next)
                for (size_t f = e->nodes[item].a; f; f = e->nodes[f].next)
                    mark_equal_type(e, e->info[f].type);
    }
}
static void descriptor(emitter *e, ps_lang_type t) {
    if (t >= PS_TYPE_OPTIONAL_BASE || t == PS_TYPE_STRING)
        out(e, "&psrt_array_element_type");
    else
        out(e, "&psdesc_%u", (unsigned)t);
}
static void keeper(emitter *e, ps_lang_type t) {
    if (ps_lang_function_type(t))
        out(e, "pskeep_function");
    else if (t == PS_TYPE_CONSTRAINT_RESULT)
        out(e, "psrt_constraints_keep");
    else if (t == PS_TYPE_ODE_RESULT)
        out(e, "psrt_ode_result_keep");
    else if (t >= PS_TYPE_OPTIONAL_BASE || t == PS_TYPE_STRING)
        out(e, "pskeep_array");
    else
        out(e, "pskeep_%u", (unsigned)t);
}
static void destroyer(emitter *e, ps_lang_type t) {
    if (ps_lang_function_type(t))
        out(e, "psrt_function_destroy");
    else if (t == PS_TYPE_CONSTRAINT_RESULT)
        out(e, "psrt_constraints_drop");
    else if (t == PS_TYPE_ODE_RESULT)
        out(e, "psrt_ode_result_drop");
    else if (t >= PS_TYPE_OPTIONAL_BASE || t == PS_TYPE_STRING)
        out(e, "psrt_array_destroy");
    else
        out(e, "psdrop_%u", (unsigned)t);
}
static void retain_temp(emitter *e, size_t id) {
    keeper(e, e->info[id].type);
    out(e, "(&psv_%zu, ", id);
    site(e, id);
    out(e, ");\n");
}
static void register_temp(emitter *e, size_t id) {
    out(e, "psrt_cleanup psown_%zu; psrt_cleanup_push(&psown_%zu, &psv_%zu, ", id, id, id);
    destroyer(e, e->info[id].type);
    out(e, ");\n");
}
static void checked_end(emitter *e, size_t id) {
    out(e, ", ");
    site(e, id);
    out(e, ");\n");
}
static size_t closure_capture(emitter *e, size_t lambda, size_t decl) {
    if (!lambda || (e->nodes[lambda].kind != PS_AST_LAMBDA &&
                    e->nodes[lambda].kind != PS_AST_LOCAL_FUNCTION))
        return 0;
    for (size_t capture = e->info[lambda].capture_head; capture;
         capture = e->info[capture].capture_next)
        if (e->info[capture].binding == decl)
            return capture;
    return 0;
}
static void storage(emitter *e, size_t id) {
    if (id == e->current_function && e->nodes[id].kind == PS_AST_LOCAL_FUNCTION) {
        out(e, "(*psself)");
        return;
    }
    if (closure_capture(e, e->current_function, id)) {
        out(e, "pscapture->psfield_%zu", id);
        return;
    }
    out(e, "%sps%c_%zu", e->experiment && e->global[id] ? "psstate->" : "",
        e->global[id] ? 'g' : 'v', id);
}
static void field_name(emitter *e, size_t binding) {
    const char *name = ps_lang_member_name(binding);
    if (name)
        out(e, "%s", name);
    else
        out(e, "psfield_%zu", binding);
}
static void target(emitter *e, size_t id) {
    if (e->nodes[id].kind == PS_AST_MEMBER) {
        target(e, e->nodes[id].a);
        out(e, ".");
        field_name(e, e->info[id].binding);
    } else
        storage(e, e->info[id].binding);
}
static size_t target_root(emitter *e, size_t id) {
    while (e->nodes[id].kind == PS_AST_MEMBER)
        id = e->nodes[id].a;
    return e->info[id].binding;
}
static void initialized(emitter *e, size_t decl, size_t use) {
    if (e->global[decl] || (e->nodes[decl].kind == PS_AST_VARIABLE &&
                            !e->nodes[decl].b &&
                            !closure_capture(e, e->current_function, decl))) {
        out(e, "%s(%spsready_%zu, ",
            e->global[decl] ? "psrt_initialized" : "psrt_local_initialized",
            e->experiment && e->global[decl] ? "psstate->" : "", decl);
        site(e, use);
        out(e, ");\n");
    }
}
static const char *operator_text(ps_lang_kind k) {
    switch (k) {
    case PS_LANG_PLUS:
        return "+";
    case PS_LANG_MINUS:
        return "-";
    case PS_LANG_STAR:
        return "*";
    case PS_LANG_EQ:
        return "==";
    case PS_LANG_NE:
        return "!=";
    case PS_LANG_LT:
        return "<";
    case PS_LANG_LE:
        return "<=";
    case PS_LANG_GT:
        return ">";
    case PS_LANG_GE:
        return ">=";
    default:
        return "?";
    }
}
static void expression(emitter *e, size_t id);
static void equal_value(emitter *e, ps_lang_type t, const char *left, const char *right);
static size_t range_base(emitter *e, size_t id) {
    return e->nodes[id].kind == PS_AST_STRIDED_RANGE ? e->nodes[id].a : id;
}
static size_t range_step(emitter *e, size_t id) {
    return e->nodes[id].kind == PS_AST_STRIDED_RANGE ? e->nodes[id].b : 0;
}
static void positive_step(emitter *e, size_t id) {
    if (!id)
        return;
    out(e, "if (psv_%zu <= 0) psrt_fail(", id);
    site(e, id);
    out(e, ", \"Range step must be positive\");\n");
}
static void nonzero_step(emitter *e, size_t id) {
    if (!id)
        return;
    out(e, "if (psv_%zu == 0) psrt_fail(", id);
    site(e, id);
    out(e, ", \"Range step must not be zero\");\n");
}
static void range_arguments(emitter *e, const ps_lang_node *range) {
    out(e, "%d,", range->a != 0);
    if (range->a)
        out(e, "psv_%zu", range->a);
    else
        out(e, "0");
    out(e, ",%d,", range->b != 0);
    if (range->b)
        out(e, "psv_%zu", range->b);
    else
        out(e, "0");
    out(e, ",%d", range->token.kind == PS_LANG_RANGE_CLOSED);
}
static void writeback(emitter *e, size_t id, size_t node, size_t value);
static int optional_call(size_t binding) {
    return binding == PS_LANG_BUILTIN_OPTIONAL_SOME || binding == PS_LANG_BUILTIN_OPTIONAL_UNWRAP ||
           binding == PS_LANG_BUILTIN_OPTIONAL_OR;
}
static int borrowed_symbol_builtin(const ps_lang_builtin *builtin) {
    if (!builtin)
        return 0;
    const char *name = builtin->c_name;
    return !strcmp(name, "psrt_unit") || !strcmp(name, "psrt_unit_multiply") ||
           !strcmp(name, "psrt_unit_divide") || !strcmp(name, "psrt_unit_power") ||
           !strcmp(name, "psrt_quantity_multiply") || !strcmp(name, "psrt_quantity_divide");
}
static void optional_init(emitter *e, size_t id) {
    out(e, "psrt_array psv_%zu; psvalue_check(psrt_array_init(", id);
    descriptor(e, e->info[e->info[id].type - PS_TYPE_OPTIONAL_BASE].optional_element);
    out(e, ",psrt_memory_allocator(&%s),0,&psv_%zu)",
        e->experiment ? "psstate->memory" : "psmemory", id);
    checked_end(e, id);
    register_temp(e, id);
}
/* Replace an owned String receiver snapshot with an owned array of scalars.
 * Both handles share the array representation and cleanup function. */
static void scalar_array_receiver(emitter *e, size_t receiver, size_t id) {
    out(e, "psrt_array psscalars_%zu={0};\n"
           "psvalue_check(psrt_string_scalars(&psv_%zu,", id, receiver);
    descriptor(e, PS_TYPE_STRING);
    out(e, ",psrt_memory_allocator(&%s),&psscalars_%zu)",
        e->experiment ? "psstate->memory" : "psmemory", id);
    checked_end(e, id);
    out(e, "psrt_string_destroy(&psv_%zu); psv_%zu=psscalars_%zu;\n",
        receiver, receiver, id);
}
static void call(emitter *e, size_t id) {
    const ps_lang_node *n = &e->nodes[id];
    size_t fn = e->info[id].binding;
    if (fn == PS_LANG_BUILTIN_INT64_IS_MULTIPLE || fn == PS_LANG_BUILTIN_INT64_SIGNUM) {
        size_t receiver = e->nodes[n->a].a;
        expression(e, receiver);
        if (fn == PS_LANG_BUILTIN_INT64_IS_MULTIPLE) {
            size_t arg = e->nodes[n->b].a;
            expression(e, arg);
            out(e, "bool psv_%zu = psrt_int64_is_multiple(psv_%zu, psv_%zu);\n",
                id, receiver, arg);
        } else {
            out(e, "int64_t psv_%zu = psrt_int64_signum(psv_%zu);\n", id, receiver);
        }
        return;
    }
    if (fn == PS_LANG_BUILTIN_INDIRECT_CALL) {
        expression(e, n->a);
        for (size_t a = n->b; a; a = e->nodes[a].next)
            expression(e, e->nodes[a].a);
        if (e->info[id].type != PS_TYPE_VOID)
            out(e, "%s psv_%zu = {0};\n", type(e, e->info[id].type), id);
        out(e, "switch (psv_%zu.tag) {\n", n->a);
        for (size_t target = 1; target < e->node_count; target++) {
            if (e->nodes[target].kind == PS_AST_CASE &&
                e->info[target].function_type == e->info[n->a].type) {
                out(e, "case %zu: psv_%zu.tag = INT64_C(%zu); ", target, id, target);
                size_t argument = n->b;
                for (size_t field = e->nodes[target].a; field;
                     field = e->nodes[field].next) {
                    out(e, "psv_%zu.psdata.pscase_%zu.psfield_%zu = psv_%zu; ",
                        id, target, field, e->nodes[argument].a);
                    argument = e->nodes[argument].next;
                }
                if (owns(e, e->info[id].type))
                    retain_temp(e, id);
                out(e, "break;\n");
                continue;
            }
            if ((e->nodes[target].kind != PS_AST_FUNCTION &&
                 e->nodes[target].kind != PS_AST_LAMBDA &&
                 e->nodes[target].kind != PS_AST_LOCAL_FUNCTION) ||
                e->info[target].function_type != e->info[n->a].type)
                continue;
            out(e, "case %zu: ", target);
            int bound = e->info[target].method_owner && !e->info[target].method_static;
            int lambda = e->nodes[target].kind == PS_AST_LAMBDA;
            int local = e->nodes[target].kind == PS_AST_LOCAL_FUNCTION;
            if (bound || ((lambda || local) && e->info[target].capture_head)) {
                out(e, "if(psrt_array_count(&psv_%zu.receiver)!=1) psrt_fail(", n->a);
                site(e, id);
                out(e, ",\"Invalid bound method receiver\"); ");
            }
            if (e->info[id].type != PS_TYPE_VOID)
                out(e, "psv_%zu = ", id);
            out(e, "psfn_%zu(", target);
            if (e->experiment)
                out(e, "psstate%s", bound || lambda || local || n->b ? ", " : "");
            if (local) {
                out(e, "&psv_%zu%s", n->a, n->b ? ", " : "");
            } else if (lambda) {
                out(e, "(const pscapture_%zu *)psrt_array_data(&psv_%zu.receiver)%s",
                    target, n->a, n->b ? ", " : "");
            } else if (bound) {
                size_t self = e->nodes[target].a;
                if (e->info[target].method_mutating)
                    out(e, "(%s *)psrt_function_mutable_receiver(&psv_%zu)%s",
                        type(e, e->info[self].type), n->a, n->b ? ", " : "");
                else
                    out(e, "*(const %s *)psrt_array_data(&psv_%zu.receiver)%s",
                        type(e, e->info[self].type), n->a, n->b ? ", " : "");
            }
            for (size_t a = n->b; a; a = e->nodes[a].next)
                out(e, "psv_%zu%s", e->nodes[a].a, e->nodes[a].next ? ", " : "");
            out(e, "); break;\n");
        }
        out(e, "default: psrt_fail(");
        site(e, id);
        out(e, ", \"Invalid function value\");\n}\n");
        return;
    }
    if (fn == PS_LANG_BUILTIN_STRING_CAST) {
        size_t value = e->nodes[n->b].a;
        ps_lang_type input = e->info[value].type;
        expression(e, value);
        out(e, "psrt_string psv_%zu; psstring_check(", id);
        if (input == PS_TYPE_STRING)
            out(e, "psrt_string_clone(&psv_%zu,&psv_%zu)", value, id);
        else {
            out(e, "%s(psv_%zu,psrt_memory_allocator(&%s),&psv_%zu)",
                input == PS_TYPE_INT64 ? "psrt_string_from_int64"
                : input == PS_TYPE_FLOAT64 ? "psrt_string_from_float64"
                                           : "psrt_string_from_bool",
                value, e->experiment ? "psstate->memory" : "psmemory", id);
        }
        checked_end(e, id);
        return;
    }
    if (fn == PS_LANG_BUILTIN_STRING_JOINED) {
        size_t receiver = e->nodes[n->a].a, separator = e->nodes[n->b].a;
        expression(e, receiver);
        expression(e, separator);
        out(e, "psrt_string psv_%zu; psstring_check(psrt_string_join("
               "&psv_%zu,&psv_%zu,psrt_memory_allocator(&%s),&psv_%zu)",
            id, receiver, separator,
            e->experiment ? "psstate->memory" : "psmemory", id);
        checked_end(e, id);
        return;
    }
    if (fn == PS_LANG_BUILTIN_STRING_TRIMMED) {
        size_t receiver = e->nodes[n->a].a;
        expression(e, receiver);
        out(e, "psrt_string psv_%zu; psstring_check(psrt_string_trimmed("
               "&psv_%zu,psrt_memory_allocator(&%s),&psv_%zu)",
            id, receiver, e->experiment ? "psstate->memory" : "psmemory", id);
        checked_end(e, id);
        return;
    }
    if (fn == PS_LANG_BUILTIN_STRING_REVERSED) {
        size_t receiver = e->nodes[n->a].a;
        expression(e, receiver);
        out(e, "psrt_string psv_%zu; psstring_check(psrt_string_reversed("
               "&psv_%zu,psrt_memory_allocator(&%s),&psv_%zu)",
            id, receiver, e->experiment ? "psstate->memory" : "psmemory", id);
        checked_end(e, id);
        return;
    }
    if (fn == PS_LANG_BUILTIN_STRING_REPEATED) {
        size_t receiver = e->nodes[n->b].a;
        size_t count = e->nodes[e->nodes[n->b].next].a;
        expression(e, receiver);
        expression(e, count);
        out(e, "if(psv_%zu<0) psrt_fail(", count);
        site(e, id);
        out(e, ",\"String repetition count must be nonnegative\");\n");
        out(e, "psrt_string psv_%zu; psstring_check(psrt_string_repeat("
               "&psv_%zu,psv_%zu,psrt_memory_allocator(&%s),&psv_%zu)",
            id, receiver, count, e->experiment ? "psstate->memory" : "psmemory", id);
        checked_end(e, id);
        return;
    }
    if (fn == PS_LANG_BUILTIN_STRING_SPLIT || fn == PS_LANG_BUILTIN_ARRAY_SPLIT) {
        int string = fn == PS_LANG_BUILTIN_STRING_SPLIT;
        size_t receiver = e->nodes[n->a].a, separator = e->nodes[n->b].a;
        size_t second = e->nodes[n->b].next;
        size_t max_arg = second && e->info[second].type == PS_TYPE_INT64 ? second : 0;
        size_t omit_arg = max_arg ? e->nodes[max_arg].next : second;
        expression(e, receiver);
        expression(e, separator);
        if (max_arg) {
            expression(e, e->nodes[max_arg].a);
            out(e, "if(psv_%zu<0) psrt_fail(", e->nodes[max_arg].a);
            site(e, id);
            out(e, ",\"split maxSplits must be nonnegative\");\n");
        }
        if (omit_arg)
            expression(e, e->nodes[omit_arg].a);
        out(e, "psrt_array psv_%zu; %s(%s("
               "&psv_%zu,&psv_%zu,", id,
            string ? "psstring_check" : "psvalue_check",
            string ? "psrt_string_split" : "psrt_array_split",
            receiver, separator);
        if (max_arg)
            out(e, "psv_%zu,", e->nodes[max_arg].a);
        else
            out(e, "INT64_MAX,");
        if (omit_arg)
            out(e, "psv_%zu,", e->nodes[omit_arg].a);
        else
            out(e, "true,");
        if (!string)
            out(e, "pssplit_equal_%zu,", id);
        out(e, "psrt_memory_allocator(&%s),&psv_%zu)",
            e->experiment ? "psstate->memory" : "psmemory", id);
        checked_end(e, id);
        return;
    }
    if (fn == PS_LANG_BUILTIN_ARRAY_STARTS_WITH ||
        fn == PS_LANG_BUILTIN_ARRAY_ELEMENTS_EQUAL) {
        size_t receiver = e->nodes[n->a].a, candidate = e->nodes[n->b].a;
        ps_lang_type array = e->info[receiver].type;
        ps_lang_type element = e->info[array - PS_TYPE_ARRAY_BASE].array_element;
        expression(e, receiver);
        expression(e, candidate);
        out(e, "bool psv_%zu = psrt_array_count(&psv_%zu) %s psrt_array_count(&psv_%zu);\n",
            id, candidate, fn == PS_LANG_BUILTIN_ARRAY_STARTS_WITH ? "<=" : "==",
            receiver);
        out(e, "const unsigned char *psleft_%zu = psrt_array_data(&psv_%zu);\n"
               "const unsigned char *psright_%zu = psrt_array_data(&psv_%zu);\n",
            id, receiver, id, candidate);
        out(e, "for(size_t psindex_%zu=0; psv_%zu && "
               "psindex_%zu<psrt_array_count(&psv_%zu); psindex_%zu++) {\nif(!(",
            id, id, id, candidate, id);
        char left[128], right[128];
        snprintf(left, sizeof left, "psleft_%zu + psindex_%zu * psv_%zu.type->size",
                 id, id, receiver);
        snprintf(right, sizeof right, "psright_%zu + psindex_%zu * psv_%zu.type->size",
                 id, id, candidate);
        equal_value(e, element, left, right);
        out(e, ")) psv_%zu = false;\n}\n", id);
        return;
    }
    if (fn == PS_LANG_BUILTIN_STRING_REPLACING) {
        size_t receiver = e->nodes[n->a].a;
        size_t search = e->nodes[n->b].a;
        size_t replacement = e->nodes[e->nodes[n->b].next].a;
        expression(e, receiver);
        expression(e, search);
        expression(e, replacement);
        out(e, "psrt_string psv_%zu; psstring_check(psrt_string_replace("
               "&psv_%zu,&psv_%zu,&psv_%zu,psrt_memory_allocator(&%s),&psv_%zu)",
            id, receiver, search, replacement,
            e->experiment ? "psstate->memory" : "psmemory", id);
        checked_end(e, id);
        return;
    }
    if (fn == PS_LANG_BUILTIN_STRING_CONTAINS ||
        fn == PS_LANG_BUILTIN_STRING_FIRST_INDEX ||
        fn == PS_LANG_BUILTIN_STRING_LAST_INDEX ||
        fn == PS_LANG_BUILTIN_STRING_HAS_PREFIX ||
        fn == PS_LANG_BUILTIN_STRING_HAS_SUFFIX) {
        size_t receiver = e->nodes[n->a].a, value = e->nodes[n->b].a;
        expression(e, receiver);
        expression(e, value);
        if (fn == PS_LANG_BUILTIN_STRING_HAS_PREFIX ||
            fn == PS_LANG_BUILTIN_STRING_HAS_SUFFIX) {
            out(e, "bool psfound_%zu;\n", id);
            out(e, "psstring_check(psrt_string_matches_edge(&psv_%zu,&psv_%zu,%s,"
                   "&psfound_%zu)", receiver, value,
                fn == PS_LANG_BUILTIN_STRING_HAS_SUFFIX ? "true" : "false", id);
        } else {
            out(e, "bool psfound_%zu; int64_t psindex_%zu = 0;\n", id, id);
            out(e, "psstring_check(%s(&psv_%zu,&psv_%zu,"
                   "&psfound_%zu,&psindex_%zu)",
                fn == PS_LANG_BUILTIN_STRING_LAST_INDEX ? "psrt_string_find_last"
                                                        : "psrt_string_find",
                receiver, value, id, id);
        }
        checked_end(e, id);
        if (fn != PS_LANG_BUILTIN_STRING_FIRST_INDEX &&
            fn != PS_LANG_BUILTIN_STRING_LAST_INDEX)
            out(e, "bool psv_%zu = psfound_%zu;\n", id, id);
        else {
            optional_init(e, id);
            out(e, "if (psfound_%zu) {\n", id);
            out(e, "psvalue_check(psrt_array_build_begin(&psv_%zu,1)", id);
            checked_end(e, id);
            out(e, "psvalue_check(psrt_array_copy_one(psv_%zu.block,&psindex_%zu)", id, id);
            checked_end(e, id);
            out(e, "}\n");
        }
        return;
    }
    if (fn == PS_LANG_BUILTIN_STRING_MAP ||
        fn == PS_LANG_BUILTIN_STRING_COMPACT_MAP ||
        fn == PS_LANG_BUILTIN_STRING_FLAT_MAP) {
        int compact = fn == PS_LANG_BUILTIN_STRING_COMPACT_MAP;
        int flattening = fn == PS_LANG_BUILTIN_STRING_FLAT_MAP;
        size_t receiver = e->nodes[n->a].a, transform = e->nodes[n->b].a;
        ps_lang_type mapped = e->info[e->info[id].type - PS_TYPE_ARRAY_BASE].array_element;
        ps_lang_type transform_result = compact || flattening
            ? e->info[e->info[transform].type - PS_TYPE_FUNCTION_BASE].function_result
            : mapped;
        expression(e, receiver);
        expression(e, transform);
        out(e, "psrt_array psv_%zu; psvalue_check(psrt_array_init(", id);
        descriptor(e, mapped);
        out(e, ",psrt_memory_allocator(&%s),0,&psv_%zu)",
            e->experiment ? "psstate->memory" : "psmemory", id);
        checked_end(e, id);
        register_temp(e, id);
        out(e, "size_t pscursor_%zu=0;\n"
               "size_t psbyte_count_%zu=psrt_string_byte_count(&psv_%zu);\n",
            id, id, receiver);
        out(e, "while(pscursor_%zu<psbyte_count_%zu) {\n"
               "psrt_string psitem_%zu;\n", id, id, id);
        out(e, "psstring_check(psrt_string_next_scalar(&psv_%zu,&pscursor_%zu,"
               "psrt_memory_allocator(&%s),&psitem_%zu)",
            receiver, id, e->experiment ? "psstate->memory" : "psmemory", id);
        checked_end(e, id);
        out(e, "psrt_cleanup *psitem_mark_%zu=psrt_cleanups;\n"
               "psrt_cleanup psitem_owner_%zu; "
               "psrt_cleanup_push(&psitem_owner_%zu,&psitem_%zu,"
               "psrt_string_destroy);\n", id, id, id, id);
        out(e, "%s psmapped_%zu={0};\nswitch(psv_%zu.tag) {\n",
            type(e, transform_result), id, transform);
        for (size_t target = 1; target < e->node_count; target++) {
            if (!compact && !flattening && e->nodes[target].kind == PS_AST_CASE &&
                e->info[target].function_type == e->info[transform].type) {
                size_t field = e->nodes[target].a;
                out(e, "case %zu: psmapped_%zu.tag=INT64_C(%zu); "
                       "psmapped_%zu.psdata.pscase_%zu.psfield_%zu=psitem_%zu; ",
                    target, id, target, id, target, field, id);
                if (owns(e, mapped)) {
                    keeper(e, mapped);
                    out(e, "(&psmapped_%zu, ", id);
                    site(e, id);
                    out(e, "); ");
                }
                out(e, "break;\n");
                continue;
            }
            if ((e->nodes[target].kind != PS_AST_FUNCTION &&
                 e->nodes[target].kind != PS_AST_LAMBDA &&
                 e->nodes[target].kind != PS_AST_LOCAL_FUNCTION) ||
                e->info[target].function_type != e->info[transform].type)
                continue;
            int bound = e->info[target].method_owner && !e->info[target].method_static;
            int lambda = e->nodes[target].kind == PS_AST_LAMBDA;
            int local = e->nodes[target].kind == PS_AST_LOCAL_FUNCTION;
            out(e, "case %zu: ", target);
            if (bound || ((lambda || local) && e->info[target].capture_head)) {
                out(e, "if(psrt_array_count(&psv_%zu.receiver)!=1) psrt_fail(", transform);
                site(e, id);
                out(e, ",\"Invalid bound method receiver\"); ");
            }
            out(e, "psmapped_%zu=psfn_%zu(", id, target);
            if (e->experiment)
                out(e, "psstate, ");
            if (local)
                out(e, "&psv_%zu, ", transform);
            else if (lambda)
                out(e, "(const pscapture_%zu *)psrt_array_data(&psv_%zu.receiver), ",
                    target, transform);
            else if (bound) {
                size_t self = e->nodes[target].a;
                if (e->info[target].method_mutating)
                    out(e, "(%s *)psrt_function_mutable_receiver(&psv_%zu), ",
                        type(e, e->info[self].type), transform);
                else
                    out(e, "*(const %s *)psrt_array_data(&psv_%zu.receiver), ",
                        type(e, e->info[self].type), transform);
            }
            out(e, "psitem_%zu); break;\n", id);
        }
        out(e, "default: psrt_fail(");
        site(e, id);
        out(e, ",\"Invalid function value\");\n}\n");
        if (owns(e, transform_result)) {
            out(e, "psrt_cleanup *psmap_mark_%zu=psrt_cleanups;\n"
                   "psrt_cleanup psmap_owner_%zu; "
                   "psrt_cleanup_push(&psmap_owner_%zu,&psmapped_%zu,",
                id, id, id, id);
            destroyer(e, transform_result);
            out(e, ");\n");
        }
        if (compact || flattening)
            out(e, "if(psrt_array_count(&psmapped_%zu)) {\n", id);
        if (compact || flattening) {
            out(e, "psvalue_check(psrt_array_builder_append(&psv_%zu,"
                   "psrt_array_data(&psmapped_%zu),", id, id);
            if (flattening)
                out(e, "psrt_array_count(&psmapped_%zu))", id);
            else
                out(e, "1)");
        } else
            out(e, "psvalue_check(psrt_array_builder_append(&psv_%zu,&psmapped_%zu,1)",
                id, id);
        checked_end(e, id);
        if (compact || flattening)
            out(e, "}\n");
        if (owns(e, transform_result))
            out(e, "psrt_cleanup_unwind(psmap_mark_%zu);\n", id);
        out(e, "psrt_cleanup_unwind(psitem_mark_%zu);\n}\n", id);
        return;
    }
    if (fn == PS_LANG_BUILTIN_STRING_FIRST_WHERE ||
        fn == PS_LANG_BUILTIN_STRING_LAST_WHERE ||
        fn == PS_LANG_BUILTIN_STRING_FIRST_INDEX_WHERE ||
        fn == PS_LANG_BUILTIN_STRING_LAST_INDEX_WHERE) {
        int reverse = fn == PS_LANG_BUILTIN_STRING_LAST_WHERE ||
                      fn == PS_LANG_BUILTIN_STRING_LAST_INDEX_WHERE;
        int indexed = fn == PS_LANG_BUILTIN_STRING_FIRST_INDEX_WHERE ||
                      fn == PS_LANG_BUILTIN_STRING_LAST_INDEX_WHERE;
        size_t receiver = e->nodes[n->a].a, predicate = e->nodes[n->b].a;
        expression(e, receiver);
        expression(e, predicate);
        optional_init(e, id);
        out(e, "size_t psbyte_count_%zu=psrt_string_byte_count(&psv_%zu);\n",
            id, receiver);
        if (reverse)
            out(e, "size_t pscursor_%zu=psbyte_count_%zu;\n"
                   "size_t psindex_%zu=psrt_string_scalar_count(&psv_%zu);\n",
                id, id, id, receiver);
        else
            out(e, "size_t pscursor_%zu=0;\nsize_t psindex_%zu=0;\n", id, id);
        if (reverse)
            out(e, "while(pscursor_%zu>0) {\npsindex_%zu--;\n", id, id);
        else
            out(e, "while(pscursor_%zu<psbyte_count_%zu) {\n", id, id);
        out(e, "psrt_string psitem_%zu;\n", id);
        out(e, "psstring_check(psrt_string_%s_scalar(&psv_%zu,&pscursor_%zu,"
               "psrt_memory_allocator(&%s),&psitem_%zu)",
            reverse ? "previous" : "next", receiver, id,
            e->experiment ? "psstate->memory" : "psmemory", id);
        checked_end(e, id);
        out(e, "psrt_cleanup *psitem_mark_%zu=psrt_cleanups;\n"
               "psrt_cleanup psitem_owner_%zu; "
               "psrt_cleanup_push(&psitem_owner_%zu,&psitem_%zu,"
               "psrt_string_destroy);\n", id, id, id, id);
        out(e, "bool psaccepted_%zu=false;\nswitch(psv_%zu.tag) {\n", id, predicate);
        for (size_t target = 1; target < e->node_count; target++) {
            if ((e->nodes[target].kind != PS_AST_FUNCTION &&
                 e->nodes[target].kind != PS_AST_LAMBDA &&
                 e->nodes[target].kind != PS_AST_LOCAL_FUNCTION) ||
                e->info[target].function_type != e->info[predicate].type)
                continue;
            int bound = e->info[target].method_owner && !e->info[target].method_static;
            int lambda = e->nodes[target].kind == PS_AST_LAMBDA;
            int local = e->nodes[target].kind == PS_AST_LOCAL_FUNCTION;
            out(e, "case %zu: ", target);
            if (bound || ((lambda || local) && e->info[target].capture_head)) {
                out(e, "if(psrt_array_count(&psv_%zu.receiver)!=1) psrt_fail(", predicate);
                site(e, id);
                out(e, ",\"Invalid bound method receiver\"); ");
            }
            out(e, "psaccepted_%zu=psfn_%zu(", id, target);
            if (e->experiment)
                out(e, "psstate, ");
            if (local)
                out(e, "&psv_%zu, ", predicate);
            else if (lambda)
                out(e, "(const pscapture_%zu *)psrt_array_data(&psv_%zu.receiver), ",
                    target, predicate);
            else if (bound) {
                size_t self = e->nodes[target].a;
                if (e->info[target].method_mutating)
                    out(e, "(%s *)psrt_function_mutable_receiver(&psv_%zu), ",
                        type(e, e->info[self].type), predicate);
                else
                    out(e, "*(const %s *)psrt_array_data(&psv_%zu.receiver), ",
                        type(e, e->info[self].type), predicate);
            }
            out(e, "psitem_%zu); break;\n", id);
        }
        out(e, "default: psrt_fail(");
        site(e, id);
        out(e, ",\"Invalid function value\");\n}\n");
        out(e, "if(psaccepted_%zu) {\n", id);
        out(e, "psvalue_check(psrt_array_build_begin(&psv_%zu,1)", id);
        checked_end(e, id);
        if (indexed) {
            out(e, "int64_t psfound_%zu=(int64_t)psindex_%zu;\n", id, id);
            out(e, "psvalue_check(psrt_array_copy_one(psv_%zu.block,&psfound_%zu)",
                id, id);
        } else
            out(e, "psvalue_check(psrt_array_copy_one(psv_%zu.block,&psitem_%zu)",
                id, id);
        checked_end(e, id);
        out(e, "psrt_cleanup_unwind(psitem_mark_%zu);\nbreak;\n}\n", id);
        out(e, "psrt_cleanup_unwind(psitem_mark_%zu);\n", id);
        if (!reverse)
            out(e, "psindex_%zu++;\n", id);
        out(e, "}\n");
        return;
    }
    if (fn == PS_LANG_BUILTIN_STRING_PREFIX_WHILE ||
        fn == PS_LANG_BUILTIN_STRING_DROP_WHILE ||
        fn == PS_LANG_BUILTIN_STRING_FILTER ||
        fn == PS_LANG_BUILTIN_STRING_ANY ||
        fn == PS_LANG_BUILTIN_STRING_ALL) {
        size_t receiver = e->nodes[n->a].a, predicate = e->nodes[n->b].a;
        expression(e, receiver);
        expression(e, predicate);
        if (fn == PS_LANG_BUILTIN_STRING_FILTER) {
            out(e, "psrt_string psv_%zu; psvalue_check(psrt_array_init("
                   "&psrt_string_byte_type,psrt_memory_allocator(&%s),0,&psv_%zu)",
                id, e->experiment ? "psstate->memory" : "psmemory", id);
            checked_end(e, id);
            register_temp(e, id);
        } else if (fn == PS_LANG_BUILTIN_STRING_ANY ||
                   fn == PS_LANG_BUILTIN_STRING_ALL)
            out(e, "bool psv_%zu=%s;\n", id,
                fn == PS_LANG_BUILTIN_STRING_ALL ? "true" : "false");
        out(e, "size_t psbyte_count_%zu=psrt_string_byte_count(&psv_%zu);\n"
               "size_t pscursor_%zu=0;\n", id, receiver, id);
        out(e, "while(pscursor_%zu<psbyte_count_%zu) {\n", id, id);
        out(e, "size_t psstart_%zu=pscursor_%zu;\npsrt_string psitem_%zu;\n",
            id, id, id);
        out(e, "psstring_check(psrt_string_next_scalar(&psv_%zu,&pscursor_%zu,"
               "psrt_memory_allocator(&%s),&psitem_%zu)",
            receiver, id, e->experiment ? "psstate->memory" : "psmemory", id);
        checked_end(e, id);
        out(e, "psrt_cleanup *psitem_mark_%zu=psrt_cleanups;\n"
               "psrt_cleanup psitem_owner_%zu; "
               "psrt_cleanup_push(&psitem_owner_%zu,&psitem_%zu,"
               "psrt_string_destroy);\n", id, id, id, id);
        out(e, "bool psaccepted_%zu=false;\nswitch(psv_%zu.tag) {\n", id, predicate);
        for (size_t target = 1; target < e->node_count; target++) {
            if ((e->nodes[target].kind != PS_AST_FUNCTION &&
                 e->nodes[target].kind != PS_AST_LAMBDA &&
                 e->nodes[target].kind != PS_AST_LOCAL_FUNCTION) ||
                e->info[target].function_type != e->info[predicate].type)
                continue;
            int bound = e->info[target].method_owner && !e->info[target].method_static;
            int lambda = e->nodes[target].kind == PS_AST_LAMBDA;
            int local = e->nodes[target].kind == PS_AST_LOCAL_FUNCTION;
            out(e, "case %zu: ", target);
            if (bound || ((lambda || local) && e->info[target].capture_head)) {
                out(e, "if(psrt_array_count(&psv_%zu.receiver)!=1) psrt_fail(", predicate);
                site(e, id);
                out(e, ",\"Invalid bound method receiver\"); ");
            }
            out(e, "psaccepted_%zu=psfn_%zu(", id, target);
            if (e->experiment)
                out(e, "psstate, ");
            if (local)
                out(e, "&psv_%zu, ", predicate);
            else if (lambda)
                out(e, "(const pscapture_%zu *)psrt_array_data(&psv_%zu.receiver), ",
                    target, predicate);
            else if (bound) {
                size_t self = e->nodes[target].a;
                if (e->info[target].method_mutating)
                    out(e, "(%s *)psrt_function_mutable_receiver(&psv_%zu), ",
                        type(e, e->info[self].type), predicate);
                else
                    out(e, "*(const %s *)psrt_array_data(&psv_%zu.receiver), ",
                        type(e, e->info[self].type), predicate);
            }
            out(e, "psitem_%zu); break;\n", id);
        }
        out(e, "default: psrt_fail(");
        site(e, id);
        out(e, ",\"Invalid function value\");\n}\n");
        out(e, "psrt_cleanup_unwind(psitem_mark_%zu);\n", id);
        if (fn == PS_LANG_BUILTIN_STRING_ANY ||
            fn == PS_LANG_BUILTIN_STRING_ALL) {
            out(e, "if(psaccepted_%zu) {", id);
            if (fn == PS_LANG_BUILTIN_STRING_ANY)
                out(e, "psv_%zu=true; break;", id);
            out(e, "}\n");
            if (fn == PS_LANG_BUILTIN_STRING_ALL)
                out(e, "else {psv_%zu=false; break;}\n", id);
            out(e, "}\n");
            return;
        }
        if (fn == PS_LANG_BUILTIN_STRING_FILTER) {
            out(e, "if(psaccepted_%zu) { psvalue_check(psrt_array_builder_append("
                   "&psv_%zu,(const unsigned char *)psrt_array_data(&psv_%zu)"
                   "+psstart_%zu,pscursor_%zu-psstart_%zu)",
                id, id, receiver, id, id, id);
            checked_end(e, id);
            out(e, "}\n");
        } else
            out(e, "if(!psaccepted_%zu) { pscursor_%zu=psstart_%zu; break; }\n",
                id, id, id);
        if (fn == PS_LANG_BUILTIN_STRING_FILTER) {
            out(e, "}\nunsigned char psterminator_%zu=0;\n"
                   "psvalue_check(psrt_array_builder_append(&psv_%zu,"
                   "&psterminator_%zu,1)", id, id, id);
            checked_end(e, id);
            return;
        }
        out(e, "}\npsrt_string psv_%zu; psstring_check(psrt_string_make("
               "psrt_memory_allocator(&%s),",
            id, e->experiment ? "psstate->memory" : "psmemory");
        if (fn == PS_LANG_BUILTIN_STRING_PREFIX_WHILE)
            out(e, "psrt_array_data(&psv_%zu),pscursor_%zu", receiver, id);
        else
            out(e, "(const unsigned char *)psrt_array_data(&psv_%zu)+pscursor_%zu,"
                   "psbyte_count_%zu-pscursor_%zu", receiver, id, id, id);
        out(e, ",&psv_%zu)", id);
        checked_end(e, id);
        return;
    }
    if (fn == PS_LANG_BUILTIN_ARRAY_FOR_EACH ||
        fn == PS_LANG_BUILTIN_STRING_FOR_EACH) {
        int string = fn == PS_LANG_BUILTIN_STRING_FOR_EACH;
        size_t receiver = e->nodes[n->a].a;
        size_t action = e->nodes[n->b].a;
        ps_lang_type element = string ? PS_TYPE_STRING
            : e->info[e->info[receiver].type - PS_TYPE_ARRAY_BASE].array_element;
        expression(e, receiver);
        expression(e, action);
        if (string) {
            out(e, "size_t pscursor_%zu=0;\n"
                   "size_t psbyte_count_%zu=psrt_string_byte_count(&psv_%zu);\n"
                   "while(pscursor_%zu<psbyte_count_%zu) {\n"
                   "psrt_string psitem_%zu;\n",
                id, id, receiver, id, id, id);
            out(e, "psstring_check(psrt_string_next_scalar(&psv_%zu,&pscursor_%zu,"
                   "psrt_memory_allocator(&%s),&psitem_%zu)",
                receiver, id, e->experiment ? "psstate->memory" : "psmemory", id);
            checked_end(e, id);
            out(e, "psrt_cleanup *psitem_mark_%zu=psrt_cleanups;\n"
                   "psrt_cleanup psitem_owner_%zu; "
                   "psrt_cleanup_push(&psitem_owner_%zu,&psitem_%zu,"
                   "psrt_string_destroy);\n", id, id, id, id);
        } else {
            out(e, "for(size_t psindex_%zu=0;psindex_%zu<psrt_array_count(&psv_%zu);"
                   "psindex_%zu++) {\n", id, id, receiver, id);
            out(e, "%s psitem_%zu; memcpy(&psitem_%zu,"
                   "(const unsigned char *)psrt_array_data(&psv_%zu)+"
                   "psindex_%zu*psv_%zu.type->size,sizeof psitem_%zu);\n",
                type(e, element), id, id, receiver, id, receiver, id);
        }
        out(e, "switch(psv_%zu.tag) {\n", action);
        for (size_t target = 1; target < e->node_count; target++) {
            if ((e->nodes[target].kind != PS_AST_FUNCTION &&
                 e->nodes[target].kind != PS_AST_LAMBDA &&
                 e->nodes[target].kind != PS_AST_LOCAL_FUNCTION) ||
                e->info[target].function_type != e->info[action].type)
                continue;
            int bound = e->info[target].method_owner && !e->info[target].method_static;
            int lambda = e->nodes[target].kind == PS_AST_LAMBDA;
            int local = e->nodes[target].kind == PS_AST_LOCAL_FUNCTION;
            out(e, "case %zu: ", target);
            if (bound || ((lambda || local) && e->info[target].capture_head)) {
                out(e, "if(psrt_array_count(&psv_%zu.receiver)!=1) psrt_fail(", action);
                site(e, id);
                out(e, ",\"Invalid bound method receiver\"); ");
            }
            out(e, "psfn_%zu(", target);
            if (e->experiment)
                out(e, "psstate, ");
            if (local)
                out(e, "&psv_%zu, ", action);
            else if (lambda)
                out(e, "(const pscapture_%zu *)psrt_array_data(&psv_%zu.receiver), ",
                    target, action);
            else if (bound) {
                size_t self = e->nodes[target].a;
                if (e->info[target].method_mutating)
                    out(e, "(%s *)psrt_function_mutable_receiver(&psv_%zu), ",
                        type(e, e->info[self].type), action);
                else
                    out(e, "*(const %s *)psrt_array_data(&psv_%zu.receiver), ",
                        type(e, e->info[self].type), action);
            }
            out(e, "psitem_%zu); break;\n", id);
        }
        out(e, "default: psrt_fail(");
        site(e, id);
        out(e, ",\"Invalid function value\");\n}\n");
        if (string)
            out(e, "psrt_cleanup_unwind(psitem_mark_%zu);\n", id);
        out(e, "}\n");
        return;
    }
    if (fn == PS_LANG_BUILTIN_ARRAY_REDUCE ||
        fn == PS_LANG_BUILTIN_STRING_REDUCE) {
        int string = fn == PS_LANG_BUILTIN_STRING_REDUCE;
        size_t receiver = e->nodes[n->a].a;
        size_t initial = e->nodes[n->b].a;
        size_t combine = e->nodes[e->nodes[n->b].next].a;
        ps_lang_type array = e->info[receiver].type;
        ps_lang_type element = string ? PS_TYPE_STRING
            : e->info[array - PS_TYPE_ARRAY_BASE].array_element;
        ps_lang_type accumulator = e->info[id].type;
        expression(e, receiver);
        expression(e, initial);
        expression(e, combine);
        out(e, "%s psacc_%zu=psv_%zu;\n", type(e, accumulator), id, initial);
        if (owns(e, accumulator)) {
            keeper(e, accumulator);
            out(e, "(&psacc_%zu, ", id);
            site(e, id);
            out(e, ");\npsrt_cleanup psacc_owner_%zu; "
                   "psrt_cleanup_push(&psacc_owner_%zu,&psacc_%zu,", id, id, id);
            destroyer(e, accumulator);
            out(e, ");\n");
        }
        if (string) {
            out(e, "size_t pscursor_%zu=0;\n"
                   "size_t psbyte_count_%zu=psrt_string_byte_count(&psv_%zu);\n"
                   "while(pscursor_%zu<psbyte_count_%zu) {\n"
                   "psrt_string psitem_%zu;\n",
                id, id, receiver, id, id, id);
            out(e, "psstring_check(psrt_string_next_scalar(&psv_%zu,&pscursor_%zu,"
                   "psrt_memory_allocator(&%s),&psitem_%zu)",
                receiver, id, e->experiment ? "psstate->memory" : "psmemory", id);
            checked_end(e, id);
            out(e, "psrt_cleanup *psitem_mark_%zu=psrt_cleanups;\n"
                   "psrt_cleanup psitem_owner_%zu; "
                   "psrt_cleanup_push(&psitem_owner_%zu,&psitem_%zu,"
                   "psrt_string_destroy);\n", id, id, id, id);
        } else {
            out(e, "for(size_t psindex_%zu=0;psindex_%zu<psrt_array_count(&psv_%zu);"
                   "psindex_%zu++) {\n", id, id, receiver, id);
            out(e, "%s psitem_%zu; memcpy(&psitem_%zu,"
                   "(const unsigned char *)psrt_array_data(&psv_%zu)+"
                   "psindex_%zu*psv_%zu.type->size,sizeof psitem_%zu);\n",
                type(e, element), id, id, receiver, id, receiver, id);
        }
        out(e, "%s psnext_%zu={0}; switch(psv_%zu.tag) {\n",
            type(e, accumulator), id, combine);
        for (size_t target = 1; target < e->node_count; target++) {
            if ((e->nodes[target].kind != PS_AST_FUNCTION &&
                 e->nodes[target].kind != PS_AST_LAMBDA &&
                 e->nodes[target].kind != PS_AST_LOCAL_FUNCTION) ||
                e->info[target].function_type != e->info[combine].type)
                continue;
            int bound = e->info[target].method_owner && !e->info[target].method_static;
            int lambda = e->nodes[target].kind == PS_AST_LAMBDA;
            int local = e->nodes[target].kind == PS_AST_LOCAL_FUNCTION;
            out(e, "case %zu: ", target);
            if (bound || ((lambda || local) && e->info[target].capture_head)) {
                out(e, "if(psrt_array_count(&psv_%zu.receiver)!=1) psrt_fail(", combine);
                site(e, id);
                out(e, ",\"Invalid bound method receiver\"); ");
            }
            out(e, "psnext_%zu=psfn_%zu(", id, target);
            if (e->experiment)
                out(e, "psstate, ");
            if (local)
                out(e, "&psv_%zu, ", combine);
            else if (lambda)
                out(e, "(const pscapture_%zu *)psrt_array_data(&psv_%zu.receiver), ",
                    target, combine);
            else if (bound) {
                size_t self = e->nodes[target].a;
                if (e->info[target].method_mutating)
                    out(e, "(%s *)psrt_function_mutable_receiver(&psv_%zu), ",
                        type(e, e->info[self].type), combine);
                else
                    out(e, "*(const %s *)psrt_array_data(&psv_%zu.receiver), ",
                        type(e, e->info[self].type), combine);
            }
            out(e, "psacc_%zu,psitem_%zu); break;\n", id, id);
        }
        out(e, "default: psrt_fail(");
        site(e, id);
        out(e, ",\"Invalid function value\");\n}\n");
        if (string)
            out(e, "psrt_cleanup_unwind(psitem_mark_%zu);\n", id);
        if (owns(e, accumulator)) {
            destroyer(e, accumulator);
            out(e, "(&psacc_%zu);\n", id);
        }
        out(e, "psacc_%zu=psnext_%zu;\n}\n", id, id);
        out(e, "%s psv_%zu=psacc_%zu;\n", type(e, accumulator), id, id);
        if (owns(e, accumulator)) {
            out(e, "memset(&psacc_%zu,0,sizeof psacc_%zu);\n", id, id);
            register_temp(e, id);
        }
        return;
    }
    if (fn == PS_LANG_BUILTIN_ARRAY_REMOVE_ALL) {
        size_t receiver = e->nodes[n->a].a;
        expression(e, receiver);
        out(e, "psvalue_check(psrt_array_replace(&psv_%zu,0,"
               "psrt_array_count(&psv_%zu),NULL,0)", receiver, receiver);
        checked_end(e, id);
        writeback(e, id, receiver, receiver);
        return;
    }
    if (fn == PS_LANG_BUILTIN_ARRAY_FILTER || fn == PS_LANG_BUILTIN_ARRAY_MAP ||
        fn == PS_LANG_BUILTIN_ARRAY_COMPACT_MAP ||
        fn == PS_LANG_BUILTIN_ARRAY_FLAT_MAP ||
        fn == PS_LANG_BUILTIN_ARRAY_PREFIX_WHILE ||
        fn == PS_LANG_BUILTIN_ARRAY_DROP_WHILE ||
        fn == PS_LANG_BUILTIN_ARRAY_REMOVE_ALL_WHERE ||
        fn == PS_LANG_BUILTIN_ARRAY_ANY || fn == PS_LANG_BUILTIN_ARRAY_ALL ||
        fn == PS_LANG_BUILTIN_ARRAY_FIRST_WHERE ||
        fn == PS_LANG_BUILTIN_ARRAY_LAST_WHERE ||
        fn == PS_LANG_BUILTIN_ARRAY_FIRST_INDEX_WHERE ||
        fn == PS_LANG_BUILTIN_ARRAY_LAST_INDEX_WHERE) {
        int mapping = fn == PS_LANG_BUILTIN_ARRAY_MAP;
        int compact = fn == PS_LANG_BUILTIN_ARRAY_COMPACT_MAP;
        int flattening = fn == PS_LANG_BUILTIN_ARRAY_FLAT_MAP;
        int prefixing = fn == PS_LANG_BUILTIN_ARRAY_PREFIX_WHILE;
        int dropping = fn == PS_LANG_BUILTIN_ARRAY_DROP_WHILE;
        int transforming = mapping || compact || flattening;
        int removing = fn == PS_LANG_BUILTIN_ARRAY_REMOVE_ALL_WHERE;
        int quantified = fn == PS_LANG_BUILTIN_ARRAY_ANY || fn == PS_LANG_BUILTIN_ARRAY_ALL;
        int all = fn == PS_LANG_BUILTIN_ARRAY_ALL;
        int indexed = fn == PS_LANG_BUILTIN_ARRAY_FIRST_INDEX_WHERE ||
                      fn == PS_LANG_BUILTIN_ARRAY_LAST_INDEX_WHERE;
        int searched = fn == PS_LANG_BUILTIN_ARRAY_FIRST_WHERE ||
                       fn == PS_LANG_BUILTIN_ARRAY_LAST_WHERE || indexed;
        int reverse = fn == PS_LANG_BUILTIN_ARRAY_LAST_WHERE ||
                      fn == PS_LANG_BUILTIN_ARRAY_LAST_INDEX_WHERE;
        size_t receiver = e->nodes[n->a].a, predicate = e->nodes[n->b].a;
        ps_lang_type array = e->info[receiver].type;
        ps_lang_type element = e->info[array - PS_TYPE_ARRAY_BASE].array_element;
        ps_lang_type transform_result = transforming
            ? e->info[e->info[predicate].type - PS_TYPE_FUNCTION_BASE].function_result
            : PS_TYPE_NONE;
        ps_lang_type mapped = transforming
            ? e->info[e->info[id].type - PS_TYPE_ARRAY_BASE].array_element : PS_TYPE_NONE;
        expression(e, receiver);
        expression(e, predicate);
        if (quantified)
            out(e, "bool psv_%zu=%s;\n", id, all ? "true" : "false");
        else if (searched)
            optional_init(e, id);
        else if (removing) {
            out(e, "psrt_array psv_%zu={0};\n", id);
            out(e, "psrt_cleanup psown_%zu; psrt_cleanup_push(&psown_%zu,"
                   "&psv_%zu,psrt_array_destroy);\n", id, id, id);
            out(e, "psrt_cleanup *psmask_mark_%zu=psrt_cleanups;\n"
                   "psrt_sort_scratch psmask_%zu={0};\n", id, id);
            out(e, "if(psrt_array_count(&psv_%zu)) {\n", receiver);
            out(e, "psvalue_check(psrt_sort_scratch_init(&psmask_%zu,"
                   "psv_%zu.allocator,psrt_array_count(&psv_%zu),1)",
                id, receiver, receiver);
            checked_end(e, id);
            out(e, "}\npsrt_cleanup psmask_owner_%zu; "
                   "psrt_cleanup_push(&psmask_owner_%zu,&psmask_%zu,"
                   "psrt_sort_scratch_destroy);\n", id, id, id);
        }
        else {
            out(e, "psrt_array psv_%zu; psvalue_check(psrt_array_init(", id);
            if (transforming)
                descriptor(e, mapped);
            else
                out(e, "psv_%zu.type", receiver);
            out(e, ",psrt_memory_allocator(&%s),0,&psv_%zu)",
                e->experiment ? "psstate->memory" : "psmemory", id);
            checked_end(e, id);
            register_temp(e, id);
        }
        if (dropping)
            out(e, "bool psdropping_%zu=true;\n", id);
        if (reverse)
            out(e, "for(size_t psindex_%zu=psrt_array_count(&psv_%zu);"
                   "psindex_%zu!=0;) {\npsindex_%zu--;\n", id, receiver, id, id);
        else
            out(e, "for(size_t psindex_%zu=0;psindex_%zu<psrt_array_count(&psv_%zu);"
                   "psindex_%zu++) {\n", id, id, receiver, id);
        out(e, "%s psitem_%zu; memcpy(&psitem_%zu,"
               "(const unsigned char *)psrt_array_data(&psv_%zu)+"
               "psindex_%zu*psv_%zu.type->size,sizeof psitem_%zu);\n",
            type(e, element), id, id, receiver, id, receiver, id);
        if (transforming)
            out(e, "%s psmapped_%zu={0};\n",
                type(e, compact || flattening ? transform_result : mapped), id);
        else
            out(e, "bool psaccepted_%zu=false;\n", id);
        if (dropping)
            out(e, "if(psdropping_%zu) {\n", id);
        out(e, "switch(psv_%zu.tag) {\n", predicate);
        for (size_t target = 1; target < e->node_count; target++) {
            if (mapping && e->nodes[target].kind == PS_AST_CASE &&
                e->info[target].function_type == e->info[predicate].type) {
                size_t field = e->nodes[target].a;
                out(e, "case %zu: psmapped_%zu.tag=INT64_C(%zu); "
                       "psmapped_%zu.psdata.pscase_%zu.psfield_%zu=psitem_%zu; ",
                    target, id, target, id, target, field, id);
                if (owns(e, mapped)) {
                    keeper(e, mapped);
                    out(e, "(&psmapped_%zu, ", id);
                    site(e, id);
                    out(e, "); ");
                }
                out(e, "break;\n");
                continue;
            }
            if ((e->nodes[target].kind != PS_AST_FUNCTION &&
                 e->nodes[target].kind != PS_AST_LAMBDA &&
                 e->nodes[target].kind != PS_AST_LOCAL_FUNCTION) ||
                e->info[target].function_type != e->info[predicate].type)
                continue;
            int bound = e->info[target].method_owner && !e->info[target].method_static;
            int lambda = e->nodes[target].kind == PS_AST_LAMBDA;
            int local = e->nodes[target].kind == PS_AST_LOCAL_FUNCTION;
            out(e, "case %zu: ", target);
            if (bound || ((lambda || local) && e->info[target].capture_head)) {
                out(e, "if(psrt_array_count(&psv_%zu.receiver)!=1) psrt_fail(", predicate);
                site(e, id);
                out(e, ",\"Invalid bound method receiver\"); ");
            }
            out(e, "%s_%zu=psfn_%zu(", transforming ? "psmapped" : "psaccepted", id, target);
            if (e->experiment)
                out(e, "psstate, ");
            if (local)
                out(e, "&psv_%zu, ", predicate);
            else if (lambda)
                out(e, "(const pscapture_%zu *)psrt_array_data(&psv_%zu.receiver), ",
                    target, predicate);
            else if (bound) {
                size_t self = e->nodes[target].a;
                if (e->info[target].method_mutating)
                    out(e, "(%s *)psrt_function_mutable_receiver(&psv_%zu), ",
                        type(e, e->info[self].type), predicate);
                else
                    out(e, "*(const %s *)psrt_array_data(&psv_%zu.receiver), ",
                        type(e, e->info[self].type), predicate);
            }
            out(e, "psitem_%zu); break;\n", id);
        }
        out(e, "default: psrt_fail(");
        site(e, id);
        out(e, ",\"Invalid function value\");\n}\n");
        if (dropping)
            out(e, "}\n");
        if (compact || flattening || (mapping && owns(e, mapped))) {
            out(e, "psrt_cleanup *psmap_mark_%zu=psrt_cleanups;\n", id);
            out(e, "psrt_cleanup psmap_owner_%zu; psrt_cleanup_push(&psmap_owner_%zu,"
                   "&psmapped_%zu,", id, id, id);
            destroyer(e, compact || flattening ? transform_result : mapped);
            out(e, ");\n");
        }
        if (quantified)
            out(e, "if(psaccepted_%zu==%s) { psv_%zu=%s; break; }\n",
                id, all ? "false" : "true", id, all ? "false" : "true");
        else if (searched) {
            out(e, "if(psaccepted_%zu) {\n", id);
            out(e, "psvalue_check(psrt_array_build_begin(&psv_%zu,1)", id);
            checked_end(e, id);
            if (indexed) {
                out(e, "int64_t psfound_%zu=(int64_t)psindex_%zu;\n", id, id);
                out(e, "psvalue_check(psrt_array_copy_one(psv_%zu.block,&psfound_%zu)",
                    id, id);
            } else
                out(e, "psvalue_check(psrt_array_copy_one(psv_%zu.block,&psitem_%zu)",
                    id, id);
            checked_end(e, id);
            out(e, "break; }\n");
        }
        else if (removing)
            out(e, "((unsigned char *)psmask_%zu.data)[psindex_%zu]="
                   "!psaccepted_%zu;\n", id, id, id);
        else {
            if (prefixing)
                out(e, "if(!psaccepted_%zu) break;\n", id);
            if (dropping)
                out(e, "if(psdropping_%zu) { if(psaccepted_%zu) continue; "
                       "psdropping_%zu=false; }\n", id, id, id);
            if (compact || flattening)
                out(e, "if(psrt_array_count(&psmapped_%zu)) { ", id);
            else if (!mapping && !prefixing && !dropping)
                out(e, "if(psaccepted_%zu) { ", id);
            if (compact || flattening) {
                out(e, "psvalue_check(psrt_array_builder_append(&psv_%zu,"
                       "psrt_array_data(&psmapped_%zu),", id, id);
                if (flattening)
                    out(e, "psrt_array_count(&psmapped_%zu))", id);
                else
                    out(e, "1)");
            }
            else
                out(e, "psvalue_check(psrt_array_builder_append(&psv_%zu,"
                       "&%s_%zu,1)", id, mapping ? "psmapped" : "psitem", id);
            checked_end(e, id);
            if (compact || flattening)
                out(e, "}\n");
            if (compact || flattening || (mapping && owns(e, mapped)))
                out(e, "psrt_cleanup_unwind(psmap_mark_%zu);\n", id);
            if (!transforming && !prefixing && !dropping)
                out(e, "}\n");
        }
        out(e, "}\n");
        if (removing) {
            out(e, "psvalue_check(psrt_array_select_mask(&psv_%zu,"
                   "psmask_%zu.data,&psv_%zu)", receiver, id, id);
            checked_end(e, id);
            out(e, "psrt_cleanup_unwind(psmask_mark_%zu);\n", id);
            out(e, "psrt_array_destroy(&psv_%zu); psv_%zu=psv_%zu; "
                   "psv_%zu=(psrt_array){0};\n", receiver, receiver, id, id);
            writeback(e, id, receiver, receiver);
        }
        return;
    }
    if (fn == PS_LANG_BUILTIN_ARRAY_CONTAINS ||
        fn == PS_LANG_BUILTIN_ARRAY_FIRST_INDEX ||
        fn == PS_LANG_BUILTIN_ARRAY_LAST_INDEX) {
        size_t receiver = e->nodes[n->a].a, value = e->nodes[n->b].a;
        ps_lang_type array = e->info[receiver].type;
        ps_lang_type element = e->info[array - PS_TYPE_ARRAY_BASE].array_element;
        expression(e, receiver);
        expression(e, value);
        if (fn != PS_LANG_BUILTIN_ARRAY_CONTAINS)
            optional_init(e, id);
        else
            out(e, "bool psv_%zu = false;\n", id);
        out(e, "const unsigned char *psitems_%zu = psrt_array_data(&psv_%zu);\n",
            id, receiver);
        if (fn == PS_LANG_BUILTIN_ARRAY_LAST_INDEX)
            out(e, "for (size_t psindex_%zu = psrt_array_count(&psv_%zu); "
                   "psindex_%zu != 0; ) {\npsindex_%zu--;\nif (",
                id, receiver, id, id);
        else
            out(e, "for (size_t psindex_%zu = 0; "
                   "psindex_%zu < psrt_array_count(&psv_%zu); psindex_%zu++) {\nif (",
                id, id, receiver, id);
        char left[128], right[64];
        snprintf(left, sizeof left, "psitems_%zu + psindex_%zu * psv_%zu.type->size",
                 id, id, receiver);
        snprintf(right, sizeof right, "&psv_%zu", value);
        equal_value(e, element, left, right);
        if (fn != PS_LANG_BUILTIN_ARRAY_CONTAINS) {
            out(e, ") { int64_t psfound_%zu = (int64_t)psindex_%zu;\n", id, id);
            out(e, "psvalue_check(psrt_array_build_begin(&psv_%zu,1)", id);
            checked_end(e, id);
            out(e, "psvalue_check(psrt_array_copy_one(psv_%zu.block,&psfound_%zu)", id, id);
            checked_end(e, id);
            out(e, "break; }\n}\n");
        } else
            out(e, ") { psv_%zu = true; break; }\n}\n", id);
        return;
    }
    if (fn == PS_LANG_BUILTIN_ENUM_FROM_RAW) {
        size_t value = e->nodes[n->b].a;
        size_t owner = e->info[e->nodes[n->a].a].binding;
        expression(e, value);
        optional_init(e, id);
        out(e, "pst_%zu psraw_case_%zu;\nif(psraw_from_%zu(psv_%zu,&psraw_case_%zu)) {\n",
            owner, id, owner, value, id);
        out(e, "psvalue_check(psrt_array_build_begin(&psv_%zu,1)", id);
        checked_end(e, id);
        out(e, "psvalue_check(psrt_array_copy_one(psv_%zu.block,&psraw_case_%zu)", id, id);
        checked_end(e, id);
        out(e, "}\n");
        return;
    }
    if (fn < e->node_count && e->nodes[fn].kind == PS_AST_CASE) {
        for (size_t a = n->b; a; a = e->nodes[a].next)
            expression(e, e->nodes[a].a);
        out(e, "%s psv_%zu = {0};\npsv_%zu.tag = INT64_C(%zu);\n",
            type(e, e->info[id].type), id, id, fn);
        for (size_t a = n->b; a; a = e->nodes[a].next)
            out(e, "psv_%zu.psdata.pscase_%zu.psfield_%zu = psv_%zu;\n",
                id, fn, e->info[a].binding, e->nodes[a].a);
        return;
    }
    if (optional_call(fn)) {
        if (fn == PS_LANG_BUILTIN_OPTIONAL_SOME) {
            size_t value = e->nodes[n->b].a;
            expression(e, value);
            optional_init(e, id);
            out(e, "psvalue_check(psrt_array_build_begin(&psv_%zu,1)", id);
            checked_end(e, id);
            out(e, "psvalue_check(psrt_array_copy_one(psv_%zu.block,&psv_%zu)", id, value);
            checked_end(e, id);
        } else {
            size_t receiver = e->nodes[n->a].a;
            expression(e, receiver);
            if (n->b)
                expression(e, e->nodes[n->b].a);
            else {
                out(e, "if (!psrt_array_count(&psv_%zu)) psrt_fail(", receiver);
                site(e, id);
                out(e, ",\"Cannot unwrap nil optional\");\n");
            }
            out(e, "%s psv_%zu;\n", type(e, e->info[id].type), id);
            if (n->b)
                out(e, "if (!psrt_array_count(&psv_%zu)) psv_%zu=psv_%zu; else\n",
                    receiver, id, e->nodes[n->b].a);
            out(e, "memcpy(&psv_%zu,psrt_array_data(&psv_%zu),sizeof psv_%zu);\n", id, receiver, id);
            if (owns(e, e->info[id].type)) {
                retain_temp(e, id);
                register_temp(e, id);
            }
        }
        return;
    }
    if (fn == PS_LANG_BUILTIN_ATTEMPT) {
        size_t value = e->nodes[n->b].a;
        optional_init(e, id);
        out(e, "{\npsrt_cleanup *psattempt_mark_%zu = psrt_cleanups;\n"
               "psrt_try_frame psattempt_%zu;\n"
               "psattempt_%zu.previous = psrt_try_current;\n"
               "psattempt_%zu.mark = psattempt_mark_%zu;\n"
               "psattempt_%zu.depth = psrt_depth;\n"
               "psrt_try_current = &psattempt_%zu;\n"
               "if (setjmp(psattempt_%zu.jump) == 0) {\n",
            id, id, id, id, id, id, id, id);
        expression(e, value);
        out(e, "psrt_try_current = psattempt_%zu.previous;\n", id);
        out(e, "psvalue_check(psrt_array_build_begin(&psv_%zu,1)", id);
        checked_end(e, id);
        out(e, "psvalue_check(psrt_array_copy_one(psv_%zu.block,&psv_%zu)", id, value);
        checked_end(e, id);
        /* Generated temporaries are scoped to the success branch. */
        out(e, "psrt_cleanup_unwind(psattempt_mark_%zu);\n", id);
        out(e, "} else {\npsrt_try_current = psattempt_%zu.previous;\n}\n"
               "}\n", id);
        return;
    }
    if (fn == PS_LANG_BUILTIN_INT64_PARSE || fn == PS_LANG_BUILTIN_FLOAT64_PARSE) {
        size_t value = e->nodes[n->b].a;
        expression(e, value);
        optional_init(e, id);
        out(e, "%s psparsed_%zu;\nif (%s(psrt_string_cstr(&psv_%zu),&psparsed_%zu",
            fn == PS_LANG_BUILTIN_INT64_PARSE ? "int64_t" : "double", id,
            fn == PS_LANG_BUILTIN_INT64_PARSE ? "psrt_try_parse_int64"
                                               : "psrt_try_parse_float64",
            value, id);
        if (fn == PS_LANG_BUILTIN_FLOAT64_PARSE) {
            out(e, ",");
            site(e, id);
        }
        out(e, ") == PSRT_PARSE_VALID) {\n"
               "psvalue_check(psrt_array_build_begin(&psv_%zu,1)", id);
        checked_end(e, id);
        out(e, "psvalue_check(psrt_array_copy_one(psv_%zu.block,&psparsed_%zu)", id, id);
        checked_end(e, id);
        out(e, "}\n");
        return;
    }
    if (fn == PS_LANG_BUILTIN_ARRAY_POP_LAST) {
        size_t receiver = e->nodes[n->a].a;
        expression(e, receiver);
        optional_init(e, id);
        out(e, "if (psrt_array_count(&psv_%zu)) {\n"
               "size_t psremove_index_%zu = psrt_array_count(&psv_%zu)-1;\n",
            receiver, id, receiver);
        out(e, "psvalue_check(psrt_array_build_begin(&psv_%zu,1)", id);
        checked_end(e, id);
        out(e, "psvalue_check(psrt_array_copy_one(psv_%zu.block,"
               "(const unsigned char *)psrt_array_data(&psv_%zu) + "
               "psremove_index_%zu * psv_%zu.type->size)",
            id, receiver, id, receiver);
        checked_end(e, id);
        out(e, "psvalue_check(psrt_array_replace(&psv_%zu,"
               "psremove_index_%zu,1,NULL,0)", receiver, id);
        checked_end(e, id);
        writeback(e, id, receiver, receiver);
        out(e, "}\n");
        return;
    }
    if (fn == PS_LANG_BUILTIN_ARRAY_REMOVE_SUBRANGE ||
        fn == PS_LANG_BUILTIN_ARRAY_REPLACE_SUBRANGE) {
        size_t receiver = e->nodes[n->a].a, bounds_arg = n->b;
        size_t bounds = e->nodes[bounds_arg].a;
        const ps_lang_node *range = &e->nodes[bounds];
        expression(e, receiver);
        expression(e, range->a);
        expression(e, range->b);
        out(e, "size_t psrange_start_%zu, psrange_count_%zu;\n", id, id);
        out(e, "psvalue_check(psrt_array_range_bounds(&psv_%zu,", receiver);
        range_arguments(e, range);
        out(e, ",&psrange_start_%zu,&psrange_count_%zu)", id, id);
        checked_end(e, id);
        if (fn == PS_LANG_BUILTIN_ARRAY_REPLACE_SUBRANGE) {
            size_t replacement = e->nodes[e->nodes[bounds_arg].next].a;
            expression(e, replacement);
            out(e, "psvalue_check(psrt_array_replace(&psv_%zu,psrange_start_%zu,"
                   "psrange_count_%zu,psrt_array_data(&psv_%zu),psrt_array_count(&psv_%zu))",
                receiver, id, id, replacement, replacement);
        } else
            out(e, "psvalue_check(psrt_array_replace(&psv_%zu,psrange_start_%zu,"
                   "psrange_count_%zu,NULL,0)", receiver, id, id);
        checked_end(e, id);
        writeback(e, id, receiver, receiver);
        return;
    }
    if (fn == PS_LANG_BUILTIN_ARRAY_SWAP_AT) {
        size_t receiver = e->nodes[n->a].a, first_arg = n->b;
        size_t first = e->nodes[first_arg].a;
        size_t second = e->nodes[e->nodes[first_arg].next].a;
        expression(e, receiver);
        expression(e, first);
        expression(e, second);
        out(e, "psvalue_check(psrt_array_swap_at(&psv_%zu,psv_%zu,psv_%zu)",
            receiver, first, second);
        checked_end(e, id);
        writeback(e, id, receiver, receiver);
        return;
    }
    if (fn == PS_LANG_BUILTIN_ARRAY_INSERT ||
        fn == PS_LANG_BUILTIN_ARRAY_INSERT_CONTENTS) {
        size_t receiver = e->nodes[n->a].a, value_arg = n->b;
        size_t value = e->nodes[value_arg].a;
        size_t position = e->nodes[e->nodes[value_arg].next].a;
        expression(e, receiver);
        expression(e, value);
        expression(e, position);
        out(e, "psvalue_check(%s(&psv_%zu,psv_%zu,&psv_%zu)",
            fn == PS_LANG_BUILTIN_ARRAY_INSERT_CONTENTS ? "psrt_array_insert_contents"
                                                       : "psrt_array_insert",
            receiver, position, value);
        checked_end(e, id);
        writeback(e, id, receiver, receiver);
        return;
    }
    if (fn == PS_LANG_BUILTIN_STRING_SORTED_BY ||
        fn == PS_LANG_BUILTIN_ARRAY_SORTED_BY ||
        fn == PS_LANG_BUILTIN_ARRAY_SORT_BY) {
        int string = fn == PS_LANG_BUILTIN_STRING_SORTED_BY;
        size_t receiver = e->nodes[n->a].a, comparator = e->nodes[n->b].a;
        ps_lang_type array = e->info[receiver].type;
        ps_lang_type element = string ? PS_TYPE_STRING
            : e->info[array - PS_TYPE_ARRAY_BASE].array_element;
        expression(e, receiver);
        expression(e, comparator);
        if (string)
            scalar_array_receiver(e, receiver, id);
        out(e, "psrt_array psv_%zu={0};\n", id);
        out(e, "psrt_cleanup psown_%zu; psrt_cleanup_push(&psown_%zu,&psv_%zu,"
               "psrt_array_destroy);\n", id, id, id);
        out(e, "psvalue_check(psrt_array_copy_elements(&psv_%zu,&psv_%zu)",
            receiver, id);
        checked_end(e, id);
        out(e, "size_t pssort_count_%zu=psrt_array_count(&psv_%zu);\n"
               "if(pssort_count_%zu>1) {\n", id, id, id);
        out(e, "psrt_cleanup *pssort_mark_%zu=psrt_cleanups;\n"
               "psrt_sort_scratch pssort_scratch_%zu={0};\n", id, id);
        out(e, "psvalue_check(psrt_sort_scratch_init(&pssort_scratch_%zu,"
               "psv_%zu.allocator,pssort_count_%zu,psv_%zu.type->size)",
            id, id, id, id);
        checked_end(e, id);
        out(e, "psrt_cleanup pssort_owner_%zu; psrt_cleanup_push(&pssort_owner_%zu,"
               "&pssort_scratch_%zu,psrt_sort_scratch_destroy);\n", id, id, id);
        out(e, "unsigned char *pssort_data_%zu=(unsigned char *)(void *)"
               "psrt_array_data(&psv_%zu);\n"
               "unsigned char *pssort_temp_%zu=pssort_scratch_%zu.data;\n"
               "size_t pssort_stride_%zu=psv_%zu.type->size;\n",
            id, id, id, id, id, id);
        out(e, "for(size_t pssort_width_%zu=1;pssort_width_%zu<pssort_count_%zu;) {\n",
            id, id, id);
        out(e, "for(size_t pssort_base_%zu=0;pssort_base_%zu<pssort_count_%zu;) {\n",
            id, id, id);
        out(e, "size_t pssort_mid_%zu=pssort_base_%zu+"
               "(pssort_width_%zu<pssort_count_%zu-pssort_base_%zu ? "
               "pssort_width_%zu : pssort_count_%zu-pssort_base_%zu);\n",
            id, id, id, id, id, id, id, id);
        out(e, "size_t pssort_end_%zu=pssort_mid_%zu+"
               "(pssort_width_%zu<pssort_count_%zu-pssort_mid_%zu ? "
               "pssort_width_%zu : pssort_count_%zu-pssort_mid_%zu);\n",
            id, id, id, id, id, id, id, id);
        out(e, "size_t pssort_left_%zu=pssort_base_%zu,"
               "pssort_right_%zu=pssort_mid_%zu,pssort_out_%zu=pssort_base_%zu;\n",
            id, id, id, id, id, id);
        out(e, "while(pssort_left_%zu<pssort_mid_%zu && "
               "pssort_right_%zu<pssort_end_%zu) {\n", id,id,id,id);
        out(e, "bool pssort_right_first_%zu=false;\n", id);
        out(e, "%s pssort_right_value_%zu,pssort_left_value_%zu;\n",
            type(e, element), id, id);
        out(e, "memcpy(&pssort_right_value_%zu,pssort_data_%zu+"
               "pssort_right_%zu*pssort_stride_%zu,sizeof pssort_right_value_%zu);\n"
               "memcpy(&pssort_left_value_%zu,pssort_data_%zu+"
               "pssort_left_%zu*pssort_stride_%zu,sizeof pssort_left_value_%zu);\n",
            id,id,id,id,id,id,id,id,id,id);
        out(e, "switch(psv_%zu.tag) {\n", comparator);
        for (size_t target = 1; target < e->node_count; target++) {
            if ((e->nodes[target].kind != PS_AST_FUNCTION &&
                 e->nodes[target].kind != PS_AST_LAMBDA &&
                 e->nodes[target].kind != PS_AST_LOCAL_FUNCTION) ||
                e->info[target].function_type != e->info[comparator].type)
                continue;
            int bound = e->info[target].method_owner && !e->info[target].method_static;
            int lambda = e->nodes[target].kind == PS_AST_LAMBDA;
            int local = e->nodes[target].kind == PS_AST_LOCAL_FUNCTION;
            out(e, "case %zu: ", target);
            if (bound || ((lambda || local) && e->info[target].capture_head)) {
                out(e, "if(psrt_array_count(&psv_%zu.receiver)!=1) psrt_fail(", comparator);
                site(e, id);
                out(e, ",\"Invalid bound method receiver\"); ");
            }
            out(e, "pssort_right_first_%zu=psfn_%zu(", id, target);
            if (e->experiment)
                out(e, "psstate, ");
            if (local)
                out(e, "&psv_%zu, ", comparator);
            else if (lambda)
                out(e, "(const pscapture_%zu *)psrt_array_data(&psv_%zu.receiver), ",
                    target, comparator);
            else if (bound) {
                size_t self = e->nodes[target].a;
                if (e->info[target].method_mutating)
                    out(e, "(%s *)psrt_function_mutable_receiver(&psv_%zu), ",
                        type(e, e->info[self].type), comparator);
                else
                    out(e, "*(const %s *)psrt_array_data(&psv_%zu.receiver), ",
                        type(e, e->info[self].type), comparator);
            }
            out(e, "pssort_right_value_%zu,pssort_left_value_%zu); break;\n", id, id);
        }
        out(e, "default: psrt_fail(");
        site(e, id);
        out(e, ",\"Invalid function value\");\n}\n");
        out(e, "size_t pssort_pick_%zu=pssort_right_first_%zu ? "
               "pssort_right_%zu++ : pssort_left_%zu++;\n"
               "memcpy(pssort_temp_%zu+pssort_out_%zu*pssort_stride_%zu,"
               "pssort_data_%zu+pssort_pick_%zu*pssort_stride_%zu,"
               "pssort_stride_%zu); pssort_out_%zu++;\n}\n",
            id,id,id,id,id,id,id,id,id,id,id,id);
        out(e, "while(pssort_left_%zu<pssort_mid_%zu) {\n", id, id);
        out(e, "memcpy(pssort_temp_%zu+pssort_out_%zu*pssort_stride_%zu,"
               "pssort_data_%zu+pssort_left_%zu*pssort_stride_%zu,"
               "pssort_stride_%zu);\n", id, id, id, id, id, id, id);
        out(e, "pssort_left_%zu++; pssort_out_%zu++; }\n", id, id);
        out(e, "while(pssort_right_%zu<pssort_end_%zu) {\n", id, id);
        out(e, "memcpy(pssort_temp_%zu+pssort_out_%zu*pssort_stride_%zu,"
               "pssort_data_%zu+pssort_right_%zu*pssort_stride_%zu,"
               "pssort_stride_%zu);\n", id, id, id, id, id, id, id);
        out(e, "pssort_right_%zu++; pssort_out_%zu++; }\n", id, id);
        out(e, "pssort_base_%zu=pssort_end_%zu;\n}\n", id, id);
        out(e, "memcpy(pssort_data_%zu,pssort_temp_%zu,"
               "pssort_count_%zu*pssort_stride_%zu);\n"
               "if(pssort_width_%zu>pssort_count_%zu/2) break;\n"
               "pssort_width_%zu*=2;\n}\n"
               "psrt_cleanup_unwind(pssort_mark_%zu);\n}\n",
            id,id,id,id,id,id,id,id,id);
        if (fn == PS_LANG_BUILTIN_ARRAY_SORT_BY) {
            out(e, "psrt_array_destroy(&psv_%zu); psv_%zu=psv_%zu; "
                   "psv_%zu=(psrt_array){0};\n", receiver, receiver, id, id);
            writeback(e, id, receiver, receiver);
        }
        return;
    }
    if (fn == PS_LANG_BUILTIN_ARRAY_MIN || fn == PS_LANG_BUILTIN_ARRAY_MAX ||
        fn == PS_LANG_BUILTIN_ARRAY_MIN_BY || fn == PS_LANG_BUILTIN_ARRAY_MAX_BY ||
        fn == PS_LANG_BUILTIN_STRING_MIN || fn == PS_LANG_BUILTIN_STRING_MAX ||
        fn == PS_LANG_BUILTIN_STRING_MIN_BY || fn == PS_LANG_BUILTIN_STRING_MAX_BY) {
        int string = fn == PS_LANG_BUILTIN_STRING_MIN ||
                     fn == PS_LANG_BUILTIN_STRING_MAX ||
                     fn == PS_LANG_BUILTIN_STRING_MIN_BY ||
                     fn == PS_LANG_BUILTIN_STRING_MAX_BY;
        int maximum = fn == PS_LANG_BUILTIN_ARRAY_MAX ||
                      fn == PS_LANG_BUILTIN_ARRAY_MAX_BY ||
                      fn == PS_LANG_BUILTIN_STRING_MAX ||
                      fn == PS_LANG_BUILTIN_STRING_MAX_BY;
        int custom = fn == PS_LANG_BUILTIN_ARRAY_MIN_BY ||
                     fn == PS_LANG_BUILTIN_ARRAY_MAX_BY ||
                     fn == PS_LANG_BUILTIN_STRING_MIN_BY ||
                     fn == PS_LANG_BUILTIN_STRING_MAX_BY;
        size_t receiver = e->nodes[n->a].a;
        size_t comparator = custom ? e->nodes[n->b].a : 0;
        ps_lang_type array = e->info[receiver].type;
        ps_lang_type element = string ? PS_TYPE_STRING
            : e->info[array - PS_TYPE_ARRAY_BASE].array_element;
        expression(e, receiver);
        if (custom)
            expression(e, comparator);
        if (string)
            scalar_array_receiver(e, receiver, id);
        optional_init(e, id);
        out(e, "size_t psextreme_%zu=0;\n", id);
        if (custom) {
            out(e, "for(size_t psscan_%zu=1;psscan_%zu<psrt_array_count(&psv_%zu);"
                   "psscan_%zu++) {\n", id, id, receiver, id);
            out(e, "%s pscandidate_%zu,psbest_%zu;\n", type(e, element), id, id);
            out(e, "memcpy(&pscandidate_%zu,(const unsigned char *)"
                   "psrt_array_data(&psv_%zu)+psscan_%zu*psv_%zu.type->size,"
                   "sizeof pscandidate_%zu);\n", id, receiver, id, receiver, id);
            out(e, "memcpy(&psbest_%zu,(const unsigned char *)"
                   "psrt_array_data(&psv_%zu)+psextreme_%zu*psv_%zu.type->size,"
                   "sizeof psbest_%zu);\n", id, receiver, id, receiver, id);
            out(e, "bool psbetter_%zu=false; switch(psv_%zu.tag) {\n", id, comparator);
            for (size_t target = 1; target < e->node_count; target++) {
                if ((e->nodes[target].kind != PS_AST_FUNCTION &&
                     e->nodes[target].kind != PS_AST_LAMBDA &&
                     e->nodes[target].kind != PS_AST_LOCAL_FUNCTION) ||
                    e->info[target].function_type != e->info[comparator].type)
                    continue;
                int bound = e->info[target].method_owner && !e->info[target].method_static;
                int lambda = e->nodes[target].kind == PS_AST_LAMBDA;
                int local = e->nodes[target].kind == PS_AST_LOCAL_FUNCTION;
                out(e, "case %zu: ", target);
                if (bound || ((lambda || local) && e->info[target].capture_head)) {
                    out(e, "if(psrt_array_count(&psv_%zu.receiver)!=1) psrt_fail(",
                        comparator);
                    site(e, id);
                    out(e, ",\"Invalid bound method receiver\"); ");
                }
                out(e, "psbetter_%zu=psfn_%zu(", id, target);
                if (e->experiment)
                    out(e, "psstate, ");
                if (local)
                    out(e, "&psv_%zu, ", comparator);
                else if (lambda)
                    out(e, "(const pscapture_%zu *)psrt_array_data(&psv_%zu.receiver), ",
                        target, comparator);
                else if (bound) {
                    size_t self = e->nodes[target].a;
                    if (e->info[target].method_mutating)
                        out(e, "(%s *)psrt_function_mutable_receiver(&psv_%zu), ",
                            type(e, e->info[self].type), comparator);
                    else
                        out(e, "*(const %s *)psrt_array_data(&psv_%zu.receiver), ",
                            type(e, e->info[self].type), comparator);
                }
                out(e, maximum ? "psbest_%zu,pscandidate_%zu); break;\n"
                               : "pscandidate_%zu,psbest_%zu); break;\n", id, id);
            }
            out(e, "default: psrt_fail(");
            site(e, id);
            out(e, ",\"Invalid function value\");\n}\n"
                   "if(psbetter_%zu) psextreme_%zu=psscan_%zu;\n}\n", id, id, id);
        } else {
            const char *compare = element == PS_TYPE_INT64 ? "psrt_compare_int64"
                                : element == PS_TYPE_FLOAT64 ? "psrt_compare_float64"
                                                             : "psrt_compare_string_element";
            const char *valid = element == PS_TYPE_FLOAT64 ? "psrt_finite_float64_element"
                                : element == PS_TYPE_STRING ? "psrt_valid_string_element" : "NULL";
            out(e, "psvalue_check(psrt_array_extremum_index(&psv_%zu,%s,%s,%s,"
                   "&psextreme_%zu)", receiver, compare, valid,
                maximum ? "true" : "false", id);
            checked_end(e, id);
        }
        out(e, "if(psrt_array_count(&psv_%zu)) {\n", receiver);
        out(e, "psvalue_check(psrt_array_build_begin(&psv_%zu,1)", id);
        checked_end(e, id);
        out(e, "const unsigned char *psselected_%zu=(const unsigned char *)"
               "psrt_array_data(&psv_%zu)+psextreme_%zu*psv_%zu.type->size;\n",
            id, receiver, id, receiver);
        out(e, "psvalue_check(psrt_array_copy_one(psv_%zu.block,psselected_%zu)", id, id);
        checked_end(e, id);
        out(e, "}\n");
        return;
    }
    if (fn == PS_LANG_BUILTIN_ARRAY_SORTED || fn == PS_LANG_BUILTIN_ARRAY_SORT ||
        fn == PS_LANG_BUILTIN_STRING_SORTED) {
        int string = fn == PS_LANG_BUILTIN_STRING_SORTED;
        size_t receiver = e->nodes[n->a].a;
        ps_lang_type array = e->info[receiver].type;
        ps_lang_type element = string ? PS_TYPE_STRING
            : e->info[array - PS_TYPE_ARRAY_BASE].array_element;
        const char *compare = element == PS_TYPE_INT64 ? "psrt_compare_int64"
                            : element == PS_TYPE_FLOAT64 ? "psrt_compare_float64"
                                                         : "psrt_compare_string_element";
        const char *valid = element == PS_TYPE_FLOAT64 ? "psrt_finite_float64_element"
                            : element == PS_TYPE_STRING ? "psrt_valid_string_element" : "NULL";
        expression(e, receiver);
        if (string)
            scalar_array_receiver(e, receiver, id);
        if (fn == PS_LANG_BUILTIN_ARRAY_SORTED || string) {
            out(e, "psrt_array psv_%zu = {0};\n", id);
            register_temp(e, id);
            out(e, "psvalue_check(psrt_array_sorted(&psv_%zu,%s,%s,&psv_%zu)",
                receiver, compare, valid, id);
        } else
            out(e, "psvalue_check(psrt_array_sort(&psv_%zu,%s,%s)",
                receiver, compare, valid);
        checked_end(e, id);
        if (fn == PS_LANG_BUILTIN_ARRAY_SORT)
            writeback(e, id, receiver, receiver);
        return;
    }
    if (fn == PS_LANG_BUILTIN_ARRAY_REPEATED) {
        size_t receiver = e->nodes[n->b].a;
        size_t count = e->nodes[e->nodes[n->b].next].a;
        expression(e, receiver);
        expression(e, count);
        out(e, "if(psv_%zu<0) psrt_fail(", count);
        site(e, id);
        out(e, ",\"Array repetition count must be nonnegative\");\n");
        out(e, "psrt_array psv_%zu; psvalue_check(psrt_array_init(", id);
        descriptor(e, e->info[receiver].type);
        out(e, ",psrt_memory_allocator(&%s),0,&psv_%zu)",
            e->experiment ? "psstate->memory" : "psmemory", id);
        checked_end(e, id);
        register_temp(e, id);
        out(e, "psvalue_check(psrt_array_replace(&psv_%zu,0,0,&psv_%zu,1)",
            id, receiver);
        checked_end(e, id);
        out(e, "psrt_array psrepeat_result_%zu = {0};\n", id);
        out(e, "psvalue_check(psrt_array_repeated(&psv_%zu,psv_%zu,&psrepeat_result_%zu)",
            id, count, id);
        checked_end(e, id);
        out(e, "psrt_array_destroy(&psv_%zu); psv_%zu = psrepeat_result_%zu;\n",
            id, id, id);
        return;
    }
    if (fn == PS_LANG_BUILTIN_ARRAY_PREFIX || fn == PS_LANG_BUILTIN_ARRAY_SUFFIX ||
        fn == PS_LANG_BUILTIN_ARRAY_DROP_FIRST || fn == PS_LANG_BUILTIN_ARRAY_DROP_LAST) {
        size_t receiver = e->nodes[n->a].a;
        size_t count = n->b ? e->nodes[n->b].a : 0;
        expression(e, receiver);
        if (count)
            expression(e, count);
        int string = e->info[receiver].type == PS_TYPE_STRING;
        if (string)
            out(e, "psrt_string psv_%zu; psstring_check(psrt_string_select_edge(&psv_%zu,",
                id, receiver);
        else {
            out(e, "psrt_array psv_%zu = {0};\n", id);
            register_temp(e, id);
            out(e, "psvalue_check(psrt_array_select_edge(&psv_%zu,", receiver);
        }
        if (count)
            out(e, "psv_%zu", count);
        else
            out(e, "1");
        out(e, ",%s,%s,",
            fn == PS_LANG_BUILTIN_ARRAY_DROP_FIRST ||
                    fn == PS_LANG_BUILTIN_ARRAY_DROP_LAST ? "true" : "false",
            fn == PS_LANG_BUILTIN_ARRAY_SUFFIX ||
                    fn == PS_LANG_BUILTIN_ARRAY_DROP_LAST ? "true" : "false");
        if (string)
            out(e, "psrt_memory_allocator(&%s),", e->experiment ? "psstate->memory" : "psmemory");
        out(e, "&psv_%zu)", id);
        checked_end(e, id);
        if (string)
            register_temp(e, id);
        return;
    }
    if (fn == PS_LANG_BUILTIN_ARRAY_REVERSED) {
        size_t receiver = e->nodes[n->a].a;
        expression(e, receiver);
        out(e, "psrt_array psv_%zu = {0};\n", id);
        register_temp(e, id);
        out(e, "psvalue_check(psrt_array_reversed(&psv_%zu,&psv_%zu)", receiver, id);
        checked_end(e, id);
        return;
    }
    if (fn == PS_LANG_BUILTIN_ARRAY_REVERSE) {
        size_t receiver = e->nodes[n->a].a;
        expression(e, receiver);
        out(e, "psvalue_check(psrt_array_reverse(&psv_%zu)", receiver);
        checked_end(e, id);
        writeback(e, id, receiver, receiver);
        return;
    }
    if (fn == PS_LANG_BUILTIN_ARRAY_REMOVE_FIRST_COUNT ||
        fn == PS_LANG_BUILTIN_ARRAY_REMOVE_LAST_COUNT) {
        size_t receiver = e->nodes[n->a].a, count = e->nodes[n->b].a;
        expression(e, receiver);
        expression(e, count);
        out(e, "psvalue_check(psrt_array_remove_edge_count(&psv_%zu,psv_%zu,%s)",
            receiver, count,
            fn == PS_LANG_BUILTIN_ARRAY_REMOVE_LAST_COUNT ? "true" : "false");
        checked_end(e, id);
        writeback(e, id, receiver, receiver);
        return;
    }
    if (fn == PS_LANG_BUILTIN_ARRAY_APPEND ||
        fn == PS_LANG_BUILTIN_ARRAY_APPEND_CONTENTS ||
        fn == PS_LANG_BUILTIN_ARRAY_REMOVE ||
        fn == PS_LANG_BUILTIN_ARRAY_REMOVE_FIRST ||
        fn == PS_LANG_BUILTIN_ARRAY_REMOVE_LAST) {
        size_t receiver = e->nodes[n->a].a;
        size_t value = n->b ? e->nodes[n->b].a : 0;
        expression(e, receiver);
        if (value)
            expression(e, value);
        if (fn == PS_LANG_BUILTIN_ARRAY_REMOVE ||
            fn == PS_LANG_BUILTIN_ARRAY_REMOVE_FIRST ||
            fn == PS_LANG_BUILTIN_ARRAY_REMOVE_LAST) {
            if (fn == PS_LANG_BUILTIN_ARRAY_REMOVE)
                out(e, "int64_t psremove_index_%zu=psv_%zu;\n", id, value);
            else if (fn == PS_LANG_BUILTIN_ARRAY_REMOVE_FIRST)
                out(e, "int64_t psremove_index_%zu=0;\n", id);
            else
                out(e, "int64_t psremove_index_%zu=psrt_array_count(&psv_%zu) ? "
                       "(int64_t)(psrt_array_count(&psv_%zu)-1) : -1;\n",
                    id, receiver, receiver);
            out(e,
                "const void *psremoved_%zu; psvalue_check(psrt_array_at(&psv_%zu,"
                "psremove_index_%zu,"
                "&psremoved_%zu)",
                id, receiver, id, id);
            checked_end(e, id);
            out(e, "%s psv_%zu; memcpy(&psv_%zu,psremoved_%zu,sizeof psv_%zu);\n",
                type(e, e->info[id].type), id, id, id, id);
            if (owns(e, e->info[id].type)) {
                retain_temp(e, id);
                register_temp(e, id);
            }
            out(e, "psvalue_check(psrt_array_replace(&psv_%zu,"
                   "(size_t)psremove_index_%zu,1,NULL,0)", receiver, id);
        } else if (fn == PS_LANG_BUILTIN_ARRAY_APPEND_CONTENTS)
            out(e, "psvalue_check(psrt_array_replace(&psv_%zu,"
                   "psrt_array_count(&psv_%zu),0,psrt_array_data(&psv_%zu),"
                   "psrt_array_count(&psv_%zu))",
                receiver, receiver, value, value);
        else
            out(e,
                "psvalue_check(psrt_array_replace(&psv_%zu, psrt_array_count(&psv_%zu), "
                "0, &psv_%zu, 1)",
                receiver, receiver, value);
        checked_end(e, id);
        writeback(e, id, receiver, receiver);
        return;
    }
    const ps_lang_builtin *builtin = ps_lang_builtin_get(fn);
    if (fn < e->node_count && e->nodes[fn].kind == PS_AST_LOCAL_FUNCTION)
        expression(e, n->a);
    if (e->info[n->a].receiver_slot)
        expression(e, e->nodes[n->a].a);
    for (size_t a = n->b; a; a = e->nodes[a].next)
        expression(e, e->nodes[a].a);
    if (builtin) {
        if (builtin->count && builtin->types[0] == PS_TYPE_FUNCTION) {
            for (size_t a = n->b; a; a = e->nodes[a].next)
                if (e->info[a].binding == 1) {
                    out(e, "psrt_scalar_value_context pscb_ctx_%zu = {&psv_%zu, %s, ",
                        id, e->nodes[a].a, e->experiment ? "psstate" : "NULL");
                    site(e, id);
                    out(e, "};\n");
                    break;
                }
        }
        for (size_t a = n->b; a; a = e->nodes[a].next)
            if (builtin->types[e->info[a].binding - 1] == PS_LANG_STRING_ARRAY) {
                out(e, "const char *pslabels_%zu[8]; psvalue_check("
                       "psrt_string_array_cstr(&psv_%zu,pslabels_%zu,8)",
                    id, e->nodes[a].a, id);
                checked_end(e, id);
            }
        if (borrowed_symbol_builtin(builtin)) {
            for (size_t a = n->b; a; a = e->nodes[a].next)
                if (builtin->types[e->info[a].binding - 1] == PS_TYPE_STRING) {
                    out(e, "const char *pssymbol_%zu; psstring_check(psrt_string_pin_cstr(&%s,"
                           "&psv_%zu,&pssymbol_%zu)", id,
                        e->experiment ? "psstate->memory" : "psmemory", e->nodes[a].a, id);
                    checked_end(e, id);
                    break;
                }
        }
        if (builtin->count && builtin->types[0] == PS_LANG_ODE_CALLBACK) {
            for (size_t a = n->b; a; a = e->nodes[a].next)
                if (e->info[a].binding == 2) {
                    out(e, "psrt_ode_value_context psode_ctx_%zu = {{"
                           "psrt_memory_allocator(&%s), psrt_array_count(&psv_%zu)%s, ",
                        id, e->experiment ? "psstate->memory" : "psmemory", e->nodes[a].a,
                        strcmp(builtin->name, "verletStep") == 0 ? "/ 2" : "");
                    site(e, id);
                    size_t callback = 0;
                    for (size_t p = n->b; p; p = e->nodes[p].next)
                        if (e->info[p].binding == 1) callback = e->nodes[p].a;
                    out(e, ", %s, %s}, &psv_%zu};\n",
                        e->experiment ? "psstate" : "NULL",
                        strcmp(builtin->name, "verletStep") == 0 ? "true" : "false",
                        callback);
                    break;
                }
        }
    }
    if (e->info[id].type != PS_TYPE_VOID)
        out(e, "%s psv_%zu = ", type(e, e->info[id].type), id);
    if (fn == PS_LANG_BUILTIN_ARRAY_COUNT) {
        out(e, "(int64_t)psrt_array_count(&psv_%zu);\n", e->nodes[n->b].a);
        return;
    }
    if (fn == PS_LANG_BUILTIN_INT64 || fn == PS_LANG_BUILTIN_FLOAT64) {
        size_t value = e->nodes[n->b].a;
        if (e->info[value].type == e->info[id].type)
            out(e, "psv_%zu;\n", value);
        else if (e->info[value].type == PS_TYPE_STRING) {
            out(e, "%s(psrt_string_cstr(&psv_%zu), ",
                fn == PS_LANG_BUILTIN_INT64 ? "psrt_parse_int64" : "psrt_parse_float64", value);
            site(e, id);
            out(e, ");\n");
        }
        else {
            out(e, "%s(psv_%zu, ", fn == PS_LANG_BUILTIN_INT64 ? "psrt_int64" : "psrt_float64",
                value);
            site(e, id);
            out(e, ");\n");
        }
        return;
    }
    if (builtin) {
        out(e, "%s(", builtin->c_name);
        if (builtin->host)
            out(e, "&psstate->host, ");
        if (strcmp(builtin->name, "quantile") == 0)
            out(e, "psrt_memory_allocator(&%s), ",
                e->experiment ? "psstate->memory" : "psmemory");
        if (ps_lang_signature_element(builtin->result) ||
            builtin->result == PS_TYPE_STRING ||
            builtin->result == PS_TYPE_CONSTRAINT_RESULT ||
            builtin->result == PS_TYPE_ODE_RESULT)
            out(e, "psrt_memory_allocator(&%s), ", e->experiment ? "psstate->memory" : "psmemory");
        for (unsigned p = 1; p <= builtin->count; p++) {
            if (p == e->info[n->a].receiver_slot) {
                out(e, "%spsv_%zu, ", e->info[n->a].method_mutating ? "&" : "",
                    e->nodes[n->a].a);
                continue;
            }
            for (size_t a = n->b; a; a = e->nodes[a].next)
                if (e->info[a].binding == p) {
                    size_t value = e->nodes[a].a;
                    if (builtin->types[p - 1] == PS_LANG_STRING_ARRAY)
                        out(e, "pslabels_%zu, psrt_array_count(&psv_%zu), ", id, value);
                    else if (builtin->types[p - 1] == PS_TYPE_FUNCTION)
                        out(e, "pscb_%zu, &pscb_ctx_%zu, ", id, id);
                    else if (builtin->types[p - 1] == PS_LANG_ODE_CALLBACK)
                        out(e, "psodecb_%zu, &psode_ctx_%zu, ", id, id);
                    else if (ps_lang_signature_element(builtin->types[p - 1]))
                        out(e, "psrt_array_data(&psv_%zu), psrt_array_count(&psv_%zu), ",
                            value, value);
                    else if (builtin->types[p - 1] == PS_TYPE_STRING &&
                             borrowed_symbol_builtin(builtin))
                        out(e, "pssymbol_%zu, ", id);
                    else if (builtin->types[p - 1] == PS_TYPE_STRING)
                        out(e, "psrt_string_cstr(&psv_%zu), ", value);
                    else
                        out(e, "psv_%zu, ", value);
                    break;
                }
        }
        site(e, id);
        out(e, ");\n");
        if (e->info[n->a].method_mutating) {
            if (owns(e, e->info[id].type))
                register_temp(e, id);
            size_t receiver = e->nodes[n->a].a;
            writeback(e, id, receiver, receiver);
        }
        return;
    }
    if (fn == PS_LANG_BUILTIN_ASSERT || fn == PS_LANG_BUILTIN_PRINT) {
        size_t value = e->nodes[n->b].a;
        size_t message = e->nodes[n->b].next;
        if (fn == PS_LANG_BUILTIN_ASSERT)
            out(e, "%s(", message ? "psrt_assert_message" : "psrt_assert");
        else {
            ps_lang_type t = e->info[value].type;
            out(e, "psrt_print_%c(",
                t == PS_TYPE_INT64     ? 'i'
                : t == PS_TYPE_FLOAT64 ? 'f'
                : t == PS_TYPE_BOOL    ? 'b'
                                       : 's');
        }
        if (fn == PS_LANG_BUILTIN_PRINT && e->info[value].type == PS_TYPE_STRING)
            out(e, "psrt_string_cstr(&psv_%zu), ", value);
        else
            out(e, "psv_%zu, ", value);
        if (message)
            out(e, "psrt_string_cstr(&psv_%zu), ", e->nodes[message].a);
        site(e, id);
        out(e, ");\n");
        return;
    }
    if (e->nodes[fn].kind == PS_AST_STRUCT) {
        out(e, "{0};\n");
        if (owns(e, e->info[id].type))
            register_temp(e, id);
        for (size_t f = e->nodes[fn].a; f; f = e->nodes[f].next) {
            size_t supplied = 0;
            for (size_t a = n->b; a; a = e->nodes[a].next)
                if (e->info[a].binding == f) {
                    supplied = e->nodes[a].a;
                    break;
                }
            size_t value = supplied ? supplied : e->nodes[f].b;
            if (!supplied) {
                out(e, "{\n");
                if (e->arrays)
                    out(e, "psrt_cleanup *psdefault_mark_%zu = psrt_cleanups;\n", id);
                expression(e, value);
            }
            if (owns(e, e->info[f].type)) {
                out(e, "psvalue_check(psvalue_copy(");
                descriptor(e, e->info[f].type);
                out(e, ",&psv_%zu.psfield_%zu,&psv_%zu)", id, f, value);
                checked_end(e, value);
            } else
                out(e, "psv_%zu.psfield_%zu = psv_%zu;\n", id, f, value);
            if (!supplied) {
                if (e->arrays)
                    out(e, "psrt_cleanup_unwind(psdefault_mark_%zu);\n", id);
                out(e, "}\n");
            }
        }
        return;
    }
    out(e, "psfn_%zu(", fn);
    int comma = 0;
    if (e->experiment) {
        out(e, "psstate");
        comma = 1;
    }
    if (e->nodes[fn].kind == PS_AST_LOCAL_FUNCTION) {
        out(e, "%s&psv_%zu", comma ? ", " : "", n->a);
        comma = 1;
    }
    for (size_t p = e->nodes[fn].a; p; p = e->nodes[p].next) {
        if (e->nodes[p].kind == PS_AST_SELF_PARAMETER) {
            out(e, "%s%spsv_%zu", comma ? ", " : "", e->info[fn].method_mutating ? "&" : "",
                e->nodes[n->a].a);
            comma = 1;
            continue;
        }
        for (size_t a = n->b; a; a = e->nodes[a].next) {
            if (e->info[a].binding == p) {
                out(e, "%spsv_%zu", comma ? ", " : "", e->nodes[a].a);
                comma = 1;
                break;
            }
        }
    }
    out(e, ");\n");
    if (e->info[fn].method_mutating) {
        if (owns(e, e->info[id].type))
            register_temp(e, id);
        size_t receiver = e->nodes[n->a].a;
        writeback(e, id, receiver, receiver);
    }
}
static void binary(emitter *e, size_t id) {
    const ps_lang_node *n = &e->nodes[id];
    ps_lang_kind op = n->token.kind;
    expression(e, n->a);
    if (op == PS_LANG_AND || op == PS_LANG_OR) {
        out(e, "bool psv_%zu = psv_%zu;\nif (%spsv_%zu) {\n", id, n->a, op == PS_LANG_OR ? "!" : "",
            id);
        if (e->arrays)
            out(e, "psrt_cleanup *pslogic_%zu = psrt_cleanups;\n", id);
        expression(e, n->b);
        out(e, "psv_%zu = psv_%zu;\n", id, n->b);
        if (e->arrays)
            out(e, "psrt_cleanup_unwind(pslogic_%zu);\n", id);
        out(e, "}\n");
        return;
    }
    if (op == PS_LANG_COALESCE) {
        out(e, "%s psv_%zu;\nif (psrt_array_count(&psv_%zu)) {\n",
            type(e, e->info[id].type), id, n->a);
        out(e, "memcpy(&psv_%zu,psrt_array_data(&psv_%zu),sizeof psv_%zu);\n",
            id, n->a, id);
        if (owns(e, e->info[id].type))
            retain_temp(e, id);
        out(e, "} else {\npsrt_cleanup *pscoalesce_%zu = psrt_cleanups;\n", id);
        expression(e, n->b);
        out(e, "psv_%zu = psv_%zu;\n", id, n->b);
        if (owns(e, e->info[id].type))
            retain_temp(e, id);
        out(e, "psrt_cleanup_unwind(pscoalesce_%zu);\n}\n", id);
        if (owns(e, e->info[id].type))
            register_temp(e, id);
        return;
    }
    expression(e, n->b);
    if (op == PS_LANG_PLUS && e->info[n->a].type >= PS_TYPE_ARRAY_BASE) {
        out(e, "psrt_array psv_%zu; psvalue_check(psrt_array_clone(&psv_%zu,&psv_%zu)",
            id, n->a, id);
        checked_end(e, id);
        register_temp(e, id);
        out(e, "psvalue_check(psrt_array_replace(&psv_%zu,psrt_array_count(&psv_%zu),0,"
               "psrt_array_data(&psv_%zu),psrt_array_count(&psv_%zu))",
            id, id, n->b, n->b);
        checked_end(e, id);
        return;
    }
    if (op == PS_LANG_PLUS && e->info[n->a].type == PS_TYPE_STRING) {
        out(e, "psrt_string psv_%zu; psstring_check(psrt_string_concat(&psv_%zu,&psv_%zu,"
               "psrt_memory_allocator(&%s),&psv_%zu)", id, n->a, n->b,
            e->experiment ? "psstate->memory" : "psmemory", id);
        checked_end(e, id);
        return;
    }
    ps_lang_type quantity_left = e->info[n->a].type;
    ps_lang_type quantity_right = e->info[n->b].type;
    if (quantity_left == PS_TYPE_QUANTITY && quantity_right == PS_TYPE_QUANTITY &&
        (op == PS_LANG_PLUS || op == PS_LANG_MINUS)) {
        out(e, "ps_quantity psv_%zu = %s(psv_%zu, psv_%zu, ", id,
            op == PS_LANG_PLUS ? "psrt_quantity_add" : "psrt_quantity_subtract",
            n->a, n->b);
        site(e, id);
        out(e, ");\n");
        return;
    }
    if (op == PS_LANG_STAR &&
        ((quantity_left == PS_TYPE_QUANTITY && quantity_right == PS_TYPE_FLOAT64) ||
         (quantity_left == PS_TYPE_FLOAT64 && quantity_right == PS_TYPE_QUANTITY))) {
        out(e, "ps_quantity psv_%zu = psrt_quantity_scale(psv_%zu, psv_%zu, ", id,
            quantity_left == PS_TYPE_QUANTITY ? n->a : n->b,
            quantity_left == PS_TYPE_QUANTITY ? n->b : n->a);
        site(e, id);
        out(e, ");\n");
        return;
    }
    if (op == PS_LANG_SLASH && quantity_left == PS_TYPE_QUANTITY &&
        quantity_right == PS_TYPE_FLOAT64) {
        out(e, "ps_quantity psv_%zu = psrt_quantity_divide_scalar(psv_%zu, psv_%zu, ",
            id, n->a, n->b);
        site(e, id);
        out(e, ");\n");
        return;
    }
    out(e, "%s psv_%zu = ", type(e, e->info[id].type), id);
    ps_lang_type left = e->info[n->a].type, right = e->info[n->b].type;
    if (ps_lang_optional_type(left)) {
        if (e->nodes[n->a].token.kind != PS_LANG_NIL &&
            e->nodes[n->b].token.kind != PS_LANG_NIL) {
            out(e, "psequal_%u(&psv_%zu,&psv_%zu);\n", (unsigned)left, n->a, n->b);
            if (op == PS_LANG_NE) out(e, "psv_%zu = !psv_%zu;\n", id, id);
            return;
        }
        out(e, "psrt_array_count(&psv_%zu) == psrt_array_count(&psv_%zu);\n",
            n->a, n->b);
        if (op == PS_LANG_NE)
            out(e, "psv_%zu = !psv_%zu;\n", id, id);
        return;
    }
    if ((ps_lang_record_type(left) || left >= PS_TYPE_ARRAY_BASE) &&
        (op == PS_LANG_EQ || op == PS_LANG_NE)) {
        out(e, "psequal_%u(&psv_%zu,&psv_%zu);\n", (unsigned)left, n->a, n->b);
        if (op == PS_LANG_NE) out(e, "psv_%zu = !psv_%zu;\n", id, id);
        return;
    }
    unsigned left_vector = ps_lang_vector_dimensions(left);
    unsigned right_vector = ps_lang_vector_dimensions(right);
    if (left_vector || right_vector) {
        unsigned dimensions = left_vector ? left_vector : right_vector;
        if (op == PS_LANG_EQ || op == PS_LANG_NE) {
            out(e, op == PS_LANG_NE ? "!(" : "(");
            for (unsigned axis = 0; axis < dimensions; axis++)
                out(e, "%spsv_%zu.%c == psv_%zu.%c", axis ? " && " : "", n->a, "xyzw"[axis], n -> b,
                    "xyzw"[axis]);
            out(e, ");\n");
            return;
        }
        out(e, "psrt_vec%u(", dimensions);
        for (unsigned axis = 0; axis < dimensions; axis++) {
            if (op == PS_LANG_SLASH)
                out(e, "psrt_fdiv(");
            out(e, "psv_%zu", n->a);
            if (left_vector)
                out(e, ".%c", "xyzw"[axis]);
            out(e, op == PS_LANG_SLASH ? ", " : " %s ", operator_text(op));
            out(e, "psv_%zu", n->b);
            if (right_vector)
                out(e, ".%c", "xyzw"[axis]);
            if (op == PS_LANG_SLASH) {
                out(e, ", ");
                site(e, id);
                out(e, ")");
            }
            out(e, ", ");
        }
        site(e, id);
        out(e, ");\n");
        return;
    }
    const char *helper = NULL;
    if (e->info[n->a].type == PS_TYPE_INT64) {
        switch (op) {
        case PS_LANG_PLUS:
            helper = "psrt_add";
            break;
        case PS_LANG_MINUS:
            helper = "psrt_sub";
            break;
        case PS_LANG_STAR:
            helper = "psrt_mul";
            break;
        case PS_LANG_SLASH:
            helper = "psrt_div";
            break;
        case PS_LANG_PERCENT:
            helper = "psrt_mod";
            break;
        case PS_LANG_AMP:
            helper = "psrt_bit_and";
            break;
        case PS_LANG_PIPE:
            helper = "psrt_bit_or";
            break;
        case PS_LANG_CARET:
            helper = "psrt_bit_xor";
            break;
        case PS_LANG_SHIFT_LEFT:
            helper = "psrt_shift_left";
            break;
        case PS_LANG_SHIFT_RIGHT:
            helper = "psrt_shift_right";
            break;
        default:
            break;
        }
    } else if (op == PS_LANG_SLASH)
        helper = "psrt_fdiv";
    if (helper) {
        out(e, "%s(psv_%zu, psv_%zu, ", helper, n->a, n->b);
        site(e, id);
        out(e, ")");
    } else if (e->info[n->a].type == PS_TYPE_STRING) {
        out(e, "(strcmp(psrt_string_cstr(&psv_%zu), psrt_string_cstr(&psv_%zu)) %s 0)",
            n->a, n->b, operator_text(op));
    } else {
        int checked = e->info[id].type == PS_TYPE_FLOAT64;
        if (checked)
            out(e, "psrt_finite(");
        out(e, "(psv_%zu %s psv_%zu)", n->a, operator_text(op), n->b);
        if (checked) {
            out(e, ", ");
            site(e, id);
            out(e, ")");
        }
    }
    out(e, ";\n");
}
static uint64_t integer(emitter *e, ps_lang_token token) {
    return ps_lang_integer_value(e->source, token);
}
static void expression(emitter *e, size_t id) {
    if (e->cached == id)
        return;
    if (e->error.kind == PS_LANG_ERROR)
        return;
    if (e->depth >= 128) {
        fail(e, id, "Code generation nesting limit exceeded");
        return;
    }
    e->depth++;
    const ps_lang_node *n = &e->nodes[id];
    ps_lang_type t = e->info[id].type;
    switch (n->kind) {
    case PS_AST_LAMBDA:
    case PS_AST_LOCAL_FUNCTION:
        out(e, "psrt_function_value psv_%zu = {%zu,{0}};\n", id, id);
        if (e->info[id].capture_head) {
            out(e, "pscapture_%zu pscaptured_%zu = {0};\n", id, id);
            for (size_t capture = e->info[id].capture_head; capture;
                 capture = e->info[capture].capture_next) {
                size_t decl = e->info[capture].binding;
                initialized(e, decl, capture);
                out(e, "pscaptured_%zu.psfield_%zu = ", id, decl);
                storage(e, decl);
                out(e, ";\n");
            }
            out(e, "psvalue_check(psrt_array_init(&psdesc_capture_%zu,"
                   "psrt_memory_allocator(&%s),1,&psv_%zu.receiver)",
                id, e->experiment ? "psstate->memory" : "psmemory", id);
            checked_end(e, id);
            register_temp(e, id);
            out(e, "psvalue_check(psrt_array_replace(&psv_%zu.receiver,0,0,"
                   "&pscaptured_%zu,1)", id, id);
            checked_end(e, id);
            e->depth--;
            return;
        }
        break;
    case PS_AST_ARRAY: {
        size_t count = 0;
        for (size_t item = n->a; item; item = e->nodes[item].next)
            count++;
        out(e, "psrt_array psv_%zu; psvalue_check(psrt_array_init(", id);
        descriptor(e, e->info[t - PS_TYPE_ARRAY_BASE].array_element);
        out(e, ",psrt_memory_allocator(&%s),0,&psv_%zu)",
            e->experiment ? "psstate->memory" : "psmemory", id);
        checked_end(e, id);
        register_temp(e, id);
        out(e, "psvalue_check(psrt_array_build_begin(&psv_%zu,%zu)", id, count);
        checked_end(e, id);
        for (size_t item = n->a; item; item = e->nodes[item].next) {
            expression(e, item);
            out(e, "psvalue_check(psrt_array_copy_one(psv_%zu.block,&psv_%zu)", id, item);
            checked_end(e, id);
        }
        break;
    }
    case PS_AST_INDEX:
        expression(e, n->a);
        if (e->info[n->a].type == PS_TYPE_STRING) {
            if (e->info[n->b].type == PS_TYPE_RANGE) {
                const ps_lang_node *range = &e->nodes[range_base(e, n->b)];
                size_t step = range_step(e, n->b);
                if (range->a)
                    expression(e, range->a);
                if (range->b)
                    expression(e, range->b);
                if (step)
                    expression(e, step);
                positive_step(e, step);
                out(e, "psrt_string psv_%zu; psstring_check(psrt_string_slice_bounds(&psv_%zu,",
                    id, n->a);
                range_arguments(e, range);
                if (step)
                    out(e, ",psv_%zu,", step);
                else
                    out(e, ",1,");
            } else {
                expression(e, n->b);
                out(e, "psrt_string psv_%zu; psstring_check(psrt_string_slice_bounds(&psv_%zu,"
                       "1,psv_%zu,1,psv_%zu,1,1,", id, n->a, n->b, n->b);
            }
            out(e, "psrt_memory_allocator(&%s),&psv_%zu)",
                e->experiment ? "psstate->memory" : "psmemory", id);
            checked_end(e, id);
            register_temp(e, id);
            e->depth--;
            return;
        }
        if (e->info[n->b].type == PS_TYPE_RANGE) {
            const ps_lang_node *range = &e->nodes[range_base(e, n->b)];
            size_t step = range_step(e, n->b);
            if (range->a)
                expression(e, range->a);
            if (range->b)
                expression(e, range->b);
            if (step)
                expression(e, step);
            positive_step(e, step);
            out(e, "psrt_array psv_%zu; psvalue_check(psrt_array_slice_strided_bounds(&psv_%zu,",
                id, n->a);
            range_arguments(e, range);
            if (step)
                out(e, ",psv_%zu,&psv_%zu)", step, id);
            else
                out(e, ",1,&psv_%zu)", id);
            checked_end(e, id);
            register_temp(e, id);
            e->depth--;
            return;
        }
        expression(e, n->b);
        out(e, "const void *psptr_%zu; psvalue_check(psrt_array_at(&psv_%zu,psv_%zu,&psptr_%zu)",
            id, n->a, n->b, id);
        checked_end(e, id);
        out(e, "%s psv_%zu; memcpy(&psv_%zu,psptr_%zu,sizeof psv_%zu);\n", type(e, t), id, id, id,
            id);
        break;
    case PS_AST_STRING_SEGMENT:
        string_segment_literal(e, id, n->token);
        out(e, "psrt_string psv_%zu; psstring_check(psrt_string_make("
               "psrt_memory_allocator(&%s),pslit_%zu,sizeof pslit_%zu-1,&psv_%zu)",
            id, e->experiment ? "psstate->memory" : "psmemory", id, id, id);
        checked_end(e, id);
        break;
    case PS_AST_INTERPOLATED_STRING:
        out(e, "psrt_string psv_%zu; psstring_check(psrt_string_make("
               "psrt_memory_allocator(&%s),NULL,0,&psv_%zu)",
            id, e->experiment ? "psstate->memory" : "psmemory", id);
        checked_end(e, id);
        register_temp(e, id);
        for (size_t part = n->a; part && e->error.kind != PS_LANG_ERROR;
             part = e->nodes[part].next) {
            out(e, "{\npsrt_cleanup *pspartmark_%zu = psrt_cleanups;\n", part);
            expression(e, part);
            if (e->info[part].type != PS_TYPE_STRING) {
                out(e, "psrt_string psformatted_%zu; psstring_check(%s(psv_%zu,"
                       "psrt_memory_allocator(&%s),&psformatted_%zu)",
                    part, e->info[part].type == PS_TYPE_INT64 ? "psrt_string_from_int64"
                          : e->info[part].type == PS_TYPE_FLOAT64 ? "psrt_string_from_float64"
                                                                    : "psrt_string_from_bool",
                    part, e->experiment ? "psstate->memory" : "psmemory", part);
                checked_end(e, part);
                out(e, "psrt_cleanup psformat_cleanup_%zu; psrt_cleanup_push("
                       "&psformat_cleanup_%zu,&psformatted_%zu,psrt_array_destroy);\n",
                    part, part, part);
            }
            out(e, "psrt_string psnext_%zu; psstring_check(psrt_string_concat("
                   "&psv_%zu,&%s_%zu,psrt_memory_allocator(&%s),&psnext_%zu)",
                part, id, e->info[part].type == PS_TYPE_STRING ? "psv" : "psformatted",
                part, e->experiment ? "psstate->memory" : "psmemory", part);
            checked_end(e, part);
            out(e, "psrt_array_destroy(&psv_%zu); psv_%zu = psnext_%zu;\n"
                   "psrt_cleanup_unwind(pspartmark_%zu);\n}\n", id, id, part, part);
        }
        e->depth--;
        return;
    case PS_AST_LITERAL:
        if (n->token.kind == PS_LANG_NIL) {
            optional_init(e, id);
            e->depth--;
            return;
        }
        if (t == PS_TYPE_STRING)
            string_literal(e, id, n->token);
        else if (t == PS_TYPE_FLOAT64 &&
                 !ps_lang_prefixed_integer(e->source, n->token))
            literal_bytes(e, id, e->source + n->token.offset, n->token.length);
        if (t == PS_TYPE_STRING) {
            out(e, "psrt_string psv_%zu; psstring_check(psrt_string_make("
                   "psrt_memory_allocator(&%s),pslit_%zu,sizeof pslit_%zu-1,&psv_%zu)",
                id, e->experiment ? "psstate->memory" : "psmemory", id, id, id);
            checked_end(e, id);
            break;
        }
        out(e, "%s psv_%zu = ", type(e, t), id);
        if (t == PS_TYPE_FLOAT64) {
            if (ps_lang_prefixed_integer(e->source, n->token))
                out(e, "(double)UINT64_C(%" PRIu64 ")", integer(e, n->token));
            else {
                out(e, "psrt_number((const char *)pslit_%zu", id);
                out(e, ", ");
                site(e, id);
                out(e, ")");
            }
        } else if (t == PS_TYPE_INT64)
            out(e, "INT64_C(%" PRIu64 ")", integer(e, n->token));
        else
            out(e, "%s", n->token.kind == PS_LANG_TRUE ? "true" : "false");
        out(e, ";\n");
        break;
    case PS_AST_NAME:
        if (e->info[id].binding < e->node_count &&
            e->nodes[e->info[id].binding].kind == PS_AST_FUNCTION &&
            ps_lang_function_type(t)) {
            out(e, "psrt_function_value psv_%zu = {%zu,{0}};\n", id,
                e->info[id].binding);
            break;
        }
        initialized(e, e->info[id].binding, id);
        out(e, "%s psv_%zu = ", type(e, t), id);
        storage(e, e->info[id].binding);
        out(e, ";\n");
        break;
    case PS_AST_TYPE_APPLY:
        if (e->info[id].binding < e->node_count &&
            e->nodes[e->info[id].binding].kind == PS_AST_LOCAL_FUNCTION) {
            out(e, "psrt_function_value psv_%zu = ", id);
            storage(e, e->info[id].binding);
            out(e, ";\n");
            break;
        }
        if (e->info[id].binding < e->node_count &&
            e->info[e->info[id].binding].method_owner &&
            !e->info[e->info[id].binding].method_static) {
            size_t receiver = e->nodes[n->a].a;
            expression(e, receiver);
            out(e, "psrt_function_value psv_%zu = {%zu,{0}};\n", id,
                e->info[id].binding);
            out(e, "psvalue_check(psrt_array_init(");
            descriptor(e, e->info[receiver].type);
            out(e, ",psrt_memory_allocator(&%s),1,&psv_%zu.receiver)",
                e->experiment ? "psstate->memory" : "psmemory", id);
            checked_end(e, id);
            register_temp(e, id);
            out(e, "psvalue_check(psrt_array_replace(&psv_%zu.receiver,0,0,"
                   "&psv_%zu,1)", id, receiver);
            checked_end(e, id);
            e->depth--;
            return;
        }
        out(e, "psrt_function_value psv_%zu = {%zu,{0}};\n", id,
            e->info[id].binding);
        break;
    case PS_AST_CALL:
        call(e, id);
        if (optional_call(e->info[id].binding) ||
            e->info[id].binding == PS_LANG_BUILTIN_INT64_IS_MULTIPLE ||
            e->info[id].binding == PS_LANG_BUILTIN_INT64_SIGNUM ||
            e->info[id].binding == PS_LANG_BUILTIN_INT64_PARSE ||
            e->info[id].binding == PS_LANG_BUILTIN_FLOAT64_PARSE ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_POP_LAST ||
            e->info[id].binding == PS_LANG_BUILTIN_ATTEMPT ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_INSERT ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_SWAP_AT ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_REMOVE_SUBRANGE ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_REPLACE_SUBRANGE ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_PREFIX ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_SUFFIX ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_DROP_FIRST ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_DROP_LAST ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_INSERT_CONTENTS ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_APPEND_CONTENTS ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_REMOVE ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_REMOVE_FIRST ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_REMOVE_LAST ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_REMOVE_FIRST_COUNT ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_REMOVE_LAST_COUNT ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_FIRST_INDEX ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_LAST_INDEX ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_REVERSED ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_REPEATED ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_REVERSE ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_SORTED ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_SORT ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_SORTED_BY ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_SORT_BY ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_MIN ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_MAX ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_MIN_BY ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_MAX_BY ||
            e->info[id].binding == PS_LANG_BUILTIN_STRING_SORTED ||
            e->info[id].binding == PS_LANG_BUILTIN_STRING_SORTED_BY ||
            e->info[id].binding == PS_LANG_BUILTIN_STRING_MIN ||
            e->info[id].binding == PS_LANG_BUILTIN_STRING_MAX ||
            e->info[id].binding == PS_LANG_BUILTIN_STRING_MIN_BY ||
            e->info[id].binding == PS_LANG_BUILTIN_STRING_MAX_BY ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_FILTER ||
            e->info[id].binding == PS_LANG_BUILTIN_STRING_FILTER ||
            e->info[id].binding == PS_LANG_BUILTIN_STRING_MAP ||
            e->info[id].binding == PS_LANG_BUILTIN_STRING_COMPACT_MAP ||
            e->info[id].binding == PS_LANG_BUILTIN_STRING_FLAT_MAP ||
            e->info[id].binding == PS_LANG_BUILTIN_STRING_FIRST_WHERE ||
            e->info[id].binding == PS_LANG_BUILTIN_STRING_LAST_WHERE ||
            e->info[id].binding == PS_LANG_BUILTIN_STRING_FIRST_INDEX_WHERE ||
            e->info[id].binding == PS_LANG_BUILTIN_STRING_LAST_INDEX_WHERE ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_REMOVE_ALL ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_REMOVE_ALL_WHERE ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_MAP ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_COMPACT_MAP ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_FLAT_MAP ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_PREFIX_WHILE ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_DROP_WHILE ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_ANY ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_ALL ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_FIRST_WHERE ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_LAST_WHERE ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_FIRST_INDEX_WHERE ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_LAST_INDEX_WHERE ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_REDUCE ||
            e->info[id].binding == PS_LANG_BUILTIN_STRING_REDUCE ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_FOR_EACH ||
            e->info[id].binding == PS_LANG_BUILTIN_STRING_FOR_EACH ||
            e->info[id].binding == PS_LANG_BUILTIN_STRING_FIRST_INDEX ||
            e->info[id].binding == PS_LANG_BUILTIN_STRING_LAST_INDEX ||
            e->info[id].binding == PS_LANG_BUILTIN_ENUM_FROM_RAW ||
            (e->info[id].binding < e->node_count &&
             e->nodes[e->info[id].binding].kind == PS_AST_STRUCT) ||
            e->info[n->a].method_mutating) {
            e->depth--;
            return;
        }
        break;
    case PS_AST_MEMBER:
        if (e->info[id].binding < e->node_count &&
            (e->nodes[e->info[id].binding].kind == PS_AST_FUNCTION ||
             e->nodes[e->info[id].binding].kind == PS_AST_CASE) &&
            ps_lang_function_type(t)) {
            size_t method = e->info[id].binding;
            if (e->info[method].method_owner && !e->info[method].method_static) {
                expression(e, n->a);
                out(e, "psrt_function_value psv_%zu = {%zu,{0}};\n", id, method);
                out(e, "psvalue_check(psrt_array_init(");
                descriptor(e, e->info[n->a].type);
                out(e, ",psrt_memory_allocator(&%s),1,&psv_%zu.receiver)",
                    e->experiment ? "psstate->memory" : "psmemory", id);
                checked_end(e, id);
                register_temp(e, id);
                out(e, "psvalue_check(psrt_array_replace(&psv_%zu.receiver,0,0,"
                       "&psv_%zu,1)", id, n->a);
                checked_end(e, id);
                e->depth--;
                return;
            }
            out(e, "psrt_function_value psv_%zu = {%zu,{0}};\n", id,
                e->info[id].binding);
            break;
        }
        if (e->nodes[n->a].kind == PS_AST_NAME &&
            e->info[n->a].binding && e->info[n->a].binding < e->node_count &&
            e->nodes[e->info[n->a].binding].kind == PS_AST_IMPORT &&
            e->info[id].binding && e->info[id].binding < e->node_count &&
            e->nodes[e->info[id].binding].kind == PS_AST_VARIABLE) {
            initialized(e, e->info[id].binding, id);
            out(e, "%s psv_%zu = ", type(e, t), id);
            storage(e, e->info[id].binding);
            out(e, ";\n");
            break;
        }
        if (e->info[id].binding == PS_LANG_BUILTIN_OPTIONAL_HAS) {
            expression(e, n->a);
            out(e, "bool psv_%zu = psrt_array_count(&psv_%zu) != 0;\n", id, n->a);
            break;
        }
        if (e->info[id].binding == PS_LANG_BUILTIN_ARRAY_COUNT) {
            expression(e, n->a);
            out(e, "int64_t psv_%zu = (int64_t)psrt_array_count(&psv_%zu);\n", id, n->a);
            break;
        }
        if (e->info[id].binding == PS_LANG_BUILTIN_ARRAY_IS_EMPTY) {
            expression(e, n->a);
            out(e, "bool psv_%zu = psrt_array_count(&psv_%zu) == 0;\n", id, n->a);
            break;
        }
        if (e->info[id].binding == PS_LANG_BUILTIN_ARRAY_FIRST ||
            e->info[id].binding == PS_LANG_BUILTIN_ARRAY_LAST) {
            expression(e, n->a);
            optional_init(e, id);
            out(e, "if (psrt_array_count(&psv_%zu)) {\n", n->a);
            if (e->info[id].binding == PS_LANG_BUILTIN_ARRAY_FIRST)
                out(e, "size_t psaccess_index_%zu = 0;\n", id);
            else
                out(e, "size_t psaccess_index_%zu = psrt_array_count(&psv_%zu)-1;\n",
                    id, n->a);
            out(e, "psvalue_check(psrt_array_build_begin(&psv_%zu,1)", id);
            checked_end(e, id);
            out(e, "psvalue_check(psrt_array_copy_one(psv_%zu.block,"
                   "(const unsigned char *)psrt_array_data(&psv_%zu) + "
                   "psaccess_index_%zu * psv_%zu.type->size)",
                id, n->a, id, n->a);
            checked_end(e, id);
            out(e, "}\n");
            e->depth--;
            return;
        }
        if (e->info[id].binding == PS_LANG_BUILTIN_STRING_FIRST ||
            e->info[id].binding == PS_LANG_BUILTIN_STRING_LAST) {
            int last = e->info[id].binding == PS_LANG_BUILTIN_STRING_LAST;
            expression(e, n->a);
            optional_init(e, id);
            out(e, "if (psrt_string_byte_count(&psv_%zu)) {\n"
                   "size_t pscursor_%zu=0;\npsrt_string psitem_%zu;\n",
                n->a, id, id);
            if (last)
                out(e, "pscursor_%zu=psrt_string_byte_count(&psv_%zu);\n", id, n->a);
            out(e, "psstring_check(psrt_string_%s_scalar(&psv_%zu,&pscursor_%zu,"
                   "psrt_memory_allocator(&%s),&psitem_%zu)",
                last ? "previous" : "next", n->a, id,
                e->experiment ? "psstate->memory" : "psmemory", id);
            checked_end(e, id);
            out(e, "psrt_cleanup *psitem_mark_%zu=psrt_cleanups;\n"
                   "psrt_cleanup psitem_owner_%zu; "
                   "psrt_cleanup_push(&psitem_owner_%zu,&psitem_%zu,"
                   "psrt_string_destroy);\n", id, id, id, id);
            out(e, "psvalue_check(psrt_array_build_begin(&psv_%zu,1)", id);
            checked_end(e, id);
            out(e, "psvalue_check(psrt_array_copy_one(psv_%zu.block,&psitem_%zu)",
                id, id);
            checked_end(e, id);
            out(e, "psrt_cleanup_unwind(psitem_mark_%zu);\n}\n", id);
            e->depth--;
            return;
        }
        if (e->info[id].binding == PS_LANG_BUILTIN_STRING_COUNT ||
            e->info[id].binding == PS_LANG_BUILTIN_STRING_BYTE_COUNT ||
            e->info[id].binding == PS_LANG_BUILTIN_STRING_IS_EMPTY) {
            size_t receiver = e->info[id].binding == PS_LANG_BUILTIN_STRING_BYTE_COUNT
                                  ? e->nodes[n->a].a : n->a;
            expression(e, receiver);
            if (e->info[id].binding == PS_LANG_BUILTIN_STRING_IS_EMPTY)
                out(e, "bool psv_%zu = psrt_string_byte_count(&psv_%zu) == 0;\n", id, receiver);
            else
                out(e, "int64_t psv_%zu = (int64_t)%s(&psv_%zu);\n", id,
                    e->info[id].binding == PS_LANG_BUILTIN_STRING_COUNT
                        ? "psrt_string_scalar_count" : "psrt_string_byte_count", receiver);
            break;
        }
        if (e->info[id].binding == PS_LANG_BUILTIN_ENUM_RAW_VALUE) {
            expression(e, n->a);
            out(e, "int64_t psv_%zu = psraw_value_%zu(psv_%zu);\n", id,
                (size_t)(e->info[n->a].type - PS_TYPE_RECORD_BASE), n->a);
            break;
        }
        if (ps_lang_record_type(t) && e->nodes[e->info[id].binding].kind == PS_AST_CASE) {
            if (enum_payload(e, t))
                out(e, "%s psv_%zu = {0}; psv_%zu.tag = INT64_C(%zu);\n",
                    type(e, t), id, id, e->info[id].binding);
            else
                out(e, "%s psv_%zu = INT64_C(%zu);\n", type(e, t), id, e->info[id].binding);
            break;
        }
        expression(e, n->a);
        out(e, "%s psv_%zu = psv_%zu.", type(e, t), id, n->a);
        field_name(e, e->info[id].binding);
        out(e, ";\n");
        break;
    case PS_AST_BINARY:
        binary(e, id);
        break;
    case PS_AST_CONDITIONAL:
        expression(e, n->a);
        out(e, "%s psv_%zu;\nif (psv_%zu) {\n", type(e, t), id, n->a);
        if (e->arrays)
            out(e, "psrt_cleanup *psbranch_%zu = psrt_cleanups;\n", id);
        expression(e, n->b);
        out(e, "psv_%zu = psv_%zu;\n", id, n->b);
        if (owns(e, t))
            retain_temp(e, id);
        if (e->arrays)
            out(e, "psrt_cleanup_unwind(psbranch_%zu);\n", id);
        out(e, "} else {\n");
        if (e->arrays)
            out(e, "psrt_cleanup *psbranch_%zu = psrt_cleanups;\n", id);
        expression(e, n->c);
        out(e, "psv_%zu = psv_%zu;\n", id, n->c);
        if (owns(e, t))
            retain_temp(e, id);
        if (e->arrays)
            out(e, "psrt_cleanup_unwind(psbranch_%zu);\n", id);
        out(e, "}\n");
        if (owns(e, t))
            register_temp(e, id);
        e->depth--;
        return;
    case PS_AST_UNARY:
        if (n->token.kind == PS_LANG_MINUS && t == PS_TYPE_INT64 &&
            e->nodes[n->a].kind == PS_AST_LITERAL &&
            integer(e, e->nodes[n->a].token) == (uint64_t)INT64_MAX + 1u) {
            out(e, "int64_t psv_%zu = INT64_MIN;\n", id);
            break;
        }
        expression(e, n->a);
        if (t == PS_TYPE_QUANTITY) {
            out(e, "ps_quantity psv_%zu = ", id);
            if (n->token.kind == PS_LANG_MINUS)
                out(e, "psrt_quantity_negate(");
            out(e, "psv_%zu", n->a);
            if (n->token.kind == PS_LANG_MINUS)
                out(e, ")");
            out(e, ";\n");
            break;
        }
        out(e, "%s psv_%zu = ", type(e, t), id);
        if (n->token.kind == PS_LANG_TILDE) {
            out(e, "psrt_bit_not(psv_%zu)", n->a);
        } else if (n->token.kind == PS_LANG_MINUS && t == PS_TYPE_INT64) {
            out(e, "psrt_neg(psv_%zu, ", n->a);
            site(e, id);
            out(e, ")");
        } else if (ps_lang_vector_dimensions(t)) {
            if (n->token.kind == PS_LANG_PLUS)
                out(e, "psv_%zu", n->a);
            else {
                unsigned dimensions = ps_lang_vector_dimensions(t);
                out(e, "psrt_vec%u(", dimensions);
                for (unsigned axis = 0; axis < dimensions; axis++)
                    out(e, "-psv_%zu.%c, ", n->a, "xyzw"[axis]);
                site(e, id);
                out(e, ")");
            }
        } else
            out(e, "%spsv_%zu",
                n->token.kind == PS_LANG_NOT     ? "!"
                : n->token.kind == PS_LANG_MINUS ? "-"
                                                 : "+",
                n->a);
        out(e, ";\n");
        break;
    default:
        fail(e, id, "Expression is not supported by C backend");
        break;
    }
    if (owns(e, t) && n->kind != PS_AST_ARRAY &&
        !(n->kind == PS_AST_BINARY && n->token.kind == PS_LANG_COALESCE) &&
        !(n->kind == PS_AST_BINARY && n->token.kind == PS_LANG_PLUS &&
          t >= PS_TYPE_ARRAY_BASE)) {
        if ((n->kind != PS_AST_CALL && n->kind != PS_AST_LITERAL &&
             n->kind != PS_AST_STRING_SEGMENT &&
             !(n->kind == PS_AST_BINARY && n->token.kind == PS_LANG_PLUS &&
               t == PS_TYPE_STRING)) ||
            (e->info[id].binding < e->node_count && !ps_lang_builtin_get(e->info[id].binding) &&
            (e->nodes[e->info[id].binding].kind == PS_AST_STRUCT ||
             e->nodes[e->info[id].binding].kind == PS_AST_CASE)))
            retain_temp(e, id);
        register_temp(e, id);
    }
    e->depth--;
}
static void statement(emitter *e, size_t id);
static void raw_constant(emitter *e, int64_t value);
static int indexed_target(emitter *e, size_t id) {
    while (e->nodes[id].kind == PS_AST_MEMBER || e->nodes[id].kind == PS_AST_INDEX) {
        if (e->nodes[id].kind == PS_AST_INDEX)
            return 1;
        id = e->nodes[id].a;
    }
    return 0;
}
static void writeback(emitter *e, size_t id, size_t node, size_t value) {
    while (e->nodes[node].kind == PS_AST_MEMBER || e->nodes[node].kind == PS_AST_INDEX) {
        size_t parent = e->nodes[node].a;
        if (e->nodes[node].kind == PS_AST_INDEX) {
            out(e, "psvalue_check(psrt_array_replace(&psv_%zu,(size_t)psv_%zu,1,&psv_%zu,1)",
                parent, e->nodes[node].b, value);
            checked_end(e, id);
        } else {
            if (owns(e, e->info[node].type)) {
                retain_temp(e, value);
                destroyer(e, e->info[node].type);
                out(e, "(&psv_%zu.", parent);
                field_name(e, e->info[node].binding);
                out(e, ");\n");
            }
            out(e, "psv_%zu.", parent);
            field_name(e, e->info[node].binding);
            out(e, " = psv_%zu;\n", value);
        }
        value = parent;
        node = parent;
    }
    if (owns(e, e->info[value].type)) {
        retain_temp(e, value);
        destroyer(e, e->info[value].type);
        out(e, "(&");
        storage(e, e->info[node].binding);
        out(e, ");\n");
    }
    storage(e, e->info[node].binding);
    out(e, " = psv_%zu;\n", value);
}
static void indexed_assignment(emitter *e, size_t id) {
    const ps_lang_node *assignment = &e->nodes[id];
    size_t index = assignment->a;
    if (e->nodes[index].kind == PS_AST_INDEX && e->info[e->nodes[index].b].type == PS_TYPE_RANGE) {
        size_t parent = e->nodes[index].a, range_id = e->nodes[index].b;
        size_t range = range_base(e, range_id), step = range_step(e, range_id);
        expression(e, parent);
        if (e->nodes[range].a)
            expression(e, e->nodes[range].a);
        if (e->nodes[range].b)
            expression(e, e->nodes[range].b);
        if (step)
            expression(e, step);
        out(e, "size_t psrange_start_%zu, psrange_count_%zu;\n", id, id);
        out(e, "psvalue_check(psrt_array_range_bounds(&psv_%zu,", parent);
        range_arguments(e, &e->nodes[range]);
        out(e, ",&psrange_start_%zu,&psrange_count_%zu)", id, id);
        checked_end(e, id);
        positive_step(e, step);
        if (assignment->token.kind == PS_LANG_PLUS_EQUAL) {
            out(e, "psrt_array psv_%zu; psvalue_check(psrt_array_slice_strided_bounds(&psv_%zu,",
                index, parent);
            range_arguments(e, &e->nodes[range]);
            out(e, ",1,&psv_%zu)", index);
            checked_end(e, id);
            register_temp(e, index);
            e->cached = index;
            expression(e, assignment->b);
            e->cached = 0;
        } else
            expression(e, assignment->b);
        if (step)
            out(e, "psvalue_check(psrt_array_scatter(&psv_%zu,psrange_start_%zu,"
                   "psrange_count_%zu,psv_%zu,&psv_%zu)",
                parent, id, id, step, assignment->b);
        else
            out(e, "psvalue_check(psrt_array_replace(&psv_%zu,psrange_start_%zu,"
                   "psrange_count_%zu,psrt_array_data(&psv_%zu),psrt_array_count(&psv_%zu))",
                parent, id, id, assignment->b, assignment->b);
        checked_end(e, id);
        writeback(e, id, parent, parent);
        return;
    }
    expression(e, assignment->a);
    e->cached = assignment->a;
    expression(e, assignment->b);
    e->cached = 0;
    writeback(e, id, assignment->a, assignment->b);
}
static void publish_self(emitter *e) {
    size_t self = e->self_parameter;
    if (!self)
        return;
    if (owns(e, e->info[self].type)) {
        destroyer(e, e->info[self].type);
        out(e, "(psmut_%zu);\n", self);
    }
    out(e, "*psmut_%zu = psv_%zu;\n", self, self);
    if (owns(e, e->info[self].type))
        out(e, "memset(&psv_%zu,0,sizeof psv_%zu);\n", self, self);
}
static void sequence(emitter *e, size_t first) {
    for (size_t id = first; id && e->error.kind != PS_LANG_ERROR; id = e->nodes[id].next)
        statement(e, id);
}
static void block(emitter *e, size_t id) {
    out(e, "{\n");
    if (e->arrays)
        out(e, "psrt_cleanup *psmark_%zu = psrt_cleanups;\n", id);
    sequence(e, e->nodes[id].a);
    if (e->arrays)
        out(e, "psrt_cleanup_unwind(psmark_%zu);\n", id);
    out(e, "}\n");
}
static void conditional_block(emitter *e, size_t binding, size_t body) {
    if (e->nodes[binding].kind != PS_AST_OPTIONAL_BINDING) {
        block(e, body);
        return;
    }
    out(e, "{\npsrt_cleanup *psmark_%zu = psrt_cleanups;\n", body);
    out(e, "%s psv_%zu; memcpy(&psv_%zu,psrt_array_data(&psv_%zu),sizeof psv_%zu);\n",
        type(e, e->info[binding].type), binding, binding, e->nodes[binding].a, binding);
    if (owns(e, e->info[binding].type)) {
        retain_temp(e, binding);
        register_temp(e, binding);
    }
    sequence(e, e->nodes[body].a);
    out(e, "psrt_cleanup_unwind(psmark_%zu);\n}\n", body);
}
static void optional_payload_pointer(emitter *e, size_t id, const char *owner,
                                     char result[128]) {
    int length = snprintf(result, 128, "(const %s *)psrt_array_data(%s)",
                          type(e, e->info[id].type), owner);
    if (length < 0 || length >= 128)
        fail(e, id, "Optional payload nesting is too deep for C emission");
}
static void prepare_payload_pattern(emitter *e, size_t id, unsigned depth) {
    if (depth >= 128) {
        fail(e, id, "Payload pattern nesting limit exceeded");
        return;
    }
    ps_lang_type t = e->info[id].type;
    const ps_lang_node *n = &e->nodes[id];
    if (n->kind == PS_AST_NAME ||
        (t == PS_TYPE_INT64 && n->kind == PS_AST_BINARY) ||
        (ps_lang_optional_type(t) && n->kind == PS_AST_LITERAL))
        return;
    if (t == PS_TYPE_FLOAT64 && n->kind == PS_AST_BINARY) {
        expression(e, n->a);
        expression(e, n->b);
        return;
    }
    if (ps_lang_optional_type(t) && n->kind == PS_AST_CALL) {
        out(e, "const psrt_array *pspat_%zu = NULL;\n", id);
        prepare_payload_pattern(e, e->nodes[n->b].a, depth + 1);
        return;
    }
    if (ps_lang_optional_type(t) && n->kind == PS_AST_MEMBER) return;
    if (t == PS_TYPE_STRING) {
        if (n->kind == PS_AST_BINARY) {
            string_literal(e, n->a, e->nodes[n->a].token);
            string_literal(e, n->b, e->nodes[n->b].token);
        } else
            string_literal(e, id, n->token);
    } else
        expression(e, id);
}
static void payload_pattern_condition(emitter *e, size_t id, ps_lang_type t,
                                      const char *pointer, unsigned depth) {
    if (depth >= 128) {
        fail(e, id, "Payload pattern nesting limit exceeded");
        return;
    }
    const ps_lang_node *n = &e->nodes[id];
    if (n->kind == PS_AST_NAME) {
        out(e, "true");
    } else if (ps_lang_optional_type(t)) {
        if (n->kind == PS_AST_LITERAL)
            out(e, "psrt_array_count((const psrt_array *)%s)==0", pointer);
        else if (n->kind == PS_AST_MEMBER)
            out(e, "psrt_array_count((const psrt_array *)%s)==1", pointer);
        else {
            size_t child = e->nodes[n->b].a;
            char owner[40], inner[128];
            snprintf(owner, sizeof owner, "pspat_%zu", id);
            optional_payload_pointer(e, child, owner, inner);
            out(e, "(pspat_%zu=(const psrt_array *)%s,psrt_array_count(pspat_%zu)==1 && ",
                id, pointer, id);
            payload_pattern_condition(e, child, e->info[child].type, inner, depth + 1);
            out(e, ")");
        }
    } else if (t == PS_TYPE_STRING) {
        if (n->kind == PS_AST_BINARY)
            out(e, "(strcmp(psrt_string_cstr((const psrt_string *)%s),"
                   "(const char *)pslit_%zu)>=0 && "
                   "strcmp(psrt_string_cstr((const psrt_string *)%s),"
                   "(const char *)pslit_%zu)%s0)",
                pointer, n->a, pointer, n->b,
                n->token.kind == PS_LANG_RANGE_OPEN ? "<" : "<=");
        else
            out(e, "strcmp(psrt_string_cstr((const psrt_string *)%s),"
                   "(const char *)pslit_%zu)==0", pointer, id);
    } else if (t == PS_TYPE_INT64 && n->kind == PS_AST_BINARY) {
        out(e, "(*((const int64_t *)%s) >= ", pointer);
        raw_constant(e, e->info[n->a].raw_value);
        out(e, " && *((const int64_t *)%s) %s ", pointer,
            n->token.kind == PS_LANG_RANGE_OPEN ? "<" : "<=");
        raw_constant(e, e->info[n->b].raw_value);
        out(e, ")");
    } else if (t == PS_TYPE_FLOAT64 && n->kind == PS_AST_BINARY) {
        out(e, "(*((const double *)%s) >= psv_%zu && *((const double *)%s) %s psv_%zu)",
            pointer, n->a, pointer,
            n->token.kind == PS_LANG_RANGE_OPEN ? "<" : "<=", n->b);
    } else
        out(e, "*((const %s *)%s) == psv_%zu", type(e, t), pointer, id);
}
static void payload_pattern_bindings(emitter *e, size_t id, ps_lang_type t,
                                     const char *pointer, unsigned depth) {
    if (depth >= 128) {
        fail(e, id, "Payload pattern nesting limit exceeded");
        return;
    }
    const ps_lang_node *n = &e->nodes[id];
    if (n->kind == PS_AST_NAME) {
        if (n->token.length == 1 && e->source[n->token.offset] == '_') return;
        out(e, "%s psv_%zu; memcpy(&psv_%zu,%s,sizeof psv_%zu);\n",
            type(e, t), id, id, pointer, id);
        if (owns(e, t)) {
            retain_temp(e, id);
            register_temp(e, id);
        }
    } else if (ps_lang_optional_type(t) && n->kind == PS_AST_CALL) {
        size_t child = e->nodes[n->b].a;
        char owner[40], inner[128];
        snprintf(owner, sizeof owner, "pspat_%zu", id);
        optional_payload_pointer(e, child, owner, inner);
        payload_pattern_bindings(e, child, e->info[child].type, inner, depth + 1);
    }
}
static void statement(emitter *e, size_t id) {
    if (e->error.kind == PS_LANG_ERROR)
        return;
    if (e->depth >= 128) {
        fail(e, id, "Code generation nesting limit exceeded");
        return;
    }
    e->depth++;
    const ps_lang_node *n = &e->nodes[id];
    source_line(e, id);
    switch (n->kind) {
    case PS_AST_GENERIC_FUNCTION:
        for (size_t concrete = 1; concrete < e->node_count; concrete++)
            if (e->nodes[concrete].kind == PS_AST_LOCAL_FUNCTION &&
                e->info[concrete].generic_origin == id)
                expression(e, concrete);
        break;
    case PS_AST_LOCAL_FUNCTION:
        expression(e, id);
        break;
    case PS_AST_VARIABLE:
        if (!n->b) {
            if (!e->global[id]) {
                out(e, "%s ", type(e, e->info[id].type));
                storage(e, id);
                out(e, " = {0};\nbool psready_%zu = false;\n", id);
                if (owns(e, e->info[id].type)) {
                    out(e, "psrt_cleanup pslocal_%zu; psrt_cleanup_push(&pslocal_%zu,&", id, id);
                    storage(e, id);
                    out(e, ",");
                    destroyer(e, e->info[id].type);
                    out(e, ");\n");
                }
            }
            break;
        }
        expression(e, n->b);
        if (!e->global[id])
            out(e, "%s ", type(e, e->info[id].type));
        storage(e, id);
        out(e, " = psv_%zu;\n", n->b);
        if (owns(e, e->info[id].type)) {
            keeper(e, e->info[id].type);
            out(e, "(&");
            storage(e, id);
            checked_end(e, id);
            if (!e->global[id]) {
                out(e, "psrt_cleanup pslocal_%zu; psrt_cleanup_push(&pslocal_%zu,&", id, id);
                storage(e, id);
                out(e, ",");
                destroyer(e, e->info[id].type);
                out(e, ");\n");
            }
        }
        if (e->global[id])
            out(e, "%spsready_%zu = true;\n", e->experiment ? "psstate->" : "", id);
        break;
    case PS_AST_ASSIGN: {
        if (indexed_target(e, n->a)) {
            indexed_assignment(e, id);
            break;
        }
        size_t assignment_root = target_root(e, n->a);
        int first_assignment = e->nodes[n->a].kind == PS_AST_NAME &&
            n->token.kind == PS_LANG_EQUAL &&
            e->nodes[assignment_root].kind == PS_AST_VARIABLE &&
            !e->nodes[assignment_root].b;
        if (!first_assignment)
            initialized(e, assignment_root, n->a);
        expression(e, n->b);
        if (owns(e, e->info[n->a].type)) {
            retain_temp(e, n->b);
            destroyer(e, e->info[n->a].type);
            out(e, "(&");
            target(e, n->a);
            out(e, ");\n");
        }
        target(e, n->a);
        out(e, " = psv_%zu;\n", n->b);
        if (first_assignment)
            out(e, "%spsready_%zu = true;\n",
                e->experiment && e->global[assignment_root] ? "psstate->" : "",
                assignment_root);
        break;
    }
    case PS_AST_EXPRESSION:
        if (e->info[n->a].type == PS_TYPE_RANGE) {
            size_t base = range_base(e, n->a), step = range_step(e, n->a);
            if (e->nodes[base].a)
                expression(e, e->nodes[base].a);
            if (e->nodes[base].b)
                expression(e, e->nodes[base].b);
            if (step) {
                expression(e, step);
                nonzero_step(e, step);
            }
        } else
            expression(e, n->a);
        break;
    case PS_AST_RETURN:
        if (n->a)
            expression(e, n->a);
        publish_self(e);
        if (n->a && owns(e, e->info[n->a].type)) {
            out(e, "%s psreturn_%zu = psv_%zu;\n", type(e, e->info[n->a].type), id, n->a);
            keeper(e, e->info[n->a].type);
            out(e, "(&psreturn_%zu", id);
            checked_end(e, id);
        }
        if (e->arrays)
            out(e, "psrt_cleanup_unwind(psfunction_mark);\n");
        out(e, "psrt_leave();\nreturn");
        if (n->a)
            out(e, owns(e, e->info[n->a].type) ? " psreturn_%zu" : " psv_%zu",
                owns(e, e->info[n->a].type) ? id : n->a);
        out(e, ";\n");
        break;
    case PS_AST_IF: {
        int binding = e->nodes[n->a].kind == PS_AST_OPTIONAL_BINDING;
        size_t source = binding ? e->nodes[n->a].a : n->a;
        if (binding)
            out(e, "{\npsrt_cleanup *pscondition_%zu = psrt_cleanups;\n", id);
        expression(e, source);
        out(e, binding ? "if (psrt_array_count(&psv_%zu)) " : "if (psv_%zu) ", source);
        conditional_block(e, n->a, n->b);
        if (n->c) {
            out(e, "else {\n");
            if (e->arrays)
                out(e, "psrt_cleanup *pselse_%zu = psrt_cleanups;\n", id);
            if (e->nodes[n->c].kind == PS_AST_IF)
                statement(e, n->c);
            else
                sequence(e, e->nodes[n->c].a);
            if (e->arrays)
                out(e, "psrt_cleanup_unwind(pselse_%zu);\n", id);
            out(e, "}\n");
        }
        if (binding)
            out(e, "psrt_cleanup_unwind(pscondition_%zu);\n}\n", id);
        break;
    }
    case PS_AST_GUARD: {
        int binding = e->nodes[n->a].kind == PS_AST_OPTIONAL_BINDING;
        size_t source = binding ? e->nodes[n->a].a : n->a;
        expression(e, source);
        out(e, binding ? "if (!psrt_array_count(&psv_%zu)) {\n"
                       : "if (!psv_%zu) {\n", source);
        block(e, n->b);
        out(e, "psrt_fail(");
        site(e, id);
        out(e, ", \"Guard else did not exit\");\n}\n");
        if (binding) {
            out(e, "%s psv_%zu; memcpy(&psv_%zu,psrt_array_data(&psv_%zu),"
                   "sizeof psv_%zu);\n",
                type(e, e->info[n->a].type), n->a, n->a, source, n->a);
            if (owns(e, e->info[n->a].type)) {
                retain_temp(e, n->a);
                register_temp(e, n->a);
            }
        }
        break;
    }
    case PS_AST_SWITCH: {
        size_t saved_break = e->break_mark;
        e->break_mark = id;
        out(e, "{\n");
        if (e->arrays)
            out(e, "psrt_cleanup *psloop_%zu = psrt_cleanups;\n", id);
        expression(e, n->a);
        for (size_t arm = n->b; arm; arm = e->nodes[arm].next) {
            out(e, "{\n");
            if (e->arrays)
                out(e, "psrt_cleanup *psarm_%zu = psrt_cleanups;\n", arm);
            if (e->nodes[arm].a) {
                if (e->info[n->a].type == PS_TYPE_STRING)
                    for (size_t pattern = e->nodes[arm].a; pattern;
                         pattern = e->nodes[pattern].next) {
                        if (e->nodes[pattern].kind == PS_AST_BINARY) {
                            size_t lower = e->nodes[pattern].a, upper = e->nodes[pattern].b;
                            string_literal(e, lower, e->nodes[lower].token);
                            string_literal(e, upper, e->nodes[upper].token);
                        } else
                            string_literal(e, pattern, e->nodes[pattern].token);
                    }
                if (e->info[n->a].type == PS_TYPE_FLOAT64)
                    for (size_t pattern = e->nodes[arm].a; pattern;
                         pattern = e->nodes[pattern].next) {
                        if (e->nodes[pattern].kind == PS_AST_BINARY) {
                            expression(e, e->nodes[pattern].a);
                            expression(e, e->nodes[pattern].b);
                        } else
                            expression(e, pattern);
                    }
                for (size_t pattern = e->nodes[arm].a; pattern;
                     pattern = e->nodes[pattern].next)
                    if (e->nodes[pattern].kind == PS_AST_CALL)
                        for (size_t arg = e->nodes[pattern].b; arg;
                             arg = e->nodes[arg].next) {
                            size_t value = e->nodes[arg].a;
                            prepare_payload_pattern(e, value, 0);
                        }
                out(e, "if (");
                int first_pattern = 1;
                for (size_t pattern = e->nodes[arm].a; pattern;
                     pattern = e->nodes[pattern].next) {
                    ps_lang_type subject = e->info[n->a].type;
                    if (ps_lang_optional_type(subject)) {
                        out(e, "%spsrt_array_count(&psv_%zu) == %" PRId64,
                            first_pattern ? "" : " || ", n->a,
                            e->info[pattern].raw_value);
                        if (e->nodes[pattern].kind == PS_AST_CALL) {
                            size_t value = e->nodes[e->nodes[pattern].b].a;
                            if (e->nodes[value].kind != PS_AST_NAME) {
                                char owner[40], pointer[128];
                                snprintf(owner, sizeof owner, "&psv_%zu", n->a);
                                optional_payload_pointer(e, value, owner, pointer);
                                out(e, " && ");
                                payload_pattern_condition(e, value, e->info[value].type,
                                                          pointer, 0);
                            }
                        }
                    }
                    else if (subject == PS_TYPE_STRING) {
                        if (e->nodes[pattern].kind == PS_AST_BINARY) {
                            size_t lower = e->nodes[pattern].a, upper = e->nodes[pattern].b;
                            out(e, "%s(strcmp(psrt_string_cstr(&psv_%zu),"
                                   "(const char *)pslit_%zu)>=0 && "
                                   "strcmp(psrt_string_cstr(&psv_%zu),"
                                   "(const char *)pslit_%zu)%s0)",
                                first_pattern ? "" : " || ", n->a, lower, n->a, upper,
                                e->nodes[pattern].token.kind == PS_LANG_RANGE_OPEN ? "<" : "<=");
                        } else
                            out(e, "%sstrcmp(psrt_string_cstr(&psv_%zu),"
                                   "(const char *)pslit_%zu)==0",
                                first_pattern ? "" : " || ", n->a, pattern);
                    }
                    else if (subject == PS_TYPE_FLOAT64) {
                        if (e->nodes[pattern].kind == PS_AST_BINARY) {
                            size_t lower = e->nodes[pattern].a, upper = e->nodes[pattern].b;
                            out(e, "%s(psv_%zu >= psv_%zu && psv_%zu %s psv_%zu)",
                                first_pattern ? "" : " || ", n->a, lower, n->a,
                                e->nodes[pattern].token.kind == PS_LANG_RANGE_OPEN ? "<" : "<=",
                                upper);
                        } else
                            out(e, "%spsv_%zu == psv_%zu", first_pattern ? "" : " || ",
                                n->a, pattern);
                    }
                    else if (subject == PS_TYPE_INT64 &&
                             e->nodes[pattern].kind == PS_AST_BINARY) {
                        size_t lower = e->nodes[pattern].a, upper = e->nodes[pattern].b;
                        out(e, "%s(psv_%zu >= ", first_pattern ? "" : " || ", n->a);
                        raw_constant(e, e->info[lower].raw_value);
                        out(e, " && psv_%zu %s ", n->a,
                            e->nodes[pattern].token.kind == PS_LANG_RANGE_OPEN ? "<" : "<=");
                        raw_constant(e, e->info[upper].raw_value);
                        out(e, ")");
                    } else if (subject == PS_TYPE_BOOL || subject == PS_TYPE_INT64) {
                        out(e, "%spsv_%zu == ", first_pattern ? "" : " || ", n->a);
                        if (subject == PS_TYPE_BOOL)
                            out(e, e->info[pattern].raw_value ? "true" : "false");
                        else
                            raw_constant(e, e->info[pattern].raw_value);
                    } else {
                        out(e, "%spsv_%zu%s == INT64_C(%zu)", first_pattern ? "" : " || ",
                            n->a, enum_payload(e, subject) ? ".tag" : "",
                            e->info[pattern].binding);
                        if (e->nodes[pattern].kind == PS_AST_CALL)
                            for (size_t arg = e->nodes[pattern].b; arg;
                                 arg = e->nodes[arg].next) {
                                size_t value = e->nodes[arg].a;
                                if (e->nodes[value].kind == PS_AST_NAME) continue;
                                size_t item = e->info[pattern].binding;
                                size_t field = e->info[arg].binding;
                                char pointer[128];
                                int length = snprintf(pointer, sizeof pointer,
                                    "&psv_%zu.psdata.pscase_%zu.psfield_%zu", n->a, item, field);
                                if (length < 0 || length >= (int)sizeof pointer)
                                    fail(e, value, "Enum payload path is too long for C emission");
                                out(e, " && ");
                                payload_pattern_condition(e, value, e->info[value].type,
                                                          pointer, 0);
                            }
                    }
                    first_pattern = 0;
                }
                out(e, ") {\n");
            } else
                out(e, "if (true) {\n");
            size_t pattern = e->nodes[arm].a;
            if (pattern && e->nodes[pattern].kind == PS_AST_CALL &&
                ps_lang_optional_type(e->info[n->a].type)) {
                size_t value = e->nodes[e->nodes[pattern].b].a;
                char owner[40], pointer[128];
                snprintf(owner, sizeof owner, "&psv_%zu", n->a);
                optional_payload_pointer(e, value, owner, pointer);
                payload_pattern_bindings(e, value, e->info[value].type, pointer, 0);
            } else if (pattern && e->nodes[pattern].kind == PS_AST_CALL) {
                size_t item = e->info[pattern].binding;
                for (size_t a = e->nodes[pattern].b; a; a = e->nodes[a].next) {
                    size_t value = e->nodes[a].a;
                    size_t field = e->info[a].binding;
                    char pointer[128];
                    int length = snprintf(pointer, sizeof pointer,
                        "&psv_%zu.psdata.pscase_%zu.psfield_%zu", n->a, item, field);
                    if (length < 0 || length >= (int)sizeof pointer)
                        fail(e, value, "Enum payload path is too long for C emission");
                    payload_pattern_bindings(e, value, e->info[value].type, pointer, 0);
                }
            }
            if (e->nodes[arm].c) {
                expression(e, e->nodes[arm].c);
                out(e, "if (psv_%zu) {\n", e->nodes[arm].c);
            }
            block(e, e->nodes[arm].b);
            if (e->arrays)
                out(e, "psrt_cleanup_unwind(psarm_%zu);\n", arm);
            out(e, "goto psafter_switch_%zu;\n", id);
            if (e->nodes[arm].c)
                out(e, "}\n");
            if (e->arrays)
                out(e, "psrt_cleanup_unwind(psarm_%zu);\n", arm);
            out(e, "}\n");
            if (e->arrays)
                out(e, "psrt_cleanup_unwind(psarm_%zu);\n", arm);
            out(e, "}\n");
        }
        out(e, "psafter_switch_%zu:;\n", id);
        if (e->arrays)
            out(e, "psrt_cleanup_unwind(psloop_%zu);\n", id);
        out(e, "}\n");
        e->break_mark = saved_break;
        break;
    }
    case PS_AST_WHILE: {
        size_t saved_break = e->break_mark, saved_continue = e->continue_mark;
        e->break_mark = e->continue_mark = id;
        out(e, "while (true) {\n");
        if (e->arrays)
            out(e, "psrt_cleanup *psloop_%zu = psrt_cleanups;\n", id);
        int binding = e->nodes[n->a].kind == PS_AST_OPTIONAL_BINDING;
        size_t source = binding ? e->nodes[n->a].a : n->a;
        expression(e, source);
        out(e, binding ? "if (!psrt_array_count(&psv_%zu)) {\n" : "if (!psv_%zu) {\n", source);
        if (e->arrays)
            out(e, "psrt_cleanup_unwind(psloop_%zu);\n", id);
        out(e, "break; }\n");
        conditional_block(e, n->a, n->b);
        if (e->arrays)
            out(e, "psrt_cleanup_unwind(psloop_%zu);\n", id);
        out(e, "}\n");
        e->break_mark = saved_break;
        e->continue_mark = saved_continue;
        break;
    }
    case PS_AST_FOR: {
        size_t saved_break = e->break_mark, saved_continue = e->continue_mark;
        e->break_mark = e->continue_mark = id;
        if (e->info[n->a].type == PS_TYPE_STRING) {
            out(e, "{\npsrt_cleanup *psiter_%zu = psrt_cleanups;\n", id);
            expression(e, n->a);
            out(e, "for(size_t psbyte_%zu=0; psbyte_%zu<psrt_string_byte_count(&psv_%zu); ) "
                   "{\npsrt_cleanup *psloop_%zu = psrt_cleanups;\n",
                id, id, n->a, id);
            out(e, "psrt_string psv_%zu; psstring_check(psrt_string_next_scalar(&psv_%zu,"
                   "&psbyte_%zu,psrt_memory_allocator(&%s),&psv_%zu)",
                id, n->a, id, e->experiment ? "psstate->memory" : "psmemory", id);
            checked_end(e, id);
            register_temp(e, id);
            block(e, n->b);
            out(e, "psrt_cleanup_unwind(psloop_%zu);\n}\npsrt_cleanup_unwind(psiter_%zu);\n}\n",
                id, id);
            e->break_mark = saved_break;
            e->continue_mark = saved_continue;
            break;
        }
        if (e->info[n->a].type >= PS_TYPE_ARRAY_BASE) {
            out(e, "{\npsrt_cleanup *psiter_%zu = psrt_cleanups;\n", id);
            expression(e, n->a);
            out(e,
                "for(size_t psindex_%zu=0; psindex_%zu<psrt_array_count(&psv_%zu); psindex_%zu++) "
                "{\n",
                id, id, n->a, id);
            out(e, "psrt_cleanup *psloop_%zu = psrt_cleanups;\n", id);
            out(e,
                "const void *psitem_%zu; "
                "psvalue_check(psrt_array_at(&psv_%zu,(int64_t)psindex_%zu,&psitem_%zu)",
                id, n->a, id, id);
            checked_end(e, id);
            out(e, "%s psv_%zu; memcpy(&psv_%zu,psitem_%zu,sizeof psv_%zu);\n",
                type(e, e->info[id].type), id, id, id, id);
            if (owns(e, e->info[id].type)) {
                retain_temp(e, id);
                register_temp(e, id);
            }
            block(e, n->b);
            out(e, "psrt_cleanup_unwind(psloop_%zu);\n}\npsrt_cleanup_unwind(psiter_%zu);\n}\n", id,
                id);
            e->break_mark = saved_break;
            e->continue_mark = saved_continue;
            break;
        }
        size_t base = range_base(e, n->a), step = range_step(e, n->a);
        const ps_lang_node *range = &e->nodes[base];
        expression(e, range->a);
        expression(e, range->b);
        if (step) {
            expression(e, step);
            nonzero_step(e, step);
        }
        out(e, "{\nint64_t psv_%zu = psv_%zu;\nbool psfirst_%zu = true;\nwhile (true) {\n", id,
            range->a, id);
        if (step)
            out(e, "if (!psfirst_%zu) {\n"
                   "if (psv_%zu > 0) { if (psv_%zu > INT64_MAX - psv_%zu) break; }\n"
                   "else { if (psv_%zu < INT64_MIN - psv_%zu) break; }\n"
                   "psv_%zu += psv_%zu;\n}\n", id, step, id, step, id, step, id, step);
        else
            out(e, "if (!psfirst_%zu) {\nif (psv_%zu == INT64_MAX) break;\npsv_%zu++;\n}\n",
                id, id, id);
        if (step)
            out(e, "psfirst_%zu = false;\n"
                   "if (psv_%zu > 0 ? !(psv_%zu %s psv_%zu) : !(psv_%zu %s psv_%zu)) break;\n",
                id, step, id, range->token.kind == PS_LANG_RANGE_OPEN ? "<" : "<=",
                range->b, id, range->token.kind == PS_LANG_RANGE_OPEN ? ">" : ">=", range->b);
        else
            out(e, "psfirst_%zu = false;\nif (!(psv_%zu %s psv_%zu)) break;\n", id, id,
                range->token.kind == PS_LANG_RANGE_OPEN ? "<" : "<=", range->b);
        if (e->arrays)
            out(e, "psrt_cleanup *psloop_%zu = psrt_cleanups;\n", id);
        block(e, n->b);
        if (e->arrays)
            out(e, "psrt_cleanup_unwind(psloop_%zu);\n", id);
        out(e, "}\n}\n");
        e->break_mark = saved_break;
        e->continue_mark = saved_continue;
        break;
    }
    case PS_AST_BREAK:
        if (e->arrays)
            out(e, "psrt_cleanup_unwind(psloop_%zu);\n", e->break_mark);
        if (e->nodes[e->break_mark].kind == PS_AST_SWITCH)
            out(e, "goto psafter_switch_%zu;\n", e->break_mark);
        else
            out(e, "break;\n");
        break;
    case PS_AST_CONTINUE:
        if (e->arrays)
            out(e, "psrt_cleanup_unwind(psloop_%zu);\n", e->continue_mark);
        out(e, "continue;\n");
        break;
    default:
        fail(e, id, "Statement is not supported by C backend");
        break;
    }
    e->depth--;
}
static void record_definition(emitter *e, size_t id) {
    if (e->record_done[id] == 2 || e->error.kind == PS_LANG_ERROR)
        return;
    if (e->record_done[id] == 1) {
        fail(e, id, e->nodes[id].kind == PS_AST_ENUM
                        ? "Recursive enum values have infinite size"
                        : "Recursive struct values have infinite size");
        return;
    }
    if (e->depth >= 128) {
        fail(e, id, "Code generation nesting limit exceeded");
        return;
    }
    e->depth++;
    e->record_done[id] = 1;
    if (e->nodes[id].kind == PS_AST_ENUM) {
        for (size_t item = e->nodes[id].a; item; item = e->nodes[item].next)
            for (size_t f = e->nodes[item].a; f; f = e->nodes[f].next)
                if (ps_lang_record_type(e->info[f].type))
                    record_definition(e, (size_t)(e->info[f].type - PS_TYPE_RECORD_BASE));
        source_line(e, id);
        if (enum_payload(e, e->info[id].type)) {
            out(e, "typedef struct { int64_t tag; union {\n");
            for (size_t item = e->nodes[id].a; item; item = e->nodes[item].next) {
                if (!e->nodes[item].a) continue;
                out(e, "struct {\n");
                for (size_t f = e->nodes[item].a; f; f = e->nodes[f].next)
                    out(e, "%s psfield_%zu;\n", type(e, e->info[f].type), f);
                out(e, "} pscase_%zu;\n", item);
            }
            out(e, "} psdata; } pst_%zu;\n", id);
        } else
            out(e, "typedef int64_t pst_%zu;\n", id);
        e->record_done[id] = 2;
        e->depth--;
        return;
    }
    for (size_t f = e->nodes[id].a; f; f = e->nodes[f].next)
        if (ps_lang_record_type(e->info[f].type))
            record_definition(e, (size_t)(e->info[f].type - PS_TYPE_RECORD_BASE));
    source_line(e, id);
    out(e, "typedef struct {\n");
    if (!e->nodes[id].a)
        out(e, "unsigned char ps_empty;\n");
    for (size_t f = e->nodes[id].a; f; f = e->nodes[f].next) {
        source_line(e, f);
        out(e, "%s psfield_%zu;\n", type(e, e->info[f].type), f);
    }
    out(e, "} pst_%zu;\n", id);
    e->record_done[id] = 2;
    e->depth--;
}
static void signature(emitter *e, size_t id) {
    out(e, "static %s psfn_%zu(",
        type(e, (e->nodes[id].kind == PS_AST_LAMBDA ||
                 e->nodes[id].kind == PS_AST_LOCAL_FUNCTION)
                    ? e->info[id].lambda_result : e->info[id].type), id);
    size_t p = e->nodes[id].a;
    int lambda = e->nodes[id].kind == PS_AST_LAMBDA;
    int local = e->nodes[id].kind == PS_AST_LOCAL_FUNCTION;
    if (e->experiment)
        out(e, "ps_module_state *psstate%s", p || lambda || local ? ", " : "");
    if (lambda)
        out(e, "const pscapture_%zu *pscapture%s", id, p ? ", " : "");
    if (local)
        out(e, "const psrt_function_value *psself%s", p ? ", " : "");
    if (!p && !e->experiment && !lambda && !local)
        out(e, "void");
    for (; p; p = e->nodes[p].next) {
        int mutable_self = e->nodes[p].kind == PS_AST_SELF_PARAMETER && e->info[id].method_mutating;
        out(e, "%s %s%zu%s", type(e, e->info[p].type), mutable_self ? "*psmut_" : "psv_", p,
            e->nodes[p].next ? ", " : "");
    }
    out(e, ")");
}
static void equal_value(emitter *e, ps_lang_type t, const char *left, const char *right) {
    if (t == PS_TYPE_STRING) {
        out(e, "strcmp(psrt_string_cstr((const psrt_string *)(%s)),"
               "psrt_string_cstr((const psrt_string *)(%s)))==0", left, right);
    } else if (t == PS_TYPE_BOOL || t == PS_TYPE_INT64 || t == PS_TYPE_FLOAT64) {
        const char *name = type(e, t);
        out(e, "(*(const %s *)(%s) == *(const %s *)(%s))", name, left, name, right);
    } else if (ps_lang_vector_dimensions(t)) {
        const char *name = type(e, t);
        out(e, "(");
        for (unsigned axis = 0; axis < ps_lang_vector_dimensions(t); axis++)
            out(e, "%s((const %s *)(%s))->%c == ((const %s *)(%s))->%c",
                axis ? " && " : "", name, left, "xyzw"[axis], name, right, "xyzw"[axis]);
        out(e, ")");
    } else {
        out(e, "psequal_%u(%s,%s)", (unsigned)t, left, right);
    }
}
static void equality_definitions(emitter *e) {
    for (size_t id = 1; id < e->node_count; id++) {
        ps_lang_type types[3] = {(ps_lang_type)(PS_TYPE_RECORD_BASE + id),
                                 (ps_lang_type)(PS_TYPE_OPTIONAL_BASE + id),
                                 (ps_lang_type)(PS_TYPE_ARRAY_BASE + id)};
        for (size_t k = 0; k < 3; k++)
            if (*equal_state(e, types[k]))
                out(e, "static bool psequal_%u(const void *,const void *);\n",
                    (unsigned)types[k]);
    }
    for (size_t id = 1; id < e->node_count; id++) {
        ps_lang_type types[3] = {(ps_lang_type)(PS_TYPE_RECORD_BASE + id),
                                 (ps_lang_type)(PS_TYPE_OPTIONAL_BASE + id),
                                 (ps_lang_type)(PS_TYPE_ARRAY_BASE + id)};
        for (size_t k = 0; k < 3; k++) {
            ps_lang_type t = types[k];
            if (!*equal_state(e, t)) continue;
            out(e, "static bool psequal_%u(const void *left,const void *right) {\n", (unsigned)t);
            if (k) {
                ps_lang_type element = k == 1 ? e->info[id].optional_element
                                               : e->info[id].array_element;
                out(e, "const psrt_array *a=left,*b=right;\n"
                       "size_t count=psrt_array_count(a);\n"
                       "if(count!=psrt_array_count(b)) return false;\n"
                       "const unsigned char *x=psrt_array_data(a),*y=psrt_array_data(b);\n"
                       "for(size_t i=0;i<count;i++) if(!(");
                equal_value(e, element, "x+i*a->type->size", "y+i*b->type->size");
                out(e, ")) return false;\nreturn true; }\n");
                continue;
            }
            out(e, "const %s *a=left,*b=right;\n", type(e, t));
            if (e->nodes[id].kind == PS_AST_ENUM) {
                if (!enum_payload(e, t)) {
                    out(e, "return *a==*b; }\n");
                    continue;
                }
                out(e, "if(a->tag!=b->tag) return false;\nswitch(a->tag) {\n");
                for (size_t item = e->nodes[id].a; item; item = e->nodes[item].next) {
                    out(e, "case INT64_C(%zu):\n", item);
                    for (size_t f = e->nodes[item].a; f; f = e->nodes[f].next) {
                        char l[96], r[96];
                        snprintf(l, sizeof l, "&a->psdata.pscase_%zu.psfield_%zu", item, f);
                        snprintf(r, sizeof r, "&b->psdata.pscase_%zu.psfield_%zu", item, f);
                        out(e, "if(!(");
                        equal_value(e, e->info[f].type, l, r);
                        out(e, ")) return false;\n");
                    }
                    out(e, "return true;\n");
                }
                out(e, "default: return false; } }\n");
            } else {
                for (size_t f = e->nodes[id].a; f; f = e->nodes[f].next) {
                    char l[64], r[64];
                    snprintf(l, sizeof l, "&a->psfield_%zu", f);
                    snprintf(r, sizeof r, "&b->psfield_%zu", f);
                    out(e, "if(!(");
                    equal_value(e, e->info[f].type, l, r);
                    out(e, ")) return false;\n");
                }
                out(e, "return true; }\n");
            }
        }
    }
    for (size_t id = 1; id < e->node_count; id++) {
        if (e->nodes[id].kind != PS_AST_CALL ||
            e->info[id].binding != PS_LANG_BUILTIN_ARRAY_SPLIT)
            continue;
        size_t receiver = e->nodes[e->nodes[id].a].a;
        ps_lang_type array = e->info[receiver].type;
        ps_lang_type element = e->info[array - PS_TYPE_ARRAY_BASE].array_element;
        out(e, "static bool pssplit_equal_%zu(const void *left,const void *right) { return ", id);
        equal_value(e, element, "left", "right");
        out(e, "; }\n");
    }
}
static void ownership_definitions(emitter *e) {
    out(e, "static ps_result psvalue_copy(const psrt_element_type *t, void *d, const void *s) {\n"
           "if (t->copy) return t->copy(d,s); memcpy(d,s,t->size); return PS_OK; }\n"
           "static void psvalue_check(ps_result r, psrt_site site) {\n"
           "if (r != PS_OK) psrt_fail(site, r == PS_MEMORY ? \"Array memory budget or allocation "
           "exhausted\" : \"Array index, size or ownership limit exceeded\"); }\n"
           "static void pskeep_array(psrt_array *v, psrt_site site) { psrt_array copy;\n"
           "psvalue_check(psrt_array_clone(v,&copy),site); *v=copy; }\n");
    out(e, "static void pskeep_function(psrt_function_value *v, psrt_site site) {\n"
           "psrt_function_value copy; psvalue_check(psrt_function_copy(&copy,v),site); *v=copy; }\n");
    if (e->strings)
        out(e, "static void psstring_check(ps_result r, psrt_site site) {\n"
               "if (r == PS_MEMORY) psrt_fail(site, \"String memory budget or allocation "
               "exhausted\");\n"
               "if (r == PS_LIMIT) psrt_fail(site, \"String index, slice or size out of range\");\n"
               "if (r != PS_OK) psrt_fail(site, \"Invalid String value\"); }\n");
    for (unsigned t = PS_TYPE_BOOL; t < PS_TYPE_RECORD_BASE && t < 64; t++)
        if (t != PS_TYPE_STRING && (e->primitive_types & (UINT64_C(1) << t)))
            out(e, "static const psrt_element_type psdesc_%u = {sizeof(%s),%s};\n", t,
                type(e, (ps_lang_type)t), t == PS_TYPE_CONSTRAINT_RESULT
                ? "psrt_constraints_copy,psrt_constraints_drop"
                : t == PS_TYPE_ODE_RESULT
                ? "psrt_ode_result_copy,psrt_ode_result_drop" : "NULL,NULL");
    for (size_t id = 1; id < e->node_count; id++)
        if (e->info[id].function_type == (ps_lang_type)(PS_TYPE_FUNCTION_BASE + id))
            out(e, "static const psrt_element_type psdesc_%u = "
                   "{sizeof(psrt_function_value),psrt_function_copy,psrt_function_destroy};\n",
                (unsigned)e->info[id].function_type);
    for (size_t id = 1; id < e->node_count; id++) {
        if (e->nodes[id].kind != PS_AST_STRUCT && e->nodes[id].kind != PS_AST_ENUM)
            continue;
        unsigned t = (unsigned)e->info[id].type;
        out(e, "static const psrt_element_type psdesc_%u;\n", t);
        if (owns(e, e->info[id].type))
            out(e,
                "static void psdrop_%u(void *);\nstatic ps_result pscopy_%u(void *, const void "
                "*);\n",
                t, t);
    }
    for (size_t id = 1; id < e->node_count; id++) {
        if (e->nodes[id].kind != PS_AST_STRUCT && e->nodes[id].kind != PS_AST_ENUM)
            continue;
        ps_lang_type t = e->info[id].type;
        if (!owns(e, t)) {
            out(e, "static const psrt_element_type psdesc_%u = {sizeof(%s),NULL,NULL};\n",
                (unsigned)t, type(e, t));
            continue;
        }
        out(e, "static void psdrop_%u(void *object) { %s *v=object;\n", (unsigned)t, type(e, t));
        if (e->nodes[id].kind == PS_AST_ENUM) {
            out(e, "switch(v->tag) {\n");
            for (size_t item = e->nodes[id].a; item; item = e->nodes[item].next) {
                if (!e->nodes[item].a) continue;
                out(e, "case INT64_C(%zu):\n", item);
                for (size_t f = e->nodes[item].a; f; f = e->nodes[f].next)
                    if (owns(e, e->info[f].type)) {
                        destroyer(e, e->info[f].type);
                        out(e, "(&v->psdata.pscase_%zu.psfield_%zu);\n", item, f);
                    }
                out(e, "break;\n");
            }
            out(e, "default: break; }\n");
        } else {
            for (size_t stop = 0; stop != e->nodes[id].a;) {
                size_t f = e->nodes[id].a;
                while (e->nodes[f].next != stop)
                    f = e->nodes[f].next;
                if (owns(e, e->info[f].type)) {
                    destroyer(e, e->info[f].type);
                    out(e, "(&v->psfield_%zu);\n", f);
                }
                stop = f;
            }
        }
        out(e, "memset(v,0,sizeof *v); }\n");
        out(e, "static ps_result pscopy_%u(void *destination,const void *source) {\n", (unsigned)t);
        out(e, "const %s *s=source;\n", type(e, t));
        out(e, "%s v=*s; ps_result r=PS_OK;\n", type(e, t));
        if (e->nodes[id].kind == PS_AST_ENUM) {
            out(e, "switch(v.tag) {\n");
            for (size_t item = e->nodes[id].a; item; item = e->nodes[item].next) {
                if (!e->nodes[item].a) continue;
                out(e, "case INT64_C(%zu):\n", item);
                for (size_t f = e->nodes[item].a; f; f = e->nodes[f].next)
                    if (owns(e, e->info[f].type))
                        out(e, "memset(&v.psdata.pscase_%zu.psfield_%zu,0,"
                               "sizeof v.psdata.pscase_%zu.psfield_%zu);\n",
                            item, f, item, f);
                for (size_t f = e->nodes[item].a; f; f = e->nodes[f].next)
                    if (owns(e, e->info[f].type)) {
                        out(e, "r=psvalue_copy(");
                        descriptor(e, e->info[f].type);
                        out(e, ",&v.psdata.pscase_%zu.psfield_%zu,"
                               "&s->psdata.pscase_%zu.psfield_%zu); if(r!=PS_OK) goto failed;\n",
                            item, f, item, f);
                    }
                out(e, "break;\n");
            }
            out(e, "default: break; }\n");
        } else {
            for (size_t f = e->nodes[id].a; f; f = e->nodes[f].next)
                if (owns(e, e->info[f].type))
                    out(e, "memset(&v.psfield_%zu,0,sizeof v.psfield_%zu);\n", f, f);
            for (size_t f = e->nodes[id].a; f; f = e->nodes[f].next)
                if (owns(e, e->info[f].type)) {
                    out(e, "r=psvalue_copy(");
                    descriptor(e, e->info[f].type);
                    out(e, ",&v.psfield_%zu,&s->psfield_%zu); if(r!=PS_OK) goto failed;\n", f, f);
                }
        }
        out(e, "*(%s *)destination=v; return PS_OK;\n", type(e, t));
        out(e, "failed: psdrop_%u(&v); return r; }\n", (unsigned)t);
        out(e, "static const psrt_element_type psdesc_%u = {sizeof(%s),pscopy_%u,psdrop_%u};\n",
            (unsigned)t, type(e, t), (unsigned)t, (unsigned)t);
        out(e, "static void pskeep_%u(%s *v,psrt_site site) {\n", (unsigned)t, type(e, t));
        out(e, "%s copy; psvalue_check(pscopy_%u(&copy,v),site); *v=copy; }\n", type(e, t),
            (unsigned)t);
    }
}
static void closure_definitions(emitter *e) {
    for (size_t id = 1; id < e->node_count; id++) {
        if (e->nodes[id].kind != PS_AST_LAMBDA &&
            e->nodes[id].kind != PS_AST_LOCAL_FUNCTION)
            continue;
        out(e, "typedef struct {\n");
        if (!e->info[id].capture_head)
            out(e, "unsigned char empty;\n");
        for (size_t capture = e->info[id].capture_head; capture;
             capture = e->info[capture].capture_next) {
            size_t decl = e->info[capture].binding;
            out(e, "%s psfield_%zu;\n", type(e, e->info[decl].type), decl);
        }
        out(e, "} pscapture_%zu;\n", id);
        if (!e->info[id].capture_head)
            continue;
        int owned = 0;
        for (size_t capture = e->info[id].capture_head; capture;
             capture = e->info[capture].capture_next)
            owned |= owns(e, e->info[capture].type);
        if (owned) {
            out(e, "static void psdrop_capture_%zu(void *object) {\n"
                   "pscapture_%zu *v=object;\n", id, id);
            for (size_t capture = e->info[id].capture_head; capture;
                 capture = e->info[capture].capture_next) {
                if (!owns(e, e->info[capture].type)) continue;
                size_t decl = e->info[capture].binding;
                destroyer(e, e->info[capture].type);
                out(e, "(&v->psfield_%zu);\n", decl);
            }
            out(e, "memset(v,0,sizeof *v); }\n");
            out(e, "static ps_result pscopy_capture_%zu(void *destination,"
                   "const void *source) {\n"
                   "const pscapture_%zu *s=source; pscapture_%zu v=*s;"
                   "ps_result r=PS_OK;\n", id, id, id);
            for (size_t capture = e->info[id].capture_head; capture;
                 capture = e->info[capture].capture_next)
                if (owns(e, e->info[capture].type))
                    out(e, "memset(&v.psfield_%zu,0,sizeof v.psfield_%zu);\n",
                        e->info[capture].binding, e->info[capture].binding);
            for (size_t capture = e->info[id].capture_head; capture;
                 capture = e->info[capture].capture_next) {
                if (!owns(e, e->info[capture].type)) continue;
                size_t decl = e->info[capture].binding;
                out(e, "r=psvalue_copy(");
                descriptor(e, e->info[capture].type);
                out(e, ",&v.psfield_%zu,&s->psfield_%zu);"
                       "if(r!=PS_OK) goto failed;\n", decl, decl);
            }
            out(e, "*(pscapture_%zu *)destination=v; return PS_OK;\n"
                   "failed: psdrop_capture_%zu(&v); return r; }\n", id, id);
        }
        out(e, "static const psrt_element_type psdesc_capture_%zu = "
               "{sizeof(pscapture_%zu),", id, id);
        if (owned)
            out(e, "pscopy_capture_%zu,psdrop_capture_%zu", id, id);
        else
            out(e, "NULL,NULL");
        out(e, "};\n");
    }
}
static void raw_constant(emitter *e, int64_t value) {
    if (value == INT64_MIN)
        out(e, "INT64_MIN");
    else
        out(e, "INT64_C(%" PRId64 ")", value);
}
static void raw_definitions(emitter *e) {
    for (size_t id = 1; id < e->node_count; id++) {
        if (e->nodes[id].kind != PS_AST_ENUM || !e->info[id].raw_enum) continue;
        out(e, "static int64_t psraw_value_%zu(pst_%zu value) {\nswitch(value) {\n", id, id);
        for (size_t item = e->nodes[id].a; item; item = e->nodes[item].next) {
            out(e, "case INT64_C(%zu): return ", item);
            raw_constant(e, e->info[item].raw_value);
            out(e, ";\n");
        }
        out(e, "default: return 0; } }\n");
        out(e, "static bool psraw_from_%zu(int64_t raw,pst_%zu *result) {\nswitch(raw) {\n",
            id, id);
        for (size_t item = e->nodes[id].a; item; item = e->nodes[item].next) {
            out(e, "case ");
            raw_constant(e, e->info[item].raw_value);
            out(e, ": *result=INT64_C(%zu); return true;\n", item);
        }
        out(e, "default: return false; } }\n");
    }
}
static ps_lang_check_result emit(FILE *output, const char *source_path,
                                 const char *const *paths, size_t path_count, const void *source,
                                 size_t source_size, const ps_lang_node *nodes,
                                 ps_lang_parse_result parsed, const ps_lang_semantic *info,
                                 int experiment) {
    ps_lang_token bad = {PS_LANG_ERROR, 0, 0, 1, 1, "Invalid C backend input", 0};
    if (!output || !source_path || !paths || !path_count || !nodes || !info || !parsed.root ||
        parsed.root >= parsed.count)
        return (ps_lang_check_result){0, bad, parsed.count};
    emitter e = {0};
    e.out = output;
    e.source = source;
    e.source_path = source_path;
    e.paths = paths;
    e.path_count = path_count;
    e.nodes = nodes;
    e.info = info;
    e.node_count = parsed.count;
    e.experiment = experiment;
    size_t callbacks[4] = {0};
    const char *names[] = {"create", "reset", "step", "scene"};
    for (size_t id = 1; id < parsed.count; id++) {
        if (ps_lang_function_type(info[id].type) ||
            info[id].type >= PS_TYPE_OPTIONAL_BASE ||
            info[id].type == PS_TYPE_CONSTRAINT_RESULT || info[id].type == PS_TYPE_ODE_RESULT)
            e.arrays = e.sdk = 1;
        if (info[id].type == PS_TYPE_STRING || info[id].array_element == PS_TYPE_STRING ||
            info[id].optional_element == PS_TYPE_STRING)
            e.strings = e.arrays = 1;
        if ((info[id].type >= PS_TYPE_BOOL && info[id].type <= PS_TYPE_STRING) ||
            (info[id].type >= PS_TYPE_VEC2 && info[id].type <= PS_TYPE_SCALAR_RESULT))
            e.primitive_types |= UINT64_C(1) << info[id].type;
        if ((info[id].array_element >= PS_TYPE_BOOL && info[id].array_element <= PS_TYPE_STRING) ||
            (info[id].array_element >= PS_TYPE_VEC2 && info[id].array_element <= PS_TYPE_SCALAR_RESULT))
            e.primitive_types |= UINT64_C(1) << info[id].array_element;
        if ((info[id].optional_element >= PS_TYPE_BOOL && info[id].optional_element <= PS_TYPE_STRING) ||
            (info[id].optional_element >= PS_TYPE_VEC2 && info[id].optional_element <= PS_TYPE_SCALAR_RESULT))
            e.primitive_types |= UINT64_C(1) << info[id].optional_element;
        const ps_lang_builtin *builtin = ps_lang_builtin_get(info[id].binding);
        if (builtin) {
            e.sdk = 1;
            if (builtin->host && !experiment)
                fail(&e, id, "Host API requires --emit-experiment or --emit-analysis");
            else if (builtin->host && builtin->host != (unsigned)experiment)
                fail(&e, id, "Host API is unavailable in this module kind");
        }
        if (info[id].type >= PS_TYPE_VEC2 && info[id].type <= PS_TYPE_SCALAR_RESULT)
            e.sdk = 1;
        if (info[id].type >= PS_TYPE_DATASET && info[id].type <= PS_TYPE_TABLE && experiment != 2)
            fail(&e, id, "Analysis handles require --emit-analysis");
        if (experiment && info[id].binding == PS_LANG_BUILTIN_PRINT)
            fail(&e, id,
                 "print is unavailable in native modules; use reports or measurement channels");
    }
    if (experiment == 1) {
        for (size_t id = nodes[parsed.root].a; id; id = nodes[id].next) {
            if (nodes[id].kind != PS_AST_FUNCTION || info[id].generic_origin ||
                nodes[id].token.file != nodes[parsed.root].token.file)
                continue;
            for (unsigned which = 0; which < 4; which++) {
                ps_lang_token token = nodes[id].token;
                if (token.length != strlen(names[which]) ||
                    memcmp(e.source + token.offset, names[which], token.length) != 0)
                    continue;
                size_t param = nodes[id].a;
                int args_ok =
                    which == 2 ? param && !nodes[param].next && info[param].type == PS_TYPE_FLOAT64
                               : !param;
                if (!args_ok || info[id].type != PS_TYPE_VOID)
                    fail(&e, id,
                         "Invalid experiment callback signature; expected Void and only step(dt: "
                         "Float64)");
                callbacks[which] = id;
            }
        }
        for (unsigned which = 0; which < 4; which++)
            if (!callbacks[which])
                fail(&e, parsed.root,
                     "Experiment requires create(), reset(), step(dt: Float64), and scene()");
    }
    if (experiment == 2) {
        for (size_t id = nodes[parsed.root].a; id; id = nodes[id].next) {
            if (nodes[id].kind == PS_AST_FUNCTION && !info[id].generic_origin &&
                nodes[id].token.file == nodes[parsed.root].token.file && nodes[id].token.length == 7 &&
                !memcmp(e.source + nodes[id].token.offset, "analyze", 7)) {
                callbacks[0] = id;
                if (nodes[id].a || info[id].type != PS_TYPE_VOID)
                    fail(&e, id, "Analysis requires analyze() -> Void");
            }
        }
        if (!callbacks[0])
            fail(&e, parsed.root, "Analysis requires analyze() -> Void");
    }
    if (e.error.kind == PS_LANG_ERROR)
        return (ps_lang_check_result){0, e.error, parsed.count};
    e.global = calloc(parsed.count, 6);
    if (!e.global) {
        bad.error = "Cannot allocate C backend bookkeeping";
        return (ps_lang_check_result){0, bad, parsed.count};
    }
    e.record_done = e.global + parsed.count;
    e.owned = e.record_done + parsed.count;
    e.equal_record = e.owned + parsed.count;
    e.equal_optional = e.equal_record + parsed.count;
    e.equal_array = e.equal_optional + parsed.count;
    for (size_t id = 1; id < parsed.count; id++) {
        if (nodes[id].kind == PS_AST_BINARY &&
            (nodes[id].token.kind == PS_LANG_EQ || nodes[id].token.kind == PS_LANG_NE) &&
            nodes[nodes[id].a].token.kind != PS_LANG_NIL &&
            nodes[nodes[id].b].token.kind != PS_LANG_NIL)
            mark_equal_type(&e, info[nodes[id].a].type);
        if (nodes[id].kind == PS_AST_CALL &&
            (info[id].binding == PS_LANG_BUILTIN_ARRAY_CONTAINS ||
             info[id].binding == PS_LANG_BUILTIN_ARRAY_FIRST_INDEX ||
             info[id].binding == PS_LANG_BUILTIN_ARRAY_LAST_INDEX ||
             info[id].binding == PS_LANG_BUILTIN_ARRAY_SPLIT ||
             info[id].binding == PS_LANG_BUILTIN_ARRAY_STARTS_WITH ||
             info[id].binding == PS_LANG_BUILTIN_ARRAY_ELEMENTS_EQUAL)) {
            size_t receiver = nodes[nodes[id].a].a;
            ps_lang_type array = info[receiver].type;
            mark_equal_type(&e, info[array - PS_TYPE_ARRAY_BASE].array_element);
        }
    }
    out(&e, "/* Generated by physimc for Physim language " PS_LANGUAGE_VERSION_TEXT
            "; edit the .phys source. */\n"
            "#define PSRT_LANGUAGE_VERSION \"" PS_LANGUAGE_VERSION_TEXT "\"\n"
            "#define PSRT_COMPILER_VERSION \"" PS_COMPILER_VERSION_TEXT "\"\n#ifndef _WIN32\n"
            "#ifndef _GNU_SOURCE\n#define _GNU_SOURCE 1\n#endif\n#endif\n"
            "#define PSRT_SOURCE ");
    quoted(&e, (const unsigned char *)source_path, strlen(source_path), 0);
    if (experiment)
        out(&e, "\n#define PSRT_MODULE 1");
    out(&e, "\n#include <physim/%s>\n",
        experiment == 2       ? "language_analysis_sdk.h"
        : e.sdk || experiment ? "language_sdk.h"
                              : "language_runtime.h");
    if (e.strings)
        out(&e, "#include <physim/language_string.h>\n");
    else if (e.arrays)
        out(&e, "#include <physim/language_array.h>\n");
    int scalar_callbacks = 0, ode_callbacks = 0;
    for (size_t id = 1; id < parsed.count; id++) {
        if (nodes[id].kind != PS_AST_CALL) continue;
        const ps_lang_builtin *builtin = ps_lang_builtin_get(info[id].binding);
        if (!builtin || !builtin->count) continue;
        scalar_callbacks |= builtin->types[0] == PS_TYPE_FUNCTION;
        ode_callbacks |= builtin->types[0] == PS_LANG_ODE_CALLBACK;
    }
    if (scalar_callbacks)
        out(&e, "typedef struct { const psrt_function_value *function; "
                "void *module_state; psrt_site site; } "
                "psrt_scalar_value_context;\n");
    if (ode_callbacks)
        out(&e, "typedef struct { psrt_ode_callback_context base; "
                "const psrt_function_value *function; } "
                "psrt_ode_value_context;\n");
    size_t first = nodes[parsed.root].a;
    for (size_t id = 1; id < parsed.count && e.error.kind != PS_LANG_ERROR; id++)
        if (nodes[id].kind == PS_AST_ENUM)
            record_definition(&e, id);
    for (size_t id = 1; id < parsed.count && e.error.kind != PS_LANG_ERROR; id++)
        if (nodes[id].kind == PS_AST_STRUCT)
            record_definition(&e, id);
    if (e.arrays)
        ownership_definitions(&e);
    closure_definitions(&e);
    equality_definitions(&e);
    raw_definitions(&e);
    if (experiment)
        out(&e, "typedef struct {\n%s host; psrt_trap trap; bool failed;\n",
            experiment == 2 ? "psra_host" : "psrt_host");
    if (experiment == 2)
        out(&e, "char error[4096];\n");
    if (e.arrays)
        out(&e, "%spsrt_memory %s;\n", experiment ? "" : "static ",
            experiment ? "memory" : "psmemory");
    for (size_t id = first; id; id = nodes[id].next) {
        if (nodes[id].kind == PS_AST_VARIABLE) {
            e.global[id] = 1;
            out(&e, "%s%s psg_%zu;\n%sbool psready_%zu;\n", experiment ? "" : "static ",
                type(&e, info[id].type), id, experiment ? "" : "static ", id);
        }
    }
    if (experiment)
        out(&e, "} ps_module_state;\n");
    if (e.arrays) {
        out(&e, "#define PSRT_HAS_ARRAYS 1\n");
        out(&e, experiment
                    ? "static void ps_module_values_destroy(ps_module_state *psstate) {\n"
                    : "static void ps_global_values_destroy(void *unused) { (void)unused;\n");
        for (size_t id = first; id; id = nodes[id].next)
            if (nodes[id].kind == PS_AST_VARIABLE && owns(&e, info[id].type)) {
                out(&e, "if (%spsready_%zu) {\n", experiment ? "psstate->" : "", id);
                destroyer(&e, info[id].type);
                out(&e, "(&");
                storage(&e, id);
                out(&e, ");\n");
                out(&e, "%spsready_%zu=false; }\n", experiment ? "psstate->" : "", id);
            }
        if (e.strings)
            out(&e, experiment ? "psrt_string_pins_destroy(&psstate->memory);\n"
                               : "psrt_string_pins_destroy(&psmemory);\n");
        out(&e, "}\n");
    }
    for (size_t id = 1; id < parsed.count; id++) {
        if ((nodes[id].kind == PS_AST_FUNCTION || nodes[id].kind == PS_AST_LAMBDA ||
             nodes[id].kind == PS_AST_LOCAL_FUNCTION) &&
            info[id].type != PS_TYPE_NONE) {
            signature(&e, id);
            out(&e, ";\n");
        }
    }
    for (size_t id = 1; id < parsed.count; id++) {
        if (nodes[id].kind != PS_AST_CALL) continue;
        const ps_lang_builtin *builtin = ps_lang_builtin_get(info[id].binding);
        if (!builtin || !builtin->count ||
            (builtin->types[0] != PS_TYPE_FUNCTION &&
             builtin->types[0] != PS_LANG_ODE_CALLBACK))
            continue;
        size_t callback = 0;
        for (size_t a = nodes[id].b; a; a = nodes[a].next)
            if (info[a].binding == 1) callback = nodes[a].a;
        if (!callback) continue;
        ps_lang_type callback_type = info[callback].type;
        if (builtin->types[0] == PS_TYPE_FUNCTION) {
            out(&e, "static double pscb_%zu(double x, void *user) {\n"
                    "psrt_scalar_value_context *ctx=user;\nswitch(ctx->function->tag) {\n", id);
            for (size_t target = 1; target < parsed.count; target++) {
                if ((nodes[target].kind != PS_AST_FUNCTION &&
                     nodes[target].kind != PS_AST_LAMBDA &&
                     nodes[target].kind != PS_AST_LOCAL_FUNCTION) ||
                    info[target].function_type != callback_type)
                    continue;
                int bound = info[target].method_owner && !info[target].method_static;
                int lambda = nodes[target].kind == PS_AST_LAMBDA;
                int local = nodes[target].kind == PS_AST_LOCAL_FUNCTION;
                out(&e, "case %zu: ", target);
                if (bound || ((lambda || local) && info[target].capture_head))
                    out(&e, "if(psrt_array_count(&ctx->function->receiver)!=1) "
                            "psrt_fail(ctx->site,\"Invalid bound method receiver\"); ");
                out(&e, "return psfn_%zu(", target);
                if (experiment)
                    out(&e, "(ps_module_state *)ctx->module_state, ");
                if (bound) {
                    if (info[target].method_mutating)
                        out(&e, "(%s *)psrt_function_mutable_receiver(ctx->function), ",
                            type(&e, info[nodes[target].a].type));
                    else
                        out(&e, "*(const %s *)psrt_array_data(&ctx->function->receiver), ",
                            type(&e, info[nodes[target].a].type));
                }
                if (lambda)
                    out(&e, "(const pscapture_%zu *)psrt_array_data(&ctx->function->receiver), ",
                        target);
                if (local)
                    out(&e, "ctx->function, ");
                out(&e, "x);\n");
            }
            out(&e, "default: psrt_fail(ctx->site,\"Invalid function value\");\n"
                    "}\nreturn 0;\n}\n");
        } else {
            out(&e, "static void psodecb_%zu(double time, const double *values, "
                    "double *derivative, void *user) {\n"
                    "psrt_ode_value_context *value_ctx=user;\n"
                    "psrt_ode_callback_context *ctx=&value_ctx->base;\n"
                    "static const psrt_element_type element={sizeof(double),NULL,NULL};\n"
                    "psrt_cleanup *mark=psrt_cleanups;\n"
                    "psrt_array input;\n"
                    "if(psrt_array_init(&element,ctx->allocator,0,&input)!=PS_OK) "
                    "psrt_fail(ctx->site,\"ODE callback exceeds array memory budget\");\n"
                    "psrt_cleanup input_owner; "
                    "psrt_cleanup_push(&input_owner,&input,psrt_array_destroy);\n"
                    "if(psrt_array_replace(&input,0,0,values,ctx->count)!=PS_OK) "
                    "psrt_fail(ctx->site,\"ODE callback exceeds array memory budget\");\n"
                    "psrt_array output={0};\nswitch(value_ctx->function->tag) {\n", id);
            for (size_t target = 1; target < parsed.count; target++) {
                if ((nodes[target].kind != PS_AST_FUNCTION &&
                     nodes[target].kind != PS_AST_LAMBDA &&
                     nodes[target].kind != PS_AST_LOCAL_FUNCTION) ||
                    info[target].function_type != callback_type)
                    continue;
                int bound = info[target].method_owner && !info[target].method_static;
                int lambda = nodes[target].kind == PS_AST_LAMBDA;
                int local = nodes[target].kind == PS_AST_LOCAL_FUNCTION;
                out(&e, "case %zu: ", target);
                if (bound || ((lambda || local) && info[target].capture_head))
                    out(&e, "if(psrt_array_count(&value_ctx->function->receiver)!=1) "
                            "psrt_fail(ctx->site,\"Invalid bound method receiver\"); ");
                out(&e, "output=psfn_%zu(", target);
                if (experiment)
                    out(&e, "(ps_module_state *)ctx->module_state, ");
                if (bound) {
                    if (info[target].method_mutating)
                        out(&e, "(%s *)psrt_function_mutable_receiver(value_ctx->function), ",
                            type(&e, info[nodes[target].a].type));
                    else
                        out(&e, "*(const %s *)psrt_array_data(&value_ctx->function->receiver), ",
                            type(&e, info[nodes[target].a].type));
                }
                if (lambda)
                    out(&e, "(const pscapture_%zu *)psrt_array_data(&value_ctx->function->receiver), ",
                        target);
                if (local)
                    out(&e, "value_ctx->function, ");
                out(&e, "time,input); break;\n");
            }
            out(&e, "default: psrt_fail(ctx->site,\"Invalid function value\");\n}\n"
                    "psrt_cleanup output_owner; "
                    "psrt_cleanup_push(&output_owner,&output,psrt_array_destroy);\n"
                    "if(psrt_array_count(&output)!=ctx->count) "
                    "psrt_fail(ctx->site,ctx->acceleration ? "
                    "\"Verlet acceleration has wrong state dimension\" : "
                    "\"ODE derivative has wrong state dimension\");\n"
                    "const double *items=psrt_array_data(&output);\n"
                    "for(size_t i=0;i<ctx->count;i++) if(!isfinite(items[i])) "
                    "psrt_fail(ctx->site,ctx->acceleration ? "
                    "\"Verlet acceleration is non-finite\" : "
                    "\"ODE derivative is non-finite\");\n"
                    "memcpy(derivative,items,ctx->count*sizeof(double));\n"
                    "psrt_cleanup_unwind(mark);\n}\n");
        }
    }
    for (size_t id = 1; id < parsed.count && e.error.kind != PS_LANG_ERROR; id++) {
        if ((nodes[id].kind != PS_AST_FUNCTION && nodes[id].kind != PS_AST_LAMBDA &&
             nodes[id].kind != PS_AST_LOCAL_FUNCTION) ||
            info[id].type == PS_TYPE_NONE)
            continue;
        source_line(&e, id);
        signature(&e, id);
        out(&e, " {\npsrt_enter(");
        site(&e, id);
        out(&e, ");\n");
        e.self_parameter = info[id].method_mutating ? nodes[id].a : 0;
        e.current_function = id;
        if (nodes[id].kind == PS_AST_LOCAL_FUNCTION)
            out(&e, "const pscapture_%zu *pscapture = "
                    "(const pscapture_%zu *)psrt_array_data(&psself->receiver);\n", id, id);
        if (e.self_parameter)
            out(&e, "%s psv_%zu = *psmut_%zu;\n", type(&e, info[e.self_parameter].type),
                e.self_parameter, e.self_parameter);
        if (e.arrays) {
            out(&e, "psrt_cleanup *psfunction_mark=psrt_cleanups;\n");
            for (size_t p = nodes[id].a; p; p = nodes[p].next)
                if (owns(&e, info[p].type)) {
                    retain_temp(&e, p);
                    register_temp(&e, p);
                }
        }
        sequence(&e, nodes[nodes[id].c].a);
        if (((nodes[id].kind == PS_AST_LAMBDA ||
              nodes[id].kind == PS_AST_LOCAL_FUNCTION)
                 ? info[id].lambda_result : info[id].type)
            == PS_TYPE_VOID) {
            publish_self(&e);
            if (e.arrays)
                out(&e, "psrt_cleanup_unwind(psfunction_mark);\n");
            out(&e, "psrt_leave();\nreturn;\n");
        } else {
            out(&e, "psrt_fail(");
            site(&e, id);
            out(&e, ", \"Missing return\");\n");
        }
        out(&e, "}\n");
        e.current_function = 0;
    }
    e.self_parameter = 0;
    out(&e, experiment ? "static void ps_module_init(ps_module_state *psstate) {\n"
                       : "int main(void) {\n");
    if (e.arrays) {
        out(&e, "psrt_cleanup *psinit_mark=psrt_cleanups;\n");
        if (!experiment)
            out(&e, "psrt_cleanup psglobal_owner; "
                    "psrt_cleanup_push(&psglobal_owner,NULL,ps_global_values_destroy);\n");
    }
    for (size_t id = first; id && e.error.kind != PS_LANG_ERROR; id = nodes[id].next)
        if (nodes[id].kind != PS_AST_FUNCTION && nodes[id].kind != PS_AST_STRUCT &&
            nodes[id].kind != PS_AST_ENUM && nodes[id].kind != PS_AST_IMPORT &&
            nodes[id].kind != PS_AST_GENERIC_STRUCT &&
            nodes[id].kind != PS_AST_GENERIC_ENUM &&
            nodes[id].kind != PS_AST_GENERIC_FUNCTION)
            statement(&e, id);
    if (e.arrays) {
        out(&e, "psrt_cleanup_unwind(psinit_mark);\n");
        if (!experiment)
            out(&e, "if (psmemory.live_bytes) psrt_fail(PSRT_AT(1,1), \"Internal array ownership "
                    "imbalance\");\n");
    }
    if (experiment) {
        uint64_t hash = UINT64_C(14695981039346656037);
        for (size_t i = 0; i < source_size; i++) {
            hash ^= e.source[i];
            hash *= UINT64_C(1099511628211);
        }
        out(&e,
            "}\n#define PSRT_EXPERIMENT_ADAPTER 1\n#define PSRT_SOURCE_HASH \"%016" PRIx64 "\"\n",
            hash);
        if (experiment == 2) {
            out(&e, "#define PSRT_FN_ANALYZE psfn_%zu\n#include <physim/language_analysis.h>\n",
                callbacks[0]);
        } else {
            out(&e, "#define PSRT_FN_CREATE psfn_%zu\n#define PSRT_FN_RESET psfn_%zu\n",
                callbacks[0], callbacks[1]);
            out(&e, "#define PSRT_FN_STEP psfn_%zu\n#define PSRT_FN_SCENE psfn_%zu\n", callbacks[2],
                callbacks[3]);
            out(&e, "#define PSRT_EXPERIMENT_NAME PSRT_SOURCE\n#include "
                    "<physim/language_experiment.h>\n");
        }
    } else
        out(&e, "if (fflush(stdout) != 0) psrt_fail(PSRT_AT(1, 1), \"Output failed\");\nreturn "
                "0;\n}\n");
    if (ferror(output))
        fail(&e, parsed.root, "Cannot write generated C");
    free(e.global);
    return (ps_lang_check_result){e.error.kind != PS_LANG_ERROR, e.error, parsed.count};
}
ps_lang_check_result ps_lang_emit_c(FILE *output, const char *source_path,
                                    const char *const *paths, size_t path_count, const void *source,
                                    const ps_lang_node *nodes, ps_lang_parse_result parsed,
                                    const ps_lang_semantic *info) {
    return emit(output, source_path, paths, path_count, source, 0, nodes, parsed, info, 0);
}
ps_lang_check_result ps_lang_emit_experiment(FILE *output, const char *source_path,
                                             const char *const *paths, size_t path_count,
                                             const void *source, size_t source_size,
                                             const ps_lang_node *nodes, ps_lang_parse_result parsed,
                                             const ps_lang_semantic *info) {
    return emit(output, source_path, paths, path_count, source, source_size, nodes, parsed, info, 1);
}
ps_lang_check_result ps_lang_emit_analysis(FILE *output, const char *source_path,
                                           const char *const *paths, size_t path_count,
                                           const void *source, size_t source_size,
                                           const ps_lang_node *nodes, ps_lang_parse_result parsed,
                                           const ps_lang_semantic *info) {
    return emit(output, source_path, paths, path_count, source, source_size, nodes, parsed, info, 2);
}
