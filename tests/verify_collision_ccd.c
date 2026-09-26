#include "physim/data.h"
#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "CCD template line %d: %s\n", __LINE__, #x);                           \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static int run(const char *const *args) {
    ps_process process = {0};
    if (!ps_process_start(&process, args, NULL))
        return -1;
    double until = ps_clock() + 15;
    char text[4096];
    while (ps_process_poll(&process) && ps_clock() < until) {
        while (ps_process_read(&process, text, sizeof text) > 0) {
        }
        ps_sleep(1);
    }
    int result = process.running ? -1 : process.exit_code;
    ps_process_close(&process);
    return result;
}
int main(int argc, char **argv) {
    CHECK(argc == 4);
    double final[2] = {0};
    for (unsigned mode = 0; mode < 3; mode++) {
        const char *path = mode == 0   ? "ccd-coarse.psrun"
                           : mode == 1 ? "ccd-fine.psrun"
                                       : "ccd-discrete.psrun";
        const char *step = mode == 1 ? "0.003" : "0.05";
        const char *steps = mode == 1 ? "50" : "3";
        remove(path);
        const char *args[] = {
            argv[1], argv[mode == 2 ? 3 : 2], path, "--dt", step, "--steps", steps, NULL};
        CHECK(run(args) == 0);
        ps_run_reader reader;
        CHECK(ps_run_open(&reader, path) == PS_OK && reader.channels == 11);
        CHECK(strstr(reader.metadata, mode == 2 ? "ccd=none" : "ccd=linear sphere sweep"));
        CHECK(strstr(reader.metadata, "initial_speed_m_s=100"));
        unsigned samples = 0, contacts = 0;
        double time, values[PS_MAX_CHANNELS], last = 0;
        ps_result r;
        while ((r = ps_run_next(&reader, &time, values)) == PS_OK) {
            double dt = mode == 1 ? .003 : .05;
            CHECK(fabs(time - samples * dt) < 1e-12);
            double expected = mode == 2 || time < .008 ? -1 + 100 * time : .6 - 100 * time;
            CHECK(fabs(values[0] - expected) < 1e-10 && fabs(values[1] + expected) < 1e-10);
            CHECK(fabs(values[4] - 10000) < 1e-8 && fabs(values[7]) + fabs(values[8]) < 1e-12);
            double speed = mode == 2 || time < .008 ? 100 : -100;
            CHECK(fabs(values[2] - speed) < 1e-10 && fabs(values[3] + speed) < 1e-10);
            if (fabs(values[10]) > 1e-10) {
                CHECK(fabs(values[10] + 200) < 1e-10);
                contacts++;
            }
            last = values[0];
            samples++;
        }
        CHECK(r == PS_EOF && samples == (mode == 1 ? 51u : 4u));
        CHECK(contacts == (mode == 2 ? 0u : 1u));
        if (mode < 2)
            final[mode] = last;
        else
            CHECK(last > 0); /* Discrete comparison tunnels through its partner. */
        ps_run_reader_close(&reader);
        remove(path);
    }
    CHECK(fabs(final[0] - final[1]) < 1e-10 && fabs(final[0] + 14.4) < 1e-10);
    puts("CCD template: coarse/fine trajectories, energy, impulse and discrete tunneling passed");
    return 0;
}
