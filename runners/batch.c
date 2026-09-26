#include "batch.h"
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
static volatile sig_atomic_t cancelled;
static void stop(int signal_number) {
    (void)signal_number;
    cancelled = 1;
}
static bool proceed(uint32_t completed, uint32_t active, void *user) {
    (void)active;
    uint32_t *previous = user;
    if (completed != *previous) {
        fprintf(stderr, "Completed: %u\n", completed);
        *previous = completed;
    }
    return !cancelled;
}
static bool unsigned_number(const char *s, uint64_t *value) {
    *value = 0;
    if (!*s)
        return false;
    for (; *s; s++) {
        if (*s < '0' || *s > '9' || *value > (UINT64_MAX - (unsigned)(*s - '0')) / 10)
            return false;
        *value = *value * 10 + (unsigned)(*s - '0');
    }
    return true;
}
int main(int argc, char **argv) {
    if (argc < 9) {
        fprintf(stderr,
                "Usage: physim-batch runner module new-directory channel runs steps dt seed "
                "[source] [--workers N] [--timeout S] [--memory-mib N] "
                "[--param name=value ...] [--sweep name=start:end]\n"
                "All paths must be absolute. Ctrl+C cancels and preserves completed runs.\n");
        return 2;
    }
    ps_batch_options options = {0};
    options.workers = 1;
    options.timeout_s = 30;
    char *paths[] = {options.runner, options.module, options.directory};
    for (unsigned i = 0; i < 3; i++) {
        if (strlen(argv[i + 1]) >= 4000)
            return 2;
        strcpy(paths[i], argv[i + 1]);
    }
    if (strlen(argv[4]) >= sizeof options.channel)
        return 2;
    strcpy(options.channel, argv[4]);
    bool workers_seen = false, timeout_seen = false, memory_seen = false;
    for (int i = 9; i < argc; i++) {
        if (!strcmp(argv[i], "--workers")) {
            uint64_t workers;
            if (workers_seen || ++i == argc || !unsigned_number(argv[i], &workers) || !workers ||
                workers > PS_BATCH_MAX_WORKERS)
                return 2;
            options.workers = (uint32_t)workers;
            workers_seen = true;
        } else if (!strcmp(argv[i], "--timeout")) {
            if (timeout_seen || ++i == argc)
                return 2;
            char *end;
            errno = 0;
            options.timeout_s = strtod(argv[i], &end);
            if (errno || end == argv[i] || *end)
                return 2;
            timeout_seen = true;
        } else if (!strcmp(argv[i], "--memory-mib")) {
            uint64_t mib;
            if (memory_seen || ++i == argc || !unsigned_number(argv[i], &mib) || mib > 16384)
                return 2;
            options.memory_bytes = mib * UINT64_C(1048576);
            memory_seen = true;
        } else if (!strcmp(argv[i], "--param")) {
            if (++i == argc || options.parameter_count == PS_MAX_PARAMETERS)
                return 2;
            const char *equals = strchr(argv[i], '=');
            if (!equals || equals == argv[i] ||
                equals - argv[i] >= (ptrdiff_t)sizeof options.parameters[0].name)
                return 2;
            uint32_t index = options.parameter_count++;
            size_t length = (size_t)(equals - argv[i]);
            memcpy(options.parameters[index].name, argv[i], length);
            options.parameters[index].name[length] = 0;
            char *end;
            errno = 0;
            options.parameters[index].value = strtod(equals + 1, &end);
            if (errno || end == equals + 1 || *end)
                return 2;
        } else if (!strcmp(argv[i], "--sweep")) {
            if (options.sweep || ++i == argc)
                return 2;
            const char *equals = strchr(argv[i], '=');
            if (!equals || equals == argv[i] ||
                equals - argv[i] >= (ptrdiff_t)sizeof options.sweep_name)
                return 2;
            size_t length = (size_t)(equals - argv[i]);
            memcpy(options.sweep_name, argv[i], length);
            options.sweep_name[length] = 0;
            char *end;
            errno = 0;
            options.sweep_start = strtod(equals + 1, &end);
            if (errno || end == equals + 1 || *end != ':')
                return 2;
            const char *second = end + 1;
            errno = 0;
            options.sweep_end = strtod(second, &end);
            if (errno || end == second || *end)
                return 2;
            options.sweep = true;
        } else if (argv[i][0] != '-' && !options.source[0] && strlen(argv[i]) < 4000)
            strcpy(options.source, argv[i]);
        else
            return 2;
    }
    uint64_t runs, steps;
    if (!unsigned_number(argv[5], &runs) || runs > UINT32_MAX ||
        !unsigned_number(argv[6], &steps) || steps > UINT32_MAX ||
        !unsigned_number(argv[8], &options.seed))
        return 2;
    options.runs = (uint32_t)runs;
    options.steps = (uint32_t)steps;
    char *end;
    errno = 0;
    options.dt = strtod(argv[7], &end);
    if (errno || end == argv[7] || *end)
        return 2;
    if (ps_batch_validate(&options) != PS_OK)
        return 2;
    signal(SIGINT, stop);
    signal(SIGTERM, stop);
    ps_batch_result result;
    uint32_t previous = 0;
    ps_result r = ps_batch_run(&options, proceed, &previous, &result);
    fprintf(stderr, "%s: %u/%u runs. %s\n", result.cancelled ? "Cancelled" : ps_result_string(r),
            result.completed, options.runs, result.error);
    return r != PS_OK ? 1 : result.cancelled ? 130 : 0;
}
