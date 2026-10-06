#include "physim/run_index.h"
#include "physim/units.h"
#include "test_allocator.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Run index line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static ps_context context;
static ps_scene scene;
static bool copy_variant(const char *source,const char *target,unsigned mode) {
    FILE *in=fopen(source,"rb"),*out=fopen(target,"wbx");if(!in || !out){if(in)fclose(in);if(out)fclose(out);return false;}
    unsigned char header[16],data[8192];bool ok=fread(header,1,16,in)==16 && fwrite(header,1,16,out)==16,changed=false;
    while(ok && fread(header,1,12,in)==12) {
        uint32_t type=ps_get_u32(header),size=ps_get_u32(header+4);if(size>sizeof data || fread(data,1,size,in)!=size){ok=false;break;}
        if(mode==1 && (type==6 || type==7))continue; /* Legacy finalized run. */
        if(mode==2 && type==6)break; /* Crash before the index/footer. */
        if(mode==3 && type==6 && !changed){data[24]^=1;ps_put_u32(header+8,ps_crc32(data,size));changed=true;}
        if(mode==4 && type==6 && !changed){data[size-1]^=1;changed=true;}
        if(mode==5 && type==7){ps_put_u32(data,2);ps_put_u32(header+8,ps_crc32(data,size));}
        ok=fwrite(header,1,12,out)==12 && fwrite(data,1,size,out)==size;
    }
    if(ferror(in))ok=false;
    fclose(in);
    if(fclose(out))ok=false;
    return ok;
}
static double expected_time(uint64_t i){return i==777?-3:(double)i*.001;}
static bool damage_selected(const char *path,uint32_t selected) {
    FILE *file=fopen(path,"r+b");if(!file)return false;bool ok=fseek(file,16,SEEK_SET)==0,changed=false;
    unsigned char header[12],data[8192];
    while(ok && !changed) {
        long offset=ftell(file);ok=offset>=0 && fread(header,1,12,file)==12;if(!ok)break;
        uint32_t type=ps_get_u32(header),size=ps_get_u32(header+4);ok=size<=sizeof data && fread(data,1,size,file)==size;
        if(ok && type==selected) {
            if(type==3)header[8]^=1;
            else {ps_put_u32(data,99);ps_put_u32(header+8,ps_crc32(data,size));}
            ok=fseek(file,offset,SEEK_SET)==0 && fwrite(header,1,12,file)==12 && fwrite(data,1,size,file)==size;changed=true;
        }
    }
    if(fclose(file))ok=false;
    return ok && changed;
}
int main(int argc,char **argv) {
    CHECK(argc==2);char path[4096],variant[4096];snprintf(path,sizeof path,"%s/index α.psrun",argv[1]);
    context.struct_size=sizeof context;context.api_version=PS_API_VERSION;context.dt_s=.001;
    CHECK(ps_channel_add(&context,"position",PS_METRE,"x") == 0 && ps_channel_add(&context,"speed",PS_METRE,"v") == 1);
    CHECK(ps_scene_add_id(&scene,1,PS_SPHERE,ps_v3(0,0,0),ps_v3(0,0,0),.1,UINT32_MAX)==PS_OK);
    ps_run_writer writer;CHECK(ps_run_create(&writer,path,&context,"Indexed α")==PS_OK);
    for(uint64_t i=0;i<1000;i++) {
        context.time_s=expected_time(i);context.values[0]=(double)i;context.values[1]=-(double)i;
        CHECK(ps_run_append(&writer,context.time_s,context.values)==PS_OK);
        if(i==10) {unsigned char unknown[16];ps_put_u32(unknown,99);ps_put_u32(unknown+4,4);ps_put_u32(unknown+12,42);ps_put_u32(unknown+8,ps_crc32(unknown+12,4));CHECK(fwrite(unknown,1,sizeof unknown,writer.file)==sizeof unknown);}
        if(i%3==0){scene.objects[0].a.x=(double)i;CHECK(ps_run_append_snapshot(&writer,&context,&scene,true)==PS_OK);}
    }
    CHECK(ps_run_close(&writer)==PS_OK);
    for(unsigned mode=0;mode<=5;mode++) {
        const char *input=path;if(mode){snprintf(variant,sizeof variant,"%s/variant-%u.psrun",argv[1],mode);CHECK(copy_variant(path,variant,mode));input=variant;}
        ps_run_index *index=NULL;ps_result result=ps_run_index_open(input,ps_allocator_default(),338,&index);
        CHECK(result==(mode==2 || mode==4?PS_RECOVERED:PS_OK) && index);
        ps_run_index_info info={.struct_size=sizeof info,.version=PS_RUN_INDEX_VERSION};CHECK(ps_run_index_get_info(index,&info)==PS_OK && info.samples==1000 && info.snapshots==334 && info.checkpoints==338);
        CHECK(info.complete==(mode!=2 && mode!=4) && info.persisted==(mode==0) && info.channels==2 && strstr(info.metadata,"Indexed α"));
        ps_run_index_info invalid=info;invalid.version++;ps_run_index_info unchanged=invalid;
        CHECK(ps_run_index_get_info(index,&invalid)==PS_VERSION && !memcmp(&invalid,&unchanged,sizeof invalid));
        invalid=info;invalid.struct_size=8;unchanged=invalid;
        CHECK(ps_run_index_get_info(index,&invalid)==PS_VERSION && !memcmp(&invalid,&unchanged,sizeof invalid));
        double times[256],values[512];
        for(unsigned first=0;first<1000;first+=113) {
            size_t count=1000-first<256?1000-first:256;
            CHECK(ps_run_index_read(index,first,count,times,values)==PS_OK);
            for(size_t j=0;j<count;j++)CHECK(times[j]==expected_time(first+j) && values[2*j]==first+j && values[2*j+1]==-(double)(first+j));
        }
        times[0]=123;values[0]=456;
        CHECK(ps_run_index_read(index,1000,1,times,values)==PS_EOF && times[0]==123 && values[0]==456);
        CHECK(ps_run_index_read(index,0,257,times,values)==PS_LIMIT && times[0]==123 && values[0]==456);
        CHECK(ps_run_index_read(index,UINT64_MAX,1,times,values)==PS_EOF && ps_run_index_read(index,1000,0,NULL,NULL)==PS_OK);
        ps_snapshot snapshot={0},before=snapshot;
        CHECK(ps_run_index_snapshot(index,334,&snapshot)==PS_EOF && !memcmp(&snapshot,&before,sizeof snapshot));
        for(uint64_t i=0;i<334;i+=29)CHECK(ps_run_index_snapshot(index,i,&snapshot)==PS_OK && snapshot.values[0]==i*3 && snapshot.scene.objects[0].a.x==i*3);
        ps_run_index_destroy(index);
    }
    test_allocator allocator={0};ps_run_index *index=NULL;
    CHECK(ps_run_index_open(path,test_domain(&allocator),338,&index)==PS_OK);size_t attempts=allocator.attempts;
    ps_run_index_destroy(index);CHECK(!allocator.invalid && !allocator.live_blocks && !allocator.live_bytes);
    for(size_t failure=1;failure<=attempts;failure++) {
        memset(&allocator,0,sizeof allocator);allocator.fail_on=failure;index=(ps_run_index *)(uintptr_t)1;
        CHECK(ps_run_index_open(path,test_domain(&allocator),338,&index)==PS_MEMORY && index==(ps_run_index *)(uintptr_t)1);
        CHECK(!allocator.invalid && !allocator.live_blocks && !allocator.live_bytes);
    }
    memset(&allocator,0,sizeof allocator);index=(ps_run_index *)(uintptr_t)1;
    CHECK(ps_run_index_open(path,test_domain(&allocator),1,&index)==PS_LIMIT && index==(ps_run_index *)(uintptr_t)1 && !allocator.live_blocks && !allocator.invalid);
    CHECK(ps_run_index_open(path,test_domain(&allocator),SIZE_MAX,&index)==PS_LIMIT && !allocator.live_blocks);
    snprintf(variant,sizeof variant,"%s/selected-damage.psrun",argv[1]);CHECK(copy_variant(path,variant,0));
    CHECK(ps_run_index_open(variant,ps_allocator_default(),338,&index)==PS_OK);
    CHECK(damage_selected(variant,3));double changed_time=123,changed_values[2]={456,789};
    CHECK(ps_run_index_read(index,0,1,&changed_time,changed_values)==PS_RECOVERED && changed_time==123 && changed_values[0]==456 && changed_values[1]==789);
    CHECK(ps_run_index_read(index,256,1,&changed_time,changed_values)==PS_OK && changed_values[0]==256);
    ps_snapshot damaged={0},saved=damaged;CHECK(damage_selected(variant,5));
    CHECK(ps_run_index_snapshot(index,0,&damaged)==PS_VERSION && !memcmp(&damaged,&saved,sizeof damaged));ps_run_index_destroy(index);
    /* Raw canonical chunks avoid a million per-row flushes in the stress fixture.
     * The public close still validates and indexes all one million measurements. */
    snprintf(variant,sizeof variant,"%s/million.psrun",argv[1]);CHECK(ps_run_create(&writer,variant,&context,"Million")==PS_OK);
    unsigned char row[36];ps_put_u32(row,3);ps_put_u32(row+4,24);
    for(uint64_t i=0;i<1000000;i++) {
        ps_put_f64(row+12,(double)i*.001);ps_put_f64(row+20,(double)i);ps_put_f64(row+28,-(double)i);ps_put_u32(row+8,ps_crc32(row+12,24));
        CHECK(fwrite(row,1,sizeof row,writer.file)==sizeof row);writer.samples++;
    }
    CHECK(ps_run_close(&writer)==PS_OK);memset(&allocator,0,sizeof allocator);allocator.budget=600000;
    CHECK(ps_run_index_open(variant,test_domain(&allocator),3907,&index)==PS_OK);
    ps_run_index_info info={.struct_size=sizeof info,.version=PS_RUN_INDEX_VERSION};CHECK(ps_run_index_get_info(index,&info)==PS_OK && info.samples==1000000 && info.checkpoints==3907 && info.persisted);
    double time,value[2];CHECK(ps_run_index_read(index,999999,1,&time,value)==PS_OK && time==999.999 && value[0]==999999 && value[1]==-999999);
    ps_run_index_destroy(index);CHECK(!allocator.live_blocks && !allocator.invalid && allocator.peak_bytes<600000);
    puts("Run index: persisted pages, legacy/recovered rebuilds, random rows/scenes, atomic outputs, allocator failures and one million rows passed");return 0;
}
