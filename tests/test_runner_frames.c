#include "physim/data.h"
#include "physim/math.h"
#include "platform.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Runner frames line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static int run(const char *runner,const char *module,const char *path,const char *work,int expected) {
    const char *args[]={runner,module,path,"--steps","20","--dt","0.01","--record-scenes",NULL};
    ps_process child={0};CHECK(ps_process_start(&child,args,work));double deadline=ps_clock()+20;char bytes[4096];
    while(ps_process_poll(&child) && ps_clock()<deadline){while(ps_process_read(&child,bytes,sizeof bytes)>0){}ps_sleep(1);}
    if(child.running)ps_process_kill(&child);int code=child.exit_code;ps_process_close(&child);CHECK(code==expected);return 0;
}
int main(int argc,char **argv) {
    CHECK(argc==6);char paths[3][4096];
    for(unsigned mode=0;mode<3;mode++){snprintf(paths[mode],sizeof paths[mode],"%s/frames-%u.psrun",argv[5],mode);CHECK(!run(argv[1],argv[2+mode],paths[mode],argv[5],mode==2?7:0));}
    ps_run_reader readers[2];for(unsigned i=0;i<2;i++)CHECK(ps_run_open(&readers[i],paths[i])==PS_OK);
    for(unsigned sample=0;sample<=20;sample++) {
        double t[2],v[2][PS_MAX_CHANNELS];for(unsigned i=0;i<2;i++)CHECK(ps_run_next(&readers[i],&t[i],v[i])==PS_OK);
        CHECK(t[0]==t[1] && v[0][0]==v[1][0] && fabs(v[0][0]-t[0])<1e-14);
    }
    for(unsigned i=0;i<2;i++){ps_run_reader_close(&readers[i]);CHECK(ps_run_open(&readers[i],paths[i])==PS_OK);}
    ps_snapshot frames[2];unsigned count=0;ps_result result;
    while((result=ps_run_snapshot_next(&readers[0],&frames[0]))==PS_OK) {
        CHECK(ps_run_snapshot_next(&readers[1],&frames[1])==PS_OK);
        CHECK(frames[0].time==frames[1].time && frames[0].scene.count==11 && !memcmp(&frames[0].scene,&frames[1].scene,sizeof frames[0].scene));
        ps_vec3 world;CHECK(ps_scene_world_point(&frames[0].scene,3,frames[0].scene.objects[3].a,&world)==PS_OK);
        CHECK(fabs(world.x-(.4-.75*frames[0].time))<1e-12 && fabs(world.y-.025)<1e-12);
        count++;
    }
    CHECK(result==PS_EOF && count>=10 && readers[0].samples==21 && ps_run_snapshot_next(&readers[1],&frames[1])==PS_EOF);
    for(unsigned i=0;i<2;i++)ps_run_reader_close(&readers[i]);
    puts("C/Physim coordinate frames preserve all measurements, local geometry, world poses and recorded replay; missing capability is rejected");return 0;
}
