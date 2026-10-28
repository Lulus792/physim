#ifndef PHYSIM_FILE_BROWSER_H
#define PHYSIM_FILE_BROWSER_H
#include "physim/core.h"
#include <stdio.h>

enum { PS_BROWSER_PATH = 4096, PS_BROWSER_ENTRY_LIMIT = 8192 };
typedef enum {
    PS_BROWSER_DIRECTORY, PS_BROWSER_C_SOURCE, PS_BROWSER_PHYS_SOURCE,
    PS_BROWSER_RUN, PS_BROWSER_IMAGE, PS_BROWSER_PROJECT,
    PS_BROWSER_TEXT, PS_BROWSER_OTHER
} ps_browser_kind;
typedef enum { PS_BROWSER_FILES, PS_BROWSER_FOLDERS, PS_BROWSER_RUNS } ps_browser_mode;
typedef enum { PS_BROWSER_SORT_NAME, PS_BROWSER_SORT_TYPE } ps_browser_sort;
typedef struct {
    char *path; /* Owned UTF-8; name is path + name_offset. */
    size_t name_offset;
    uint64_t size;
    int64_t modified_ns;
    ps_browser_kind kind;
    ps_result error;
    bool hidden;
} ps_browser_entry;
typedef struct {
    char directory[PS_BROWSER_PATH];
    ps_browser_entry *entries;
    size_t count, capacity, omitted; /* Known omissions; a lower bound at limit. */
    ps_result status;
} ps_browser_listing;
typedef bool (*ps_browser_cancel)(void *user);

/* Pure lexical resolution, using the existing absolute-path rules. Normalize
 * separators/dot segments, preserving drive and UNC-share roots on Windows.
 * Windows drive-relative/device namespaces are rejected. Outputs may alias
 * inputs and remain unchanged on error. No directory enumeration is performed.
 * These are lexical breadcrumb utilities; filesystem operations below resolve
 * POSIX links/.. using native semantics instead of this textual shortening. */
ps_result ps_browser_path(const char *path, char out[PS_BROWSER_PATH]);
ps_result ps_browser_parent(const char *path, char out[PS_BROWSER_PATH]);
ps_result ps_browser_child(const char *directory, const char *name, char out[PS_BROWSER_PATH]);

/* Initialize listings to zero; destroy releases all owned storage. Read only
 * enumerates one directory. Successful/partial snapshots replace out atomically:
 * PS_LIMIT/PS_CORRUPT report omissions (a lower bound if enumeration stops).
 * PS_IO/PS_MEMORY/PS_INVALID
 * and PS_EOF (cooperative cancellation) leave the previous snapshot untouched.
 * limit=0 uses the bounded default; callers run blocking filesystem work in
 * their worker, not during painting. A callback cannot interrupt a kernel call. */
void ps_browser_listing_destroy(ps_browser_listing *listing);
ps_result ps_browser_listing_read(const char *directory, size_t limit,
                                  ps_browser_cancel cancel, void *user,
                                  ps_browser_listing *out);
void ps_browser_listing_sort(ps_browser_listing *listing, ps_browser_sort sort);
const char *ps_browser_entry_name(const ps_browser_entry *entry);
bool ps_browser_entry_matches(const ps_browser_entry *entry, ps_browser_mode mode,
                               const char *query, bool show_hidden);
/* Confirm rechecks the actual regular-file/directory type and run extension;
 * content validation remains with the existing importer. Cached metadata is
 * not authority and no object is opened/activated by this query. */
bool ps_browser_selectable(const char *path, ps_browser_mode mode);
/* Open one existing regular file for read-only preview. Uses the native path
 * spelling (including POSIX link/.. semantics and Windows extended paths).
 * Rejects directories and special files without blocking on a FIFO open.
 * Caller closes the stream. Outputs change only on success. */
ps_result ps_browser_open_file(const char *path, FILE **out, uint64_t *size);

typedef enum {
    PS_BROWSER_CREATED, PS_BROWSER_NAME_INVALID, PS_BROWSER_COLLISION,
    PS_BROWSER_CREATE_FAILED
} ps_browser_create_result;
/* Exclusive single-child mkdir. Existing files/folders/links are never treated
 * as newly created or overwritten. Only CREATED writes the resulting path. */
ps_browser_create_result ps_browser_create_directory(const char *parent, const char *name,
                                                     char out[PS_BROWSER_PATH]);
#endif
