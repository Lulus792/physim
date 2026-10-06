#ifndef PHYSIM_LANGUAGE_RUN_INDEX_H
#define PHYSIM_LANGUAGE_RUN_INDEX_H
/* Private compiled-language bindings, not a stable module ABI. */
#include "language_string.h"
#include "run_index.h"
typedef struct {
    size_t references;
    ps_allocator allocator;
    ps_run_index *index;
    ps_run_index_info info;
} psrt_run_owner;
typedef struct {psrt_run_owner *owner;} psrt_run_index;
typedef struct {double time,values[PS_MAX_CHANNELS];} psrt_run_row;
typedef struct {psrt_array rows;int64_t channels;} psrt_run_block;
static inline void psrt_run_check(ps_result result,psrt_site site) {
    if(result==PS_EOF)psrt_fail_code(site,PS_INVALID,"Requested range is outside the readable run prefix");
    if(result==PS_RECOVERED)psrt_fail_code(site,PS_CORRUPT,"Run data changed or contains a damaged selected block");
    if(result!=PS_OK)psrt_fail_code(site,result,ps_result_string(result));
}
static inline void psrt_run_index_drop(void *value) {
    psrt_run_index *run=value;
    if(run->owner && !--run->owner->references) {
        psrt_run_owner *owner=run->owner;
        ps_run_index_destroy(owner->index);ps_memory_free(owner->allocator,owner,sizeof *owner);
    }
    run->owner=NULL;
}
static inline ps_result psrt_run_index_copy(void *out,const void *source) {
    psrt_run_index value=*(const psrt_run_index *)source;
    if(value.owner){if(value.owner->references==SIZE_MAX)return PS_LIMIT;value.owner->references++;}
    *(psrt_run_index *)out=value;return PS_OK;
}
static inline void psrt_run_index_keep(psrt_run_index *value,psrt_site site) {
    psrt_run_index copy;psrt_run_check(psrt_run_index_copy(&copy,value),site);*value=copy;
}
static inline const psrt_run_owner *psrt_run_owner_get(psrt_run_index run,psrt_site site) {
    if(!run.owner){psrt_fail_code(site,PS_INVALID,"RunIndex is closed");}return run.owner;
}
static inline psrt_run_index psrt_run_index_open(ps_allocator allocator,const char *path,int64_t maximum,psrt_site site) {
    if(maximum<=0 || (uint64_t)maximum>SIZE_MAX)psrt_fail_code(site,PS_INVALID,"RunIndex maximumEntries must be positive");
    ps_run_index *index=NULL;ps_result result=ps_run_index_open(path,allocator,(size_t)maximum,&index);
    if(result!=PS_OK && result!=PS_RECOVERED)psrt_run_check(result,site);
    psrt_run_owner *owner=NULL;result=ps_memory_zero(allocator,1,sizeof *owner,(void **)&owner);
    if(result!=PS_OK){ps_run_index_destroy(index);psrt_run_check(result,site);}
    owner->allocator=allocator;owner->index=index;owner->references=1;
    owner->info.struct_size=sizeof owner->info;owner->info.version=PS_RUN_INDEX_VERSION;
    result=ps_run_index_get_info(index,&owner->info);
    if(result!=PS_OK || owner->info.samples>INT64_MAX || owner->info.snapshots>INT64_MAX || owner->info.checkpoints>INT64_MAX) {
        ps_run_index_destroy(index);ps_memory_free(allocator,owner,sizeof *owner);
        psrt_run_check(result==PS_OK?PS_LIMIT:result,site);
    }
    return (psrt_run_index){owner};
}
static inline void psrt_run_index_close(psrt_run_index *run,psrt_site site){(void)site;psrt_run_index_drop(run);}
static inline bool psrt_run_index_is_open(psrt_run_index run,psrt_site site){(void)site;return run.owner!=NULL;}
#define PSRT_RUN_INFO(name,field,ctype) static inline ctype psrt_run_index_##name(psrt_run_index run,psrt_site site){return (ctype)psrt_run_owner_get(run,site)->info.field;}
PSRT_RUN_INFO(samples,samples,int64_t)
PSRT_RUN_INFO(snapshots,snapshots,int64_t)
PSRT_RUN_INFO(checkpoints,checkpoints,int64_t)
PSRT_RUN_INFO(channels,channels,int64_t)
PSRT_RUN_INFO(complete,complete,bool)
PSRT_RUN_INFO(persisted,persisted,bool)
#undef PSRT_RUN_INFO
static inline const ps_channel *psrt_run_channel(psrt_run_index run,int64_t channel,psrt_site site) {
    const psrt_run_owner *owner=psrt_run_owner_get(run,site);
    if(channel<0 || (uint64_t)channel>=owner->info.channels)psrt_fail_code(site,PS_INVALID,"RunIndex channel out of bounds");
    return &owner->info.schema[channel];
}
static inline psrt_string psrt_run_string(ps_allocator allocator,const char *text,psrt_site site) {
    psrt_string value;psrt_run_check(psrt_string_make(allocator,text,strlen(text),&value),site);return value;
}
static inline psrt_string psrt_run_index_metadata(ps_allocator a,psrt_run_index run,psrt_site s){return psrt_run_string(a,psrt_run_owner_get(run,s)->info.metadata,s);}
#define PSRT_RUN_CHANNEL_TEXT(name,field) static inline psrt_string psrt_run_index_##name(ps_allocator a,psrt_run_index run,int64_t channel,psrt_site s){return psrt_run_string(a,psrt_run_channel(run,channel,s)->field,s);}
PSRT_RUN_CHANNEL_TEXT(name,name)
PSRT_RUN_CHANNEL_TEXT(symbol,unit)
PSRT_RUN_CHANNEL_TEXT(description,description)
#undef PSRT_RUN_CHANNEL_TEXT
static inline int64_t psrt_run_index_dimension(psrt_run_index run,int64_t channel,int64_t axis,psrt_site s) {
    if(axis<0 || axis>=7){psrt_fail_code(s,PS_INVALID,"SI dimension index out of bounds");}return psrt_run_channel(run,channel,s)->dimension[axis];
}
static inline void psrt_run_block_drop(void *value){psrt_run_block *block=value;psrt_array_destroy(&block->rows);memset(block,0,sizeof *block);}
static inline ps_result psrt_run_block_copy(void *out,const void *source){const psrt_run_block *s=source;psrt_run_block value=*s;ps_result r=psrt_array_clone(&s->rows,&value.rows);if(r==PS_OK)*(psrt_run_block *)out=value;return r;}
static inline void psrt_run_block_keep(psrt_run_block *value,psrt_site s){psrt_run_block copy;psrt_run_check(psrt_run_block_copy(&copy,value),s);*value=copy;}
static inline psrt_run_block psrt_run_index_read(ps_allocator a,psrt_run_index run,int64_t first,int64_t count,psrt_site s) {
    const psrt_run_owner *owner=psrt_run_owner_get(run,s);
    if(first<0 || count<0)psrt_fail_code(s,PS_INVALID,"RunIndex row and count must be nonnegative");
    if(count>PS_RUN_INDEX_BLOCK)psrt_fail_code(s,PS_LIMIT,"RunIndex reads at most 256 rows");
    double times[PS_RUN_INDEX_BLOCK],values[PS_RUN_INDEX_BLOCK*PS_MAX_CHANNELS];
    psrt_run_check(ps_run_index_read(owner->index,(uint64_t)first,(size_t)count,times,values),s);
    psrt_run_row rows[PS_RUN_INDEX_BLOCK];memset(rows,0,sizeof rows);
    for(int64_t i=0;i<count;i++){rows[i].time=times[i];memcpy(rows[i].values,values+i*owner->info.channels,owner->info.channels*sizeof(double));}
    static const psrt_element_type element={sizeof(psrt_run_row),NULL,NULL};psrt_run_block block={0};block.channels=owner->info.channels;
    psrt_run_check(psrt_array_init(&element,a,PS_RUN_INDEX_BLOCK,&block.rows),s);
    ps_result result=psrt_array_replace(&block.rows,0,0,rows,(size_t)count);
    if(result!=PS_OK){psrt_run_block_drop(&block);psrt_run_check(result,s);}return block;
}
static inline int64_t psrt_run_block_count(psrt_run_block block,psrt_site s){(void)s;return (int64_t)psrt_array_count(&block.rows);}
static inline int64_t psrt_run_block_channels(psrt_run_block block,psrt_site s){(void)s;return block.channels;}
static inline const psrt_run_row *psrt_run_block_row(psrt_run_block block,int64_t row,psrt_site s){if(row<0 || (uint64_t)row>=psrt_array_count(&block.rows))psrt_fail_code(s,PS_INVALID,"RunBlock row out of bounds");return (const psrt_run_row *)psrt_array_data(&block.rows)+row;}
static inline double psrt_run_block_time(psrt_run_block block,int64_t row,psrt_site s){return psrt_run_block_row(block,row,s)->time;}
static inline double psrt_run_block_value(psrt_run_block block,int64_t row,int64_t channel,psrt_site s){if(channel<0 || channel>=block.channels)psrt_fail_code(s,PS_INVALID,"RunBlock channel out of bounds");return psrt_run_block_row(block,row,s)->values[channel];}
static inline psrt_array psrt_run_block_column_impl(ps_allocator a,psrt_run_block block,int64_t channel,psrt_site s) {
    if(channel< -1 || channel>=block.channels)psrt_fail_code(s,PS_INVALID,"RunBlock channel out of bounds");
    double values[PS_RUN_INDEX_BLOCK];size_t count=psrt_array_count(&block.rows);const psrt_run_row *rows=psrt_array_data(&block.rows);
    for(size_t i=0;i<count;i++)values[i]=channel<0?rows[i].time:rows[i].values[channel];
    static const psrt_element_type element={sizeof(double),NULL,NULL};psrt_array result;
    psrt_run_check(psrt_array_init(&element,a,PS_RUN_INDEX_BLOCK,&result),s);
    ps_result status=psrt_array_replace(&result,0,0,values,count);
    if(status!=PS_OK){psrt_array_destroy(&result);psrt_run_check(status,s);}return result;
}
static inline psrt_array psrt_run_block_column(ps_allocator a,psrt_run_block block,int64_t channel,psrt_site s){if(channel<0)psrt_fail_code(s,PS_INVALID,"RunBlock channel out of bounds");return psrt_run_block_column_impl(a,block,channel,s);}
static inline psrt_array psrt_run_block_times(ps_allocator a,psrt_run_block b,psrt_site s){return psrt_run_block_column_impl(a,b,-1,s);}
static inline ps_snapshot psrt_run_index_snapshot(psrt_run_index run,int64_t ordinal,psrt_site s) {
    if(ordinal<0)psrt_fail_code(s,PS_INVALID,"RunIndex scene number must be nonnegative");
    ps_snapshot value;psrt_run_check(ps_run_index_snapshot(psrt_run_owner_get(run,s)->index,(uint64_t)ordinal,&value),s);return value;
}
static inline double psrt_run_snapshot_time(ps_snapshot value,psrt_site s){(void)s;return value.time;}
static inline bool psrt_run_snapshot_paused(ps_snapshot value,psrt_site s){(void)s;return value.paused;}
static inline int64_t psrt_run_snapshot_channels(ps_snapshot value,psrt_site s){(void)s;return value.count;}
static inline int64_t psrt_run_snapshot_objects(ps_snapshot value,psrt_site s){(void)s;return value.scene.count;}
static inline int64_t psrt_run_snapshot_points(ps_snapshot value,psrt_site s){(void)s;return value.scene.point_count;}
static inline double psrt_run_snapshot_value(ps_snapshot value,int64_t channel,psrt_site s){if(channel<0 || (uint64_t)channel>=value.count)psrt_fail_code(s,PS_INVALID,"RunSnapshot channel out of bounds");return value.values[channel];}
static inline const ps_object *psrt_run_snapshot_object(const ps_snapshot *value,int64_t index,psrt_site s){if(index<0 || (uint64_t)index>=value->scene.count)psrt_fail_code(s,PS_INVALID,"RunSnapshot object out of bounds");return &value->scene.objects[index];}
#define PSRT_RUN_OBJECT(name,field,ctype) static inline ctype psrt_run_snapshot_##name(ps_snapshot value,int64_t index,psrt_site s){return psrt_run_snapshot_object(&value,index,s)->field;}
PSRT_RUN_OBJECT(id,id,int64_t)
PSRT_RUN_OBJECT(parent,parent_id,int64_t)
PSRT_RUN_OBJECT(shape,shape,int64_t)
PSRT_RUN_OBJECT(color,color,int64_t)
PSRT_RUN_OBJECT(position,a,ps_vec3)
PSRT_RUN_OBJECT(size,b,ps_vec3)
PSRT_RUN_OBJECT(radius,radius,double)
PSRT_RUN_OBJECT(rotation,orientation,ps_quat)
PSRT_RUN_OBJECT(point_first,point_first,int64_t)
PSRT_RUN_OBJECT(point_count,point_count,int64_t)
#undef PSRT_RUN_OBJECT
static inline psrt_string psrt_run_snapshot_text(ps_allocator a,ps_snapshot value,int64_t index,psrt_site s){return psrt_run_string(a,psrt_run_snapshot_object(&value,index,s)->text,s);}
static inline ps_vec3 psrt_run_snapshot_point(ps_snapshot value,int64_t index,psrt_site s){if(index<0 || (uint64_t)index>=value.scene.point_count)psrt_fail_code(s,PS_INVALID,"RunSnapshot point out of bounds");return value.scene.points[index];}
static inline ps_vec3 psrt_run_snapshot_world_point(ps_snapshot value,int64_t id,ps_vec3 local,psrt_site s){if(id<0 || (uint64_t)id>=value.scene.count)psrt_fail_code(s,PS_INVALID,"Scene object index out of bounds");ps_vec3 out;psrt_run_check(ps_scene_world_point(&value.scene,(uint32_t)id,local,&out),s);return out;}
static inline ps_mat4 psrt_run_snapshot_transform(ps_snapshot value,int64_t id,psrt_site s){if(id<0 || (uint64_t)id>=value.scene.count)psrt_fail_code(s,PS_INVALID,"Scene object index out of bounds");ps_mat4 out[PS_MAX_OBJECTS];psrt_run_check(ps_scene_transforms(&value.scene,out),s);return out[id];}
#endif
