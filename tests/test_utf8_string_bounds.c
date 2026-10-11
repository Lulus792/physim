#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION
#include "nuklear.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "UTF-8 bounds line %d: %s\n", __LINE__, #x); return 1; } } while (0)
static const char *tokens[] = {"a", "α", "😀", "b", "\n", "λ", "終", "z"};
static int deletion_matrix(void) {
    char original[64] = "";
    for (unsigned i = 0; i < 8; i++) strcat(original, tokens[i]);
    size_t bytes = strlen(original);
    for (unsigned start = 0; start <= 8; start++)
        for (unsigned count = 0; count <= 8 - start; count++) {
            char expected[64] = "";
            for (unsigned i = 0; i < 8; i++) if (i < start || i >= start + count) strcat(expected, tokens[i]);
            char *memory = malloc(bytes); CHECK(memory);
            struct nk_str text; nk_str_init_fixed(&text, memory, bytes);
            CHECK(nk_str_append_text_char(&text, original, (int)bytes) != 0);
            nk_str_delete_runes(&text, (int)start, (int)count);
            CHECK(nk_str_len_char(&text) == (int)strlen(expected) && nk_str_len(&text) == 8 - (int)count);
            CHECK(!strlen(expected) || !memcmp(memory, expected, strlen(expected)));
            nk_str_free(&text); free(memory);
        }
    return 0;
}
static int suffix_matrix(void) {
    char original[64] = "";
    for (unsigned i = 0; i < 8; i++) strcat(original, tokens[i]);
    size_t bytes = strlen(original);
    for (unsigned count = 0; count <= 9; count++) {
        char expected[64] = "";
        unsigned kept = count >= 8 ? 0 : 8 - count;
        for (unsigned i = 0; i < kept; i++) strcat(expected, tokens[i]);
        char *memory = malloc(bytes); CHECK(memory);
        struct nk_str text; nk_str_init_fixed(&text, memory, bytes);
        CHECK(nk_str_append_text_char(&text, original, (int)bytes) != 0);
        nk_str_remove_runes(&text, (int)count);
        CHECK(nk_str_len_char(&text) == (int)strlen(expected) && nk_str_len(&text) == (int)kept);
        CHECK(!strlen(expected) || !memcmp(memory, expected, strlen(expected)));
        nk_str_delete_runes(&text, -1, 1);
        nk_str_delete_runes(&text, 100, 1);
        CHECK(nk_str_len_char(&text) == (int)strlen(expected));
        nk_str_free(&text); free(memory);
    }
    return 0;
}
int main(void) {
    CHECK(deletion_matrix() == 0);
    CHECK(suffix_matrix() == 0);
    puts("UTF-8 string bounds: deletion ranges, suffix removal, empty byte/rune counts and invalid ranges passed");
    return 0;
}
