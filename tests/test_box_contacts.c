#include "physim/mechanics.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Boxes line %d: %s\n", __LINE__, #x);                                  \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static bool near(double x, double y) { return fabs(x - y) < 1e-8; }
static double norm(ps_vec3 v) { return hypot(hypot(v.x, v.y), v.z); }
static ps_vec3 angular_momentum(const ps_body *b) {
    ps_vec3 sum = ps_vcross(b->position_m, ps_vscale(b->velocity_m_s, b->mass_kg));
    const double inertia[] = {b->inertia_kg_m2.x, b->inertia_kg_m2.y, b->inertia_kg_m2.z};
    for (unsigned i = 0; i < 3; i++) {
        ps_vec3 axis = ps_quat_rotate(b->orientation, ps_v3(i == 0, i == 1, i == 2));
        sum = ps_vadd(sum, ps_vscale(axis, inertia[i] * ps_vdot(axis, b->angular_velocity_rad_s)));
    }
    return sum;
}
static ps_vec3 random_vector(ps_rng *r, double size) {
    return ps_v3(size * (2 * ps_rng_uniform(r) - 1), size * (2 * ps_rng_uniform(r) - 1),
                 size * (2 * ps_rng_uniform(r) - 1));
}
static void vertices(const ps_body *b, ps_vec3 size, ps_vec3 out[8]) {
    for (unsigned i = 0; i < 8; i++)
        out[i] = ps_vadd(b->position_m,
                         ps_quat_rotate(b->orientation, ps_v3((i & 1 ? 1 : -1) * size.x * .5,
                                                              (i & 2 ? 1 : -1) * size.y * .5,
                                                              (i & 4 ? 1 : -1) * size.z * .5)));
}
/* Independent vertex projection intervals in long double, not support radii. */
static bool reference(const ps_body *a, ps_vec3 sa, const ps_body *b, ps_vec3 sb,
                      bool *only_edge_separates) {
    ps_vec3 va[8], vb[8], axes[15];
    vertices(a, sa, va);
    vertices(b, sb, vb);
    for (unsigned i = 0; i < 3; i++) {
        ps_vec3 unit = ps_v3(i == 0, i == 1, i == 2);
        axes[i] = ps_quat_rotate(a->orientation, unit);
        axes[i + 3] = ps_quat_rotate(b->orientation, unit);
    }
    for (unsigned i = 0; i < 9; i++)
        axes[i + 6] = ps_vcross(axes[i / 3], axes[3 + i % 3]);
    for (unsigned i = 0; i < 15; i++) {
        if (norm(axes[i]) < 1e-14)
            continue;
        long double amin = LDBL_MAX, amax = -LDBL_MAX, bmin = LDBL_MAX, bmax = -LDBL_MAX;
        for (unsigned j = 0; j < 8; j++) {
            long double x = (long double)va[j].x * axes[i].x + (long double)va[j].y * axes[i].y +
                            (long double)va[j].z * axes[i].z;
            long double y = (long double)vb[j].x * axes[i].x + (long double)vb[j].y * axes[i].y +
                            (long double)vb[j].z * axes[i].z;
            amin = fminl(amin, x);
            amax = fmaxl(amax, x);
            bmin = fminl(bmin, y);
            bmax = fmaxl(bmax, y);
        }
        if (amin > bmax + 1e-12L || bmin > amax + 1e-12L) {
            *only_edge_separates = i >= 6;
            return false;
        }
    }
    return true;
}
static bool on_surface(const ps_body *b, ps_vec3 size, ps_vec3 p) {
    ps_quat q = b->orientation;
    q.x = -q.x;
    q.y = -q.y;
    q.z = -q.z;
    p = ps_quat_rotate(q, ps_vsub(p, b->position_m));
    double gaps[] = {size.x * .5 - fabs(p.x), size.y * .5 - fabs(p.y), size.z * .5 - fabs(p.z)};
    return gaps[0] >= -1e-8 && gaps[1] >= -1e-8 && gaps[2] >= -1e-8 &&
           fmin(fabs(gaps[0]), fmin(fabs(gaps[1]), fabs(gaps[2]))) < 1e-8;
}
int main(void) {
    ps_body a, b;
    ps_vec3 size = ps_v3(2, 2, 2);
    CHECK(ps_body_box(1, size, &a) == PS_OK && ps_body_box(1, size, &b) == PS_OK);
    b.position_m.x = 1.75;
    ps_contact_manifold m;
    CHECK(ps_contacts_boxes(&a, size, &b, size, &m) == PS_OK && m.count == 4);
    for (unsigned i = 0; i < m.count; i++) {
        CHECK(near(m.points[i].penetration_m, .25) && near(m.points[i].normal.x, 1));
        CHECK(near(m.points[i].point_m.x, .875) && near(fabs(m.points[i].point_m.y), 1) &&
              near(fabs(m.points[i].point_m.z), 1));
    }
    a.velocity_m_s.x = 1;
    b.velocity_m_s.x = -1;
    ps_contact_solver solver = PS_CONTACT_SOLVER_DEFAULT;
    solver.iterations = 128;
    solver.restitution = 1;
    solver.friction = 0;
    solver.penetration_slop_m = 0;
    solver.correction_fraction = 1;
    ps_contact_solution solution;
    CHECK(ps_contacts_resolve(&a, &b, &m, &solver, &solution) == PS_OK);
    CHECK(near(a.velocity_m_s.x, -1) && near(b.velocity_m_s.x, 1));
    CHECK(norm(a.angular_velocity_rad_s) < 1e-8 && norm(b.angular_velocity_rad_s) < 1e-8);
    CHECK(near(b.position_m.x - a.position_m.x, 2) && solution.max_normal_error_m_s < 1e-8);
    CHECK(ps_contacts_boxes(&a, size, &b, size, &m) == PS_OK && m.count == 4);
    b.position_m.x += 1e-7;
    CHECK(ps_contacts_boxes(&a, size, &b, size, &m) == PS_OK && m.count == 0);
    a.position_m = ps_v3(0, 0, 0);
    b.position_m = ps_v3(2, 2, 2);
    CHECK(ps_contacts_boxes(&a, size, &b, size, &m) == PS_OK && m.count == 1);
    b.position_m = ps_v3(2, 2, 0);
    CHECK(ps_contacts_boxes(&a, size, &b, size, &m) == PS_OK && m.count == 2);
    b.position_m = ps_v3(0, 0, 0);
    CHECK(ps_contacts_boxes(&a, ps_v3(6, 6, 6), &b, size, &m) == PS_OK && m.count == 4);
    CHECK(near(m.points[0].penetration_m, 4));
    b.position_m = ps_v3(0, 1.9, 0);
    b.orientation = ps_quat_axis_angle(ps_v3(0, 1, 0), PS_PI / 4);
    CHECK(ps_contacts_boxes(&a, size, &b, size, &m) == PS_OK && m.count == 8);
    for (unsigned i = 0; i < m.count; i++)
        CHECK(near(m.points[i].point_m.y, .95) && near(m.points[i].penetration_m, .1));
    b.position_m = ps_v3(1.9, 0, 0);
    b.orientation = ps_quat_axis_angle(ps_v3(0, 0, 1), 1e-10);
    CHECK(ps_contacts_boxes(&a, size, &b, size, &m) == PS_OK && m.count == 4);
    ps_contact_manifold saved = m;
    CHECK(ps_contacts_boxes(&a, size, &a, size, &m) == PS_INVALID && !memcmp(&m, &saved, sizeof m));
    CHECK(ps_contacts_boxes(&a, ps_v3(0, 2, 2), &b, size, &m) == PS_INVALID &&
          !memcmp(&m, &saved, sizeof m));
    CHECK(ps_contacts_boxes(NULL, size, &b, size, &m) == PS_INVALID &&
          !memcmp(&m, &saved, sizeof m));
    b.position_m.x = INFINITY;
    CHECK(ps_contacts_boxes(&a, size, &b, size, &m) == PS_INVALID && !memcmp(&m, &saved, sizeof m));
    b.position_m.x = DBL_MAX;
    a.position_m.x = -DBL_MAX;
    CHECK(ps_contacts_boxes(&a, size, &b, size, &m) == PS_NUMERIC && !memcmp(&m, &saved, sizeof m));
    /* Scale invariance including tiny and very large static bodies. */
    for (int exponent = -150; exponent <= 150; exponent += 150) {
        double s = pow(10, exponent);
        ps_vec3 shape = ps_vscale(size, s);
        CHECK(ps_body_box(0, shape, &a) == PS_OK && ps_body_box(0, shape, &b) == PS_OK);
        b.position_m.x = 1.75 * s;
        CHECK(ps_contacts_boxes(&a, shape, &b, shape, &m) == PS_OK && m.count == 4);
        CHECK(near(m.points[0].penetration_m / s, .25) && near(m.points[0].point_m.x / s, .875));
    }
    ps_rng rng;
    ps_rng_seed(&rng, 1234567);
    unsigned hits = 0, edge_gaps = 0, single = 0;
    for (unsigned trial = 0; trial < 12000; trial++) {
        ps_vec3 sa = ps_vadd(ps_v3(1.6, 1.6, 1.6), random_vector(&rng, 1.3));
        ps_vec3 sb = ps_vadd(ps_v3(1.6, 1.6, 1.6), random_vector(&rng, 1.3));
        CHECK(ps_body_box(1, sa, &a) == PS_OK && ps_body_box(1, sb, &b) == PS_OK);
        a.position_m = random_vector(&rng, 1.5);
        b.position_m = random_vector(&rng, 1.5);
        a.orientation = ps_quat_axis_angle(random_vector(&rng, 1), ps_rng_uniform(&rng) * 6);
        b.orientation = ps_quat_axis_angle(random_vector(&rng, 1), ps_rng_uniform(&rng) * 6);
        bool edge = false, expected = reference(&a, sa, &b, sb, &edge);
        edge_gaps += edge;
        ps_result r = ps_contacts_boxes(&a, sa, &b, sb, &m);
        if (r != PS_OK || (m.count != 0) != expected) {
            fprintf(stderr, "Trial %u expected %d result %d count %u\n", trial, expected, r,
                    m.count);
            return 1;
        }
        ps_contact_manifold reverse, again;
        CHECK(ps_contacts_boxes(&b, sb, &a, sa, &reverse) == PS_OK &&
              (reverse.count != 0) == expected);
        CHECK(ps_contacts_boxes(&a, sa, &b, sb, &again) == PS_OK && !memcmp(&m, &again, sizeof m));
        if (!expected)
            continue;
        CHECK(reverse.count == m.count);
        CHECK(norm(ps_vadd(reverse.points[0].normal, m.points[0].normal)) < 1e-8);
        for (unsigned i = 0; i < m.count; i++) {
            bool found = false;
            for (unsigned j = 0; j < reverse.count; j++)
                found |= norm(ps_vsub(m.points[i].point_m, reverse.points[j].point_m)) < 1e-8 &&
                         near(m.points[i].penetration_m, reverse.points[j].penetration_m);
            CHECK(found);
        }
        hits++;
        single += m.count == 1;
        for (unsigned i = 0; i < m.count; i++) {
            ps_contact c = m.points[i];
            CHECK(near(norm(c.normal), 1) && c.penetration_m >= 0 && isfinite(c.penetration_m));
            ps_vec3 offset = ps_vscale(c.normal, c.penetration_m * .5);
            CHECK(on_surface(&a, sa, ps_vadd(c.point_m, offset)));
            CHECK(on_surface(&b, sb, ps_vsub(c.point_m, offset)));
            CHECK(ps_vdot(c.normal, ps_vsub(b.position_m, a.position_m)) >= -1e-8);
        }
        if (trial % 31 == 0) {
            a.velocity_m_s = random_vector(&rng, 2);
            b.velocity_m_s = random_vector(&rng, 2);
            a.angular_velocity_rad_s = random_vector(&rng, 2);
            b.angular_velocity_rad_s = random_vector(&rng, 2);
            ps_vec3 momentum = ps_vadd(a.velocity_m_s, b.velocity_m_s);
            ps_vec3 angular = ps_vadd(angular_momentum(&a), angular_momentum(&b));
            double before_a, before_b, after_a, after_b;
            CHECK(ps_body_kinetic_energy(&a, &before_a) == PS_OK &&
                  ps_body_kinetic_energy(&b, &before_b) == PS_OK);
            solver.restitution = 0;
            solver.friction = .4;
            solver.correction_fraction = 0; /* Inspect impulse response without geometric shifts. */
            CHECK(ps_contacts_resolve(&a, &b, &m, &solver, &solution) == PS_OK);
            CHECK(norm(ps_vsub(momentum, ps_vadd(a.velocity_m_s, b.velocity_m_s))) < 1e-9);
            CHECK(norm(ps_vsub(angular, ps_vadd(angular_momentum(&a), angular_momentum(&b)))) <
                  1e-9);
            CHECK(ps_body_kinetic_energy(&a, &after_a) == PS_OK &&
                  ps_body_kinetic_energy(&b, &after_b) == PS_OK);
            CHECK(after_a + after_b <= before_a + before_b + 1e-9);
        }
    }
    CHECK(hits > 1000 && edge_gaps > 100 && single > 100);
    printf("Boxes: 12000 reference pairs, %u overlaps, %u edge-only separating cases, %u single "
           "contacts\n",
           hits, edge_gaps, single);
    return 0;
}
