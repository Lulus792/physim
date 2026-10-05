#ifndef PS_PACING_H
#define PS_PACING_H
#include <stdbool.h>
#include <math.h>

/* Wall-clock scheduling only. The caller supplies the proposed physics duration.
 * Speed 0 is offline; real-time multipliers range from 0.1 to 16. */
typedef struct {
    double speed, credit, last;
    bool running;
} ps_pacer;
static inline bool ps_speed_valid(double speed) {
    return isfinite(speed) && (speed == 0 || (speed >= .1 && speed <= 16));
}
static inline void ps_pacer_restart(ps_pacer *p, double now) {
    p->credit = 0;
    p->last = now;
}
static inline bool ps_pacer_due(ps_pacer *p, double now, double dt) {
    double elapsed = fmax(0, now - p->last);
    p->last = now;
    if (!p->running) {
        p->credit = 0;
        return false;
    }
    if (p->speed == 0)
        return true;
    /* Discard excessive wall-clock debt after suspension or backpressure.
     * Keep at least one full dt so slow speeds and large steps still progress. */
    double maximum = fmax(dt, .25 * p->speed);
    p->credit = fmin(maximum, p->credit + fmin(elapsed, .25) * p->speed);
    if (p->credit < dt)
        return false;
    p->credit -= dt;
    return true;
}
/* A rejected numerical trial can shorten an accepted adaptive step. Return
 * the unused scheduling credit; single steps bypass the pacer entirely. */
static inline void ps_pacer_refund(ps_pacer *p,double proposed,double accepted) {
    if(p->running && p->speed!=0 && accepted<proposed)
        p->credit += proposed-accepted;
}
#endif
