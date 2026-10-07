#include "physim/numerics.h"
#include <math.h>
#include <stdio.h>
typedef struct {
    double scale, target, lo, hi;
    unsigned mode, calls;
    bool outside;
    double initial[2];
} sample;
static double callback(double x, void *user) {
    sample *s = user;
    if (s->calls < 2)
        s->initial[s->calls] = x;
    s->calls++;
    if (!isfinite(x) || x < s->lo || x > s->hi)
        s->outside = true;
    if (s->mode == 0)
        return 1;
    double value = x / s->scale - s->target / s->scale;
    return s->mode == 1 ? value : fabs(value);
}
int main(void) {
    unsigned method, limit;
    double lo, hi, absolute, relative;
    sample s;
    while (scanf("%u %u %la %la %la %la %la %la %u", &method, &s.mode, &lo, &hi, &absolute,
                 &relative, &s.scale, &s.target, &limit) == 9) {
        s.lo = lo;
        s.hi = hi;
        s.calls = 0;
        s.outside = false;
        ps_scalar_report r = {7, 8, 9, 10, 11, 12};
        ps_result code =
            method ? ps_minimize_golden(callback, &s, lo, hi, absolute, relative, limit, &r)
                   : ps_root_bisect(callback, &s, lo, hi, absolute, relative, limit, &r);
        printf("%d %a %a %a %a %u %u %u %d %a %a\n", code, r.x, r.value, r.lower, r.upper,
               r.iterations, r.evaluations, s.calls, s.outside, s.initial[0], s.initial[1]);
    }
    return ferror(stdin) ? 2 : 0;
}
