#ifndef PHYSIM_PROTOCOL_STORE_H
#define PHYSIM_PROTOCOL_STORE_H
#include "physim/log.h"

#define PS_PROTOCOL_CAPACITY 200u
#define PS_PROTOCOL_BYTE_BUDGET (4u * 1024u * 1024u)
typedef enum { PS_PROTOCOL_APP, PS_PROTOCOL_BUILD, PS_PROTOCOL_SIMULATION,
               PS_PROTOCOL_ANALYSIS, PS_PROTOCOL_ORIGIN_COUNT } ps_protocol_origin;
/* Supplied by the host's actual process state, never inferred from log level. */
typedef enum { PS_PROTOCOL_NO_OUTCOME, PS_PROTOCOL_RUNNING, PS_PROTOCOL_SUCCEEDED,
               PS_PROTOCOL_FAILED, PS_PROTOCOL_CANCELLED } ps_protocol_outcome;
typedef struct {
    ps_log_level level;
    ps_protocol_origin origin;
    ps_protocol_outcome outcome;
    uint64_t transcript_id; /* Optional complete process output; may be evicted. */
    const char *title, *context, *source;
    const char *operation, *argument;
    bool has_code;
    ps_result code;
    uint32_t line, column;
    bool has_time;
    double time_s;
    const void *raw;
    size_t raw_size;
} ps_protocol_message;
typedef struct {
    uint64_t id, omitted_bytes;
    ps_protocol_message message;
} ps_protocol_record;
typedef struct ps_protocol_entry ps_protocol_entry;
/* Zero initialization is sufficient. One owner thread; not synchronized.
 * Private fields must only be changed through this API. */
typedef struct {
    ps_protocol_entry *_entries[PS_PROTOCOL_CAPACITY];
    size_t _head, _count, _bytes;
    uint64_t _next_id, _omitted_messages, _omitted_bytes;
} ps_protocol;
typedef struct {
    size_t count, bytes;
    uint64_t omitted_messages, omitted_bytes;
    size_t levels[PS_LOG_ERROR+1], origins[PS_PROTOCOL_ORIGIN_COUNT];
} ps_protocol_summary;
/* Owns full copies including raw embedded NUL/LF/CR/tab/UTF-8 bytes. No text
 * truncation. Invalid input preserves everything. Capacity eviction is oldest
 * first; limits/allocation failure are explicit and counted as omissions.
 * Allocation before eviction can temporarily use twice the retained budget. */
ps_result ps_protocol_push(ps_protocol *store, const ps_protocol_message *message, uint64_t *id);
/* Append preserves identity/metadata. Failure preserves stored bytes and marks
 * the transcript incomplete. Missing IDs/invalid arguments change nothing. */
ps_result ps_protocol_append(ps_protocol *store, uint64_t id, const void *raw, size_t size);
bool ps_protocol_set_outcome(ps_protocol *store, uint64_t id, ps_protocol_outcome outcome);
/* Explicit removal for a superseded host diagnostic; not capacity loss. */
bool ps_protocol_remove(ps_protocol *store, uint64_t id);
/* Chronological order and stable IDs. Borrowed pointers survive only until the
 * next mutation. Clear frees all memory but never reuses a previous ID. */
const ps_protocol_record *ps_protocol_at(const ps_protocol *store, size_t index);
const ps_protocol_record *ps_protocol_find(const ps_protocol *store, uint64_t id);
ps_protocol_summary ps_protocol_describe(const ps_protocol *store);
void ps_protocol_clear(ps_protocol *store);
#endif
