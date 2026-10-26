#ifndef PS_GRAPHIC_OUTPUT_H
#define PS_GRAPHIC_OUTPUT_H
#include "graphics.h"
/* Owned immutable CPU outputs; no app/UI/GL pointers reach OS callbacks. */
typedef struct {unsigned char *data;size_t size;} ps_graphic_bytes;
void ps_graphic_bytes_free(ps_graphic_bytes *bytes);
ps_result ps_graphic_image_bmp(const ps_graphics_image *image,ps_graphic_bytes *out);
ps_result ps_graphic_image_clipboard(const ps_graphics_image *image);
typedef struct ps_graphic_save ps_graphic_save;
typedef enum {PS_GRAPHIC_SAVE_WAITING,PS_GRAPHIC_SAVE_WRITING,PS_GRAPHIC_SAVE_OK,
              PS_GRAPHIC_SAVE_CANCELLED,PS_GRAPHIC_SAVE_ERROR} ps_graphic_save_state;
/* Takes bytes on success. One main-thread owner; native callback owns a separate
 * reference. poll starts the single IO worker. destroy cancels a pending chooser;
 * a later callback frees its CPU owner. Started writes finish before destruction. */
ps_graphic_save *ps_graphic_save_create(ps_graphic_bytes *bytes);
void ps_graphic_save_show(ps_graphic_save *save,SDL_Window *parent,const char *path,bool svg);
/* Same immutable byte owner and atomic writer, with a text-file chooser. */
void ps_graphic_save_show_text(ps_graphic_save *save,SDL_Window *parent,const char *path);
/* Shared OS-reply path for deterministic tests: once, only without show(). */
void ps_graphic_save_resolve(ps_graphic_save *save,const char *path,bool error);
ps_graphic_save_state ps_graphic_save_poll(ps_graphic_save *save);
const char *ps_graphic_save_path(ps_graphic_save *save); /* after terminal poll, borrowed */
void ps_graphic_save_destroy(ps_graphic_save *save);
#endif
