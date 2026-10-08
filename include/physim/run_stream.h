#ifndef PHYSIM_RUN_STREAM_H
#define PHYSIM_RUN_STREAM_H
#include "data.h"
#include "memory.h"
#define PS_RUN_STREAM_MAX_READERS 8u
#define PS_RUN_STREAM_MAX_WRITERS 8u
typedef struct ps_run_store ps_run_store;
typedef struct {
    const ps_run_store *owner;
    uint32_t slot, generation;
} ps_run_read_handle;
typedef struct {
    const ps_run_store *owner;
    uint32_t slot, generation;
} ps_run_write_handle;
typedef struct {
    uint32_t channels;
    uint64_t samples;
    bool complete;
    char metadata[8192];
    ps_channel schema[PS_MAX_CHANNELS];
} ps_run_read_info;
typedef struct {
    uint32_t channels;
    uint64_t samples;
} ps_run_write_info;
/* Explicit allocation domain and owned open files; no mutable public internals.
 * Descriptor copied by value; callbacks/user outlive store. Per-store external
 * synchronization required. Handles belong to this live store; do not retain
 * them after destruction. Closing and slot reuse never resurrect an old handle.
 * At most eight readers and eight writers; exhausted generations retire slots.
 * All constructors/open/describe operations preserve outputs on failure. */
ps_result ps_run_store_create(ps_allocator allocator, ps_run_store **out);
/* Closes readers and finalizes live writers. All resources are released even
 * on I/O failure; the first writer-finalization failure is returned. */
ps_result ps_run_store_destroy(ps_run_store *store);
ps_result ps_run_reader_open(ps_run_store *store, const char *path, ps_run_read_handle *out);
ps_result ps_run_reader_describe(ps_run_store *store, ps_run_read_handle handle,
                                 ps_run_read_info *out);
/* capacity must hold the entire schema. PS_OK commits exactly one validated
 * measurement row; every other status preserves time, values and count.
 * EOF/recovered status remains terminal for this handle. */
ps_result ps_run_reader_next(ps_run_store *store, ps_run_read_handle handle, double *time,
                             double *values, size_t capacity, size_t *count);
ps_result ps_run_reader_snapshot_next(ps_run_store *store, ps_run_read_handle handle,
                                      ps_snapshot *out);
ps_result ps_run_reader_release(ps_run_store *store, ps_run_read_handle handle);
ps_result ps_run_writer_create(ps_run_store *store, const char *path, const ps_context *context,
                               const char *name, ps_run_write_handle *out);
ps_result ps_run_writer_describe(ps_run_store *store, ps_run_write_handle handle,
                                 ps_run_write_info *out);
/* Exactly the declared channel count, finite row. Existing run-format contracts
 * apply; failed file writes can leave an incomplete recoverable tail. */
ps_result ps_run_writer_append(ps_run_store *store, ps_run_write_handle handle, double time,
                               const double *values, size_t count);
ps_result ps_run_writer_snapshot(ps_run_store *store, ps_run_write_handle handle,
                                 const ps_context *context, const ps_scene *scene, bool paused);
/* Closes without a footer, preserving an incomplete recoverable prefix.
 * Invalidates the handle even on fclose failure. */
ps_result ps_run_writer_abort(ps_run_store *store, ps_run_write_handle handle);
/* Finalizes and invalidates the handle even when finalization returns an error. */
ps_result ps_run_writer_release(ps_run_store *store, ps_run_write_handle handle);
#endif
