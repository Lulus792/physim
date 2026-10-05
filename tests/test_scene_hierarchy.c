#include "physim/data.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if(!(x)) { fprintf(stderr,"Hierarchy line %d: %s\n",__LINE__,#x);return 1; } } while(0)
int main(int argc,char **argv) {
    CHECK(argc==2);
    ps_scene scene={0};
    CHECK(ps_scene_group(&scene,100,0,"Versuch α")==PS_OK);
    CHECK(ps_scene_group(&scene,200,100,"Modell")==PS_OK);
    CHECK(ps_scene_add_id(&scene,1,PS_SPHERE,ps_v3(1,2,3),ps_v3(0,0,0),.2,UINT32_MAX)==PS_OK);
    CHECK(ps_scene_set_parent(&scene,1,200)==PS_OK);
    CHECK(ps_scene_valid(&scene) && ps_scene_parent_index(&scene,2)==1 && ps_scene_parent_index(&scene,1)==0);
    ps_scene before=scene;
    CHECK(ps_scene_set_parent(&scene,100,1)==PS_INVALID && !memcmp(&scene,&before,sizeof scene));
    CHECK(ps_scene_set_parent(&scene,1,1)==PS_INVALID && !memcmp(&scene,&before,sizeof scene));
    CHECK(ps_scene_set_parent(&scene,1,999)==PS_INVALID && !memcmp(&scene,&before,sizeof scene));
    CHECK(ps_scene_group(&scene,300,999,"Missing")==PS_INVALID && !memcmp(&scene,&before,sizeof scene));
    CHECK(ps_scene_group(&scene,100,0,"Duplicate")==PS_INVALID && !memcmp(&scene,&before,sizeof scene));
    CHECK(ps_scene_group(&scene,0,0,"Anonymous")==PS_INVALID && ps_scene_group(&scene,400,0,"")==PS_INVALID);
    CHECK(ps_scene_set_parent(&scene,1,0)==PS_OK && ps_scene_parent_index(&scene,2)==-1);
    CHECK(ps_scene_set_parent(&scene,1,200)==PS_OK);
    ps_object swap=scene.objects[0];scene.objects[0]=scene.objects[2];scene.objects[2]=swap;
    CHECK(ps_scene_valid(&scene) && ps_scene_parent_index(&scene,0)==1 && ps_scene_parent_index(&scene,1)==2);
    ps_context context={.time_s=.25,.channel_count=1};context.values[0]=42;
    unsigned char bytes[PS_SNAPSHOT_MAX],damaged[PS_SNAPSHOT_MAX];
    size_t size=ps_snapshot_encode(bytes,&context,&scene,false);CHECK(size);
    ps_snapshot snapshot={0};
    CHECK(ps_snapshot_decode(bytes,(uint32_t)size,&snapshot.time,snapshot.values,&snapshot.count,&snapshot.scene,&snapshot.paused));
    CHECK(snapshot.time==.25 && snapshot.values[0]==42 && !memcmp(&snapshot.scene,&scene,sizeof scene));
    ps_snapshot preserved=snapshot;
    memcpy(damaged,bytes,size);ps_put_u32(damaged+PS_SNAPSHOT_HEADER+8+172,1); /* self-parent */
    CHECK(!ps_snapshot_decode(damaged,(uint32_t)size,&snapshot.time,snapshot.values,&snapshot.count,&snapshot.scene,&snapshot.paused));
    CHECK(!memcmp(&snapshot,&preserved,sizeof snapshot));
    CHECK(!ps_snapshot_decode_version(99,bytes,(uint32_t)size,&snapshot.time,snapshot.values,&snapshot.count,&snapshot.scene,&snapshot.paused));
    /* A real version-1 layout has 172 bytes per object and no parent field. */
    ps_scene flat={0};
    CHECK(ps_scene_add_id(&flat,1,PS_SPHERE,ps_v3(1,2,3),ps_v3(0,0,0),.2,UINT32_MAX)==PS_OK);
    CHECK(ps_scene_label_id(&flat,2,ps_v3(1,2,3),"Old α",UINT32_MAX)==PS_OK);
    size=ps_snapshot_encode(bytes,&context,&flat,true);CHECK(size);
    size_t head=PS_SNAPSHOT_HEADER+8,legacy_size=head+2*172;
    memcpy(damaged,bytes,head);
    for(unsigned i=0;i<2;i++) memcpy(damaged+head+i*172,bytes+head+i*PS_SNAPSHOT_OBJECT_SIZE,172);
    CHECK(ps_snapshot_decode_version(1,damaged,(uint32_t)legacy_size,&snapshot.time,snapshot.values,&snapshot.count,&snapshot.scene,&snapshot.paused));
    CHECK(snapshot.paused && !memcmp(&snapshot.scene,&flat,sizeof flat));
    CHECK(!ps_snapshot_decode(damaged,(uint32_t)legacy_size,&snapshot.time,snapshot.values,&snapshot.count,&snapshot.scene,&snapshot.paused));
    char path[4096];snprintf(path,sizeof path,"%s/scene-v1.psrun",argv[1]);
    ps_context recorded={0};CHECK(ps_channel_add(&recorded,"value",PS_METRE,"legacy") == 0);recorded.values[0]=42;
    ps_run_writer writer;CHECK(ps_run_create(&writer,path,&recorded,"scene-v1 fixture")==PS_OK);
    CHECK(ps_run_append(&writer,.25,recorded.values)==PS_OK);
    unsigned char payload[PS_SNAPSHOT_MAX],chunk[12];ps_put_u32(payload,1);memcpy(payload+4,damaged,legacy_size);
    ps_put_u32(chunk,5);ps_put_u32(chunk+4,(uint32_t)legacy_size+4);ps_put_u32(chunk+8,ps_crc32(payload,legacy_size+4));
    CHECK(fwrite(chunk,1,12,writer.file)==12 && fwrite(payload,1,legacy_size+4,writer.file)==legacy_size+4);
    CHECK(ps_run_close(&writer)==PS_OK);
    ps_run_reader reader;CHECK(ps_run_open(&reader,path)==PS_OK);
    CHECK(ps_run_snapshot_next(&reader,&snapshot)==PS_OK && !memcmp(&snapshot.scene,&flat,sizeof flat));
    CHECK(ps_run_snapshot_next(&reader,&snapshot)==PS_EOF && reader.samples==1);ps_run_reader_close(&reader);
    puts("Scene hierarchy: groups, cycles, reparenting, atomic errors, wire preservation and version-1 run replay passed.");
    return 0;
}
