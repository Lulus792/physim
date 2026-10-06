#include "physim/experiment.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Collision instances %d: %s\n",__LINE__,#x);return 1;}}while(0)
static int verify(const char *path) {
    void *module=ps_module_open(path);CHECK(module);void *symbol=ps_module_symbol(module,"ps_get_experiment");ps_experiment_entry entry=NULL;
    memcpy(&entry,&symbol,sizeof entry);CHECK(entry);const ps_experiment_api *api=entry();
    ps_context a={.struct_size=sizeof a,.api_version=PS_API_VERSION},b=a;
    CHECK(ps_parameter_override(&a,"restitution",1)==PS_OK && ps_parameter_override(&b,"restitution",0)==PS_OK && ps_parameter_override(&b,"massA",2)==PS_OK);
    CHECK(api->create(&a)==PS_OK && api->create(&b)==PS_OK && ps_parameter_finalize(&a)==PS_OK && ps_parameter_finalize(&b)==PS_OK);
    ps_parameter_unit unit;unsigned index=0;while(index<b.parameter_count && strcmp(b.parameters[index].name,"massA"))index++;
    CHECK(index<b.parameter_count && ps_parameter_unit_read(&b,index,&unit)==PS_OK && unit.dimension[1]==1);
    for(unsigned i=0;i<200;i++) {
        CHECK(api->step(&a,.01)==PS_OK && api->step(&b,.01)==PS_OK);a.time_s=b.time_s=(i+1)*.01;
    }
    CHECK(fabs(a.values[2]+.6)<1e-12 && fabs(a.values[3]-.6)<1e-12 && a.values[6]==0);
    CHECK(fabs(b.values[2]-.2)<1e-12 && fabs(b.values[3]-.2)<1e-12 && fabs(b.values[6]-.48)<1e-12 && b.values[10]==1);
    for(unsigned i=0;i<3;i++) {
        double saved[PS_MAX_CHANNELS];memcpy(saved,a.values,sizeof saved);
        CHECK(api->step(&a,i==0?0:i==1?-1:NAN)==PS_INVALID && !memcmp(saved,a.values,sizeof saved));
    }
    CHECK(api->reset(&a)==PS_OK && api->reset(&b)==PS_OK && a.values[0]==-1 && b.values[0]==-1 && b.values[6]==0 && b.values[10]==0 && fabs(b.values[7]-.54)<1e-12);
    api->destroy(&a);api->destroy(&b);ps_module_close(module);return 0;
}
int main(int argc,char **argv){CHECK(argc==3 && !verify(argv[1]) && !verify(argv[2]));puts("Collision instances: independent masses/restitution, typed units, reset and rejected steps passed");return 0;}
