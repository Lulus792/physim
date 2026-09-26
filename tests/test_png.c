#include "png.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "PNG line %d: %s\n", __LINE__, #x);                                    \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
int main(int argc, char **argv) {
    CHECK(argc == 2);
    unsigned char pixels[65][200];
    memset(pixels, 213, sizeof pixels);
    for (unsigned y = 0; y < 65; y++)
        for (unsigned x = 0; x < 64; x++) {
            pixels[y][3 * x] = (unsigned char)(x * 3);
            pixels[y][3 * x + 1] = (unsigned char)(y * 3);
            pixels[y][3 * x + 2] = (unsigned char)(x ^ y);
        }
    remove(argv[1]);
    CHECK(ps_png_write(argv[1], &pixels[0][0], 0, 65, 200) == PS_INVALID);
    CHECK(ps_png_write(argv[1], &pixels[0][0], 8193, 1, 30000) == PS_INVALID);
    CHECK(ps_png_write(argv[1], &pixels[0][0], 64, 65, 191) == PS_INVALID);
    CHECK(ps_png_write(argv[1], &pixels[0][0], 64, 65, SIZE_MAX) == PS_INVALID);
    CHECK(ps_png_write(NULL, &pixels[0][0], 64, 65, 200) == PS_INVALID);
    CHECK(ps_png_write(argv[1], NULL, 64, 65, 200) == PS_INVALID);
    CHECK(ps_png_write(argv[1], &pixels[0][0], 64, 65, 200) == PS_OK);
    CHECK(ps_png_write(argv[1], &pixels[0][0], 1, 1, 3) == PS_IO);
    FILE *f = fopen(argv[1], "rb");
    CHECK(f);
    unsigned char header[33];
    CHECK(fread(header, 1, sizeof header, f) == sizeof header);
    CHECK(!memcmp(header, "\211PNG\r\n\032\n", 8));
    CHECK(header[19] == 64 && header[23] == 65 && header[24] == 8 && header[25] == 2);
    CHECK(!fclose(f));
    unsigned char *edge = malloc(8192 * 3);
    CHECK(edge);
    for (unsigned i = 0; i < 8192; i++) {
        edge[i * 3] = 7;
        edge[i * 3 + 1] = 8;
        edge[i * 3 + 2] = 9;
    }
    const unsigned sizes[][2] = {{1, 1}, {8192, 1}, {1, 8192}};
    for (unsigned i = 0; i < 3; i++) {
        char path[4096];
        int n = snprintf(path, sizeof path, "%s.edge-%u.png", argv[1], i);
        CHECK(n > 0 && (size_t)n < sizeof path);
        remove(path);
        CHECK(ps_png_write(path, edge, sizes[i][0], sizes[i][1], sizes[i][0] * 3) == PS_OK);
    }
    free(edge);
    /* Incompressible data crosses output-buffer and DEFLATE block boundaries. */
    unsigned char *noise = malloc(257 * 131 * 3);
    CHECK(noise);
    uint32_t state = 12345;
    for (size_t i = 0; i < 257 * 131 * 3; i++) {
        state = state * 1664525u + 1013904223u;
        noise[i] = (unsigned char)(state >> 24);
    }
    char noise_path[4096];
    int n = snprintf(noise_path, sizeof noise_path, "%s.noise.png", argv[1]);
    CHECK(n > 0 && (size_t)n < sizeof noise_path);
    remove(noise_path);
    CHECK(ps_png_write(noise_path, noise, 257, 131, 257 * 3) == PS_OK);
    free(noise);
    puts("PNG input validation and exclusive output passed");
    return 0;
}
