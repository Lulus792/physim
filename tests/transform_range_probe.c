#include "physim/math.h"
#include <stdio.h>
int main(void) {
    unsigned mode;
    ps_mat4 m;
    ps_vec3 v, r;
    while (scanf("%u", &mode) == 1) {
        for (size_t i = 0; i < 16; i++)
            if (scanf("%la", m.m + i) != 1)
                return 2;
        if (scanf("%la %la %la", &v.x, &v.y, &v.z) != 3)
            return 2;
        r = ps_v3(7, 8, 9);
        ps_result s = mode == 0   ? ps_transform_point(m, v, &r)
                      : mode == 1 ? ps_transform_direction(m, v, &r)
                                  : ps_transform_normal(m, v, &r);
        printf("%d %a %a %a\n", s, r.x, r.y, r.z);
    }
    return ferror(stdin) ? 2 : 0;
}
