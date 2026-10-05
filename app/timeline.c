#include "timeline.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
void ps_timeline_clear(ps_timeline *t) {
    if (!t) return;
    t->count = 0;
    t->seen = 0;
    t->stride = 1;
    memset(&t->latest, 0, sizeof t->latest);
}
void ps_timeline_destroy(ps_timeline *t) {
    if (!t) return;
    free(t->frames);
    memset(t, 0, sizeof *t);
}
ps_result ps_timeline_push(ps_timeline *t, const ps_snapshot *s) {
    if (!t || !s || !isfinite(s->time) || s->count > PS_MAX_CHANNELS ||
        (t->seen && (s->time < t->latest.time || s->count != t->latest.count)))
        return PS_INVALID;
    if (t->seen && s->time == t->latest.time) {
        t->latest = *s;
        if (t->count && t->frames[t->count - 1].time == s->time)
            t->frames[t->count - 1] = *s;
        return PS_OK;
    }
    if (t->seen == UINT64_MAX) return PS_LIMIT;
    if (!t->stride) t->stride = 1;
    if (t->seen % t->stride == 0 && t->count == PS_TIMELINE_CAPACITY - 1) {
        for (uint32_t i = 0; i <= t->count / 2; i++) t->frames[i] = t->frames[i * 2];
        t->count = (t->count + 1) / 2;
        if (t->stride <= UINT64_MAX / 2) t->stride *= 2;
    }
    if (t->seen % t->stride == 0) {
        if (t->count == t->capacity) {
            uint32_t capacity = t->capacity ? t->capacity * 2 : 16;
            if (capacity > PS_TIMELINE_CAPACITY - 1) capacity = PS_TIMELINE_CAPACITY - 1;
            ps_snapshot *frames = realloc(t->frames, capacity * sizeof *frames);
            if (!frames) return PS_MEMORY;
            t->frames = frames;
            t->capacity = capacity;
        }
        t->frames[t->count++] = *s;
    }
    t->latest = *s;
    t->seen++;
    return PS_OK;
}
uint32_t ps_timeline_count(const ps_timeline *t) {
    if (!t || !t->seen) return 0;
    return t->count + (t->latest.time > t->frames[t->count - 1].time);
}
const ps_snapshot *ps_timeline_get(const ps_timeline *t, uint32_t index) {
    uint32_t count = ps_timeline_count(t);
    if (index >= count) return NULL;
    return index < t->count ? &t->frames[index] : &t->latest;
}
uint32_t ps_timeline_nearest(const ps_timeline *t, double time) {
    uint32_t count = ps_timeline_count(t), first = 0, end = count;
    if (!count || !isfinite(time)) return 0;
    while (first < end) {
        uint32_t middle = first + (end - first) / 2;
        if (ps_timeline_get(t, middle)->time < time) first = middle + 1;
        else end = middle;
    }
    if (!first) return 0;
    if (first == count) return count - 1;
    double previous = ps_timeline_get(t, first - 1)->time;
    double next = ps_timeline_get(t, first)->time;
    return time - previous <= next - time ? first - 1 : first;
}
