#include "analysis_comparison.h"
#include "physim/data.h"
#include <SDL3/SDL.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Comparison %d: %s\n",__LINE__,#x);return 1;}}while(0)
static bool make_run(const char *path,unsigned n,double start,double step,bool reorder) {
    ps_context c={0};c.dt_s=step;
    if(reorder && ps_channel_add(&c,"other",PS_ONE,"")<0)return false;
    if(ps_channel_add(&c,"position",PS_METRE,"")<0)return false;
    ps_run_writer w;if(ps_run_create(&w,path,&c,"Async comparison")!=PS_OK)return false;
    for(unsigned i=0;i<n;i++){double v[2]={i*.25,i*.25};if(ps_run_append(&w,start+step*i,v)!=PS_OK)return false;}
    return ps_run_close(&w)==PS_OK;
}
static bool settle(ps_comparison *c) {
    Uint64 end=SDL_GetTicks()+20000;
    do{ps_comparison_poll(c);if(!ps_comparison_read(c).loading)return true;SDL_Delay(1);}while(SDL_GetTicks()<end);
    return false;
}
int main(int argc,char **argv) {
    CHECK(argc==2);char a[4096],b[4096];snprintf(a,sizeof a,"%s/a.psrun",argv[1]);snprintf(b,sizeof b,"%s/b.psrun",argv[1]);
    CHECK(make_run(a,6001,0,.01,false) && make_run(b,9,.25,1.5,true));
    ps_comparison_key key={.count=8,.reference=0,.channel={.name="position",.unit="m",.dimension={1}},.unit={.scale=1,.symbol="m",.dimension={1}}};
    snprintf(key.scope,sizeof key.scope,"project A");snprintf(key.scratch_prefix,sizeof key.scratch_prefix,"%s/scratch",argv[1]);
    for(unsigned i=0;i<8;i++)snprintf(key.paths[i],sizeof key.paths[i],"%s",a);
    ps_comparison *c=ps_comparison_create();CHECK(c);
    CHECK(ps_comparison_request(c,&key)==PS_OK);ps_comparison_poll(c);CHECK(ps_comparison_read(c).loading);
    /* No polling publication occurs between starting A and changing to B.
     * Both a completed A and a cooperatively cancelled A must be discarded. */
    key.count=1;key.unit.scale=.01;strcpy(key.unit.symbol,"cm");strcpy(key.paths[0],b);
    CHECK(ps_comparison_request(c,&key)==PS_OK && ps_comparison_read(c).snapshot==NULL);
    ps_comparison_key requested=key;memset(&key,0xdd,sizeof key);CHECK(settle(c));
    ps_comparison_state state=ps_comparison_read(c);ps_graphic_description d;ps_graphic_run_info run;
    CHECK(state.result==PS_OK && state.published==2 && state.requested==2 && state.discarded==1 && state.snapshot);
    CHECK(ps_graphic_snapshot_describe(state.snapshot,&d)==PS_OK && d.plot.curves==1 && !strcmp(d.plot.y_unit.symbol,"cm"));
    const ps_curve_data *curve;CHECK(ps_report_curve_view(ps_graphic_snapshot_report(state.snapshot),0,0,&curve)==PS_OK && curve->x[1]==1.75 && curve->y[1]==25);
    CHECK(ps_graphic_snapshot_run(state.snapshot,0,&run)==PS_OK && !strcmp(run.path,b) && run.samples==9 && run.si_statistics.count==9 && run.si_statistics.mean==1);
    CHECK(ps_comparison_request(c,&requested)==PS_OK && ps_comparison_read(c).requested==2); /* idempotent, no disk reread */
    ps_graphic_snapshot *frozen=NULL;CHECK(ps_graphic_snapshot_copy(state.snapshot,NULL,ps_allocator_default(),&frozen)==PS_OK);
    requested.unit.scale=1;strcpy(requested.unit.symbol,"m");requested.count=8;
    for(unsigned i=0;i<8;i++)strcpy(requested.paths[i],i%2?b:a);
    CHECK(ps_comparison_request(c,&requested)==PS_OK && settle(c));
    state=ps_comparison_read(c);CHECK(state.result==PS_OK && ps_graphic_snapshot_describe(state.snapshot,&d)==PS_OK && d.plot.curves==8 && d.reduced);
    CHECK(ps_report_curve_view(ps_graphic_snapshot_report(state.snapshot),0,1,&curve)==PS_OK && curve->x[1]==1.75 && curve->y[1]==.25);
    requested.revision++;strcpy(requested.channel.name,"absent");CHECK(ps_comparison_request(c,&requested)==PS_OK && settle(c));
    CHECK(ps_comparison_read(c).result==PS_INVALID && !ps_comparison_read(c).snapshot);
    /* Project identity and a return to a previous key are distinct requests. */
    strcpy(requested.scope,"project B");strcpy(requested.channel.name,"position");
    CHECK(ps_comparison_request(c,&requested)==PS_OK);ps_comparison_poll(c);
    requested.count=0;requested.revision++;CHECK(ps_comparison_request(c,&requested)==PS_OK);
    CHECK(!ps_comparison_read(c).loading && !ps_comparison_read(c).snapshot);ps_comparison_destroy(c);
    CHECK(!remove(a) && !remove(b));
    CHECK(ps_graphic_snapshot_run(frozen,0,&run)==PS_OK && !strcmp(run.path,b));
    CHECK(ps_graphic_snapshot_describe(frozen,&d)==PS_OK && !strcmp(d.plot.y_unit.symbol,"cm"));
    ps_graphic_snapshot_destroy(frozen);
    puts("COMPARISON CACHE: owned keys, latest publication, stale discard, 8 axes, units, clear/shutdown and frozen copy passed");return 0;
}
