#include "physim/math.h"
#include "physim/numerics.h"
#include <math.h>

/* Convex interpolation without overflowing b-a for opposite-signed endpoints. */
static double curve_lerp(double a, double b, double t) {
    if (t == 0)
        return a;
    if (t == 1)
        return b;
    if ((a < 0) != (b < 0))
        return (1 - t) * a + t * b;
    return a + t * (b - a);
}
static ps_vec3 curve_mix(ps_vec3 a, ps_vec3 b, double t) {
    return ps_v3(curve_lerp(a.x, b.x, t), curve_lerp(a.y, b.y, t), curve_lerp(a.z, b.z, t));
}
static bool curve_valid(const ps_bezier3 *curve, double t) {
    if (!curve || !isfinite(t) || t < 0 || t > 1)
        return false;
    for (unsigned i = 0; i < 4; i++) {
        ps_vec3 p = curve->points[i];
        if (!isfinite(p.x) || !isfinite(p.y) || !isfinite(p.z))
            return false;
    }
    return true;
}
static void curve_subdivide(const ps_bezier3 *c, double t, ps_bezier3 *left, ps_bezier3 *right) {
    ps_vec3 a = curve_mix(c->points[0], c->points[1], t);
    ps_vec3 b = curve_mix(c->points[1], c->points[2], t);
    ps_vec3 d = curve_mix(c->points[2], c->points[3], t);
    ps_vec3 e = curve_mix(a, b, t), f = curve_mix(b, d, t), p = curve_mix(e, f, t);
    *left = (ps_bezier3){{c->points[0], a, e, p}};
    *right = (ps_bezier3){{p, f, d, c->points[3]}};
}
ps_result ps_bezier3_evaluate(const ps_bezier3 *curve, double t, ps_curve_sample3 *out) {
    if (!out || !curve_valid(curve, t))
        return PS_INVALID;
    ps_bezier3 left, right;
    curve_subdivide(curve, t, &left, &right);
    ps_curve_sample3 result = {left.points[3],
                               ps_vscale(ps_vsub(right.points[1], left.points[2]), 3)};
    if (!isfinite(result.tangent.x) || !isfinite(result.tangent.y) || !isfinite(result.tangent.z))
        return PS_NUMERIC;
    *out = result;
    return PS_OK;
}
ps_result ps_bezier3_split(const ps_bezier3 *curve, double t, ps_bezier3 *left, ps_bezier3 *right) {
    if (!left || !right || left == right || !curve_valid(curve, t))
        return PS_INVALID;
    ps_bezier3 a, b;
    curve_subdivide(curve, t, &a, &b);
    *left = a;
    *right = b;
    return PS_OK;
}

ps_vec2 ps_v2(double x, double y) { return (ps_vec2){x, y}; }
ps_vec2 ps_v2add(ps_vec2 a, ps_vec2 b) { return ps_v2(a.x + b.x, a.y + b.y); }
ps_vec2 ps_v2sub(ps_vec2 a, ps_vec2 b) { return ps_v2(a.x - b.x, a.y - b.y); }
ps_vec2 ps_v2scale(ps_vec2 a, double s) { return ps_v2(a.x * s, a.y * s); }
double ps_v2dot(ps_vec2 a, ps_vec2 b) { return a.x * b.x + a.y * b.y; }
double ps_v2cross(ps_vec2 a, ps_vec2 b) { return a.x * b.y - a.y * b.x; }
double ps_v2length(ps_vec2 a) { return hypot(a.x, a.y); }
ps_vec2 ps_v2normalize(ps_vec2 a) {
    if (!isfinite(a.x) || !isfinite(a.y))
        return ps_v2(NAN, NAN);
    double scale = fmax(fabs(a.x), fabs(a.y));
    if (scale == 0)
        return ps_v2(0, 0);
    a = ps_v2(a.x / scale, a.y / scale);
    double n = ps_v2length(a);
    return ps_v2(a.x / n, a.y / n);
}
ps_vec4 ps_v4(double x, double y, double z, double w) { return (ps_vec4){x, y, z, w}; }
ps_vec4 ps_v4add(ps_vec4 a, ps_vec4 b) { return ps_v4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }
ps_vec4 ps_v4sub(ps_vec4 a, ps_vec4 b) { return ps_v4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); }
ps_vec4 ps_v4scale(ps_vec4 a, double s) { return ps_v4(a.x * s, a.y * s, a.z * s, a.w * s); }
double ps_v4dot(ps_vec4 a, ps_vec4 b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }
double ps_v4length(ps_vec4 a) { return hypot(hypot(a.x, a.y), hypot(a.z, a.w)); }
ps_vec4 ps_v4normalize(ps_vec4 a) {
    if (!isfinite(a.x) || !isfinite(a.y) || !isfinite(a.z) || !isfinite(a.w))
        return ps_v4(NAN, NAN, NAN, NAN);
    double scale = fmax(fmax(fabs(a.x), fabs(a.y)), fmax(fabs(a.z), fabs(a.w)));
    if (scale == 0)
        return ps_v4(0, 0, 0, 0);
    a = ps_v4(a.x / scale, a.y / scale, a.z / scale, a.w / scale);
    double n = ps_v4length(a);
    return ps_v4(a.x / n, a.y / n, a.z / n, a.w / n);
}
bool ps_close(double a, double b, double absolute, double relative) {
    if (!isfinite(a) || !isfinite(b) || !isfinite(absolute) || !isfinite(relative) ||
        absolute < 0 || relative < 0)
        return false;
    if (a == b)
        return true;
    double difference = fabs(a - b), scale = fmax(fabs(a), fabs(b));
    if (difference <= absolute)
        return true;
    /* Subtract absolute before dividing so neither the bound nor a-b overflows. */
    if (isfinite(difference))
        return (difference - absolute) / scale <= relative;
    return fabs(a / scale - b / scale) <= absolute / scale + relative;
}
ps_mat3 ps_mat3_identity(void) { return (ps_mat3){{1, 0, 0, 0, 1, 0, 0, 0, 1}}; }
ps_mat3 ps_mat3_multiply(ps_mat3 a, ps_mat3 b) {
    ps_mat3 r = {{0}};
    for (int c = 0; c < 3; c++)
        for (int row = 0; row < 3; row++)
            for (int k = 0; k < 3; k++)
                r.m[c * 3 + row] += a.m[k * 3 + row] * b.m[c * 3 + k];
    return r;
}
ps_mat3 ps_mat3_transpose(ps_mat3 a) {
    ps_mat3 r;
    for (int c = 0; c < 3; c++)
        for (int row = 0; row < 3; row++)
            r.m[c * 3 + row] = a.m[row * 3 + c];
    return r;
}
ps_vec3 ps_mat3_apply(ps_mat3 a, ps_vec3 v) {
    return ps_v3(a.m[0] * v.x + a.m[3] * v.y + a.m[6] * v.z,
                 a.m[1] * v.x + a.m[4] * v.y + a.m[7] * v.z,
                 a.m[2] * v.x + a.m[5] * v.y + a.m[8] * v.z);
}
ps_mat4 ps_mat4_transpose(ps_mat4 a) {
    ps_mat4 r;
    for (int c = 0; c < 4; c++)
        for (int row = 0; row < 4; row++)
            r.m[c * 4 + row] = a.m[row * 4 + c];
    return r;
}
ps_vec4 ps_mat4_apply(ps_mat4 a, ps_vec4 v) {
    return ps_v4(a.m[0] * v.x + a.m[4] * v.y + a.m[8] * v.z + a.m[12] * v.w,
                 a.m[1] * v.x + a.m[5] * v.y + a.m[9] * v.z + a.m[13] * v.w,
                 a.m[2] * v.x + a.m[6] * v.y + a.m[10] * v.z + a.m[14] * v.w,
                 a.m[3] * v.x + a.m[7] * v.y + a.m[11] * v.z + a.m[15] * v.w);
}
static ps_result inverse(const double *a, size_t n, double tolerance, double *out) {
    double row_major[16];
    for (size_t row = 0; row < n; row++)
        for (size_t c = 0; c < n; c++)
            row_major[row * n + c] = a[c * n + row];
    for (size_t c = 0; c < n; c++) {
        double rhs[4] = {0};
        rhs[c] = 1;
        ps_result r = ps_linear_solve(row_major, rhs, n, tolerance, out + c * n);
        if (r != PS_OK)
            return r;
    }
    return PS_OK;
}
ps_result ps_mat3_inverse(ps_mat3 a, double tolerance, ps_mat3 *out) {
    if (!out)
        return PS_INVALID;
    ps_mat3 r;
    ps_result e = inverse(a.m, 3, tolerance, r.m);
    if (e == PS_OK)
        *out = r;
    return e;
}
ps_result ps_mat4_inverse(ps_mat4 a, double tolerance, ps_mat4 *out) {
    if (!out)
        return PS_INVALID;
    ps_mat4 r;
    ps_result e = inverse(a.m, 4, tolerance, r.m);
    if (e == PS_OK)
        *out = r;
    return e;
}
ps_quat ps_quat_identity(void) { return (ps_quat){0, 0, 0, 1}; }
ps_quat ps_quat_conjugate(ps_quat q) { return (ps_quat){-q.x, -q.y, -q.z, q.w}; }
ps_quat ps_quat_multiply(ps_quat a, ps_quat b) {
    return (ps_quat){a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
                     a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
                     a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
                     a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z};
}
ps_result ps_quat_normalize(ps_quat q, ps_quat *out) {
    ps_vec4 v = ps_v4normalize(ps_v4(q.x, q.y, q.z, q.w));
    if (!out || !isfinite(v.x) || (v.x == 0 && v.y == 0 && v.z == 0 && v.w == 0))
        return PS_INVALID;
    *out = (ps_quat){v.x, v.y, v.z, v.w};
    return PS_OK;
}
ps_result ps_quat_slerp(ps_quat a, ps_quat b, double t, ps_quat *out) {
    if (!out || !isfinite(t) || t < 0 || t > 1 || ps_quat_normalize(a, &a) != PS_OK ||
        ps_quat_normalize(b, &b) != PS_OK)
        return PS_INVALID;
    ps_vec4 av = ps_v4(a.x, a.y, a.z, a.w), bv = ps_v4(b.x, b.y, b.z, b.w);
    double dot = ps_v4dot(av, bv);
    if (dot < 0) {
        bv = ps_v4scale(bv, -1);
        dot = -dot;
    }
    double wa = 1 - t, wb = t;
    if (dot < 1 - 1e-8) {
        double theta = acos(fmin(1, dot)), den = sin(theta);
        wa = sin((1 - t) * theta) / den;
        wb = sin(t * theta) / den;
    }
    ps_vec4 v = ps_v4add(ps_v4scale(av, wa), ps_v4scale(bv, wb));
    return ps_quat_normalize((ps_quat){v.x, v.y, v.z, v.w}, out);
}
ps_mat4 ps_mat4_translation(ps_vec3 t) {
    ps_mat4 r = ps_mat4_identity();
    r.m[12] = t.x;
    r.m[13] = t.y;
    r.m[14] = t.z;
    return r;
}
ps_mat4 ps_mat4_scale(ps_vec3 s) {
    ps_mat4 r = ps_mat4_identity();
    r.m[0] = s.x;
    r.m[5] = s.y;
    r.m[10] = s.z;
    return r;
}
ps_result ps_mat4_rotation(ps_quat q, ps_mat4 *out) {
    if (!out || ps_quat_normalize(q, &q) != PS_OK)
        return PS_INVALID;
    double x = q.x, y = q.y, z = q.z, w = q.w;
    ps_mat4 r = {{1 - 2 * (y * y + z * z), 2 * (x * y + z * w), 2 * (x * z - y * w), 0,
                  2 * (x * y - z * w), 1 - 2 * (x * x + z * z), 2 * (y * z + x * w), 0,
                  2 * (x * z + y * w), 2 * (y * z - x * w), 1 - 2 * (x * x + y * y), 0, 0, 0, 0,
                  1}};
    *out = r;
    return PS_OK;
}
static bool finite3(ps_vec3 v) { return isfinite(v.x) && isfinite(v.y) && isfinite(v.z); }
static bool finite4(ps_mat4 a) {
    for (int i = 0; i < 16; i++)
        if (!isfinite(a.m[i]))
            return false;
    return true;
}
static bool affine(ps_mat4 a) { return a.m[3] == 0 && a.m[7] == 0 && a.m[11] == 0 && a.m[15] == 1; }
ps_result ps_mat4_trs(ps_vec3 t, ps_quat q, ps_vec3 s, ps_mat4 *out) {
    ps_mat4 r;
    if (!out || !finite3(t) || !finite3(s) || ps_mat4_rotation(q, &r) != PS_OK)
        return PS_INVALID;
    for (int row = 0; row < 3; row++) {
        r.m[row] *= s.x;
        r.m[4 + row] *= s.y;
        r.m[8 + row] *= s.z;
    }
    r.m[12] = t.x;
    r.m[13] = t.y;
    r.m[14] = t.z;
    if (!finite4(r))
        return PS_NUMERIC;
    *out = r;
    return PS_OK;
}
ps_result ps_transform_point(ps_mat4 a, ps_vec3 v, ps_vec3 *out) {
    if (!out || !finite4(a) || !finite3(v))
        return PS_INVALID;
    ps_vec4 h = ps_mat4_apply(a, ps_v4(v.x, v.y, v.z, 1));
    if (!isfinite(h.x) || !isfinite(h.y) || !isfinite(h.z) || !isfinite(h.w))
        return PS_NUMERIC;
    if (h.w == 0)
        return PS_SINGULAR;
    ps_vec3 r = ps_v3(h.x / h.w, h.y / h.w, h.z / h.w);
    if (!finite3(r))
        return PS_NUMERIC;
    *out = r;
    return PS_OK;
}
ps_result ps_transform_direction(ps_mat4 a, ps_vec3 v, ps_vec3 *out) {
    if (!out || !finite4(a) || !finite3(v) || !affine(a))
        return PS_INVALID;
    ps_vec4 h = ps_mat4_apply(a, ps_v4(v.x, v.y, v.z, 0));
    ps_vec3 r = ps_v3(h.x, h.y, h.z);
    if (!finite3(r))
        return PS_NUMERIC;
    *out = r;
    return PS_OK;
}
ps_result ps_transform_normal(ps_mat4 a, ps_vec3 v, ps_vec3 *out) {
    if (!out || !finite4(a) || !finite3(v) || !affine(a) || (v.x == 0 && v.y == 0 && v.z == 0))
        return PS_INVALID;
    ps_mat3 linear = {{a.m[0], a.m[1], a.m[2], a.m[4], a.m[5], a.m[6], a.m[8], a.m[9], a.m[10]}};
    ps_result e = ps_mat3_inverse(linear, 0, &linear);
    if (e != PS_OK)
        return e;
    /* Only direction matters: scale inverse and input before the dot products. */
    double scale = 0;
    for (int i = 0; i < 9; i++)
        scale = fmax(scale, fabs(linear.m[i]));
    for (int i = 0; i < 9; i++)
        linear.m[i] /= scale;
    ps_vec3 r = ps_mat3_apply(ps_mat3_transpose(linear), ps_vnormalize(v));
    if (!finite3(r) || (r.x == 0 && r.y == 0 && r.z == 0))
        return PS_NUMERIC;
    *out = ps_vnormalize(r);
    return PS_OK;
}
