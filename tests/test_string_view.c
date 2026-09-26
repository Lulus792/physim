#include "physim/string_view.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "String view line %d: %s\n", __LINE__, #x);                            \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static ps_string_view view(const char *text) { return (ps_string_view){text, strlen(text)}; }
static size_t reference_find(const unsigned char *text, size_t n, const unsigned char *needle,
                             size_t m, size_t start) {
    for (size_t i = start; i <= n; i++) {
        size_t j = 0;
        while (j < m && j < n - i && text[i + j] == needle[j])
            j++;
        if (j == m)
            return i;
    }
    return SIZE_MAX;
}
int main(void) {
    ps_string_view empty = {0}, invalid = {NULL, 1}, v;
    CHECK(ps_string_view_valid(empty) && !ps_string_view_valid(invalid));
    CHECK(ps_string_view_equal(empty, view("")) && !ps_string_view_equal(invalid, invalid));
    CHECK(ps_string_view_make(NULL, 0, &v) == PS_OK && v.data == NULL && v.size == 0);
    CHECK(ps_string_view_cstr("a\0b", &v) == PS_OK && v.size == 1);
    ps_string_view saved = v;
    CHECK(ps_string_view_make(NULL, 1, &v) == PS_INVALID && v.data == saved.data &&
          v.size == saved.size);
    CHECK(ps_string_view_cstr(NULL, &v) == PS_INVALID && v.data == saved.data);
    CHECK(ps_string_view_make("a", 1, NULL) == PS_INVALID);
    CHECK(ps_string_view_slice(empty, 0, 0, &v) == PS_OK && v.data == NULL);
    CHECK(ps_string_view_slice(view("abc"), 3, 0, &v) == PS_OK && v.size == 0);
    saved = v;
    CHECK(ps_string_view_slice(view("abc"), SIZE_MAX, 1, &v) == PS_INVALID && v.data == saved.data);
    CHECK(ps_string_view_slice(view("abc"), 1, SIZE_MAX, &v) == PS_INVALID);
    CHECK(ps_string_view_trim_ascii(view(" \t\r\n\f\v x \t"), &v) == PS_OK &&
          ps_string_view_equal(v, view("x")));
    CHECK(ps_string_view_trim_ascii(view(" \t"), &v) == PS_OK && v.size == 0);
    CHECK(ps_string_view_trim_ascii(empty, &v) == PS_OK && v.data == NULL);
    const char utf8[] = "\xc2\xa0\xc3\xa4\xc2\xa0";
    CHECK(ps_string_view_trim_ascii(view(utf8), &v) == PS_OK && v.size == 6);
    CHECK(ps_string_view_slice(v, 3, 1, &v) == PS_OK && (unsigned char)v.data[0] == 0xa4);
    char binary[] = {' ', 0, ' ', 'a', 0};
    CHECK(ps_string_view_trim_ascii((ps_string_view){binary, 5}, &v) == PS_OK && v.size == 4 &&
          v.data[0] == 0);
    ps_string_view before, after, delimiter = view("::");
    CHECK(ps_string_view_split(view("::a::"), delimiter, &before, &after) == PS_OK &&
          before.size == 0 && ps_string_view_equal(after, view("a::")));
    CHECK(ps_string_view_split(after, delimiter, &before, &after) == PS_OK &&
          ps_string_view_equal(before, view("a")) && after.size == 0);
    saved = before;
    ps_string_view saved_after = after;
    CHECK(ps_string_view_split(view("absent"), delimiter, &before, &after) == PS_EOF &&
          before.data == saved.data && after.data == saved_after.data);
    CHECK(ps_string_view_split(view("a"), empty, &before, &after) == PS_INVALID);
    CHECK(ps_string_view_split(view("a::b"), delimiter, &before, &before) == PS_INVALID);
    CHECK(ps_string_view_split((ps_string_view){binary, 5}, (ps_string_view){binary + 1, 1},
                               &before, &after) == PS_OK &&
          before.size == 1 && after.size == 3);
    /* Unsigned byte order must not depend on whether plain char is signed. */
    for (unsigned a = 0; a < 256; a++)
        for (unsigned b = 0; b < 256; b++) {
            char x = (char)a, y = (char)b;
            int order = 7;
            CHECK(ps_string_view_compare((ps_string_view){&x, 1}, (ps_string_view){&y, 1},
                                         &order) == PS_OK);
            CHECK(order == (a < b ? -1 : a > b ? 1 : 0));
        }
    int order = 7;
    CHECK(ps_string_view_compare(view("a"), view("aa"), &order) == PS_OK && order == -1);
    CHECK(ps_string_view_compare(empty, view(""), &order) == PS_OK && order == 0);
    CHECK(ps_string_view_compare(invalid, empty, &order) == PS_INVALID && order == 0);
    ps_rng rng;
    ps_rng_seed(&rng, 37492);
    for (unsigned trial = 0; trial < 5000; trial++) {
        size_t n = (size_t)(ps_rng_uniform(&rng) * 65), m = (size_t)(ps_rng_uniform(&rng) * 17);
        size_t start = (size_t)(ps_rng_uniform(&rng) * (n + 1));
        unsigned char *text = n ? malloc(n) : NULL, *needle = m ? malloc(m) : NULL;
        CHECK((!n || text) && (!m || needle));
        for (size_t i = 0; i < n; i++)
            text[i] = (unsigned char)(ps_rng_uniform(&rng) * 4);
        for (size_t i = 0; i < m; i++)
            needle[i] = (unsigned char)(ps_rng_uniform(&rng) * 4);
        size_t expected = reference_find(text, n, needle, m, start), actual = SIZE_MAX;
        ps_result r = ps_string_view_find((ps_string_view){(char *)text, n},
                                          (ps_string_view){(char *)needle, m}, start, &actual);
        CHECK(r == (expected == SIZE_MAX ? PS_EOF : PS_OK) && actual == expected);
        free(text);
        free(needle);
    }
    size_t found = 99;
    CHECK(ps_string_view_find(view("abc"), empty, 3, &found) == PS_OK && found == 3);
    CHECK(ps_string_view_find(view("abc"), view("a"), 4, &found) == PS_INVALID && found == 3);
    CHECK(ps_string_view_find(invalid, empty, 0, &found) == PS_INVALID);
    CHECK(ps_string_view_find(empty, empty, 0, &found) == PS_OK && found == 0);
    char output[8] = "keep";
    CHECK(ps_string_view_copy(view("abcd"), output, 4) == PS_LIMIT && !strcmp(output, "keep"));
    CHECK(ps_string_view_copy((ps_string_view){"x", SIZE_MAX}, output, SIZE_MAX) == PS_LIMIT);
    CHECK(ps_string_view_copy(invalid, output, sizeof output) == PS_INVALID &&
          !strcmp(output, "keep"));
    CHECK(ps_string_view_copy((ps_string_view){binary, 5}, output, sizeof output) == PS_OK &&
          !memcmp(output, binary, 5) && output[5] == 0);
    memcpy(output, "abcdef", 7);
    CHECK(ps_string_view_copy((ps_string_view){output + 1, 4}, output, sizeof output) == PS_OK &&
          !strcmp(output, "bcde"));
    CHECK(ps_string_view_copy((ps_string_view){output, 4}, output + 1, 7) == PS_OK &&
          !strcmp(output + 1, "bcde"));
    CHECK(ps_string_view_copy(empty, output, 1) == PS_OK && output[0] == 0);
    CHECK(ps_string_view_copy(empty, output, 0) == PS_LIMIT);
    CHECK(ps_string_view_copy(empty, NULL, 0) == PS_INVALID);
    puts("String views: 5000 bounded searches, all byte order pairs, empty/binary/UTF-8 ranges and "
         "overlap passed");
    return 0;
}
