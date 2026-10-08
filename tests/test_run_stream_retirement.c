/* Force the shipping counters to their retirement edge, without public test hooks. */
#include "../src/run_stream.c"
#include <stdio.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "stream retirement %d: %s\n", __LINE__, #x); return 1; } } while (0)
int main(int argc, char **argv) {
    CHECK(argc == 2);
    ps_run_store *store;
    CHECK(ps_run_store_create(ps_allocator_default(), &store) == PS_OK);
    ps_context c = {0};
    c.struct_size = sizeof c;
    c.channel_count = 1;
    strcpy(c.channels[0].name, "value");
    c.dt_s = .1;
    char path[4096];
    snprintf(path, sizeof path, "%s/last-generation.psrun", argv[1]);
    for (unsigned i = 0; i < PS_RUN_STREAM_MAX_WRITERS; i++)
        store->writers[i].generation = UINT32_MAX;
    store->writers[0].generation = UINT32_MAX - 1;
    ps_run_write_handle w;
    CHECK(ps_run_writer_create(store, path, &c, "retirement", &w) == PS_OK &&
          w.generation == UINT32_MAX);
    CHECK(ps_run_writer_release(store, w) == PS_OK);
    w = (ps_run_write_handle){store, 99, 99};
    CHECK(ps_run_writer_create(store, path, &c, "retired", &w) == PS_LIMIT &&
          w.slot == 99 && w.generation == 99);
    for (unsigned i = 0; i < PS_RUN_STREAM_MAX_READERS; i++)
        store->readers[i].generation = UINT32_MAX;
    store->readers[0].generation = UINT32_MAX - 1;
    ps_run_read_handle r;
    CHECK(ps_run_reader_open(store, path, &r) == PS_OK && r.generation == UINT32_MAX);
    CHECK(ps_run_reader_release(store, r) == PS_OK);
    r = (ps_run_read_handle){store, 99, 99};
    CHECK(ps_run_reader_open(store, path, &r) == PS_LIMIT && r.slot == 99 && r.generation == 99);
    CHECK(ps_run_store_destroy(store) == PS_OK);
    puts("Run stream retirement: last generation valid once, retired slots never wrap or reuse passed");
    return 0;
}
