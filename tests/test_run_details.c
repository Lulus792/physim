#include "run_details.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Run details %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void) {
 char text[PS_RUN_DETAILS_TEXT_CAPACITY],before[PS_RUN_DETAILS_TEXT_CAPACITY];memset(before,0x5a,sizeof before);
 const char *metadata="model=Point pendulum α\r\nexcluded=rod inertia\nlayer.geometry=ideal|Point mass; rigid rod\nlayer.laws=active|Uniform gravity\nlayer.losses=omitted|Vacuum\nlayer.numerics=active|RK4\nlayer.measurement=ideal|Exact angle\nlayer.sampling=omitted|One deterministic run\nparameter.mass=2\nparameter_unit.mass=kg\nseed=42\nmodule_compiler=Fixture compiler\nfuture_key=DO_NOT_OPEN ; anything\n";
 CHECK(ps_run_details_format(metadata,strlen(metadata),text,sizeof text)==PS_OK);
 CHECK(strstr(text,"Idealisiert · Point mass; rigid rod") && strstr(text,"Aktiv · Uniform gravity") && strstr(text,"Nicht modelliert · Vacuum"));
 CHECK(strstr(text,"Material und Medium\nNicht dokumentiert") && strstr(text,"parameter.mass=2") && strstr(text,"parameter_unit.mass=kg") && strstr(text,"Seed: 42"));
 CHECK(ps_run_details_format(NULL,0,text,sizeof text)==PS_OK && strstr(text,"Nicht dokumentiert"));
 const char *bad[]={"layer.geometry=unknown|Test","layer.geometry=active|","layer.geometry=active|One\nlayer.geometry=ideal|Two","model=One\nmodel=Two","layer.geometry=active","layer.geometry=active|\xff"};
 for(unsigned i=0;i<sizeof bad/sizeof *bad;i++){memcpy(text,before,sizeof text);CHECK(ps_run_details_format(bad[i],strlen(bad[i]),text,sizeof text)==PS_CORRUPT && !memcmp(text,before,sizeof text));}
 char nul[]="layer.geometry=active|Test\0hidden";memcpy(text,before,sizeof text);
 CHECK(ps_run_details_format(nul,sizeof nul-1,text,sizeof text)==PS_CORRUPT && !memcmp(text,before,sizeof text));
 CHECK(ps_run_details_format(metadata,strlen(metadata),text,1)==PS_LIMIT && !memcmp(text,before,sizeof text));
 CHECK(ps_run_details_format(NULL,1,text,sizeof text)==PS_INVALID && !memcmp(text,before,sizeof text));
 puts("Run details: declared active/ideal/omitted layers, legacy unknowns, parameters, strict UTF-8/duplicates and atomic bounds passed");return 0;
}
