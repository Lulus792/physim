#include "physim/mechanics.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"Momentum line %d: %s\n",__LINE__,#x); return 1; } } while (0)
static bool close3(ps_vec3 a, ps_vec3 b) {
    return fabs(a.x-b.x)<1e-11 && fabs(a.y-b.y)<1e-11 && fabs(a.z-b.z)<1e-11;
}
int main(void) {
    ps_body a,b;
    ps_vec3 p,l,l0,zero={0},sentinel={17,19,23};
    CHECK(ps_body_with_inertia(2,ps_v3(3,5,7),&a)==PS_OK);
    a.position_m=ps_v3(2,3,4); a.velocity_m_s=ps_v3(5,6,7);
    a.angular_velocity_rad_s=ps_v3(1,2,3);
    CHECK(ps_body_linear_momentum(&a,&p)==PS_OK && close3(p,ps_v3(10,12,14)));
    CHECK(ps_body_angular_momentum(&a,zero,&l0)==PS_OK && close3(l0,ps_v3(-3,22,15)));
    CHECK(ps_body_angular_momentum(&a,ps_v3(1,1,1),&l)==PS_OK && close3(l,ps_v3(-5,26,13)));
    CHECK(close3(l,ps_vsub(l0,ps_vcross(ps_v3(1,1,1),p))));
    a.orientation=ps_quat_axis_angle(ps_v3(0,0,1),PS_PI/2);
    CHECK(ps_body_angular_momentum(&a,a.position_m,&l)==PS_OK && close3(l,ps_v3(5,6,21)));
    ps_vec3 origin=ps_v3(-2,1,3),point=ps_v3(3,-1,2),j=ps_v3(2,4,-3),before;
    CHECK(ps_body_angular_momentum(&a,origin,&before)==PS_OK);
    CHECK(ps_body_apply_impulse(&a,j,point)==PS_OK);
    CHECK(ps_body_linear_momentum(&a,&l)==PS_OK && close3(l,ps_vadd(p,j)));
    CHECK(ps_body_angular_momentum(&a,origin,&l)==PS_OK &&
          close3(ps_vsub(l,before),ps_vcross(ps_vsub(point,origin),j)));
    /* Shared contact point, no positional correction: internal impulses conserve
       total linear and angular momentum, including friction and spin. */
    for (unsigned mode=0;mode<3;mode++) {
        CHECK(ps_body_sphere(2,1,&a)==PS_OK && ps_body_sphere(3,1,&b)==PS_OK);
        a.position_m=ps_v3(-1,0,0); b.position_m=ps_v3(1,0,0);
        a.velocity_m_s=ps_v3(3,2,0); b.velocity_m_s=ps_v3(-1,-1,0);
        ps_vec3 pa,pb,la,lb;
        CHECK(ps_body_linear_momentum(&a,&pa)==PS_OK && ps_body_linear_momentum(&b,&pb)==PS_OK);
        CHECK(ps_body_angular_momentum(&a,origin,&la)==PS_OK && ps_body_angular_momentum(&b,origin,&lb)==PS_OK);
        p=ps_vadd(pa,pb); l0=ps_vadd(la,lb);
        ps_contact contact={zero,{1,0,0},0};
        CHECK(ps_contact_resolve(&a,&b,&contact,mode==0?1:0,mode==2?.4:0,NULL)==PS_OK);
        CHECK(ps_body_linear_momentum(&a,&pa)==PS_OK && ps_body_linear_momentum(&b,&pb)==PS_OK);
        CHECK(ps_body_angular_momentum(&a,origin,&la)==PS_OK && ps_body_angular_momentum(&b,origin,&lb)==PS_OK);
        CHECK(close3(p,ps_vadd(pa,pb)) && close3(l0,ps_vadd(la,lb)));
    }
    CHECK(ps_body_with_inertia(0,zero,&a)==PS_OK);
    a.position_m=ps_v3(DBL_MAX,-DBL_MAX,DBL_MAX);
    CHECK(ps_body_angular_momentum(&a,ps_v3(-DBL_MAX,DBL_MAX,0),&l)==PS_OK && close3(l,zero));
    CHECK(ps_body_linear_momentum(&a,&p)==PS_OK && close3(p,zero));
    /* Individual orbital products and center-origin differences overflow, but
       exact cancellation leaves only the small spin component. */
    CHECK(ps_body_with_inertia(DBL_MAX,ps_v3(1,1,1),&a)==PS_OK);
    a.position_m=ps_v3(DBL_MAX,DBL_MAX,0);
    a.velocity_m_s=ps_v3(DBL_MAX,DBL_MAX,0);
    a.angular_velocity_rad_s=ps_v3(0,0,1);
    CHECK(ps_body_angular_momentum(&a,ps_v3(-DBL_MAX,-DBL_MAX,0),&l)==PS_OK && close3(l,ps_v3(0,0,1)));
    p=sentinel; CHECK(ps_body_linear_momentum(&a,&p)==PS_NUMERIC && !memcmp(&p,&sentinel,sizeof p));
    a.velocity_m_s=ps_v3(0,DBL_MAX,0);
    l=sentinel; CHECK(ps_body_angular_momentum(&a,zero,&l)==PS_NUMERIC && !memcmp(&l,&sentinel,sizeof l));
    CHECK(ps_body_angular_momentum(&a,ps_v3(NAN,0,0),&l)==PS_INVALID && !memcmp(&l,&sentinel,sizeof l));
    CHECK(ps_body_linear_momentum(NULL,&p)==PS_INVALID && ps_body_angular_momentum(NULL,zero,&l)==PS_INVALID);
    CHECK(ps_body_linear_momentum(&a,NULL)==PS_INVALID && ps_body_angular_momentum(&a,zero,NULL)==PS_INVALID);
    CHECK(ps_body_with_inertia(1,ps_v3(1,1,1),&a)==PS_OK);
    a.velocity_m_s=ps_v3(DBL_TRUE_MIN,-DBL_TRUE_MIN,0); a.mass_kg=.5;
    CHECK(ps_body_linear_momentum(&a,&p)==PS_OK && p.x==0 && p.y==0);
    a.angular_velocity_rad_s=ps_v3(DBL_TRUE_MIN,-DBL_TRUE_MIN,0);
    a.inertia_kg_m2=ps_v3(.5,.5,.5);
    CHECK(ps_body_angular_momentum(&a,zero,&l)==PS_OK && l.x==0 && l.y==0);
    a.orientation.w=2;
    p=l=sentinel;
    CHECK(ps_body_linear_momentum(&a,&p)==PS_INVALID && !memcmp(&p,&sentinel,sizeof p));
    CHECK(ps_body_angular_momentum(&a,zero,&l)==PS_INVALID && !memcmp(&l,&sentinel,sizeof l));
    puts("Body momentum: analytic, impulse, collision and numeric boundary checks passed");
    return 0;
}
