#include "physim/math.h"
#include "physim/data.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Frames line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static bool near(ps_vec3 a,ps_vec3 b){return ps_vlength(ps_vsub(a,b))<1e-12;}
int main(int argc,char **argv) {
    CHECK(argc==2);
    ps_scene scene={0};ps_quat turn=ps_quat_axis_angle(ps_v3(0,0,1),PS_PI/2);
    CHECK(ps_scene_frame(&scene,100,0,"Root α",ps_v3(10,0,0),turn,ps_v3(2,3,4))==PS_OK);
    CHECK(ps_scene_group(&scene,200,100,"Organization")==PS_OK);
    CHECK(ps_scene_frame(&scene,300,200,"Child",ps_v3(1,0,0),turn,ps_v3(-1,2,1))==PS_OK);
    CHECK(ps_scene_add_id(&scene,1,PS_SPHERE,ps_v3(1,2,3),ps_v3(0,0,0),.2,UINT32_MAX)==PS_OK);
    CHECK(ps_scene_set_parent(&scene,1,300)==PS_OK);
    ps_vec3 point={0};CHECK(ps_scene_world_point(&scene,3,scene.objects[3].a,&point)==PS_OK && near(point,ps_v3(13,-6,12)));
    CHECK(ps_scene_world_point(&scene,2,ps_v3(0,0,0),&point)==PS_OK && near(point,ps_v3(10,2,0)));
    /* A geometry parent does not add its own center or orientation to children. */
    CHECK(ps_scene_label_id(&scene,2,ps_v3(1,2,3),"Anchor ☃",UINT32_MAX)==PS_OK);
    CHECK(ps_scene_set_parent(&scene,2,1)==PS_OK);
    CHECK(ps_scene_world_point(&scene,4,scene.objects[4].a,&point)==PS_OK && near(point,ps_v3(13,-6,12)));
    ps_vec3 points[]={ps_v3(0,0,0),ps_v3(1,0,0)};
    CHECK(ps_scene_polyline_id(&scene,3,points,2,.01,UINT32_MAX)==PS_OK && ps_scene_set_parent(&scene,3,300)==PS_OK);
    ps_object shared=scene.objects[5];shared.id=4;shared.parent_id=100;
    CHECK(ps_scene_push(&scene,&shared)==PS_OK && scene.point_count==2);
    CHECK(ps_scene_world_point(&scene,5,scene.points[1],&point)==PS_OK && near(point,ps_v3(13,2,0)));
    CHECK(ps_scene_world_point(&scene,6,scene.points[1],&point)==PS_OK && near(point,ps_v3(10,2,0)));
    CHECK(!memcmp(scene.points,points,sizeof points));
    ps_scene before=scene;ps_mat4 matrices[PS_MAX_OBJECTS];memset(matrices,0x5a,sizeof matrices);
    CHECK(ps_scene_transforms(&scene,matrices)==PS_OK && !memcmp(&scene,&before,sizeof scene));
    ps_mat4 expected=matrices[3];
    ps_object swap=scene.objects[0];scene.objects[0]=scene.objects[3];scene.objects[3]=swap;
    CHECK(ps_scene_transforms(&scene,matrices)==PS_OK && !memcmp(&expected,&matrices[0],sizeof expected));scene=before;
    CHECK(ps_scene_frame(&scene,400,0,"Zero",ps_v3(0,0,0),ps_quat_identity(),ps_v3(0,1,1))==PS_INVALID && !memcmp(&scene,&before,sizeof scene));
    CHECK(ps_scene_frame(&scene,400,0,"NaN",ps_v3(NAN,0,0),ps_quat_identity(),ps_v3(1,1,1))==PS_INVALID && !memcmp(&scene,&before,sizeof scene));
    CHECK(ps_scene_frame(&scene,400,0,"Rotation",ps_v3(0,0,0),(ps_quat){0},ps_v3(1,1,1))==PS_INVALID && !memcmp(&scene,&before,sizeof scene));
    CHECK(ps_scene_frame(&scene,400,300,"Overflow",ps_v3(0,0,0),ps_quat_identity(),ps_v3(DBL_MAX,DBL_MAX,DBL_MAX))==PS_INVALID && !memcmp(&scene,&before,sizeof scene));
    CHECK(ps_scene_set_parent(&scene,100,300)==PS_INVALID && !memcmp(&scene,&before,sizeof scene));
    ps_scene bad=scene;bad.objects[0].b=ps_v3(1e154,1e154,1e154);bad.objects[2].b=ps_v3(DBL_MAX,DBL_MAX,DBL_MAX);
    ps_mat4 preserved[PS_MAX_OBJECTS];memcpy(preserved,matrices,sizeof matrices);
    CHECK(ps_scene_transforms(&bad,matrices)==PS_NUMERIC && !memcmp(matrices,preserved,sizeof matrices));
    bad=scene;bad.objects[0].b=ps_v3(1e20,1,1);bad.objects[0].orientation=ps_quat_axis_angle(ps_v3(0,0,1),PS_PI/4);
    CHECK(ps_scene_transforms(&bad,matrices)==PS_SINGULAR && !memcmp(matrices,preserved,sizeof matrices));
    bad=scene;bad.objects[0].b.x=0;
    CHECK(!ps_scene_valid(&bad) && ps_scene_transforms(&bad,matrices)==PS_INVALID);
    ps_vec3 unchanged=point;CHECK(ps_scene_world_point(&bad,3,ps_v3(1,2,3),&point)==PS_INVALID && !memcmp(&point,&unchanged,sizeof point));
    ps_context context={.time_s=.25,.channel_count=1};context.values[0]=42;
    unsigned char bytes[PS_SNAPSHOT_MAX];size_t n=ps_snapshot_encode(bytes,&context,&scene,true);CHECK(n);
    ps_snapshot decoded={0};CHECK(ps_snapshot_decode(bytes,(uint32_t)n,&decoded.time,decoded.values,&decoded.count,&decoded.scene,&decoded.paused));
    CHECK(!memcmp(&scene,&decoded.scene,sizeof scene));ps_snapshot saved=decoded;
    CHECK(!ps_snapshot_decode_version(2,bytes,(uint32_t)n,&decoded.time,decoded.values,&decoded.count,&decoded.scene,&decoded.paused) && !memcmp(&saved,&decoded,sizeof saved));
    char path[4096];snprintf(path,sizeof path,"%s/frames.psrun",argv[1]);
    context.struct_size=sizeof context;context.api_version=PS_API_VERSION;context.channel_count=0;
    CHECK(ps_channel_add(&context,"value",PS_METRE,"value")==0);context.values[0]=42;
    ps_run_writer writer;CHECK(ps_run_create(&writer,path,&context,"frames")==PS_OK);
    CHECK(ps_run_append(&writer,.25,context.values)==PS_OK && ps_run_append_snapshot(&writer,&context,&scene,true)==PS_OK && ps_run_close(&writer)==PS_OK);
    ps_run_reader reader;CHECK(ps_run_open(&reader,path)==PS_OK && ps_run_snapshot_next(&reader,&decoded)==PS_OK);
    CHECK(!memcmp(&scene,&decoded.scene,sizeof scene));CHECK(ps_run_snapshot_next(&reader,&decoded)==PS_EOF);ps_run_reader_close(&reader);
    puts("Coordinate frames: nested TRS/reflections, shared paths, immutable world transforms, numeric rollback and version-3 persistence passed");return 0;
}
