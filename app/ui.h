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
bool nk_sdl_accessibility_choose(struct nk_context *ctx,const char *group,const char *label);
bool nk_sdl_accessibility_focus(struct nk_context *ctx,const char *group,const char *label);
bool nk_sdl_accessibility_event(struct nk_context *ctx,const SDL_Event *event);
bool nk_sdl_accessibility_is_focused(struct nk_context *ctx,const char *group,const char *label);
bool nk_sdl_accessibility_has_focus(struct nk_context *ctx);
void ps_ui_accessibility_route(struct nk_context *ctx,bool allowed);
bool nk_sdl_accessibility_press(struct nk_context *ctx,const char *label);
bool nk_sdl_accessibility_available(struct nk_context *ctx);
void nk_sdl_set_ui_size(struct nk_context *ctx, unsigned size);
enum { PS_UI_LAYOUT_MAGIC = 0x50534C59u };
typedef unsigned (*ps_ui_a11y_hook)(void *user,const char *window,const char *label,
                                int role,const float bounds[4],bool enabled,bool checked,const char *group,bool activated);
typedef unsigned (*ps_ui_a11y_range_hook)(void *user,const char *window,const char *label,
                                const float bounds[4],bool enabled,double values[4]);
typedef struct {
    unsigned magic,ui_size;
    ps_ui_a11y_hook accessibility;
    ps_ui_a11y_range_hook accessibility_range;
    void *accessibility_user;
    const struct nk_window *accessibility_route_window;
    bool accessibility_route_allowed;
} ps_ui_font_layout;
/* Finish queued field input before a semantic keyboard focus change. */
void ps_ui_flush_edit(struct nk_context *ctx,const char *window,char *text,size_t capacity,struct nk_rect bounds);
float nk_sdl_row_height(const struct nk_context *ctx, float requested);
void ps_ui_label_wrap(struct nk_context *ctx,const char *text);
nk_bool ps_ui_option_label(struct nk_context *ctx,const char *group,const char *text,nk_bool active);
nk_bool ps_ui_checkbox_named(struct nk_context *ctx,const char *text,const char *name,nk_bool *active);
nk_bool ps_ui_checkbox_label(struct nk_context *ctx,const char *text,nk_bool *active);
nk_bool ps_ui_button_label(struct nk_context *ctx,const char *text);
nk_bool ps_ui_slider_named(struct nk_context *ctx,const char *name,float minimum,float *value,float maximum,float increment);
void ps_ui_label(struct nk_context *ctx,const char *text,nk_flags alignment);
void ps_ui_label_colored(struct nk_context *ctx,const char *text,nk_flags alignment,struct nk_color color);
typedef enum {
    PS_ICON_CLOSE, PS_ICON_DRAG, PS_ICON_FILE, PS_ICON_FOLDER, PS_ICON_SEARCH,
    PS_ICON_SAVE, PS_ICON_BUILD, PS_ICON_LEFT, PS_ICON_RIGHT, PS_ICON_ERROR,
    PS_ICON_FUNCTION, PS_ICON_TYPE, PS_ICON_VALUE, PS_ICON_FILE_ADD, PS_ICON_FOLDER_ADD,
    PS_ICON_LAYOUT, PS_ICON_CONSOLE, PS_ICON_REFRESH, PS_ICON_CHART, PS_ICON_DICE,
    PS_ICON_SIDEBAR, PS_ICON_WINDOW, PS_ICON_LAYOUT_RESET, PS_ICON_INSPECTOR,
    PS_ICON_BOOK, PS_ICON_REFERENCE, PS_ICON_COMPLETION
} ps_ui_icon;
void ps_ui_draw_icon(struct nk_command_buffer *canvas,ps_ui_icon icon,struct nk_rect bounds,struct nk_color color);
nk_bool ps_ui_icon_button(struct nk_context *ctx,ps_ui_icon icon,const char *name);
nk_bool ps_ui_button_at(struct nk_context *ctx,struct nk_rect bounds,const char *name,bool selected);
/* Publish a single semantic button for a control drawn from multiple fragments.
 * Only queued native activation is returned; pointer hit testing stays with the
 * renderer's actual visible fragments. */
nk_bool ps_ui_accessibility_button(struct nk_context *ctx,struct nk_rect bounds,const char *name);
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
#define nk_checkbox_label ps_ui_checkbox_label
#define nk_button_label ps_ui_button_label
#define nk_label ps_ui_label
#define nk_label_colored ps_ui_label_colored
#endif
