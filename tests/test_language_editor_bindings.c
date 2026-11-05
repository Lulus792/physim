#include "language/editor_bindings.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#define CHECK(x) do {if (!(x)) {fprintf(stderr,"Editor bindings line %d: %s\n",__LINE__,#x);return 1;}} while (0)
static ps_lang_node nodes[8192];
static ps_lang_node strict_nodes[8192];
static ps_lang_editor_binding bindings[1024];
static ps_lang_editor_bindings_result result;
static char source[16384];
static size_t cursor, cases;
static int same_node(const ps_lang_node *a,const ps_lang_node *b) {
    const ps_lang_token *x=&a->token,*y=&b->token;
    return a->kind==b->kind && a->a==b->a && a->b==b->b && a->c==b->c && a->next==b->next &&
        a->type_start==b->type_start && a->type_end==b->type_end && x->kind==y->kind &&
        x->offset==y->offset && x->length==y->length && x->line==y->line && x->column==y->column &&
        x->file==y->file && x->error==y->error;
}
static int inspect(const char *marked) {
    const char *at = strchr(marked,'@');
    if (!at || strlen(marked) >= sizeof source) return 0;
    cursor = (size_t)(at-marked);
    memcpy(source,marked,cursor);strcpy(source+cursor,at+1);
    char original[sizeof source];memcpy(original,source,sizeof source);
    result = ps_lang_editor_bindings(source,strlen(source),cursor,nodes,8192,bindings,1024);
    if (memcmp(original,source,sizeof source)) return 0;
    if (result.status != PS_LANG_EDITOR_OK && result.status != PS_LANG_EDITOR_PREFIX) {
        fprintf(stderr,"Prefix status=%d offset=%zu cursor=%zu error=%s\n",result.status,result.diagnostic.offset,cursor,result.diagnostic.error?result.diagnostic.error:"");
        return 0;
    }
    ps_lang_parse_result strict=ps_lang_parse(source,cursor,strict_nodes,8192);
    for (size_t i=0;i<strict.count;i++) if (!same_node(&nodes[i],&strict_nodes[i])) return 0;
    cases++;return 1;
}
static const ps_lang_editor_binding *find(const char *name) {
    for (size_t i=result.count;i--;) {
        const ps_lang_editor_binding *b=&bindings[i];
        if (b->kind==PS_LANG_EDITOR_SELF) {if (!strcmp(name,"self")) return b;}
        else if (b->name.length==strlen(name) && !memcmp(source+b->name.offset,name,b->name.length)) return b;
    }
    return NULL;
}
static int type_is(const char *name,const char *text) {
    const ps_lang_editor_binding *b=find(name);if (!b || b->type_end<b->type_start) return 0;
    size_t end=b->type_end;
    while (end>b->type_start && (source[end-1]==' ' || source[end-1]=='\t' || source[end-1]=='\r' || source[end-1]=='\n')) end--;
    return end-b->type_start==strlen(text) && !memcmp(source+b->type_start,text,strlen(text));
}
int main(void) {
    CHECK(inspect("var global: Int64 = 1\nfunc other(foreign: Int64):\n    var hidden = 2\nfunc step(dt: Float64):\n    var velocity: Vec3 = Vec3(0,1,0)\n    let gravity: Vec3 = Vec3(0,-9,0)\n    let damping = 0.02\n    let result = Vec3.add(ve@, gravity)\n    var later = 4\n"));
    CHECK(find("global") && find("dt") && find("velocity") && find("gravity") && find("damping"));
    CHECK(!find("foreign") && !find("hidden") && !find("result") && !find("later"));
    CHECK(type_is("dt","Float64") && type_is("velocity","Vec3"));
    CHECK(find("dt")->kind==PS_LANG_EDITOR_PARAMETER && find("dt")->name.line==4);
    CHECK(find("damping")->type_start==0 && find("damping")->type_end==0);
    CHECK(result.status==PS_LANG_EDITOR_PREFIX);

    CHECK(inspect("var value: Int64 = 1\nfunc step(value: Float64):\n    if true:\n        var value: String = \"inner\"\n        print(va@)\n"));
    CHECK(type_is("value","String") && find("value")->name.line==4);
    CHECK(inspect("var value: Int64 = 1\nfunc step(value: Float64):\n    if true:\n        var value: String = \"inner\"\n    print(va@)\n"));
    CHECK(type_is("value","Float64") && find("value")->kind==PS_LANG_EDITOR_PARAMETER);
    CHECK(inspect("var value: Int64 = 1\nfunc step(value: Float64):\n    print(value)\nprint(va@)\n"));
    CHECK(type_is("value","Int64"));

    CHECK(inspect("func outer(a: Float64):\n    var captured = 1\n    func nested(b: Int64):\n        let inside = 2\n        print(@)\n"));
    CHECK(find("a") && find("b") && find("captured") && find("inside"));
    CHECK(inspect("func outer(a: Float64):\n    func nested(b: Int64):\n        let inside = 2\n    print(@)\n"));
    CHECK(find("a") && !find("b") && !find("inside"));
    CHECK(inspect("func outer(a: Float64):\n    let closure = func(b: Int64) -> Int64:\n        let local = b\n        return a + @\n"));
    CHECK(find("a") && find("b") && find("local") && !find("closure"));
    CHECK(inspect("func outer(a: Float64):\n    let closure = func(b: Int64) -> Int64:\n        return b\n    print(cl@)\n"));
    CHECK(find("closure") && !find("b"));
    CHECK(inspect("func outer(a: Float64):\n    let closure = func(b: Int64) -> Int64:\n        return b\n    @"));
    CHECK(find("closure") && !find("b"));

    CHECK(inspect("func step():\n    for index in 0..<4:\n        let local = index\n        print(@)\n"));
    CHECK(find("index") && find("index")->kind==PS_LANG_EDITOR_ITERATOR && find("local"));
    CHECK(inspect("func step():\n    for index in 0..<4:\n        let local = index\n    print(@)\n"));
    CHECK(!find("index") && !find("local"));
    CHECK(inspect("func step(optional: Float64?):\n    if let value: Float64 = optional:\n        print(@)\n"));
    CHECK(find("value") && type_is("value","Float64"));
    CHECK(inspect("func step(optional: Float64?):\n    if let value = optional:\n        print(value)\n    else:\n        print(@)\n"));
    CHECK(find("optional") && !find("value"));
    CHECK(inspect("func step(optional: Float64?):\n    while let value = optional:\n        print(@)\n"));
    CHECK(find("value"));
    CHECK(inspect("func step(optional: Float64?):\n    guard let value = optional else:\n        print(@)\n        return\n"));
    CHECK(find("optional") && !find("value"));
    CHECK(inspect("func step(optional: Float64?):\n    guard let value = optional else:\n        return\n    print(@)\n"));
    CHECK(find("value"));
    CHECK(inspect("func step(optional: Float64?):\n    guard let value = optional else:\n        return\n    @"));
    CHECK(find("value"));

    CHECK(inspect("func step(event: Event):\n    switch event:\n        case Event.item(payload) if accepts(@):\n            return\n"));
    CHECK(find("payload") && find("payload")->kind==PS_LANG_EDITOR_PATTERN);
    CHECK(inspect("func step(event: Event):\n    switch event:\n        case Event.item(Optional.some(payload), _):\n            print(@)\n"));
    CHECK(find("payload") && !find("_"));
    CHECK(inspect("func step(event: Event):\n    switch event:\n        case Event.item(payload):\n            print(payload)\n        default:\n            print(@)\n"));
    CHECK(!find("payload") && find("event"));

    CHECK(inspect("struct Particle:\n    var mass: Float64\n    func step(dt: Float64):\n        print(@)\n"));
    CHECK(find("self") && type_is("self","Particle") && find("dt") && !find("mass"));
    CHECK(inspect("struct Particle:\n    var mass: Float64\n    static func step(dt: Float64):\n        print(@)\n"));
    CHECK(!find("self") && find("dt") && !find("mass"));
    CHECK(inspect("enum State:\n    case idle\n    func step(dt: Float64):\n        print(@)\n"));
    CHECK(find("self") && type_is("self","State"));
    CHECK(inspect("func outer<T>(values: [T], callback: (func(Int64, [Float64]?) -> Module.Value?)):\n    print(@)\n"));
    CHECK(type_is("values","[T]") && type_is("callback","(func(Int64, [Float64]?) -> Module.Value?)"));

    CHECK(inspect("var earlier = 1\nvar own: Int64 = make(ow@)\n"));
    CHECK(find("earlier") && !find("own"));
    CHECK(inspect("var earlier = 1\nvar own = ow@"));
    CHECK(find("earlier") && !find("own"));
    CHECK(inspect("var earlier = 1; var uninitialized: Float64\nprint(@)\n"));
    CHECK(find("earlier") && type_is("uninitialized","Float64"));
    CHECK(inspect("let a = 1; let b = 2; print(@)"));
    CHECK(find("a") && find("b"));
    CHECK(inspect("/* outer /* let fake = 0 */ nested */\nlet real = \"let fictional = 1\"\n// var imaginary = 2\nprint(@)"));
    CHECK(find("real") && !find("fake") && !find("fictional") && !find("imaginary"));
    CHECK(inspect("func f(Größe: Float64):\n    let größe = 1\n    print(@)\n"));
    CHECK(find("Größe") && find("größe"));
    CHECK(inspect("func f(value: Float64):\n    let local = 1\n    @"));
    CHECK(find("value") && find("local"));
    CHECK(inspect("func f(value: Float64):\n    let local = 1\n@"));
    CHECK(!find("value") && !find("local"));
    CHECK(inspect("func f(value: Float64):\n    print(\n@"));
    CHECK(find("value"));
    CHECK(inspect("func f(value: Float64):\n    @"));
    CHECK(find("value"));
    CHECK(inspect("func f(value: Float64):\n    let local = 1\n@\n    print(value)\n"));
    CHECK(find("value") && find("local"));
    CHECK(inspect("func f(value: Float64):\n    if true:\n        let hidden = 1\n@\n    print(value)\n    let future = 2\n"));
    CHECK(find("value") && !find("hidden") && !find("future"));
    CHECK(inspect("foo(@"));CHECK(result.call.active && result.call.at_start && result.call.argument==0 && result.call.open==3);
    CHECK(inspect("foo(first, @"));CHECK(result.call.active && result.call.at_start && result.call.argument==1);
    CHECK(inspect("foo(first, bar(@"));CHECK(result.call.active && result.call.at_start && result.call.argument==0 && result.call.open==14);
    CHECK(inspect("foo(first, bar(value), @"));CHECK(result.call.active && result.call.at_start && result.call.argument==2 && result.call.open==3);
    CHECK(inspect("foo([first, second], @"));CHECK(result.call.active && result.call.at_start && result.call.argument==1);
    CHECK(inspect("foo([first, @"));CHECK(result.call.active && !result.call.at_start && result.call.argument==0);
    CHECK(inspect("foo((@"));CHECK(result.call.active && !result.call.at_start);
    CHECK(inspect("foo(label: @"));CHECK(result.call.active && result.call.at_start && result.call.argument==0);
    CHECK(inspect("foo(\"a,b\", /* , */ @"));CHECK(result.call.active && result.call.at_start && result.call.argument==1);
    CHECK(inspect("foo()@"));CHECK(!result.call.active);
    CHECK(inspect("func foo(value: @"));CHECK(!result.call.active);
    CHECK(inspect("var callback: func(@"));CHECK(!result.call.active);
    CHECK(inspect("foo(na@"));CHECK(result.call.active && !result.call.at_start);

    const char bad[]="var global = 1\nfunc broken(:\n    print(\n";
    result=ps_lang_editor_bindings(bad,strlen(bad),strlen(bad),nodes,8192,bindings,1024);
    CHECK(result.status==PS_LANG_EDITOR_INVALID && !result.count && result.diagnostic.offset<strlen(bad));
    const char bounded[]="var value = 1\nprint(\n";
    result=ps_lang_editor_bindings(bounded,strlen(bounded),strlen(bounded),nodes,2,bindings,1024);
    CHECK(result.status==PS_LANG_EDITOR_LIMIT && !result.count);
    const char exact[]="let exact = 1\n";
    ps_lang_parse_result exact_parse=ps_lang_parse(exact,strlen(exact),strict_nodes,8192);
    CHECK(exact_parse.root);
    result=ps_lang_editor_bindings(exact,strlen(exact),strlen(exact),nodes,exact_parse.count,bindings,1024);
    CHECK(result.status==PS_LANG_EDITOR_OK && result.count==1);
    const char closed[]="func previous(parameter: Float64):\n    let local = 1\nprint(\n";
    result=ps_lang_editor_bindings(closed,strlen(closed),strlen(closed),nodes,8192,NULL,0);
    CHECK(result.status==PS_LANG_EDITOR_PREFIX && !result.count);
    result=ps_lang_editor_bindings(bounded,strlen(bounded),strlen(bounded),nodes,8192,NULL,0);
    CHECK(result.status==PS_LANG_EDITOR_LIMIT && !result.count);
    result=ps_lang_editor_bindings(NULL,0,0,nodes,8192,NULL,0);
    CHECK(result.status==PS_LANG_EDITOR_OK && !result.count);
    CHECK(ps_lang_editor_bindings(NULL,1,0,nodes,8192,bindings,1024).status==PS_LANG_EDITOR_INVALID);
    CHECK(ps_lang_editor_bindings(bounded,strlen(bounded),strlen(bounded)+1,nodes,8192,bindings,1024).status==PS_LANG_EDITOR_INVALID);
    CHECK(ps_lang_editor_bindings("é",2,1,nodes,8192,bindings,1024).status==PS_LANG_EDITOR_INVALID);
    /* Compare every AST field against the independent strict entry point,
     * including rejected prefixes. The observer must not change parsing. */
    uint32_t random=0x5c0f2026;
    const char alphabet[]="abc012 \n:(),[]+-*/=\"";
    for (size_t sample=0;sample<1024;sample++) {
        char fuzz[128];size_t length=sample%sizeof fuzz;
        for (size_t i=0;i<length;i++) {random=random*1664525u+1013904223u;fuzz[i]=alphabet[random%(sizeof alphabet-1)];}
        ps_lang_editor_bindings_result observed=ps_lang_editor_bindings(fuzz,length,length,nodes,8192,bindings,1024);
        ps_lang_parse_result strict=ps_lang_parse(fuzz,length,strict_nodes,8192);
        CHECK(strict.count<=8192);
        for (size_t i=0;i<strict.count;i++) CHECK(same_node(&nodes[i],&strict_nodes[i]));
        CHECK(observed.status!=PS_LANG_EDITOR_OK || strict.root);
        CHECK(observed.status!=PS_LANG_EDITOR_PREFIX || (!strict.root && strict.diagnostic.offset==length));
        for (size_t i=0;i<observed.count;i++) {
            CHECK(bindings[i].name.offset<=length && bindings[i].name.length<=length-bindings[i].name.offset);
            CHECK(bindings[i].type_start<=bindings[i].type_end && bindings[i].type_end<=length);
        }
    }
    printf("Physim editor bindings: %zu real-parser prefixes, lexical scopes, shadow lookup, declarations, parameters, loops, guards, patterns, self, type spans, UTF-8 and bounded failures passed\n",cases);
    puts("Strict/observed AST field parity and binding bounds: 1024 deterministic byte inputs passed");
    return 0;
}
