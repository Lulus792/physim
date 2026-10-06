#include "autosave.h"
#include "text_document.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#endif
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Document line %d: %s\n", __LINE__, #x);                               \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static bool write_bytes(const char *path, const char *text, size_t length) {
    FILE *f = fopen(path, "wb");
    if (!f)
        return false;
    bool ok = !length || fwrite(text, 1, length, f) == length;
    return !fclose(f) && ok;
}
static bool matches(const char *path, const char *expected) {
    FILE *f = fopen(path, "rb");
    if (!f)
        return false;
    char buffer[256];
    size_t n = fread(buffer, 1, sizeof buffer, f);
    bool ok = !ferror(f) && n == strlen(expected) && !memcmp(buffer, expected, n);
    fclose(f);
    return ok;
}
int main(int argc, char **argv) {
    CHECK(argc == 2);
    char root[4096], path[4096], backup[4096], other[4096], alias[4096];
    snprintf(root, sizeof root, "%s/doc-%llu", argv[1], (unsigned long long)SDL_GetTicksNS());
    CHECK(SDL_CreateDirectory(root));
    snprintf(path, sizeof path, "%s/notes ä.txt", root);
    snprintf(backup, sizeof backup, "%s.bak", path);
    snprintf(other, sizeof other, "%s/other", root);
    snprintf(alias, sizeof alias, "%s/alias.txt", root);
    CHECK(write_bytes(path, "α = 1\r\n", strlen("α = 1\r\n")));
    ps_text_document doc = {0};
    CHECK(ps_text_document_open(&doc, path) == PS_DOCUMENT_OK);
    CHECK(!strcmp(doc.saved, "α = 1\r\n") && doc.length == strlen(doc.saved));
    CHECK(ps_text_document_same_file(path, doc.path));
#ifdef _WIN32
    CHECK(CreateHardLinkA(alias, path, NULL));
#else
    CHECK(!link(path, alias));
#endif
    CHECK(ps_text_document_same_file(alias, path));
    CHECK(!ps_text_document_same_file(other, path));
    CHECK(SDL_RemovePath(alias));
    CHECK(ps_text_document_save(&doc, "β = 2\n", strlen("β = 2\n")) == PS_DOCUMENT_OK);
    CHECK(matches(path, "β = 2\n") && matches(backup, "α = 1\r\n"));
    char drafts[4096], draft_path[4096], temporary[4096];
    snprintf(drafts, sizeof drafts, "%s-app-data", root);
    CHECK(ps_document_draft_path(drafts, doc.path, draft_path) == PS_OK);
    ps_autosave *draft = NULL;
    CHECK(ps_document_draft_read(drafts, doc.path, &draft) == PS_EOF && !draft);
    CHECK(ps_document_draft_discard(drafts, doc.path) == PS_OK);
    CHECK(ps_document_draft_write(drafts, &doc, "draft γ", strlen("draft γ"), 123) == PS_OK);
    CHECK(matches(path, "β = 2\n") && !strcmp(doc.saved, "β = 2\n"));
    CHECK(ps_document_draft_read(drafts, doc.path, &draft) == PS_OK);
    CHECK(!strcmp(draft->text[0], "draft γ") && !strcmp(draft->text[2], doc.saved) &&
          draft->saved_at_s == 123);
    ps_autosave_destroy(draft);
    draft = NULL;
    snprintf(temporary, sizeof temporary, "%s.tmp", draft_path);
    CHECK(SDL_CreateDirectory(temporary));
    CHECK(ps_document_draft_write(drafts, &doc, "new", 3, 124) == PS_OK);
    CHECK(ps_document_draft_read(drafts, doc.path, &draft) == PS_OK);
    CHECK(!strcmp(draft->text[0], "new"));
    CHECK(SDL_RemovePath(temporary));
    /* A valid bundle for another source at the same cache key is never replaced. */
    char *original_path = draft->text[1];
    draft->text[1] = other;
    draft->length[1] = (uint32_t)strlen(other);
    CHECK(ps_autosave_write(draft_path, draft) == PS_OK);
    draft->text[1] = original_path;
    draft->length[1] = (uint32_t)strlen(original_path);
    ps_autosave *foreign = NULL;
    CHECK(ps_document_draft_read(drafts, doc.path, &foreign) == PS_CORRUPT && !foreign);
    CHECK(ps_document_draft_write(drafts, &doc, "new", 3, 124) == PS_CORRUPT);
    CHECK(ps_document_draft_discard(drafts, doc.path) == PS_INVALID);
    CHECK(ps_autosave_write(draft_path, draft) == PS_OK);
    ps_autosave_destroy(draft);
    draft = NULL;
    CHECK(write_bytes(draft_path, "damaged", 7));
    CHECK(ps_document_draft_read(drafts, doc.path, &draft) == PS_CORRUPT && !draft);
    CHECK(ps_document_draft_write(drafts, &doc, "new", 3, 125) == PS_CORRUPT);
    CHECK(matches(draft_path, "damaged"));
    CHECK(ps_document_draft_discard(drafts, doc.path) == PS_OK);
    CHECK(ps_document_draft_write(drafts, &doc, NULL, 0, 126) == PS_OK);
    CHECK(ps_document_draft_read(drafts, doc.path, &draft) == PS_OK && draft->length[0] == 0);
    ps_autosave_destroy(draft);
    CHECK(ps_document_draft_discard(drafts, doc.path) == PS_OK);
    CHECK(write_bytes(path, "external", 8));
    CHECK(ps_text_document_save(&doc, "local", 5) == PS_DOCUMENT_CONFLICT);
    CHECK(matches(path, "external") && !strcmp(doc.saved, "β = 2\n"));
    CHECK(ps_text_document_open(&doc, doc.path) == PS_DOCUMENT_OK);
    CHECK(!strcmp(doc.saved, "external"));
    CHECK(SDL_RemovePath(backup) && SDL_CreateDirectory(backup));
    CHECK(ps_text_document_save(&doc, "local", 5) == PS_DOCUMENT_IO);
    CHECK(matches(path, "external") && !strcmp(doc.saved, "external"));
    CHECK(SDL_RemovePath(backup));
    CHECK(ps_text_document_save(&doc, NULL, 0) == PS_DOCUMENT_OK);
    CHECK(matches(path, "") && matches(backup, "external"));
#ifndef _WIN32
    const unsigned modes[]={0600,0640,0700,0755};
    for(unsigned i=0;i<4;i++) {
        CHECK(!chmod(path,modes[i]));
        CHECK(ps_text_document_save(&doc,"mode",4)==PS_DOCUMENT_OK);
        struct stat source_info,backup_info;
        CHECK(!stat(path,&source_info) && !stat(backup,&backup_info));
        CHECK((source_info.st_mode&0777)==modes[i] && (backup_info.st_mode&0777)==modes[i]);
    }
#endif
    char *large = malloc(PS_SOURCE_MAX_BYTES + 1);
    CHECK(large);
    memset(large, 'a', PS_SOURCE_MAX_BYTES + 1);
    CHECK(ps_text_document_save(&doc, large, PS_SOURCE_MAX_BYTES) == PS_DOCUMENT_OK);
    CHECK(doc.length == PS_SOURCE_MAX_BYTES);
    CHECK(ps_text_document_save(&doc, large, PS_SOURCE_MAX_BYTES + 1) == PS_DOCUMENT_LIMIT);
    CHECK(write_bytes(other, large, PS_SOURCE_MAX_BYTES + 1));
    CHECK(ps_text_document_open(&doc, other) == PS_DOCUMENT_LIMIT &&
          doc.length == PS_SOURCE_MAX_BYTES);
    free(large);
    const char *invalid[] = {"\xff", "a\0b", "\x01", "\xed\xa0\x80"};
    const size_t lengths[] = {1, 3, 1, 3};
    for (unsigned i = 0; i < 4; i++) {
        CHECK(write_bytes(other, invalid[i], lengths[i]));
        CHECK(ps_text_document_open(&doc, other) == PS_DOCUMENT_INVALID);
        CHECK(ps_text_document_save(&doc, invalid[i], lengths[i]) == PS_DOCUMENT_INVALID);
        CHECK(doc.length == PS_SOURCE_MAX_BYTES);
    }
    CHECK(SDL_RemovePath(path));
    CHECK(ps_text_document_save(&doc, "local", 5) == PS_DOCUMENT_CONFLICT);
    CHECK(doc.length == PS_SOURCE_MAX_BYTES);
    CHECK(ps_text_document_open(&doc, root) == PS_DOCUMENT_IO);
    ps_text_document_destroy(&doc);
    CHECK(!doc.saved && !doc.path[0]);
    puts("Text documents: UTF-8, identity, limits, backups, conflicts and failure preservation "
         "passed");
    return 0;
}
