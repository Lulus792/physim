#include "physim/run_index.h"
#include "physim/array.h"
#include "run_index_internal.h"
#include <math.h>
#include <string.h>

#define INDEX_PAGE 6u
#define INDEX_ROOT 7u
#define INDEX_ENTRIES 255u
typedef struct {uint32_t kind;uint64_t ordinal,offset;double time;} checkpoint;
struct ps_run_index {
    ps_run_reader reader;
    ps_allocator allocator;
    ps_array samples,scenes;
    size_t maximum;
    ps_run_index_info info;
};
static void put_u64(unsigned char *p,uint64_t v){ps_put_u32(p,(uint32_t)v);ps_put_u32(p+4,(uint32_t)(v>>32));}
static uint64_t get_u64(const unsigned char *p){return (uint64_t)ps_get_u32(p)|((uint64_t)ps_get_u32(p+4)<<32);}
static void encode_entry(unsigned char *p,const checkpoint *e) {
    ps_put_u32(p,e->kind);ps_put_u32(p+4,0);put_u64(p+8,e->ordinal);put_u64(p+16,e->offset);ps_put_f64(p+24,e->time);
}
static bool entry_matches(const unsigned char *p,const checkpoint *e) {
    unsigned char expected[32];encode_entry(expected,e);return !memcmp(expected,p,sizeof expected);
}
static ps_result write_chunk(FILE *file,uint32_t type,const unsigned char *data,uint32_t size) {
    unsigned char header[12];ps_put_u32(header,type);ps_put_u32(header+4,size);ps_put_u32(header+8,ps_crc32(data,size));
    return fwrite(header,1,12,file)==12 && fwrite(data,1,size,file)==size?PS_OK:PS_IO;
}
/* Shared validation for writer finalization, reconstruction and selected reads. */
static ps_result record(uint32_t type,const unsigned char *data,uint32_t size,uint32_t channels,double *time,ps_snapshot *scene) {
    if(type==3) {
        if(size!=8*(channels+1))return PS_CORRUPT;
        for(uint32_t i=0;i<=channels;i++)if(!isfinite(ps_get_f64(data+8*i)))return PS_CORRUPT;
        *time=ps_get_f64(data);return PS_OK;
    }
    if(type==5) {
        if(size<4)return PS_CORRUPT;
        uint32_t version=ps_get_u32(data);
        if(version!=1 && version!=2 && version!=PS_SNAPSHOT_VERSION)return PS_VERSION;
        ps_snapshot value={0};
        if(!ps_snapshot_decode_version(version,data+4,size-4,&value.time,value.values,&value.count,&value.scene,&value.paused) ||
           value.count!=channels)return PS_CORRUPT;
        *time=value.time;if(scene)*scene=value;return PS_OK;
    }
    return PS_OK;
}
static ps_result flush_page(FILE *file,unsigned char *page,uint32_t count,uint64_t *write_at) {
    uint64_t read_at;
    if(!ps_run_position(file,&read_at) || !ps_run_seek(file,*write_at))return PS_IO;
    ps_put_u32(page,PS_RUN_INDEX_VERSION);ps_put_u32(page+4,PS_RUN_INDEX_STRIDE);ps_put_u32(page+8,count);ps_put_u32(page+12,0);
    ps_result result=write_chunk(file,INDEX_PAGE,page,16+32*count);
    if(result!=PS_OK || !ps_run_position(file,write_at) || !ps_run_seek(file,read_at))return PS_IO;
    return PS_OK;
}
ps_result ps_run_write_index(FILE *file,uint32_t channels,uint64_t samples) {
    uint64_t end,write_at;
    if(fflush(file) || !ps_run_position(file,&end) || !ps_run_seek(file,16))return PS_IO;
    write_at=end;
    unsigned char data[8192],page[16+32*INDEX_ENTRIES];uint32_t type,size,count=0;
    if(ps_run_chunk_read(file,&type,data,&size)!=PS_OK || type!=1 ||
       ps_run_chunk_read(file,&type,data,&size)!=PS_OK || type!=2 || size!=4+167*channels || ps_get_u32(data)!=channels)return PS_CORRUPT;
    uint64_t rows=0,scenes=0,pages=0;
    for(;;) {
        uint64_t offset,after;
        if(!ps_run_position(file,&offset))return PS_IO;
        if(offset==end)break;
        if(offset>end)return PS_CORRUPT;
        ps_result result=ps_run_chunk_read(file,&type,data,&size);double time=0;
        if(result!=PS_OK)return result;
        if(!ps_run_position(file,&after) || after>end || type==1 || type==2 || type==4 || type==INDEX_PAGE || type==INDEX_ROOT)return PS_CORRUPT;
        result=record(type,data,size,channels,&time,NULL);if(result!=PS_OK)return result;
        if(type==5 || (type==3 && rows%PS_RUN_INDEX_STRIDE==0)) {
            checkpoint entry={type,type==3?rows:scenes,offset,time};encode_entry(page+16+32*count,&entry);count++;
            if(count==INDEX_ENTRIES){result=flush_page(file,page,count,&write_at);if(result!=PS_OK)return result;pages++;count=0;}
        }
        if(type==3){if(rows==UINT64_MAX)return PS_LIMIT;rows++;}
        if(type==5){if(scenes==UINT64_MAX)return PS_LIMIT;scenes++;}
    }
    if(rows!=samples)return PS_CORRUPT;
    if(count){ps_result result=flush_page(file,page,count,&write_at);if(result!=PS_OK)return result;pages++;}
    if(!ps_run_seek(file,write_at))return PS_IO;
    unsigned char root[40];ps_put_u32(root,PS_RUN_INDEX_VERSION);ps_put_u32(root+4,PS_RUN_INDEX_STRIDE);
    put_u64(root+8,end);put_u64(root+16,pages);put_u64(root+24,rows);put_u64(root+32,scenes);
    return write_chunk(file,INDEX_ROOT,root,sizeof root);
}
static ps_result append_checkpoint(ps_run_index *index,uint32_t type,uint64_t ordinal,uint64_t offset,double time) {
    if(index->samples.count>=index->maximum-index->scenes.count)return PS_LIMIT;
    checkpoint value={type,ordinal,offset,time};
    return ps_array_append(type==3?&index->samples:&index->scenes,&value,1);
}
static const checkpoint *next_checkpoint(ps_run_index *index,size_t *sample,size_t *scene) {
    const checkpoint *a=*sample<index->samples.count?(checkpoint *)index->samples.data+*sample:NULL;
    const checkpoint *b=*scene<index->scenes.count?(checkpoint *)index->scenes.data+*scene:NULL;
    if(a && (!b || a->offset<b->offset)){(*sample)++;return a;}
    if(b){(*scene)++;return b;}return NULL;
}
static ps_result scan(ps_run_index *index) {
    uint64_t rows=0,scenes=0,index_start=0,pages=0;
    size_t checked_samples=0,checked_scenes=0;bool valid_index=true,root_seen=false;
    for(;;) {
        uint64_t offset;if(!ps_run_position(index->reader.file,&offset))return PS_IO;
        unsigned char data[8192];uint32_t type,size;
        ps_result result=ps_run_chunk_read(index->reader.file,&type,data,&size);
        if(result!=PS_OK){index->info.samples=rows;index->info.snapshots=scenes;return result==PS_CORRUPT?PS_RECOVERED:result;}
        double time=0;result=record(type,data,size,index->reader.channels,&time,NULL);
        if(result!=PS_OK)return result;
        if(type==3 || type==5) {
            if(pages || root_seen)valid_index=false;
            if(type==5 || rows%PS_RUN_INDEX_STRIDE==0){
                result=append_checkpoint(index,type,type==3?rows:scenes,offset,time);if(result!=PS_OK)return result;
            }
            if(type==3){if(rows==UINT64_MAX)return PS_LIMIT;rows++;}
            else {if(scenes==UINT64_MAX)return PS_LIMIT;scenes++;}
        } else if(type==INDEX_PAGE) {
            if(!pages)index_start=offset;
            pages++;
            uint32_t count=size>=16?ps_get_u32(data+8):0;
            if(root_seen || size<16 || ps_get_u32(data)!=PS_RUN_INDEX_VERSION || ps_get_u32(data+4)!=PS_RUN_INDEX_STRIDE ||
               ps_get_u32(data+12)!=0 || !count || count>INDEX_ENTRIES || size!=16+32*count)valid_index=false;
            else for(uint32_t i=0;i<count;i++) {
                const checkpoint *expected=next_checkpoint(index,&checked_samples,&checked_scenes);
                if(!expected || !entry_matches(data+16+32*i,expected))valid_index=false;
            }
        } else if(type==INDEX_ROOT) {
            if(!pages)index_start=offset;
            if(root_seen || size!=40 || ps_get_u32(data)!=PS_RUN_INDEX_VERSION || ps_get_u32(data+4)!=PS_RUN_INDEX_STRIDE ||
               get_u64(data+8)!=index_start || get_u64(data+16)!=pages || get_u64(data+24)!=rows || get_u64(data+32)!=scenes ||
               checked_samples!=index->samples.count || checked_scenes!=index->scenes.count)valid_index=false;
            root_seen=true;
        } else if(type==4) {
            if(size!=8 || get_u64(data)!=rows)return PS_CORRUPT;
            index->info.samples=rows;index->info.snapshots=scenes;index->info.complete=true;
            index->info.persisted=root_seen && valid_index;return PS_OK;
        } else if(type==1 || type==2)return PS_CORRUPT;
        else if(pages || root_seen)valid_index=false;
    }
}
ps_result ps_run_index_open(const char *path,ps_allocator allocator,size_t maximum,ps_run_index **out) {
    if(!path || !out || !maximum || !ps_allocator_valid(allocator))return PS_INVALID;
    ps_run_index *index=NULL;ps_result result=ps_memory_zero(allocator,1,sizeof *index,(void **)&index);
    if(result!=PS_OK)return result;
    index->allocator=allocator;index->maximum=maximum;
    result=ps_array_init(allocator,sizeof(checkpoint),maximum,&index->samples);
    if(result==PS_OK)result=ps_array_init(allocator,sizeof(checkpoint),maximum,&index->scenes);
    if(result==PS_OK)result=ps_run_open(&index->reader,path);
    if(result==PS_OK) {
        index->info.struct_size=sizeof index->info;index->info.version=PS_RUN_INDEX_VERSION;index->info.channels=index->reader.channels;
        memcpy(index->info.schema,index->reader.schema,sizeof index->info.schema);
        memcpy(index->info.metadata,index->reader.metadata,sizeof index->info.metadata);
        result=scan(index);index->info.checkpoints=index->samples.count+index->scenes.count;
    }
    if(result==PS_OK || result==PS_RECOVERED){*out=index;return result;}
    ps_run_index_destroy(index);return result;
}
void ps_run_index_destroy(ps_run_index *index) {
    if(!index)return;
    ps_run_reader_close(&index->reader);ps_array_destroy(&index->samples);ps_array_destroy(&index->scenes);
    ps_memory_free(index->allocator,index,sizeof *index);
}
ps_result ps_run_index_get_info(const ps_run_index *index,ps_run_index_info *out) {
    if(!index || !out)return PS_INVALID;
    if(out->struct_size<sizeof *out || out->version!=PS_RUN_INDEX_VERSION)return PS_VERSION;
    *out=index->info;return PS_OK;
}
ps_result ps_run_index_read(ps_run_index *index,uint64_t first,size_t count,double *times,double *values) {
    if(!index || (count && (!times || !values)))return PS_INVALID;
    if(count>PS_RUN_INDEX_BLOCK)return PS_LIMIT;
    if(first>index->info.samples || count>index->info.samples-first)return PS_EOF;
    if(!count)return PS_OK;
    size_t slot=(size_t)(first/PS_RUN_INDEX_STRIDE);
    if(slot>=index->samples.count)return PS_CORRUPT;
    const checkpoint *anchor=(checkpoint *)index->samples.data+slot;
    if(!ps_run_seek(index->reader.file,anchor->offset))return PS_IO;
    ps_run_reader reader=index->reader;reader.samples=anchor->ordinal;reader.complete=false;
    double block_times[PS_RUN_INDEX_BLOCK],block_values[PS_RUN_INDEX_BLOCK*PS_MAX_CHANNELS],time,row[PS_MAX_CHANNELS];
    for(uint64_t ordinal=anchor->ordinal;ordinal<first+count;ordinal++) {
        ps_result result;
        if(ordinal==anchor->ordinal) {
            unsigned char data[8192];uint32_t type,size;
            result=ps_run_chunk_read(reader.file,&type,data,&size);if(result!=PS_OK)return result;
            if(type!=3)return PS_CORRUPT;
            result=record(type,data,size,reader.channels,&time,NULL);if(result!=PS_OK)return result;
            if(memcmp(&time,&anchor->time,sizeof time))return PS_CORRUPT;
            for(uint32_t channel=0;channel<reader.channels;channel++)row[channel]=ps_get_f64(data+8*(channel+1));
            reader.samples++;
        } else result=ps_run_next(&reader,&time,row);
        if(result!=PS_OK)return result;
        if(ordinal>=first){size_t at=(size_t)(ordinal-first);block_times[at]=time;memcpy(block_values+at*reader.channels,row,reader.channels*sizeof *row);}
    }
    memcpy(times,block_times,count*sizeof *times);memcpy(values,block_values,count*reader.channels*sizeof *values);return PS_OK;
}
ps_result ps_run_index_snapshot(ps_run_index *index,uint64_t ordinal,ps_snapshot *out) {
    if(!index || !out)return PS_INVALID;
    if(ordinal>=index->scenes.count)return PS_EOF;
    const checkpoint *anchor=(checkpoint *)index->scenes.data+(size_t)ordinal;
    if(!ps_run_seek(index->reader.file,anchor->offset))return PS_IO;
    unsigned char data[8192];uint32_t type,size;double time;
    ps_result result=ps_run_chunk_read(index->reader.file,&type,data,&size);if(result!=PS_OK)return result;
    if(type!=5)return PS_CORRUPT;
    ps_snapshot snapshot;
    result=record(type,data,size,index->reader.channels,&time,&snapshot);if(result!=PS_OK)return result;
    if(memcmp(&time,&anchor->time,sizeof time))return PS_CORRUPT;
    *out=snapshot;return PS_OK;
}
