#ifndef PS_STEP_BREAKPOINTS_H
#define PS_STEP_BREAKPOINTS_H
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#define PS_STEP_BREAKPOINT_MAX 32u
#define PS_STEP_BREAKPOINT_TEXT 384u
#define PS_STEP_BREAKPOINT_LIMIT UINT64_C(1000000000)
/* Run-local, sorted, one-shot stops at accepted step boundaries. */
typedef struct {
    uint64_t step[PS_STEP_BREAKPOINT_MAX];
    unsigned count, next;
} ps_step_breakpoints;
static inline bool ps_step_breakpoint_add(ps_step_breakpoints *points, uint64_t step) {
    if (!points || points->count >= PS_STEP_BREAKPOINT_MAX || step > PS_STEP_BREAKPOINT_LIMIT)
        return false;
    unsigned at = 0;
    while (at < points->count && points->step[at] < step) at++;
    if (at < points->count && points->step[at] == step) return false;
    memmove(points->step + at + 1, points->step + at,
            (points->count - at) * sizeof points->step[0]);
    points->step[at] = step; points->count++;
    return true;
}
/* Empty text disables stops. Decimal integers, comma-separated; ASCII spaces
 * around tokens are permitted. A failed parse preserves the output. */
static inline bool ps_step_breakpoints_parse(const char *text, ps_step_breakpoints *out) {
    if (!text || !out) return false;
    ps_step_breakpoints draft = {0};
    while (*text == ' ' || *text == '\t') text++;
    if (!*text) { *out = draft; return true; }
    for (;;) {
        if (*text < '0' || *text > '9') return false;
        uint64_t step = 0;
        do {
            unsigned digit = (unsigned)(*text++ - '0');
            if (step > (PS_STEP_BREAKPOINT_LIMIT - digit) / 10) return false;
            step = step * 10 + digit;
        } while (*text >= '0' && *text <= '9');
        if (!ps_step_breakpoint_add(&draft, step)) return false;
        while (*text == ' ' || *text == '\t') text++;
        if (!*text) { *out = draft; return true; }
        if (*text++ != ',') return false;
        while (*text == ' ' || *text == '\t') text++;
    }
}
static inline bool ps_step_breakpoint_hit(ps_step_breakpoints *points, uint64_t step) {
    while (points->next < points->count && points->step[points->next] < step) points->next++;
    if (points->next >= points->count || points->step[points->next] != step) return false;
    points->next++;
    return true;
}
#endif
