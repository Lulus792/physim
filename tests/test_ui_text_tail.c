#include "ui.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "text tail %d: %s\n", __LINE__, #x);                                   \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static float width(nk_handle user, float height, const char *text, int length) {
    (void)user;
    (void)height;
    (void)text;
    return (float)length;
}
int main(void) {
    struct nk_user_font font = {0};
    font.height = 16;
    font.width = width;
    struct nk_context ctx;
    CHECK(nk_init_default(&ctx, &font));
    char text[1501];
    for (size_t i = 0; i < 300; i++)
        memcpy(text + 5 * i, "A🌍", 5);
    text[1500] = 0;
    if (nk_begin(&ctx, "tail", nk_rect(0, 0, 2000, 100), 0)) {
        nk_draw_text(nk_window_get_canvas(&ctx), nk_rect(0, 0, 1800, 24), text, 1500, &font,
                     nk_rgba(0, 0, 0, 0), nk_rgba(255, 255, 255, 255));
    }
    nk_end(&ctx);
    const struct nk_command *command;
    bool found = false;
    nk_foreach(command, &ctx) if (command->type == NK_COMMAND_TEXT) {
        const struct nk_command_text *draw = (const struct nk_command_text *)command;
        if (draw->length == 1500) {
            CHECK(!memcmp(draw->string, text, 1500) && draw->string[1500] == 0);
            found = true;
        }
    }
    CHECK(found);
    nk_free(&ctx);
    puts("UI text tail: 1500-byte rendered UTF-8 command, complete copy and trailing NUL passed");
    return 0;
}
