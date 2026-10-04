#include "workspace_state.h"
#include "autosave.h"
#include "physim/data.h"
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool absolute(const char *path) {
#ifdef _WIN32
    return ((path[0] >= 'A' && path[0] <= 'Z') || (path[0] >= 'a' && path[0] <= 'z'))
               ? path[1] == ':' && (path[2] == '/' || path[2] == '\\')
               : (path[0] == '\\' && path[1] == '\\');
#else
    return path[0] == '/';
#endif
}
static bool path_valid(const char *path, bool empty) {
    const char *end = memchr(path, 0, PS_WORKSPACE_PATH);
    if (!end)
        return false;
    size_t n = (size_t)(end - path);
    return n == 0 ? empty : absolute(path) && ps_source_text_valid(path, n);
}
ps_result ps_workspace_absolute(const char *path, char out[PS_WORKSPACE_PATH]) {
    if (!path || !*path || !out)
        return PS_INVALID;
    char resolved[PS_WORKSPACE_PATH];
    int n;
    if (absolute(path))
        n = snprintf(resolved, sizeof resolved, "%s", path);
    else {
#ifdef _WIN32
        /* Drive-relative and root-relative Windows paths have separate rules. */
        if (strchr(path, ':') || path[0] == '/' || path[0] == '\\')
            return PS_INVALID;
#endif
        char *directory = SDL_GetCurrentDirectory();
        if (!directory)
            return PS_IO;
        n = snprintf(resolved, sizeof resolved, "%s%s", directory, path);
        SDL_free(directory);
    }
    if (n < 0 || (size_t)n >= sizeof resolved)
        return PS_LIMIT;
    if (!path_valid(resolved, false))
        return PS_INVALID;
    memcpy(out, resolved, (size_t)n + 1);
    return PS_OK;
}
static void put32(unsigned char *p, uint32_t v) {
    for (unsigned i = 0; i < 4; i++)
        p[i] = (unsigned char)(v >> (8 * i));
}
static uint32_t get32(const unsigned char *p) {
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}
static bool editor_valid(const ps_workspace_editor *e) {
    return e->cursor <= PS_WORKSPACE_TEXT_LIMIT && e->select_start <= PS_WORKSPACE_TEXT_LIMIT &&
           e->select_end <= PS_WORKSPACE_TEXT_LIMIT && e->scroll_x <= PS_WORKSPACE_SCROLL_LIMIT &&
           e->scroll_y <= PS_WORKSPACE_SCROLL_LIMIT;
}
static void editor_read(const unsigned char *p, ps_workspace_editor *e) {
    e->cursor = get32(p);
    e->select_start = get32(p + 4);
    e->select_end = get32(p + 8);
    e->scroll_x = get32(p + 12);
    e->scroll_y = get32(p + 16);
}
static void editor_write(unsigned char *p, const ps_workspace_editor *e) {
    put32(p, e->cursor);
    put32(p + 4, e->select_start);
    put32(p + 8, e->select_end);
    put32(p + 12, e->scroll_x);
    put32(p + 16, e->scroll_y);
}
static bool views_valid(const ps_workspace_state *s) {
    if (s->document_count > PS_WORKSPACE_DOCUMENTS || s->view > 3 || s->analysis_editor > 1 ||
        (s->document_count ? s->active_document >= s->document_count : s->active_document != 0) ||
        (s->view == 3 && !s->document_count) ||
        (!s->root[0] && (s->document_count || s->view || s->analysis_editor)) ||
        !editor_valid(&s->experiment) || !editor_valid(&s->analysis))
        return false;
    for (uint32_t i = 0; i < s->document_count; i++)
        if (!path_valid(s->documents[i].path, false) || !editor_valid(&s->documents[i].editor))
            return false;
    return true;
}
enum { MAX_BYTES = 24 + PS_WORKSPACE_PATH + PS_WORKSPACE_ADDITIONS * (4 + PS_WORKSPACE_PATH) +
                   56 + PS_WORKSPACE_DOCUMENTS * (24 + PS_WORKSPACE_PATH) };
ps_result ps_workspace_state_read(const char *path, ps_workspace_state *out) {
    if (!path || !*path || !out)
        return PS_INVALID;
    FILE *f = fopen(path, "rb");
    if (!f)
        return errno == ENOENT ? PS_EOF : PS_IO;
    unsigned char *bytes = malloc(MAX_BYTES);
    ps_workspace_state *state = calloc(1, sizeof *state);
    if (!bytes || !state) {
        free(bytes);
        free(state);
        fclose(f);
        return PS_MEMORY;
    }
    size_t n = fread(bytes, 1, MAX_BYTES, f);
    bool ok = fgetc(f) == EOF && !ferror(f);
    if (fclose(f))
        ok = false;
    ps_result result = PS_CORRUPT;
    if (!ok || n < 24 || memcmp(bytes, "PSWORK", 6))
        goto done;
    bool views = !memcmp(bytes + 6, "02", 2);
    if (!views && memcmp(bytes + 6, "01", 2)) {
        result = PS_VERSION;
        goto done;
    }
    if (get32(bytes + 8) != n || get32(bytes + n - 4) != ps_crc32(bytes, n - 4))
        goto done;
    state->count = get32(bytes + 12);
    if (state->count > PS_WORKSPACE_ADDITIONS)
        goto done;
    size_t offset = 16;
    for (uint32_t i = 0; i <= state->count; i++) {
        if (n - 4 - offset < 4)
            goto done;
        uint32_t length = get32(bytes + offset);
        offset += 4;
        if (length >= PS_WORKSPACE_PATH || length > n - 4 - offset ||
            memchr(bytes + offset, 0, length))
            goto done;
        char *target = i == 0 ? state->root : state->additions[i - 1];
        memcpy(target, bytes + offset, length);
        offset += length;
        if (!path_valid(target, i == 0 && state->count == 0))
            goto done;
    }
    if (views) {
        if (n - 4 - offset < 56)
            goto done;
        state->document_count = get32(bytes + offset);
        state->active_document = get32(bytes + offset + 4);
        state->view = get32(bytes + offset + 8);
        state->analysis_editor = get32(bytes + offset + 12);
        editor_read(bytes + offset + 16, &state->experiment);
        editor_read(bytes + offset + 36, &state->analysis);
        offset += 56;
        if (state->document_count > PS_WORKSPACE_DOCUMENTS)
            goto done;
        for (uint32_t i = 0; i < state->document_count; i++) {
            if (n - 4 - offset < 4)
                goto done;
            uint32_t length = get32(bytes + offset);
            offset += 4;
            if (length >= PS_WORKSPACE_PATH || length > n - 4 - offset ||
                memchr(bytes + offset, 0, length))
                goto done;
            memcpy(state->documents[i].path, bytes + offset, length);
            offset += length;
            if (n - 4 - offset < 20)
                goto done;
            editor_read(bytes + offset, &state->documents[i].editor);
            offset += 20;
        }
    }
    if (offset != n - 4 || !views_valid(state))
        goto done;
    *out = *state;
    result = PS_OK;
done:
    free(bytes);
    free(state);
    return result;
}
ps_result ps_workspace_state_write(const char *path, const ps_workspace_state *state) {
    if (!path || !*path || !state || state->count > PS_WORKSPACE_ADDITIONS ||
        !path_valid(state->root, state->count == 0) || !views_valid(state))
        return PS_INVALID;
    size_t size = 24 + strlen(state->root) + 56;
    for (uint32_t i = 0; i < state->count; i++) {
        if (!path_valid(state->additions[i], false))
            return PS_INVALID;
        size += 4 + strlen(state->additions[i]);
    }
    for (uint32_t i = 0; i < state->document_count; i++)
        size += 24 + strlen(state->documents[i].path);
    unsigned char *bytes = calloc(size, 1);
    if (!bytes)
        return PS_MEMORY;
    memcpy(bytes, "PSWORK02", 8);
    put32(bytes + 8, (uint32_t)size);
    put32(bytes + 12, state->count);
    size_t offset = 16;
    for (uint32_t i = 0; i <= state->count; i++) {
        const char *source = i == 0 ? state->root : state->additions[i - 1];
        size_t length = strlen(source);
        put32(bytes + offset, (uint32_t)length);
        memcpy(bytes + offset + 4, source, length);
        offset += 4 + length;
    }
    put32(bytes + offset, state->document_count);
    put32(bytes + offset + 4, state->active_document);
    put32(bytes + offset + 8, state->view);
    put32(bytes + offset + 12, state->analysis_editor);
    editor_write(bytes + offset + 16, &state->experiment);
    editor_write(bytes + offset + 36, &state->analysis);
    offset += 56;
    for (uint32_t i = 0; i < state->document_count; i++) {
        const ps_workspace_document *doc = &state->documents[i];
        size_t length = strlen(doc->path);
        put32(bytes + offset, (uint32_t)length);
        memcpy(bytes + offset + 4, doc->path, length);
        offset += 4 + length;
        editor_write(bytes + offset, &doc->editor);
        offset += 20;
    }
    put32(bytes + size - 4, ps_crc32(bytes, size - 4));
    char temporary[PS_WORKSPACE_PATH];
    FILE *f = NULL;
    ps_result result = PS_IO;
    for (unsigned i = 0; i < 16 && !f; i++) {
        int n = snprintf(temporary, sizeof temporary, "%s.tmp-%llu-%u", path,
                         (unsigned long long)SDL_GetTicksNS(), i);
        if (n < 0 || (size_t)n >= sizeof temporary) {
            result = PS_LIMIT;
            goto done;
        }
        f = fopen(temporary, "wbx");
        if (!f && errno != EEXIST)
            goto done;
    }
    if (!f)
        goto done;
    bool ok = fwrite(bytes, 1, size, f) == size;
    if (fclose(f))
        ok = false;
    if (ok)
        ok = SDL_RenamePath(temporary, path);
    if (!ok)
        SDL_RemovePath(temporary);
    result = ok ? PS_OK : PS_IO;
done:
    free(bytes);
    return result;
}
