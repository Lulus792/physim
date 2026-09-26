#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "batch.h"
#include "platform.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <signal.h>
#include <unistd.h>
#endif
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Parallel batch line %d: %s\n", __LINE__, #x);                         \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
typedef struct {
    uint32_t previous, maximum, cancel_after;
    bool invalid;
} progress;
static bool proceed(uint32_t completed, uint32_t active, void *user) {
    progress *p = user;
    if (completed < p->previous || active > 4)
        p->invalid = true;
    p->previous = completed;
    if (active > p->maximum)
        p->maximum = active;
    return !p->cancel_after || completed < p->cancel_after;
}
static bool read_text(const char *directory, const char *name, char text[4096]) {
    char path[4096];
    snprintf(path, sizeof path, "%s/%s", directory, name);
    FILE *file = fopen(path, "rb");
    if (!file)
        return false;
    size_t n = fread(text, 1, 4095, file);
    text[n] = 0;
    fclose(file);
    return true;
}
static bool no_children(const char *directory) {
    for (unsigned i = 0; i < 4; i++) {
        char name[64], text[4096];
        snprintf(name, sizeof name, "pid-%u.txt", i);
        if (!read_text(directory, name, text))
            return false;
        int pid = atoi(text);
        if (pid <= 0)
            return false;
#ifdef _WIN32
        HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, (DWORD)pid);
        if (process) {
            DWORD state = WaitForSingleObject(process, 0);
            CloseHandle(process);
            if (state != WAIT_OBJECT_0)
                return false;
        } else if (GetLastError() != ERROR_INVALID_PARAMETER)
            return false;
#else
        if (kill(pid, 0) != -1 || errno != ESRCH)
            return false;
#endif
    }
    return true;
}
static int command(const char *const *args) {
    ps_process child = {0};
    if (!ps_process_start(&child, args, NULL))
        return -1;
    double deadline = ps_clock() + 15;
    while (ps_process_poll(&child) && ps_clock() < deadline) {
        char output[4096];
        for (unsigned n = 0; n < 8; n++)
            if (ps_process_read(&child, output, sizeof output) <= 0)
                break;
        ps_sleep(2);
    }
    int code = child.running ? -1 : child.exit_code;
    ps_process_close(&child);
    return code;
}
int main(int argc, char **argv) {
    /* runner, build dir, barrier/cancel/fail/noise modules, batch CLI, source */
    CHECK(argc == 9);
    char root[4096], text[4096], journal[4096];
    snprintf(root, sizeof root, "%s/parallel-reference-%.0f", argv[2], ps_clock() * 1e9);
    CHECK(ps_make_directory_exclusive(root));
    ps_batch_options o = {0};
    strcpy(o.runner, argv[1]);
    strcpy(o.module, argv[3]);
    strcpy(o.channel, "test");
    o.runs = 4;
    o.steps = 1;
    o.dt = .005;
    o.workers = 4;
    o.timeout_s = 5;
    snprintf(o.directory, sizeof o.directory, "%s/barrier", root);
    ps_batch_result result;
    progress p = {0};
    CHECK(ps_batch_run(&o, proceed, &p, &result) == PS_OK && result.completed == 4 &&
          result.started == 4 && result.peak_active == 4 && result.active == 0 &&
          !result.cancelled && !p.invalid && p.maximum == 4);
    CHECK(no_children(o.directory));
    CHECK(read_text(o.directory, "completed.csv", journal) &&
          read_text(o.directory, "endpoints.csv", text));
    const char *rows[4], *last = strstr(journal, "\n1,0,"), *other = strstr(journal, "\n4,3,");
    rows[0] = strstr(text, "\n1,0,");
    rows[1] = strstr(text, "\n2,1,");
    rows[2] = strstr(text, "\n3,2,");
    rows[3] = strstr(text, "\n4,3,");
    CHECK(rows[0] && rows[1] && rows[2] && rows[3] && last && other);
    CHECK(strcmp(journal, text) && rows[0] < rows[1] && rows[1] < rows[2] && rows[2] < rows[3]);
    CHECK(last > other);
    for (unsigned i = 0; i < 4; i++) {
        char name[64];
        CHECK(result.finished[i] && result.values[i] == 100 + i);
        snprintf(name, sizeof name, "work-%04u/scratch.txt", i + 1);
        CHECK(read_text(o.directory, name, text) && atoi(text) == (int)i);
    }
    CHECK(read_text(o.directory, "series.txt", text) && strstr(text, "physim_batch=2") &&
          strstr(text, "workers=4"));
    /* Preserve non-contiguous completions when lower indices are still active. */
    snprintf(o.directory, sizeof o.directory, "%s/gap-cancel", root);
    memset(&p, 0, sizeof p);
    p.cancel_after = 2;
    CHECK(ps_batch_run(&o, proceed, &p, &result) == PS_OK && result.cancelled &&
          result.completed == 2 && !result.finished[0] && !p.invalid && result.active == 0);
    CHECK(no_children(o.directory) && read_text(o.directory, "endpoints.csv", text) &&
          !strstr(text, "\n1,0,"));
    CHECK(!read_text(o.directory, "summary.psreport", text));
    /* One successful/error exit must cancel every other hung process, and a
     * flooded stdout pipe must not postpone the deadline indefinitely. */
    for (unsigned mode = 0; mode < 3; mode++) {
        strcpy(o.module, argv[4 + mode]);
        snprintf(o.directory, sizeof o.directory, "%s/stop-%u", root, mode);
        o.runs = 8;
        o.steps = mode == 0 ? 2048 : 1;
        o.timeout_s = mode == 2 ? 2 : 5;
        memset(&p, 0, sizeof p);
        p.cancel_after = mode ? 0 : 1;
        double begin = ps_clock();
        ps_result r = ps_batch_run(&o, proceed, &p, &result);
        CHECK(r == (mode == 0 ? PS_OK : mode == 1 ? PS_IO : PS_LIMIT));
        CHECK(result.cancelled == (mode == 0) && result.started == 4 &&
              result.completed == (mode == 0 ? 1u : 0u) && result.active == 0 && !p.invalid);
        CHECK(ps_clock() - begin < 8 && no_children(o.directory));
        CHECK(!read_text(o.directory, "summary.psreport", text));
        CHECK(read_text(o.directory, "status.txt", text) &&
              strstr(text, mode == 0 ? "status=cancelled" : "status=failed"));
    }
    /* CLI accepts the worker option with/without a source path and refuses
     * invalid settings before creating an output directory. */
    snprintf(o.directory, sizeof o.directory, "%s/cli", root);
    const char *args[] = {argv[7], argv[1], argv[3],     o.directory, "test",  "4", "1",
                          ".005",  "0",     "--workers", "4",         argv[8], NULL};
    CHECK(command(args) == 0 && no_children(o.directory));
    CHECK(read_text(o.directory, "experiment.c", text) && strstr(text, "Parallel process fixture"));
    snprintf(o.directory, sizeof o.directory, "%s/cli-no-source", root);
    args[11] = NULL;
    CHECK(command(args) == 0 && no_children(o.directory));
    snprintf(o.directory, sizeof o.directory, "%s/invalid", root);
    const char *invalid[] = {"0", "9", "-1", "4.0", "18446744073709551616", ""};
    for (unsigned i = 0; i < sizeof invalid / sizeof *invalid; i++) {
        args[10] = invalid[i];
        CHECK(command(args) == 2);
    }
    CHECK(ps_make_directory_exclusive(o.directory));
    o.workers = 0;
    CHECK(ps_batch_validate(&o) == PS_INVALID);
    o.workers = PS_BATCH_MAX_WORKERS + 1;
    CHECK(ps_batch_validate(&o) == PS_INVALID);
    printf("Parallel batches: four-process barrier, ordered outputs, private working directories, "
           "partial results, cancellation, failure and noisy timeout cleanup, CLI passed.\n");
    return 0;
}
