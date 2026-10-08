/* Uniform solid regular tetrahedron, centroid and principal axes at the origin.
 * Vertex covariance is diag(1/5,1/5,1/5), so each principal inertia is 2*m/5.
 * Discrete geometry example; no time integration or resting-manifold claim. */
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
    puts("Convex tetrahedron geometry passed");
    return 0;
}
