#include "preferences.h"
#include "physim/data.h"
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_timer.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
const ps_preferences PS_PREFERENCES_DEFAULT = {1440, 940, 0, 248, 178, 16, 30, 31, 0, 0,
                                               PS_THEME_DARK, 280, PS_DOCK_DEFAULT_INITIALIZER};
bool ps_preferences_valid(const ps_preferences *p) {
    return p && p->width >= 1080 && p->width <= 8192 && p->height >= 740 && p->height <= 8192 &&
           p->maximized <= 1 && p->sidebar_width >= 208 && p->sidebar_width <= 360 &&
           p->log_height >= 150 && p->log_height <= 340 && p->editor_size >= 16 &&
           p->editor_size <= 22 && p->editor_size % 2 == 0 &&
           (p->autosave_seconds == 10 || p->autosave_seconds == 30 || p->autosave_seconds == 60 ||
            p->autosave_seconds == 120) &&
           !(p->view_flags & ~127u) && p->inspector_open <= 1 && p->workspace <= 2 &&
           p->theme < PS_THEME_COUNT && p->inspector_width>=208 && p->inspector_width<=480 &&
           ps_dock_valid(&p->dock);
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
    unsigned char bytes[308];
    size_t n = fread(bytes, 1, sizeof bytes, f);
    bool ok = (n == 56 || n == 60 || n == 240 || n==308) && fgetc(f) == EOF && !ferror(f);
    if (fclose(f))
        ok = false;
    if (!ok || memcmp(bytes, "PSPREF", 6))
        return PS_CORRUPT;
    bool legacy = !memcmp(bytes + 6, "01", 2);
    bool second = !memcmp(bytes + 6, "02", 2);
    bool third = !memcmp(bytes + 6, "03", 2);
    if (!legacy && !second && !third && memcmp(bytes + 6, "04", 2)) return PS_VERSION;
    size_t payload = legacy ? 40 : second ? 44 : third?224:292, crc = 12 + payload;
    if (n != crc + 4 || get32(bytes + 8) != payload || get32(bytes + crc) != ps_crc32(bytes, crc))
        return PS_CORRUPT;
    ps_preferences p = {get32(bytes + 12), get32(bytes + 16), get32(bytes + 20), get32(bytes + 24),
                        get32(bytes + 28), get32(bytes + 32), get32(bytes + 36), get32(bytes + 40),
                        get32(bytes + 44), get32(bytes + 48), legacy ? PS_THEME_DARK : get32(bytes + 52),
                        legacy || second || third?280:get32(bytes+56), PS_DOCK_DEFAULT_INITIALIZER};
    if(!legacy && !second && !third) {
        if(!ps_dock_decode(bytes+60,PS_DOCK_WIRE_BYTES,&p.dock))return PS_CORRUPT;
    } else if (third) {
        const unsigned char *at = bytes + 56;
        memset(p.dock.nodes,0,sizeof p.dock.nodes);
        for (unsigned i = 0; i < 5; i++, at += 24)
            p.dock.nodes[i] = (ps_dock_node){get32(at),get32(at+4),get32(at+8),get32(at+12),get32(at+16),get32(at+20)};
        p.dock.root=get32(at); p.dock.floating=get32(at+4); p.dock.hidden=get32(at+8); at+=12;
        for (unsigned i=0;i<3;i++,at+=16)
            p.dock.floats[i]=(ps_dock_float){get32(at),get32(at+4),get32(at+8),get32(at+12)};
        if((p.dock.floating|p.dock.hidden)&~7u)return PS_CORRUPT;
        for(unsigned i=0;i<5;i++)
            if((p.dock.nodes[i].panels&~7u) ||
               (p.dock.nodes[i].kind==PS_DOCK_GROUP && p.dock.nodes[i].active>=3)) return PS_CORRUPT;
        p.dock.hidden|=1u<<PS_DOCK_INSPECTOR;
    }
    if (!ps_preferences_valid(&p))
        return PS_CORRUPT;
    *out = p;
    return PS_OK;
}
ps_result ps_preferences_write(const char *path, const ps_preferences *p) {
    if (!path || !*path || !ps_preferences_valid(p))
        return PS_INVALID;
    unsigned char bytes[308] = {0};
    memcpy(bytes, "PSPREF04", 8);
    put32(bytes + 8, 292);
    uint32_t fields[] = {p->width,          p->height,      p->maximized,        p->sidebar_width,
                         p->log_height,     p->editor_size, p->autosave_seconds, p->view_flags,
                         p->inspector_open, p->workspace, p->theme,p->inspector_width};
    for (unsigned i = 0; i < 12; i++)
        put32(bytes + 12 + 4 * i, fields[i]);
    if(!ps_dock_encode(&p->dock,bytes+60,PS_DOCK_WIRE_BYTES))return PS_INVALID;
    put32(bytes+304,ps_crc32(bytes,304));
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
