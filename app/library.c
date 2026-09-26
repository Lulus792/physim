#include "library.h"
#include <SDL3/SDL_filesystem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static bool filename_valid(const char *text) {
    const unsigned char *p = (const unsigned char *)text;
    while (*p) {
        uint32_t c = *p++;
        unsigned extra = 0;
        uint32_t minimum = 0;
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
        } else if (c >= 0x80)
            return false;
        for (unsigned i = 0; i < extra; i++) {
            if ((*p & 0xc0) != 0x80)
                return false;
            c = (c << 6) | (*p++ & 63);
        }
        if (c < minimum || c > 0x10ffff || (c >= 0xd800 && c <= 0xdfff) || c < 32 ||
            (c >= 0x7f && c <= 0x9f) || c == '/' || c == '\\')
            return false;
    }
    return text[0] != 0;
}
typedef struct {
    ps_library *library;
    ps_result result;
} scan_state;
static SDL_EnumerationResult SDLCALL entry(void *user, const char *directory, const char *name) {
    scan_state *state = user;
    ps_library *library = state->library;
    const char *extension = strrchr(name, '.');
    if (!extension)
        return SDL_ENUM_CONTINUE;
    ps_library_kind kind;
    if (!SDL_strcasecmp(extension, ".psrun"))
        kind = PS_LIBRARY_RUN;
    else if (!SDL_strcasecmp(extension, ".psreport"))
        kind = PS_LIBRARY_REPORT;
    else
        return SDL_ENUM_CONTINUE;
    if (strlen(name) >= sizeof library->items[0].name || !filename_valid(name)) {
        library->skipped++;
        return SDL_ENUM_CONTINUE;
    }
    char path[4096];
    int n = snprintf(path, sizeof path, "%s%s", directory, name);
    SDL_PathInfo info = {0};
    if (n < 0 || (size_t)n >= sizeof path || !SDL_GetPathInfo(path, &info)) {
        library->skipped++;
        return SDL_ENUM_CONTINUE;
    }
    if (info.type != SDL_PATHTYPE_FILE)
        return SDL_ENUM_CONTINUE;
    if (library->count == PS_LIBRARY_MAX_ITEMS) {
        library->limited = true;
        return SDL_ENUM_SUCCESS;
    }
    if (library->count == library->capacity) {
        size_t capacity = library->capacity ? library->capacity * 2 : 128;
        ps_library_item *items = realloc(library->items, capacity * sizeof *items);
        if (!items) {
            state->result = PS_MEMORY;
            return SDL_ENUM_FAILURE;
        }
        library->items = items;
        library->capacity = capacity;
    }
    ps_library_item *item = &library->items[library->count++];
    memset(item, 0, sizeof *item);
    strcpy(item->name, name);
    item->bytes = info.size;
    item->modified_ns = info.modify_time;
    item->kind = kind;
    return SDL_ENUM_CONTINUE;
}
static int compare(const void *left, const void *right) {
    const ps_library_item *a = left, *b = right;
    if (a->modified_ns != b->modified_ns)
        return a->modified_ns > b->modified_ns ? -1 : 1;
    return strcmp(a->name, b->name);
}
ps_result ps_library_scan(const char *directory, ps_library **out) {
    if (!directory || !directory[0] || strlen(directory) >= 4096 || !out)
        return PS_INVALID;
    ps_library *library = calloc(1, sizeof *library);
    if (!library)
        return PS_MEMORY;
    strcpy(library->directory, directory);
    scan_state state = {library, PS_OK};
    bool ok = SDL_EnumerateDirectory(directory, entry, &state);
    if (!ok) {
        ps_library_destroy(library);
        return state.result == PS_OK ? PS_IO : state.result;
    }
    if (library->count > 1)
        qsort(library->items, library->count, sizeof *library->items, compare);
    *out = library;
    return PS_OK;
}
void ps_library_destroy(ps_library *library) {
    if (library) {
        free(library->items);
        free(library);
    }
}
ps_result ps_library_path(const ps_library *library, size_t index, char *out, size_t capacity) {
    if (!library || index >= library->count || !out)
        return PS_INVALID;
    char path[4096];
    int n = snprintf(path, sizeof path, "%s/%s", library->directory, library->items[index].name);
    if (n < 0 || (size_t)n >= sizeof path || (size_t)n >= capacity)
        return PS_LIMIT;
    memcpy(out, path, (size_t)n + 1);
    return PS_OK;
}
