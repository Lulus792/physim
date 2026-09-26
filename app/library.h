#ifndef PHYSIM_LIBRARY_H
#define PHYSIM_LIBRARY_H
#include "physim/core.h"
#define PS_LIBRARY_MAX_ITEMS 4096u
typedef enum { PS_LIBRARY_RUN, PS_LIBRARY_REPORT } ps_library_kind;
typedef struct {
    char name[1024];
    uint64_t bytes;
    int64_t modified_ns;
    ps_library_kind kind;
} ps_library_item;
typedef struct {
    char directory[4096];
    ps_library_item *items;
    size_t count, capacity, skipped;
    bool limited;
} ps_library;
/* Metadata-only, nonrecursive scan. Does not interpret file contents. Sorted by
 * modification time descending, then name. Failed scans leave out unchanged.
 * Enumeration stops at the explicit cap; limited signals an incomplete list. */
ps_result ps_library_scan(const char *directory, ps_library **out);
void ps_library_destroy(ps_library *library);
ps_result ps_library_path(const ps_library *library, size_t index, char *out, size_t capacity);
#endif
