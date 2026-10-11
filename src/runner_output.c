#include "runner_output.h"
#include "platform.h"
#include "protocol.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif
#define OUTPUT_LIMIT (8u*1024u*1024u)
typedef struct output_frame {
    struct output_frame *next;
    size_t bytes;
    uint32_t type;
    bool preview;
    unsigned char data[];
} output_frame;
struct ps_runner_output {
    output_frame *head,*tail;
    size_t queued;
    bool closing,done,failed,overflow;
#ifdef _WIN32
    CRITICAL_SECTION mutex;
    CONDITION_VARIABLE ready;
    HANDLE thread;
#else
    pthread_mutex_t mutex;
    pthread_cond_t ready;
    pthread_t thread;
#endif
};
static void lock_output(ps_runner_output *o) {
#ifdef _WIN32
    EnterCriticalSection(&o->mutex);
#else
    pthread_mutex_lock(&o->mutex);
#endif
}
static void unlock_output(ps_runner_output *o) {
#ifdef _WIN32
    LeaveCriticalSection(&o->mutex);
#else
    pthread_mutex_unlock(&o->mutex);
#endif
}
static void wake_output(ps_runner_output *o) {
#ifdef _WIN32
    WakeConditionVariable(&o->ready);
#else
    pthread_cond_signal(&o->ready);
#endif
}
static bool write_output(const unsigned char *data,size_t bytes) {
    while(bytes) {
#ifdef _WIN32
        DWORD written=0;
        if(!WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),data,(DWORD)bytes,&written,NULL) || !written)return false;
#else
        ssize_t written=write(STDOUT_FILENO,data,bytes);
        if(written<0 && errno==EINTR)continue;
        if(written<0 && (errno==EAGAIN || errno==EWOULDBLOCK)){ps_sleep(1);continue;}
        if(written<=0)return false;
#endif
        data+=(size_t)written;bytes-=(size_t)written;
    }
    return true;
}
static void output_worker(ps_runner_output *o) {
    uint32_t sequence=0;
    for(;;) {
        lock_output(o);
        while(!o->head && !o->closing) {
#ifdef _WIN32
            SleepConditionVariableCS(&o->ready,&o->mutex,INFINITE);
#else
            pthread_cond_wait(&o->ready,&o->mutex);
#endif
        }
        output_frame *frame=o->head;
        if(!frame){o->done=true;unlock_output(o);return;}
        o->head=frame->next;if(!o->head)o->tail=NULL;o->queued-=sizeof *frame+frame->bytes;
        unlock_output(o);
        ps_put_u32(frame->data+16,sequence++);
        bool ok=write_output(frame->data,frame->bytes);free(frame);
        if(!ok){lock_output(o);o->failed=o->done=true;unlock_output(o);return;}
    }
}
#ifdef _WIN32
static DWORD WINAPI output_thread(void *user){output_worker(user);return 0;}
#else
static void *output_thread(void *user){output_worker(user);return NULL;}
#endif
ps_runner_output *ps_runner_output_create(void) {
    ps_runner_output *o=calloc(1,sizeof *o);if(!o)return NULL;
#ifdef _WIN32
    if(!InitializeCriticalSectionEx(&o->mutex,0,0)){free(o);return NULL;}
    InitializeConditionVariable(&o->ready);
    o->thread=CreateThread(NULL,262144,output_thread,o,0,NULL);
    if(!o->thread){DeleteCriticalSection(&o->mutex);free(o);return NULL;}
#else
    if(pthread_mutex_init(&o->mutex,NULL)){free(o);return NULL;}
    if(pthread_cond_init(&o->ready,NULL)){pthread_mutex_destroy(&o->mutex);free(o);return NULL;}
    pthread_attr_t attr;
    if(pthread_attr_init(&attr)){pthread_cond_destroy(&o->ready);pthread_mutex_destroy(&o->mutex);free(o);return NULL;}
    int error=pthread_attr_setstacksize(&attr,262144);
    if(!error)error=pthread_create(&o->thread,&attr,output_thread,o);
    pthread_attr_destroy(&attr);
    if(error){pthread_cond_destroy(&o->ready);pthread_mutex_destroy(&o->mutex);free(o);return NULL;}
#endif
    return o;
}
bool ps_runner_output_send(ps_runner_output *o,uint32_t type,const void *payload,uint32_t size,bool preview) {
    if(!o || size>PS_WIRE_MAX || (!payload && size) || (preview && type!=PS_MSG_SNAPSHOT))return false;
    output_frame *frame=malloc(sizeof *frame+PS_WIRE_HEADER+size);if(!frame)return false;
    frame->next=NULL;frame->bytes=PS_WIRE_HEADER+size;frame->type=type;frame->preview=preview;
    if(!ps_wire_encode(frame->data,type,0,payload,size)){free(frame);return false;}
    lock_output(o);
    if(o->failed || o->overflow || o->closing){unlock_output(o);free(frame);return false;}
    /* Remove obsolete previews, then append at the tail: replacing in place
     * could move a newer running state ahead of a reliable paused state. */
    if(preview || type==PS_MSG_HEARTBEAT) {
        output_frame *previous=NULL,*at=o->head;
        while(at) {
            if((preview && at->preview) || (type==PS_MSG_HEARTBEAT && at->type==type)) {
                if(previous)previous->next=at->next;else o->head=at->next;
                if(o->tail==at)o->tail=previous;o->queued-=sizeof *at+at->bytes;free(at);break;
            }
            previous=at;at=at->next;
        }
    }
    if(sizeof *frame+frame->bytes>OUTPUT_LIMIT-o->queued){o->overflow=true;unlock_output(o);free(frame);return false;}
    if(o->tail)o->tail->next=frame;else o->head=frame;o->tail=frame;o->queued+=sizeof *frame+frame->bytes;
    wake_output(o);unlock_output(o);return true;
}
bool ps_runner_output_healthy(ps_runner_output *o) {
    if(!o)return false;lock_output(o);bool ok=!o->failed && !o->overflow && !o->closing;unlock_output(o);return ok;
}
bool ps_runner_output_finish(ps_runner_output *o,double timeout) {
    if(!o)return true;
    lock_output(o);o->closing=true;wake_output(o);unlock_output(o);
    double deadline=ps_clock()+timeout;bool done;
    do{lock_output(o);done=o->done;unlock_output(o);if(done)break;ps_sleep(1);}while(ps_clock()<deadline);
    if(!done)return false;
#ifdef _WIN32
    WaitForSingleObject(o->thread,INFINITE);CloseHandle(o->thread);DeleteCriticalSection(&o->mutex);
#else
    pthread_join(o->thread,NULL);pthread_cond_destroy(&o->ready);pthread_mutex_destroy(&o->mutex);
#endif
    bool ok=!o->failed && !o->overflow;
    while(o->head){output_frame *next=o->head->next;free(o->head);o->head=next;}
    free(o);return ok;
}
