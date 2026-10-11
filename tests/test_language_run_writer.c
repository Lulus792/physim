#define PSRT_MODULE
#define PSRT_SOURCE "run-writer-memory.phys"
#include "physim/language_sdk.h"
#include "test_allocator.h"
#include <stdio.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Language writer %d: %s\n",__LINE__,#x);return 1;}}while(0)
static test_allocator domain;
static psrt_trap trap;static char error[2048];
static psrt_stream_store store;
static psrt_stream_writer writer,alias,capacity[8];
static ps_snapshot scenes[3];
static void begin(void){memset(&trap,0,sizeof trap);trap.error=error;trap.capacity=sizeof error;psrt_current=&trap;}
#define FAIL(code,call) do {begin();if(!setjmp(trap.jump)){call;CHECK(false);}CHECK(trap.failure_code==(code));}while(0)
int main(int argc,char **argv) {
    CHECK(argc==2);char path[4096],source[4096];
    snprintf(path,sizeof path,"%s/c-writer-reference.psrun",argv[1]);
    snprintf(source,sizeof source,"%s/stream α.psrun",argv[1]);
    ps_run_reader reader;CHECK(ps_run_open(&reader,source)==PS_OK);
    for(unsigned i=0;i<3;i++)CHECK(ps_run_snapshot_next(&reader,&scenes[i])==PS_OK);
    CHECK(ps_run_snapshot_next(&reader,&scenes[0])==PS_EOF);ps_run_reader_close(&reader);
    ps_context c={.struct_size=sizeof c,.api_version=PS_API_VERSION,.dt_s=.125,.seed=42};
    strcpy(c.model_metadata,"writer_source=reference");
    CHECK(ps_channel_add(&c,"position.x",PS_METRE,"Measured x α")==0 && ps_channel_add(&c,"energy",PS_JOULE,"Energy")==1);
    ps_run_writer native;CHECK(ps_run_create(&native,path,&c,"writer fixture")==PS_OK);
    for(unsigned i=0;i<3;i++){double values[]={10+2*i,20+3*i};CHECK(ps_run_append(&native,i*.125,values)==PS_OK);}
    for(unsigned i=0;i<3;i++){
        c.time_s=scenes[i].time;memcpy(c.values,scenes[i].values,sizeof c.values);
        CHECK(ps_run_append_snapshot(&native,&c,&scenes[i].scene,scenes[i].paused)==PS_OK);
    }
    CHECK(ps_run_close(&native)==PS_OK);
    psrt_site site={"run-writer-memory.phys",3,4};begin();CHECK(!setjmp(trap.jump));
    store=psrt_stream_store_make(test_domain(&domain),site);size_t baseline=domain.live_bytes;
    snprintf(path,sizeof path,"%s/c-runtime-writer.psrun",argv[1]);
    FAIL(PS_INVALID,(void)psrt_stream_writer_make(test_domain(&domain),store,path,"writer fixture",site));
    CHECK(domain.live_bytes==baseline);FILE *absent=fopen(path,"rb");CHECK(!absent);
    FAIL(PS_INVALID,psrt_stream_store_configure(store,0,42,"bad",site));
    CHECK(!store.owner->configured);
    begin();CHECK(!setjmp(trap.jump));psrt_stream_store_configure(store,.125,42,"writer_source=reference",site);
    ps_context before=store.owner->schema;char large[2049];memset(large,'x',sizeof large);large[2048]=0;
    FAIL(PS_LIMIT,psrt_stream_store_configure(store,.25,9,large,site));
    CHECK(!memcmp(&before,&store.owner->schema,sizeof before));
    FAIL(PS_INVALID,psrt_stream_store_configure(store,.25,9,"\xc0\x80",site));
    CHECK(!memcmp(&before,&store.owner->schema,sizeof before));
    begin();CHECK(!setjmp(trap.jump));CHECK(psrt_stream_store_channel(store,"position.x",PS_METRE,"Measured x α",site)==0);
    before=store.owner->schema;
    FAIL(PS_INVALID,(void)psrt_stream_store_channel(store,"position.x",PS_METRE,"duplicate",site));
    ps_unit scaled=PS_METRE;scaled.scale=1000;
    FAIL(PS_INVALID,(void)psrt_stream_store_channel(store,"scaled",scaled,"bad",site));
    CHECK(!memcmp(&before,&store.owner->schema,sizeof before));
    begin();CHECK(!setjmp(trap.jump));CHECK(psrt_stream_store_channel(store,"energy",PS_JOULE,"Energy",site)==1);
    domain.fail_on=domain.attempts+1;
    FAIL(PS_MEMORY,(void)psrt_stream_writer_make(test_domain(&domain),store,path,"writer fixture",site));
    CHECK(domain.live_bytes==baseline && store.owner->references==1 && !fopen(path,"rb"));
    domain.fail_on=0;begin();CHECK(!setjmp(trap.jump));
    writer=psrt_stream_writer_make(test_domain(&domain),store,path,"writer fixture",site);
    CHECK(psrt_stream_writer_copy(&alias,&writer)==PS_OK);
    size_t bytes=domain.live_bytes;
    FAIL(PS_IO,(void)psrt_stream_writer_make(test_domain(&domain),store,path,"writer fixture",site));
    CHECK(domain.live_bytes==bytes && psrt_stream_writer_open(alias,site));
    static const psrt_element_type element={sizeof(double),NULL,NULL};psrt_array values;
    begin();CHECK(!setjmp(trap.jump));CHECK(psrt_array_init(&element,test_domain(&domain),2,&values)==PS_OK);
    double sample[]={10,20};CHECK(psrt_array_replace(&values,0,0,sample,1)==PS_OK);
    FAIL(PS_INVALID,psrt_stream_writer_append(writer,0,psrt_array_data(&values),psrt_array_count(&values),site));
    CHECK(psrt_stream_writer_samples(writer,site)==0);
    CHECK(psrt_array_replace(&values,0,1,sample,2)==PS_OK);
    FAIL(PS_INVALID,psrt_stream_writer_append(writer,NAN,psrt_array_data(&values),psrt_array_count(&values),site));
    CHECK(psrt_stream_writer_samples(writer,site)==0);
    sample[1]=NAN;CHECK(psrt_array_replace(&values,0,2,sample,2)==PS_OK);
    FAIL(PS_INVALID,psrt_stream_writer_append(writer,0,psrt_array_data(&values),psrt_array_count(&values),site));
    CHECK(psrt_stream_writer_samples(writer,site)==0);
    for(unsigned i=0;i<3;i++){
        sample[0]=10+2*i;sample[1]=20+3*i;CHECK(psrt_array_replace(&values,0,2,sample,2)==PS_OK);
        begin();CHECK(!setjmp(trap.jump));psrt_stream_writer_append(alias,i*.125,psrt_array_data(&values),psrt_array_count(&values),site);
        CHECK(psrt_stream_writer_samples(writer,site)==i+1);
    }
    for(unsigned i=0;i<3;i++)psrt_stream_writer_snapshot(writer,scenes[i],site);
    CHECK(psrt_stream_writer_samples(writer,site)==3);
    ps_snapshot malformed=scenes[0];malformed.count=1;
    FAIL(PS_INVALID,psrt_stream_writer_snapshot(writer,malformed,site));
    malformed=scenes[0];malformed.time=NAN;
    FAIL(PS_INVALID,psrt_stream_writer_snapshot(writer,malformed,site));
    CHECK(psrt_stream_writer_samples(writer,site)==3);
    ps_run_write_handle retired=writer.owner->handle;size_t refs=writer.owner->references;
    writer.owner->references=SIZE_MAX;psrt_stream_writer untouched={0};
    CHECK(psrt_stream_writer_copy(&untouched,&writer)==PS_LIMIT && !untouched.owner);writer.owner->references=refs;
    psrt_stream_writer_close(alias,site);CHECK(!psrt_stream_writer_open(writer,site));
    FAIL(PS_INVALID,psrt_stream_writer_append(writer,1,psrt_array_data(&values),psrt_array_count(&values),site));
    begin();CHECK(!setjmp(trap.jump));large[2047]=0;
    psrt_stream_store_configure(store,.5,-1,large,site);
    CHECK(strlen(store.owner->schema.model_metadata)==2047 && store.owner->schema.seed==UINT64_MAX);
    for(unsigned i=2;i<PS_MAX_CHANNELS;i++){
        char name[48];snprintf(name,sizeof name,"channel-%u",i);
        CHECK(psrt_stream_store_channel(store,name,PS_METRE,"capacity",site)==i);
    }
    before=store.owner->schema;
    FAIL(PS_LIMIT,(void)psrt_stream_store_channel(store,"overflow",PS_METRE,"capacity",site));
    CHECK(!memcmp(&before,&store.owner->schema,sizeof before));
    for(unsigned i=0;i<8;i++){
        snprintf(path,sizeof path,"%s/c-writer-slot-%u.psrun",argv[1],i);
        begin();CHECK(!setjmp(trap.jump));capacity[i]=psrt_stream_writer_make(test_domain(&domain),store,path,"slot",site);
    }
    CHECK(capacity[0].owner->handle.slot==retired.slot && capacity[0].owner->handle.generation!=retired.generation);
    ps_run_write_info invalid;CHECK(ps_run_writer_describe(store.owner->store,retired,&invalid)==PS_INVALID);
    snprintf(path,sizeof path,"%s/c-writer-overflow.psrun",argv[1]);bytes=domain.live_bytes;
    FAIL(PS_LIMIT,(void)psrt_stream_writer_make(test_domain(&domain),store,path,"overflow",site));
    CHECK(bytes==domain.live_bytes && !fopen(path,"rb"));
    begin();CHECK(!setjmp(trap.jump));psrt_stream_writer_abort(capacity[0],site);
    psrt_stream_writer replacement=psrt_stream_writer_make(test_domain(&domain),store,path,"replacement",site);
    CHECK(!psrt_stream_writer_open(capacity[0],site));
    psrt_stream_store_close(store,site);CHECK(!psrt_stream_writer_open(replacement,site));
    for(unsigned i=0;i<8;i++)psrt_stream_writer_drop(&capacity[i]);
    psrt_stream_writer_drop(&replacement);psrt_stream_writer_drop(&alias);psrt_stream_writer_drop(&writer);
    psrt_array_destroy(&values);psrt_stream_store_drop(&store);psrt_current=NULL;
    CHECK(!domain.live_blocks && !domain.live_bytes && !domain.invalid);
    puts("Language writers: atomic schema, allocation failure, shared ownership, exclusive files, counters, snapshots, stale slots and cleanup passed");return 0;
}
