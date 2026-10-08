#include "physim/collision.h"
#include <stdio.h>
#include <string.h>
static const uint32_t triangles[][3]={{0,2,1},{0,3,2},{4,5,6},{4,6,7},{0,1,5},{0,5,4},{3,7,6},{3,6,2},{0,4,7},{0,7,3},{1,2,6},{1,6,5}};
static void box(double x,double y,double z,ps_vec3 v[8]) {
    const int sign[8][3]={{-1,-1,-1},{1,-1,-1},{1,1,-1},{-1,1,-1},{-1,-1,1},{1,-1,1},{1,1,1},{-1,1,1}};
    for(unsigned i=0;i<8;i++)v[i]=ps_v3(x*sign[i][0],y*sign[i][1],z*sign[i][2]);
}
int main(void){
    unsigned mode,iterations;double l,w,h,angle,linear,quadratic,radius,tolerance;
    while(scanf("%u %la %la %la %la %la %la %la %la %u",&mode,&l,&w,&h,&angle,&linear,&quadratic,&radius,&tolerance,&iterations)==10){
        ps_vec3 vertices[8],target[8];box(l,w,w,vertices);box(radius,radius,radius,target);
        ps_convex_mesh mesh={vertices,8,triangles,12},other={target,8,triangles,12};ps_body a,b;
        if(ps_body_with_inertia(1,ps_v3(1,1,1),&a)!=PS_OK)return 2;
        b=a;
        ps_rigid_motion motion={ps_v3(0,linear,0),ps_v3(0,0,angle),ps_v3(0,quadratic,0)},still={0};
        ps_ccd_settings settings={tolerance,iterations};ps_sweep_hit hit={.75,{ps_v3(77,88,99),ps_v3(1,0,0),42}},saved=hit;bool touching=true;
        ps_result status;
        if(mode==0){a.position_m.y=h;status=ps_sweep_convex_plane_motion(&a,&mesh,motion,ps_v3(0,0,0),ps_v3(0,1,0),&settings,&hit,&touching);}
        else if(mode==1){b.position_m.y=h;status=ps_sweep_sphere_convex_motion(&b,radius,still,&a,&mesh,motion,&settings,&hit,&touching);}
        else{b.position_m.y=h;status=ps_sweep_convexes_motion(&a,&mesh,motion,&b,&other,still,&settings,&hit,&touching);}
        if((status!=PS_OK||!touching)&&memcmp(&hit,&saved,sizeof hit))return 3;
        if(status!=PS_OK&&!touching)return 4;
        printf("%d %u %a %a %a %a %a %a %a %a\n",status,touching,hit.fraction,hit.contact.penetration_m,
               hit.contact.normal.x,hit.contact.normal.y,hit.contact.normal.z,hit.contact.point_m.x,hit.contact.point_m.y,hit.contact.point_m.z);
    }
    return ferror(stdin)?2:0;
}
