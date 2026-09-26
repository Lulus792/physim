#include "physim/mechanics.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Contacts line %d: %s\n", __LINE__, #x);                               \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static bool near(double a, double b) { return fabs(a - b) < 1e-8; }
static bool vector_near(ps_vec3 a, ps_vec3 b) {
    return near(a.x, b.x) && near(a.y, b.y) && near(a.z, b.z);
}
int main(void) {
    ps_body sphere, box;
    ps_vec3 zero = {0}, up = {0, 1, 0};
    CHECK(ps_body_sphere(1, .5, &sphere) == PS_OK && ps_body_box(0, ps_v3(2, 4, 6), &box) == PS_OK);
    box.position_m = ps_v3(10, 20, 30);
    box.orientation = ps_quat_axis_angle(ps_v3(0, 0, 1), PS_PI / 2);
    sphere.position_m = ps_v3(10, 21.4, 30);
    ps_contact c = {0};
    bool hit = false;
    CHECK(ps_contact_sphere_box(&sphere, .5, &box, ps_v3(2, 4, 6), &c, &hit) == PS_OK && hit);
    CHECK(vector_near(c.point_m, ps_v3(10, 21, 30)) && vector_near(c.normal, ps_v3(0, -1, 0)) &&
          near(c.penetration_m, .1));
    sphere.position_m = ps_v3(10, 20.8, 30);
    CHECK(ps_contact_sphere_box(&sphere, .5, &box, ps_v3(2, 4, 6), &c, &hit) == PS_OK && hit &&
          near(c.penetration_m, .7));
    CHECK(ps_contact_resolve(&sphere, &box, &c, 0, 0, NULL) == PS_OK &&
          vector_near(sphere.position_m, ps_v3(10, 21.5, 30)));
    sphere.position_m = ps_v3(7.6, 21.3, 33);
    CHECK(ps_contact_sphere_box(&sphere, .6, &box, ps_v3(2, 4, 6), &c, &hit) == PS_OK && hit);
    CHECK(vector_near(c.point_m, ps_v3(8, 21, 33)) && vector_near(c.normal, ps_v3(.8, -.6, 0)) &&
          near(c.penetration_m, .1));
    sphere.position_m = ps_v3(100, 100, 100);
    ps_contact saved_contact = c;
    CHECK(ps_contact_sphere_box(&sphere, .5, &box, ps_v3(2, 4, 6), &c, &hit) == PS_OK && !hit &&
          !memcmp(&c, &saved_contact, sizeof c));
    sphere.position_m = box.position_m;
    CHECK(ps_contact_sphere_box(&sphere, .5, &box, ps_v3(2, 2, 2), &c, &hit) == PS_OK && hit &&
          vector_near(c.normal, ps_v3(0, -1, 0)) && near(c.penetration_m, 1.5));
    CHECK(ps_contact_sphere_box(&sphere, .5, &box, ps_v3(0, 2, 2), &c, &hit) == PS_INVALID);
    CHECK(ps_contact_sphere_box(&box, .5, &box, ps_v3(2, 2, 2), &c, &hit) == PS_INVALID);

    ps_contact_manifold manifold = {0};
    CHECK(ps_body_box(1, ps_v3(2, 2, 2), &box) == PS_OK);
    box.position_m.y = .95;
    CHECK(ps_contacts_box_plane(&box, ps_v3(2, 2, 2), zero, up, &manifold) == PS_OK &&
          manifold.count == 4);
    for (uint32_t i = 0; i < manifold.count; i++)
        CHECK(near(manifold.points[i].point_m.y, -.05) &&
              near(manifold.points[i].penetration_m, .05) &&
              vector_near(manifold.points[i].normal, ps_v3(0, -1, 0)));
    box.orientation = ps_quat_axis_angle(ps_v3(0, 0, 1), PS_PI / 4);
    box.position_m.y = 1.3;
    CHECK(ps_contacts_box_plane(&box, ps_v3(2, 2, 2), zero, up, &manifold) == PS_OK &&
          manifold.count == 2);
    for (uint32_t i = 0; i < manifold.count; i++)
        CHECK(near(manifold.points[i].penetration_m, sqrt(2) - 1.3));
    box.position_m.y = 3;
    CHECK(ps_contacts_box_plane(&box, ps_v3(2, 2, 2), zero, up, &manifold) == PS_OK &&
          !manifold.count);
    box.position_m.y = -3;
    CHECK(ps_contacts_box_plane(&box, ps_v3(2, 2, 2), zero, up, &manifold) == PS_OK &&
          manifold.count == 8);
    ps_contact_manifold saved_manifold = manifold;
    CHECK(ps_contacts_box_plane(&box, ps_v3(2, 2, 2), zero, ps_v3(0, 2, 0), &manifold) ==
              PS_INVALID &&
          !memcmp(&manifold, &saved_manifold, sizeof manifold));

    ps_contact_solver settings = PS_CONTACT_SOLVER_DEFAULT;
    settings.iterations = 128;
    settings.friction = 0;
    settings.restitution = .5;
    settings.penetration_slop_m = .001;
    CHECK(ps_body_box(1, ps_v3(2, 2, 2), &box) == PS_OK);
    box.position_m.y = .95;
    box.velocity_m_s = ps_v3(2, -3, 0);
    CHECK(ps_contacts_box_plane(&box, ps_v3(2, 2, 2), zero, up, &manifold) == PS_OK);
    ps_contact_solution solution;
    CHECK(ps_contacts_resolve(&box, NULL, &manifold, &settings, &solution) == PS_OK);
    CHECK(vector_near(box.velocity_m_s, ps_v3(2, 1.5, 0)) &&
          vector_near(box.angular_velocity_rad_s, zero));
    CHECK(near(box.position_m.y, .95 + .8 * (.05 - .001)) && solution.max_normal_error_m_s < 1e-8);
    ps_vec3 total = {0};
    for (uint32_t i = 0; i < solution.count; i++)
        total = ps_vadd(total, solution.impulse_on_a_ns[i]);
    CHECK(vector_near(total, ps_v3(0, 4.5, 0)));
    for (int sticking = 0; sticking <= 1; sticking++) {
        CHECK(ps_body_box(1, ps_v3(2, 2, 2), &box) == PS_OK);
        box.position_m.y = 1;
        box.velocity_m_s = ps_v3(2, -3, 0);
        settings.restitution = 0;
        settings.friction = sticking ? 1 : .5;
        CHECK(ps_contacts_box_plane(&box, ps_v3(2, 2, 2), zero, up, &manifold) == PS_OK);
        CHECK(ps_contacts_resolve(&box, NULL, &manifold, &settings, &solution) == PS_OK);
        CHECK(vector_near(box.velocity_m_s, ps_v3(sticking ? 0 : .5, 0, 0)));
        CHECK(vector_near(box.angular_velocity_rad_s, zero) &&
              solution.max_normal_error_m_s < 1e-8);
        for (uint32_t i = 0; i < solution.count; i++) {
            ps_vec3 impulse = solution.impulse_on_a_ns[i];
            CHECK(impulse.y >= -1e-12 &&
                  hypot(impulse.x, impulse.z) <= settings.friction * impulse.y + 1e-10);
        }
    }
    /* Oblique sliding with unequal principal inertias exercises the 2D cone. */
    CHECK(ps_body_box(1, ps_v3(2, 1, 3), &box) == PS_OK);
    box.position_m.y = .5;
    box.velocity_m_s = ps_v3(1.2, -3, 1.6);
    settings.iterations = 128;
    settings.friction = .5;
    CHECK(ps_contacts_box_plane(&box, ps_v3(2, 1, 3), zero, up, &manifold) == PS_OK);
    CHECK(ps_contacts_resolve(&box, NULL, &manifold, &settings, &solution) == PS_OK);
    CHECK(vector_near(box.velocity_m_s, ps_v3(.3, 0, .4)));
    CHECK(vector_near(box.angular_velocity_rad_s, zero));
    /* Two finite masses: shared manifold impulses preserve total linear momentum. */
    CHECK(ps_body_sphere(2, 1, &sphere) == PS_OK && ps_body_sphere(1, 1, &box) == PS_OK);
    sphere.position_m.x = -1;
    box.position_m.x = 1;
    sphere.velocity_m_s.x = 3;
    box.velocity_m_s.x = -1;
    CHECK(ps_contact_spheres(&sphere, 1, &box, 1, &c, &hit) == PS_OK && hit);
    manifold.count = 4;
    for (unsigned i = 0; i < 4; i++)
        manifold.points[i] = c; /* redundant contacts must not multiply impact */
    settings.restitution = 1;
    settings.friction = 0;
    CHECK(ps_contacts_resolve(&sphere, &box, &manifold, &settings, &solution) == PS_OK);
    CHECK(near(sphere.velocity_m_s.x, 1. / 3) && near(box.velocity_m_s.x, 13. / 3));
    CHECK(near(2 * sphere.velocity_m_s.x + box.velocity_m_s.x, 5));
    double ea, eb;
    CHECK(ps_body_kinetic_energy(&sphere, &ea) == PS_OK &&
          ps_body_kinetic_energy(&box, &eb) == PS_OK && near(ea + eb, 9.5));

    /* Stable resting support across 2000 integration/contact steps, yawed box. */
    CHECK(ps_body_box(1, ps_v3(2, 2, 2), &box) == PS_OK);
    box.position_m.y = 3;
    box.orientation = ps_quat_axis_angle(up, .3);
    settings = PS_CONTACT_SOLVER_DEFAULT;
    for (unsigned i = 0; i < 2000; i++) {
        CHECK(ps_body_step(&box, ps_v3(0, -9.81, 0), zero, .005) == PS_OK);
        CHECK(ps_contacts_box_plane(&box, ps_v3(2, 2, 2), zero, up, &manifold) == PS_OK);
        CHECK(ps_contacts_resolve(&box, NULL, &manifold, &settings, &solution) == PS_OK);
        CHECK(ps_body_kinetic_energy(&box, &ea) == PS_OK && ea + 9.81 * box.position_m.y < 29.44);
    }
    CHECK(fabs(box.position_m.y - 1) < .0002 && ps_vlength(box.velocity_m_s) < 1e-6 &&
          ps_vlength(box.angular_velocity_rad_s) < 1e-6 && solution.max_normal_error_m_s < 1e-6);
    ps_body saved_body = box;
    ps_contact_solution saved_solution = solution;
    manifold.points[manifold.count - 1].normal = zero;
    CHECK(ps_contacts_resolve(&box, NULL, &manifold, &settings, &solution) == PS_INVALID &&
          !memcmp(&box, &saved_body, sizeof box) &&
          !memcmp(&solution, &saved_solution, sizeof solution));
    manifold.count = 0;
    CHECK(ps_contacts_resolve(&box, NULL, &manifold, &settings, &solution) == PS_OK &&
          !solution.count && !memcmp(&box, &saved_body, sizeof box));
    settings.iterations = 0;
    CHECK(ps_contacts_resolve(&box, NULL, &manifold, &settings, &solution) == PS_INVALID);
    settings = PS_CONTACT_SOLVER_DEFAULT;
    manifold.count = 1;
    manifold.points[0] = (ps_contact){ps_v3(DBL_MAX, DBL_MAX, DBL_MAX), up, 0};
    CHECK(ps_contacts_resolve(&box, NULL, &manifold, &settings, &solution) == PS_NUMERIC &&
          !memcmp(&box, &saved_body, sizeof box));
    puts("Contacts: oriented geometry, impact, Coulomb cone, duplicate points, support and "
         "transactional errors passed");
    return 0;
}
