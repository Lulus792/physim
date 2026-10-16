/* Completes the callback around the unchanged published statements. */
    return result;
}
#include <math.h>
#include <stdio.h>
#include <string.h>
typedef struct {unsigned calls;ps_result response;ps_log_record last;} published_sink;
static ps_result published_capture(void *user,const ps_log_record *record) {
    published_sink *sink=user;++sink->calls;sink->last=*record;return sink->response;
}
int main(void) {
    published_sink sink={0};
    ps_context context={0};context.api_version=PS_API_VERSION;context.struct_size=sizeof context;
    context.time_s=12.5;context.logger=(ps_logger){&sink,published_capture};
    unsigned char before[sizeof context];memcpy(before,&context,sizeof context);
    if(published_logging_example(&context)!=PS_OK || sink.calls!=1 ||
       sink.last.level!=PS_LOG_INFO || sink.last.time_s!=12.5 ||
       strcmp(sink.last.message,"Experiment bereit") || memcmp(&context,before,sizeof context))return 1;
    sink.response=PS_IO;
    if(published_logging_example(&context)!=PS_IO || sink.calls!=2)return 1;
    context.time_s=NAN;
    if(published_logging_example(&context)!=PS_INVALID || sink.calls!=2)return 1;
    context.time_s=12.5;context.struct_size=(uint32_t)offsetof(ps_context,logger);
    if(published_logging_example(&context)!=PS_VERSION || sink.calls!=2)return 1;
    context.struct_size=sizeof context;context.logger.write=NULL;
    if(published_logging_example(&context)!=PS_OK || sink.calls!=2)return 1;
    puts("Published logging example: message, time, sink errors, validation and legacy context passed");
    return 0;
}
