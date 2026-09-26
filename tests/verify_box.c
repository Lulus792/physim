#include "physim/data.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Box reference line %d: %s\n", __LINE__, #x);                          \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
int main(int argc, char **argv) {
    if (argc != 2)
        return 2;
    ps_run_reader reader;
    CHECK(ps_run_open(&reader, argv[1]) == PS_OK && reader.channels == 11);
    CHECK(strstr(reader.metadata, "model=oriented box-plane contacts") &&
          strstr(reader.metadata, "iterations=64") && strstr(reader.metadata, "ccd=none") &&
          strstr(reader.metadata, "penetration_slop_m=") && strstr(reader.metadata, "friction="));
    const char *names[] = {"position.x",    "position.y",   "position.z",
                           "velocity.y",    "energy",       "kinetic.energy",
                           "angular.speed", "contacts",     "contact.impulse.y",
                           "clearance",     "contact.error"};
    for (unsigned i = 0; i < 11; i++)
        CHECK(!strcmp(reader.schema[i].name, names[i]));
    double time = 0, values[PS_MAX_CHANNELS], initial_energy = 0;
    unsigned count = 0, touching = 0;
    ps_result r;
    while ((r = ps_run_next(&reader, &time, values)) == PS_OK) {
        CHECK(fabs(time - count * .005) < 1e-12);
        if (!count)
            initial_energy = values[4];
        CHECK(values[4] < initial_energy + .01 && values[5] >= 0 && values[6] >= 0);
        CHECK(values[7] >= 0 && values[7] <= 8 && values[7] == floor(values[7]) &&
              values[8] >= -1e-10);
        CHECK(values[9] > -.02 && values[10] >= 0);
        if (values[7] > 0)
            touching++;
        if (time > 8) {
            CHECK(fabs(values[3]) < 1e-4 && values[5] < 1e-6 && values[6] < 1e-3);
            CHECK(fabs(values[9]) < .0002 && values[10] < 1e-4);
            CHECK(fabs(values[8] - 9.81 * .005) < 1e-4);
        }
        count++;
    }
    CHECK(r == PS_EOF && count == 2001 && touching > 1000);
    ps_run_reader_close(&reader);
    puts("Box runner: tilted landing, resting support, energy bounds, impulses and recorded solver "
         "settings passed");
    return 0;
}
