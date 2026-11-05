/* Ordinary-name lookup shares the exact grammar/index with signature projection;
 * it does not change the completed-candidate contract in c_bindings_source_at. */
typedef struct {
    collector *c;
    const char *name;
    size_t bytes,base,base_end,declarator,body,visible,depth,scope_end;
    bool scope_open,parameter;
    ps_c_name_kind kind;
    ps_c_binding binding;
} name_lookup;
static bool lookup_equal(const name_lookup *q,size_t node){
    const collector *c=q->c;
    return node<c->count && c->nodes[node].end-c->nodes[node].start==q->bytes &&
        !memcmp(c->source+c->nodes[node].start,q->name,q->bytes);
}
static size_t lookup_name(const collector *c,size_t d){
    while(d<c->count){
        if(is(c,d,"identifier") || is(c,d,"type_identifier"))return d;
        size_t next=child(c,d,"declarator");
        if(next>=c->count && is(c,d,"parenthesized_declarator"))
            for(size_t i=d+1;i<c->index[d].end;i=c->index[i].end)if(c->nodes[i].named && !is(c,i,"comment")){next=i;break;}
        d=next;
    }
    return SIZE_MAX;
}
static bool lookup_block(const collector *c,size_t open){
    size_t parent=c->nodes[open].parent;
    return !is(c,parent,"enumerator_list") && !is(c,parent,"field_declaration_list") && !is(c,parent,"initializer_list");
}
static bool lookup_control(const collector *c,size_t node){
    return is(c,node,"for_statement") || is(c,node,"while_statement") || is(c,node,"do_statement") ||
        is(c,node,"if_statement") || is(c,node,"switch_statement");
}
static void lookup_extent(const collector *c,size_t node,size_t *depth,size_t *end,bool *open){
    *depth=0;*end=c->bytes;*open=true;
    for(size_t i=c->index[node].scope;i<c->count;i=c->index[i].scope)if(lookup_block(c,i)){
        if(!contains(c,i)){*end=0;return;}
        if(!*depth){size_t close=c->index[i].mate;
            if(close<c->count){*end=c->nodes[close].end;*open=false;}}
        (*depth)++;
    }
    for(size_t i=node;i<c->count;i=c->nodes[i].parent){
        size_t parent=c->nodes[i].parent;
        /* A non-compound substatement still owns a block scope. A compound
         * or nested control statement already supplies its own block below. */
        bool substatement=lookup_control(c,parent) && !is(c,i,"compound_statement") && !lookup_control(c,i) &&
            (field(c,i,"body") || field(c,i,"consequence") || field(c,i,"alternative"));
        if(!lookup_control(c,i) && !substatement)continue;
        (*depth)++;
        if(c->nodes[i].end<*end || (c->nodes[i].end==*end && !c->index[i].bad)){
            size_t last=c->index[i].end;
            do{last--;}while(last>i && (c->nodes[last].missing || is(c,last,"comment")));
            *end=c->nodes[i].end;*open=c->index[i].bad && c->nodes[i].end==c->bytes && !is(c,last,";") && !is(c,last,"}");
        }
    }
}
static void lookup_consider(name_lookup *q,size_t base,size_t base_end,size_t d,
                            size_t body,ps_c_name_kind kind,size_t parameter_end,bool parameter_open){
    collector *c=q->c;
    if(d>=c->count || c->index[d].bad || c->nodes[d].end>c->cursor)return;
    size_t name=lookup_name(c,d);if(!lookup_equal(q,name))return;
    size_t depth,end;bool open;lookup_extent(c,d,&depth,&end,&open);
    if(parameter_end){depth++;end=parameter_end;open=parameter_open;}
    if(c->cursor>end || (c->cursor==end && !open) ||
       (q->kind!=PS_C_NAME_NONE && (depth<q->depth || (depth==q->depth && c->nodes[d].end<=q->visible))))return;
    q->base=base;q->base_end=base_end;q->declarator=d;q->body=body;
    q->visible=c->nodes[d].end;q->depth=depth;q->scope_end=end;q->scope_open=open;
    q->kind=kind;q->parameter=kind==PS_C_NAME_PARAMETER;
    q->binding=(ps_c_binding){.name_start=c->nodes[name].start,.name_end=c->nodes[name].end,
        .line=c->nodes[name].line,.column=c->nodes[name].column,
        .type_start=c->nodes[base].start,.type_end=base_end,
        .declarator_start=c->nodes[d].start,.declarator_end=c->nodes[d].end,.parameter=q->parameter};
}
static void lookup_declaration(name_lookup *q,size_t declaration,ps_c_name_kind kind){
    collector *c=q->c;
    if(c->index[declaration].legacy_parameter || is(c,c->nodes[declaration].parent,"function_definition") || !loop_visible(c,declaration))return;
    size_t first=child(c,declaration,"declarator");
    if(first>=c->count || !signature_base(c,declaration,first))return;
    for(size_t i=first;i<c->index[declaration].end;i=c->index[i].end)if(field(c,i,"declarator")){
        size_t d=is(c,i,"init_declarator")?child(c,i,"declarator"):i;
        lookup_consider(q,declaration,c->nodes[first].start,d,SIZE_MAX,kind,0,false);
    }
}
static void lookup_recovery(name_lookup *q,size_t error){
    collector *c=q->c;size_t base=SIZE_MAX,base_end=0;bool typed=false;
    for(size_t i=error+1;i<c->index[error].end;i=c->index[i].end){
        if(specifier(c,i) || is(c,i,"type_qualifier") || is(c,i,"storage_class_specifier") || is(c,i,"attribute_specifier")){
            if(c->index[i].bad){base=SIZE_MAX;typed=false;continue;}
            if(base==SIZE_MAX)base=i;
            typed|=specifier(c,i);base_end=c->nodes[i].end;continue;
        }
        if(typed && (declarator_node(c,i) || is(c,i,"init_declarator") || is(c,i,"identifier"))){
            size_t d=is(c,i,"init_declarator")?child(c,i,"declarator"):i;
            size_t body=definition_body(c,i),next=next_token(c,d);
            if(is(c,body,"{")){
                signature_mark_legacy(c,d,body);
                lookup_consider(q,base,base_end,d,body,PS_C_NAME_DECLARATION,0,false);
            }else if(next>=c->count || is(c,next,"=") || is(c,next,",") || is(c,next,";"))
                lookup_consider(q,base,base_end,d,SIZE_MAX,PS_C_NAME_DECLARATION,0,false);
            continue;
        }
        if(is(c,i,","))continue;
        base=SIZE_MAX;typed=false;
    }
}
static bool lookup_parameter_region(const collector *c,size_t list,size_t *outer_out,size_t *body_out,size_t *end_out,bool *open_out){
    if(!is(c,list,"parameter_list"))return false;
    size_t function=c->nodes[list].parent,outer=function;
    if(!is(c,function,"function_declarator"))return false;
    while(c->nodes[outer].parent<c->count && declarator_node(c,c->nodes[outer].parent))outer=c->nodes[outer].parent;
    size_t operation,name=name_node(c,outer,&operation);(void)name;
    /* The returned function type's list is not the factory's definition list.
     * Only the actual operation nearest its declared name binds body parameters. */
    size_t body=operation==function && is(c,operation,"function_declarator")?definition_body(c,outer):SIZE_MAX;
    size_t end=c->nodes[function].end;bool open=false;
    if(is(c,body,"{")){
        size_t close=c->index[body].mate;end=close<c->count?c->nodes[close].end:c->bytes;open=close>=c->count;
    }
    *outer_out=outer;*body_out=body;*end_out=end;*open_out=open;return true;
}
static void lookup_parameter(name_lookup *q,size_t node){
    collector *c=q->c;size_t outer,body,end;bool open;
    if(!lookup_parameter_region(c,c->nodes[node].parent,&outer,&body,&end,&open))return;
    if(is(c,node,"parameter_declaration")){
        size_t d=child(c,node,"declarator");if(d<c->count)lookup_consider(q,node,c->nodes[d].start,d,SIZE_MAX,PS_C_NAME_PARAMETER,end,open);
    }else if(is(c,node,"identifier") && is(c,body,"{") && lookup_equal(q,node)){
        /* Actual old-style header name; annotation declarations are not globals.
         * Only an unambiguous written annotation supplies a callable header. */
        size_t found=SIZE_MAX,base=SIZE_MAX,base_end=0;bool duplicate=false;
        for(size_t i=c->index[outer].end;i<body;i=c->index[i].end)if(is(c,i,"declaration")){
            size_t first=child(c,i,"declarator");if(first>=c->count)continue;
            for(size_t d=first;d<c->index[i].end;d=c->index[d].end)if(field(c,d,"declarator") && lookup_equal(q,lookup_name(c,d))){
                duplicate|=found<c->count;found=d;base=i;base_end=c->nodes[first].start;
            }
        }
        if(found<c->count && !duplicate)lookup_consider(q,base,base_end,found,SIZE_MAX,PS_C_NAME_PARAMETER,end,open);
        else lookup_consider(q,node,c->nodes[node].start,node,SIZE_MAX,PS_C_NAME_PARAMETER,end,open);
    }
}
ps_c_lookup_result ps_c_bindings_source_lookup(const ps_c_bindings_source *source,
    size_t cursor,const char *name,size_t name_bytes,ps_c_parameter *parameters,size_t capacity,ps_allocator allocator){
    ps_c_lookup_result result={.status=PS_INVALID};
    if(!source || !name || !name_bytes || name_bytes>PS_SOURCE_MAX_BYTES || (!parameters && capacity) || cursor>source->bytes)return result;
    if(memchr(name,0,name_bytes) || (cursor<source->bytes && ((unsigned char)source->text[cursor]&0xc0)==0x80))return result;
    if(!allocator.allocate && !allocator.deallocate)allocator=ps_allocator_default();
    if(!ps_allocator_valid(allocator))return result;
    collector c={.source=source->text,.bytes=source->bytes,.cursor=cursor,.count=source->count,.nodes=source->nodes};
    name_lookup q={.c=&c,.name=name,.bytes=name_bytes,.declarator=SIZE_MAX,.body=SIZE_MAX};
    result.has_error=source->has_error;
    result.status=ps_memory_allocate(allocator,c.count*sizeof *c.index,(void**)&c.index);if(result.status!=PS_OK)goto done;
    result.status=build_index(&c);if(result.status!=PS_OK)goto done;
    /* Mark actual definitions/old-style annotations before ordinary declarations. */
    for(size_t i=0;i<c.count;i++){
        if(is(&c,i,"function_definition")){
            size_t d=child(&c,i,"declarator"),body=child(&c,i,"body");
            if(d>=c.count || body>=c.count || body+1>=c.count || !signature_base(&c,i,d))continue;
            body=c.index[body+1].next;if(!is(&c,body,"{"))continue;
            signature_mark_legacy(&c,d,body);lookup_consider(&q,i,c.nodes[d].start,d,body,PS_C_NAME_DECLARATION,0,false);
        }else if(is(&c,i,"ERROR"))lookup_recovery(&q,i);
    }
    for(size_t i=0;i<c.count;i++){
        if(is(&c,i,"declaration"))lookup_declaration(&q,i,PS_C_NAME_DECLARATION);
        else if(is(&c,i,"type_definition"))lookup_declaration(&q,i,PS_C_NAME_TYPEDEF);
        else if(is(&c,i,"parameter_declaration") || (is(&c,i,"identifier") && is(&c,c.nodes[i].parent,"parameter_list")))lookup_parameter(&q,i);
        else if(is(&c,i,"enumerator")){
            size_t d=child(&c,i,"name");
            /* Enumerator scope starts after its defining enumerator, including
             * its value; never inside the enum's own initializer. */
            size_t end=0;bool open=false;
            for(size_t p=i;p<c.count;p=c.nodes[p].parent)if(is(&c,p,"parameter_list")){
                size_t outer,body;lookup_parameter_region(&c,p,&outer,&body,&end,&open);break;
            }
            if(d<c.count && !c.index[i].bad && c.nodes[i].end<=cursor)
                lookup_consider(&q,d,c.nodes[d].start,d,SIZE_MAX,PS_C_NAME_ENUMERATOR,end,open);
        }
    }
    if(q.kind!=PS_C_NAME_NONE){
        result.found=true;result.kind=q.kind;result.declaration=q.binding;
        if(q.kind!=PS_C_NAME_TYPEDEF && q.kind!=PS_C_NAME_ENUMERATOR){
            signature_projection s={.c=&c,.allocator=allocator,.functions=&result.function,.function_capacity=1,
                .parameters=parameters,.parameter_capacity=capacity,.single=true};
            result.status=signature_emit(&s,q.base,q.base_end,q.declarator,q.body,SIZE_MAX,q.visible);
            if(result.status!=PS_OK)goto failed;
            result.callable=s.function_count==1;result.parameter_count=s.parameter_count;
            if(result.callable){result.function.scope_depth=q.depth;result.function.scope_end=q.scope_end;
                result.function.scope_open=q.scope_open;result.function.declaration.parameter=q.parameter;}
            if(capacity && s.parameter_count>capacity){result.status=PS_LIMIT;goto failed;}
        }
    }
    goto done;
failed:{
    size_t required=result.status==PS_LIMIT?result.parameter_count:0;
    result=(ps_c_lookup_result){.status=result.status,.has_error=source->has_error,.parameter_count=required};
}
done:
    ps_memory_free(allocator,c.index,c.count*sizeof *c.index);return result;
}
