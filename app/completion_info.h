#ifndef PHYSIM_COMPLETION_INFO_H
#define PHYSIM_COMPLETION_INFO_H
#include "physim/core.h"
#define PS_COMPLETION_INFO_DELAY 0.280
typedef enum {PS_COMPLETION_INFO_HIDDEN,PS_COMPLETION_INFO_PENDING,PS_COMPLETION_INFO_VISIBLE} ps_completion_info_mode;
typedef struct {
    const void *editor;
    size_t candidate,cursor,bytes;
    const char *text;
    double scroll_x,scroll_y;
} ps_completion_info_key;
typedef struct {
    ps_completion_info_mode mode;
    const void *editor;
    size_t candidate,cursor,bytes;
    char *text;
    double scroll_x,scroll_y,due;
} ps_completion_info;
/* Zero-initialize. One owned exact source snapshot, reused for unchanged bytes.
 * Explicit requests set the deadline; idle updates never postpone it. Invalid
 * requests preserve the old state. No worker, OS timer or source execution. */
ps_result ps_completion_info_request(ps_completion_info *info,const ps_completion_info_key *key,double now,bool immediate);
/* Current owner/candidate/cursor/viewport and every source byte must still match.
 * Mismatch cancels and frees the request, including an already visible card. */
bool ps_completion_info_update(ps_completion_info *info,const ps_completion_info_key *key,double now);
void ps_completion_info_cancel(ps_completion_info *info);
#endif
