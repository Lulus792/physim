#define PSRT_MODULE
#define PSRT_SOURCE "run-stream-memory.phys"
#include "physim/language_sdk.h"
#include "test_allocator.h"
#include <stdio.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Language stream %d: %s\n",__LINE__,#x);return 1;}}while(0)
static test_allocator domain;
static psrt_trap trap;static char error[2048];
static psrt_stream_store store;static psrt_stream_reader reader;
static void begin(void){memset(&trap,0,sizeof trap);trap.error=error;trap.capacity=sizeof error;psrt_current=&trap;}
int main(int argc,char **argv) {
    CHECK(argc==2);char path[4096],partial[4096];
    snprintf(path,sizeof path,"%s/stream α.psrun",argv[1]);snprintf(partial,sizeof partial,"%s/partial.psrun",argv[1]);
    ps_context c={.struct_size=sizeof c,.api_version=PS_API_VERSION,.dt_s=.125,.seed=42};
    CHECK(ps_channel_add(&c,"position.x",PS_METRE,"Measured x α")==0 && ps_channel_add(&c,"energy",PS_JOULE,"Energy")==1);
    ps_run_store *native=NULL;CHECK(ps_run_store_create(ps_allocator_default(),&native)==PS_OK);
    ps_run_write_handle writer;CHECK(ps_run_writer_create(native,path,&c,"stream fixture",&writer)==PS_OK);
    for(unsigned i=0;i<3;i++) {
        c.time_s=i*.125;c.values[0]=10+2*i;c.values[1]=20+3*i;
        CHECK(ps_run_writer_append(native,writer,c.time_s,c.values,2)==PS_OK);
        ps_scene scene={0};CHECK(ps_scene_label_id(&scene,42,ps_v3(i,2,3),"Copied α",0xff00ffff)==PS_OK);
        CHECK(ps_run_writer_snapshot(native,writer,&c,&scene,i==0)==PS_OK);
    }
    CHECK(ps_run_writer_release(native,writer)==PS_OK);
    CHECK(ps_run_writer_create(native,partial,&c,"partial fixture",&writer)==PS_OK);
    CHECK(ps_run_writer_append(native,writer,0,c.values,2)==PS_OK && ps_run_writer_abort(native,writer)==PS_OK);
    CHECK(ps_run_store_destroy(native)==PS_OK);
    psrt_site site={"run-stream-memory.phys",3,4};
    for(volatile unsigned fail=1;fail<=2;fail++) {
        memset(&domain,0,sizeof domain);domain.fail_on=fail;begin();
        if(!setjmp(trap.jump)){(void)psrt_stream_store_make(test_domain(&domain),site);CHECK(false);}
        CHECK(trap.failure_code==PS_MEMORY && !domain.live_blocks && !domain.invalid);
    }
    memset(&domain,0,sizeof domain);begin();CHECK(!setjmp(trap.jump));
    store=psrt_stream_store_make(test_domain(&domain),site);size_t live=domain.live_bytes;
    domain.fail_on=domain.attempts+1;begin();
    if(!setjmp(trap.jump)){(void)psrt_stream_reader_make(test_domain(&domain),store,path,site);CHECK(false);}
    CHECK(trap.failure_code==PS_MEMORY && domain.live_bytes==live && store.owner->references==1);
    domain.fail_on=0;begin();CHECK(!setjmp(trap.jump));
    reader=psrt_stream_reader_make(test_domain(&domain),store,path,site);
    psrt_stream_reader alias;CHECK(psrt_stream_reader_copy(&alias,&reader)==PS_OK);
    CHECK(psrt_stream_reader_next(alias,site) && psrt_stream_reader_samples(reader,site)==1);
    size_t accepted=domain.live_bytes;domain.fail_on=domain.attempts+1;begin();
    if(!setjmp(trap.jump)){(void)psrt_stream_reader_row(test_domain(&domain),reader,site);CHECK(false);}
    CHECK(trap.failure_code==PS_MEMORY && domain.live_bytes==accepted && reader.owner->row.time==0 && reader.owner->row.values[0]==10);
    domain.fail_on=0;begin();CHECK(!setjmp(trap.jump));
    psrt_run_block row=psrt_stream_reader_row(test_domain(&domain),reader,site);
    CHECK(psrt_run_block_value(row,0,0,site)==10 && psrt_stream_reader_next(reader,site));
    CHECK(psrt_run_block_value(row,0,0,site)==10 && reader.owner->row.values[0]==12);
    CHECK(psrt_stream_reader_next(reader,site) && !psrt_stream_reader_next(alias,site));
    CHECK(psrt_stream_reader_complete(reader,site) && reader.owner->status==PS_EOF && !psrt_stream_reader_next(reader,site));
    begin();if(!setjmp(trap.jump)){(void)psrt_stream_reader_next_snapshot(reader,site);CHECK(false);}
    CHECK(trap.failure_code==PS_INVALID && reader.owner->status==PS_EOF);
    begin();CHECK(!setjmp(trap.jump));psrt_stream_reader_close(reader,site);
    CHECK(!psrt_stream_reader_open(alias,site));
    psrt_stream_reader reused=psrt_stream_reader_make(test_domain(&domain),store,path,site);
    CHECK(reused.owner->handle.slot==reader.owner->handle.slot && reused.owner->handle.generation!=reader.owner->handle.generation);
    CHECK(!psrt_stream_reader_open(reader,site) && psrt_stream_reader_next_snapshot(reused,site));
    ps_snapshot snapshot=psrt_stream_reader_snapshot(reused,site);
    CHECK(snapshot.scene.count==1 && snapshot.scene.objects[0].id==42 && snapshot.scene.objects[0].a.x==0);
    CHECK(psrt_stream_reader_next_snapshot(reused,site) && snapshot.scene.objects[0].a.x==0);
    psrt_stream_reader recovered=psrt_stream_reader_make(test_domain(&domain),store,partial,site);
    CHECK(psrt_stream_reader_next(recovered,site) && !psrt_stream_reader_next(recovered,site));
    CHECK(psrt_stream_reader_recovered(recovered,site) && !psrt_stream_reader_complete(recovered,site));
    psrt_stream_store copied;CHECK(psrt_stream_store_copy(&copied,&store)==PS_OK);
    psrt_stream_store untouched={0};size_t store_refs=store.owner->references;CHECK(store_refs==5);store.owner->references=SIZE_MAX;
    CHECK(psrt_stream_store_copy(&untouched,&store)==PS_LIMIT && !untouched.owner);
    store.owner->references=store_refs;
    size_t refs=reader.owner->references;reader.owner->references=SIZE_MAX;
    psrt_stream_reader output={0};CHECK(psrt_stream_reader_copy(&output,&reader)==PS_LIMIT && !output.owner);
    reader.owner->references=refs;
    psrt_stream_reader capacity[6];
    for(unsigned i=0;i<6;i++)capacity[i]=psrt_stream_reader_make(test_domain(&domain),store,path,site);
    size_t full=domain.live_bytes;begin();
    if(!setjmp(trap.jump)){(void)psrt_stream_reader_make(test_domain(&domain),store,path,site);CHECK(false);}
    CHECK(trap.failure_code==PS_LIMIT && domain.live_bytes==full);
    ps_run_read_handle retired=capacity[0].owner->handle;
    for(unsigned i=0;i<6;i++)psrt_stream_reader_drop(&capacity[i]);
    begin();CHECK(!setjmp(trap.jump));
    psrt_stream_reader replacement=psrt_stream_reader_make(test_domain(&domain),store,path,site);
    CHECK(replacement.owner->handle.slot==retired.slot && replacement.owner->handle.generation!=retired.generation);
    ps_run_read_info invalid;
    CHECK(ps_run_reader_describe(store.owner->store,retired,&invalid)==PS_INVALID);
    psrt_stream_reader_drop(&replacement);
    psrt_stream_store_close(copied,site);
    CHECK(!psrt_stream_store_open(store,site) && !psrt_stream_reader_open(reused,site));
    CHECK(psrt_run_block_value(row,0,0,site)==10 && snapshot.scene.objects[0].a.x==0);
    psrt_run_block_drop(&row);psrt_stream_reader_drop(&alias);psrt_stream_reader_drop(&reader);
    psrt_stream_reader_drop(&reused);psrt_stream_reader_drop(&recovered);psrt_stream_store_drop(&copied);psrt_stream_store_drop(&store);
    psrt_current=NULL;CHECK(!domain.live_blocks && !domain.live_bytes && !domain.invalid);
    puts("Language streams: allocator failures, shared cursors, stale aliases, snapshots, terminal states and cleanup passed");return 0;
}
