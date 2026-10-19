#ifndef PS_EDITOR_SEARCH_H
#define PS_EDITOR_SEARCH_H

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

/* Literal, case-sensitive, non-overlapping matches. Offsets are bytes, not
 * Unicode code points. Valid UTF-8 text/query therefore retain rune boundaries.
 * One pass uses O(text_length + query_length) time and bounded stack storage. */
#define PS_EDITOR_SEARCH_CAPACITY 128
typedef struct {
    size_t count, ordinal, begin, end;
    bool found, wrapped;
} ps_editor_search_result;

static inline bool ps_editor_search(const char *text, size_t length,
                                    const char *query, size_t query_length,
                                    size_t cursor, bool backward,
                                    ps_editor_search_result *result) {
    if (!result) return false;
    memset(result, 0, sizeof *result);
    if ((!text && length) || !query || cursor > length ||
        query_length >= PS_EDITOR_SEARCH_CAPACITY) return false;
    if (!query_length) return true;
    size_t prefix[PS_EDITOR_SEARCH_CAPACITY] = {0};
    for (size_t i = 1, j = 0; i < query_length; ++i) {
        while (j && query[i] != query[j]) j = prefix[j - 1];
        if (query[i] == query[j]) ++j;
        prefix[i] = j;
    }
    size_t first = 0, last = 0, chosen = 0, chosen_ordinal = 0;
    for (size_t i = 0, j = 0; i < length; ++i) {
        while (j && text[i] != query[j]) j = prefix[j - 1];
        if (text[i] == query[j]) ++j;
        if (j != query_length) continue;
        size_t at = i + 1 - query_length;
        if (!result->count) first = at;
        last = at;
        ++result->count;
        if ((backward && at < cursor) || (!backward && !chosen_ordinal && at >= cursor)) {
            chosen = at;
            chosen_ordinal = result->count;
        }
        j = 0;
    }
    if (!result->count) return true;
    result->found = true;
    result->wrapped = !chosen_ordinal;
    result->begin = chosen_ordinal ? chosen : backward ? last : first;
    result->end = result->begin + query_length;
    result->ordinal = chosen_ordinal ? chosen_ordinal : backward ? result->count : 1;
    return true;
}
#endif
