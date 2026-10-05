#include "physim/data.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "Runner snapshots line %d: %s\n", __LINE__, #x); return 1; } } while (0)
static int run(const char *runner, const char *module, const char *path, const char *work, bool scenes) {
    const char *args[] = {runner, module, path, "--steps", "200", "--dt", "0.005", "--seed", "42", scenes ? "--record-scenes" : NULL, NULL};
    ps_process child = {0};
    CHECK(ps_process_start(&child, args, work));
    double deadline = ps_clock() + 20;
    char bytes[4096];
    while (ps_process_poll(&child) && ps_clock() < deadline) {
        while (ps_process_read(&child, bytes, sizeof bytes) > 0) {}
        ps_sleep(1);
    }
    if (child.running) ps_process_kill(&child);
    int code = child.exit_code;
    ps_process_close(&child);
    CHECK(code == 0);
    return 0;
}
int main(int argc, char **argv) {
    CHECK(argc == 4);
    char base[4096], archive[4096];
    unsigned long long stamp = (unsigned long long)(ps_clock() * 1e9);
    snprintf(base, sizeof base, "%s/reference-%llu.psrun", argv[3], stamp);
    snprintf(archive, sizeof archive, "%s/scenes-%llu.psrun", argv[3], stamp);
    CHECK(!run(argv[1], argv[2], base, argv[3], false));
    CHECK(!run(argv[1], argv[2], archive, argv[3], true));
    ps_run_reader r;
    CHECK(ps_run_open(&r, base) == PS_OK);
    double times[201], values[201][PS_MAX_CHANNELS];
    uint32_t channels = r.channels;
    for (unsigned i = 0; i <= 200; i++) CHECK(ps_run_next(&r, &times[i], values[i]) == PS_OK);
    double time, row[PS_MAX_CHANNELS];
    CHECK(ps_run_next(&r, &time, row) == PS_EOF && r.samples == 201);
    ps_run_reader_close(&r);
    CHECK(ps_run_open(&r, archive) == PS_OK && r.channels == channels);
    for (unsigned i = 0; i <= 200; i++) {
        CHECK(ps_run_next(&r, &time, row) == PS_OK && time == times[i]);
        for (uint32_t j = 0; j < channels; j++) CHECK(row[j] == values[i][j]);
    }
    CHECK(ps_run_next(&r, &time, row) == PS_EOF && r.samples == 201);
    ps_run_reader_close(&r);
    CHECK(ps_run_open(&r, base) == PS_OK);
    ps_snapshot frame;
    CHECK(ps_run_snapshot_next(&r, &frame) == PS_EOF && r.samples == 201);
    ps_run_reader_close(&r);
    CHECK(ps_run_open(&r, archive) == PS_OK);
    unsigned count = 0;
    double previous = -1;
    ps_result result;
    while ((result = ps_run_snapshot_next(&r, &frame)) == PS_OK) {
        unsigned sample = (unsigned)llround(frame.time / .005);
        CHECK(sample <= 200 && frame.time == times[sample] && frame.count == channels);
        CHECK(frame.scene.count > 0 && ps_scene_valid(&frame.scene));
        for (uint32_t j = 0; j < channels; j++) CHECK(frame.values[j] == values[sample][j]);
        if (!count) CHECK(frame.time == 0);
        else CHECK(frame.time > previous && (frame.time == 1 || frame.time - previous >= 1.0 / 60 - 1e-12));
        previous = frame.time;
        count++;
    }
    CHECK(result == PS_EOF && r.samples == 201 && previous == 1 && count >= 50 && count <= 61);
    ps_run_reader_close(&r);
    printf("%u scene frames preserve all 201 measurements, initial/final states, matching values and legacy reads.\n", count);
    return 0;
}
