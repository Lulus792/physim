#include "physim/collision.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"Convex line %d: %s\n",__LINE__,#x); return 1; } } while(0)
static const ps_vec3 vertices[]={{-1,-1,-1},{1,-1,-1},{1,1,-1},{-1,1,-1},{-1,-1,1},{1,-1,1},{1,1,1},{-1,1,1}};
static const uint32_t triangles[][3]={{0,2,1},{0,3,2},{4,5,6},{4,6,7},{0,1,5},{0,5,4},{3,7,6},{3,6,2},{0,4,7},{0,7,3},{1,2,6},{1,6,5}};
static bool near(double a,double b) {return fabs(a-b)<1e-10;}
int main(void) {
    ps_convex_mesh mesh={vertices,8,triangles,12};
    CHECK(ps_convex_validate(&mesh)==PS_OK);
    ps_body explicit_body={0},explicit_saved=explicit_body;
    CHECK(ps_body_with_inertia(1,ps_v3(.4,.4,.4),&explicit_body)==PS_OK);
    CHECK(near(explicit_body.mass_kg,1) && near(explicit_body.orientation.w,1));
    explicit_saved=explicit_body;
    CHECK(ps_body_with_inertia(1,ps_v3(0,.4,.4),&explicit_body)==PS_INVALID && !memcmp(&explicit_body,&explicit_saved,sizeof explicit_body));
    CHECK(ps_body_with_inertia(0,ps_v3(0,0,0),&explicit_body)==PS_OK);
    CHECK(ps_body_with_inertia(-1,ps_v3(1,1,1),&explicit_body)==PS_INVALID);
    ps_body a,b,sphere;CHECK(ps_body_box(1,ps_v3(2,2,2),&a)==PS_OK);b=a;
    ps_contact c={ps_v3(77,88,99),ps_v3(1,0,0),42},saved=c;bool hit=true;
    b.position_m.x=3;
    CHECK(ps_contact_convexes(&a,&mesh,&b,&mesh,&c,&hit)==PS_OK && !hit && !memcmp(&c,&saved,sizeof c));
    b.position_m.x=2;
    CHECK(ps_contact_convexes(&a,&mesh,&b,&mesh,&c,&hit)==PS_OK && hit && near(c.penetration_m,0) && near(c.normal.x,1) && near(c.point_m.x,1));
    b.position_m.x=1.5;
    CHECK(ps_contact_convexes(&a,&mesh,&b,&mesh,&c,&hit)==PS_OK && hit && near(c.penetration_m,.5) && near(c.normal.x,1) && near(c.point_m.x,.75));
    ps_contact reverse;
    CHECK(ps_contact_convexes(&b,&mesh,&a,&mesh,&reverse,&hit)==PS_OK && hit && near(reverse.penetration_m,.5) && near(reverse.normal.x,-1));
    b.position_m.x=0;
    ps_vec3 small[8];for(unsigned i=0;i<8;i++)small[i]=ps_vscale(vertices[i],.25);
    ps_convex_mesh inner={small,8,triangles,12};
    CHECK(ps_contact_convexes(&a,&mesh,&b,&inner,&c,&hit)==PS_OK && hit && near(c.penetration_m,1.25));
    a.position_m.y=.5;
    CHECK(ps_contact_convex_plane(&a,&mesh,ps_v3(0,0,0),ps_v3(0,1,0),&c,&hit)==PS_OK && hit && near(c.penetration_m,.5) && near(c.normal.y,-1));
    a.position_m=ps_v3(0,0,0);CHECK(ps_body_sphere(1,.5,&sphere)==PS_OK);
    sphere.position_m=ps_v3(1.4,0,0);
    CHECK(ps_contact_sphere_convex(&sphere,.5,&a,&mesh,&c,&hit)==PS_OK && hit && near(c.penetration_m,.1) && near(c.normal.x,-1) && near(c.point_m.x,.95));
    sphere.position_m=ps_v3(1.4,1.4,0);
    CHECK(ps_contact_sphere_convex(&sphere,.5,&a,&mesh,&c,&hit)==PS_OK && !hit);
    sphere.position_m=ps_v3(0,0,0);
    CHECK(ps_contact_sphere_convex(&sphere,.5,&a,&mesh,&c,&hit)==PS_OK && hit && near(c.penetration_m,1.5));
    ps_aabb bounds;CHECK(ps_aabb_convex(&a,&mesh,&bounds)==PS_OK && bounds.minimum_m.x<=-1 && bounds.maximum_m.x>=1);
    uint32_t bad[12][3];memcpy(bad,triangles,sizeof bad);bad[0][0]=88;
    ps_convex_mesh invalid={vertices,8,(const uint32_t (*)[3])bad,12};c=saved;hit=true;
    CHECK(ps_contact_convexes(&a,&invalid,&b,&mesh,&c,&hit)==PS_INVALID && hit && !memcmp(&c,&saved,sizeof c));
    memcpy(bad,triangles,sizeof bad);bad[0][1]=1;bad[0][2]=2;CHECK(ps_convex_validate(&invalid)==PS_INVALID);
    invalid=mesh;invalid.triangle_count=11;CHECK(ps_convex_validate(&invalid)==PS_INVALID);
    invalid=mesh;invalid.vertex_count=PS_CONVEX_MAX_VERTICES+1;CHECK(ps_convex_validate(&invalid)==PS_LIMIT);
    ps_vec3 dented[8];memcpy(dented,vertices,sizeof dented);dented[6]=ps_v3(0,0,0);invalid=(ps_convex_mesh){dented,8,triangles,12};CHECK(ps_convex_validate(&invalid)==PS_INVALID);
    invalid=mesh;invalid.vertices_m=NULL;CHECK(ps_convex_validate(&invalid)==PS_INVALID);
    CHECK(ps_convex_validate(NULL)==PS_INVALID);
    /* Full-capacity closed bipyramid: 64 vertices and 124 triangles. */
    ps_vec3 many[64];uint32_t many_t[124][3];
    for(unsigned i=0;i<62;i++)many[i]=ps_v3(cos(6.283185307179586*i/62),sin(6.283185307179586*i/62),0);
    many[62]=ps_v3(0,0,1);many[63]=ps_v3(0,0,-1);
    for(unsigned i=0;i<62;i++) {
        many_t[2*i][0]=i;many_t[2*i][1]=(i+1)%62;many_t[2*i][2]=62;
        many_t[2*i+1][0]=(i+1)%62;many_t[2*i+1][1]=i;many_t[2*i+1][2]=63;
    }
    ps_convex_mesh capacity={many,64,(const uint32_t (*)[3])many_t,124};
    CHECK(ps_convex_validate(&capacity)==PS_OK);
    b.position_m=ps_v3(3,0,0);
    CHECK(ps_contact_convexes(&a,&capacity,&b,&capacity,&c,&hit)==PS_OK && !hit);
    b.position_m=ps_v3(.5,0,0);
    CHECK(ps_contact_convexes(&a,&capacity,&b,&capacity,&c,&hit)==PS_OK && hit);
    capacity.triangle_count=129;CHECK(ps_convex_validate(&capacity)==PS_LIMIT);
    for(unsigned i=0;i<8;i++)dented[i]=vertices[i];dented[0].x=NAN;
    invalid=(ps_convex_mesh){dented,8,triangles,12};CHECK(ps_convex_validate(&invalid)==PS_INVALID);
    /* Same contact at extreme SI scales, with no unscaled squared geometry. */
    for(unsigned run=0;run<2;run++) {
        double scale=run?1e200:1e-200;ps_vec3 scaled[8];
        for(unsigned i=0;i<8;i++)scaled[i]=ps_vscale(vertices[i],scale);
        ps_convex_mesh extreme={scaled,8,triangles,12};
        b.position_m=ps_v3(1.5*scale,0,0);
        CHECK(ps_contact_convexes(&a,&extreme,&b,&extreme,&c,&hit)==PS_OK && hit);
        CHECK(fabs(c.penetration_m/scale-.5)<1e-12 && near(c.normal.x,1));
    }
    b.position_m=ps_v3(0,0,0);
    sphere.position_m=ps_v3(1,0,0);
    CHECK(ps_contact_sphere_convex(&sphere,.5,&a,&mesh,&c,&hit)==PS_OK && hit && near(c.normal.x,-1));
    ps_body bad_body=a;bad_body.orientation.w=2;c=saved;hit=true;
    CHECK(ps_contact_convexes(&bad_body,&mesh,&b,&mesh,&c,&hit)==PS_INVALID && hit && !memcmp(&c,&saved,sizeof c));
    ps_body far_a=a,far_b=a;far_a.position_m.x=-DBL_MAX;far_b.position_m.x=DBL_MAX;
    c=saved;hit=true;
    CHECK(ps_contact_convexes(&far_a,&mesh,&far_b,&mesh,&c,&hit)==PS_NUMERIC && hit && !memcmp(&c,&saved,sizeof c));
    ps_aabb old_bounds=bounds;far_a.position_m.x=DBL_MAX;
    CHECK(ps_aabb_convex(&far_a,&mesh,&bounds)==PS_NUMERIC && !memcmp(&bounds,&old_bounds,sizeof bounds));
    /* Single contact drives the existing impulse solver; central sphere impact. */
    sphere.position_m=ps_v3(1.4,0,0);sphere.velocity_m_s=ps_v3(-1,0,0);
    CHECK(ps_contact_sphere_convex(&sphere,.5,&a,&mesh,&c,&hit)==PS_OK && hit);
    ps_vec3 impulse;CHECK(ps_contact_resolve(&sphere,&a,&c,1,0,&impulse)==PS_OK);
    CHECK(near(sphere.velocity_m_s.x,0) && near(a.velocity_m_s.x,-1));
    puts("Convex mesh validation, SAT, containment, mixed contacts, bounds, atomic errors and impulse response passed");
    return 0;
}
