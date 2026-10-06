#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "autosave.h"
#include "physim/data.h"
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_timer.h>
#ifndef _WIN32
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Exclusive creation protects existing regular files, links and directories.
 * POSIX content is private from creation, including while still being written. */
ps_result ps_private_temporary_write(const char *path, const void *bytes, size_t length,
                                     const char *mode_source, char temporary[4096]) {
    temporary[0] = 0;
#ifndef _WIN32
    mode_t mode = 0600;
    struct stat metadata;
    if (mode_source) {
        if (stat(mode_source, &metadata)) return PS_IO;
        mode = metadata.st_mode & 0777; /* Never copy set-ID or sticky bits. */
    }
#else
    (void)mode_source;
#endif
    FILE *f = NULL;
    char candidate[4096];
    for (unsigned i = 0; i < 16 && !f; i++) {
        int n = snprintf(candidate, sizeof candidate, "%s.tmp-%llu-%u", path,
                         (unsigned long long)SDL_GetTicksNS(), i);
        if (n < 0 || n >= 4096) return PS_LIMIT;
#ifdef _WIN32
        f = fopen(candidate, "wbx");
#else
        int fd = open(candidate, O_WRONLY | O_CREAT | O_EXCL, 0600);
        if (fd >= 0) {
            f = fdopen(fd, "wb");
            if (!f) { close(fd); SDL_RemovePath(candidate); return PS_IO; }
        }
#endif
        if (!f && errno != EEXIST) return PS_IO;
    }
    if (!f) return PS_IO;
    bool ok = !length || fwrite(bytes, 1, length, f) == length;
#ifndef _WIN32
    /* Flush content before applying the original access mode; failure never
     * publishes the replacement. Backups use the same original permissions. */
    if (fflush(f) || (ok && fchmod(fileno(f), mode))) ok = false;
#endif
    if (fclose(f)) ok = false;
    if (!ok) { SDL_RemovePath(candidate); return PS_IO; }
    memcpy(temporary, candidate, strlen(candidate) + 1);
    return PS_OK;
}
bool ps_source_text_valid(const char *text, size_t length) {
    if ((!text && length) || length > PS_SOURCE_MAX_BYTES)
        return false;
    for (size_t i = 0; i < length;) {
        uint32_t c = (unsigned char)text[i++], minimum = 0;
        unsigned extra = 0;
        if (c < 128) {
            if (c == 127 || (c < 32 && c != '\t' && c != '\r' && c != '\n'))
                return false;
        } else {
            if (c >= 0xc2 && c <= 0xdf) {
                extra = 1;
                minimum = 0x80;
                c &= 31;
            } else if (c >= 0xe0 && c <= 0xef) {
                extra = 2;
                minimum = 0x800;
                c &= 15;
            } else if (c >= 0xf0 && c <= 0xf4) {
                extra = 3;
                minimum = 0x10000;
                c &= 7;
            } else
                return false;
            if (length - i < extra)
                return false;
            for (unsigned j = 0; j < extra; j++) {
                unsigned char tail = (unsigned char)text[i++];
                if ((tail & 0xc0) != 0x80)
                    return false;
                c = (c << 6) | (tail & 63);
            }
            if (c < minimum || c > 0x10ffff || (c >= 0xd800 && c <= 0xdfff))
                return false;
        }
    }
    return true;
}
static void put32(unsigned char *p, uint32_t v) {
    for (unsigned i = 0; i < 4; i++)
        p[i] = (unsigned char)(v >> (8 * i));
}
static uint32_t get32(const unsigned char *p) {
    uint32_t v = 0;
    for (unsigned i = 0; i < 4; i++)
        v |= (uint32_t)p[i] << (8 * i);
    return v;
}
void ps_autosave_destroy(ps_autosave *s) {
    if (!s)
        return;
    for (unsigned i = 0; i < 4; i++)
        free(s->text[i]);
    free(s);
}
/* 40-byte header: magic, four lengths, timestamp, zero reserved, CRC of header
 * (CRC field zeroed) and all four texts. All integers little-endian. */
ps_result ps_autosave_write(const char *path, const ps_autosave *s) {
    if (!path || !*path || !s)
        return PS_INVALID;
    size_t size = 40;
    for (unsigned i = 0; i < 4; i++) {
        if (!ps_source_text_valid(s->text[i], s->length[i]))
            return PS_INVALID;
        size += s->length[i];
    }
    char temp[4096];
    unsigned char *bytes = calloc(size, 1);
    if (!bytes)
        return PS_MEMORY;
    memcpy(bytes, "PSAUTO01", 8);
    put32(bytes + 24, (uint32_t)s->saved_at_s);
    put32(bytes + 28, (uint32_t)(s->saved_at_s >> 32));
    size_t offset = 40;
    for (unsigned i = 0; i < 4; i++) {
        put32(bytes + 8 + 4 * i, s->length[i]);
        if (s->length[i])
            memcpy(bytes + offset, s->text[i], s->length[i]);
        offset += s->length[i];
    }
    put32(bytes + 36, ps_crc32(bytes, size));
    ps_result result = ps_private_temporary_write(path, bytes, size, NULL, temp);
    free(bytes);
    if (result != PS_OK) return result;
    bool ok = SDL_RenamePath(temp, path);
    if (!ok) SDL_RemovePath(temp);
    return ok ? PS_OK : PS_IO;
}
ps_result ps_autosave_read(const char *path, ps_autosave **out) {
    if (!path || !*path || !out)
        return PS_INVALID;
    FILE *f = fopen(path, "rb");
    if (!f)
        return errno == ENOENT ? PS_EOF : PS_IO;
    unsigned char header[40];
    ps_result r = PS_CORRUPT;
    unsigned char *bytes = NULL;
    ps_autosave *s = NULL;
    if (fread(header, 1, sizeof header, f) != sizeof header)
        goto done;
    if (memcmp(header, "PSAUTO01", 8) || get32(header + 32))
        goto done;
    size_t size = sizeof header;
    for (unsigned i = 0; i < 4; i++) {
        uint32_t n = get32(header + 8 + 4 * i);
        if (n > PS_SOURCE_MAX_BYTES)
            goto done;
        size += n;
    }
    bytes = malloc(size);
    if (!bytes) {
        r = PS_MEMORY;
        goto done;
    }
    memcpy(bytes, header, sizeof header);
    if (fread(bytes + sizeof header, 1, size - sizeof header, f) != size - sizeof header ||
        fgetc(f) != EOF || ferror(f))
        goto done;
    uint32_t crc = get32(bytes + 36);
    put32(bytes + 36, 0);
    if (ps_crc32(bytes, size) != crc)
        goto done;
    s = calloc(1, sizeof *s);
    if (!s) {
        r = PS_MEMORY;
        goto done;
    }
    s->saved_at_s = get32(bytes + 24) | ((uint64_t)get32(bytes + 28) << 32);
    size_t offset = sizeof header;
    for (unsigned i = 0; i < 4; i++) {
        s->length[i] = get32(bytes + 8 + 4 * i);
        if (!ps_source_text_valid((char *)bytes + offset, s->length[i]))
            goto done;
        s->text[i] = malloc((size_t)s->length[i] + 1);
        if (!s->text[i]) {
            r = PS_MEMORY;
            goto done;
        }
        memcpy(s->text[i], bytes + offset, s->length[i]);
        s->text[i][s->length[i]] = 0;
        offset += s->length[i];
    }
    *out = s;
    s = NULL;
    r = PS_OK;
done:
    if (ferror(f))
        r = PS_IO;
    fclose(f);
    free(bytes);
    ps_autosave_destroy(s);
    return r;
}
