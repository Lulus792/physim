#include "completion.h"
#include "c_lexical.h"
#include "language/lexer.h"
#include <string.h>
static const ps_completion_symbol symbols[]={
#include "completion_symbols.inc"
};
size_t ps_completion_count(void){return sizeof symbols/sizeof *symbols;}
const ps_completion_symbol *ps_completion_at(size_t index){return index<ps_completion_count()?&symbols[index]:NULL;}
static bool word(unsigned char c){return (c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || c=='_';}
size_t ps_completion_identifier_width(const char *text,size_t bytes,bool first,bool language){
    if(!text || !bytes)return 0;
    if(!language && *text=='$')return 1;
    if(!language && bytes>=2 && text[0]=='\\' && (text[1]=='u'||text[1]=='U')){
        size_t n=text[1]=='u'?6:10;if(bytes<n)return 0;
        for(size_t i=2;i<n;i++)if(!((text[i]>='0'&&text[i]<='9')||(text[i]>='a'&&text[i]<='f')||(text[i]>='A'&&text[i]<='F')))return 0;
        return n;
    }
    return ps_lang_identifier_width(text,bytes,first);
}
static size_t word_before(const char *text,size_t at,bool language){
    if(!at)return at;
    if(!language){
        if(at>=10 && text[at-10]=='\\' && text[at-9]=='U' && ps_completion_identifier_width(text+at-10,10,false,false)==10)return at-10;
        if(at>=6 && text[at-6]=='\\' && text[at-5]=='u' && ps_completion_identifier_width(text+at-6,6,false,false)==6)return at-6;
    }
    size_t begin=at-1;while(begin && ((unsigned char)text[begin]&0xc0)==0x80)begin--;
    return ps_completion_identifier_width(text+begin,at-begin,false,language)==at-begin?begin:at;
}
ps_result ps_completion_prepare(const char *text,size_t bytes,size_t cursor,bool language,ps_completion_query *out) {
    if(!text || !out || cursor>bytes || (cursor<bytes && ((unsigned char)text[cursor]&0xc0)==0x80))return PS_INVALID;
    ps_completion_query q={.start=cursor,.end=cursor,.language=language};
    for(;;){size_t before=word_before(text,q.start,language);if(before==q.start)break;q.start=before;}
    while(q.end<bytes){size_t width=ps_completion_identifier_width(text+q.end,bytes-q.end,false,language);if(!width)break;q.end+=width;}
    if(q.start<cursor && !ps_completion_identifier_width(text+q.start,cursor-q.start,true,language))return PS_INVALID;
    if(q.start<cursor && text[q.start]>='0' && text[q.start]<='9')return PS_INVALID;
    if(cursor-q.start>=sizeof q.prefix)return PS_LIMIT;
    memcpy(q.prefix,text+q.start,cursor-q.start);
    if(!language && !ps_c_lexical_cursor(text,bytes,cursor,NULL))return PS_INVALID;
    unsigned block=0;bool line=false;char quote=0;
    if(language){
    for(size_t i=0;i<q.start;i++) {
        char c=text[i],next=i+1<q.start?text[i+1]:0;
        if(line){
            /* C splices a physical newline before recognizing // comments.
             * Physim line comments do not have that C continuation rule. */
            if(!language && c=='\\' && next=='\n'){i++;continue;}
            if(!language && c=='\\' && next=='\r' && i+2<q.start && text[i+2]=='\n'){i+=2;continue;}
            if(c=='\n')line=false;
            continue;
        }
        if(block){if(c=='*' && next=='/'){block--;i++;}else if(language && c=='/' && next=='*'){block++;i++;}continue;}
        if(quote){if(c=='\\' && next)i++;else if(c==quote)quote=0;continue;}
        if(c=='/' && next=='/'){line=true;i++;}
        else if(c=='/' && next=='*'){block=1;i++;}
        else if(language && c=='"'){
            /* One real language token covers multiline delimiters, escapes and
             * nested interpolation strings. Read the bounded full source so a
             * caret inside an opening/closing delimiter is not mistaken for
             * a completed empty string in a truncated prefix. */
            ps_lang_lexer literal;ps_lang_lexer_init(&literal,text+i,bytes-i);
            ps_lang_token token=ps_lang_lexer_next(&literal);
            if(token.kind!=PS_LANG_STRING || !token.length || token.length>q.start-i)return PS_INVALID;
            i+=token.length-1;
        }
        else if(c=='"' || (!language && c=='\''))quote=c;
    }
    if(block || line || quote)return PS_INVALID;
    }
    if(q.start && text[q.start-1]=='.') {
        if(!language)return PS_INVALID;
        q.member=true;size_t begin=q.start-1;
        for(;;){size_t before=word_before(text,begin,true);if(before==begin)break;begin=before;}
        size_t length=q.start-1-begin;
        if(length>=sizeof q.receiver)return PS_LIMIT;
        memcpy(q.receiver,text+begin,length);
    } else if(!language && q.start>=2 && text[q.start-2]=='-' && text[q.start-1]=='>')return PS_INVALID;
    *out=q;return PS_OK;
}
const char *ps_completion_insertion(const ps_completion_query *q,const ps_completion_symbol *s) {
    if(!q || !s)return NULL;
    const char *dot=q->member?strrchr(s->name,'.'):NULL;
    return dot?dot+1:s->name;
}
size_t ps_completion_matches(const ps_completion_query *q,size_t *indices,size_t capacity) {
    if(!q || (capacity && !indices))return 0;
    bool known_receiver=false;size_t receiver=strlen(q->receiver),prefix=strlen(q->prefix),count=0;
    if(q->member && receiver)for(size_t i=0;i<ps_completion_count();i++) {
        const ps_completion_symbol *s=&symbols[i];
        if(s->language && !strcmp(s->name,q->receiver) &&
           (s->kind==PS_COMPLETION_TYPE || (s->name[0]>='A' && s->name[0]<='Z'))){known_receiver=true;break;}
    }
    for(size_t i=0;i<ps_completion_count();i++) {
        const ps_completion_symbol *s=&symbols[i];
        if(s->language!=q->language)continue;
        if(q->language && q->member!= (strchr(s->name,'.')!=NULL))continue;
        if(known_receiver && (strncmp(s->name,q->receiver,receiver) || s->name[receiver]!='.'))continue;
        if(q->language && q->member && s->instance==known_receiver)continue;
        const char *insert=ps_completion_insertion(q,s);
        if(strncmp(insert,q->prefix,prefix))continue;
        if(count<capacity)indices[count]=i;
        count++;
    }
    return count;
}
static bool completion_space(unsigned char c){return c==' ' || c=='\t' || c=='\r' || c=='\n' || c=='\v' || c=='\f';}
/* The generated catalog is ordered by (language,name). Search the C prefix
 * without allocating a temporary token or walking every documented symbol. */
const ps_completion_symbol *ps_completion_lookup(const char *text,size_t n,bool language) {
    if(!text || !n)return NULL;
    size_t low=0,high=ps_completion_count();
    while(low<high){size_t middle=low+(high-low)/2;const ps_completion_symbol *s=&symbols[middle];
        size_t length=strlen(s->name),common=length<n?length:n;
        int order=s->language!=language?(s->language?1:-1):memcmp(s->name,text,common);
        if(!order)order=length<n?-1:length>n?1:0;
        if(order<0)low=middle+1;else high=middle;}
    if(low==ps_completion_count() || symbols[low].language!=language)return NULL;
    const ps_completion_symbol *s=&symbols[low];return strlen(s->name)==n && !memcmp(s->name,text,n)?s:NULL;
}
static size_t c_reference_trivia(const char *text,size_t n,size_t at) {
    while(at<n){
        if(completion_space((unsigned char)text[at])){at++;continue;}
        if(at+1<n && text[at]=='/' && text[at+1]=='*'){
            at+=2;while(at+1<n && !(text[at]=='*' && text[at+1]=='/'))at++;
            if(at+1>=n)return n;
            at+=2;continue;}
        if(at+1<n && text[at]=='/' && text[at+1]=='/'){
            at+=2;while(at<n && text[at]!='\n'){
                if(text[at]=='\\' && at+1<n && text[at+1]=='\n'){at+=2;continue;}
                if(text[at]=='\\' && at+2<n && text[at+1]=='\r' && text[at+2]=='\n'){at+=3;continue;}at++;}
            continue;}
        break;
    }return at;
}
const ps_completion_symbol *ps_completion_c_reference(const char *text,size_t bytes,size_t *offset) {
    if(!text)return NULL;
    const ps_completion_symbol *fallback=NULL;size_t fallback_at=0,at=0;unsigned char previous=0,before_previous=0;
    while(at<bytes){
        size_t next=c_reference_trivia(text,bytes,at);if(next!=at){at=next;continue;}
        unsigned char c=(unsigned char)text[at];
        if(c=='"' || c=='\''){
            unsigned char quote=c;at++;
            while(at<bytes){c=(unsigned char)text[at++];if(c=='\\' && at<bytes)at++;else if(c==quote)break;}
            before_previous=0;previous=quote;continue;
        }
        if(word(c)){
            size_t begin=at;while(at<bytes && word((unsigned char)text[at]))at++;
            bool boundary=(!begin || (unsigned char)text[begin-1]<0x80) && (at==bytes || (unsigned char)text[at]<0x80);
            bool member=previous=='.' || (previous=='>' && before_previous=='-') || (previous==':' && before_previous==':');
            const ps_completion_symbol *s=boundary && !(c>='0' && c<='9') && !member?ps_completion_lookup(text+begin,at-begin,false):NULL;
            if(s){
                if(!fallback){fallback=s;fallback_at=begin;}
                size_t call=c_reference_trivia(text,bytes,at);
                if(s->kind==PS_COMPLETION_FUNCTION && call<bytes && text[call]=='('){if(offset)*offset=begin;return s;}
            }
            before_previous=0;previous=(unsigned char)text[at-1];continue;
        }
        before_previous=previous;previous=c;at++;
    }
    if(fallback && offset)*offset=fallback_at;
    return fallback;
}
