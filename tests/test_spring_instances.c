#include "physim/experiment.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Spring instances %d: %s\n",__LINE__,#x);return 1;}}while(0)
static int verify(const char *path) {
    void *module=ps_module_open(path);CHECK(module);void *symbol=ps_module_symbol(module,"ps_get_experiment");ps_experiment_entry entry=NULL;memcpy(&entry,&symbol,sizeof entry);CHECK(entry);
    const ps_experiment_api *api=entry();ps_context a={0},b={0};a.struct_size=b.struct_size=sizeof a;a.api_version=b.api_version=PS_API_VERSION;
    CHECK(ps_parameter_override(&a,"damping",0)==PS_OK && ps_parameter_override(&b,"damping",8)==PS_OK);
    CHECK(api->create(&a)==PS_OK && api->create(&b)==PS_OK && ps_parameter_finalize(&a)==PS_OK && ps_parameter_finalize(&b)==PS_OK);
    ps_parameter_unit unit;CHECK(ps_parameter_unit_read(&a,0,&unit)==PS_OK && !strcmp(unit.symbol,"N s/m") && unit.dimension[1]==1 && unit.dimension[2]==-1);
    for(unsigned i=0;i<=200;i++) {
        double t=i*.005;CHECK(fabs(a.values[0]-.35*cos(4*t))<3e-8 && fabs(b.values[0]-.35*(1+4*t)*exp(-4*t))<3e-8);
        if(i==200)break;
        CHECK(api->step(&a,.005)==PS_OK && api->step(&b,.005)==PS_OK);a.time_s=b.time_s=(i+1)*.005;
    }
    CHECK(api->reset(&a)==PS_OK && api->reset(&b)==PS_OK && a.values[0]==.35 && b.values[0]==.35 && a.values[5]==0 && b.values[5]==0);
    double saved[PS_MAX_CHANNELS];memcpy(saved,a.values,sizeof saved);CHECK(api->step(&a,0)==PS_INVALID && !memcmp(saved,a.values,sizeof saved));
    api->destroy(&a);api->destroy(&b);ps_module_close(module);return 0;
}
int main(int argc,char **argv){CHECK(argc==3 && !verify(argv[1]) && !verify(argv[2]));puts("Spring instances: independent damping values, unit metadata, resets and rejected steps passed");return 0;}
