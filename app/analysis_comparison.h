#ifndef PS_ANALYSIS_COMPARISON_H
#define PS_ANALYSIS_COMPARISON_H
#include "graphic_snapshot.h"

/* Main-thread-owned cache. A single worker owns an immutable request and result;
 * latest selection replaces pending work, never an unbounded queue. */
typedef struct ps_comparison ps_comparison;
typedef struct {
    char scope[4096],scratch_prefix[4096],paths[PS_GRAPHIC_MAX_INPUTS][PS_GRAPHIC_PATH_BYTES];
    uint64_t revision,scratch_quota;
    uint32_t count,reference;
    ps_channel channel;
    ps_report_unit unit;
} ps_comparison_key;
typedef struct {
    const ps_graphic_snapshot *snapshot; /* borrowed until request/poll/destroy */
    ps_result result;
    uint32_t failed_input;
    uint64_t requested,published,discarded;
    bool loading;
} ps_comparison_state;
ps_comparison *ps_comparison_create(void);
/* Own copies; no file I/O or blocking join. count=0 clears the view and cancels.
 * Equal keys are idempotent, including a failed result. Change revision to retry. */
ps_result ps_comparison_request(ps_comparison *cache,const ps_comparison_key *key);
void ps_comparison_poll(ps_comparison *cache);
ps_comparison_state ps_comparison_read(const ps_comparison *cache);
/* Cancels and joins the one owned worker before releasing requests/results. */
void ps_comparison_destroy(ps_comparison *cache);
#endif
