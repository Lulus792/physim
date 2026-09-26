#ifndef PS_LANGUAGE_LOADER_H
#define PS_LANGUAGE_LOADER_H
#include "parser.h"

enum { PS_MODULE_LIMIT = 128, PS_MODULE_PATH_LIMIT = 16 };
typedef struct ps_lang_loader {
    unsigned char *source;
    ps_lang_node *nodes;
    char *paths[PS_MODULE_LIMIT];
    const char *module_paths[PS_MODULE_PATH_LIMIT];
    size_t module_path_count;
    size_t roots[PS_MODULE_LIMIT];
    unsigned char states[PS_MODULE_LIMIT];
    size_t files, source_size, count, first, last;
    ps_lang_token error;
    int operational_error;
} ps_lang_loader;

size_t ps_lang_load(ps_lang_loader *loader, const char *path, ps_lang_token from);
void ps_lang_loader_destroy(ps_lang_loader *loader);
#endif
