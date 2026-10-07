#include "physim/language_fluid.h"
#include "test_allocator.h"
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Fluid array memory %d: %s\n",__LINE__,#x);return 1;}}while(0)
static test_allocator tracker;static psrt_array result;
static int64_t a[]={0,1,1},b[]={1,2,3},fixed[]={1,0,1,1};
static double conductance[]={1,1,1},boundary[]={100,0,0,0},profile[]={0,1,0};
static bool attempt(bool network,double dt) {
    psrt_try_frame frame={0};frame.previous=psrt_try_current;frame.mark=psrt_cleanups;frame.depth=psrt_depth;psrt_try_current=&frame;
    if(setjmp(frame.jump))return false;
    psrt_site site={"fluid-memory.phys",1,1};
    result=network?psrt_pipe_network(test_domain(&tracker),a,3,b,3,conductance,3,fixed,4,boundary,4,site)
                  :psrt_transport_step(test_domain(&tracker),profile,3,1,0,1,dt,site);
    psrt_try_current=frame.previous;return true;
}
int main(void) {
    for(unsigned network=0;network<2;network++) {
        memset(&tracker,0,sizeof tracker);CHECK(attempt(network,.5));size_t allocations=tracker.attempts;
        CHECK(psrt_array_count(&result)==(network?7:3));
        psrt_array copy;CHECK(psrt_array_clone(&result,&copy)==PS_OK);psrt_array_destroy(&result);
        CHECK(tracker.live_blocks==1 && psrt_array_count(&copy)==(network?7:3));psrt_array_destroy(&copy);
        CHECK(!tracker.invalid && !tracker.live_bytes && !tracker.live_blocks);
        for(size_t fail=1;fail<=allocations;fail++) {
            memset(&tracker,0,sizeof tracker);tracker.fail_on=fail;CHECK(!attempt(network,.5));
            CHECK(!tracker.invalid && !tracker.live_bytes && !tracker.live_blocks);
        }
    }
    memset(&tracker,0,sizeof tracker);CHECK(!attempt(false,2));CHECK(!tracker.live_bytes && !tracker.live_blocks && !tracker.invalid && profile[1]==1);
    fixed[0]=fixed[2]=fixed[3]=0;size_t allocations=tracker.attempts;
    CHECK(!attempt(true,.5) && tracker.attempts==allocations && !tracker.live_bytes);fixed[0]=fixed[2]=fixed[3]=1;
    CHECK(attempt(true,.5));psrt_array_destroy(&result);CHECK(!tracker.live_bytes && !tracker.invalid);
    puts("Fluid array memory: independent owners, every result allocation failure, stability rollback and unanchored preallocation rejection passed");return 0;
}
