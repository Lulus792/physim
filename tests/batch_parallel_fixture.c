#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "physim/experiment.h"
#include "platform.h"
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <process.h>
#define getpid _getpid
#else
#include <unistd.h>
#endif
#ifndef PARALLEL_FIXTURE_MODE
#define PARALLEL_FIXTURE_MODE 0
#endif
static bool exists(const char *path) {
    FILE *file = fopen(path, "rb");
    if (!file)
        return false;
    fclose(file);
    return true;
}
static ps_result create(ps_context *c) {
    ps_channel_add(c, "test", PS_METRE, "Parallel process fixture");
    c->values[0] = 100 + (double)c->seed;
    /* Identical relative names must not collide between simultaneous runs. */
    FILE *scratch = fopen("scratch.txt", "wbx");
    if (!scratch)
        return PS_IO;
    fprintf(scratch, "%llu\n", (unsigned long long)c->seed);
    fclose(scratch);
    char path[64];
    snprintf(path, sizeof path, "../pid-%llu.txt", (unsigned long long)c->seed);
    FILE *pid = fopen(path, "wbx");
    if (!pid)
        return PS_IO;
    fprintf(pid, "%d\n", (int)getpid());
    fclose(pid);
    /* A real four-process barrier. Sequential execution cannot pass this test. */
    double deadline = ps_clock() + 10;
    for (;;) {
        bool ready = true;
        for (unsigned i = 0; i < 4; i++) {
            snprintf(path, sizeof path, "../pid-%u.txt", i);
            ready = ready && exists(path);
        }
        if (ready)
            return PS_OK;
        if (ps_clock() >= deadline)
            return PS_LIMIT;
        ps_sleep(2);
    }
}
static ps_result step(ps_context *c, double dt) {
    (void)c;
    (void)dt;
#if PARALLEL_FIXTURE_MODE == 0
    if (!c->seed) {
        /* Force index 0 to finish after the other three accepted endpoints.
         * This also detects a controller that blocks on the first child. */
        double deadline = ps_clock() + 10;
        for (;;) {
            char rows[4096] = {0};
            FILE *file = fopen("../completed.csv", "rb");
            if (file) {
                size_t n = fread(rows, 1, sizeof rows - 1, file);
                rows[n] = 0;
                fclose(file);
            }
            if (strstr(rows, "\n2,1,") && strstr(rows, "\n3,2,") && strstr(rows, "\n4,3,"))
                break;
            if (ps_clock() >= deadline)
                return PS_LIMIT;
            ps_sleep(2);
        }
    }
    return PS_OK;
#else
#if PARALLEL_FIXTURE_MODE != 3
    if (!c->seed)
        return PARALLEL_FIXTURE_MODE == 1 ? PS_OK : PS_INVALID;
#endif
    double deadline = ps_clock() + 10;
    while (ps_clock() < deadline) {
#if PARALLEL_FIXTURE_MODE == 3
        char noise[4096];
        memset(noise, 'x', sizeof noise);
        fwrite(noise, 1, sizeof noise, stdout);
        fflush(stdout);
#else
        ps_sleep(2);
#endif
    }
    return PS_LIMIT;
#endif
}
static void scene(ps_context *c, ps_scene *s) {
    (void)c;
    (void)s;
}
static void destroy(ps_context *c) { (void)c; }
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api = {
        sizeof api, PS_ABI_VERSION, 0, "parallel fixture", create, create, step, scene, destroy};
    return &api;
}
