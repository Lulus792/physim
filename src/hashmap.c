#include "physim/hashmap.h"
#include <string.h>

typedef struct entry {
    struct entry *next;
    uint64_t hash;
    size_t key_size;
    ps_memory_alignment alignment; /* value immediately after struct is aligned */
} entry;
struct ps_hashmap {
    ps_allocator allocator;
    size_t value_size, maximum_entries, maximum_key_bytes, count, bucket_count;
    entry **buckets;
    bool visiting;
};
static void *value_of(entry *e) { return e + 1; }
static const char *key_of(const ps_hashmap *m, const entry *e) {
    return (const char *)(e + 1) + m->value_size;
}
static uint64_t hash_key(ps_string_view key) {
    uint64_t hash = UINT64_C(14695981039346656037);
    for (size_t i = 0; i < key.size; i++) {
        hash ^= (unsigned char)key.data[i];
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}
static ps_result valid_key(const ps_hashmap *m, ps_string_view key) {
    if (!m || !ps_string_view_valid(key))
        return PS_INVALID;
    return key.size > m->maximum_key_bytes ? PS_LIMIT : PS_OK;
}
static entry **find_link(ps_hashmap *m, ps_string_view key, uint64_t hash) {
    if (!m->bucket_count)
        return NULL;
    entry **link = &m->buckets[hash & (m->bucket_count - 1)];
    while (*link && ((*link)->hash != hash || (*link)->key_size != key.size ||
                     (key.size && memcmp(key_of(m, *link), key.data, key.size))))
        link = &(*link)->next;
    return link;
}
ps_result ps_hashmap_create(ps_allocator allocator, size_t value_size, size_t maximum,
                            size_t key_limit, ps_hashmap **out) {
    if (!out || !ps_allocator_valid(allocator) || !value_size || !maximum)
        return PS_INVALID;
    if (value_size > SIZE_MAX - sizeof(entry) || key_limit > SIZE_MAX - sizeof(entry) - value_size)
        return PS_LIMIT;
    void *storage;
    ps_result r = ps_memory_zero(allocator, 1, sizeof(ps_hashmap), &storage);
    if (r != PS_OK)
        return r;
    ps_hashmap *m = storage;
    m->allocator = allocator;
    m->value_size = value_size;
    m->maximum_entries = maximum;
    m->maximum_key_bytes = key_limit;
    *out = m;
    return PS_OK;
}
ps_result ps_hashmap_reserve(ps_hashmap *m, size_t required) {
    if (!m || m->visiting)
        return PS_INVALID;
    if (required > m->maximum_entries)
        return PS_LIMIT;
    if (required <= m->bucket_count)
        return PS_OK;
    size_t capacity = m->bucket_count ? m->bucket_count : 8;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2)
            return PS_LIMIT;
        capacity *= 2;
    }
    if (capacity > SIZE_MAX / sizeof(entry *))
        return PS_LIMIT;
    void *storage;
    ps_result r = ps_memory_allocate(m->allocator, capacity * sizeof(entry *), &storage);
    if (r != PS_OK)
        return r;
    entry **buckets = storage;
    for (size_t i = 0; i < capacity; i++)
        buckets[i] = NULL;
    for (size_t i = 0; i < m->bucket_count; i++) {
        entry *e = m->buckets[i];
        while (e) {
            entry *next = e->next;
            size_t slot = e->hash & (capacity - 1);
            e->next = buckets[slot];
            buckets[slot] = e;
            e = next;
        }
    }
    ps_memory_free(m->allocator, m->buckets, m->bucket_count * sizeof(entry *));
    m->buckets = buckets;
    m->bucket_count = capacity;
    return PS_OK;
}
ps_result ps_hashmap_set(ps_hashmap *m, ps_string_view key, const void *value) {
    ps_result r = valid_key(m, key);
    if (r != PS_OK)
        return r;
    if (!value || m->visiting)
        return PS_INVALID;
    uint64_t hash = hash_key(key);
    entry **link = find_link(m, key, hash);
    if (link && *link) {
        memmove(value_of(*link), value, m->value_size);
        return PS_OK;
    }
    if (m->count == m->maximum_entries)
        return PS_LIMIT;
    size_t bytes = sizeof(entry) + m->value_size + key.size;
    void *storage;
    r = ps_memory_allocate(m->allocator, bytes, &storage);
    if (r != PS_OK)
        return r;
    entry *e = storage;
    e->hash = hash;
    e->key_size = key.size;
    memcpy(value_of(e), value, m->value_size);
    if (key.size)
        memcpy((char *)value_of(e) + m->value_size, key.data, key.size);
    r = ps_hashmap_reserve(m, m->count + 1);
    if (r != PS_OK) {
        ps_memory_free(m->allocator, e, bytes);
        return r;
    }
    size_t slot = hash & (m->bucket_count - 1);
    e->next = m->buckets[slot];
    m->buckets[slot] = e;
    m->count++;
    return PS_OK;
}
ps_result ps_hashmap_get(const ps_hashmap *m, ps_string_view key, void *out) {
    ps_result r = valid_key(m, key);
    if (r != PS_OK)
        return r;
    if (!out)
        return PS_INVALID;
    if (!m->bucket_count)
        return PS_EOF;
    uint64_t hash = hash_key(key);
    for (entry *e = m->buckets[hash & (m->bucket_count - 1)]; e; e = e->next)
        if (e->hash == hash && e->key_size == key.size &&
            (!key.size || !memcmp(key_of(m, e), key.data, key.size))) {
            memcpy(out, value_of(e), m->value_size);
            return PS_OK;
        }
    return PS_EOF;
}
ps_result ps_hashmap_erase(ps_hashmap *m, ps_string_view key) {
    ps_result r = valid_key(m, key);
    if (r != PS_OK)
        return r;
    if (m->visiting)
        return PS_INVALID;
    entry **link = find_link(m, key, hash_key(key));
    if (!link || !*link)
        return PS_EOF;
    entry *e = *link;
    *link = e->next;
    ps_memory_free(m->allocator, e, sizeof *e + m->value_size + e->key_size);
    m->count--;
    return PS_OK;
}
size_t ps_hashmap_count(const ps_hashmap *m) { return m ? m->count : 0; }
ps_result ps_hashmap_clear(ps_hashmap *m) {
    if (!m || m->visiting)
        return PS_INVALID;
    for (size_t i = 0; i < m->bucket_count; i++) {
        entry *e = m->buckets[i];
        while (e) {
            entry *next = e->next;
            ps_memory_free(m->allocator, e, sizeof *e + m->value_size + e->key_size);
            e = next;
        }
        m->buckets[i] = NULL;
    }
    m->count = 0;
    return PS_OK;
}
ps_result ps_hashmap_visit(ps_hashmap *m, ps_hashmap_visit_fn visit, void *user) {
    if (!m || !visit || m->visiting)
        return PS_INVALID;
    m->visiting = true;
    ps_result r = PS_OK;
    for (size_t i = 0; i < m->bucket_count && r == PS_OK; i++)
        for (entry *e = m->buckets[i]; e && r == PS_OK; e = e->next)
            r = visit((ps_string_view){key_of(m, e), e->key_size}, value_of(e), user);
    m->visiting = false;
    return r;
}
void ps_hashmap_destroy(ps_hashmap *m) {
    if (!m || m->visiting)
        return;
    ps_hashmap_clear(m);
    ps_memory_free(m->allocator, m->buckets, m->bucket_count * sizeof(entry *));
    ps_memory_free(m->allocator, m, sizeof *m);
}
