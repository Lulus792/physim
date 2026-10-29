#include "file_browser_worker.h"
#include "autosave.h"
#include <SDL3/SDL.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    ps_browser_request request;
    ps_browser_result result;
    SDL_Thread *thread;
    SDL_AtomicInt cancel, done;
} browser_job;
struct ps_browser_worker {
    ps_browser_request pending;
    ps_browser_worker_state state;
    browser_job *job;
    bool waiting;
};
/* Tests gate actual worker phases with semaphores. No instrumentation or
 * callback into owner/UI state exists in the production build. */
#ifdef PS_BROWSER_WORKER_TESTING
extern void ps_browser_worker_test_gate(uint64_t version,int phase);
#define TEST_GATE(v,p) ps_browser_worker_test_gate(v,p)
#else
#define TEST_GATE(v,p) ((void)0)
#endif
static bool cancelled(void *user) {return SDL_GetAtomicInt(&((browser_job *)user)->cancel)!=0;}
void ps_browser_result_destroy(ps_browser_result *result) {
    if(result){ps_browser_listing_destroy(&result->listing);free(result->text);free(result->api.document);memset(result,0,sizeof *result);}
}
/* Strict UTF-8, no replacement characters. A cut may discard only a valid,
 * unfinished final codepoint; malformed bytes within the prefix are errors.
 * Disallow NUL and binary controls, retaining TAB/LF/CR for source previews. */
static bool text_prefix(char *text,size_t *length,bool cut) {
    size_t at=0,n=*length;
    while(at<n) {
        unsigned char c=(unsigned char)text[at];unsigned count;
        if(c<0x80){if(c<32 && c!=9 && c!=10 && c!=13)return false;at++;continue;}
        if(c>=0xc2 && c<=0xdf)count=2;
        else if(c>=0xe0 && c<=0xef)count=3;
        else if(c>=0xf0 && c<=0xf4)count=4;
        else return false;
        size_t available=n-at;
        for(unsigned k=1;k<count && k<available;k++)if(((unsigned char)text[at+k]&0xc0)!=0x80)return false;
        if(available>1 && ((c==0xe0 && (unsigned char)text[at+1]<0xa0) ||
           (c==0xed && (unsigned char)text[at+1]>=0xa0) ||
           (c==0xf0 && (unsigned char)text[at+1]<0x90) ||
           (c==0xf4 && (unsigned char)text[at+1]>=0x90)))return false;
        if(available<count){if(!cut)return false;n=at;break;}
        at+=count;
    }
    text[n]=0;*length=n;return true;
}
static ps_result read_text_file(browser_job *job,ps_browser_result *result,const char *path,size_t limit) {
    if(cancelled(job))return PS_EOF;
    FILE *stream=NULL;ps_result status=ps_browser_open_file(path,&stream,&result->size);
    if(status!=PS_OK)return status;
    /* Three extra bytes cover a BOM plus a full 64 KiB text prefix. The extra
     * lookahead detects truncation from actual bytes, even if a writer grows
     * or shrinks the file after the initial stat. */
    const size_t capacity=limit+4;
    char *text=malloc(capacity+1);size_t used=0;
    if(!text){fclose(stream);return PS_MEMORY;}
    while(used<capacity && !cancelled(job)) {
        size_t bytes=capacity-used;if(bytes>4096)bytes=4096;
        size_t got=fread(text+used,1,bytes,stream);used+=got;
        TEST_GATE(job->result.version,2);
        if(got<bytes)break;
    }
    bool error=ferror(stream)!=0;if(fclose(stream))error=true;
    if(cancelled(job))status=PS_EOF;
    else if(error)status=PS_IO;
    else {
        size_t bom=used>=3 && !memcmp(text,"\xef\xbb\xbf",3)?3:0;
        if(bom){memmove(text,text+bom,used-bom);used-=bom;}
        bool cut=used>limit;
        size_t length=cut?limit:used;
        /* Validate the lookahead as well, then cut at a codepoint boundary.
         * This prevents accepting an invalid partial sequence at the limit. */
        size_t validate=used;
        if(!text_prefix(text,&validate,used==capacity-bom) ||
           !text_prefix(text,&length,cut))status=PS_CORRUPT;
        else {result->text=text;result->length=length;result->truncated=cut;text=NULL;}
    }
    free(text);return status;
}
static void read_api(browser_job *job) {
    ps_browser_result *result=&job->result;
    result->api.status=PS_EOF;
    if(!job->request.reference_root[0] || cancelled(job))return;
    result->api.symbol=ps_completion_c_reference(result->text,result->length,&result->api.offset);
    if(!result->api.symbol)return;
    char path[PS_BROWSER_PATH];
    int count=snprintf(path,sizeof path,"%s/docs/reference/%s.md",job->request.reference_root,result->api.symbol->module);
    if(count<0 || (size_t)count>=sizeof path){result->api.status=PS_LIMIT;return;}
    /* Read and parse away from the UI thread. A partial manual is never
     * presented as a complete section. Source bytes/status remain independent
     * when the installed reference is missing, invalid or too large. */
    ps_browser_result reference={0};
    ps_result status=read_text_file(job,&reference,path,PS_DOC_MAX_BYTES);
    if(status==PS_OK && reference.truncated)status=PS_LIMIT;
    ps_document *document=NULL;
    if(status==PS_OK && !cancelled(job))status=ps_document_parse(reference.text,reference.length,false,&document);
    if(status==PS_OK && !cancelled(job))status=ps_document_api_section(document,result->api.symbol->name,false,&result->api.first,&result->api.end);
    if(status==PS_OK && !cancelled(job))status=ps_document_api_preview(document,result->api.first,result->api.end,&result->api.preview);
    if(cancelled(job))status=PS_EOF;
    if(status==PS_OK){result->api.document=document;document=NULL;}
    free(document);ps_browser_result_destroy(&reference);result->api.status=status;
}
static ps_result read_text(browser_job *job) {
    ps_result status=read_text_file(job,&job->result,job->request.path,PS_BROWSER_PREVIEW_BYTES);
    job->result.api.status=PS_EOF;
    if(status==PS_OK)read_api(job);
    return status;
}
static int browser_work(void *user) {
    browser_job *job=user;ps_browser_request *request=&job->request;
    ps_browser_result *result=&job->result;
    TEST_GATE(result->version,0);
    if(cancelled(job))result->status=PS_EOF;
    else switch(request->operation) {
        case PS_BROWSER_READ_DIRECTORY:
            result->status=ps_browser_listing_read(request->path,request->limit,cancelled,job,&result->listing);break;
        case PS_BROWSER_READ_TEXT:result->status=read_text(job);break;
        case PS_BROWSER_CONFIRM:
            result->selectable=ps_browser_selectable(request->path,request->mode);
            result->status=result->selectable?PS_OK:PS_IO;break;
        case PS_BROWSER_MAKE_DIRECTORY:
            result->created=ps_browser_create_directory(request->path,request->name,result->path);
            result->status=result->created==PS_BROWSER_CREATED?PS_OK:
                result->created==PS_BROWSER_NAME_INVALID?PS_INVALID:PS_IO;break;
    }
    TEST_GATE(result->version,1);
    SDL_SetAtomicInt(&job->done,1);return 0;
}
static void job_destroy(browser_job *job) {
    if(job){SDL_SetAtomicInt(&job->cancel,1);if(job->thread)SDL_WaitThread(job->thread,NULL);
        ps_browser_result_destroy(&job->result);free(job);}
}
ps_browser_worker *ps_browser_worker_create(void) {return calloc(1,sizeof(ps_browser_worker));}
void ps_browser_worker_destroy(ps_browser_worker *worker) {
    if(worker){job_destroy(worker->job);free(worker);}
}
static bool valid_string(const char *text,size_t capacity) {
    size_t n=SDL_strnlen(text,capacity);return n>0 && n<capacity && ps_source_text_valid(text,n);
}
ps_result ps_browser_worker_request(ps_browser_worker *worker,const ps_browser_request *request) {
    if(!worker || !request || request->operation<PS_BROWSER_READ_DIRECTORY || request->operation>PS_BROWSER_MAKE_DIRECTORY ||
       request->mode<PS_BROWSER_FILES || request->mode>PS_BROWSER_RUNS ||
       !valid_string(request->path,sizeof request->path) ||
       (request->reference_root[0] && !valid_string(request->reference_root,sizeof request->reference_root)) ||
       (request->operation==PS_BROWSER_MAKE_DIRECTORY && !valid_string(request->name,sizeof request->name)))return PS_INVALID;
    /* Never wrap to an old version. Recreate the worker at this unreachable
     * practical limit rather than weakening the publication contract. */
    if(worker->state.requested==UINT64_MAX)return PS_LIMIT;
    worker->pending=*request;worker->state.requested++;worker->waiting=worker->state.loading=true;
    if(worker->job)SDL_SetAtomicInt(&worker->job->cancel,1);
    return PS_OK;
}
void ps_browser_worker_cancel(ps_browser_worker *worker) {
    if(!worker)return;
    if(worker->state.requested<UINT64_MAX)worker->state.requested++;
    worker->waiting=worker->state.loading=false;
    if(worker->job)SDL_SetAtomicInt(&worker->job->cancel,1);
}
bool ps_browser_worker_poll(ps_browser_worker *worker,ps_browser_result *out) {
    if(!worker || !out)return false;
    if(worker->job && SDL_GetAtomicInt(&worker->job->done)) {
        browser_job *job=worker->job;worker->job=NULL;
        SDL_WaitThread(job->thread,NULL);job->thread=NULL;
        /* loading also rejects cancel at the maximum version. Completion
         * alone never authorizes publishing a retired result. */
        bool publish=worker->state.loading && job->result.version==worker->state.requested;
        if(publish){ps_browser_result_destroy(out);*out=job->result;memset(&job->result,0,sizeof job->result);worker->state.loading=false;}
        else worker->state.discarded++;
        job_destroy(job);if(publish)return true;
    }
    if(!worker->job && worker->waiting) {
        worker->waiting=false;browser_job *job=calloc(1,sizeof *job);
        if(job){job->request=worker->pending;job->result.version=worker->state.requested;
            job->result.operation=job->request.operation;memcpy(job->result.path,job->request.path,sizeof job->result.path);
            job->thread=SDL_CreateThread(browser_work,"physim-explorer",job);}
        if(!job || !job->thread) {
            job_destroy(job);ps_browser_result_destroy(out);out->version=worker->state.requested;
            out->operation=worker->pending.operation;memcpy(out->path,worker->pending.path,sizeof out->path);
            out->status=PS_MEMORY;worker->state.loading=false;return true;
        }
        worker->job=job;
    }
    return false;
}
ps_browser_worker_state ps_browser_worker_read(const ps_browser_worker *worker) {
    return worker?worker->state:(ps_browser_worker_state){0};
}
