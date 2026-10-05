#include "parameter_catalog.h"
#include <math.h>
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
    ps_parameter_catalog scaled={0};
    CHECK(ps_parameter_catalog_restore(&scaled,"length","0.5"));
    const char *centimetres="PHYSIM_PARAMETERS_2\n2\n"
        "length\t1.5\t0.1\t10\tLength\tcm\t0.01\t1,0,0,0,0,0,0\n"
        "gain\t1\t0\t2\tNo declaration\t\t1\t0,0,0,0,0,0,0\n";
    CHECK(ps_parameter_catalog_parse(&scaled,centimetres));
    CHECK(scaled.units[0].declared && !scaled.units[1].declared && !strcmp(scaled.selected[0],"50"));
    CHECK(ps_parameter_catalog_value(&scaled,0,&value)==PS_OK && value==.5);
    CHECK(ps_parameter_catalog_input(&scaled,0,"10",&value)==PS_OK && value==.1);
    CHECK(ps_parameter_catalog_input(&scaled,0,"1000",&value)==PS_OK && value==10);
    CHECK(ps_parameter_catalog_input(&scaled,0,"9",&value)==PS_INVALID);
    CHECK(ps_parameter_catalog_input(&scaled,0,"nan",&value)==PS_INVALID);
    CHECK(ps_parameter_catalog_display(&scaled,0,1.5)==150);
    CHECK(ps_parameter_catalog_parse(&scaled,"PHYSIM_PARAMETERS_2\n1\n"
        "length\t1.5\t0.1\t10\tLength\tm\t1\t1,0,0,0,0,0,0\n"));
    CHECK(!strcmp(scaled.selected[0],"0.5") && ps_parameter_catalog_value(&scaled,0,&value)==PS_OK && value==.5);
    ps_parameter_catalog saved=scaled;
    CHECK(!ps_parameter_catalog_parse(&scaled,"PHYSIM_PARAMETERS_2\n1\n"
        "length\t1.5\t0.1\t10\tLength\tcm\t0\t1,0,0,0,0,0,0\n"));
    CHECK(!memcmp(&scaled,&saved,sizeof scaled));
    CHECK(!ps_parameter_catalog_parse(&scaled,"PHYSIM_PARAMETERS_2\n1\n"
        "length\t1.5\t0.1\t10\tLength\t\t1\t1,0,0,0,0,0,0\n"));
    CHECK(ps_parameter_catalog_parse(&scaled,"PHYSIM_PARAMETERS_2\n1\n"
        "length\t1.5\t0.1\t10\tChanged dimension\ts\t1\t0,0,1,0,0,0,0\n"));
    CHECK(!strcmp(scaled.selected[0],"1.5"));
    CHECK(ps_parameter_catalog_parse(&scaled,"PHYSIM_PARAMETERS_2\n1\n"
        "length\t0.29\t0\t1\tRounding check\tcm\t0.01\t1,0,0,0,0,0,0\n"));
    CHECK(ps_parameter_catalog_value(&scaled,0,&value)==PS_OK && value==.29);
    CHECK(ps_parameter_catalog_input(&scaled,0,"28.999999999999996",&value)==PS_OK && value==.29);
    CHECK(ps_parameter_catalog_parse(&scaled,"PHYSIM_PARAMETERS_2\n1\n"
        "length\t0\t-1\t1\tSigned zero\tcm\t0.01\t1,0,0,0,0,0,0\n"));
    CHECK(ps_parameter_catalog_input(&scaled,0,"-0",&value)==PS_OK && value==0 && signbit(value));
    CHECK(ps_parameter_catalog_input(&scaled,0,"0",&value)==PS_OK && value==0 && !signbit(value));
    ps_parameter_catalog small={0};
    CHECK(ps_parameter_catalog_parse(&small,"PHYSIM_PARAMETERS_2\n1\n"
        "length\t1e-300\t0\t1e-290\tSubnormal display\tm\t1e10\t1,0,0,0,0,0,0\n"));
    CHECK(ps_parameter_catalog_value(&small,0,&value)==PS_OK && value==1e-300);
    CHECK(ps_parameter_catalog_input(&small,0,"1e-999",&value)==PS_INVALID);
    ps_parameter_catalog tiny={0};
    CHECK(ps_parameter_catalog_restore(&tiny,"length","1e-200"));
    ps_parameter_catalog unchanged=tiny;
    CHECK(!ps_parameter_catalog_parse(&tiny,"PHYSIM_PARAMETERS_2\n1\n"
        "length\t0.5\t0\t1\tCannot display prior selection\tm\t1e308\t1,0,0,0,0,0,0\n"));
    CHECK(!memcmp(&tiny,&unchanged,sizeof tiny));
    return 0;
}
