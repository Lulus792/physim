#ifndef PHYSIM_MEMORY_H
#define PHYSIM_MEMORY_H
#include "core.h"

/* UCRT's C headers do not expose max_align_t. These are the maximally aligned
 * fundamental C scalar types in the supported MSVC/Clang-Cl Windows ABI. */
#ifdef _MSC_VER
typedef union {
    long double floating;
    long long integer;
    void *pointer;
} ps_memory_alignment;
#else
typedef max_align_t ps_memory_alignment;
#endif
#define PS_MEMORY_ALIGNMENT _Alignof(ps_memory_alignment)

/* An allocation domain, passed explicitly and copied by value into owners.
 * user and callback code must outlive every allocation. No global allocator.
 * allocate returns NULL on failure or distinct writable storage for `bytes`,
 * aligned to PS_MEMORY_ALIGNMENT. Both callbacks receive strictly positive sizes;
 * deallocate receives the exact original allocation size. They must not throw,
 * longjmp or reenter the object currently being allocated/destroyed. */
typedef struct {
    void *user;
    void *(*allocate)(void *user, size_t bytes);
    void (*deallocate)(void *user, void *pointer, size_t bytes);
} ps_allocator;
ps_allocator ps_allocator_default(void);
bool ps_allocator_valid(ps_allocator allocator);
/* Checked helpers: PS_INVALID for invalid allocator/output or mismatched
 * NULL/size; PS_LIMIT for count*size overflow; PS_MEMORY on allocation failure.
 * Outputs remain unchanged on failure. A zero-byte request succeeds with NULL
 * and does not invoke a callback. Newly allocated bytes are otherwise uninitialized. */
ps_result ps_memory_allocate(ps_allocator allocator, size_t bytes, void **out);
ps_result ps_memory_zero(ps_allocator allocator, size_t count, size_t size, void **out);
void ps_memory_free(ps_allocator allocator, void *pointer, size_t bytes);
/* Allocate-copy-free, preserving min(old_bytes,new_bytes); new tail uninitialized.
 * On failure old storage/content and *out survive. On success old storage is freed,
 * except for equal-size requests. new_bytes=0 frees old storage and returns NULL.
 * old_bytes must match its allocation, and out must not lie inside old storage. */
ps_result ps_memory_resize(ps_allocator allocator, void *old_pointer, size_t old_bytes,
                           size_t new_bytes, void **out);

/* Fixed caller-owned storage, no heap fallback. Treat fields as read-only after
 * init. Each request uses PS_MEMORY_ALIGNMENT, even with an unaligned buffer.
 * Individual frees do not reclaim space; destroy users before reset. Resize
 * consumes a new block. All pointers become invalid after reset/reinitialization.
 * The arena and its buffer must outlive users; external synchronization required. */
typedef struct {
    unsigned char *buffer;
    size_t capacity, used;
} ps_arena;
ps_result ps_arena_init(ps_arena *arena, void *buffer, size_t capacity);
ps_allocator ps_arena_allocator(ps_arena *arena);
void ps_arena_reset(ps_arena *arena);
#endif
