#include "physim/math.h"
#include <stdio.h>
int main(void) {
    ps_bezier3 curve;
    double t;
    while (scanf("%la", &t) == 1) {
        for (unsigned i = 0; i < 4; i++)
            if (scanf("%la %la %la", &curve.points[i].x, &curve.points[i].y, &curve.points[i].z) != 3)
                return 2;
        ps_curve_sample3 out = {ps_v3(7, 8, 9), ps_v3(10, 11, 12)};
        ps_bezier3 left = {{{13, 14, 15}}}, right = {{{16, 17, 18}}};
        ps_result r = ps_bezier3_evaluate(&curve, t, &out);
        ps_result split = ps_bezier3_split(&curve, t, &left, &right);
        printf("%d %d %a %a %a %a %a %a", r, split, out.position.x, out.position.y,
               out.position.z, out.tangent.x, out.tangent.y, out.tangent.z);
        for (unsigned side = 0; side < 2; side++)
            for (unsigned i = 0; i < 4; i++) {
                ps_vec3 p = side ? right.points[i] : left.points[i];
                printf(" %a %a %a", p.x, p.y, p.z);
            }
        putchar('\n');
    }
    return ferror(stdin) ? 2 : 0;
}
