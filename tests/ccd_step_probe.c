#include "physim/contact_world.h"
#include <stdio.h>
int main(void){
    unsigned n;double dt;
    while(scanf("%u %la",&n,&dt)==2){
        if(n>16)return 2;
        ps_body bodies[16];ps_ccd_collider models[16];
        for(unsigned i=0;i<n;i++){unsigned id;double mass,x,v;
            if(scanf("%u %la %la %la",&id,&mass,&x,&v)!=4 || ps_body_sphere(mass,.5,&bodies[i])!=PS_OK)return 2;
            bodies[i].position_m.x=x;bodies[i].velocity_m_s.x=v;
            models[i]=(ps_ccd_collider){{id,i,PS_COLLIDER_SPHERE,{.5,0,0},{0,0,0}},NULL};
        }
        ps_contact_solver solver=PS_CONTACT_SOLVER_DEFAULT;solver.iterations=200;solver.restitution=1;solver.friction=0;solver.bounce_threshold_m_s=0;
        ps_ccd_step_options options=PS_CCD_STEP_DEFAULT;options.ccd.distance_tolerance_m=1e-9;options.contact_offset_m=1e-7;options.max_events=1024;
        ps_ccd_step_result report={0};ps_result result=ps_ccd_step(bodies,n,models,n,NULL,NULL,dt,&solver,&options,&report);
        printf("%d %u",result,report.events);
        for(unsigned i=0;i<n;i++)printf(" %a %a",bodies[i].position_m.x,bodies[i].velocity_m_s.x);
        putchar('\n');
    }
    return ferror(stdin)?2:0;
}
