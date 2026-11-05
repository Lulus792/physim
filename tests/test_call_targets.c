#include "completion_locals.h"
#include <string.h>
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Call target line %d: %s\n",__LINE__,#x);return 1;}}while(0)
typedef struct {const char *text,*name,*receiver;size_t argument;bool language,active,member;} vector;
#define C(t,n,r,a,m) {t,n,r,a,false,true,m}
#define P(t,n,r,a,m) {t,n,r,a,true,true,m}
static const vector cases[]={
 C("void f(int dt){foo(|","foo","",0,false),
 C("void f(int dt){foo(dt,|);}","foo","",1,false),
 C("void f(int dt){foo(inner(1,|","inner","",1,false),
 C("void f(int dt){foo(inner(1,2),|","foo","",1,false),
 C("void f(int dt){foo(a[1,2],|","foo","",1,false),
 C("void f(int dt){(foo)(|","foo","",0,false),
 C("void f(int dt){((foo))(|","foo","",0,false),
 C("void f(int dt){foo /* before call */ (|","foo","",0,false),
 C("void f(int dt){Größe(|","Größe","",0,false),
 C("void f(int dt){Gr\\u00f6sse(|","Gr\\u00f6sse","",0,false),
 C("void f(int dt){$call(|","$call","",0,false),
 C("void f(int dt){obj.run(|","run","obj",0,true),
 C("void f(int dt){p->run(|","run","p",0,true),
 C("void f(int dt){factory().run(|","run","factory()",0,true),
 C("void f(int dt){(*callback)(|","","",0,false),
 C("void f(int dt){callbacks[0](|","","",0,false),
 C("void f(int dt){factory()(|","","",0,false),
 C("void f(int dt){int result=foo(|","foo","",0,false),
 {"void declaration(int |","","",0,false,false,false},
 {"void f(int dt){foo(\"open|","","",0,false,false,false},
 P("func step(dt: Float64):\n    foo(|","foo","",0,false),
 P("func step(dt: Float64):\n    foo(dt,|)","foo","",1,false),
 P("func step(dt: Float64):\n    foo(inner(1,|","inner","",1,false),
 P("func step(dt: Float64):\n    foo(inner(1,2),|","foo","",1,false),
 P("func step(dt: Float64):\n    (foo)(|","foo","",0,false),
 P("func step(dt: Float64):\n    ((foo))(|","foo","",0,false),
 P("func step(dt: Float64):\n    Größe(|","Größe","",0,false),
 P("func step(dt: Float64):\n    foo<Float64>(|","foo","",0,false),
 P("func step(dt: Float64):\n    obj.run(|","run","obj",0,true),
 P("func step(dt: Float64):\n    (obj.run)(|","run","obj",0,true),
 P("func step(dt: Float64):\n    factory().run(|","run","factory()",0,true),
 P("func step(dt: Float64):\n    Vec3<Float64>.make(|","make","Vec3<Float64>",0,true),
 P("func step(dt: Float64):\n    callbacks[0](|","","",0,false),
 P("func step(dt: Float64):\n    factory()(|","","",0,false),
 {"func declaration(value: |","","",0,true,false,false},
 {"func step(dt: Float64):\n    foo(\"open|","","",0,true,false,false},
};
#undef C
#undef P
static bool span(const char *text,size_t bytes,size_t start,size_t end,const char *expected){
    return start<=end && end<=bytes && end-start==strlen(expected) && !memcmp(text+start,expected,end-start);
}
int main(int argc,char **argv){
    bool only_phys=argc==2 && !strcmp(argv[1],"phys");size_t total=0;
    int owner=0;ps_completion_locals p={0};
    for(size_t i=0;i<sizeof cases/sizeof *cases;i++){
        const vector *v=&cases[i];if(only_phys && !v->language)continue;
        const char *mark=strchr(v->text,'|');CHECK(mark);size_t cursor=(size_t)(mark-v->text),bytes=strlen(v->text)-1;
        char text[1024];CHECK(bytes<sizeof text);memcpy(text,v->text,cursor);memcpy(text+cursor,mark+1,bytes-cursor);text[bytes]=0;
        CHECK(ps_completion_locals_update(&p,&owner,text,bytes,cursor,v->language)==PS_OK);
        ps_lang_editor_call call=p.call;
        if(call.active!=v->active || call.argument!=v->argument || call.member!=v->member ||
           !span(text,bytes,call.name_start,call.name_end,v->name) || !span(text,bytes,call.receiver_start,call.receiver_end,v->receiver)){
            fprintf(stderr,"Call target vector %zu language=%d active=%d arg=%zu member=%d name=%zu..%zu receiver=%zu..%zu expected %s / %s source=%s\n",i,v->language,call.active,call.argument,call.member,call.name_start,call.name_end,call.receiver_start,call.receiver_end,v->name,v->receiver,text);return 1;
        }
        if(call.active){
            CHECK(call.open<cursor && text[call.open]=='(' && call.callee_start<call.callee_end && call.callee_end<=call.open);
            if(call.name_end)CHECK(call.callee_start<=call.name_start && call.name_end<=call.callee_end);
        }
        ps_completion_locals fresh={0};CHECK(ps_completion_locals_update(&fresh,&owner,text,bytes,cursor,v->language)==PS_OK);
        CHECK(fresh.call.callee_start==call.callee_start && fresh.call.callee_end==call.callee_end && fresh.call.name_start==call.name_start && fresh.call.name_end==call.name_end && fresh.call.receiver_start==call.receiver_start && fresh.call.receiver_end==call.receiver_end && fresh.call.member==call.member);
        ps_completion_locals_destroy(&fresh);total++;
    }
    ps_completion_locals_destroy(&p);
    printf("Call targets: %zu actual C/Physim grammar vectors, exact original name/receiver spans and unnamed indirect targets passed\n",total);return 0;
}
