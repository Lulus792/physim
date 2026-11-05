#include "completion_locals.h"
#include "c_lexical.h"
#include "autosave.h"
#include <stdlib.h>
#include <string.h>

static bool whitespace(unsigned char c){return c==' ' || c=='\t' || c=='\r' || c=='\n' || c=='\f' || c=='\v';}
static size_t hash_name(const char *name,size_t bytes){
    uint64_t h=UINT64_C(14695981039346656037);
    for(size_t i=0;i<bytes;i++){h^=(unsigned char)name[i];h*=UINT64_C(1099511628211);}return (size_t)h;
}
static const char *binding_name(const char *source,ps_lang_editor_binding b,size_t *bytes){
    if(b.kind==PS_LANG_EDITOR_SELF){*bytes=4;return "self";}
    *bytes=b.name.length;return source+b.name.offset;
}
static void type_span(const char *source,ps_lang_editor_binding b,size_t *begin,size_t *end){
    *begin=b.type_start;*end=b.type_end;
    while(*begin<*end && whitespace((unsigned char)source[*begin]))(*begin)++;
    while(*end>*begin && whitespace((unsigned char)source[*end-1]))(*end)--;
}
static const char *binding_prefix(ps_lang_editor_binding b){
    return b.kind==PS_LANG_EDITOR_VARIABLE?(b.name.kind==PS_LANG_VAR?"var ":"let "):"";
}
void ps_completion_locals_destroy(ps_completion_locals *p){
    if(!p)return;
    ps_c_bindings_source_destroy(p->c_source);
    if(ps_allocator_valid(p->allocator)){
        if(p->source)ps_memory_free(p->allocator,p->source,p->bytes+1);
        if(p->strings)ps_memory_free(p->allocator,p->strings,p->string_bytes);
        if(p->entries)ps_memory_free(p->allocator,p->entries,p->count*sizeof *p->entries);
        if(p->slots)ps_memory_free(p->allocator,p->slots,p->slot_count*sizeof *p->slots);
    }
    memset(p,0,sizeof *p);
}
static int order(const void *left,const void *right){
    size_t a=((const ps_completion_local*)left)->id,b=((const ps_completion_local*)right)->id;
    return (a>b)-(a<b);
}
static char *copy_string(char **at,const char *text,size_t bytes){
    char *start=*at;memcpy(start,text,bytes);start[bytes]=0;*at+=bytes+1;return start;
}
static ps_lang_editor_binding raw_binding(const ps_lang_editor_binding *raw,const ps_c_binding *craw,size_t i){
    if(raw)return raw[i];
    ps_c_binding b=craw[i];
    return (ps_lang_editor_binding){.name={.kind=PS_LANG_IDENTIFIER,.offset=b.name_start,.length=b.name_end-b.name_start,.line=b.line,.column=b.column},
        .kind=b.parameter?PS_LANG_EDITOR_PARAMETER:PS_LANG_EDITOR_VARIABLE,.type_start=b.type_start,.type_end=b.type_end};
}
ps_result ps_completion_locals_update(ps_completion_locals *p,const void *owner,
    const char *source,size_t bytes,size_t cursor,bool language){
    if(!p || !owner || (!source && bytes) || cursor>bytes)return PS_INVALID;
    if(bytes>PS_SOURCE_MAX_BYTES)return PS_LIMIT;
    if((bytes && memchr(source,0,bytes)) || (cursor<bytes && ((unsigned char)source[cursor]&0xc0)==0x80))return PS_INVALID;
    ps_allocator a=p->allocator;
    if(!a.allocate && !a.deallocate)a=ps_allocator_default();
    if(!ps_allocator_valid(a))return PS_INVALID;
    bool same=p->language==language && p->owner==owner && p->bytes==bytes && p->source && (!bytes || !memcmp(source,p->source,bytes));
    if(same && p->cursor==cursor)return PS_OK;
    ps_completion_locals next={.allocator=a,.owner=owner,.bytes=bytes,.cursor=cursor,.language=language};
    bool borrowed_syntax=false;
    ps_lang_node *nodes=NULL;ps_lang_editor_binding *raw=NULL;unsigned char *chosen=NULL;
    ps_c_binding *craw=NULL;
    size_t *seen=NULL,raw_count=0,capacity=bytes<32767?bytes*2+2:65536,slots=0;
    ps_result r=ps_memory_allocate(a,bytes+1,(void**)&next.source);if(r!=PS_OK)goto done;
    if(bytes)memcpy(next.source,source,bytes);
    next.source[bytes]=0;
    if(language){
        r=ps_memory_zero(a,capacity,sizeof *nodes,(void**)&nodes);if(r!=PS_OK)goto done;
        r=ps_memory_zero(a,capacity,sizeof *raw,(void**)&raw);if(r!=PS_OK)goto done;
        ps_lang_editor_bindings_result parsed=ps_lang_editor_bindings(next.source,bytes,cursor,nodes,capacity,raw,capacity);
        next.status=parsed.status;next.call=parsed.call;raw_count=parsed.count;
    }else{
        r=ps_memory_zero(a,capacity,sizeof *craw,(void**)&craw);if(r!=PS_OK)goto done;
        ps_c_bindings_result parsed={.status=PS_OK};
        if(same && p->c_source){next.c_source=p->c_source;borrowed_syntax=true;}
        else parsed.status=ps_c_bindings_source_create(next.source,bytes,a,&next.c_source);
        if(parsed.status==PS_OK)parsed=ps_c_bindings_source_at(next.c_source,cursor,craw,capacity,a);
        if(parsed.status==PS_MEMORY){r=PS_MEMORY;goto done;}
        next.status=parsed.status==PS_LIMIT?PS_LANG_EDITOR_LIMIT:parsed.status!=PS_OK?PS_LANG_EDITOR_INVALID:parsed.has_error?PS_LANG_EDITOR_PREFIX:PS_LANG_EDITOR_OK;
        next.call=(ps_lang_editor_call){.open=parsed.call.open,.argument=parsed.call.argument,.active=parsed.call.active,.at_start=parsed.call.at_start,
            .callee_start=parsed.call.callee_start,.callee_end=parsed.call.callee_end,.name_start=parsed.call.name_start,.name_end=parsed.call.name_end,
            .receiver_start=parsed.call.receiver_start,.receiver_end=parsed.call.receiver_end,.member=parsed.call.member};
        raw_count=parsed.count;
    }
    if(!raw_count)goto publish;
    slots=1;while(slots<raw_count*2+1)slots*=2;
    r=ps_memory_zero(a,slots,sizeof *seen,(void**)&seen);if(r!=PS_OK)goto done;
    r=ps_memory_zero(a,raw_count,sizeof *chosen,(void**)&chosen);if(r!=PS_OK)goto done;
    for(size_t i=raw_count;i--;){
        ps_lang_editor_binding b=raw_binding(raw,craw,i);
        if(b.name.offset>bytes || b.name.length>bytes-b.name.offset || b.type_start>b.type_end || b.type_end>bytes){r=PS_INVALID;goto done;}
        size_t n=0;const char *name=binding_name(next.source,b,&n);size_t slot=hash_name(name,n)&(slots-1);
        while(seen[slot]){
            size_t other_n=0;const char *other=binding_name(next.source,raw_binding(raw,craw,seen[slot]-1),&other_n);
            if(n==other_n && !memcmp(name,other,n))break;
            slot=(slot+1)&(slots-1);
        }
        if(!seen[slot]){seen[slot]=i+1;chosen[i]=1;next.count++;}
    }
    r=ps_memory_zero(a,next.count,sizeof *next.entries,(void**)&next.entries);if(r!=PS_OK)goto done;
    for(size_t i=0;i<raw_count;i++)if(chosen[i]){
        ps_lang_editor_binding b=raw_binding(raw,craw,i);size_t n=0,begin,end;binding_name(next.source,b,&n);type_span(next.source,b,&begin,&end);
        size_t type=end-begin,prefix=strlen(binding_prefix(b)),need;
        if(language)need=2*n+2*type+prefix+(type?5:3);
        else{
            size_t tn=ps_c_binding_type(next.source,bytes,craw[i],NULL,0),sn=ps_c_binding_signature(next.source,bytes,craw[i],NULL,0);
            if(!tn || !sn){r=PS_INVALID;goto done;}need=n+1+tn+sn;
        }
        if(next.string_bytes>SIZE_MAX-need){r=PS_LIMIT;goto done;}next.string_bytes+=need;
    }
    r=ps_memory_allocate(a,next.string_bytes,(void**)&next.strings);if(r!=PS_OK)goto done;
    char *at=next.strings;size_t used=0,base=ps_completion_count()+1;
    for(size_t i=0;i<raw_count;i++)if(chosen[i]){
        ps_completion_local *entry=&next.entries[used++];size_t n=0,begin,end;
        ps_lang_editor_binding b=raw_binding(raw,craw,i);const char *name=binding_name(next.source,b,&n);type_span(next.source,b,&begin,&end);
        char *owned_name=copy_string(&at,name,n),*type,*signature;size_t signature_name=0;
        if(language){
            const char *prefix=binding_prefix(b);size_t pn=strlen(prefix),tn=end-begin;
            type=copy_string(&at,next.source+begin,tn);signature=at;
            signature_name=pn;
            memcpy(at,prefix,pn);at+=pn;memcpy(at,name,n);at+=n;
            if(tn){memcpy(at,": ",2);at+=2;memcpy(at,type,tn);at+=tn;}*at++=0;
        }else{
            size_t tn=ps_c_binding_type(next.source,bytes,craw[i],NULL,0),sn=ps_c_binding_signature(next.source,bytes,craw[i],NULL,0);
            type=at;ps_c_binding_type(next.source,bytes,craw[i],at,tn);at+=tn;
            signature=at;ps_c_binding_signature(next.source,bytes,craw[i],at,sn);at+=sn;
            size_t prefix_start=craw[i].declarator_start;
            while(prefix_start<craw[i].name_start && whitespace((unsigned char)next.source[prefix_start]))prefix_start++;
            signature_name=end-begin+1+craw[i].name_start-prefix_start;
        }
        if(b.name.offset>SIZE_MAX-base){r=PS_LIMIT;goto done;}
        *entry=(ps_completion_local){.symbol={owned_name,"Lokal",signature,PS_COMPLETION_VARIABLE,language,false},
            .declaration=b,.c_declaration=craw?craw[i]:(ps_c_binding){0},.id=base+b.name.offset,.signature_name_offset=signature_name,.type=type};
    }
    if((size_t)(at-next.strings)!=next.string_bytes){r=PS_INVALID;goto done;}
    qsort(next.entries,next.count,sizeof *next.entries,order);
    memset(seen,0,slots*sizeof *seen);
    for(size_t i=0;i<next.count;i++){
        const char *name=next.entries[i].symbol.name;size_t slot=hash_name(name,strlen(name))&(slots-1);
        while(seen[slot])slot=(slot+1)&(slots-1);
        seen[slot]=i+1;
    }
    next.slots=seen;next.slot_count=slots;seen=NULL;
publish:
    if(borrowed_syntax){p->c_source=NULL;borrowed_syntax=false;}
    ps_completion_locals_destroy(p);*p=next;memset(&next,0,sizeof next);
done:
    ps_memory_free(a,nodes,capacity*sizeof *nodes);ps_memory_free(a,raw,capacity*sizeof *raw);
    ps_memory_free(a,craw,capacity*sizeof *craw);
    ps_memory_free(a,chosen,raw_count*sizeof *chosen);ps_memory_free(a,seen,slots*sizeof *seen);
    if(borrowed_syntax)next.c_source=NULL;
    ps_completion_locals_destroy(&next);return r;
}
const ps_completion_local *ps_completion_locals_find(const ps_completion_locals *p,const char *name,size_t bytes){
    if(!p || !name || !bytes || !p->slot_count)return NULL;
    size_t slot=hash_name(name,bytes)&(p->slot_count-1);
    while(p->slots[slot]){
        const ps_completion_local *entry=&p->entries[p->slots[slot]-1];
        if(strlen(entry->symbol.name)==bytes && !memcmp(entry->symbol.name,name,bytes))return entry;
        slot=(slot+1)&(p->slot_count-1);
    }return NULL;
}
const ps_completion_local *ps_completion_locals_at(const ps_completion_locals *p,size_t id){
    if(!p)return NULL;
    size_t low=0,high=p->count;
    while(low<high){size_t middle=low+(high-low)/2;if(p->entries[middle].id<id)low=middle+1;else high=middle;}
    return low<p->count && p->entries[low].id==id?&p->entries[low]:NULL;
}
size_t ps_completion_locals_matches(const ps_completion_locals *p,const ps_completion_query *q,size_t *ids,size_t capacity){
    if(!p || !q || q->language!=p->language || q->member || (capacity && !ids))return 0;
    size_t count=0,prefix=strlen(q->prefix);
    for(size_t i=0;i<p->count;i++)if(!strncmp(p->entries[i].symbol.name,q->prefix,prefix)){
        if(count<capacity)ids[count]=p->entries[i].id;
        count++;
    }return count;
}
bool ps_completion_locals_argument_hint(const char *source,size_t bytes,size_t cursor,bool language){
    if((!source && bytes) || cursor>bytes || bytes>PS_SOURCE_MAX_BYTES)return false;
    if(!language){int last=0;return ps_c_lexical_cursor(source,bytes,cursor,&last) && (last=='(' || last==',');}
    ps_lang_lexer lexer;ps_lang_lexer_init(&lexer,source,cursor);ps_lang_token last={0};
    for(;;){ps_lang_token t=ps_lang_lexer_next(&lexer);if(t.kind==PS_LANG_ERROR)return false;if(t.kind==PS_LANG_EOF)break;if(t.kind!=PS_LANG_NEWLINE)last=t;}
    return last.kind==PS_LANG_LPAREN || last.kind==PS_LANG_COMMA || last.kind==PS_LANG_COLON;
}
