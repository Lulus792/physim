#ifndef PHYSIM_LANGUAGE_RUN_STREAM_H
#define PHYSIM_LANGUAGE_RUN_STREAM_H
#include "run_stream.h"
#include "language_run_index.h"
/* Reference handles: copies share one store/cursor. Explicit close invalidates
 * all aliases; last-owner cleanup releases files. Reader results are value copies. */
typedef struct {
    size_t references;
    ps_allocator allocator;
    ps_run_store *store;
    ps_context schema;
    bool configured;
} psrt_stream_store_owner;
typedef struct {psrt_stream_store_owner *owner;} psrt_stream_store;
typedef struct {
    size_t references;
    psrt_stream_store_owner *store;
    ps_run_read_handle handle;
    ps_result status;
    unsigned mode;
    bool has_row,has_snapshot;
    psrt_run_row row;
    ps_snapshot snapshot;
} psrt_stream_reader_owner;
typedef struct {psrt_stream_reader_owner *owner;} psrt_stream_reader;
typedef struct {
    size_t references;
    psrt_stream_store_owner *store;
    ps_run_write_handle handle;
} psrt_stream_writer_owner;
typedef struct {psrt_stream_writer_owner *owner;} psrt_stream_writer;
static inline void psrt_stream_store_release(psrt_stream_store_owner *owner) {
    if(owner && !--owner->references) {
        if(owner->store)ps_run_store_destroy(owner->store);
        ps_memory_free(owner->allocator,owner,sizeof *owner);
    }
}
static inline void psrt_stream_store_drop(void *value) {
    psrt_stream_store *store=value;psrt_stream_store_release(store->owner);store->owner=NULL;
}
static inline ps_result psrt_stream_store_copy(void *out,const void *source) {
    psrt_stream_store value=*(const psrt_stream_store *)source;
    if(value.owner){if(value.owner->references==SIZE_MAX)return PS_LIMIT;value.owner->references++;}
    *(psrt_stream_store *)out=value;return PS_OK;
}
static inline void psrt_stream_store_keep(psrt_stream_store *value,psrt_site site) {
    psrt_stream_store copy;ps_result r=psrt_stream_store_copy(&copy,value);
    if(r!=PS_OK)psrt_fail_code(site,r,"RunStore ownership limit exceeded");
    *value=copy;
}
static inline ps_run_store *psrt_stream_store_get(psrt_stream_store value,psrt_site site) {
    if(!value.owner || !value.owner->store)psrt_fail_code(site,PS_INVALID,"RunStore is closed");
    return value.owner->store;
}
static inline psrt_stream_store psrt_stream_store_make(ps_allocator allocator,psrt_site site) {
    psrt_stream_store_owner *owner=NULL;
    ps_result r=ps_memory_zero(allocator,1,sizeof *owner,(void **)&owner);
    if(r!=PS_OK)psrt_fail_code(site,r,"RunStore allocation failed");
    r=ps_run_store_create(allocator,&owner->store);
    if(r!=PS_OK){ps_memory_free(allocator,owner,sizeof *owner);psrt_fail_code(site,r,"RunStore creation failed");}
    owner->references=1;owner->allocator=allocator;
    owner->schema.struct_size=sizeof owner->schema;owner->schema.api_version=PS_API_VERSION;
    return (psrt_stream_store){owner};
}
static inline void psrt_stream_store_close(psrt_stream_store value,psrt_site site) {
    ps_run_store *store=psrt_stream_store_get(value,site);
    value.owner->store=NULL;
    ps_result r=ps_run_store_destroy(store);
    if(r!=PS_OK)psrt_fail_code(site,r,"RunStore finalization failed");
}
static inline bool psrt_stream_store_open(psrt_stream_store value,psrt_site site) {
    (void)site;return value.owner && value.owner->store;
}
static inline void psrt_stream_store_configure(psrt_stream_store value,double dt,int64_t seed,
                                               const char *metadata,psrt_site site) {
    (void)psrt_stream_store_get(value,site);
    if(!isfinite(dt) || dt<=0 || !metadata)psrt_fail_code(site,PS_INVALID,"RunStore requires a positive finite time step and metadata");
    size_t size=strlen(metadata);
    if(size>=sizeof value.owner->schema.model_metadata)psrt_fail_code(site,PS_LIMIT,"RunStore metadata capacity exceeded");
    if(!psrt_string_utf8_valid(metadata,size))psrt_fail_code(site,PS_INVALID,"RunStore metadata must be valid UTF-8");
    /* Inputs may refer to the destination. All validation precedes commit. */
    memmove(value.owner->schema.model_metadata,metadata,size+1);
    value.owner->schema.dt_s=dt;value.owner->schema.seed=(uint64_t)seed;value.owner->configured=true;
}
static inline int64_t psrt_stream_store_channel(psrt_stream_store value,const char *name,
                                                ps_unit unit,const char *description,psrt_site site) {
    (void)psrt_stream_store_get(value,site);
    if(value.owner->schema.channel_count>=PS_MAX_CHANNELS)psrt_fail_code(site,PS_LIMIT,"RunStore channel capacity exceeded");
    int index=ps_channel_add(&value.owner->schema,name,unit,description);
    if(index<0)psrt_fail_code(site,PS_INVALID,"RunStore requires unique bounded UTF-8 channels and canonical SI units");
    return index;
}
static inline void psrt_stream_writer_drop(void *value) {
    psrt_stream_writer *writer=value;psrt_stream_writer_owner *owner=writer->owner;
    if(owner && !--owner->references) {
        psrt_stream_store_owner *store=owner->store;ps_allocator allocator=store->allocator;
        if(store->store)ps_run_writer_release(store->store,owner->handle);
        ps_memory_free(allocator,owner,sizeof *owner);psrt_stream_store_release(store);
    }
    writer->owner=NULL;
}
static inline ps_result psrt_stream_writer_copy(void *out,const void *source) {
    psrt_stream_writer value=*(const psrt_stream_writer *)source;
    if(value.owner){if(value.owner->references==SIZE_MAX)return PS_LIMIT;value.owner->references++;}
    *(psrt_stream_writer *)out=value;return PS_OK;
}
static inline void psrt_stream_writer_keep(psrt_stream_writer *value,psrt_site site) {
    psrt_stream_writer copy;ps_result r=psrt_stream_writer_copy(&copy,value);
    if(r!=PS_OK)psrt_fail_code(site,r,"RunWriter ownership limit exceeded");
    *value=copy;
}
static inline psrt_stream_writer psrt_stream_writer_make(ps_allocator allocator,psrt_stream_store value,
                                                         const char *path,const char *name,psrt_site site) {
    ps_run_store *store=psrt_stream_store_get(value,site);(void)allocator;
    if(!value.owner->configured || !value.owner->schema.channel_count)psrt_fail_code(site,PS_INVALID,"RunStore requires configured metadata and channels before creating a writer");
    if(value.owner->references==SIZE_MAX)psrt_fail_code(site,PS_LIMIT,"RunStore ownership limit exceeded");
    psrt_stream_writer_owner *owner=NULL;
    ps_result r=ps_memory_zero(value.owner->allocator,1,sizeof *owner,(void **)&owner);
    if(r!=PS_OK)psrt_fail_code(site,r,"RunWriter allocation failed");
    r=ps_run_writer_create(store,path,&value.owner->schema,name,&owner->handle);
    if(r!=PS_OK){ps_memory_free(value.owner->allocator,owner,sizeof *owner);psrt_fail_code(site,r,"RunWriter creation failed");}
    owner->references=1;owner->store=value.owner;value.owner->references++;
    return (psrt_stream_writer){owner};
}
static inline ps_run_write_info psrt_stream_writer_info(psrt_stream_writer value,psrt_site site) {
    ps_run_write_info info;
    if(!value.owner || !value.owner->store->store)psrt_fail_code(site,PS_INVALID,"RunWriter is closed");
    ps_result r=ps_run_writer_describe(value.owner->store->store,value.owner->handle,&info);
    if(r!=PS_OK)psrt_fail_code(site,r,"RunWriter is closed");
    return info;
}
static inline bool psrt_stream_writer_open(psrt_stream_writer value,psrt_site site) {
    (void)site;ps_run_write_info info;
    return value.owner && value.owner->store->store &&
        ps_run_writer_describe(value.owner->store->store,value.owner->handle,&info)==PS_OK;
}
static inline void psrt_stream_writer_close(psrt_stream_writer value,psrt_site site) {
    (void)psrt_stream_writer_info(value,site);
    ps_result r=ps_run_writer_release(value.owner->store->store,value.owner->handle);
    if(r!=PS_OK)psrt_fail_code(site,r,"RunWriter finalization failed");
}
static inline void psrt_stream_writer_abort(psrt_stream_writer value,psrt_site site) {
    (void)psrt_stream_writer_info(value,site);
    ps_result r=ps_run_writer_abort(value.owner->store->store,value.owner->handle);
    if(r!=PS_OK)psrt_fail_code(site,r,"RunWriter abort failed");
}
static inline int64_t psrt_stream_writer_channels(psrt_stream_writer value,psrt_site s){return psrt_stream_writer_info(value,s).channels;}
static inline int64_t psrt_stream_writer_samples(psrt_stream_writer value,psrt_site s) {
    uint64_t count=psrt_stream_writer_info(value,s).samples;
    if(count>INT64_MAX)psrt_fail_code(s,PS_LIMIT,"RunWriter sample count exceeds Int64");
    return (int64_t)count;
}
static inline void psrt_stream_writer_append(psrt_stream_writer value,double time,const double *values,size_t count,psrt_site site) {
    (void)psrt_stream_writer_info(value,site);
    ps_result r=ps_run_writer_append(value.owner->store->store,value.owner->handle,time,
                                     values,count);
    if(r!=PS_OK)psrt_fail_code(site,r,"RunWriter append failed");
}
static inline void psrt_stream_writer_snapshot(psrt_stream_writer value,ps_snapshot snapshot,psrt_site site) {
    ps_run_write_info info=psrt_stream_writer_info(value,site);
    if(snapshot.count!=info.channels)psrt_fail_code(site,PS_INVALID,"RunWriter snapshot channel count differs from schema");
    ps_context context={0};context.time_s=snapshot.time;context.channel_count=snapshot.count;
    memcpy(context.values,snapshot.values,snapshot.count*sizeof(double));
    ps_result r=ps_run_writer_snapshot(value.owner->store->store,value.owner->handle,&context,&snapshot.scene,snapshot.paused);
    if(r!=PS_OK)psrt_fail_code(site,r,"RunWriter snapshot append failed");
}
static inline void psrt_stream_reader_drop(void *value) {
    psrt_stream_reader *reader=value;psrt_stream_reader_owner *owner=reader->owner;
    if(owner && !--owner->references) {
        psrt_stream_store_owner *store=owner->store;ps_allocator allocator=store->allocator;
        if(store->store)ps_run_reader_release(store->store,owner->handle);
        ps_memory_free(allocator,owner,sizeof *owner);psrt_stream_store_release(store);
    }
    reader->owner=NULL;
}
static inline ps_result psrt_stream_reader_copy(void *out,const void *source) {
    psrt_stream_reader value=*(const psrt_stream_reader *)source;
    if(value.owner){if(value.owner->references==SIZE_MAX)return PS_LIMIT;value.owner->references++;}
    *(psrt_stream_reader *)out=value;return PS_OK;
}
static inline void psrt_stream_reader_keep(psrt_stream_reader *value,psrt_site site) {
    psrt_stream_reader copy;ps_result r=psrt_stream_reader_copy(&copy,value);
    if(r!=PS_OK)psrt_fail_code(site,r,"RunReader ownership limit exceeded");
    *value=copy;
}
static inline psrt_stream_reader psrt_stream_reader_make(ps_allocator allocator,psrt_stream_store value,
                                                         const char *path,psrt_site site) {
    ps_run_store *store=psrt_stream_store_get(value,site);
    if(value.owner->references==SIZE_MAX)psrt_fail_code(site,PS_LIMIT,"RunStore ownership limit exceeded");
    /* The reader owner belongs to the store's allocator, which outlives all aliases. */
    (void)allocator;psrt_stream_reader_owner *owner=NULL;
    ps_result r=ps_memory_zero(value.owner->allocator,1,sizeof *owner,(void **)&owner);
    if(r!=PS_OK)psrt_fail_code(site,r,"RunReader allocation failed");
    r=ps_run_reader_open(store,path,&owner->handle);
    if(r!=PS_OK){ps_memory_free(value.owner->allocator,owner,sizeof *owner);psrt_fail_code(site,r,"RunReader open failed");}
    owner->references=1;owner->store=value.owner;value.owner->references++;
    return (psrt_stream_reader){owner};
}
static inline ps_run_read_info psrt_stream_reader_info(psrt_stream_reader value,psrt_site site) {
    ps_run_read_info info;
    if(!value.owner || !value.owner->store->store)psrt_fail_code(site,PS_INVALID,"RunReader is closed");
    ps_result r=ps_run_reader_describe(value.owner->store->store,value.owner->handle,&info);
    if(r!=PS_OK)psrt_fail_code(site,r,"RunReader is closed");
    return info;
}
static inline bool psrt_stream_reader_open(psrt_stream_reader value,psrt_site site) {
    (void)site;ps_run_read_info info;
    return value.owner && value.owner->store->store &&
        ps_run_reader_describe(value.owner->store->store,value.owner->handle,&info)==PS_OK;
}
static inline void psrt_stream_reader_close(psrt_stream_reader value,psrt_site site) {
    (void)psrt_stream_reader_info(value,site);
    ps_result r=ps_run_reader_release(value.owner->store->store,value.owner->handle);
    if(r!=PS_OK)psrt_fail_code(site,r,"RunReader close failed");
}
static inline bool psrt_stream_reader_advance(psrt_stream_reader value,bool snapshots,psrt_site site) {
    (void)psrt_stream_reader_info(value,site);psrt_stream_reader_owner *owner=value.owner;
    unsigned mode=snapshots?2:1;
    if(owner->mode && owner->mode!=mode)psrt_fail_code(site,PS_INVALID,"RunReader cannot mix sample and snapshot traversal");
    owner->mode=mode;
    ps_result r;
    if(snapshots) {
        r=ps_run_reader_snapshot_next(owner->store->store,owner->handle,&owner->snapshot);
        if(r==PS_OK)owner->has_snapshot=true;
    } else {
        size_t count=0;
        r=ps_run_reader_next(owner->store->store,owner->handle,&owner->row.time,
                            owner->row.values,PS_MAX_CHANNELS,&count);
        if(r==PS_OK)owner->has_row=true;
    }
    owner->status=r;
    if(r==PS_EOF || r==PS_RECOVERED)return false;
    if(r!=PS_OK)psrt_fail_code(site,r,"RunReader traversal failed");
    return true;
}
static inline bool psrt_stream_reader_next(psrt_stream_reader value,psrt_site s){return psrt_stream_reader_advance(value,false,s);}
static inline bool psrt_stream_reader_next_snapshot(psrt_stream_reader value,psrt_site s){return psrt_stream_reader_advance(value,true,s);}
static inline psrt_run_block psrt_stream_reader_row(ps_allocator allocator,psrt_stream_reader value,psrt_site site) {
    ps_run_read_info info=psrt_stream_reader_info(value,site);
    if(!value.owner->has_row)psrt_fail_code(site,PS_INVALID,"RunReader has no accepted row");
    static const psrt_element_type element={sizeof(psrt_run_row),NULL,NULL};
    psrt_run_block block={0};block.channels=info.channels;
    ps_result r=psrt_array_init(&element,allocator,1,&block.rows);
    if(r==PS_OK)r=psrt_array_replace(&block.rows,0,0,&value.owner->row,1);
    if(r!=PS_OK){psrt_run_block_drop(&block);psrt_fail_code(site,r,"RunReader row allocation failed");}
    return block;
}
static inline ps_snapshot psrt_stream_reader_snapshot(psrt_stream_reader value,psrt_site site) {
    (void)psrt_stream_reader_info(value,site);
    if(!value.owner->has_snapshot)psrt_fail_code(site,PS_INVALID,"RunReader has no accepted snapshot");
    return value.owner->snapshot;
}
static inline int64_t psrt_stream_reader_channels(psrt_stream_reader value,psrt_site s){return psrt_stream_reader_info(value,s).channels;}
static inline int64_t psrt_stream_reader_samples(psrt_stream_reader value,psrt_site s) {
    uint64_t count=psrt_stream_reader_info(value,s).samples;
    if(count>INT64_MAX)psrt_fail_code(s,PS_LIMIT,"RunReader sample count exceeds Int64");
    return (int64_t)count;
}
static inline bool psrt_stream_reader_complete(psrt_stream_reader value,psrt_site s){return psrt_stream_reader_info(value,s).complete;}
static inline int64_t psrt_stream_reader_status(psrt_stream_reader value,psrt_site s){(void)psrt_stream_reader_info(value,s);return value.owner->status;}
static inline bool psrt_stream_reader_recovered(psrt_stream_reader value,psrt_site s){return psrt_stream_reader_status(value,s)==PS_RECOVERED;}
static inline psrt_string psrt_stream_reader_metadata(ps_allocator a,psrt_stream_reader value,psrt_site s) {
    ps_run_read_info info=psrt_stream_reader_info(value,s);return psrt_run_string(a,info.metadata,s);
}
static inline ps_channel psrt_stream_reader_channel(psrt_stream_reader value,int64_t channel,psrt_site s) {
    ps_run_read_info info=psrt_stream_reader_info(value,s);
    if(channel<0 || (uint64_t)channel>=info.channels)psrt_fail_code(s,PS_INVALID,"RunReader channel out of bounds");
    return info.schema[channel];
}
#define PSRT_STREAM_CHANNEL_TEXT(name,field) static inline psrt_string psrt_stream_reader_##name(ps_allocator a,psrt_stream_reader value,int64_t channel,psrt_site s){ps_channel c=psrt_stream_reader_channel(value,channel,s);return psrt_run_string(a,c.field,s);}
PSRT_STREAM_CHANNEL_TEXT(name,name)
PSRT_STREAM_CHANNEL_TEXT(symbol,unit)
PSRT_STREAM_CHANNEL_TEXT(description,description)
#undef PSRT_STREAM_CHANNEL_TEXT
static inline int64_t psrt_stream_reader_dimension(psrt_stream_reader value,int64_t channel,int64_t axis,psrt_site s) {
    if(axis<0 || axis>=7)psrt_fail_code(s,PS_INVALID,"RunReader dimension axis out of bounds");
    return psrt_stream_reader_channel(value,channel,s).dimension[axis];
}
#endif
