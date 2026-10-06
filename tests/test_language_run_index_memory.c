#include "physim/language_sdk.h"
#include "test_allocator.h"
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"RunIndex language memory line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static psrt_site site={"memory.phys",1,1};
static test_allocator tracker;
static psrt_run_index run;
static bool open_attempt(const char *path) {
    psrt_try_frame frame={0};frame.previous=psrt_try_current;frame.mark=psrt_cleanups;frame.depth=psrt_depth;psrt_try_current=&frame;
    if(setjmp(frame.jump))return false;
    run=psrt_run_index_open(test_domain(&tracker),path,1000,site);psrt_try_current=frame.previous;return true;
}
static bool read_attempt(void) {
    psrt_try_frame frame={0};frame.previous=psrt_try_current;frame.mark=psrt_cleanups;frame.depth=psrt_depth;psrt_try_current=&frame;
    if(setjmp(frame.jump))return false;
    psrt_run_block block=psrt_run_index_read(test_domain(&tracker),run,255,2,site);
    psrt_run_block_drop(&block);psrt_try_current=frame.previous;return true;
}
int main(int argc,char **argv) {
    CHECK(argc==2 && open_attempt(argv[1]));size_t attempts=tracker.attempts;
    psrt_run_index copy;CHECK(psrt_run_index_copy(&copy,&run)==PS_OK);
    psrt_run_index_close(&run,site);CHECK(!run.owner && copy.owner);run=copy;
    CHECK(read_attempt());psrt_run_index_drop(&run);CHECK(!tracker.invalid && !tracker.live_bytes && !tracker.live_blocks);
    for(size_t fail=1;fail<=attempts;fail++) {
        memset(&tracker,0,sizeof tracker);tracker.fail_on=fail;CHECK(!open_attempt(argv[1]));
        CHECK(!tracker.invalid && !tracker.live_bytes && !tracker.live_blocks);
    }
    memset(&tracker,0,sizeof tracker);CHECK(open_attempt(argv[1]));size_t live=tracker.live_bytes;
    tracker.fail_on=tracker.attempts+1;CHECK(!read_attempt() && tracker.live_bytes==live && !tracker.invalid);
    tracker.fail_on=0;CHECK(read_attempt());psrt_run_index_drop(&run);CHECK(!tracker.live_blocks && !tracker.live_bytes);
    puts("RunIndex language memory: copied file owner, close aliases, every open allocation failure and failed block allocation passed");return 0;
}
