#include "physim/units.h"
#include <stdio.h>
#include <string.h>
int main(void) {
    unsigned mode;
    double value, from, to;
    while (scanf("%u %la %la %la", &mode, &value, &from, &to) == 4) {
        ps_unit a = PS_METRE, b = mode == 1 ? PS_SECOND : PS_METRE;
        a.scale = from; b.scale = to;
        double result = 77;
        ps_result status = ps_convert(value, a, b, mode == 2 ? NULL : &result);
        ps_quantity quantity = {value, a}, converted = {88, PS_SECOND};
        ps_result qstatus = ps_quantity_convert(quantity, b, mode == 2 ? NULL : &converted);
        if (qstatus == PS_OK) {
            if (converted.unit.scale != to || memcmp(converted.unit.dimension, b.dimension, 7) ||
                converted.unit.symbol != b.symbol) return 3;
        } else if (converted.value != 88 || converted.unit.scale != 1 ||
                   memcmp(converted.unit.dimension, PS_SECOND.dimension, 7) ||
                   converted.unit.symbol != PS_SECOND.symbol) return 4;
        if (mode != 2) {
            double alias = value;
            ps_result as = ps_convert(alias, a, b, &alias);
            if (as != status || (as == PS_OK && memcmp(&alias, &result, sizeof alias)) ||
                (as != PS_OK && memcmp(&alias, &value, sizeof alias))) return 5;
            ps_quantity qalias = quantity;
            as = ps_quantity_convert(qalias, b, &qalias);
            if (as != qstatus || (as == PS_OK && memcmp(&qalias.value, &converted.value, sizeof value)) ||
                (as != PS_OK && memcmp(&qalias.value, &value, sizeof value))) return 6;
        }
        printf("%d %a %d %a\n", status, result, qstatus, converted.value);
    }
    return ferror(stdin) ? 2 : 0;
}
