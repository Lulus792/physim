#ifndef PHYSIM_LANGUAGE_BATCH_HOST_H
#define PHYSIM_LANGUAGE_BATCH_HOST_H
#include "language_analysis_sdk.h"
/* Included after psra_host and psra_check are defined. */
static inline const ps_analysis_services *psra_batch_services(psra_host *h,bool resume,psrt_site site) {
    const ps_analysis_services *services=h->services;
    if(!services || services->struct_size<(resume?sizeof *services:offsetof(ps_analysis_services,resume_batch)) ||
       services->version!=PS_ANALYSIS_SERVICES_VERSION ||
       (resume?!services->resume_batch:!services->run_batch))
        psrt_fail_code(site,PS_VERSION,"Analysis host does not provide Batch services");
    return services;
}
static inline psrt_batch psra_batch_run_until(psra_host *h,ps_allocator allocator,psrt_batch request,int64_t completions,psrt_site site) {
    const ps_analysis_services *services=psra_batch_services(h,false,site);
    psrt_batch_storage initial=*psrt_batch_data(request,site);psrt_batch_clear_result(&initial);
    if(completions<0 || (uint64_t)completions>initial.options.runs)
        psrt_fail_code(site,PS_INVALID,"Batch completion limit out of bounds");
    /* Allocate the result before starting processes or creating an archive. */
    psrt_batch value=psrt_batch_store(allocator,&initial,site);
    psrt_batch_storage *storage=(psrt_batch_storage *)psrt_array_data(&value.storage);
    uint32_t stop_after=(uint64_t)completions==initial.options.runs?0:(uint32_t)completions;
    storage->code=services->run_batch(services->user,&storage->options,stop_after,&storage->result);
    storage->executed=true;
    return value;
}
static inline psrt_batch psra_batch_run(psra_host *h,ps_allocator allocator,psrt_batch request,psrt_site site) {
    return psra_batch_run_until(h,allocator,request,0,site);
}
static inline psrt_batch psra_batch_resume(psra_host *h,ps_allocator allocator,const char *series,const char *directory,psrt_site site) {
    const ps_analysis_services *services=psra_batch_services(h,true,site);
    psrt_batch_storage storage={0};
    ps_result r=services->resume_batch(services->user,series,directory,&storage.options);
    if(r!=PS_OK)psrt_fail_code(site,r,"Batch checkpoint cannot be resumed");
    if(storage.options.source_text || storage.options.source_size)
        psrt_fail_code(site,PS_INVALID,"Batch host returned borrowed source text");
    return psrt_batch_store(allocator,&storage,site);
}
static inline void psra_batch_require_success(psra_host *h,psrt_batch value,psrt_site site) {
    (void)h;const psrt_batch_storage *s=psrt_batch_data(value,site);
    if(!s->executed)psrt_fail_code(site,PS_INVALID,"Batch has not been executed");
    if(s->code!=PS_OK)psrt_fail_code(site,s->code,s->result.error[0]?s->result.error:ps_result_string(s->code));
    if(s->result.cancelled || s->result.completed!=s->options.runs)
        psrt_fail_code(site,PS_INVALID,"Batch is incomplete or cancelled");
}
static inline ps_series psra_batch_series(psra_host *h,psrt_batch value,psrt_site site) {
    const psrt_batch_storage *s=psrt_batch_data(value,site);
    if(!s->executed || !s->result.valid)psrt_fail_code(site,PS_INVALID,"Batch has no valid endpoints");
    double values[PS_BATCH_MAX_RUNS];size_t count=0;
    for(uint32_t i=0;i<s->options.runs;i++)if(s->result.endpoint_status[i]==1)values[count++]=s->result.values[i];
    ps_unit unit={.scale=1,.symbol=s->result.channel.unit};memcpy(unit.dimension,s->result.channel.dimension,sizeof unit.dimension);
    ps_series result;psra_check(h,ps_series_from_values(h->context,values,count,unit,s->options.channel,&result),site);return result;
}
#endif
