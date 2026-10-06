#include "physim/language_sdk.h"
#include "test_allocator.h"
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"ContactWorld memory line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static test_allocator tracker;
static psrt_contact_world original;
static ps_body bodies[2];
static ps_collider colliders[2];
static psrt_site site={"contact_memory.phys",1,1};
static bool operation(unsigned op) {
    psrt_try_frame frame={0};frame.previous=psrt_try_current;frame.mark=psrt_cleanups;frame.depth=psrt_depth;psrt_try_current=&frame;
    if(setjmp(frame.jump))return false;
    ps_allocator allocator=test_domain(&tracker);
    if(op==0){psrt_contact_world w=psrt_world_defaults(allocator,site);psrt_world_drop(&w);}
    if(op==1){psrt_contact_world w=psrt_world_solve(allocator,original,bodies,2,colliders,2,PS_CONTACT_SOLVER_DEFAULT,.01,site);psrt_world_drop(&w);}
    if(op==2){psrt_contact_world w=psrt_world_reset(allocator,original,site);psrt_world_drop(&w);}
    if(op==3){psrt_array a=psrt_world_bodies(allocator,original,site);psrt_array_destroy(&a);}
    if(op==4){psrt_graph_contact c={1,0,psrt_world_contact(original,0,site)->constraint.contact};c.contact.normal=ps_vscale(c.contact.normal,-1);
        ps_vec3 seed=ps_v3(0,0,0);psrt_constraint_result r=psrt_constraints_solve_warm(allocator,PS_CONTACT_SOLVER_DEFAULT,bodies,2,&c,1,&seed,1,site);psrt_constraints_drop(&r);}
    psrt_try_current=frame.previous;return true;
}
int main(void) {
    CHECK(ps_body_sphere(0,.5,&bodies[0])==PS_OK && ps_body_sphere(1,.5,&bodies[1])==PS_OK);
    bodies[1].position_m.y=.5;bodies[1].velocity_m_s.y=-.1;
    colliders[0]=psrt_collider_plane(1,0,ps_v3(0,1,0),site);colliders[1]=psrt_collider_sphere(2,1,.5,site);
    original=psrt_world_defaults(test_domain(&tracker),site);
    psrt_contact_world ready=psrt_world_solve(test_domain(&tracker),original,bodies,2,colliders,2,PS_CONTACT_SOLVER_DEFAULT,.01,site);
    psrt_world_drop(&original);original=ready;
    psrt_contact_world copy;CHECK(psrt_world_copy(&copy,&original)==PS_OK);size_t live=tracker.live_bytes;
    psrt_world_storage before=*psrt_world_data(original,site);ps_body before_bodies[2];memcpy(before_bodies,bodies,sizeof bodies);
    for(unsigned op=0;op<5;op++) {
        CHECK(operation(op) && tracker.live_bytes==live);
        tracker.fail_on=tracker.attempts+1;CHECK(!operation(op));tracker.fail_on=0;
        CHECK(tracker.live_bytes==live && !tracker.invalid);
        CHECK(!memcmp(&before,psrt_world_data(original,site),sizeof before) && !memcmp(before_bodies,bodies,sizeof bodies));
        CHECK(psrt_world_contact_count(copy,site)==1);
    }
    colliders[1].id=1;CHECK(!operation(1) && tracker.live_bytes==live);
    CHECK(!memcmp(&before,psrt_world_data(copy,site),sizeof before));
    psrt_world_drop(&original);CHECK(psrt_world_contact_count(copy,site)==1);psrt_world_drop(&copy);
    CHECK(!tracker.live_bytes && !tracker.live_blocks && !tracker.invalid);
    puts("ContactWorld memory: independent snapshots, every allocation failure, invalid solve and exact cleanup passed");return 0;
}
