/* Uses the same immutable flat grammar and private structural index as locals. */
typedef struct {
    size_t header,declaration,declarator;
    bool duplicate;
} signature_legacy;
typedef struct {
    collector *c;
    ps_allocator allocator;
    ps_c_function *functions;
    ps_c_parameter *parameters;
    size_t function_count,function_capacity,parameter_count,parameter_capacity;
    bool single;
} signature_projection;

static ps_c_binding signature_binding(const collector *c,size_t base,size_t base_end,size_t d,bool parameter){
    size_t operation,name=name_node(c,d,&operation);
    ps_c_binding b={.type_start=c->nodes[base].start,.type_end=base_end,
        .declarator_start=c->nodes[d].start,.declarator_end=c->nodes[d].end,.parameter=parameter};
    if(name<c->count){ps_c_node n=c->nodes[name];b.name_start=n.start;b.name_end=n.end;b.line=n.line;b.column=n.column;}
    return b;
}
static size_t signature_callable(const collector *c,size_t d,bool *pointer){
    size_t nearest=SIZE_MAX,outer=SIZE_MAX;*pointer=false;
    while(d<c->count){
        if(is(c,d,"identifier")){
            if(is(c,nearest,"function_declarator"))return nearest;
            if(is(c,nearest,"pointer_declarator") && is(c,outer,"function_declarator")){*pointer=true;return outer;}
            return SIZE_MAX;
        }
        size_t next=child(c,d,"declarator");
        if(next==SIZE_MAX && is(c,d,"parenthesized_declarator"))
            for(size_t n=d+1;n<c->index[d].end;n=c->index[n].end)if(c->nodes[n].named && !is(c,n,"comment")){next=n;break;}
        if(!is(c,d,"parenthesized_declarator") && !is(c,d,"attributed_declarator")){outer=nearest;nearest=d;}
        d=next;
    }
    return SIZE_MAX;
}
static bool signature_void(const collector *c,size_t parameter){
    size_t type=SIZE_MAX;
    for(size_t i=parameter+1;i<c->index[parameter].end;i=c->index[i].end){
        if(is(c,i,"comment"))continue;
        if(type!=SIZE_MAX || !is(c,i,"primitive_type"))return false;
        type=i;
    }
    return type<c->count && c->nodes[type].end-c->nodes[type].start==4 && !memcmp(c->source+c->nodes[type].start,"void",4);
}
static bool signature_params(const collector *c,size_t list,size_t *count,bool *old_style){
    *count=0;*old_style=false;
    if(list>=c->count || c->index[list].bad)return false;
    size_t open=list+1<c->count?c->index[list+1].next:SIZE_MAX;
    if(!is(c,open,"(") || c->index[open].mate==SIZE_MAX)return false;
    for(size_t i=list+1;i<c->index[list].end;i=c->index[i].end){
        if(is(c,i,"parameter_declaration"))(*count)++;
        else if(is(c,i,"identifier")){(*count)++;*old_style=true;}
        else if(!is(c,i,"(") && !is(c,i,")") && !is(c,i,",") && !is(c,i,"comment") && !is(c,i,"variadic_parameter"))return false;
    }
    return true;
}
static void signature_scope(const collector *c,size_t node,size_t parameter_body,ps_c_function *f){
    size_t open=parameter_body<c->count?parameter_body:c->index[node].scope;
    f->scope_end=c->bytes;f->scope_open=true;
    if(open<c->count){
        f->scope_start=c->nodes[open].end;
        size_t close=c->index[open].mate;
        if(close<c->count){f->scope_end=c->nodes[close].end;f->scope_open=false;}
    }
    for(size_t i=open;i<c->count;i=c->index[i].scope)f->scope_depth++;
    for(size_t i=node;i<c->count;i=c->nodes[i].parent)if(is(c,i,"for_statement")){
        f->scope_depth++;
        if(c->nodes[i].start>f->scope_start)f->scope_start=c->nodes[i].start;
        if(c->nodes[i].end<=f->scope_end){
            f->scope_end=c->nodes[i].end;
            size_t last=c->index[i].end;
            do{last--;}while(last>i && (c->nodes[last].missing || is(c,last,"comment")));
            f->scope_open=c->nodes[i].end==c->bytes && c->index[i].bad && !is(c,last,";") && !is(c,last,"}");
        }
    }
    if(parameter_body<c->count)f->visible_from=c->nodes[parameter_body].end;
}
static size_t signature_hash(const collector *c,size_t name){
    size_t hash=2166136261u;
    for(size_t i=c->nodes[name].start;i<c->nodes[name].end;i++)hash=(hash^(unsigned char)c->source[i])*16777619u;
    return hash;
}
static bool signature_name_equal(const collector *c,size_t a,size_t b){
    return c->nodes[a].end-c->nodes[a].start==c->nodes[b].end-c->nodes[b].start &&
        !memcmp(c->source+c->nodes[a].start,c->source+c->nodes[b].start,c->nodes[a].end-c->nodes[a].start);
}
static signature_legacy *signature_slot(const collector *c,signature_legacy *table,size_t capacity,size_t name){
    size_t at=signature_hash(c,name)&(capacity-1);
    while(table[at].header && !signature_name_equal(c,table[at].header,name))at=(at+1)&(capacity-1);
    return &table[at];
}
static ps_result signature_legacy_types(signature_projection *s,size_t outer,size_t body,size_t list,size_t count,
                                       signature_legacy **out,size_t *capacity){
    collector *c=s->c;*capacity=2;while(*capacity<count*2)*capacity*=2;
    ps_result r=ps_memory_zero(s->allocator,*capacity,sizeof **out,(void**)out);if(r!=PS_OK)return r;
    for(size_t i=list+1;i<c->index[list].end;i=c->index[i].end)if(is(c,i,"identifier")){
        signature_legacy *entry=signature_slot(c,*out,*capacity,i);
        if(entry->header)entry->duplicate=true;
        else *entry=(signature_legacy){.header=i,.declaration=SIZE_MAX,.declarator=SIZE_MAX};
    }
    for(size_t i=c->index[outer].end;i<body;i=c->index[i].end)if(is(c,i,"declaration")){
        c->index[i].legacy_parameter=true;
        size_t first=child(c,i,"declarator");if(first>=c->count || c->index[i].bad)continue;
        for(size_t d=first;d<c->index[i].end;d=c->index[d].end)if(field(c,d,"declarator")){
            size_t operation,name=name_node(c,d,&operation);if(name>=c->count)continue;
            signature_legacy *entry=signature_slot(c,*out,*capacity,name);if(!entry->header)continue;
            if(entry->declarator<c->count)entry->duplicate=true;
            else{entry->declaration=i;entry->declarator=d;}
        }
    }
    return PS_OK;
}
static void signature_mark_legacy(collector *c,size_t outer,size_t body){
    if(outer>=c->count || body>=c->count)return;
    size_t operation,name=name_node(c,outer,&operation);
    if(name>=c->count || !is(c,operation,"function_declarator"))return;
    /* A malformed header cannot turn its grammar-recognized annotation
     * declarations into unrelated file-scope callbacks. */
    for(size_t i=c->index[outer].end;i<body;i=c->index[i].end)
        if(is(c,i,"declaration"))c->index[i].legacy_parameter=true;
}
static ps_result signature_emit(signature_projection *s,size_t base,size_t base_end,size_t d,
                                size_t definition_body_node,size_t parameter_body,size_t visible_from){
    collector *c=s->c;bool pointer=false,old_style=false;size_t count=0;
    if(d>=c->count)return PS_OK;
    signature_mark_legacy(c,d,definition_body_node);
    if(c->index[d].bad)return PS_OK;
    size_t callable=signature_callable(c,d,&pointer);if(callable>=c->count)return PS_OK;
    size_t list=child(c,callable,"parameters");if(!signature_params(c,list,&count,&old_style))return PS_OK;
    ps_c_function f={.declaration=signature_binding(c,base,base_end,d,parameter_body<c->count),
        .parameters_start=c->nodes[list].start,.parameters_end=c->nodes[list].end,
        .parameter_first=s->parameter_count,.pointer=pointer,.definition=definition_body_node<c->count,
        .unspecified=!count && definition_body_node>=c->count,.old_style=old_style,.visible_from=visible_from};
    signature_scope(c,d,parameter_body,&f);
    signature_legacy *legacy=NULL;size_t legacy_capacity=0;ps_result r=PS_OK;
    if(old_style && definition_body_node<c->count){
        r=signature_legacy_types(s,d,definition_body_node,list,count,&legacy,&legacy_capacity);if(r!=PS_OK)return r;
    }
    for(size_t i=list+1;i<c->index[list].end;i=c->index[i].end){
        if(is(c,i,"variadic_parameter")){f.variadic_start=c->nodes[i].start;f.variadic_end=c->nodes[i].end;continue;}
        if(!is(c,i,"parameter_declaration") && !is(c,i,"identifier"))continue;
        if(count==1 && signature_void(c,i))continue;
        ps_c_parameter p={.start=c->nodes[i].start,.end=c->nodes[i].end};
        if(is(c,i,"identifier")){
            p.type_missing=true;p.declaration=signature_binding(c,i,c->nodes[i].start,i,true);
            if(legacy){signature_legacy *entry=signature_slot(c,legacy,legacy_capacity,i);
                if(entry->declarator<c->count && !entry->duplicate){size_t first=child(c,entry->declaration,"declarator");
                    p.declaration=signature_binding(c,entry->declaration,c->nodes[first].start,entry->declarator,true);p.type_missing=false;}}
        }else{
            size_t actual=child(c,i,"declarator");
            if(actual<c->count)p.declaration=signature_binding(c,i,c->nodes[actual].start,actual,true);
            else p.declaration=(ps_c_binding){.type_start=p.start,.type_end=p.end,.declarator_start=p.end,.declarator_end=p.end,.parameter=true};
        }
        if(s->parameter_count<s->parameter_capacity)s->parameters[s->parameter_count]=p;
        s->parameter_count++;f.parameter_count++;
    }
    if(s->function_count<s->function_capacity)s->functions[s->function_count]=f;
    s->function_count++;
    /* Only top-level parameters of an actual definition bind callback names in
     * its body. A nested callback's prototype parameter names never escape. */
    if(!s->single && definition_body_node<c->count)for(size_t i=list+1;i<c->index[list].end;i=c->index[i].end){
        size_t parameter=i,actual=SIZE_MAX;
        if(is(c,i,"parameter_declaration"))actual=child(c,i,"declarator");
        else if(is(c,i,"identifier") && legacy){signature_legacy *entry=signature_slot(c,legacy,legacy_capacity,i);
            if(!entry->duplicate){parameter=entry->declaration;actual=entry->declarator;}}
        if(actual<c->count){size_t first=child(c,parameter,"declarator");
            r=signature_emit(s,parameter,c->nodes[first].start,actual,SIZE_MAX,definition_body_node,c->nodes[definition_body_node].end);
            if(r!=PS_OK)break;}
    }
    ps_memory_free(s->allocator,legacy,legacy_capacity*sizeof *legacy);return r;
}
static bool signature_base(const collector *c,size_t base,size_t first){
    bool typed=false;
    for(size_t i=base+1;i<first;i=c->index[i].end){if(c->index[i].bad)return false;typed|=specifier(c,i);}
    return typed;
}
static ps_result signature_declaration(signature_projection *s,size_t declaration){
    collector *c=s->c;
    if(c->index[declaration].legacy_parameter || is(c,c->nodes[declaration].parent,"function_definition"))return PS_OK;
    size_t first=child(c,declaration,"declarator");if(first>=c->count || !signature_base(c,declaration,first))return PS_OK;
    for(size_t i=first;i<c->index[declaration].end;i=c->index[i].end)if(field(c,i,"declarator")){
        size_t delimiter=next_token(c,i),actual=is(c,i,"init_declarator")?child(c,i,"declarator"):i;
        if((!is(c,delimiter,",") && !is(c,delimiter,";")) || c->index[i].bad)continue;
        ps_result r=signature_emit(s,declaration,c->nodes[first].start,actual,SIZE_MAX,SIZE_MAX,c->nodes[delimiter].end);
        if(r!=PS_OK)return r;
    }
    return PS_OK;
}
static ps_result signature_recovery(signature_projection *s,size_t error){
    collector *c=s->c;size_t base=SIZE_MAX,base_end=0;bool typed=false;
    for(size_t i=error+1;i<c->index[error].end;i=c->index[i].end){
        if(specifier(c,i) || is(c,i,"type_qualifier") || is(c,i,"storage_class_specifier") || is(c,i,"attribute_specifier")){
            if(c->index[i].bad){base=SIZE_MAX;typed=false;continue;}
            if(base==SIZE_MAX)base=i;
            typed|=specifier(c,i);base_end=c->nodes[i].end;continue;
        }
        if(typed && (declarator_node(c,i) || is(c,i,"init_declarator"))){
            size_t actual=is(c,i,"init_declarator")?child(c,i,"declarator"):i,delimiter=next_token(c,i),body=definition_body(c,i);
            if(actual>=c->count)continue;
            if(is(c,body,"{")){
                ps_result r=signature_emit(s,base,base_end,actual,body,SIZE_MAX,c->nodes[actual].end);if(r!=PS_OK)return r;
            }else if((is(c,delimiter,",") || is(c,delimiter,";")) && !c->index[i].bad){
                ps_result r=signature_emit(s,base,base_end,actual,SIZE_MAX,SIZE_MAX,c->nodes[delimiter].end);if(r!=PS_OK)return r;
            }
            continue;
        }
        if(is(c,i,","))continue;
        base=SIZE_MAX;typed=false;
    }
    return PS_OK;
}
static int signature_order(const void *a,const void *b){
    size_t x=((const ps_c_function*)a)->declaration.name_start,y=((const ps_c_function*)b)->declaration.name_start;return (x>y)-(x<y);
}
ps_c_signatures_result ps_c_bindings_source_signatures(const ps_c_bindings_source *source,
    ps_c_function *functions,size_t function_capacity,ps_c_parameter *parameters,size_t parameter_capacity,ps_allocator allocator){
    ps_c_signatures_result result={.status=PS_INVALID};
    if(!source || (!functions && function_capacity) || (!parameters && parameter_capacity))return result;
    if(!allocator.allocate && !allocator.deallocate)allocator=ps_allocator_default();
    if(!ps_allocator_valid(allocator))return result;
    collector c={.source=source->text,.bytes=source->bytes,.count=source->count,.nodes=source->nodes};
    signature_projection s={.c=&c,.allocator=allocator,.functions=functions,.parameters=parameters,
        .function_capacity=function_capacity,.parameter_capacity=parameter_capacity};
    result.has_error=source->has_error;
    result.status=ps_memory_allocate(allocator,c.count*sizeof *c.index,(void**)&c.index);if(result.status!=PS_OK)goto done;
    result.status=build_index(&c);if(result.status!=PS_OK)goto done;
    /* Definitions first, so their old-style annotation declarations are marked
     * before the ordinary declaration pass. Each query has its own index. */
    for(size_t i=0;i<c.count;i++){
        if(is(&c,i,"function_definition")){
            size_t d=child(&c,i,"declarator"),body=child(&c,i,"body");
            if(d>=c.count || body>=c.count || body+1>=c.count || !signature_base(&c,i,d))continue;
            body=c.index[body+1].next;
            if(is(&c,body,"{"))result.status=signature_emit(&s,i,c.nodes[d].start,d,body,SIZE_MAX,c.nodes[d].end);
        }else if(is(&c,i,"ERROR"))result.status=signature_recovery(&s,i);
        if(result.status!=PS_OK)goto done;
    }
    for(size_t i=0;i<c.count;i++)if(is(&c,i,"declaration")){
        result.status=signature_declaration(&s,i);if(result.status!=PS_OK)goto done;
    }
    result.function_count=s.function_count;result.parameter_count=s.parameter_count;
    if((function_capacity || parameter_capacity) && (s.function_count>function_capacity || s.parameter_count>parameter_capacity))result.status=PS_LIMIT;
    else if(s.function_count>1 && function_capacity)qsort(functions,s.function_count,sizeof *functions,signature_order);
done:
    ps_memory_free(allocator,c.index,c.count*sizeof *c.index);return result;
}
