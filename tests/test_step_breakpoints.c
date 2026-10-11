#include "step_breakpoints.h"
#include <stdio.h>
#define CHECK(x) do { if(!(x)) {fprintf(stderr,"Breakpoints line %d: %s\n",__LINE__,#x);return 1;} } while(0)
int main(void) {
    ps_step_breakpoints points={0};
    CHECK(ps_step_breakpoints_parse("7, 0,3",&points));
    CHECK(points.count==3 && points.step[0]==0 && points.step[1]==3 && points.step[2]==7);
    CHECK(ps_step_breakpoint_hit(&points,0) && !ps_step_breakpoint_hit(&points,0));
    CHECK(!ps_step_breakpoint_hit(&points,1) && ps_step_breakpoint_hit(&points,3));
    CHECK(!ps_step_breakpoint_hit(&points,3) && ps_step_breakpoint_hit(&points,7));
    CHECK(!ps_step_breakpoint_hit(&points,7));
    CHECK(ps_step_breakpoints_parse("1000000000",&points) && points.step[0]==PS_STEP_BREAKPOINT_LIMIT);
    CHECK(ps_step_breakpoints_parse(" \t",&points) && !points.count);
    char text[256]={0};size_t used=0;
    for(unsigned i=0;i<32;i++)used+=(size_t)snprintf(text+used,sizeof text-used,"%s%u",i?",":"",i);
    CHECK(ps_step_breakpoints_parse(text,&points) && points.count==32);
    ps_step_breakpoints preserved=points;
    const char *invalid[]={"1,","3,3","-1","+1","1.1","1e2","1000000001","18446744073709551615","1,,2",",1"};
    for(unsigned i=0;i<sizeof invalid/sizeof invalid[0];i++) {
        CHECK(!ps_step_breakpoints_parse(invalid[i],&points));CHECK(!memcmp(&points,&preserved,sizeof points));
    }
    CHECK(!ps_step_breakpoint_add(&points,32) && !memcmp(&points,&preserved,sizeof points));
    CHECK(!ps_step_breakpoints_parse(NULL,&points) && !ps_step_breakpoints_parse("",NULL));
    puts("Step breakpoint schedule: sorted stops, one-shot boundaries, limits and transactional parse passed");return 0;
}
