/* Uniform solid regular tetrahedron, centroid and principal axes at the origin.
 * Vertex covariance is diag(1/5,1/5,1/5), so each principal inertia is 2*m/5.
 * Geometry plus one force-free linear sweep event; no resting-manifold claim. */
#include "physim/collision.h"
#include <math.h>
#include <stdio.h>
int main(void) {
    const ps_vec3 vertices[]={{1,1,1},{1,-1,-1},{-1,1,-1},{-1,-1,1}};
    const uint32_t triangles[][3]={{0,1,2},{0,3,1},{0,2,3},{1,3,2}};
    const ps_convex_mesh mesh={vertices,4,triangles,4};
    ps_body body,sphere;
    if(ps_body_with_inertia(1,ps_v3(.4,.4,.4),&body)!=PS_OK ||
       ps_body_sphere(1,.5,&sphere)!=PS_OK)return 1;
    sphere.position_m=ps_v3(1.2,1.2,1.2);
    ps_contact contact;bool hit;
    if(ps_contact_sphere_convex(&sphere,.5,&body,&mesh,&contact,&hit)!=PS_OK || !hit ||
       fabs(contact.penetration_m-(.5-sqrt(.12)))>1e-12)return 2;
    ps_aabb bounds;
    if(ps_aabb_convex(&body,&mesh,&bounds)!=PS_OK || bounds.minimum_m.x>-1 || bounds.maximum_m.x<1)return 3;
    body.orientation=ps_quat_axis_angle(ps_v3(0,0,1),.4);
    if(ps_contact_convex_plane(&body,&mesh,ps_v3(0,0,-.8),ps_v3(0,0,1),&contact,&hit)!=PS_OK || !hit || fabs(contact.penetration_m-.2)>1e-12)return 4;
    /* One force-free event against a static tetrahedron, then the remainder.
     * Translation before impact, central vertex impulse, no induced rotation. */
    ps_body wall;
    if(ps_body_with_inertia(0,ps_v3(0,0,0),&wall)!=PS_OK)return 5;
    sphere.position_m=ps_v3(10,1,1);sphere.velocity_m_s=ps_v3(-20,0,0);
    ps_sweep_hit event;
    if(ps_sweep_sphere_convex(&sphere,.5,ps_v3(-20,0,0),&wall,&mesh,ps_v3(0,0,0),&event,&hit)!=PS_OK || !hit || fabs(event.fraction-.425)>1e-12)return 6;
    if(ps_body_step(&sphere,ps_v3(0,0,0),ps_v3(0,0,0),event.fraction)!=PS_OK ||
       ps_contact_resolve(&sphere,&wall,&event.contact,1,0,NULL)!=PS_OK ||
       ps_body_step(&sphere,ps_v3(0,0,0),ps_v3(0,0,0),1-event.fraction)!=PS_OK)return 7;
    double energy;
    if(ps_body_kinetic_energy(&sphere,&energy)!=PS_OK || fabs(energy-200)>1e-10 || fabs(sphere.position_m.x-13)>1e-10)return 8;
    /* A full-turn tetrahedron is clear at both endpoints but reaches the plane
     * between them. Its earliest vertex height solves sin(theta)+cos(theta)=1.2. */
    ps_body rotor;
    if(ps_body_with_inertia(1,ps_v3(.4,.4,.4),&rotor)!=PS_OK)return 9;
    rotor.position_m.y=1.2;rotor.angular_velocity_rad_s.z=2*PS_PI;
    ps_rigid_motion spin={ps_v3(0,0,0),ps_v3(0,0,2*PS_PI),ps_v3(0,0,0)};
    if(ps_sweep_convex_plane_motion(&rotor,&mesh,spin,ps_v3(0,0,0),ps_v3(0,1,0),NULL,&event,&hit)!=PS_OK || !hit)return 10;
    double reference=(asin(1.2/sqrt(2.0))-PS_PI/4)/(2*PS_PI);
    if(fabs(event.fraction-reference)>1e-7)return 11;
    puts("Convex tetrahedron geometry passed");
    return 0;
}
