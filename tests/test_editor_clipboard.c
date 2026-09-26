#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Clipboard line %d: %s\n", __LINE__, #x);                              \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static bool denied;
static void *allocate(nk_handle user, void *old, nk_size size) {
    (void)user;
    (void)old;
    return denied ? NULL : malloc(size);
}
static void release(nk_handle user, void *memory) {
    (void)user;
    free(memory);
}
static bool equals(struct nk_text_edit *edit, const char *text) {
    return nk_str_len_char(&edit->string) == (int)strlen(text) &&
           !memcmp(nk_str_get_const(&edit->string), text, strlen(text));
}
int main(void) {
    char buffer[64];
    struct nk_text_edit edit;
    nk_textedit_init_fixed(&edit, buffer, sizeof buffer);
    edit.mode = NK_TEXT_EDIT_MODE_INSERT;
    /* Padding makes the old glyph-count implementation fail without reading out of bounds. */
    const char text[32] = "\xc3\xa4\xe2\x82\xac";
    CHECK(nk_textedit_paste(&edit, text, 5));
    CHECK(equals(&edit, text) && edit.cursor == 2);
    nk_textedit_undo(&edit);
    CHECK(equals(&edit, "") && edit.cursor == 0);
    nk_textedit_redo(&edit);
    CHECK(equals(&edit, text) && edit.cursor == 2);
    edit.select_start = 0;
    edit.select_end = 2;
    CHECK(nk_textedit_paste(&edit, "A\xf0\x9f\x98\x80\nB", 7));
    CHECK(equals(&edit, "A\xf0\x9f\x98\x80\nB") && edit.cursor == 4);
    nk_textedit_undo(&edit);
    CHECK(equals(&edit, text));
    nk_textedit_redo(&edit);
    CHECK(equals(&edit, "A\xf0\x9f\x98\x80\nB"));
    edit.select_start = 1;
    edit.select_end = 2;
    struct nk_text_edit before = edit;
    char saved[64];
    memcpy(saved, buffer, sizeof buffer);
    char large[80];
    memset(large, 'x', sizeof large);
    CHECK(!nk_textedit_paste(&edit, large, sizeof large));
    CHECK(!memcmp(&edit, &before, sizeof edit) && !memcmp(buffer, saved, sizeof buffer));
    CHECK(!nk_textedit_paste(&edit, "\xc3", 1));
    CHECK(!nk_textedit_paste(&edit, "\xc0\xaf", 2));
    CHECK(!nk_textedit_paste(&edit, "", 0));
    CHECK(!memcmp(&edit, &before, sizeof edit) && !memcmp(buffer, saved, sizeof buffer));
    /* A replacement may fit even though appending would not. */
    char small[8];
    nk_textedit_init_fixed(&edit, small, sizeof small);
    edit.mode = NK_TEXT_EDIT_MODE_INSERT;
    CHECK(nk_textedit_paste(&edit, "abcdefg", 7));
    edit.select_start = 1;
    edit.select_end = 6;
    CHECK(nk_textedit_paste(&edit, "\xc3\xa4", 2));
    CHECK(equals(&edit, "a\xc3\xa4g") && edit.cursor == 2);
    /* A failed dynamic reserve cannot delete selection or lose the original allocation. */
    struct nk_allocator allocator = {0};
    allocator.alloc = allocate;
    allocator.free = release;
    nk_textedit_init(&edit, &allocator, 8);
    edit.mode = NK_TEXT_EDIT_MODE_INSERT;
    CHECK(nk_textedit_paste(&edit, "abcdefg", 7));
    edit.select_start = 1;
    edit.select_end = 3;
    before = edit;
    denied = true;
    CHECK(!nk_textedit_paste(&edit, large, sizeof large));
    CHECK(!memcmp(&edit, &before, sizeof edit) && equals(&edit, "abcdefg"));
    denied = false;
    CHECK(nk_textedit_paste(&edit, large, sizeof large));
    CHECK(edit.string.len == 85 && edit.cursor == 81);
    nk_textedit_undo(&edit);
    CHECK(equals(&edit, "abcdefg"));
    nk_textedit_free(&edit);
    puts("Editor clipboard: UTF-8, cursor, undo/redo, replacement and allocation failure passed");
    return 0;
}
