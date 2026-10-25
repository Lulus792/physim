#include "protocol_store.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Protocol line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void) {
    ps_protocol store={0};uint64_t id=999;
    const unsigned char original[]={'A',0,'B','\n','\r','\t',0xce,0xb1,0xe4};
    char title[]="Warnung",context[]="Lauf 004",source[]="src/model.c",operation[]="step",argument[]="dt";
    ps_protocol_message m={.level=PS_LOG_WARNING,.origin=PS_PROTOCOL_SIMULATION,.title=title,.context=context,.source=source,
        .operation=operation,.argument=argument,.has_code=true,.code=PS_NUMERIC,.transcript_id=777,
        .line=12,.column=4,.has_time=true,.time_s=2.5,.raw=original,.raw_size=sizeof original};
    CHECK(ps_protocol_push(&store,&m,&id)==PS_OK && id!=999 && id);
    memset(title,'x',strlen(title));memset(context,'y',strlen(context));memset(source,'z',strlen(source));
    memset(operation,'x',strlen(operation));memset(argument,'x',strlen(argument));
    const ps_protocol_record *r=ps_protocol_find(&store,id);
    CHECK(r && !strcmp(r->message.title,"Warnung") && !strcmp(r->message.context,"Lauf 004") && !strcmp(r->message.source,"src/model.c"));
    CHECK(!strcmp(r->message.operation,"step") && !strcmp(r->message.argument,"dt") && r->message.has_code && r->message.code==PS_NUMERIC && r->message.transcript_id==777);
    CHECK(r->message.raw_size==sizeof original && !memcmp(r->message.raw,original,sizeof original));
    CHECK(r->message.has_time && r->message.time_s==2.5 && r->message.line==12 && r->message.column==4 && r->message.outcome==PS_PROTOCOL_NO_OUTCOME);
    /* Alias-safe append, including bytes after an embedded NUL. */
    CHECK(ps_protocol_append(&store,id,r->message.raw,r->message.raw_size)==PS_OK);
    r=ps_protocol_find(&store,id);
    CHECK(r->message.raw_size==2*sizeof original && !memcmp(r->message.raw,original,sizeof original) && !memcmp((const char*)r->message.raw+sizeof original,original,sizeof original));
    CHECK(ps_protocol_set_outcome(&store,id,PS_PROTOCOL_RUNNING));
    CHECK(ps_protocol_find(&store,id)->message.outcome==PS_PROTOCOL_RUNNING);
    CHECK(!ps_protocol_set_outcome(&store,id,(ps_protocol_outcome)99));
    ps_protocol_summary summary=ps_protocol_describe(&store);
    CHECK(summary.count==1 && summary.levels[PS_LOG_WARNING]==1 && summary.origins[PS_PROTOCOL_SIMULATION]==1 && !summary.omitted_bytes);
    ps_protocol previous=store;uint64_t unchanged=1234;
    ps_protocol_message bad=m;bad.level=(ps_log_level)0;
    CHECK(ps_protocol_push(&store,&bad,&unchanged)==PS_INVALID && unchanged==1234 && !memcmp(&store,&previous,sizeof store));
    bad=m;bad.raw=NULL;CHECK(ps_protocol_push(&store,&bad,NULL)==PS_INVALID);
    bad=m;bad.source="";CHECK(ps_protocol_push(&store,&bad,NULL)==PS_INVALID);
    bad=m;bad.has_time=true;bad.time_s=NAN;CHECK(ps_protocol_push(&store,&bad,NULL)==PS_INVALID);
    CHECK(ps_protocol_append(&store,id,NULL,1)==PS_INVALID && !memcmp(&store,&previous,sizeof store));
    CHECK(ps_protocol_append(&store,UINT64_MAX,original,sizeof original)==PS_INVALID);
    CHECK(!ps_protocol_remove(&store,UINT64_MAX) && !ps_protocol_at(&store,1));
    /* An error record has no implied failed outcome or host action. */
    m=(ps_protocol_message){.level=PS_LOG_ERROR,.origin=PS_PROTOCOL_BUILD,.title="Build",.raw="error: x\n  source();\n\t^",.raw_size=23};
    m.raw_size=strlen(m.raw);
    CHECK(ps_protocol_push(&store,&m,NULL)==PS_OK && ps_protocol_at(&store,1)->message.outcome==PS_PROTOCOL_NO_OUTCOME);
    CHECK(ps_protocol_remove(&store,id) && !ps_protocol_find(&store,id) && ps_protocol_describe(&store).count==1);
    uint64_t last=ps_protocol_at(&store,0)->id;
    ps_protocol_clear(&store);CHECK(!ps_protocol_describe(&store).count && !ps_protocol_describe(&store).bytes);
    CHECK(ps_protocol_push(&store,&m,&id)==PS_OK && id>last);
    for(unsigned i=0;i<PS_PROTOCOL_CAPACITY+17;i++) {
        char label[40];snprintf(label,sizeof label,"Meldung %u",i);m.title=label;
        m.level=(ps_log_level)(PS_LOG_DEBUG+i%4);m.origin=(ps_protocol_origin)(i%PS_PROTOCOL_ORIGIN_COUNT);
        CHECK(ps_protocol_push(&store,&m,NULL)==PS_OK);
    }
    summary=ps_protocol_describe(&store);
    CHECK(summary.count==200 && summary.omitted_messages==18 && summary.bytes<=PS_PROTOCOL_BYTE_BUDGET);
    CHECK(!strcmp(ps_protocol_at(&store,0)->message.title,"Meldung 17") && !strcmp(ps_protocol_at(&store,199)->message.title,"Meldung 216"));
    for(unsigned i=1;i<summary.count;i++)CHECK(ps_protocol_at(&store,i-1)->id<ps_protocol_at(&store,i)->id);
    /* Copy from an entry that this push will evict. */
    ps_protocol_message borrowed=ps_protocol_at(&store,0)->message;
    CHECK(ps_protocol_push(&store,&borrowed,&id)==PS_OK);
    CHECK(!strcmp(ps_protocol_find(&store,id)->message.title,"Meldung 17"));
    size_t before_count=ps_protocol_describe(&store).count;
    CHECK(ps_protocol_remove(&store,ps_protocol_at(&store,33)->id));
    CHECK(ps_protocol_describe(&store).count==before_count-1 && ps_protocol_find(&store,id));
    ps_protocol_clear(&store);
    size_t large_size=PS_PROTOCOL_BYTE_BUDGET/2;
    char *large=malloc(PS_PROTOCOL_BYTE_BUDGET+1u);CHECK(large);memset(large,'q',PS_PROTOCOL_BYTE_BUDGET);large[PS_PROTOCOL_BYTE_BUDGET]=0;
    m=(ps_protocol_message){.level=PS_LOG_DEBUG,.origin=PS_PROTOCOL_BUILD,.title="Originalausgabe",.raw=large,.raw_size=large_size};
    CHECK(ps_protocol_push(&store,&m,&last)==PS_OK && ps_protocol_push(&store,&m,&id)==PS_OK);
    summary=ps_protocol_describe(&store);
    CHECK(summary.count==1 && summary.omitted_messages==1 && !ps_protocol_find(&store,last));
    r=ps_protocol_find(&store,id);CHECK(r->message.raw_size==large_size && !memcmp(r->message.raw,large,large_size));
    CHECK(ps_protocol_append(&store,id,large,large_size)==PS_LIMIT);
    r=ps_protocol_find(&store,id);CHECK(r->message.raw_size==large_size && r->omitted_bytes==large_size && !memcmp(r->message.raw,large,large_size));
    m.raw_size=PS_PROTOCOL_BYTE_BUDGET;
    CHECK(ps_protocol_push(&store,&m,&unchanged)==PS_LIMIT && unchanged==1234 && ps_protocol_find(&store,id));
    summary=ps_protocol_describe(&store);CHECK(summary.omitted_messages==2 && summary.omitted_bytes==large_size*2+PS_PROTOCOL_BYTE_BUDGET);
    CHECK(ps_protocol_append(&store,id,NULL,0)==PS_OK);
    ps_protocol_clear(&store);free(large);
    m=(ps_protocol_message){.level=PS_LOG_DEBUG,.origin=PS_PROTOCOL_ANALYSIS,.title="Long transcript"};
    CHECK(ps_protocol_push(&store,&m,&id)==PS_OK);
    unsigned char chunk[4096];
    for(unsigned i=0;i<256;i++){memset(chunk,(unsigned char)i,sizeof chunk);CHECK(ps_protocol_append(&store,id,chunk,sizeof chunk)==PS_OK);}
    r=ps_protocol_find(&store,id);CHECK(r->message.raw_size==256*sizeof chunk && !r->omitted_bytes);
    for(unsigned i=0;i<256;i++)for(size_t j=0;j<sizeof chunk;j++)CHECK(((const unsigned char*)r->message.raw)[i*sizeof chunk+j]==(unsigned char)i);
    CHECK(ps_protocol_describe(&store).bytes<=PS_PROTOCOL_BYTE_BUDGET);
    ps_protocol_clear(&store);
    large=malloc(3u*1024u*1024u);CHECK(large);memset(large,'r',3u*1024u*1024u);
    m.raw=large;m.raw_size=1024u*1024u;
    CHECK(ps_protocol_push(&store,&m,&last)==PS_OK && ps_protocol_push(&store,&m,&id)==PS_OK);
    CHECK(ps_protocol_append(&store,id,large,3u*1024u*1024u)==PS_LIMIT);
    CHECK(ps_protocol_find(&store,id)->message.raw_size==1024u*1024u && ps_protocol_find(&store,last));
    ps_protocol_clear(&store);free(large);
    CHECK(!ps_protocol_describe(&store).count && !ps_protocol_describe(&store).bytes && !ps_protocol_describe(&store).omitted_bytes);
    ps_protocol_clear(NULL);CHECK(!ps_protocol_at(NULL,0) && !ps_protocol_find(NULL,1) && !ps_protocol_describe(NULL).count);
    puts("Protocol: owned originals, stable IDs, FIFO/quota omissions, source/time/outcome and alias-safe transcripts passed");return 0;
}
