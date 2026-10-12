#include "physim/run_stream.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Typed identity %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char **argv) {
 CHECK(argc==2);const char *project="0123456789abcdef0123456789abcdef",*experiment="fedcba9876543210fedcba9876543210";
 ps_run_identity id,sentinel;memset(&sentinel,0x5a,sizeof sentinel);id=sentinel;
 CHECK(ps_run_identity_make("bad",experiment,&id)==PS_INVALID && !memcmp(&id,&sentinel,sizeof id));
 CHECK(ps_run_identity_make(project,experiment,&id)==PS_OK && ps_run_identity_validate(&id)==PS_OK);
 CHECK(ps_run_identity_make(id.project_id,id.experiment_id,&id)==PS_OK && !strcmp(id.project_id,project));
 ps_run_identity malformed=id;malformed.project_id[32]='x';CHECK(ps_run_identity_validate(&malformed)==PS_INVALID);
 const char *bad[]={"project_id=bad\nexperiment_id=fedcba9876543210fedcba9876543210\n", "project_id=0123456789abcdef0123456789abcdef\nproject_id=0123456789abcdef0123456789abcdef\n", "project_id=0123456789ABCDEF0123456789ABCDEF\n", "experiment_id=\n"};
 for(unsigned i=0;i<sizeof bad/sizeof *bad;i++){ps_run_identity out=sentinel;CHECK(ps_run_identity_read(bad[i],strlen(bad[i]),&out)==PS_CORRUPT && !memcmp(&out,&sentinel,sizeof out));}
 ps_run_identity out=sentinel;CHECK(ps_run_identity_read(NULL,0,&out)==PS_EOF && !memcmp(&out,&sentinel,sizeof out));
 CHECK(ps_run_identity_read("project_id=unavailable\nexperiment_id=unavailable\n",49,&out)==PS_EOF);
 char text[160];snprintf(text,sizeof text,"project_id=%s\r\nexperiment_id=%s",project,experiment);
 CHECK(ps_run_identity_read(text,strlen(text),&out)==PS_OK && !memcmp(&out,&id,sizeof id));
 char embedded[]={0,'x'};out=sentinel;CHECK(ps_run_identity_read(embedded,sizeof embedded,&out)==PS_CORRUPT && !memcmp(&out,&sentinel,sizeof out));
 char path[4096];snprintf(path,sizeof path,"%s/typed-c.psrun",argv[1]);
 ps_context c={0};c.struct_size=sizeof c;c.dt_s=.125;c.seed=42;c.channel_count=1;strcpy(c.model_metadata,"source=typed");strcpy(c.channels[0].name,"value");strcpy(c.channels[0].unit,"m");c.channels[0].dimension[0]=1;
 ps_run_writer writer,saved;memset(&saved,0x5a,sizeof saved);writer=saved;
 CHECK(ps_run_create_with_identity(&writer,path,&c,"typed identity",&malformed)==PS_INVALID && !memcmp(&writer,&saved,sizeof writer));
 CHECK(ps_run_create_with_identity(&writer,path,&c,"typed identity",&id)==PS_OK);double value=7;CHECK(ps_run_append(&writer,0,&value)==PS_OK && ps_run_close(&writer)==PS_OK);
 writer=saved;CHECK(ps_run_create_with_identity(&writer,path,&c,"typed identity",&id)==PS_IO && !memcmp(&writer,&saved,sizeof writer));
 ps_run_store *store;CHECK(ps_run_store_create(ps_allocator_default(),&store)==PS_OK);ps_run_read_handle reader;
 CHECK(ps_run_reader_open(store,path,&reader)==PS_OK && ps_run_reader_identity(store,reader,&out)==PS_OK && !memcmp(&out,&id,sizeof id));
 CHECK(ps_run_reader_release(store,reader)==PS_OK);out=sentinel;CHECK(ps_run_reader_identity(store,reader,&out)==PS_INVALID && !memcmp(&out,&sentinel,sizeof out));
 snprintf(path,sizeof path,"%s/typed-handle-c.psrun",argv[1]);ps_run_write_handle h,hs={store,7,999};h=hs;
 strcpy(c.model_metadata,"source=typed\rproject_id=forged");CHECK(ps_run_writer_create_with_identity(store,path,&c,"typed identity",&id,&h)==PS_INVALID && h.slot==hs.slot && h.generation==hs.generation);
 FILE *file=fopen(path,"rb");CHECK(!file);strcpy(c.model_metadata,"source=typed");
 CHECK(ps_run_writer_create_with_identity(store,path,&c,"typed identity",&id,&h)==PS_OK && ps_run_writer_append(store,h,0,&value,1)==PS_OK && ps_run_writer_release(store,h)==PS_OK);
 CHECK(ps_run_store_destroy(store)==PS_OK);puts("Typed identities: validated/aliased construction, strict bounded decoding, C and handle writers, collisions and failure/output atomicity passed");return 0;
}
