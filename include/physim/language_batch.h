#ifndef PHYSIM_LANGUAGE_BATCH_H
#define PHYSIM_LANGUAGE_BATCH_H
#include "batch.h"
#include "language_array.h"
#include "language_string.h"
#include <float.h>
/* Immutable request/result snapshots. Paths and endpoint arrays are owned by
 * this value; copies retain storage, setters allocate independent snapshots. */
typedef struct {
    ps_batch_options options;
    ps_batch_result result;
    ps_result code;
    bool executed;
} psrt_batch_storage;
typedef struct {psrt_array storage;} psrt_batch;
static inline void psrt_batch_drop(void *value) {
    psrt_batch *b=value;psrt_array_destroy(&b->storage);memset(b,0,sizeof *b);
}
static inline ps_result psrt_batch_copy(void *destination,const void *source) {
    const psrt_batch *b=source;psrt_batch copy=*b;
    ps_result r=psrt_array_clone(&b->storage,&copy.storage);
    if(r==PS_OK)*(psrt_batch *)destination=copy;
    return r;
}
static inline void psrt_batch_keep(psrt_batch *value,psrt_site site) {
    psrt_batch copy;ps_result r=psrt_batch_copy(&copy,value);
    if(r!=PS_OK)psrt_fail_code(site,r,"Batch ownership limit exceeded");
    *value=copy;
}
static inline const psrt_batch_storage *psrt_batch_data(psrt_batch value,psrt_site site) {
    if(!value.storage.block)psrt_fail(site,"Uninitialized Batch");
    return psrt_array_data(&value.storage);
}
static inline psrt_batch psrt_batch_store(ps_allocator allocator,const psrt_batch_storage *storage,psrt_site site) {
    static const psrt_element_type element={sizeof(psrt_batch_storage),NULL,NULL};
    psrt_batch value={0};ps_result r=psrt_array_init(&element,allocator,1,&value.storage);
    if(r==PS_OK)r=psrt_array_replace(&value.storage,0,0,storage,1);
    if(r!=PS_OK){psrt_batch_drop(&value);psrt_fail_code(site,r,"Batch memory budget or allocation exhausted");}
    return value;
}
static inline void psrt_batch_text(char *out,size_t capacity,const char *text,psrt_site site) {
    if(!text || strlen(text)>=capacity)psrt_fail_code(site,PS_LIMIT,"Batch text exceeds its field limit");
    memcpy(out,text,strlen(text)+1);
}
static inline void psrt_batch_clear_result(psrt_batch_storage *s) {
    memset(&s->result,0,sizeof s->result);s->executed=false;s->code=PS_OK;
}
static inline psrt_batch psrt_batch_make(ps_allocator allocator,const char *module,const char *directory,const char *channel,
    int64_t runs,int64_t steps,double dt,int64_t seed,int64_t workers,psrt_site site) {
    if(runs<1 || runs>PS_BATCH_MAX_RUNS || steps<1 || steps>100000 || workers<1 || workers>PS_BATCH_MAX_WORKERS ||
       (uint64_t)runs*((uint64_t)steps+1)>PS_BATCH_MAX_SAMPLES || (uint64_t)seed>UINT64_MAX-((uint64_t)runs-1) ||
       !isfinite(dt) || dt<DBL_MIN || dt>1)psrt_fail_code(site,PS_INVALID,"Invalid Batch counts, seed range or timestep");
    psrt_batch_storage s={0};s.options.runs=(uint32_t)runs;s.options.steps=(uint32_t)steps;
    s.options.workers=(uint32_t)workers;s.options.dt=dt;s.options.seed=(uint64_t)seed;s.options.timeout_s=30;
    s.options.minimum_dt=1e-8;s.options.maximum_dt=1;
    psrt_batch_text(s.options.module,sizeof s.options.module,module,site);
    psrt_batch_text(s.options.directory,sizeof s.options.directory,directory,site);
    psrt_batch_text(s.options.channel,sizeof s.options.channel,channel,site);
    return psrt_batch_store(allocator,&s,site);
}
static inline psrt_batch psrt_batch_parameter(ps_allocator allocator,psrt_batch value,const char *name,double parameter,psrt_site site) {
    psrt_batch_storage s=*psrt_batch_data(value,site);
    if(!isfinite(parameter) || !name || !*name || (s.options.sweep && !strcmp(name,s.options.sweep_name)))
        psrt_fail_code(site,PS_INVALID,"Invalid or conflicting Batch parameter");
    uint32_t i=0;while(i<s.options.parameter_count && strcmp(s.options.parameters[i].name,name))i++;
    if(i==PS_MAX_PARAMETERS)psrt_fail_code(site,PS_LIMIT,"Batch parameter limit exceeded");
    psrt_batch_text(s.options.parameters[i].name,sizeof s.options.parameters[i].name,name,site);
    s.options.parameters[i].value=parameter;if(i==s.options.parameter_count)s.options.parameter_count++;
    psrt_batch_clear_result(&s);return psrt_batch_store(allocator,&s,site);
}
static inline psrt_batch psrt_batch_sweep(ps_allocator allocator,psrt_batch value,const char *name,double start,double end,psrt_site site) {
    psrt_batch_storage s=*psrt_batch_data(value,site);
    if(s.options.runs<2 || !isfinite(start) || !isfinite(end) || start==end || !name || !*name || s.options.parameter_count==PS_MAX_PARAMETERS)
        psrt_fail_code(site,PS_INVALID,"Invalid Batch parameter sweep");
    for(uint32_t i=0;i<s.options.parameter_count;i++)if(!strcmp(s.options.parameters[i].name,name))
        psrt_fail_code(site,PS_INVALID,"Batch sweep conflicts with a fixed parameter");
    s.options.sweep=true;s.options.sweep_start=start;s.options.sweep_end=end;
    psrt_batch_text(s.options.sweep_name,sizeof s.options.sweep_name,name,site);
    psrt_batch_clear_result(&s);return psrt_batch_store(allocator,&s,site);
}
static inline psrt_batch psrt_batch_target(ps_allocator allocator,psrt_batch value,double time,psrt_site site) {
    if(!isfinite(time) || time<=0 || time>1e9)psrt_fail_code(site,PS_INVALID,"Invalid Batch end time");
    psrt_batch_storage s=*psrt_batch_data(value,site);s.options.end_time=time;
    psrt_batch_clear_result(&s);return psrt_batch_store(allocator,&s,site);
}
static inline psrt_batch psrt_batch_adaptive(ps_allocator allocator,psrt_batch value,double minimum,double maximum,psrt_site site) {
    psrt_batch_storage s=*psrt_batch_data(value,site);
    if(!isfinite(minimum) || !isfinite(maximum) || minimum<DBL_MIN || minimum>s.options.dt || maximum<s.options.dt || maximum>1 || s.options.end_time<=0)
        psrt_fail_code(site,PS_INVALID,"Batch adaptive bounds require a common end time");
    s.options.adaptive=true;s.options.minimum_dt=minimum;s.options.maximum_dt=maximum;
    psrt_batch_clear_result(&s);return psrt_batch_store(allocator,&s,site);
}
static inline psrt_batch psrt_batch_limits(ps_allocator allocator,psrt_batch value,double timeout,int64_t memory,psrt_site site) {
    if(!isfinite(timeout) || timeout<=0 || timeout>3600 || memory<0 || memory>16384)
        psrt_fail_code(site,PS_INVALID,"Invalid Batch resource limits");
    psrt_batch_storage s=*psrt_batch_data(value,site);s.options.timeout_s=timeout;s.options.memory_bytes=(uint64_t)memory*1048576u;
    psrt_batch_clear_result(&s);return psrt_batch_store(allocator,&s,site);
}
static inline psrt_batch psrt_batch_source(ps_allocator allocator,psrt_batch value,const char *path,psrt_site site) {
    psrt_batch_storage s=*psrt_batch_data(value,site);
    psrt_batch_text(s.options.source,sizeof s.options.source,path,site);
    psrt_batch_clear_result(&s);return psrt_batch_store(allocator,&s,site);
}
#define PSRT_BATCH_INT(name,field) static inline int64_t psrt_batch_##name(psrt_batch b,psrt_site site){return psrt_batch_data(b,site)->field;}
PSRT_BATCH_INT(runs,options.runs)
PSRT_BATCH_INT(steps,options.steps)
PSRT_BATCH_INT(workers,options.workers)
PSRT_BATCH_INT(completed,result.completed)
PSRT_BATCH_INT(started,result.started)
PSRT_BATCH_INT(reused,result.reused)
PSRT_BATCH_INT(valid,result.valid)
PSRT_BATCH_INT(peak,result.peak_active)
PSRT_BATCH_INT(code,code)
#undef PSRT_BATCH_INT
static inline int64_t psrt_batch_seed(psrt_batch b,psrt_site site){uint64_t seed=psrt_batch_data(b,site)->options.seed;int64_t bits;memcpy(&bits,&seed,sizeof bits);return bits;}
static inline double psrt_batch_dt(psrt_batch b,psrt_site site){return psrt_batch_data(b,site)->options.dt;}
static inline double psrt_batch_end_time(psrt_batch b,psrt_site site){return psrt_batch_data(b,site)->options.end_time;}
static inline bool psrt_batch_executed(psrt_batch b,psrt_site site){return psrt_batch_data(b,site)->executed;}
static inline bool psrt_batch_cancelled(psrt_batch b,psrt_site site){return psrt_batch_data(b,site)->result.cancelled;}
static inline uint32_t psrt_batch_index(psrt_batch b,int64_t index,psrt_site site){
    if(index<0 || (uint64_t)index>=psrt_batch_data(b,site)->options.runs)psrt_fail_code(site,PS_LIMIT,"Batch run index out of bounds");return (uint32_t)index;
}
static inline int64_t psrt_batch_status(psrt_batch b,int64_t index,psrt_site site){return psrt_batch_data(b,site)->result.endpoint_status[psrt_batch_index(b,index,site)];}
static inline bool psrt_batch_finished(psrt_batch b,int64_t index,psrt_site site){return psrt_batch_data(b,site)->result.finished[psrt_batch_index(b,index,site)];}
static inline double psrt_batch_value(psrt_batch b,int64_t index,psrt_site site){
    const psrt_batch_storage *s=psrt_batch_data(b,site);uint32_t i=psrt_batch_index(b,index,site);
    if(!s->executed || s->result.endpoint_status[i]!=1)psrt_fail_code(site,PS_INVALID,"Batch endpoint is not valid");return s->result.values[i];
}
static inline psrt_string psrt_batch_string(ps_allocator allocator,const char *text,psrt_site site){
    psrt_string result;ps_result r=psrt_string_make(allocator,text,strlen(text),&result);
    if(r!=PS_OK)psrt_fail_code(site,r,"Batch text allocation failed");return result;
}
static inline psrt_string psrt_batch_directory(ps_allocator a,psrt_batch b,psrt_site site){return psrt_batch_string(a,psrt_batch_data(b,site)->options.directory,site);}
static inline psrt_string psrt_batch_module(ps_allocator a,psrt_batch b,psrt_site site){return psrt_batch_string(a,psrt_batch_data(b,site)->options.module,site);}
static inline psrt_string psrt_batch_error(ps_allocator a,psrt_batch b,psrt_site site){return psrt_batch_string(a,psrt_batch_data(b,site)->result.error,site);}
static inline psrt_string psrt_batch_run_path(ps_allocator a,psrt_batch b,int64_t index,psrt_site site){
    uint32_t i=psrt_batch_index(b,index,site);char path[4096];int n=snprintf(path,sizeof path,"%s/run-%04u.psrun",psrt_batch_data(b,site)->options.directory,i+1);
    if(n<0 || (size_t)n>=sizeof path)psrt_fail_code(site,PS_LIMIT,"Batch archive path exceeds its limit");return psrt_batch_string(a,path,site);
}
static inline ps_unit psrt_batch_unit(psrt_memory *memory,psrt_batch b,psrt_site site){
    const psrt_batch_storage *s=psrt_batch_data(b,site);
    if(!s->executed || !s->result.completed)psrt_fail_code(site,PS_INVALID,"Batch has no verified channel schema");
    ps_unit unit={.scale=1};memcpy(unit.dimension,s->result.channel.dimension,sizeof unit.dimension);
    psrt_string text=psrt_batch_string(psrt_memory_allocator(memory),s->result.channel.unit,site);
    ps_result r=psrt_string_pin_cstr(memory,&text,&unit.symbol);psrt_string_destroy(&text);
    if(r!=PS_OK)psrt_fail_code(site,r,"Batch unit symbol allocation failed");return unit;
}
static inline psrt_array psrt_batch_values(ps_allocator allocator,psrt_batch b,psrt_site site){
    static const psrt_element_type element={sizeof(double),NULL,NULL};
    const psrt_batch_storage *s=psrt_batch_data(b,site);double values[PS_BATCH_MAX_RUNS];size_t count=0;
    for(uint32_t i=0;i<s->options.runs;i++)if(s->result.endpoint_status[i]==1)values[count++]=s->result.values[i];
    psrt_array result={0};ps_result r=psrt_array_init(&element,allocator,PS_BATCH_MAX_RUNS,&result);
    if(r==PS_OK)r=psrt_array_replace(&result,0,0,values,count);
    if(r!=PS_OK){psrt_array_destroy(&result);psrt_fail_code(site,r,"Batch endpoint allocation failed");}return result;
}
static inline psrt_array psrt_batch_statuses(ps_allocator allocator,psrt_batch b,psrt_site site){
    static const psrt_element_type element={sizeof(int64_t),NULL,NULL};
    const psrt_batch_storage *s=psrt_batch_data(b,site);int64_t values[PS_BATCH_MAX_RUNS];
    for(uint32_t i=0;i<s->options.runs;i++)values[i]=s->result.endpoint_status[i];
    psrt_array result={0};ps_result r=psrt_array_init(&element,allocator,PS_BATCH_MAX_RUNS,&result);
    if(r==PS_OK)r=psrt_array_replace(&result,0,0,values,s->options.runs);
    if(r!=PS_OK){psrt_array_destroy(&result);psrt_fail_code(site,r,"Batch status allocation failed");}return result;
}
#endif
