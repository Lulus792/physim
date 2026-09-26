#include "library.h"
#include <SDL3/SDL_filesystem.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Library line %d: %s\n", __LINE__, #x);                                \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
int main(void) {
    const char *dir = "library-fixture ä";
    CHECK(SDL_CreateDirectory(dir));
    const char *files[] = {"Messung ä.psrun", "Bericht.psreport", "UPPER.PSRUN", "ignore.csv",
                           "suffix.psrun.bak"};
    for (size_t i = 0; i < 5; i++) {
        char path[512];
        snprintf(path, sizeof path, "%s/%s", dir, files[i]);
        FILE *f = fopen(path, "wb");
        CHECK(f != NULL);
        CHECK(fputs("fixture", f) >= 0);
        CHECK(!fclose(f));
    }
    CHECK(SDL_CreateDirectory("library-fixture ä/directory.psrun"));
    ps_library *library = NULL;
    CHECK(ps_library_scan(dir, &library) == PS_OK && library->count == 3 && !library->limited &&
          !library->skipped);
    unsigned runs = 0, reports = 0;
    for (size_t i = 0; i < library->count; i++) {
        runs += library->items[i].kind == PS_LIBRARY_RUN;
        reports += library->items[i].kind == PS_LIBRARY_REPORT;
        CHECK(library->items[i].bytes == 7);
        if (i)
            CHECK(library->items[i - 1].modified_ns >= library->items[i].modified_ns);
        char path[4096];
        CHECK(ps_library_path(library, i, path, sizeof path) == PS_OK);
        FILE *f = fopen(path, "rb");
        CHECK(f != NULL && fgetc(f) == 'f');
        fclose(f);
    }
    CHECK(runs == 2 && reports == 1);
    char unchanged[8] = "keep";
    CHECK(ps_library_path(library, 0, unchanged, sizeof unchanged) == PS_LIMIT &&
          !strcmp(unchanged, "keep"));
    ps_library *old = library;
    CHECK(ps_library_scan("library-fixture ä/missing", &old) == PS_IO && old == library);
    CHECK(ps_library_path(library, 999, unchanged, sizeof unchanged) == PS_INVALID);
    ps_library_destroy(library);
    for (size_t i = 0; i < 5; i++) {
        char path[512];
        snprintf(path, sizeof path, "%s/%s", dir, files[i]);
        CHECK(SDL_RemovePath(path));
    }
    CHECK(SDL_RemovePath("library-fixture ä/directory.psrun"));
    CHECK(SDL_RemovePath(dir));
    puts("Library: UTF-8, types, sorting, metadata, bounds and failed refresh passed");
    return 0;
}
