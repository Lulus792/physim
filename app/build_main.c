/* Native project builder. CMake is not invoked or needed by this executable. */
#ifdef __APPLE__
#define _DARWIN_C_SOURCE 1 /* flock is a BSD extension, outside strict POSIX. */
#endif
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700
#endif
#include "platform.h"
#include "project_file.h"
#include <SDL3/SDL.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>
#endif
#define PATH_SIZE 4096
enum { SDK_OBJECT_COUNT = 18, ARTIFACT_COUNT = SDK_OBJECT_COUNT + 4 };
typedef struct {
    Uint64 size;
    Uint32 crc;
} artifact_digest;
typedef struct {
    char compiler[PATH_SIZE], linker[PATH_SIZE];
    char includes[4][PATH_SIZE], libraries[3][PATH_SIZE];
    unsigned include_count, library_count;
    bool msvc;
} toolchain;
static bool path_join(char out[PATH_SIZE], const char *a, const char *b) {
    int n = snprintf(out, PATH_SIZE, "%s/%s", a, b);
    return n > 0 && n < PATH_SIZE;
}
static bool file_exists(const char *path) {
    SDL_PathInfo info;
    return SDL_GetPathInfo(path, &info) && info.type == SDL_PATHTYPE_FILE;
}
static bool absolute_path(const char *path, char output[PATH_SIZE]) {
#ifdef _WIN32
    wchar_t input[PATH_SIZE], absolute[PATH_SIZE];
    if (!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, input, PATH_SIZE))
        return false;
    DWORD n = GetFullPathNameW(input, PATH_SIZE, absolute, NULL);
    return n && n < PATH_SIZE &&
           WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, absolute, -1, output, PATH_SIZE, NULL,
                               NULL) &&
           strlen(output) <= 1800;
#else
    char *resolved = realpath(path, NULL);
    if (resolved) {
        bool ok = strlen(resolved) <= 1800;
        if (ok)
            strcpy(output, resolved);
        free(resolved);
        return ok;
    }
    if (path[0] == '/') {
        if (strlen(path) > 1800)
            return false;
        strcpy(output, path);
        return true;
    }
    char *current = SDL_GetCurrentDirectory();
    bool ok = current && path_join(output, current, path) && strlen(output) <= 1800;
    SDL_free(current);
    return ok;
#endif
}
static int run(const char *const *args, const char *directory, FILE *capture, char *text,
               size_t capacity) {
    ps_process process = {0};
    ps_process_limits limits = {0, 120};
    if (!ps_process_start_limited(&process, args, directory, &limits)) {
        fprintf(stderr, "Cannot start compiler command: %s\n", args[0]);
        return 1;
    }
    size_t used = 0, total = 0;
    bool ok = true;
    for (;;) {
        char bytes[8192];
        int count;
        while ((count = ps_process_read(&process, bytes, sizeof bytes)) > 0) {
            total += (size_t)count;
            if (capture) {
                if (total > 64u * 1024u * 1024u ||
                    fwrite(bytes, 1, (size_t)count, capture) != (size_t)count)
                    ok = false;
            } else if (!text)
                fwrite(bytes, 1, (size_t)count, stdout);
            if (text) {
                if ((size_t)count >= capacity - used)
                    ok = false;
                else {
                    memcpy(text + used, bytes, (size_t)count);
                    used += (size_t)count;
                }
            }
        }
        if (!ok)
            ps_process_kill(&process);
        if (!process.running)
            break;
        ps_process_poll(&process);
        ps_sleep(2);
    }
    if (text && capacity)
        text[used] = 0;
    int result = ok ? process.exit_code : 1;
    ps_process_close(&process);
    fflush(stdout);
    return result;
}
static bool equal_files(const char *left, const char *right) {
    FILE *a = fopen(left, "rb"), *b = fopen(right, "rb");
    bool same = a && b;
    if (same) {
        char x[8192], y[8192];
        size_t nx, ny;
        do {
            nx = fread(x, 1, sizeof x, a);
            ny = fread(y, 1, sizeof y, b);
            same = nx == ny && !memcmp(x, y, nx);
        } while (same && nx);
        same = same && !ferror(a) && !ferror(b);
    }
    if (a)
        fclose(a);
    if (b)
        fclose(b);
    return same;
}
static bool digest_file(const char *path, artifact_digest *digest) {
    FILE *file = fopen(path, "rb");
    if (!file)
        return false;
    artifact_digest value = {0};
    unsigned char bytes[16384];
    size_t count;
    while ((count = fread(bytes, 1, sizeof bytes, file)) != 0) {
        value.size += count;
        value.crc = SDL_crc32(value.crc, bytes, count);
    }
    bool ok = !ferror(file);
    if (fclose(file))
        ok = false;
    if (ok)
        *digest = value;
    return ok;
}
static bool artifact_matches(const char *path, const artifact_digest *expected) {
    SDL_PathInfo info;
    if (!SDL_GetPathInfo(path, &info) || info.type != SDL_PATHTYPE_FILE || info.size != expected->size)
        return false;
    artifact_digest actual;
    return digest_file(path, &actual) && actual.size == expected->size && actual.crc == expected->crc;
}
static bool read_artifacts(const char *path, artifact_digest digests[ARTIFACT_COUNT]) {
    FILE *file = fopen(path, "rb");
    if (!file)
        return false;
    char header[32];
    bool ok = fgets(header, sizeof header, file) && !strcmp(header, "physim_artifacts=1\n");
    for (unsigned i = 0; ok && i < ARTIFACT_COUNT; i++) {
        unsigned long long size;
        unsigned crc;
        char end;
        ok = fscanf(file, "%16llx %8x%c", &size, &crc, &end) == 3 && end == '\n';
        if (ok) {
            digests[i].size = size;
            digests[i].crc = crc;
        }
    }
    ok = ok && fgetc(file) == EOF && !ferror(file);
    if (fclose(file))
        ok = false;
    return ok;
}
static bool write_artifacts(const char *path, const artifact_digest digests[ARTIFACT_COUNT]) {
    char pending[PATH_SIZE];
    if (snprintf(pending, sizeof pending, "%s.next", path) >= PATH_SIZE)
        return false;
    FILE *file = fopen(pending, "wb");
    if (!file)
        return false;
    bool ok = fputs("physim_artifacts=1\n", file) != EOF;
    for (unsigned i = 0; ok && i < ARTIFACT_COUNT; i++)
        ok = fprintf(file, "%016llx %08x\n", (unsigned long long)digests[i].size,
                     (unsigned)digests[i].crc) > 0;
    if (fclose(file))
        ok = false;
    if (ok && equal_files(pending, path))
        return SDL_RemovePath(pending);
    if (ok && SDL_RenamePath(pending, path))
        return true;
    SDL_RemovePath(pending);
    return false;
}
static bool executable(const char *name, char output[PATH_SIZE]) {
#ifdef _WIN32
    wchar_t input[PATH_SIZE], found[PATH_SIZE];
    if (!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, name, -1, input, PATH_SIZE))
        return false;
    DWORD n = SearchPathW(NULL, input, L".exe", PATH_SIZE, found, NULL);
    return n && n < PATH_SIZE &&
           WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, found, -1, output, PATH_SIZE, NULL,
                               NULL);
#else
    if (strchr(name, '/')) {
        char *resolved = realpath(name, NULL);
        bool ok = resolved && strlen(resolved) < PATH_SIZE && !access(resolved, X_OK);
        if (ok)
            strcpy(output, resolved);
        free(resolved);
        return ok;
    }
    const char *search = getenv("PATH");
    if (!search)
        return false;
    while (*search) {
        const char *end = strchr(search, ':');
        size_t n = end ? (size_t)(end - search) : strlen(search);
        int length =
            snprintf(output, PATH_SIZE, "%.*s/%s", (int)(n ? n : 1), n ? search : ".", name);
        if (length > 0 && length < PATH_SIZE && !access(output, X_OK)) {
            char *resolved = realpath(output, NULL);
            bool ok = resolved && strlen(resolved) < PATH_SIZE;
            if (ok)
                strcpy(output, resolved);
            free(resolved);
            if (ok)
                return true;
        }
        if (!end)
            break;
        search = end + 1;
    }
    return false;
#endif
}
#ifdef _WIN32
typedef struct {
    char root[PATH_SIZE], version[128];
} sdk_search;
static SDL_EnumerationResult sdk_version(void *user, const char *directory, const char *name) {
    (void)directory;
    sdk_search *search = user;
    if (strncmp(name, "10.", 3) || strlen(name) >= sizeof search->version)
        return SDL_ENUM_CONTINUE;
    unsigned a[4] = {0}, b[4] = {0};
    if (sscanf(name, "%u.%u.%u.%u", a, a + 1, a + 2, a + 3) != 4)
        return SDL_ENUM_CONTINUE;
    sscanf(search->version, "%u.%u.%u.%u", b, b + 1, b + 2, b + 3);
    bool newer = !search->version[0];
    for (unsigned i = 0; i < 4; i++) {
        if (a[i] != b[i]) {
            newer = a[i] > b[i];
            break;
        }
    }
    char header[PATH_SIZE];
    int n = snprintf(header, sizeof header, "%s/Include/%s/um/Windows.h", search->root, name);
    if (newer && n > 0 && n < PATH_SIZE && file_exists(header))
        strcpy(search->version, name);
    return SDL_ENUM_CONTINUE;
}
static bool windows_toolchain(toolchain *tc, const char *override) {
    char finder[PATH_SIZE], install[PATH_SIZE] = {0}, tools[PATH_SIZE], version_path[PATH_SIZE];
    char programs[PATH_SIZE];
    wchar_t programs_wide[PATH_SIZE];
    /* Windows environment names are case insensitive. SDL's environment map
     * can preserve a parent's upper-case spelling and miss this mixed-case key. */
    DWORD programs_length = GetEnvironmentVariableW(L"ProgramFiles(x86)", programs_wide, PATH_SIZE);
    if (!programs_length || programs_length >= PATH_SIZE ||
        !WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, programs_wide, -1, programs,
                            PATH_SIZE, NULL, NULL) ||
        !path_join(finder, programs, "Microsoft Visual Studio/Installer/vswhere.exe"))
        return false;
    const char *args[] = {finder,      "-latest",
                          "-products", "*",
                          "-requires", "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
                          "-property", "installationPath",
                          "-utf8",     NULL};
    if (run(args, NULL, NULL, install, sizeof install))
        return false;
    install[strcspn(install, "\r\n")] = 0;
    if (strlen(install) > 1800)
        return false;
    if (!*install || !path_join(version_path, install,
                                "VC/Auxiliary/Build/Microsoft.VCToolsVersion.default.txt"))
        return false;
    char version[128];
    FILE *f = fopen(version_path, "rb");
    if (!f)
        return false;
    bool ok = fgets(version, sizeof version, f) != NULL;
    fclose(f);
    if (!ok)
        return false;
    version[strcspn(version, "\r\n ")] = 0;
    int n = snprintf(tools, sizeof tools, "%s/VC/Tools/MSVC/%s", install, version);
    if (n < 0 || n >= PATH_SIZE)
        return false;
    if (!path_join(tc->compiler, tools, "bin/Hostx64/x64/cl.exe") ||
        !path_join(tc->linker, tools, "bin/Hostx64/x64/link.exe"))
        return false;
    if (override && !executable(override, tc->compiler))
        return false;
    if (!file_exists(tc->compiler) || !file_exists(tc->linker))
        return false;
    sdk_search search = {0};
    wchar_t root[PATH_SIZE];
    DWORD size = sizeof root;
    if (RegGetValueW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots",
                     L"KitsRoot10", RRF_RT_REG_SZ | RRF_SUBKEY_WOW6464KEY, NULL, root,
                     &size) != ERROR_SUCCESS ||
        !WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, root, -1, search.root, PATH_SIZE, NULL,
                             NULL))
        return false;
    char includes[PATH_SIZE];
    if (strlen(search.root) > 1800)
        return false;
    if (!path_join(includes, search.root, "Include") ||
        !SDL_EnumerateDirectory(includes, sdk_version, &search) || !search.version[0])
        return false;
    path_join(tc->includes[0], tools, "include");
    const char *kinds[] = {"ucrt", "shared", "um"};
    for (unsigned i = 0; i < 3; i++)
        snprintf(tc->includes[i + 1], PATH_SIZE, "%s/Include/%s/%s", search.root, search.version,
                 kinds[i]);
    path_join(tc->libraries[0], tools, "lib/x64");
    snprintf(tc->libraries[1], PATH_SIZE, "%s/Lib/%s/ucrt/x64", search.root, search.version);
    snprintf(tc->libraries[2], PATH_SIZE, "%s/Lib/%s/um/x64", search.root, search.version);
    tc->include_count = 4;
    tc->library_count = 3;
    tc->msvc = true;
    return true;
}
#endif
static bool discover(toolchain *tc, const char *override) {
#ifdef _WIN32
    const char *base = override ? strrchr(override, '/') : NULL;
    const char *back = override ? strrchr(override, '\\') : NULL;
    if (back && (!base || back > base))
        base = back;
    base = base ? base + 1 : override;
    if (!override || !SDL_strcasecmp(base, "cl.exe") || !SDL_strcasecmp(base, "cl") ||
        !SDL_strcasecmp(base, "clang-cl.exe") || !SDL_strcasecmp(base, "clang-cl"))
        return windows_toolchain(tc, override);
#endif
    if (!executable(override ? override : "cc", tc->compiler))
        return false;
    strcpy(tc->linker, tc->compiler);
    return true;
}
static bool emit_source(const char *compiler, const char *source, const char *kind,
                        const char *output, const char *directory) {
    char pending[PATH_SIZE];
    if (snprintf(pending, sizeof pending, "%s.next", output) >= PATH_SIZE)
        return false;
    FILE *f = fopen(pending, "wb");
    if (!f)
        return false;
    const char *args[] = {compiler, kind, source, NULL};
    int result = run(args, directory, f, NULL, 0);
    if (fclose(f))
        result = 1;
    if (result) {
        f = fopen(pending, "rb");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof line, f))
                fputs(line, stderr);
            fclose(f);
        }
        SDL_RemovePath(pending);
        return false;
    }
    /* A failed translation must not replace the last complete C source.
       Keep its timestamp too when the successful output has not changed. */
    if (equal_files(pending, output))
        return SDL_RemovePath(pending);
    if (!SDL_RenamePath(pending, output)) {
        fprintf(stderr, "Cannot publish generated source: %s\n", SDL_GetError());
        SDL_RemovePath(pending);
        return false;
    }
    return true;
}
static bool compile(toolchain *tc, const char *source, const char *object, const char *sdk,
                    const char *project, const char *directory, bool release, bool strict,
                    bool force, bool *changed, artifact_digest *digest) {
    char preprocessed[PATH_SIZE], previous[PATH_SIZE], next_object[PATH_SIZE];
    char include[PATH_SIZE], project_include[PATH_SIZE], output[PATH_SIZE];
    if (snprintf(preprocessed, sizeof preprocessed, "%s.next.i", object) >= PATH_SIZE ||
        snprintf(previous, sizeof previous, "%s.i", object) >= PATH_SIZE ||
        snprintf(next_object, sizeof next_object, "%s.next", object) >= PATH_SIZE ||
        snprintf(include, sizeof include, "%s/include", sdk) >= PATH_SIZE)
        return false;
    snprintf(project_include, sizeof project_include, "%s", project);
    const char *args[40];
    unsigned n = 0;
    args[n++] = tc->compiler;
    args[n++] = tc->msvc ? "/nologo" : "-std=c17";
    if (tc->msvc) {
        args[n++] = "/std:c17";
        args[n++] = "/utf-8";
        args[n++] = "/TC";
        args[n++] = release ? "/MD" : "/MDd";
        args[n++] = release ? "/O2" : "/Od";
        args[n++] = strict ? "/fp:strict" : "/fp:precise";
        if (release)
            args[n++] = "/DNDEBUG";
        args[n++] = "/D_CRT_SECURE_NO_WARNINGS";
        args[n++] = "/P";
        snprintf(output, sizeof output, "/Fi%s", preprocessed);
        args[n++] = output;
    } else {
        args[n++] = "-D_POSIX_C_SOURCE=200809L";
        args[n++] = "-fPIC";
        args[n++] = release ? "-O2" : "-O0";
        args[n++] = "-fno-fast-math";
        args[n++] = "-ffp-contract=off";
        if (release)
            args[n++] = "-DNDEBUG";
        args[n++] = "-E";
        args[n++] = "-o";
        args[n++] = preprocessed;
    }
    args[n++] = tc->msvc ? "/I" : "-I";
    args[n++] = include;
    args[n++] = tc->msvc ? "/I" : "-I";
    args[n++] = project_include;
    for (unsigned i = 0; i < tc->include_count; i++) {
        args[n++] = "/I";
        args[n++] = tc->includes[i];
    }
    args[n++] = source;
    args[n] = NULL;
    if (run(args, directory, NULL, NULL, 0))
        return false;
    if (!force && artifact_matches(object, digest) && equal_files(preprocessed, previous)) {
        SDL_RemovePath(preprocessed);
        return true;
    }
    n = 0;
    args[n++] = tc->compiler;
    if (tc->msvc) {
        args[n++] = "/nologo";
        args[n++] = "/std:c17";
        args[n++] = "/utf-8";
        args[n++] = "/TC";
        args[n++] = "/c";
        args[n++] = "/W4";
        args[n++] = strict ? "/fp:strict" : "/fp:precise";
        args[n++] = release ? "/MD" : "/MDd";
        args[n++] = release ? "/O2" : "/Od";
        if (!release)
            args[n++] = "/Z7";
        snprintf(output, sizeof output, "/Fo%s", next_object);
        args[n++] = output;
    } else {
        args[n++] = "-std=c17";
        args[n++] = "-fPIC";
        args[n++] = "-fno-fast-math";
        args[n++] = "-ffp-contract=off";
        args[n++] = release ? "-O2" : "-O0";
        if (!release)
            args[n++] = "-g";
        args[n++] = "-c";
        args[n++] = "-x";
        args[n++] = "cpp-output";
        args[n++] = "-o";
        args[n++] = next_object;
    }
    args[n++] = preprocessed;
    args[n] = NULL;
    printf("Compile: %s\n", source);
    fflush(stdout);
    if (run(args, directory, NULL, NULL, 0) || !SDL_RenamePath(next_object, object) ||
        !SDL_RenamePath(preprocessed, previous) || !digest_file(object, digest))
        return false;
    *changed = true;
    return true;
}
static bool link_module(toolchain *tc, const char *module, const char *object,
                        char core[SDK_OBJECT_COUNT][PATH_SIZE], const char *directory, bool release) {
    const char *args[40];
    char output[PATH_SIZE], libs[3][PATH_SIZE];
    unsigned n = 0;
    args[n++] = tc->linker;
    if (tc->msvc) {
        args[n++] = "/NOLOGO";
        args[n++] = "/DLL";
        args[n++] = "/INCREMENTAL:NO";
        args[n++] = "/MACHINE:X64";
        if (!release)
            args[n++] = "/DEBUG";
        snprintf(output, sizeof output, "/OUT:%s", module);
        args[n++] = output;
        for (unsigned i = 0; i < tc->library_count; i++) {
            snprintf(libs[i], sizeof libs[i], "/LIBPATH:%s", tc->libraries[i]);
            args[n++] = libs[i];
        }
    } else {
#ifdef __APPLE__
        args[n++] = "-bundle";
        args[n++] = "-Wl,-undefined,error";
#else
        args[n++] = "-shared";
        /* Reject unresolved symbols before replacing the last working modules. */
        args[n++] = "-Wl,--no-undefined";
#endif
        args[n++] = "-o";
        args[n++] = module;
    }
    args[n++] = object;
    for (unsigned i = 0; i < SDK_OBJECT_COUNT; i++)
        args[n++] = core[i];
    if (!tc->msvc)
        args[n++] = "-lm";
    args[n] = NULL;
    printf("Link: %s\n", module);
    fflush(stdout);
    return !run(args, directory, NULL, NULL, 0);
}
int main(int argc, char **argv) {
    const char *project = NULL, *sdk = NULL, *directory = NULL, *language_compiler = NULL;
    const char *cc = SDL_getenv("PHYSIM_CC"), *profile = NULL;
    for (int i = 1; i < argc; i += 2) {
        if (i + 1 >= argc)
            return 2;
        if (!strcmp(argv[i], "--project"))
            project = argv[i + 1];
        else if (!strcmp(argv[i], "--sdk"))
            sdk = argv[i + 1];
        else if (!strcmp(argv[i], "--output"))
            directory = argv[i + 1];
        else if (!strcmp(argv[i], "--physimc"))
            language_compiler = argv[i + 1];
        else if (!strcmp(argv[i], "--profile"))
            profile = argv[i + 1];
        else if (!strcmp(argv[i], "--cc"))
            cc = argv[i + 1];
        else
            return 2;
    }
    if (!project || !sdk || !directory || !language_compiler ||
        (profile && strcmp(profile, "Debug") && strcmp(profile, "Release"))) {
        fputs("Usage: physim-build --project directory --sdk directory --output cache-directory "
              "--physimc executable [--profile Debug|Release] [--cc executable]\n",
              stderr);
        return 2;
    }
    ps_binary_stdio();
    /* Leave room for all derived paths and compiler option prefixes. */
    for (int i = 1; i < argc; i++) {
        if (strlen(argv[i]) > 1800) {
            fputs("Build argument is too long\n", stderr);
            return 2;
        }
    }
    char absolute_project[PATH_SIZE], absolute_sdk[PATH_SIZE], absolute_directory[PATH_SIZE];
    char absolute_language_compiler[PATH_SIZE];
    if (!absolute_path(project, absolute_project) || !absolute_path(sdk, absolute_sdk) ||
        !absolute_path(directory, absolute_directory)) {
        fputs("Invalid or overlong build path\n", stderr);
        return 2;
    }
    project = absolute_project;
    sdk = absolute_sdk;
    directory = absolute_directory;
    char manifest_path[PATH_SIZE];
    ps_project_settings project_settings;
    if (!path_join(manifest_path, project, "physim.project") ||
        ps_project_settings_read(manifest_path, &project_settings) != PS_DOCUMENT_OK) {
        fputs("Invalid physim.project\n", stderr);
        return 1;
    }
    if (!profile)
        profile = project_settings.release ? "Release" : "Debug";
    bool release = !strcmp(profile, "Release");
    const char *experiment = project_settings.language_experiment ? "main.phys" : "main.c";
    const char *analysis = project_settings.language_analysis ? "analysis.phys" : "analysis.c";
    if (strstr(experiment, ".phys") || strstr(analysis, ".phys")) {
        if (!executable(language_compiler, absolute_language_compiler)) {
            fputs("Cannot find physimc\n", stderr);
            return 1;
        }
        language_compiler = absolute_language_compiler;
    }
    if (!SDL_CreateDirectory(directory)) {
        fputs("Cannot create build cache\n", stderr);
        return 1;
    }
#ifndef _WIN32
    if (!absolute_path(directory, absolute_directory))
        return 1;
#endif
    char lock_path[PATH_SIZE];
    if (!path_join(lock_path, directory, "build.lock"))
        return 1;
#ifdef _WIN32
    wchar_t wide[PATH_SIZE];
    if (!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, lock_path, -1, wide, PATH_SIZE))
        return 1;
    HANDLE lock = CreateFileW(wide, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL, NULL);
    if (lock == INVALID_HANDLE_VALUE) {
        fputs("Build cache is already in use or unavailable\n", stderr);
        return 1;
    }
#else
    int lock = open(lock_path, O_CREAT | O_RDWR | O_CLOEXEC, 0600);
    if (lock < 0 || flock(lock, LOCK_EX | LOCK_NB)) {
        fputs("Build cache is already in use\n", stderr);
        return 1;
    }
#endif
    toolchain *tc = calloc(1, sizeof *tc);
    int result = 1;
    if (!tc || !discover(tc, cc)) {
        fputs("No C17 toolchain found. Install Visual Studio C++ Build Tools and Windows SDK, "
              "or select a compiler executable with PHYSIM_CC.\n",
              stderr);
        goto done;
    }
    printf("Compiler: %s\nProfile: %s\n", tc->compiler, profile);
    fflush(stdout);
    char config[PATH_SIZE], next_config[PATH_SIZE];
    path_join(config, directory, "build.config");
    path_join(next_config, directory, "build.config.next");
    FILE *settings = fopen(next_config, "wb");
    if (!settings)
        goto done;
    SDL_PathInfo compiler_info = {0}, linker_info = {0}, builder_info = {0};
    char builder[PATH_SIZE];
    if (executable(argv[0], builder))
        SDL_GetPathInfo(builder, &builder_info);
    SDL_GetPathInfo(tc->compiler, &compiler_info);
    SDL_GetPathInfo(tc->linker, &linker_info);
    bool settings_ok =
        fprintf(settings,
                "physim_native_build=1\nproject=%s\nsdk=%s\ncompiler=%s\n"
                "compiler_time=%lld\nlinker=%s\nlinker_time=%lld\nprofile=%s\nbuilder_time=%lld\n",
                project, sdk, tc->compiler, (long long)compiler_info.modify_time, tc->linker,
                (long long)linker_info.modify_time, profile,
                (long long)builder_info.modify_time) > 0;
    for (unsigned i = 0; i < tc->library_count; i++)
        settings_ok = fprintf(settings, "lib=%s\n", tc->libraries[i]) > 0 && settings_ok;
    if (fclose(settings))
        settings_ok = false;
    if (!settings_ok)
        goto done;
    char artifacts_path[PATH_SIZE];
    path_join(artifacts_path, directory, "build.artifacts");
    artifact_digest artifacts[ARTIFACT_COUNT] = {{0}};
    bool force = !read_artifacts(artifacts_path, artifacts) || !equal_files(config, next_config);
    bool changed = force;
    char pending[PATH_SIZE];
    path_join(pending, directory, "build.pending");
    changed = changed || file_exists(pending);
    FILE *marker = fopen(pending, "wb");
    if (!marker)
        goto done;
    if (fclose(marker))
        goto done;
    const char *names[] = {"core",         "memory",    "array",      "string_view",   "hashmap",
                           "math",         "data",      "analysis",   "scene",         "numerics",
                           "units",        "series",    "report",     "report_export", "mechanics",
                           "box_contacts", "collision", "measurement"};
    _Static_assert(SDL_arraysize(names) == SDK_OBJECT_COUNT, "SDK artifact catalog mismatch");
    char(*objects)[PATH_SIZE] = calloc(SDK_OBJECT_COUNT, PATH_SIZE);
    if (!objects)
        goto done;
    for (unsigned i = 0; i < SDK_OBJECT_COUNT; i++) {
        char source[PATH_SIZE];
        snprintf(source, sizeof source, "%s/src/%s.c", sdk, names[i]);
        snprintf(objects[i], PATH_SIZE, "%s/sdk-%s.obj", directory, names[i]);
        if (!compile(tc, source, objects[i], sdk, project, directory, release, false, force,
                     &changed, &artifacts[i])) {
            free(objects);
            goto done;
        }
    }
    char modules[2][PATH_SIZE], staged[2][PATH_SIZE], user_objects[2][PATH_SIZE];
    bool ok = true;
    for (unsigned i = 0; ok && i < 2; i++) {
        const char *name = i ? "analysis" : "experiment", *input = i ? analysis : experiment;
        char source[PATH_SIZE];
        path_join(source, project, input);
        if (strstr(input, ".phys")) {
            char generated[PATH_SIZE];
            snprintf(generated, sizeof generated, "%s/%s.physim.c", directory, name);
            ok = emit_source(language_compiler, source, i ? "--emit-analysis" : "--emit-experiment",
                             generated, directory);
            strcpy(source, generated);
        }
        snprintf(user_objects[i], PATH_SIZE, "%s/%s.obj", directory, name);
        if (ok)
            ok = compile(tc, source, user_objects[i], sdk, project, directory, release,
                         strstr(input, ".phys") != NULL, force, &changed,
                         &artifacts[SDK_OBJECT_COUNT + i]);
#ifdef _WIN32
        const char *extension = "dll";
#else
        const char *extension = "so";
#endif
        snprintf(modules[i], PATH_SIZE, "%s/%s.%s", directory, name, extension);
        snprintf(staged[i], PATH_SIZE, "%s/%s.next.%s", directory, name, extension);
        if (!artifact_matches(modules[i], &artifacts[SDK_OBJECT_COUNT + 2 + i]))
            changed = true;
    }
    for (unsigned i = 0; ok && changed && i < 2; i++)
        ok = link_module(tc, staged[i], user_objects[i], objects, directory, release);
    for (unsigned i = 0; ok && changed && i < 2; i++)
        ok = SDL_RenamePath(staged[i], modules[i]) &&
             digest_file(modules[i], &artifacts[SDK_OBJECT_COUNT + 2 + i]);
    free(objects);
    if (ok && write_artifacts(artifacts_path, artifacts) && SDL_RenamePath(next_config, config)) {
        SDL_RemovePath(pending);
        puts(changed ? "Build successful." : "Build up to date.");
        result = 0;
    }
done:
    free(tc);
#ifdef _WIN32
    CloseHandle(lock);
#else
    close(lock);
#endif
    return result;
}
