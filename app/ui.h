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
/* Borrow the owner's immutable font atlas in a shared GL context. The owner
 * must outlive the borrower; input, windows and accessibility stay independent. */
void nk_sdl_borrow_fonts(struct nk_context *ctx, const struct nk_context *owner);
struct nk_font_atlas *nk_sdl_font_stash_begin(struct nk_context *ctx);
/* Loaded Nuklear font contains a real source glyph and a matching baked entry,
 * rather than the fallback glyph. Baked source presence survives blob cleanup. */
bool ps_ui_font_has_glyph(const struct nk_user_font *font,nk_rune codepoint);
/* Before atlas cleanup: convert CSS em sizing to stb's ascent/descent height. */
float ps_ui_font_em_ratio(const struct nk_font *font);
/* Copy the primary face before atlas cleanup. The independent resource uses
 * the supplied allocator until destroy; append/replacement owns it separately.
 * A bounded pair cache belongs to the UI thread. Pair adjustment is in
 * logical pixels at stb ascent/descent height. This is
 * pair kerning only, not general script shaping or fallback-font positioning. */
typedef struct ps_ui_font_pairs ps_ui_font_pairs;
ps_ui_font_pairs *ps_ui_font_pairs_create(const struct nk_font *font,const struct nk_allocator *allocator);
void ps_ui_font_pairs_destroy(ps_ui_font_pairs *pairs);
float ps_ui_font_pair_advance(const ps_ui_font_pairs *pairs,float height,nk_rune first,nk_rune second);
/* Custom export callbacks retain floating coordinates through conversion;
 * this is the same clip operation as Nuklear's ordinary scissor conversion. */
void ps_ui_draw_list_clip(struct nk_draw_list *list,struct nk_rect bounds);
/* Load the first non-italic face matching the OpenType bold flag. A missing
 * match leaves the atlas unchanged, allowing the caller's offline fallback. */
struct nk_font *ps_ui_font_add_file_face(struct nk_font_atlas *atlas,const char *path,
                                        float height,const struct nk_font_config *config,bool bold);
/* Before baking: add only supplied codepoints absent from the target's sources.
 * Target must be the atlas's last font; all allocated ranges belong to the atlas. */
bool ps_ui_font_add_fallback(struct nk_font_atlas *atlas,struct nk_font *target,
                             const char *path,const nk_rune *ranges);
bool nk_sdl_font_stash_end(struct nk_context *ctx);
/* Normalize a completed atlas baked in device pixels to logical font heights. */
bool ps_ui_font_logical_scale(struct nk_font_atlas *atlas,float raster_scale);
/* Bake/upload an unbaked candidate, preserving the old atlas on failure. Success takes
 * ownership of candidate and installs default_font before releasing old data.
 * Replace before recording a frame; successful replacement retires old handles.
 * If failure leaves baked pixels/glyphs, clear it before rebuilding/retrying. */
bool nk_sdl_font_stash_replace(struct nk_context *ctx,struct nk_font_atlas *candidate,
                               float raster_scale,struct nk_font *default_font);
/* Same current GL context as the renderer. Append an unbaked candidate at the installed raster
 * scale without changing the style or invalidating existing text commands.
 * Success takes candidate ownership. Failure leaves ownership with the caller;
 * if baked pixels/glyphs remain, clear it before rebuilding/retrying.
 * A successful replacement or shutdown releases all appended atlases. */
#define PS_UI_FONT_APPEND_LIMIT 38
bool nk_sdl_font_stash_append(struct nk_context *ctx,struct nk_font_atlas *candidate,
                              float raster_scale);
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
/* Application focus routing through the common published model. The method
 * above deliberately exercises native adapters for platform regression tests. */
bool nk_sdl_accessibility_request_focus(struct nk_context *ctx,const char *group,const char *label);
bool nk_sdl_accessibility_event(struct nk_context *ctx,const SDL_Event *event);
bool nk_sdl_accessibility_is_focused(struct nk_context *ctx,const char *group,const char *label);
/* Read the published focused target inside logical bounds, without changing it. */
bool nk_sdl_accessibility_focus_in(struct nk_context *ctx,struct nk_rect bounds);
/* Read the published, grouped choice state without scheduling an action. */
bool nk_sdl_accessibility_choice_state(struct nk_context *ctx,const char *group,const char *label,bool *selected);
bool nk_sdl_accessibility_toggle_state(struct nk_context *ctx,const char *label,bool *enabled,bool *checked);
bool nk_sdl_accessibility_has_focus(struct nk_context *ctx);
/* Publish the custom keyboard route's visible target without queuing an
 * assistive action. A granted native focus request takes ownership back. */
void nk_sdl_accessibility_keyboard_begin(struct nk_context *ctx);
bool nk_sdl_accessibility_keyboard_owned(struct nk_context *ctx);
bool nk_sdl_accessibility_keyboard_target(struct nk_context *ctx,struct nk_rect bounds);
void ps_ui_accessibility_route(struct nk_context *ctx,bool allowed);
bool nk_sdl_accessibility_press(struct nk_context *ctx,const char *label);
bool nk_sdl_accessibility_available(struct nk_context *ctx);
bool nk_sdl_accessibility_document(struct nk_context *ctx,uint64_t version,const char *name,const char *text,struct nk_rect viewport);
bool nk_sdl_accessibility_document_focus(struct nk_context *ctx,uint64_t version,bool focused);
struct ps_a11y_table_row_input;
struct ps_a11y_table;
struct ps_a11y_table_cell;
bool nk_sdl_accessibility_document_table(struct nk_context *ctx,size_t begin,size_t end,
    size_t rows,size_t columns,bool headers,const struct ps_a11y_table_row_input *input);
void nk_sdl_accessibility_document_abort(struct nk_context *ctx);
/* Atomically copy published table/cell metadata under the accessibility mutex. */
bool nk_sdl_accessibility_document_table_cell(struct nk_context *ctx,size_t index,size_t row,
    size_t column,struct ps_a11y_table *table,struct ps_a11y_table_cell *cell);
void nk_sdl_accessibility_text_box(struct nk_context *ctx,size_t begin,size_t end,struct nk_rect bounds);
bool nk_sdl_accessibility_document_scroll(struct nk_context *ctx,uint64_t version,size_t *byte,bool *top);
bool nk_sdl_accessibility_document_preserve_position(struct nk_context *ctx,uint64_t version);
bool nk_sdl_accessibility_document_link(struct nk_context *ctx,const char *name,uint64_t version,
                                        size_t begin,size_t end,struct nk_rect bounds);
bool nk_sdl_accessibility_document_link_target(struct nk_context *ctx,const char *name,const char *target,
                                        uint64_t version,size_t begin,size_t end,struct nk_rect bounds);
void nk_sdl_accessibility_document_link_invoked(struct nk_context *ctx,uint64_t version,size_t begin,size_t end);
void nk_sdl_accessibility_suspend(struct nk_context *ctx);
bool nk_sdl_accessibility_document_visible(struct nk_context *ctx,uint64_t version,size_t byte);
bool nk_sdl_accessibility_document_native_read_test(struct nk_context *ctx);
bool nk_sdl_accessibility_native_edit_test_step(struct nk_context *ctx,int step);
bool nk_sdl_accessibility_native_links_test_step(struct nk_context *ctx,int step);
nk_flags nk_sdl_edit_named(struct nk_context *ctx,const char *name,nk_flags flags,char *text,int capacity,nk_plugin_filter filter);
enum {PS_UI_EDIT_NATIVE_FOCUS=1u<<24};
void nk_sdl_set_ui_size(struct nk_context *ctx, unsigned size);
bool nk_sdl_reduce_transparency(struct nk_context *ctx);
/* Force conversion boundaries around a backdrop marker; Nuklear can otherwise
 * merge consecutive solid shapes despite their different command userdata. */
void ps_ui_draw_backdrop(struct nk_context *ctx,const ps_ui_backdrop *material);
enum { PS_UI_LAYOUT_MAGIC = 0x50534C59u };
typedef unsigned (*ps_ui_a11y_hook)(void *user,const char *window,const char *label,
                                int role,const float bounds[4],bool enabled,bool checked,const char *group,bool activated);
typedef unsigned (*ps_ui_a11y_range_hook)(void *user,const char *window,const char *label,
                                int role,const float bounds[4],bool enabled,double values[4]);
typedef struct {
    unsigned magic,ui_size;
    ps_ui_a11y_hook accessibility;
    ps_ui_a11y_range_hook accessibility_range;
    void *accessibility_user;
    const struct nk_window *accessibility_route_window;
    bool accessibility_route_allowed;
    struct nk_color focus_color;
} ps_ui_font_layout;
void ps_ui_set_focus_color(struct nk_context *ctx,struct nk_color color);
struct nk_color ps_ui_focus_color(const struct nk_context *ctx);
/* Finish queued field input before a semantic keyboard focus change. */
void ps_ui_flush_edit(struct nk_context *ctx,const char *window,char *text,size_t capacity,struct nk_rect bounds);
float nk_sdl_row_height(const struct nk_context *ctx, float requested);
/* Register a read-only label drawn by a component's own wrapping renderer. */
void ps_ui_accessibility_label(struct nk_context *ctx,struct nk_rect bounds,const char *text);
void ps_ui_label_wrap(struct nk_context *ctx,const char *text);
nk_bool ps_ui_option_label(struct nk_context *ctx,const char *group,const char *text,nk_bool active);
nk_bool ps_ui_checkbox_named(struct nk_context *ctx,const char *text,const char *name,nk_bool *active);
nk_bool ps_ui_checkbox_label(struct nk_context *ctx,const char *text,nk_bool *active);
nk_bool ps_ui_button_label(struct nk_context *ctx,const char *text);
/* Measured absolute slots: ordinary Nuklear button plus native semantics,
 * without the flow-row adapter's automatic large-text height changes. */
nk_bool ps_ui_fixed_button_label(struct nk_context *ctx,const char *text);
/* Named menu button using Nuklear's ordinary nonblocking combo lifecycle.
 * The caller draws the field background before reserving its layout slot. */
nk_bool ps_ui_combo_begin_named(struct nk_context *ctx,const char *name,const char *label,struct nk_vec2 size);
nk_bool ps_ui_slider_named(struct nk_context *ctx,const char *name,float minimum,float *value,float maximum,float increment);
typedef struct {bool active;float pointer,value;} ps_ui_scrollbar_drag;
/* Same scrollbar material as Nuklear, with a minimum usable thumb. Numeric
 * position is published separately through the existing native range adapter. */
nk_bool ps_ui_horizontal_scrollbar_at(struct nk_context *ctx,struct nk_rect bounds,float *value,float maximum,float page,ps_ui_scrollbar_drag *drag,bool input_enabled);
nk_bool ps_ui_range_at(struct nk_context *ctx,struct nk_rect bounds,const char *name,float *value,float maximum,float increment);
nk_bool ps_ui_separator_at(struct nk_context *ctx,struct nk_rect bounds,const char *name,float minimum,float *value,float maximum);
nk_bool ps_ui_separator_step_at(struct nk_context *ctx,struct nk_rect bounds,const char *name,float minimum,float *value,float maximum,float step);
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
/* Custom painted switch, using the shared native checkbox/check-state contract. */
nk_bool ps_ui_switch_at(struct nk_context *ctx,struct nk_rect bounds,const char *name,nk_bool *checked);
/* Input and native radio semantics for custom painted segmented controls/tabs. */
nk_bool ps_ui_option_at(struct nk_context *ctx,struct nk_rect bounds,const char *group,const char *name,bool selected);
/* Publish a single semantic button for a control drawn from multiple fragments.
 * Only queued native activation is returned; pointer hit testing stays with the
 * renderer's actual visible fragments. */
nk_bool ps_ui_accessibility_button(struct nk_context *ctx,struct nk_rect bounds,const char *name);
/* Keep short label/control rows readable with enlarged UI fonts. Spacer and
 * chart/editor heights remain explicit; the default 16 px layout is unchanged. */
/* Measured font-role layouts already include text scaling. Keep a direct path
 * before the object-like Nuklear aliases below; parentheses cannot bypass them. */
static inline void ps_ui_row_exact_dynamic(struct nk_context *ctx,float height,int columns) {
    nk_layout_row_dynamic(ctx,height,columns);
}
static inline void ps_ui_row_exact_begin(struct nk_context *ctx,enum nk_layout_format format,float height,int columns) {
    nk_layout_row_begin(ctx,format,height,columns);
}
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
