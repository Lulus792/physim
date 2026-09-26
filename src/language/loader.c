#include "loader.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { SOURCE_LIMIT = 1024 * 1024, NODE_LIMIT = 65536, PATH_LIMIT = 4096 };

static void fail(ps_lang_loader *l, ps_lang_token token, const char *message,
                 int operational) {
    if (l->error.kind != PS_LANG_ERROR) {
        l->error = token;
        l->error.kind = PS_LANG_ERROR;
        l->error.error = message;
        l->operational_error = operational;
    }
}
static char *copy_path(const char *path) {
    size_t length = strlen(path);
    if (length >= PATH_LIMIT)
        return NULL;
    char *copy = malloc(length + 1);
    if (copy)
        memcpy(copy, path, length + 1);
    return copy;
}
static char *canonical_path(const char *path) {
#ifdef _WIN32
    char *resolved = _fullpath(NULL, path, 0);
#else
    char *resolved = realpath(path, NULL);
#endif
    if (resolved && strlen(resolved) < PATH_LIMIT)
        return resolved;
    free(resolved);
    return copy_path(path);
}
static int same_path(const char *left, const char *right) {
#ifdef _WIN32
    return _stricmp(left, right) == 0;
#else
    return strcmp(left, right) == 0;
#endif
}
static char *module_path(const char *directory, const unsigned char *name, size_t length) {
    size_t prefix = strlen(directory);
    int separator = prefix && directory[prefix - 1] != '/' && directory[prefix - 1] != '\\';
    if (prefix + (size_t)separator + length + sizeof(".phys") > PATH_LIMIT)
        return NULL;
    char *path = malloc(prefix + (size_t)separator + length + sizeof(".phys"));
    if (!path)
        return NULL;
    memcpy(path, directory, prefix);
    if (separator)
        path[prefix++] = '/';
    for (size_t i = 0; i < length; i++)
        path[prefix + i] = name[i] == '.' ? '/' : (char)name[i];
    memcpy(path + prefix + length, ".phys", sizeof(".phys"));
    return path;
}
static int accessible_file(const char *path) {
    FILE *file = fopen(path, "rb");
    if (file) {
        fclose(file);
        return 1;
    }
    return errno != ENOENT && errno != ENOTDIR;
}
static char *import_path(const char *owner, const unsigned char *name, size_t length) {
    const char *slash = strrchr(owner, '/');
    const char *backslash = strrchr(owner, '\\');
    if (!slash || (backslash && backslash > slash))
        slash = backslash;
    size_t directory = slash ? (size_t)(slash - owner + 1) : 0;
    char *base = malloc(directory + 1);
    if (!base)
        return NULL;
    memcpy(base, owner, directory);
    base[directory] = 0;
    char *path = module_path(base, name, length);
    free(base);
    return path;
}
size_t ps_lang_load(ps_lang_loader *l, const char *path, ps_lang_token from) {
    char *owned_path = canonical_path(path);
    if (!owned_path) {
        fail(l, from, "Module path is too long or cannot be allocated", 1);
        return 0;
    }
    for (size_t i = 0; i < l->files; i++)
        if (same_path(l->paths[i], owned_path)) {
            if (l->states[i] == 1)
                fail(l, from, "Cyclic module import", 0);
            free(owned_path);
            return l->roots[i];
        }
    if (l->files >= PS_MODULE_LIMIT) {
        fail(l, from, "Module count exceeds 128", 1);
        free(owned_path);
        return 0;
    }
    size_t file_index = l->files++;
    l->paths[file_index] = owned_path;
    l->states[file_index] = 1;
    FILE *file = fopen(owned_path, "rb");
    if (!file) {
        fail(l, from, from.kind == PS_LANG_ERROR ? "Cannot open source file"
                                                 : "Cannot open imported module", 1);
        return 0;
    }
    size_t base_source = l->source_size;
    size_t capacity = SOURCE_LIMIT - base_source;
    size_t size = fread(l->source + base_source, 1, capacity + 1, file);
    int read_failed = ferror(file);
    if (fclose(file) != 0)
        read_failed = 1;
    if (read_failed || size > capacity) {
        fail(l, from, read_failed ? "Cannot read source file"
                                  : "Total module source exceeds 1 MiB limit", 1);
        return 0;
    }
    l->source_size += size;
    ps_lang_node *scratch = calloc(NODE_LIMIT, sizeof(*scratch));
    if (!scratch) {
        fail(l, from, "Cannot allocate parser buffer", 1);
        return 0;
    }
    ps_lang_parse_result parsed = ps_lang_parse(l->source + base_source, size, scratch, NODE_LIMIT);
    if (!parsed.root) {
        parsed.diagnostic.file = file_index;
        l->error = parsed.diagnostic;
        free(scratch);
        return 0;
    }
    ps_lang_node *local = malloc(parsed.count * sizeof(*local));
    if (!local) {
        fail(l, from, "Cannot allocate module syntax", 1);
        free(scratch);
        return 0;
    }
    memcpy(local, scratch, parsed.count * sizeof(*local));
    free(scratch);
    for (size_t id = local[parsed.root].a; id && l->error.kind != PS_LANG_ERROR;
         id = local[id].next) {
        if (local[id].kind != PS_AST_IMPORT)
            continue;
        ps_lang_token token = local[id].token;
        ps_lang_token path_token = local[id].b ? local[local[id].b].token : token;
        token.file = file_index;
        const unsigned char *name = l->source + base_source + path_token.offset;
        char *dependency = import_path(owned_path, name, path_token.length);
        if (!dependency) {
            fail(l, token, "Imported module path is too long or cannot be allocated", 1);
            break;
        }
        if (!accessible_file(dependency))
            for (size_t root = 0; root < l->module_path_count; root++) {
                char *candidate = module_path(l->module_paths[root], name, path_token.length);
                if (!candidate) {
                    fail(l, token, "Imported module path is too long or cannot be allocated", 1);
                    break;
                }
                if (accessible_file(candidate)) {
                    free(dependency);
                    dependency = candidate;
                    break;
                }
                free(candidate);
            }
        if (l->error.kind != PS_LANG_ERROR)
            local[id].a = ps_lang_load(l, dependency, token);
        free(dependency);
    }
    if (l->error.kind == PS_LANG_ERROR) {
        free(local);
        return 0;
    }
    if (l->count + parsed.count > NODE_LIMIT) {
        fail(l, from, "Total syntax node capacity exceeded", 1);
        free(local);
        return 0;
    }
    size_t base_node = l->count - 1;
    for (size_t id = 1; id < parsed.count; id++) {
        ps_lang_node n = local[id];
        n.token.offset += base_source;
        n.token.file = file_index;
        if (n.a && n.kind != PS_AST_IMPORT)
            n.a += base_node;
        if (n.b)
            n.b += base_node;
        if (n.c)
            n.c += base_node;
        if (n.next)
            n.next += base_node;
        l->nodes[base_node + id] = n;
    }
    l->count += parsed.count - 1;
    size_t root = base_node + parsed.root;
    l->roots[file_index] = root;
    l->states[file_index] = 2;
    size_t first = l->nodes[root].a;
    if (first) {
        if (l->last)
            l->nodes[l->last].next = first;
        else
            l->first = first;
        size_t last = first;
        while (l->nodes[last].next)
            last = l->nodes[last].next;
        l->last = last;
    }
    free(local);
    return root;
}
void ps_lang_loader_destroy(ps_lang_loader *l) {
    for (size_t i = 0; i < l->files; i++)
        free(l->paths[i]);
    free(l->nodes);
    free(l->source);
}
