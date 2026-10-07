#include "physim/measurement.h"
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if(!(x)) {fprintf(stderr,"RNG reference %d: %s\n",__LINE__,#x);return 1;} } while(0)
int main(void) {
    const uint64_t seeds[]={0,42,UINT64_C(9223372036854775808),UINT64_MAX};
    for(unsigned s=0;s<4;s++) {
        ps_rng r;ps_rng_seed(&r,seeds[s]);
        for(unsigned i=0;i<32;i++) {
            uint32_t v=ps_rng_u32(&r);
            printf("raw %" PRIu64 " %u %" PRIu32 " %" PRIu64 " %" PRIu64 "\n",seeds[s],i,v,r.state,r.increment);
        }
        ps_rng_seed(&r,seeds[s]);
        for(unsigned i=0;i<32;i++) {
            double v=ps_rng_normal(&r,2,3);
            printf("normal %" PRIu64 " %u %.17g %" PRIu64 " %" PRIu64 "\n",seeds[s],i,v,r.state,r.increment);
        }
        ps_rng_seed(&r,seeds[s]);
        for(unsigned i=0;i<32;i++) {
            ps_distribution d=i%5==0?(ps_distribution){PS_DIST_UNIFORM,0,1}:
                i%5==1?(ps_distribution){PS_DIST_NORMAL,2,3}:
                i%5==2?(ps_distribution){PS_DIST_CONSTANT,7,0}:
                i%5==3?(ps_distribution){PS_DIST_UNIFORM,5,5}:
                        (ps_distribution){PS_DIST_NORMAL,2,0};
            double v;CHECK(ps_distribution_sample(d,&r,&v)==PS_OK);
            printf("mixed %" PRIu64 " %u %.17g %" PRIu64 " %" PRIu64 "\n",seeds[s],i,v,r.state,r.increment);
        }
        ps_rng saved=r,copy=r,other;ps_rng_seed(&other,123);
        for(unsigned i=0;i<100;i++) {
            uint32_t expected=ps_rng_u32(&r);
            (void)ps_rng_normal(&other,0,1);
            CHECK(ps_rng_u32(&copy)==expected);
        }
        r=saved;copy=saved;
        for(unsigned i=0;i<100;i++) {
            double a=ps_rng_normal(&r,2,3),b=ps_rng_normal(&copy,2,3);
            CHECK(a==b && r.state==copy.state && r.increment==copy.increment);
        }
        saved=r;double untouched=19;
        CHECK(ps_distribution_sample((ps_distribution){PS_DIST_NORMAL,0,-1},&r,&untouched)==PS_INVALID);
        CHECK(r.state==saved.state && r.increment==saved.increment && untouched==19);
    }
    return 0;
}
