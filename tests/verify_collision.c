#include "physim/data.h"
#include "platform.h"
#include <math.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Collision line %d: %s\n", __LINE__, #x);                              \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static int run(const char *const *args) {
    ps_process process = {0};
    if (!ps_process_start(&process, args, NULL))
        return -1;
    double until = ps_clock() + 10;
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
    if (argc != 6)
        return 2;
    const char *paths[] = {"collision-vacuum-test.psrun", "collision-air-test.psrun",
                           "collision-custom-test.psrun", "collision-friction-test.psrun"};
    const char *media[] = {"medium=vacuum", "medium=air at 15 C, sea level", "medium=custom medium",
                           "medium=vacuum"};
    const double rho[] = {0, 1.225, 5, 0};
    double final_energy[4] = {0};
    for (int mode = 0; mode < 4; mode++) {
        remove(paths[mode]);
        const char *args[] = {argv[1], argv[mode + 2], paths[mode], "--steps", "400",
                              "--dt",  "0.005",        "--seed",    "42",      NULL};
        CHECK(run(args) == 0);
        ps_run_reader reader;
        CHECK(ps_run_open(&reader, paths[mode]) == PS_OK && reader.channels == 11);
        CHECK(strstr(reader.metadata, media[mode]) && strstr(reader.metadata, "drag=quadratic") &&
              strstr(reader.metadata, "gravity=none") &&
              strstr(reader.metadata,
                     mode == 0 || mode == 3 ? "ccd=linear sphere sweep" : "ccd=none") &&
              strstr(reader.metadata,
                     mode == 0 || mode == 3 ? "contact=continuous" : "contact=discrete") &&
              strstr(reader.metadata, "seed=42"));
        CHECK(!strcmp(reader.schema[4].name, "energy") &&
              !strcmp(reader.schema[10].name, "a.contact_impulse_x"));
        unsigned samples = 0, contacts = 0;
        double time = 0, values[PS_MAX_CHANNELS], speed = .6, previous_energy = 1e10;
        double k = .5 * rho[mode] * .47 * PS_PI * .2 * .2;
        ps_result result;
        while ((result = ps_run_next(&reader, &time, values)) == PS_OK) {
            CHECK(fabs(time - samples * .005) < 1e-12);
            if (samples)
                speed -= k * speed * speed * .005;
            if (mode < 3) {
                CHECK(fabs(values[4] - speed * speed) < 1e-11);
                CHECK(fabs(fabs(values[2]) - speed) < 1e-11 && fabs(values[2] + values[3]) < 1e-12);
                CHECK(fabs(values[5]) + fabs(values[6]) < 1e-12);
            }
            CHECK(values[4] <= previous_energy + 1e-12);
            CHECK(fabs(values[7]) + fabs(values[8]) < 1e-12);
            if (fabs(values[10]) > 1e-12)
                contacts++;
            previous_energy = values[4];
            samples++;
        }
        CHECK(result == PS_EOF && samples == 401 && contacts == 1);
        final_energy[mode] = previous_energy;
        if (mode == 3)
            CHECK(values[5] < 1 && values[6] < 0 && final_energy[mode] < .368);
        ps_run_reader_close(&reader);
        remove(paths[mode]);
    }
    CHECK(fabs(final_energy[0] - .36) < 1e-12 && final_energy[0] > final_energy[1] &&
          final_energy[1] > final_energy[2]);
    puts("Collision runner: vacuum/air/custom drag references, rotational friction, metadata and "
         "all samples passed");
    return 0;
}
