#include "plot_view.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Plot view line %d: %s\n", __LINE__, #x);                              \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
int main(void) {
    ps_plot_view v = {0};
    CHECK(!ps_plot_view_zoom(&v, 2, .5, .5));
    ps_plot_view_reset(&v);
    CHECK(!ps_plot_view_changed(&v));
    CHECK(ps_plot_view_zoom(&v, 2, .25, .75));
    CHECK(v.low[0] == .125 && v.high[0] == .625 && v.low[1] == .375 && v.high[1] == .875);
    CHECK(ps_plot_view_pan(&v, .25, -.5));
    CHECK(v.low[0] == .25 && v.high[0] == .75 && v.low[1] == .125 && v.high[1] == .625);
    CHECK(ps_plot_view_pan(&v, DBL_MAX, -DBL_MAX) && v.low[0] == .5 && v.high[1] == .5 &&
          v.low[1] == 0);
    CHECK(ps_plot_view_zoom(&v, .01, .5, .5) && !ps_plot_view_changed(&v));
    for (unsigned i = 0; i < 200; i++)
        CHECK(ps_plot_view_zoom(&v, 2, .3, .7));
    CHECK(fabs(v.high[0] - v.low[0] - 1e-6) < 1e-15 && fabs(v.high[1] - v.low[1] - 1e-6) < 1e-15);
    ps_plot_view old = v;
    CHECK(!ps_plot_view_zoom(&v, NAN, .5, .5) && !ps_plot_view_zoom(&v, 2, -1, .5) &&
          !ps_plot_view_pan(&v, INFINITY, 0));
    CHECK(!memcmp(&old, &v, sizeof v));
    CHECK(ps_plot_axis_value(-DBL_MAX, DBL_MAX, .5) == 0);
    CHECK(fabs(ps_plot_axis_value(-DBL_MAX, DBL_MAX, .25) / DBL_MAX + .5) < DBL_EPSILON);
    CHECK(ps_plot_axis_value(DBL_MAX / 2, DBL_MAX, 1) == DBL_MAX);
    CHECK(fabs(ps_plot_axis_value(1e-300, 3e-300, .5) / 2e-300 - 1) < 2 * DBL_EPSILON);
    CHECK(ps_plot_axis_offset(1e6, 1e6 + .001) == 1e6);
    CHECK(ps_plot_axis_offset(-1e6, -1e6 + .001) == -1e6);
    CHECK(ps_plot_axis_offset(-DBL_MAX, DBL_MAX) == 0);
    CHECK(ps_plot_axis_offset(0, 1) == 0 && ps_plot_axis_offset(1, 1) == 0);
    double lines[][4] = {{-2, .5, 2, .5},      {.5, -2, .5, 2}, {-2, -2, 2, 2},
                         {.25, .25, .75, .75}, {-1, 0, 0, 0},   {.5, .5, .5, .5}};
    const double expected[][4] = {{0, .5, 1, .5},       {.5, 0, .5, 1}, {0, 0, 1, 1},
                                  {.25, .25, .75, .75}, {0, 0, 0, 0},   {.5, .5, .5, .5}};
    for (unsigned i = 0; i < 6; i++) {
        double *p = lines[i];
        CHECK(ps_plot_clip_line(p, p + 1, p + 2, p + 3));
        for (unsigned j = 0; j < 4; j++)
            CHECK(fabs(p[j] - expected[i][j]) < 1e-14);
    }
    double x = -1, y = -1, a = -1, b = 2;
    CHECK(!ps_plot_clip_line(&x, &y, &a, &b) && x == -1 && y == -1 && a == -1 && b == 2);
    x = -1e6;
    y = -1e6;
    a = 1e6;
    b = 1e6;
    CHECK(ps_plot_clip_line(&x, &y, &a, &b) && x == 0 && y == 0 && fabs(a - 1) < 1e-9 &&
          fabs(b - 1) < 1e-9);
    x = NAN;
    CHECK(!ps_plot_clip_line(&x, &y, &a, &b));
    puts("Plot zoom, anchored pan, limits, extreme axes and clipping passed.");
    return 0;
}
