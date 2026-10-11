#include "physim/measurement.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc,char **argv) {
    if(argc!=5)return 2;
    ps_distribution d={(ps_distribution_kind)atoi(argv[1]),strtod(argv[2],NULL),strtod(argv[3],NULL)};
    double mean=8,sd=9,value=7;ps_rng rng;ps_rng_seed(&rng,strtoull(argv[4],NULL,10));
    ps_result moment=ps_distribution_moments(d,&mean,&sd),sample=ps_distribution_sample(d,&rng,&value);
    printf("%d %.17g %.17g %d %.17g %" PRIu64 " %" PRIu64 "\n",moment,mean,sd,sample,value,rng.state,rng.increment);
    return 0;
}
