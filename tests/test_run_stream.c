#include "physim/run_stream.h"
#include "test_allocator.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "run stream %d: %s\n", __LINE__, #x);                                  \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
int main(int argc, char **argv) {
    CHECK(argc == 2);
    char path[4096], other[4096];
    snprintf(path, sizeof path, "%s/handle run ä.psrun", argv[1]);
    snprintf(other, sizeof other, "%s/other.psrun", argv[1]);
    ps_context context = {0};
    context.struct_size = sizeof context;
    context.channel_count = 1;
    strcpy(context.channels[0].name, "value");
    strcpy(context.channels[0].unit, "m");
    context.channels[0].dimension[0] = 1;
    context.dt_s = .1;
    test_allocator domain = {0};
    ps_run_store *store = NULL, *foreign = NULL;
    domain.fail_on = 1;
    CHECK(ps_run_store_create(test_domain(&domain), &store) == PS_MEMORY && !store &&
          !domain.live_blocks);
    domain.fail_on = 0;
    CHECK(ps_run_store_create(test_domain(&domain), &store) == PS_OK);
    CHECK(ps_run_store_create(ps_allocator_default(), &foreign) == PS_OK);
    ps_run_write_handle writer;
    CHECK(ps_run_writer_create(store, path, &context, "handles", &writer) == PS_OK);
    double value = 42;
    CHECK(ps_run_writer_append(store, writer, 0, &value, 1) == PS_OK);
    ps_run_write_info wi;
    CHECK(ps_run_writer_describe(store, writer, &wi) == PS_OK && wi.samples == 1 &&
          wi.channels == 1);
    CHECK(ps_run_writer_append(foreign, writer, 1, &value, 1) == PS_INVALID);
    CHECK(ps_run_writer_append(store, writer, 1, &value, 0) == PS_INVALID);
    value = NAN;
    CHECK(ps_run_writer_append(store, writer, 1, &value, 1) == PS_INVALID);
    value = 43;
    CHECK(ps_run_writer_append(store, writer, .1, &value, 1) == PS_OK);
    ps_run_write_handle copy = writer;
    CHECK(ps_run_writer_release(store, writer) == PS_OK);
    CHECK(ps_run_writer_release(store, copy) == PS_INVALID);
    ps_run_write_handle sentinel = {foreign, 123, 456};
    writer = sentinel;
    CHECK(ps_run_writer_create(store, path, &context, "exclusive", &writer) == PS_IO &&
          writer.owner == sentinel.owner && writer.slot == 123 && writer.generation == 456);
    CHECK(ps_run_writer_create(store, other, &context, "reused", &writer) == PS_OK &&
          writer.slot == copy.slot && writer.generation != copy.generation);
    CHECK(ps_run_writer_append(store, copy, 0, &value, 1) == PS_INVALID);
    ps_scene scene = {0};
    CHECK(ps_scene_label_id(&scene, 42, ps_v3(1, 2, 3), "Handle snapshot", 0xff00ffff) == PS_OK);
    CHECK(ps_run_writer_snapshot(store, writer, &context, &scene, true) == PS_OK);
    ps_run_read_handle readers[PS_RUN_STREAM_MAX_READERS];
    for (unsigned i = 0; i < PS_RUN_STREAM_MAX_READERS; i++)
        CHECK(ps_run_reader_open(store, path, &readers[i]) == PS_OK);
    ps_run_read_handle rh = {foreign, 123, 456};
    CHECK(ps_run_reader_open(store, path, &rh) == PS_LIMIT && rh.slot == 123 &&
          rh.generation == 456);
    ps_run_read_info info;
    CHECK(ps_run_reader_describe(store, readers[0], &info) == PS_OK && info.channels == 1 &&
          !strcmp(info.schema[0].name, "value"));
    double time = 99, row[PS_MAX_CHANNELS] = {88, 77};
    size_t count = 66;
    CHECK(ps_run_reader_next(store, readers[0], &time, row, 0, &count) == PS_INVALID &&
          time == 99 && row[0] == 88 && count == 66);
    CHECK(ps_run_reader_next(foreign, readers[0], &time, row, 2, &count) == PS_INVALID);
    CHECK(ps_run_reader_next(store, readers[0], &time, row, 2, &count) == PS_OK && time == 0 &&
          row[0] == 42 && row[1] == 77 && count == 1);
    CHECK(ps_run_reader_next(store, readers[0], &time, row, 2, &count) == PS_OK && time == .1 &&
          row[0] == 43);
    time = 99;
    row[0] = 88;
    count = 66;
    CHECK(ps_run_reader_next(store, readers[0], &time, row, 2, &count) == PS_EOF && time == 99 &&
          row[0] == 88 && count == 66);
    CHECK(ps_run_reader_next(store, readers[0], &time, row, 2, &count) == PS_EOF);
    CHECK(ps_run_reader_describe(store, readers[0], &info) == PS_OK && info.complete &&
          info.samples == 2);
    rh = readers[0];
    CHECK(ps_run_reader_release(store, rh) == PS_OK);
    CHECK(ps_run_reader_release(store, rh) == PS_INVALID);
    CHECK(ps_run_reader_open(store, path, &readers[0]) == PS_OK && readers[0].slot == rh.slot &&
          readers[0].generation != rh.generation);
    CHECK(ps_run_reader_next(store, rh, &time, row, 2, &count) == PS_INVALID);
    ps_run_read_handle forged = readers[0];
    forged.generation = 0;
    CHECK(ps_run_reader_describe(store, forged, &info) == PS_INVALID);
    forged = readers[0];
    forged.slot = UINT32_MAX;
    CHECK(ps_run_reader_release(store, forged) == PS_INVALID);
    CHECK(ps_run_store_destroy(store) == PS_OK && !domain.live_blocks && !domain.invalid);
    CHECK(ps_run_store_destroy(foreign) == PS_OK);
    CHECK(ps_run_store_create(ps_allocator_default(), &store) == PS_OK);
    CHECK(ps_run_reader_open(store, other, &rh) == PS_OK);
    ps_snapshot *snapshot = malloc(sizeof *snapshot);
    CHECK(snapshot);
    CHECK(ps_run_reader_snapshot_next(store, rh, snapshot) == PS_OK && snapshot->scene.count == 1 &&
          snapshot->scene.objects[0].id == 42);
    CHECK(ps_run_reader_snapshot_next(store, rh, snapshot) == PS_EOF);
    free(snapshot);
    CHECK(ps_run_store_destroy(store) == PS_OK);
    ps_run_reader legacy;
    CHECK(ps_run_open(&legacy, other) == PS_OK);
    CHECK(ps_run_next(&legacy, &time, row) == PS_EOF);
    ps_run_reader_close(&legacy);
    CHECK(ps_run_store_create(ps_allocator_default(), &store) == PS_OK);
    char incomplete[4096];
    snprintf(incomplete, sizeof incomplete, "%s/aborted.psrun", argv[1]);
    CHECK(ps_run_writer_create(store, incomplete, &context, "aborted", &writer) == PS_OK);
    value = 9;
    CHECK(ps_run_writer_append(store, writer, 0, &value, 1) == PS_OK);
    CHECK(ps_run_writer_abort(store, writer) == PS_OK &&
          ps_run_writer_abort(store, writer) == PS_INVALID);
    CHECK(ps_run_reader_open(store, incomplete, &rh) == PS_OK);
    CHECK(ps_run_reader_next(store, rh, &time, row, 2, &count) == PS_OK && row[0] == 9);
    time = 99;
    row[0] = 88;
    count = 66;
    CHECK(ps_run_reader_next(store, rh, &time, row, 2, &count) == PS_RECOVERED && time == 99 &&
          row[0] == 88 && count == 66);
    CHECK(ps_run_reader_next(store, rh, &time, row, 2, &count) == PS_RECOVERED);
    CHECK(ps_run_store_destroy(store) == PS_OK);
    puts("Run stream: opaque ownership, stale/foreign/forged handles, reuse, bounded slots, "
         "allocator failure, atomic reads, exclusive writes and destroy finalization passed");
    return 0;
}
