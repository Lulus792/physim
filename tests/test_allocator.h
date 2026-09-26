#ifndef PHYSIM_TEST_ALLOCATOR_H
#define PHYSIM_TEST_ALLOCATOR_H
#include "physim/memory.h"
#include <stdlib.h>
#include <string.h>

/* Per-test failure injection, byte budget and exact allocation-domain tracking.
 * Poison fresh storage so callers cannot accidentally depend on zeroed malloc. */
typedef struct {
    struct {
        void *pointer;
        size_t bytes;
    } blocks[256];
    size_t attempts, fail_on, budget, live_bytes, peak_bytes, live_blocks;
    bool invalid;
} test_allocator;
static void *test_allocate(void *user, size_t bytes) {
    test_allocator *a = user;
    a->attempts++;
    if (!bytes) {
        a->invalid = true;
        return NULL;
    }
    if (a->attempts == a->fail_on || bytes > SIZE_MAX - a->live_bytes ||
        (a->budget && (a->live_bytes > a->budget || bytes > a->budget - a->live_bytes)))
        return NULL;
    size_t slot = 0;
    while (slot < 256 && a->blocks[slot].pointer)
        slot++;
    if (slot == 256) {
        a->invalid = true;
        return NULL;
    }
    void *p = malloc(bytes);
    if (!p)
        return NULL;
    memset(p, 0xa5, bytes);
    a->blocks[slot].pointer = p;
    a->blocks[slot].bytes = bytes;
    a->live_bytes += bytes;
    if (a->peak_bytes < a->live_bytes)
        a->peak_bytes = a->live_bytes;
    a->live_blocks++;
    return p;
}
static void test_deallocate(void *user, void *pointer, size_t bytes) {
    test_allocator *a = user;
    size_t slot = 0;
    while (slot < 256 && a->blocks[slot].pointer != pointer)
        slot++;
    if (!pointer || !bytes || slot == 256 || a->blocks[slot].bytes != bytes) {
        a->invalid = true;
        return;
    }
    a->live_bytes -= bytes;
    a->live_blocks--;
    a->blocks[slot].pointer = NULL;
    memset(pointer, 0xdd, bytes);
    free(pointer);
}
static ps_allocator test_domain(test_allocator *a) {
    return (ps_allocator){a, test_allocate, test_deallocate};
}
#endif
