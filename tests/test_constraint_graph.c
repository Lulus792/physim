#include "physim/mechanics.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Mixed graph line %d: %s\n", __LINE__, #x);                            \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
int main(void) {
    ps_body bodies[2];
    for (unsigned i = 0; i < 2; i++) {
        CHECK(ps_body_sphere(1, .5, &bodies[i]) == PS_OK);
        bodies[i].position_m.y = .5 + i;
        bodies[i].velocity_m_s.y = -1;
    }
    ps_body initial[2];
    memcpy(initial, bodies, sizeof bodies);
    ps_contact_constraint floor = {0, PS_CONTACT_WORLD, {{0, 0, 0}, {0, -1, 0}, 0}};
    ps_distance_constraint joint = {0, 1, {{0}, {0}, 1, .2}};
    ps_contact_solver settings = PS_CONTACT_SOLVER_DEFAULT;
    settings.iterations = 128;
    settings.friction = 0;
    ps_constraint_graph_solution out;
    CHECK(ps_constraints_resolve_graph(bodies, 2, &floor, 1, &joint, 1, &settings, .01, &out) ==
          PS_OK);
    CHECK(fabs(bodies[0].velocity_m_s.y) < 1e-14 && fabs(bodies[1].velocity_m_s.y) < 1e-14);
    CHECK(fabs(out.contacts.impulse_on_a_ns[0].y - 2) < 1e-14);
    CHECK(fabs(out.joint_impulse_on_a_ns[0].y + 1) < 1e-14);
    CHECK(out.max_joint_velocity_error_m_s < 1e-14 && out.contacts.max_normal_error_m_s < 1e-14);
    CHECK(out.max_joint_length_error_m == 0);
    /* Restitution must retain its pre-joint initial target. */
    memcpy(bodies, initial, sizeof bodies);
    settings.restitution = 1;
    CHECK(ps_constraints_resolve_graph(bodies, 2, &floor, 1, &joint, 1, &settings, .01, &out) ==
          PS_OK);
    CHECK(fabs(bodies[0].velocity_m_s.y - 1) < 1e-14 && fabs(bodies[1].velocity_m_s.y - 1) < 1e-14);
    CHECK(fabs(out.contacts.impulse_on_a_ns[0].y - 4) < 1e-14);
    settings.restitution = 0;
    settings.iterations = 1;
    memcpy(bodies, initial, sizeof bodies);
    CHECK(ps_constraints_resolve_graph(bodies, 2, &floor, 1, &joint, 1, &settings, .01, &out) ==
          PS_OK);
    CHECK(fabs(out.contacts.max_normal_error_m_s - .5) < 1e-14);
    settings.iterations = 128;
    /* Contact projection changes length and must be visible in final residual. */
    memcpy(bodies, initial, sizeof bodies);
    floor.contact.penetration_m = .1;
    settings.correction_fraction = 1;
    settings.penetration_slop_m = 0;
    CHECK(ps_constraints_resolve_graph(bodies, 2, &floor, 1, &joint, 1, &settings, .01, &out) ==
          PS_OK);
    CHECK(fabs(out.max_joint_length_error_m - .1) < 1e-14);
    floor.contact.penetration_m = 0;
    /* A contact-only call is identical to the original public entry point. */
    ps_body old[2];
    memcpy(old, initial, sizeof old);
    memcpy(bodies, initial, sizeof bodies);
    ps_contact_graph_solution contact_out;
    CHECK(ps_contacts_resolve_graph(old, 2, &floor, 1, &settings, &contact_out) == PS_OK);
    CHECK(ps_constraints_resolve_graph(bodies, 2, &floor, 1, NULL, 0, &settings, .01, &out) ==
          PS_OK);
    CHECK(memcmp(old, bodies, sizeof old) == 0);
    CHECK(memcmp(&contact_out, &out.contacts, sizeof contact_out) == 0);
    /* Later joints invalidate earlier ones: residual must use final velocities. */
    ps_distance_constraint conflict[2] = {{0, PS_CONTACT_WORLD, {{0}, {0, 1.5, 0}, 1, .1}},
                                          {0, PS_CONTACT_WORLD, {{0}, {0, 1.5, 0}, 2, .1}}};
    memcpy(bodies, initial, sizeof bodies);
    CHECK(ps_constraints_resolve_graph(bodies, 2, NULL, 0, conflict, 2, &settings, .1, &out) ==
          PS_OK);
    CHECK(fabs(out.max_joint_velocity_error_m_s - 1) < 1e-14);
    /* Late failure rolls back contact impulses as well as joint impulses. */
    memcpy(bodies, initial, sizeof bodies);
    memset(&out, 0xa5, sizeof out);
    ps_constraint_graph_solution saved = out;
    joint.joint.length_m = 0;
    CHECK(ps_constraints_resolve_graph(bodies, 2, &floor, 1, &joint, 1, &settings, .01, &out) ==
          PS_INVALID);
    joint.joint.length_m = 2;
    CHECK(ps_constraints_resolve_graph(bodies, 2, &floor, 1, &joint, 1, &settings,
                                       DBL_MIN * DBL_EPSILON, &out) == PS_NUMERIC);
    CHECK(memcmp(bodies, initial, sizeof bodies) == 0 && memcmp(&out, &saved, sizeof out) == 0);
    CHECK(ps_constraints_resolve_graph(bodies, 2, NULL, 0, &joint, 257, &settings, .01, &out) ==
          PS_LIMIT);
    CHECK(ps_constraints_resolve_graph(bodies, 2, NULL, 0, NULL, 1, &settings, .01, &out) ==
          PS_INVALID);
    CHECK(ps_constraints_resolve_graph(NULL, 0, NULL, 0, NULL, 0, &settings, .01, &out) == PS_OK);
    CHECK(out.joint_count == 0 && out.contacts.count == 0 && out.max_joint_velocity_error_m_s == 0);
    /* Exercise all three capacity limits simultaneously, with redundant rows. */
    ps_body many[128];
    ps_contact_constraint contacts[512];
    ps_distance_constraint joints[256];
    for (unsigned i = 0; i < 128; i++) {
        CHECK(ps_body_sphere(1, .5, &many[i]) == PS_OK);
        many[i].position_m.y = 2 * i;
        many[i].velocity_m_s = ps_v3(1, -1, 0);
        for (unsigned k = 0; k < 4; k++)
            contacts[4 * i + k] =
                (ps_contact_constraint){i, PS_CONTACT_WORLD, {many[i].position_m, {0, -1, 0}, 0}};
        for (unsigned k = 0; k < 2; k++)
            joints[2 * i + k] =
                (ps_distance_constraint){i, PS_CONTACT_WORLD, {{0}, {1, 2 * i, 0}, 1, .2}};
    }
    CHECK(ps_constraints_resolve_graph(many, 128, contacts, 512, joints, 256, &settings, .01,
                                       &out) == PS_OK);
    CHECK(out.joint_count == 256 && out.contacts.count == 512);
    for (unsigned i = 0; i < 128; i++) {
        CHECK(fabs(many[i].velocity_m_s.x) < 1e-14 && fabs(many[i].velocity_m_s.y) < 1e-14);
        CHECK(fabs(out.joint_impulse_on_a_ns[2 * i].x + out.joint_impulse_on_a_ns[2 * i + 1].x +
                   1) < 1e-14);
    }
    puts("Mixed graph: coupled support, restitution, residuals, projection, limits and atomicity "
         "passed");
    return 0;
}
