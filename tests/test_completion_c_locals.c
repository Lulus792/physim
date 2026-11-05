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
static int equivalent(const ps_completion_locals *a,const ps_completion_locals *b){
    CHECK(a->bytes==b->bytes && a->cursor==b->cursor && a->language==b->language && a->status==b->status && a->count==b->count);
    CHECK(!memcmp(a->source,b->source,a->bytes));
    CHECK(a->call.active==b->call.active && a->call.open==b->call.open && a->call.argument==b->call.argument && a->call.at_start==b->call.at_start);
    for(size_t i=0;i<a->count;i++){
        const ps_completion_local *x=&a->entries[i],*y=&b->entries[i];
        CHECK(x->id==y->id && x->signature_name_offset==y->signature_name_offset && x->symbol.kind==y->symbol.kind && x->symbol.language==y->symbol.language && x->symbol.instance==y->symbol.instance);
        CHECK(!strcmp(x->symbol.name,y->symbol.name) && !strcmp(x->type,y->type) && !strcmp(x->symbol.signature,y->symbol.signature));
        CHECK(x->declaration.name.offset==y->declaration.name.offset && x->declaration.name.length==y->declaration.name.length && x->declaration.name.line==y->declaration.name.line && x->declaration.name.column==y->declaration.name.column && x->declaration.kind==y->declaration.kind);
        const ps_c_binding c=x->c_declaration,d=y->c_declaration;
        CHECK(c.name_start==d.name_start && c.name_end==d.name_end && c.line==d.line && c.column==d.column && c.type_start==d.type_start && c.type_end==d.type_end && c.declarator_start==d.declarator_start && c.declarator_end==d.declarator_end && c.parameter==d.parameter);
    }
    const char *prefixes[]={"","v","dt","Gr","$","ps_"};
    for(size_t i=0;i<sizeof prefixes/sizeof *prefixes;i++){
        ps_completion_query q={0};strcpy(q.prefix,prefixes[i]);size_t x[64],y[64];
        size_t count=ps_completion_locals_matches(a,&q,x,64);CHECK(count==ps_completion_locals_matches(b,&q,y,64) && count<=64);
        for(size_t j=0;j<count;j++)CHECK(x[j]==y[j]);
    }
    return 0;
}
static int revision_compare(ps_completion_locals *current,const char *text,size_t bytes,size_t cursor,const void *owner){
    ps_completion_locals fresh={0};
    CHECK(ps_completion_locals_update(current,owner,text,bytes,cursor,false)==PS_OK);
    CHECK(ps_completion_locals_update(&fresh,owner,text,bytes,cursor,false)==PS_OK);
    CHECK(!equivalent(current,&fresh));ps_completion_locals_destroy(&fresh);return 0;
}
static int revisions(void){
    const char *sources[]={text,
        "int velocity=1; void f(int dt){int local=2; consume(velocity, local);}",
        "int position=1; void f(int dt){int local=2; consume(position, local);}",
        "int global=1; void f(int dt){ {double global=2; consume(global);} consume(global);}",
        "void f(a,b) int a; double b; {int local=1; consume(a,b,local);} int after=0;",
        "void f(int dt){for(int index=0; index<3; ++index) consume(index); consume(dt);}",
        "void f(int dt){const char *value=\"not a name,\"; /* comment */ consume(value, dt);}",
        "void f(int dt){int Größe=1, $value=2, Gr\\u00f6sse=3; consume(Größe, dt);}",
        "void f(int dt){int ready=1, unfinished=; consume(ready,", ")", ""};
    int owner=0,other=1;ps_completion_locals current={0};size_t comparisons=0;
    char buffer[1024];
    const char *declaration="void f(int dt){double value=dt; consume(  );}";
    const char *expression="void f(int dt){return value=dt; consume(  );}";
    size_t length=strlen(declaration),position=(size_t)(strstr(declaration,"consume(")-declaration)+8;
    CHECK(length==strlen(expression));
    memcpy(buffer,declaration,length+1);CHECK(!revision_compare(&current,buffer,length,position,&owner));comparisons++;
    CHECK(ps_completion_locals_find(&current,"value",5));
    memcpy(buffer,expression,length+1);CHECK(!revision_compare(&current,buffer,length,position,&owner));comparisons++;
    CHECK(!ps_completion_locals_find(&current,"value",5) && ps_completion_locals_find(&current,"dt",2));
    for(size_t s=0;s<sizeof sources/sizeof *sources;s++){
        size_t bytes=strlen(sources[s]);CHECK(bytes<sizeof buffer);memcpy(buffer,sources[s],bytes+1);
        /* Same buffer address and equal-length renamed revision; no pointer-
         * only or length-only cache identity is admissible. Reverse traversal
         * crosses scopes/calls, UTF-8 boundaries and legacy parameter regions. */
        for(size_t i=bytes+1;i--;){
            if(i<bytes && ((unsigned char)buffer[i]&0xc0)==0x80)continue;
            CHECK(!revision_compare(&current,buffer,bytes,i,&owner));comparisons++;
        }
        CHECK(!revision_compare(&current,buffer,bytes,bytes,&other));comparisons++;
    }
    const char *typing="void f(int dt){double velocity=dt; consume(velocity, dt);}";
    size_t bytes=strlen(typing);
    /* Every inserted/deleted byte creates a real new full-source revision;
     * recovery, closing delimiters and UTF-8 names are covered above. */
    for(size_t n=0;n<=bytes;n++){memcpy(buffer,typing,n);buffer[n]=0;CHECK(!revision_compare(&current,buffer,n,n,&owner));comparisons++;}
    for(size_t n=bytes;n--;){buffer[n]=0;CHECK(!revision_compare(&current,buffer,n,n,&owner));comparisons++;}
    ps_completion_locals_destroy(&current);
    printf("C completion revisions: %zu fresh-reference comparisons across edits/cursors/owners\n",comparisons);return 0;
}
static int cache_failures(bool edit){
    const char *before="int value=1; void f(int dt){double velocity=dt; consume(velocity, dt);}";
    const char *after="int value=1; void f(int dt){double position=dt; consume(position, dt);}";
    int owner=0;size_t bytes=strlen(before),first=(size_t)(strstr(before,"consume(")-before)+8;
    size_t next=edit?strlen(after):first+10;const char *target=edit?after:before;
    test_allocator probe={0};ps_completion_locals p={.allocator=test_domain(&probe)};
    CHECK(ps_completion_locals_update(&p,&owner,before,bytes,first,false)==PS_OK);
    size_t count=probe.attempts;CHECK(ps_completion_locals_update(&p,&owner,target,strlen(target),next,false)==PS_OK);
    count=probe.attempts-count;ps_completion_locals_destroy(&p);CHECK(!probe.invalid && !probe.live_blocks && count);
    ps_completion_locals oracle={0};CHECK(ps_completion_locals_update(&oracle,&owner,before,bytes,first,false)==PS_OK);
    for(size_t failure=1;failure<=count;failure++){
        test_allocator a={0};p=(ps_completion_locals){.allocator=test_domain(&a)};
        CHECK(ps_completion_locals_update(&p,&owner,before,bytes,first,false)==PS_OK);
        const char *source=p.source,*strings=p.strings;const ps_completion_local *entries=p.entries;
        const ps_c_bindings_source *syntax=p.c_source;size_t live=a.live_bytes,blocks=a.live_blocks;
        a.fail_on=a.attempts+failure;
        CHECK(ps_completion_locals_update(&p,&owner,target,strlen(target),next,false)==PS_MEMORY);
        CHECK(p.source==source && p.strings==strings && p.entries==entries && p.c_source==syntax && a.live_bytes==live && a.live_blocks==blocks && !a.invalid && !equivalent(&p,&oracle));
        a.fail_on=0;CHECK(ps_completion_locals_update(&p,&owner,target,strlen(target),next,false)==PS_OK);
        CHECK(edit || p.c_source==syntax);CHECK(!revision_compare(&p,target,strlen(target),next,&owner));
        ps_completion_locals_destroy(&p);CHECK(!a.invalid && !a.live_blocks && !a.live_bytes);
    }
    ps_completion_locals_destroy(&oracle);
    printf("C completion %s failures: %zu/%zu preserve source/syntax/results and retry\n",edit?"edit":"cursor",count,count);return 0;
}
int main(void){CHECK(!scopes() && !failures() && !hints() && !revisions() && !cache_failures(false) && !cache_failures(true));puts("C completion locals: real types/source, scope merge, full count, cache/owner/language, revisions and atomic faults passed");return 0;}
