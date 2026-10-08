#define PSRT_MODULE
#define PSRT_SOURCE "convex-runtime.phys"
#include "physim/language_sdk.h"
#include <float.h>
#include <stdio.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Convex runtime %d: %s\n",__LINE__,#x);return 1;}}while(0)
static psrt_trap trap;
static ps_diagnostic diagnostic;
static char error[2048];
static const ps_vec3 vertices[]={{1,1,1},{1,-1,-1},{-1,1,-1},{-1,-1,1}};
static const int64_t indices[]={0,1,2,0,3,1,0,2,3,1,3,2};
static int rejected(unsigned operation,ps_result expected) {
    memset(&trap,0,sizeof trap);memset(&diagnostic,0,sizeof diagnostic);
    trap.error=error;trap.capacity=sizeof error;trap.diagnostic=&diagnostic;
    trap.operation="convex.operation";psrt_current=&trap;
    if(!setjmp(trap.jump)) {
        psrt_site site={"convex-runtime.phys",3,4};
        ps_body a=psrt_body_with_inertia(1,ps_v3(.4,.4,.4),site),b=a;
        if(operation==0)(void)psrt_aabb_convex(a,vertices,4,indices,11,site);
        if(operation==1)(void)psrt_aabb_convex(a,vertices,65,indices,12,site);
        if(operation==2){a.position_m.x=-DBL_MAX;b.position_m.x=DBL_MAX;
            (void)psrt_contacts_convexes(a,vertices,4,indices,12,b,vertices,4,indices,12,site);}
        if(operation==3)(void)psrt_body_with_inertia(1,ps_v3(-1,1,1),site);
        if(operation==4){const int64_t bad[]={0,1,-1};(void)psrt_aabb_convex(a,vertices,4,bad,3,site);}
        if(operation==5)(void)psrt_sweep_convexes(a,vertices,4,indices,12,ps_v3(NAN,0,0),b,vertices,4,indices,12,ps_v3(0,0,0),site);
        if(operation==6){b.position_m.x=10;(void)psrt_sweep_convexes(a,vertices,4,indices,12,ps_v3(DBL_MAX,0,0),b,vertices,4,indices,12,ps_v3(-DBL_MAX,0,0),site);}
        if(operation==7)(void)psrt_aabb_swept_convex(a,vertices,65,indices,12,ps_v3(1,0,0),site);
        psrt_current=NULL;CHECK(false);
    }
    psrt_current=NULL;
    CHECK(trap.failure_code==expected && ps_diagnostic_valid(&diagnostic) && diagnostic.code==expected);
    CHECK(!strcmp(diagnostic.operation,"convex.operation") && strstr(error,"convex-runtime.phys:3:4:"));
    CHECK(trap.cleanup==NULL);
    return 0;
}
int main(void) {
    CHECK(rejected(0,PS_INVALID)==0);CHECK(rejected(1,PS_LIMIT)==0);
    CHECK(rejected(2,PS_NUMERIC)==0);CHECK(rejected(3,PS_INVALID)==0);CHECK(rejected(4,PS_INVALID)==0);
    CHECK(rejected(5,PS_INVALID)==0);CHECK(rejected(6,PS_NUMERIC)==0);CHECK(rejected(7,PS_LIMIT)==0);
    puts("Convex runtime: original status codes, structured diagnostics, source sites and clean traps passed");
    return 0;
}
