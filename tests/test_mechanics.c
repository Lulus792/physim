#include "physim/mechanics.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Mechanics line %d: %s\n", __LINE__, #x);                              \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static bool near(double a, double b) { return fabs(a - b) < 1e-10; }
static bool vec_near(ps_vec3 a, ps_vec3 b) {
    return near(a.x, b.x) && near(a.y, b.y) && near(a.z, b.z);
}
static ps_vec3 momentum(const ps_body *b) { return ps_vscale(b->velocity_m_s, b->mass_kg); }
static ps_vec3 angular_momentum(const ps_body *b) {
    ps_quat q = b->orientation, inverse = {-q.x, -q.y, -q.z, q.w};
    ps_vec3 w = ps_quat_rotate(inverse, b->angular_velocity_rad_s), d = b->inertia_kg_m2;
    return ps_vadd(ps_vcross(b->position_m, momentum(b)),
                   ps_quat_rotate(q, ps_v3(w.x * d.x, w.y * d.y, w.z * d.z)));
}
static double energy(const ps_body *b) {
    double e = NAN;
    ps_body_kinetic_energy(b, &e);
    return e;
}
static double spin_error(double dt) {
    ps_body b;
    if (ps_body_box(12, ps_v3(2, 4, 6), &b) != PS_OK)
        return INFINITY;
    b.angular_velocity_rad_s = ps_v3(1, 2, 3);
    double e = energy(&b);
    unsigned n = (unsigned)llround(.1 / dt);
    for (unsigned i = 0; i < n; i++)
        if (ps_body_step(&b, ps_v3(0, 0, 0), ps_v3(0, 0, 0), dt) != PS_OK)
            return INFINITY;
    return fabs(energy(&b) - e);
}
int main(void) {
    ps_body a, b;
    ps_vec3 zero = {0}, v, j;
    CHECK(ps_body_sphere(2, .5, &a) == PS_OK && near(a.inertia_kg_m2.x, .2));
    CHECK(ps_body_box(12, ps_v3(2, 4, 6), &b) == PS_OK &&
          vec_near(b.inertia_kg_m2, ps_v3(52, 40, 20)));
    b.orientation = ps_quat_axis_angle(ps_v3(0, 0, 1), PS_PI / 2);
    CHECK(ps_body_apply_impulse(&b, ps_v3(0, 1, 0), ps_v3(0, 0, 1)) == PS_OK);
    CHECK(vec_near(b.angular_velocity_rad_s, ps_v3(-1. / 40, 0, 0)));
    CHECK(near(energy(&b), 1. / 24 + 1. / 80));
    CHECK(ps_body_force_torque(&b, ps_v3(0, 2, 0), ps_v3(1, 0, 0), &v) == PS_OK &&
          vec_near(v, ps_v3(0, 0, 2)));
    CHECK(ps_body_sphere(1, 1, &a) == PS_OK);
    a.angular_velocity_rad_s = ps_v3(0, 0, 2);
    CHECK(ps_body_point_velocity(&a, ps_v3(1, 0, 0), &v) == PS_OK && vec_near(v, ps_v3(0, 2, 0)));
    for (int i = 0; i < 100; i++)
        CHECK(ps_body_step(&a, zero, zero, .01) == PS_OK);
    CHECK(vec_near(ps_quat_rotate(a.orientation, ps_v3(1, 0, 0)), ps_v3(cos(2), sin(2), 0)));
    CHECK(near(energy(&a), .8));
    CHECK(ps_body_box(12, ps_v3(2, 4, 6), &b) == PS_OK);
    b.angular_velocity_rad_s = ps_v3(1, 2, 3);
    CHECK(ps_body_step(&b, zero, zero, 1e-6) == PS_OK);
    CHECK(vec_near(b.angular_velocity_rad_s,
                   ps_v3(1 + 1e-6 * 120 / 52, 2 - 1e-6 * 96 / 40, 3 + 1e-6 * 24 / 20)));
    double coarse = spin_error(.0001), fine = spin_error(.00005);
    CHECK(isfinite(fine) && fine > 0 && coarse / fine > 1.9 && coarse / fine < 2.1);
    for (int n = 100; n <= 200; n *= 2) {
        CHECK(ps_body_sphere(2, 1, &a) == PS_OK);
        for (int i = 0; i < n; i++)
            CHECK(ps_body_step(&a, ps_v3(0, -19.62, 0), zero, 1. / n) == PS_OK);
        CHECK(near(a.velocity_m_s.y, -9.81));
        CHECK(near(a.position_m.y, -4.905 * (1 + 1. / n)));
    }
    CHECK(ps_body_sphere(0, 1, &a) == PS_OK);
    ps_body saved = a;
    CHECK(ps_body_step(&a, ps_v3(1, 2, 3), ps_v3(4, 5, 6), .01) == PS_OK &&
          !memcmp(&a, &saved, sizeof a));
    CHECK(ps_body_apply_impulse(&a, ps_v3(2, 3, 4), zero) == PS_OK &&
          !memcmp(&a, &saved, sizeof a));
    a.velocity_m_s.x = 1;
    CHECK(ps_body_validate(&a) == PS_INVALID);

    for (int elastic = 0; elastic <= 1; elastic++) {
        CHECK(ps_body_sphere(2, 1, &a) == PS_OK && ps_body_sphere(1, 1, &b) == PS_OK);
        a.position_m = ps_v3(-1, 0, 0);
        b.position_m = ps_v3(1, 0, 0);
        a.velocity_m_s = ps_v3(3, 0, 0);
        b.velocity_m_s = ps_v3(-1, 0, 0);
        ps_contact contact;
        bool hit = false;
        CHECK(ps_contact_spheres(&a, 1, &b, 1, &contact, &hit) == PS_OK && hit);
        CHECK(ps_contact_resolve(&a, &b, &contact, (double)elastic, 0, &j) == PS_OK);
        CHECK(near(a.velocity_m_s.x, elastic ? 1. / 3 : 5. / 3));
        CHECK(near(b.velocity_m_s.x, elastic ? 13. / 3 : 5. / 3));
        CHECK(near(momentum(&a).x + momentum(&b).x, 5));
        CHECK(near(energy(&a) + energy(&b), elastic ? 9.5 : 25. / 6));
    }
    for (int stick = 0; stick <= 1; stick++) {
        CHECK(ps_body_sphere(1, 1, &a) == PS_OK);
        a.position_m = ps_v3(0, 1, 0);
        a.velocity_m_s = ps_v3(4, -2, 0);
        ps_contact contact;
        bool hit = false;
        CHECK(ps_contact_sphere_plane(&a, 1, zero, ps_v3(0, 1, 0), &contact, &hit) == PS_OK && hit);
        CHECK(ps_contact_resolve(&a, NULL, &contact, .5, stick ? 1 : .1, &j) == PS_OK);
        double friction = stick ? 8. / 7 : .3;
        CHECK(near(j.x, -friction) && near(j.y, 3));
        CHECK(near(a.velocity_m_s.x, 4 - friction) && near(a.velocity_m_s.y, 1));
        CHECK(near(a.angular_velocity_rad_s.z, -friction / .4));
        CHECK(energy(&a) < 10);
        if (stick)
            CHECK(ps_body_point_velocity(&a, contact.point_m, &v) == PS_OK && near(v.x, 0));
    }
    CHECK(ps_body_sphere(1, 1, &a) == PS_OK && ps_body_sphere(2, 1, &b) == PS_OK);
    a.position_m = ps_v3(-1, 0, 0);
    b.position_m = ps_v3(1, 0, 0);
    a.velocity_m_s = ps_v3(2, 3, 1);
    b.velocity_m_s = ps_v3(-1, 0, 0);
    ps_vec3 before_p = ps_vadd(momentum(&a), momentum(&b));
    ps_vec3 before_l = ps_vadd(angular_momentum(&a), angular_momentum(&b));
    double before_e = energy(&a) + energy(&b);
    ps_contact contact;
    bool hit = false;
    CHECK(ps_contact_spheres(&a, 1, &b, 1, &contact, &hit) == PS_OK && hit);
    CHECK(ps_contact_resolve(&a, &b, &contact, .8, .3, NULL) == PS_OK);
    CHECK(vec_near(before_p, ps_vadd(momentum(&a), momentum(&b))));
    CHECK(vec_near(before_l, ps_vadd(angular_momentum(&a), angular_momentum(&b))));
    CHECK(energy(&a) + energy(&b) < before_e);
    CHECK(ps_body_sphere(1, 1, &a) == PS_OK);
    a.position_m.y = .9;
    a.velocity_m_s.y = 1;
    CHECK(ps_contact_sphere_plane(&a, 1, zero, ps_v3(0, 1, 0), &contact, &hit) == PS_OK && hit);
    CHECK(ps_contact_resolve(&a, NULL, &contact, 1, 1, &j) == PS_OK && near(a.position_m.y, 1));
    CHECK(near(a.velocity_m_s.y, 1) && vec_near(j, zero));
    a.position_m.y = 5;
    ps_contact unchanged_contact = contact;
    CHECK(ps_contact_sphere_plane(&a, 1, zero, ps_v3(0, 1, 0), &contact, &hit) == PS_OK && !hit &&
          !memcmp(&contact, &unchanged_contact, sizeof contact));
    CHECK(ps_contact_spheres(&a, 1, &a, 1, &contact, &hit) == PS_INVALID);
    CHECK(ps_contact_sphere_plane(&a, 1, zero, ps_v3(0, 2, 0), &contact, &hit) == PS_INVALID);
    saved = a;
    j = ps_v3(7, 8, 9);
    CHECK(ps_contact_resolve(&a, NULL, &contact, NAN, 0, &j) == PS_INVALID &&
          !memcmp(&a, &saved, sizeof a) && vec_near(j, ps_v3(7, 8, 9)));
    CHECK(ps_body_step(&a, ps_v3(DBL_MAX, 0, 0), zero, 2) == PS_NUMERIC &&
          !memcmp(&a, &saved, sizeof a));
    CHECK(ps_body_sphere(DBL_MAX, DBL_MAX, &a) == PS_NUMERIC && !memcmp(&a, &saved, sizeof a));
    CHECK(ps_body_step(&a, zero, zero, 0) == PS_INVALID);
    a.orientation.w = 0;
    CHECK(ps_body_validate(&a) == PS_INVALID);
    CHECK(ps_body_sphere(1, 1, &a) == PS_OK && ps_body_sphere(1, 1, &b) == PS_OK);
    b.position_m.x = DBL_MIN * .5;
    CHECK(ps_contact_spheres(&a, 1, &b, 1, &contact, &hit) == PS_OK && hit &&
          vec_near(contact.normal, ps_v3(1, 0, 0)));

    CHECK(ps_sphere_drag(ps_v3(2, 0, 0), PS_AIR, PS_DRAG_QUADRATIC, .5, .4, &v) == PS_OK &&
          near(v.x, -.5 * 1.225 * .4 * PS_PI * .25 * 4));
    CHECK(ps_sphere_drag(ps_v3(2, 0, 0), PS_VACUUM, PS_DRAG_QUADRATIC, .5, .4, &v) == PS_OK &&
          vec_near(v, zero));
    CHECK(ps_sphere_drag(ps_v3(DBL_MAX, DBL_MAX, 0), PS_VACUUM, PS_DRAG_QUADRATIC, .5, .4, &v) ==
              PS_OK &&
          vec_near(v, zero));
    CHECK(ps_sphere_drag(ps_v3(2, 0, 0), PS_WATER, PS_DRAG_NONE, .5, .4, &v) == PS_OK &&
          vec_near(v, zero));
    ps_medium medium = {1, 1 / (6 * PS_PI * .5), "test linear drag"};
    CHECK(ps_sphere_drag(ps_v3(2, 0, 0), medium, PS_DRAG_STOKES, .5, 0, &v) == PS_OK &&
          near(v.x, -2));
    CHECK(ps_body_sphere(1, .5, &a) == PS_OK);
    a.velocity_m_s.x = 2;
    for (int i = 0; i < 1000; i++) {
        CHECK(ps_sphere_drag(a.velocity_m_s, medium, PS_DRAG_STOKES, .5, 0, &v) == PS_OK);
        CHECK(ps_body_step(&a, v, zero, .001) == PS_OK);
    }
    CHECK(near(a.velocity_m_s.x, 2 * pow(.999, 1000)) &&
          fabs(a.velocity_m_s.x - 2 * exp(-1)) < .0004);
    CHECK(ps_spring_force(zero, zero, ps_v3(2, 0, 0), ps_v3(1, 0, 0), 10, 1, 3, &v) == PS_OK &&
          near(v.x, 13));
    CHECK(ps_spring_force(ps_v3(2, 0, 0), ps_v3(1, 0, 0), zero, zero, 10, 1, 3, &j) == PS_OK &&
          vec_near(j, ps_vscale(v, -1)));
    CHECK(ps_spring_force(zero, zero, zero, zero, 10, 1, 3, &v) == PS_SINGULAR);
    CHECK(ps_spring_force(zero, zero, zero, zero, 0, 0, 0, &v) == PS_OK && vec_near(v, zero));
    puts("Mechanics: inertia, torque, rotation, convergence, momentum/energy, friction and media "
         "passed");
    return 0;
}
