#include "physim/mechanics.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Joint line %d: %s\n", __LINE__, #x);                                  \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static double orbit_error(double dt) {
    ps_body a;
    if (ps_body_sphere(1, .1, &a) != PS_OK)
        return -1;
    a.position_m.x = 1;
    a.velocity_m_s.y = 1;
    ps_distance_joint joint = {{0}, {0}, 1, .2};
    double maximum = 0;
    unsigned steps = (unsigned)round(2 / dt);
    for (unsigned i = 0; i < steps; i++) {
        if (ps_distance_joint_resolve(&a, NULL, &joint, dt, NULL) != PS_OK ||
            ps_body_step(&a, ps_v3(0, 0, 0), ps_v3(0, 0, 0), dt) != PS_OK)
            return -1;
        maximum = fmax(maximum, fabs(hypot(a.position_m.x, a.position_m.y) - 1));
    }
    return maximum;
}
int main(void) {
    ps_distance_joint valid = {{0}, {0}, 1, 0};
    CHECK(ps_distance_joint_validate(&valid) == PS_OK);
    CHECK(ps_distance_joint_validate(NULL) == PS_INVALID);
    for (unsigned i = 0; i < 7; i++) {
        ps_distance_joint invalid = valid;
        if (i == 0)
            invalid.length_m = 0;
        if (i == 1)
            invalid.length_m = INFINITY;
        if (i == 2)
            invalid.stabilization = -0.1;
        if (i == 3)
            invalid.stabilization = 1.1;
        if (i == 4)
            invalid.stabilization = NAN;
        if (i == 5)
            invalid.anchor_a_m.x = NAN;
        if (i == 6)
            invalid.anchor_b_m.y = INFINITY;
        CHECK(ps_distance_joint_validate(&invalid) == PS_INVALID);
    }
    ps_body a, b;
    CHECK(ps_body_sphere(2, 1, &a) == PS_OK);
    CHECK(ps_body_sphere(1, 1, &b) == PS_OK);
    b.position_m.x = 2;
    a.velocity_m_s.x = -1;
    b.velocity_m_s.x = 2;
    ps_distance_joint joint = {{0}, {0}, 2, 0};
    ps_distance_joint_solution out;
    CHECK(ps_distance_joint_resolve(&a, &b, &joint, .01, &out) == PS_OK);
    CHECK(fabs(a.velocity_m_s.x) < 1e-14 && fabs(b.velocity_m_s.x) < 1e-14);
    CHECK(fabs(out.impulse_on_a_ns.x - 2) < 1e-14 && out.velocity_error_m_s < 1e-14);
    CHECK(a.position_m.x == 0 && b.position_m.x == 2);
    /* A rotated, off-center anchor has effective inverse mass 1 + 1/(2/3). */
    CHECK(ps_body_box(1, ps_v3(2, 2, 2), &a) == PS_OK);
    a.orientation = (ps_quat){0, 0, sqrt(.5), sqrt(.5)};
    a.velocity_m_s.x = 1;
    joint = (ps_distance_joint){{1, 0, 0}, {2, 1, 0}, 2, 0};
    CHECK(ps_distance_joint_resolve(&a, NULL, &joint, .01, &out) == PS_OK);
    CHECK(fabs(out.impulse_on_a_ns.x + .4) < 1e-14);
    CHECK(fabs(a.velocity_m_s.x - .6) < 1e-14);
    CHECK(fabs(a.angular_velocity_rad_s.z - .6) < 1e-14);
    CHECK(out.velocity_error_m_s < 1e-14);
    /* A stretched joint prescribes a closing speed, without teleporting. */
    CHECK(ps_body_sphere(1, 1, &a) == PS_OK);
    a.position_m.x = 2;
    joint = (ps_distance_joint){{0}, {0}, 1, .2};
    CHECK(ps_distance_joint_resolve(&a, NULL, &joint, .1, &out) == PS_OK);
    CHECK(fabs(a.velocity_m_s.x + 2) < 1e-14 && a.position_m.x == 2);
    CHECK(out.length_error_m == 1 && out.velocity_error_m_s < 1e-14);
    double coarse = orbit_error(.02), fine = orbit_error(.01);
    CHECK(coarse > 0 && fine > 0 && fine < coarse * .3 && coarse < .002);
    /* Every rejected operation preserves both bodies and the report. */
    ps_body before = a, before_b = b;
    memset(&out, 0xa5, sizeof out);
    ps_distance_joint_solution saved = out;
    joint.length_m = 0;
    CHECK(ps_distance_joint_resolve(&a, &b, &joint, .1, &out) == PS_INVALID);
    joint.length_m = 1;
    joint.anchor_a_m.x = DBL_MAX;
    joint.anchor_b_m.x = -DBL_MAX;
    CHECK(ps_distance_joint_resolve(&a, &b, &joint, .1, &out) == PS_NUMERIC);
    CHECK(memcmp(&a, &before, sizeof a) == 0 && memcmp(&b, &before_b, sizeof b) == 0);
    CHECK(memcmp(&out, &saved, sizeof out) == 0);
    joint = (ps_distance_joint){{0}, {2, 0, 0}, 1, .2};
    CHECK(ps_distance_joint_resolve(&a, NULL, &joint, .1, &out) == PS_SINGULAR);
    CHECK(memcmp(&a, &before, sizeof a) == 0 && memcmp(&out, &saved, sizeof out) == 0);
    CHECK(ps_distance_joint_resolve(&a, &a, &joint, .1, &out) == PS_INVALID);
    CHECK(ps_distance_joint_resolve(&a, NULL, &joint, 0, &out) == PS_INVALID);
    CHECK(ps_body_sphere(0, 1, &a) == PS_OK);
    CHECK(ps_distance_joint_resolve(&a, NULL, &joint, .1, &out) == PS_OK);
    CHECK(out.impulse_on_a_ns.x == 0 && fabs(out.velocity_error_m_s - 2) < 1e-14);
    puts("Distance joint: momentum, rotated anchors, stabilization, convergence and atomicity "
         "passed");
    return 0;
}
