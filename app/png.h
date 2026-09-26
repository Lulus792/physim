#ifndef PHYSIM_PNG_H
#define PHYSIM_PNG_H
#include "physim/core.h"
#include <stddef.h>
/* RGB8, top row first, 1..8192 pixels per dimension. Exclusive output creation.
 * Streaming lossless DEFLATE uses a row buffer and statically linked zlib. */
ps_result ps_png_write(const char *path, const unsigned char *rgb, unsigned width, unsigned height,
                       size_t stride);
#endif
