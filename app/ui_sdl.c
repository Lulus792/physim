/* SDL input/clipboard routines adapted from Nuklear's SDL3 demo.
 * Copyright (c) 2017 Micha Mettke. See third_party/Nuklear-LICENSE (MIT option).
 * Physim supplies the OpenGL renderer and font lifecycle. */
#include "ui.h"
#include "accessibility_native.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#include <dwmapi.h>
#endif
/* Ask the compositor for rounded outer corners, including the integrated main
 * window header. Windows keeps maximized/snapped windows flush with the screen. */
static void window_chrome(SDL_Window *window) {
#ifdef _WIN32
    HWND hwnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(window),
                                             SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
    if (hwnd) {
        BOOL dark = TRUE;
        DWORD corners = 2; /* DWMWCP_ROUND; ignored on Windows versions before 11. */
        COLORREF background = RGB(20, 21, 24), text = RGB(237, 238, 242);
        DwmSetWindowAttribute(hwnd, 20 /* immersive dark mode */, &dark, sizeof dark);
        DwmSetWindowAttribute(hwnd, 33 /* corner preference */, &corners, sizeof corners);
        DwmSetWindowAttribute(hwnd, 34 /* border color */, &background, sizeof background);
        DwmSetWindowAttribute(hwnd, 35 /* caption color */, &background, sizeof background);
        DwmSetWindowAttribute(hwnd, 36 /* text color */, &text, sizeof text);
    }
#else
    (void)window;
#endif
}
struct nk_sdl {
    ps_ui_font_layout font_layout; /* First: shared layout lookup without SDL linkage. */
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
    ps_a11y_model *accessibility;
    SDL_Mutex *accessibility_mutex;
    ps_a11y_native *accessibility_native;
};
static bool accessibility_widget(void *user,const char *window,const char *label,
                                 int role,const float bounds[4],bool enabled) {
    struct nk_sdl *sdl=user;if(!sdl->accessibility)return false;
    SDL_LockMutex(sdl->accessibility_mutex);
    bool pressed=ps_a11y_record(sdl->accessibility,window,label,(ps_a11y_role)role,bounds,enabled);
    SDL_UnlockMutex(sdl->accessibility_mutex);return pressed;
}
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
    sdl->font_layout=(ps_ui_font_layout){.magic=PS_UI_LAYOUT_MAGIC,.ui_size=16};
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
    sdl->accessibility=malloc(sizeof *sdl->accessibility);
    sdl->accessibility_mutex=SDL_CreateMutex();
    if(sdl->accessibility && sdl->accessibility_mutex) {
        ps_a11y_init(sdl->accessibility);ps_a11y_begin(sdl->accessibility);
        sdl->accessibility_native=ps_a11y_native_create(window,sdl->accessibility,sdl->accessibility_mutex);
    }
    if(sdl->accessibility_native) {
        sdl->font_layout.accessibility=accessibility_widget;
        sdl->font_layout.accessibility_user=sdl;
    } else {
        free(sdl->accessibility);sdl->accessibility=NULL;
        if(sdl->accessibility_mutex)SDL_DestroyMutex(sdl->accessibility_mutex);
        sdl->accessibility_mutex=NULL;
    }
    return &sdl->ctx;
}
bool nk_sdl_accessibility_available(struct nk_context *ctx) {
    return ctx && ctx->userdata.ptr && ((struct nk_sdl *)ctx->userdata.ptr)->accessibility_native;
}
bool nk_sdl_accessibility_press(struct nk_context *ctx,const char *label) {
    if(!ctx || !ctx->userdata.ptr)return false;
    struct nk_sdl *sdl=ctx->userdata.ptr;
    return ps_a11y_native_press_label(sdl->accessibility_native,label);
}
void nk_sdl_set_ui_size(struct nk_context *ctx,unsigned size) {
    if(ctx && ctx->userdata.ptr && size>=16 && size<=22 && size%2==0)
        ((struct nk_sdl *)ctx->userdata.ptr)->font_layout.ui_size=size;
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
    if(sdl->accessibility) {
        SDL_LockMutex(sdl->accessibility_mutex);
        bool changed=ps_a11y_publish(sdl->accessibility);
        SDL_UnlockMutex(sdl->accessibility_mutex);
        ps_a11y_native_publish(sdl->accessibility_native,changed);
        SDL_LockMutex(sdl->accessibility_mutex);ps_a11y_begin(sdl->accessibility);
        SDL_UnlockMutex(sdl->accessibility_mutex);
    }
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
    ps_a11y_native_destroy(sdl->accessibility_native);
    free(sdl->accessibility);
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

/* Keep platform conventions here so editor and other text fields agree. The
 * platform argument also lets the input regression exercise both mappings. */
static bool nk_sdl_edit_key(struct nk_context *ctx, SDL_Keycode key, SDL_Keymod mod, bool down,
                            bool mac) {
    bool command = (mod & (mac ? SDL_KMOD_GUI : SDL_KMOD_CTRL)) != 0;
    bool word = (mod & (mac ? SDL_KMOD_ALT : SDL_KMOD_CTRL)) != 0;
    bool shift = (mod & SDL_KMOD_SHIFT) != 0;
    enum nk_keys keys[3], selected = NK_KEY_NONE;
    unsigned count = 0;
    switch (key) {
    case SDLK_Z:
        keys[count++] = NK_KEY_TEXT_UNDO;
        keys[count++] = NK_KEY_TEXT_REDO;
        if (command)
            selected = shift ? NK_KEY_TEXT_REDO : NK_KEY_TEXT_UNDO;
        break;
    case SDLK_Y:
        if (mac)
            return false;
        /* Ctrl+Y and Ctrl+Shift+Z both repeat the undone edit. */
        /* fall through */
    case SDLK_R:
        keys[count++] = NK_KEY_TEXT_REDO;
        if (command)
            selected = NK_KEY_TEXT_REDO;
        break;
    case SDLK_LEFT:
    case SDLK_RIGHT: {
        bool left = key == SDLK_LEFT;
        keys[count++] = left ? NK_KEY_LEFT : NK_KEY_RIGHT;
        keys[count++] = left ? NK_KEY_TEXT_WORD_LEFT : NK_KEY_TEXT_WORD_RIGHT;
        keys[count++] = left ? NK_KEY_TEXT_LINE_START : NK_KEY_TEXT_LINE_END;
        selected = keys[mac && command ? 2 : word ? 1 : 0];
        break;
    }
    case SDLK_UP:
    case SDLK_DOWN:
        keys[count++] = key == SDLK_UP ? NK_KEY_UP : NK_KEY_DOWN;
        keys[count++] = key == SDLK_UP ? NK_KEY_TEXT_START : NK_KEY_TEXT_END;
        selected = keys[mac && command ? 1 : 0];
        break;
    case SDLK_HOME:
    case SDLK_END: {
        bool home = key == SDLK_HOME;
        keys[count++] = home ? NK_KEY_TEXT_LINE_START : NK_KEY_TEXT_LINE_END;
        keys[count++] = home ? NK_KEY_TEXT_START : NK_KEY_TEXT_END;
        keys[count++] = home ? NK_KEY_SCROLL_START : NK_KEY_SCROLL_END;
        selected = keys[command ? 1 : mac ? 2 : 0];
        break;
    }
    default:
        return false;
    }
    if (ctx->input.keyboard.keys[NK_KEY_SHIFT].down != shift)
        nk_input_key(ctx, NK_KEY_SHIFT, shift);
    /* Key-up can arrive after the modifier was released. Clear every variant
     * of that physical key so a word/line action cannot remain held. */
    for (unsigned i = 0; i < count; i++) {
        bool pressed = down && keys[i] == selected;
        /* Nuklear counts even redundant releases as clicks. Only release an
         * action that
         * was held, while still forwarding repeated key-downs. */
        if (pressed || ctx->input.keyboard.keys[keys[i]].down)
            nk_input_key(ctx, keys[i], pressed);
    }
    return true;
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
        int ctrl_down = evt->key.mod & PS_UI_COMMAND_MOD;
#ifdef __APPLE__
        const bool mac = true;
#else
        const bool mac = false;
#endif
        if (nk_sdl_edit_key(ctx, evt->key.key, evt->key.mod, down != 0, mac))
            return 1;

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

static float test_text_width(nk_handle user, float height, const char *text, int length) {
    (void)user;
    (void)text;
    return height * 0.5f * (float)length;
}
static void test_edit_frame(struct nk_context *ctx, struct nk_text_edit *edit) {
    if (nk_begin(ctx, "Keyboard regression", nk_rect(0, 0, 600, 400), 0)) {
        nk_layout_row_dynamic(ctx, 300, 1);
        nk_edit_focus(ctx, NK_EDIT_ALWAYS_INSERT_MODE);
        nk_edit_buffer(ctx, NK_EDIT_BOX | NK_EDIT_ALWAYS_INSERT_MODE, edit, nk_filter_default);
    }
    nk_end(ctx);
    nk_clear(ctx);
}
static bool test_editor_navigation(bool mac) {
    struct nk_user_font font = {0};
    font.height = 16;
    font.width = test_text_width;
    struct nk_context ctx;
    if (!nk_init_default(&ctx, &font))
        return false;
    char buffer[128];
    struct nk_text_edit edit;
    nk_textedit_init_fixed(&edit, buffer, sizeof buffer);
    edit.mode = NK_TEXT_EDIT_MODE_INSERT;
    nk_sdl_paste_text(&edit, "one two\nthree four");
    test_edit_frame(&ctx, &edit);
    SDL_Keymod command = mac ? SDL_KMOD_GUI : SDL_KMOD_CTRL;
    SDL_Keymod word = mac ? SDL_KMOD_ALT : SDL_KMOD_CTRL;
    const struct {
        SDL_Keycode key;
        SDL_Keymod mod;
        int cursor;
    } cases[] = {
        {SDLK_LEFT, word, 8},
        {SDLK_RIGHT, word, 14},
        {mac ? SDLK_LEFT : SDLK_HOME, mac ? command : 0, 8},
        {mac ? SDLK_RIGHT : SDLK_END, mac ? command : 0, 18},
        {mac ? SDLK_UP : SDLK_HOME, command, 0},
        {mac ? SDLK_DOWN : SDLK_END, command, 18},
        {SDLK_LEFT, 0, 10},
        {SDLK_RIGHT, 0, 12},
        {SDLK_LEFT, (SDL_Keymod)(word | SDL_KMOD_SHIFT), 8},
        {mac ? SDLK_RIGHT : SDLK_END, (SDL_Keymod)((mac ? command : 0) | SDL_KMOD_SHIFT), 18},
    };
    bool ok = true;
    for (unsigned i = 0; i < sizeof cases / sizeof cases[0]; i++) {
        edit.cursor = edit.select_start = edit.select_end = 11;
        nk_input_begin(&ctx);
        nk_sdl_edit_key(&ctx, cases[i].key, cases[i].mod, true, mac);
        nk_input_end(&ctx);
        test_edit_frame(&ctx, &edit);
        bool selected = (cases[i].mod & SDL_KMOD_SHIFT) != 0;
        ok = ok && edit.cursor == cases[i].cursor &&
             (selected ? edit.select_start == 11 && edit.select_end == cases[i].cursor
                       : edit.select_start == edit.select_end);
        /* Release modifiers first: no action may remain held or move the cursor again. */
        nk_input_begin(&ctx);
        nk_sdl_edit_key(&ctx, cases[i].key, 0, false, mac);
        nk_input_end(&ctx);
        test_edit_frame(&ctx, &edit);
        ok = ok && edit.cursor == cases[i].cursor;
        for (int key = 0; key < NK_KEY_MAX; key++)
            ok = ok && !ctx.input.keyboard.keys[key].down;
    }
    /* A quick press/release within one frame must undo or redo exactly once. */
    for (unsigned i = 0; i < 4; i++) {
        SDL_Keycode key = i == 3 ? (mac ? SDLK_R : SDLK_Y) : SDLK_Z;
        SDL_Keymod mod = (SDL_Keymod)(command | (i == 1 ? SDL_KMOD_SHIFT : 0));
        nk_input_begin(&ctx);
        nk_sdl_edit_key(&ctx, key, mod, true, mac);
        nk_sdl_edit_key(&ctx, key, 0, false, mac);
        nk_input_end(&ctx);
        test_edit_frame(&ctx, &edit);
        ok = ok && nk_str_len_char(&edit.string) == (i % 2 ? 18 : 0);
    }
    nk_free(&ctx);
    return ok;
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
    event.key.key = SDLK_C;
    event.key.mod = PS_UI_COMMAND_MOD;
    nk_sdl_handle_event(ctx, &event);
    ok = ok && ctx->input.keyboard.keys[NK_KEY_COPY].down;
    event.type = SDL_EVENT_KEY_UP;
    event.key.down = false;
    nk_sdl_handle_event(ctx, &event);
    ok = ok && !ctx->input.keyboard.keys[NK_KEY_COPY].down;
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
    ok = test_editor_navigation(false) && ok;
    ok = test_editor_navigation(true) && ok;
    nk_sdl_shutdown(ctx);
    if (!ok)
        SDL_SetError("SDL input test: Unicode, clipboard, editor navigation or focus reset failed");
    else
        puts("INPUT TEST: UTF-8, undo/redo, clipboard, Mac/PC navigation/selection, focus PASSED");
    return ok;
}
