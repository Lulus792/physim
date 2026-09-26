#include "autosave.h"
#include "physim/data.h"
#include <SDL3/SDL_filesystem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Autosave line %d: %s\n", __LINE__, #x);                               \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static bool write_bytes(const char *path, const void *bytes, size_t n) {
    FILE *f = fopen(path, "wb");
    if (!f)
        return false;
    bool ok = fwrite(bytes, 1, n, f) == n;
    return fclose(f) == 0 && ok;
}
static void put32(unsigned char *p, uint32_t n) {
    for (unsigned i = 0; i < 4; i++)
        p[i] = (unsigned char)(n >> (8 * i));
}
int main(void) {
    const char *dir = "autosave fixture ä", *path = "autosave fixture ä/draft.psauto";
    CHECK(SDL_CreateDirectory(dir));
    ps_autosave original = {
        {"// Änderung α\nint a = 2;\n", "// Analyse 🚀\n", "int a = 1;\n", ""}, {0}, 1789580000};
    for (unsigned i = 0; i < 4; i++)
        original.length[i] = (uint32_t)strlen(original.text[i]);
    ps_autosave *s = NULL;
    CHECK(ps_autosave_read("autosave fixture ä/absent", &s) == PS_EOF && !s);
    CHECK(ps_autosave_write(path, &original) == PS_OK);
    CHECK(ps_autosave_read(path, &s) == PS_OK && s->saved_at_s == original.saved_at_s);
    for (unsigned i = 0; i < 4; i++)
        CHECK(s->length[i] == original.length[i] && !strcmp(s->text[i], original.text[i]));
    ps_autosave *old = s;
    FILE *f = fopen(path, "rb");
    CHECK(f != NULL);
    unsigned char bytes[1024];
    size_t size = fread(bytes, 1, sizeof bytes, f);
    CHECK(feof(f) && !ferror(f) && !fclose(f) && size > 40);
    /* Every interrupted write must fail transactionally, including in each text. */
    const char *bad = "autosave fixture ä/bad.psauto";
    for (size_t n = 0; n < size; n++) {
        CHECK(write_bytes(bad, bytes, n));
        CHECK(ps_autosave_read(bad, &s) == PS_CORRUPT && s == old);
    }
    for (size_t i = 0; i < size; i++) {
        bytes[i] ^= 0x80;
        CHECK(write_bytes(bad, bytes, size));
        CHECK(ps_autosave_read(bad, &s) == PS_CORRUPT && s == old);
        bytes[i] ^= 0x80;
    }
    bytes[size] = 0;
    CHECK(write_bytes(bad, bytes, size + 1));
    CHECK(ps_autosave_read(bad, &s) == PS_CORRUPT && s == old);
    /* Valid CRC cannot make illegal UTF-8 or embedded NUL valid source text. */
    unsigned char saved = bytes[40];
    for (unsigned i = 0; i < 2; i++) {
        bytes[40] = i ? 0xff : 0;
        put32(bytes + 36, 0);
        put32(bytes + 36, ps_crc32(bytes, size));
        CHECK(write_bytes(bad, bytes, size));
        CHECK(ps_autosave_read(bad, &s) == PS_CORRUPT && s == old);
    }
    bytes[40] = saved;
    /* Failed temporary-file creation preserves the previous complete generation. */
    CHECK(SDL_CreateDirectory("autosave fixture ä/draft.psauto.tmp"));
    original.saved_at_s++;
    CHECK(ps_autosave_write(path, &original) == PS_IO);
    ps_autosave *preserved = NULL;
    CHECK(ps_autosave_read(path, &preserved) == PS_OK &&
          preserved->saved_at_s + 1 == original.saved_at_s &&
          !strcmp(preserved->text[0], old->text[0]));
    ps_autosave_destroy(preserved);
    CHECK(SDL_RemovePath("autosave fixture ä/draft.psauto.tmp"));
    CHECK(SDL_CreateDirectory("autosave fixture ä/occupied.psauto"));
    CHECK(ps_autosave_write("autosave fixture ä/occupied.psauto", &original) == PS_IO);
    CHECK(SDL_RemovePath("autosave fixture ä/occupied.psauto.tmp"));
    CHECK(SDL_RemovePath("autosave fixture ä/occupied.psauto"));
    CHECK(ps_autosave_write(path, &original) == PS_OK);
    CHECK(ps_autosave_read(path, &preserved) == PS_OK &&
          preserved->saved_at_s == original.saved_at_s);
    ps_autosave_destroy(preserved);
    const char *invalid[] = {"\xc0\xaf", "\xed\xa0\x80", "\xf4\x90\x80\x80", "\xe2\x82", "\x80",
                             "\x01",     "\x7f"};
    for (unsigned i = 0; i < sizeof invalid / sizeof invalid[0]; i++)
        CHECK(!ps_source_text_valid(invalid[i], strlen(invalid[i])));
    CHECK(ps_source_text_valid(NULL, 0) && ps_source_text_valid("\t\r\n", 3));
    char *large = malloc(PS_SOURCE_MAX_BYTES + 1);
    CHECK(large != NULL);
    memset(large, 'x', PS_SOURCE_MAX_BYTES + 1);
    ps_autosave maximum = {{large, large, large, large}, {0}, 0};
    for (unsigned i = 0; i < 4; i++)
        maximum.length[i] = PS_SOURCE_MAX_BYTES;
    CHECK(ps_autosave_write(path, &maximum) == PS_OK);
    CHECK(ps_autosave_read(path, &preserved) == PS_OK);
    for (unsigned i = 0; i < 4; i++)
        CHECK(preserved->length[i] == PS_SOURCE_MAX_BYTES &&
              !memcmp(preserved->text[i], large, PS_SOURCE_MAX_BYTES));
    ps_autosave_destroy(preserved);
    maximum.length[0]++;
    CHECK(ps_autosave_write(path, &maximum) == PS_INVALID);
    free(large);
    ps_autosave_destroy(old);
    CHECK(SDL_RemovePath(path) && SDL_RemovePath(bad) && SDL_RemovePath(dir));
    puts("Autosave: UTF-8, baselines, all truncations, mutations, failed replace and size bounds "
         "passed");
    return 0;
}
