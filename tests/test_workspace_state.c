#include "physim/data.h"
#include "workspace_state.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Workspace line %d: %s\n", __LINE__, #x);                              \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static bool write_bytes(const char *path, const void *bytes, size_t n) {
    FILE *f = fopen(path, "wb");
    if (!f)
        return false;
    bool ok = fwrite(bytes, 1, n, f) == n;
    return !fclose(f) && ok;
}
static void put32(unsigned char *p, uint32_t v) {
    for (unsigned i = 0; i < 4; i++)
        p[i] = (unsigned char)(v >> (8 * i));
}
int main(int argc, char **argv) {
    CHECK(argc == 2);
    char path[4096], bad[4096], directory[4096];
    snprintf(path, sizeof path, "%s/workspace-unit.bin", argv[1]);
    snprintf(bad, sizeof bad, "%s/workspace-bad.bin", argv[1]);
    snprintf(directory, sizeof directory, "%s/workspace-target", argv[1]);
    remove(path);
    ps_workspace_state *s = calloc(1, sizeof *s), *out = calloc(1, sizeof *out);
    CHECK(s && out);
    CHECK(ps_workspace_state_read(path, out) == PS_EOF);
    CHECK(ps_workspace_state_write(path, s) == PS_OK);
    CHECK(ps_workspace_state_read(path, out) == PS_OK && !memcmp(s, out, sizeof *s));
    CHECK(ps_workspace_absolute("Versuch ä", s->root) == PS_OK);
    CHECK(ps_workspace_absolute("Messdaten/λ.txt", s->additions[0]) == PS_OK);
    s->count = 1;
    CHECK(ps_workspace_state_write(path, s) == PS_OK);
    CHECK(ps_workspace_state_read(path, out) == PS_OK && !memcmp(s, out, sizeof *s));
    unsigned char bytes[16384];
    FILE *f = fopen(path, "rb");
    CHECK(f);
    size_t size = fread(bytes, 1, sizeof bytes - 1, f);
    CHECK(!ferror(f) && !fclose(f) && size < sizeof bytes - 1);
    for (size_t n = 0; n < size; n++) {
        CHECK(write_bytes(bad, bytes, n));
        CHECK(ps_workspace_state_read(bad, out) == PS_CORRUPT);
        CHECK(!memcmp(s, out, sizeof *s));
    }
    for (size_t i = 0; i < size; i++) {
        bytes[i] ^= 1;
        CHECK(write_bytes(bad, bytes, size));
        CHECK(ps_workspace_state_read(bad, out) != PS_OK && !memcmp(s, out, sizeof *s));
        bytes[i] ^= 1;
    }
    bytes[size] = 0;
    CHECK(write_bytes(bad, bytes, size + 1) && ps_workspace_state_read(bad, out) == PS_CORRUPT);
    /* Semantic errors remain invalid even with a recomputed CRC. */
    for (unsigned test = 0; test < 5; test++) {
        unsigned char mutated[16384];
        memcpy(mutated, bytes, size);
        if (test == 0)
            put32(mutated + 12, 33);
        if (test == 1)
            put32(mutated + 16, 4096);
        if (test == 2)
            mutated[20] = 0;
        if (test == 3)
            mutated[20] = 0xff;
        if (test == 4) {
            mutated[20] = '.';
            mutated[21] = '.';
        }
        put32(mutated + size - 4, ps_crc32(mutated, size - 4));
        CHECK(write_bytes(bad, mutated, size));
        CHECK(ps_workspace_state_read(bad, out) == PS_CORRUPT && !memcmp(s, out, sizeof *s));
    }
    s->count = 33;
    CHECK(ps_workspace_state_write(path, s) == PS_INVALID);
    s->count = 1;
    CHECK(ps_workspace_state_read(path, out) == PS_OK && !memcmp(s, out, sizeof *s));
    CHECK(SDL_CreateDirectory(directory));
    CHECK(ps_workspace_state_write(directory, s) == PS_IO);
    CHECK(ps_workspace_state_read(path, out) == PS_OK && !memcmp(s, out, sizeof *s));
    /* Every slot and the maximum path length survive serialization. */
    for (unsigned i = 0; i < PS_WORKSPACE_ADDITIONS; i++) {
        memcpy(s->additions[i], s->root, strlen(s->root));
        memset(s->additions[i] + strlen(s->root), 'a', 4095 - strlen(s->root));
        s->additions[i][4095] = 0;
    }
    s->count = PS_WORKSPACE_ADDITIONS;
    CHECK(ps_workspace_state_write(path, s) == PS_OK);
    CHECK(ps_workspace_state_read(path, out) == PS_OK && !memcmp(s, out, sizeof *s));
    memset(s->additions[0], 'a', 4096);
    CHECK(ps_workspace_state_write(path, s) == PS_INVALID);
    char resolved[4096] = "unchanged", too_long[4097];
    memset(too_long, 'x', sizeof too_long - 1);
    too_long[4096] = 0;
    CHECK(ps_workspace_absolute(too_long, resolved) == PS_LIMIT && !strcmp(resolved, "unchanged"));
    CHECK(ps_workspace_absolute("\xff", resolved) == PS_INVALID && !strcmp(resolved, "unchanged"));
    free(s);
    free(out);
    puts("Workspace state: roundtrip, UTF-8, limits, corruption and failure preservation passed");
    return 0;
}
