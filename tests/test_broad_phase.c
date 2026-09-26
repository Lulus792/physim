#include "physim/collision.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Broad phase line %d: %s\n", __LINE__, #x);                            \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static bool overlap(ps_aabb a, ps_aabb b) {
    return a.minimum_m.x <= b.maximum_m.x && b.minimum_m.x <= a.maximum_m.x &&
           a.minimum_m.y <= b.maximum_m.y && b.minimum_m.y <= a.maximum_m.y &&
           a.minimum_m.z <= b.maximum_m.z && b.minimum_m.z <= a.maximum_m.z;
}
static bool contains(ps_aabb a, ps_vec3 p) {
    return a.minimum_m.x <= p.x && p.x <= a.maximum_m.x && a.minimum_m.y <= p.y &&
           p.y <= a.maximum_m.y && a.minimum_m.z <= p.z && p.z <= a.maximum_m.z;
}
static unsigned rng = 12345;
static double random_value(void) {
    rng = rng * 1664525u + 1013904223u;
    return (double)(rng >> 8) / 16777216.;
}
int main(void) {
    size_t max_pairs = 1024u * 1023u / 2;
    ps_collision_pair *pairs = malloc(max_pairs * sizeof *pairs);
    CHECK(pairs);
    ps_aabb bounds[1024];
    for (unsigned trial = 0; trial < 80; trial++) {
        for (unsigned i = 0; i < 128; i++) {
            ps_vec3 p = ps_v3(random_value() * 8 - 4, random_value() * 8 - 4, random_value() * 8 - 4);
            ps_vec3 size = ps_v3(random_value() * 3, random_value() * 3, random_value() * 3);
            bounds[i] = (ps_aabb){p, ps_vadd(p, size)};
        }
        size_t got = 0;
        CHECK(ps_broad_phase(bounds, 128, pairs, max_pairs, &got) == PS_OK);
        size_t expected = 0;
        for (unsigned a = 0; a < 128; a++)
            for (unsigned b = a + 1; b < 128; b++)
                if (overlap(bounds[a], bounds[b])) {
                    CHECK(expected < got && pairs[expected].a == a && pairs[expected].b == b);
                    expected++;
                }
        CHECK(got == expected);
    }
    for (unsigned i = 0; i < 1024; i++)
        bounds[i] = (ps_aabb){{0, 0, 0}, {1, 1, 1}};
    size_t got = 99;
    pairs[0] = (ps_collision_pair){999, 998};
    CHECK(ps_broad_phase(bounds, 1024, pairs, max_pairs - 1, &got) == PS_LIMIT &&
          got == max_pairs && pairs[0].a == 999);
    CHECK(ps_broad_phase(bounds, 1024, NULL, 0, &got) == PS_LIMIT && got == max_pairs);
    CHECK(ps_broad_phase(bounds, 1024, pairs, max_pairs, &got) == PS_OK && got == max_pairs);
    size_t index = 0;
    for (unsigned a = 0; a < 1024; a++)
        for (unsigned b = a + 1; b < 1024; b++) {
            CHECK(pairs[index].a == a && pairs[index].b == b);
            index++;
        }
    bounds[1] = (ps_aabb){{1, 1, 1}, {2, 2, 2}};
    CHECK(ps_broad_phase(bounds, 2, pairs, 1, &got) == PS_OK && got == 1);
    bounds[1].minimum_m.x = nextafter(1, INFINITY);
    CHECK(ps_broad_phase(bounds, 2, NULL, 0, &got) == PS_OK && got == 0);
    bounds[1].minimum_m.x = NAN;
    got = 987;
    pairs[0] = (ps_collision_pair){123, 456};
    CHECK(ps_broad_phase(bounds, 2, pairs, 1, &got) == PS_INVALID && got == 987 &&
          pairs[0].a == 123);
    CHECK(ps_broad_phase(bounds, 1025, pairs, 1, &got) == PS_LIMIT && got == 987);
    CHECK(ps_broad_phase(NULL, 0, NULL, 0, &got) == PS_OK && got == 0);
    ps_body body;
    CHECK(ps_body_box(1, ps_v3(2, 3, 4), &body) == PS_OK);
    ps_aabb box;
    for (unsigned trial = 0; trial < 200; trial++) {
        body.position_m = ps_v3(random_value() * 100, random_value() * 100, random_value() * 100);
        body.orientation = ps_quat_axis_angle(ps_v3(1, 2, 3), random_value() * 6);
        CHECK(ps_aabb_box(&body, ps_v3(2, 3, 4), &box) == PS_OK);
        for (unsigned c = 0; c < 8; c++) {
            ps_vec3 v = ps_v3(c & 1 ? 1 : -1, c & 2 ? 1.5 : -1.5, c & 4 ? 2 : -2);
            CHECK(contains(box, ps_vadd(body.position_m, ps_quat_rotate(body.orientation, v))));
        }
        CHECK(ps_aabb_sphere(&body, .5, &box) == PS_OK);
        CHECK(contains(box, ps_vadd(body.position_m, ps_v3(.5, 0, 0))) &&
              contains(box, ps_vadd(body.position_m, ps_v3(0, -.5, 0))));
    }
    ps_aabb saved = box;
    CHECK(ps_aabb_sphere(&body, -1, &box) == PS_INVALID && !memcmp(&box, &saved, sizeof box));
    CHECK(ps_aabb_box(&body, ps_v3(1, 0, 1), &box) == PS_INVALID &&
          !memcmp(&box, &saved, sizeof box));
    body.position_m.x = DBL_MAX;
    CHECK(ps_aabb_sphere(&body, 1, &box) == PS_NUMERIC && !memcmp(&box, &saved, sizeof box));
    /* Actual rotated narrow-phase contacts must survive candidate culling. */
    for (unsigned trial = 0; trial < 200; trial++) {
        ps_body a, b;
        CHECK(ps_body_box(1, ps_v3(2, 2, 2), &a) == PS_OK);
        CHECK(ps_body_box(1, ps_v3(2, 2, 2), &b) == PS_OK);
        a.orientation = ps_quat_axis_angle(ps_v3(1, 2, 3), random_value() * 6);
        b.orientation = ps_quat_axis_angle(ps_v3(3, 1, 2), random_value() * 6);
        b.position_m = ps_v3(random_value() * 4, random_value() * 4, random_value() * 4);
        ps_contact_manifold contact;
        CHECK(ps_contacts_boxes(&a, ps_v3(2, 2, 2), &b, ps_v3(2, 2, 2), &contact) == PS_OK);
        CHECK(ps_aabb_box(&a, ps_v3(2, 2, 2), &bounds[0]) == PS_OK);
        CHECK(ps_aabb_box(&b, ps_v3(2, 2, 2), &bounds[1]) == PS_OK);
        CHECK(ps_broad_phase(bounds, 2, pairs, 1, &got) == PS_OK);
        CHECK(!contact.count || got == 1);
    }
    free(pairs);
    puts("Broad phase: brute-force agreement, capacity, bounds and narrow-phase coverage passed");
    return 0;
}
