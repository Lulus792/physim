#include "physim/run_stream.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"events %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char **argv) {
    CHECK(argc==2);char path[4096];snprintf(path,sizeof path,"%s/events-c.psrun",argv[1]);
    ps_context c={0};c.struct_size=sizeof c;c.dt_s=.125;c.seed=42;c.channel_count=1;
    strcpy(c.channels[0].name,"value");strcpy(c.channels[0].unit,"m");c.channels[0].dimension[0]=1;
    ps_run_store *store,*foreign;CHECK(ps_run_store_create(ps_allocator_default(),&store)==PS_OK);
    CHECK(ps_run_store_create(ps_allocator_default(),&foreign)==PS_OK);
    ps_run_write_handle w;CHECK(ps_run_writer_create(store,path,&c,"events",&w)==PS_OK);
    ps_log_record event={.level=PS_LOG_WARNING,.time_s=.125};strcpy(event.message,"warning α\nline\t☃");
    CHECK(ps_run_writer_event(foreign,w,&event)==PS_INVALID);
    event.time_s=NAN;CHECK(ps_run_writer_event(store,w,&event)==PS_INVALID);event.time_s=.125;
    event.level=0;CHECK(ps_run_writer_event(store,w,&event)==PS_INVALID);event.level=PS_LOG_WARNING;
    char valid[sizeof event.message];memcpy(valid,event.message,sizeof valid);
    memset(event.message,'x',sizeof event.message);CHECK(ps_run_writer_event(store,w,&event)==PS_INVALID);
    memcpy(event.message,valid,sizeof valid);CHECK(ps_run_writer_event(store,w,&event)==PS_OK);
    double value=7;CHECK(ps_run_writer_append(store,w,0,&value,1)==PS_OK);
    event.level=PS_LOG_ERROR;event.time_s=.25;strcpy(event.message,"finished");
    CHECK(ps_run_writer_event(store,w,&event)==PS_OK);
    ps_run_write_info info;CHECK(ps_run_writer_describe(store,w,&info)==PS_OK && info.samples==1);
    CHECK(ps_run_writer_release(store,w)==PS_OK && ps_run_writer_event(store,w,&event)==PS_INVALID);
    ps_run_read_handle r;CHECK(ps_run_reader_open(store,path,&r)==PS_OK);
    ps_log_record out;CHECK(ps_run_reader_event_next(foreign,r,&out)==PS_INVALID);
    CHECK(ps_run_reader_event_next(store,r,&out)==PS_OK && out.level==3 && out.time_s==.125 && !strcmp(out.message,"warning α\nline\t☃"));
    CHECK(ps_run_reader_event_next(store,r,&out)==PS_OK && out.level==4 && !strcmp(out.message,"finished"));
    ps_log_record saved=out;CHECK(ps_run_reader_event_next(store,r,&out)==PS_EOF && !memcmp(&out,&saved,sizeof out));
    CHECK(ps_run_reader_event_next(store,r,&out)==PS_EOF);
    ps_run_read_info ri;CHECK(ps_run_reader_describe(store,r,&ri)==PS_OK && ri.samples==1 && ri.complete);
    CHECK(ps_run_reader_release(store,r)==PS_OK);
    CHECK(ps_run_store_destroy(store)==PS_OK && ps_run_store_destroy(foreign)==PS_OK);
    /* A low-level reader probe supplies statuses for independently mutated files. */
    puts("Run events: validated writes, counts, handle ownership/generation and terminal output preservation passed");return 0;
}
