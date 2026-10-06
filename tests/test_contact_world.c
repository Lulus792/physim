#include "physim/contact_world.h"
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Contact world line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static ps_contact_world world,before,warm,cold;
static ps_contact_world_result result,saved;
static ps_body bodies[PS_CONTACT_GRAPH_MAX_BODIES],original[PS_CONTACT_GRAPH_MAX_BODIES];
static ps_collider colliders[PS_CONTACT_GRAPH_MAX_BODIES];
static void stack(unsigned count) {
    ps_body_sphere(0,.5,&bodies[0]);colliders[0]=(ps_collider){1,0,PS_COLLIDER_PLANE,{0,0,0},{0,1,0}};
    for(unsigned i=1;i<=count;i++){ps_body_sphere(1,.5,&bodies[i]);bodies[i].position_m.y=i-.5;colliders[i]=(ps_collider){i+1,i,PS_COLLIDER_SPHERE,{.5,0,0},{0,0,0}};bodies[i].velocity_m_s.y=-.1;}
}
int main(void) {
    CHECK(ps_contact_world_init(&world,sizeof world,NULL)==PS_OK);
    before=world;ps_contact_world_settings invalid=PS_CONTACT_WORLD_DEFAULT;invalid.match_distance_m=NAN;
    CHECK(ps_contact_world_init(&world,sizeof world,&invalid)==PS_INVALID && !memcmp(&world,&before,sizeof world));
    CHECK(ps_contact_world_init(&world,8,NULL)==PS_VERSION && !memcmp(&world,&before,sizeof world));
    ps_contact_solver solver=PS_CONTACT_SOLVER_DEFAULT;solver.friction=0;solver.correction_fraction=0;solver.iterations=256;
    stack(4);CHECK(ps_contact_world_solve(&world,bodies,5,colliders,5,&solver,.01,&result)==PS_OK);
    CHECK(result.count==4 && result.created==4 && !result.matched && !result.warmed && !result.ended);
    for(unsigned i=1;i<=4;i++)CHECK(fabs(bodies[i].velocity_m_s.y)<1e-12);
    CHECK(result.solution.max_normal_error_m_s<1e-12);
    warm=world;CHECK(ps_contact_world_init(&cold,sizeof cold,NULL)==PS_OK);solver.iterations=1;
    for(unsigned i=1;i<=4;i++)bodies[i].velocity_m_s.y=-.1;
    memcpy(original,bodies,sizeof bodies);
    CHECK(ps_contact_world_solve(&warm,bodies,5,colliders,5,&solver,.01,&result)==PS_OK && result.matched==4 && result.warmed==4);
    double warm_error=result.solution.max_normal_error_m_s;
    memcpy(bodies,original,sizeof bodies);CHECK(ps_contact_world_solve(&cold,bodies,5,colliders,5,&solver,.01,&result)==PS_OK);
    CHECK(warm_error<1e-12 && result.solution.max_normal_error_m_s>.04);
    /* dt scaling preserves the same support force. */
    world=warm;for(unsigned i=1;i<=4;i++)bodies[i].velocity_m_s.y=-.2;
    CHECK(ps_contact_world_solve(&world,bodies,5,colliders,5,&solver,.02,&result)==PS_OK && result.warmed==4 && result.solution.max_normal_error_m_s<1e-12);
    /* Reorder bodies and colliders while retaining physical IDs. */
    world=warm;for(unsigned i=1;i<=4;i++)original[4-i+1]=bodies[i];original[0]=bodies[0];
    for(unsigned i=1;i<=4;i++){colliders[i].body=5-i;original[5-i].velocity_m_s.y=-.1;}
    ps_collider swap=colliders[1];colliders[1]=colliders[4];colliders[4]=swap;
    CHECK(ps_contact_world_solve(&world,original,5,colliders,5,&solver,.01,&result)==PS_OK && result.matched==4 && result.warmed==4 && result.solution.max_normal_error_m_s<1e-12);
    /* Missing contacts expire, and reappearing contacts start cold. */
    for(unsigned i=1;i<=4;i++)original[i].position_m.x=i*10;
    original[0].position_m.y=-10;
    CHECK(ps_contact_world_solve(&world,original,5,colliders,5,&solver,.01,&result)==PS_OK && result.count==0 && result.ended==4);
    CHECK(ps_contact_world_reset(&world)==PS_OK && !world.count && !world.dt_s);
    stack(1);CHECK(ps_contact_world_solve(&world,bodies,2,colliders,2,&solver,.01,&result)==PS_OK);
    before=world;bodies[1].velocity_m_s.y=-.1;
    CHECK(ps_contact_world_solve(&world,bodies,2,colliders,2,&solver,.2,&result)==PS_OK && result.matched==1 && result.warmed==0);
    world=before;bodies[1].velocity_m_s.y=1;
    CHECK(ps_contact_world_solve(&world,bodies,2,colliders,2,&solver,.01,&result)==PS_OK && result.matched==1 && result.warmed==0 && bodies[1].velocity_m_s.y==1);
    world=before;CHECK(ps_body_sphere(2,.5,&bodies[1])==PS_OK);bodies[1].position_m.y=.5;bodies[1].velocity_m_s.y=-.1;
    CHECK(ps_contact_world_solve(&world,bodies,2,colliders,2,&solver,.01,&result)==PS_OK && result.created==1 && result.ended==1 && !result.matched);
    CHECK(ps_contact_world_reset(&world)==PS_OK);stack(1);
    bodies[0].orientation=(ps_quat){0,0,sin(.25*3.141592653589793),cos(.25*3.141592653589793)};
    bodies[1].position_m=ps_v3(-.49,0,0);bodies[1].velocity_m_s=ps_v3(.1,0,0);
    CHECK(ps_contact_world_solve(&world,bodies,2,colliders,2,&solver,.01,&result)==PS_OK && result.count==1 && fabs(bodies[1].velocity_m_s.x)<1e-12);
    CHECK(ps_contact_world_reset(&world)==PS_OK);
    CHECK(ps_body_sphere(1,.25,&bodies[0])==PS_OK && ps_body_box(0,ps_v3(1,1,1),&bodies[1])==PS_OK);
    bodies[0].position_m.x=.6;bodies[0].velocity_m_s.x=-1;
    colliders[0]=(ps_collider){10,0,PS_COLLIDER_SPHERE,{.25,0,0},{0,0,0}};
    colliders[1]=(ps_collider){90,1,PS_COLLIDER_BOX,{1,1,1},{0,0,0}};
    CHECK(ps_contact_world_solve(&world,bodies,2,colliders,2,&solver,.01,&result)==PS_OK && result.count==1 && bodies[0].velocity_m_s.x==0);
    CHECK(ps_contact_world_reset(&world)==PS_OK);
    CHECK(ps_body_box(1,ps_v3(1,1,1),&bodies[0])==PS_OK && ps_body_box(1,ps_v3(1,1,1),&bodies[1])==PS_OK);
    bodies[0].position_m.x=-.4;bodies[1].position_m.x=.4;bodies[0].velocity_m_s.x=1;bodies[1].velocity_m_s.x=-1;
    colliders[0]=(ps_collider){10,0,PS_COLLIDER_BOX,{1,1,1},{0,0,0}};solver.iterations=128;
    CHECK(ps_contact_world_solve(&world,bodies,2,colliders,2,&solver,.01,&result)==PS_OK && result.count==4 && result.solution.max_normal_error_m_s<1e-12);
    CHECK(fabs(bodies[0].velocity_m_s.x)<1e-12 && fabs(bodies[1].velocity_m_s.x)<1e-12);
    /* Canonical plane orientation, box manifold and one-to-one matching. */
    CHECK(ps_body_sphere(0,.5,&bodies[0])==PS_OK && ps_body_box(1,ps_v3(1,1,1),&bodies[1])==PS_OK);
    bodies[1].position_m.y=.5;bodies[1].velocity_m_s=ps_v3(2,-1,0);
    colliders[0]=(ps_collider){90,0,PS_COLLIDER_PLANE,{0,0,0},{0,1,0}};
    colliders[1]=(ps_collider){10,1,PS_COLLIDER_BOX,{1,1,1},{0,0,0}};
    solver=PS_CONTACT_SOLVER_DEFAULT;solver.correction_fraction=0;
    CHECK(ps_contact_world_solve(&world,bodies,2,colliders,2,&solver,.01,&result)==PS_OK && result.count==4);
    CHECK(world.contacts[0].id_a==10 && world.contacts[0].id_b==90 && world.contacts[0].normal.y==-1);
    bodies[1].velocity_m_s=ps_v3(2,-1,0);bodies[1].angular_velocity_rad_s=ps_v3(0,0,0);
    CHECK(ps_contact_world_solve(&world,bodies,2,colliders,2,&solver,.01,&result)==PS_OK && result.matched==4 && result.warmed==4);
    solver.friction=0;bodies[1].velocity_m_s=ps_v3(2,-1,0);bodies[1].angular_velocity_rad_s=ps_v3(0,0,0);
    CHECK(ps_contact_world_solve(&world,bodies,2,colliders,2,&solver,.01,&result)==PS_OK && fabs(bodies[1].velocity_m_s.x-2)<1e-12);
    for(unsigned i=0;i<4;i++)CHECK(fabs(result.solution.impulse_on_a_ns[i].x)<1e-12);
    before=world;saved=result;memcpy(original,bodies,sizeof bodies);colliders[1].id=90;
    CHECK(ps_contact_world_solve(&world,bodies,2,colliders,2,&solver,.01,&result)==PS_INVALID && !memcmp(&world,&before,sizeof world) && !memcmp(&result,&saved,sizeof result) && !memcmp(bodies,original,sizeof bodies));
    colliders[1].id=10;world.contacts[0].local_a_m.x=NAN;before=world;
    CHECK(ps_contact_world_solve(&world,bodies,2,colliders,2,&solver,.01,&result)==PS_INVALID && !memcmp(&world,&before,sizeof world));
    CHECK(ps_contact_world_init(&world,sizeof world,NULL)==PS_OK);
    /* Dense colliders exceed contact capacity atomically. */
    for(unsigned i=0;i<40;i++){CHECK(ps_body_sphere(1,.5,&bodies[i])==PS_OK);colliders[i]=(ps_collider){i+1,i,PS_COLLIDER_SPHERE,{.5,0,0},{0,0,0}};}
    before=world;saved=result;memcpy(original,bodies,sizeof bodies);
    CHECK(ps_contact_world_solve(&world,bodies,40,colliders,40,&solver,.01,&result)==PS_LIMIT && !memcmp(&world,&before,sizeof world) && !memcmp(&result,&saved,sizeof result) && !memcmp(bodies,original,sizeof bodies));
    /* Warm solver keeps cold restitution targets, clamps friction and rolls back invalid seeds. */
    CHECK(ps_body_sphere(1,.5,&bodies[0])==PS_OK);bodies[0].velocity_m_s.y=-2;
    ps_contact_constraint contact={0,PS_CONTACT_WORLD,{{0,-.5,0},{0,-1,0},0}};
    solver.restitution=1;solver.friction=0;solver.iterations=1;ps_vec3 seed=ps_v3(10,50,0);ps_contact_graph_solution solution;
    CHECK(ps_contacts_resolve_graph_warm(bodies,1,&contact,1,&solver,&seed,&solution)==PS_OK && fabs(bodies[0].velocity_m_s.y-2)<1e-12 && bodies[0].velocity_m_s.x==0 && fabs(solution.impulse_on_a_ns[0].y-4)<1e-12);
    original[0]=bodies[0];seed.x=NAN;ps_contact_graph_solution previous=solution;
    CHECK(ps_contacts_resolve_graph_warm(bodies,1,&contact,1,&solver,&seed,&solution)==PS_INVALID && !memcmp(bodies,original,sizeof(ps_body)) && !memcmp(&solution,&previous,sizeof solution));
    seed=ps_v3(0,DBL_MAX,0);contact.contact.point_m.x=2;
    CHECK(ps_contacts_resolve_graph_warm(bodies,1,&contact,1,&solver,&seed,&solution)==PS_NUMERIC && !memcmp(bodies,original,sizeof(ps_body)) && !memcmp(&solution,&previous,sizeof solution));
    printf("Contact world: stack warm residual %.17g, timestep scaling, canonical IDs, lifecycle, manifolds, friction, restitution and atomic limits passed\n",warm_error);return 0;
}
