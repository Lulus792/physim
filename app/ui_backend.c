/* Third-party implementations are isolated from the application's warning policy. */
#ifdef _MSC_VER
#pragma warning(push, 0)
#pragma warning(disable : 4116 4701 4706)
#endif
#define NK_IMPLEMENTATION
#include "ui.h"

void nk_sdl_window_raise(struct nk_context *ctx, const char *name) {
    struct nk_window *window=nk_find_window(ctx,nk_murmur_hash(name,nk_strlen(name),NK_WINDOW_TITLE),name);
    if(!window || window==ctx->end) return;
    struct nk_window *active=ctx->active;
    struct nk_window *previous_top=ctx->end;
    nk_flags window_flags=window->flags,top_flags=previous_top->flags;
    nk_remove_window(ctx,window);
    nk_insert_window(ctx,window,NK_INSERT_BACK);
    /* Nuklear's insert also changes input flags. Stacking must preserve them,
     * including a held mouse drag in an editor, plot or slider. */
    window->flags=window_flags;
    previous_top->flags=top_flags;
    ctx->active=active;
}
