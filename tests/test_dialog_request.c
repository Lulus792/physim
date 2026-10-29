#include "dialog_request.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Dialog request line%d: %s\n",__LINE__,#x);return 1;}}while(0)
typedef struct {ps_dialog_request *request;SDL_Semaphore *ready,*go;const char *path;bool completed;} reply;
static int callback(void *user) {
    reply *r=user;SDL_SignalSemaphore(r->ready);
    if(!SDL_WaitSemaphoreTimeout(r->go,5000)){ps_dialog_request_release(r->request);return 1;}
    r->completed=ps_dialog_request_complete(r->request,r->path,false);ps_dialog_request_release(r->request);return 0;
}
int main(void) {
    CHECK(SDL_Init(0));ps_dialog_requests requests={0};ps_dialog_request *one=NULL,*two=NULL;ps_dialog_result result={0};
    CHECK(!ps_dialog_requests_pending(&requests));
    CHECK(ps_dialog_request_begin(&requests,1,&one)==PS_OK);ps_dialog_ticket first=ps_dialog_request_ticket(one);
    CHECK(first.route==1 && first.version && ps_dialog_request_find(&requests,first)==one);
    CHECK(!ps_dialog_request_poll(&requests,1,&result) && !result.path);
    CHECK(ps_dialog_request_begin(&requests,4,&two)==PS_OK && ps_dialog_requests_pending(&requests));
    char original[]="D:/Projekt λ Á 🌍/Modell.c";CHECK(ps_dialog_request_complete(one,original,false));
    original[0]='X';CHECK(!ps_dialog_request_complete(one,"D:/wrong",false));
    CHECK(ps_dialog_request_poll(&requests,1,&result) && result.status==PS_OK && result.path[0]=='D' && !strcmp(result.path+1,original+1));
    CHECK(!ps_dialog_request_find(&requests,first) && ps_dialog_requests_pending(&requests));
    char *owned=result.path;result.path=NULL;ps_dialog_result_destroy(&result);
    CHECK(ps_dialog_request_complete(two,"",false) && ps_dialog_request_poll(&requests,4,&result));
    CHECK(result.status==PS_EOF && !result.path && !ps_dialog_requests_pending(&requests));SDL_free(owned);
    /* A cancelled producer may finish arbitrarily late, even after its registry
     * owner has gone. It must not publish over a newer request on that route. */
    SDL_Semaphore *ready=SDL_CreateSemaphore(0),*go=SDL_CreateSemaphore(0);CHECK(ready && go);
    CHECK(ps_dialog_request_begin(&requests,4,&one)==PS_OK);first=ps_dialog_request_ticket(one);
    reply late={ps_dialog_request_retain(one),ready,go,"D:/old",false};SDL_Thread *thread=SDL_CreateThread(callback,"dialog-late",&late);CHECK(thread && SDL_WaitSemaphoreTimeout(ready,5000));
    CHECK(ps_dialog_request_begin(&requests,4,&two)==PS_OK);ps_dialog_ticket current=ps_dialog_request_ticket(two);
    CHECK(!ps_dialog_request_cancel(&requests,first) && ps_dialog_request_find(&requests,current)==two);
    SDL_SignalSemaphore(go);int code=-1;SDL_WaitThread(thread,&code);CHECK(code==0 && late.completed);
    CHECK(!ps_dialog_request_poll(&requests,4,&result));
    CHECK(ps_dialog_request_complete(two,"D:/new",false) && ps_dialog_request_poll(&requests,4,&result));
    CHECK(result.status==PS_OK && !strcmp(result.path,"D:/new"));ps_dialog_result_destroy(&result);
    CHECK(ps_dialog_request_begin(&requests,2,&one)==PS_OK);
    late=(reply){ps_dialog_request_retain(one),ready,go,"D:/after shutdown",false};thread=SDL_CreateThread(callback,"dialog-shutdown",&late);
    CHECK(thread && SDL_WaitSemaphoreTimeout(ready,5000));ps_dialog_requests_cancel_all(&requests);
    SDL_SignalSemaphore(go);SDL_WaitThread(thread,&code);CHECK(code==0 && late.completed && !ps_dialog_requests_pending(&requests));
    /* Two actual producers race; only one can own the immutable completion. */
    for(unsigned i=0;i<32;i++){
        CHECK(ps_dialog_request_begin(&requests,3,&one)==PS_OK);
        reply a={ps_dialog_request_retain(one),ready,go,"D:/A",false},b={ps_dialog_request_retain(one),ready,go,"D:/B",false};
        SDL_Thread *ta=SDL_CreateThread(callback,"dialog-one",&a),*tb=SDL_CreateThread(callback,"dialog-two",&b);
        CHECK(ta && tb && SDL_WaitSemaphoreTimeout(ready,5000) && SDL_WaitSemaphoreTimeout(ready,5000));
        SDL_SignalSemaphore(go);SDL_SignalSemaphore(go);int ca,cb;SDL_WaitThread(ta,&ca);SDL_WaitThread(tb,&cb);
        CHECK(!ca && !cb && a.completed!=b.completed && ps_dialog_request_poll(&requests,3,&result));
        CHECK(result.status==PS_OK && (!strcmp(result.path,"D:/A") || !strcmp(result.path,"D:/B")));ps_dialog_result_destroy(&result);
    }
    const char *paths[]={NULL,"","\xed\xa0\x80","D:/bad\x01","D:/ordinary","D:/native-\xc2\x85-name","D:/native-\t\n-name"};
    const ps_result statuses[]={PS_EOF,PS_EOF,PS_CORRUPT,PS_CORRUPT,PS_OK,PS_OK,PS_OK};
    for(unsigned i=0;i<sizeof paths/sizeof *paths;i++){
        CHECK(ps_dialog_request_begin(&requests,1,&one)==PS_OK && ps_dialog_request_complete(one,paths[i],false));
        CHECK(ps_dialog_request_poll(&requests,1,&result) && result.status==statuses[i]);ps_dialog_result_destroy(&result);
    }
    char large[PS_DIALOG_REQUEST_PATH+1];memset(large,'x',sizeof large);large[sizeof large-1]=0;
    CHECK(ps_dialog_request_begin(&requests,1,&one)==PS_OK && ps_dialog_request_complete(one,large,false));
    CHECK(ps_dialog_request_poll(&requests,1,&result) && result.status==PS_LIMIT && !result.path);
    CHECK(ps_dialog_request_begin(&requests,1,&one)==PS_OK && ps_dialog_request_complete(one,"D:/ignored",true));
    CHECK(ps_dialog_request_poll(&requests,1,&result) && result.status==PS_IO && !result.path);
    CHECK(ps_dialog_request_begin(&requests,1,&one)==PS_OK);first=ps_dialog_request_ticket(one);
    ps_dialog_request *sentinel=one;ps_dialog_requests saved=requests;
    CHECK(ps_dialog_request_begin(&requests,PS_DIALOG_REQUEST_ROUTES,&sentinel)==PS_INVALID && sentinel==one && !memcmp(&saved,&requests,sizeof saved));
    CHECK(ps_dialog_request_begin(&requests,1,NULL)==PS_INVALID && !memcmp(&saved,&requests,sizeof saved));
    requests.versions[1]=UINT64_MAX;
    CHECK(ps_dialog_request_begin(&requests,1,&sentinel)==PS_LIMIT && sentinel==one && ps_dialog_request_find(&requests,first)==one);
    CHECK(ps_dialog_request_cancel(&requests,first) && !ps_dialog_request_find(&requests,first));
    CHECK(ps_dialog_request_begin(&requests,1,&sentinel)==PS_LIMIT);
    ps_dialog_requests_cancel_all(&requests);ps_dialog_result_destroy(&result);SDL_DestroySemaphore(ready);SDL_DestroySemaphore(go);SDL_Quit();
    puts("Dialog requests: independent routes, owned immutable completion, retired replies, late shutdown, 32 producer races, UTF-8/limit and overflow passed");return 0;
}
