#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700
#endif
#include "text_document.h"
#include "autosave.h"
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_timer.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <io.h>
#include <windows.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif
static ps_document_result document_path(const char *path, char output[4096]) {
#ifdef _WIN32
    FILE *f = fopen(path, "rb");
    if (!f)
        return PS_DOCUMENT_IO;
    wchar_t resolved[4096];
    DWORD n = GetFinalPathNameByHandleW((HANDLE)_get_osfhandle(_fileno(f)), resolved, 4096,
                                        FILE_NAME_NORMALIZED | VOLUME_NAME_DOS);
    fclose(f);
    if (!n)
        return PS_DOCUMENT_IO;
    if (n >= 4096)
        return PS_DOCUMENT_LIMIT;
    /* Keep the long-path prefix. The UTF-8 CRT manifest handles this spelling. */
    return WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, resolved, -1, output, 4096, NULL,
                               NULL)
               ? PS_DOCUMENT_OK
               : PS_DOCUMENT_LIMIT;
#else
    char *resolved = realpath(path, NULL);
    if (!resolved)
        return PS_DOCUMENT_IO;
    size_t n = strlen(resolved);
    if (n < 4096)
        memcpy(output, resolved, n + 1);
    free(resolved);
    return n < 4096 ? PS_DOCUMENT_OK : PS_DOCUMENT_LIMIT;
#endif
}
bool ps_text_document_same_file(const char *left, const char *right) {
    if (!left || !right)
        return false;
    FILE *a = fopen(left, "rb"), *b = fopen(right, "rb");
    bool same = false;
    if (a && b) {
#ifdef _WIN32
        BY_HANDLE_FILE_INFORMATION x, y;
        same = GetFileInformationByHandle((HANDLE)_get_osfhandle(_fileno(a)), &x) &&
               GetFileInformationByHandle((HANDLE)_get_osfhandle(_fileno(b)), &y) &&
               x.dwVolumeSerialNumber == y.dwVolumeSerialNumber &&
               x.nFileIndexHigh == y.nFileIndexHigh && x.nFileIndexLow == y.nFileIndexLow;
#else
        struct stat x, y;
        same = !fstat(fileno(a), &x) && !fstat(fileno(b), &y) && x.st_dev == y.st_dev &&
               x.st_ino == y.st_ino;
#endif
    }
    if (a)
        fclose(a);
    if (b)
        fclose(b);
    return same;
}
void ps_text_document_destroy(ps_text_document *document) {
    if (!document)
        return;
    free(document->saved);
    *document = (ps_text_document){0};
}
ps_result ps_document_draft_path(const char *directory, const char *source, char path[4096]) {
    if (!directory || !*directory || !source || !*source || !path)
        return PS_INVALID;
    /* Hash only locates a bundle. Read validates its complete source path before
     * use, so a hash collision never restores or overwrites another document. */
    uint64_t hash = UINT64_C(14695981039346656037);
    for (const unsigned char *p = (const unsigned char *)source; *p; ++p) {
        hash ^= *p;
        hash *= UINT64_C(1099511628211);
    }
    int n = snprintf(path, 4096, "%s/document-%016llx.draft", directory, (unsigned long long)hash);
    if (n < 0 || n >= 4096) {
        path[0] = 0;
        return PS_LIMIT;
    }
    return PS_OK;
}
ps_result ps_document_draft_read(const char *directory, const char *source, ps_autosave **out) {
    if (!out)
        return PS_INVALID;
    char path[4096];
    ps_result r = ps_document_draft_path(directory, source, path);
    if (r != PS_OK)
        return r;
    ps_autosave *draft = NULL;
    r = ps_autosave_read(path, &draft);
    if (r != PS_OK)
        return r;
    if (strcmp(draft->text[1], source) || strcmp(draft->text[3], "Physim document draft 1")) {
        ps_autosave_destroy(draft);
        return PS_CORRUPT;
    }
    *out = draft;
    return PS_OK;
}
ps_result ps_document_draft_write(const char *directory, const ps_text_document *document,
                                  const char *text, size_t length, uint64_t timestamp) {
    if (!document || !document->saved || !ps_source_text_valid(text, length))
        return PS_INVALID;
    char path[4096];
    ps_result r = ps_document_draft_path(directory, document->path, path);
    if (r != PS_OK)
        return r;
    /* A damaged bundle may have appeared since opening the document. */
    ps_autosave *existing = NULL;
    r = ps_document_draft_read(directory, document->path, &existing);
    ps_autosave_destroy(existing);
    if (r != PS_OK && r != PS_EOF)
        return r;
    if (!SDL_CreateDirectory(directory))
        return PS_IO;
    ps_autosave draft = {0};
    draft.text[0] = (char *)text;
    draft.length[0] = (uint32_t)length;
    draft.text[1] = (char *)document->path;
    draft.length[1] = (uint32_t)strlen(document->path);
    draft.text[2] = document->saved;
    draft.length[2] = (uint32_t)document->length;
    draft.text[3] = "Physim document draft 1";
    draft.length[3] = (uint32_t)strlen(draft.text[3]);
    draft.saved_at_s = timestamp;
    return ps_autosave_write(path, &draft);
}
ps_result ps_document_draft_discard(const char *directory, const char *source) {
    char path[4096];
    ps_result r = ps_document_draft_path(directory, source, path);
    if (r != PS_OK)
        return r;
    ps_autosave *existing = NULL;
    r = ps_autosave_read(path, &existing);
    if (r == PS_EOF)
        return PS_OK;
    if (r == PS_OK) {
        bool foreign = strcmp(existing->text[1], source) != 0;
        ps_autosave_destroy(existing);
        if (foreign)
            return PS_INVALID;
    } else if (r != PS_CORRUPT && r != PS_EOF)
        return r;
    return SDL_RemovePath(path) ? PS_OK : PS_IO;
}
static ps_document_result read_text(const char *path, char **text, size_t *length) {
    SDL_PathInfo info;
    if (!SDL_GetPathInfo(path, &info) || info.type != SDL_PATHTYPE_FILE)
        return PS_DOCUMENT_IO;
    if (info.size > PS_SOURCE_MAX_BYTES)
        return PS_DOCUMENT_LIMIT;
    FILE *f = fopen(path, "rb");
    if (!f)
        return PS_DOCUMENT_IO;
    char *bytes = malloc(PS_SOURCE_MAX_BYTES + 1);
    if (!bytes) {
        fclose(f);
        return PS_DOCUMENT_MEMORY;
    }
    size_t n = fread(bytes, 1, PS_SOURCE_MAX_BYTES + 1, f);
    bool ok = !ferror(f);
    if (fclose(f))
        ok = false;
    ps_document_result r = !ok                               ? PS_DOCUMENT_IO
                           : n > PS_SOURCE_MAX_BYTES         ? PS_DOCUMENT_LIMIT
                           : !ps_source_text_valid(bytes, n) ? PS_DOCUMENT_INVALID
                                                             : PS_DOCUMENT_OK;
    if (r != PS_DOCUMENT_OK) {
        free(bytes);
        return r;
    }
    bytes[n] = 0;
    *text = bytes;
    *length = n;
    return PS_DOCUMENT_OK;
}
ps_document_result ps_text_document_open(ps_text_document *document, const char *path) {
    if (!document || !path || !*path)
        return PS_DOCUMENT_INVALID;
    if (strlen(path) >= sizeof document->path)
        return PS_DOCUMENT_LIMIT;
    char copied_path[4096];
    ps_document_result r = document_path(path, copied_path);
    if (r != PS_DOCUMENT_OK)
        return r;
    char *text = NULL;
    size_t length = 0;
    r = read_text(copied_path, &text, &length);
    if (r != PS_DOCUMENT_OK)
        return r;
    ps_text_document_destroy(document);
    memcpy(document->path, copied_path, strlen(copied_path) + 1);
    document->saved = text;
    document->length = length;
    return PS_DOCUMENT_OK;
}
static bool unchanged(const ps_text_document *document) {
    char *text = NULL;
    size_t length = 0;
    ps_document_result r = read_text(document->path, &text, &length);
    bool same =
        r == PS_DOCUMENT_OK && length == document->length && !memcmp(text, document->saved, length);
    free(text);
    return same;
}
ps_document_result ps_text_document_save(ps_text_document *document, const char *text,
                                         size_t length) {
    if (!document || !document->saved || !ps_source_text_valid(text, length))
        return length > PS_SOURCE_MAX_BYTES ? PS_DOCUMENT_LIMIT : PS_DOCUMENT_INVALID;
    if (!unchanged(document))
        return PS_DOCUMENT_CONFLICT;
    char *copy = malloc(length + 1);
    if (!copy)
        return PS_DOCUMENT_MEMORY;
    if (length)
        memcpy(copy, text, length);
    copy[length] = 0;
    char backup[4096], temporary[4096] = {0}, backup_temporary[4096] = {0};
    ps_document_result r = PS_DOCUMENT_LIMIT;
    int n = snprintf(backup, sizeof backup, "%s.bak", document->path);
    if (n < 0 || (size_t)n >= sizeof backup)
        goto done;
    ps_result written = ps_private_temporary_write(document->path, copy, length, document->path, temporary);
    r = written == PS_OK ? PS_DOCUMENT_OK : written == PS_LIMIT ? PS_DOCUMENT_LIMIT : PS_DOCUMENT_IO;
    if (r != PS_DOCUMENT_OK)
        goto done;
    written = ps_private_temporary_write(backup, document->saved, document->length, document->path, backup_temporary);
    r = written == PS_OK ? PS_DOCUMENT_OK : written == PS_LIMIT ? PS_DOCUMENT_LIMIT : PS_DOCUMENT_IO;
    if (r != PS_DOCUMENT_OK)
        goto done;
    if (!unchanged(document)) {
        r = PS_DOCUMENT_CONFLICT;
        goto done;
    }
    if (!SDL_RenamePath(backup_temporary, backup)) {
        r = PS_DOCUMENT_IO;
        goto done;
    }
    backup_temporary[0] = 0;
    if (!SDL_RenamePath(temporary, document->path)) {
        r = PS_DOCUMENT_IO;
        goto done;
    }
    temporary[0] = 0;
    free(document->saved);
    document->saved = copy;
    document->length = length;
    copy = NULL;
    r = PS_DOCUMENT_OK;
done:
    if (temporary[0])
        SDL_RemovePath(temporary);
    if (backup_temporary[0])
        SDL_RemovePath(backup_temporary);
    free(copy);
    return r;
}
