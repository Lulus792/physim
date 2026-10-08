#include "physim/run_stream.h"
#include <limits.h>
#include <string.h>
typedef struct {
    ps_run_reader reader;
    uint32_t generation;
    bool live;
    ps_result terminal;
} read_slot;
typedef struct {
    ps_run_writer writer;
    uint32_t generation;
    bool live;
} write_slot;
struct ps_run_store {
    ps_allocator allocator;
    read_slot readers[PS_RUN_STREAM_MAX_READERS];
    write_slot writers[PS_RUN_STREAM_MAX_WRITERS];
};
static read_slot *reader_slot(ps_run_store *store, ps_run_read_handle h) {
    if (!store || h.owner != store || !h.generation || h.slot >= PS_RUN_STREAM_MAX_READERS)
        return NULL;
    read_slot *slot = &store->readers[h.slot];
    return slot->live && slot->generation == h.generation ? slot : NULL;
}
static write_slot *writer_slot(ps_run_store *store, ps_run_write_handle h) {
    if (!store || h.owner != store || !h.generation || h.slot >= PS_RUN_STREAM_MAX_WRITERS)
        return NULL;
    write_slot *slot = &store->writers[h.slot];
    return slot->live && slot->generation == h.generation ? slot : NULL;
}
ps_result ps_run_store_create(ps_allocator allocator, ps_run_store **out) {
    if (!out || !ps_allocator_valid(allocator))
        return PS_INVALID;
    ps_run_store *store;
    ps_result r = ps_memory_zero(allocator, 1, sizeof *store, (void **)&store);
    if (r != PS_OK)
        return r;
    store->allocator = allocator;
    *out = store;
    return PS_OK;
}
ps_result ps_run_store_destroy(ps_run_store *store) {
    if (!store)
        return PS_INVALID;
    ps_result result = PS_OK;
    for (size_t i = 0; i < PS_RUN_STREAM_MAX_READERS; i++)
        if (store->readers[i].live)
            ps_run_reader_close(&store->readers[i].reader);
    for (size_t i = 0; i < PS_RUN_STREAM_MAX_WRITERS; i++)
        if (store->writers[i].live) {
            ps_result r = ps_run_close(&store->writers[i].writer);
            if (result == PS_OK)
                result = r;
        }
    ps_allocator allocator = store->allocator;
    ps_memory_free(allocator, store, sizeof *store);
    return result;
}
ps_result ps_run_reader_open(ps_run_store *store, const char *path, ps_run_read_handle *out) {
    if (!store || !path || !out)
        return PS_INVALID;
    for (uint32_t i = 0; i < PS_RUN_STREAM_MAX_READERS; i++) {
        read_slot *s = &store->readers[i];
        if (s->live || s->generation == UINT32_MAX)
            continue;
        ps_run_reader draft;
        ps_result r = ps_run_open(&draft, path);
        if (r != PS_OK)
            return r;
        s->reader = draft;
        s->generation++;
        s->live = true;
        s->terminal = PS_OK;
        *out = (ps_run_read_handle){store, i, s->generation};
        return PS_OK;
    }
    return PS_LIMIT;
}
ps_result ps_run_reader_describe(ps_run_store *store, ps_run_read_handle h, ps_run_read_info *out) {
    read_slot *s = reader_slot(store, h);
    if (!s || !out)
        return PS_INVALID;
    ps_run_read_info value = {0};
    value.channels = s->reader.channels;
    value.samples = s->reader.samples;
    value.complete = s->reader.complete;
    memcpy(value.metadata, s->reader.metadata, sizeof value.metadata);
    memcpy(value.schema, s->reader.schema, sizeof value.schema);
    *out = value;
    return PS_OK;
}
ps_result ps_run_reader_next(ps_run_store *store, ps_run_read_handle h, double *time,
                             double *values, size_t capacity, size_t *count) {
    read_slot *s = reader_slot(store, h);
    if (!s || !time || !values || !count || capacity < s->reader.channels)
        return PS_INVALID;
    if (s->terminal != PS_OK)
        return s->terminal;
    double t, row[PS_MAX_CHANNELS];
    ps_result r = ps_run_next(&s->reader, &t, row);
    if (r != PS_OK) {
        s->terminal = r;
        return r;
    }
    memcpy(values, row, s->reader.channels * sizeof *values);
    *time = t;
    *count = s->reader.channels;
    return PS_OK;
}
ps_result ps_run_reader_snapshot_next(ps_run_store *store, ps_run_read_handle h, ps_snapshot *out) {
    read_slot *s = reader_slot(store, h);
    if (!s || !out)
        return PS_INVALID;
    if (s->terminal != PS_OK)
        return s->terminal;
    ps_result r = ps_run_snapshot_next(&s->reader, out);
    if (r != PS_OK)
        s->terminal = r;
    return r;
}
ps_result ps_run_reader_release(ps_run_store *store, ps_run_read_handle h) {
    read_slot *s = reader_slot(store, h);
    if (!s)
        return PS_INVALID;
    ps_run_reader_close(&s->reader);
    s->live = false;
    return PS_OK;
}
ps_result ps_run_writer_create(ps_run_store *store, const char *path, const ps_context *context,
                               const char *name, ps_run_write_handle *out) {
    if (!store || !path || !context || !name || !out)
        return PS_INVALID;
    for (uint32_t i = 0; i < PS_RUN_STREAM_MAX_WRITERS; i++) {
        write_slot *s = &store->writers[i];
        if (s->live || s->generation == UINT32_MAX)
            continue;
        ps_run_writer draft;
        ps_result r = ps_run_create(&draft, path, context, name);
        if (r != PS_OK)
            return r;
        s->writer = draft;
        s->generation++;
        s->live = true;
        *out = (ps_run_write_handle){store, i, s->generation};
        return PS_OK;
    }
    return PS_LIMIT;
}
ps_result ps_run_writer_describe(ps_run_store *store, ps_run_write_handle h,
                                 ps_run_write_info *out) {
    write_slot *s = writer_slot(store, h);
    if (!s || !out)
        return PS_INVALID;
    *out = (ps_run_write_info){s->writer.channels, s->writer.samples};
    return PS_OK;
}
ps_result ps_run_writer_append(ps_run_store *store, ps_run_write_handle h, double time,
                               const double *values, size_t count) {
    write_slot *s = writer_slot(store, h);
    if (!s || count != s->writer.channels)
        return PS_INVALID;
    return ps_run_append(&s->writer, time, values);
}
ps_result ps_run_writer_snapshot(ps_run_store *store, ps_run_write_handle h,
                                 const ps_context *context, const ps_scene *scene, bool paused) {
    write_slot *s = writer_slot(store, h);
    if (!s)
        return PS_INVALID;
    return ps_run_append_snapshot(&s->writer, context, scene, paused);
}
ps_result ps_run_writer_abort(ps_run_store *store, ps_run_write_handle h) {
    write_slot *s = writer_slot(store, h);
    if (!s)
        return PS_INVALID;
    int error = fclose(s->writer.file);
    s->writer.file = NULL;
    s->live = false;
    return error ? PS_IO : PS_OK;
}
ps_result ps_run_writer_release(ps_run_store *store, ps_run_write_handle h) {
    write_slot *s = writer_slot(store, h);
    if (!s)
        return PS_INVALID;
    ps_result r = ps_run_close(&s->writer);
    s->live = false;
    return r;
}
