#include "completion.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"Completion line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static bool includes(const ps_completion_query *q,const char *name) {
    size_t indices[2048],count=ps_completion_matches(q,indices,2048);
    if(count>2048)return false;
    for(size_t i=0;i<count;i++)if(!strcmp(ps_completion_at(indices[i])->name,name))return true;
    return false;
}
int main(void) {
    ps_completion_query q;
    CHECK(ps_completion_count()>1000 && ps_completion_at(ps_completion_count())==NULL);
    const char *c="// α\nps_vlold";
    CHECK(ps_completion_prepare(c,strlen(c),strlen(c)-3,false,&q)==PS_OK);
    CHECK(q.start==6 && q.end==strlen(c) && !strcmp(q.prefix,"ps_vl"));
    CHECK(includes(&q,"ps_vlength"));
    CHECK(ps_completion_prepare("PS_MET",6,6,false,&q)==PS_OK && includes(&q,"PS_METRE"));
    CHECK(ps_completion_prepare("ps_memory_al",12,12,false,&q)==PS_OK && includes(&q,"ps_memory_alignment"));
    CHECK(ps_completion_prepare("Vec",3,3,true,&q)==PS_OK && includes(&q,"Vec3") && !includes(&q,"Vec3.length"));
    CHECK(ps_completion_prepare("Bool",4,4,true,&q)==PS_OK && includes(&q,"Bool"));
    CHECK(ps_completion_prepare("T",1,1,true,&q)==PS_OK && !includes(&q,"T"));
    CHECK(ps_completion_prepare("velocity.len",12,12,true,&q)==PS_OK && q.member && includes(&q,"Vec3.length"));
    size_t first=0;CHECK(ps_completion_matches(&q,&first,1)>0);
    CHECK(strncmp(ps_completion_insertion(&q,ps_completion_at(first)),"len",3)==0);
    CHECK(ps_completion_prepare("Vec3.len",8,8,true,&q)==PS_OK && !includes(&q,"Vec3.length"));
    CHECK(ps_completion_prepare("Mat4.trans",10,10,true,&q)==PS_OK && includes(&q,"Mat4.translation"));
    CHECK(ps_completion_prepare("value.trans",11,11,true,&q)==PS_OK && !includes(&q,"Mat4.translation"));
    CHECK(ps_completion_prepare("velocity.norm",13,13,true,&q)==PS_OK && includes(&q,"Vec3.normalized"));
    CHECK(ps_completion_prepare("text.sp",7,7,true,&q)==PS_OK && includes(&q,"String.split"));
    CHECK(ps_completion_prepare("array.app",9,9,true,&q)==PS_OK && includes(&q,"array.append"));
    CHECK(ps_completion_prepare("ps_",3,3,false,&q)==PS_OK);
    size_t limited[2]={0,987654};CHECK(ps_completion_matches(&q,limited,1)>100 && limited[1]==987654);
    CHECK(ps_completion_matches(&q,NULL,0)>100 && ps_completion_matches(&q,NULL,1)==0);
    const char *invalid[]={"// ps_v","/* ps_v","\"ps_v","'ps_v","ctx.ps_v","ctx->ps_v","123ps_v","αps_v"};
    for(size_t i=0;i<sizeof invalid/sizeof *invalid;i++) {
        ps_completion_query before;memset(&before,0xa5,sizeof before);q=before;
        CHECK(ps_completion_prepare(invalid[i],strlen(invalid[i]),strlen(invalid[i]),false,&q)==PS_INVALID && !memcmp(&q,&before,sizeof q));
    }
    const char *nested="/* outer /* inner */ ps_v";
    CHECK(ps_completion_prepare(nested,strlen(nested),strlen(nested),true,&q)==PS_INVALID);
    const char *escaped="\"escaped \\\" ps_v";
    CHECK(ps_completion_prepare(escaped,strlen(escaped),strlen(escaped),true,&q)==PS_INVALID);
    const char *after="\"ps_v\" ps_vl";
    CHECK(ps_completion_prepare(after,strlen(after),strlen(after),false,&q)==PS_OK && includes(&q,"ps_vlength"));
    CHECK(ps_completion_prepare("α",2,1,false,&q)==PS_INVALID);
    CHECK(ps_completion_prepare("x",1,2,false,&q)==PS_INVALID && ps_completion_prepare(NULL,0,0,false,&q)==PS_INVALID);
    char long_word[260];memset(long_word,'a',sizeof long_word);
    ps_completion_query before=q;
    CHECK(ps_completion_prepare(long_word,sizeof long_word,sizeof long_word,false,&q)==PS_LIMIT && !memcmp(&q,&before,sizeof q));
    CHECK(ps_completion_prepare("",0,0,false,&q)==PS_OK && ps_completion_matches(&q,NULL,0)>600);
    /* The source preview uses the generated catalog ordering for binary lookup. */
    for(size_t i=1;i<ps_completion_count();i++){
        const ps_completion_symbol *a=ps_completion_at(i-1),*b=ps_completion_at(i);
        CHECK(a->language<=b->language);
        if(a->language==b->language)CHECK(strcmp(a->name,b->name)<0);
    }
    for(size_t i=0;i<ps_completion_count();i++){
        const ps_completion_symbol *s=ps_completion_at(i);if(s->language)continue;size_t offset=987;
        CHECK(ps_completion_c_reference(s->name,strlen(s->name),&offset)==s && offset==0);
    }
    const struct {const char *source,*symbol;} references[]={
        {"// ps_vlength(x)\nps_scene_add_id(scene,1,shape,a,b,1,0);","ps_scene_add_id"},
        {"/* ps_vlength(x) */ ps_scene_add_id /* real call */ (scene);","ps_scene_add_id"},
        {"const char *s=\"ps_vlength(x)\"; ps_scene_add_id(scene);","ps_scene_add_id"},
        {"'\\\''; 'ps_vlength'; ps_scene_add_id(scene);","ps_scene_add_id"},
        {"// hidden \\\nps_vlength(x)\nps_scene_add_id(scene);","ps_scene_add_id"},
        {"// hidden \\\r\nps_vlength(x)\r\nps_scene_add_id(scene);","ps_scene_add_id"},
        {"ps_result result; ps_scene_add_id(scene);","ps_scene_add_id"},
        {"value . /* member */ ps_vlength(x); ps_scene_add_id(scene);","ps_scene_add_id"},
        {"value->ps_vlength(x); ps_scene_add_id(scene);","ps_scene_add_id"},
        {"αps_vlength(x); ps_scene_add_id(scene);","ps_scene_add_id"},
        {"ps_vlengthα(x); ps_scene_add_id(scene);","ps_scene_add_id"},
        {"123ps_vlength(x); ps_scene_add_id(scene);","ps_scene_add_id"},
        {"ps_result result;","ps_result"},
        {"ps_vlength /* incomplete", "ps_vlength"}
    };
    for(size_t i=0;i<sizeof references/sizeof *references;i++){
        size_t offset=123;const ps_completion_symbol *s=ps_completion_c_reference(references[i].source,strlen(references[i].source),&offset);
        if(!s || strcmp(s->name,references[i].symbol))fprintf(stderr,"C reference case%zu: expected %s, got %s\n",i,references[i].symbol,s?s->name:"none");
        CHECK(s && !s->language && !strcmp(s->name,references[i].symbol));
        CHECK(!memcmp(references[i].source+offset,s->name,strlen(s->name)));
    }
    const char *no_reference[]={"","int local=1;","// ps_vlength(x)","/* ps_vlength(x)","\"ps_vlength(x)\"","ps_vlength_suffix(x)","ctx.ps_vlength(x)"};
    for(size_t i=0;i<sizeof no_reference/sizeof *no_reference;i++){size_t offset=876;CHECK(!ps_completion_c_reference(no_reference[i],strlen(no_reference[i]),&offset) && offset==876);}
    size_t offset=876;CHECK(!ps_completion_c_reference(NULL,0,&offset) && offset==876);
    const char bounded[]="ps_scene_add_id(x)";CHECK(!ps_completion_c_reference(bounded,5,&offset) && offset==876);
    puts("API completion: C/Physim symbols, members, UTF-8 boundaries, comments, literals, limits and failure preservation passed");
    return 0;
}
