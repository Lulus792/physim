#ifndef PHYSIM_PNG_H
#define PHYSIM_PNG_H
#include "physim/core.h"
#include <stddef.h>
/* RGB8, top row first, 1..8192 pixels per dimension. Exclusive output creation.
 * Streaming lossless DEFLATE at best-speed level uses a row buffer and static
 * zlib. Compression size may vary; decoded RGB bytes are always unchanged. */
ps_result ps_png_write(const char *path, const unsigned char *rgb, unsigned width, unsigned height,
                       size_t stride);
#endif
