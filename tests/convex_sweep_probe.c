#include "physim/collision.h"
#include <stdio.h>
#include <string.h>
static int mesh_read(ps_convex_mesh *mesh,ps_vec3 vertices[64],uint32_t triangles[128][3],ps_body *b) {
    size_t n,t;
    if(scanf("%zu %zu",&n,&t)!=2)return 0;
    if(n>64 || t>128)return 0;
    for(size_t i=0;i<n;i++)if(scanf("%la %la %la",&vertices[i].x,&vertices[i].y,&vertices[i].z)!=3)return 0;
    for(size_t i=0;i<t;i++)if(scanf("%u %u %u",&triangles[i][0],&triangles[i][1],&triangles[i][2])!=3)return 0;
    *mesh=(ps_convex_mesh){vertices,n,(const uint32_t (*)[3])triangles,t};
    if(ps_body_box(1,ps_v3(2,2,2),b)!=PS_OK)return 0;
    return scanf("%la %la %la %la %la %la %la",&b->position_m.x,&b->position_m.y,&b->position_m.z,&b->orientation.x,&b->orientation.y,&b->orientation.z,&b->orientation.w)==7;
}
int main(void) {
    unsigned mode;ps_vec3 va[64],vb[64];uint32_t ta[128][3],tb[128][3];ps_convex_mesh ma,mb;ps_body a,b;
    while(scanf("%u",&mode)==1) {
        if(!mesh_read(&ma,va,ta,&a)||!mesh_read(&mb,vb,tb,&b))return 2;
        ps_vec3 da,db;double radius;
        if(scanf("%la %la %la %la %la %la %la",&da.x,&da.y,&da.z,&db.x,&db.y,&db.z,&radius)!=7)return 2;
        ps_body sa=a,sb=b;
        ps_sweep_hit hit={.75,{ps_v3(77,88,99),ps_v3(1,0,0),42}},saved=hit;bool touching=true;
        ps_result result=mode==0?ps_sweep_convexes(&a,&ma,da,&b,&mb,db,&hit,&touching)
                               :ps_sweep_sphere_convex(&a,radius,da,&b,&mb,db,&hit,&touching);
        if(memcmp(&sa,&a,sizeof a)||memcmp(&sb,&b,sizeof b))return 3;
        if((result!=PS_OK || !touching)&&memcmp(&saved,&hit,sizeof hit))return 4;
        if(result!=PS_OK&&!touching)return 5;
        printf("%d %u %a %a %a %a %a %a %a %a\n",result,touching,hit.fraction,hit.contact.penetration_m,
               hit.contact.normal.x,hit.contact.normal.y,hit.contact.normal.z,
               hit.contact.point_m.x,hit.contact.point_m.y,hit.contact.point_m.z);
    }
    return ferror(stdin)?2:0;
}
