#include "completion.h"
#include <string.h>
static const ps_completion_symbol symbols[]={
#include "completion_symbols.inc"
};
size_t ps_completion_count(void){return sizeof symbols/sizeof *symbols;}
const ps_completion_symbol *ps_completion_at(size_t index){return index<ps_completion_count()?&symbols[index]:NULL;}
static bool word(unsigned char c){return (c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || c=='_';}
ps_result ps_completion_prepare(const char *text,size_t bytes,size_t cursor,bool language,ps_completion_query *out) {
    if(!text || !out || cursor>bytes || (cursor<bytes && ((unsigned char)text[cursor]&0xc0)==0x80))return PS_INVALID;
    ps_completion_query q={.start=cursor,.end=cursor,.language=language};
    while(q.start && word((unsigned char)text[q.start-1]))q.start--;
    while(q.end<bytes && word((unsigned char)text[q.end]))q.end++;
    if(q.start && (unsigned char)text[q.start-1]>=0x80)return PS_INVALID;
    if(q.start<cursor && text[q.start]>='0' && text[q.start]<='9')return PS_INVALID;
    if(cursor-q.start>=sizeof q.prefix)return PS_LIMIT;
    memcpy(q.prefix,text+q.start,cursor-q.start);
    unsigned block=0;bool line=false;char quote=0;
    for(size_t i=0;i<q.start;i++) {
        char c=text[i],next=i+1<q.start?text[i+1]:0;
        if(line){if(c=='\n')line=false;continue;}
        if(block){if(c=='*' && next=='/'){block--;i++;}else if(language && c=='/' && next=='*'){block++;i++;}continue;}
        if(quote){if(c=='\\' && next)i++;else if(c==quote)quote=0;continue;}
        if(c=='/' && next=='/'){line=true;i++;}
        else if(c=='/' && next=='*'){block=1;i++;}
        else if(c=='"' || (!language && c=='\''))quote=c;
    }
    if(block || line || quote)return PS_INVALID;
    if(q.start && text[q.start-1]=='.') {
        if(!language)return PS_INVALID;
        q.member=true;size_t begin=q.start-1;
        while(begin && word((unsigned char)text[begin-1]))begin--;
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
static const ps_completion_symbol *c_reference_symbol(const char *text,size_t n) {
    size_t low=0,high=ps_completion_count();
    while(low<high){size_t middle=low+(high-low)/2;const ps_completion_symbol *s=&symbols[middle];
        int order=s->language?1:strncmp(s->name,text,n);if(!order && s->name[n])order=1;
        if(order<0)low=middle+1;else high=middle;}
    if(low==ps_completion_count() || symbols[low].language)return NULL;
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
            const ps_completion_symbol *s=boundary && !(c>='0' && c<='9') && !member?c_reference_symbol(text+begin,at-begin):NULL;
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
