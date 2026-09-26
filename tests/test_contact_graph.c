#include "physim/mechanics.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Contact graph line %d: %s\n", __LINE__, #x);                          \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
int main(void) {
    ps_body bodies[3];
    for (unsigned i = 0; i < 3; i++) {
        CHECK(ps_body_sphere(1, .5, &bodies[i]) == PS_OK);
        bodies[i].position_m.x = i;
    }
    bodies[0].velocity_m_s.x = 1;
    ps_body initial[3];
    memcpy(initial, bodies, sizeof bodies);
    ps_contact_constraint contacts[3] = {{0, 1, {{.5, 0, 0}, {1, 0, 0}, 0}},
                                         {1, 2, {{1.5, 0, 0}, {1, 0, 0}, 0}}};
    ps_contact_solver settings = PS_CONTACT_SOLVER_DEFAULT;
    settings.iterations = 128;
    settings.friction = 0;
    settings.correction_fraction = 1;
    settings.penetration_slop_m = 0;
    ps_contact_graph_solution solution;
    CHECK(ps_contacts_resolve_graph(bodies, 3, contacts, 2, &settings, &solution) == PS_OK);
    double energy = 0;
    for (unsigned i = 0; i < 3; i++) {
        CHECK(fabs(bodies[i].velocity_m_s.x - 1. / 3) < 1e-12);
        double e;
        CHECK(ps_body_kinetic_energy(&bodies[i], &e) == PS_OK);
        energy += e;
    }
    CHECK(fabs(energy - 1. / 6) < 1e-12 && solution.count == 2 &&
          solution.max_normal_error_m_s < 1e-12);
    CHECK(fabs(solution.impulse_on_a_ns[0].x + 2. / 3) < 1e-12 &&
          fabs(solution.impulse_on_a_ns[1].x + 1. / 3) < 1e-12);
    ps_contact_constraint reversed[] = {contacts[1], contacts[0]};
    memcpy(bodies, initial, sizeof bodies);
    CHECK(ps_contacts_resolve_graph(bodies, 3, reversed, 2, &settings, &solution) == PS_OK);
    for (unsigned i = 0; i < 3; i++)
        CHECK(fabs(bodies[i].velocity_m_s.x - 1. / 3) < 1e-12);
    /* A single sweep leaves a contact violation; report it instead of promising convergence. */
    memcpy(bodies, initial, sizeof bodies);
    settings.iterations = 1;
    CHECK(ps_contacts_resolve_graph(bodies, 3, contacts, 2, &settings, &solution) == PS_OK &&
          solution.max_normal_error_m_s > .1);
    settings.iterations = 128;
    /* Three vertical bodies supported by the world; velocity and position corrections propagate. */
    for (unsigned i = 0; i < 3; i++) {
        CHECK(ps_body_sphere(1, .5, &bodies[i]) == PS_OK);
        bodies[i].position_m.y = .4 + .9 * i;
        bodies[i].velocity_m_s.y = -1;
        contacts[i] = (ps_contact_constraint){
            i, i ? i - 1 : PS_CONTACT_WORLD, {{0, (double)i, 0}, {0, -1, 0}, .1}};
    }
    CHECK(ps_contacts_resolve_graph(bodies, 3, contacts, 3, &settings, &solution) == PS_OK);
    for (unsigned i = 0; i < 3; i++) {
        CHECK(fabs(bodies[i].velocity_m_s.y) < 1e-12);
        CHECK(fabs(bodies[i].position_m.y - (.5 + i)) < 1e-12);
        CHECK(fabs(solution.impulse_on_a_ns[i].y - (3 - i)) < 1e-12);
    }
    CHECK(solution.max_projection_error_m < 1e-12 && solution.max_normal_error_m_s < 1e-12);
    /* Pair solver equivalence with four frictional box/plane contacts. */
    ps_body pair, graph;
    CHECK(ps_body_box(1, ps_v3(1, 1, 1), &pair) == PS_OK);
    pair.position_m.y = .5;
    pair.velocity_m_s = ps_v3(2, -1, .5);
    pair.angular_velocity_rad_s = ps_v3(.3, .2, -.1);
    graph = pair;
    ps_contact_manifold manifold;
    CHECK(ps_contacts_box_plane(&pair, ps_v3(1, 1, 1), ps_v3(0, 0, 0), ps_v3(0, 1, 0), &manifold) ==
              PS_OK &&
          manifold.count == 4);
    ps_contact_constraint rows[PS_CONTACT_MAX_POINTS];
    for (unsigned i = 0; i < manifold.count; i++)
        rows[i] = (ps_contact_constraint){0, PS_CONTACT_WORLD, manifold.points[i]};
    settings.friction = .4;
    settings.restitution = .3;
    ps_contact_solution pair_solution;
    CHECK(ps_contacts_resolve(&pair, NULL, &manifold, &settings, &pair_solution) == PS_OK);
    CHECK(ps_contacts_resolve_graph(&graph, 1, rows, manifold.count, &settings, &solution) ==
          PS_OK);
    CHECK(!memcmp(&pair, &graph, sizeof pair));
    CHECK(!memcmp(pair_solution.impulse_on_a_ns, solution.impulse_on_a_ns,
                  manifold.count * sizeof(ps_vec3)));
    CHECK(pair_solution.max_normal_error_m_s == solution.max_normal_error_m_s);
    /* Invalid or numeric failure never publishes partially solved bodies or reports. */
    memcpy(initial, bodies, sizeof bodies);
    ps_contact_graph_solution saved = solution;
    contacts[2].b = 99;
    CHECK(ps_contacts_resolve_graph(bodies, 3, contacts, 3, &settings, &solution) == PS_INVALID);
    CHECK(!memcmp(initial, bodies, sizeof bodies) && !memcmp(&saved, &solution, sizeof solution));
    contacts[2].b = 1;
    contacts[2].contact.point_m.x = DBL_MAX;
    CHECK(ps_contacts_resolve_graph(bodies, 3, contacts, 3, &settings, &solution) == PS_NUMERIC);
    CHECK(!memcmp(initial, bodies, sizeof bodies) && !memcmp(&saved, &solution, sizeof solution));
    CHECK(ps_contacts_resolve_graph(bodies, 129, NULL, 0, &settings, &solution) == PS_LIMIT);
    CHECK(ps_contacts_resolve_graph(bodies, 3, contacts, 513, &settings, &solution) == PS_LIMIT);
    CHECK(!memcmp(&saved, &solution, sizeof solution));
    CHECK(ps_contacts_resolve_graph(NULL, 0, NULL, 0, &settings, &solution) == PS_OK &&
          solution.count == 0);
    /* Static overlap cannot be projected; keep a nonzero projection residual. */
    CHECK(ps_body_sphere(0, .5, &graph) == PS_OK);
    rows[0] = (ps_contact_constraint){0, PS_CONTACT_WORLD, {{0, 0, 0}, {0, -1, 0}, .1}};
    CHECK(ps_contacts_resolve_graph(&graph, 1, rows, 1, &settings, &solution) == PS_OK &&
          fabs(solution.max_projection_error_m - .1) < 1e-15);
    ps_body many_bodies[PS_CONTACT_GRAPH_MAX_BODIES];
    ps_contact_constraint many_rows[PS_CONTACT_GRAPH_MAX_CONTACTS];
    settings.restitution = 0;
    settings.friction = 0;
    for (unsigned i = 0; i < PS_CONTACT_GRAPH_MAX_BODIES; i++) {
        CHECK(ps_body_sphere(1, .5, &many_bodies[i]) == PS_OK);
        many_bodies[i].position_m = ps_v3(i * 2, .5, 0);
        many_bodies[i].velocity_m_s.y = -1;
        for (unsigned j = 0; j < 4; j++)
            many_rows[4 * i + j] =
                (ps_contact_constraint){i, PS_CONTACT_WORLD, {{i * 2, 0, 0}, {0, -1, 0}, 0}};
    }
    CHECK(ps_contacts_resolve_graph(many_bodies, PS_CONTACT_GRAPH_MAX_BODIES, many_rows,
                                    PS_CONTACT_GRAPH_MAX_CONTACTS, &settings, &solution) == PS_OK);
    CHECK(solution.count == PS_CONTACT_GRAPH_MAX_CONTACTS && solution.max_normal_error_m_s < 1e-12);
    for (unsigned i = 0; i < PS_CONTACT_GRAPH_MAX_BODIES; i++) {
        CHECK(fabs(many_bodies[i].velocity_m_s.y) < 1e-12);
        double total = 0;
        for (unsigned j = 0; j < 4; j++)
            total += solution.impulse_on_a_ns[4 * i + j].y;
        CHECK(fabs(total - 1) < 1e-12);
    }
    puts("Contact graph: chains, supported stack, friction equivalence, residuals and atomicity "
         "passed");
    return 0;
}
