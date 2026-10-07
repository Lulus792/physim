#include "physim/language_analysis_sdk.h"
#include "test_allocator.h"
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Batch memory line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static test_allocator tracker;
static psrt_batch original;
static unsigned calls;
static psrt_site site={"batch_memory.phys",1,1};
static ps_result execute(void *user,const ps_batch_options *request,uint32_t stop_after,ps_batch_result *result){
    (void)user;(void)stop_after;calls++;memset(result,0,sizeof *result);
    result->completed=1;result->started=2;result->valid=1;result->finished[0]=true;
    result->endpoint_status[0]=1;result->values[0]=17;
    strcpy(result->channel.unit,"m");result->channel.dimension[0]=1;
    strcpy(result->error,"controlled partial failure");
    return request->runs==2?PS_IO:PS_INVALID;
}
static bool operation(unsigned op){
    psrt_try_frame frame={0};frame.previous=psrt_try_current;frame.mark=psrt_cleanups;frame.depth=psrt_depth;psrt_try_current=&frame;
    if(setjmp(frame.jump))return false;
    ps_allocator a=test_domain(&tracker);
    if(op==0){psrt_batch b=psrt_batch_parameter(a,original,"velocity",1.5,site);psrt_batch_drop(&b);}
    if(op==1){psrt_batch b=psrt_batch_target(a,original,.7,site);psrt_batch_drop(&b);}
    if(op==2){psrt_array values=psrt_batch_statuses(a,original,site);psrt_array_destroy(&values);}
    if(op==3){ps_analysis_services services={sizeof services,PS_ANALYSIS_SERVICES_VERSION,NULL,execute,NULL};
        psra_host host={.services=&services};psrt_batch b=psra_batch_run(&host,a,original,site);psrt_batch_drop(&b);}
    if(op==4){psra_host host={0};(void)psra_batch_run(&host,a,original,site);}
    if(op==5){(void)psrt_batch_parameter(a,original,"velocity",NAN,site);}
    if(op==6){_Alignas(ps_analysis_services) uint32_t empty=0;psra_host host={.services=(const ps_analysis_services *)&empty};(void)psra_batch_run(&host,a,original,site);}
    if(op==7){ps_analysis_services services={sizeof services,PS_ANALYSIS_SERVICES_VERSION+1,NULL,execute,NULL};psra_host host={.services=&services};(void)psra_batch_run(&host,a,original,site);}
    psrt_try_current=frame.previous;return true;
}
int main(void){
    original=psrt_batch_make(test_domain(&tracker),"/module.so","/new-series","position",2,10,.01,42,2,site);
    psrt_batch copy;CHECK(psrt_batch_copy(&copy,&original)==PS_OK);
    size_t live=tracker.live_bytes;unsigned char before[sizeof(psrt_batch_storage)];memcpy(before,psrt_batch_data(original,site),sizeof before);
    for(unsigned op=0;op<4;op++){
        CHECK(operation(op) && tracker.live_bytes==live);unsigned called=calls;
        tracker.fail_on=tracker.attempts+1;CHECK(!operation(op));tracker.fail_on=0;
        CHECK(calls==called && tracker.live_bytes==live && !tracker.invalid);
        CHECK(!memcmp(before,psrt_batch_data(original,site),sizeof before));
    }
    CHECK(!operation(4) && !operation(5) && !operation(6) && !operation(7) && tracker.live_bytes==live);
    ps_analysis_services services={sizeof services,PS_ANALYSIS_SERVICES_VERSION,NULL,execute,NULL};
    psra_host host={.services=&services};psrt_batch finished=psra_batch_run(&host,test_domain(&tracker),original,site);
    CHECK(psrt_batch_executed(finished,site) && !psrt_batch_executed(copy,site));
    CHECK(psrt_batch_code(finished,site)==PS_IO && psrt_batch_value(finished,0,site)==17);
    CHECK(psrt_batch_status(finished,1,site)==0 && psrt_batch_completed(finished,site)==1);
    psrt_array values=psrt_batch_values(test_domain(&tracker),finished,site);
    CHECK(psrt_array_count(&values)==1 && *(const double*)psrt_array_data(&values)==17);psrt_array_destroy(&values);
    psrt_memory memory={0};ps_unit unit=psrt_batch_unit(&memory,finished,site);
    psrt_batch_drop(&finished);CHECK(unit.dimension[0]==1 && unit.scale==1 && !strcmp(unit.symbol,"m"));
    psrt_string_pins_destroy(&memory);CHECK(!memory.live_bytes);
    psrt_batch_drop(&original);CHECK(psrt_batch_runs(copy,site)==2);psrt_batch_drop(&copy);
    CHECK(!tracker.live_bytes && !tracker.live_blocks && !tracker.invalid);
    puts("Batch memory: immutable snapshots, optional host rejection, partial endpoints, retained units and no side effects on allocation failure passed");return 0;
}
