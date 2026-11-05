#include "completion_locals.h"
#include "test_allocator.h"
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"C completion locals line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static const char text[]="int global=1;\nvoid other(int foreign){int hidden;}\nvoid step(double dt){\n double *velocity=0, damping=1;\n int (*callback)(double nested)=0;\n int current=foo(";
static int scopes(void){
    int owner=1,other=2;test_allocator a={0};ps_completion_locals p={.allocator=test_domain(&a)};
    CHECK(ps_completion_locals_update(&p,&owner,text,strlen(text),strlen(text),false)==PS_OK);
    CHECK(p.count==5 && p.call.active && p.call.at_start && p.status==PS_LANG_EDITOR_PREFIX && !p.language);
    const ps_completion_local *v=ps_completion_locals_find(&p,"velocity",8),*callback=ps_completion_locals_find(&p,"callback",8),*dt=ps_completion_locals_find(&p,"dt",2);
    CHECK(v && callback && dt && !strcmp(v->type,"double *") && !strcmp(v->symbol.signature,"double *velocity"));
    CHECK(!strcmp(callback->type,"int (*)(double nested)") && !strcmp(callback->symbol.signature,"int (*callback)(double nested)"));
    CHECK(!strcmp(v->symbol.signature+v->signature_name_offset,"velocity") && !strncmp(callback->symbol.signature+callback->signature_name_offset,"callback",8));
    CHECK(v->declaration.name.line==4 && dt->declaration.kind==PS_LANG_EDITOR_PARAMETER && v->symbol.kind==PS_COMPLETION_VARIABLE && !v->symbol.language);
    CHECK(v->id==ps_completion_count()+1+v->declaration.name.offset && ps_completion_locals_at(&p,v->id)==v);
    CHECK(!ps_completion_locals_find(&p,"foreign",7) && !ps_completion_locals_find(&p,"hidden",6) && !ps_completion_locals_find(&p,"nested",6) && !ps_completion_locals_find(&p,"current",7));
    ps_completion_query q={0};strcpy(q.prefix,"ve");size_t ids[8];CHECK(ps_completion_locals_matches(&p,&q,ids,8)==1 && ids[0]==v->id);
    q.prefix[0]=0;CHECK(ps_completion_locals_matches(&p,&q,NULL,0)==5);
    q.member=true;CHECK(!ps_completion_locals_matches(&p,&q,ids,8));q.member=false;
    q.language=true;CHECK(!ps_completion_locals_matches(&p,&q,ids,8));
    const char *owned=p.source;size_t attempts=a.attempts;
    CHECK(ps_completion_locals_update(&p,&owner,text,strlen(text),strlen(text),false)==PS_OK && p.source==owned && a.attempts==attempts);
    CHECK(ps_completion_locals_update(&p,&other,text,strlen(text),strlen(text),false)==PS_OK && p.owner==&other && a.attempts>attempts);
    const char *shadow="int value=1; void step(double value){ {const char *value=\"yes\"; foo(";
    CHECK(ps_completion_locals_update(&p,&owner,shadow,strlen(shadow),strlen(shadow),false)==PS_OK && p.count==1);
    CHECK(!strcmp(ps_completion_locals_find(&p,"value",5)->type,"const char *"));
    const char *many="void step(int dt){int a=0,b=1,c=2,d=3,e=4,f=5,g=6,h=7;foo(";
    CHECK(ps_completion_locals_update(&p,&owner,many,strlen(many),strlen(many),false)==PS_OK && p.count==9);
    q=(ps_completion_query){0};CHECK(ps_completion_locals_matches(&p,&q,ids,3)==9 && ps_completion_locals_matches(&p,&q,NULL,0)==9);
    CHECK(ps_completion_locals_update(&p,&owner,many,strlen(many),strlen(many),true)==PS_OK && p.language && !p.count);
    CHECK(ps_completion_locals_update(&p,&owner,many,strlen(many),strlen(many),false)==PS_OK && !p.language && p.count==9);
    const char *unicode="void step(int dt){ int Größe=1, $value=2, Gr\\u00f6sse=3; foo(";
    CHECK(ps_completion_locals_update(&p,&owner,unicode,strlen(unicode),strlen(unicode),false)==PS_OK && p.count==4);
    const char *prefixes[]={"Grö","$va","Gr\\u00f6"};
    for(size_t i=0;i<3;i++){
        ps_completion_query unicode_query={0};size_t length=strlen(prefixes[i]);
        CHECK(ps_completion_prepare(prefixes[i],length,length,false,&unicode_query)==PS_OK && unicode_query.start==0 && unicode_query.end==length && !strcmp(unicode_query.prefix,prefixes[i]));
        CHECK(ps_completion_locals_matches(&p,&unicode_query,ids,8)==1);
    }
    CHECK(ps_completion_locals_update(&p,&owner,")",1,1,false)==PS_OK && !p.count && p.status==PS_LANG_EDITOR_INVALID && !p.call.active);
    ps_completion_locals_destroy(&p);CHECK(!a.invalid && !a.live_blocks && !a.live_bytes);return 0;
}
static int failures(void){
    int owner=1,other=2;test_allocator a={0};ps_completion_locals p={.allocator=test_domain(&a)};
    CHECK(ps_completion_locals_update(&p,&owner,text,strlen(text),strlen(text),false)==PS_OK);size_t attempts=a.attempts;ps_completion_locals_destroy(&p);
    CHECK(attempts>10 && !a.invalid && !a.live_blocks);
    for(size_t fail=1;fail<=attempts;fail++){
        test_allocator t={0};p=(ps_completion_locals){.allocator=test_domain(&t)};
        CHECK(ps_completion_locals_update(&p,&owner,text,strlen(text),strlen(text),false)==PS_OK);
        const char *source=p.source;const ps_completion_local *v=ps_completion_locals_find(&p,"velocity",8);size_t live=t.live_bytes,blocks=t.live_blocks;
        t.fail_on=t.attempts+fail;CHECK(ps_completion_locals_update(&p,&other,text,strlen(text),strlen(text),false)==PS_MEMORY);
        CHECK(p.source==source && p.owner==&owner && ps_completion_locals_find(&p,"velocity",8)==v && t.live_bytes==live && t.live_blocks==blocks && !t.invalid);
        t.fail_on=0;CHECK(ps_completion_locals_update(&p,&other,text,strlen(text),strlen(text),false)==PS_OK);
        ps_completion_locals_destroy(&p);CHECK(!t.invalid && !t.live_blocks && !t.live_bytes);
    }
    printf("C completion locals allocation failures: %zu/%zu preserve snapshot and retry\n",attempts,attempts);return 0;
}
static int hints(void){
    const char *valid[]={"foo(","foo('x',", "foo(\"text,\",", "foo(/* closed */", "foo(// closed\n", "foo(\\\n"};
    const char *invalid[]={"foo(/* open", "foo(// open,", "foo(\"open,", "foo('open,", "foo(/\\\n* open,", "foo(// line\\\n continued,"};
    for(size_t i=0;i<sizeof valid/sizeof *valid;i++)CHECK(ps_completion_locals_argument_hint(valid[i],strlen(valid[i]),strlen(valid[i]),false));
    for(size_t i=0;i<sizeof invalid/sizeof *invalid;i++){
        CHECK(!ps_completion_locals_argument_hint(invalid[i],strlen(invalid[i]),strlen(invalid[i]),false));
        ps_completion_query q={0};CHECK(ps_completion_prepare(invalid[i],strlen(invalid[i]),strlen(invalid[i]),false,&q)==PS_INVALID);
    }
    return 0;
}
int main(void){CHECK(!scopes() && !failures() && !hints());puts("C completion locals: real types/source, scope merge, full count, cache/owner/language and atomic faults passed");return 0;}
