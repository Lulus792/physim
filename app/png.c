#include "png.h"
#include "physim/data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
static void big32(unsigned char *p, uint32_t v) {
    p[0] = (unsigned char)(v >> 24);
    p[1] = (unsigned char)(v >> 16);
    p[2] = (unsigned char)(v >> 8);
    p[3] = (unsigned char)v;
}
/* Buffer includes four bytes before the payload for the chunk type. */
static bool chunk(FILE *f, unsigned char *buffer, const char *type, size_t size) {
    unsigned char length[4], crc[4];
    memcpy(buffer, type, 4);
    big32(length, (uint32_t)size);
    big32(crc, ps_crc32(buffer, size + 4));
    return fwrite(length, 1, 4, f) == 4 && fwrite(buffer, 1, size + 4, f) == size + 4 &&
           fwrite(crc, 1, 4, f) == 4;
}
ps_result ps_png_write(const char *path, const unsigned char *rgb, unsigned width, unsigned height,
                       size_t stride) {
    if (!path || !*path || !rgb || !width || !height || width > 8192 || height > 8192 ||
        stride < (size_t)width * 3 || stride > SIZE_MAX / height)
        return PS_INVALID;
    size_t row = (size_t)width * 3;
    enum { OUTPUT_SIZE = 65536 };
    unsigned char *input = malloc(row + 1);
    unsigned char *buffer = malloc(OUTPUT_SIZE + 4);
    if (!input || !buffer) {
        free(input);
        free(buffer);
        return PS_MEMORY;
    }
    z_stream stream = {0};
    int status = deflateInit(&stream, 6);
    if (status != Z_OK) {
        free(input);
        free(buffer);
        return status == Z_MEM_ERROR ? PS_MEMORY : PS_IO;
    }
    FILE *f = fopen(path, "wbx");
    if (!f) {
        deflateEnd(&stream);
        free(input);
        free(buffer);
        return PS_IO;
    }
    static const unsigned char signature[] = {137, 80, 78, 71, 13, 10, 26, 10};
    unsigned char header[17] = {0};
    big32(header + 4, width);
    big32(header + 8, height);
    header[12] = 8;
    header[13] = 2; /* RGB, no alpha; filter 0, non-interlaced. */
    bool ok = fwrite(signature, 1, 8, f) == 8 && chunk(f, header, "IHDR", 13);
    for (unsigned y = 0; ok && y < height; y++) {
        input[0] = 0; /* PNG filter None */
        memcpy(input + 1, rgb + (size_t)y * stride, row);
        stream.next_in = input;
        stream.avail_in = (uInt)(row + 1);
        while (ok && stream.avail_in) {
            stream.next_out = buffer + 4;
            stream.avail_out = OUTPUT_SIZE;
            status = deflate(&stream, Z_NO_FLUSH);
            size_t produced = OUTPUT_SIZE - stream.avail_out;
            ok = status == Z_OK && (!produced || chunk(f, buffer, "IDAT", produced));
        }
    }
    while (ok) {
        stream.next_out = buffer + 4;
        stream.avail_out = OUTPUT_SIZE;
        status = deflate(&stream, Z_FINISH);
        size_t produced = OUTPUT_SIZE - stream.avail_out;
        ok = (status == Z_OK || status == Z_STREAM_END) &&
             (!produced || chunk(f, buffer, "IDAT", produced));
        if (status == Z_STREAM_END)
            break;
    }
    deflateEnd(&stream);
    if (ok)
        ok = chunk(f, buffer, "IEND", 0);
    if (fclose(f))
        ok = false;
    free(buffer);
    free(input);
    if (!ok)
        remove(path); /* Only our exclusively created, incomplete output. */
    return ok ? PS_OK : (status == Z_MEM_ERROR ? PS_MEMORY : PS_IO);
}
