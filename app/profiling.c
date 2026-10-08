#include "profiling.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PROFILE_CAPACITY 1025u
typedef struct {
    bool process;
    uint64_t sequence;
    union { ps_app_profile_frame frame; ps_app_profile_process child; } value;
} profile_item;
struct ps_app_profile {
    SDL_Thread *thread;
    SDL_AtomicInt stopping, failed, read_index, write_index;
    profile_item queue[PROFILE_CAPACITY];
    uint64_t written, dropped, process_written, process_dropped, process_sequence;
    char directory[3900];
};
static void row(FILE *f, const ps_app_profile_frame *r) {
    fprintf(f,"%llu,%.9f,%.9f,%.9f,%.9f,%.9f,%.9f,%.9f,%.9f,%.9f,%.9f,%.9f,"
              "%.9f,%.9f,%llu,%llu,%llu,%u,%u,%.9f,%.9f,%.9f,%zu,%zu,%zu,%.9f,%.9f,%zu,%zu,%u\n",
            (unsigned long long)r->frame,r->time_seconds,r->interval_seconds,r->frame_seconds,
            r->event_seconds,r->work_seconds,r->ui_seconds,r->render_seconds,r->present_seconds,
            r->documentation_seconds,r->capture_seconds,r->startup_ready_seconds,
            r->usage.user_seconds,r->usage.system_seconds,
            (unsigned long long)r->usage.peak_resident_bytes,
            (unsigned long long)r->runner_bytes,(unsigned long long)r->job_bytes,
            r->captured?1u:0u,r->has_scene?1u:0u,
            r->scene.setup_seconds,r->scene.tessellation_seconds,r->scene.submission_seconds,
            r->scene.vertex_bytes,r->scene.index_bytes,r->scene.retained_bytes,
            r->ui.conversion_seconds,r->ui.upload_seconds,r->ui.vertex_bytes,r->ui.index_bytes,r->rendered?1u:0u);
}
static void process_row(FILE *f,const profile_item *item) {
    const ps_app_profile_process *r=&item->value.child;
    char user[64]="",system[64]="",peak[64]="";
    if(r->available) {
        snprintf(user,sizeof user,"%.9f",r->usage.user_seconds);
        snprintf(system,sizeof system,"%.9f",r->usage.system_seconds);
        snprintf(peak,sizeof peak,"%llu",(unsigned long long)r->usage.peak_resident_bytes);
    }
    fprintf(f,"%llu,%llu,%.9f,%d,%d,%u,%u,%u,%s,%s,%s\n",
            (unsigned long long)item->sequence,(unsigned long long)r->process_id,r->time_seconds,
            r->kind,r->exit_code,r->timed_out?1u:0u,r->available?1u:0u,(unsigned)r->scope,user,system,peak);
}
static int worker(void *data) {
    ps_app_profile *p=data;
    char path[4096]; FILE *f=NULL,*processes=NULL;
    if (!ps_make_directory_exclusive(p->directory)) goto failed;
    snprintf(path,sizeof path,"%s/frames.csv",p->directory);
    f=fopen(path,"wbx"); if(!f)goto failed;
    fputs("frame,time_seconds,interval_seconds,frame_seconds,event_seconds,work_seconds,ui_seconds,"
          "render_seconds,present_seconds,documentation_seconds,capture_seconds,startup_ready_seconds,"
          "user_cpu_seconds,system_cpu_seconds,peak_resident_bytes,runner_bytes,job_bytes,captured,has_scene,"
          "scene_setup_seconds,scene_tessellation_seconds,scene_submission_seconds,scene_vertex_bytes,"
          "scene_index_bytes,scene_retained_bytes,ui_conversion_seconds,ui_upload_seconds,ui_vertex_bytes,ui_index_bytes,rendered\n",f);
    snprintf(path,sizeof path,"%s/processes.csv",p->directory);
    processes=fopen(path,"wbx");if(!processes)goto failed;
    fputs("sequence,process_id,time_seconds,kind,exit_code,timed_out,usage_available,usage_scope,user_cpu_seconds,system_cpu_seconds,peak_resident_bytes\n",processes);
    for (;;) {
        profile_item batch[32]; unsigned count=0;
        unsigned read=(unsigned)SDL_GetAtomicInt(&p->read_index);
        unsigned write=(unsigned)SDL_GetAtomicInt(&p->write_index);
        while(count<32 && read!=write) {
            batch[count++]=p->queue[read];read=(read+1)%PROFILE_CAPACITY;
        }
        /* Release slots only after their contents have been copied. SDL atomics
         * provide the barriers for this single-producer/single-consumer ring. */
        SDL_SetAtomicInt(&p->read_index,(int)read);
        for(unsigned i=0;i<count;++i) {
            if(batch[i].process){process_row(processes,&batch[i]);++p->process_written;}
            else {row(f,&batch[i].value.frame);++p->written;}
        }
        if(ferror(f) || ferror(processes))goto failed;
        if(!count) {
            if(SDL_GetAtomicInt(&p->stopping) &&
               read==(unsigned)SDL_GetAtomicInt(&p->write_index))break;
            SDL_Delay(5);
        }
    }
    if(fclose(processes)){processes=NULL;goto failed;}processes=NULL;
    if(fclose(f)){f=NULL;goto failed;} f=NULL;
    /* finish publishes stopping only after the final producer call. */
    snprintf(path,sizeof path,"%s/status.json",p->directory);
    f=fopen(path,"wbx");if(!f)goto failed;
    fprintf(f,"{\"schema\":2,\"written\":%llu,\"dropped\":%llu,\"process_written\":%llu,\"process_dropped\":%llu,\"complete\":%s,"
              "\"scope\":\"app process, all threads, excludes runner/child CPU and memory; lifetime peak RAM; logical received pipe bytes\"}\n",
            (unsigned long long)p->written,(unsigned long long)p->dropped,
            (unsigned long long)p->process_written,(unsigned long long)p->process_dropped,
            (p->dropped || p->process_dropped)?"false":"true");
    if(fclose(f)){f=NULL;goto failed;}f=NULL;
    return 0;
failed:
    if(f)fclose(f);
    if(processes)fclose(processes);
    SDL_SetAtomicInt(&p->failed,1);
    fprintf(stderr,"App profiling failed; existing files preserved, partial artifacts in: %s\n",p->directory);
    return 1;
}
ps_app_profile *ps_app_profile_start(const char *directory) {
    if(!directory || !*directory || strlen(directory)>=3900)return NULL;
    ps_app_profile *p=calloc(1,sizeof *p);if(!p)return NULL;
    snprintf(p->directory,sizeof p->directory,"%s",directory);
    p->thread=SDL_CreateThread(worker,"physim-profile",p);
    if(!p->thread){free(p);return NULL;}
    return p;
}
bool ps_app_profile_record(ps_app_profile *p,const ps_app_profile_frame *frame) {
    if(!p)return false;
    if(!frame){++p->dropped;return false;}
    if(SDL_GetAtomicInt(&p->failed)){++p->dropped;return false;}
    unsigned write=(unsigned)SDL_GetAtomicInt(&p->write_index);
    unsigned next=(write+1)%PROFILE_CAPACITY;
    if(next==(unsigned)SDL_GetAtomicInt(&p->read_index)){++p->dropped;return false;}
    p->queue[write].process=false;
    p->queue[write].value.frame=*frame;
    SDL_SetAtomicInt(&p->write_index,(int)next);
    return true;
}
bool ps_app_profile_record_process(ps_app_profile *p,const ps_app_profile_process *process) {
    if(!p)return false;
    uint64_t sequence=p->process_sequence++;
    if(!process || SDL_GetAtomicInt(&p->failed)){++p->process_dropped;return false;}
    unsigned write=(unsigned)SDL_GetAtomicInt(&p->write_index),next=(write+1)%PROFILE_CAPACITY;
    if(next==(unsigned)SDL_GetAtomicInt(&p->read_index)){++p->process_dropped;return false;}
    p->queue[write].process=true;p->queue[write].sequence=sequence;p->queue[write].value.child=*process;
    SDL_SetAtomicInt(&p->write_index,(int)next);return true;
}
ps_app_profile_result ps_app_profile_finish(ps_app_profile *p) {
    if(!p)return (ps_app_profile_result){0};
    SDL_SetAtomicInt(&p->stopping,1);SDL_WaitThread(p->thread,NULL);
    ps_app_profile_result result={p->written,p->dropped,SDL_GetAtomicInt(&p->failed)!=0,
                                 p->process_written,p->process_dropped};
    free(p);return result;
}
