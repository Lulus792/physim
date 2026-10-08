#include "physim/contact_world.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"CCD step %d: %s\n",__LINE__,#x);return 1;}}while(0)
static bool near(double a,double b){return fabs(a-b)<1e-4;}
int main(void){
    ps_body bodies[3];for(unsigned i=0;i<3;i++)CHECK(ps_body_sphere(1,.5,&bodies[i])==PS_OK);
    bodies[0].position_m.x=-4;bodies[1].position_m.x=0;bodies[2].position_m.x=4;bodies[0].velocity_m_s.x=10;
    ps_ccd_collider models[3]={{{3,0,PS_COLLIDER_SPHERE,{.5,0,0},{0,0,0}},NULL},{{2,1,PS_COLLIDER_SPHERE,{.5,0,0},{0,0,0}},NULL},{{1,2,PS_COLLIDER_SPHERE,{.5,0,0},{0,0,0}},NULL}};
    ps_contact_solver solver=PS_CONTACT_SOLVER_DEFAULT;solver.restitution=1;solver.friction=0;solver.bounce_threshold_m_s=0;solver.iterations=200;
    ps_ccd_step_result report;
    CHECK(ps_ccd_step(bodies,3,models,3,NULL,NULL,1,&solver,NULL,&report)==PS_OK);
    CHECK(report.events==2 && near(bodies[0].velocity_m_s.x,0) && near(bodies[1].velocity_m_s.x,0) && near(bodies[2].velocity_m_s.x,10));
    CHECK(near(bodies[0].position_m.x,-1) && near(bodies[1].position_m.x,3) && near(bodies[2].position_m.x,8));
    /* Stable IDs preserve physical results across body/model array reorder. */
    ps_body ordered[3];memcpy(ordered,bodies,sizeof ordered);
    ps_ccd_collider reordered_models[3]={models[2],models[0],models[1]};
    for(unsigned i=0;i<3;i++)CHECK(ps_body_sphere(1,.5,&bodies[i])==PS_OK);
    bodies[2].position_m.x=-4;bodies[0].position_m.x=0;bodies[1].position_m.x=4;bodies[2].velocity_m_s.x=10;
    reordered_models[0].collider.body=1;reordered_models[1].collider.body=2;reordered_models[2].collider.body=0;
    CHECK(ps_ccd_step(bodies,3,reordered_models,3,NULL,NULL,1,&solver,NULL,&report)==PS_OK && report.events==2);
    CHECK(near(bodies[2].position_m.x,ordered[0].position_m.x) && near(bodies[0].position_m.x,ordered[1].position_m.x) && near(bodies[1].position_m.x,ordered[2].position_m.x));
    CHECK(near(bodies[2].velocity_m_s.x,ordered[0].velocity_m_s.x) && near(bodies[0].velocity_m_s.x,ordered[1].velocity_m_s.x) && near(bodies[1].velocity_m_s.x,ordered[2].velocity_m_s.x));
    ps_body saved[3];memcpy(saved,bodies,sizeof bodies);ps_ccd_step_result old=report;
    ps_ccd_step_options budget=PS_CCD_STEP_DEFAULT;budget.max_events=1;
    for(unsigned i=0;i<3;i++){bodies[i].position_m.x=-4+4*(double)i;bodies[i].velocity_m_s.x=i?0:10;}
    memcpy(saved,bodies,sizeof bodies);
    CHECK(ps_ccd_step(bodies,3,models,3,NULL,NULL,1,&solver,&budget,&report)==PS_LIMIT && !memcmp(saved,bodies,sizeof bodies) && !memcmp(&old,&report,sizeof report));
    /* Simultaneous inelastic chain, not sequential order-dependent pair response. */
    bodies[0].position_m.x=-2;bodies[1].position_m.x=0;bodies[2].position_m.x=2;
    bodies[0].velocity_m_s.x=1;bodies[1].velocity_m_s.x=0;bodies[2].velocity_m_s.x=-1;solver.restitution=0;
    CHECK(ps_ccd_step(bodies,3,models,3,NULL,NULL,2,&solver,NULL,&report)==PS_OK);
    CHECK(report.events==1 && report.contacts==2 && near(bodies[0].velocity_m_s.x,0) && near(bodies[1].velocity_m_s.x,0) && near(bodies[2].velocity_m_s.x,0));
    /* Gravity kick and resting floor support over repeated frames. */
    CHECK(ps_body_sphere(0,1,&bodies[1])==PS_OK);CHECK(ps_body_sphere(1,.5,&bodies[0])==PS_OK);bodies[0].position_m.y=.5;
    models[0]=(ps_ccd_collider){{1,0,PS_COLLIDER_SPHERE,{.5,0,0},{0,0,0}},NULL};models[1]=(ps_ccd_collider){{2,1,PS_COLLIDER_PLANE,{0,0,0},{0,1,0}},NULL};
    ps_vec3 force[2]={{0,-9.81,0},{0,0,0}};
    for(unsigned i=0;i<100;i++)CHECK(ps_ccd_step(bodies,2,models,2,force,NULL,.01,&solver,NULL,&report)==PS_OK);
    CHECK(fabs(bodies[0].position_m.y-.5)<1e-4 && fabs(bodies[0].velocity_m_s.y)<1e-4);
    CHECK(ps_body_box(1,ps_v3(1,1,1),&bodies[0])==PS_OK);bodies[0].position_m.y=.5;
    models[0]=(ps_ccd_collider){{1,0,PS_COLLIDER_BOX,{1,1,1},{0,0,0}},NULL};
    for(unsigned i=0;i<30;i++)CHECK(ps_ccd_step(bodies,2,models,2,force,NULL,.01,&solver,NULL,&report)==PS_OK);
    CHECK(fabs(bodies[0].position_m.y-.5)<1e-4 && fabs(bodies[0].velocity_m_s.y)<1e-4 && report.contacts>=4);
    /* No-contact step reproduces the existing kick/gyroscopic drift method. */
    CHECK(ps_body_box(2,ps_v3(1,2,3),&bodies[0])==PS_OK);bodies[0].velocity_m_s=ps_v3(1,2,3);bodies[0].angular_velocity_rad_s=ps_v3(.2,.3,.4);
    ps_body reference=bodies[0];ps_vec3 f=ps_v3(1,-2,3),torque=ps_v3(.1,.2,.3);
    CHECK(ps_body_step(&reference,f,torque,.02)==PS_OK);
    CHECK(ps_ccd_step(bodies,1,NULL,0,&f,&torque,.02,&solver,NULL,&report)==PS_OK && report.events==0);
    CHECK(near(bodies[0].position_m.x,reference.position_m.x) && near(bodies[0].velocity_m_s.y,reference.velocity_m_s.y) && near(bodies[0].orientation.w,reference.orientation.w));
    /* A rotor reaches a plane between clear endpoints; the controller resolves
     * and searches the changed motion again within the same frame. */
    CHECK(ps_body_box(1,ps_v3(4,.2,.2),&bodies[0])==PS_OK);bodies[0].position_m.y=1.5;bodies[0].angular_velocity_rad_s.z=3.141592653589793;
    CHECK(ps_body_with_inertia(0,ps_v3(0,0,0),&bodies[1])==PS_OK);
    models[0]=(ps_ccd_collider){{1,0,PS_COLLIDER_BOX,{4,.2,.2},{0,0,0}},NULL};
    solver.restitution=.5;solver.friction=.2;
    CHECK(ps_ccd_step(bodies,2,models,2,NULL,NULL,.5,&solver,NULL,&report)==PS_OK && report.events>0);
    ps_body before_invalid[2];memcpy(before_invalid,bodies,sizeof before_invalid);old=report;
    models[1].collider.id=1;
    CHECK(ps_ccd_step(bodies,2,models,2,NULL,NULL,.1,&solver,NULL,&report)==PS_INVALID && !memcmp(before_invalid,bodies,sizeof before_invalid) && !memcmp(&old,&report,sizeof report));
    models[1].collider.id=2;
    ps_vec3 bad_force[2]={{NAN,0,0},{0,0,0}};
    CHECK(ps_ccd_step(bodies,2,models,2,bad_force,NULL,.1,&solver,NULL,&report)==PS_INVALID && !memcmp(before_invalid,bodies,sizeof before_invalid) && !memcmp(&old,&report,sizeof report));
    CHECK(ps_ccd_step(bodies,129,NULL,0,NULL,NULL,.1,&solver,NULL,&report)==PS_LIMIT && !memcmp(before_invalid,bodies,sizeof before_invalid) && !memcmp(&old,&report,sizeof report));
    puts("Continuous event step: sequential and simultaneous collisions, atomic event budget and gravity-supported contact passed");return 0;
}
