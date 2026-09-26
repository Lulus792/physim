#include "physim/array.h"
#include <string.h>

static bool valid(const ps_array *a) {
    return a && ps_allocator_valid(a->allocator) && a->element_size && a->maximum_count &&
           a->maximum_count <= SIZE_MAX / a->element_size && a->count <= a->capacity &&
           a->capacity <= a->maximum_count && (!a->data == !a->capacity);
}
ps_result ps_array_init(ps_allocator allocator, size_t size, size_t maximum, ps_array *out) {
    if (!out || !ps_allocator_valid(allocator) || !size)
        return PS_INVALID;
    if (maximum > SIZE_MAX / size)
        return PS_LIMIT;
    *out = (ps_array){NULL, 0, 0, size, maximum ? maximum : SIZE_MAX / size, allocator};
    return PS_OK;
}
static size_t grown_capacity(const ps_array *a, size_t required) {
    size_t capacity = a->capacity ? a->capacity : (a->maximum_count < 8 ? a->maximum_count : 8);
    while (capacity < required) {
        if (capacity > a->maximum_count / 2)
            return a->maximum_count;
        capacity *= 2;
    }
    return capacity;
}
static ps_result replace_capacity(ps_array *a, size_t capacity) {
    void *data;
    ps_result r = ps_memory_allocate(a->allocator, capacity * a->element_size, &data);
    if (r != PS_OK)
        return r;
    if (a->count)
        memcpy(data, a->data, a->count * a->element_size);
    ps_memory_free(a->allocator, a->data, a->capacity * a->element_size);
    a->data = data;
    a->capacity = capacity;
    return PS_OK;
}
ps_result ps_array_reserve(ps_array *a, size_t minimum) {
    if (!valid(a))
        return PS_INVALID;
    if (minimum > a->maximum_count)
        return PS_LIMIT;
    return minimum <= a->capacity ? PS_OK : replace_capacity(a, grown_capacity(a, minimum));
}
ps_result ps_array_resize(ps_array *a, size_t count) {
    ps_result r = ps_array_reserve(a, count);
    if (r != PS_OK)
        return r;
    if (count > a->count)
        memset((unsigned char *)a->data + a->count * a->element_size, 0,
               (count - a->count) * a->element_size);
    a->count = count;
    return PS_OK;
}
ps_result ps_array_insert(ps_array *a, size_t index, const void *elements, size_t count) {
    if (!valid(a) || index > a->count || (!elements && count))
        return PS_INVALID;
    if (count > a->maximum_count - a->count)
        return PS_LIMIT;
    if (!count)
        return PS_OK;
    size_t bytes = count * a->element_size, prefix = index * a->element_size;
    size_t live = a->count * a->element_size, capacity_bytes = a->capacity * a->element_size;
    bool internal = false;
    if (a->data) {
        uintptr_t source = (uintptr_t)elements, base = (uintptr_t)a->data;
        if (source >= base && source - base < capacity_bytes) {
            size_t offset = (size_t)(source - base);
            if (offset % a->element_size || offset > live || bytes > live - offset)
                return PS_INVALID;
            internal = true;
        } else if (source < base && base - source < bytes)
            return PS_INVALID;
    }
    size_t new_count = a->count + count;
    if (new_count > a->capacity) {
        size_t capacity = grown_capacity(a, new_count);
        void *storage;
        ps_result r = ps_memory_allocate(a->allocator, capacity * a->element_size, &storage);
        if (r != PS_OK)
            return r;
        unsigned char *data = storage;
        if (prefix)
            memcpy(data, a->data, prefix);
        memcpy(data + prefix, elements, bytes);
        if (live > prefix)
            memcpy(data + prefix + bytes, (unsigned char *)a->data + prefix, live - prefix);
        ps_memory_free(a->allocator, a->data, capacity_bytes);
        a->data = data;
        a->capacity = capacity;
    } else {
        void *temporary = NULL;
        if (internal && index < a->count) {
            ps_result r = ps_memory_allocate(a->allocator, bytes, &temporary);
            if (r != PS_OK)
                return r;
            memcpy(temporary, elements, bytes);
            elements = temporary;
        }
        unsigned char *data = a->data;
        if (live > prefix)
            memmove(data + prefix + bytes, data + prefix, live - prefix);
        memcpy(data + prefix, elements, bytes);
        ps_memory_free(a->allocator, temporary, bytes);
    }
    a->count = new_count;
    return PS_OK;
}
ps_result ps_array_append(ps_array *a, const void *elements, size_t count) {
    return a ? ps_array_insert(a, a->count, elements, count) : PS_INVALID;
}
ps_result ps_array_erase(ps_array *a, size_t index, size_t count) {
    if (!valid(a) || index > a->count || count > a->count - index)
        return PS_INVALID;
    if (count && index + count < a->count)
        memmove((unsigned char *)a->data + index * a->element_size,
                (unsigned char *)a->data + (index + count) * a->element_size,
                (a->count - index - count) * a->element_size);
    a->count -= count;
    return PS_OK;
}
void ps_array_clear(ps_array *a) {
    if (valid(a))
        a->count = 0;
}
ps_result ps_array_shrink(ps_array *a) {
    if (!valid(a))
        return PS_INVALID;
    return a->capacity == a->count ? PS_OK : replace_capacity(a, a->count);
}
void ps_array_destroy(ps_array *a) {
    if (!a)
        return;
    if (valid(a))
        ps_memory_free(a->allocator, a->data, a->capacity * a->element_size);
    memset(a, 0, sizeof *a);
}
