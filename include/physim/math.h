#ifndef PHYSIM_MATH_H
#define PHYSIM_MATH_H
#include "core.h"

/* Arithmetic uses double and performs no allocation. Like the existing Vec3
 * arithmetic, unchecked operations may overflow. Matrices are column-major,
 * act on column vectors, and A*B applies B first. Angles are radians. */
typedef struct {
    double x, y;
} ps_vec2;
typedef struct {
    double x, y, z, w;
} ps_vec4;
typedef struct {
    double m[9];
} ps_mat3;
ps_vec2 ps_v2(double x, double y);
ps_vec2 ps_v2add(ps_vec2 a, ps_vec2 b);
ps_vec2 ps_v2sub(ps_vec2 a, ps_vec2 b);
ps_vec2 ps_v2scale(ps_vec2 a, double s);
double ps_v2dot(ps_vec2 a, ps_vec2 b);
double ps_v2cross(ps_vec2 a, ps_vec2 b); /* signed area, +Z component */
double ps_v2length(ps_vec2 a);
ps_vec2 ps_v2normalize(ps_vec2 a);
ps_vec4 ps_v4(double x, double y, double z, double w);
ps_vec4 ps_v4add(ps_vec4 a, ps_vec4 b);
ps_vec4 ps_v4sub(ps_vec4 a, ps_vec4 b);
ps_vec4 ps_v4scale(ps_vec4 a, double s);
double ps_v4dot(ps_vec4 a, ps_vec4 b);
double ps_v4length(ps_vec4 a);
ps_vec4 ps_v4normalize(ps_vec4 a);
/* Length avoids spurious square overflow/underflow. Normalize rescales first,
 * including subnormals and vectors whose length exceeds DBL_MAX. Zero maps to
 * zero; nonfinite input maps to all-NaN. The same contract holds for Vec3. */
bool ps_close(double a, double b, double absolute_tolerance, double relative_tolerance);

typedef struct {
    ps_vec3 points[4];
} ps_bezier3;
typedef struct {
    ps_vec3 position;
    ps_vec3 tangent; /* derivative with respect to dimensionless parameter t */
} ps_curve_sample3;
/* Cubic Bezier with four finite world-space control points, t in [0,1].
 * All coordinates share the caller's length unit. Tangent is d(position)/dt,
 * not a normalized direction or physical velocity. No arc-length parametrization.
 * De Casteljau evaluation preserves endpoints, including degenerate curves.
 * No allocation. Invalid inputs -> PS_INVALID, nonfinite result -> PS_NUMERIC;
 * all outputs remain unchanged on error. */
ps_result ps_bezier3_evaluate(const ps_bezier3 *curve, double t, ps_curve_sample3 *out);
/* Exact geometric subdivision at t. Each output uses its own parameter [0,1]:
 * left(u)=curve(t*u), right(u)=curve(t+(1-t)*u), up to floating-point roundoff.
 * At t=0/1 one side is a constant curve. left and right must be distinct objects;
 * either may alias curve. No mutation on invalid input. */
ps_result ps_bezier3_split(const ps_bezier3 *curve, double t, ps_bezier3 *left, ps_bezier3 *right);

ps_mat3 ps_mat3_identity(void);
ps_mat3 ps_mat3_multiply(ps_mat3 a, ps_mat3 b);
ps_mat3 ps_mat3_transpose(ps_mat3 a);
ps_vec3 ps_mat3_apply(ps_mat3 a, ps_vec3 v);
ps_mat4 ps_mat4_transpose(ps_mat4 a);
ps_vec4 ps_mat4_apply(ps_mat4 a, ps_vec4 v);
/* Checked operations below preserve *out on every error; input/output aliasing
 * is supported. Invalid inputs -> PS_INVALID; unrepresentable arithmetic ->
 * PS_NUMERIC. Inverse uses scaled partial pivoting; tolerance=0 selects n*eps,
 * otherwise 0<tolerance<1. Rejected pivots -> PS_SINGULAR, not a condition estimate. */
ps_result ps_mat3_inverse(ps_mat3 a, double pivot_tolerance, ps_mat3 *out);
ps_result ps_mat4_inverse(ps_mat4 a, double pivot_tolerance, ps_mat4 *out);

ps_quat ps_quat_identity(void);
ps_quat ps_quat_conjugate(ps_quat q);
ps_quat ps_quat_multiply(ps_quat a, ps_quat b);
ps_result ps_quat_normalize(ps_quat q, ps_quat *out);
/* Normalize endpoints and interpolate the shortest rotation; t in [0,1].
 * q and -q describe the same rotation; output sign need not equal endpoint sign. */
ps_result ps_quat_slerp(ps_quat a, ps_quat b, double t, ps_quat *out);

ps_mat4 ps_mat4_translation(ps_vec3 translation);
ps_mat4 ps_mat4_scale(ps_vec3 scale);
/* Right-handed active rotation, normalized internally. Zero quaternion invalid.
 * TRS applies local scale, then rotation, then translation. Zero scale allowed. */
ps_result ps_mat4_rotation(ps_quat rotation, ps_mat4 *out);
ps_result ps_mat4_trs(ps_vec3 translation, ps_quat rotation, ps_vec3 scale, ps_mat4 *out);
/* Point: homogeneous divide by w (zero w -> PS_SINGULAR).
 * Direction/normal require an affine last row [0,0,0,1] exactly.
 * Point/direction rows use exact binary products and sums, followed by one
 * nearest-even rounding; point division retains the exact homogeneous ratio.
 * Normal solves the row-equilibrated inverse-transpose system and returns a
 * unit vector. Intermediate inverse/solution magnitudes need not fit Double;
 * final direction and solver conditioning still follow Double precision.
 * Zero normal invalid. Outputs unchanged on every error. */
ps_result ps_transform_point(ps_mat4 transform, ps_vec3 point, ps_vec3 *out);
ps_result ps_transform_direction(ps_mat4 transform, ps_vec3 direction, ps_vec3 *out);
ps_result ps_transform_normal(ps_mat4 transform, ps_vec3 normal, ps_vec3 *out);
#endif
