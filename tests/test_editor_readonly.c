#include "ui.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "Readonly editor line %d: %s\n", __LINE__, #x); return 1; } } while (0)
typedef struct { char text[1024]; unsigned paste_calls; } clipboard;
static float width(nk_handle user, float height, const char *text, int length) {
    (void)user; (void)text; return height * .5f * (float)length;
}
static void copy(nk_handle user, const char *text, int length) {
    clipboard *value = user.ptr;
    if (length < 0 || length >= (int)sizeof value->text) return;
    memcpy(value->text, text, (size_t)length); value->text[length] = 0;
}
static void paste(nk_handle user, struct nk_text_edit *edit) {
    clipboard *value = user.ptr; value->paste_calls++;
    (void)nk_textedit_paste(edit, "ATTEMPTED PASTE", 15);
}
static void frame(struct nk_context *ctx, struct nk_text_edit *edit, int mouse, int key, bool text, bool disabled) {
    nk_input_begin(ctx);
    for (int i = 0; i < NK_KEY_MAX; i++) nk_input_key(ctx, (enum nk_keys)i, nk_false);
    nk_input_motion(ctx, 30, 40);
    if (mouse >= 0) nk_input_button(ctx, NK_BUTTON_LEFT, 30, 40, mouse != 0);
    if (key >= 0) nk_input_key(ctx, (enum nk_keys)key, nk_true);
    if (text) nk_input_unicode(ctx, 'X');
    nk_input_end(ctx);
    if (nk_begin(ctx, "readonly", nk_rect(0,0,400,180), 0)) {
        nk_layout_row_dynamic(ctx, 130, 1);
        if (disabled) nk_widget_disable_begin(ctx);
        nk_edit_buffer(ctx, NK_EDIT_EDITOR | NK_EDIT_READ_ONLY | NK_EDIT_PRESERVE_UNDO, edit, nk_filter_default);
        if (disabled) nk_widget_disable_end(ctx);
    }
    nk_end(ctx); nk_clear(ctx);
}
int main(void) {
    struct nk_user_font font = {.height = 12, .width = width};
    struct nk_context ctx; CHECK(nk_init_default(&ctx, &font));
    clipboard value = {0}; ctx.clip.userdata = nk_handle_ptr(&value); ctx.clip.copy = copy; ctx.clip.paste = paste;
    struct nk_text_edit edit; nk_textedit_init_default(&edit); edit.mode = NK_TEXT_EDIT_MODE_INSERT;
    char expected[1024] = "Undoable prefix\n";
    CHECK(nk_textedit_paste(&edit, expected, (int)strlen(expected)));
    for (unsigned i = 0; i < 40; i++) { char line[24]; snprintf(line, sizeof line, "line %02u\n", i); strcat(expected, line); CHECK(nk_textedit_paste(&edit, line, (int)strlen(line))); }
    frame(&ctx, &edit, -1, -1, false, false);
    frame(&ctx, &edit, 1, -1, false, false); frame(&ctx, &edit, 0, -1, false, false);
    CHECK(edit.active);
    frame(&ctx, &edit, -1, NK_KEY_TEXT_SELECT_ALL, false, false);
    CHECK(edit.select_start == 0 && edit.select_end == nk_str_len(&edit.string));
    frame(&ctx, &edit, -1, NK_KEY_COPY, false, false); CHECK(!strcmp(value.text, expected));
    const enum nk_keys mutations[] = {NK_KEY_DEL, NK_KEY_BACKSPACE, NK_KEY_CUT, NK_KEY_PASTE,
        NK_KEY_TEXT_UNDO, NK_KEY_TEXT_REDO, NK_KEY_TEXT_INSERT_MODE, NK_KEY_TEXT_REPLACE_MODE, NK_KEY_ENTER, NK_KEY_TAB};
    for (unsigned i = 0; i < sizeof mutations / sizeof *mutations; i++) {
        frame(&ctx, &edit, -1, mutations[i], true, false);
        CHECK(nk_str_len_char(&edit.string) == (int)strlen(expected) && !memcmp(nk_str_get_const(&edit.string), expected, strlen(expected)));
        CHECK(edit.mode == NK_TEXT_EDIT_MODE_VIEW && !value.paste_calls);
    }
    frame(&ctx, &edit, -1, NK_KEY_TEXT_END, false, false);
    CHECK(edit.cursor == nk_str_len(&edit.string) && edit.scrollbar.y > 200);
    frame(&ctx, &edit, -1, NK_KEY_SCROLL_END, false, false);
    CHECK(!ctx.input.keyboard.keys[NK_KEY_SCROLL_END].clicked && !ctx.input.keyboard.keys[NK_KEY_SCROLL_END].down);
    edit.cursor = edit.select_start = edit.select_end = 0;
    frame(&ctx, &edit, -1, NK_KEY_TEXT_END, true, true);
    CHECK(edit.cursor == 0 && !memcmp(nk_str_get_const(&edit.string), expected, strlen(expected)));
    nk_textedit_free(&edit); nk_free(&ctx);
    puts("Readonly editor: focus, selection/copy, rejected mutations, target-row scrolling and disabled input passed");
    return 0;
}
