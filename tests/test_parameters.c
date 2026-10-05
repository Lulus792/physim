#include "physim/experiment.h"
#include <math.h>
#include <float.h>
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
    ps_parameter_unit unit;
    CHECK(ps_parameter_unit_read(&c,0,&unit)==PS_OK && !unit.declared && unit.scale==1);
    ps_context typed={0};typed.struct_size=sizeof typed;typed.api_version=PS_API_VERSION;
    char symbol[]="cm";ps_unit display={{1,0,0,0,0,0,0},.01,symbol};
    CHECK(ps_parameter_override(&typed,"length",.5)==PS_OK);
    CHECK(ps_parameter_define_unit(&typed,"length","Length",display,1,.1,2,&value)==PS_OK && value==.5);
    symbol[0]='x';
    CHECK(ps_parameter_unit_read(&typed,0,&unit)==PS_OK && unit.declared &&
          unit.scale==.01 && unit.dimension[0]==1 && !strcmp(unit.symbol,"cm"));
    ps_context before=typed;value=-99;
    display.scale=0;
    CHECK(ps_parameter_define_unit(&typed,"bad","Bad",display,1,0,2,&value)==PS_INVALID &&
          !memcmp(&typed,&before,sizeof typed) && value==-99);
    display.scale=DBL_MIN;display.symbol="tiny";
    CHECK(ps_parameter_define_unit(&typed,"bad","Bad",display,1,0,DBL_MAX,&value)==PS_NUMERIC &&
          !memcmp(&typed,&before,sizeof typed));
    display.scale=DBL_MAX;
    CHECK(ps_parameter_define_unit(&typed,"bad","Bad",display,DBL_MIN,0,1,&value)==PS_NUMERIC &&
          !memcmp(&typed,&before,sizeof typed));
    display.scale=1;display.symbol="bad\tunit";
    CHECK(ps_parameter_define_unit(&typed,"bad","Bad",display,1,0,2,&value)==PS_INVALID);
    display.symbol="1234567890123456";
    CHECK(ps_parameter_define_unit(&typed,"bad","Bad",display,1,0,2,&value)==PS_INVALID);
    CHECK(ps_parameter_define_unit(&typed,"one","Explicit dimensionless",PS_RADIAN,1,0,2,&value)==PS_OK);
    CHECK(ps_parameter_unit_parse("parameter.length=1\n","length",&unit)==PS_OK && !unit.declared);
    const char *metadata="parameter_unit.length=cm\nparameter_scale.length=0.01\nparameter_dimension.length=1,0,0,0,0,0,0\n";
    CHECK(ps_parameter_unit_parse(metadata,"length",&unit)==PS_OK && unit.declared &&
          unit.scale==.01 && unit.dimension[0]==1 && !strcmp(unit.symbol,"cm"));
    CHECK(ps_parameter_unit_parse("parameter_unit.length=tiny\nparameter_scale.length=1e-310\n"
          "parameter_dimension.length=1,0,0,0,0,0,0\n","length",&unit)==PS_OK && unit.scale==1e-310);
    ps_parameter_unit saved=unit;
    const char *bad[]={
        "parameter_unit.length=cm\n",
        "parameter_scale.length=0.01\nparameter_dimension.length=1,0,0,0,0,0,0\n",
        "parameter_unit.length=cm\nparameter_scale.length=nan\nparameter_dimension.length=1,0,0,0,0,0,0\n",
        "parameter_unit.length=cm\nparameter_scale.length=-1\nparameter_dimension.length=1,0,0,0,0,0,0\n",
        "parameter_unit.length=cm\nparameter_scale.length=0.01\nparameter_dimension.length=128,0,0,0,0,0,0\n",
        "parameter_unit.length=cm\nparameter_scale.length=0.01\nparameter_dimension.length=1,0,0,0,0,0\n",
        "parameter_unit.length=cm\nparameter_unit.length=m\nparameter_scale.length=0.01\nparameter_dimension.length=1,0,0,0,0,0,0\n"};
    for(unsigned i=0;i<sizeof bad/sizeof *bad;i++)
        CHECK(ps_parameter_unit_parse(bad[i],"length",&unit)==PS_CORRUPT && !memcmp(&unit,&saved,sizeof unit));
    /* A pre-extension host still supports plain parameters; no unit-tail access. */
    typed.struct_size=offsetof(ps_context,parameter_units);
    CHECK(ps_parameter_unit_read(&typed,0,&unit)==PS_OK && !unit.declared && unit.scale==1);
    CHECK(ps_parameter_define_unit(&typed,"bad","Bad",PS_METRE,1,0,2,&value)==PS_VERSION);
    CHECK(ps_parameter_define(&typed,"old","Old parameter",1,0,2,&value)==PS_OK && value==1);
    c.struct_size = offsetof(ps_context, parameter_count);
    CHECK(ps_parameter_override(&c, "old", 1) == PS_VERSION);
    CHECK(ps_parameter_define(&c, "old", "Old", 1, 0, 2, &value) == PS_VERSION);
    return 0;
}
