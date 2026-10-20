#include "../app/source_outline.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void expect(const char *s,const ps_source_outline *o,size_t at,const char *name,ps_outline_kind kind,size_t line) {
    assert(at<o->count);ps_outline_symbol v=o->symbols[at];
    assert(v.offset<=strlen(s) && v.length<=strlen(s)-v.offset);
    assert(strlen(name)==v.length && !memcmp(s+v.offset,name,v.length));
    assert(v.kind==kind && v.line==line);
}
int main(void) {
    ps_source_outline *o=malloc(sizeof *o);assert(o);
    ps_source_outline_build(NULL,0,false,o);assert(o->status==PS_OUTLINE_OK && !o->count);
    const char *c="#define FALSE_FUNCTION(x) \\\n static void hidden(void) {}\n"
       "/* static int ghost(void) {} */\n"
       "static const double length_m=1.5, mass=1;\n"
       "typedef struct State { double ignored; } State;\n"
       "typedef void (*Callback)(int parameter);\n"
       "static const char *text=\"void phantom() { }\";\n"
       "static void derivative(double time, const double *y) {\n"
       "  int local=0; if(local) { local++; }\n}\n"
       "PS_EXPORT const descriptor *query(unsigned version) { return 0; }\n";
    ps_source_outline_build(c,strlen(c),false,o);
    assert(o->status==PS_OUTLINE_OK && o->count==8);
    expect(c,o,0,"length_m",PS_OUTLINE_BINDING,4);expect(c,o,1,"mass",PS_OUTLINE_BINDING,4);
    expect(c,o,2,"State",PS_OUTLINE_TYPE,5);expect(c,o,3,"State",PS_OUTLINE_TYPE,5);
    expect(c,o,4,"Callback",PS_OUTLINE_TYPE,6);expect(c,o,5,"text",PS_OUTLINE_BINDING,7);
    expect(c,o,6,"derivative",PS_OUTLINE_FUNCTION,8);expect(c,o,7,"query",PS_OUTLINE_FUNCTION,11);
    const char *phys="let länge = 1.5\nfunc derivative(t: Float64) -> Float64:\n    let local = t\n    return local\nstruct State:\n    let value: Float64\n";
    ps_source_outline_build(phys,strlen(phys),true,o);assert(o->status==PS_OUTLINE_OK && o->count==3);
    expect(phys,o,0,"länge",PS_OUTLINE_BINDING,1);expect(phys,o,1,"derivative",PS_OUTLINE_FUNCTION,2);expect(phys,o,2,"State",PS_OUTLINE_TYPE,5);
    const char *decl="int (*callback)(int x)=0;\nint (*factory(void))(int x) { return callback; }\nvoid plain(int argument);\nint array[8];\n";
    ps_source_outline_build(decl,strlen(decl),false,o);assert(o->status==PS_OUTLINE_OK && o->count==4);
    expect(decl,o,0,"callback",PS_OUTLINE_BINDING,1);expect(decl,o,1,"factory",PS_OUTLINE_FUNCTION,2);
    expect(decl,o,2,"plain",PS_OUTLINE_FUNCTION,3);expect(decl,o,3,"array",PS_OUTLINE_BINDING,4);
    ps_source_outline_build("func broken(\n",13,true,o);assert(o->status==PS_OUTLINE_INCOMPLETE && !o->count);
    ps_source_outline_build("int x; /* unfinished",20,false,o);assert(o->status==PS_OUTLINE_INCOMPLETE && o->count==1);
    ps_source_outline_build("int x;\0int hidden;",18,false,o);assert(o->status==PS_OUTLINE_INVALID && !o->count);
    ps_source_outline_build(NULL,1,false,o);assert(o->status==PS_OUTLINE_INVALID);
    ps_source_outline_build("",(size_t)-1,false,o);assert(o->status==PS_OUTLINE_LIMITED);
    char *large=malloc(40000);assert(large);size_t used=0;
    for(unsigned i=0;i<700;i++)used+=(size_t)sprintf(large+used,"int variable_%u;\n",i);
    ps_source_outline_build(large,used,false,o);assert(o->status==PS_OUTLINE_LIMITED && o->count==PS_OUTLINE_LIMIT);
    for(size_t i=0;i<o->count;i++){char name[32];sprintf(name,"variable_%u",(unsigned)i);expect(large,o,i,name,PS_OUTLINE_BINDING,i+1);}
    free(large);
    char *boundary=malloc(PS_OUTLINE_SOURCE_LIMIT);assert(boundary);memset(boundary,' ',PS_OUTLINE_SOURCE_LIMIT);
    ps_source_outline_build(boundary,PS_OUTLINE_SOURCE_LIMIT,false,o);assert(o->status==PS_OUTLINE_OK && !o->count);
    ps_source_outline_build(boundary,PS_OUTLINE_SOURCE_LIMIT,true,o);assert(o->status==PS_OUTLINE_OK && !o->count);
    ps_source_outline_build(boundary,PS_OUTLINE_SOURCE_LIMIT+1u,false,o);assert(o->status==PS_OUTLINE_LIMITED && !o->count);
    free(boundary);free(o);puts("source outline tests passed");return 0;
}
