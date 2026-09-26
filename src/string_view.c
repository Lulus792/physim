#include "physim/string_view.h"
#include <string.h>

bool ps_string_view_valid(ps_string_view view) { return view.data || !view.size; }
ps_result ps_string_view_make(const char *data, size_t size, ps_string_view *out) {
    if (!out || (!data && size))
        return PS_INVALID;
    *out = (ps_string_view){data, size};
    return PS_OK;
}
ps_result ps_string_view_cstr(const char *text, ps_string_view *out) {
    return text && out ? ps_string_view_make(text, strlen(text), out) : PS_INVALID;
}
ps_result ps_string_view_compare(ps_string_view a, ps_string_view b, int *order) {
    if (!order || !ps_string_view_valid(a) || !ps_string_view_valid(b))
        return PS_INVALID;
    size_t common = a.size < b.size ? a.size : b.size;
    int result = common ? memcmp(a.data, b.data, common) : 0;
    *order = result < 0 ? -1 : result > 0 ? 1 : a.size < b.size ? -1 : a.size > b.size ? 1 : 0;
    return PS_OK;
}
bool ps_string_view_equal(ps_string_view a, ps_string_view b) {
    return ps_string_view_valid(a) && ps_string_view_valid(b) && a.size == b.size &&
           (!a.size || !memcmp(a.data, b.data, a.size));
}
ps_result ps_string_view_slice(ps_string_view v, size_t first, size_t count, ps_string_view *out) {
    if (!out || !ps_string_view_valid(v) || first > v.size || count > v.size - first)
        return PS_INVALID;
    *out = (ps_string_view){v.data ? v.data + first : NULL, count};
    return PS_OK;
}
ps_result ps_string_view_find(ps_string_view v, ps_string_view needle, size_t start, size_t *out) {
    if (!out || !ps_string_view_valid(v) || !ps_string_view_valid(needle) || start > v.size)
        return PS_INVALID;
    if (!needle.size) {
        *out = start;
        return PS_OK;
    }
    if (needle.size > v.size - start)
        return PS_EOF;
    size_t last = v.size - needle.size;
    for (size_t at = start; at <= last; at++)
        if (v.data[at] == needle.data[0] && !memcmp(v.data + at, needle.data, needle.size)) {
            *out = at;
            return PS_OK;
        }
    return PS_EOF;
}
ps_result ps_string_view_split(ps_string_view v, ps_string_view delimiter, ps_string_view *before,
                               ps_string_view *after) {
    if (!before || !after || before == after || !delimiter.size)
        return PS_INVALID;
    size_t at;
    ps_result r = ps_string_view_find(v, delimiter, 0, &at);
    if (r != PS_OK)
        return r;
    ps_string_view left = {v.data, at},
                   right = {v.data + at + delimiter.size, v.size - at - delimiter.size};
    *before = left;
    *after = right;
    return PS_OK;
}
static bool ascii_space(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f' || c == '\v';
}
ps_result ps_string_view_trim_ascii(ps_string_view v, ps_string_view *out) {
    if (!out || !ps_string_view_valid(v))
        return PS_INVALID;
    size_t start = 0, end = v.size;
    while (start < end && ascii_space(v.data[start]))
        start++;
    while (end > start && ascii_space(v.data[end - 1]))
        end--;
    return ps_string_view_slice(v, start, end - start, out);
}
ps_result ps_string_view_copy(ps_string_view v, char *out, size_t capacity) {
    if (!out || !ps_string_view_valid(v))
        return PS_INVALID;
    if (capacity <= v.size)
        return PS_LIMIT;
    if (v.size)
        memmove(out, v.data, v.size);
    out[v.size] = 0;
    return PS_OK;
}
