#include "syntax.h"
#include "completion.h"
#include "language/lexer.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Syntax line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static bool role_at(const char *source,bool language,const char *needle,unsigned occurrence,ps_syntax_role expected){
    const char *where=source;
    do{where=strstr(where,needle);if(!where)return false;if(occurrence)where++;}while(occurrence--);
    size_t offset=(size_t)(where-source);ps_syntax_scan scan;ps_syntax_token token;
    if(!ps_syntax_init(&scan,source,strlen(source),language))return false;
    while(ps_syntax_next(&scan,&token))if(token.begin<=offset && offset<token.end)return token.role==expected;
    return false;
}
static bool partition(const char *source,size_t bytes,bool language){
    ps_syntax_scan scan;if(!ps_syntax_init(&scan,source,bytes,language))return false;
    ps_syntax_token token;size_t at=0,steps=0;
    while(ps_syntax_next(&scan,&token)){
        if(token.begin!=at || token.end<=at || token.end>bytes || token.role>=PS_SYNTAX_COUNT)return false;
        for(size_t i=token.begin;i<token.end;i++)if(source[i]=='\n' || source[i]=='\r'){
            if(token.role!=PS_SYNTAX_TEXT || i!=token.begin ||
               (token.end-token.begin!=1 && !(token.end-token.begin==2 && source[i]=='\r' && source[i+1]=='\n')))return false;
            break;
        }
        at=token.end;if(++steps>bytes)return false;
    }
    ps_syntax_token sentinel={9,11,PS_SYNTAX_FUNCTION};token=sentinel;
    return at==bytes && !ps_syntax_next(&scan,&token) && !memcmp(&token,&sentinel,sizeof token);
}
static ps_syntax_role local_role(void *user,size_t offset,size_t bytes){
    const char *source=user;
    return (bytes==5 && !memcmp(source+offset,"ps_v3",5)) || (bytes==6 && !memcmp(source+offset,"size_t",6))?PS_SYNTAX_VARIABLE:PS_SYNTAX_TEXT;
}
int main(void){
    for(size_t i=0;i<ps_completion_count();i++){
        const ps_completion_symbol *s=ps_completion_at(i);size_t n=strlen(s->name);
        CHECK(ps_completion_lookup(s->name,n,s->language)==s);
        CHECK(ps_syntax_catalog_role(s->name,n,s->language)==(s->kind==PS_COMPLETION_FUNCTION?PS_SYNTAX_FUNCTION:s->kind==PS_COMPLETION_TYPE?PS_SYNTAX_TYPE:PS_SYNTAX_VARIABLE));
    }
    CHECK(!ps_completion_lookup(NULL,9,false) && !ps_completion_lookup("",0,false));
    const char embedded[]={'p','s','_','v','3',0,'x'};
    CHECK(!ps_completion_lookup(embedded,sizeof embedded,false));
    CHECK(!ps_completion_lookup("ps_v3more",9,false) && !ps_completion_lookup("ps_v",4,false));
    CHECK(!ps_completion_lookup("ps_v3",5,true) && !ps_completion_lookup("Vec3",4,false));
    const char *c="#include \"physim/core.h\"\nvoid step(double dt) {\n ps_vec3 velocity = ps_v3(0.0, -9.81e-2, 0x1p+2);\n ps_unknown = velocity; if (dt) return;\n step /* retained */ (dt);\n}\n";
    CHECK(partition(c,strlen(c),false));
    CHECK(role_at(c,false,"include",0,PS_SYNTAX_PREPROCESSOR));
    CHECK(role_at(c,false,"physim/core.h",0,PS_SYNTAX_STRING));
    CHECK(role_at(c,false,"void",0,PS_SYNTAX_KEYWORD));
    CHECK(role_at(c,false,"step",0,PS_SYNTAX_FUNCTION) && role_at(c,false,"step",1,PS_SYNTAX_FUNCTION));
    CHECK(role_at(c,false,"ps_vec3",0,PS_SYNTAX_TYPE) && role_at(c,false,"ps_v3",0,PS_SYNTAX_FUNCTION));
    CHECK(role_at(c,false,"velocity",0,PS_SYNTAX_VARIABLE) && role_at(c,false,"ps_unknown",0,PS_SYNTAX_VARIABLE));
    CHECK(role_at(c,false,"if",0,PS_SYNTAX_CONTROL) && role_at(c,false,"return",0,PS_SYNTAX_CONTROL));
    CHECK(role_at(c,false,"9.81e-2",0,PS_SYNTAX_NUMBER) && role_at(c,false,"0x1p+2",0,PS_SYNTAX_NUMBER));
    CHECK(role_at("size_t count; uint64_t bits; max_align_t aligned;",false,"size_t",0,PS_SYNTAX_TYPE));
    CHECK(role_at("size_t count; uint64_t bits; max_align_t aligned;",false,"uint64_t",0,PS_SYNTAX_TYPE));
    CHECK(role_at("size_t count; uint64_t bits; max_align_t aligned;",false,"max_align_t",0,PS_SYNTAX_TYPE));
    const char *continuation="// continued" "\\" "\r\nps_v3(1);\nps_v3(2);";
    CHECK(partition(continuation,strlen(continuation),false));
    CHECK(role_at(continuation,false,"ps_v3",0,PS_SYNTAX_COMMENT) && role_at(continuation,false,"ps_v3",1,PS_SYNTAX_FUNCTION));
    CHECK(role_at(continuation,true,"ps_v3",0,PS_SYNTAX_FUNCTION));
    const char *quoted="const char *s=\"string" "\\" "\n/* ps_v3 */\"; ps_v3(0);";
    CHECK(partition(quoted,strlen(quoted),false));
    CHECK(role_at(quoted,false,"ps_v3",0,PS_SYNTAX_STRING) && role_at(quoted,false,"ps_v3",1,PS_SYNTAX_FUNCTION));
    const char *ph="func step(Δt: Float64) { let velocity = Vec3(0, 1, 0); /* outer /* inner */ still */ if true { return velocity } }\nlet text = \"\"\"line\n/* // not comments */\nVec3\n\"\"\"; step(Δt)";
    CHECK(partition(ph,strlen(ph),true));
    CHECK(role_at(ph,true,"Float64",0,PS_SYNTAX_TYPE) && role_at(ph,true,"Vec3",0,PS_SYNTAX_FUNCTION));
    CHECK(role_at(ph,true,"Δt",0,PS_SYNTAX_VARIABLE) && role_at(ph,true,"still",0,PS_SYNTAX_COMMENT));
    CHECK(role_at(ph,true,"not comments",0,PS_SYNTAX_STRING) && role_at(ph,true,"Vec3",1,PS_SYNTAX_STRING));
    CHECK(role_at("func f(v: Vec3) -> Vec3 { return v }",true,"Vec3",0,PS_SYNTAX_TYPE));
    CHECK(role_at("func f(v: Vec3) -> Vec3 { return v }",true,"Vec3",1,PS_SYNTAX_TYPE));
    CHECK(role_at("let method = Mat4.translation",true,"translation",0,PS_SYNTAX_FUNCTION));
    CHECK(role_at("let member = velocity.translation",true,"translation",0,PS_SYNTAX_VARIABLE));
    CHECK(role_at("guard ready else { return } goto",true,"guard",0,PS_SYNTAX_CONTROL));
    CHECK(role_at("guard ready else { return } goto",true,"goto",0,PS_SYNTAX_VARIABLE));
    {const char *ranges[]={"1...5","0..<3"};
    for(unsigned i=0;i<2;i++){
        ps_syntax_scan range;ps_syntax_token op;CHECK(ps_syntax_init(&range,ranges[i],5,true));
        CHECK(ps_syntax_next(&range,&op) && op.begin==0 && op.end==1 && op.role==PS_SYNTAX_NUMBER);
        CHECK(ps_syntax_next(&range,&op) && op.begin==1 && op.end==4 && op.role==PS_SYNTAX_TEXT);
        CHECK(ps_syntax_next(&range,&op) && op.begin==4 && op.end==5 && op.role==PS_SYNTAX_NUMBER);
        ps_lang_lexer actual;ps_lang_lexer_init(&actual,ranges[i],5);
        CHECK(ps_lang_lexer_next(&actual).kind==PS_LANG_INTEGER);
        CHECK(ps_lang_lexer_next(&actual).kind==(i?PS_LANG_RANGE_OPEN:PS_LANG_RANGE_CLOSED));
    }}
    for(size_t n=0;n<=strlen(ph);n++)CHECK(partition(ph,n,true));
    for(size_t n=0;n<=strlen(c);n++)CHECK(partition(c,n,false));
    ps_syntax_scan scan;ps_syntax_token token;CHECK(ps_syntax_init(&scan,"ps_v3(0)",8,false));
    scan.resolve=local_role;scan.user=(void*)scan.source;
    CHECK(ps_syntax_next(&scan,&token) && token.role==PS_SYNTAX_VARIABLE);
    CHECK(ps_syntax_init(&scan,"size_t",6,false));scan.resolve=local_role;scan.user=(void*)scan.source;
    CHECK(ps_syntax_next(&scan,&token) && token.role==PS_SYNTAX_VARIABLE);
    ps_syntax_scan saved=scan;CHECK(!ps_syntax_init(&scan,NULL,1,false) && !memcmp(&scan,&saved,sizeof scan));
    const char invalid[]={0,(char)0xff,(char)0x80,'x','\r','\n','"','/'};
    CHECK(partition(invalid,sizeof invalid,false) && partition(invalid,sizeof invalid,true));
    puts("Shared syntax: exact catalog roles, C/Physim, declaration override, line continuations, nested comments, multiline strings, Unicode, all prefix partitions and failure preservation passed");
    return 0;
}
