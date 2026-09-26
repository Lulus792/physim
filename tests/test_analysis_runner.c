#include "physim/analysis.h"
#include "platform.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Analysis runner line %d: %s\n", __LINE__, #x);                        \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static int execute(const char *const *args, bool hang) {
    ps_process p = {0};
    if (!ps_process_start(&p, args, NULL))
        return -1;
    double until = ps_clock() + (hang ? .2 : 5);
    char output[1024];
    while (ps_process_poll(&p) && ps_clock() < until) {
        while (ps_process_read(&p, output, sizeof output) > 0) {
        }
        ps_sleep(1);
    }
    if (p.running) {
        ps_process_kill(&p);
        ps_process_close(&p);
        return hang ? 100 : -1;
    }
    int code = p.exit_code;
    ps_process_close(&p);
    return code;
}
int main(int argc, char **argv) {
    if (argc != 6)
        return 2;
    remove("analysis-fixture-input.psrun");
    ps_context context = {0};
    context.dt_s = .01;
    ps_channel_add(&context, "position", PS_METRE, "fixture");
    ps_run_writer writer;
    CHECK(ps_run_create(&writer, "analysis-fixture-input.psrun", &context, "fixture") == PS_OK);
    double value = 1;
    CHECK(ps_run_append(&writer, 0, &value) == PS_OK && ps_run_close(&writer) == PS_OK);
    for (int mode = 0; mode < 4; mode++) {
        char prefix[64], manifest[96];
        snprintf(prefix, sizeof prefix, "analysis-fixture-%d", mode);
        snprintf(manifest, sizeof manifest, "%s.inputs.csv", prefix);
        remove(manifest);
        const char *args[] = {argv[1], argv[mode + 2], "analysis-fixture-input.psrun", prefix,
                              NULL};
        int code = execute(args, mode == 3);
        CHECK(mode == 0 ? code == 0 : mode == 1 ? code == 4 : mode == 2 ? code > 0 : code == 100);
        remove(manifest);
    }
    const char *old_many[] = {
        argv[1],       argv[2], "--runs", "unsupported-many", "analysis-fixture-input.psrun",
        "other.psrun", NULL};
    CHECK(execute(old_many, false) == 6);
    const char *old_empty[] = {argv[1], argv[2], "--runs", "unsupported-empty", NULL};
    CHECK(execute(old_empty, false) == 6);
    const char *duplicate[] = {argv[1],
                               argv[2],
                               "--runs",
                               "unsupported-many",
                               "analysis-fixture-input.psrun",
                               "analysis-fixture-input.psrun",
                               NULL};
    CHECK(execute(duplicate, false) == 2);
    remove("analysis-fixture-input.psrun");
    puts("Analysis runner: legacy ABI, optional multi-run contract, wrong ABI, crash and hang "
         "isolation passed");
    return 0;
}
