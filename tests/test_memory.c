#include "test_allocator.h"
#include <stdio.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Memory line %d: %s\n", __LINE__, #x);                                 \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static int helpers(void) {
    test_allocator tracker = {0};
    ps_allocator a = test_domain(&tracker);
    int sentinel;
    void *p = &sentinel;
    CHECK(ps_memory_allocate((ps_allocator){0}, 1, &p) == PS_INVALID && p == &sentinel);
    CHECK(ps_memory_allocate(a, 1, NULL) == PS_INVALID);
    CHECK(ps_memory_zero(a, SIZE_MAX, 2, &p) == PS_LIMIT && p == &sentinel);
    CHECK(tracker.attempts == 0);
    CHECK(ps_memory_allocate(a, 0, &p) == PS_OK && p == NULL);
    CHECK(ps_memory_zero(a, SIZE_MAX, 0, &p) == PS_OK && p == NULL);
    CHECK(tracker.attempts == 0);
    tracker.fail_on = 1;
    p = &sentinel;
    CHECK(ps_memory_allocate(a, 17, &p) == PS_MEMORY && p == &sentinel);
    tracker.fail_on = 0;
    CHECK(ps_memory_zero(a, 7, 13, &p) == PS_OK);
    CHECK((uintptr_t)p % PS_MEMORY_ALIGNMENT == 0);
    for (size_t i = 0; i < 91; i++)
        CHECK(((unsigned char *)p)[i] == 0);
    memset(p, 0x67, 91);
    void *old = p;
    size_t attempts = tracker.attempts;
    CHECK(ps_memory_resize(a, p, 91, 91, &p) == PS_OK && p == old && tracker.attempts == attempts);
    tracker.fail_on = tracker.attempts + 1;
    CHECK(ps_memory_resize(a, p, 91, 200, &p) == PS_MEMORY && p == old);
    CHECK(tracker.live_blocks == 1 && tracker.live_bytes == 91);
    for (size_t i = 0; i < 91; i++)
        CHECK(((unsigned char *)p)[i] == 0x67);
    tracker.fail_on = 0;
    CHECK(ps_memory_resize(a, p, 91, 200, &p) == PS_OK);
    CHECK(tracker.live_blocks == 1 && tracker.live_bytes == 200);
    for (size_t i = 0; i < 91; i++)
        CHECK(((unsigned char *)p)[i] == 0x67);
    for (size_t i = 91; i < 200; i++)
        CHECK(((unsigned char *)p)[i] == 0xa5);
    CHECK(ps_memory_resize(a, p, 200, 31, &p) == PS_OK);
    for (size_t i = 0; i < 31; i++)
        CHECK(((unsigned char *)p)[i] == 0x67);
    CHECK(ps_memory_resize(a, p, 31, 0, &p) == PS_OK && p == NULL);
    CHECK(tracker.live_blocks == 0 && tracker.live_bytes == 0 && !tracker.invalid);
    CHECK(ps_memory_resize(a, NULL, 1, 10, &p) == PS_INVALID);
    CHECK(ps_memory_resize(a, &sentinel, 0, 10, &p) == PS_INVALID);
    CHECK(ps_memory_resize(a, NULL, 0, 10, &p) == PS_OK);
    ps_memory_free(a, p, 10);
    ps_memory_free(a, NULL, 0);
    CHECK(tracker.live_bytes == 0 && !tracker.invalid);
    a = ps_allocator_default();
    CHECK(ps_allocator_valid(a));
    CHECK(ps_memory_allocate(a, sizeof(ps_memory_alignment), &p) == PS_OK);
    CHECK((uintptr_t)p % PS_MEMORY_ALIGNMENT == 0);
    ps_memory_free(a, p, sizeof(ps_memory_alignment));
    return 0;
}
static int arenas(void) {
    unsigned char bytes[1024];
    for (size_t offset = 0; offset < 2 * PS_MEMORY_ALIGNMENT; offset++) {
        memset(bytes, 0x7b, sizeof bytes);
        ps_arena arena;
        CHECK(ps_arena_init(&arena, bytes + offset, 512) == PS_OK);
        ps_allocator a = ps_arena_allocator(&arena);
        void *first = NULL;
        for (size_t size = 1; size < 20; size++) {
            void *p;
            CHECK(ps_memory_allocate(a, size, &p) == PS_OK);
            if (size == 1)
                first = p;
            CHECK((uintptr_t)p % PS_MEMORY_ALIGNMENT == 0);
            CHECK((unsigned char *)p >= bytes + offset &&
                  (unsigned char *)p + size <= bytes + offset + 512);
            memset(p, (int)size, size);
        }
        for (size_t i = 0; i < offset; i++)
            CHECK(bytes[i] == 0x7b);
        for (size_t i = offset + 512; i < sizeof bytes; i++)
            CHECK(bytes[i] == 0x7b);
        size_t used = arena.used;
        ps_memory_free(a, first, 1);
        CHECK(arena.used == used);
        void *unchanged = first;
        CHECK(ps_memory_allocate(a, SIZE_MAX, &unchanged) == PS_MEMORY && unchanged == first);
        CHECK(arena.used == used);
        ps_arena_reset(&arena);
        CHECK(arena.used == 0);
        CHECK(ps_memory_zero(a, 4, 8, &unchanged) == PS_OK && unchanged == first);
        for (int i = 0; i < 32; i++)
            CHECK(((unsigned char *)unchanged)[i] == 0);
        ps_arena before = arena;
        CHECK(ps_arena_init(&arena, NULL, 1) == PS_INVALID);
        CHECK(memcmp(&before, &arena, sizeof arena) == 0);
    }
    _Alignas(PS_MEMORY_ALIGNMENT) unsigned char exact[64];
    ps_arena arena;
    CHECK(ps_arena_init(&arena, exact, sizeof exact) == PS_OK);
    ps_allocator a = ps_arena_allocator(&arena);
    void *p, *old;
    CHECK(ps_memory_allocate(a, sizeof exact, &p) == PS_OK && p == exact && arena.used == 64);
    old = p;
    CHECK(ps_memory_allocate(a, 1, &p) == PS_MEMORY && p == old && arena.used == 64);
    ps_arena_reset(&arena);
    CHECK(ps_memory_allocate(a, 8, &p) == PS_OK);
    memset(p, 0x26, 8);
    CHECK(ps_memory_resize(a, p, 8, 16, &p) == PS_OK);
    CHECK(arena.used >= 24);
    for (int i = 0; i < 8; i++)
        CHECK(((unsigned char *)p)[i] == 0x26);
    CHECK(ps_arena_init(&arena, NULL, 0) == PS_OK);
    old = p;
    CHECK(ps_memory_allocate(a, 1, &p) == PS_MEMORY && p == old);
    CHECK(!ps_allocator_valid(ps_arena_allocator(NULL)));
    return 0;
}
int main(void) {
    CHECK(helpers() == 0);
    CHECK(arenas() == 0);
    puts("Memory: failure preservation, exact frees, overflow, alignment, arena limits and reuse "
         "passed");
    return 0;
}
