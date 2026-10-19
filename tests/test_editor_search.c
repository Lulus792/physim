#include "../app/editor_search.h"
#include <assert.h>
#include <stdio.h>

/* Independent exhaustive oracle deliberately uses the simple bounded scan. */
static ps_editor_search_result reference(const char *s, const char *q, size_t cursor, bool back) {
    ps_editor_search_result r = {0};
    size_t n = strlen(s), m = strlen(q), first = 0, last = 0;
    if (!m) return r;
    for (size_t i = 0; i + m <= n;) {
        if (memcmp(s + i, q, m)) { ++i; continue; }
        if (!r.count) first = i;
        last = i;
        ++r.count;
        if ((back && i < cursor) || (!back && !r.ordinal && i >= cursor)) {
            r.ordinal = r.count;
            r.begin = i;
        }
        i += m;
    }
    if (r.count) {
        r.found = true;
        r.wrapped = !r.ordinal;
        if (!r.ordinal) { r.ordinal = back ? r.count : 1; r.begin = back ? last : first; }
        r.end = r.begin + m;
    }
    return r;
}
static void check(const char *s, const char *q) {
    for (size_t cursor = 0; cursor <= strlen(s); ++cursor) for (int back = 0; back < 2; ++back) {
        ps_editor_search_result actual, expected = reference(s, q, cursor, back != 0);
        assert(ps_editor_search(s, strlen(s), q, strlen(q), cursor, back != 0, &actual));
        assert(actual.count == expected.count && actual.ordinal == expected.ordinal);
        assert(actual.begin == expected.begin && actual.end == expected.end);
        assert(actual.found == expected.found && actual.wrapped == expected.wrapped);
    }
}
int main(void) {
    check("", ""); check("", "x"); check("aaaAAAaaa", "aa");
    check("ä😀\nAä😀\n", "ä😀"); check("\n\nx\n\n", "\n\n");
    check("abcabcabc", "abc"); check("aaaaaaaabaaaaab", "aaaab");
    char text[11], query[6];
    for (unsigned n = 0; n <= 9; ++n) for (unsigned bits = 0; bits < (1u << n); ++bits) {
        for (unsigned i = 0; i < n; ++i) text[i] = (bits & (1u << i)) ? 'a' : 'b';
        text[n] = 0;
        for (unsigned m = 0; m <= 4; ++m) for (unsigned qb = 0; qb < (1u << m); ++qb) {
            for (unsigned i = 0; i < m; ++i) query[i] = (qb & (1u << i)) ? 'a' : 'b';
            query[m] = 0;
            check(text, query);
        }
    }
    ps_editor_search_result r;
    char long_query[PS_EDITOR_SEARCH_CAPACITY + 1];
    memset(long_query, 'a', sizeof long_query);
    assert(!ps_editor_search("a", 1, "a", 1, 2, false, &r));
    assert(!ps_editor_search("a", 1, long_query, sizeof long_query - 1, 0, false, &r));
    assert(!ps_editor_search(NULL, 1, "a", 1, 0, false, &r));
    assert(!ps_editor_search("a", 1, NULL, 0, 0, false, &r));
    assert(!ps_editor_search("a", 1, "a", 1, 0, false, NULL));
    assert(ps_editor_search(NULL, 0, "", 0, 0, false, &r) && !r.found);
    char large[65536]; memset(large, 'a', sizeof large); large[sizeof large - 1] = 'b';
    memset(long_query, 'a', sizeof long_query); long_query[126] = 'b';
    assert(ps_editor_search(large, sizeof large, long_query, 127, 0, false, &r));
    assert(r.count == 1 && r.begin == sizeof large - 127);
    puts("Literal editor search checks passed");
    return 0;
}
