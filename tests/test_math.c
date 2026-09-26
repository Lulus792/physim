#include "physim/math.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Math line %d: %s\n", __LINE__, #x);                                   \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
#define NEAR(a, b, e) CHECK(isfinite(a) && fabs((a) - (b)) <= (e))
static bool near3(ps_vec3 a, ps_vec3 b, double tol) {
    return isfinite(a.x) && isfinite(a.y) && isfinite(a.z) && fabs(a.x - b.x) <= tol &&
           fabs(a.y - b.y) <= tol && fabs(a.z - b.z) <= tol;
}
static double random_value(ps_rng *rng) { return 2 * ps_rng_uniform(rng) - 1; }
static ps_vec3 random_vector(ps_rng *rng) {
    double x = random_value(rng), y = random_value(rng), z = random_value(rng);
    return ps_v3(x, y, z);
}
/* Independent Rodrigues formula, no library matrix or quaternion operations. */
static ps_vec3 rodrigues(ps_vec3 a, double theta, ps_vec3 v) {
    double n = sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
    double x = a.x / n, y = a.y / n, z = a.z / n, c = cos(theta), s = sin(theta);
    double dot = x * v.x + y * v.y + z * v.z;
    return ps_v3(c * v.x + s * (y * v.z - z * v.y) + (1 - c) * dot * x,
                 c * v.y + s * (z * v.x - x * v.z) + (1 - c) * dot * y,
                 c * v.z + s * (x * v.y - y * v.x) + (1 - c) * dot * z);
}
static int vectors(void) {
    ps_vec2 a = ps_v2(3, 4), b = ps_v2(-4, 3);
    NEAR(ps_v2dot(a, b), 0, 0);
    NEAR(ps_v2cross(a, b), 25, 0);
    NEAR(ps_v2length(a), 5, 0);
    a = ps_v2scale(ps_v2sub(ps_v2add(a, b), b), 2);
    CHECK(a.x == 6 && a.y == 8);
    ps_vec4 v = ps_v4(1, 2, 2, 4), w = ps_v4(-2, 1, 0, 0);
    NEAR(ps_v4dot(v, w), 0, 0);
    NEAR(ps_v4length(v), 5, 0);
    v = ps_v4scale(ps_v4sub(ps_v4add(v, w), w), 2);
    CHECK(v.x == 2 && v.y == 4 && v.z == 4 && v.w == 8);
    double magnitudes[] = {DBL_TRUE_MIN, DBL_MIN, 1e-200, 1, 1e200, DBL_MAX};
    for (size_t i = 0; i < sizeof magnitudes / sizeof *magnitudes; i++) {
        double d = magnitudes[i];
        a = ps_v2normalize(ps_v2(d, -d));
        NEAR(a.x, sqrt(.5), 4 * DBL_EPSILON);
        NEAR(a.y, -sqrt(.5), 4 * DBL_EPSILON);
        ps_vec3 q = ps_vnormalize(ps_v3(d, -d, d));
        NEAR(q.x, 1 / sqrt(3), 4 * DBL_EPSILON);
        NEAR(q.y, -q.x, 0);
        NEAR(q.z, q.x, 0);
        v = ps_v4normalize(ps_v4(d, -d, d, -d));
        NEAR(v.x, .5, DBL_EPSILON);
        NEAR(v.y, -.5, DBL_EPSILON);
        NEAR(v.z, .5, DBL_EPSILON);
        NEAR(v.w, -.5, DBL_EPSILON);
        NEAR(ps_v2length(ps_v2(d, 0)), d, 0);
        NEAR(ps_vlength(ps_v3(0, d, 0)), d, 0);
        NEAR(ps_v4length(ps_v4(0, 0, 0, d)), d, 0);
    }
    CHECK(ps_v2normalize(ps_v2(0, 0)).x == 0);
    CHECK(ps_vnormalize(ps_v3(0, 0, 0)).y == 0);
    CHECK(ps_v4normalize(ps_v4(0, 0, 0, 0)).w == 0);
    CHECK(isnan(ps_v2normalize(ps_v2(INFINITY, 1)).y));
    CHECK(isnan(ps_vnormalize(ps_v3(0, NAN, 0)).x));
    CHECK(isnan(ps_v4normalize(ps_v4(1, 2, 3, -INFINITY)).z));
    CHECK(ps_close(1, 1, 0, 0));
    CHECK(!ps_close(1, nextafter(1, 2), 0, 0));
    CHECK(ps_close(DBL_MAX, -DBL_MAX, 0, 2));
    CHECK(!ps_close(DBL_MAX, -DBL_MAX, 0, 1.9));
    CHECK(ps_close(DBL_MAX, -DBL_MAX, DBL_MAX, 1));
    CHECK(!ps_close(DBL_MAX, -DBL_MAX, DBL_MAX, .9));
    CHECK(ps_close(DBL_TRUE_MIN, 0, DBL_TRUE_MIN, 0));
    CHECK(!ps_close(DBL_TRUE_MIN, 0, 0, .99));
    CHECK(!ps_close(INFINITY, INFINITY, 0, 0));
    CHECK(!ps_close(NAN, 1, 1, 1));
    CHECK(!ps_close(1, 1, -1, 0));
    CHECK(!ps_close(1, 1, 0, NAN));
    return 0;
}
static int matrices(void) {
    ps_mat3 a = {{0, 1, 2, 1, 0, 3, 4, 5, 6}}, ai;
    ps_vec3 v = ps_mat3_apply(a, ps_v3(1, 2, 3));
    CHECK(near3(v, ps_v3(14, 16, 26), 0));
    ps_mat3 at = ps_mat3_transpose(ps_mat3_transpose(a));
    CHECK(memcmp(&a, &at, sizeof a) == 0);
    CHECK(ps_mat3_inverse(a, 0, &ai) == PS_OK);
    at = ps_mat3_multiply(a, ai);
    for (int i = 0; i < 9; i++)
        NEAR(at.m[i], i % 4 == 0 ? 1 : 0, 3e-15);
    CHECK(ps_mat3_inverse(a, 0, &a) == PS_OK);
    CHECK(memcmp(&a, &ai, sizeof a) == 0);
    ps_mat4 m = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16}}, inverse;
    ps_vec4 h = ps_mat4_apply(m, ps_v4(1, 2, 3, 4));
    CHECK(h.x == 80 && h.y == 90 && h.z == 100 && h.w == 114);
    ps_mat4 mt = ps_mat4_transpose(ps_mat4_transpose(m));
    CHECK(memcmp(&m, &mt, sizeof m) == 0);
    ps_rng rng;
    ps_rng_seed(&rng, 401);
    for (int trial = 0; trial < 2000; trial++) {
        /* Strictly diagonally dominant general matrices, including projective rows. */
        for (int c = 0; c < 4; c++)
            for (int row = 0; row < 4; row++)
                m.m[c * 4 + row] = random_value(&rng) + (row == c ? 5 : 0);
        CHECK(ps_mat4_inverse(m, 0, &inverse) == PS_OK);
        /* Independent residual multiplication in row/column notation. */
        for (int side = 0; side < 2; side++)
            for (int row = 0; row < 4; row++)
                for (int col = 0; col < 4; col++) {
                    double sum = 0;
                    for (int k = 0; k < 4; k++)
                        sum += side ? inverse.m[k * 4 + row] * m.m[col * 4 + k]
                                    : m.m[k * 4 + row] * inverse.m[col * 4 + k];
                    NEAR(sum, row == col ? 1 : 0, 2e-14);
                }
        ps_vec3 p = random_vector(&rng), back;
        CHECK(ps_transform_point(m, p, &back) == PS_OK);
        CHECK(ps_transform_point(inverse, back, &back) == PS_OK);
        CHECK(near3(p, back, 3e-14));
    }
    double scales[] = {1e-250, 1e250};
    for (int i = 0; i < 2; i++) {
        m = ps_mat4_identity();
        for (int j = 0; j < 16; j++)
            m.m[j] *= scales[i];
        CHECK(ps_mat4_inverse(m, 0, &inverse) == PS_OK);
        for (int j = 0; j < 16; j++)
            NEAR(inverse.m[j] * scales[i], j % 5 == 0 ? 1 : 0, 2e-15);
    }
    ps_mat4 saved = ps_mat4_translation(ps_v3(9, 8, 7));
    inverse = saved;
    m = ps_mat4_identity();
    m.m[0] = 0;
    CHECK(ps_mat4_inverse(m, 0, &inverse) == PS_SINGULAR);
    CHECK(memcmp(&inverse, &saved, sizeof saved) == 0);
    m = ps_mat4_identity();
    m.m[0] = DBL_TRUE_MIN;
    CHECK(ps_mat4_inverse(m, 0, &inverse) == PS_NUMERIC);
    CHECK(memcmp(&inverse, &saved, sizeof saved) == 0);
    m = ps_mat4_identity();
    m.m[0] = NAN;
    CHECK(ps_mat4_inverse(m, 0, &inverse) == PS_INVALID);
    CHECK(memcmp(&inverse, &saved, sizeof saved) == 0);
    CHECK(ps_mat4_inverse(ps_mat4_identity(), -1, &inverse) == PS_INVALID);
    CHECK(ps_mat4_inverse(ps_mat4_identity(), 1, &inverse) == PS_INVALID);
    CHECK(ps_mat4_inverse(ps_mat4_identity(), NAN, &inverse) == PS_INVALID);
    CHECK(ps_mat4_inverse(ps_mat4_identity(), 0, NULL) == PS_INVALID);
    /* Two almost parallel columns, chosen tolerance controls rejection. */
    m = ps_mat4_identity();
    m.m[0] = 1;
    m.m[1] = 1;
    m.m[4] = 1;
    m.m[5] = 1 + 1e-8;
    CHECK(ps_mat4_inverse(m, 1e-7, &inverse) == PS_SINGULAR);
    CHECK(ps_mat4_inverse(m, 1e-10, &inverse) == PS_OK);
    a = ps_mat3_identity();
    ai = a;
    a.m[0] = 0;
    CHECK(ps_mat3_inverse(a, 0, &ai) == PS_SINGULAR);
    CHECK(ai.m[0] == 1 && ai.m[4] == 1 && ai.m[8] == 1);
    return 0;
}
static int rotations(void) {
    ps_quat identity = ps_quat_identity(), q = ps_quat_axis_angle(ps_v3(0, 0, 0), 1);
    CHECK(memcmp(&q, &identity, sizeof q) == 0);
    CHECK(isnan(ps_quat_axis_angle(ps_v3(1, 0, 0), NAN).w));
    q = ps_quat_axis_angle(ps_v3(DBL_MAX, 0, 0), PS_PI);
    CHECK(near3(ps_quat_rotate(q, ps_v3(0, 1, 0)), ps_v3(0, -1, 0), 1e-14));
    ps_quat large = {DBL_MAX, -DBL_MAX, DBL_MAX, -DBL_MAX};
    CHECK(ps_quat_normalize(large, &large) == PS_OK);
    CHECK(large.x == .5 && large.y == -.5 && large.z == .5 && large.w == -.5);
    ps_quat small = {0, 0, 0, DBL_TRUE_MIN};
    CHECK(ps_quat_normalize(small, &small) == PS_OK && small.w == 1);
    ps_quat before = q;
    CHECK(ps_quat_normalize((ps_quat){0}, &q) == PS_INVALID);
    CHECK(memcmp(&q, &before, sizeof q) == 0);
    CHECK(ps_quat_normalize((ps_quat){INFINITY, 0, 0, 1}, &q) == PS_INVALID);
    CHECK(ps_quat_normalize(identity, NULL) == PS_INVALID);
    ps_rng rng;
    ps_rng_seed(&rng, 8234);
    for (int trial = 0; trial < 2000; trial++) {
        ps_vec3 axis = random_vector(&rng), axis2 = random_vector(&rng), v = random_vector(&rng);
        double angle = random_value(&rng) * PS_PI, angle2 = random_value(&rng) * PS_PI;
        q = ps_quat_axis_angle(axis, angle);
        ps_quat other = ps_quat_axis_angle(axis2, angle2);
        ps_vec3 expected = rodrigues(axis, angle, v);
        CHECK(near3(ps_quat_rotate(q, v), expected, 3e-14));
        ps_mat4 rotation;
        CHECK(ps_mat4_rotation(q, &rotation) == PS_OK);
        ps_vec3 actual;
        CHECK(ps_transform_direction(rotation, v, &actual) == PS_OK);
        CHECK(near3(actual, expected, 3e-14));
        ps_quat product = ps_quat_multiply(q, other);
        expected = rodrigues(axis, angle, rodrigues(axis2, angle2, v));
        CHECK(near3(ps_quat_rotate(product, v), expected, 5e-14));
        CHECK(near3(ps_quat_rotate(ps_quat_conjugate(q), ps_quat_rotate(q, v)), v, 3e-14));
        double t = ps_rng_uniform(&rng);
        ps_quat mid;
        CHECK(ps_quat_slerp(identity, q, t, &mid) == PS_OK);
        CHECK(near3(ps_quat_rotate(mid, v), rodrigues(axis, angle * t, v), 3e-12));
        ps_quat negative = {-q.x, -q.y, -q.z, -q.w};
        CHECK(ps_quat_slerp(q, negative, t, &mid) == PS_OK);
        CHECK(near3(ps_quat_rotate(mid, v), ps_quat_rotate(q, v), 3e-14));
        CHECK(ps_quat_slerp(q, other, 0, &mid) == PS_OK);
        CHECK(near3(ps_quat_rotate(mid, v), ps_quat_rotate(q, v), 3e-14));
        CHECK(ps_quat_slerp(q, other, 1, &mid) == PS_OK);
        CHECK(near3(ps_quat_rotate(mid, v), ps_quat_rotate(other, v), 3e-14));
    }
    before = q;
    CHECK(ps_quat_slerp(identity, q, -.1, &q) == PS_INVALID);
    CHECK(ps_quat_slerp(identity, q, 1.1, &q) == PS_INVALID);
    CHECK(ps_quat_slerp(identity, q, NAN, &q) == PS_INVALID);
    CHECK(ps_quat_slerp(identity, (ps_quat){0}, .5, &q) == PS_INVALID);
    CHECK(memcmp(&q, &before, sizeof q) == 0);
    return 0;
}
static int transforms(void) {
    ps_mat4 t;
    CHECK(ps_mat4_trs(ps_v3(10, 20, 30), ps_quat_axis_angle(ps_v3(0, 0, 1), PS_PI / 2),
                      ps_v3(2, 3, -4), &t) == PS_OK);
    ps_vec3 v;
    CHECK(ps_transform_point(t, ps_v3(1, 2, 3), &v) == PS_OK);
    CHECK(near3(v, ps_v3(4, 22, 18), 1e-14));
    CHECK(ps_transform_direction(t, ps_v3(1, 2, 3), &v) == PS_OK);
    CHECK(near3(v, ps_v3(-6, 2, -12), 1e-14));
    ps_mat4 manual =
        ps_mat4_multiply(ps_mat4_translation(ps_v3(10, 20, 30)), ps_mat4_scale(ps_v3(2, 3, -4)));
    CHECK(ps_transform_point(manual, ps_v3(1, 2, 3), &v) == PS_OK);
    CHECK(near3(v, ps_v3(12, 26, 18), 0));
    ps_rng rng;
    ps_rng_seed(&rng, 2236);
    for (int i = 0; i < 2000; i++) {
        ps_vec3 axis = random_vector(&rng), translation = random_vector(&rng),
                p = random_vector(&rng);
        ps_vec3 scale =
            ps_v3(.2 + ps_rng_uniform(&rng), -.2 - ps_rng_uniform(&rng), .2 + ps_rng_uniform(&rng));
        double angle = random_value(&rng) * PS_PI;
        CHECK(ps_mat4_trs(translation, ps_quat_axis_angle(axis, angle), scale, &t) == PS_OK);
        ps_vec3 expected =
            rodrigues(axis, angle, ps_v3(p.x * scale.x, p.y * scale.y, p.z * scale.z));
        expected = ps_vadd(expected, translation);
        CHECK(ps_transform_point(t, p, &v) == PS_OK);
        CHECK(near3(v, expected, 2e-14));
        /* Add shear: a normal must remain perpendicular to both transformed tangents. */
        ps_mat4 shear = ps_mat4_identity();
        shear.m[4] = .4;
        shear.m[8] = -.3;
        shear.m[9] = .7;
        t = ps_mat4_multiply(t, shear);
        ps_vec3 tangent = random_vector(&rng), normal = ps_vcross(p, tangent), n, u, w;
        CHECK(ps_transform_normal(t, normal, &n) == PS_OK);
        CHECK(ps_transform_direction(t, p, &u) == PS_OK);
        CHECK(ps_transform_direction(t, tangent, &w) == PS_OK);
        NEAR(ps_vlength(n), 1, 4e-15);
        NEAR(ps_vdot(n, u), 0, 3e-14);
        NEAR(ps_vdot(n, w), 0, 3e-14);
    }
    t = ps_mat4_scale(ps_v3(1e250, 1e250, 1e250));
    CHECK(ps_transform_normal(t, ps_v3(DBL_MAX, DBL_MAX, 0), &v) == PS_OK);
    CHECK(near3(v, ps_v3(sqrt(.5), sqrt(.5), 0), 3e-15));
    ps_vec3 saved = ps_v3(7, 8, 9);
    v = saved;
    t = ps_mat4_identity();
    t.m[15] = 0;
    CHECK(ps_transform_point(t, ps_v3(1, 2, 3), &v) == PS_SINGULAR);
    CHECK(ps_transform_direction(t, ps_v3(1, 2, 3), &v) == PS_INVALID);
    CHECK(ps_transform_normal(t, ps_v3(1, 2, 3), &v) == PS_INVALID);
    CHECK(memcmp(&saved, &v, sizeof v) == 0);
    t = ps_mat4_identity();
    t.m[11] = 1;
    CHECK(ps_transform_point(t, ps_v3(2, 4, 1), &v) == PS_OK);
    CHECK(near3(v, ps_v3(1, 2, .5), 0));
    v = saved;
    t = ps_mat4_scale(ps_v3(0, 1, 1));
    CHECK(ps_transform_normal(t, ps_v3(1, 0, 0), &v) == PS_SINGULAR);
    CHECK(memcmp(&saved, &v, sizeof v) == 0);
    t = ps_mat4_scale(ps_v3(DBL_MAX, 1, 1));
    CHECK(ps_transform_point(t, ps_v3(2, 0, 0), &v) == PS_NUMERIC);
    CHECK(ps_transform_direction(t, ps_v3(2, 0, 0), &v) == PS_NUMERIC);
    CHECK(memcmp(&saved, &v, sizeof v) == 0);
    CHECK(ps_transform_normal(ps_mat4_identity(), ps_v3(0, 0, 0), &v) == PS_INVALID);
    CHECK(ps_transform_point(ps_mat4_identity(), ps_v3(NAN, 0, 0), &v) == PS_INVALID);
    CHECK(ps_transform_point(ps_mat4_identity(), saved, NULL) == PS_INVALID);
    t = ps_mat4_identity();
    ps_mat4 before = t;
    CHECK(ps_mat4_rotation((ps_quat){0}, &t) == PS_INVALID);
    CHECK(ps_mat4_trs(ps_v3(0, 0, NAN), ps_quat_identity(), saved, &t) == PS_INVALID);
    CHECK(ps_mat4_trs(saved, ps_quat_identity(), ps_v3(1, INFINITY, 1), &t) == PS_INVALID);
    CHECK(memcmp(&before, &t, sizeof t) == 0);
    return 0;
}
static int curves(void) {
    const ps_bezier3 polynomial = {{{0, 0, 0}, {1. / 3, 0, 0}, {2. / 3, 1. / 3, 0}, {1, 1, 1}}};
    ps_curve_sample3 sample;
    for (unsigned i = 0; i <= 100; i++) {
        double t = i / 100.;
        CHECK(ps_bezier3_evaluate(&polynomial, t, &sample) == PS_OK);
        CHECK(near3(sample.position, ps_v3(t, t * t, t * t * t), 1e-14));
        CHECK(near3(sample.tangent, ps_v3(1, 2 * t, 3 * t * t), 1e-14));
    }
    ps_rng rng;
    ps_rng_seed(&rng, 13475);
    for (unsigned i = 0; i < 1000; i++) {
        ps_bezier3 curve, left, right;
        for (unsigned j = 0; j < 4; j++)
            curve.points[j] = random_vector(&rng);
        double t = ps_rng_uniform(&rng);
        CHECK(ps_bezier3_split(&curve, t, &left, &right) == PS_OK);
        for (unsigned j = 0; j <= 10; j++) {
            double u = j / 10.;
            ps_curve_sample3 part, original;
            CHECK(ps_bezier3_evaluate(&left, u, &part) == PS_OK);
            CHECK(ps_bezier3_evaluate(&curve, t * u, &original) == PS_OK);
            CHECK(near3(part.position, original.position, 1e-14));
            CHECK(near3(part.tangent, ps_vscale(original.tangent, t), 1e-14));
            CHECK(ps_bezier3_evaluate(&right, u, &part) == PS_OK);
            CHECK(ps_bezier3_evaluate(&curve, t + (1 - t) * u, &original) == PS_OK);
            CHECK(near3(part.position, original.position, 1e-14));
            CHECK(near3(part.tangent, ps_vscale(original.tangent, 1 - t), 1e-14));
        }
        ps_bezier3 alias = curve, other;
        CHECK(ps_bezier3_split(&alias, t, &alias, &other) == PS_OK);
        CHECK(!memcmp(&alias, &left, sizeof left) && !memcmp(&other, &right, sizeof right));
        alias = curve;
        CHECK(ps_bezier3_split(&alias, t, &other, &alias) == PS_OK);
        CHECK(!memcmp(&alias, &right, sizeof right) && !memcmp(&other, &left, sizeof left));
    }
    ps_bezier3 left, right;
    CHECK(ps_bezier3_split(&polynomial, 0, &left, &right) == PS_OK);
    CHECK(!memcmp(&polynomial, &right, sizeof right));
    for (unsigned i = 0; i < 4; i++)
        CHECK(near3(left.points[i], polynomial.points[0], 0));
    CHECK(ps_bezier3_split(&polynomial, 1, &left, &right) == PS_OK);
    CHECK(!memcmp(&polynomial, &left, sizeof left));
    for (unsigned i = 0; i < 4; i++)
        CHECK(near3(right.points[i], polynomial.points[3], 0));
    ps_bezier3 huge;
    for (unsigned i = 0; i < 4; i++)
        huge.points[i] = ps_v3(DBL_MAX, DBL_MAX, DBL_MAX);
    CHECK(ps_bezier3_evaluate(&huge, .5, &sample) == PS_OK && sample.position.x == DBL_MAX &&
          sample.tangent.x == 0);
    huge.points[1].x = huge.points[2].x = -DBL_MAX;
    CHECK(ps_bezier3_evaluate(&huge, .5, &sample) == PS_OK && sample.position.x == -DBL_MAX / 2 &&
          sample.tangent.x == 0);
    CHECK(ps_bezier3_split(&huge, .5, &left, &right) == PS_OK);
    ps_curve_sample3 saved = sample;
    CHECK(ps_bezier3_evaluate(&huge, 0, &sample) == PS_NUMERIC &&
          !memcmp(&saved, &sample, sizeof sample));
    CHECK(ps_bezier3_evaluate(&polynomial, NAN, &sample) == PS_INVALID &&
          !memcmp(&saved, &sample, sizeof sample));
    CHECK(ps_bezier3_evaluate(&polynomial, -1, &sample) == PS_INVALID);
    CHECK(ps_bezier3_evaluate(&polynomial, 2, &sample) == PS_INVALID);
    CHECK(ps_bezier3_evaluate(NULL, .5, &sample) == PS_INVALID);
    CHECK(ps_bezier3_evaluate(&polynomial, .5, NULL) == PS_INVALID);
    ps_bezier3 bad = polynomial, saved_left = left, saved_right = right;
    bad.points[3].z = INFINITY;
    CHECK(ps_bezier3_split(&bad, .5, &left, &right) == PS_INVALID);
    CHECK(!memcmp(&left, &saved_left, sizeof left) && !memcmp(&right, &saved_right, sizeof right));
    CHECK(ps_bezier3_split(&polynomial, .5, &left, &left) == PS_INVALID &&
          !memcmp(&left, &saved_left, sizeof left));
    CHECK(ps_bezier3_split(&polynomial, .5, NULL, &right) == PS_INVALID);
    return 0;
}
int main(void) {
    CHECK(curves() == 0);
    CHECK(vectors() == 0);
    CHECK(matrices() == 0);
    CHECK(rotations() == 0);
    CHECK(transforms() == 0);
    puts("Math: 1000 curve subdivisions, vector extremes, 2000 matrix inverses, 2000 independent rotations and 2000 "
         "transforms passed");
    return 0;
}
