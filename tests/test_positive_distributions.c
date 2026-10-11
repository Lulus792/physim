#include "physim/measurement.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Positive distributions %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void) {
    ps_rng rng,replay;ps_rng_seed(&rng,42);replay=rng;double value=7,mean=8,sd=9;
    ps_distribution exponential={PS_DIST_EXPONENTIAL,2,0};
    CHECK(ps_distribution_moments(exponential,&mean,&sd)==PS_OK && mean==2 && sd==2);
    double expected=-2*log(ps_rng_uniform(&replay));
    CHECK(ps_distribution_sample(exponential,&rng,&value)==PS_OK && value==expected && !memcmp(&rng,&replay,sizeof rng));
    ps_distribution lognormal={PS_DIST_LOGNORMAL,.25,.5};
    expected=exp(ps_rng_normal(&replay,.25,.5));
    CHECK(ps_distribution_sample(lognormal,&rng,&value)==PS_OK && value==expected && !memcmp(&rng,&replay,sizeof rng));
    ps_distribution constant={PS_DIST_LOGNORMAL,0,0};
    CHECK(ps_distribution_sample(constant,&rng,&value)==PS_OK && value==1 && !memcmp(&rng,&replay,sizeof rng));
    CHECK(ps_distribution_moments(constant,&mean,&sd)==PS_OK && mean==1 && sd==0);
    ps_distribution invalid[]={{PS_DIST_EXPONENTIAL,0,0},{PS_DIST_EXPONENTIAL,-1,0},
        {PS_DIST_EXPONENTIAL,1,1},{PS_DIST_LOGNORMAL,0,-1},{PS_DIST_LOGNORMAL,NAN,1}};
    for(unsigned i=0;i<sizeof invalid/sizeof *invalid;i++) {
        value=7;mean=8;sd=9;
        CHECK(ps_distribution_sample(invalid[i],&rng,&value)==PS_INVALID && value==7 && !memcmp(&rng,&replay,sizeof rng));
        CHECK(ps_distribution_moments(invalid[i],&mean,&sd)==PS_INVALID && mean==8 && sd==9);
    }
    for(unsigned i=0;i<2;i++) {
        ps_distribution unrepresentable={PS_DIST_LOGNORMAL,i?1000:-1000,0};
        value=7;mean=8;sd=9;
        CHECK(ps_distribution_sample(unrepresentable,&rng,&value)==PS_NUMERIC && value==7 && !memcmp(&rng,&replay,sizeof rng));
        CHECK(ps_distribution_moments(unrepresentable,&mean,&sd)==PS_NUMERIC && mean==8 && sd==9);
    }
    CHECK(ps_distribution_moments((ps_distribution){PS_DIST_LOGNORMAL,0,1e-200},&mean,&sd)==PS_OK && mean==1 && sd==1e-200);
    CHECK(ps_distribution_moments((ps_distribution){PS_DIST_LOGNORMAL,-800,30},&mean,&sd)==PS_OK && mean>0 && isfinite(mean) && sd>0 && isfinite(sd));
    /* Indexed positive noise preserves a sensor's sparse/full equivalence. */
    ps_sensor_config config={0};config.unit=PS_METRE;config.rate_hz=100;config.noise=exponential;
    ps_sensor complete,sparse;CHECK(ps_sensor_init(&complete,&config,42)==PS_OK && ps_sensor_init(&sparse,&config,42)==PS_OK);
    for(unsigned i=0;i<100;i++) {
        ps_measurement a,b;CHECK(ps_sensor_read(&complete,i*.01,(ps_quantity){1,PS_METRE},&a)==PS_OK &&
                               a.state==PS_MEASUREMENT_VALID && a.value.value>1 && a.standard_uncertainty==2);
        if(!(i%7))CHECK(ps_sensor_read(&sparse,i*.01,(ps_quantity){1,PS_METRE},&b)==PS_OK &&
                       a.value.value==b.value.value && a.index==b.index);
    }
    config.noise=(ps_distribution){PS_DIST_LOGNORMAL,-1000,0};ps_sensor sensor;
    CHECK(ps_sensor_init(&sensor,&config,42)==PS_OK);ps_sensor saved=sensor;
    ps_measurement out,sentinel;memset(&sentinel,0x5a,sizeof sentinel);out=sentinel;
    CHECK(ps_sensor_read(&sensor,0,(ps_quantity){1,PS_METRE},&out)==PS_NUMERIC &&
          !memcmp(&sensor,&saved,sizeof sensor) && !memcmp(&out,&sentinel,sizeof out));
    puts("Positive distributions: draws, degenerate values, stable moments, invalid/range atomicity and indexed sensor noise passed");return 0;
}
