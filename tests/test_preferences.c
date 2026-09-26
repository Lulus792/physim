#include "physim/data.h"
#include "preferences.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Preferences line %d: %s\n", __LINE__, #x);                            \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static bool bytes_write(const char *path, const void *data, size_t size) {
    FILE *f = fopen(path, "wb");
    if (!f)
        return false;
    bool ok = fwrite(data, 1, size, f) == size;
    return !fclose(f) && ok;
}
static void put32(unsigned char *p, uint32_t v) {
    for (unsigned i = 0; i < 4; i++)
        p[i] = (unsigned char)(v >> (8 * i));
}
int main(int argc, char **argv) {
    CHECK(argc == 2);
    char path[4096], bad[4096];
    snprintf(path, sizeof path, "%s/preferences-unit.bin", argv[1]);
    snprintf(bad, sizeof bad, "%s/preferences-corrupt.bin", argv[1]);
    remove(path);
    remove(bad);
    ps_preferences p = PS_PREFERENCES_DEFAULT, out = p;
    CHECK(ps_preferences_valid(&p) && ps_preferences_read(path, &out) == PS_EOF);
    CHECK(ps_preferences_write(path, &p) == PS_OK);
    p.editor_size = 22;
    p.autosave_seconds = 120;
    p.width = 1200;
    p.height = 800;
    p.maximized = 1;
    p.sidebar_width = 360;
    p.log_height = 340;
    p.view_flags = 127;
    p.workspace = 2;
    p.inspector_open = 1;
    CHECK(ps_preferences_write(path, &p) == PS_OK && ps_preferences_read(path, &out) == PS_OK);
    CHECK(!memcmp(&p, &out, sizeof p));
    ps_preferences invalid = p;
    invalid.editor_size = 17;
    CHECK(ps_preferences_write(path, &invalid) == PS_INVALID);
    CHECK(ps_preferences_read(path, &out) == PS_OK && !memcmp(&p, &out, sizeof p));
    invalid = p;
    invalid.autosave_seconds = 0;
    CHECK(!ps_preferences_valid(&invalid));
    invalid = p;
    invalid.view_flags = 128;
    CHECK(!ps_preferences_valid(&invalid));
    invalid = p;
    invalid.width = 1079;
    CHECK(!ps_preferences_valid(&invalid));
    invalid = p;
    invalid.workspace = 3;
    CHECK(!ps_preferences_valid(&invalid));
    unsigned char bytes[57] = {0};
    FILE *f = fopen(path, "rb");
    CHECK(f && fread(bytes, 1, 56, f) == 56 && !fclose(f));
    for (size_t n = 0; n < 56; n++) {
        CHECK(bytes_write(bad, bytes, n));
        CHECK(ps_preferences_read(bad, &out) == PS_CORRUPT && !memcmp(&p, &out, sizeof p));
    }
    CHECK(bytes_write(bad, bytes, 57) && ps_preferences_read(bad, &out) == PS_CORRUPT);
    for (unsigned i = 0; i < 56; i++) {
        bytes[i] ^= 1;
        CHECK(bytes_write(bad, bytes, 56));
        CHECK(ps_preferences_read(bad, &out) != PS_OK && !memcmp(&p, &out, sizeof p));
        bytes[i] ^= 1;
    }
    put32(bytes + 32, 17);
    put32(bytes + 52, ps_crc32(bytes, 52));
    CHECK(bytes_write(bad, bytes, 56) && ps_preferences_read(bad, &out) == PS_CORRUPT);
    /* Rename failure: a directory cannot be replaced with a settings file. */
    char directory[4096];
    snprintf(directory, sizeof directory, "%s/preferences-target", argv[1]);
    CHECK(SDL_CreateDirectory(directory));
    CHECK(ps_preferences_write(directory, &p) == PS_IO);
    CHECK(ps_preferences_read(path, &out) == PS_OK && !memcmp(&p, &out, sizeof p));
    puts("Preferences: roundtrip, replacement, bounds, CRC, all truncations and failure "
         "preservation passed");
    return 0;
}
