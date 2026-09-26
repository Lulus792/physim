#include "physim/data.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc != 3)
        return 2;
    ps_run_reader reader;
    if (ps_run_open(&reader, argv[1]) != PS_OK)
        return 3;
    char expected[128];
    double selected = strtod(argv[2], NULL);
    snprintf(expected, sizeof expected, "parameter.initialSpeed=%.17g\n", selected);
    int valid = strstr(reader.metadata, expected) != NULL &&
                strstr(reader.metadata, "parameter_default.initialSpeed=2\n") != NULL &&
                strstr(reader.metadata, "parameter_min.initialSpeed=-10\n") != NULL &&
                strstr(reader.metadata, "parameter_max.initialSpeed=10\n") != NULL &&
                reader.channels == 1 && strcmp(reader.schema[0].name, "speed") == 0;
    double time, values[PS_MAX_CHANNELS];
    for (int i = 0; valid && i < 2; i++)
        valid = ps_run_next(&reader, &time, values) == PS_OK &&
                fabs(time - i * 0.005) < 1e-12 && values[0] == selected;
    if (valid)
        valid = ps_run_next(&reader, &time, values) == PS_EOF;
    ps_run_reader_close(&reader);
    if (!valid)
        fprintf(stderr, "Parameter metadata or samples differ from selected value\n");
    return valid ? 0 : 1;
}
