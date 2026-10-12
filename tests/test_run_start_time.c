#include "physim/run_stream.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Start time %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char **argv) {
 CHECK(argc==2);ps_run_start_time value,sentinel;memset(&sentinel,0x5a,sizeof sentinel);value=sentinel;
 CHECK(ps_run_start_time_make(0,1000000000,&value)==PS_INVALID && !memcmp(&value,&sentinel,sizeof value));
 CHECK(ps_run_start_time_make(INT64_MIN,999999999,&value)==PS_OK && value.unix_seconds==INT64_MIN && value.nanoseconds==999999999);
 const char *good="start_unix_seconds=-9223372036854775808\r\nstart_nanoseconds=999999999";
 CHECK(ps_run_start_time_read(good,strlen(good),&value)==PS_OK && value.unix_seconds==INT64_MIN && value.nanoseconds==999999999);
 const char *bad[]={"start_unix_seconds=+9223372036854775808\nstart_nanoseconds=000000000\n", "start_unix_seconds=+0000000000000000000\n", "start_nanoseconds=1000000000\n", "start_unix_seconds=0\nstart_nanoseconds=000000000\n", "start_unix_seconds=+0000000000000000000\nstart_nanoseconds=000000000\nstart_nanoseconds=000000001\n"};
 for(unsigned i=0;i<sizeof bad/sizeof *bad;i++){value=sentinel;CHECK(ps_run_start_time_read(bad[i],strlen(bad[i]),&value)==PS_CORRUPT && !memcmp(&value,&sentinel,sizeof value));}
 value=sentinel;CHECK(ps_run_start_time_read(NULL,0,&value)==PS_EOF && !memcmp(&value,&sentinel,sizeof value));
 ps_run_start_time before,after,stored;CHECK(ps_run_start_time_now(&before)==PS_OK && ps_run_start_time_validate(&before)==PS_OK);
 char path[4096];snprintf(path,sizeof path,"%s/start-c.psrun",argv[1]);ps_context c={0};c.struct_size=sizeof c;c.dt_s=.125;c.seed=42;c.channel_count=1;strcpy(c.channels[0].name,"value");strcpy(c.channels[0].unit,"m");c.channels[0].dimension[0]=1;
 ps_run_store *store;CHECK(ps_run_store_create(ps_allocator_default(),&store)==PS_OK);ps_run_write_handle w;
 CHECK(ps_run_writer_create(store,path,&c,"start fixture",&w)==PS_OK);double sample=7;CHECK(ps_run_writer_append(store,w,0,&sample,1)==PS_OK && ps_run_writer_release(store,w)==PS_OK);
 CHECK(ps_run_start_time_now(&after)==PS_OK);ps_run_read_handle r;CHECK(ps_run_reader_open(store,path,&r)==PS_OK);
 CHECK(ps_run_reader_start_time(store,r,&stored)==PS_OK && stored.unix_seconds>=before.unix_seconds-1 && stored.unix_seconds<=after.unix_seconds+1);
 ps_run_read_info info;CHECK(ps_run_reader_describe(store,r,&info)==PS_OK && info.samples==0);
 double time,values[PS_MAX_CHANNELS];size_t count;CHECK(ps_run_reader_next(store,r,&time,values,PS_MAX_CHANNELS,&count)==PS_OK && time==0 && values[0]==7);
 CHECK(ps_run_reader_release(store,r)==PS_OK);value=sentinel;CHECK(ps_run_reader_start_time(store,r,&value)==PS_INVALID && !memcmp(&value,&sentinel,sizeof value));
 snprintf(path,sizeof path,"%s/override-time.psrun",argv[1]);strcpy(c.model_metadata,"start_nanoseconds=000000000");
 ps_run_write_handle sentinel_handle={store,7,999};w=sentinel_handle;
 CHECK(ps_run_writer_create(store,path,&c,"start fixture",&w)==PS_INVALID && w.slot==sentinel_handle.slot && w.generation==sentinel_handle.generation);
 FILE *file=fopen(path,"rb");CHECK(!file);
 CHECK(ps_run_store_destroy(store)==PS_OK);
 puts("Start time: signed epoch extremes, strict decoder/duplicates/partial fields, output atomicity, real capture and independent reader cursor passed");return 0;
}
