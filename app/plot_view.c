#include "plot_view.h"
#include <math.h>
#include <stddef.h>
#define MIN_SPAN 1e-6
void ps_plot_view_reset(ps_plot_view *v) {
    if (v)
        *v = (ps_plot_view){{0, 0}, {1, 1}, true};
}
static bool valid(const ps_plot_view *v) {
    if (!v || !v->initialized)
        return false;
    for (unsigned i = 0; i < 2; i++)
        if (!isfinite(v->low[i]) || !isfinite(v->high[i]) || v->low[i] < 0 || v->high[i] > 1 ||
            v->low[i] >= v->high[i])
            return false;
    return true;
}
bool ps_plot_view_changed(const ps_plot_view *v) {
    return valid(v) && (v->low[0] != 0 || v->low[1] != 0 || v->high[0] != 1 || v->high[1] != 1);
}
bool ps_plot_view_zoom(ps_plot_view *v, double factor, double x, double y) {
    if (!valid(v) || !isfinite(factor) || factor <= 0 || !isfinite(x) || !isfinite(y) || x < 0 ||
        x > 1 || y < 0 || y > 1)
        return false;
    ps_plot_view next = *v;
    const double anchor[] = {x, y};
    for (unsigned i = 0; i < 2; i++) {
        double span = v->high[i] - v->low[i], size = fmin(1, fmax(MIN_SPAN, span / factor));
        double position = v->low[i] + anchor[i] * span;
        next.low[i] = fmin(1 - size, fmax(0, position - anchor[i] * size));
        next.high[i] = next.low[i] + size;
    }
    *v = next;
    return true;
}
bool ps_plot_view_pan(ps_plot_view *v, double x, double y) {
    if (!valid(v) || !isfinite(x) || !isfinite(y))
        return false;
    ps_plot_view next = *v;
    const double delta[] = {x, y};
    for (unsigned i = 0; i < 2; i++) {
        double span = v->high[i] - v->low[i];
        next.low[i] = fmin(1 - span, fmax(0, v->low[i] + delta[i] * span));
        next.high[i] = next.low[i] + span;
    }
    *v = next;
    return true;
}
double ps_plot_axis_value(double low, double high, double f) {
    if (!isfinite(low) || !isfinite(high) || low > high || !isfinite(f) || f < 0 || f > 1)
        return NAN;
    if (f == 0)
        return low;
    if (f == 1)
        return high;
    double range = high - low;
    return isfinite(range) ? low + f * range : (1 - f) * low + f * high;
}
double ps_plot_axis_offset(double low, double high) {
    double span = high - low;
    return isfinite(low) && isfinite(high) && span > 0 && isfinite(span) && fabs(low) / span > 1000
               ? low
               : 0;
}
bool ps_plot_clip_line(double *x0, double *y0, double *x1, double *y1) {
    if (!x0 || !y0 || !x1 || !y1 || !isfinite(*x0) || !isfinite(*y0) || !isfinite(*x1) ||
        !isfinite(*y1))
        return false;
    double dx = *x1 - *x0, dy = *y1 - *y0, enter = 0, leave = 1;
    if (!isfinite(dx) || !isfinite(dy))
        return false;
    const double p[] = {-dx, dx, -dy, dy}, q[] = {*x0, 1 - *x0, *y0, 1 - *y0};
    for (unsigned i = 0; i < 4; i++) {
        if (p[i] == 0) {
            if (q[i] < 0)
                return false;
        } else {
            double t = q[i] / p[i];
            if (p[i] < 0)
                enter = fmax(enter, t);
            else
                leave = fmin(leave, t);
            if (enter > leave)
                return false;
        }
    }
    double a = *x0, b = *y0;
    *x0 = fmin(1, fmax(0, a + enter * dx));
    *y0 = fmin(1, fmax(0, b + enter * dy));
    *x1 = fmin(1, fmax(0, a + leave * dx));
    *y1 = fmin(1, fmax(0, b + leave * dy));
    return true;
}
