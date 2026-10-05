#include "parameter_catalog.h"
#include "number_parse.h"
#include <errno.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool number(const char *text, double *value) {
    if (!text || !*text || !value)
        return false;
    return ps_parse_finite_number(text,NULL,value);
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
    catalog->units[index].scale=1;
    memcpy(catalog->selected[index], value, strlen(value) + 1);
    return true;
}
ps_result ps_parameter_catalog_value(const ps_parameter_catalog *catalog, uint32_t index,
                                     double *value) {
    if(!catalog || index>=catalog->count || index>=PS_MAX_PARAMETERS) return PS_INVALID;
    return ps_parameter_catalog_input(catalog,index,catalog->selected[index],value);
}
double ps_parameter_catalog_display(const ps_parameter_catalog *catalog,uint32_t index,double value) {
    return value/(catalog->units[index].declared?catalog->units[index].scale:1);
}
static bool same_number(double a,double b) {
    return a==b && (a!=0 || !!signbit(a)==!!signbit(b));
}
ps_result ps_parameter_catalog_input(const ps_parameter_catalog *catalog,uint32_t index,
                                     const char *text,double *value) {
    if (!catalog || !value || index >= catalog->count || index >= PS_MAX_PARAMETERS)
        return PS_INVALID;
    double selected;
    if (!number(text, &selected)) return PS_INVALID;
    double scale=catalog->units[index].declared?catalog->units[index].scale:1;
    if(!isfinite(scale) || scale<=0) return PS_INVALID;
    /* Preserve unchanged selections, defaults and boundaries exactly in SI:
     * display division/multiplication can otherwise introduce a rounding step. */
    if(same_number(selected,catalog->entries[index].value/scale)) selected=catalog->entries[index].value;
    else if(same_number(selected,catalog->entries[index].default_value/scale)) selected=catalog->entries[index].default_value;
    else if(same_number(selected,catalog->entries[index].minimum/scale)) selected=catalog->entries[index].minimum;
    else if(same_number(selected,catalog->entries[index].maximum/scale)) selected=catalog->entries[index].maximum;
    else {
        double input=selected;selected*=scale;
        if(input!=0 && selected==0)return PS_INVALID;
    }
    if (!isfinite(selected) ||
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
    char *cursor = strstr(copy, "PHYSIM_PARAMETERS_2\n");
    bool typed=cursor!=NULL;
    if(!cursor)cursor=strstr(copy,"PHYSIM_PARAMETERS_1\n");
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
        char *fields[8] = {cursor};
        unsigned field_count=typed?8:5;
        for (unsigned field = 1; field < field_count; field++) {
            char *tab = strchr(fields[field - 1], '\t');
            if (!tab)
                return false;
            *tab = 0;
            fields[field] = tab + 1;
        }
        if (strchr(fields[field_count-1], '\t'))
            return false;
        double standard, minimum, maximum, selected;
        if (!number(fields[1], &standard) || !number(fields[2], &minimum) ||
            !number(fields[3], &maximum))
            return false;
        ps_result status;
        if(typed && *fields[5]) {
            char metadata[512];
            int n=snprintf(metadata,sizeof metadata,"parameter_unit.%s=%s\nparameter_scale.%s=%s\n"
                "parameter_dimension.%s=%s\n",fields[0],fields[5],fields[0],fields[6],fields[0],fields[7]);
            ps_parameter_unit unit;
            if(n<0 || n>=(int)sizeof metadata ||
               ps_parameter_unit_parse(metadata,fields[0],&unit)!=PS_OK || !unit.declared) return false;
            ps_unit display={{0},unit.scale,unit.symbol};memcpy(display.dimension,unit.dimension,7);
            status=ps_parameter_define_unit(&context,fields[0],fields[4],display,standard,minimum,maximum,&selected);
        } else {
            if(typed && (strcmp(fields[6],"1") || strcmp(fields[7],"0,0,0,0,0,0,0")))return false;
            status=ps_parameter_define(&context,fields[0],fields[4],standard,minimum,maximum,&selected);
        }
        if(status!=PS_OK)return false;
        cursor = line_end + 1;
    }
    ps_parameter_catalog updated = {0};
    updated.count = context.parameter_count;
    memcpy(updated.entries, context.parameters, sizeof updated.entries);
    for(uint32_t i=0;i<updated.count;i++)ps_parameter_unit_read(&context,i,&updated.units[i]);
    for (uint32_t i = 0; i < updated.count; i++) {
        snprintf(updated.selected[i], sizeof updated.selected[i], "%.17g",
                 ps_parameter_catalog_display(&updated,i,updated.entries[i].default_value));
        for (uint32_t old = 0; old < catalog->count && old < PS_MAX_PARAMETERS; old++) {
            double previous;
            if (!strcmp(updated.entries[i].name, catalog->entries[old].name) &&
                ps_parameter_catalog_value(catalog, old, &previous) == PS_OK &&
                previous >= updated.entries[i].minimum &&
                previous <= updated.entries[i].maximum &&
                (!catalog->units[old].declared || !updated.units[i].declared ||
                 !memcmp(catalog->units[old].dimension,updated.units[i].dimension,7))) {
                double display=ps_parameter_catalog_display(&updated,i,previous);
                if(!isfinite(display) || (previous!=0 && display==0))return false;
                snprintf(updated.selected[i], sizeof updated.selected[i], "%.17g",display);
                updated.entries[i].value=previous;
                break;
            }
        }
    }
    *catalog = updated;
    return true;
}
