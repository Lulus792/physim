#include "physim/mechanics.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Buoyancy line %d: %s\n", __LINE__, #x);                               \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static bool close_to(double a, double b, double tolerance) {
    return fabs(a - b) <= tolerance * fmax(1, fabs(b));
}
int main(void) {
    ps_submersion s;
    CHECK(ps_sphere_submersion(2, 3, &s) == PS_OK && s.volume_m3 == 0 && s.centroid_offset_m == 0);
    CHECK(ps_sphere_submersion(2, 2, &s) == PS_OK && s.volume_m3 == 0);
    CHECK(ps_sphere_submersion(2, -2, &s) == PS_OK &&
          close_to(s.volume_m3, 32 * PS_PI / 3, 1e-14) && s.centroid_offset_m == 0);
    CHECK(ps_sphere_submersion(2, 0, &s) == PS_OK && close_to(s.volume_m3, 16 * PS_PI / 3, 1e-14) &&
          close_to(s.centroid_offset_m, -.75, 1e-14));
    /* Independent numerical integration of cross-sectional area and first moment. */
    double previous = 0;
    for (int k = 0; k <= 100; k++) {
        double height = 2 - .04 * k, top = -height, dz = (top + 2) / 10000;
        double volume = 0, moment = 0;
        CHECK(ps_sphere_submersion(2, height, &s) == PS_OK);
        for (int i = 0; i < 10000; i++) {
            double z = -2 + (i + .5) * dz, dv = PS_PI * (4 - z * z) * dz;
            volume += dv;
            moment += z * dv;
        }
        CHECK(close_to(s.volume_m3, volume, 2e-8) && s.volume_m3 >= previous);
        if (volume > 0)
            CHECK(close_to(s.centroid_offset_m, moment / volume, 2e-8));
        previous = s.volume_m3;
        ps_submersion complement;
        CHECK(ps_sphere_submersion(2, -height, &complement) == PS_OK);
        CHECK(close_to(s.volume_m3 + complement.volume_m3, 32 * PS_PI / 3, 1e-14));
    }
    ps_vec3 force, gravity = {0, -9.81, 0}, zero = {0};
    CHECK(ps_buoyancy_force(1000, .002, gravity, &force) == PS_OK &&
          close_to(force.y, 19.62, 1e-14) && force.x == 0 && force.z == 0);
    CHECK(ps_buoyancy_force(2, 3, ps_v3(1, -2, 4), &force) == PS_OK && force.x == -6 &&
          force.y == 12 && force.z == -24);
    CHECK(ps_buoyancy_force(0, DBL_MAX, gravity, &force) == PS_OK && force.y == 0);
    CHECK(ps_buoyancy_force(DBL_MAX, DBL_MAX, zero, &force) == PS_OK && force.y == 0);
    CHECK(ps_buoyancy_force(1e300, 1e300, ps_v3(0, -1e-300, 0), &force) == PS_OK &&
          close_to(force.y / 1e300, 1, 1e-14));
    CHECK(ps_buoyancy_force(1e-300, 1e-300, ps_v3(0, -1e300, 0), &force) == PS_OK &&
          close_to(force.y / 1e-300, 1, 1e-14));
    CHECK(ps_sphere_submersion(1e103, 1e103 * (1 - 1e-5), &s) == PS_OK && isfinite(s.volume_m3) &&
          s.volume_m3 > 1e298);
    /* Half-density sphere floats half submerged; heavier/lighter bodies sink/rise. */
    CHECK(ps_sphere_submersion(.1, -1, &s) == PS_OK);
    double full = s.volume_m3;
    for (int i = 1; i <= 3; i++) {
        double ratio = .5 * i;
        ps_body b;
        CHECK(ps_body_sphere(1000 * full * ratio, .1, &b) == PS_OK);
        CHECK(ps_buoyancy_force(1000, full, gravity, &force) == PS_OK);
        ps_vec3 net = ps_vadd(force, ps_vscale(gravity, b.mass_kg));
        for (int step = 0; step < 100; step++)
            CHECK(ps_body_step(&b, net, zero, .001) == PS_OK);
        CHECK(close_to(b.velocity_m_s.y, .981 * (1 / ratio - 1), 1e-12));
    }
    ps_body floating;
    CHECK(ps_body_sphere(500 * full, .1, &floating) == PS_OK);
    for (int step = 0; step < 100; step++) {
        CHECK(ps_sphere_submersion(.1, floating.position_m.y, &s) == PS_OK);
        CHECK(ps_buoyancy_force(1000, s.volume_m3, gravity, &force) == PS_OK);
        ps_vec3 point = ps_vadd(floating.position_m, ps_v3(0, s.centroid_offset_m, 0)), torque;
        CHECK(ps_body_force_torque(&floating, force, point, &torque) == PS_OK);
        CHECK(ps_body_step(&floating, ps_vadd(force, ps_vscale(gravity, floating.mass_kg)), torque,
                           .001) == PS_OK);
    }
    CHECK(fabs(floating.position_m.y) < 1e-14 && fabs(floating.angular_velocity_rad_s.z) < 1e-14);
    for (int sign = -1; sign <= 1; sign += 2) {
        CHECK(ps_sphere_submersion(.1, sign * .001, &s) == PS_OK);
        CHECK(ps_buoyancy_force(1000, s.volume_m3, gravity, &force) == PS_OK);
        CHECK(sign * (force.y - floating.mass_kg * 9.81) < 0);
    }
    /* Failed calls leave all outputs untouched. */
    ps_submersion sentinel = {7, 8};
    s = sentinel;
    CHECK(ps_sphere_submersion(0, 0, &s) == PS_INVALID && !memcmp(&s, &sentinel, sizeof s));
    CHECK(ps_sphere_submersion(1, NAN, &s) == PS_INVALID && !memcmp(&s, &sentinel, sizeof s));
    CHECK(ps_sphere_submersion(DBL_MAX, -DBL_MAX, &s) == PS_NUMERIC &&
          !memcmp(&s, &sentinel, sizeof s));
    CHECK(ps_sphere_submersion(1, 0, NULL) == PS_INVALID);
    force = ps_v3(7, 8, 9);
    ps_vec3 saved = force;
    CHECK(ps_buoyancy_force(-1, 1, gravity, &force) == PS_INVALID &&
          !memcmp(&force, &saved, sizeof force));
    CHECK(ps_buoyancy_force(1, -1, gravity, &force) == PS_INVALID &&
          !memcmp(&force, &saved, sizeof force));
    CHECK(ps_buoyancy_force(1, 1, ps_v3(NAN, 0, 0), &force) == PS_INVALID &&
          !memcmp(&force, &saved, sizeof force));
    CHECK(ps_buoyancy_force(DBL_MAX, 2, gravity, &force) == PS_NUMERIC &&
          !memcmp(&force, &saved, sizeof force));
    CHECK(ps_buoyancy_force(1, 1, gravity, NULL) == PS_INVALID);
    puts("Buoyancy: cap quadrature, centroid, equilibrium, sink/rise and numeric contracts passed");
    return 0;
}
