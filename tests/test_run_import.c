#include "run_import.h"
#include "physim/data.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Import line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static bool text(const char *p,const char *s) {FILE *f=fopen(p,"wb");if(!f)return false;bool ok=fputs(s,f)>=0;return !fclose(f) && ok;}
static bool same(const char *a,const char *b) {
    size_t na=0,nb=0;void *pa=SDL_LoadFile(a,&na),*pb=SDL_LoadFile(b,&nb);
    bool ok=pa && pb && na==nb && !memcmp(pa,pb,na);SDL_free(pa);SDL_free(pb);return ok;
}
int main(int argc,char **argv) {
    CHECK(argc==2);char source[4096],dest[4096],side[4096],copy[4096],bad[4096],partial[4096];
    snprintf(source,sizeof source,"%s/source ä.psrun",argv[1]);snprintf(dest,sizeof dest,"%s/imported ä.psrun",argv[1]);
    snprintf(partial,sizeof partial,"%s/partial.psrun",argv[1]);snprintf(bad,sizeof bad,"%s/bad.psrun",argv[1]);
    ps_context c={.struct_size=sizeof c,.api_version=PS_API_VERSION,.dt_s=.01};ps_channel_add(&c,"position.x",PS_METRE,"Position");
    ps_scene scene={0};CHECK(ps_scene_label_id(&scene,42,ps_v3(1,2,3),"Snapshot ä",0xff0000ff)==PS_OK);
    ps_run_writer w;CHECK(ps_run_create(&w,source,&c,"Import test")==PS_OK);
    for(unsigned i=0;i<600;i++){c.values[0]=i;c.time_s=i*.01;CHECK(ps_run_append(&w,c.time_s,c.values)==PS_OK);
        if(i%100==0)CHECK(ps_run_append_snapshot(&w,&c,&scene,false)==PS_OK);}
    CHECK(ps_run_close(&w)==PS_OK);
    snprintf(side,sizeof side,"%s.experiment.c",source);CHECK(text(side,"/* source ä */\n"));
    CHECK(ps_run_import(source,dest)==PS_OK && same(source,dest));
    snprintf(copy,sizeof copy,"%s.experiment.c",dest);CHECK(same(side,copy));
    CHECK(ps_run_import(source,dest)==PS_IO && same(source,dest) && same(side,copy));
    /* A colliding sidecar rolls back only newly published files. */
    snprintf(copy,sizeof copy,"%s.experiment.c",bad);CHECK(text(copy,"protected"));
    CHECK(ps_run_import(source,bad)==PS_IO);SDL_PathInfo info;CHECK(!SDL_GetPathInfo(bad,&info));
    size_t bytes;char *protected=SDL_LoadFile(copy,&bytes);CHECK(protected && bytes==9 && !memcmp(protected,"protected",9));SDL_free(protected);
    SDL_RemovePath(copy);
    CHECK(ps_run_create(&w,partial,&c,"Partial")==PS_OK && ps_run_append(&w,0,c.values)==PS_OK && fclose(w.file)==0);
    CHECK(ps_run_import(partial,bad)==PS_RECOVERED && same(partial,bad));
    SDL_RemovePath(bad);CHECK(text(partial,"corrupt") && ps_run_import(partial,bad)==PS_CORRUPT && !SDL_GetPathInfo(bad,&info));
    SDL_RemovePath(partial);
    CHECK(ps_run_create(&w,partial,&c,"Reversed time")==PS_OK && ps_run_append(&w,1,c.values)==PS_OK && ps_run_append(&w,.5,c.values)==PS_OK && ps_run_close(&w)==PS_OK);
    CHECK(ps_run_import(partial,bad)==PS_CORRUPT && !SDL_GetPathInfo(bad,&info));
    /* Numeric records cannot hide an unknown snapshot version. */
    size_t n=0;unsigned char *data=SDL_LoadFile(source,&n);CHECK(data && n>16);
    bool changed=false;
    for(size_t at=16;at+12<=n;) {
        uint32_t type=ps_get_u32(data+at),size=ps_get_u32(data+at+4);CHECK(size<=n-at-12);
        if(type==5){ps_put_u32(data+at+12,PS_SNAPSHOT_VERSION+1);ps_put_u32(data+at+8,ps_crc32(data+at+12,size));changed=true;break;}
        at+=12+size;
    }
    CHECK(changed);FILE *f=fopen(partial,"wb");CHECK(f && fwrite(data,1,n,f)==n && !fclose(f));SDL_free(data);
    CHECK(ps_run_import(partial,bad)==PS_VERSION && !SDL_GetPathInfo(bad,&info));
    CHECK(ps_run_import("/missing-physim-input",bad)==PS_EOF);
    puts("Streaming import: byte-identical runs and sources, collision rollback, recovery, corruption and time validation passed");return 0;
}
