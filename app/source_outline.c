#include "source_outline.h"
#include "language/parser.h"
#include <stdlib.h>
#include <string.h>

typedef struct {size_t offset,length,line;int kind;} token;
typedef struct {const char *s;size_t n,at,line;bool incomplete;} scanner;
enum { IDENT=256,LITERAL };
static bool id_start(unsigned char c){return c=='_' || (c>='a' && c<='z') || (c>='A' && c<='Z') || c>=128;}
static bool id_part(unsigned char c){return id_start(c) || (c>='0' && c<='9');}
static bool is(const char *s,token t,const char *word){return strlen(word)==t.length && !memcmp(s+t.offset,word,t.length);}
static bool type_word(const char *s,token t) {
    static const char *words[]={"void","char","short","int","long","float","double","signed","unsigned","_Bool","_Complex","const","volatile","restrict","static","extern","inline","_Noreturn","_Atomic","__attribute__","__declspec","_Alignas"};
    for(size_t i=0;i<sizeof words/sizeof *words;i++)if(is(s,t,words[i]))return true;
    return false;
}
static token next(scanner *p) {
    while(p->at<p->n) {
        size_t at=p->at;unsigned char c=(unsigned char)p->s[at],d=at+1<p->n?(unsigned char)p->s[at+1]:0;
        if(c==' ' || c=='\t' || c=='\r' || c=='\n' || c=='\f' || c=='\v'){p->at++;if(c=='\n')p->line++;continue;}
        if(c=='#') { /* Preprocessor logical lines are not declarations. */
            do {c=(unsigned char)p->s[p->at++];if(c=='\n'){p->line++;size_t before=p->at-1;if(before && p->s[before-1]=='\r')before--;if(!before || p->s[before-1]!='\\')break;}}while(p->at<p->n);
            continue;
        }
        if(c=='/' && d=='/'){p->at+=2;while(p->at<p->n && p->s[p->at]!='\n')p->at++;continue;}
        if(c=='/' && d=='*') {
            p->at+=2;bool closed=false;
            while(p->at<p->n){c=(unsigned char)p->s[p->at++];if(c=='\n')p->line++;if(c=='*' && p->at<p->n && p->s[p->at]=='/'){p->at++;closed=true;break;}}
            if(!closed)p->incomplete=true;continue;
        }
        token t={at,1,p->line,c};p->at++;
        if(id_start(c)){while(p->at<p->n && id_part((unsigned char)p->s[p->at]))p->at++;t.kind=IDENT;}
        else if(c=='"' || c=='\'') {
            bool closed=false;char quote=(char)c;
            while(p->at<p->n){c=(unsigned char)p->s[p->at++];if(c=='\n')p->line++;if(c=='\\' && p->at<p->n){if(p->s[p->at++]=='\n')p->line++;}else if(c==quote){closed=true;break;}}
            if(!closed)p->incomplete=true;t.kind=LITERAL;
        }
        t.length=p->at-at;return t;
    }
    return (token){p->at,0,p->line,0};
}
static void add(ps_source_outline *out,token t,ps_outline_kind kind) {
    if(!t.length)return;
    if(out->count==PS_OUTLINE_LIMIT){out->status=PS_OUTLINE_LIMITED;return;}
    out->symbols[out->count++]=(ps_outline_symbol){t.offset,t.length,t.line,kind};
}
static void c_outline(const char *source,size_t bytes,ps_source_outline *out) {
    scanner p={source,bytes,0,1,false};token previous={0},name={0},function={0},tag={0},pointer={0};
    unsigned braces=0,parens=0,brackets=0;bool alias=false,initializer=false,body=false,expect_tag=false;
    for(token t=next(&p);t.kind;t=next(&p)) {
        if(braces) {
            if(t.kind=='{')braces++;
            else if(t.kind=='}' && !--braces && body){name=function=tag=pointer=(token){0};alias=initializer=body=expect_tag=false;}
            previous=t;continue;
        }
        if(t.kind=='(') {
            if(parens==1 && !alias && !initializer && pointer.length && previous.offset==pointer.offset){function=pointer;pointer=(token){0};}
            if(!parens && !brackets && !initializer && !alias && !function.length && previous.kind==IDENT && !type_word(source,previous))function=previous;
            parens++;
        } else if(t.kind==')'){if(parens)parens--;else out->status=PS_OUTLINE_INCOMPLETE;}
        else if(t.kind=='[')brackets++;
        else if(t.kind==']'){if(brackets)brackets--;else out->status=PS_OUTLINE_INCOMPLETE;}
        else if(t.kind==IDENT && !initializer) {
            if(!parens && !brackets) {
                if(is(source,t,"typedef"))alias=true;
                else if(is(source,t,"struct") || is(source,t,"union") || is(source,t,"enum")){expect_tag=true;tag=(token){0};}
                else if(!type_word(source,t)) {name=t;if(expect_tag){tag=t;expect_tag=false;}}
            } else if(parens==1 && previous.kind=='*' && !function.length)pointer=t;
        } else if(!parens && !brackets) {
            if(t.kind=='{') {
                if(function.length && !initializer){add(out,function,PS_OUTLINE_FUNCTION);body=true;}
                else if(tag.length && !initializer){add(out,tag,PS_OUTLINE_TYPE);name=(token){0};tag=(token){0};}
                braces=1;
            } else if(t.kind=='=' && !initializer) {
                add(out,pointer.length?pointer:name,alias?PS_OUTLINE_TYPE:PS_OUTLINE_BINDING);initializer=true;
            } else if(t.kind==';' || t.kind==',') {
                if(!initializer) {
                    if(function.length && !pointer.length)add(out,function,PS_OUTLINE_FUNCTION);
                    else if(pointer.length || (name.length && name.offset!=tag.offset))add(out,pointer.length?pointer:name,alias?PS_OUTLINE_TYPE:PS_OUTLINE_BINDING);
                }
                name=function=pointer=tag=(token){0};initializer=expect_tag=false;if(t.kind==';')alias=false;
            } else if(t.kind=='}')out->status=PS_OUTLINE_INCOMPLETE;
        }
        previous=t;
    }
    if(p.incomplete || braces || parens || brackets || name.length || function.length) {
        if(out->status!=PS_OUTLINE_LIMITED)out->status=PS_OUTLINE_INCOMPLETE;
        out->diagnostic_line=p.line;
    }
}
static void language_outline(const char *source,size_t bytes,ps_source_outline *out) {
    size_t capacity=bytes<32767?bytes*2+2:65536;
    ps_lang_node *nodes=calloc(capacity,sizeof *nodes);
    if(!nodes){out->status=PS_OUTLINE_NO_MEMORY;return;}
    ps_lang_parse_result parsed=ps_lang_parse(source,bytes,nodes,capacity);
    if(!parsed.root){out->status=parsed.count>=capacity?PS_OUTLINE_LIMITED:PS_OUTLINE_INCOMPLETE;out->diagnostic_line=parsed.diagnostic.line;}
    else for(size_t i=nodes[parsed.root].a;i;i=nodes[i].next) {
        ps_lang_node n=nodes[i];ps_outline_kind kind;
        if(n.kind==PS_AST_FUNCTION || n.kind==PS_AST_GENERIC_FUNCTION)kind=PS_OUTLINE_FUNCTION;
        else if(n.kind==PS_AST_VARIABLE)kind=PS_OUTLINE_BINDING;
        else if(n.kind==PS_AST_STRUCT || n.kind==PS_AST_ENUM || n.kind==PS_AST_GENERIC_STRUCT || n.kind==PS_AST_GENERIC_ENUM)kind=PS_OUTLINE_TYPE;
        else continue;
        add(out,(token){n.token.offset,n.token.length,n.token.line,IDENT},kind);
    }
    free(nodes);
}
void ps_source_outline_build(const char *source,size_t bytes,bool language,ps_source_outline *out) {
    if(!out)return;
    memset(out,0,sizeof *out);
    if(bytes>PS_OUTLINE_SOURCE_LIMIT){out->status=PS_OUTLINE_LIMITED;return;}
    if((!source && bytes) || (source && memchr(source,0,bytes))){out->status=PS_OUTLINE_INVALID;return;}
    if(language)language_outline(source,bytes,out);else c_outline(source,bytes,out);
}
