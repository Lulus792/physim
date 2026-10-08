#define SDL_MAIN_HANDLED
#include "profiling.h"
#include <SDL3/SDL_main.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"Profiler %d: %s\n",__LINE__,#x); return 1; } } while(0)
static uint64_t hash(const char *path) {
    FILE *f=fopen(path,"rb");if(!f)return 0;
    uint64_t value=UINT64_C(14695981039346656037);int c;
    while((c=fgetc(f))!=EOF){value^=(unsigned)c;value*=UINT64_C(1099511628211);}
    fclose(f);return value;
}
int main(int argc,char **argv) {
    CHECK(argc==2);SDL_SetMainReady();CHECK(SDL_Init(0));
    CHECK(!ps_app_profile_start(NULL) && !ps_app_profile_start(""));
    char root[4096],path[4096];snprintf(root,sizeof root,"%s/profile",argv[1]);
    ps_app_profile *p=ps_app_profile_start(root);CHECK(p);
    uint64_t accepted=0;const uint64_t attempts=30000;
    ps_app_profile_frame row={0};row.usage.peak_resident_bytes=1;row.rendered=true;
    for(uint64_t i=0;i<attempts;++i){row.frame=i;row.runner_bytes=i;if(ps_app_profile_record(p,&row))++accepted;}
    ps_app_profile_result result=ps_app_profile_finish(p);
    CHECK(!result.failed && result.written==accepted && result.written+result.dropped==attempts && accepted);
    snprintf(path,sizeof path,"%s/frames.csv",root);
    FILE *f=fopen(path,"rb");CHECK(f);char line[2048];CHECK(fgets(line,sizeof line,f));
    CHECK(strstr(line,"startup_ready_seconds") && strstr(line,"rendered"));
    uint64_t count=0,previous=0;
    while(fgets(line,sizeof line,f)) {unsigned long long index;CHECK(sscanf(line,"%llu,",&index)==1);
        CHECK(index<attempts && (!count || index>previous));previous=index;++count;}
    CHECK(!ferror(f) && !fclose(f) && count==accepted);
    uint64_t saved=hash(path);CHECK(saved);
    p=ps_app_profile_start(root);CHECK(p);result=ps_app_profile_finish(p);
    CHECK(result.failed && !result.written && hash(path)==saved);
    CHECK(!ps_app_profile_finish(NULL).failed);
    SDL_Quit();puts("Profiler queue: concurrent FIFO, exact accepted/dropped counts, drain and existing-file preservation verified");return 0;
}
