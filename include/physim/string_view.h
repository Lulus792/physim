#ifndef PHYSIM_STRING_VIEW_H
#define PHYSIM_STRING_VIEW_H
#include "core.h"

/* Borrowed read-only bytes, not necessarily NUL terminated. Caller guarantees a
 * readable region of size bytes and keeps it alive; views never own memory.
 * NULL is valid only with size=0. Embedded NULs are ordinary bytes. UTF-8 may be
 * stored, but offsets, ordering and slicing are byte-based, not Unicode-aware. */
typedef struct {
    const char *data;
    size_t size;
} ps_string_view;
bool ps_string_view_valid(ps_string_view view);
ps_result ps_string_view_make(const char *data, size_t size, ps_string_view *out);
/* Requires a valid terminated C string; NULL is invalid. */
ps_result ps_string_view_cstr(const char *text, ps_string_view *out);
/* Lexicographic unsigned-byte order; result is -1, 0 or 1. Invalid -> PS_INVALID.
 * All checked operations leave outputs unchanged on error, including PS_EOF. */
ps_result ps_string_view_compare(ps_string_view a, ps_string_view b, int *order);
/* Invalid views compare unequal, including to each other. */
bool ps_string_view_equal(ps_string_view a, ps_string_view b);
ps_result ps_string_view_slice(ps_string_view view, size_t first, size_t count,
                               ps_string_view *out);
/* First match at/after start<=view.size. Empty needle matches start, including
 * the end. No match -> PS_EOF. Bounded reads, no allocation; worst case O(n*m). */
ps_result ps_string_view_find(ps_string_view view, ps_string_view needle, size_t start,
                              size_t *position);
/* Split at the first delimiter, excluding it from both outputs. Delimiter must
 * be nonempty; outputs must be distinct objects. Leading/trailing empty fields
 * are preserved. A missing delimiter yields PS_EOF with both outputs unchanged. */
ps_result ps_string_view_split(ps_string_view view, ps_string_view delimiter,
                               ps_string_view *before, ps_string_view *after);
/* Remove ASCII space, tab, CR, LF, form feed and vertical tab at both ends.
 * Locale-independent; Unicode whitespace and embedded NULs remain unchanged. */
ps_result ps_string_view_trim_ascii(ps_string_view view, ps_string_view *out);
/* Copy every byte, then append NUL. Overlap is supported. capacity must exceed
 * view.size; insufficient capacity -> PS_LIMIT, no partial write. Embedded NULs
 * are preserved, so such output is not a single conventional C text string.
 * Output storage must not overlap a descriptor used after the call. */
ps_result ps_string_view_copy(ps_string_view view, char *output, size_t capacity);
#endif
