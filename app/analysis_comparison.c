#include "analysis_comparison.h"
#include "text_validation.h"
#include <SDL3/SDL.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    ps_comparison_key key;
    uint64_t version;
    SDL_Thread *thread;
    SDL_AtomicInt done,cancel;
    ps_graphic_snapshot *snapshot;
    ps_graphic_failure failure;
} comparison_job;
struct ps_comparison {
    ps_comparison_key desired;
    comparison_job *job;
    ps_graphic_snapshot *snapshot;
    ps_comparison_state state;
    bool initialized,pending;
};
static bool cancelled(void *user){return SDL_GetAtomicInt(&((comparison_job *)user)->cancel)!=0;}
static int comparison_work(void *user) {
    comparison_job *j=user;ps_graphic_run_request r={.count=j->key.count,.reference=j->key.reference,.channel=j->key.channel,.display_unit=j->key.unit};
    for(uint32_t i=0;i<r.count;i++)r.inputs[i]=(ps_graphic_run_input){.path=j->key.paths[i],.kind=PS_PLOT_LINE};
    ps_graphic_snapshot_compare_runs(&r,j->key.scratch_prefix,j->key.scratch_quota,cancelled,j,
        ps_allocator_default(),&j->snapshot,&j->failure);
    SDL_SetAtomicInt(&j->done,1);return 0;
}
static void job_destroy(comparison_job *j) {
    if(j){SDL_SetAtomicInt(&j->cancel,1);if(j->thread)SDL_WaitThread(j->thread,NULL);ps_graphic_snapshot_destroy(j->snapshot);free(j);}
}
ps_comparison *ps_comparison_create(void){return calloc(1,sizeof(ps_comparison));}
void ps_comparison_destroy(ps_comparison *c){if(c){job_destroy(c->job);ps_graphic_snapshot_destroy(c->snapshot);free(c);}}
static bool same_key(const ps_comparison_key *a,const ps_comparison_key *b) {
    if(a->revision!=b->revision || strcmp(a->scope,b->scope) || a->count!=b->count)return false;
    if(!a->count)return true;
    if(a->reference!=b->reference || a->scratch_quota!=b->scratch_quota || strcmp(a->scratch_prefix,b->scratch_prefix) ||
       strcmp(a->channel.name,b->channel.name) || strcmp(a->channel.unit,b->channel.unit) || memcmp(a->channel.dimension,b->channel.dimension,7) ||
       a->unit.scale!=b->unit.scale || strcmp(a->unit.symbol,b->unit.symbol) || memcmp(a->unit.dimension,b->unit.dimension,7))return false;
    for(uint32_t i=0;i<a->count;i++)if(strcmp(a->paths[i],b->paths[i]))return false;
    return true;
}
ps_result ps_comparison_request(ps_comparison *c,const ps_comparison_key *key) {
    if(!c || !key || key->count>PS_GRAPHIC_MAX_INPUTS || !ps_text_valid(key->scope,sizeof key->scope,false))return PS_INVALID;
    if(key->count) {
        if(key->reference>=key->count || !ps_text_valid(key->scratch_prefix,sizeof key->scratch_prefix,false) || !key->scratch_prefix[0] ||
           !ps_text_valid(key->channel.name,sizeof key->channel.name,false) || !key->channel.name[0] ||
           !ps_text_valid(key->channel.unit,sizeof key->channel.unit,false) || !ps_text_valid(key->unit.symbol,sizeof key->unit.symbol,false) ||
           !isfinite(key->unit.scale) || key->unit.scale<=0 || memcmp(key->channel.dimension,key->unit.dimension,7))return PS_INVALID;
        for(uint32_t i=0;i<key->count;i++)if(!key->paths[i][0] || !ps_text_valid(key->paths[i],sizeof key->paths[i],false))return PS_INVALID;
    }
    if(c->initialized && same_key(&c->desired,key))return PS_OK;
    c->desired=*key;c->initialized=true;c->state.requested++;
    if(c->job)SDL_SetAtomicInt(&c->job->cancel,1);
    ps_graphic_snapshot_destroy(c->snapshot);c->snapshot=NULL;c->state.snapshot=NULL;
    c->state.result=PS_OK;c->state.failed_input=UINT32_MAX;c->state.loading=c->pending=key->count!=0;
    return PS_OK;
}
void ps_comparison_poll(ps_comparison *c) {
    if(!c)return;
    if(c->job && SDL_GetAtomicInt(&c->job->done)) {
        comparison_job *j=c->job;c->job=NULL;
        SDL_WaitThread(j->thread,NULL);j->thread=NULL;
        if(j->version==c->state.requested) {
            c->snapshot=j->snapshot;j->snapshot=NULL;c->state.snapshot=c->snapshot;
            c->state.result=j->failure.result;c->state.failed_input=j->failure.input;
            c->state.published=j->version;c->state.loading=false;
        } else c->state.discarded++;
        job_destroy(j);
    }
    if(!c->job && c->pending) {
        c->pending=false;comparison_job *j=calloc(1,sizeof *j);
        if(j){j->key=c->desired;j->version=c->state.requested;j->thread=SDL_CreateThread(comparison_work,"physim-comparison",j);}
        if(!j || !j->thread) {
            free(j);c->state.loading=false;c->state.result=PS_MEMORY;c->state.published=c->state.requested;
        } else c->job=j;
    }
}
ps_comparison_state ps_comparison_read(const ps_comparison *c){return c?c->state:(ps_comparison_state){.result=PS_MEMORY,.failed_input=UINT32_MAX};}
