#include "parameter_catalog.h"
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "Parameter catalog failed at line %d: %s\n", __LINE__, #condition); \
    return 1; \
} } while (0)

int main(void) {
    ps_parameter_catalog catalog = {0};
    const char *first = "module output\nPHYSIM_PARAMETERS_1\n2\n"
                        "speed\t2\t-10\t10\tInitial speed\n"
                        "mass\t1\t0.1\t20\tMass in kg\n";
    CHECK(ps_parameter_catalog_parse(&catalog, first));
    CHECK(catalog.count == 2 && !strcmp(catalog.entries[0].name, "speed"));
    double value = 0;
    CHECK(ps_parameter_catalog_value(&catalog, 0, &value) == PS_OK && value == 2);
    snprintf(catalog.selected[0], sizeof catalog.selected[0], "4.5");
    CHECK(ps_parameter_catalog_value(&catalog, 0, &value) == PS_OK && value == 4.5);
    CHECK(!ps_parameter_catalog_parse(&catalog, "PHYSIM_PARAMETERS_1\n1\n"
                                                "speed\t2\t10\t-10\tBad bounds\n"));
    CHECK(catalog.count == 2 && ps_parameter_catalog_value(&catalog, 0, &value) == PS_OK &&
          value == 4.5);
    CHECK(ps_parameter_catalog_parse(&catalog, "PHYSIM_PARAMETERS_1\n1\n"
                                               "speed\t3\t0\t5\tChanged\n"));
    CHECK(catalog.count == 1 && ps_parameter_catalog_value(&catalog, 0, &value) == PS_OK &&
          value == 4.5);
    CHECK(ps_parameter_catalog_parse(&catalog, "PHYSIM_PARAMETERS_1\n1\n"
                                               "speed\t3\t0\t4\tChanged\n"));
    CHECK(ps_parameter_catalog_value(&catalog, 0, &value) == PS_OK && value == 3);
    snprintf(catalog.selected[0], sizeof catalog.selected[0], "nan");
    CHECK(ps_parameter_catalog_value(&catalog, 0, &value) == PS_INVALID);
    snprintf(catalog.selected[0], sizeof catalog.selected[0], "5");
    CHECK(ps_parameter_catalog_value(&catalog, 0, &value) == PS_INVALID);
    ps_parameter_catalog restored = {0};
    CHECK(ps_parameter_catalog_restore(&restored, "speed", "4.5"));
    CHECK(!ps_parameter_catalog_restore(&restored, "speed", "3"));
    CHECK(!ps_parameter_catalog_restore(&restored, "bad name", "3"));
    CHECK(!ps_parameter_catalog_restore(&restored, "mass", "nan"));
    CHECK(ps_parameter_catalog_parse(&restored, "PHYSIM_PARAMETERS_1\n1\n"
                                               "speed\t2\t0\t5\tChanged\n"));
    CHECK(ps_parameter_catalog_value(&restored, 0, &value) == PS_OK && value == 4.5);
    CHECK(ps_parameter_catalog_parse(&restored, "PHYSIM_PARAMETERS_1\n1\n"
                                               "speed\t2\t0\t4\tChanged\n"));
    CHECK(ps_parameter_catalog_value(&restored, 0, &value) == PS_OK && value == 2);
    return 0;
}
