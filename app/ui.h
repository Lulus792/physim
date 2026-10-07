#ifndef PHYSIM_UI_H
#define PHYSIM_UI_H
#include <stdbool.h>
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#ifdef __APPLE__
#define PS_UI_COMMAND_MOD SDL_KMOD_GUI
#define PS_UI_SHORTCUT "Cmd+"
#else
#define PS_UI_COMMAND_MOD SDL_KMOD_CTRL
#define PS_UI_SHORTCUT "Ctrl+"
#endif
#define NK_BOOL bool
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_UINT_DRAW_INDEX
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_COMMAND_USERDATA
#define NK_MAX_FLOAT_PRECISION 5
#define NK_INPUT_MAX 4096
#include "graphics.h"
#include "nuklear.h"
struct nk_context *nk_sdl_init(SDL_Window *window, ps_graphics *graphics);
struct nk_font_atlas *nk_sdl_font_stash_begin(struct nk_context *ctx);
bool nk_sdl_font_stash_end(struct nk_context *ctx);
int nk_sdl_handle_event(struct nk_context *ctx, SDL_Event *event);
bool nk_sdl_render(struct nk_context *ctx);
ps_result nk_sdl_export_png(struct nk_context *ctx, struct nk_context *drawing, int width,
                            int height, const char *path);
ps_result nk_sdl_export_png_scaled(struct nk_context *ctx, struct nk_context *drawing, int width,
                                   int height, unsigned scale, const char *path);
void nk_sdl_update_TextInput(struct nk_context *ctx);
/* Change stacking without moving keyboard focus. */
void nk_sdl_window_raise(struct nk_context *ctx, const char *name);
void nk_sdl_shutdown(struct nk_context *ctx);
bool nk_sdl_test_input(SDL_Window *window, ps_graphics *graphics);
bool nk_sdl_accessibility_press(struct nk_context *ctx,const char *label);
bool nk_sdl_accessibility_available(struct nk_context *ctx);
void nk_sdl_set_ui_size(struct nk_context *ctx, unsigned size);
enum { PS_UI_LAYOUT_MAGIC = 0x50534C59u };
typedef bool (*ps_ui_a11y_hook)(void *user,const char *window,const char *label,
                                int role,const float bounds[4],bool enabled);
typedef struct {
    unsigned magic,ui_size;
    ps_ui_a11y_hook accessibility;
    void *accessibility_user;
} ps_ui_font_layout;
/* Finish queued field input before a semantic keyboard focus change. */
void ps_ui_flush_edit(struct nk_context *ctx,const char *window,char *text,size_t capacity,struct nk_rect bounds);
float nk_sdl_row_height(const struct nk_context *ctx, float requested);
void ps_ui_label_wrap(struct nk_context *ctx,const char *text);
nk_bool ps_ui_button_label(struct nk_context *ctx,const char *text);
void ps_ui_label(struct nk_context *ctx,const char *text,nk_flags alignment);
void ps_ui_label_colored(struct nk_context *ctx,const char *text,nk_flags alignment,struct nk_color color);
/* Keep short label/control rows readable with enlarged UI fonts. Spacer and
 * chart/editor heights remain explicit; the default 16 px layout is unchanged. */
static inline void ps_ui_row_dynamic(struct nk_context *ctx,float height,int columns) {
    nk_layout_row_dynamic(ctx,nk_sdl_row_height(ctx,height),columns);
}
static inline void ps_ui_row_static(struct nk_context *ctx,float height,int width,int columns) {
    nk_layout_row_static(ctx,nk_sdl_row_height(ctx,height),width,columns);
}
static inline void ps_ui_row_begin(struct nk_context *ctx,enum nk_layout_format format,float height,int columns) {
    nk_layout_row_begin(ctx,format,nk_sdl_row_height(ctx,height),columns);
}
#define nk_layout_row_dynamic ps_ui_row_dynamic
#define nk_layout_row_static ps_ui_row_static
#define nk_layout_row_begin ps_ui_row_begin
#define nk_label_wrap ps_ui_label_wrap
#define nk_button_label ps_ui_button_label
#define nk_label ps_ui_label
#define nk_label_colored ps_ui_label_colored
#endif
