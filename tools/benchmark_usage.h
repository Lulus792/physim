#ifndef PS_BENCHMARK_USAGE_H
#define PS_BENCHMARK_USAGE_H
#include "platform.h"
#include <math.h>

typedef struct {
    double wall;
    ps_process_usage usage;
} ps_benchmark_mark;

static bool ps_benchmark_begin(ps_benchmark_mark *mark) {
    if (!ps_process_usage_self(&mark->usage)) return false;
    mark->wall = ps_clock();
    return isfinite(mark->wall);
}

/* Clock interval excludes the resource queries. CPU includes their small cost.
 * CPU sums all threads and may exceed elapsed wall time. Peak is not subtracted. */
static bool ps_benchmark_end(ps_benchmark_mark mark, double *wall,
                              ps_process_usage *delta) {
    double end = ps_clock();
    ps_process_usage usage;
    if (!ps_process_usage_self(&usage) || !isfinite(end) || end < mark.wall ||
        usage.user_seconds < mark.usage.user_seconds ||
        usage.system_seconds < mark.usage.system_seconds) return false;
    *wall = end - mark.wall;
    *delta = (ps_process_usage){usage.user_seconds - mark.usage.user_seconds,
                               usage.system_seconds - mark.usage.system_seconds,
                               usage.peak_resident_bytes};
    return true;
}
#endif
