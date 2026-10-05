#include "physim/data.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if(!(x)) { fprintf(stderr,"Runner hierarchy line %d: %s\n",__LINE__,#x);return 1; } } while(0)
static int run(const char *runner,const char *module,const char *path,const char *work) {
    const char *args[]={runner,module,path,"--steps","20","--dt","0.01","--record-scenes",NULL};
    ps_process child={0};CHECK(ps_process_start(&child,args,work));
    double deadline=ps_clock()+20;char output[4096];
    while(ps_process_poll(&child) && ps_clock()<deadline) {
        int n;while((n=ps_process_read(&child,output,sizeof output))>0) fwrite(output,1,(size_t)n,stderr);
        ps_sleep(1);
    }
    if(child.running) ps_process_kill(&child);
    int code=child.exit_code;ps_process_close(&child);CHECK(code==0);return 0;
}
static uint32_t checksum(const ps_snapshot *frame,bool flat) {
    ps_context context={.time_s=frame->time,.channel_count=frame->count};
    memcpy(context.values,frame->values,sizeof context.values);ps_scene scene=frame->scene;
    if(flat) { scene.count=1;scene.objects[0].parent_id=0; }
    unsigned char data[PS_SNAPSHOT_MAX];size_t size=ps_snapshot_encode(data,&context,&scene,frame->paused);
    return size?ps_crc32(data,size):0;
}
int main(int argc,char **argv) {
    CHECK(argc==6);char paths[3][4096];
    for(unsigned mode=0;mode<3;mode++) {
        snprintf(paths[mode],sizeof paths[mode],"%s/hierarchy-%u.psrun",argv[5],mode);
        CHECK(!run(argv[1],argv[2+mode],paths[mode],argv[5]));
    }
    ps_run_reader readers[3];
    for(unsigned mode=0;mode<3;mode++) CHECK(ps_run_open(&readers[mode],paths[mode])==PS_OK);
    for(unsigned sample=0;sample<=20;sample++) {
        double time[3],values[3][PS_MAX_CHANNELS];
        for(unsigned mode=0;mode<3;mode++) CHECK(ps_run_next(&readers[mode],&time[mode],values[mode])==PS_OK);
        CHECK(time[0]==time[1] && time[1]==time[2] && values[0][0]==values[1][0] && values[1][0]==values[2][0]);
    }
    double time,values[PS_MAX_CHANNELS];
    for(unsigned mode=0;mode<3;mode++) {
        CHECK(ps_run_next(&readers[mode],&time,values)==PS_EOF);ps_run_reader_close(&readers[mode]);
        CHECK(ps_run_open(&readers[mode],paths[mode])==PS_OK);
    }
    unsigned frames=0;ps_snapshot snapshots[3];
    while(ps_run_snapshot_next(&readers[0],&snapshots[0])==PS_OK) {
        for(unsigned mode=1;mode<3;mode++) CHECK(ps_run_snapshot_next(&readers[mode],&snapshots[mode])==PS_OK);
        CHECK(snapshots[0].scene.count==3 && snapshots[0].scene.objects[0].parent_id==200 &&
              snapshots[0].scene.objects[1].id==100 && snapshots[0].scene.objects[2].parent_id==100);
        CHECK(checksum(&snapshots[0],false) && checksum(&snapshots[0],false)==checksum(&snapshots[1],false));
        CHECK(snapshots[2].scene.count==1 && snapshots[2].scene.objects[0].parent_id==0);
        CHECK(checksum(&snapshots[0],true)==checksum(&snapshots[2],false));
        frames++;
    }
    CHECK(frames>=10 && readers[0].complete);
    for(unsigned mode=1;mode<3;mode++) CHECK(ps_run_snapshot_next(&readers[mode],&snapshots[mode])==PS_EOF);
    for(unsigned mode=0;mode<3;mode++) ps_run_reader_close(&readers[mode]);
    puts("C/Physim scene hierarchies match; legacy ABI-3 padding is ignored; all 21 measurements are unchanged.");
    return 0;
}
