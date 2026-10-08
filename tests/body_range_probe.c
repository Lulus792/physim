#include "physim/mechanics.h"
#include <stdio.h>
#include <string.h>

int main(void) {
    unsigned mode;
    double mass;
    ps_vec3 size, velocity, angular;
    while (scanf("%u %la %la %la %la %la %la %la %la %la %la", &mode, &mass,
                 &size.x, &size.y, &size.z, &velocity.x, &velocity.y, &velocity.z,
                 &angular.x, &angular.y, &angular.z) == 11) {
        ps_body body = {0};
        body.mass_kg = 77;
        body.orientation.w = 1;
        ps_body saved = body;
        ps_result status;
        if (mode == 2) {
            body.mass_kg = mass;
            body.inertia_kg_m2 = size;
            if (scanf("%la %la %la %la", &body.orientation.x, &body.orientation.y,
                      &body.orientation.z, &body.orientation.w) != 4)
                return 2;
            status = PS_OK;
        } else {
            status = mode == 0 ? ps_body_sphere(mass, size.x, &body)
                               : ps_body_box(mass, size, &body);
        }
        double energy = 99;
        ps_result es = status;
        if (status == PS_OK) {
            body.velocity_m_s = velocity;
            body.angular_velocity_rad_s = angular;
            ps_body before = body;
            es = ps_body_kinetic_energy(&body, &energy);
            if (memcmp(&body, &before, sizeof body))
                return 3;
            if (es != PS_OK && energy != 99)
                return 5;
        } else if (memcmp(&body, &saved, sizeof body)) {
            return 4;
        }
        printf("%d %a %a %a %d %a\n", status, body.inertia_kg_m2.x,
               body.inertia_kg_m2.y, body.inertia_kg_m2.z, es, energy);
    }
    return ferror(stdin) ? 2 : 0;
}
