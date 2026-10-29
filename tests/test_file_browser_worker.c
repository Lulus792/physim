#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "file_browser_worker.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Browser worker line %d: %s\n",__LINE__,#x);exit(1);}} while(0)
static SDL_Semaphore *entered,*release;
static SDL_AtomicInt gate_version,gate_phase,gate_armed,gate_timeout;
void ps_browser_worker_test_gate(uint64_t version,int phase) {
    if(version==(uint64_t)SDL_GetAtomicInt(&gate_version) && phase==SDL_GetAtomicInt(&gate_phase) &&
       SDL_CompareAndSwapAtomicInt(&gate_armed,1,0)) {
        SDL_SignalSemaphore(entered);
        if(!SDL_WaitSemaphoreTimeout(release,10000))SDL_SetAtomicInt(&gate_timeout,1);
    }
}
static void gate(uint64_t version,int phase) {
    CHECK(version<INT32_MAX);SDL_SetAtomicInt(&gate_version,(int)version);
    SDL_SetAtomicInt(&gate_phase,phase);SDL_SetAtomicInt(&gate_armed,1);
}
static void write_bytes(const char *path,const void *bytes,size_t length) {
    SDL_IOStream *stream=SDL_IOFromFile(path,"wb");CHECK(stream);
    CHECK(SDL_WriteIO(stream,bytes,length)==length);CHECK(SDL_CloseIO(stream));
}
static ps_browser_request request(ps_browser_operation operation,const char *path) {
    ps_browser_request r={.operation=operation,.mode=PS_BROWSER_FILES};
    CHECK(SDL_strlcpy(r.path,path,sizeof r.path)<sizeof r.path);return r;
}
static void wait_result(ps_browser_worker *worker,ps_browser_result *result) {
    Uint64 end=SDL_GetTicks()+10000;
    while(!ps_browser_worker_poll(worker,result) && SDL_GetTicks()<end)SDL_Delay(1);
    CHECK(!ps_browser_worker_read(worker).loading);
    CHECK(result->version==ps_browser_worker_read(worker).requested);
    CHECK(!SDL_GetAtomicInt(&gate_timeout));
}
static void expect_text(ps_browser_worker *worker,const char *path,const char *text,size_t n,bool truncated) {
    ps_browser_request r=request(PS_BROWSER_READ_TEXT,path);ps_browser_result result={0};
    CHECK(ps_browser_worker_request(worker,&r)==PS_OK);wait_result(worker,&result);
    CHECK(result.status==PS_OK && result.text && result.length==n && result.truncated==truncated);
    CHECK(!memcmp(result.text,text,n) && result.text[n]==0);ps_browser_result_destroy(&result);
}
static int unblock(void *unused) {(void)unused;SDL_Delay(20);SDL_SignalSemaphore(release);return 0;}
static void api_context(ps_browser_worker *worker,const char *parent) {
    char root[PS_BROWSER_PATH],docs[PS_BROWSER_PATH],reference[PS_BROWSER_PATH],module[PS_BROWSER_PATH],path[PS_BROWSER_PATH];
    CHECK(ps_browser_create_directory(parent,"reference-root",root)==PS_BROWSER_CREATED);
    CHECK(ps_browser_create_directory(root,"docs",docs)==PS_BROWSER_CREATED);
    CHECK(ps_browser_create_directory(docs,"reference",reference)==PS_BROWSER_CREATED);
    CHECK(ps_browser_child(reference,"experiment.md",module)==PS_OK);
    CHECK(ps_browser_child(root,"Quelle λ.c",path)==PS_OK);
    const char *source="// ps_vlength(v)\nconst char *s = \"ps_vlength(v)\";\nps_result r = ps_scene_add_id(scene, 1, PS_SPHERE, a, b, 1, 0);\n";
    const char *manual="# Experiment\n\n## ps_scene_add_id\n\nEchte Beschreibung λ.\n\n```c\nps_result ps_scene_add_id(ps_scene *scene, uint32_t id);\n```\n\n| Argument | Bedeutung |\n| --- | --- |\n| scene | Zielszene |\n| id | Objektkennung |\n\n## Andere Funktion\n\nNicht Teil der Vorschau.\n";
    write_bytes(path,source,strlen(source));write_bytes(module,manual,strlen(manual));
    ps_browser_request r=request(PS_BROWSER_READ_TEXT,path);strcpy(r.reference_root,root);
    ps_browser_result result={0};CHECK(ps_browser_worker_request(worker,&r)==PS_OK);
    memset(&r,0xdd,sizeof r); /* Both file and documentation root are job-owned. */
    wait_result(worker,&result);
    CHECK(result.status==PS_OK && result.length==strlen(source) && !memcmp(result.text,source,result.length));
    CHECK(result.api.status==PS_OK && result.api.document && result.api.symbol);
    CHECK(!strcmp(result.api.symbol->name,"ps_scene_add_id") && !strcmp(result.api.symbol->module,"experiment"));
    CHECK(result.api.offset==(size_t)(strstr(source,"ps_scene_add_id")-source));
    ps_document *document=result.api.document;ps_doc_api_preview preview=result.api.preview;
    CHECK(preview.summary!=SIZE_MAX && preview.signature!=SIZE_MAX && preview.arguments!=SIZE_MAX && preview.example==SIZE_MAX);
    CHECK(!strcmp(document->text+document->blocks[preview.summary].text,"Echte Beschreibung λ."));
    CHECK(!strcmp(document->text+document->blocks[preview.signature].text,"ps_result ps_scene_add_id(ps_scene *scene, uint32_t id);\n"));
    CHECK(document->blocks[preview.arguments].table_rows==3 && result.api.end>result.api.first);
    /* A new request must neither mutate nor free the already published doc. */
    result.api.document=NULL;ps_browser_result_destroy(&result);
    r=request(PS_BROWSER_READ_TEXT,path);strcpy(r.reference_root,root);
    CHECK(ps_browser_worker_request(worker,&r)==PS_OK);uint64_t version=ps_browser_worker_read(worker).requested;
    ps_browser_request invalid=r;memset(invalid.reference_root,'x',sizeof invalid.reference_root);
    CHECK(ps_browser_worker_request(worker,&invalid)==PS_INVALID && ps_browser_worker_read(worker).requested==version);
    strcpy(invalid.reference_root,"\xc0\xaf");
    CHECK(ps_browser_worker_request(worker,&invalid)==PS_INVALID && ps_browser_worker_read(worker).requested==version);
    /* Retire a fully parsed document immediately before worker publication. */
    gate(version,1);CHECK(!ps_browser_worker_poll(worker,&result));CHECK(SDL_WaitSemaphoreTimeout(entered,5000));
    CHECK(!strcmp(document->text+document->blocks[preview.summary].text,"Echte Beschreibung λ."));free(document);
    ps_browser_worker_cancel(worker);SDL_SignalSemaphore(release);
    CHECK(ps_browser_worker_request(worker,&r)==PS_OK);wait_result(worker,&result);
    CHECK(result.status==PS_OK && result.api.status==PS_OK && result.api.document);ps_browser_result_destroy(&result);
    /* A missing, unrelated, invalid or oversized manual never hides the source. */
    CHECK(SDL_RemovePath(module));
    const char *bad[]={NULL,"# Unrelated\n\nNo matching section.\n","\xef\xbb\xbf\xed\xa0\x80"};
    const ps_result statuses[]={PS_IO,PS_INVALID,PS_CORRUPT};
    for(size_t k=0;k<3;k++) {
        if(bad[k])write_bytes(module,bad[k],strlen(bad[k]));
        CHECK(ps_browser_worker_request(worker,&r)==PS_OK);wait_result(worker,&result);
        CHECK(result.status==PS_OK && result.api.status==statuses[k] && !result.api.document && result.api.symbol);
        CHECK(result.length==strlen(source) && !memcmp(result.text,source,result.length));ps_browser_result_destroy(&result);
    }
    char *large=malloc(PS_DOC_MAX_BYTES+8);CHECK(large);memset(large,' ',PS_DOC_MAX_BYTES+8);memcpy(large,manual,strlen(manual));
    write_bytes(module,large,PS_DOC_MAX_BYTES+1);
    CHECK(ps_browser_worker_request(worker,&r)==PS_OK);wait_result(worker,&result);
    CHECK(result.status==PS_OK && result.api.status==PS_LIMIT && !result.api.document);ps_browser_result_destroy(&result);
    write_bytes(module,large,PS_DOC_MAX_BYTES);free(large);
    CHECK(ps_browser_worker_request(worker,&r)==PS_OK);wait_result(worker,&result);
    CHECK(result.status==PS_OK && result.api.status==PS_OK && result.api.document);ps_browser_result_destroy(&result);
    const char *bom="\xef\xbb\xbf";
    size_t bytes=strlen(manual);char *with_bom=malloc(bytes+3);CHECK(with_bom);memcpy(with_bom,bom,3);memcpy(with_bom+3,manual,bytes);
    write_bytes(module,with_bom,bytes+3);free(with_bom);
    CHECK(ps_browser_worker_request(worker,&r)==PS_OK);wait_result(worker,&result);
    CHECK(result.status==PS_OK && result.api.status==PS_OK && result.api.document);ps_browser_result_destroy(&result);
    r.reference_root[0]=0; /* Explicit opt-in, even when the source contains API calls. */
    CHECK(ps_browser_worker_request(worker,&r)==PS_OK);wait_result(worker,&result);
    CHECK(result.status==PS_OK && result.api.status==PS_EOF && !result.api.symbol && !result.api.document);ps_browser_result_destroy(&result);
    const char *plain="int main(void) {return 0;}\n";
    strcpy(r.reference_root,root);write_bytes(path,plain,strlen(plain));
    CHECK(ps_browser_worker_request(worker,&r)==PS_OK);wait_result(worker,&result);
    CHECK(result.status==PS_OK && result.api.status==PS_EOF && !result.api.symbol && !result.api.document);ps_browser_result_destroy(&result);
    puts("Browser C context: real symbol/module, owned parsed docs, stale completion, bounded UTF-8 and independent source errors passed");
}
int main(int argc,char **argv) {
    CHECK(argc==2 && SDL_Init(0));entered=SDL_CreateSemaphore(0);release=SDL_CreateSemaphore(0);CHECK(entered && release);
    char root[PS_BROWSER_PATH],a[PS_BROWSER_PATH],b[PS_BROWSER_PATH],path[PS_BROWSER_PATH],missing[PS_BROWSER_PATH];
    CHECK(ps_browser_create_directory(argv[1],"worker",root)==PS_BROWSER_CREATED);
    CHECK(ps_browser_create_directory(root,"A",a)==PS_BROWSER_CREATED);
    CHECK(ps_browser_create_directory(root,"B",b)==PS_BROWSER_CREATED);
    CHECK(ps_browser_child(root,"missing",missing)==PS_OK);
    CHECK(ps_browser_child(root,"Quelle λ.phys",path)==PS_OK);
    const char *source="// Quelle λ\r\nlet x = 1;\n";write_bytes(path,source,strlen(source));
    ps_browser_worker *worker=ps_browser_worker_create();CHECK(worker);ps_browser_result result={0};
    /* Actual I/O was completed by A, but no owner publication has occurred.
     * A late completion must not replace a newer B request; C is coalesced. */
    ps_browser_request r=request(PS_BROWSER_READ_DIRECTORY,a);
    CHECK(ps_browser_worker_request(worker,&r)==PS_OK);uint64_t first=ps_browser_worker_read(worker).requested;
    gate(first,1);CHECK(!ps_browser_worker_poll(worker,&result));CHECK(SDL_WaitSemaphoreTimeout(entered,5000));
    r=request(PS_BROWSER_READ_DIRECTORY,missing);CHECK(ps_browser_worker_request(worker,&r)==PS_OK);
    r=request(PS_BROWSER_READ_DIRECTORY,b);CHECK(ps_browser_worker_request(worker,&r)==PS_OK);
    memset(&r,0xdd,sizeof r); /* Caller storage is no longer owned by the job. */
    CHECK(!ps_browser_worker_poll(worker,&result) && !result.version);SDL_SignalSemaphore(release);
    wait_result(worker,&result);CHECK(result.status==PS_OK && !strcmp(result.listing.directory,b));
    CHECK(result.version==first+2 && ps_browser_worker_read(worker).discarded==1);
    /* Owner keeps a published listing until it chooses to replace it. An
     * active worker and cancel cannot mutate/free that transferred snapshot. */
    ps_browser_listing saved=result.listing;memset(&result.listing,0,sizeof result.listing);
    ps_browser_result_destroy(&result);
    r=request(PS_BROWSER_READ_TEXT,path);CHECK(ps_browser_worker_request(worker,&r)==PS_OK);
    uint64_t version=ps_browser_worker_read(worker).requested;gate(version,2);
    CHECK(!ps_browser_worker_poll(worker,&result));CHECK(SDL_WaitSemaphoreTimeout(entered,5000));
    ps_browser_worker_cancel(worker);CHECK(!ps_browser_worker_read(worker).loading);
    CHECK(!ps_browser_worker_poll(worker,&result) && !result.version);SDL_SignalSemaphore(release);
    /* A subsequent request is accepted while the cancelled thread unwinds. */
    expect_text(worker,path,source,strlen(source),false);
    CHECK(!strcmp(saved.directory,b));ps_browser_listing_destroy(&saved);
    FILE *in=NULL;uint64_t bytes=UINT64_MAX;CHECK(ps_browser_open_file(path,&in,&bytes)==PS_OK && bytes==strlen(source));
    CHECK(fgetc(in)=='/' && !fclose(in));in=NULL;bytes=777;
    CHECK(ps_browser_open_file(missing,&in,&bytes)==PS_IO && !in && bytes==777);
    CHECK(ps_browser_open_file(root,&in,&bytes)!=PS_OK && !in && bytes==777);
    /* UTF-8 BOM, exact boundary, cut in each multibyte width, malformed final
     * sequence, binary control and an empty file. No edited replacement text. */
    const char bom[]="\xef\xbb\xbfQuelle Ω\n";write_bytes(path,bom,sizeof bom-1);
    expect_text(worker,path,bom+3,sizeof bom-4,false);
    char *large=malloc(PS_BROWSER_PREVIEW_BYTES+8);CHECK(large);memset(large,'x',PS_BROWSER_PREVIEW_BYTES+8);
    write_bytes(path,large,PS_BROWSER_PREVIEW_BYTES);expect_text(worker,path,large,PS_BROWSER_PREVIEW_BYTES,false);
    const char *runes[]={"λ","€","😀"};
    for(size_t k=0;k<3;k++)for(size_t cut=1;cut<strlen(runes[k]);cut++) {
        size_t start=PS_BROWSER_PREVIEW_BYTES-cut;memset(large,'x',PS_BROWSER_PREVIEW_BYTES+8);
        memcpy(large+start,runes[k],strlen(runes[k]));write_bytes(path,large,PS_BROWSER_PREVIEW_BYTES+7);
        expect_text(worker,path,large,start,true);
    }
    const char *bad[]={"\xed\xa0\x80","\xf4\x90\x80\x80","\xc0\xaf","x\x01","\xe2\x82"};
    for(size_t k=0;k<sizeof bad/sizeof *bad;k++) {
        write_bytes(path,bad[k],strlen(bad[k]));r=request(PS_BROWSER_READ_TEXT,path);
        CHECK(ps_browser_worker_request(worker,&r)==PS_OK);wait_result(worker,&result);
        CHECK(result.status==PS_CORRUPT && !result.text);ps_browser_result_destroy(&result);
    }
    /* Invalid byte immediately beyond the limit is not silently accepted as
     * the continuation of a partial leading sequence. */
    memset(large,'x',PS_BROWSER_PREVIEW_BYTES+8);((unsigned char *)large)[PS_BROWSER_PREVIEW_BYTES-1]=0xe2;
    write_bytes(path,large,PS_BROWSER_PREVIEW_BYTES+7);r=request(PS_BROWSER_READ_TEXT,path);
    CHECK(ps_browser_worker_request(worker,&r)==PS_OK);wait_result(worker,&result);
    CHECK(result.status==PS_CORRUPT && !result.text);ps_browser_result_destroy(&result);free(large);
    write_bytes(path,"",0);expect_text(worker,path,"",0,false);
    /* A stale file selection cannot be confirmed using cached metadata. */
    r=request(PS_BROWSER_CONFIRM,path);CHECK(ps_browser_worker_request(worker,&r)==PS_OK);wait_result(worker,&result);
    CHECK(result.status==PS_OK && result.selectable);CHECK(SDL_RemovePath(path));
    CHECK(ps_browser_worker_request(worker,&r)==PS_OK);wait_result(worker,&result);
    CHECK(result.status==PS_IO && !result.selectable);ps_browser_result_destroy(&result);
    /* Exclusive mutation and an invalid request which must not supersede the
     * valid pending create. Collision leaves the original directory intact. */
    r=request(PS_BROWSER_MAKE_DIRECTORY,root);strcpy(r.name,"Neu λ");
    CHECK(ps_browser_worker_request(worker,&r)==PS_OK);version=ps_browser_worker_read(worker).requested;
    ps_browser_request invalid=r;memset(invalid.path,'x',sizeof invalid.path);
    CHECK(ps_browser_worker_request(worker,&invalid)==PS_INVALID && ps_browser_worker_read(worker).requested==version);
    wait_result(worker,&result);CHECK(result.status==PS_OK && result.created==PS_BROWSER_CREATED);
    CHECK(ps_browser_selectable(result.path,PS_BROWSER_FOLDERS));
    CHECK(ps_browser_worker_request(worker,&r)==PS_OK);wait_result(worker,&result);
    CHECK(result.status==PS_IO && result.created==PS_BROWSER_COLLISION);ps_browser_result_destroy(&result);
    /* Cancel after mkdir cannot promise a rollback, but cannot publish success
     * into a dismissed picker either. The filesystem effect is explicit. */
    strcpy(r.name,"Completed before cancel");CHECK(ps_browser_worker_request(worker,&r)==PS_OK);
    gate(ps_browser_worker_read(worker).requested,1);CHECK(!ps_browser_worker_poll(worker,&result));
    CHECK(SDL_WaitSemaphoreTimeout(entered,5000));ps_browser_worker_cancel(worker);SDL_SignalSemaphore(release);
    CHECK(ps_browser_child(root,r.name,path)==PS_OK && ps_browser_selectable(path,PS_BROWSER_FOLDERS));
    r=request(PS_BROWSER_READ_DIRECTORY,root);CHECK(ps_browser_worker_request(worker,&r)==PS_OK);wait_result(worker,&result);
    CHECK(result.status==PS_OK && result.listing.count==4);ps_browser_result_destroy(&result);
    api_context(worker,b);
#ifndef _WIN32
    /* DrvFS on the required D: build directory cannot create named FIFOs.
     * An actual anonymous pipe exercises special-file rejection without any
     * fixture data or temporary directory outside D:. No writer is retained. */
    int descriptors[2];CHECK(!pipe(descriptors) && !close(descriptors[1]));
#ifdef __linux__
    CHECK(snprintf(path,sizeof path,"/proc/self/fd/%d",descriptors[0])<(int)sizeof path);
#else
    CHECK(snprintf(path,sizeof path,"/dev/fd/%d",descriptors[0])<(int)sizeof path);
#endif
    bytes=777;in=NULL;
    Uint64 start=SDL_GetTicks();CHECK(ps_browser_open_file(path,&in,&bytes)==PS_INVALID && !in && bytes==777);
    CHECK(!close(descriptors[0]));
    CHECK(SDL_GetTicks()-start<1000);
    /* Kernel link/.. semantics must survive the read-only file adapter too. */
    char nested[PS_BROWSER_PATH],link[PS_BROWSER_PATH];
    CHECK(ps_browser_create_directory(b,"nested",nested)==PS_BROWSER_CREATED);
    CHECK(ps_browser_child(b,"actual.txt",path)==PS_OK);write_bytes(path,source,strlen(source));
    CHECK(ps_browser_child(a,"link",link)==PS_OK && !symlink(nested,link));
    CHECK(snprintf(path,sizeof path,"%s/../actual.txt",link)<(int)sizeof path);
    expect_text(worker,path,source,strlen(source),false);
#endif
    /* Closing/reopening invalidates an already-computed listing of the same
     * path. Shutdown joins its cancelled worker before freeing job storage. */
    r=request(PS_BROWSER_READ_DIRECTORY,root);CHECK(ps_browser_worker_request(worker,&r)==PS_OK);
    gate(ps_browser_worker_read(worker).requested,1);CHECK(!ps_browser_worker_poll(worker,&result));
    CHECK(SDL_WaitSemaphoreTimeout(entered,5000));ps_browser_worker_cancel(worker);
    SDL_Thread *unblocker=SDL_CreateThread(unblock,"test-unblock",NULL);CHECK(unblocker);
    ps_browser_worker_destroy(worker);SDL_WaitThread(unblocker,NULL);CHECK(!SDL_GetAtomicInt(&gate_timeout));
    worker=ps_browser_worker_create();CHECK(worker && ps_browser_worker_request(worker,&r)==PS_OK);
    wait_result(worker,&result);CHECK(result.status==PS_OK);ps_browser_result_destroy(&result);
    /* The filesystem model's long-path support must also reach the opened
     * preview handle, rather than ending at a successful directory stat. */
    strcpy(path,root);
    for(unsigned k=0;k<16;k++) {
        char component[64],child[PS_BROWSER_PATH];snprintf(component,sizeof component,"long-segment-%02u-for-preview",k);
        CHECK(ps_browser_create_directory(path,component,child)==PS_BROWSER_CREATED);strcpy(path,child);
    }
    char long_file[PS_BROWSER_PATH];CHECK(ps_browser_child(path,"Quelle λ.txt",long_file)==PS_OK);
    CHECK(strlen(long_file)>260);
#ifdef _WIN32
    char native[PS_BROWSER_PATH+8];CHECK(snprintf(native,sizeof native,"\\\\?\\%s",long_file)<(int)sizeof native);
    for(char *at=native;*at;at++)if(*at=='/')*at='\\';
    wchar_t wide[PS_BROWSER_PATH+8];CHECK(MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,native,-1,wide,PS_BROWSER_PATH+8));
    FILE *out=_wfopen(wide,L"wb");CHECK(out && fwrite(source,1,strlen(source),out)==strlen(source) && !fclose(out));
#else
    write_bytes(long_file,source,strlen(source));
#endif
    expect_text(worker,long_file,source,strlen(source),false);
    printf("{\"kind\":\"browser_worker\",\"preview_limit\":%u,\"long_file_bytes\":%zu,\"stale_completion_gated\":true,\"mid_read_cancel_gated\":true}\n",PS_BROWSER_PREVIEW_BYTES,strlen(long_file));
    ps_browser_worker_destroy(worker);SDL_DestroySemaphore(entered);SDL_DestroySemaphore(release);SDL_Quit();
    puts("Browser worker: actual stale completion, coalescing, owned requests/results, mid-read cancel, exclusive mutation, strict bounded UTF-8 and shutdown passed");
    return 0;
}
