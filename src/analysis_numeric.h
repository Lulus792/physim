#ifndef PHYSIM_ANALYSIS_NUMERIC_H
#define PHYSIM_ANALYSIS_NUMERIC_H
#include <float.h>
#include <math.h>
/* Internal shared arithmetic. Callers validate finite inputs and xa < xb. */
/* Scale only overflowing differences. Scaling both operands preserves a
 * representable secant even when dy and dx themselves exceed DBL_MAX. */
static inline double ps_numeric_secant(double xa, double xb, double ya, double yb) {
    double dx = xb - xa, dy = yb - ya;
    int shift_x = !isfinite(dx), shift_y = !isfinite(dy);
    if (shift_x) dx = xb * .5 - xa * .5;
    if (shift_y) dy = yb * .5 - ya * .5;
    int ex, ey;
    double mx = frexp(dx, &ex), my = frexp(dy, &ey);
    return scalbn(my / mx, ey - ex + shift_y - shift_x);
}

static inline double ps_numeric_trapezoid(double xa, double xb, double ya, double yb) {
    double width = xb - xa, average = yb * .5 + ya * .5;
    int extra = !isfinite(width), ew, ea;
    if (extra) width = xb * .5 - xa * .5;
    /* Preserve subnormal means by scaling the sum upwards when needed. */
    int small = fabs(yb) < DBL_MIN && fabs(ya) < DBL_MIN;
    if (small) average = yb + ya;
    double mw = frexp(width, &ew), ma = frexp(average, &ea);
    double area = scalbn(mw * ma, ew + ea + extra - small);
    return area;
}
#endif
