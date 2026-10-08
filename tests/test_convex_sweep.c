#include "physim/collision.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Convex sweep %d: %s\n",__LINE__,#x);return 1;}}while(0)
static const ps_vec3 v[]={{-1,-1,-1},{1,-1,-1},{1,1,-1},{-1,1,-1},{-1,-1,1},{1,-1,1},{1,1,1},{-1,1,1}};
static const uint32_t t[][3]={{0,2,1},{0,3,2},{4,5,6},{4,6,7},{0,1,5},{0,5,4},{3,7,6},{3,6,2},{0,4,7},{0,7,3},{1,2,6},{1,6,5}};
static bool near(double a,double b){return fabs(a-b)<1e-12;}
int main(void){
    ps_convex_mesh mesh={v,8,t,12};ps_body a,b;
    CHECK(ps_body_box(1,ps_v3(2,2,2),&a)==PS_OK);b=a;b.position_m.x=10;
    ps_sweep_hit hit={0};bool touching;
    CHECK(ps_sweep_convexes(&a,&mesh,ps_v3(20,0,0),&b,&mesh,ps_v3(0,0,0),&hit,&touching)==PS_OK && touching && near(hit.fraction,.4));
    CHECK(near(hit.contact.normal.x,1) && near(hit.contact.point_m.x,9) && hit.contact.penetration_m==0);
    ps_sweep_hit reverse;CHECK(ps_sweep_convexes(&b,&mesh,ps_v3(0,0,0),&a,&mesh,ps_v3(20,0,0),&reverse,&touching)==PS_OK && touching && near(reverse.fraction,.4) && near(reverse.contact.normal.x,-1));
    CHECK(ps_sweep_convexes(&a,&mesh,ps_v3(15,0,0),&b,&mesh,ps_v3(5,0,0),&hit,&touching)==PS_OK && touching && near(hit.fraction,.8));
    ps_sweep_hit saved=hit;
    CHECK(ps_sweep_convexes(&a,&mesh,ps_v3(-20,0,0),&b,&mesh,ps_v3(0,0,0),&hit,&touching)==PS_OK && !touching && !memcmp(&hit,&saved,sizeof hit));
    CHECK(ps_sweep_convexes(&a,&mesh,ps_v3(8,0,0),&b,&mesh,ps_v3(0,0,0),&hit,&touching)==PS_OK && touching && hit.fraction==1);
    b.position_m.y=2;
    CHECK(ps_sweep_convexes(&a,&mesh,ps_v3(20,0,0),&b,&mesh,ps_v3(0,0,0),&hit,&touching)==PS_OK && touching && near(hit.fraction,.4));
    b.position_m.y=2.001;
    CHECK(ps_sweep_convexes(&a,&mesh,ps_v3(20,0,0),&b,&mesh,ps_v3(0,0,0),&hit,&touching)==PS_OK && !touching);
    b.position_m=ps_v3(1,0,0);
    CHECK(ps_sweep_convexes(&a,&mesh,ps_v3(-20,0,0),&b,&mesh,ps_v3(0,0,0),&hit,&touching)==PS_OK && touching && hit.fraction==0 && near(hit.contact.penetration_m,1));
    b.position_m=ps_v3(1e12,0,0);
    CHECK(ps_sweep_convexes(&a,&mesh,ps_v3(2e12,0,0),&b,&mesh,ps_v3(0,0,0),&hit,&touching)==PS_OK && touching && fabs(hit.fraction-(1e12-2)/2e12)<1e-16);
    a.position_m=ps_v3(0,10,0);
    CHECK(ps_sweep_convex_plane(&a,&mesh,ps_v3(3,-20,0),ps_v3(0,0,0),ps_v3(0,1,0),&hit,&touching)==PS_OK && touching && near(hit.fraction,.45));
    CHECK(near(hit.contact.point_m.y,0) && near(hit.contact.normal.y,-1));
    ps_aabb bounds;CHECK(ps_aabb_swept_convex(&a,&mesh,ps_v3(3,-20,0),&bounds)==PS_OK && bounds.minimum_m.y<=-11 && bounds.maximum_m.y>=11);
    saved=hit;touching=true;
    CHECK(ps_sweep_convexes(&a,&mesh,ps_v3(NAN,0,0),&b,&mesh,ps_v3(0,0,0),&hit,&touching)==PS_INVALID && touching && !memcmp(&hit,&saved,sizeof hit));
    /* Intersection of time windows at exactly one instant (corner grazing). */
    a.position_m=ps_v3(0,0,0);b.position_m=ps_v3(10,8,0);
    CHECK(ps_sweep_convexes(&a,&mesh,ps_v3(20,10,0),&b,&mesh,ps_v3(0,0,0),&hit,&touching)==PS_OK && touching && near(hit.fraction,.6));
    b.position_m.y=8.001;
    CHECK(ps_sweep_convexes(&a,&mesh,ps_v3(20,10,0),&b,&mesh,ps_v3(0,0,0),&hit,&touching)==PS_OK && !touching);
    saved=hit;touching=true;b.position_m=ps_v3(10,0,0);
    CHECK(ps_sweep_convexes(&a,&mesh,ps_v3(-DBL_MAX,0,0),&b,&mesh,ps_v3(DBL_MAX,0,0),&hit,&touching)==PS_NUMERIC && touching && !memcmp(&hit,&saved,sizeof hit));
    ps_body sphere;CHECK(ps_body_sphere(1,.5,&sphere)==PS_OK);
    a.position_m=ps_v3(0,0,0);sphere.position_m=ps_v3(-10,0,0);
    CHECK(ps_sweep_sphere_convex(&sphere,.5,ps_v3(20,0,0),&a,&mesh,ps_v3(0,0,0),&hit,&touching)==PS_OK && touching && near(hit.fraction,.425) && near(hit.contact.normal.x,1));
    sphere.position_m=ps_v3(-10,1.3,0);
    CHECK(ps_sweep_sphere_convex(&sphere,.5,ps_v3(20,0,0),&a,&mesh,ps_v3(0,0,0),&hit,&touching)==PS_OK && touching && near(hit.fraction,.43));
    sphere.position_m=ps_v3(-10,1.3,1.3);
    CHECK(ps_sweep_sphere_convex(&sphere,.5,ps_v3(20,0,0),&a,&mesh,ps_v3(0,0,0),&hit,&touching)==PS_OK && touching && near(hit.fraction,(9-sqrt(.07))/20));
    sphere.position_m=ps_v3(-10,1.5,0);
    CHECK(ps_sweep_sphere_convex(&sphere,.5,ps_v3(20,0,0),&a,&mesh,ps_v3(0,0,0),&hit,&touching)==PS_OK && touching && near(hit.fraction,.45));
    sphere.position_m=ps_v3(-10,1.501,0);
    CHECK(ps_sweep_sphere_convex(&sphere,.5,ps_v3(20,0,0),&a,&mesh,ps_v3(0,0,0),&hit,&touching)==PS_OK && !touching);
    sphere.position_m=ps_v3(-10,0,0);
    CHECK(ps_sweep_sphere_convex(&sphere,.5,ps_v3(9,0,0),&a,&mesh,ps_v3(.5,0,0),&hit,&touching)==PS_OK && touching && near(hit.fraction,1));
    CHECK(ps_sweep_sphere_convex(&sphere,.5,ps_v3(20,0,0),&a,&mesh,ps_v3(5,0,0),&hit,&touching)==PS_OK && touching && near(hit.fraction,8.5/15));
    saved=hit;touching=true;
    CHECK(ps_sweep_sphere_convex(&sphere,.5,ps_v3(DBL_MAX,0,0),&a,&mesh,ps_v3(-DBL_MAX,0,0),&hit,&touching)==PS_NUMERIC && touching && !memcmp(&hit,&saved,sizeof hit));
    ps_aabb preserved=bounds;ps_body far=a;far.position_m.x=DBL_MAX;
    CHECK(ps_aabb_swept_convex(&far,&mesh,ps_v3(DBL_MAX,0,0),&bounds)==PS_NUMERIC && !memcmp(&bounds,&preserved,sizeof bounds));
    puts("Convex linear sweeps: tunnelling, both movers, grazing, endpoints, initial overlap, long paths, planes and atomic errors passed");
    return 0;
}
