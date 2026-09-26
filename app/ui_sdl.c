/* SDL input/clipboard routines adapted from Nuklear's SDL3 demo.
 * Copyright (c) 2017 Micha Mettke. See third_party/Nuklear-LICENSE (MIT option).
 * Physim supplies the OpenGL renderer and font lifecycle. */
#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#include <dwmapi.h>
#endif
/* Keep native resizing, snapping and caption buttons, but blend the caption
 * into the same canvas as the renderer. Unsupported attributes are harmless
 * on Windows 10, where the dark-caption fallback remains available. */
static void window_chrome(SDL_Window *window) {
#ifdef _WIN32
    HWND hwnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(window),
                                             SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
    if (hwnd) {
        BOOL dark = TRUE;
        COLORREF background = RGB(20, 21, 24), text = RGB(237, 238, 242);
        DwmSetWindowAttribute(hwnd, 20 /* immersive dark mode */, &dark, sizeof dark);
        DwmSetWindowAttribute(hwnd, 34 /* border color */, &background, sizeof background);
        DwmSetWindowAttribute(hwnd, 35 /* caption color */, &background, sizeof background);
        DwmSetWindowAttribute(hwnd, 36 /* text color */, &text, sizeof text);
    }
#else
    (void)window;
#endif
}
struct nk_sdl {
    SDL_Window *win;
    ps_graphics *graphics;
    struct nk_context ctx;
    struct nk_font_atlas atlas;
    struct nk_buffer commands;
    struct nk_draw_null_texture null_texture;
    struct nk_allocator allocator;
    unsigned font_texture;
    Uint64 last_render;
    bool insert_toggle;
};
static void *ui_alloc(nk_handle user, void *old, nk_size size) {
    (void)user;
    (void)old;
    return malloc(size);
}
static void ui_free(nk_handle user, void *memory) {
    (void)user;
    free(memory);
}
static void nk_sdl_clipboard_paste(nk_handle usr, struct nk_text_edit *edit);
static void nk_sdl_clipboard_copy(nk_handle usr, const char *text, int len);
struct nk_context *nk_sdl_init(SDL_Window *window, ps_graphics *graphics) {
    window_chrome(window);
    struct nk_sdl *sdl = calloc(1, sizeof *sdl);
    if (!sdl)
        return NULL;
    sdl->win = window;
    sdl->graphics = graphics;
    sdl->last_render = SDL_GetTicksNS();
    sdl->allocator.alloc = ui_alloc;
    sdl->allocator.free = ui_free;
    if (!nk_init(&sdl->ctx, &sdl->allocator, NULL)) {
        free(sdl);
        return NULL;
    }
    sdl->ctx.userdata = nk_handle_ptr(sdl);
    sdl->ctx.clip.copy = nk_sdl_clipboard_copy;
    sdl->ctx.clip.paste = nk_sdl_clipboard_paste;
    sdl->ctx.clip.userdata = nk_handle_ptr(sdl);
    nk_buffer_init_default(&sdl->commands);
    return &sdl->ctx;
}
struct nk_font_atlas *nk_sdl_font_stash_begin(struct nk_context *ctx) {
    struct nk_sdl *sdl = ctx->userdata.ptr;
    nk_font_atlas_init_default(&sdl->atlas);
    nk_font_atlas_begin(&sdl->atlas);
    return &sdl->atlas;
}
bool nk_sdl_font_stash_end(struct nk_context *ctx) {
    struct nk_sdl *sdl = ctx->userdata.ptr;
    int w, h;
    const void *pixels = nk_font_atlas_bake(&sdl->atlas, &w, &h, NK_FONT_ATLAS_RGBA32);
    if (!pixels)
        return false;
    sdl->font_texture = ps_graphics_texture(sdl->graphics, pixels, w, h);
    if (!sdl->font_texture)
        return false;
    nk_font_atlas_end(&sdl->atlas, nk_handle_id((int)sdl->font_texture), &sdl->null_texture);
    return true;
}
bool nk_sdl_render(struct nk_context *ctx) {
    struct nk_sdl *sdl = ctx->userdata.ptr;
    Uint64 now = SDL_GetTicksNS();
    ctx->delta_time_seconds = (float)(now - sdl->last_render) / (float)SDL_NS_PER_SECOND;
    sdl->last_render = now;
    bool ok = ps_graphics_ui(sdl->graphics, ctx, &sdl->commands, &sdl->null_texture);
    nk_clear(ctx);
    nk_buffer_clear(&sdl->commands);
    return ok;
}
void nk_sdl_update_TextInput(struct nk_context *ctx) {
    struct nk_sdl *sdl = ctx->userdata.ptr;
    bool active = ctx->active && (ctx->active->popup.win ? ctx->active->popup.win->edit.active
                                                         : ctx->active->edit.active);
    if (active && !SDL_TextInputActive(sdl->win))
        SDL_StartTextInput(sdl->win);
    if (!active && SDL_TextInputActive(sdl->win))
        SDL_StopTextInput(sdl->win);
}
ps_result nk_sdl_export_png(struct nk_context *ctx, struct nk_context *drawing, int width,
                            int height, const char *path) {
    return nk_sdl_export_png_scaled(ctx, drawing, width, height, 2, path);
}
ps_result nk_sdl_export_png_scaled(struct nk_context *ctx, struct nk_context *drawing, int width,
                                   int height, unsigned scale, const char *path) {
    if (!ctx || !ctx->userdata.ptr) return PS_INVALID;
    struct nk_sdl *sdl = ctx->userdata.ptr;
    return ps_graphics_ui_png_scaled(sdl->graphics, drawing, &sdl->null_texture, width, height, scale, path);
}
void nk_sdl_shutdown(struct nk_context *ctx) {
    if (!ctx)
        return;
    struct nk_sdl *sdl = ctx->userdata.ptr;
    ps_graphics_delete_texture(sdl->graphics, sdl->font_texture);
    if (sdl->atlas.temporary.alloc)
        nk_font_atlas_clear(&sdl->atlas);
    nk_buffer_free(&sdl->commands);
    nk_free(ctx);
    free(sdl);
}
static bool nk_sdl_paste_text(struct nk_text_edit *edit, const char *text) {
    /* The vendored paste patch takes bytes and preserves scalar-based undo. */
    size_t bytes = strlen(text);
    return bytes && bytes <= 262144 && nk_textedit_paste(edit, text, (int)bytes);
}
static void nk_sdl_clipboard_paste(nk_handle usr, struct nk_text_edit *edit) {
    char *text;
    (void)usr;

    text = SDL_GetClipboardText();
    if (!text)
        return;

    nk_sdl_paste_text(edit, text);
    SDL_free(text);
}

static void nk_sdl_clipboard_copy(nk_handle usr, const char *text, int len) {
    const char *ptext;
    char *str;
    size_t buflen;
    int i;
    struct nk_sdl *sdl = (struct nk_sdl *)usr.ptr;
    SDL_assert(sdl);
    if (len <= 0 || text == NULL)
        return;

    /* This snapshot's copy callback supplies a Unicode scalar count; SDL needs
     * the corresponding UTF-8 bytes. Paste has a separate, patched byte contract. */
    ptext = text;
    for (i = len; i > 0; i--)
        (void)SDL_StepUTF8(&ptext, NULL);
    buflen = (size_t)(ptext - text) + 1;

    str = (char *)sdl->allocator.alloc(sdl->allocator.userdata, 0, buflen);
    if (!str)
        return;
    SDL_strlcpy(str, text, buflen);
    SDL_SetClipboardText(str);
    sdl->allocator.free(sdl->allocator.userdata, str);
}

NK_API int nk_sdl_handle_event(struct nk_context *ctx, SDL_Event *evt) {
    struct nk_sdl *sdl;

    SDL_assert(ctx);
    SDL_assert(evt);

    sdl = (struct nk_sdl *)ctx->userdata.ptr;
    SDL_assert(sdl);

    /* We only care about Window currently used by Nuklear */
    if (sdl->win != SDL_GetWindowFromEvent(evt)) {
        return 0;
    }

    switch (evt->type) {
    case SDL_EVENT_WINDOW_FOCUS_LOST:
        for (int key = 0; key < NK_KEY_MAX; key++)
            nk_input_key(ctx, (enum nk_keys)key, false);
        for (int button = 0; button < NK_BUTTON_MAX; button++)
            nk_input_button(ctx, (enum nk_buttons)button, 0, 0, false);
        return 1;
    case SDL_EVENT_KEY_UP: /* KEYUP & KEYDOWN share same routine */
    case SDL_EVENT_KEY_DOWN: {
        int down = evt->type == SDL_EVENT_KEY_DOWN;
        int ctrl_down = evt->key.mod & SDL_KMOD_CTRL;

        switch (evt->key.key) {
        case SDLK_LALT:
        case SDLK_RALT:
            nk_input_key(ctx, NK_KEY_ALT, down);
            break;
        case SDLK_RSHIFT: /* RSHIFT & LSHIFT share same routine */
        case SDLK_LSHIFT:
            nk_input_key(ctx, NK_KEY_SHIFT, down);
            break;
        case SDLK_DELETE:
            nk_input_key(ctx, NK_KEY_DEL, down);
            break;
        case SDLK_KP_ENTER:
        case SDLK_RETURN:
            nk_input_key(ctx, NK_KEY_ENTER, down);
            break;
        case SDLK_TAB:
            nk_input_key(ctx, NK_KEY_TAB, down);
            break;
        case SDLK_BACKSPACE:
            nk_input_key(ctx, NK_KEY_BACKSPACE, down);
            break;
        case SDLK_HOME:
            nk_input_key(ctx, NK_KEY_TEXT_START, down);
            nk_input_key(ctx, NK_KEY_SCROLL_START, down);
            break;
        case SDLK_END:
            nk_input_key(ctx, NK_KEY_TEXT_END, down);
            nk_input_key(ctx, NK_KEY_SCROLL_END, down);
            break;
        case SDLK_PAGEDOWN:
            nk_input_key(ctx, NK_KEY_SCROLL_DOWN, down);
            break;
        case SDLK_PAGEUP:
            nk_input_key(ctx, NK_KEY_SCROLL_UP, down);
            break;
        case SDLK_F1:
            nk_input_key(ctx, NK_KEY_F1, down);
            break;
        case SDLK_F2:
            nk_input_key(ctx, NK_KEY_F2, down);
            break;
        case SDLK_F3:
            nk_input_key(ctx, NK_KEY_F3, down);
            break;
        case SDLK_F4:
            nk_input_key(ctx, NK_KEY_F4, down);
            break;
        case SDLK_F5:
            nk_input_key(ctx, NK_KEY_F5, down);
            break;
        case SDLK_F6:
            nk_input_key(ctx, NK_KEY_F6, down);
            break;
        case SDLK_F7:
            nk_input_key(ctx, NK_KEY_F7, down);
            break;
        case SDLK_F8:
            nk_input_key(ctx, NK_KEY_F8, down);
            break;
        case SDLK_F9:
            nk_input_key(ctx, NK_KEY_F9, down);
            break;
        case SDLK_F10:
            nk_input_key(ctx, NK_KEY_F10, down);
            break;
        case SDLK_F11:
            nk_input_key(ctx, NK_KEY_F11, down);
            break;
        case SDLK_F12:
            nk_input_key(ctx, NK_KEY_F12, down);
            break;
        case SDLK_A:
            nk_input_key(ctx, NK_KEY_TEXT_SELECT_ALL, down && ctrl_down);
            break;
        case SDLK_Z:
            nk_input_key(ctx, NK_KEY_TEXT_UNDO, down && ctrl_down);
            break;
        case SDLK_R:
            nk_input_key(ctx, NK_KEY_TEXT_REDO, down && ctrl_down);
            break;
        case SDLK_C:
            nk_input_key(ctx, NK_KEY_COPY, down && ctrl_down);
            break;
        case SDLK_V:
            nk_input_key(ctx, NK_KEY_PASTE, down && ctrl_down);
            break;
        case SDLK_X:
            nk_input_key(ctx, NK_KEY_CUT, down && ctrl_down);
            break;
        case SDLK_B:
            nk_input_key(ctx, NK_KEY_TEXT_LINE_START, down && ctrl_down);
            break;
        case SDLK_E:
            nk_input_key(ctx, NK_KEY_TEXT_LINE_END, down && ctrl_down);
            break;
        case SDLK_UP:
            nk_input_key(ctx, NK_KEY_UP, down);
            break;
        case SDLK_DOWN:
            nk_input_key(ctx, NK_KEY_DOWN, down);
            break;
        case SDLK_ESCAPE:
            nk_input_key(ctx, NK_KEY_TEXT_RESET_MODE, down);
            break;
        case SDLK_INSERT:
            if (down)
                sdl->insert_toggle = !sdl->insert_toggle;
            if (sdl->insert_toggle) {
                nk_input_key(ctx, NK_KEY_TEXT_INSERT_MODE, down);
            } else {
                nk_input_key(ctx, NK_KEY_TEXT_REPLACE_MODE, down);
            }
            break;
        case SDLK_LEFT:
            if (ctrl_down)
                nk_input_key(ctx, NK_KEY_TEXT_WORD_LEFT, down);
            else
                nk_input_key(ctx, NK_KEY_LEFT, down);
            break;
        case SDLK_RIGHT:
            if (ctrl_down)
                nk_input_key(ctx, NK_KEY_TEXT_WORD_RIGHT, down);
            else
                nk_input_key(ctx, NK_KEY_RIGHT, down);
            break;
        default:
            return 0;
        }
        return 1;
    }

    case SDL_EVENT_MOUSE_BUTTON_UP: /* MOUSEBUTTONUP & MOUSEBUTTONDOWN share same routine */
    case SDL_EVENT_MOUSE_BUTTON_DOWN: {
        const int x = (int)evt->button.x, y = (int)evt->button.y;
        const int down = evt->button.down;
        switch (evt->button.button) {
        case SDL_BUTTON_LEFT:
            if (evt->button.clicks > 1)
                nk_input_button(ctx, NK_BUTTON_DOUBLE, x, y, down);
            nk_input_button(ctx, NK_BUTTON_LEFT, x, y, down);
            break;
        case SDL_BUTTON_MIDDLE:
            nk_input_button(ctx, NK_BUTTON_MIDDLE, x, y, down);
            break;
        case SDL_BUTTON_RIGHT:
            nk_input_button(ctx, NK_BUTTON_RIGHT, x, y, down);
            break;
        case SDL_BUTTON_X1:
            nk_input_button(ctx, NK_BUTTON_X1, x, y, down);
            break;
        case SDL_BUTTON_X2:
            nk_input_button(ctx, NK_BUTTON_X2, x, y, down);
            break;
        default:
            return 0;
        }
    }
        return 1;

    case SDL_EVENT_MOUSE_MOTION:
        ctx->input.mouse.pos.x = evt->motion.x;
        ctx->input.mouse.pos.y = evt->motion.y;
        ctx->input.mouse.delta.x = ctx->input.mouse.pos.x - ctx->input.mouse.prev.x;
        ctx->input.mouse.delta.y = ctx->input.mouse.pos.y - ctx->input.mouse.prev.y;
        return 1;

    case SDL_EVENT_TEXT_INPUT: {
        const char *text = evt->text.text;
        int remaining = (int)SDL_strlen(text);
        while (remaining > 0) {
            nk_rune codepoint;
            int size = nk_utf_decode(text, &codepoint, remaining);
            if (size <= 0)
                break;
            nk_input_unicode(ctx, codepoint);
            text += size;
            remaining -= size;
        }
    }
        return 1;

    case SDL_EVENT_MOUSE_WHEEL: {
        float direction = evt->wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1.f : 1.f;
        nk_input_scroll(ctx, nk_vec2(direction * evt->wheel.x, direction * evt->wheel.y));
    }
        return 1;
    }
    return 0;
}

bool nk_sdl_test_input(SDL_Window *window, ps_graphics *graphics) {
    struct nk_context *ctx = nk_sdl_init(window, graphics);
    if (!ctx)
        return false;
    SDL_WindowID id = SDL_GetWindowID(window);
    const char text[] = "A\xc3\xa4\xe2\x82\xac";
    SDL_Event event = {0};
    nk_input_begin(ctx);
    event.type = SDL_EVENT_TEXT_INPUT;
    event.text.windowID = id;
    event.text.text = text;
    nk_sdl_handle_event(ctx, &event);
    bool ok = ctx->input.keyboard.text_len == sizeof(text) - 1 &&
              !memcmp(ctx->input.keyboard.text, text, sizeof(text) - 1);
    event.type = SDL_EVENT_KEY_DOWN;
    event.key.windowID = id;
    event.key.key = SDLK_LSHIFT;
    event.key.down = true;
    nk_sdl_handle_event(ctx, &event);
    ok = ok && ctx->input.keyboard.keys[NK_KEY_SHIFT].down;
    event.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
    event.button.windowID = id;
    event.button.button = SDL_BUTTON_RIGHT;
    event.button.down = true;
    event.button.x = 100;
    event.button.y = 100;
    nk_sdl_handle_event(ctx, &event);
    ok = ok && ctx->input.mouse.buttons[NK_BUTTON_RIGHT].down;
    event.type = SDL_EVENT_WINDOW_FOCUS_LOST;
    event.window.windowID = id;
    nk_sdl_handle_event(ctx, &event);
    ok = ok && !ctx->input.keyboard.keys[NK_KEY_SHIFT].down &&
         !ctx->input.mouse.buttons[NK_BUTTON_RIGHT].down;
    nk_input_end(ctx);
    char buffer[32];
    struct nk_text_edit edit;
    nk_textedit_init_fixed(&edit, buffer, sizeof buffer);
    edit.mode = NK_TEXT_EDIT_MODE_INSERT;
    ok = ok && nk_sdl_paste_text(&edit, text) && edit.cursor == 3 &&
         nk_str_len_char(&edit.string) == sizeof(text) - 1 &&
         !memcmp(nk_str_get_const(&edit.string), text, sizeof(text) - 1);
    nk_textedit_undo(&edit);
    ok = ok && nk_str_len_char(&edit.string) == 0 && edit.cursor == 0;
    nk_textedit_redo(&edit);
    ok = ok && nk_str_len_char(&edit.string) == sizeof(text) - 1 && edit.cursor == 3;
    char *oversized = malloc(262146);
    if (oversized) {
        memset(oversized, 'x', 262145);
        oversized[262145] = 0;
        edit.select_start = 0;
        edit.select_end = 3;
        ok = ok && !nk_sdl_paste_text(&edit, oversized) && edit.select_end == 3 &&
             nk_str_len_char(&edit.string) == sizeof(text) - 1;
        free(oversized);
    } else
        ok = false;
    nk_sdl_shutdown(ctx);
    if (!ok)
        SDL_SetError("SDL input test: Unicode, clipboard or focus reset failed");
    else
        puts("INPUT TEST: UTF-8 input/paste, undo/redo, clipboard limit, keys, focus loss PASSED");
    return ok;
}
