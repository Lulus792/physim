#include "physim/experiment.h"
#include "physim/units.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Pendulum instances %d: %s\n",__LINE__,#x);return 1;}}while(0)
static int verify(const char *path) {
    void *module=ps_module_open(path);CHECK(module);void *symbol=ps_module_symbol(module,"ps_get_experiment");
    ps_experiment_entry entry=NULL;memcpy(&entry,&symbol,sizeof entry);CHECK(entry);const ps_experiment_api *api=entry();
    ps_context a={.struct_size=sizeof a,.api_version=PS_API_VERSION},b=a;
    CHECK(ps_parameter_override(&a,"integrator",0)==PS_OK && ps_parameter_override(&b,"integrator",3)==PS_OK);
    CHECK(ps_parameter_override(&b,"length",.7)==PS_OK && ps_parameter_override(&b,"initialAngle",-.7)==PS_OK);
    CHECK(api->create(&a)==PS_OK && api->create(&b)==PS_OK && ps_parameter_finalize(&a)==PS_OK && ps_parameter_finalize(&b)==PS_OK);
    CHECK(a.values[0]==.45 && b.values[0]==-.7 && a.values[1]==0 && b.values[1]==0);
    ps_parameter_unit unit;unsigned length_index=0;while(length_index<a.parameter_count && strcmp(a.parameters[length_index].name,"length"))length_index++;
    CHECK(length_index<a.parameter_count && ps_parameter_unit_read(&a,length_index,&unit)==PS_OK && unit.dimension[0]==1);
    for(unsigned i=0;i<3;i++) {
        double saved[PS_MAX_CHANNELS];memcpy(saved,a.values,sizeof saved);
        CHECK(api->step(&a,i==0?0:i==1?-1:NAN)==PS_INVALID && !memcmp(saved,a.values,sizeof saved));
    }
    CHECK(api->step(&a,.01)==PS_OK && api->step(&b,.01)==PS_OK);
    CHECK(a.values[0]==.45 && fabs(a.values[1]+.01*9.80665/1.5*sin(.45))<1e-15);
    CHECK(b.values[0]>-.7 && b.values[1]>0);
    ps_step_interval interval={42,43};double saved[PS_MAX_CHANNELS];memcpy(saved,a.values,sizeof saved);
    ps_result rejected=api->adaptive_step(&a,.1,1e-8,.1,&interval);
    CHECK((rejected==PS_INVALID || rejected==PS_NUMERIC) && !memcmp(saved,a.values,sizeof saved));
    CHECK(api->reset(&a)==PS_OK);memcpy(saved,a.values,sizeof saved);
    CHECK(api->step(&a,1e155)==PS_NUMERIC && !memcmp(saved,a.values,sizeof saved));
    CHECK(api->step(&b,.01)==PS_OK); /* A failed instance never poisons B. */
    CHECK(api->reset(&a)==PS_OK && api->step(&a,.01)==PS_OK && a.values[0]==.45 &&
          fabs(a.values[1]+.01*9.80665/1.5*sin(.45))<1e-14);
    CHECK(api->reset(&a)==PS_OK && api->reset(&b)==PS_OK && a.values[0]==.45 && b.values[0]==-.7);
    api->destroy(&a);api->destroy(&b);ps_module_close(module);return 0;
}
int main(int argc,char **argv){CHECK(argc==3 && !verify(argv[1]) && !verify(argv[2]));puts("Pendulum instances: independent methods/parameters, typed units, reset and failed-step preservation passed");return 0;}
