#include "run_import.h"
#include "physim/run_stream.h"
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_filesystem.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
static ps_result copy(const char *source,const char *destination) {
    FILE *in=fopen(source,"rb");if(!in)return errno==ENOENT?PS_EOF:PS_IO;
    FILE *out=fopen(destination,"wbx");if(!out){fclose(in);return PS_IO;}
    unsigned char bytes[32768];bool ok=true;size_t n;
    while((n=fread(bytes,1,sizeof bytes,in))!=0)if(fwrite(bytes,1,n,out)!=n){ok=false;break;}
    if(ferror(in))ok=false;
    if(fclose(in))ok=false;
    if(fclose(out))ok=false;
    if(!ok)SDL_RemovePath(destination);
    return ok?PS_OK:PS_IO;
}
static bool publish(const char *temporary,const char *destination) {
#ifdef _WIN32
    wchar_t from[4096],to[4096];
    if(!MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,temporary,-1,from,4096) ||
       !MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,destination,-1,to,4096))return false;
    return MoveFileExW(from,to,MOVEFILE_WRITE_THROUGH)!=0; /* No replacement flag. */
#else
    if(link(temporary,destination))return false;
    (void)unlink(temporary);return true;
#endif
}
static ps_result validate(const char *path) {
    ps_run_store *store;ps_result r=ps_run_store_create(ps_allocator_default(),&store);
    if(r!=PS_OK)return r;
    ps_run_read_handle reader;r=ps_run_reader_open(store,path,&reader);
    if(r!=PS_OK){ps_run_store_destroy(store);return r;}
    double time,values[PS_MAX_CHANNELS],previous=0;bool have=false;size_t count;
    while((r=ps_run_reader_next(store,reader,&time,values,PS_MAX_CHANNELS,&count))==PS_OK) {
        if(have && time<=previous){r=PS_CORRUPT;break;}previous=time;have=true;
    }
    ps_run_reader_release(store,reader);
    if(r!=PS_EOF && r!=PS_RECOVERED){ps_run_store_destroy(store);return r;}
    bool recovered=r==PS_RECOVERED;
    r=ps_run_reader_open(store,path,&reader);
    if(r!=PS_OK){ps_run_store_destroy(store);return r;}
    ps_snapshot snapshot;
    while((r=ps_run_reader_snapshot_next(store,reader,&snapshot))==PS_OK){}
    ps_run_reader_release(store,reader);ps_run_store_destroy(store);
    return r==PS_EOF || r==PS_RECOVERED?(recovered || r==PS_RECOVERED?PS_RECOVERED:PS_OK):r;
}
ps_result ps_run_import(const char *source,const char *destination) {
    if(!source || !*source || !destination || !*destination)return PS_INVALID;
    char temporary[4096],from[4096],to[4096];
    const char *suffix[]={"",".experiment.c",".experiment.phys",".limits.txt"};
    bool copied[4]={false},published[4]={false};ps_result result=PS_OK;
    unsigned long long stamp=(unsigned long long)SDL_GetTicksNS();
    for(unsigned i=0;i<4;i++) {
        int a=snprintf(temporary,sizeof temporary,"%s.import-%llu-%u",destination,stamp,i);
        int b=snprintf(from,sizeof from,"%s%s",source,suffix[i]);
        int c=snprintf(to,sizeof to,"%s%s",destination,suffix[i]);
        if(a<0 || b<0 || c<0 || (size_t)a>=sizeof temporary || (size_t)b>=sizeof from || (size_t)c>=sizeof to){result=PS_LIMIT;break;}
        SDL_PathInfo existing;
        if(SDL_GetPathInfo(to,&existing)){result=PS_IO;break;}
        ps_result r=copy(from,temporary);
        if(i && r==PS_EOF)continue;
        if(r!=PS_OK){result=r;break;}copied[i]=true;
        if(!i) {result=validate(temporary);if(result!=PS_OK && result!=PS_RECOVERED)break;}
    }
    if(result==PS_OK || result==PS_RECOVERED)for(unsigned i=0;i<4;i++)if(copied[i]) {
        snprintf(temporary,sizeof temporary,"%s.import-%llu-%u",destination,stamp,i);
        snprintf(to,sizeof to,"%s%s",destination,suffix[i]);
        if(!publish(temporary,to)){result=PS_IO;break;}published[i]=true;
    }
    for(unsigned i=0;i<4;i++)if(copied[i]) {
        snprintf(temporary,sizeof temporary,"%s.import-%llu-%u",destination,stamp,i);SDL_RemovePath(temporary);
        if(published[i] && result!=PS_OK && result!=PS_RECOVERED){snprintf(to,sizeof to,"%s%s",destination,suffix[i]);SDL_RemovePath(to);}
    }
    return result;
}
