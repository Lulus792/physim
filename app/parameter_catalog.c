#include "parameter_catalog.h"
#include <errno.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool number(const char *text, double *value) {
    if (!text || !*text || !value)
        return false;
    char *end;
    errno = 0;
    double parsed = strtod(text, &end);
    if (errno || end == text || *end || !isfinite(parsed))
        return false;
    *value = parsed;
    return true;
}
bool ps_parameter_catalog_restore(ps_parameter_catalog *catalog, const char *name,
                                  const char *value) {
    if (!catalog || !name || !value || catalog->count >= PS_MAX_PARAMETERS ||
        strlen(value) >= sizeof catalog->selected[0])
        return false;
    double parsed;
    if (!number(value, &parsed))
        return false;
    ps_context context = {0};
    context.struct_size = sizeof context;
    context.api_version = PS_API_VERSION;
    if (ps_parameter_override(&context, name, parsed) != PS_OK)
        return false;
    for (uint32_t i = 0; i < catalog->count; i++)
        if (!strcmp(catalog->entries[i].name, name))
            return false;
    uint32_t index = catalog->count++;
    catalog->entries[index] = context.parameters[0];
    catalog->entries[index].minimum = -DBL_MAX;
    catalog->entries[index].maximum = DBL_MAX;
    memcpy(catalog->selected[index], value, strlen(value) + 1);
    return true;
}
ps_result ps_parameter_catalog_value(const ps_parameter_catalog *catalog, uint32_t index,
                                     double *value) {
    if (!catalog || !value || index >= catalog->count || index >= PS_MAX_PARAMETERS)
        return PS_INVALID;
    double selected;
    if (!number(catalog->selected[index], &selected) ||
        selected < catalog->entries[index].minimum ||
        selected > catalog->entries[index].maximum)
        return PS_INVALID;
    *value = selected;
    return PS_OK;
}
bool ps_parameter_catalog_parse(ps_parameter_catalog *catalog, const char *output) {
    if (!catalog || !output)
        return false;
    size_t length = strlen(output);
    if (length >= 8192)
        return false;
    char copy[8192];
    memcpy(copy, output, length + 1);
    char *cursor = strstr(copy, "PHYSIM_PARAMETERS_1\n");
    if (!cursor)
        return false;
    cursor += strlen("PHYSIM_PARAMETERS_1\n");
    char *end;
    unsigned long count = strtoul(cursor, &end, 10);
    if (end == cursor || *end != '\n' || count > PS_MAX_PARAMETERS)
        return false;
    cursor = end + 1;
    ps_context context = {0};
    context.struct_size = sizeof context;
    context.api_version = PS_API_VERSION;
    for (unsigned long i = 0; i < count; i++) {
        char *line_end = strchr(cursor, '\n');
        if (!line_end)
            return false;
        *line_end = 0;
        char *fields[5] = {cursor};
        for (unsigned field = 1; field < 5; field++) {
            char *tab = strchr(fields[field - 1], '\t');
            if (!tab)
                return false;
            *tab = 0;
            fields[field] = tab + 1;
        }
        if (strchr(fields[4], '\t'))
            return false;
        double standard, minimum, maximum, selected;
        if (!number(fields[1], &standard) || !number(fields[2], &minimum) ||
            !number(fields[3], &maximum) ||
            ps_parameter_define(&context, fields[0], fields[4], standard, minimum,
                                maximum, &selected) != PS_OK)
            return false;
        cursor = line_end + 1;
    }
    ps_parameter_catalog updated = {0};
    updated.count = context.parameter_count;
    memcpy(updated.entries, context.parameters, sizeof updated.entries);
    for (uint32_t i = 0; i < updated.count; i++) {
        snprintf(updated.selected[i], sizeof updated.selected[i], "%.17g",
                 updated.entries[i].default_value);
        for (uint32_t old = 0; old < catalog->count && old < PS_MAX_PARAMETERS; old++) {
            double previous;
            if (!strcmp(updated.entries[i].name, catalog->entries[old].name) &&
                ps_parameter_catalog_value(catalog, old, &previous) == PS_OK &&
                previous >= updated.entries[i].minimum &&
                previous <= updated.entries[i].maximum) {
                snprintf(updated.selected[i], sizeof updated.selected[i], "%s",
                         catalog->selected[old]);
                break;
            }
        }
    }
    *catalog = updated;
    return true;
}
