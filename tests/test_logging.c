#include "physim/experiment.h"
#include "protocol.h"
#include "physim/language_sdk.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Logging line %d: %s\n",__LINE__,#x);return 1;}}while(0)
typedef struct {unsigned calls;ps_result result;ps_log_record last;} sink;
static ps_result capture(void *user,const ps_log_record *record) {
    sink *s=user;s->calls++;s->last=*record;return s->result;
}
int main(void) {
    sink a={0},b={0};ps_logger x={&a,capture},y={&b,capture};
    CHECK(ps_logger_emit(&x,PS_LOG_INFO,.25,"Unicode α, quote \" and newline\n") == PS_OK);
    CHECK(a.calls==1 && b.calls==0 && a.last.time_s==.25 && ps_log_record_valid(&a.last));
    CHECK(ps_logger_emit(&y,PS_LOG_DEBUG,-1,"Independent sink") == PS_OK && b.calls==1 && a.calls==1);
    unsigned calls=a.calls;
    CHECK(ps_logger_emit(&x,(ps_log_level)0,0,"bad")==PS_INVALID && ps_logger_emit(&x,PS_LOG_INFO,NAN,"bad")==PS_INVALID);
    CHECK(ps_logger_emit(&x,PS_LOG_INFO,0,"")==PS_INVALID && ps_logger_emit(&x,PS_LOG_INFO,0,"\xc0\x80")==PS_INVALID);
    CHECK(ps_logger_emit(&x,PS_LOG_INFO,0,"\033bad")==PS_INVALID && a.calls==calls);
    char text[PS_LOG_MESSAGE_MAX+2];memset(text,'x',sizeof text);text[PS_LOG_MESSAGE_MAX]=0;
    CHECK(ps_logger_emit(&x,PS_LOG_ERROR,0,text)==PS_OK);text[PS_LOG_MESSAGE_MAX]='x';text[PS_LOG_MESSAGE_MAX+1]=0;
    CHECK(ps_logger_emit(&x,PS_LOG_INFO,0,text)==PS_INVALID);
    a.result=PS_LIMIT;CHECK(ps_logger_emit(&x,PS_LOG_WARNING,0,"Rejected by sink")==PS_LIMIT);
    ps_logger disabled={0};CHECK(ps_logger_emit(&disabled,PS_LOG_INFO,0,"disabled")==PS_OK);
    ps_context context={0};context.struct_size=sizeof context;context.api_version=PS_API_VERSION;context.time_s=.5;context.logger=y;
    ps_context before=context;CHECK(ps_experiment_log(&context,PS_LOG_INFO,"context") == PS_OK && !memcmp(&before,&context,sizeof context));
    context.struct_size=(uint32_t)offsetof(ps_context,logger);CHECK(ps_experiment_log(&context,PS_LOG_INFO,"old context")==PS_VERSION);
    psrt_host host={.context=&context};psrt_site site={"logging",1,1};
    CHECK(!psrt_log_info(&host,"old host",site));
    context.struct_size=sizeof context;
    CHECK(psrt_log_debug(&host,"language debug",site) && b.last.level==PS_LOG_DEBUG);
    CHECK(psrt_log_info(&host,"language info",site) && b.last.level==PS_LOG_INFO);
    CHECK(psrt_log_warning(&host,"language warning",site) && b.last.level==PS_LOG_WARNING);
    CHECK(psrt_log_error(&host,"language error",site) && b.last.level==PS_LOG_ERROR);
    CHECK(!psrt_log_info(&host,"",site) && !psrt_log_info(&host,text,site));
    b.result=PS_IO;CHECK(!psrt_log_info(&host,"sink IO rejection",site));b.result=PS_OK;
    unsigned char wire[12+PS_LOG_MESSAGE_MAX];size_t n=ps_wire_log_encode(wire,sizeof wire,&b.last);CHECK(n>12);
    ps_log_record decoded={0};CHECK(ps_wire_log_decode(wire,n,&decoded) && !strcmp(decoded.message,b.last.message));
    ps_log_record unchanged=decoded;wire[0]=9;
    CHECK(!ps_wire_log_decode(wire,n,&decoded) && !memcmp(&decoded,&unchanged,sizeof decoded));wire[0]=PS_LOG_INFO;
    CHECK(!ps_wire_log_decode(wire,12,&decoded));wire[12]=0;CHECK(!ps_wire_log_decode(wire,n,&decoded));
    CHECK(!strcmp(ps_log_level_name(PS_LOG_WARNING),"warning") && !strcmp(ps_log_level_name((ps_log_level)9),"unknown"));
    puts("Logging: explicit sinks, UTF-8 bounds, context compatibility, rejection and transactional wire decoding passed");return 0;
}
