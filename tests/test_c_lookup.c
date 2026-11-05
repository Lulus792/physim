#include "c_signatures.h"
#include "test_allocator.h"
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"C lookup line %d: %s\n",__LINE__,#x);return 1;}}while(0)
typedef struct {const char *text,*name;ps_c_name_kind kind;bool callable;const char *parameters;} example;
static const example examples[]={
 {"void f(){int ps_vadd=ps_vadd(|","ps_vadd",PS_C_NAME_DECLARATION,false,""},
 {"int api(double x); void f(){int api=api(|","api",PS_C_NAME_DECLARATION,false,""},
 {"int api(double x); void f(){api(|); int api=0;}","api",PS_C_NAME_DECLARATION,true,"double"},
 {"void f(){typedef int api; api(|);}","api",PS_C_NAME_TYPEDEF,false,""},
 {"typedef int api; void f(){api(|);}","api",PS_C_NAME_TYPEDEF,false,""},
 {"void f(){enum {api=1};api(|);}","api",PS_C_NAME_ENUMERATOR,false,""},
 {"int api(double x); void f(){enum {api=api(|)};}","api",PS_C_NAME_DECLARATION,true,"double"},
 {"int api(double x); void f(){enum {other=api(|)};}","api",PS_C_NAME_DECLARATION,true,"double"},
 {"int api(double x); struct api {int api;}; void f(){api(|);}","api",PS_C_NAME_DECLARATION,true,"double"},
 {"int api(double x); void f(){api: api(|);}","api",PS_C_NAME_DECLARATION,true,"double"},
 {"int api(double x); void f(int api){api(|);}","api",PS_C_NAME_PARAMETER,false,""},
 {"int api(double x); void f(int api){api(|","api",PS_C_NAME_PARAMETER,false,""},
 {"void f(int (*api)(float x)){api(|);}","api",PS_C_NAME_PARAMETER,true,"float"},
 {"void f(int api(float x)){api(|);}","api",PS_C_NAME_PARAMETER,true,"float"},
 {"int (*api)(double x)=api(|","api",PS_C_NAME_DECLARATION,true,"double"},
 {"void f(){int (*api)(double x)=api(|","api",PS_C_NAME_DECLARATION,true,"double"},
 {"int (*api)(double x); void f(){int (**api)(int y)=0;api(|);}","api",PS_C_NAME_DECLARATION,false,""},
 {"int (*api)(double x); void f(){int (*api[3])(int y);api(|);}","api",PS_C_NAME_DECLARATION,false,""},
 {"int api(double x); void f(){{int api;}api(|);}","api",PS_C_NAME_DECLARATION,true,"double"},
 {"int api(double x); void f(){for(int api=0;api<1;api++){}api(|);}","api",PS_C_NAME_DECLARATION,true,"double"},
 {"int api(double x); void f(){for(int api=0;api<1;api++)api(|);}","api",PS_C_NAME_DECLARATION,false,""},
 {"int api(double x); void f(){for(int api=api(|);;){}}","api",PS_C_NAME_DECLARATION,false,""},
 {"int api(double x); void f(){int api[sizeof(api(|))];}","api",PS_C_NAME_DECLARATION,true,"double"},
 {"int api(double x); void other(){int api;} void f(){api(|);}","api",PS_C_NAME_DECLARATION,true,"double"},
 {"void f(){api(|);}","api",PS_C_NAME_NONE,false,""},
 {"void f(){api(|);} int api(double x);","api",PS_C_NAME_NONE,false,""},
 {"int api(double x); void f(){/*int api;*/ const char *s=\"int api;\";api(|);}","api",PS_C_NAME_DECLARATION,true,"double"},
 {"int api(double x); void f(){double api(float y);api(|);}","api",PS_C_NAME_DECLARATION,true,"float"},
 {"int f(api) int api; {api(|);}","api",PS_C_NAME_PARAMETER,false,""},
 {"int f(api) int (*api)(double x); {api(|);}","api",PS_C_NAME_PARAMETER,true,"double"},
 {"int f(api) int (*api)(double x); {return 0;} void g(){api(|);}","api",PS_C_NAME_NONE,false,""},
 {"void proto(int api); void f(){api(|);}","api",PS_C_NAME_NONE,false,""},
 {"void proto(int (*cb)(int api)); void f(){api(|);}","api",PS_C_NAME_NONE,false,""},
 {"void proto(int api,int later[sizeof(api(|))]);","api",PS_C_NAME_PARAMETER,false,""},
 {"int Größe(double wert); void f(){Größe(|);}","Größe",PS_C_NAME_DECLARATION,true,"double"},
 {"int Gr\\u00f6sse(double wert); void f(){Gr\\u00f6sse(|);}","Gr\\u00f6sse",PS_C_NAME_DECLARATION,true,"double"},
 {"int $api(double wert); void f(){$api(|);}","$api",PS_C_NAME_DECLARATION,true,"double"},
 {"void f(){enum {api};} void g(){api(|);}","api",PS_C_NAME_NONE,false,""},
 {"struct S {enum {api=1} field;}; void f(){api(|);}","api",PS_C_NAME_ENUMERATOR,false,""},
 {"int api(double x); void f(enum {api=1} p){api(|);}","api",PS_C_NAME_ENUMERATOR,false,""},
 {"int api(double x); void proto(enum {api=1} p); void f(){api(|);}","api",PS_C_NAME_DECLARATION,true,"double"},
 {"int api(double x); void proto(enum {api=1} p,int q[sizeof(api(|))]);","api",PS_C_NAME_ENUMERATOR,false,""},
 {"int api(double x); void f(){if(sizeof(enum {api=1}))api(|);}","api",PS_C_NAME_ENUMERATOR,false,""},
 {"int api(double x); void f(){if(sizeof(enum {api=1})){}api(|);}","api",PS_C_NAME_DECLARATION,true,"double"},
 {"int api(double x); void f(){while(sizeof(enum {api=1}))api(|);}","api",PS_C_NAME_ENUMERATOR,false,""},
 {"int api(double x); void f(){while(sizeof(enum {api=1})){}api(|);}","api",PS_C_NAME_DECLARATION,true,"double"},
 {"int api(double x); void f(){switch(sizeof(enum {api=1})){default:api(|);}}","api",PS_C_NAME_ENUMERATOR,false,""},
 {"int api(double x); void f(){switch(sizeof(enum {api=1})){default:;}api(|);}","api",PS_C_NAME_DECLARATION,true,"double"},
 {"int api(double x); void f(){if(1)(void)sizeof(enum {api=1});else api(|);}","api",PS_C_NAME_DECLARATION,true,"double"},
 {"int api(double x); void f(){do (void)sizeof(enum {api=1}); while(api(|));}","api",PS_C_NAME_DECLARATION,true,"double"},
 {"int api(double x); int (*factory(int own))(double api){api(|);}","api",PS_C_NAME_DECLARATION,true,"double"},
 {"int (*factory(int own))(double api){api(|);}","api",PS_C_NAME_NONE,false,""},
};
static int fixture(const char *marked,char *text,size_t capacity,size_t *cursor){
    const char *at=strchr(marked,'|');if(!at || strlen(marked)>=capacity)return 0;
    *cursor=(size_t)(at-marked);memcpy(text,marked,*cursor);strcpy(text+*cursor,at+1);return 1;
}
static int vectors(void){
    size_t failures=0;
    for(size_t k=0;k<sizeof examples/sizeof *examples;k++){
        const example *v=&examples[k];char text[1024],actual[1024]={0};size_t cursor;
        CHECK(fixture(v->text,text,sizeof text,&cursor));test_allocator a={0};ps_c_bindings_source *source=NULL;
        CHECK(ps_c_bindings_source_create(text,strlen(text),test_domain(&a),&source)==PS_OK);
        size_t held=a.live_bytes;ps_c_parameter parameters[32];
        ps_c_lookup_result r=ps_c_bindings_source_lookup(source,cursor,v->name,strlen(v->name),parameters,32,test_domain(&a));
        if(r.status!=PS_OK || r.found!=(v->kind!=PS_C_NAME_NONE) || r.kind!=v->kind || r.callable!=v->callable){
            fprintf(stderr,"C lookup vector %zu: %s\nstatus=%u found=%d kind=%u callable=%d expected=%u/%d\n",k,v->text,(unsigned)r.status,r.found,(unsigned)r.kind,r.callable,(unsigned)v->kind,v->callable);
            ps_c_bindings_source_destroy(source);CHECK(!a.live_bytes && !a.live_blocks && !a.invalid);failures++;continue;
        }
        if(r.found){CHECK(r.declaration.name_end-r.declaration.name_start==strlen(v->name));CHECK(!memcmp(text+r.declaration.name_start,v->name,strlen(v->name)));}
        for(size_t p=0;p<r.parameter_count;p++){
            char type[256];CHECK(!parameters[p].type_missing);
            CHECK(ps_c_binding_type(text,strlen(text),parameters[p].declaration,type,sizeof type)>0);
            if(p)strcat(actual,"|");
            strcat(actual,type);
        }
        if(strcmp(actual,v->parameters)){
            fprintf(stderr,"C lookup parameter vector %zu: actual=%s expected=%s\n",k,actual,v->parameters);
            ps_c_bindings_source_destroy(source);CHECK(!a.live_bytes && !a.live_blocks && !a.invalid);failures++;continue;
        }
        ps_c_lookup_result count=ps_c_bindings_source_lookup(source,cursor,v->name,strlen(v->name),NULL,0,test_domain(&a));
        CHECK(count.status==PS_OK && count.parameter_count==r.parameter_count && count.callable==r.callable && count.kind==r.kind);
        CHECK(!a.invalid && a.live_bytes==held);ps_c_bindings_source_destroy(source);CHECK(!a.live_bytes && !a.live_blocks && !a.invalid);
    }
    if(failures)return 1;
    printf("C ordinary lookup: %zu initializer/namespace/shadow/scope/callback vectors passed\n",sizeof examples/sizeof *examples);return 0;
}
static int failures(void){
    const char text[]="int f(a,b) int a; double b; {return a;}";test_allocator a={0};ps_c_bindings_source *source=NULL;
    CHECK(ps_c_bindings_source_create(text,strlen(text),test_domain(&a),&source)==PS_OK);
    size_t held=a.live_bytes,attempts=a.attempts;ps_c_parameter p[8];
    ps_c_lookup_result good=ps_c_bindings_source_lookup(source,strlen(text),"f",1,p,8,test_domain(&a));
    CHECK(good.status==PS_OK && good.callable && good.parameter_count==2);size_t allocations=a.attempts-attempts;CHECK(allocations>0);
    for(size_t i=1;i<=allocations;i++){
        a.fail_on=a.attempts+i;
        ps_c_lookup_result bad=ps_c_bindings_source_lookup(source,strlen(text),"f",1,p,8,test_domain(&a));
        CHECK(bad.status==PS_MEMORY && !bad.found && !bad.callable && !bad.parameter_count && !bad.declaration.name_end);
        CHECK(a.live_bytes==held && !a.invalid);a.fail_on=0;
        CHECK(ps_c_bindings_source_lookup(source,strlen(text),"f",1,p,8,test_domain(&a)).status==PS_OK);
    }
    ps_c_lookup_result limited=ps_c_bindings_source_lookup(source,strlen(text),"f",1,p,1,test_domain(&a));
    CHECK(limited.status==PS_LIMIT && limited.parameter_count==2 && !limited.found && !limited.callable && !limited.declaration.name_end);
    CHECK(ps_c_bindings_source_lookup(source,strlen(text)+1,"f",1,p,8,test_domain(&a)).status==PS_INVALID);
    CHECK(ps_c_bindings_source_lookup(source,0,NULL,1,p,8,test_domain(&a)).status==PS_INVALID);
    CHECK(ps_c_bindings_source_lookup(source,0,"",0,p,8,test_domain(&a)).status==PS_INVALID);
    CHECK(ps_c_bindings_source_lookup(source,0,"f",1,NULL,1,test_domain(&a)).status==PS_INVALID);
    ps_c_binding bindings[16];CHECK(ps_c_bindings_source_at(source,strlen(text),bindings,16,test_domain(&a)).status==PS_OK);
    CHECK(!strcmp(text,"int f(a,b) int a; double b; {return a;}"));
    ps_c_bindings_source_destroy(source);CHECK(!a.live_bytes && !a.live_blocks && !a.invalid);
    printf("C ordinary lookup: %zu scratch failures, clean capacity counts, immutable source and subsequent query passed\n",allocations);return 0;
}
static int flags(void){
    const char text[]="void empty(void); void unknown(); int var(const char *fmt,...); int old(a,b) int a; {return a;}";
    ps_c_bindings_source *source=NULL;CHECK(ps_c_bindings_source_create(text,strlen(text),(ps_allocator){0},&source)==PS_OK);
    ps_c_parameter p[8];size_t cursor=strlen(text);
    ps_c_lookup_result r=ps_c_bindings_source_lookup(source,cursor,"empty",5,p,8,(ps_allocator){0});
    CHECK(r.status==PS_OK && r.callable && !r.parameter_count && !r.function.unspecified);
    r=ps_c_bindings_source_lookup(source,cursor,"unknown",7,p,8,(ps_allocator){0});
    CHECK(r.status==PS_OK && r.callable && !r.parameter_count && r.function.unspecified);
    r=ps_c_bindings_source_lookup(source,cursor,"var",3,p,8,(ps_allocator){0});
    CHECK(r.status==PS_OK && r.callable && r.parameter_count==1 && r.function.variadic_end-r.function.variadic_start==3);
    r=ps_c_bindings_source_lookup(source,cursor,"old",3,p,8,(ps_allocator){0});
    CHECK(r.status==PS_OK && r.callable && r.parameter_count==2 && r.function.old_style && p[1].type_missing);
    CHECK(!p[0].type_missing && !memcmp(text+p[0].declaration.type_start,"int",3));
    ps_c_bindings_source_destroy(source);
    CHECK(ps_c_bindings_source_create("",0,(ps_allocator){0},&source)==PS_OK);
    r=ps_c_bindings_source_lookup(source,0,"absent",6,p,8,(ps_allocator){0});CHECK(r.status==PS_OK && !r.found);
    ps_c_bindings_source_destroy(source);
    const char unicode[]="int Größe;";CHECK(ps_c_bindings_source_create(unicode,strlen(unicode),(ps_allocator){0},&source)==PS_OK);
    r=ps_c_bindings_source_lookup(source,7,"Größe",6,p,8,(ps_allocator){0});CHECK(r.status==PS_INVALID);
    ps_c_bindings_source_destroy(source);puts("C ordinary lookup: void/unspecified/variadic/old-style/empty/UTF-8 contracts passed");return 0;
}
int main(void){CHECK(!vectors());CHECK(!failures());CHECK(!flags());return 0;}
