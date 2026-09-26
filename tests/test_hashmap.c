#include "physim/hashmap.h"
#include "test_allocator.h"
#include <stdio.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Hashmap line %d: %s\n", __LINE__, #x);                                \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static ps_string_view key_of(unsigned id, char bytes[2]) {
    bytes[0] = (char)(id & 255);
    bytes[1] = (char)(id >> 8);
    return (ps_string_view){bytes, 2};
}
typedef struct {
    bool present[256], seen[256];
    int values[256];
    size_t count;
    ps_hashmap *map;
} reference;
static ps_result inspect(ps_string_view key, const void *value, void *user) {
    reference *r = user;
    if (key.size != 2 || (uintptr_t)value % PS_MEMORY_ALIGNMENT)
        return PS_CORRUPT;
    unsigned id = (unsigned char)key.data[0] + 256u * (unsigned char)key.data[1];
    if (id >= 256 || !r->present[id] || r->seen[id] || *(const int *)value != r->values[id])
        return PS_CORRUPT;
    r->seen[id] = true;
    r->count++;
    int copy;
    if (ps_hashmap_get(r->map, key, &copy) != PS_OK || copy != r->values[id])
        return PS_CORRUPT;
    if (ps_hashmap_set(r->map, key, &copy) != PS_INVALID ||
        ps_hashmap_erase(r->map, key) != PS_INVALID || ps_hashmap_clear(r->map) != PS_INVALID ||
        ps_hashmap_reserve(r->map, 256) != PS_INVALID ||
        ps_hashmap_visit(r->map, inspect, user) != PS_INVALID)
        return PS_CORRUPT;
    return PS_OK;
}
static int random_operations(void) {
    test_allocator tracker = {0};
    reference r = {0};
    CHECK(ps_hashmap_create(test_domain(&tracker), sizeof(int), 256, 2, &r.map) == PS_OK);
    ps_rng rng;
    ps_rng_seed(&rng, 674324);
    size_t count = 0;
    for (unsigned step = 0; step < 5000; step++) {
        unsigned id = (unsigned)(ps_rng_uniform(&rng) * 256);
        unsigned operation = (unsigned)(ps_rng_uniform(&rng) * 3);
        char bytes[2];
        ps_string_view key = key_of(id, bytes);
        int value = (int)step;
        if (!operation) {
            CHECK(ps_hashmap_set(r.map, key, &value) == PS_OK);
            if (!r.present[id])
                count++;
            r.present[id] = true;
            r.values[id] = value;
            bytes[0] ^= 0x55;
            value = -1; /* map must own both copies */
        } else if (operation == 1) {
            CHECK(ps_hashmap_erase(r.map, key) == (r.present[id] ? PS_OK : PS_EOF));
            if (r.present[id])
                count--;
            r.present[id] = false;
        } else {
            CHECK(ps_hashmap_get(r.map, key, &value) == (r.present[id] ? PS_OK : PS_EOF));
            CHECK(value == (r.present[id] ? r.values[id] : (int)step));
        }
        CHECK(ps_hashmap_count(r.map) == count);
        if (step % 100 == 0) {
            memset(r.seen, 0, sizeof r.seen);
            r.count = 0;
            CHECK(ps_hashmap_visit(r.map, inspect, &r) == PS_OK && r.count == count);
        }
    }
    CHECK(ps_hashmap_clear(r.map) == PS_OK && ps_hashmap_count(r.map) == 0);
    CHECK(tracker.live_blocks == 2); /* map + retained buckets */
    ps_hashmap_destroy(r.map);
    CHECK(!tracker.invalid && !tracker.live_bytes && !tracker.live_blocks);
    return 0;
}
static uint64_t hash_bytes(const unsigned char *key, size_t n) {
    uint64_t h = UINT64_C(14695981039346656037);
    for (size_t i = 0; i < n; i++)
        h = (h ^ key[i]) * UINT64_C(1099511628211);
    return h;
}
static int collisions(void) {
    ps_hashmap *map;
    CHECK(ps_hashmap_create(ps_allocator_default(), sizeof(int), 64, 4, &map) == PS_OK);
    unsigned char keys[64][4];
    unsigned found = 0;
    for (unsigned id = 0; found < 64 && id < 100000; id++) {
        unsigned char key[4] = {(unsigned char)id, (unsigned char)(id >> 8),
                                (unsigned char)(id >> 16), 0};
        if ((hash_bytes(key, 4) & 255) != 0)
            continue;
        memcpy(keys[found], key, 4);
        int value = (int)found;
        CHECK(ps_hashmap_set(map, (ps_string_view){(char *)key, 4}, &value) == PS_OK);
        found++;
    }
    CHECK(found == 64 && ps_hashmap_count(map) == 64);
    /* All 64 entries share the same bucket before and after explicit reserve. */
    CHECK(ps_hashmap_reserve(map, 64) == PS_OK);
    for (unsigned parity = 0; parity < 2; parity++) {
        for (unsigned i = parity; i < 64; i += 2) {
            int value = -1;
            ps_string_view key = {(char *)keys[i], 4};
            CHECK(ps_hashmap_get(map, key, &value) == PS_OK && value == (int)i);
            CHECK(ps_hashmap_erase(map, key) == PS_OK);
            CHECK(ps_hashmap_get(map, key, &value) == PS_EOF && value == (int)i);
        }
    }
    ps_hashmap_destroy(map);
    return 0;
}
static ps_result stop_visit(ps_string_view key, const void *value, void *user) {
    (void)key;
    (void)value;
    (*(unsigned *)user)++;
    return PS_EOF;
}
static int failures(void) {
    /* Constructor, entry, first buckets, rehash and node failures. */
    test_allocator tracker = {0};
    ps_hashmap *map = NULL;
    tracker.fail_on = 1;
    CHECK(ps_hashmap_create(test_domain(&tracker), 4, 16, 8, &map) == PS_MEMORY && map == NULL);
    tracker.fail_on = 0;
    CHECK(ps_hashmap_create(test_domain(&tracker), sizeof(int), 16, 8, &map) == PS_OK);
    size_t base_bytes = tracker.live_bytes;
    int value = 17;
    for (size_t failure = 1; failure <= 2; failure++) {
        tracker.fail_on = tracker.attempts + failure;
        CHECK(ps_hashmap_set(map, (ps_string_view){0}, &value) == PS_MEMORY);
        CHECK(ps_hashmap_count(map) == 0 && tracker.live_bytes == base_bytes);
    }
    tracker.fail_on = 0;
    CHECK(ps_hashmap_set(map, (ps_string_view){0}, &value) == PS_OK);
    for (unsigned i = 1; i < 8; i++) {
        char bytes[2];
        CHECK(ps_hashmap_set(map, key_of(i, bytes), &value) == PS_OK);
    }
    size_t live = tracker.live_bytes;
    for (size_t failure = 1; failure <= 2; failure++) {
        char bytes[2];
        tracker.fail_on = tracker.attempts + failure;
        CHECK(ps_hashmap_set(map, key_of(8, bytes), &value) == PS_MEMORY);
        CHECK(ps_hashmap_count(map) == 8 && tracker.live_bytes == live);
        int actual = 0;
        CHECK(ps_hashmap_get(map, (ps_string_view){0}, &actual) == PS_OK && actual == 17);
    }
    tracker.fail_on = 0;
    unsigned visits = 0;
    CHECK(ps_hashmap_visit(map, stop_visit, &visits) == PS_EOF && visits == 1);
    CHECK(ps_hashmap_clear(map) == PS_OK); /* traversal guard reset on early exit */
    size_t attempts = tracker.attempts;
    CHECK(ps_hashmap_set(map, (ps_string_view){NULL, 1}, &value) == PS_INVALID);
    CHECK(ps_hashmap_set(map, (ps_string_view){"long key!", 9}, &value) == PS_LIMIT);
    CHECK(ps_hashmap_reserve(map, SIZE_MAX) == PS_LIMIT && tracker.attempts == attempts);
    ps_hashmap_destroy(map);
    CHECK(!tracker.invalid && tracker.live_bytes == 0);
    CHECK(ps_hashmap_create(test_domain(&tracker), SIZE_MAX, 1, 0, &map) == PS_LIMIT);
    CHECK(ps_hashmap_create(test_domain(&tracker), 1, 1, SIZE_MAX, &map) == PS_LIMIT);
    CHECK(ps_hashmap_create(test_domain(&tracker), sizeof(int), 1, 0, &map) == PS_OK);
    CHECK(ps_hashmap_set(map, (ps_string_view){0}, &value) == PS_OK);
    tracker.fail_on = tracker.attempts + 1;
    value = 99;
    CHECK(ps_hashmap_set(map, (ps_string_view){0}, &value) == PS_OK);
    int actual = 0;
    CHECK(ps_hashmap_get(map, (ps_string_view){0}, &actual) == PS_OK && actual == 99);
    ps_hashmap_destroy(map);
    CHECK(!tracker.invalid && !tracker.live_bytes);
    tracker.fail_on = 0;
    CHECK(ps_hashmap_create(test_domain(&tracker), sizeof(int), 1, 1, &map) == PS_OK);
    CHECK(ps_hashmap_set(map, (ps_string_view){"a", 1}, &value) == PS_OK);
    attempts = tracker.attempts;
    CHECK(ps_hashmap_set(map, (ps_string_view){"b", 1}, &value) == PS_LIMIT);
    CHECK(tracker.attempts == attempts && ps_hashmap_count(map) == 1);
    CHECK(ps_hashmap_set(map, (ps_string_view){"a", 1}, &value) == PS_OK &&
          tracker.attempts == attempts);
    CHECK(ps_hashmap_erase(map, (ps_string_view){"a", 1}) == PS_OK);
    CHECK(ps_hashmap_set(map, (ps_string_view){"b", 1}, &value) == PS_OK);
    ps_hashmap_destroy(map);
    CHECK(ps_hashmap_create(test_domain(&tracker), 1, SIZE_MAX, 0, &map) == PS_OK);
    attempts = tracker.attempts;
    CHECK(ps_hashmap_reserve(map, SIZE_MAX) == PS_LIMIT && tracker.attempts == attempts);
    ps_hashmap_destroy(map);
    CHECK(!tracker.invalid && !tracker.live_bytes);
    ps_hashmap_destroy(NULL);
    return 0;
}
int main(void) {
    CHECK(random_operations() == 0);
    CHECK(collisions() == 0);
    CHECK(failures() == 0);
    puts("Hashmap: 5000 model operations, 64 chained collisions, ownership, visitor guards and "
         "allocation failures passed");
    return 0;
}
