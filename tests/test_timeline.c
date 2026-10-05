#include "timeline.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "Timeline line %d: %s\n", __LINE__, #x); return 1; } } while (0)
int main(void) {
    ps_timeline t = {0};
    CHECK(!ps_timeline_count(&t) && !ps_timeline_get(&t, 0));
    ps_snapshot s = {.count = 1, .paused = true};
    CHECK(ps_timeline_push(&t, &s) == PS_OK);
    for (unsigned i = 1; i <= 10000; i++) {
        s.time = i * .001;
        s.values[0] = i;
        CHECK(ps_timeline_push(&t, &s) == PS_OK);
        CHECK(ps_timeline_count(&t) <= PS_TIMELINE_CAPACITY);
        CHECK(ps_timeline_get(&t, 0)->time == 0);
        CHECK(ps_timeline_get(&t, ps_timeline_count(&t) - 1)->time == s.time);
    }
    CHECK(t.seen == 10001 && t.stride > 1);
    uint32_t count = ps_timeline_count(&t);
    double previous = -1;
    for (uint32_t i = 0; i < count; i++) {
        const ps_snapshot *frame = ps_timeline_get(&t, i);
        CHECK(frame->time > previous && frame->values[0] * .001 == frame->time);
        CHECK(ps_timeline_nearest(&t, frame->time) == i);
        previous = frame->time;
    }
    CHECK(ps_timeline_nearest(&t, -1) == 0 && ps_timeline_nearest(&t, 20) == count - 1);
    ps_snapshot frozen = *ps_timeline_get(&t, count / 2), saved = frozen;
    uint64_t seen = t.seen;
    s.values[0] = -7;
    CHECK(ps_timeline_push(&t, &s) == PS_OK && t.seen == seen);
    CHECK(ps_timeline_get(&t, count - 1)->values[0] == -7);
    CHECK(!memcmp(&saved, &frozen, sizeof saved));
    s.time = -1;
    CHECK(ps_timeline_push(&t, &s) == PS_INVALID && t.seen == seen);
    s.time = 9;
    CHECK(ps_timeline_push(&t, &s) == PS_INVALID && t.latest.time == 10);
    s.time = 11; s.count = 2;
    CHECK(ps_timeline_push(&t, &s) == PS_INVALID);
    ps_timeline_clear(&t);
    CHECK(ps_timeline_count(&t) == 0 && t.capacity <= PS_TIMELINE_CAPACITY);
    s.time = 0; s.count = 1;
    CHECK(ps_timeline_push(&t, &s) == PS_OK && t.seen == 1);
    ps_timeline_destroy(&t);
    CHECK(!t.frames && !t.capacity && !ps_timeline_count(&t));
    s.time = -2;
    CHECK(ps_timeline_push(&t, &s) == PS_OK && ps_timeline_get(&t, 0)->time == -2);
    s.time = -1;
    CHECK(ps_timeline_push(&t, &s) == PS_OK && ps_timeline_nearest(&t, -1.1) == 1);
    ps_timeline_destroy(&t);
    puts("Timeline thinning preserves initial/latest states, chronology, values, frozen copies and reset.");
    return 0;
}
