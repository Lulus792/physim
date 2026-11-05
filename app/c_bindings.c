#include "c_bindings.h"
#include "c_lexical.h"
#include "autosave.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    size_t end,scope,mate,group,previous,next;
    bool bad,legacy_parameter;
} index_entry;
typedef struct {
    const char *source;
    size_t bytes,cursor,count,used,capacity;
    ps_c_node *nodes;
    index_entry *index;
    ps_c_binding *output;
} collector;
static bool is(const collector *c,size_t i,const char *type){return i<c->count && !strcmp(c->nodes[i].type,type);}
static bool field(const collector *c,size_t i,const char *name){return i<c->count && c->nodes[i].field && !strcmp(c->nodes[i].field,name);}
static bool space(unsigned char ch){return ch==' '||ch=='\t'||ch=='\n'||ch=='\r'||ch=='\v'||ch=='\f';}
static size_t child(const collector *c,size_t parent,const char *name){
    for(size_t i=parent+1;i<c->index[parent].end;i=c->index[i].end)if(field(c,i,name))return i;
    return SIZE_MAX;
}
static size_t next_token(const collector *c,size_t node){
    size_t end=c->index[node].end;
    return end<c->count?c->index[end].next:SIZE_MAX;
}
static bool contains(const collector *c,size_t open){
    return open==SIZE_MAX || (c->nodes[open].end<=c->cursor &&
        (c->index[open].mate==SIZE_MAX || c->cursor<=c->nodes[c->index[open].mate].start));
}
static bool opener(const collector *c,size_t i){return is(c,i,"(")||is(c,i,"[")||is(c,i,"{");}
static bool closer(const collector *c,size_t i){return is(c,i,")")||is(c,i,"]")||is(c,i,"}");}
static bool pair(const collector *c,size_t a,size_t b){return (is(c,a,"(")&&is(c,b,")"))||(is(c,a,"[")&&is(c,b,"]"))||(is(c,a,"{")&&is(c,b,"}"));}
static ps_result build_index(collector *c){
    for(size_t i=0;i<c->count;i++)c->index[i]=(index_entry){.end=i+1,.scope=SIZE_MAX,.mate=SIZE_MAX,.group=SIZE_MAX,.previous=SIZE_MAX,.next=SIZE_MAX,.bad=c->nodes[i].error||c->nodes[i].missing};
    for(size_t i=c->count;i-->1;){
        size_t p=c->nodes[i].parent;if(p>=i)return PS_INVALID;
        c->index[p].end=c->index[i].end>c->index[p].end?c->index[i].end:c->index[p].end;
        c->index[p].bad|=c->index[i].bad;
    }
    size_t next=SIZE_MAX;
    for(size_t i=c->count;i--;){
        if(c->index[i].end==i+1 && !c->nodes[i].missing && !is(c,i,"comment"))next=i;
        c->index[i].next=next;
    }
    size_t group=SIZE_MAX,scope=SIZE_MAX,previous=SIZE_MAX;
    for(size_t i=0;i<c->count;i++){
        c->index[i].scope=scope;c->index[i].group=group;c->index[i].previous=previous;
        if(c->index[i].end!=i+1 || c->nodes[i].missing || is(c,i,"comment"))continue;
        if(opener(c,i)){
            group=i;if(is(c,i,"{"))scope=i;
        }else if(closer(c,i)){
            if(group==SIZE_MAX || !pair(c,group,i))return PS_INVALID;
            c->index[group].mate=i;c->index[i].mate=group;
            if(is(c,i,"}"))scope=c->index[group].scope;
            group=c->index[group].group;
        }
        previous=i;
    }
    return PS_OK;
}
/* Follow only actual declarator grammar edges. The operation nearest the name
 * distinguishes a function declaration from an object pointer to a function. */
static size_t name_node(const collector *c,size_t declarator,size_t *operation){
    size_t i=declarator;*operation=SIZE_MAX;
    while(i<c->count){
        if(is(c,i,"identifier"))return i;
        size_t next=child(c,i,"declarator");
        if(next==SIZE_MAX && is(c,i,"parenthesized_declarator")){
            for(size_t n=i+1;n<c->index[i].end;n=c->index[n].end)if(c->nodes[n].named && !is(c,n,"comment")){next=n;break;}
        }
        if(next==SIZE_MAX)return SIZE_MAX;
        if(!is(c,i,"parenthesized_declarator") && !is(c,i,"attributed_declarator"))*operation=i;
        i=next;
    }
    return SIZE_MAX;
}
static ps_result append(collector *c,size_t declaration,size_t declarator,size_t base_end,bool parameter){
    if(c->index[declarator].bad)return PS_OK;
    size_t operation,name=name_node(c,declarator,&operation);
    if(name==SIZE_MAX || (!parameter && is(c,operation,"function_declarator")) || c->nodes[name].end>c->cursor)return PS_OK;
    if(c->used>=c->capacity)return PS_LIMIT;
    ps_c_node n=c->nodes[name],d=c->nodes[declarator];
    c->output[c->used++]=(ps_c_binding){.name_start=n.start,.name_end=n.end,.line=n.line,.column=n.column,
        .type_start=c->nodes[declaration].start,.type_end=base_end,
        .declarator_start=d.start,.declarator_end=d.end,.parameter=parameter};
    return PS_OK;
}
static bool loop_visible(const collector *c,size_t declaration){
    if(!field(c,declaration,"initializer"))return true;
    size_t parent=c->nodes[declaration].parent;
    if(is(c,parent,"for_statement"))return c->cursor<c->nodes[parent].end ||
        (c->cursor==c->nodes[parent].end && c->index[parent].bad);
    /* ERROR recovery retains initializer and real for-header punctuation. */
    size_t group=c->index[declaration].group;
    if(!is(c,group,"(") || !is(c,c->index[group].previous,"for"))return false;
    size_t close=c->index[group].mate;
    if(close==SIZE_MAX || c->cursor<=c->nodes[close].start)return true;
    size_t body=next_token(c,close);
    if(body==SIZE_MAX)return c->cursor>=c->nodes[close].end;
    if(is(c,body,"{"))return contains(c,body);
    /* A complete single-statement body has a real for_statement even when its
     * enclosing function is ERROR. Otherwise only an unfinished current body
     * is admissible; never step past a real terminating statement token. */
    for(size_t i=body;i<c->count;){
        if(c->nodes[i].start>=c->cursor)break;
        if(is(c,i,";") || is(c,i,"}"))return false;
        if(is(c,i,"(") && c->index[i].mate!=SIZE_MAX)i=c->index[i].mate+1;
        else i=c->index[i].end;
    }
    return body<c->count && c->nodes[body].start<=c->cursor;
}
static ps_result declaration(collector *c,size_t d){
    if(c->index[d].legacy_parameter || is(c,c->nodes[d].parent,"function_definition"))return PS_OK;
    if(!contains(c,c->index[d].scope) || !loop_visible(c,d))return PS_OK;
    size_t first=child(c,d,"declarator");if(first==SIZE_MAX)return PS_OK;
    for(size_t i=d+1;i<first;i=c->index[i].end)if(c->index[i].bad)return PS_OK;
    size_t base_end=c->nodes[first].start;
    for(size_t i=first;i<c->index[d].end;i=c->index[i].end)if(field(c,i,"declarator")){
        size_t delimiter=next_token(c,i),actual=i;
        if(!is(c,delimiter,",") && !is(c,delimiter,";"))continue;
        if(c->nodes[delimiter].end>c->cursor || c->index[i].bad)continue;
        if(is(c,i,"init_declarator"))actual=child(c,i,"declarator");
        if(actual==SIZE_MAX)continue;
        ps_result r=append(c,d,actual,base_end,false);if(r!=PS_OK)return r;
    }
    return PS_OK;
}
static ps_result parameters(collector *c,size_t outer,size_t body){
    size_t function,name=name_node(c,outer,&function);
    if(name==SIZE_MAX || !is(c,function,"function_declarator"))return PS_OK;
    size_t params=child(c,function,"parameters");if(params==SIZE_MAX)return PS_OK;
    /* Grammar-recognized old-style parameter declarations occur between the
     * function declarator and body. They must never become file-scope locals. */
    for(size_t i=c->index[outer].end;i<body;i=c->index[i].end)if(is(c,i,"declaration")){
        c->index[i].legacy_parameter=true;
        if(!contains(c,body))continue;
        size_t first=child(c,i,"declarator");if(first==SIZE_MAX || c->index[i].bad)continue;
        for(size_t d=first;d<c->index[i].end;d=c->index[d].end)if(field(c,d,"declarator")){
            ps_result r=append(c,i,d,c->nodes[first].start,true);if(r!=PS_OK)return r;
        }
    }
    if(!contains(c,body))return PS_OK;
    for(size_t i=params+1;i<c->index[params].end;i=c->index[i].end)if(is(c,i,"parameter_declaration")){
        size_t d=child(c,i,"declarator");if(d==SIZE_MAX || c->index[i].bad)continue;
        ps_result r=append(c,i,d,c->nodes[d].start,true);if(r!=PS_OK)return r;
    }
    return PS_OK;
}
static size_t definition_body(const collector *c,size_t declarator){
    size_t i=c->index[declarator].end;
    while(i<c->count && (is(c,i,"declaration")||is(c,i,"comment")||is(c,i,"attribute_specifier")))i=c->index[i].end;
    return i<c->count?c->index[i].next:SIZE_MAX;
}
static bool specifier(const collector *c,size_t i){
    return is(c,i,"primitive_type")||is(c,i,"sized_type_specifier")||is(c,i,"type_identifier")||
        is(c,i,"struct_specifier")||is(c,i,"union_specifier")||is(c,i,"enum_specifier")||is(c,i,"macro_type_specifier");
}
static bool declarator_node(const collector *c,size_t i){
    return is(c,i,"function_declarator")||is(c,i,"pointer_declarator")||
        is(c,i,"parenthesized_declarator")||is(c,i,"attributed_declarator")||is(c,i,"array_declarator");
}
static ps_result recovered_declaration(collector *c,size_t error){
    /* An unfinished later comma declarator can remove the declaration parent,
     * while retaining original specifier and complete init_declarator siblings.
     * Recover only those grammar-recognized declarators with a real delimiter. */
    size_t base=SIZE_MAX,base_end=0;bool typed=false;
    for(size_t i=error+1;i<c->index[error].end;i=c->index[i].end){
        if(specifier(c,i) || is(c,i,"type_qualifier") || is(c,i,"storage_class_specifier") || is(c,i,"attribute_specifier")){
            if(base==SIZE_MAX)base=i;
            typed|=specifier(c,i);base_end=c->nodes[i].end;continue;
        }
        if(typed && (is(c,i,"init_declarator") || is(c,i,"identifier"))){
            size_t delimiter=next_token(c,i);
            if((is(c,delimiter,",")||is(c,delimiter,";")) && c->nodes[delimiter].end<=c->cursor &&
                !c->index[i].bad && contains(c,c->index[i].scope)){
                size_t d=is(c,i,"init_declarator")?child(c,i,"declarator"):i;
                if(d!=SIZE_MAX){ps_result r=append(c,base,d,base_end,false);if(r!=PS_OK)return r;}
            }
            continue;
        }
        if(is(c,i,","))continue;
        base=SIZE_MAX;typed=false;
    }
    return PS_OK;
}
static int order(const void *a,const void *b){
    size_t x=((const ps_c_binding*)a)->name_start,y=((const ps_c_binding*)b)->name_start;return (x>y)-(x<y);
}
static bool ancestor(const collector *c,size_t node,const char *type){
    for(size_t i=node;i<c->count;i=c->nodes[i].parent)if(is(c,i,type))return true;
    return false;
}
static bool call_opener(const collector *c,size_t open){
    size_t p=c->nodes[open].parent;
    if(is(c,p,"argument_list") && is(c,c->nodes[p].parent,"call_expression"))return true;
    if(is(c,p,"parameter_list") || ancestor(c,open,"type_descriptor"))return false;
    size_t before=c->index[open].previous;
    if(is(c,before,")") || is(c,before,"]")){
        return ancestor(c,before,"parenthesized_expression") || ancestor(c,before,"subscript_expression") ||
            (is(c,c->nodes[before].parent,"argument_list") && is(c,c->nodes[c->nodes[before].parent].parent,"call_expression"));
    }
    if(!is(c,before,"identifier") && !is(c,before,"field_identifier"))return false;
    if(field(c,before,"declarator"))return false;
    /* Recovery may leave `void name(` without a parameter_list. Distinguish a
     * written declaration prefix from a call in an initializer/expression using
     * original grammar specifiers in this exact delimiter group. */
    size_t group=c->index[open].group;bool comma=false;
    for(size_t i=c->index[before].previous;i<c->count;i=c->index[i].previous){
        if(i==group || is(c,i,"{") || is(c,i,"}") || is(c,i,";"))break;
        if(c->index[i].group!=group)continue;
        if(is(c,i,",")){comma=true;continue;}
        if((is(c,i,"=") && !comma) || is(c,i,"return") || is(c,i,"?") || is(c,i,":"))break;
        for(size_t n=i;n<c->count;n=c->nodes[n].parent){
            if(specifier(c,n))return false;
            if(c->index[n].group!=group)break;
        }
    }
    return true;
}
static ps_c_call call_context(const collector *c){
    ps_c_call call={0};
    if(!ps_c_lexical_cursor(c->source,c->bytes,c->cursor,NULL))return call;
    for(size_t i=0;i<c->count;i++){
        ps_c_node n=c->nodes[i];if(n.start>=c->cursor)break;
        if(is(c,i,"comment") && c->cursor<=n.end){
            bool closed=n.end>=n.start+4 && c->source[n.start]=='/' && c->source[n.start+1]=='*' &&
                c->source[n.end-2]=='*' && c->source[n.end-1]=='/';
            if(c->cursor<n.end || !closed)return (ps_c_call){0};
        }
        if(c->index[i].end!=i+1 || n.missing || is(c,i,"comment"))continue;
        if(is(c,i,"(") && contains(c,i) && call_opener(c,i))call=(ps_c_call){.open=n.start,.active=true,.at_start=true};
    }
    if(!call.active)return call;
    size_t open=SIZE_MAX;
    for(size_t i=0;i<c->count;i++)if(c->nodes[i].start==call.open && is(c,i,"(")){open=i;break;}
    size_t conditional=0;
    for(size_t i=open+1;i<c->count && c->nodes[i].start<c->cursor;i++){
        if(c->index[i].end!=i+1 || c->nodes[i].missing || is(c,i,"comment") || c->index[i].group!=open)continue;
        if(is(c,i,"?"))conditional++;
        else if(is(c,i,":") && conditional)conditional--;
        if(is(c,i,",") && !conditional){call.argument++;call.at_start=true;}
        else call.at_start=false;
    }
    return call;
}
ps_c_bindings_result ps_c_bindings(const char *source,size_t bytes,size_t cursor,
    ps_c_binding *bindings,size_t capacity,ps_allocator allocator){
    ps_c_bindings_result result={.status=PS_INVALID};
    if((!source && bytes) || (!bindings && capacity) || cursor>bytes)return result;
    if(bytes>PS_SOURCE_MAX_BYTES){result.status=PS_LIMIT;return result;}
    if((bytes && memchr(source,0,bytes)) || (cursor<bytes && ((unsigned char)source[cursor]&0xc0)==0x80))return result;
    if(!allocator.allocate && !allocator.deallocate)allocator=ps_allocator_default();
    if(!ps_allocator_valid(allocator))return result;
    size_t nodes_capacity=bytes<32767?bytes*2+2:65536;
    collector c={.source=source,.bytes=bytes,.cursor=cursor,.capacity=capacity,.output=bindings};
    result.status=ps_memory_zero(allocator,nodes_capacity,sizeof *c.nodes,(void**)&c.nodes);if(result.status!=PS_OK)goto done;
    result.status=ps_memory_zero(allocator,nodes_capacity,sizeof *c.index,(void**)&c.index);if(result.status!=PS_OK)goto done;
    ps_c_syntax_result syntax=ps_c_syntax(source,bytes,c.nodes,nodes_capacity,allocator,(ps_c_limits){0});
    result.status=syntax.status;result.has_error=syntax.has_error;c.count=syntax.count;if(result.status!=PS_OK)goto done;
    result.status=build_index(&c);if(result.status!=PS_OK)goto done;
    for(size_t i=0;i<c.count;i++){
        if(is(&c,i,"declaration"))result.status=declaration(&c,i);
        if(is(&c,i,"function_definition")){
            size_t d=child(&c,i,"declarator"),body=child(&c,i,"body");
            if(d!=SIZE_MAX && body!=SIZE_MAX && body+1<c.count){body=c.index[body+1].next;if(is(&c,body,"{"))result.status=parameters(&c,d,body);}
        }else if(declarator_node(&c,i) && is(&c,c.nodes[i].parent,"ERROR")){
            size_t body=definition_body(&c,i);if(is(&c,body,"{"))result.status=parameters(&c,i,body);
        }else if(is(&c,i,"ERROR"))result.status=recovered_declaration(&c,i);
        if(result.status!=PS_OK)goto done;
    }
    if(c.used>1)qsort(bindings,c.used,sizeof *bindings,order);
    result.count=c.used;result.call=call_context(&c);
done:
    ps_memory_free(allocator,c.index,nodes_capacity*sizeof *c.index);
    ps_memory_free(allocator,c.nodes,nodes_capacity*sizeof *c.nodes);return result;
}
static size_t binding_text(const char *source,size_t bytes,ps_c_binding b,char *out,size_t capacity,bool name){
    if(bytes>PS_SOURCE_MAX_BYTES || (!source && bytes) || (!out && capacity) || b.type_start>b.type_end || b.type_end>b.declarator_start ||
       b.declarator_start>b.name_start || b.name_start>=b.name_end || b.name_end>b.declarator_end || b.declarator_end>bytes)return 0;
    size_t base=b.type_start,end=b.type_end,prefix=b.declarator_start,suffix=b.declarator_end;
    while(base<end && space((unsigned char)source[base]))base++;
    while(end>base && space((unsigned char)source[end-1]))end--;
    while(prefix<b.name_start && space((unsigned char)source[prefix]))prefix++;
    while(suffix>b.name_end && space((unsigned char)source[suffix-1]))suffix--;
    size_t extra=b.name_start-prefix+suffix-b.name_end+(name?b.name_end-b.name_start:0),needed=end-base+(extra?1:0)+extra+1,at=0;
#define PUT(ch) do{if(at+1<capacity)out[at]=(ch);at++;}while(0)
    for(size_t i=base;i<end;i++)PUT(source[i]);
    if(extra)PUT(' ');
    for(size_t i=prefix;i<b.name_start;i++)PUT(source[i]);
    if(name)for(size_t i=b.name_start;i<b.name_end;i++)PUT(source[i]);
    for(size_t i=b.name_end;i<suffix;i++)PUT(source[i]);
    if(capacity)out[at<capacity?at:capacity-1]=0;
#undef PUT
    return needed;
}
size_t ps_c_binding_type(const char *source,size_t bytes,ps_c_binding b,char *out,size_t capacity){return binding_text(source,bytes,b,out,capacity,false);}
size_t ps_c_binding_signature(const char *source,size_t bytes,ps_c_binding b,char *out,size_t capacity){return binding_text(source,bytes,b,out,capacity,true);}
