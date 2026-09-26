#ifndef PHYSIM_LANGUAGE_MATRIX_H
#define PHYSIM_LANGUAGE_MATRIX_H
/* Value bindings for the shared matrix library. No allocation or mutation. */
#include "language_runtime.h"
#include "math.h"

static inline ps_mat3 psrt_mat3_checked(ps_mat3 value, psrt_site site) {
    for (size_t i = 0; i < 9; i++)
        (void)psrt_finite(value.m[i], site);
    return value;
}
static inline ps_mat4 psrt_mat4_checked(ps_mat4 value, psrt_site site) {
    for (size_t i = 0; i < 16; i++)
        (void)psrt_finite(value.m[i], site);
    return value;
}
static inline ps_mat3 psrt_mat3(ps_vec3 c0, ps_vec3 c1, ps_vec3 c2, psrt_site site) {
    ps_mat3 value = {{c0.x, c0.y, c0.z, c1.x, c1.y, c1.z, c2.x, c2.y, c2.z}};
    return psrt_mat3_checked(value, site);
}
static inline ps_mat4 psrt_mat4(ps_vec4 c0, ps_vec4 c1, ps_vec4 c2, ps_vec4 c3, psrt_site site) {
    ps_mat4 value = {{c0.x, c0.y, c0.z, c0.w, c1.x, c1.y, c1.z, c1.w, c2.x, c2.y, c2.z, c2.w, c3.x,
                      c3.y, c3.z, c3.w}};
    return psrt_mat4_checked(value, site);
}
static inline ps_mat3 psrt_mat3_identity(psrt_site site) {
    (void)site;
    return ps_mat3_identity();
}
static inline ps_mat4 psrt_mat4_identity(psrt_site site) {
    (void)site;
    return ps_mat4_identity();
}
static inline double psrt_mat3_element(ps_mat3 value, int64_t row, int64_t column, psrt_site site) {
    if (row < 0 || row >= 3 || column < 0 || column >= 3)
        psrt_fail(site, "Mat3 row and column must be in 0..<3");
    return psrt_finite(value.m[(size_t)column * 3 + (size_t)row], site);
}
static inline double psrt_mat4_element(ps_mat4 value, int64_t row, int64_t column, psrt_site site) {
    if (row < 0 || row >= 4 || column < 0 || column >= 4)
        psrt_fail(site, "Mat4 row and column must be in 0..<4");
    return psrt_finite(value.m[(size_t)column * 4 + (size_t)row], site);
}
static inline ps_mat3 psrt_mat3_multiply(ps_mat3 left, ps_mat3 right, psrt_site site) {
    return psrt_mat3_checked(ps_mat3_multiply(left, right), site);
}
static inline ps_mat4 psrt_mat4_multiply(ps_mat4 left, ps_mat4 right, psrt_site site) {
    return psrt_mat4_checked(ps_mat4_multiply(left, right), site);
}
static inline ps_mat3 psrt_mat3_transpose(ps_mat3 value, psrt_site site) {
    return psrt_mat3_checked(ps_mat3_transpose(value), site);
}
static inline ps_mat4 psrt_mat4_transpose(ps_mat4 value, psrt_site site) {
    return psrt_mat4_checked(ps_mat4_transpose(value), site);
}
static inline ps_vec3 psrt_mat3_apply(ps_mat3 value, ps_vec3 vector, psrt_site site) {
    ps_vec3 out = ps_mat3_apply(value, vector);
    return ps_v3(psrt_finite(out.x, site), psrt_finite(out.y, site), psrt_finite(out.z, site));
}
static inline ps_vec4 psrt_mat4_apply(ps_mat4 value, ps_vec4 vector, psrt_site site) {
    ps_vec4 out = ps_mat4_apply(value, vector);
    return ps_v4(psrt_finite(out.x, site), psrt_finite(out.y, site), psrt_finite(out.z, site),
                 psrt_finite(out.w, site));
}
static inline void psrt_matrix_status(ps_result status, psrt_site site) {
    if (status == PS_SINGULAR)
        psrt_fail(site, "Matrix operation is singular or rejected by pivot tolerance");
    if (status == PS_INVALID)
        psrt_fail(site, "Invalid matrix operation arguments");
    if (status != PS_OK)
        psrt_fail(site, "Matrix operation exceeds numeric range");
}
static inline ps_mat3 psrt_mat3_inverse(ps_mat3 value, double tolerance, psrt_site site) {
    ps_mat3 out;
    psrt_matrix_status(ps_mat3_inverse(value, tolerance, &out), site);
    return out;
}
static inline ps_mat4 psrt_mat4_inverse(ps_mat4 value, double tolerance, psrt_site site) {
    ps_mat4 out;
    psrt_matrix_status(ps_mat4_inverse(value, tolerance, &out), site);
    return out;
}
static inline ps_mat4 psrt_mat4_translation(ps_vec3 translation, psrt_site site) {
    return psrt_mat4_checked(ps_mat4_translation(translation), site);
}
static inline ps_mat4 psrt_mat4_scale(ps_vec3 scale, psrt_site site) {
    return psrt_mat4_checked(ps_mat4_scale(scale), site);
}
static inline ps_mat4 psrt_mat4_rotation(ps_quat rotation, psrt_site site) {
    ps_mat4 out;
    psrt_matrix_status(ps_mat4_rotation(rotation, &out), site);
    return out;
}
static inline ps_mat4 psrt_mat4_trs(ps_vec3 translation, ps_quat rotation, ps_vec3 scale,
                                    psrt_site site) {
    ps_mat4 out;
    psrt_matrix_status(ps_mat4_trs(translation, rotation, scale, &out), site);
    return out;
}
static inline ps_vec3 psrt_matrix_point(ps_mat4 transform, ps_vec3 point, psrt_site site) {
    ps_vec3 out;
    psrt_matrix_status(ps_transform_point(transform, point, &out), site);
    return out;
}
static inline ps_vec3 psrt_matrix_direction(ps_mat4 transform, ps_vec3 direction, psrt_site site) {
    ps_vec3 out;
    psrt_matrix_status(ps_transform_direction(transform, direction, &out), site);
    return out;
}
static inline ps_vec3 psrt_matrix_normal(ps_mat4 transform, ps_vec3 normal, psrt_site site) {
    ps_vec3 out;
    psrt_matrix_status(ps_transform_normal(transform, normal, &out), site);
    return out;
}
#endif
