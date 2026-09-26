#include "physim/array.h"
#include "test_allocator.h"
#include <stdio.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Array line %d: %s\n", __LINE__, #x);                                  \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static int operations(void) {
    test_allocator tracker = {0};
    ps_array a;
    CHECK(ps_array_init(test_domain(&tracker), sizeof(int), 128, &a) == PS_OK);
    CHECK(a.data == NULL && a.count == 0 && tracker.attempts == 0);
    int reference[128] = {0};
    size_t used = 0;
    ps_rng rng;
    ps_rng_seed(&rng, 8734);
    for (unsigned step = 0; step < 5000; step++) {
        unsigned operation = (unsigned)(ps_rng_uniform(&rng) * 5);
        size_t index = (size_t)(ps_rng_uniform(&rng) * (used + 1));
        if (operation == 0 && used < 128) {
            int value = (int)step;
            CHECK(ps_array_insert(&a, index, &value, 1) == PS_OK);
            memmove(reference + index + 1, reference + index, (used - index) * sizeof(int));
            reference[index] = value;
            used++;
        } else if (operation == 1 && index < used) {
            size_t count = 1 + (size_t)(ps_rng_uniform(&rng) * (used - index));
            CHECK(ps_array_erase(&a, index, count) == PS_OK);
            memmove(reference + index, reference + index + count,
                    (used - index - count) * sizeof(int));
            used -= count;
        } else if (operation == 2) {
            size_t count = (size_t)(ps_rng_uniform(&rng) * 129);
            CHECK(ps_array_resize(&a, count) == PS_OK);
            if (count > used)
                memset(reference + used, 0, (count - used) * sizeof(int));
            used = count;
        } else if (operation == 3) {
            CHECK(ps_array_shrink(&a) == PS_OK && a.capacity == used);
        } else if (used && used < 128) {
            size_t start = (size_t)(ps_rng_uniform(&rng) * used);
            size_t count = used - start;
            if (count > 128 - used)
                count = 128 - used;
            int snapshot[128];
            memcpy(snapshot, reference + start, count * sizeof(int));
            CHECK(ps_array_insert(&a, index, (int *)a.data + start, count) == PS_OK);
            memmove(reference + index + count, reference + index, (used - index) * sizeof(int));
            memcpy(reference + index, snapshot, count * sizeof(int));
            used += count;
        }
        CHECK(a.count == used && a.capacity >= used && a.capacity <= 128);
        CHECK(!used || !memcmp(a.data, reference, used * sizeof(int)));
        CHECK(!a.data || (uintptr_t)a.data % PS_MEMORY_ALIGNMENT == 0);
        CHECK(tracker.live_bytes == a.capacity * sizeof(int) && !tracker.invalid);
    }
    ps_array_clear(&a);
    CHECK(a.count == 0 && ps_array_shrink(&a) == PS_OK && a.data == NULL);
    CHECK(ps_array_append(&a, NULL, 0) == PS_OK);
    ps_array_destroy(&a);
    ps_array_destroy(&a);
    CHECK(!tracker.live_bytes && !tracker.live_blocks && !tracker.invalid);
    return 0;
}
static int failures(void) {
    test_allocator tracker = {0};
    ps_array a = {0}, saved = a;
    CHECK(ps_array_init(test_domain(&tracker), 2, SIZE_MAX, &a) == PS_LIMIT &&
          !memcmp(&a, &saved, sizeof a));
    CHECK(ps_array_init(test_domain(&tracker), 0, 1, &a) == PS_INVALID);
    CHECK(ps_array_init((ps_allocator){0}, 1, 1, &a) == PS_INVALID);
    CHECK(ps_array_init(test_domain(&tracker), sizeof(int), 10, &a) == PS_OK);
    int input[] = {1, 2, 3, 4};
    tracker.fail_on = tracker.attempts + 1;
    saved = a;
    CHECK(ps_array_append(&a, input, 4) == PS_MEMORY && !memcmp(&a, &saved, sizeof a));
    tracker.fail_on = 0;
    CHECK(ps_array_append(&a, input, 4) == PS_OK && a.capacity == 8);
    saved = a;
    /* In-place overlapping insertion needs a snapshot; failure is transactional. */
    tracker.fail_on = tracker.attempts + 1;
    CHECK(ps_array_insert(&a, 1, a.data, 3) == PS_MEMORY && !memcmp(&a, &saved, sizeof a));
    CHECK(!memcmp(a.data, input, sizeof input));
    CHECK(ps_array_shrink(&a) == PS_OK); /* next attempt after the injected failure */
    saved = a;
    tracker.fail_on = tracker.attempts + 1;
    CHECK(ps_array_append(&a, a.data, 4) == PS_MEMORY && !memcmp(&a, &saved, sizeof a));
    CHECK(!memcmp(a.data, input, sizeof input));
    tracker.fail_on = tracker.attempts + 1;
    CHECK(ps_array_resize(&a, 9) == PS_MEMORY && !memcmp(&a, &saved, sizeof a));
    tracker.fail_on = 0;
    CHECK(ps_array_reserve(&a, 9) == PS_OK && a.capacity == 10);
    saved = a;
    tracker.fail_on = tracker.attempts + 1;
    CHECK(ps_array_shrink(&a) == PS_MEMORY && !memcmp(&a, &saved, sizeof a));
    size_t attempts = tracker.attempts;
    CHECK(ps_array_insert(&a, 0, (unsigned char *)a.data + 1, 1) == PS_INVALID);
    CHECK(ps_array_insert(&a, 0, (int *)a.data + 3, 2) == PS_INVALID);
    CHECK(ps_array_insert(&a, 5, input, 1) == PS_INVALID);
    CHECK(ps_array_insert(&a, 0, NULL, 1) == PS_INVALID);
    CHECK(ps_array_append(&a, input, SIZE_MAX) == PS_LIMIT);
    CHECK(ps_array_reserve(&a, 11) == PS_LIMIT && ps_array_resize(&a, 11) == PS_LIMIT);
    CHECK(ps_array_erase(&a, 4, 1) == PS_INVALID);
    CHECK(tracker.attempts == attempts && !memcmp(&a, &saved, sizeof a));
    CHECK(!memcmp(a.data, input, sizeof input));
    tracker.fail_on = 0;
    CHECK(ps_array_append(&a, a.data, 4) == PS_OK && tracker.attempts == attempts);
    CHECK(!memcmp(a.data, input, sizeof input) && !memcmp((int *)a.data + 4, input, sizeof input));
    ps_array_destroy(&a);
    CHECK(!tracker.live_bytes && !tracker.invalid);
    CHECK(ps_array_reserve(&a, 1) == PS_INVALID);
    CHECK(ps_array_append(NULL, input, 1) == PS_INVALID);
    ps_array_destroy(NULL);
    CHECK(ps_array_init(test_domain(&tracker), 3, 0, &a) == PS_OK &&
          a.maximum_count == SIZE_MAX / 3);
    CHECK(ps_array_reserve(&a, SIZE_MAX) == PS_LIMIT);
    ps_array_destroy(&a);
    CHECK(ps_array_init(test_domain(&tracker), 1, 0, &a) == PS_OK);
    tracker.fail_on = tracker.attempts + 1;
    CHECK(ps_array_reserve(&a, SIZE_MAX) == PS_MEMORY && a.data == NULL && a.capacity == 0);
    ps_array_destroy(&a);
    return 0;
}
static int arena_and_budget(void) {
    unsigned char buffer[256];
    ps_arena arena;
    CHECK(ps_arena_init(&arena, buffer, sizeof buffer) == PS_OK);
    ps_array a;
    CHECK(ps_array_init(ps_arena_allocator(&arena), sizeof(double), 16, &a) == PS_OK);
    CHECK(ps_array_resize(&a, 16) == PS_OK);
    for (unsigned i = 0; i < 16; i++)
        CHECK(((double *)a.data)[i] == 0);
    ps_array_destroy(&a);
    CHECK(arena.used >= 128); /* arena frees deliberately do not reclaim bytes */
    ps_arena_reset(&arena);
    /* A distinct allocation may begin immediately after array storage. */
    ps_array adjacent;
    CHECK(ps_array_init(ps_arena_allocator(&arena), 1, PS_MEMORY_ALIGNMENT, &a) == PS_OK);
    CHECK(ps_array_reserve(&a, PS_MEMORY_ALIGNMENT) == PS_OK);
    CHECK(ps_array_init(ps_arena_allocator(&arena), 1, 4, &adjacent) == PS_OK);
    const unsigned char text[] = {1, 2, 3, 4};
    CHECK(ps_array_append(&adjacent, text, 4) == PS_OK);
    CHECK((unsigned char *)a.data + a.capacity == adjacent.data);
    CHECK(ps_array_append(&a, adjacent.data, 4) == PS_OK && !memcmp(a.data, text, 4));
    ps_array_destroy(&a);
    ps_array_destroy(&adjacent);
    ps_arena_reset(&arena);
    test_allocator tracker = {0};
    tracker.budget = 16;
    CHECK(ps_array_init(test_domain(&tracker), 1, 100, &a) == PS_OK);
    CHECK(ps_array_resize(&a, 8) == PS_OK);
    void *old = a.data;
    CHECK(ps_array_resize(&a, 9) == PS_MEMORY && a.data == old && a.count == 8);
    CHECK(tracker.live_bytes == 8 && tracker.peak_bytes == 8);
    ps_array_destroy(&a);
    CHECK(!tracker.invalid && !tracker.live_bytes);
    return 0;
}
int main(void) {
    CHECK(operations() == 0);
    CHECK(failures() == 0);
    CHECK(arena_and_budget() == 0);
    puts("Array: 5000 reference operations, self-insertion, allocation failures, limits and arenas "
         "passed");
    return 0;
}
