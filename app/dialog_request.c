#include "dialog_request.h"
#include "autosave.h"
#include <SDL3/SDL.h>
#include <string.h>
struct ps_dialog_request {
    ps_dialog_ticket ticket;
    SDL_AtomicInt references,claimed,done;
    ps_result status;
    char *path;
};
ps_dialog_request *ps_dialog_request_retain(ps_dialog_request *request) {
    if(request)SDL_AddAtomicInt(&request->references,1);
    return request;
}
void ps_dialog_request_release(ps_dialog_request *request) {
    if(request && SDL_AddAtomicInt(&request->references,-1)==1){SDL_free(request->path);SDL_free(request);}
}
ps_dialog_ticket ps_dialog_request_ticket(const ps_dialog_request *request) {
    return request?request->ticket:(ps_dialog_ticket){0};
}
ps_dialog_request *ps_dialog_request_find(const ps_dialog_requests *requests,ps_dialog_ticket ticket) {
    if(!requests || ticket.route>=PS_DIALOG_REQUEST_ROUTES || !ticket.version)return NULL;
    ps_dialog_request *request=requests->pending[ticket.route];
    return request && request->ticket.version==ticket.version?request:NULL;
}
ps_result ps_dialog_request_begin(ps_dialog_requests *requests,unsigned route,ps_dialog_request **out) {
    if(!requests || !out || route>=PS_DIALOG_REQUEST_ROUTES)return PS_INVALID;
    if(requests->versions[route]==UINT64_MAX)return PS_LIMIT;
    ps_dialog_request *request=SDL_calloc(1,sizeof *request);if(!request)return PS_MEMORY;
    request->ticket=(ps_dialog_ticket){route,requests->versions[route]+1};SDL_SetAtomicInt(&request->references,1);
    ps_dialog_request *previous=requests->pending[route];requests->pending[route]=request;
    requests->versions[route]=request->ticket.version;ps_dialog_request_release(previous);*out=request;return PS_OK;
}
void ps_dialog_request_cancel_route(ps_dialog_requests *requests,unsigned route) {
    if(!requests || route>=PS_DIALOG_REQUEST_ROUTES)return;
    ps_dialog_request *previous=requests->pending[route];requests->pending[route]=NULL;
    if(requests->versions[route]<UINT64_MAX)requests->versions[route]++;
    ps_dialog_request_release(previous);
}
bool ps_dialog_request_cancel(ps_dialog_requests *requests,ps_dialog_ticket ticket) {
    if(!ps_dialog_request_find(requests,ticket))return false;
    ps_dialog_request_cancel_route(requests,ticket.route);return true;
}
void ps_dialog_requests_cancel_all(ps_dialog_requests *requests) {
    if(requests)for(unsigned route=0;route<PS_DIALOG_REQUEST_ROUTES;route++)ps_dialog_request_cancel_route(requests,route);
}
bool ps_dialog_requests_pending(const ps_dialog_requests *requests) {
    if(requests)for(unsigned route=0;route<PS_DIALOG_REQUEST_ROUTES;route++)if(requests->pending[route])return true;
    return false;
}
bool ps_dialog_request_complete(ps_dialog_request *request,const char *path,bool error) {
    if(!request || !SDL_CompareAndSwapAtomicInt(&request->claimed,0,1))return false;
    ps_result status=error?PS_IO:PS_EOF;char *copy=NULL;
    if(!error && path && *path){
        size_t bytes=SDL_strnlen(path,PS_DIALOG_REQUEST_PATH);
        if(bytes==PS_DIALOG_REQUEST_PATH)status=PS_LIMIT;
        else if(!ps_source_text_valid(path,bytes))status=PS_CORRUPT;
        else {copy=SDL_strdup(path);status=copy?PS_OK:PS_MEMORY;}
    }
    request->path=copy;request->status=status;SDL_SetAtomicInt(&request->done,1);return true;
}
void ps_dialog_result_destroy(ps_dialog_result *result) {
    if(result){SDL_free(result->path);memset(result,0,sizeof *result);}
}
bool ps_dialog_request_poll(ps_dialog_requests *requests,unsigned route,ps_dialog_result *out) {
    if(!requests || !out || route>=PS_DIALOG_REQUEST_ROUTES)return false;
    ps_dialog_request *request=requests->pending[route];
    if(!request || !SDL_GetAtomicInt(&request->done))return false;
    ps_dialog_result_destroy(out);*out=(ps_dialog_result){request->ticket,request->status,request->path};request->path=NULL;
    requests->pending[route]=NULL;ps_dialog_request_release(request);return true;
}
