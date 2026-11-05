#include "c_signatures.h"
#include "test_allocator.h"
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"C signatures line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static ps_c_function functions[64];
static ps_c_parameter parameters[256];
typedef struct {const char *text,*names,*counts,*types;} example;
static const example examples[]={
 {"int sum(int a, const char *b);","sum","2","int|const char *"},
 {"void empty(void); void unspecified(); void defined(){}","empty,unspecified,defined","0,0,0",""},
 {"int log(const char *format, ...);","log","1","const char *"},
 {"int unnamed(int, const char *, int (*)(double nested));","unnamed","3","int|const char *|int (*)(double nested)"},
 {"void step(int (*cb)(double x), int y){cb(1);}","step,cb","2,1","int (*)(double x)|int|double"},
 {"void step(int cb(double x)){}","step,cb","1,1","int (double x)|double"},
 {"int (*factory(int own))(double returned){}","factory","1","int"},
 {"int *return_pointer(double x){}","return_pointer","1","double"},
 {"int (*cb)(double x);","cb","1","double"},
 {"int (**double_pointer)(double x); int (*array[3])(double x);","","",""},
 {"typedef int Fn(double x); Fn alias;","","",""},
 {"struct S {int (*field)(double x);};","","",""},
 {"void outer(void){int (*cb)(double x); {float (*cb)(int y);}}","outer,cb,cb","0,1,1","double|int"},
 {"void f(void){for(int (*cb)(double x)=0; cb; ) cb(1); cb(2);}","f,cb","0,1","double"},
 {"extern int f(int x), g(double y);","f,g","1,1","int|double"},
 {"int (*a)(int x)=0, (*b)(float y)=0;","a,b","1,1","int|float"},
 {"void f(void){int (*cb)(double x)=call(","f","0",""},
 {"int f(int x){call(","f","1","int"},
 {"const int * const f(int x){call(","f","1","int"},
 {"int f(a,b) int a; const char *b; {return a;}","f","2","int|const char *"},
 {"int f(cb) int (*cb)(double x); {return cb(1);}","f,cb","1,1","int (*)(double x)|double"},
 {"int f(a,b) int a; {return a;}","f","2","int|<missing>"},
 {"int f(cb) int (*cb)(double x); {return cb(1);} void after(void){}","f,cb,after","1,1,0","int (*)(double x)|double"},
 {"int incomplete(int","","",""},
 {"/* int fake(int x); */ const char *s=\"int fake(int x);\"; int real(int x);","real","1","int"},
 {"int Größe(int wert); int Gr\\u00f6sse(float wert); int $call(double wert);","Größe,Gr\\u00f6sse,$call","1,1,1","int|float|double"},
 {"void f(void){for(int (*cb)(double x)=0; cb;){cb(1);}}","f,cb","0,1","double"},
 {"int f(a,b) int a,b; {return a;}","f","2","int|int"},
 {"int f(a) int a; double a; {return 0;}","f","1","<missing>"},
 {"int f(cb) int (*cb)(double x); {call(","f,cb","1,1","int (*)(double x)|double"},
 {"int *f(int x){call(","f","1","int"},
 {"int (*f(int x))(double result){call(","f","1","int"},
 {"int (*unknown)(); int (*empty)(void);","unknown,empty","0,0",""},
};
static int type_text(const char *text,ps_c_parameter p,char *out,size_t capacity){
    if(p.type_missing){snprintf(out,capacity,"<missing>");return 1;}
    if(p.declaration.name_end){size_t n=ps_c_binding_type(text,strlen(text),p.declaration,out,capacity);return n>0 && n<=capacity;}
    size_t a=p.start,b=p.end;while(a<b && (text[a]==' ' || text[a]=='\n'))a++;while(b>a && (text[b-1]==' ' || text[b-1]=='\n'))b--;
    if(b-a>=capacity)return 0;
    memcpy(out,text+a,b-a);out[b-a]=0;return 1;
}
static int vectors(void){
    for(size_t k=0;k<sizeof examples/sizeof *examples;k++){
        const example *v=&examples[k];test_allocator a={0};ps_c_bindings_source *source=NULL;
        CHECK(ps_c_bindings_source_create(v->text,strlen(v->text),test_domain(&a),&source)==PS_OK);
        size_t attempts=a.attempts,held=a.live_bytes;char names[512]={0},counts[256]={0},types[2048]={0};
        ps_c_signatures_result r=ps_c_bindings_source_signatures(source,functions,64,parameters,256,test_domain(&a));
        CHECK(r.status==PS_OK && a.attempts>attempts && a.live_bytes==held && !a.invalid);
        for(size_t i=0;i<r.function_count;i++){
            ps_c_function f=functions[i];ps_c_binding b=f.declaration;CHECK(b.name_start<b.name_end && b.name_end<=strlen(v->text));
            CHECK(f.parameters_start<f.parameters_end && v->text[f.parameters_start]=='(' && v->text[f.parameters_end-1]==')');
            CHECK(f.scope_start<=f.visible_from && f.visible_from<=strlen(v->text) && f.scope_end<=strlen(v->text));
            if(i){strcat(names,",");strcat(counts,",");CHECK(functions[i-1].declaration.name_start<b.name_start);}
            strncat(names,v->text+b.name_start,b.name_end-b.name_start);char number[32];snprintf(number,sizeof number,"%zu",f.parameter_count);strcat(counts,number);
            CHECK(f.parameter_first<=r.parameter_count && f.parameter_count<=r.parameter_count-f.parameter_first);
            for(size_t j=0;j<f.parameter_count;j++){
                if(types[0])strcat(types,"|");
                char type[512];CHECK(type_text(v->text,parameters[f.parameter_first+j],type,sizeof type));strcat(types,type);
            }
        }
        if(strcmp(names,v->names) || strcmp(counts,v->counts) || strcmp(types,v->types)){
            fprintf(stderr,"Signature vector %zu source=%s status=%u\nnames=%s counts=%s types=%s\nexpected=%s / %s / %s\n",k,v->text,(unsigned)r.status,names,counts,types,v->names,v->counts,v->types);return 1;
        }
        ps_c_signatures_result query=ps_c_bindings_source_signatures(source,NULL,0,NULL,0,test_domain(&a));
        CHECK(query.status==PS_OK && query.function_count==r.function_count && query.parameter_count==r.parameter_count && query.has_error==r.has_error);
        ps_c_bindings_source_destroy(source);CHECK(!a.invalid && !a.live_bytes && !a.live_blocks);
    }
    printf("C signature declarations: %zu actual declarator/prototype/pointer/legacy/recovery vectors passed\n",sizeof examples/sizeof *examples);return 0;
}
static int flags_scopes(void){
    const char text[]="void empty(void); void unspecified(); void defined(){} int var(const char *fmt,...); void outer(int (*cb)(double x)){for(int (*local)(int y)=0;local;) local(1); cb(1);}";
    ps_c_bindings_source *source=NULL;CHECK(ps_c_bindings_source_create(text,strlen(text),(ps_allocator){0},&source)==PS_OK);
    ps_c_signatures_result r=ps_c_bindings_source_signatures(source,functions,64,parameters,256,(ps_allocator){0});CHECK(r.status==PS_OK && r.function_count==7);
    CHECK(!functions[0].unspecified && functions[1].unspecified && !functions[2].unspecified && functions[2].definition);
    CHECK(functions[3].parameter_count==1 && functions[3].variadic_end-functions[3].variadic_start==3 && !memcmp(text+functions[3].variadic_start,"...",3));
    CHECK(!functions[4].pointer && functions[4].definition && functions[5].pointer && functions[5].declaration.parameter);
    CHECK(functions[5].scope_start==(size_t)(strchr(strstr(text,"void outer"),'{')-text)+1 && functions[5].scope_end==strlen(text) && !functions[5].scope_open && functions[5].scope_depth==1);
    CHECK(functions[6].scope_depth==2 && functions[6].scope_start==(size_t)(strstr(text,"for(")-text) && functions[6].scope_end==(size_t)(strstr(text,"; cb(")-text)+1 && !functions[6].scope_open);
    ps_c_bindings_source_destroy(source);
    const char prefix[]="int f(int (*cb)(double x)){cb(";source=NULL;
    CHECK(ps_c_bindings_source_create(prefix,strlen(prefix),(ps_allocator){0},&source)==PS_OK);
    r=ps_c_bindings_source_signatures(source,functions,64,parameters,256,(ps_allocator){0});CHECK(r.status==PS_OK && r.has_error && r.function_count==2 && functions[1].scope_open && functions[1].scope_end==strlen(prefix));
    ps_c_bindings_source_destroy(source);return 0;
}
static int failures(void){
    const char text[]="int f(cb,a) int (*cb)(double x); int a; {return cb(a);} int g(x) double x; {return x;}";
    test_allocator owner={0};ps_c_bindings_source *source=NULL;CHECK(ps_c_bindings_source_create(text,strlen(text),test_domain(&owner),&source)==PS_OK);
    size_t held=owner.live_bytes,attempts=owner.attempts;test_allocator probe={0};
    ps_c_signatures_result r=ps_c_bindings_source_signatures(source,functions,64,parameters,256,test_domain(&probe));CHECK(r.status==PS_OK && r.function_count==3 && r.parameter_count==4 && probe.attempts>=3 && !probe.live_bytes);
    size_t faults=probe.attempts;
    for(size_t n=1;n<=faults;n++){
        test_allocator a={.fail_on=n};r=ps_c_bindings_source_signatures(source,functions,64,parameters,256,test_domain(&a));
        CHECK(r.status==PS_MEMORY && !r.function_count && !r.parameter_count && !a.invalid && !a.live_bytes && !a.live_blocks);
        a.fail_on=0;r=ps_c_bindings_source_signatures(source,functions,64,parameters,256,test_domain(&a));CHECK(r.status==PS_OK && r.function_count==3 && r.parameter_count==4 && !a.invalid && !a.live_bytes);
    }
    CHECK(owner.live_bytes==held && owner.attempts==attempts);
    ps_c_binding locals[32];ps_c_bindings_result b=ps_c_bindings_source_at(source,(size_t)(strstr(text,"cb(a)")-text)+3,locals,32,test_domain(&probe));CHECK(b.status==PS_OK && b.count==2);
    ps_c_bindings_source_destroy(source);CHECK(!owner.invalid && !owner.live_bytes && !owner.live_blocks && !probe.invalid && !probe.live_bytes);
    printf("C signature scratch failures: %zu/%zu clean counts, immutable source, retry and subsequent local-query preservation passed\n",faults,faults);return 0;
}
static int bounds(void){
    ps_c_signatures_result r=ps_c_bindings_source_signatures(NULL,functions,64,parameters,256,(ps_allocator){0});CHECK(r.status==PS_INVALID);
    ps_c_bindings_source *source=NULL;const char *text="int f(int a,double b); int g(float c);";
    CHECK(ps_c_bindings_source_create(text,strlen(text),(ps_allocator){0},&source)==PS_OK);
    struct {ps_c_function f;unsigned char guard[16];} f;struct {ps_c_parameter p;unsigned char guard[16];} p;memset(&f,0xa5,sizeof f);memset(&p,0x5a,sizeof p);
    r=ps_c_bindings_source_signatures(source,&f.f,1,&p.p,1,(ps_allocator){0});CHECK(r.status==PS_LIMIT && r.function_count==2 && r.parameter_count==3);
    for(size_t i=0;i<16;i++)CHECK(f.guard[i]==0xa5 && p.guard[i]==0x5a);
    r=ps_c_bindings_source_signatures(source,NULL,0,parameters,256,(ps_allocator){0});CHECK(r.status==PS_LIMIT && r.function_count==2 && r.parameter_count==3);
    r=ps_c_bindings_source_signatures(source,NULL,1,NULL,0,(ps_allocator){0});CHECK(r.status==PS_INVALID && !r.function_count);
    ps_c_bindings_source_destroy(source);return 0;
}
static int prefixes(void){
    const char *text="int proto(int x); void step(int (*cb)(double nested)){int (*local)(int y)=0; {float (*local)(float z)=0;} for(int (*loop)(int t)=0;loop;) loop(1); cb(1);} int legacy(a,b) int a; const char *b; {return a;}";
    size_t bytes=strlen(text),passed=0,invalid=0;
    for(size_t size=0;size<=bytes;size++){
        test_allocator a={0};ps_c_bindings_source *source=NULL;CHECK(ps_c_bindings_source_create(text,size,test_domain(&a),&source)==PS_OK);
        ps_c_signatures_result r=ps_c_bindings_source_signatures(source,functions,64,parameters,256,test_domain(&a));
        CHECK(r.status==PS_OK || r.status==PS_INVALID);
        if(r.status==PS_INVALID){invalid++;CHECK(!r.function_count && !r.parameter_count);}
        else{
            passed++;
            for(size_t i=0;i<r.function_count;i++){
                ps_c_function f=functions[i];CHECK(f.declaration.name_start<f.declaration.name_end && f.declaration.name_end<=size);
                CHECK(f.parameters_start<f.parameters_end && f.parameters_end<=size && f.scope_start<=f.scope_end && f.scope_end<=size && f.visible_from<=size);
                CHECK(!i || functions[i-1].declaration.name_start<f.declaration.name_start);
                CHECK(f.parameter_first<=r.parameter_count && f.parameter_count<=r.parameter_count-f.parameter_first);
            }
            for(size_t i=0;i<r.parameter_count;i++){
                ps_c_parameter p=parameters[i];CHECK(p.start<p.end && p.end<=size && p.declaration.name_end<=size);
                CHECK(p.declaration.type_start<=p.declaration.type_end && p.declaration.type_end<=p.declaration.declarator_start && p.declaration.declarator_start<=p.declaration.declarator_end && p.declaration.declarator_end<=size);
            }
        }
        ps_c_bindings_source_destroy(source);CHECK(!a.invalid && !a.live_bytes && !a.live_blocks);
    }
    printf("C signature prefixes: %zu every-byte inputs, %zu projected / %zu structurally invalid, original spans and clean ownership passed\n",bytes+1,passed,invalid);return 0;
}
static int scope_oracle(void){
    const char *text="int (*global)(int x); void outer(int (*cb)(double a)){int (*local)(int b)=0; {float (*local)(float c)=0;} for(int (*loop)(int d)=0;loop;) loop(1); cb(1);}";
    ps_c_bindings_source *source=NULL;CHECK(ps_c_bindings_source_create(text,strlen(text),(ps_allocator){0},&source)==PS_OK);
    ps_c_signatures_result r=ps_c_bindings_source_signatures(source,functions,64,parameters,256,(ps_allocator){0});CHECK(r.status==PS_OK);
    size_t compared=0;
    for(size_t cursor=0;cursor<=strlen(text);cursor++){
        ps_c_binding locals[64];ps_c_bindings_result local=ps_c_bindings_source_at(source,cursor,locals,64,(ps_allocator){0});CHECK(local.status==PS_OK);
        for(size_t i=0;i<r.function_count;i++){
            ps_c_function f=functions[i];if(!f.pointer && !f.declaration.parameter)continue;
            bool expected=false;for(size_t j=0;j<local.count;j++)if(locals[j].name_start==f.declaration.name_start)expected=true;
            bool visible=cursor>=f.visible_from && cursor>=f.scope_start && (cursor<f.scope_end || (f.scope_open && cursor==f.scope_end));
            if(expected!=visible){fprintf(stderr,"C signature scope mismatch at cursor %zu name %zu expected=%u visible=%u start=%zu end=%zu from=%zu open=%u\n",cursor,f.declaration.name_start,expected,visible,f.scope_start,f.scope_end,f.visible_from,f.scope_open);ps_c_bindings_source_destroy(source);return 1;}
            compared++;
        }
    }
    ps_c_bindings_source_destroy(source);printf("C signature scopes: %zu cursor/name comparisons against existing independent local-binding collection passed\n",compared);return 0;
}
static int written_signatures(void){
    const char *text="int (*a)(int x)=0, (*b)(double y)=0, missing=";ps_c_bindings_source *source=NULL;
    CHECK(ps_c_bindings_source_create(text,strlen(text),(ps_allocator){0},&source)==PS_OK);
    ps_c_signatures_result r=ps_c_bindings_source_signatures(source,functions,64,parameters,256,(ps_allocator){0});CHECK(r.status==PS_OK && r.function_count==2);
    char result[256];CHECK(ps_c_binding_signature(text,strlen(text),functions[1].declaration,result,sizeof result)>0);
    if(strcmp(result,"int (*b)(double y)")){fprintf(stderr,"Recovered comma signature: %s\n",result);ps_c_bindings_source_destroy(source);return 1;}
    ps_c_bindings_source_destroy(source);return 0;
}
static int prototype_contract(void){
    const char *text="int unknown(); int empty(void); int defined(){}";ps_c_bindings_source *source=NULL;
    CHECK(ps_c_bindings_source_create(text,strlen(text),(ps_allocator){0},&source)==PS_OK);
    ps_c_signatures_result r=ps_c_bindings_source_signatures(source,functions,64,parameters,256,(ps_allocator){0});
    bool valid=r.status==PS_OK && r.function_count==3 && functions[0].unspecified && !functions[1].unspecified && !functions[2].unspecified;
    ps_c_bindings_source_destroy(source);
    if(!valid){fprintf(stderr,"C signature unspecified prototype was presented as a known zero-parameter function\n");return 1;}
    return 0;
}
static int invalid_legacy(void){
    const char *text="int f(cb,) int (*cb)(double x); {call(";ps_c_bindings_source *source=NULL;
    CHECK(ps_c_bindings_source_create(text,strlen(text),(ps_allocator){0},&source)==PS_OK);
    ps_c_signatures_result r=ps_c_bindings_source_signatures(source,functions,64,parameters,256,(ps_allocator){0});
    bool valid=r.status==PS_OK && !r.function_count && !r.parameter_count;ps_c_bindings_source_destroy(source);
    if(!valid){fprintf(stderr,"C signature invalid old-style header leaked its callback annotation as a file-scope function\n");return 1;}
    return 0;
}
int main(int argc,char **argv){
    if(argc==2){if(!strcmp(argv[1],"scope"))return scope_oracle();if(!strcmp(argv[1],"written"))return written_signatures();if(!strcmp(argv[1],"prototype"))return prototype_contract();if(!strcmp(argv[1],"legacy"))return invalid_legacy();return 2;}
    CHECK(!vectors());CHECK(!flags_scopes());CHECK(!failures());CHECK(!bounds());CHECK(!prefixes());CHECK(!scope_oracle());CHECK(!written_signatures());CHECK(!prototype_contract());CHECK(!invalid_legacy());return 0;
}
