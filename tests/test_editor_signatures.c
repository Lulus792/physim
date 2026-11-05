#include "language/editor_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Editor signatures line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static ps_lang_node nodes[8192];
static ps_lang_editor_function functions[64];
static ps_lang_editor_parameter parameters[128];
static ps_lang_editor_signatures_result collect(const char *text){
    memset(functions,0,sizeof functions);memset(parameters,0,sizeof parameters);
    return ps_lang_editor_signatures(text,strlen(text),nodes,8192,functions,64,parameters,128);
}
static int span(const char *text,size_t start,size_t end,const char *expected){
    return start<=end && end<=strlen(text) && end-start==strlen(expected) && !memcmp(text+start,expected,end-start);
}
static int token_equal(ps_lang_token a,ps_lang_token b){
    return a.kind==b.kind && a.offset==b.offset && a.length==b.length && a.line==b.line && a.column==b.column && a.file==b.file &&
        ((!a.error && !b.error) || (a.error && b.error && !strcmp(a.error,b.error)));
}
static int node_equal(ps_lang_node a,ps_lang_node b){
    return a.kind==b.kind && token_equal(a.token,b.token) && a.a==b.a && a.b==b.b && a.c==b.c && a.next==b.next &&
        a.type_start==b.type_start && a.type_end==b.type_end;
}
static int declarations(void){
    const char *text="func size(value: [Vec3<Float64>?], map: func(Int64, Float64) -> String) -> Int64:\n    return 1\nfunc empty():\n    return\n";
    ps_lang_editor_signatures_result r=collect(text);
    CHECK(r.status==PS_LANG_EDITOR_OK && r.function_count==2 && r.parameter_count==2);
    CHECK(span(text,functions[0].name_start,functions[0].name_end,"size"));
    CHECK(span(text,functions[0].header_start,functions[0].header_end,"size(value: [Vec3<Float64>?], map: func(Int64, Float64) -> String) -> Int64"));
    CHECK(span(text,functions[0].result_start,functions[0].result_end,"Int64"));
    CHECK(functions[0].parameter_first==0 && functions[0].parameter_count==2 && !functions[0].method && !functions[0].generic);
    CHECK(span(text,parameters[0].name_start,parameters[0].name_end,"value"));
    CHECK(span(text,parameters[0].type_start,parameters[0].type_end,"[Vec3<Float64>?]"));
    CHECK(span(text,parameters[1].name_start,parameters[1].name_end,"map"));
    CHECK(span(text,parameters[1].type_start,parameters[1].type_end,"func(Int64, Float64) -> String"));
    CHECK(functions[1].parameter_first==2 && !functions[1].parameter_count && !functions[1].result_start && !functions[1].result_end);
    for(size_t i=0;i<r.function_count;i++)CHECK(!functions[i].scope_start && !functions[i].scope_depth && !functions[i].visible_from && functions[i].scope_end==strlen(text));
    ps_lang_parse_result strict=ps_lang_parse(text,strlen(text),nodes,8192);CHECK(strict.root && !strict.diagnostic.error);
    text="func Größe<T: Numeric>(\n    wert: (T)?,\n    liste: [T],\n) -> T:\n    return wert\n";
    r=collect(text);CHECK(r.status==PS_LANG_EDITOR_OK && r.function_count==1 && r.parameter_count==2 && functions[0].generic);
    CHECK(span(text,functions[0].name_start,functions[0].name_end,"Größe"));
    CHECK(span(text,parameters[0].type_start,parameters[0].type_end,"(T)?"));
    CHECK(span(text,parameters[1].type_start,parameters[1].type_end,"[T]"));
    return 0;
}
static int owners(void){
    const char *text="struct Particle<T>:\n    var value: T\n    func get() -> T:\n        return self.value\n    mutating func set(value: T):\n        self.value = value\n    static func make(value: T) -> Particle<T>:\n        return Particle<T>(value)\nenum Mode:\n    case idle\n    func label() -> String:\n        return \"idle\"\nfunc free():\n    return\n";
    ps_lang_editor_signatures_result r=collect(text);CHECK(r.status==PS_LANG_EDITOR_OK && r.function_count==5 && r.parameter_count==2);
    for(size_t i=0;i<3;i++){CHECK(functions[i].method==(int)i+1);CHECK(span(text,functions[i].owner_start,functions[i].owner_end,"Particle"));}
    CHECK(!functions[0].parameter_count && functions[1].parameter_count==1 && functions[2].parameter_count==1);
    CHECK(span(text,parameters[0].name_start,parameters[0].name_end,"value"));
    CHECK(functions[3].method==1 && span(text,functions[3].owner_start,functions[3].owner_end,"Mode"));
    CHECK(!functions[4].method && !functions[4].owner_start && !functions[4].owner_end);
    return 0;
}
static int scopes(void){
    const char *text="func outer():\n    func local(x: Int64):\n        return\n    if true:\n        func local(x: String):\n            return\n        local(\"inside\")\n    local(1)\nfunc after():\n    return\n";
    ps_lang_editor_signatures_result r=collect(text);CHECK(r.status==PS_LANG_EDITOR_OK && r.function_count==4 && r.parameter_count==2);
    const char *after=strstr(text,"func after"),*outside=strstr(text,"    local(1)");
    CHECK(!functions[0].scope_depth && !functions[3].scope_depth);
    CHECK(functions[1].scope_depth==1 && functions[1].scope_start==strlen("func outer():") && functions[1].scope_end==(size_t)(after-text));
    CHECK(functions[2].scope_depth==2 && functions[2].scope_end==(size_t)(outside-text));
    CHECK(functions[1].scope_start<functions[2].scope_start && functions[2].scope_start<functions[2].visible_from);
    CHECK(functions[1].visible_from==functions[1].header_end && functions[2].visible_from==functions[2].header_end);
    /* A method owner must not leak into a local free function. */
    text="struct Particle:\n    func step():\n        func inner(x: Int64):\n            return\n        inner(1)\n";
    r=collect(text);CHECK(r.status==PS_LANG_EDITOR_OK && r.function_count==2);
    CHECK(functions[0].method==1 && !functions[1].method && !functions[1].owner_end && functions[1].scope_depth==1);
    return 0;
}
static int prefixes(void){
    static const struct {const char *text;ps_lang_editor_status status;size_t count,parameters;} cases[]={
        {"",PS_LANG_EDITOR_OK,0,0},
        {"func ready(x: Int64):",PS_LANG_EDITOR_PREFIX,1,1},
        {"func ready(x: Int64):\n",PS_LANG_EDITOR_PREFIX,1,1},
        {"func ready(x: Int64):\n    call(",PS_LANG_EDITOR_PREFIX,1,1},
        {"func ready(x: Int64):\n    return\nfunc next(",PS_LANG_EDITOR_PREFIX,1,1},
        {"func incomplete(x:",PS_LANG_EDITOR_PREFIX,0,0},
        {"func incomplete(x: Int64)",PS_LANG_EDITOR_PREFIX,0,0},
        {"func incomplete(x: Int64) ->",PS_LANG_EDITOR_PREFIX,0,0},
        {"func broken(x:):\n    return",PS_LANG_EDITOR_INVALID,0,0},
        {"func ready():\n    return\nfunc broken(x:):\n    return",PS_LANG_EDITOR_INVALID,0,0},
        {"func ready():\nreturn\n",PS_LANG_EDITOR_INVALID,0,0},
        {"let text = \"func fake(x: Int64):\"\n// func comment():\nfunc actual():\n    return\n",PS_LANG_EDITOR_OK,1,0},
        {"func first():\r\n    func second(x: Int64):\r\n        return\r\nfunc last():\r\n    return\r\n",PS_LANG_EDITOR_OK,3,1},
    };
    for(size_t i=0;i<sizeof cases/sizeof *cases;i++){
        ps_lang_editor_signatures_result r=collect(cases[i].text);
        if(r.status!=cases[i].status || r.function_count!=cases[i].count || r.parameter_count!=cases[i].parameters){
            fprintf(stderr,"Signature prefix %zu status=%d count=%zu parameters=%zu diagnostic=%s\n",i,r.status,r.function_count,r.parameter_count,r.diagnostic.error?r.diagnostic.error:"");return 1;
        }
    }
    return 0;
}
static int limits(void){
    const char text[]="func one(x: Int64, y: String):\n    return\nfunc two(z: Float64):\n    return\n";
    ps_lang_editor_signatures_result r=ps_lang_editor_signatures(text,sizeof text-1,nodes,8192,NULL,0,NULL,0);
    CHECK(r.status==PS_LANG_EDITOR_OK && r.function_count==2 && r.parameter_count==3);
    struct {ps_lang_editor_function function;unsigned char guard[16];} f;
    struct {ps_lang_editor_parameter parameter;unsigned char guard[16];} p;
    memset(&f,0xa5,sizeof f);memset(&p,0x5a,sizeof p);
    r=ps_lang_editor_signatures(text,sizeof text-1,nodes,8192,&f.function,1,&p.parameter,1);
    CHECK(r.status==PS_LANG_EDITOR_LIMIT && r.function_count==2 && r.parameter_count==3);
    for(size_t i=0;i<16;i++)CHECK(f.guard[i]==0xa5 && p.guard[i]==0x5a);
    r=ps_lang_editor_signatures(text,sizeof text-1,nodes,8192,functions,64,NULL,0);
    CHECK(r.status==PS_LANG_EDITOR_LIMIT && r.parameter_count==3);
    ps_lang_parse_result strict=ps_lang_parse(text,sizeof text-1,nodes,8192);CHECK(strict.root);
    for(size_t capacity=2;capacity<strict.count;capacity++){
        r=ps_lang_editor_signatures(text,sizeof text-1,nodes,capacity,functions,64,parameters,128);
        CHECK(r.status==PS_LANG_EDITOR_LIMIT && !r.function_count && !r.parameter_count);
    }
    r=ps_lang_editor_signatures(text,sizeof text-1,nodes,strict.count,functions,64,parameters,128);CHECK(r.status==PS_LANG_EDITOR_OK);
    r=ps_lang_editor_signatures(NULL,1,nodes,8192,NULL,0,NULL,0);CHECK(r.status==PS_LANG_EDITOR_INVALID);
    r=ps_lang_editor_signatures(NULL,0,nodes,8192,NULL,0,NULL,0);CHECK(r.status==PS_LANG_EDITOR_OK);
    r=ps_lang_editor_signatures(text,sizeof text-1,NULL,8192,NULL,0,NULL,0);CHECK(r.status==PS_LANG_EDITOR_INVALID);
    r=ps_lang_editor_signatures(text,sizeof text-1,nodes,1,NULL,0,NULL,0);CHECK(r.status==PS_LANG_EDITOR_INVALID);
    r=ps_lang_editor_signatures(text,sizeof text-1,nodes,8192,NULL,1,NULL,0);CHECK(r.status==PS_LANG_EDITOR_INVALID);
    const char nul[]="func a():\n    return\0x";r=ps_lang_editor_signatures(nul,sizeof nul-1,nodes,8192,NULL,0,NULL,0);CHECK(r.status==PS_LANG_EDITOR_INVALID);
    r=ps_lang_editor_signatures((const void *)1,256u*1024u+1,nodes,8192,NULL,0,NULL,0);CHECK(r.status==PS_LANG_EDITOR_LIMIT);
    char nested[1024];size_t at=(size_t)sprintf(nested,"func deep(x: ");for(size_t i=0;i<140;i++)nested[at++]='[';
    memcpy(nested+at,"Int64",5);at+=5;for(size_t i=0;i<140;i++)nested[at++]=']';strcpy(nested+at,"):\n    return");
    r=collect(nested);CHECK(r.status==PS_LANG_EDITOR_LIMIT && !r.function_count && !r.parameter_count);
    return 0;
}
static int ast_equivalence(void){
    /* The observer must preserve the actual strict AST, including incomplete
     * bodies. Check every byte boundary, not only hand-picked complete headers. */
    const char *text="struct Particle<T>:\n    var value: T\n    func get() -> T:\n        return self.value\nfunc outer<T>(x: [T]?, fn: func(T) -> T):\n    func local(y: T) -> T:\n        return fn(y)\n    if true:\n        local<T>(x[0])\nfunc last():\n    outer<Int64>([1], func(v: Int64) -> Int64:\n        return v\n    )\n";
    static ps_lang_node expected[8192];size_t total=strlen(text)+1,padding_differences=0;
    for(size_t size=0;size<total;size++){
        memset(expected,0,sizeof expected);memset(nodes,0,sizeof nodes);
        ps_lang_parse_result strict=ps_lang_parse(text,size,expected,8192);
        ps_lang_editor_signatures_result r=ps_lang_editor_signatures(text,size,nodes,8192,functions,64,parameters,128);
        for(size_t n=0;n<strict.count;n++){
            if(!node_equal(expected[n],nodes[n])){fprintf(stderr,"Signature strict AST differs at source byte %zu node %zu\n",size,n);return 1;}
            if(memcmp(&expected[n],&nodes[n],sizeof nodes[n]))padding_differences++;
        }
        CHECK(strict.root?r.status==PS_LANG_EDITOR_OK:r.status==(strict.diagnostic.offset==size?PS_LANG_EDITOR_PREFIX:PS_LANG_EDITOR_INVALID));
        if(r.status==PS_LANG_EDITOR_INVALID)CHECK(!r.function_count && !r.parameter_count);
        else for(size_t i=0;i<r.function_count;i++){
            ps_lang_editor_function f=functions[i];CHECK(f.name_start<f.name_end && f.name_end<=f.header_end && f.header_end<size);
            CHECK(f.scope_start<=f.name_start && f.name_end<=f.scope_end && f.scope_end<=size);
            CHECK(f.parameter_first<=r.parameter_count && f.parameter_count<=r.parameter_count-f.parameter_first);
        }
    }
    printf("Physim signatures: %zu strict-AST/prefix byte comparisons passed; %zu padding-only node representation differences\n",total,padding_differences);return 0;
}
static int function_types(void){
    const char *text="(func(Int64, func([Float64]?, Module.Item) -> String) -> (func() -> Bool))";
    ps_lang_editor_function_type_result r=ps_lang_editor_function_type(text,strlen(text),nodes,8192,NULL,0);
    CHECK(r.status==PS_LANG_EDITOR_OK && r.callable && r.parameter_count==2);
    CHECK(span(text,r.result_start,r.result_end,"(func() -> Bool)"));
    memset(parameters,0xa5,sizeof parameters);
    ps_lang_editor_parameter held[128];memcpy(held,parameters,sizeof held);
    r=ps_lang_editor_function_type(text,strlen(text),nodes,8192,parameters,1);
    CHECK(r.status==PS_LANG_EDITOR_LIMIT && r.parameter_count==2 && !memcmp(held,parameters,sizeof held));
    r=ps_lang_editor_function_type(text,strlen(text),nodes,8192,parameters,128);
    CHECK(r.status==PS_LANG_EDITOR_OK && r.callable && r.parameter_count==2);
    CHECK(!parameters[0].name_start && !parameters[0].name_end);
    CHECK(span(text,parameters[0].type_start,parameters[0].type_end,"Int64"));
    CHECK(span(text,parameters[1].type_start,parameters[1].type_end,"func([Float64]?, Module.Item) -> String"));
    static const char *noncallable[]={"Int64","Callback","[func(Int64) -> Bool]","(func(Int64) -> Bool)?"};
    for(size_t i=0;i<sizeof noncallable/sizeof *noncallable;i++){
        r=ps_lang_editor_function_type(noncallable[i],strlen(noncallable[i]),nodes,8192,parameters,128);
        CHECK(r.status==PS_LANG_EDITOR_OK && !r.callable && !r.parameter_count && !r.result_end);
    }
    static const char *invalid[]={"","func(","func(Int64)","func(Int64) ->","func(Int64,) -> Bool","func(x: Int64) -> Bool","func() -> Bool extra","func() -> \xc0\xaf"};
    for(size_t i=0;i<sizeof invalid/sizeof *invalid;i++){
        memcpy(parameters,held,sizeof held);
        r=ps_lang_editor_function_type(invalid[i],strlen(invalid[i]),nodes,8192,parameters,128);
        CHECK(r.status==PS_LANG_EDITOR_INVALID && !r.callable && !r.parameter_count && !r.result_end && !memcmp(held,parameters,sizeof held));
    }
    r=ps_lang_editor_function_type("func() -> Void",14,nodes,8192,parameters,128);
    CHECK(r.status==PS_LANG_EDITOR_OK && r.callable && !r.parameter_count);
    r=ps_lang_editor_function_type(text,strlen(text),nodes,2,parameters,128);
    CHECK(r.status==PS_LANG_EDITOR_LIMIT && !r.callable && !r.parameter_count);
    r=ps_lang_editor_function_type("func()\0 -> Void",15,nodes,8192,parameters,128);
    CHECK(r.status==PS_LANG_EDITOR_INVALID && !r.callable);
    r=ps_lang_editor_function_type(NULL,1,nodes,8192,parameters,128);CHECK(r.status==PS_LANG_EDITOR_INVALID);
    r=ps_lang_editor_function_type(text,strlen(text),NULL,8192,parameters,128);CHECK(r.status==PS_LANG_EDITOR_INVALID);
    r=ps_lang_editor_function_type(text,strlen(text),nodes,8192,NULL,1);CHECK(r.status==PS_LANG_EDITOR_INVALID);
    r=ps_lang_editor_function_type(text,256u*1024u+1,nodes,8192,parameters,128);CHECK(r.status==PS_LANG_EDITOR_LIMIT);
    char deep[512];memset(deep,'(',140);memcpy(deep+140,"Int64",5);memset(deep+145,')',140);
    r=ps_lang_editor_function_type(deep,285,nodes,8192,parameters,128);CHECK(r.status==PS_LANG_EDITOR_LIMIT && !r.callable);
    puts("Physim function types: real grammar, grouped/nested/optional/array distinction, exact unnamed spans, invalid input and atomic output/node/depth boundaries passed");return 0;
}
int main(void){CHECK(!declarations());CHECK(!owners());CHECK(!scopes());CHECK(!prefixes());CHECK(!limits());CHECK(!ast_equivalence());CHECK(!function_types());
    puts("Physim signature declarations: exact names/types/counts, generic/method owners, lexical/dedent scopes, complete EOF headers, invalid input and all node/output capacity boundaries passed");return 0;}
