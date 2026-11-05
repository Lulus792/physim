#include "signature_help.h"
#include "c_signatures.h"
#include "language/builtins.h"
#include "test_allocator.h"
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Signature help line %d: %s\n",__LINE__,#x);return 1;}}while(0)
typedef struct {const char *source,*signature;bool language;ps_signature_state state;size_t count,argument,active;} example;
static const example cases[]={
 {"void step(){ps_vadd(|);}","ps_vec3 ps_vadd(ps_vec3 a, ps_vec3 b)",false,PS_SIGNATURE_READY,2,0,0},
 {"void step(){ps_vadd(a, |);}","ps_vec3 ps_vadd(ps_vec3 a, ps_vec3 b)",false,PS_SIGNATURE_READY,2,1,1},
 {"void step(){ps_vadd(a,b,|);}","ps_vec3 ps_vadd(ps_vec3 a, ps_vec3 b)",false,PS_SIGNATURE_READY,2,2,SIZE_MAX},
 {"int custom(int a,const char *text);void f(){custom(|);}","int custom(int a, const char *text)",false,PS_SIGNATURE_READY,2,0,0},
 {"int custom(int a,const char *text);void f(){custom(1,|);}","int custom(int a, const char *text)",false,PS_SIGNATURE_READY,2,1,1},
 {"void f(){int ps_vadd=ps_vadd(|","",false,PS_SIGNATURE_MASKED,0,0,SIZE_MAX},
 {"int ps_vadd(int x);void f(){ps_vadd(|);}","int ps_vadd(int x)",false,PS_SIGNATURE_READY,1,0,0},
 {"int (*cb)(float x)=cb(|","int (*cb)(float x)",false,PS_SIGNATURE_READY,1,0,0},
 {"int (*factory(int own))(double returned);void f(){factory(|);}","int (*factory(int own))(double returned)",false,PS_SIGNATURE_READY,1,0,0},
 {"int old(a,b) int a; double b; {return a;} void f(){old(|);}","int old(int a, double b)",false,PS_SIGNATURE_READY,2,0,0},
 {"int old(a,b) int a; {return a;} void f(){old(|);}","int old(int a, b)",false,PS_SIGNATURE_READY,2,0,0},
 {"int unnamed(void (*)(int),int);void f(){unnamed(|);}","int unnamed(void (*)(int), int)",false,PS_SIGNATURE_READY,2,0,0},
 {"void empty(void);void f(){empty(|);}","void empty(void)",false,PS_SIGNATURE_READY,0,0,SIZE_MAX},
 {"void unknown();void f(){unknown(|);}","void unknown()",false,PS_SIGNATURE_READY,0,0,SIZE_MAX},
 {"int variadic(const char *format,...);void f(){variadic(1,|);}","int variadic(const char *format, ...)",false,PS_SIGNATURE_READY,1,1,SIZE_MAX},
 {"int outer(int a,int b);float inner(int x);void f(){outer(inner(|),2);}","float inner(int x)",false,PS_SIGNATURE_READY,1,0,0},
 {"int outer(int a,int b);float inner(int x);void f(){outer(inner(1),|);}","int outer(int a, int b)",false,PS_SIGNATURE_READY,2,1,1},
 {"int api(double x);void f(){api(sizeof(enum {api=1})+|);}","int api(double x)",false,PS_SIGNATURE_READY,1,0,0},
 {"void f(){absent(|);}","",false,PS_SIGNATURE_UNKNOWN,0,0,SIZE_MAX},
 {"void f(){object.ps_vadd(|);}","",false,PS_SIGNATURE_UNKNOWN,0,0,SIZE_MAX},
 {"func step():\n    sin(|)\n","sin(angle: Float64) -> Float64",true,PS_SIGNATURE_READY,1,0,0},
 {"func step():\n    Vec3(1, |)\n","Vec3(x: Float64, y: Float64, z: Float64) -> Vec3",true,PS_SIGNATURE_READY,3,1,1},
 {"func step():\n    let v: Vec3 = Vec3(1,2,3)\n    v.cross(|)\n","Vec3.cross(right: Vec3) -> Vec3",true,PS_SIGNATURE_READY,1,0,0},
 {"func step():\n    Mat3.identity(|)\n","Mat3.identity() -> Mat3",true,PS_SIGNATURE_READY,0,0,SIZE_MAX},
 {"func step():\n    let v = Vec3(1,2,3)\n    v.cross(|)\n","",true,PS_SIGNATURE_UNKNOWN,0,0,SIZE_MAX},
 {"func step():\n    let sin: Int64 = 1\n    sin(|)\n","",true,PS_SIGNATURE_MASKED,0,0,SIZE_MAX},
 {"func step():\n    later(|)\nfunc later(value: Float64) -> Int64:\n    return 1\n","later(value: Float64) -> Int64",true,PS_SIGNATURE_READY,1,0,0},
 {"func outer():\n    func local(value: Int64):\n        return\n    local(|)\n","local(value: Int64)",true,PS_SIGNATURE_READY,1,0,0},
 {"func local(x: Float64):\n    return\nfunc outer():\n    func local(value: Int64):\n        return\n    local(|)\n","local(value: Int64)",true,PS_SIGNATURE_READY,1,0,0},
 {"func local(x: Float64):\n    return\nfunc outer():\n    if true:\n        func local(value: Int64):\n            return\n    local(|)\n","local(x: Float64)",true,PS_SIGNATURE_READY,1,0,0},
 {"struct Particle:\n    func step():\n        self.next(|)\n    func next(x: Int64):\n        return\n","next(x: Int64)",true,PS_SIGNATURE_READY,1,0,0},
 {"struct Particle:\n    static func make(x: Float64) -> Particle:\n        return Particle()\nfunc step():\n    Particle.make(|)\n","make(x: Float64) -> Particle",true,PS_SIGNATURE_READY,1,0,0},
 {"func Größe(wert: Float64):\n    return\nfunc step():\n    Größe(|)\n","Größe(wert: Float64)",true,PS_SIGNATURE_READY,1,0,0},
 {"func generic<T>(values: [T]?, map: func(Int64, Float64) -> String) -> T:\n    return values[0]\nfunc step():\n    generic(|)\n","generic<T>(values: [T]?, map: func(Int64, Float64) -> String) -> T",true,PS_SIGNATURE_READY,2,0,0},
 {"func step():\n    String(repeating: |)\n","",true,PS_SIGNATURE_UNKNOWN,0,0,SIZE_MAX},
 {"func step():\n    Array(repeating: |)\n","",true,PS_SIGNATURE_UNKNOWN,0,0,SIZE_MAX},
 {"func step():\n    absent(|)\n","",true,PS_SIGNATURE_UNKNOWN,0,0,SIZE_MAX},
 {"func same(x: Int64):\n    return\nfunc same(x: Float64):\n    return\nfunc step():\n    same(|)\n","",true,PS_SIGNATURE_AMBIGUOUS,0,0,SIZE_MAX},
 {"func step(cb: func(Int64, Float64) -> String):\n    cb(1, |)\n","cb(Int64, Float64) -> String",true,PS_SIGNATURE_READY,2,1,1},
 {"func step(cb: func() -> Void):\n    cb(|)\n","cb() -> Void",true,PS_SIGNATURE_READY,0,0,SIZE_MAX},
 {"func step(cb: (func((Int64), [Float64]?) -> (String))):\n    cb(|)\n","cb((Int64), [Float64]?) -> (String)",true,PS_SIGNATURE_READY,2,0,0},
 {"func step(cb: func(func(Int64) -> String, [Vec3?]) -> Bool):\n    cb(1, |)\n","cb(func(Int64) -> String, [Vec3?]) -> Bool",true,PS_SIGNATURE_READY,2,1,1},
 {"func step(sin: func(Int64) -> Bool):\n    sin(|)\n","sin(Int64) -> Bool",true,PS_SIGNATURE_READY,1,0,0},
 {"func step(cb: func(Int64) -> Bool):\n    cb(1, |)\n","cb(Int64) -> Bool",true,PS_SIGNATURE_READY,1,1,SIZE_MAX},
 {"func step(cb: (func(Int64) -> Bool)?):\n    cb(|)\n","",true,PS_SIGNATURE_MASKED,0,0,SIZE_MAX},
 {"func step(cb: [func(Int64) -> Bool]):\n    cb(|)\n","",true,PS_SIGNATURE_MASKED,0,0,SIZE_MAX},
 {"func step(cb: Callback):\n    cb(|)\n","",true,PS_SIGNATURE_MASKED,0,0,SIZE_MAX},
 {"func step(cb: func(Int64) -> Bool):\n    let cb: Int64 = 1\n    cb(|)\n","",true,PS_SIGNATURE_MASKED,0,0,SIZE_MAX},
 {"func convert(value: Int64) -> String:\n    return \"\"\nfunc step():\n    let cb: func(Int64) -> String = convert\n    cb(|)\n","cb(Int64) -> String",true,PS_SIGNATURE_READY,1,0,0},
 {"func convert(value: Int64) -> String:\n    return \"\"\nfunc step():\n    let cb = convert\n    cb(|)\n","",true,PS_SIGNATURE_MASKED,0,0,SIZE_MAX},
};
static int fixture(ps_completion_locals *l,const void *owner,const char *marked,bool language){
    const char *point=strchr(marked,'|');if(!point)return 0;size_t size=strlen(marked),cursor=(size_t)(point-marked);
    char *source=malloc(size);if(!source)return 0;
    memcpy(source,marked,cursor);memcpy(source+cursor,point+1,size-cursor);
    ps_result r=ps_completion_locals_update(l,owner,source,size-1,cursor,language);free(source);return r==PS_OK;
}
static int vectors(void){
    for(size_t k=0;k<sizeof cases/sizeof *cases;k++){
        const example *v=&cases[k];test_allocator a={0};ps_completion_locals l={0};ps_signature_help h={.allocator=test_domain(&a)};
        CHECK(fixture(&l,&a,v->source,v->language));ps_result r=ps_signature_help_update(&h,&l);
        bool valid=r==PS_OK && h.state==v->state && h.active_parameter==v->active;
        if(h.display)valid=valid && !strcmp(h.display->text,v->signature) && h.display->count==v->count && h.call.argument==v->argument;
        else valid=valid && !v->signature[0];
        if(!valid){
            fprintf(stderr,"Signature vector %zu language=%d result=%u state=%u call=%d arg=%zu active=%zu\ntext=%s expected=%s state=%u\n",k,v->language,(unsigned)r,(unsigned)h.state,h.call.active,h.call.argument,h.active_parameter,h.display?h.display->text:"<none>",v->signature,(unsigned)v->state);
            ps_signature_help_destroy(&h);ps_completion_locals_destroy(&l);CHECK(!a.live_bytes && !a.invalid);return 1;
        }
        CHECK(ps_signature_help_matches(&h,&l));size_t attempts=a.attempts;CHECK(ps_signature_help_update(&h,&l)==PS_OK && a.attempts==attempts);
        if(h.display){
            ps_signature_display *d=h.display;CHECK(d->name_begin<d->name_end && d->name_end<=d->text_bytes);
            for(size_t i=0;i<d->count;i++){ps_signature_parameter p=d->parameters[i];CHECK(p.begin<p.end && p.end<=d->text_bytes && p.name_begin<=p.name_end && p.name_end<=d->text_bytes);}
            if(k==10)CHECK(d->parameters[1].type_missing);
            if(k==11)CHECK(!d->parameters[0].name_end && !d->parameters[1].name_end);
            if(k==13)CHECK(d->unspecified);
            if(k==14)CHECK(d->variadic_end>d->variadic_begin);
        }
        ps_completion_locals_destroy(&l); /* Every visible string/source remains owned. */
        if(h.display)CHECK(!strcmp(h.display->text,v->signature));
        ps_signature_help_destroy(&h);CHECK(!a.live_bytes && !a.live_blocks && !a.invalid);
    }
    printf("Signature model: %zu C/Physim API/user/header/scope/ownership vectors passed\n",sizeof cases/sizeof *cases);return 0;
}
static int transition(void){
    const char *text="func step():\n    Vec3(1, 2, 3)\n";const char *first=strstr(text,"1,");const char *second=strstr(text,"2,");int owner=0,other=0;
    ps_completion_locals l={0};test_allocator a={0};ps_signature_help h={.allocator=test_domain(&a)};
    CHECK(ps_completion_locals_update(&l,&owner,text,strlen(text),(size_t)(first-text),true)==PS_OK);
    CHECK(ps_signature_help_update(&h,&l)==PS_OK && h.state==PS_SIGNATURE_READY && h.active_parameter==0);
    const char *source=h.source;const void *functions=h.functions;const void *display=h.display;
    CHECK(ps_completion_locals_update(&l,&owner,text,strlen(text),(size_t)(second-text),true)==PS_OK);
    size_t attempts=a.attempts;
    CHECK(!ps_signature_help_matches(&h,&l));CHECK(ps_signature_help_update(&h,&l)==PS_OK && h.active_parameter==1);
    if(a.attempts!=attempts){
        fprintf(stderr,"Signature same-call cache: cursor-only argument change allocated %zu times\n",a.attempts-attempts);
        ps_completion_locals_destroy(&l);ps_signature_help_destroy(&h);CHECK(!a.live_bytes && !a.invalid);return 1;
    }
    CHECK(h.source==source && h.functions==functions && h.display==display);
    CHECK(ps_completion_locals_update(&l,&other,text,strlen(text),(size_t)(second-text),true)==PS_OK);
    CHECK(!ps_signature_help_matches(&h,&l));CHECK(ps_signature_help_update(&h,&l)==PS_OK && h.source!=source);
    CHECK(ps_completion_locals_update(&l,&other,text,strlen(text),0,true)==PS_OK);
    CHECK(ps_signature_help_update(&h,&l)==PS_OK && h.state==PS_SIGNATURE_NONE && !h.display && h.active_parameter==SIZE_MAX);
    ps_completion_locals_destroy(&l);ps_signature_help_destroy(&h);CHECK(!a.live_bytes && !a.invalid);
    puts("Signature model: exact revision/owner/cursor, module/display reuse and call exit passed");return 0;
}
static int faults(bool language,bool function_value){
    const char *old=language?"func old():\n    return\nfunc step():\n    old(|)\n":"int old(void);void f(){old(|);}";
    const char *fresh=language?"func new(value: Float64):\n    return\nfunc step():\n    new(|)\n":"void f(){ps_v3(|);}";
    if(function_value)fresh="func step(cb: func(Int64, Float64) -> String):\n    cb(|)\n";
    ps_completion_locals baseline={0},target={0};int owner=0;CHECK(fixture(&baseline,&owner,old,language));CHECK(fixture(&target,&owner,fresh,language));
    test_allocator a={0};ps_signature_help h={.allocator=test_domain(&a)};
    CHECK(ps_signature_help_update(&h,&baseline)==PS_OK && h.state==PS_SIGNATURE_READY);
    size_t start=a.attempts;CHECK(ps_signature_help_update(&h,&target)==PS_OK && h.state==PS_SIGNATURE_READY);size_t count=a.attempts-start;
    ps_signature_help_destroy(&h);CHECK(count>0 && !a.live_bytes && !a.invalid);
    for(size_t fail=1;fail<=count;fail++){
        h.allocator=test_domain(&a);CHECK(ps_signature_help_update(&h,&baseline)==PS_OK);
        size_t held=a.live_bytes;const void *source=h.source,*display=h.display,*functions=h.functions;
        a.fail_on=a.attempts+fail;ps_result r=ps_signature_help_update(&h,&target);
        if(r!=PS_MEMORY){fprintf(stderr,"Signature fault language=%d fail=%zu/%zu status=%u\n",language,fail,count,(unsigned)r);return 1;}
        CHECK(h.source==source && h.display==display && h.functions==functions && a.live_bytes==held && !a.invalid);
        CHECK(!ps_signature_help_matches(&h,&target));a.fail_on=0;
        CHECK(ps_signature_help_update(&h,&target)==PS_OK && h.state==PS_SIGNATURE_READY);
        ps_signature_help_destroy(&h);CHECK(!a.live_bytes && !a.live_blocks && !a.invalid);
    }
    if(!language)CHECK(ps_c_bindings_source_lookup(target.c_source,target.cursor,"ps_v3",5,NULL,0,(ps_allocator){0}).status==PS_OK);
    ps_completion_locals_destroy(&target);ps_completion_locals_destroy(&baseline);
    printf("Signature model: %s %zu/%zu allocation failures preserve old snapshot, clear stale matching and retry\n",function_value?"function value":language?"Physim":"C",count,count);return 0;
}
static ps_lang_type checked_owner(const char *name){
    char text[192];const char *annotation=!strcmp(name,"array")?"[Float64]":!strcmp(name,"text")?"String":name;
    snprintf(text,sizeof text,"func owner(value: %s):\n    return\n",annotation);
    ps_lang_node nodes[64];ps_lang_semantic info[64];
    ps_lang_parse_result parsed=ps_lang_parse(text,strlen(text),nodes,64);
    if(!parsed.root)return PS_TYPE_NONE;
    ps_lang_check_result checked=ps_lang_check(text,strlen(text),nodes,parsed,info,64,64);
    if(!checked.ok)return PS_TYPE_NONE;
    for(size_t i=1;i<checked.count;i++)if(nodes[i].kind==PS_AST_PARAMETER)return info[nodes[i].a].type;
    return PS_TYPE_NONE;
}
static int catalog(void){
    size_t c=0,ph=0,special=0;int owner=0;
    for(size_t i=0;i<ps_completion_count();i++){
        const ps_completion_symbol *s=ps_completion_at(i);if(s->kind!=PS_COMPLETION_FUNCTION)continue;
        char marked[512];bool formal=!s->language;const ps_lang_builtin *builtin=NULL;size_t binding=0;
        const char *dot=strchr(s->name,'.');
        if(!s->language)snprintf(marked,sizeof marked,"void step(){%s(|);}",s->name);
        else if(s->instance && dot){
            char type[80];size_t n=(size_t)(dot-s->name);CHECK(n<sizeof type);memcpy(type,s->name,n);type[n]=0;
            const char *annotation=!strcmp(type,"array")?"[Float64]":!strcmp(type,"text")?"String":type;
            snprintf(marked,sizeof marked,"func step(value: %s):\n    value.%s(|)\n",annotation,dot+1);
            const ps_lang_method *method=ps_lang_method_find(checked_owner(type),dot+1,strlen(dot+1),&binding);
            if(method)builtin=ps_lang_builtin_get(binding);
        }else{
            snprintf(marked,sizeof marked,"func step():\n    %s(|)\n",s->name);
            builtin=dot?ps_lang_static_method_find(s->name,(size_t)(dot-s->name),dot+1,strlen(dot+1),&binding):ps_lang_builtin_find(s->name,strlen(s->name),&binding);
        }
        formal=formal || builtin!=NULL;
        ps_completion_locals l={0};ps_signature_help h={0};CHECK(fixture(&l,&owner,marked,s->language));
        ps_result result=ps_signature_help_update(&h,&l);
        if(result!=PS_OK || (formal && (h.state!=PS_SIGNATURE_READY || !h.display || h.api_symbol!=s)) || (!formal && h.state==PS_SIGNATURE_READY)){
            fprintf(stderr,"Signature catalog symbol=%s language=%d formal=%d result=%u state=%u\n",s->name,s->language,formal,(unsigned)result,(unsigned)h.state);
            ps_signature_help_destroy(&h);ps_completion_locals_destroy(&l);return 1;
        }
        if(!s->language)c++;else if(formal)ph++;else special++;
        ps_signature_help_destroy(&h);ps_completion_locals_destroy(&l);
    }
    printf("Signature catalog: %zu C and %zu compiler-registered Physim formal calls resolved; %zu special/property/documentation entries explicitly unresolved\n",c,ph,special);return 0;
}
static int borrowed_faults(void){
    const char text[]="func step():\n    Vec3(1,2,3)\n    sin(1)\n";size_t first=(size_t)(strstr(text,"1,")-text),second=(size_t)(strstr(text,"sin(1")-text)+4;int owner=0;
    ps_completion_locals baseline={0},target={0};CHECK(ps_completion_locals_update(&baseline,&owner,text,strlen(text),first,true)==PS_OK);
    CHECK(ps_completion_locals_update(&target,&owner,text,strlen(text),second,true)==PS_OK);
    test_allocator a={0};ps_signature_help h={.allocator=test_domain(&a)};CHECK(ps_signature_help_update(&h,&baseline)==PS_OK);
    size_t start=a.attempts;CHECK(ps_signature_help_update(&h,&target)==PS_OK && h.state==PS_SIGNATURE_READY);size_t count=a.attempts-start;
    ps_signature_help_destroy(&h);
    for(size_t i=1;i<=count;i++){
        h.allocator=test_domain(&a);CHECK(ps_signature_help_update(&h,&baseline)==PS_OK);
        size_t held=a.live_bytes;const void *source=h.source,*functions=h.functions,*display=h.display;
        a.fail_on=a.attempts+i;CHECK(ps_signature_help_update(&h,&target)==PS_MEMORY);
        CHECK(h.source==source && h.functions==functions && h.display==display && a.live_bytes==held && !a.invalid);
        a.fail_on=0;CHECK(ps_signature_help_update(&h,&target)==PS_OK && h.state==PS_SIGNATURE_READY);
        ps_signature_help_destroy(&h);CHECK(!a.live_bytes && !a.live_blocks && !a.invalid);
    }
    ps_completion_locals_destroy(&baseline);ps_completion_locals_destroy(&target);
    printf("Signature model: %zu/%zu borrowed source/module failure paths preserve old ownership and retry\n",count,count);return 0;
}
static int complete_counts(void){
    for(unsigned language=0;language<2;language++){
        char source[2048],*at=source;size_t left=sizeof source;
        int n=snprintf(at,left,language?"func many(":"int many(");at+=n;left-=(size_t)n;
        for(unsigned i=0;i<16;i++){n=snprintf(at,left,language?"%sp%u: Int64":"%sint p%u",i?", ":"",i);at+=n;left-=(size_t)n;}
        n=snprintf(at,left,language?"):\n    return\nfunc step():\n    many(":");void step(){many(");at+=n;left-=(size_t)n;
        for(unsigned i=0;i<15;i++){n=snprintf(at,left,"0,");at+=n;left-=(size_t)n;}
        snprintf(at,left,language?"|)\n":"|);}");
        ps_completion_locals l={0};ps_signature_help h={0};CHECK(fixture(&l,&h,source,language!=0));
        CHECK(ps_signature_help_update(&h,&l)==PS_OK && h.state==PS_SIGNATURE_READY && h.display->count==16 && h.active_parameter==15);
        ps_signature_parameter last=h.display->parameters[15];CHECK(last.name_end-last.name_begin==3 && !memcmp(h.display->text+last.name_begin,"p15",3));
        ps_completion_locals_destroy(&l);ps_signature_help_destroy(&h);
    }
    puts("Signature model: all16 user parameters survive for both languages; no five-entry cap");return 0;
}
static int nested_transition(void){
    const char *texts[]={"void f(){ps_v3(({int ps_v3=0; ps_v3(1); 0;}),0,0);}","func step():\n    Vec3(sin(0),2,3)\n"};int owner=0;
    for(unsigned language=0;language<2;language++){
        const char *text=texts[language];size_t positions[3];
        positions[0]=(size_t)(strstr(text,language?"sin(0)":"({")-text);
        positions[1]=(size_t)(strstr(text,language?"sin(0)":"ps_v3(1)")-text)+(language?4:6);
        positions[2]=(size_t)(strstr(text,language?"sin(0),":"}),0")-text)+(language?7:3);
        test_allocator a={0};ps_signature_help h={.allocator=test_domain(&a)};ps_completion_locals l={0};
        for(unsigned i=0;i<3;i++){
            CHECK(ps_completion_locals_update(&l,&owner,text,strlen(text),positions[i],language!=0)==PS_OK);
            CHECK(ps_signature_help_update(&h,&l)==PS_OK);
            if(!language && i==1)CHECK(h.state==PS_SIGNATURE_MASKED && !h.display);
            else CHECK(h.state==PS_SIGNATURE_READY && h.display && h.display->count==(language && i==1?1:3) && h.active_parameter==(i==2?1:0));
        }
        ps_completion_locals_destroy(&l);ps_signature_help_destroy(&h);CHECK(!a.live_bytes && !a.live_blocks && !a.invalid);
    }
    puts("Signature model: inner target/scope changes invalidate cache; C mask and Physim inner-to-outer resume passed");return 0;
}
static int invalid_transition(void){
    const char text[]="func step():\n    Vec3(1,,2)\n";int owner=0;size_t valid=(size_t)(strstr(text,"1,,")-text)+1,invalid=valid+2;
    ps_completion_locals l={0};test_allocator a={0};ps_signature_help h={.allocator=test_domain(&a)};
    CHECK(ps_completion_locals_update(&l,&owner,text,strlen(text),valid,true)==PS_OK);
    CHECK(ps_signature_help_update(&h,&l)==PS_OK && h.state==PS_SIGNATURE_READY);
    CHECK(ps_completion_locals_update(&l,&owner,text,strlen(text),invalid,true)==PS_OK);
    CHECK(l.status==PS_LANG_EDITOR_INVALID);
    CHECK(ps_signature_help_update(&h,&l)==PS_OK && h.state==PS_SIGNATURE_INVALID && !h.display);
    CHECK(ps_completion_locals_update(&l,&owner,text,strlen(text),valid,true)==PS_OK);
    CHECK(ps_signature_help_update(&h,&l)==PS_OK && h.state==PS_SIGNATURE_READY && h.active_parameter==0);
    ps_completion_locals_destroy(&l);ps_signature_help_destroy(&h);CHECK(!a.live_bytes && !a.live_blocks && !a.invalid);
    puts("Signature model: invalid prefix hides display; returning to valid point restores the call");return 0;
}
int main(void){CHECK(!vectors());CHECK(!transition());CHECK(!faults(false,false));CHECK(!faults(true,false));CHECK(!faults(true,true));CHECK(!borrowed_faults());CHECK(!complete_counts());CHECK(!catalog());CHECK(!nested_transition());CHECK(!invalid_transition());return 0;}
