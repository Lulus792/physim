#include "signature_help.h"
#include "c_signatures.h"
#include "autosave.h"
#include "language/builtins.h"
#include <string.h>

#define SIGNATURE_TEXT_MAX ((size_t)1024*1024)
typedef struct {
    const char *name,*definition,*owner,*text;
    ps_lang_type owner_type,result;
    size_t first,count,name_begin,name_end;
} signature_registry_row;
#include "completion_signature_registry.inc"

static bool whitespace(unsigned char c){return c==' '||c=='\t'||c=='\n'||c=='\r'||c=='\f'||c=='\v';}
static void trim(const char *s,size_t *a,size_t *b){while(*a<*b && whitespace((unsigned char)s[*a]))(*a)++;while(*b>*a && whitespace((unsigned char)s[*b-1]))(*b)--;}
static bool equal(const char *s,size_t a,size_t b,const char *name,size_t n){return a<=b && b-a==n && !memcmp(s+a,name,n);}
static void display_free(ps_allocator a,ps_signature_display *d){if(d)ps_memory_free(a,d,d->allocation_bytes);}
void ps_signature_help_destroy(ps_signature_help *p){
    if(!p)return;
    if(ps_allocator_valid(p->allocator)){
        ps_memory_free(p->allocator,p->source,p->bytes+1);
        ps_memory_free(p->allocator,p->functions,p->function_count*sizeof *p->functions);
        ps_memory_free(p->allocator,p->parameters,p->parameter_count*sizeof *p->parameters);
        display_free(p->allocator,p->display);
    }
    memset(p,0,sizeof *p);
}
static bool same_document(const ps_signature_help *p,const ps_completion_locals *l){
    return p && l && p->owner && p->owner==l->owner && p->source && l->source &&
        p->language==l->language && p->bytes==l->bytes && !memcmp(p->source,l->source,l->bytes);
}
bool ps_signature_help_matches(const ps_signature_help *p,const ps_completion_locals *l){return same_document(p,l) && p->cursor==l->cursor;}
static bool same_target(ps_lang_editor_call a,ps_lang_editor_call b){
    return a.active && b.active && a.open==b.open && a.member==b.member &&
        a.callee_start==b.callee_start && a.callee_end==b.callee_end &&
        a.name_start==b.name_start && a.name_end==b.name_end &&
        a.receiver_start==b.receiver_start && a.receiver_end==b.receiver_end;
}
static ps_result display_new(ps_allocator a,size_t count,size_t bytes,ps_signature_display **out){
    if(bytes>SIGNATURE_TEXT_MAX || count>(SIZE_MAX-sizeof(ps_signature_display)-bytes-1)/sizeof(ps_signature_parameter))return PS_LIMIT;
    size_t allocation=sizeof(ps_signature_display)+count*sizeof(ps_signature_parameter)+bytes+1;
    ps_signature_display *d=NULL;ps_result r=ps_memory_zero(a,1,allocation,(void**)&d);if(r!=PS_OK)return r;
    d->parameters=(ps_signature_parameter*)(d+1);d->text=(char*)(d->parameters+count);
    d->count=count;d->text_bytes=bytes;d->allocation_bytes=allocation;*out=d;return PS_OK;
}
typedef struct {char *text;size_t bytes;ps_result status;} writer;
static void append(writer *w,const char *s,size_t n){
    if(w->status!=PS_OK)return;
    if(n>SIGNATURE_TEXT_MAX-w->bytes){w->status=PS_LIMIT;return;}
    if(w->text && s && n)memcpy(w->text+w->bytes,s,n);
    w->bytes+=n;
}
static void binding_append(writer *w,const char *s,size_t bytes,ps_c_binding b,size_t *name){
    size_t n=ps_c_binding_signature(s,bytes,b,NULL,0),a=b.type_start,z=b.type_end,d=b.declarator_start;
    if(!n){w->status=PS_INVALID;return;}
    trim(s,&a,&z);while(d<b.name_start && whitespace((unsigned char)s[d]))d++;
    *name=w->bytes+(z-a)+1+b.name_start-d;
    size_t at=w->bytes;append(w,NULL,n-1);
    if(w->text && w->status==PS_OK)ps_c_binding_signature(s,bytes,b,w->text+at,n);
}
static ps_result c_display(ps_allocator a,const char *s,size_t bytes,ps_c_function f,const ps_c_parameter *params,ps_signature_display **out){
    ps_c_binding b=f.declaration;size_t begin=b.type_start,end=b.type_end,d=b.declarator_start,z=b.declarator_end;
    trim(s,&begin,&end);while(d<b.name_start && whitespace((unsigned char)s[d]))d++;
    while(z>b.name_end && whitespace((unsigned char)s[z-1]))z--;
    if(f.parameters_start<d || f.parameters_end>z || f.parameters_start>=f.parameters_end)return PS_INVALID;
    ps_signature_display *display=NULL;ps_result r=PS_OK;
    for(unsigned pass=0;pass<2;pass++){
        writer w={.text=display?display->text:NULL,.status=PS_OK};
        append(&w,s+begin,end-begin);append(&w," ",1);
        if(display){display->name_begin=w.bytes+b.name_start-d;display->name_end=w.bytes+b.name_end-d;display->unspecified=f.unspecified;}
        append(&w,s+d,f.parameters_start-d);append(&w,"(",1);
        for(size_t i=0;i<f.parameter_count;i++){
            if(i)append(&w,", ",2);
            ps_c_parameter p=params[i];size_t start=w.bytes,name_start=0,name_end=0;
            if(p.type_missing || !p.declaration.name_end){
                size_t x=p.start,y=p.end;trim(s,&x,&y);append(&w,s+x,y-x);
                if(p.type_missing){name_start=start+p.declaration.name_start-p.start;name_end=name_start+p.declaration.name_end-p.declaration.name_start;}
            }else{binding_append(&w,s,bytes,p.declaration,&name_start);name_end=name_start+p.declaration.name_end-p.declaration.name_start;}
            if(display)display->parameters[i]=(ps_signature_parameter){start,w.bytes,name_start,name_end,p.type_missing};
        }
        if(f.variadic_end>f.variadic_start){
            if(f.parameter_count)append(&w,", ",2);
            if(display)display->variadic_begin=w.bytes;
            append(&w,s+f.variadic_start,f.variadic_end-f.variadic_start);
            if(display)display->variadic_end=w.bytes;
        }else if(!f.parameter_count)append(&w,s+f.parameters_start+1,f.parameters_end-f.parameters_start-2);
        append(&w,")",1);append(&w,s+f.parameters_end,z-f.parameters_end);
        if(w.status!=PS_OK){r=w.status;goto done;}
        if(!pass){r=display_new(a,f.parameter_count,w.bytes,&display);if(r!=PS_OK)goto done;}
        else{if(w.bytes!=display->text_bytes){r=PS_INVALID;goto done;}display->text[w.bytes]=0;}
    }
    *out=display;display=NULL;
done:display_free(a,display);return r;
}
static ps_result c_selected(ps_allocator a,const ps_c_bindings_source *syntax,const char *text,size_t bytes,size_t cursor,const char *name,size_t n,
                            ps_c_lookup_result initial,ps_signature_display **out){
    ps_c_parameter *params=NULL;ps_result r=PS_OK;
    if(initial.parameter_count){
        r=ps_memory_zero(a,initial.parameter_count,sizeof *params,(void**)&params);if(r!=PS_OK)return r;
        ps_c_lookup_result full=ps_c_bindings_source_lookup(syntax,cursor,name,n,params,initial.parameter_count,a);
        if(full.status!=PS_OK){r=full.status;goto done;}
        if(!full.callable || full.parameter_count!=initial.parameter_count || full.declaration.name_start!=initial.declaration.name_start){r=PS_INVALID;goto done;}
        initial=full;
    }
    r=c_display(a,text,bytes,initial.function,params,out);
done:ps_memory_free(a,params,initial.parameter_count*sizeof *params);return r;
}
static const signature_registry_row *registry_find(const char *name){
    for(size_t i=0;i<sizeof registry_rows/sizeof *registry_rows;i++)if(!strcmp(registry_rows[i].name,name))return &registry_rows[i];
    return NULL;
}
static bool registry_valid(const signature_registry_row *row){
    const ps_lang_builtin *b=NULL;size_t binding=0,receiver=SIZE_MAX;
    const char *dot=strchr(row->name,'.');
    if(row->owner[0]){
        const ps_lang_method *method=dot?ps_lang_method_find(row->owner_type,dot+1,strlen(dot+1),&binding):NULL;
        if(method){b=ps_lang_builtin_get(binding);receiver=method->receiver&PS_LANG_METHOD_RECEIVER_MASK;}
    }else if(dot)b=ps_lang_static_method_find(row->name,(size_t)(dot-row->name),dot+1,strlen(dot+1),&binding);
    else b=ps_lang_builtin_find(row->name,strlen(row->name),&binding);
    if(!b || strcmp(b->name,row->definition) || b->result!=row->result || b->count!=row->count+(receiver!=SIZE_MAX))return false;
    size_t p=0;
    for(size_t i=0;i<b->count;i++)if(i!=receiver){
        ps_signature_parameter span=registry_parameters[row->first+p];
        if(b->types[i]!=registry_types[row->first+p] || !b->labels[i] ||
           !equal(row->text,span.name_begin,span.name_end,b->labels[i],strlen(b->labels[i])))return false;
        p++;
    }
    return p==row->count;
}
static ps_result api_display(ps_signature_help *next,const ps_signature_help *previous,const ps_completion_symbol *symbol,bool *borrowed){
    if(!symbol || symbol->kind!=PS_COMPLETION_FUNCTION){next->state=PS_SIGNATURE_UNKNOWN;return PS_OK;}
    next->api_symbol=symbol;
    if(previous->state==PS_SIGNATURE_READY && previous->api_symbol==symbol && previous->display){next->display=previous->display;*borrowed=true;next->state=PS_SIGNATURE_READY;return PS_OK;}
    ps_result r=PS_OK;
    if(symbol->language){
        const signature_registry_row *row=registry_find(symbol->name);
        if(!row || !registry_valid(row)){next->state=PS_SIGNATURE_UNKNOWN;return PS_OK;}
        r=display_new(next->allocator,row->count,strlen(row->text),&next->display);if(r!=PS_OK)return r;
        memcpy(next->display->text,row->text,strlen(row->text)+1);
        memcpy(next->display->parameters,registry_parameters+row->first,row->count*sizeof *next->display->parameters);
        next->display->name_begin=row->name_begin;next->display->name_end=row->name_end;
    }else{
        ps_c_bindings_source *syntax=NULL;size_t bytes=strlen(symbol->signature),n=strlen(symbol->name);
        r=ps_c_bindings_source_create(symbol->signature,bytes,next->allocator,&syntax);
        if(r==PS_OK){
            ps_c_lookup_result q=ps_c_bindings_source_lookup(syntax,bytes,symbol->name,n,NULL,0,next->allocator);
            r=q.status;
            if(r==PS_OK && q.callable)r=c_selected(next->allocator,syntax,symbol->signature,bytes,bytes,symbol->name,n,q,&next->display);
        }
        ps_c_bindings_source_destroy(syntax);
        if(r==PS_INVALID){next->state=PS_SIGNATURE_UNKNOWN;return PS_OK;}
        if(r!=PS_OK)return r;
    }
    next->state=next->display?PS_SIGNATURE_READY:PS_SIGNATURE_UNKNOWN;return PS_OK;
}
static ps_result ph_headers(ps_signature_help *next,ps_lang_node *nodes,size_t capacity,bool module,
                             ps_lang_editor_function **functions,size_t *fc,ps_lang_editor_parameter **parameters,size_t *pc){
    size_t bytes=module?next->bytes:next->cursor;
    ps_lang_editor_signatures_result q=module?ps_lang_editor_module_signatures(next->source,bytes,nodes,capacity,NULL,0,NULL,0):ps_lang_editor_signatures(next->source,bytes,nodes,capacity,NULL,0,NULL,0);
    if(q.status==PS_LANG_EDITOR_LIMIT)return PS_LIMIT;
    if(q.status!=PS_LANG_EDITOR_OK && q.status!=PS_LANG_EDITOR_PREFIX)return PS_INVALID;
    *fc=q.function_count;*pc=q.parameter_count;ps_result r=PS_OK;
    if(*fc){r=ps_memory_zero(next->allocator,*fc,sizeof **functions,(void**)functions);if(r!=PS_OK)return r;}
    if(*pc){r=ps_memory_zero(next->allocator,*pc,sizeof **parameters,(void**)parameters);if(r!=PS_OK)return r;}
    if(*fc || *pc){
        q=module?ps_lang_editor_module_signatures(next->source,bytes,nodes,capacity,*functions,*fc,*parameters,*pc):ps_lang_editor_signatures(next->source,bytes,nodes,capacity,*functions,*fc,*parameters,*pc);
        if(q.status==PS_LANG_EDITOR_LIMIT)return PS_LIMIT;
        if((q.status!=PS_LANG_EDITOR_OK && q.status!=PS_LANG_EDITOR_PREFIX) || q.function_count!=*fc || q.parameter_count!=*pc)return PS_INVALID;
    }
    return PS_OK;
}
static bool binding_equal(const char *source,ps_lang_editor_binding b,const char *name,size_t n){
    return b.kind==PS_LANG_EDITOR_SELF?(n==4 && !memcmp(name,"self",4)):(b.name.length==n && !memcmp(source+b.name.offset,name,n));
}
static ps_result ph_display(ps_signature_help *next,ps_lang_editor_function f,const ps_lang_editor_parameter *parameters){
    if(f.header_start>f.name_start || f.name_end>f.header_end || f.header_end>next->bytes)return PS_INVALID;
    ps_result r=display_new(next->allocator,f.parameter_count,f.header_end-f.header_start,&next->display);if(r!=PS_OK)return r;
    ps_signature_display *d=next->display;
    memcpy(d->text,next->source+f.header_start,d->text_bytes);d->text[d->text_bytes]=0;
    d->name_begin=f.name_start-f.header_start;d->name_end=f.name_end-f.header_start;
    for(size_t i=0;i<f.parameter_count;i++){
        ps_lang_editor_parameter p=parameters[f.parameter_first+i];
        if(p.name_start<f.header_start || p.type_end>f.header_end)return PS_INVALID;
        d->parameters[i]=(ps_signature_parameter){p.name_start-f.header_start,p.type_end-f.header_start,p.name_start-f.header_start,p.name_end-f.header_start,false};
    }
    next->declaration=f.name_start;next->state=PS_SIGNATURE_READY;return PS_OK;
}
static ps_result ph_select(ps_signature_help *next,const ps_signature_help *previous,bool *borrowed_display){
    ps_allocator a=next->allocator;size_t capacity=next->bytes<32767?next->bytes*2+2:65536;
    ps_lang_node *nodes=NULL;ps_lang_editor_binding *bindings=NULL;
    ps_lang_editor_function *local=NULL;ps_lang_editor_parameter *params=NULL;size_t fc=0,pc=0;
    ps_result r=ps_memory_zero(a,capacity,sizeof *nodes,(void**)&nodes);if(r!=PS_OK)goto done;
    if(!next->module_ready){
        r=ph_headers(next,nodes,capacity,true,&next->functions,&next->function_count,&next->parameters,&next->parameter_count);
        if(r!=PS_OK)goto done;
        next->module_ready=true;
    }
    r=ph_headers(next,nodes,capacity,false,&local,&fc,&params,&pc);if(r!=PS_OK)goto done;
    r=ps_memory_zero(a,capacity,sizeof *bindings,(void**)&bindings);if(r!=PS_OK)goto done;
    size_t point=next->call.name_start;const char *name=next->source+point;size_t n=next->call.name_end-point;
    ps_lang_editor_bindings_result scope=ps_lang_editor_bindings(next->source,next->bytes,point,nodes,capacity,bindings,capacity);
    if(scope.status==PS_LANG_EDITOR_LIMIT){r=PS_LIMIT;goto done;}
    if(scope.status!=PS_LANG_EDITOR_OK && scope.status!=PS_LANG_EDITOR_PREFIX){r=PS_INVALID;goto done;}
    const char *owner=NULL;size_t owner_bytes=0;bool instance=false,masked=false;
    ps_lang_editor_binding binding={0};bool has_binding=false;
    const char *target=name;size_t target_bytes=n;
    if(next->call.member){
        target=next->source+next->call.receiver_start;target_bytes=next->call.receiver_end-next->call.receiver_start;
        size_t at=0;
        while(at<target_bytes){size_t width=ps_completion_identifier_width(target+at,target_bytes-at,at==0,true);if(!width)break;at+=width;}
        if(!target_bytes || at!=target_bytes)goto done;
        owner=target;owner_bytes=target_bytes;
    }
    for(size_t i=0;i<scope.count;i++)if(binding_equal(next->source,bindings[i],target,target_bytes)){binding=bindings[i];has_binding=true;}
    if(next->call.member && has_binding){
        size_t x=binding.type_start,y=binding.type_end;trim(next->source,&x,&y);
        if(x==y)goto done;
        owner=next->source+x;owner_bytes=y-x;instance=true;
        size_t at=0;
        while(at<owner_bytes){size_t width=ps_completion_identifier_width(owner+at,owner_bytes-at,at==0,true);if(!width)break;at+=width;}
        if(!at || (at<owner_bytes && owner[at]!='<'))goto done;
        owner_bytes=at;
    }
    const ps_lang_editor_function *best=NULL;const ps_lang_editor_parameter *best_params=NULL;bool ambiguous=false;
    for(unsigned pass=0;pass<2;pass++){
        const ps_lang_editor_function *functions=pass?local:next->functions;
        const ps_lang_editor_parameter *entries=pass?params:next->parameters;size_t count=pass?fc:next->function_count;
        for(size_t i=0;i<count;i++){
            const ps_lang_editor_function *f=&functions[i];
            if(pass && !f->scope_depth)continue; /* Full document owns module declarations once. */
            if(!equal(next->source,f->name_start,f->name_end,name,n) || point<f->visible_from || point<f->scope_start || point>=f->scope_end)continue;
            if(next->call.member){if(!f->method || !equal(next->source,f->owner_start,f->owner_end,owner,owner_bytes) || (instance?(f->method==3):(f->method!=3)))continue;}
            else if(f->method)continue;
            if(!best || f->scope_depth>best->scope_depth){best=f;best_params=entries;ambiguous=false;}
            else if(f->scope_depth==best->scope_depth)ambiguous=true;
        }
    }
    if(!next->call.member && has_binding)masked=!best || !best->scope_depth || binding.name.offset>best->name_start;
    if(masked){next->state=PS_SIGNATURE_MASKED;goto done;}
    if(ambiguous){next->state=PS_SIGNATURE_AMBIGUOUS;goto done;}
    if(best){r=ph_display(next,*best,best_params);goto done;}
    if(next->call.member){
        /* Match the explicit receiver annotation/type spelling to a catalog
         * owner, never guess a type from the variable's name or initializer. */
        for(size_t i=0;i<ps_completion_count();i++){
            const ps_completion_symbol *s=ps_completion_at(i);const char *dot=strchr(s->name,'.');
            if(s->language && s->kind==PS_COMPLETION_FUNCTION && dot && s->instance==instance &&
               (size_t)(dot-s->name)==owner_bytes && !memcmp(s->name,owner,owner_bytes)){
                /* name is a source span, not NUL-terminated. */
                if(strlen(dot+1)==n && !memcmp(dot+1,name,n)){r=api_display(next,previous,s,borrowed_display);break;}
            }
        }
    }else r=api_display(next,previous,ps_completion_lookup(name,n,true),borrowed_display);
done:
    ps_memory_free(a,nodes,capacity*sizeof *nodes);ps_memory_free(a,bindings,capacity*sizeof *bindings);
    ps_memory_free(a,local,fc*sizeof *local);ps_memory_free(a,params,pc*sizeof *params);
    return r;
}
ps_result ps_signature_help_update(ps_signature_help *p,const ps_completion_locals *l){
    if(!p || !l || !l->owner || !l->source || l->cursor>l->bytes || l->bytes>PS_SOURCE_MAX_BYTES)return PS_INVALID;
    if(ps_signature_help_matches(p,l))return PS_OK;
    ps_allocator a=p->allocator;if(!a.allocate && !a.deallocate)a=ps_allocator_default();if(!ps_allocator_valid(a))return PS_INVALID;
    ps_signature_help next={.allocator=a,.owner=l->owner,.bytes=l->bytes,.cursor=l->cursor,.language=l->language,.call=l->call,.active_parameter=SIZE_MAX,.declaration=SIZE_MAX};
    bool borrow_source=same_document(p,l),borrow_meta=borrow_source && p->module_ready,borrow_display=false;
    ps_result r=PS_OK;
    if(borrow_source)next.source=p->source;
    else{r=ps_memory_allocate(a,l->bytes+1,(void**)&next.source);if(r!=PS_OK)goto done;memcpy(next.source,l->source,l->bytes+1);}
    if(borrow_meta){next.functions=p->functions;next.parameters=p->parameters;next.function_count=p->function_count;next.parameter_count=p->parameter_count;next.module_ready=true;}
    if(l->status!=PS_LANG_EDITOR_OK && l->status!=PS_LANG_EDITOR_PREFIX){next.state=PS_SIGNATURE_INVALID;goto publish;}
    if(!next.call.active)goto publish;
    next.state=PS_SIGNATURE_UNKNOWN;
    if(next.call.name_start>=next.call.name_end || next.call.name_end>next.bytes || next.call.receiver_start>next.call.receiver_end || next.call.receiver_end>next.bytes)goto publish;
    /* Resolution belongs to the immutable callee location. Merely moving among
     * arguments cannot change it; a different inner call still queries afresh. */
    if(borrow_source && p->state!=PS_SIGNATURE_INVALID && same_target(p->call,next.call)){
        next.state=p->state;next.declaration=p->declaration;next.api_symbol=p->api_symbol;
        next.display=p->display;borrow_display=true;goto publish;
    }
    if(next.language)r=ph_select(&next,p,&borrow_display);
    else if(!next.call.member && l->c_source){
        const char *name=next.source+next.call.name_start;size_t n=next.call.name_end-next.call.name_start;
        ps_c_lookup_result q=ps_c_bindings_source_lookup(l->c_source,next.call.name_start,name,n,NULL,0,a);r=q.status;
        if(r==PS_OK){
            if(q.found){next.state=PS_SIGNATURE_MASKED;next.declaration=q.declaration.name_start;
                if(q.callable){r=c_selected(a,l->c_source,next.source,next.bytes,next.call.name_start,name,n,q,&next.display);if(r==PS_OK)next.state=PS_SIGNATURE_READY;}}
            else r=api_display(&next,p,ps_completion_lookup(name,n,false),&borrow_display);
        }
    }
    if(r==PS_INVALID){next.state=PS_SIGNATURE_INVALID;display_free(a,next.display);next.display=NULL;r=PS_OK;}
    if(r!=PS_OK)goto done;
publish:
    if(next.state==PS_SIGNATURE_READY && next.display && !next.display->unspecified && next.call.argument<next.display->count)next.active_parameter=next.call.argument;
    if(borrow_source)p->source=NULL;
    if(borrow_meta){p->functions=NULL;p->parameters=NULL;}
    if(borrow_display)p->display=NULL;
    ps_signature_help_destroy(p);*p=next;memset(&next,0,sizeof next);
done:
    if(borrow_source)next.source=NULL;
    if(borrow_meta){next.functions=NULL;next.parameters=NULL;}
    if(borrow_display)next.display=NULL;
    ps_signature_help_destroy(&next);return r;
}
