#include "compiler_diagnostic.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Compiler diagnostic line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void) {
    const struct {const char *text,*source;int line,column;} cases[]={
        {"main.c:12:7: error: broken","main.c",12,7},
        {"analysis.phys:8:4: runtime error: old module failure","analysis.phys",8,4},
        {"/Users/name/Project ä/include/helper α.h:2:4: fatal error: no symbol","/Users/name/Project ä/include/helper α.h",2,4},
        {"lib/file42:7: warning: conversion","lib/file42",7,1},
        {"C:\\Project α\\helper.h:22:3: error: missing","C:\\Project α\\helper.h",22,3},
        {"C:\\Program Files\\model.c(41,9): error C2143: missing ';'","C:\\Program Files\\model.c",41,9},
        {"include/foo (copy).h(9): warning C4244: conversion","include/foo (copy).h",9,1},
        {"  source file.c:3:5: warning: spaced path","source file.c",3,5},
        {"a.h:2147483647:2147483647: error: limit","a.h",2147483647,2147483647},
        {"nested/helper.h(18,2) : fatal error C1083: cannot open file","nested/helper.h",18,2}
    };
    ps_compiler_diagnostic value={0};
    for(unsigned i=0;i<sizeof cases/sizeof cases[0];i++) {
        CHECK(ps_compiler_diagnostic_parse(cases[i].text,&value));
        CHECK(!strcmp(value.source,cases[i].source) && value.line==cases[i].line && value.column==cases[i].column);
        CHECK(strstr(value.message,"error") || strstr(value.message,"warning"));
    }
    const char *invalid[]={"", "error: compiler unavailable", "a.h:2:3: note: previous declaration",
        "a.h:0:3: error: zero", "a.h:2:0: error: zero", "a.h:-1:3: error: negative",
        "a.h:2:-1: error: negative", "a.h:2147483648:3: error: large", "a.h:2:2147483648: error: large",
        "a.h(0): error C1: zero", "a.h(1,0): error C1: zero", "a.h(2,3: error C1: missing delimiter",
        "a.h:2:3: errorless text", "a.h:2:3: warningly text", ":2:3: error: no source", "a\tb.h:2:3: error: control"};
    for(unsigned i=0;i<sizeof invalid/sizeof invalid[0];i++) {
        ps_compiler_diagnostic previous=value;
        CHECK(!ps_compiler_diagnostic_parse(invalid[i],&value) && !memcmp(&value,&previous,sizeof value));
    }
    char large[4300];memset(large,'a',4096);strcpy(large+4096,":2:3: error: oversized path");
    CHECK(!ps_compiler_diagnostic_parse(large,&value));
    CHECK(!ps_compiler_diagnostic_parse(NULL,&value) && !ps_compiler_diagnostic_parse("x.h:1: error: x",NULL));
    puts("Compiler diagnostics: GCC/Clang/MSVC paths, Unicode, coordinates and atomic rejection passed");
    return 0;
}
