#include "physim/experiment.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Monte Carlo instances %d: %s\n",__LINE__,#x);return 1;}}while(0)
static int verify(const char *path) {
    void *module=ps_module_open(path);CHECK(module);void *symbol=ps_module_symbol(module,"ps_get_experiment");ps_experiment_entry entry=NULL;
    memcpy(&entry,&symbol,sizeof entry);CHECK(entry);const ps_experiment_api *api=entry();
    ps_context a={.struct_size=sizeof a,.api_version=PS_API_VERSION,.seed=42},b=a;
    CHECK(ps_parameter_override(&b,"meanVx",-4)==PS_OK && ps_parameter_override(&b,"sigmaVx",0)==PS_OK &&
        ps_parameter_override(&b,"meanVy",2)==PS_OK && ps_parameter_override(&b,"sigmaVy",0)==PS_OK);
    CHECK(api->create(&a)==PS_OK && api->create(&b)==PS_OK && ps_parameter_finalize(&a)==PS_OK && ps_parameter_finalize(&b)==PS_OK);
    double initial[PS_MAX_CHANNELS];memcpy(initial,a.values,sizeof initial);
    CHECK(a.channel_count==9 && b.values[2]==-4 && b.values[3]==2);
    CHECK(api->step(&a,1)==PS_OK && api->step(&b,1)==PS_OK);
    CHECK(fabs(b.values[0]+6)<1e-12 && fabs(b.values[1]-(2-4.903325))<1e-12 && b.values[6]==0 && b.values[7]==0);
    CHECK(fabs(a.values[8]-initial[8])<1e-12 && fabs(a.values[6]-.15)<1e-12);
    for(unsigned i=0;i<3;i++) {
        double saved[PS_MAX_CHANNELS];memcpy(saved,a.values,sizeof saved);
        CHECK(api->step(&a,i==0?0:i==1?-1:NAN)==PS_INVALID && !memcmp(saved,a.values,sizeof saved));
    }
    a.time_s=0;b.time_s=0;
    CHECK(api->reset(&a)==PS_OK && api->reset(&b)==PS_OK && !memcmp(initial,a.values,sizeof initial) && b.values[2]==-4);
    a.seed=43;CHECK(api->reset(&a)==PS_OK && a.values[2]!=initial[2] && b.values[2]==-4);
    api->destroy(&a);api->destroy(&b);ps_module_close(module);return 0;
}
int main(int argc,char **argv){CHECK(argc==3 && !verify(argv[1]) && !verify(argv[2]));puts("Monte Carlo instances: seed replay, independent parameters, zero sigma, energy and rejected steps passed");return 0;}
