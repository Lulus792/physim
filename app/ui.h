#ifndef PHYSIM_UI_H
#define PHYSIM_UI_H
#include <stdbool.h>
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
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
void nk_sdl_shutdown(struct nk_context *ctx);
bool nk_sdl_test_input(SDL_Window *window, ps_graphics *graphics);
#endif
