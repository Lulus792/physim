#include "physim/experiment.h"
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif
#define CHECK(x) do { if(!(x)) { fprintf(stderr,"Scene probe line %d: %s\n",__LINE__,#x); return 1; } } while(0)
int main(int argc,char **argv) {
    CHECK(argc==2);
#ifdef _WIN32
    wchar_t path[32768];
    CHECK(MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,argv[1],-1,path,32768)>0);
    HMODULE module=LoadLibraryW(path);
    CHECK(module);
    FARPROC symbol=GetProcAddress(module,"ps_get_experiment");
#else
    void *module=dlopen(argv[1],RTLD_NOW|RTLD_LOCAL);
    CHECK(module);
    void *symbol=dlsym(module,"ps_get_experiment");
#endif
    const ps_experiment_api *(*get)(void)=NULL;
    CHECK(symbol && sizeof get==sizeof symbol);memcpy(&get,&symbol,sizeof get);
    const ps_experiment_api *api=get();CHECK(api && api->abi_version==PS_ABI_VERSION);
    ps_context context={0};context.struct_size=sizeof context;context.api_version=PS_API_VERSION;context.seed=42;context.dt_s=.1;ps_rng_seed(&context.rng,42);
    CHECK(api->create(&context)==PS_OK && api->reset(&context)==PS_OK && context.channel_count==0);
    ps_scene first={0},second={0};api->build_scene(&context,&first);
    CHECK(first.count==8 && ps_scene_valid(&first));
    CHECK(api->step(&context,.1)==PS_OK);api->build_scene(&context,&second);
    CHECK(second.count==8 && ps_scene_valid(&second) && memcmp(&first,&second,sizeof first));
    puts(context.model_metadata);api->destroy(&context);
#ifdef _WIN32
    FreeLibrary(module);
#else
    dlclose(module);
#endif
    return 0;
}
