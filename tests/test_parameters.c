#include "physim/experiment.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "Parameter API failed at line %d: %s\n", __LINE__, #condition); \
    return 1; \
} } while (0)

int main(void) {
    ps_context c = {0};
    c.struct_size = sizeof c;
    c.api_version = PS_API_VERSION;
    double value = -99;
    CHECK(ps_parameter_override(&c, "speed", 3.5) == PS_OK);
    CHECK(ps_parameter_override(&c, "speed", 4) == PS_INVALID);
    CHECK(ps_parameter_override(&c, "9speed", 4) == PS_INVALID);
    CHECK(ps_parameter_override(&c, "bad\nname", 4) == PS_INVALID);
    CHECK(ps_parameter_override(&c, "other", NAN) == PS_INVALID);
    CHECK(c.parameter_count == 1 && ps_parameter_finalize(&c) == PS_INVALID);
    CHECK(ps_parameter_define(&c, "speed", "Speed", 2, -1, 3, &value) == PS_INVALID);
    CHECK(c.parameter_count == 1 && !c.parameters[0].defined && value == -99);
    CHECK(ps_parameter_define(&c, "speed", "Speed", 2, -1, 5, &value) == PS_OK);
    CHECK(value == 3.5 && c.parameters[0].value == 3.5);
    CHECK(ps_parameter_define(&c, "speed", "Speed", 2, -1, 5, &value) == PS_INVALID);
    CHECK(ps_parameter_define(&c, "other", "Other", 1, 2, 3, &value) == PS_INVALID);
    CHECK(ps_parameter_define(&c, "other", "Other", 1, -2, 3, &value) == PS_OK);
    CHECK(value == 1 && ps_parameter_finalize(&c) == PS_OK);
    c.struct_size = offsetof(ps_context, parameter_count);
    CHECK(ps_parameter_override(&c, "old", 1) == PS_VERSION);
    CHECK(ps_parameter_define(&c, "old", "Old", 1, 0, 2, &value) == PS_VERSION);
    return 0;
}
