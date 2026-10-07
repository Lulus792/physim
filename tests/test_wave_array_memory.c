#include "physim/language_waves_optics.h"
#include "test_allocator.h"
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Wave array memory %d: %s\n",__LINE__,#x);return 1;}}while(0)
static test_allocator tracker;
static psrt_array result;
static double previous[]={0,1,0},current[]={0,1,0};
static bool attempt(double dt,size_t previous_count,size_t count) {
    psrt_try_frame frame={0};frame.previous=psrt_try_current;frame.mark=psrt_cleanups;frame.depth=psrt_depth;psrt_try_current=&frame;
    if(setjmp(frame.jump))return false;
    result=psrt_string_wave_step(test_domain(&tracker),previous,previous_count,current,count,1,1,dt,(psrt_site){"wave-memory.phys",1,1});
    psrt_try_current=frame.previous;return true;
}
int main(void) {
    CHECK(attempt(.5,3,3));size_t attempts=tracker.attempts;
    CHECK(psrt_array_count(&result)==3 && ((const double *)psrt_array_data(&result))[1]==.5);
    psrt_array copy;CHECK(psrt_array_clone(&result,&copy)==PS_OK);
    psrt_array_destroy(&result);CHECK(psrt_array_count(&copy)==3 && tracker.live_blocks==1);
    psrt_array_destroy(&copy);CHECK(!tracker.invalid && !tracker.live_bytes && !tracker.live_blocks);
    for(size_t fail=1;fail<=attempts;fail++) {
        memset(&tracker,0,sizeof tracker);tracker.fail_on=fail;CHECK(!attempt(.5,3,3));
        CHECK(!tracker.invalid && !tracker.live_bytes && !tracker.live_blocks);
    }
    memset(&tracker,0,sizeof tracker);CHECK(!attempt(2,3,3));
    CHECK(!tracker.live_bytes && !tracker.live_blocks && !tracker.invalid && current[1]==1 && previous[1]==1);
    size_t before=tracker.attempts;CHECK(!attempt(.5,2,3) && tracker.attempts==before);
    CHECK(!attempt(.5,4097,4097) && tracker.attempts==before);
    CHECK(attempt(.5,3,3));psrt_array_destroy(&result);
    CHECK(!tracker.invalid && !tracker.live_bytes && !tracker.live_blocks);
    puts("Wave array memory: independent ownership, all allocation failures, CFL rollback and preallocation limits passed");return 0;
}
