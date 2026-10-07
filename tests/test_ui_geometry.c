#include "test_allocator.h"
#include "ui_geometry.h"
#include <stdio.h>
#include <string.h>

#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "UI geometry %d: %s\n", __LINE__, #x);                                 \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static float width(nk_handle user, float height, const char *text, int length) {
    (void)user;
    (void)text;
    return (float)length * height / 2;
}
static void glyph(nk_handle user, float height, struct nk_user_font_glyph *out, nk_rune rune,
                  nk_rune next) {
    (void)user;
    (void)rune;
    (void)next;
    *out = (struct nk_user_font_glyph){0};
    out->width = height / 2;
    out->height = height;
    out->xadvance = height / 2;
    out->uv[1] = nk_vec2(1, 1);
}
static void drawing(struct nk_context *ctx, const struct nk_user_font *font, unsigned count) {
    nk_clear(ctx);
    nk_input_begin(ctx);
    nk_input_end(ctx);
    if (nk_begin(ctx, "Test", nk_rect(0, 0, 1000, 700), NK_WINDOW_NO_SCROLLBAR)) {
        struct nk_command_buffer *canvas = nk_window_get_canvas(ctx);
        nk_push_scissor(canvas, nk_rect(10, 20, 950, 650));
        for (unsigned i = 0; i < count; i++) {
            float x = (float)(i % 40) * 20, y = (float)(i % 30) * 20;
            nk_fill_rect(canvas, nk_rect(x, y, 14, 18), 4, nk_rgb(200, 70, 40));
            nk_stroke_line(canvas, x, y, x + 20, y + 15, 1, nk_rgb(20, 200, 100));
            if (i % 16 == 0) {
                nk_push_scissor(canvas, nk_rect(0, 0, 1000, 700));
                nk_fill_circle(canvas, nk_rect(x, y, 15, 15), nk_rgb(60, 80, 210));
                nk_draw_text(canvas, nk_rect(x, y, 100, 20), "Reference", 9, font, nk_rgb(0, 0, 0),
                             nk_rgb(255, 255, 255));
            }
        }
    }
    nk_end(ctx);
}
static int compare(struct nk_context *ctx, ps_ui_geometry *g, struct nk_buffer *commands,
                   const struct nk_draw_null_texture *texture) {
    struct nk_buffer reference, vertices, indices;
    nk_buffer_init_default(&reference);
    nk_buffer_init_default(&vertices);
    nk_buffer_init_default(&indices);
    struct nk_convert_config config;
    ps_ui_geometry_config(&config, texture);
    CHECK(nk_convert(ctx, &reference, &vertices, &indices, &config) == NK_CONVERT_SUCCESS);
    unsigned command_count = ctx->draw_list.cmd_count;
    CHECK(ps_ui_geometry_convert(g, ctx, commands, texture) == PS_OK);
    CHECK(command_count == ctx->draw_list.cmd_count);
    CHECK(g->vertices.allocated == vertices.allocated && g->indices.allocated == indices.allocated);
    CHECK(!memcmp(g->vertices.memory.ptr, vertices.memory.ptr, vertices.allocated));
    CHECK(!memcmp(g->indices.memory.ptr, indices.memory.ptr, indices.allocated));
    const struct nk_draw_command *a = nk__draw_begin(ctx, &reference);
    const struct nk_draw_command *b = nk__draw_begin(ctx, commands);
    for (unsigned i = 0; i < command_count; i++) {
        CHECK(a && b && a->elem_count == b->elem_count && a->texture.id == b->texture.id);
        CHECK(!memcmp(&a->clip_rect, &b->clip_rect, sizeof a->clip_rect));
        a = nk__draw_next(a, &reference, ctx);
        b = nk__draw_next(b, commands, ctx);
    }
    CHECK(!a && !b);
    nk_buffer_free(&reference);
    nk_buffer_free(&vertices);
    nk_buffer_free(&indices);
    return 0;
}
static int large_close_button(void) {
    struct nk_user_font font={0};font.height=22;font.width=width;
    struct nk_context ctx;CHECK(nk_init_default(&ctx,&font));
    ps_ui_font_layout layout={.magic=PS_UI_LAYOUT_MAGIC,.ui_size=22};ctx.userdata=nk_handle_ptr(&layout);
    ctx.style.button.padding=nk_vec2(10,4);
    if(nk_begin(&ctx,"close-button",nk_rect(0,0,200,150),0)) {
        nk_layout_row_static(&ctx,26,24,1);
        nk_button_label(&ctx,"x");
    }
    nk_end(&ctx);
    const struct nk_command *command;bool found=false;
    nk_foreach(command,&ctx)if(command->type==NK_COMMAND_TEXT) {
        const struct nk_command_text *text=(const struct nk_command_text *)command;
        if(text->length!=1 || text->w<11 || text->h<22)
            fprintf(stderr,"Close glyph: length=%d width=%u height=%u text=%s\n",text->length,text->w,text->h,text->string);
        CHECK(text->length==1 && text->string[0]=='x' && text->w>=11 && text->h>=22);
        found=true;
    }
    CHECK(found && ctx.style.button.padding.x==10 && ctx.style.button.padding.y==4);
    nk_free(&ctx);return 0;
}
static int field_focus_input(const struct nk_user_font *font) {
    struct nk_context ctx;CHECK(nk_init_default(&ctx,font));
    char text[32]="αβ";struct nk_rect bounds;
    nk_input_begin(&ctx);nk_input_end(&ctx);
    if(nk_begin(&ctx,"Field",nk_rect(0,0,320,100),0)) {
        nk_layout_row_dynamic(&ctx,32,1);bounds=nk_widget_bounds(&ctx);
        nk_edit_focus(&ctx,NK_EDIT_ALWAYS_INSERT_MODE);
        nk_edit_string_zero_terminated(&ctx,NK_EDIT_FIELD,text,sizeof text,nk_filter_default);
    } else {nk_free(&ctx);return 1;}
    nk_end(&ctx);nk_clear(&ctx);
    nk_input_begin(&ctx);nk_input_key(&ctx,NK_KEY_TEXT_END,nk_true);nk_input_unicode(&ctx,'Z');nk_input_end(&ctx);
    ps_ui_flush_edit(&ctx,"Field",text,sizeof text,bounds);
    CHECK(!strcmp(text,"αβZ") && !ctx.input.keyboard.text_len);
    nk_input_begin(&ctx);nk_input_key(&ctx,NK_KEY_TEXT_END,nk_false);nk_input_key(&ctx,NK_KEY_SHIFT,nk_true);nk_input_key(&ctx,NK_KEY_LEFT,nk_true);nk_input_unicode(&ctx,'X');nk_input_end(&ctx);
    ps_ui_flush_edit(&ctx,"Field",text,sizeof text,bounds);
    CHECK(!strcmp(text,"αβX"));
    nk_input_begin(&ctx);nk_input_key(&ctx,NK_KEY_LEFT,nk_false);nk_input_key(&ctx,NK_KEY_SHIFT,nk_false);nk_input_key(&ctx,NK_KEY_BACKSPACE,nk_true);nk_input_end(&ctx);
    ps_ui_flush_edit(&ctx,"Field",text,sizeof text,bounds);
    CHECK(!strcmp(text,"αβ"));
    struct {char text[8];unsigned char guard;} full={"1234567",0xA5};
    nk_input_begin(&ctx);nk_input_key(&ctx,NK_KEY_BACKSPACE,nk_false);nk_input_key(&ctx,NK_KEY_TEXT_END,nk_true);nk_input_unicode(&ctx,'X');nk_input_end(&ctx);
    ps_ui_flush_edit(&ctx,"Field",full.text,sizeof full.text,bounds);
    CHECK(!strcmp(full.text,"1234567") && full.guard==0xA5);
    nk_free(&ctx);return 0;
}
int main(void) {
    CHECK(large_close_button()==0);
    struct nk_user_font font = {0};
    font.height = 18;
    font.width = width;
    font.query = glyph;
    CHECK(field_focus_input(&font)==0);
    font.texture = nk_handle_id(1);
    struct nk_context ctx;
    CHECK(nk_init_default(&ctx, &font));
    struct nk_buffer commands;
    nk_buffer_init_default(&commands);
    struct nk_draw_null_texture texture = {nk_handle_id(1), {0.5f, 0.5f}};
    test_allocator allocator = {0};
    ps_ui_geometry g;
    CHECK(ps_ui_geometry_init(&g, test_domain(&allocator), 0) == PS_OK);
    CHECK(allocator.attempts == 0);
    CHECK(compare(&ctx, &g, &commands, &texture) == 0);
    CHECK(!g.vertices.allocated && !g.indices.allocated);
    const unsigned counts[] = {0, 1, 500, 3, 16000, 0, 500};
    for (unsigned i = 0; i < sizeof counts / sizeof *counts; i++) {
        drawing(&ctx, &font, counts[i]);
        CHECK(compare(&ctx, &g, &commands, &texture) == 0);
        if (counts[i] == 16000)
            CHECK(g.vertices.allocated / sizeof(ps_ui_vertex) > 65535);
        size_t attempts = allocator.attempts;
        for (unsigned frame = 0; frame < 20; frame++)
            CHECK(ps_ui_geometry_convert(&g, &ctx, &commands, &texture) == PS_OK);
        CHECK(allocator.attempts == attempts);
    }
    CHECK(ps_ui_geometry_convert(NULL, &ctx, &commands, &texture) == PS_INVALID);
    CHECK(ps_ui_geometry_convert(&g, NULL, &commands, &texture) == PS_INVALID);
    CHECK(ps_ui_geometry_convert(&g, &ctx, NULL, &texture) == PS_INVALID);
    CHECK(ps_ui_geometry_convert(&g, &ctx, &commands, NULL) == PS_INVALID);
    ps_ui_geometry_destroy(&g);
    CHECK(!allocator.live_blocks && !allocator.invalid);
    ps_ui_geometry_destroy(&g);

    /* Exhaust the budget, then reuse the same owner for a smaller frame. */
    CHECK(ps_ui_geometry_init(&g, test_domain(&allocator), 8192) == PS_OK);
    drawing(&ctx, &font, 500);
    CHECK(ps_ui_geometry_convert(&g, &ctx, &commands, &texture) == PS_LIMIT);
    drawing(&ctx, &font, 0);
    CHECK(compare(&ctx, &g, &commands, &texture) == 0);
    ps_ui_geometry_destroy(&g);
    CHECK(!allocator.live_blocks && !allocator.invalid);

    /* Fail each actual allocation, prove recovery and byte-exact deallocation. */
    drawing(&ctx, &font, 500);
    allocator = (test_allocator){0};
    CHECK(ps_ui_geometry_init(&g, test_domain(&allocator), 0) == PS_OK);
    CHECK(ps_ui_geometry_convert(&g, &ctx, &commands, &texture) == PS_OK);
    size_t allocations = allocator.attempts;
    ps_ui_geometry_destroy(&g);
    for (size_t fail = 1; fail <= allocations; fail++) {
        allocator = (test_allocator){0};
        allocator.fail_on = fail;
        CHECK(ps_ui_geometry_init(&g, test_domain(&allocator), 0) == PS_OK);
        CHECK(ps_ui_geometry_convert(&g, &ctx, &commands, &texture) == PS_MEMORY);
        allocator.fail_on = 0;
        CHECK(compare(&ctx, &g, &commands, &texture) == 0);
        ps_ui_geometry_destroy(&g);
        CHECK(!allocator.live_blocks && !allocator.invalid);
    }
    nk_buffer_free(&commands);
    nk_free(&ctx);
    printf("UI geometry: identical commands, vertices, indices; stable allocations; %zu failure "
           "points\n",
           allocations);
    return 0;
}
