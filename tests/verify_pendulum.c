#include "physim/data.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
int main(int argc, char **argv) {
    if (argc != 2 && argc != 3)
        return 2;
    ps_run_reader reader;
    ps_result r = ps_run_open(&reader, argv[1]);
    if (r != PS_OK)
        return 1;
    int angle = -1, energy = -1;
    for (uint32_t i = 0; i < reader.channels; i++) {
        if (!strcmp(reader.schema[i].name, "angle"))
            angle = (int)i;
        if (!strcmp(reader.schema[i].name, "energy"))
            energy = (int)i;
    }
    if (angle < 0 || energy < 0) {
        ps_run_reader_close(&reader);
        return 1;
    }
    double time = 0, values[PS_MAX_CHANNELS], e0 = 0, max_error = 0, previous_angle = 0,
           previous_time = 0, last_cross = 0, sum = 0;
    unsigned periods = 0, crossings = 0;
    while ((r = ps_run_next(&reader, &time, values)) == PS_OK) {
        if (reader.samples == 1)
            e0 = values[energy];
        double error = fabs(values[energy] - e0);
        if (error > max_error)
            max_error = error;
        if (reader.samples > 1 && previous_angle < 0 && values[angle] >= 0) {
            double cross = previous_time + (time - previous_time) * (-previous_angle) /
                                               (values[angle] - previous_angle);
            if (crossings) {
                sum += cross - last_cross;
                periods++;
            }
            last_cross = cross;
            crossings++;
        }
        previous_angle = values[angle];
        previous_time = time;
    }
    /* T = 4 sqrt(L/g) K(sin(theta0/2)); binomial series for the elliptic integral.
       Ten terms give truncation error below 1e-12 at theta0=0.45 radians. */
    double k = sin(0.45 / 2), coefficient = 1, power = 1, series = 1;
    for (int n = 1; n <= 10; n++) {
        coefficient *= (2.0 * n - 1) / (2.0 * n);
        power *= k * k;
        series += coefficient * coefficient * power;
    }
    double reference = 2 * PS_PI * sqrt(1.5 / 9.80665) * series;
    bool verlet = argc == 3 && !strcmp(argv[2], "verlet");
    bool passed = r == PS_EOF && reader.samples == 4001 && fabs(time - 20) < 1e-12 &&
                  max_error < (verlet ? 1e-4 : 1e-8) && periods >= 6 &&
                  fabs(sum / periods - reference) < (verlet ? 5e-5 : 2e-6);
    if (argc == 3)
        passed =
            passed && strstr(reader.metadata, verlet ? "integrator=velocity Verlet"
                                                     : "integrator=Dormand-Prince 5(4)") != NULL;
    printf("Pendel: samples=%llu, max energy drift=%.9g J, measured period=%.12g s, "
           "reference=%.12g s\n",
           (unsigned long long)reader.samples, max_error, periods ? sum / periods : 0, reference);
    ps_run_reader_close(&reader);
    return passed ? 0 : 1;
}
