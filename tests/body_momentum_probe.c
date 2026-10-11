#include "physim/mechanics.h"
#include <stdio.h>
#include <string.h>
int main(void) {
    ps_body b={0}; b.orientation.w=1;
    ps_vec3 origin;
    while (scanf("%la %la %la %la %la %la %la %la %la %la %la %la %la %la %la %la",
                 &b.mass_kg,&b.inertia_kg_m2.x,&b.inertia_kg_m2.y,&b.inertia_kg_m2.z,
                 &b.position_m.x,&b.position_m.y,&b.position_m.z,
                 &origin.x,&origin.y,&origin.z,
                 &b.velocity_m_s.x,&b.velocity_m_s.y,&b.velocity_m_s.z,
                 &b.angular_velocity_rad_s.x,&b.angular_velocity_rad_s.y,&b.angular_velocity_rad_s.z)==16) {
        ps_body saved=b;
        ps_vec3 p={17,19,23},l=p,sentinel=p;
        ps_result ps=ps_body_linear_momentum(&b,&p),ls=ps_body_angular_momentum(&b,origin,&l);
        if (memcmp(&b,&saved,sizeof b) || (ps!=PS_OK && memcmp(&p,&sentinel,sizeof p)) ||
            (ls!=PS_OK && memcmp(&l,&sentinel,sizeof l))) return 3;
        printf("%d %a %a %a %d %a %a %a\n",ps,p.x,p.y,p.z,ls,l.x,l.y,l.z);
    }
    return ferror(stdin)?2:0;
}
