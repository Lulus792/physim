#ifndef PS_LANGUAGE_BUILTINS_H
#define PS_LANGUAGE_BUILTINS_H
#include "checker.h"
#define PS_LANG_LIBRARY_BASE (SIZE_MAX - 4096)
#define PS_LANG_MEMBER_X (SIZE_MAX - 100)
#define PS_LANG_MEMBER_Y (SIZE_MAX - 101)
#define PS_LANG_MEMBER_Z (SIZE_MAX - 102)
#define PS_LANG_MEMBER_W (SIZE_MAX - 103)
#define PS_LANG_MEMBER_VALUE (SIZE_MAX - 104)
#define PS_LANG_MEMBER_UNIT (SIZE_MAX - 105)
#define PS_LANG_MEMBER_MEASUREMENT_VALUE (SIZE_MAX - 106)
#define PS_LANG_MEMBER_STATE (SIZE_MAX - 107)
#define PS_LANG_MEMBER_TIME (SIZE_MAX - 108)
#define PS_LANG_MEMBER_UNCERTAINTY (SIZE_MAX - 109)
#define PS_LANG_MEMBER_INDEX (SIZE_MAX - 110)
#define PS_LANG_MEMBER_SKIPPED (SIZE_MAX - 111)
#define PS_LANG_MEMBER_BODY_POSITION (SIZE_MAX - 112)
#define PS_LANG_MEMBER_BODY_VELOCITY (SIZE_MAX - 113)
#define PS_LANG_MEMBER_BODY_ORIENTATION (SIZE_MAX - 114)
#define PS_LANG_MEMBER_BODY_ANGULAR_VELOCITY (SIZE_MAX - 115)
#define PS_LANG_MEMBER_BODY_MASS (SIZE_MAX - 116)
#define PS_LANG_MEMBER_BODY_INERTIA (SIZE_MAX - 117)
#define PS_LANG_MEMBER_CONTACT_COUNT (SIZE_MAX - 118)
#define PS_LANG_MEMBER_RESULT_A (SIZE_MAX - 119)
#define PS_LANG_MEMBER_RESULT_B (SIZE_MAX - 120)
#define PS_LANG_MEMBER_RESULT_COUNT (SIZE_MAX - 121)
#define PS_LANG_MEMBER_RESULT_ERROR (SIZE_MAX - 122)
#define PS_LANG_MEMBER_SOLVER_ITERATIONS (SIZE_MAX - 123)
#define PS_LANG_MEMBER_SOLVER_RESTITUTION (SIZE_MAX - 124)
#define PS_LANG_MEMBER_SOLVER_FRICTION (SIZE_MAX - 125)
#define PS_LANG_MEMBER_SOLVER_THRESHOLD (SIZE_MAX - 126)
#define PS_LANG_MEMBER_SOLVER_SLOP (SIZE_MAX - 127)
#define PS_LANG_MEMBER_SOLVER_CORRECTION (SIZE_MAX - 128)
#define PS_LANG_MEMBER_JOINT_ANCHOR_A (SIZE_MAX - 129)
#define PS_LANG_MEMBER_JOINT_ANCHOR_B (SIZE_MAX - 130)
#define PS_LANG_MEMBER_JOINT_LENGTH (SIZE_MAX - 131)
#define PS_LANG_MEMBER_JOINT_STABILIZATION (SIZE_MAX - 132)
#define PS_LANG_MEMBER_JOINT_IMPULSE (SIZE_MAX - 133)
#define PS_LANG_MEMBER_JOINT_LENGTH_ERROR (SIZE_MAX - 134)
#define PS_LANG_MEMBER_JOINT_VELOCITY_ERROR (SIZE_MAX - 135)
#define PS_LANG_MEMBER_CONSTRAINT_A (SIZE_MAX - 136)
#define PS_LANG_MEMBER_CONSTRAINT_B (SIZE_MAX - 137)
#define PS_LANG_MEMBER_CONSTRAINT_POINT (SIZE_MAX - 138)
#define PS_LANG_MEMBER_CONSTRAINT_NORMAL (SIZE_MAX - 139)
#define PS_LANG_MEMBER_CONSTRAINT_DEPTH (SIZE_MAX - 140)
#define PS_LANG_MEMBER_CONSTRAINT_JOINT (SIZE_MAX - 141)
#define PS_LANG_MEMBER_GRAPH_BODY_COUNT (SIZE_MAX - 142)
#define PS_LANG_MEMBER_GRAPH_CONTACT_COUNT (SIZE_MAX - 143)
#define PS_LANG_MEMBER_GRAPH_JOINT_COUNT (SIZE_MAX - 144)
#define PS_LANG_MEMBER_GRAPH_NORMAL_ERROR (SIZE_MAX - 145)
#define PS_LANG_MEMBER_GRAPH_PROJECTION_ERROR (SIZE_MAX - 146)
#define PS_LANG_MEMBER_GRAPH_JOINT_VELOCITY_ERROR (SIZE_MAX - 147)
#define PS_LANG_MEMBER_GRAPH_JOINT_LENGTH_ERROR (SIZE_MAX - 148)
#define PS_LANG_MEMBER_SWEEP_HIT (SIZE_MAX - 149)
#define PS_LANG_MEMBER_AABB_MIN (SIZE_MAX - 150)
#define PS_LANG_MEMBER_AABB_MAX (SIZE_MAX - 151)
#define PS_LANG_MEMBER_PAIR_A (SIZE_MAX - 152)
#define PS_LANG_MEMBER_PAIR_B (SIZE_MAX - 153)
#define PS_LANG_MEMBER_ODE_STATE (SIZE_MAX - 154)
#define PS_LANG_MEMBER_ODE_ACCEPTED (SIZE_MAX - 155)
#define PS_LANG_MEMBER_ODE_REJECTED (SIZE_MAX - 156)
#define PS_LANG_MEMBER_ODE_EVALUATIONS (SIZE_MAX - 157)
#define PS_LANG_MEMBER_ODE_REACHED_TIME (SIZE_MAX - 158)
#define PS_LANG_MEMBER_ODE_NEXT_STEP (SIZE_MAX - 159)
#define PS_LANG_MEMBER_ODE_ERROR_NORM (SIZE_MAX - 160)
#define PS_LANG_MEMBER_SCALAR_X (SIZE_MAX - 161)
#define PS_LANG_MEMBER_SCALAR_VALUE (SIZE_MAX - 162)
#define PS_LANG_MEMBER_SCALAR_LOWER (SIZE_MAX - 163)
#define PS_LANG_MEMBER_SCALAR_UPPER (SIZE_MAX - 164)
#define PS_LANG_MEMBER_SCALAR_ITERATIONS (SIZE_MAX - 165)
#define PS_LANG_MEMBER_SCALAR_EVALUATIONS (SIZE_MAX - 166)
#define PS_LANG_MEMBER_MEDIUM_DENSITY (SIZE_MAX - 167)
#define PS_LANG_MEMBER_MEDIUM_VISCOSITY (SIZE_MAX - 168)
#define PS_LANG_MEMBER_MATERIAL_DENSITY (SIZE_MAX - 169)
#define PS_LANG_MEMBER_MATERIAL_RESTITUTION (SIZE_MAX - 170)
#define PS_LANG_MEMBER_MATERIAL_FRICTION (SIZE_MAX - 171)
#define PS_LANG_MEMBER_SUBMERSION_VOLUME (SIZE_MAX - 172)
#define PS_LANG_MEMBER_SUBMERSION_CENTROID (SIZE_MAX - 173)
/* Signature-only markers, resolved to canonical array types by the checker. */
#define PS_LANG_VEC3_ARRAY ((ps_lang_type)128)
#define PS_LANG_SERIES_ARRAY ((ps_lang_type)129)
#define PS_LANG_STRING_ARRAY ((ps_lang_type)130)
#define PS_LANG_UNIT_ARRAY ((ps_lang_type)131)
#define PS_LANG_QUANTITY_ARRAY ((ps_lang_type)132)
#define PS_LANG_BODY_ARRAY ((ps_lang_type)133)
#define PS_LANG_CONTACT_CONSTRAINT_ARRAY ((ps_lang_type)134)
#define PS_LANG_JOINT_CONSTRAINT_ARRAY ((ps_lang_type)135)
#define PS_LANG_AABB_ARRAY ((ps_lang_type)136)
#define PS_LANG_PAIR_ARRAY ((ps_lang_type)137)
#define PS_LANG_FLOAT_ARRAY ((ps_lang_type)138)
#define PS_LANG_ODE_CALLBACK ((ps_lang_type)139)
static inline ps_lang_type ps_lang_signature_element(ps_lang_type type) {
    return type == PS_LANG_VEC3_ARRAY ? PS_TYPE_VEC3
           : type == PS_LANG_FLOAT_ARRAY ? PS_TYPE_FLOAT64
           : type == PS_LANG_SERIES_ARRAY ? PS_TYPE_SERIES
           : type == PS_LANG_STRING_ARRAY ? PS_TYPE_STRING
           : type == PS_LANG_UNIT_ARRAY ? PS_TYPE_UNIT
           : type == PS_LANG_QUANTITY_ARRAY ? PS_TYPE_QUANTITY
           : type == PS_LANG_BODY_ARRAY ? PS_TYPE_BODY
           : type == PS_LANG_CONTACT_CONSTRAINT_ARRAY ? PS_TYPE_CONTACT_CONSTRAINT
           : type == PS_LANG_JOINT_CONSTRAINT_ARRAY ? PS_TYPE_JOINT_CONSTRAINT
           : type == PS_LANG_AABB_ARRAY ? PS_TYPE_AABB
           : type == PS_LANG_PAIR_ARRAY ? PS_TYPE_COLLISION_PAIR : PS_TYPE_NONE;
}
typedef struct ps_lang_builtin {
    const char *name, *c_name;
    ps_lang_type result;
    unsigned count, host;
    ps_lang_type types[10];
    const char *labels[10];
} ps_lang_builtin;
const ps_lang_builtin *ps_lang_builtin_find(const void *name, size_t length, size_t *binding);
const ps_lang_builtin *ps_lang_builtin_get(size_t binding);
typedef struct ps_lang_method {
    const char *name, *function;
    ps_lang_type owner;
    unsigned receiver; /* Zero-based parameter slot, optionally ORed with MUTATING. */
} ps_lang_method;
#define PS_LANG_METHOD_MUTATING 256u
#define PS_LANG_METHOD_RECEIVER_MASK 255u
const ps_lang_method *ps_lang_method_find(ps_lang_type owner, const void *name, size_t length,
                                          size_t *binding);
const ps_lang_builtin *ps_lang_static_method_find(const void *type, size_t type_length,
                                                  const void *name, size_t name_length,
                                                  size_t *binding);
const char *ps_lang_member_name(size_t binding);
#endif
