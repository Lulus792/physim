#include "physim/collision.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Sweep line %d: %s\n", __LINE__, #x);                                  \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
int main(void) {
    ps_body a, b;
    CHECK(ps_body_sphere(1, 1, &a) == PS_OK && ps_body_sphere(1, 1, &b) == PS_OK);
    ps_vec3 zero = {0}, motion = {20, 0, 0};
    b.position_m.x = 10;
    ps_sweep_hit hit = {0};
    bool touching = false;
    CHECK(ps_sweep_spheres(&a, 1, motion, &b, 1, zero, &hit, &touching) == PS_OK && touching);
    CHECK(fabs(hit.fraction - .4) < 1e-14 && hit.contact.normal.x == 1 &&
          fabs(hit.contact.point_m.x - 9) < 1e-13);
    ps_sweep_hit reverse;
    CHECK(ps_sweep_spheres(&b, 1, zero, &a, 1, motion, &reverse, &touching) == PS_OK && touching);
    CHECK(reverse.fraction == hit.fraction && reverse.contact.normal.x == -1 &&
          reverse.contact.point_m.x == hit.contact.point_m.x);
    /* Motion of both bodies: separation 10, relative travel 10, contact at .8. */
    CHECK(ps_sweep_spheres(&a, 1, ps_v3(15, 0, 0), &b, 1, ps_v3(5, 0, 0), &hit, &touching) ==
              PS_OK &&
          touching && fabs(hit.fraction - .8) < 1e-14);
    ps_sweep_hit saved = hit;
    CHECK(ps_sweep_spheres(&a, 1, zero, &b, 1, zero, &hit, &touching) == PS_OK && !touching &&
          !memcmp(&hit, &saved, sizeof hit));
    CHECK(ps_sweep_spheres(&a, 1, ps_v3(-20, 0, 0), &b, 1, zero, &hit, &touching) == PS_OK &&
          !touching);
    CHECK(ps_sweep_spheres(&a, 1, ps_v3(7, 0, 0), &b, 1, zero, &hit, &touching) == PS_OK &&
          !touching);
    CHECK(ps_sweep_spheres(&a, 1, ps_v3(8, 0, 0), &b, 1, zero, &hit, &touching) == PS_OK &&
          touching && hit.fraction == 1);
    b.position_m.y = 2;
    CHECK(ps_sweep_spheres(&a, 1, motion, &b, 1, zero, &hit, &touching) == PS_OK && touching &&
          fabs(hit.fraction - .5) < 1e-14);
    CHECK(fabs(hit.contact.normal.y - 1) < 1e-14);
    b.position_m.y = 2.001;
    CHECK(ps_sweep_spheres(&a, 1, motion, &b, 1, zero, &hit, &touching) == PS_OK && !touching);
    b.position_m = ps_v3(1, 0, 0);
    CHECK(ps_sweep_spheres(&a, 1, ps_v3(-10, 0, 0), &b, 1, zero, &hit, &touching) == PS_OK &&
          touching && hit.fraction == 0 && hit.contact.penetration_m == 1);
    /* Long path and small target: quadratic discriminant cancellation case. */
    b.position_m = ps_v3(1e12, 0, 0);
    CHECK(ps_sweep_spheres(&a, 1, ps_v3(2e12, 0, 0), &b, 1, zero, &hit, &touching) == PS_OK &&
          touching);
    CHECK(fabs(hit.fraction - (1e12 - 2) / 2e12) < 1e-16);
    /* Cross-check geometric distance at entry for oblique trajectories. */
    for (unsigned i = 0; i < 100; i++) {
        double offset = (double)i / 50;
        b.position_m = ps_v3(10, offset, 0);
        CHECK(ps_sweep_spheres(&a, 1, motion, &b, 1, zero, &hit, &touching) == PS_OK && touching);
        double expected = (10 - sqrt(4 - offset * offset)) / 20;
        CHECK(fabs(hit.fraction - expected) < 1e-14);
        ps_vec3 d = ps_vsub(b.position_m, ps_vscale(motion, hit.fraction));
        CHECK(fabs(ps_vlength(d) - 2) < 1e-12 && fabs(ps_vlength(hit.contact.normal) - 1) < 1e-14);
    }
    a.position_m = ps_v3(0, 10, 0);
    CHECK(ps_sweep_sphere_plane(&a, 1, ps_v3(3, -20, 0), zero, ps_v3(0, 1, 0), &hit, &touching) ==
              PS_OK &&
          touching);
    CHECK(hit.fraction == .45 && fabs(hit.contact.point_m.y) < 1e-14 &&
          fabs(hit.contact.point_m.x - 1.35) < 1e-14 && hit.contact.normal.y == -1);
    /* Resolve an elastic impact at its event time, then advance the remainder. */
    ps_body bounced = a;
    bounced.velocity_m_s = ps_v3(0, -20, 0);
    ps_sweep_hit bounce;
    CHECK(ps_sweep_sphere_plane(&bounced, 1, ps_v3(0, -20, 0), zero, ps_v3(0, 1, 0), &bounce,
                                &touching) == PS_OK &&
          touching);
    bounced.position_m =
        ps_vadd(bounced.position_m, ps_vscale(bounced.velocity_m_s, bounce.fraction));
    CHECK(ps_contact_resolve(&bounced, NULL, &bounce.contact, 1, 0, NULL) == PS_OK);
    bounced.position_m =
        ps_vadd(bounced.position_m, ps_vscale(bounced.velocity_m_s, 1 - bounce.fraction));
    double energy;
    CHECK(ps_body_kinetic_energy(&bounced, &energy) == PS_OK && fabs(energy - 200) < 1e-12 &&
          fabs(bounced.position_m.y - 12) < 1e-12);
    CHECK(ps_sweep_sphere_plane(&a, 1, ps_v3(0, -9, 0), zero, ps_v3(0, 1, 0), &hit, &touching) ==
              PS_OK &&
          touching && hit.fraction == 1);
    saved = hit;
    CHECK(ps_sweep_sphere_plane(&a, 1, ps_v3(10, 0, 0), zero, ps_v3(0, 1, 0), &hit, &touching) ==
              PS_OK &&
          !touching && !memcmp(&hit, &saved, sizeof hit));
    CHECK(ps_sweep_sphere_plane(&a, 1, ps_v3(0, 10, 0), zero, ps_v3(0, 1, 0), &hit, &touching) ==
              PS_OK &&
          !touching);
    a.position_m.y = .5;
    CHECK(ps_sweep_sphere_plane(&a, 1, ps_v3(0, 10, 0), zero, ps_v3(0, 1, 0), &hit, &touching) ==
              PS_OK &&
          touching && hit.fraction == 0 && hit.contact.penetration_m == .5);
    saved = hit;
    touching = true;
    CHECK(ps_sweep_spheres(&a, 1, ps_v3(NAN, 0, 0), &b, 1, zero, &hit, &touching) == PS_INVALID &&
          touching && !memcmp(&hit, &saved, sizeof hit));
    CHECK(ps_sweep_sphere_plane(&a, 1, zero, zero, ps_v3(0, 2, 0), &hit, &touching) == PS_INVALID &&
          touching && !memcmp(&hit, &saved, sizeof hit));
    a.position_m = zero;
    b.position_m = ps_v3(10, 0, 0);
    CHECK(ps_sweep_spheres(&a, 1, ps_v3(DBL_MAX, 0, 0), &b, 1, ps_v3(-DBL_MAX, 0, 0), &hit,
                           &touching) == PS_NUMERIC &&
          touching && !memcmp(&hit, &saved, sizeof hit));
    ps_aabb bounds[2];
    ps_collision_pair pair;
    size_t count;
    CHECK(ps_aabb_sphere(&a, 1, &bounds[0]) == PS_OK && ps_aabb_sphere(&b, 1, &bounds[1]) == PS_OK);
    CHECK(ps_broad_phase(bounds, 2, &pair, 1, &count) == PS_OK && count == 0);
    CHECK(ps_aabb_swept_sphere(&a, 1, motion, &bounds[0]) == PS_OK);
    CHECK(bounds[0].minimum_m.x < -1 && bounds[0].maximum_m.x > 21);
    CHECK(ps_broad_phase(bounds, 2, &pair, 1, &count) == PS_OK && count == 1);
    ps_aabb old = bounds[0];
    a.position_m.x = DBL_MAX;
    CHECK(ps_aabb_swept_sphere(&a, 1, motion, &bounds[0]) == PS_NUMERIC &&
          !memcmp(&old, &bounds[0], sizeof old));
    puts("Sphere sweeps: tunneling, tangent, initial overlap, moving pairs, bounds and failures "
         "passed");
    return 0;
}
