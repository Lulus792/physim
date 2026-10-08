#include "physim/math.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "transform range %d: %s\n", __LINE__, #x);                             \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
int main(void) {
    ps_mat4 m = ps_mat4_identity();
    ps_vec3 r = {7, 8, 9};
    m.m[0] = DBL_MAX;
    m.m[12] = -DBL_MAX;
    CHECK(ps_transform_point(m, ps_v3(2, 0, 0), &r) == PS_OK && r.x == DBL_MAX && r.y == 0 &&
          r.z == 0);
    m = ps_mat4_identity();
    m.m[0] = DBL_MAX;
    m.m[15] = DBL_MAX;
    CHECK(ps_transform_point(m, ps_v3(2, 0, 0), &r) == PS_OK && r.x == 2);
    m = ps_mat4_identity();
    m.m[0] = DBL_MAX;
    m.m[4] = DBL_TRUE_MIN;
    m.m[8] = -DBL_MAX;
    CHECK(ps_transform_direction(m, ps_v3(1, 1, 1), &r) == PS_OK && r.x == DBL_TRUE_MIN);
    m = ps_mat4_scale(ps_v3(DBL_TRUE_MIN, 1, 1));
    CHECK(ps_transform_normal(m, ps_v3(1, 0, 0), &r) == PS_OK && r.x == 1 && r.y == 0 && r.z == 0);
    CHECK(ps_transform_normal(m, ps_v3(DBL_TRUE_MIN, 1, 0), &r) == PS_OK);
    CHECK(fabs(r.x - sqrt(.5)) < 1e-15 && fabs(r.y - sqrt(.5)) < 1e-15);
    ps_vec3 old = {7, 8, 9};
    r = old;
    m = ps_mat4_scale(ps_v3(DBL_MAX, 1, 1));
    CHECK(ps_transform_point(m, ps_v3(2, 0, 0), &r) == PS_NUMERIC && !memcmp(&r, &old, sizeof r));
    CHECK(ps_transform_direction(m, ps_v3(2, 0, 0), &r) == PS_NUMERIC &&
          !memcmp(&r, &old, sizeof r));
    m = ps_mat4_identity();
    m.m[15] = 0;
    CHECK(ps_transform_point(m, ps_v3(1, 2, 3), &r) == PS_SINGULAR && !memcmp(&r, &old, sizeof r));
    m = ps_mat4_scale(ps_v3(0, 1, 1));
    CHECK(ps_transform_normal(m, ps_v3(1, 0, 0), &r) == PS_SINGULAR && !memcmp(&r, &old, sizeof r));
    CHECK(ps_transform_point(ps_mat4_identity(), ps_v3(NAN, 0, 0), &r) == PS_INVALID &&
          !memcmp(&r, &old, sizeof r));
    puts("Transform range: affine cancellation, projective ratio, tiny residuals, normal direction "
         "and atomic errors passed");
    return 0;
}
