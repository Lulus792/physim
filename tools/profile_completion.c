/* Backend-only completion profile. No graphical/window/frame-rate claim. */
#if !defined(_WIN32) && !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200809L
#endif
#include "completion_locals.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif
typedef struct {size_t live,peak,blocks,attempts,denied,invalid,limit;} domain;
typedef union {ps_memory_alignment alignment;struct {domain *owner;size_t bytes;} data;} allocation;
static void *allocate(void *user,size_t bytes){
    domain *d=user;d->attempts++;
    if(bytes>SIZE_MAX-sizeof(allocation) || bytes>d->limit-d->live){d->denied++;return NULL;}
    allocation *p=malloc(sizeof *p+bytes);if(!p){d->denied++;return NULL;}
    p->data.owner=d;p->data.bytes=bytes;d->live+=bytes;d->blocks++;if(d->live>d->peak)d->peak=d->live;return p+1;
}
static void deallocate(void *user,void *pointer,size_t bytes){
    domain *d=user;allocation *p=(allocation*)pointer-1;
    if(p->data.owner!=d || p->data.bytes!=bytes || bytes>d->live || !d->blocks){d->invalid++;return;}
    d->live-=bytes;d->blocks--;free(p);
}
static double now(void){
#ifdef _WIN32
    LARGE_INTEGER n,hz;QueryPerformanceCounter(&n);QueryPerformanceFrequency(&hz);return (double)n.QuadPart/(double)hz.QuadPart;
#else
    struct timespec n;clock_gettime(CLOCK_MONOTONIC,&n);return (double)n.tv_sec+(double)n.tv_nsec/1e9;
#endif
}
static bool number(const char *text,size_t *value){
    if(!*text)return false;
    for(const char *p=text;*p;p++)if(*p<'0' || *p>'9')return false;
    errno=0;char *end=NULL;unsigned long long n=strtoull(text,&end,10);
    if(errno || *end || n>SIZE_MAX)return false;
    *value=(size_t)n;return true;
}
static int sample(const char *source,size_t bytes,size_t cursor,const char *mode,size_t iterations,bool tracked){
    domain d={.limit=64u*1024u*1024u};int owner=0;ps_completion_locals p={.allocator={&d,allocate,deallocate}};
    ps_allocator allocator=tracked?(ps_allocator){&d,allocate,deallocate}:ps_allocator_default();p.allocator=allocator;
    bool syntax=!strcmp(mode,"syntax"),bindings=!strcmp(mode,"bindings"),edit=!strcmp(mode,"edit");
    bool cold=!strcmp(mode,"cold")||syntax||bindings,cached=!strcmp(mode,"cached");
    size_t alternate=cursor;
    if(!cold && !cached && !edit){
        if(cursor<bytes){alternate++;while(alternate<bytes && ((unsigned char)source[alternate]&0xc0)==0x80)alternate++;}
        else if(cursor){alternate--;while(alternate && ((unsigned char)source[alternate]&0xc0)==0x80)alternate--;}
        if(alternate==cursor)return 64;
    }
    size_t capacity=bytes<32767?bytes*2+2:65536;ps_c_node *nodes=NULL;ps_c_binding *raw=NULL;
    if(syntax && ps_memory_allocate(allocator,capacity*sizeof *nodes,(void**)&nodes)!=PS_OK)return 1;
    if(bindings && ps_memory_allocate(allocator,capacity*sizeof *raw,(void**)&raw)!=PS_OK)return 1;
    char *inserted=NULL;
    if(edit){inserted=malloc(bytes+2);if(!inserted)return 1;memcpy(inserted,source,cursor);inserted[cursor]='v';memcpy(inserted+cursor+1,source+cursor,bytes-cursor);inserted[bytes+1]=0;}
    if(!cold && ps_completion_locals_update(&p,&owner,source,bytes,cached||edit?cursor:alternate,false)!=PS_OK){ps_completion_locals_destroy(&p);free(inserted);return 1;}
    int exit_code=0;
    for(size_t i=0;i<iterations;i++){
        if(cold)ps_completion_locals_destroy(&p);
        p.allocator=allocator;d.peak=d.live;size_t attempts=d.attempts;
        bool insert=edit && !(i&1);const char *text=insert?inserted:source;size_t size=bytes+(insert?1:0);
        size_t at=edit?cursor+(insert?1:0):!cold && !cached && (i&1)?alternate:cursor;
        ps_result r;size_t count;ps_lang_editor_status status;bool call=false;double start=now();
        if(syntax){ps_c_syntax_result result=ps_c_syntax(source,bytes,nodes,capacity,allocator,(ps_c_limits){0});r=result.status;count=result.count;status=result.has_error?PS_LANG_EDITOR_PREFIX:PS_LANG_EDITOR_OK;}
        else if(bindings){ps_c_bindings_result result=ps_c_bindings(source,bytes,at,raw,capacity,allocator);r=result.status;count=result.count;status=result.has_error?PS_LANG_EDITOR_PREFIX:PS_LANG_EDITOR_OK;call=result.call.active;}
        else{r=ps_completion_locals_update(&p,&owner,text,size,at,false);count=p.count;status=p.status;call=p.call.active;}
        double elapsed=now()-start;
        printf("{\"mode\":\"%s\",\"iteration\":%zu,\"source_bytes\":%zu,\"status\":%u,\"syntax_status\":%u,\"count\":%zu,\"seconds\":%.9f,\"tracked_allocator\":%s,",
            mode,i,size,(unsigned)r,(unsigned)status,count,elapsed,tracked?"true":"false");
        if(tracked)printf("\"peak_requested_bytes\":%zu,\"live_requested_bytes\":%zu,\"allocations\":%zu,\"denied\":%zu,",d.peak,d.live,d.attempts-attempts,d.denied);
        else printf("\"peak_requested_bytes\":null,\"live_requested_bytes\":null,\"allocations\":null,\"denied\":null,");
        printf("\"call\":%d}\n",call);
        if(d.invalid){exit_code=2;break;}
        if(r!=PS_OK){exit_code=1;break;}
    }
    ps_completion_locals_destroy(&p);ps_memory_free(allocator,nodes,capacity*sizeof *nodes);ps_memory_free(allocator,raw,capacity*sizeof *raw);free(inserted);
    return d.live||d.blocks||d.invalid?3:exit_code;
}
int main(int argc,char **argv){
    if(argc!=5 && argc!=6){fputs("usage: profile-completion file cursor cold|cursor|cached|edit|syntax|bindings iterations [default|tracked]\n",stderr);return 64;}
    size_t cursor,iterations;
    if(!number(argv[2],&cursor) || !number(argv[4],&iterations) || !iterations || iterations>1000)return 64;
    if(strcmp(argv[3],"cold") && strcmp(argv[3],"cursor") && strcmp(argv[3],"cached") && strcmp(argv[3],"edit") && strcmp(argv[3],"syntax") && strcmp(argv[3],"bindings"))return 64;
    if(argc==6 && strcmp(argv[5],"default") && strcmp(argv[5],"tracked"))return 64;
    FILE *f=fopen(argv[1],"rb");if(!f)return 65;
    if(fseek(f,0,SEEK_END)){fclose(f);return 65;}
    long n=ftell(f);if(n<0 || n>262144 || fseek(f,0,SEEK_SET)){fclose(f);return 65;}
    char *source=malloc((size_t)n+1);if(!source){fclose(f);return 66;}
    size_t read=fread(source,1,(size_t)n,f);int closed=fclose(f);
    if(read!=(size_t)n || closed){free(source);return 65;}source[n]=0;
    if(cursor>(size_t)n){free(source);return 64;}
    if(!strcmp(argv[3],"edit") && n==262144){free(source);return 64;}
    int result=sample(source,(size_t)n,cursor,argv[3],iterations,argc==5 || !strcmp(argv[5],"tracked"));free(source);return result;
}
