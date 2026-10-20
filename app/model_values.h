#ifndef PHYSIM_MODEL_VALUES_H
#define PHYSIM_MODEL_VALUES_H
#include <stdbool.h>
#include <stddef.h>
#include "text_document.h"
#define PS_MODEL_VALUES_MAX 32
#define PS_MODEL_SOURCE_MAX (256u*1024u)
typedef struct {
    char id[64],label[128],unit[32];
    double scale,minimum,maximum,value;
    size_t offset,length;
} ps_model_value;
typedef struct {unsigned count;ps_model_value entries[PS_MODEL_VALUES_MAX];char error[192];} ps_model_values;
typedef struct {
    ps_text_document source;
    ps_model_values bindings;
    char project[4096],text[PS_MODEL_VALUES_MAX][64],requested[PS_MODEL_VALUES_MAX][64];
    double compiled[PS_MODEL_VALUES_MAX],submitted[PS_MODEL_VALUES_MAX];
    uint64_t revision,submitted_revision;
    bool loaded,has_compiled,building,stop_build,ready,build_requested;
    char error[256];
} ps_model_edit;
/* Explicit v1 annotations bind only a complete top-level numeric declaration.
 * C: static const double ID = NUMBER;
 * Physim: let ID: Float64 = NUMBER
 * No macros, expressions, arbitrary identifiers or runtime parameter rewriting. */
bool ps_model_values_parse(const char *source,size_t length,bool physim,ps_model_values *out);
bool ps_model_value_parse(const ps_model_value *binding,const char *text,double *si);
/* Caller owns the returned source. All replacements use the validated offsets
 * of this exact snapshot; unchanged bytes are copied verbatim. */
char *ps_model_values_rewrite(const char *source,size_t length,const ps_model_values *bindings,
                              const char values[PS_MODEL_VALUES_MAX][64],size_t *out_length);
#endif
