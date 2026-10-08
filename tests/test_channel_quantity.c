#define PSRT_MODULE
#define PSRT_SOURCE "channel-quantity.phys"
#include "physim/language_sdk.h"
#include <float.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"channel quantity %d: %s\n",__LINE__,#x); return 1; } } while (0)
static psrt_trap trap;
static ps_diagnostic diagnostic;
static char error[2048];
static int rejected_host(psrt_host *host, psrt_channel channel, ps_quantity value, ps_result expected) {
    memset(&trap,0,sizeof trap);memset(&diagnostic,0,sizeof diagnostic);
    trap.error=error;trap.capacity=sizeof error;trap.diagnostic=&diagnostic;
    trap.operation="channel.sampleQuantity";psrt_current=&trap;
    if (!setjmp(trap.jump)) {
        psrt_sample_quantity(host,channel,value,(psrt_site){"channel-quantity.phys",5,7});
        psrt_current=NULL;CHECK(false);
    }
    psrt_current=NULL;
    CHECK(trap.failure_code==expected && ps_diagnostic_valid(&diagnostic) && diagnostic.code==expected);
    CHECK(diagnostic.line==5 && diagnostic.column==7 && !strcmp(diagnostic.source,"channel-quantity.phys"));
    return 0;
}
int main(void) {
    ps_context c={0};c.struct_size=sizeof c;c.api_version=PS_API_VERSION;c.time_s=3;c.seed=99;
    CHECK(ps_channel_add(&c,"length",PS_METRE,"typed SI")==0);
    CHECK(ps_channel_add(&c,"time",PS_SECOND,"unchanged")==1);
    CHECK(ps_channel_add(&c,"uncertainty",PS_METRE,"standard uncertainty")==2);
    c.values[0]=91;c.values[1]=92;c.values[2]=93;
    ps_unit cm=PS_METRE;cm.scale=.01;cm.symbol="cm";
    ps_quantity quantity={125,cm};ps_context expected=c;
    expected.values[0]=1.25;
    CHECK(ps_channel_sample_quantity(&c,0,quantity)==PS_OK && !memcmp(&c,&expected,sizeof c));
    CHECK(ps_channel_sample_quantity(NULL,0,quantity)==PS_INVALID);
    ps_context saved=c;
    ps_quantity invalid[]={{1,PS_KILOGRAM},{NAN,PS_METRE},{1,{{1},0,"bad"}},
                            {DBL_MAX,{{1},2,"large"}},{DBL_TRUE_MIN,{{1},.5,"small"}}};
    for(unsigned i=0;i<5;i++) {
        CHECK(ps_channel_sample_quantity(&c,0,invalid[i])==(i<3?PS_INVALID:PS_NUMERIC));
        CHECK(!memcmp(&c,&saved,sizeof c));
    }
    CHECK(ps_channel_sample_quantity(&c,UINT32_MAX,quantity)==PS_INVALID && !memcmp(&c,&saved,sizeof c));
    c.struct_size=(uint32_t)offsetof(ps_context,values);saved=c;
    CHECK(ps_channel_sample_quantity(&c,0,quantity)==PS_VERSION && !memcmp(&c,&saved,sizeof c));
    c.struct_size=sizeof c;c.channel_count=PS_MAX_CHANNELS+1;saved=c;
    CHECK(ps_channel_sample_quantity(&c,0,quantity)==PS_INVALID && !memcmp(&c,&saved,sizeof c));
    c.channel_count=3;memset(c.channels[0].name,'x',sizeof c.channels[0].name);saved=c;
    CHECK(ps_channel_sample_quantity(&c,0,quantity)==PS_INVALID && !memcmp(&c,&saved,sizeof c));
    strcpy(c.channels[0].name,"length");quantity.value=-0.0;
    CHECK(ps_channel_sample_quantity(&c,0,quantity)==PS_OK && signbit(c.values[0]));
    ps_sensor_config config={0};config.unit=cm;config.rate_hz=1;config.uncertainty_absolute=1;
    ps_sensor sensor;ps_measurement reading;
    CHECK(ps_sensor_init(&sensor,&config,4)==PS_OK);
    CHECK(ps_sensor_read(&sensor,0,(ps_quantity){1.25,PS_METRE},&reading)==PS_OK && reading.state==PS_MEASUREMENT_VALID);
    CHECK(reading.value.value==125 && reading.value.unit.scale==.01 && reading.standard_uncertainty==1);
    CHECK(ps_channel_sample_quantity(&c,0,reading.value)==PS_OK && c.values[0]==1.25);
    CHECK(ps_channel_sample_quantity(&c,2,(ps_quantity){reading.standard_uncertainty,reading.value.unit})==PS_OK && c.values[2]==.01);
    psrt_host host={0};host.context=&c;host.phase=PSRT_STEP;
    psrt_channel channel={&c,0};psrt_sample_quantity(&host,channel,(ps_quantity){250,cm},(psrt_site){"channel-quantity.phys",1,1});
    CHECK(c.values[0]==2.5);saved=c;
    CHECK(rejected_host(&host,channel,invalid[0],PS_INVALID)==0 && !memcmp(&c,&saved,sizeof c));
    CHECK(rejected_host(&host,channel,invalid[3],PS_NUMERIC)==0 && !memcmp(&c,&saved,sizeof c));
    host.phase=PSRT_SCENE;
    CHECK(rejected_host(&host,channel,quantity,PS_INVALID)==0 && !memcmp(&c,&saved,sizeof c));
    host.phase=PSRT_STEP;ps_context foreign={0};channel.owner=&foreign;
    CHECK(rejected_host(&host,channel,quantity,PS_INVALID)==0 && !memcmp(&c,&saved,sizeof c));
    CHECK(rejected_host(NULL,channel,quantity,PS_INVALID)==0);
    puts("Typed channel sampling: SI conversion, sensor/uncertainty bridge, dimensions, whole-context failure preservation, owner/phase checks and typed diagnostics passed");
    return 0;
}
