#include "physim/collision.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Motion sweep %d: %s\n",__LINE__,#x);return 1;}}while(0)
static const uint32_t triangles[][3]={{0,2,1},{0,3,2},{4,5,6},{4,6,7},{0,1,5},{0,5,4},{3,7,6},{3,6,2},{0,4,7},{0,7,3},{1,2,6},{1,6,5}};
static void box(double x,double y,double z,ps_vec3 vertices[8]) {
    const int signs[8][3]={{-1,-1,-1},{1,-1,-1},{1,1,-1},{-1,1,-1},{-1,-1,1},{1,-1,1},{1,1,1},{-1,1,1}};
    for(unsigned i=0;i<8;i++)vertices[i]=ps_v3(x*signs[i][0],y*signs[i][1],z*signs[i][2]);
}
int main(void){
    ps_vec3 vertices[8],other[8];box(2,.1,.1,vertices);box(.1,.1,.1,other);
    ps_convex_mesh mesh={vertices,8,triangles,12},target={other,8,triangles,12};ps_body a,b;
    CHECK(ps_body_with_inertia(1,ps_v3(1,1,1),&a)==PS_OK);b=a;b.position_m=ps_v3(0,1.5,0);
    ps_rigid_motion spin={ps_v3(0,0,0),ps_v3(0,0,3.141592653589793),ps_v3(0,0,0)};
    ps_sweep_hit hit={0};bool touching;
    CHECK(ps_sweep_convexes_motion(&a,&mesh,spin,&b,&target,(ps_rigid_motion){0},NULL,&hit,&touching)==PS_OK && touching && hit.fraction>0 && hit.fraction<.5);
    ps_body pose;CHECK(ps_body_motion_pose(&a,spin,.5,&pose)==PS_OK);
    CHECK(fabs(ps_quat_rotate(pose.orientation,ps_v3(1,0,0)).y-1)<1e-12);
    ps_aabb bounds;CHECK(ps_aabb_motion_convex(&a,&mesh,spin,&bounds)==PS_OK && bounds.maximum_m.y>2);
    ps_sweep_hit saved=hit;touching=true;ps_ccd_settings short_budget={1e-8,1};
    CHECK(ps_sweep_convexes_motion(&a,&mesh,spin,&b,&target,(ps_rigid_motion){0},&short_budget,&hit,&touching)==PS_LIMIT && touching && !memcmp(&hit,&saved,sizeof hit));
    b.position_m=ps_v3(0,5,0);
    CHECK(ps_sweep_convexes_motion(&a,&mesh,spin,&b,&target,(ps_rigid_motion){0},NULL,&hit,&touching)==PS_OK && !touching);
    a.position_m=ps_v3(0,3,0);
    ps_rigid_motion fall={ps_v3(0,0,0),ps_v3(0,0,0),ps_v3(0,-4,0)};
    CHECK(ps_sweep_convex_plane_motion(&a,&mesh,fall,ps_v3(0,0,0),ps_v3(0,1,0),NULL,&hit,&touching)==PS_OK && touching && fabs(hit.fraction-sqrt(2.9/4))<1e-7);
    CHECK(ps_aabb_motion_convex(&a,&mesh,(ps_rigid_motion){ps_v3(0,4,0),ps_v3(0,0,0),ps_v3(0,-4,0)},&bounds)==PS_OK && bounds.maximum_m.y>4);
    a.position_m=ps_v3(0,0,0);b.position_m=ps_v3(0,1.5,0);
    spin.rotation_rad.z=4*PS_PI;
    CHECK(ps_sweep_convexes_motion(&a,&mesh,spin,&b,&target,(ps_rigid_motion){0},NULL,&hit,&touching)==PS_OK && touching && hit.fraction<.25);
    saved=hit;touching=true;ps_ccd_settings precise={1e-20,4096};
    CHECK(ps_sweep_convexes_motion(&a,&mesh,spin,&b,&target,(ps_rigid_motion){0},&precise,&hit,&touching)==PS_NUMERIC && touching && !memcmp(&hit,&saved,sizeof hit));
    ps_body unchanged=a;ps_rigid_motion invalid=spin;invalid.rotation_rad.x=NAN;
    CHECK(ps_body_motion_pose(&a,invalid,.5,&pose)==PS_INVALID);
    CHECK(ps_body_motion_pose(&a,spin,1.1,&pose)==PS_INVALID);
    CHECK(!memcmp(&a,&unchanged,sizeof a));
    ps_body almost=a;almost.orientation.w=1+5e-9;
    CHECK(ps_body_motion_pose(&almost,(ps_rigid_motion){0},.5,&pose)==PS_OK && fabs(pose.orientation.w-1)<1e-15);
    /* Pose/bounds covariance: compare every vertex over a quadratic rotating path. */
    ps_rigid_motion curve={ps_v3(1,4,2),ps_v3(.3,.4,8*PS_PI),ps_v3(-.5,-4,1)};
    CHECK(ps_aabb_motion_convex(&a,&mesh,curve,&bounds)==PS_OK);
    for(unsigned sample=0;sample<=200;sample++) {
        CHECK(ps_body_motion_pose(&a,curve,(double)sample/200,&pose)==PS_OK);
        for(unsigned i=0;i<8;i++) {
            ps_vec3 p=ps_vadd(pose.position_m,ps_quat_rotate(pose.orientation,vertices[i]));
            CHECK(p.x>=bounds.minimum_m.x && p.x<=bounds.maximum_m.x &&
                  p.y>=bounds.minimum_m.y && p.y<=bounds.maximum_m.y &&
                  p.z>=bounds.minimum_m.z && p.z<=bounds.maximum_m.z);
        }
    }
    puts("Rotating/quadratic motion: interior spin collision, pose, conservative bounds, unresolved atomic budget, no hit and accelerated plane event passed");
    return 0;
}
