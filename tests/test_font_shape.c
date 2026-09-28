/* Exercise the vendored glyph parser without depending on installed fonts. */
#ifdef _MSC_VER
#pragma warning(push, 0)
#pragma warning(disable : 4116 4701 4706)
#endif
#define NK_IMPLEMENTATION
#include "ui.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Font shape line %d: %s\n", __LINE__, #x);                             \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static void *font_allocate(nk_handle user, void *old, nk_size size) {
    (void)user;
    (void)old;
    return malloc(size);
}
static void font_release(nk_handle user, void *memory) {
    (void)user;
    free(memory);
}
int main(void) {
    /* loca[0]=0, loca[1]=8, one contour, endpoint 0, no instructions,
     * one off-curve point with zero x/y deltas. */
    unsigned char bytes[40] = {0};
    bytes[3] = 8;
    bytes[17] = 1;
    bytes[30] = 0x30;
    stbtt_fontinfo info = {0};
    struct nk_allocator allocator = {0};
    allocator.alloc = font_allocate;
    allocator.free = font_release;
    info.userdata = &allocator;
    info.data = bytes;
    info.numGlyphs = 1;
    info.glyf = 16;
    info.loca = 0;
    info.indexToLocFormat = 0;
    stbtt_vertex *vertices = NULL;
    int count = stbtt__GetGlyphShapeTT(&info, 0, &vertices);
    CHECK(count == 2 && vertices);
    CHECK(vertices[0].type == STBTT_vmove && vertices[1].type == STBTT_vcurve);
    CHECK(vertices[0].x == 0 && vertices[0].y == 0 && vertices[1].x == 0 && vertices[1].y == 0);
    STBTT_free(vertices, info.userdata);
    /* Add a second contour at x=10. The first contour must not consume its point. */
    memset(bytes, 0, sizeof bytes);
    bytes[3] = 10;
    bytes[17] = 2;
    bytes[29] = 1;
    bytes[32] = 0x30;
    bytes[33] = 0x33;
    bytes[34] = 10;
    vertices = NULL;
    count = stbtt__GetGlyphShapeTT(&info, 0, &vertices);
    CHECK(count == 4 && vertices);
    CHECK(vertices[0].type == STBTT_vmove && vertices[0].x == 0);
    CHECK(vertices[2].type == STBTT_vmove && vertices[2].x == 10);
    STBTT_free(vertices, info.userdata);
    puts("Font shapes: singleton off-curve contours preserve boundaries");
    return 0;
}
