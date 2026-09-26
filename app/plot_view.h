#ifndef PHYSIM_PLOT_VIEW_H
#define PHYSIM_PLOT_VIEW_H
#include <stdbool.h>
/* Internal display state, normalized to the full data bounds. No data mutation. */
typedef struct {
    double low[2], high[2];
    bool initialized;
} ps_plot_view;
void ps_plot_view_reset(ps_plot_view *view);
bool ps_plot_view_zoom(ps_plot_view *view, double factor, double x, double y);
bool ps_plot_view_pan(ps_plot_view *view, double x, double y);
bool ps_plot_view_changed(const ps_plot_view *view);
double ps_plot_axis_value(double low, double high, double fraction);
/* Separate a large common offset so zoomed tick labels remain distinguishable. */
double ps_plot_axis_offset(double low, double high);
/* Clip a segment in normalized viewport coordinates to [0,1]^2. */
bool ps_plot_clip_line(double *x0, double *y0, double *x1, double *y1);
#endif
