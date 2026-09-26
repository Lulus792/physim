#include "physim/memory.h"
#include <stdlib.h>
#include <string.h>

static void *default_allocate(void *user, size_t bytes) {
    (void)user;
    return malloc(bytes);
}
static void default_deallocate(void *user, void *pointer, size_t bytes) {
    (void)user;
    (void)bytes;
    free(pointer);
}
ps_allocator ps_allocator_default(void) {
    return (ps_allocator){NULL, default_allocate, default_deallocate};
}
bool ps_allocator_valid(ps_allocator a) { return a.allocate && a.deallocate; }
ps_result ps_memory_allocate(ps_allocator a, size_t bytes, void **out) {
    if (!ps_allocator_valid(a) || !out)
        return PS_INVALID;
    void *p = bytes ? a.allocate(a.user, bytes) : NULL;
    if (bytes && !p)
        return PS_MEMORY;
    *out = p;
    return PS_OK;
}
ps_result ps_memory_zero(ps_allocator a, size_t count, size_t size, void **out) {
    if (!ps_allocator_valid(a) || !out)
        return PS_INVALID;
    if (size && count > SIZE_MAX / size)
        return PS_LIMIT;
    void *p;
    size_t bytes = count * size;
    ps_result r = ps_memory_allocate(a, bytes, &p);
    if (r != PS_OK)
        return r;
    if (bytes)
        memset(p, 0, bytes);
    *out = p;
    return PS_OK;
}
void ps_memory_free(ps_allocator a, void *p, size_t bytes) {
    if (p && ps_allocator_valid(a))
        a.deallocate(a.user, p, bytes);
}
ps_result ps_memory_resize(ps_allocator a, void *old, size_t old_bytes, size_t new_bytes,
                           void **out) {
    if (!ps_allocator_valid(a) || !out || (!old != !old_bytes))
        return PS_INVALID;
    if (old_bytes == new_bytes) {
        *out = old;
        return PS_OK;
    }
    void *p;
    ps_result r = ps_memory_allocate(a, new_bytes, &p);
    if (r != PS_OK)
        return r;
    if (old && new_bytes)
        memcpy(p, old, old_bytes < new_bytes ? old_bytes : new_bytes);
    ps_memory_free(a, old, old_bytes);
    *out = p;
    return PS_OK;
}
ps_result ps_arena_init(ps_arena *a, void *buffer, size_t capacity) {
    if (!a || (!buffer && capacity))
        return PS_INVALID;
    *a = (ps_arena){buffer, capacity, 0};
    return PS_OK;
}
static void *arena_allocate(void *user, size_t bytes) {
    ps_arena *a = user;
    if (!a || !a->buffer || a->used > a->capacity || bytes > a->capacity - a->used)
        return NULL;
    size_t alignment = PS_MEMORY_ALIGNMENT;
    size_t remainder = (uintptr_t)(a->buffer + a->used) % alignment;
    size_t padding = remainder ? alignment - remainder : 0;
    if (padding > a->capacity - a->used || bytes > a->capacity - a->used - padding)
        return NULL;
    void *p = a->buffer + a->used + padding;
    a->used += padding + bytes;
    return p;
}
static void arena_deallocate(void *user, void *pointer, size_t bytes) {
    (void)user;
    (void)pointer;
    (void)bytes;
}
ps_allocator ps_arena_allocator(ps_arena *a) {
    return a ? (ps_allocator){a, arena_allocate, arena_deallocate} : (ps_allocator){0};
}
void ps_arena_reset(ps_arena *a) {
    if (a)
        a->used = 0;
}
