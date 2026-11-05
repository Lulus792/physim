#include "completion_locals.h"
#include "autosave.h"
#include "test_allocator.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Completion locals line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static const char text[]="var global: Int64 = 1\nfunc other(foreign: Int64):\n    let hidden = 1\nfunc step(dt: Float64):\n    let velocity: Vec3 = Vec3(0,1,0)\n    var gravity: Vec3 = Vec3(0,-9,0)\n    let result = Vec3.add(";
static int scopes(void){
    int owner=1,other=2;test_allocator tracker={0};ps_completion_locals locals={.allocator=test_domain(&tracker)};
    CHECK(ps_completion_locals_update(&locals,&owner,text,strlen(text),strlen(text),true)==PS_OK);
    CHECK(locals.count==4 && locals.call.active && locals.call.at_start && locals.status==PS_LANG_EDITOR_PREFIX);
    const ps_completion_local *v=ps_completion_locals_find(&locals,"velocity",8);
    CHECK(v && !strcmp(v->type,"Vec3") && !strcmp(v->symbol.signature,"let velocity: Vec3") && v->symbol.kind==PS_COMPLETION_VARIABLE);
    CHECK(v->declaration.name.line==5 && v->id>ps_completion_count() && ps_completion_locals_at(&locals,v->id)==v);
    CHECK(!ps_completion_locals_find(&locals,"foreign",7) && !ps_completion_locals_find(&locals,"hidden",6) && !ps_completion_locals_find(&locals,"result",6));
    CHECK(!ps_completion_locals_at(&locals,0) && !ps_completion_locals_at(&locals,SIZE_MAX));
    ps_completion_query q={.language=true};strcpy(q.prefix,"ve");size_t ids[8]={0};
    CHECK(ps_completion_locals_matches(&locals,&q,ids,8)==1 && ids[0]==v->id);
    q.prefix[0]=0;CHECK(ps_completion_locals_matches(&locals,&q,NULL,0)==4);
    q.member=true;CHECK(!ps_completion_locals_matches(&locals,&q,ids,8));q.member=false;
    q.language=false;CHECK(!ps_completion_locals_matches(&locals,&q,ids,8));
    const char *owned=locals.source,*name=v->symbol.name;size_t attempts=tracker.attempts;
    CHECK(ps_completion_locals_update(&locals,&owner,text,strlen(text),strlen(text),true)==PS_OK && locals.source==owned && tracker.attempts==attempts);
    CHECK(!strcmp(name,"velocity"));
    CHECK(ps_completion_locals_update(&locals,&other,text,strlen(text),strlen(text),true)==PS_OK && locals.owner==&other && tracker.attempts>attempts);
    const char shadow[]="var value: Int64 = 1\nfunc step(value: Float64):\n    if true:\n        let value: String = \"yes\"\n        print(";
    CHECK(ps_completion_locals_update(&locals,&owner,shadow,strlen(shadow),strlen(shadow),true)==PS_OK && locals.count==1);
    CHECK(!strcmp(ps_completion_locals_find(&locals,"value",5)->type,"String"));
    const char unicode[]="func step(dt: Float64):\n    let Größe: Float64 = dt\n    print(";
    CHECK(ps_completion_locals_update(&locals,&owner,unicode,strlen(unicode),strlen(unicode),true)==PS_OK);
    CHECK(ps_completion_locals_find(&locals,"Größe",strlen("Größe")));
    const char inferred[]="let value = 1\nprint(";
    CHECK(ps_completion_locals_update(&locals,&owner,inferred,strlen(inferred),strlen(inferred),true)==PS_OK);
    CHECK(!*ps_completion_locals_find(&locals,"value",5)->type && !strcmp(ps_completion_locals_find(&locals,"value",5)->symbol.signature,"let value"));
    const char self[]="struct Particle:\n    func step():\n        print(";
    CHECK(ps_completion_locals_update(&locals,&owner,self,strlen(self),strlen(self),true)==PS_OK);
    CHECK(!strcmp(ps_completion_locals_find(&locals,"self",4)->type,"Particle"));
    const char invalid[]="func broken(:\n    print(";
    CHECK(ps_completion_locals_update(&locals,&owner,invalid,strlen(invalid),strlen(invalid),true)==PS_OK && !locals.count && locals.status==PS_LANG_EDITOR_INVALID && !locals.call.active);
    CHECK(ps_completion_locals_update(&locals,&owner,text,strlen(text),strlen(text),false)==PS_OK && !locals.count && !locals.source && !locals.language);
    ps_completion_locals_destroy(&locals);ps_completion_locals_destroy(&locals);ps_completion_locals_destroy(NULL);
    CHECK(!tracker.invalid && !tracker.live_blocks && !tracker.live_bytes);return 0;
}
static int failures(void){
    int owner=1,other=2;test_allocator tracker={0};ps_completion_locals probe={.allocator=test_domain(&tracker)};
    CHECK(ps_completion_locals_update(&probe,&owner,text,strlen(text),strlen(text),true)==PS_OK);
    size_t allocations=tracker.attempts;CHECK(allocations>=7);ps_completion_locals_destroy(&probe);
    CHECK(!tracker.live_blocks && !tracker.invalid);
    for(size_t fail=1;fail<=allocations;fail++){
        test_allocator failing={0};ps_completion_locals locals={.allocator=test_domain(&failing)};
        CHECK(ps_completion_locals_update(&locals,&owner,text,strlen(text),strlen(text),true)==PS_OK);
        const char *source=locals.source;const ps_completion_local *entry=ps_completion_locals_find(&locals,"velocity",8);
        size_t live=failing.live_bytes,blocks=failing.live_blocks;failing.fail_on=failing.attempts+fail;
        CHECK(ps_completion_locals_update(&locals,&other,text,strlen(text),strlen(text),true)==PS_MEMORY);
        CHECK(locals.owner==&owner && locals.source==source && ps_completion_locals_find(&locals,"velocity",8)==entry);
        CHECK(failing.live_bytes==live && failing.live_blocks==blocks && !failing.invalid);
        ps_completion_locals_destroy(&locals);CHECK(!failing.live_blocks && !failing.live_bytes && !failing.invalid);
    }
    return 0;
}
static int bounds(void){
    int owner=0;ps_completion_locals locals={0};char source[]="let alpha: Int64 = 1\nprint(";
    CHECK(ps_completion_locals_update(&locals,&owner,source,strlen(source),strlen(source),true)==PS_OK);
    const char *owned=locals.source;source[4]='b';CHECK(strstr(locals.source,"alpha") && !strstr(locals.source,"blpha"));
    CHECK(ps_completion_locals_update(&locals,&owner,"x",PS_SOURCE_MAX_BYTES+1,0,true)==PS_LIMIT && locals.source==owned);
    CHECK(ps_completion_locals_update(&locals,&owner,"x",1,2,true)==PS_INVALID && locals.source==owned);
    CHECK(ps_completion_locals_update(&locals,NULL,"x",1,1,true)==PS_INVALID && locals.source==owned);
    CHECK(ps_completion_locals_update(&locals,&owner,"é",2,1,true)==PS_INVALID && locals.source==owned);
    CHECK(ps_completion_locals_update(&locals,&owner,"x\0y",3,3,true)==PS_INVALID && locals.source==owned);
    CHECK(ps_completion_locals_argument_hint("foo( /* hi */ ",14,14,true));
    CHECK(!ps_completion_locals_argument_hint("foo(\"a,",7,7,true));
    CHECK(!ps_completion_locals_argument_hint("foo(",4,4,false));
    ps_completion_locals_destroy(&locals);return 0;
}
int main(void){CHECK(!scopes() && !failures() && !bounds());puts("Completion locals: real Physim scope merge, stable IDs, source/type ownership, Unicode, cache reuse and every allocation failure passed");return 0;}
