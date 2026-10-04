#include "preferences.h"
#include "physim/data.h"
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_timer.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
const ps_preferences PS_PREFERENCES_DEFAULT = {1440, 940, 0, 248, 178, 16, 30, 31, 0, 0,
                                               PS_THEME_DARK};
bool ps_preferences_valid(const ps_preferences *p) {
    return p && p->width >= 1080 && p->width <= 8192 && p->height >= 740 && p->height <= 8192 &&
           p->maximized <= 1 && p->sidebar_width >= 208 && p->sidebar_width <= 360 &&
           p->log_height >= 150 && p->log_height <= 340 && p->editor_size >= 16 &&
           p->editor_size <= 22 && p->editor_size % 2 == 0 &&
           (p->autosave_seconds == 10 || p->autosave_seconds == 30 || p->autosave_seconds == 60 ||
            p->autosave_seconds == 120) &&
           !(p->view_flags & ~127u) && p->inspector_open <= 1 && p->workspace <= 2 &&
           p->theme < PS_THEME_COUNT;
}
static void put32(unsigned char *p, uint32_t v) {
    for (unsigned i = 0; i < 4; i++)
        p[i] = (unsigned char)(v >> (8 * i));
}
static uint32_t get32(const unsigned char *p) {
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}
ps_result ps_preferences_read(const char *path, ps_preferences *out) {
    if (!path || !*path || !out)
        return PS_INVALID;
    FILE *f = fopen(path, "rb");
    if (!f)
        return errno == ENOENT ? PS_EOF : PS_IO;
    unsigned char bytes[60];
    size_t n = fread(bytes, 1, sizeof bytes, f);
    bool ok = (n == 56 || n == 60) && fgetc(f) == EOF && !ferror(f);
    if (fclose(f))
        ok = false;
    if (!ok || memcmp(bytes, "PSPREF", 6))
        return PS_CORRUPT;
    bool legacy = !memcmp(bytes + 6, "01", 2);
    if (!legacy && memcmp(bytes + 6, "02", 2))
        return PS_VERSION;
    size_t payload = legacy ? 40 : 44, crc = 12 + payload;
    if (n != crc + 4 || get32(bytes + 8) != payload || get32(bytes + crc) != ps_crc32(bytes, crc))
        return PS_CORRUPT;
    ps_preferences p = {get32(bytes + 12), get32(bytes + 16), get32(bytes + 20), get32(bytes + 24),
                        get32(bytes + 28), get32(bytes + 32), get32(bytes + 36), get32(bytes + 40),
                        get32(bytes + 44), get32(bytes + 48), legacy ? PS_THEME_DARK : get32(bytes + 52)};
    if (!ps_preferences_valid(&p))
        return PS_CORRUPT;
    *out = p;
    return PS_OK;
}
ps_result ps_preferences_write(const char *path, const ps_preferences *p) {
    if (!path || !*path || !ps_preferences_valid(p))
        return PS_INVALID;
    unsigned char bytes[60] = {0};
    memcpy(bytes, "PSPREF02", 8);
    put32(bytes + 8, 44);
    uint32_t fields[] = {p->width,          p->height,      p->maximized,        p->sidebar_width,
                         p->log_height,     p->editor_size, p->autosave_seconds, p->view_flags,
                         p->inspector_open, p->workspace, p->theme};
    for (unsigned i = 0; i < 11; i++)
        put32(bytes + 12 + 4 * i, fields[i]);
    put32(bytes + 56, ps_crc32(bytes, 56));
    char temporary[4096];
    FILE *f = NULL;
    for (unsigned i = 0; i < 16 && !f; i++) {
        int n = snprintf(temporary, sizeof temporary, "%s.tmp-%llu-%u", path,
                         (unsigned long long)SDL_GetTicksNS(), i);
        if (n < 0 || (size_t)n >= sizeof temporary)
            return PS_LIMIT;
        f = fopen(temporary, "wbx");
        if (!f && errno != EEXIST)
            return PS_IO;
    }
    if (!f)
        return PS_IO;
    bool ok = fwrite(bytes, 1, sizeof bytes, f) == sizeof bytes;
    if (fclose(f))
        ok = false;
    if (ok)
        ok = SDL_RenamePath(temporary, path);
    if (!ok)
        SDL_RemovePath(temporary);
    return ok ? PS_OK : PS_IO;
}
