#define PSRT_MODULE
#define PSRT_SOURCE "unit-runtime.phys"
#include "physim/language_sdk.h"
#include <float.h>
#include <stdio.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "unit runtime %d: %s\n", __LINE__, #x); return 1; } } while (0)
static psrt_trap trap;
static ps_diagnostic diagnostic;
static char error[2048];
static int rejected(unsigned operation, ps_result expected) {
    memset(&trap, 0, sizeof trap);
    memset(&diagnostic, 0, sizeof diagnostic);
    trap.error = error; trap.capacity = sizeof error; trap.diagnostic = &diagnostic;
    trap.operation = "unit.operation"; psrt_current = &trap;
    if (!setjmp(trap.jump)) {
        psrt_site site = {"unit-runtime.phys", 3, 4};
        ps_unit large = PS_METRE; large.scale = DBL_MAX;
        ps_unit twice = PS_METRE; twice.scale = 2;
        if (operation == 0) (void)psrt_convert(DBL_MAX, twice, PS_METRE, site);
        if (operation == 1) (void)psrt_convert(1, PS_METRE, PS_SECOND, site);
        if (operation == 2) (void)psrt_quantity_convert((ps_quantity){DBL_MAX, twice}, PS_METRE, site);
        if (operation == 3) (void)psrt_unit_multiply(large, twice, "big", site);
        if (operation == 4) (void)psrt_quantity_multiply((ps_quantity){DBL_MAX, PS_METRE},
                                                       (ps_quantity){2, PS_METRE}, "area", site);
        if (operation == 5) (void)psrt_convert(DBL_TRUE_MIN, PS_METRE, large, site);
        psrt_current = NULL;
        CHECK(false);
    }
    psrt_current = NULL;
    CHECK(trap.failure_code == expected && ps_diagnostic_valid(&diagnostic) && diagnostic.code == expected);
    CHECK(!strcmp(diagnostic.operation, "unit.operation") && strstr(error, "unit-runtime.phys:3:4:"));
    return 0;
}
static int definitions(void) {
    const ps_unit *bases[] = {&PS_METRE, &PS_KILOGRAM, &PS_SECOND, &PS_AMPERE,
                             &PS_KELVIN, &PS_MOLE, &PS_CANDELA};
    const char *symbols[] = {"m", "kg", "s", "A", "K", "mol", "cd"};
    for (unsigned i = 0; i < 7; i++) {
        CHECK(ps_unit_valid(*bases[i]) && bases[i]->scale == 1 && !strcmp(bases[i]->symbol, symbols[i]));
        for (unsigned j = 0; j < 7; j++) CHECK(bases[i]->dimension[j] == (i == j));
        char text[128];
        CHECK(ps_unit_format_dimension(*bases[i], text, sizeof text) == PS_OK && !strcmp(text, symbols[i]));
    }
    ps_unit unit;
    CHECK(ps_unit_multiply(PS_KILOGRAM, PS_ACCELERATION, "N", &unit) == PS_OK && ps_unit_compatible(unit, PS_NEWTON));
    CHECK(ps_unit_divide(PS_JOULE, PS_SECOND, "W", &unit) == PS_OK && ps_unit_compatible(unit, PS_WATT));
    CHECK(ps_unit_divide(PS_NEWTON, (ps_unit){{2},1,"m2"}, "Pa", &unit) == PS_OK && ps_unit_compatible(unit, PS_PASCAL));
    CHECK(ps_unit_power(PS_SECOND, -1, "Hz", &unit) == PS_OK && ps_unit_compatible(unit, PS_HERTZ));
    CHECK(ps_unit_compatible(PS_ONE, PS_RADIAN));
    char text[128] = "preserved";
    CHECK(ps_unit_format_dimension(PS_NEWTON, text, 2) == PS_LIMIT && !strcmp(text, "preserved"));
    CHECK(ps_unit_format_dimension(PS_NEWTON, text, sizeof text) == PS_OK && !strcmp(text,"m kg s^-2"));
    return 0;
}
int main(void) {
    CHECK(definitions() == 0);
    CHECK(rejected(0, PS_NUMERIC) == 0);
    CHECK(rejected(1, PS_INVALID) == 0);
    CHECK(rejected(2, PS_NUMERIC) == 0);
    CHECK(rejected(3, PS_NUMERIC) == 0);
    CHECK(rejected(4, PS_NUMERIC) == 0);
    CHECK(rejected(5, PS_NUMERIC) == 0);
    puts("Unit runtime: original C numeric/invalid status, structured diagnostics and source sites passed");
    return 0;
}
