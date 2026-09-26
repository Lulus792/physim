#ifndef PHYSIM_LANGUAGE_ARRAY_H
#define PHYSIM_LANGUAGE_ARRAY_H
/* Internal ownership substrate used by generated C17, not a stable SDK ABI. */
#include "memory.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

/* Per-program/per-module peak budget, including block headers and snapshots. */
typedef struct psrt_memory {
    size_t live_bytes;
    struct psrt_string_pin *string_pins;
} psrt_memory;
#define PSRT_MEMORY_LIMIT (64u * 1024u * 1024u)
static inline void *psrt_memory_allocate(void *context, size_t bytes) {
    psrt_memory *memory = context;
    if (bytes > PSRT_MEMORY_LIMIT - memory->live_bytes)
        return NULL;
    void *result = malloc(bytes);
    if (result)
        memory->live_bytes += bytes;
    return result;
}
static inline void psrt_memory_deallocate(void *context, void *pointer, size_t bytes) {
    psrt_memory *memory = context;
    memory->live_bytes -= bytes;
    free(pointer);
}
static inline ps_allocator psrt_memory_allocator(psrt_memory *memory) {
    return (ps_allocator){memory, psrt_memory_allocate, psrt_memory_deallocate};
}

/* Immutable descriptors and allocator contexts outlive all their values.
 * copy constructs an uninitialized destination, leaving no resources there on
 * error; it must not change the source. destroy cannot fail. Both callbacks are
 * non-reentrant with respect to this array, and must not throw or longjmp.
 * NULL callbacks mean byte-copyable, non-owning elements. Generated descriptors
 * must pair callbacks and use fundamental alignment and sizeof(T) as stride. */
typedef struct psrt_element_type {
    size_t size;
    ps_result (*copy)(void *destination, const void *source);
    void (*destroy)(void *value);
} psrt_element_type;

typedef union psrt_array_block psrt_array_block;
union psrt_array_block {
    ps_memory_alignment alignment;
    struct {
        size_t references, count, bytes;
        ps_allocator allocator;
        const psrt_element_type *type;
        psrt_array_block *next_dead;
    } value;
};
typedef struct psrt_array {
    psrt_array_block *block;
    const psrt_element_type *type;
    ps_allocator allocator;
    size_t maximum_count;
} psrt_array;

/* Treat handles as owning: initialize/clone only into unowned storage; replace
 * mutates a live handle. No raw struct copies, writable data pointers, or shared
 * access across threads. Empty arrays allocate nothing but retain their type. */
static inline ps_result psrt_array_init(const psrt_element_type *type, ps_allocator allocator,
                                        size_t maximum_count, psrt_array *out) {
    if (!out || !type || !type->size || !ps_allocator_valid(allocator) ||
        ((type->copy == NULL) != (type->destroy == NULL)))
        return PS_INVALID;
    size_t limit = (SIZE_MAX - sizeof(psrt_array_block)) / type->size;
    if (limit > INT64_MAX)
        limit = INT64_MAX;
    if (!limit || maximum_count > limit)
        return PS_LIMIT;
    *out = (psrt_array){NULL, type, allocator, maximum_count ? maximum_count : limit};
    return PS_OK;
}
static inline size_t psrt_array_count(const psrt_array *array) {
    return array->block ? array->block->value.count : 0;
}
static inline const void *psrt_array_data(const psrt_array *array) {
    return array->block ? (const void *)(array->block + 1) : NULL;
}
static inline ps_result psrt_array_clone(const psrt_array *source, psrt_array *out) {
    if (!source || !source->type || !out || source == out)
        return PS_INVALID;
    if (source->block) {
        if (source->block->value.references == SIZE_MAX)
            return PS_LIMIT;
        source->block->value.references++;
    }
    *out = *source;
    return PS_OK;
}
#ifdef _MSC_VER
static __declspec(thread) psrt_array_block *psrt_array_dead;
static __declspec(thread) int psrt_array_releasing;
#else
static _Thread_local psrt_array_block *psrt_array_dead;
static _Thread_local int psrt_array_releasing;
#endif
static inline void psrt_array_block_release(psrt_array_block *block) {
    if (!block || --block->value.references)
        return;
    block->value.next_dead = psrt_array_dead;
    psrt_array_dead = block;
    if (psrt_array_releasing)
        return;
    psrt_array_releasing = 1;
    while (psrt_array_dead) {
        block = psrt_array_dead;
        psrt_array_dead = block->value.next_dead;
        if (block->value.type->destroy) {
            unsigned char *data = (unsigned char *)(block + 1);
            for (size_t i = block->value.count; i; i--)
                block->value.type->destroy(data + (i - 1) * block->value.type->size);
        }
        ps_memory_free(block->value.allocator, block, block->value.bytes);
    }
    psrt_array_releasing = 0;
}
static inline void psrt_array_destroy(void *object) {
    psrt_array *array = object;
    if (array) {
        psrt_array_block_release(array->block);
        memset(array, 0, sizeof *array);
    }
}
/* Signed language indexing, no cast of a negative index and no out-of-bounds
 * pointer computation. The borrowed pointer lives until this handle is changed
 * or destroyed; clone the element to extend its lifetime. */
static inline ps_result psrt_array_at(const psrt_array *array, int64_t index, const void **out) {
    if (!array || !array->type || !out)
        return PS_INVALID;
    if (index < 0 || (uint64_t)index >= psrt_array_count(array))
        return PS_LIMIT;
    *out = (const unsigned char *)psrt_array_data(array) + (size_t)index * array->type->size;
    return PS_OK;
}
static inline ps_result psrt_array_copy_one(psrt_array_block *block, const void *source) {
    const psrt_element_type *type = block->value.type;
    void *destination = (unsigned char *)(block + 1) + block->value.count * type->size;
    ps_result result = PS_OK;
    if (type->copy)
        result = type->copy(destination, source);
    else
        memcpy(destination, source, type->size);
    if (result == PS_OK)
        block->value.count++;
    return result;
}
/* Private literal builder: array is initialized and empty, count is the final
 * element count. It becomes readable only after exactly count copy_one calls.
 * Register its owner before starting: destruction rolls back a partial prefix. */
static inline ps_result psrt_array_build_begin(psrt_array *array, size_t count) {
    if (!array || !array->type || array->block)
        return PS_INVALID;
    if (count > array->maximum_count)
        return PS_LIMIT;
    if (!count)
        return PS_OK;
    size_t bytes = sizeof(psrt_array_block) + count * array->type->size;
    void *allocation = NULL;
    ps_result result = ps_memory_allocate(array->allocator, bytes, &allocation);
    if (result != PS_OK)
        return result;
    array->block = allocation;
    array->block->value.references = 1;
    array->block->value.count = 0;
    array->block->value.bytes = bytes;
    array->block->value.allocator = array->allocator;
    array->block->value.type = array->type;
    return PS_OK;
}
/* Append to a private, unfinished result array. Unlike replace, this may
 * modify the current block in place and must never be used on a published
 * snapshot. On copy failure it rolls back this append; on growth failure it
 * leaves the old block unchanged. Capacity grows geometrically so repeated
 * builder appends copy a linear number of already-built elements. */
static inline ps_result psrt_array_builder_append(psrt_array *array, const void *elements,
                                                  size_t count) {
    if (!array || !array->type || (count && !elements))
        return PS_INVALID;
    size_t old_count = psrt_array_count(array);
    if (count > array->maximum_count - old_count)
        return PS_LIMIT;
    if (array->block && array->block->value.references != 1)
        return PS_INVALID;
    if (!count)
        return PS_OK;
    size_t needed = old_count + count;
    size_t capacity = array->block
        ? (array->block->value.bytes - sizeof(psrt_array_block)) / array->type->size : 0;
    if (needed > capacity) {
        size_t target = capacity ? capacity : 4;
        if (target > array->maximum_count)
            target = array->maximum_count;
        while (target < needed) {
            if (target > array->maximum_count / 2) {
                target = array->maximum_count;
                break;
            }
            target *= 2;
        }
        psrt_array grown = *array;
        grown.block = NULL;
        ps_result status = psrt_array_build_begin(&grown, target);
        if (status == PS_MEMORY && target > needed)
            status = psrt_array_build_begin(&grown, needed);
        if (status != PS_OK)
            return status;
        const unsigned char *prior = psrt_array_data(array);
        const unsigned char *added = elements;
        for (size_t i = 0; i < needed; i++) {
            const void *source = i < old_count ? prior + i * array->type->size
                                               : added + (i - old_count) * array->type->size;
            status = psrt_array_copy_one(grown.block, source);
            if (status != PS_OK) {
                psrt_array_destroy(&grown);
                return status;
            }
        }
        psrt_array_block *previous = array->block;
        array->block = grown.block;
        psrt_array_block_release(previous);
        return PS_OK;
    }
    const unsigned char *added = elements;
    ps_result status = PS_OK;
    for (size_t i = 0; i < count; i++) {
        status = psrt_array_copy_one(array->block, added + i * array->type->size);
        if (status != PS_OK)
            break;
    }
    if (status != PS_OK) {
        if (array->type->destroy) {
            unsigned char *data = (unsigned char *)(array->block + 1);
            for (size_t i = array->block->value.count; i > old_count; i--)
                array->type->destroy(data + (i - 1) * array->type->size);
        }
        array->block->value.count = old_count;
    }
    return status;
}
/* Replace [index,index+removed) by count initialized elements of the same type.
 * insert/append/set/erase use this primitive. Source may alias live array data,
 * including nested values. Every successful edit publishes a new immutable
 * block; copies retain the old snapshot. On ANY error the handle, elements,
 * borrowed pointers and reference counts are unchanged. Peak memory includes
 * both versions; allocator budgets apply to that peak. */
static inline ps_result psrt_array_replace(psrt_array *array, size_t index, size_t removed,
                                           const void *elements, size_t count) {
    if (!array || !array->type || (count && !elements))
        return PS_INVALID;
    size_t old_count = psrt_array_count(array);
    if (index > old_count || removed > old_count - index)
        return PS_LIMIT;
    size_t retained = old_count - removed;
    if (count > array->maximum_count - retained)
        return PS_LIMIT;
    if (!removed && !count)
        return PS_OK;
    size_t new_count = retained + count;
    if (!new_count) {
        psrt_array_block *old = array->block;
        array->block = NULL;
        psrt_array_block_release(old);
        return PS_OK;
    }
    size_t stride = array->type->size;
    size_t bytes = sizeof(psrt_array_block) + new_count * stride;
    void *allocation = NULL;
    ps_result result = ps_memory_allocate(array->allocator, bytes, &allocation);
    if (result != PS_OK)
        return result;
    psrt_array_block *block = allocation;
    block->value.references = 1;
    block->value.count = 0;
    block->value.bytes = bytes;
    block->value.allocator = array->allocator;
    block->value.type = array->type;
    const unsigned char *previous = psrt_array_data(array);
    for (size_t i = 0; i < new_count; i++) {
        const void *source;
        if (i < index)
            source = previous + i * stride;
        else if (i - index < count)
            source = (const unsigned char *)elements + (i - index) * stride;
        else
            source = previous + (i - count + removed) * stride;
        result = psrt_array_copy_one(block, source);
        if (result != PS_OK) {
            psrt_array_block_release(block);
            return result;
        }
    }
    psrt_array_block *old = array->block;
    array->block = block;
    psrt_array_block_release(old);
    return PS_OK;
}
static inline ps_result psrt_array_insert(psrt_array *array, int64_t index,
                                          const void *element) {
    if (!array || !array->type || !element)
        return PS_INVALID;
    if (index < 0 || (uint64_t)index > psrt_array_count(array))
        return PS_LIMIT;
    return psrt_array_replace(array, (size_t)index, 0, element, 1);
}
static inline ps_result psrt_array_insert_contents(psrt_array *array, int64_t index,
                                                   const psrt_array *contents) {
    if (!array || !array->type || !contents || !contents->type)
        return PS_INVALID;
    if (index < 0 || (uint64_t)index > psrt_array_count(array))
        return PS_LIMIT;
    return psrt_array_replace(array, (size_t)index, 0, psrt_array_data(contents),
                              psrt_array_count(contents));
}
static inline ps_result psrt_array_remove_edge_count(psrt_array *array, int64_t count,
                                                     bool last) {
    if (!array || !array->type)
        return PS_INVALID;
    size_t length = psrt_array_count(array);
    if (count < 0 || (uint64_t)count > length)
        return PS_LIMIT;
    size_t removed = (size_t)count;
    return psrt_array_replace(array, last ? length - removed : 0, removed, NULL, 0);
}
/* Validate a half-open or closed array range without allocating. Missing bounds
 * mean zero and the array length respectively; a missing end is exclusive even
 * for the closed spelling, so a tail of an empty array remains empty. */
static inline ps_result psrt_array_range_bounds(const psrt_array *array, int has_begin,
                                                int64_t begin, int has_end, int64_t end,
                                                int closed, size_t *start, size_t *count) {
    if (!array || !array->type || !start || !count)
        return PS_INVALID;
    size_t length = psrt_array_count(array);
    if ((has_begin && (begin < 0 || (uint64_t)begin > length)) ||
        (has_end && (end < 0 || (uint64_t)end > length ||
                     (closed && (uint64_t)end == length))) ||
        (has_begin && has_end && end < begin))
        return PS_LIMIT;
    size_t first = has_begin ? (size_t)begin : 0;
    size_t stop = has_end ? (size_t)end + (closed ? 1u : 0u) : length;
    if (stop < first)
        return PS_LIMIT;
    *start = first;
    *count = stop - first;
    return PS_OK;
}
static inline ps_result psrt_array_range(const psrt_array *array, int64_t begin, int64_t end,
                                         int closed, size_t *start, size_t *count) {
    return psrt_array_range_bounds(array, 1, begin, 1, end, closed, start, count);
}
/* Construct an independent slice in an uninitialized, distinct output handle.
 * Open ranges permit count..<count; an explicit closed end must name an element.
 * Validate before conversion/addition, including INT64_MAX.
 * On failure output and source ownership are unchanged. */
static inline ps_result psrt_array_slice_bounds(const psrt_array *array, int has_begin,
                                                int64_t begin, int has_end, int64_t end,
                                                int closed, psrt_array *out) {
    if (!out || array == out)
        return PS_INVALID;
    size_t start = 0, count = 0;
    ps_result range = psrt_array_range_bounds(array, has_begin, begin, has_end, end, closed,
                                             &start, &count);
    if (range != PS_OK)
        return range;
    psrt_array result = *array;
    result.block = NULL;
    const void *elements =
        count ? (const unsigned char *)psrt_array_data(array) + start * array->type->size : NULL;
    ps_result status = psrt_array_replace(&result, 0, 0, elements, count);
    if (status == PS_OK)
        *out = result;
    return status;
}
static inline ps_result psrt_array_slice(const psrt_array *array, int64_t begin, int64_t end,
                                         int closed, psrt_array *out) {
    return psrt_array_slice_bounds(array, 1, begin, 1, end, closed, out);
}
/* Nonmutating Swift-style edge selection. Clamp an oversized count and return
 * an independent array value, matching Physim's existing slice ownership. */
static inline ps_result psrt_array_select_edge(const psrt_array *array, int64_t requested,
                                               bool drop, bool last, psrt_array *out) {
    if (!array || !array->type || !out || array == out)
        return PS_INVALID;
    if (requested < 0)
        return PS_LIMIT;
    size_t length = psrt_array_count(array);
    size_t amount = (uint64_t)requested < length ? (size_t)requested : length;
    size_t count = drop ? length - amount : amount;
    size_t start = drop ? (last ? 0 : amount) : (last ? length - count : 0);
    return psrt_array_slice(array, (int64_t)start, (int64_t)(start + count), 0, out);
}
/* Build an independent value in reverse element order. A failed element copy
 * destroys the completed prefix; the caller's output remains unowned. */
static inline ps_result psrt_array_reversed(const psrt_array *array, psrt_array *out) {
    if (!array || !array->type || !out || array == out)
        return PS_INVALID;
    size_t count = psrt_array_count(array);
    psrt_array result = *array;
    result.block = NULL;
    ps_result status = psrt_array_build_begin(&result, count);
    if (status != PS_OK)
        return status;
    const unsigned char *data = psrt_array_data(array);
    for (size_t i = count; i; i--) {
        status = psrt_array_copy_one(result.block, data + (i - 1) * array->type->size);
        if (status != PS_OK) {
            psrt_array_destroy(&result);
            return status;
        }
    }
    *out = result;
    return PS_OK;
}
/* Repeat any element type with its descriptor's copy operation. A failed copy
 * releases the completed prefix and leaves both source and output unchanged. */
static inline ps_result psrt_array_repeated(const psrt_array *array, int64_t count,
                                            psrt_array *out) {
    if (!array || !array->type || count < 0 || !out || array == out)
        return PS_INVALID;
    size_t length = psrt_array_count(array);
    psrt_array result = *array;
    result.block = NULL;
    if (!length || !count) {
        *out = result;
        return PS_OK;
    }
    if ((uint64_t)count > array->maximum_count / length)
        return PS_LIMIT;
    if (count == 1)
        return psrt_array_clone(array, out);
    size_t total = (size_t)count * length;
    ps_result status = psrt_array_build_begin(&result, total);
    if (status != PS_OK)
        return status;
    const unsigned char *data = psrt_array_data(array);
    size_t stride = array->type->size;
    for (size_t i = 0; i < total; i++) {
        status = psrt_array_copy_one(result.block, data + (i % length) * stride);
        if (status != PS_OK) {
            psrt_array_destroy(&result);
            return status;
        }
    }
    *out = result;
    return PS_OK;
}
/* Publish only after the reversed value is complete. Shared snapshots and the
 * original value remain valid if allocation or an element copy fails. */
static inline ps_result psrt_array_reverse(psrt_array *array) {
    if (!array || !array->type)
        return PS_INVALID;
    psrt_array result = {0};
    ps_result status = psrt_array_reversed(array, &result);
    if (status != PS_OK)
        return status;
    psrt_array_destroy(array);
    *array = result;
    return PS_OK;
}
typedef int (*psrt_array_compare)(const void *, const void *);
typedef bool (*psrt_array_valid_element)(const void *);
/* An independent copy for generated comparator sorting. The output remains
 * inert on failure, and each owned element is released with the partial block. */
static inline ps_result psrt_array_copy_elements(const psrt_array *array, psrt_array *out) {
    if (!array || !array->type || !out || array == out)
        return PS_INVALID;
    size_t count = psrt_array_count(array);
    psrt_array result = *array;
    result.block = NULL;
    ps_result status = psrt_array_build_begin(&result, count);
    if (status != PS_OK)
        return status;
    const unsigned char *data = psrt_array_data(array);
    for (size_t i = 0; i < count; i++) {
        status = psrt_array_copy_one(result.block, data + i * array->type->size);
        if (status != PS_OK) {
            psrt_array_destroy(&result);
            return status;
        }
    }
    *out = result;
    return PS_OK;
}
/* Construct a private value before moving whole element representations. This
 * preserves snapshots and leaves the receiver untouched on copy failure. */
static inline ps_result psrt_array_swap_at(psrt_array *array, int64_t first, int64_t second) {
    if (!array || !array->type)
        return PS_INVALID;
    size_t count = psrt_array_count(array);
    if (first < 0 || second < 0 || (uint64_t)first >= count || (uint64_t)second >= count)
        return PS_LIMIT;
    if (first == second)
        return PS_OK;
    psrt_array result = {0};
    ps_result status = psrt_array_copy_elements(array, &result);
    if (status != PS_OK)
        return status;
    size_t stride = array->type->size;
    unsigned char *data = (unsigned char *)(result.block + 1);
    unsigned char *left = data + (size_t)first * stride;
    unsigned char *right = data + (size_t)second * stride;
    for (size_t i = 0; i < stride; i++) {
        unsigned char byte = left[i];
        left[i] = right[i];
        right[i] = byte;
    }
    psrt_array_destroy(array);
    *array = result;
    return PS_OK;
}
/* Keep marked elements in order. The mask is evaluated before any copying, so
 * a failing predicate or element copy cannot publish a partial mutation. */
static inline ps_result psrt_array_select_mask(const psrt_array *array,
                                               const unsigned char *keep,
                                               psrt_array *out) {
    if (!array || !array->type || !out || array == out)
        return PS_INVALID;
    size_t count = psrt_array_count(array), retained = 0;
    if (count && !keep)
        return PS_INVALID;
    for (size_t i = 0; i < count; i++)
        retained += keep[i] != 0;
    psrt_array result = *array;
    result.block = NULL;
    ps_result status = psrt_array_build_begin(&result, retained);
    if (status != PS_OK)
        return status;
    const unsigned char *data = psrt_array_data(array);
    for (size_t i = 0; i < count; i++) {
        if (!keep[i])
            continue;
        status = psrt_array_copy_one(result.block, data + i * array->type->size);
        if (status != PS_OK) {
            psrt_array_destroy(&result);
            return status;
        }
    }
    *out = result;
    return PS_OK;
}
typedef struct psrt_sort_scratch {
    ps_allocator allocator;
    void *data;
    size_t bytes;
} psrt_sort_scratch;
static inline ps_result psrt_sort_scratch_init(psrt_sort_scratch *scratch,
                                                ps_allocator allocator,
                                                size_t count, size_t stride) {
    if (!scratch || !stride || count > SIZE_MAX / stride)
        return PS_LIMIT;
    size_t bytes = count * stride;
    void *data = NULL;
    ps_result status = ps_memory_allocate(allocator, bytes, &data);
    if (status != PS_OK)
        return status;
    *scratch = (psrt_sort_scratch){allocator, data, bytes};
    return PS_OK;
}
static inline void psrt_sort_scratch_destroy(void *object) {
    psrt_sort_scratch *scratch = object;
    if (scratch && scratch->data) {
        ps_memory_free(scratch->allocator, scratch->data, scratch->bytes);
        scratch->data = NULL;
    }
}
static inline int psrt_compare_int64(const void *left, const void *right) {
    int64_t a = *(const int64_t *)left, b = *(const int64_t *)right;
    return (a > b) - (a < b);
}
static inline int psrt_compare_float64(const void *left, const void *right) {
    double a = *(const double *)left, b = *(const double *)right;
    return (a > b) - (a < b);
}
static inline bool psrt_finite_float64_element(const void *value) {
    return isfinite(*(const double *)value);
}
/* Return the first extremum on ties. An empty array leaves index untouched. */
static inline ps_result psrt_array_extremum_index(const psrt_array *array,
                                                 psrt_array_compare compare,
                                                 psrt_array_valid_element valid,
                                                 bool maximum, size_t *index) {
    if (!array || !array->type || !compare || !index)
        return PS_INVALID;
    size_t count = psrt_array_count(array);
    const unsigned char *data = psrt_array_data(array);
    size_t best = 0;
    for (size_t i = 0; i < count; i++) {
        const void *candidate = data + i * array->type->size;
        if (valid && !valid(candidate))
            return PS_NUMERIC;
        if (i && ((maximum && compare(candidate, data + best * array->type->size) > 0) ||
                  (!maximum && compare(candidate, data + best * array->type->size) < 0)))
            best = i;
    }
    if (count)
        *index = best;
    return PS_OK;
}
/* Copy before sorting so a failed allocation/copy cannot change the source.
 * Equal primitive values are indistinguishable, so qsort stability is immaterial. */
static inline ps_result psrt_array_sorted(const psrt_array *array, psrt_array_compare compare,
                                          psrt_array_valid_element valid, psrt_array *out) {
    if (!array || !array->type || !compare || !out || array == out)
        return PS_INVALID;
    size_t count = psrt_array_count(array);
    psrt_array result = *array;
    result.block = NULL;
    ps_result status = psrt_array_build_begin(&result, count);
    if (status != PS_OK)
        return status;
    const unsigned char *data = psrt_array_data(array);
    for (size_t i = 0; i < count; i++) {
        const void *element = data + i * array->type->size;
        if (valid && !valid(element)) {
            psrt_array_destroy(&result);
            return PS_NUMERIC;
        }
        status = psrt_array_copy_one(result.block, element);
        if (status != PS_OK) {
            psrt_array_destroy(&result);
            return status;
        }
    }
    if (count > 1)
        qsort(result.block + 1, count, array->type->size, compare);
    *out = result;
    return PS_OK;
}
static inline ps_result psrt_array_sort(psrt_array *array, psrt_array_compare compare,
                                        psrt_array_valid_element valid) {
    if (!array || !array->type)
        return PS_INVALID;
    psrt_array result = {0};
    ps_result status = psrt_array_sorted(array, compare, valid, &result);
    if (status != PS_OK)
        return status;
    psrt_array_destroy(array);
    *array = result;
    return PS_OK;
}
/* A positive stride selects elements from a validated range without retaining
 * source storage. Copy failure destroys the partial result and leaves out inert. */
static inline ps_result psrt_array_slice_strided_bounds(const psrt_array *array, int has_begin,
                                                        int64_t begin, int has_end, int64_t end,
                                                        int closed, int64_t step,
                                                        psrt_array *out) {
    if (!out || array == out)
        return PS_INVALID;
    if (step <= 0)
        return PS_LIMIT;
    size_t start = 0, span = 0;
    ps_result status = psrt_array_range_bounds(array, has_begin, begin, has_end, end, closed,
                                               &start, &span);
    if (status != PS_OK)
        return status;
    size_t stride = (uint64_t)step > SIZE_MAX ? SIZE_MAX : (size_t)step;
    size_t selected = span ? 1 + (span - 1) / stride : 0;
    psrt_array result = *array;
    result.block = NULL;
    status = psrt_array_build_begin(&result, selected);
    if (status != PS_OK)
        return status;
    const unsigned char *data = psrt_array_data(array);
    for (size_t i = 0; i < selected; i++) {
        status = psrt_array_copy_one(result.block,
                                     data + (start + i * stride) * array->type->size);
        if (status != PS_OK) {
            psrt_array_destroy(&result);
            return status;
        }
    }
    *out = result;
    return PS_OK;
}
/* Scatter replacement keeps the array length. The replacement must contain
 * exactly one value per selected position. All copies finish before publish. */
static inline ps_result psrt_array_scatter(psrt_array *array, size_t start, size_t span,
                                           int64_t step, const psrt_array *replacement) {
    if (!array || !array->type || !replacement || replacement->type != array->type)
        return PS_INVALID;
    if (step <= 0)
        return PS_LIMIT;
    size_t old_count = psrt_array_count(array);
    if (start > old_count || span > old_count - start)
        return PS_LIMIT;
    size_t stride = (uint64_t)step > SIZE_MAX ? SIZE_MAX : (size_t)step;
    size_t selected = span ? 1 + (span - 1) / stride : 0;
    if (psrt_array_count(replacement) != selected)
        return PS_LIMIT;
    if (!selected)
        return PS_OK;
    psrt_array result = *array;
    result.block = NULL;
    ps_result status = psrt_array_build_begin(&result, old_count);
    if (status != PS_OK)
        return status;
    const unsigned char *original = psrt_array_data(array);
    const unsigned char *items = psrt_array_data(replacement);
    size_t picked = 0, element_size = array->type->size;
    for (size_t i = 0; i < old_count; i++) {
        const void *source = picked < selected && i == start + picked * stride
                                 ? items + picked++ * element_size
                                 : original + i * element_size;
        status = psrt_array_copy_one(result.block, source);
        if (status != PS_OK) {
            psrt_array_destroy(&result);
            return status;
        }
    }
    psrt_array_block *old = array->block;
    array->block = result.block;
    psrt_array_block_release(old);
    return PS_OK;
}
/* Descriptor for nested arrays. A generated struct descriptor composes these
 * same clone/destroy operations and rolls back partial fields on copy failure. */
static inline ps_result psrt_array_element_copy(void *destination, const void *source) {
    return psrt_array_clone(source, destination);
}
static const psrt_element_type psrt_array_element_type = {
    sizeof(psrt_array), psrt_array_element_copy, psrt_array_destroy};
static inline ps_result psrt_array_split_push(psrt_array *parts, const psrt_array *source,
                                              size_t begin, size_t end) {
    psrt_array piece;
    ps_result status = psrt_array_slice(source, (int64_t)begin, (int64_t)end, 0, &piece);
    if (status != PS_OK)
        return status;
    status = psrt_array_builder_append(parts, &piece, 1);
    psrt_array_destroy(&piece);
    return status;
}
/* Split an Equatable element array into independent array values. The generated
 * equality callback follows the source element type's == semantics. */
static inline ps_result psrt_array_split(const psrt_array *source, const void *separator,
                                         int64_t max_splits, bool omit_empty,
                                         bool (*equal)(const void *, const void *),
                                         ps_allocator allocator, psrt_array *out) {
    if (!source || !source->type || !separator || max_splits < 0 || !equal ||
        !ps_allocator_valid(allocator) || !out || out == source || out == separator)
        return PS_INVALID;
    psrt_array result;
    ps_result status = psrt_array_init(&psrt_array_element_type, allocator, 0, &result);
    if (status != PS_OK)
        return status;
    size_t count = psrt_array_count(source), begin = 0, produced = 0;
    uint64_t limit = (uint64_t)max_splits;
    const unsigned char *data = psrt_array_data(source);
    for (size_t i = 0; i < count && (uint64_t)produced < limit; i++) {
        if (!equal(data + i * source->type->size, separator))
            continue;
        if (!omit_empty || i > begin) {
            status = psrt_array_split_push(&result, source, begin, i);
            if (status != PS_OK) goto failed;
            produced++;
        }
        begin = i + 1;
    }
    if (!omit_empty || begin < count) {
        status = psrt_array_split_push(&result, source, begin, count);
        if (status != PS_OK) goto failed;
    }
    *out = result;
    return PS_OK;
failed:
    psrt_array_destroy(&result);
    return status;
}
/* A function value owns a snapshot of its receiver when it names an instance
 * method. Free and static functions leave receiver empty. */
typedef struct psrt_function_value {
    size_t tag;
    psrt_array receiver;
} psrt_function_value;
/* A mutating bound method owns one receiver cell. Copies of the function value
 * intentionally share that cell, so a call through any copy advances the same
 * state. The cell must not be accessed concurrently. Ordinary arrays remain COW. */
static inline void *psrt_function_mutable_receiver(const psrt_function_value *function) {
    if (!function || psrt_array_count(&function->receiver) != 1)
        return NULL;
    return (void *)(function->receiver.block + 1);
}
static inline ps_result psrt_function_copy(void *destination, const void *source) {
    const psrt_function_value *original = source;
    psrt_function_value *copy = destination;
    *copy = (psrt_function_value){original->tag, {0}};
    return original->receiver.type
               ? psrt_array_clone(&original->receiver, &copy->receiver) : PS_OK;
}
static inline void psrt_function_destroy(void *value) {
    psrt_function_value *function = value;
    psrt_array_destroy(&function->receiver);
    function->tag = 0;
}
#endif
