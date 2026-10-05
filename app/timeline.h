#ifndef PHYSIM_TIMELINE_H
#define PHYSIM_TIMELINE_H
#include "physim/snapshot.h"
/* Bounded preview only. Every recorded snapshot remains in the run file. */
#define PS_TIMELINE_CAPACITY 2048u
typedef struct {
    ps_snapshot *frames;
    ps_snapshot latest;
    uint32_t count, capacity;
    uint64_t seen, stride;
} ps_timeline;
void ps_timeline_clear(ps_timeline *timeline);
void ps_timeline_destroy(ps_timeline *timeline);
/* Accepts validated states at nondecreasing times. Equal timestamps replace the
 * last state; preview thinning always preserves the first and latest state. */
ps_result ps_timeline_push(ps_timeline *timeline, const ps_snapshot *snapshot);
uint32_t ps_timeline_count(const ps_timeline *timeline);
const ps_snapshot *ps_timeline_get(const ps_timeline *timeline, uint32_t index);
uint32_t ps_timeline_nearest(const ps_timeline *timeline, double time);
#endif
