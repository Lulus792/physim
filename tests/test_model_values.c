#include "../app/model_values.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
int main(void) {
    const char *c="/* preface */\r\n// @physim-model-value-v1 length_m cm 0.01 0.1 10 Fadenlänge\r\nstatic const double length_m = 1.5;\r\n"
        "// unrelated length_m = 7\nstatic void f(void) { const double x=7; }\n";
    ps_model_values v;assert(ps_model_values_parse(c,strlen(c),false,&v));assert(v.count==1);
    assert(!strcmp(v.entries[0].id,"length_m") && v.entries[0].value==1.5 && v.entries[0].scale==.01);
    char values[PS_MODEL_VALUES_MAX][64]={{0}};strcpy(values[0],"175,5");double si;
    assert(ps_model_value_parse(&v.entries[0],values[0],&si) && fabs(si-1.755)<1e-12);
    size_t n;char *out=ps_model_values_rewrite(c,strlen(c),&v,values,&n);assert(out && n==strlen(out));
    assert(strstr(out,"static const double length_m = 1.7550000000000001;") || strstr(out,"static const double length_m = 1.7549999999999999;"));
    assert(strstr(out,"// unrelated length_m = 7\nstatic void f(void) { const double x=7; }\n"));free(out);
    const char *invalid[]={"nan","inf","1e999","0","1001","1,2,3","0x1p0","1;exit(0)","","-1"," 150"};
    for(unsigned i=0;i<sizeof invalid/sizeof *invalid;i++)assert(!ps_model_value_parse(&v.entries[0],invalid[i],&si));
    const char *phys="// @physim-model-value-v1 lengthDefault m 1 .1 10 Länge\nlet lengthDefault: Float64 = 1.5\nlet other = lengthDefault\n";
    assert(ps_model_values_parse(phys,strlen(phys),true,&v) && v.count==1);
    strcpy(values[0],"2");out=ps_model_values_rewrite(phys,strlen(phys),&v,values,&n);assert(out && strstr(out,"let lengthDefault: Float64 = 2\nlet other = lengthDefault"));free(out);
    const char *bad[]={
        "// @physim-model-value-v2 x m 1 0 10 x\nstatic const double x = 1;\n",
        "// @physim-model-value-v1 x m 1 0 10 x\nstatic const double y = 1;\n",
        "// @physim-model-value-v1 x m 1 0 10 x\nstatic const double x = 1+2;\n",
        "// @physim-model-value-v1 x m 0 0 10 x\nstatic const double x = 1;\n",
        "// @physim-model-value-v1 x m 1 2 10 x\nstatic const double x = 1;\n",
        "// @physim-model-value-v1 x m 1 0 10 x\nstatic const double x = 1;\n// @physim-model-value-v1 x m 1 0 10 x\nstatic const double x = 2;\n"};
    for(unsigned i=0;i<sizeof bad/sizeof *bad;i++)assert(!ps_model_values_parse(bad[i],strlen(bad[i]),false,&v) && !v.count);
    const char *comment="/*\n// @physim-model-value-v1 x m 1 0 10 x\nstatic const double x = 1;\n*/\n";
    assert(ps_model_values_parse(comment,strlen(comment),false,&v) && !v.count);
    const char *local="void f(void) {\n// @physim-model-value-v1 x m 1 0 10 x\nstatic const double x = 1;\n}\n";
    assert(ps_model_values_parse(local,strlen(local),false,&v) && !v.count);
    const char *indented="func f():\n    // @physim-model-value-v1 x m 1 0 10 x\n    let x: Float64 = 1\n";
    assert(ps_model_values_parse(indented,strlen(indented),true,&v) && !v.count);
    const char *conditional="#if SOMETHING\n// @physim-model-value-v1 x m 1 0 10 x\nstatic const double x = 1;\n#endif\n";
    assert(ps_model_values_parse(conditional,strlen(conditional),false,&v) && !v.count);
    const char *multiline="let quoted = \"\"\"\nA quoted \" word\n// @physim-model-value-v1 x m 1 0 10 x\nlet x: Float64 = 1\n\"\"\"\n";
    assert(ps_model_values_parse(multiline,strlen(multiline),true,&v) && !v.count);
    char many[8192];size_t used=0;
    for(unsigned i=0;i<PS_MODEL_VALUES_MAX;i++)used+=(size_t)snprintf(many+used,sizeof many-used,
        "// @physim-model-value-v1 x%u m 1 0 10 Wert\nstatic const double x%u = 1;\n",i,i);
    assert(ps_model_values_parse(many,used,false,&v) && v.count==PS_MODEL_VALUES_MAX);
    used+=(size_t)snprintf(many+used,sizeof many-used,"// @physim-model-value-v1 extra m 1 0 10 Wert\nstatic const double extra = 1;\n");
    assert(!ps_model_values_parse(many,used,false,&v) && !v.count);
    char *large=malloc(PS_MODEL_SOURCE_MAX+1);assert(large);memset(large,' ',PS_MODEL_SOURCE_MAX+1);
    assert(ps_model_values_parse(large,PS_MODEL_SOURCE_MAX,false,&v) && !v.count);
    assert(!ps_model_values_parse(large,PS_MODEL_SOURCE_MAX+1,false,&v));free(large);
    assert(ps_model_values_parse(c,strlen(c),false,&v));strcpy(values[0],"150");
    out=ps_model_values_rewrite(c,strlen(c),&v,values,&n);assert(out && n==strlen(c) && !memcmp(out,c,n));free(out);
    puts("Explicit typed model values, bounds, decimal validation, exact source preservation and rejected expressions passed");return 0;
}
