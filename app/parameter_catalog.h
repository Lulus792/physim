#ifndef PS_PARAMETER_CATALOG_H
#define PS_PARAMETER_CATALOG_H
#include "physim/experiment.h"

typedef struct {
    uint32_t count;
    ps_parameter entries[PS_MAX_PARAMETERS];
    ps_parameter_unit units[PS_MAX_PARAMETERS];
    char selected[PS_MAX_PARAMETERS][64];
} ps_parameter_catalog;

/* Parses the runner's bounded --describe output atomically. Existing valid
 * selections with the same name survive a rebuild. */
bool ps_parameter_catalog_parse(ps_parameter_catalog *catalog, const char *output);
/* Loads one project-file selection before the module's bounds are known. */
bool ps_parameter_catalog_restore(ps_parameter_catalog *catalog, const char *name,
                                  const char *value);
ps_result ps_parameter_catalog_value(const ps_parameter_catalog *catalog, uint32_t index,
                                     double *value);
/* Editor input is in the declared display unit; return an SI value. */
ps_result ps_parameter_catalog_input(const ps_parameter_catalog *catalog,uint32_t index,
                                     const char *text,double *value);
double ps_parameter_catalog_display(const ps_parameter_catalog *catalog,uint32_t index,
                                      double si_value);
/* One complete definition in the runner's v2 tab-separated format, excluding
 * the name. Defaults/bounds are SI; declared units carry symbol/scale/dimensions.
 * No raw structure bytes or local paths are serialized. */
bool ps_parameter_catalog_definition(const ps_parameter_catalog *catalog,uint32_t index,
                                     char *text,size_t capacity);
#endif
