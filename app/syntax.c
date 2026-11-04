#include "syntax.h"
#include "completion.h"
#include "language/lexer.h"
#include <string.h>

static bool space(char c){return c==' ' || c=='\t' || c=='\f' || c=='\v';}
static bool digit(char c){return c>='0' && c<='9';}
static bool ascii_start(char c){return (c>='a' && c<='z') || (c>='A' && c<='Z') || c=='_';}
static bool equal(const char *s,size_t n,const char *word){return strlen(word)==n && !memcmp(s,word,n);}
static bool listed(const char *s,size_t n,const char *const *list,size_t count){
    for(size_t i=0;i<count;i++)if(equal(s,n,list[i]))return true;
    return false;
}
static size_t identifier(const ps_syntax_scan *p,size_t at,bool first){
    if(at>=p->bytes)return 0;
    if(p->language)return ps_lang_identifier_width(p->source+at,p->bytes-at,first);
    return ascii_start(p->source[at]) || (!first && digit(p->source[at]))?1:0;
}
static size_t trivia(const ps_syntax_scan *p,size_t at){
    const char *s=p->source;size_t n=p->bytes;
    while(at<n){
        if(space(s[at]) || s[at]=='\r' || s[at]=='\n'){at++;continue;}
        if(at+1<n && s[at]=='/' && s[at+1]=='*'){
            size_t depth=1;at+=2;
            while(at<n && depth){
                if(at+1<n && s[at]=='*' && s[at+1]=='/'){at+=2;depth--;}
                else if(p->language && at+1<n && s[at]=='/' && s[at+1]=='*'){at+=2;depth++;}
                else at++;
            }
            continue;
        }
        if(at+1<n && s[at]=='/' && s[at+1]=='/'){
            at+=2;
            while(at<n && s[at]!='\n'){
                if(!p->language && s[at]=='\\' && at+1<n && s[at+1]=='\n')at+=2;
                else if(!p->language && s[at]=='\\' && at+2<n && s[at+1]=='\r' && s[at+2]=='\n')at+=3;
                else at++;
            }
            continue;
        }
        break;
    }
    return at;
}
ps_syntax_role ps_syntax_catalog_role(const char *name,size_t bytes,bool language){
    const ps_completion_symbol *symbol=ps_completion_lookup(name,bytes,language);
    return !symbol?PS_SYNTAX_TEXT:symbol->kind==PS_COMPLETION_FUNCTION?PS_SYNTAX_FUNCTION:
        symbol->kind==PS_COMPLETION_TYPE?PS_SYNTAX_TYPE:PS_SYNTAX_VARIABLE;
}
static ps_syntax_role word_role(ps_syntax_scan *p,size_t begin,size_t end){
    const char *s=p->source+begin;size_t n=end-begin;
    static const char *const control[]={"break","case","continue","default","do","else","for","goto","if","return","switch","while"};
    static const char *const c_words[]={"auto","char","const","double","enum","extern","float","inline","int","long","register","restrict","short","signed","sizeof","static","struct","typedef","union","unsigned","void","volatile","bool","true","false","NULL","_Alignas","_Alignof","_Atomic","_Bool","_Complex","_Generic","_Imaginary","_Noreturn","_Static_assert","_Thread_local"};
    /* Canonical C17 typedef spellings used by the SDK's stddef/stdint headers.
     * This is a lexical fallback, not evidence that an include was resolved. */
    static const char *const c_types[]={"size_t","ptrdiff_t","max_align_t","wchar_t","int8_t","int16_t","int32_t","int64_t","uint8_t","uint16_t","uint32_t","uint64_t","intptr_t","uintptr_t","intmax_t","uintmax_t","int_least8_t","int_least16_t","int_least32_t","int_least64_t","uint_least8_t","uint_least16_t","uint_least32_t","uint_least64_t","int_fast8_t","int_fast16_t","int_fast32_t","int_fast64_t","uint_fast8_t","uint_fast16_t","uint_fast32_t","uint_fast64_t"};
    static const char *const ph_types[]={
#include "language/builtin_type_names.inc"
    };
    if(p->directive){p->directive=false;return PS_SYNTAX_PREPROCESSOR;}
    if(p->language){
        ps_lang_lexer lexer;ps_lang_lexer_init(&lexer,s,n);ps_lang_token word=ps_lang_lexer_next(&lexer);
        switch(word.kind){
        case PS_LANG_RETURN:case PS_LANG_IF:case PS_LANG_GUARD:case PS_LANG_ELSE:case PS_LANG_WHILE:
        case PS_LANG_FOR:case PS_LANG_IN:case PS_LANG_BREAK:case PS_LANG_CONTINUE:case PS_LANG_CASE:
        case PS_LANG_SWITCH:case PS_LANG_DEFAULT:return PS_SYNTAX_CONTROL;
        default:break;
        }
        if(word.kind>=PS_LANG_LET && word.kind<=PS_LANG_NIL){
            p->type_name=word.kind==PS_LANG_STRUCT || word.kind==PS_LANG_ENUM;return PS_SYNTAX_KEYWORD;
        }
    }else if(listed(s,n,control,sizeof control/sizeof *control))return PS_SYNTAX_CONTROL;
    bool tag=equal(s,n,"struct") || equal(s,n,"enum") || (!p->language && equal(s,n,"union"));
    if(!p->language && listed(s,n,c_words,sizeof c_words/sizeof *c_words)){
        p->type_name=tag;return PS_SYNTAX_KEYWORD;
    }
    if(p->type_name){p->type_name=false;return PS_SYNTAX_TYPE;}
    if(p->resolve){ps_syntax_role role=p->resolve(p->user,begin,n);if(role>PS_SYNTAX_TEXT && role<PS_SYNTAX_COUNT)return role;}
    if(p->language && (equal(s,n,"self") || equal(s,n,"as")))return PS_SYNTAX_KEYWORD;
    if(!p->language && listed(s,n,c_types,sizeof c_types/sizeof *c_types))return PS_SYNTAX_TYPE;
    if(p->language && listed(s,n,ph_types,sizeof ph_types/sizeof *ph_types)){
        size_t after=trivia(p,end);
        return after<p->bytes && p->source[after]=='(' && ps_syntax_catalog_role(s,n,true)==PS_SYNTAX_FUNCTION?PS_SYNTAX_FUNCTION:PS_SYNTAX_TYPE;
    }
    ps_syntax_role role=ps_syntax_catalog_role(s,n,p->language);
    if(role!=PS_SYNTAX_TEXT)return role;
    if(p->language && begin && p->source[begin-1]=='.'){
        size_t first=begin-1;
        while(first && (ascii_start(p->source[first-1]) || digit(p->source[first-1]) || p->source[first-1]=='.'))first--;
        role=ps_syntax_catalog_role(p->source+first,end-first,true);
        if(role!=PS_SYNTAX_TEXT)return role;
    }
    size_t after=trivia(p,end);
    return after<p->bytes && p->source[after]=='('?PS_SYNTAX_FUNCTION:PS_SYNTAX_VARIABLE;
}
bool ps_syntax_init(ps_syntax_scan *p,const char *source,size_t bytes,bool language){
    if(!p || (!source && bytes))return false;
    *p=(ps_syntax_scan){.source=source?source:"",.bytes=bytes,.language=language,.line_start=true};return true;
}
bool ps_syntax_next(ps_syntax_scan *p,ps_syntax_token *out){
    if(!p || !out || p->offset>=p->bytes)return false;
    const char *s=p->source;size_t n=p->bytes,begin=p->offset,at=begin;
    ps_syntax_role role=PS_SYNTAX_TEXT;
    if(s[at]=='\r' || s[at]=='\n'){
        bool splice=!p->language && at && s[at-1]=='\\' &&
            (s[at]=='\n' || (at+1<n && s[at+1]=='\n'));
        if(s[at++]=='\r' && at<n && s[at]=='\n')at++;
        if(!splice){p->line_comment=false;p->quote=0;p->directive=false;p->line_start=true;}
    }else if(!p->triple_string && !p->quote && (p->block_depth || (at+1<n && s[at]=='/' && s[at+1]=='*'))){
        role=PS_SYNTAX_COMMENT;
        if(!p->block_depth){p->block_depth=1;at+=2;}
        while(at<n && s[at]!='\r' && s[at]!='\n'){
            if(p->language && at+1<n && s[at]=='/' && s[at+1]=='*'){p->block_depth++;at+=2;}
            else if(at+1<n && s[at]=='*' && s[at+1]=='/'){at+=2;if(!--p->block_depth)break;}
            else at++;
        }
    }else if(!p->triple_string && !p->quote && (p->line_comment || (at+1<n && s[at]=='/' && s[at+1]=='/'))){
        role=PS_SYNTAX_COMMENT;p->line_comment=true;
        while(at<n && s[at]!='\r' && s[at]!='\n')at++;
    }else if(p->triple_string || p->quote || s[at]=='"' || (!p->language && s[at]=='\'')){
        role=PS_SYNTAX_STRING;p->line_start=false;
        if(!p->triple_string && !p->quote){
            if(p->language && at+2<n && !memcmp(s+at,"\"\"\"",3)){p->triple_string=true;at+=3;}
            else p->quote=s[at++];
        }
        while(at<n && s[at]!='\r' && s[at]!='\n'){
            if(s[at]=='\\' && at+1<n && s[at+1]!='\r' && s[at+1]!='\n')at+=2;
            else if(p->triple_string && at+2<n && !memcmp(s+at,"\"\"\"",3)){at+=3;p->triple_string=false;break;}
            else if(!p->triple_string && s[at]==p->quote){at++;p->quote=0;break;}
            else at++;
        }
    }else if(space(s[at])){
        do{at++;}while(at<n && space(s[at]));
    }else if(!p->language && p->line_start && s[at]=='#'){
        role=PS_SYNTAX_PREPROCESSOR;p->directive=true;p->line_start=false;at++;
    }else if(identifier(p,at,true)){
        at+=identifier(p,at,true);
        size_t width;while((width=identifier(p,at,false))!=0)at+=width;
        role=word_role(p,begin,at);p->line_start=false;
    }else if(p->language && s[at]=='.' && at+1<n && s[at+1]=='.'){
        at+=2;if(at<n && (s[at]=='.' || s[at]=='<'))at++;
        p->line_start=false;p->type_name=false;
    }else if(digit(s[at]) || (s[at]=='.' && at+1<n && digit(s[at+1]))){
        role=PS_SYNTAX_NUMBER;at++;
        while(at<n){
            char c=s[at],previous=s[at-1];
            if(c=='.' && at+1<n && s[at+1]=='.')break;
            if(ascii_start(c) || digit(c) || c=='.' || ((c=='+' || c=='-') && (previous=='e' || previous=='E' || previous=='p' || previous=='P')))at++;
            else break;
        }
        p->line_start=false;
    }else{
        at++;while(at<n && ((unsigned char)s[at]&0xc0)==0x80)at++;
        p->line_start=false;p->type_name=false;
    }
    *out=(ps_syntax_token){begin,at,role};p->offset=at;return true;
}
