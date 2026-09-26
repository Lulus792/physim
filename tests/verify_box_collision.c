#include "physim/data.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Box runner line %d: %s\n", __LINE__, #x);                             \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static int run(const char *const *args) {
    ps_process process = {0};
    if (!ps_process_start(&process, args, NULL))
        return -1;
    double until = ps_clock() + 15;
    char output[4096];
    while (ps_process_poll(&process) && ps_clock() < until) {
        while (ps_process_read(&process, output, sizeof output) > 0) {
        }
        ps_sleep(1);
    }
    int code = process.running ? -1 : process.exit_code;
    ps_process_close(&process);
    return code;
}
int main(int argc, char **argv) {
    CHECK(argc == 5);
    for (unsigned mode = 0; mode < 3; mode++) {
        char path[128];
        snprintf(path, sizeof path, "box-collision-reference-%u.psrun", mode);
        remove(path);
        const char *args[] = {argv[1], argv[mode + 2], path,     "--steps", "400",
                              "--dt",  "0.005",        "--seed", "42",      NULL};
        CHECK(run(args) == 0);
        ps_run_reader reader;
        CHECK(ps_run_open(&reader, path) == PS_OK && reader.channels == 15);
        CHECK(strstr(reader.metadata, "contact=15-axis SAT") &&
              strstr(reader.metadata, "ccd=none") && strstr(reader.metadata, "iterations=128") &&
              strstr(reader.metadata, "seed=42") && strstr(reader.metadata, "medium=vacuum") &&
              strstr(reader.metadata, "size_m="));
        CHECK(!strcmp(reader.schema[12].name, "contacts") &&
              !strcmp(reader.schema[13].name, "a.impulse.x"));
        unsigned samples = 0, impacts = 0;
        double time = 0, values[PS_MAX_CHANNELS], previous = .36, max_spin = 0;
        ps_result result;
        while ((result = ps_run_next(&reader, &time, values)) == PS_OK) {
            CHECK(fabs(time - samples * .005) < 1e-12);
            CHECK(fabs(values[9]) + fabs(values[10]) + fabs(values[11]) < 1e-11);
            CHECK(values[12] >= 0 && values[12] <= 8 && values[14] < 1e-7);
            CHECK(values[4] >= 0 && values[4] <= previous + 1e-9);
            if (fabs(values[13]) > 1e-10)
                impacts++;
            max_spin = fmax(max_spin, values[7] + values[8]);
            if (mode < 2) {
                double e = mode == 0 ? 1 : .5;
                bool after = impacts > 0;
                double velocity = after ? -.6 * e : .6;
                double energy = after ? .36 * e * e : .36;
                double position = time <= 1 ? -1 + .6 * time : -.4 - .6 * e * (time - 1);
                CHECK(fabs(values[3] - velocity) < 1e-10 && fabs(values[6] + velocity) < 1e-10);
                CHECK(fabs(values[4] - energy) < 1e-10 && fabs(values[0] - position) < 1e-10);
                CHECK(fabs(values[5] + position) < 1e-10 && values[7] + values[8] < 1e-9);
                if (fabs(values[13]) > 1e-10) {
                    CHECK(fabs(time - 1) < .0050001 && values[12] == 4);
                    CHECK(fabs(values[13] + .6 * (1 + e)) < 1e-10);
                }
            }
            previous = values[4];
            samples++;
        }
        CHECK(result == PS_EOF && samples == 401);
        CHECK(mode < 2 ? impacts == 1 : impacts > 0 && max_spin > .1 && previous < .35);
        printf("Box runner mode %u: %u samples, %u impacts, final energy %.12g J, peak spin sum "
               "%.12g rad/s\n",
               mode, samples, impacts, previous, max_spin);
        ps_run_reader_close(&reader);
        remove(path);
    }
    return 0;
}
